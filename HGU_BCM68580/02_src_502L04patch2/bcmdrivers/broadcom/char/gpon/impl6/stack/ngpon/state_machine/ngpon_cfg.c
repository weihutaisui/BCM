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
#include <linux/list.h>
#else
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#endif
#include <bdmf_dev.h>
#include "opticaldet.h"

/* driver */
#include "ngpon_db.h"
#ifdef CONFIG_BCM_GPON_TODD
#include "ngpon_tod.h"
#endif
#include "aes_cmac.h"
#include "boardparms.h"
#include "shared_utils.h"
#if defined(USE_LOGGER)
#include "gpon_logger.h"
#endif
#include "rogue_drv.h"
#include "ngpon_tune.h"
#include "wan_drv.h"
#include "NGPON_BLOCKS.h"
#if defined(CONFIG_BCM96858)
#include "6858_map_part.h"
#endif
#if defined(CONFIG_BCM96856)
#include "6856_map_part.h"
#endif

#ifndef SIM_ENV
extern spinlock_t lock_ploam_list;
#endif

#if !defined(BCMSWAP64)
#define BCMSWAP64(val)  ((uint64)(                \
(((uint64)(val) & 0x00000000000000ffULL) << 56) | \
(((uint64)(val) & 0x000000000000ff00ULL) << 40) | \
(((uint64)(val) & 0x0000000000ff0000ULL) << 24) | \
(((uint64)(val) & 0x00000000ff000000ULL) <<  8) | \
(((uint64)(val) & 0x000000ff00000000ULL) >>  8) | \
(((uint64)(val) & 0x0000ff0000000000ULL) >> 24) | \
(((uint64)(val) & 0x00ff000000000000ULL) >> 40) | \
(((uint64)(val) & 0xff00000000000000ULL) >> 56)))
#endif

extern struct list_head us_ploam_list;


extern NGPON_CALLBACKS ngpon_callbacks;


/******************************************************************************/
/*                                                                            */
/* Types and values definitions                                               */
/*                                                                            */
/******************************************************************************/

extern const ru_block_rec *RU_NGPON_BLOCKS[];
#define NGPON_VIRTUAL_BASE ((long unsigned int) RU_NGPON_BLOCKS[0]->addr[0])

#define NGPON_WRITE_32(a,r)     WRITE_32(NGPON_VIRTUAL_BASE + (a - NGPON2_PHYS_BASE),r)
#define NGPON_READ_32(a,r)              READ_32(NGPON_VIRTUAL_BASE + (a - NGPON2_PHYS_BASE),r)

/******************************************************************************/
/*                                                                            */
/* Functions Definitions                                                      */
/*                                                                            */
/******************************************************************************/

void ngpon_pon_task ( void ) ;
PON_ERROR_DTE ngpon_clear_ranging_alloc_table ( void );
PON_ERROR_DTE ngpon_clear_traffic_alloc_table ( void );
PON_ERROR_DTE config_transceiver_data ( void );


bdmf_error_t send_message_to_pon_task ( PON_EVENTS_DTE xi_msg_opcode, ...);
PON_ERROR_DTE ngpon_enable_alloc_id_and_tcont ( PON_TCONT_ID_DTE xi_tcont_id ,uint32_t xi_alloc_index,
                                              PON_ALLOC_ID_DTE xi_alloc_id );

PON_ERROR_DTE ngpon_enable_alloc_id_by_index ( uint32_t xi_alloc_index, bdmf_boolean enable ) ;

static PON_ERROR_DTE misc_cfg(uint32_t def_preamble0_value, uint32_t def_preamble1_value, ngpon_tx_profile_profile_ctrl *profile_ctrl, uint32_t alloc_cfg1,
                uint32_t start_size, ngpon_tx_cfg_func_en *tx_status, uint8_t speed, uint8_t standard, uint8_t txen);

/* Converts array of 4 bytes into word (incase memcpy is not good due to Big Little/Endian issues) */
static void bytes_to_word ( uint8_t * xi_byte_array, uint32_t* xo_word )
{

  uint32_t  * xi_word_p = (uint32_t *) xi_byte_array ;

  /* To write to the register : network order to ARM's Little Endian  */

#ifndef SIM_ENV
    *xo_word = be32_to_cpu ( *xi_word_p ) ;
#else
  */
    *xo_word = xi_byte_array[ 0 ] ;
    (*xo_word) <<= 8 ;
    *xo_word  |= xi_byte_array[ 1 ] ;
    (*xo_word) <<= 8 ;
    *xo_word  |= xi_byte_array[ 2 ] ;
    (*xo_word) <<= 8 ;
    *xo_word  |= xi_byte_array[ 3 ] ;
#endif

}


/*
 * The function sets the stack to operate in one of 3 modes:
 *    NGPON_MODE_NGPON2_10G  - symmetric 10G US and 10G DS NGPON2
 *    NGPON_MODE_NGPON2_2_5G - assymmetric 2.5G US 10G DS NGPON2
 *    NGPON_MODE_XGPON       - XGPON ( 2.5G US 10G DS )
 *    NGPON_MODE_XGS         - symmetric 10G US and 10G DS NGPON2 w/o NGPON2-specific PLOAMs
 *
 * Requires _link_reset() to apply new stack mode to the HW
 */
void ngpon_set_stack_mode (PON_MAC_MODE stack_mode)
{
  p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Stack mode is %d  ", stack_mode) ;

  g_xgpon_db.onu_params.stack_mode = stack_mode;
  switch (stack_mode)
  {
    case NGPON_MODE_NGPON2_10G:
      g_xgpon_db.rx_params.rx_mac_mode = NGPON_MODE_NGPON2_10G;
      g_xgpon_db.tx_params.tx_line_rate = NGPON_US_LINE_RATE_10G;
      g_xgpon_db.tx_params.tx_frame_length = NGPON_US_FRAME_LENGTH_10G;
      g_xgpon_db.tx_params.tx_timeline_offset = NGPON_TX_DEFAULT_TIMELINE_OFFSET_NGPON2;
    break;

    case NGPON_MODE_NGPON2_2_5G:
      g_xgpon_db.rx_params.rx_mac_mode = NGPON_MODE_NGPON2_2_5G;
      g_xgpon_db.tx_params.tx_line_rate = NGPON_US_LINE_RATE_2_5G;
      g_xgpon_db.tx_params.tx_frame_length = NGPON_US_FRAME_LENGTH_2_5G;
      g_xgpon_db.tx_params.tx_timeline_offset = NGPON_TX_DEFAULT_TIMELINE_OFFSET_NGPON2;
    break;

    case NGPON_MODE_XGPON:
      g_xgpon_db.rx_params.rx_mac_mode = NGPON_MODE_XGPON;
      g_xgpon_db.tx_params.tx_line_rate = NGPON_US_LINE_RATE_2_5G;
      g_xgpon_db.tx_params.tx_frame_length = NGPON_US_FRAME_LENGTH_2_5G;
      g_xgpon_db.tx_params.tx_timeline_offset = NGPON_TX_DEFAULT_TIMELINE_OFFSET_XGPON;
      /*
       * PLOAMs not expected in XGPON mode are considered to be received 
       */
      g_xgpon_db.link_params.system_profile.system_profile_received = BDMF_TRUE;
      g_xgpon_db.link_params.ch_profile.channel_profile_received = BDMF_TRUE;
    break;

    case NGPON_MODE_XGS:
      g_xgpon_db.rx_params.rx_mac_mode = NGPON_MODE_NGPON2_10G;
      g_xgpon_db.tx_params.tx_line_rate = NGPON_US_LINE_RATE_10G;
      g_xgpon_db.tx_params.tx_frame_length = NGPON_US_FRAME_LENGTH_10G;
      g_xgpon_db.tx_params.tx_timeline_offset = NGPON_TX_DEFAULT_TIMELINE_OFFSET_NGPON2;
      /*
       * PLOAMs not expected in XGPON mode are considered to be received 
       */
      g_xgpon_db.link_params.system_profile.system_profile_received = BDMF_TRUE;
      g_xgpon_db.link_params.ch_profile.channel_profile_received = BDMF_TRUE;
    break;

    case NGPON_MODE_ILLEGAL:
    default:
      p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Illegal mode %d Stack will be configured to work in XGPON mode !", stack_mode) ;
      g_xgpon_db.rx_params.rx_mac_mode = NGPON_MODE_XGPON;
      g_xgpon_db.tx_params.tx_line_rate = NGPON_US_LINE_RATE_2_5G;
      g_xgpon_db.link_params.system_profile.system_profile_received = BDMF_TRUE;
      g_xgpon_db.link_params.ch_profile.channel_profile_received = BDMF_TRUE;
      g_xgpon_db.tx_params.tx_timeline_offset = NGPON_TX_DEFAULT_TIMELINE_OFFSET_XGPON;
  }

#ifdef CONFIG_BCM_GPON_TODD   
    tod_init_sw(g_xgpon_db.onu_params.stack_mode);
#endif 

}

static void npon_stack_rogue_onu_detect_init (void)
{
    gl_rogue_onu_detect_cb.get_rogue_onu_detection_params = ngpon_get_rogue_onu_detection_params;
    gl_rogue_onu_detect_cb.set_rogue_onu_detection_params = ngpon_set_rogue_onu_detection_params;
    gl_rogue_onu_detect_cb.cfg_rogue_onu_interrupts = ngpon_cfg_rogue_onu_interrupts;
}


PON_ERROR_DTE ngpon_enable_transmitter ( bdmf_boolean tx_state )
{
  bdmf_boolean transmitter_enable ;
  bdmf_boolean standard ;
  bdmf_boolean speed ;
  PON_ERROR_DTE xgpon_error ;


    xgpon_error = ag_drv_ngpon_tx_cfg_onu_tx_en_get(&transmitter_enable, &standard, &speed);
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Get Tx state due to Driver error %d !", xgpon_error ) ;
        return xgpon_error ;
    }
    standard = (g_xgpon_db.rx_params.rx_mac_mode == NGPON_MODE_XGPON)? 0 : 1;
    speed = 0;
    if (g_xgpon_db.rx_params.rx_mac_mode != NGPON_MODE_XGPON)
    {
       speed = (g_xgpon_db.tx_params.tx_line_rate == NGPON_US_LINE_RATE_2_5G)? 0 : 1;
    }

    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_enable_transmitter(): enabled %d standard %d speed %d ", tx_state, standard, speed ) ;

    xgpon_error = ag_drv_ngpon_tx_cfg_onu_tx_en_set(tx_state, standard, speed);
 
    if ( xgpon_error != PON_NO_ERROR )
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Set Tx state due to Driver error %d !", xgpon_error ) ;

    return ( xgpon_error ) ;
}


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   This function initialize the XGPON ONU software. It allocates the memory */
/* and the RTOS resources, initializes the internal database and all state    */
/* machines.                                                                  */
/*                                                                            */
/******************************************************************************/
bdmf_error_t ngpon_software_init (void)
{
    PON_ERROR_DTE stack_error ;
    uint8_t mac_version_major ;
    uint8_t mac_version_minor ;
    uint32_t mac_date ;
    ngpon_rx_int_ier  rx_interrupts_enable ;
    ngpon_tx_int_ier1 tx_interrupts_enable ;
    uint8_t  tcont_id ;
    bdmf_boolean standard ;
    bdmf_boolean speed ;
    PON_ERROR_DTE xgpon_error ;
    bdmf_boolean transmitter_enable ;

    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "start %s", __FUNCTION__ ) ;

    /* Check software state */
    if ( g_software_state == NGPON_SOFTWARE_STATE_INITIALIZED ) 
    {
        return NGPON_ERROR_SOFTWARE_ALREADY_INITIALIZED;
    }

    init_globals();

    /* OS frequency */
    g_xgpon_db.os_resources.os_frequency = bdmf_get_cpu_frequency () ;
    
    /* Allocate OS resources */
    stack_error = create_os_resources ( ) ;
    if ( stack_error != BDMF_ERR_OK )
        return stack_error;

    stack_error = ag_drv_ngpon_rx_gen_version_get (&mac_version_minor, &mac_version_major, &mac_date) ;
    if (stack_error != PON_NO_ERROR)
        return stack_error ;


    p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,  
        "XGPON MAC version Major-%d Minor-%d Date-0x%08X", 
        mac_version_major, mac_version_minor, mac_date) ;
     
     
    /* Change software state to initialized */
    g_software_state = NGPON_SOFTWARE_STATE_INITIALIZED ;

    /*
     *     6858 NGPON MAC interrupts configuration
     */
    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Enabling interrupts - IER") ;
    rx_interrupts_enable.tm_fifo_ovf_mask             = BDMF_TRUE ;
    rx_interrupts_enable.bw_fifo_ovf_mask             = BDMF_TRUE ;
    rx_interrupts_enable.aes_fail_mask                = BDMF_TRUE ;
    rx_interrupts_enable.fwi_state_change_mask        = BDMF_FALSE ;
    rx_interrupts_enable.lof_state_change_mask        = BDMF_TRUE ;
    rx_interrupts_enable.lcdg_state_change_mask       = BDMF_TRUE ;
    rx_interrupts_enable.lb_fifo_ovf_mask             = BDMF_TRUE ;
    rx_interrupts_enable.tod_update_mask              = BDMF_FALSE ;
    rx_interrupts_enable.bw_dis_tx_mask               = BDMF_FALSE ;
    rx_interrupts_enable.inv_key_mask                 = BDMF_FALSE ;
    rx_interrupts_enable.bwmap_rec_done_mask          = BDMF_TRUE ;
    rx_interrupts_enable.ponid_inconsist_mask         = BDMF_TRUE ;
    rx_interrupts_enable.inv_portid_change_mask       = BDMF_FALSE ;
    rx_interrupts_enable.fec_fatal_error_mask         = BDMF_TRUE ;
    rx_interrupts_enable.sfc_indication_mask          = BDMF_FALSE ;
    rx_interrupts_enable.amcc_grant_mask              = BDMF_FALSE ;
    rx_interrupts_enable.dwba_mask                    = 0 ;

    stack_error = ag_drv_ngpon_rx_int_ier_set (&rx_interrupts_enable) ;
    if (stack_error != PON_NO_ERROR)
    {
        return stack_error ;
    }



    printk(KERN_INFO "Reading from Tx CFG ONU Tx En\n");
    xgpon_error = ag_drv_ngpon_tx_cfg_onu_tx_en_get(&transmitter_enable, &standard, &speed);
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Get Tx state due to Driver error %d !", xgpon_error) ;
        return xgpon_error ;
    }


    for (tcont_id = NGPON_TRAFFIC_TCONT_ID_LOW;  tcont_id <= NGPON_TRAFFIC_TCONT_ID_HIGH; tcont_id++)
    {
       stack_error = ag_drv_ngpon_tx_int_ier0_set (tcont_id, BDMF_FALSE) ;
       if (stack_error != PON_NO_ERROR)
       {
          return stack_error ;
       }
    }
    p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Enabling interrupts done") ;

    tx_interrupts_enable.tx_plm_0         = BDMF_TRUE ;
    tx_interrupts_enable.tx_plm_1         = BDMF_FALSE ;
    tx_interrupts_enable.tx_plm_2         = BDMF_FALSE ;
    tx_interrupts_enable.fe_data_overun   = BDMF_FALSE ;
    tx_interrupts_enable.pd_underun       = BDMF_TRUE ;
    tx_interrupts_enable.pd_overun        = BDMF_FALSE ;
    tx_interrupts_enable.af_err           = BDMF_TRUE ;
    tx_interrupts_enable.rog_dif          = BDMF_FALSE ;
    tx_interrupts_enable.rog_len          = BDMF_FALSE ;
    tx_interrupts_enable.tx_tcont_32_dbr  = BDMF_FALSE ;
    tx_interrupts_enable.tx_tcont_33_dbr  = BDMF_FALSE ;
    tx_interrupts_enable. tx_tcont_34_dbr = BDMF_FALSE ;
    tx_interrupts_enable.tx_tcont_35_dbr  = BDMF_FALSE ;
    tx_interrupts_enable.tx_tcont_36_dbr  = BDMF_FALSE ;
    tx_interrupts_enable.tx_tcont_37_dbr  = BDMF_FALSE ;
    tx_interrupts_enable.tx_tcont_38_dbr  = BDMF_FALSE ;
    tx_interrupts_enable.tx_tcont_39_dbr  = BDMF_FALSE ;


    stack_error = ag_drv_ngpon_tx_int_ier1_set ( &tx_interrupts_enable ) ;
    if (stack_error != PON_NO_ERROR)
    {
        return stack_error ;
    }

    p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "IER1 interrupts enabled") ;

    npon_stack_rogue_onu_detect_init();

#ifdef CONFIG_BCM_GPON_TODD   
    ngpon_tod_register_funcs();
#endif 


    p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Done") ;

    pmd_dev_assign_pon_stack_callback(ngpon_set_pmd_fb_done);

    return BDMF_ERR_OK;
}


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Create all operation system resources.                                   */
/*                                                                            */
/******************************************************************************/
bdmf_error_t create_os_resources(void)
{
    bdmf_error_t resource_error;

    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "start %s", __FUNCTION__);

    /* Create task queues */
    /* PON -task queue - special queue containing the actual message */
    resource_error = bdmf_queue_create(&g_xgpon_db.os_resources.pon_messages_queue_id, 
        NGPON_QUEUE_LENGTH, sizeof(NGPON_MESSAGE));
    if (resource_error != BDMF_ERR_OK)
    {
        release_os_resources();
        return NGPON_ERROR_OS_ERROR;
    }

    bdmf_timer_init(&g_xgpon_db.os_resources.to1_timer_id, 
        (bdmf_timer_cb_t) p_pon_to1_timer_callback, 0);
    bdmf_timer_init(&g_xgpon_db.os_resources.to2_timer_id, 
        (bdmf_timer_cb_t) p_pon_to2_timer_callback, 0);
    bdmf_timer_init(&g_xgpon_db.os_resources.tk4_timer_id, 
        (bdmf_timer_cb_t) p_pon_tk4_timer_callback, 0);
    bdmf_timer_init(&g_xgpon_db.os_resources.tk5_timer_id, 
        (bdmf_timer_cb_t) p_pon_tk5_timer_callback, 0);
    bdmf_timer_init(&g_xgpon_db.os_resources.ds_sync_check_timer_id, 
        (bdmf_timer_cb_t) p_pon_ds_sync_check_timer_callback, 0);
    bdmf_timer_init(&g_xgpon_db.os_resources.toz_timer_id, 
        (bdmf_timer_cb_t)toz_timer_callback, 0);
    bdmf_timer_init(&g_xgpon_db.os_resources.to3_timer_id, 
        (bdmf_timer_cb_t)to3_timer_callback, 0);
    bdmf_timer_init(&g_xgpon_db.os_resources.to4_timer_id, 
        (bdmf_timer_cb_t)to4_timer_callback, 0);
    bdmf_timer_init(&g_xgpon_db.os_resources.to5_timer_id, 
        (bdmf_timer_cb_t)to5_timer_callback, 0);

#ifdef G989_3_AMD1
    g_xgpon_db.os_resources.tcpi_timer_is_running = BDMF_FALSE;
    bdmf_timer_init(&g_xgpon_db.os_resources.tcpi_timer_id, 
        (bdmf_timer_cb_t)tcpi_timer_callback, 0);
#endif

    /* Create Pon task */
    resource_error = bdmf_task_create("NGPON",
        g_xgpon_db.os_resources.pon_task_priority,
        g_xgpon_db.os_resources.stack_size, (void *)ngpon_pon_task,
        NULL, &g_xgpon_db.os_resources.pon_task_id);

    if (resource_error !=  BDMF_ERR_OK)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "OS error while spawning 'NGPON' task: %d", resource_error);
        return NGPON_ERROR_OS_ERROR;
    }

    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
        "finish %s", __FUNCTION__ );
#ifndef SIM_ENV
    spin_lock_init(&lock_ploam_list);
    INIT_LIST_HEAD(&us_ploam_list);
#endif
    return BDMF_ERR_OK;
}

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Delete all operation system resources.                                   */
/*                                                                            */
/******************************************************************************/
bdmf_error_t release_os_resources ( void )
{
    bdmf_error_t resource_error ;   

    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "start %s", __FUNCTION__ ) ;
        
    /**********************/
    /* Delete the tasks   */
    /**********************/
    if ( g_xgpon_db.os_resources.pon_task_id != 0 )
    {
        resource_error = bdmf_task_destroy ( g_xgpon_db.os_resources.pon_task_id ) ;
        if ( resource_error !=  BDMF_ERR_OK )
        {
           p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "OS error while stopping the 'NGPON' task: %d", resource_error ) ;
           return ( NGPON_ERROR_OS_ERROR ) ;
        }

        g_xgpon_db.os_resources.pon_task_id = 0 ;
    }


    /**********************/
    /* Delete task queues */
    /**********************/
    resource_error = bdmf_queue_delete ( & g_xgpon_db.os_resources.pon_messages_queue_id ) ;
    if ( resource_error !=  BDMF_ERR_OK )
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "OS error while deleting the 'NGPON' task queue: %d", resource_error ) ;
       return ( NGPON_ERROR_OS_ERROR ) ;
    }
    memset ( & g_xgpon_db.os_resources.pon_messages_queue_id, 0,
             sizeof (g_xgpon_db.os_resources.pon_messages_queue_id) ) ;

    /* Delete TO1 timer */
    bdmf_timer_delete ( & g_xgpon_db.os_resources.to1_timer_id ) ;

    /* Delete TO2 timer */
    bdmf_timer_delete ( &  g_xgpon_db.os_resources.to2_timer_id ) ;

    /* Delete TK4 timer */
    bdmf_timer_delete ( &  g_xgpon_db.os_resources.tk4_timer_id ) ;

    /* Delete TK5 timer */
    bdmf_timer_delete ( & g_xgpon_db.os_resources.tk5_timer_id ) ;

    bdmf_timer_delete (&g_xgpon_db.os_resources.toz_timer_id) ;

    bdmf_timer_delete (&g_xgpon_db.os_resources.to3_timer_id) ;

    bdmf_timer_delete (&g_xgpon_db.os_resources.to4_timer_id) ;

    bdmf_timer_delete (&g_xgpon_db.os_resources.to5_timer_id) ;

#ifdef G989_3_AMD1
    g_xgpon_db.os_resources.tcpi_timer_is_running = BDMF_FALSE;
    bdmf_timer_delete (&g_xgpon_db.os_resources.tcpi_timer_id) ;
#endif

    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "finish %s", __FUNCTION__ ) ;

    return( BDMF_ERR_OK ) ;
}

PON_ERROR_DTE ngpon_enable_alloc_id_by_index ( uint32_t xi_alloc_index, bdmf_boolean enable )
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;
    uint32_t      tcont_en_mask = 0 ;
    uint8_t       tcont_aux_en_mask = 0 ;

    if ( xi_alloc_index < BITS_PER_WORD)
    {

      xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_en1_get( &tcont_en_mask ); 
      if ( xgpon_error != PON_NO_ERROR )
        {
          p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                  "Unable to Get bitmask at HW. error %lu",xgpon_error);
          return ( xgpon_error ) ;
        }


      if ( enable )
         tcont_en_mask |= (1 << xi_alloc_index) ;
      else
         tcont_en_mask &= ~(1 << xi_alloc_index) ;

      xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_en1_set( tcont_en_mask ); 
      if (xgpon_error != PON_NO_ERROR)
        {
          p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                  "Unable to Enable at HW. error %lu",xgpon_error);
          return (xgpon_error);
        }
    }
    else  if ( xi_alloc_index <= NGPON_TRAFFIC_TCONT_ID_HIGH )
    {
      xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_en2_get( &tcont_aux_en_mask ); 
      if (xgpon_error != PON_NO_ERROR)
        {
          p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                  "Unable to Get at HW. error %lu",xgpon_error);
          return (xgpon_error);
        }

      if ( enable )
         tcont_aux_en_mask |= (1 << ( xi_alloc_index - BITS_PER_WORD )) ;
      else
         tcont_aux_en_mask &= ~(1 << ( xi_alloc_index - BITS_PER_WORD )) ;
 
      xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_en2_set( tcont_aux_en_mask ); 
      if (xgpon_error != PON_NO_ERROR)
        {
          p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                  "Unable to Enable at HW. error %lu",xgpon_error);
          return (xgpon_error) ;
        }
    }
    else
    {
          p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                  "Illegal Alloc ID index %lu", xi_alloc_index );
          return ( BDMF_ERR_PARM ) ;
    }

    return (PON_NO_ERROR) ;
}

PON_ERROR_DTE ngpon_enable_alloc_id_and_tcont ( PON_TCONT_ID_DTE xi_tcont_id ,uint32_t xi_alloc_index, PON_ALLOC_ID_DTE xi_alloc_id )
{
    PON_ERROR_DTE xgpon_error ;
    uint8_t       assoc_alloc_id_tcont_id_reg_idx = 0;
    uint8_t       assoc_alloc_id_tcont_id_in_reg ;
    uint8_t       tcont_num[NGPON_NUM_OF_TCONT_AT_ALLOC_INDEX_ARRAY] ;        


    /* ALLOC index to Alloc ID */
    xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_set (xi_alloc_index, xi_alloc_id);
    if (xgpon_error != PON_NO_ERROR)
    {
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                 "Unable to set_alloc_id at HW. error %lu",xgpon_error);
         return ( xgpon_error ) ;
    }

   /* Enable ALLOC ID  */
    xgpon_error = ngpon_enable_alloc_id_by_index (xi_alloc_index, BDMF_TRUE);
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to get rx_bwmap_trfc_tcnt_assoc  due to Driver errorEnable alloc ID %d !", xgpon_error ) ;
    }

    /* Associate the ALLOC Id to a TCONT   */

    assoc_alloc_id_tcont_id_reg_idx = xi_alloc_index / 4 ; /* 4 alloc ids per register */
    assoc_alloc_id_tcont_id_in_reg = xi_alloc_index % 4 ;
 
    xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_tcnt_assoc_get( assoc_alloc_id_tcont_id_reg_idx,
                                                             &tcont_num[0], &tcont_num[1], &tcont_num[2], &tcont_num[3] ) ;
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to get rx_bwmap_trfc_tcnt_assoc  due to Driver error %d !", xgpon_error ) ;
    }

    tcont_num[assoc_alloc_id_tcont_id_in_reg] = xi_tcont_id ;   /* index is always in range 0..3 */
    xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_tcnt_assoc_set( assoc_alloc_id_tcont_id_reg_idx,
                                                             tcont_num[0], tcont_num[1], tcont_num[2], tcont_num[3] ) ;
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set rx_bwmap_trfc_tcnt_assoc  due to Driver error %d !", xgpon_error ) ;
    }

    xgpon_error =  ag_drv_ngpon_tx_cfg_tcont_to_onu_id_map_set (xi_tcont_id, NGPON_ONU_INDEX_ONU_0) ;
    if ( PON_NO_ERROR != xgpon_error ) 
    {
        p_log ( ge_onu_logger.sections.stack.downstream_ploam.unknown_message_id,
                "Unable to set ONU to T-CONT mapping in Tx. error %lu",xgpon_error);
     }

    return (PON_NO_ERROR) ;
}

void ngpon_config_alloc_id ( NGPON_ASSIGN_ALLOC_ID_INDICATION_PARAMS assign_alloc_id_struct )
{
    uint32_t        table_index = 0 ;
    uint32_t        alloc_index = 0 ;
    uint32_t        empty_index = 0 ;
    PON_ERROR_DTE   xgpon_error = PON_NO_ERROR;
    bdmf_boolean  alloc_id_found = BDMF_FALSE;

    /*    uint32_t        int_lock_key ; */

    /* Check range of alloc-id */
    if ( !NGPON_ALLOC_ID_IN_RANGE( assign_alloc_id_struct.alloc_id ) )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Alloc id out of range: %d !", assign_alloc_id_struct.alloc_id ) ;
        return  ;
    }


    /* Search for this alloc id in the alloc table - index 0 is resereved for default ALLOC ID */
    for ( table_index = 0 ; table_index < NGPON_MAX_NUM_OF_TRAFFIC_TCONT_IDS ; table_index++ )
    {
        if ( g_xgpon_db.tconts_table [ table_index ].alloc_id == assign_alloc_id_struct.alloc_id )
        {
            alloc_id_found =BDMF_TRUE ;
            alloc_index = table_index ;
            p_log ( ge_onu_logger.sections.stack.debug.general_id,
                    "Found Alloc ID %d at SW TCONT table index = %d",
                    assign_alloc_id_struct.alloc_id ,table_index ) ;
            break ;
        }
    }


    /* Assign operation */
    if ( assign_alloc_id_struct.assign_flag == BDMF_TRUE )
    {
        /* if the alloc id already apears in the table */
        if ( alloc_id_found == BDMF_TRUE )
        {
            /* if this alloc already assign by the OLT */
            if ( g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag == BDMF_TRUE )
            {
                p_log ( ge_onu_logger.sections.stack.debug.general_id,
                        "Alloc-ID %d already assigned\n",
                        g_xgpon_db.tconts_table [ alloc_index ].alloc_id ) ;
               goto clean_up ;
            }
            else
            {
                /* If a TCONT is already associated to this Alloc, update the HW table */
                if ( g_xgpon_db.tconts_table [ alloc_index ].config_tcont_valid_flag == BDMF_TRUE )
                {
                   xgpon_error = ngpon_enable_alloc_id_and_tcont ( g_xgpon_db.tconts_table [ alloc_index ].tcont_id ,
                                                      alloc_index, assign_alloc_id_struct.alloc_id );
                   if ( PON_NO_ERROR != xgpon_error ) 
                   {
                        p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                                "Unable to set_alloc_id at HW. error %lu",xgpon_error);
                        goto clean_up ;
                   }
                   g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag = BDMF_TRUE ;
                   p_log ( ge_onu_logger.sections.stack.debug.general_id,
                             "Added Alloc id %lu at TCONT Table Index %d for TCONT ID %d",
                              assign_alloc_id_struct.alloc_id, alloc_index ,
                              g_xgpon_db.tconts_table [ alloc_index ].tcont_id ) ;
                }
                /* error  - the alloc apper in the table but not associate to any tcont */
                else
                {
                    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                             "The alloc  %d appers in the Alloc IDs table but not associate to any Tcont !", assign_alloc_id_struct.alloc_id ) ;
                    goto clean_up ;
                }

            }
        }
        /* The alloc does not apper in the TCONT Table table */
        else
        {
            /* Find empty entry in the tcont table - index 0 is resereved for default ALLOC ID */
            for ( empty_index = 1 ; empty_index < NGPON_MAX_NUM_OF_TRAFFIC_TCONT_IDS ; empty_index++ )
            {
                if ( ( g_xgpon_db.tconts_table [ empty_index ].assign_alloc_valid_flag == BDMF_FALSE ) &&
                     ( g_xgpon_db.tconts_table [ empty_index ].config_tcont_valid_flag == BDMF_FALSE ) )
                {
                    break;
                }
            }

            if ( NGPON_MAX_NUM_OF_TRAFFIC_TCONT_IDS <= empty_index ) 
            {
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                        "Alloc ID table is full - Unable to configure Alloc ID %d", 
                        assign_alloc_id_struct.alloc_id ) ;
                goto clean_up ;
            }

            /* Assign the Alloc to a Dummy TCONT (last Traffic TCONT so it can answer with idle XGEMs to allocations*/
            xgpon_error = ngpon_enable_alloc_id_and_tcont ( NGPON_DUMMY_TCONT_ID_VALUE ,
                                                            empty_index, assign_alloc_id_struct.alloc_id );
            if ( PON_NO_ERROR != xgpon_error ) 
            {
                 p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                         "Unable to set_alloc_id at HW. error %lu",xgpon_error);
                 goto clean_up ;
            }
            g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag = BDMF_TRUE ;
            p_log ( ge_onu_logger.sections.stack.debug.general_id,
                      "Added Alloc id %lu at TCONT Table Index %d for TCONT ID %d",
                       assign_alloc_id_struct.alloc_id, empty_index ,
                       NGPON_DUMMY_TCONT_ID_VALUE ) ;

            g_xgpon_db.tconts_table [ empty_index ].alloc_id = assign_alloc_id_struct.alloc_id ;
            g_xgpon_db.tconts_table [ empty_index ].assign_alloc_valid_flag = BDMF_TRUE ;
            g_xgpon_db.tconts_table [ empty_index ].tcont_id = NGPON_DUMMY_TCONT_ID_VALUE ;
            g_xgpon_db.tconts_table [ empty_index ].config_tcont_valid_flag = BDMF_FALSE ;
        }
    }
    /* Deassign operation*/
    else
    {

        /* if the alloc id apper in the table */
        if ( ( alloc_id_found == BDMF_TRUE ) && 
             ( g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag == BDMF_TRUE ) )
        {
            /* Disable the alloc at the HW table  */
           xgpon_error = ngpon_enable_alloc_id_by_index (alloc_index, BDMF_FALSE ) ;
            if (xgpon_error != PON_NO_ERROR)
            {
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                         "Unable to write alloc id- %d at table  due to Driver error !", g_xgpon_db.tconts_table [ alloc_index ].alloc_id ) ;
                goto clean_up ;
            }
            /* If there is tcont that assigned to this alloc dont remove the alloc just update his flag */
            if (g_xgpon_db.tconts_table [ alloc_index ].config_tcont_valid_flag == BDMF_FALSE )
            {
                g_xgpon_db.tconts_table [ alloc_index ].alloc_id = NGPON_ALLOC_ID_NOT_ASSIGNED ;
            }
            g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag = BDMF_FALSE ;

        }
        /* if the alloc not assign yet */
        else
        {
            /* Log */
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                     "Unable to deallocate alloc id- %d , the alloc was not assign yet!",
                     g_xgpon_db.tconts_table [ alloc_index ].alloc_id ) ;

            goto clean_up ;
        }
    }


clean_up:  

    xgpon_error = PON_NO_ERROR ;
}


PON_ERROR_DTE  ngpon_assign_tcont ( PON_TCONT_ID_DTE xi_tcont_id, PON_ALLOC_ID_DTE xi_alloc_id )
{
#if 0
    STT_ERROR_DTE  stt_error ;
#endif
    /*    uint32_t     int_lock_key ; */
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;
    uint32_t     index       = 0 ;
    uint32_t     tcont_index = 0 ;
    uint32_t     alloc_index = 0 ;
    uint32_t     empty_index = 0 ;
    bdmf_boolean    tcont_found_in_table = BDMF_FALSE ;
    bdmf_boolean    alloc_found_in_table = BDMF_FALSE ;


    /* Verify that the stack is initialized - TBD */

    if ( xi_tcont_id > NGPON_TRAFFIC_TCONT_ID_HIGH ) 
          return NGPON_ERROR_TCONT_ID_OUT_OF_RANGE ;

    /* Disable interrupt to prevent race between ISR and API */
#if 0
    stt_error = fi_stt_schedule_disable_interrupts ( & int_lock_key ) ;
    if ( stt_error != CE_STT_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Failed to disable interrupt (%d)", stt_error ) ;
        return ( NGPON_ERROR_OS_ERROR ) ;
    }
#endif

    /* Lookup  tcont-id and alloc-id in the table */
    for ( index = 0 ; index < NGPON_MAX_NUM_OF_TRAFFIC_TCONT_IDS ; index++ )
    {
        if ( ( g_xgpon_db.tconts_table [ index ].assign_alloc_valid_flag == BDMF_TRUE )||
             ( g_xgpon_db.tconts_table [ index ].config_tcont_valid_flag == BDMF_TRUE ) )
        {
            if ( g_xgpon_db.tconts_table [ index ].tcont_id == xi_tcont_id )
            {
                tcont_found_in_table = BDMF_TRUE ;
                tcont_index = index ;
            }

            if ( g_xgpon_db.tconts_table [ index ].alloc_id == xi_alloc_id )
            {
                alloc_found_in_table = BDMF_TRUE ;
                alloc_index = index ;
            }
        }
    }
    /* Tcont is already in the table - return error */
    if (  ( tcont_found_in_table == BDMF_TRUE ) &&
          ( g_xgpon_db.tconts_table [ alloc_index ].tcont_id != NGPON_DUMMY_TCONT_ID_VALUE ) )
    {
        if (  g_xgpon_db.tconts_table [ tcont_index ].config_tcont_valid_flag == BDMF_TRUE ) 
        {
            p_log ( ge_onu_logger.sections.stack.debug.general_id,
                     "TCONT %lu is already assigned to Alloc ID %d",
                     g_xgpon_db.tconts_table [ tcont_index ].tcont_id,
                     g_xgpon_db.tconts_table [ tcont_index ].alloc_id ) ;
            xgpon_error = NGPON_ERROR_TCONT_IS_ALREADY_CONFIGURED ;
            goto clean_up ;
        }
        else
        {
            p_log ( ge_onu_logger.sections.stack.debug.general_id,
                     "TCONT %lu is associate to another alloc %lu  ",
                     xi_tcont_id , g_xgpon_db.tconts_table [ tcont_index ].alloc_id ) ;
            xgpon_error = NGPON_ERROR_INVALID_STATE ;
            goto clean_up ;
        }

    }

    /* If the alloc appers in the table */
    if ( alloc_found_in_table == BDMF_TRUE )
    {
        /* check if this alloc already associate to another tcont*/
        if ( g_xgpon_db.tconts_table [ alloc_index ].config_tcont_valid_flag == BDMF_TRUE )
        {
            p_log ( ge_onu_logger.sections.stack.debug.general_id,
                    "Alloc-ID %lu already assigned to Tcont %d",
                    xi_alloc_id,
                    g_xgpon_db.tconts_table [ alloc_index ].tcont_id ) ;
            xgpon_error = NGPON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT ;
            goto clean_up ;
        }
        else
        {
            /* If the alloc appers in the table and has no tcont associate nor assign alloc flag - wrong state */
            if ( ( g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag == BDMF_FALSE ) &&
                 ( g_xgpon_db.tconts_table [ alloc_index ].tcont_id != NGPON_DUMMY_TCONT_ID_VALUE ) )
            {
                xgpon_error = NGPON_ERROR_INVALID_STATE ;

                goto clean_up ;
            }
            else
            {
                /* Alloc Id was already assigned by the OLT - we need to enable all HW RX/TX tables */
                xgpon_error = ngpon_enable_alloc_id_and_tcont ( xi_tcont_id , alloc_index, xi_alloc_id );
                if (PON_NO_ERROR != xgpon_error) 
                {
                     p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                             "Unable to set_alloc_id at HW. error %lu",xgpon_error);
                     goto clean_up;
                }
                g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag = BDMF_TRUE ;
                p_log ( ge_onu_logger.sections.stack.debug.general_id,
                        "Added Alloc id %lu at TCONT Table Index %d for TCONT ID %d",
                         xi_alloc_id, alloc_index , xi_tcont_id ) ;
                g_xgpon_db.tconts_table [ alloc_index ].tcont_id = xi_tcont_id ;
                g_xgpon_db.tconts_table [ alloc_index ].config_tcont_valid_flag = BDMF_TRUE ;
            }
        }
    }

    /* if the alloc and the tcont are not in the table */
    else
    {
        for ( empty_index = 0 ; empty_index < NGPON_MAX_NUM_OF_TCONT_IDS ; empty_index++ )
        {
            if ( ( g_xgpon_db.tconts_table [ empty_index ].assign_alloc_valid_flag == BDMF_FALSE ) &&
                 ( g_xgpon_db.tconts_table [ empty_index ].config_tcont_valid_flag == BDMF_FALSE ) )
            {
                break;
            }
        }

        if ( empty_index >= NGPON_MAX_NUM_OF_TCONT_IDS )
        {            
            xgpon_error = NGPON_ERROR_TX_TCONT_TABLE_IS_FULL ;
            goto clean_up ;
        }

        /* add tcont to data base */
        g_xgpon_db.tconts_table [ empty_index ].alloc_id = xi_alloc_id ;
        g_xgpon_db.tconts_table [ empty_index ].tcont_id = xi_tcont_id ;
        g_xgpon_db.tconts_table [ empty_index ].config_tcont_valid_flag =  BDMF_TRUE ;
        g_xgpon_db.tconts_table [ empty_index ].assign_alloc_valid_flag =  BDMF_TRUE ;
        xgpon_error = ngpon_enable_alloc_id_and_tcont ( xi_tcont_id , empty_index, xi_alloc_id );
    }

clean_up:  
    /* Enable interrupt */
    return ( xgpon_error ) ;
}


PON_ERROR_DTE ngpon_config_xgem_flow(PON_FLOW_ID_DTE xi_flow_id,
    NGPON_XGEM_PORT_ID xi_port_id, PON_FLOW_PRIORITY_DTE xi_priority, 
    NGPON_ENCRYPTION_RING xi_encrypt, bdmf_boolean xi_crc_enable)    
{
    PON_ERROR_DTE xgpon_error;
    uint32_t flow_id_index;
    ngpon_rx_assign_flow flow;

    /*
     * Since the encryption ring configuration is provided in the MIB format:
     *     0 - unencrypted
     *     1 - unicast encryption (bidirectional)
     *     2 - multicast encryption at this GEM
     *     3 - unicast encryption (downstream only)
     *  while the register shall be set to:
     *     0 - ONU#1 key set
     *     1 - ONU#2 key set
     *     2 - multicast key set
     *     3 - plain text (no encryption)
     *  the mapping is provided by the following table
     */
    static uint8_t enc_ring_to_reg_mapping[] = {
      0, /* unencrypted */
      0, /* unicast encryption, we _always_ use keys of ONU #1 */
      2, /* multicast key set */
      0  /* unicast encryption, downstream only - but here the downstream is configured , we _always_ use keys of ONU #1 */
    } ;


    if (!NGPON_XGEM_FLOW_ID_IN_RANGE(xi_flow_id))
        return NGPON_ERROR_XGEM_FLOW_ID_OUT_OF_RANGE;

    if (!NGPON_XGEM_PORT_ID_IN_RANGE( xi_port_id))
        return NGPON_ERROR_XGEM_PORT_ID_OUT_OF_RANGE;
                
    for (flow_id_index = 0; flow_id_index < NGPON_MAX_NUM_OF_XGEM_FLOW_IDS; flow_id_index ++) 
    {
        if (xi_port_id == g_xgpon_db.ds_xgem_flow_table[flow_id_index].xgem_port_id &&
            g_xgpon_db.ds_xgem_flow_table [flow_id_index].flow_configured && 
            g_xgpon_db.ds_xgem_flow_table[xi_flow_id].enable_flag)
        {
            return NGPON_ERROR_XGEM_PORT_ID_IS_ALREADY_CONFIGURED;
        }
    }

    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].encryption_ring = enc_ring_to_reg_mapping[xi_encrypt];                           
    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].encryption = (xi_encrypt)? BDMF_TRUE : BDMF_FALSE ;
    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].flow_id = xi_flow_id;                             
    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].flow_priority = xi_priority;                          
    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].xgem_port_id = xi_port_id;                           
    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].xgem_port_mask_id = xi_port_id;                           
    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].crc_enable = xi_crc_enable;

    /* Status is set to enable once enable XGEM is called */
    g_xgpon_db.ds_xgem_flow_table [ xi_flow_id ].enable_flag = BDMF_FALSE;

    flow.portid = g_xgpon_db.ds_xgem_flow_table[xi_flow_id].xgem_port_id;
    flow.priority =  g_xgpon_db.ds_xgem_flow_table[xi_flow_id].flow_priority;
    flow.enc_ring = g_xgpon_db.ds_xgem_flow_table[xi_flow_id].encryption_ring; 
    flow.crc_en = g_xgpon_db.ds_xgem_flow_table[xi_flow_id].crc_enable;
    flow.en = g_xgpon_db.ds_xgem_flow_table[xi_flow_id].enable_flag;

    if (OPERATION_STATE_O5 == g_xgpon_db.link_params.operation_sm.activation_state ||
        OPERATION_STATE_O4 == g_xgpon_db.link_params.operation_sm.activation_state)  
    {
        xgpon_error = ag_drv_ngpon_rx_assign_flow_set(
            g_xgpon_db.ds_xgem_flow_table[xi_flow_id].flow_id, &flow);

        if (PON_NO_ERROR != xgpon_error) 
            return xgpon_error;

        p_log(ge_onu_logger.sections.stack.debug.general_id, "assign_flow_set - done");

        g_xgpon_db.ds_xgem_flow_table[xi_flow_id].flow_configured = BDMF_TRUE;
    }

    return PON_NO_ERROR;
}


PON_ERROR_DTE ngpon_control_xgem_flow(PON_FLOW_ID_DTE xi_flow_id, bdmf_boolean  xi_flow_status)
{
    PON_ERROR_DTE         xgpon_error = PON_NO_ERROR;
    ngpon_rx_assign_flow  ngpon_flow;    

    if ( ! NGPON_XGEM_FLOW_ID_IN_RANGE( xi_flow_id ) )
        return NGPON_ERROR_XGEM_FLOW_ID_OUT_OF_RANGE ;

    xgpon_error =  ag_drv_ngpon_rx_assign_flow_get (xi_flow_id, &ngpon_flow );
    if (PON_NO_ERROR != xgpon_error) 
        return xgpon_error ;
    ngpon_flow.en = xi_flow_status ;
    xgpon_error =  ag_drv_ngpon_rx_assign_flow_set (xi_flow_id, &ngpon_flow );
    if (PON_NO_ERROR != xgpon_error) 
        return xgpon_error ;

    g_xgpon_db.ds_xgem_flow_table [ xi_flow_id ].enable_flag  = xi_flow_status ; 

    return ( PON_NO_ERROR ) ;
}


PON_ERROR_DTE ngpon_remove_xgem_flow ( PON_FLOW_ID_DTE xi_flow_id )
{
    PON_ERROR_DTE         xgpon_error = PON_NO_ERROR;
    ngpon_rx_assign_flow  ngpon_flow;    

    if ( ! NGPON_XGEM_FLOW_ID_IN_RANGE( xi_flow_id ) )
        return NGPON_ERROR_XGEM_FLOW_ID_OUT_OF_RANGE ;

    if ( NGPON_XGEM_FLOW_ID_UNASSIGNED == g_xgpon_db.ds_xgem_flow_table [ xi_flow_id ].flow_id ) 
        return NGPON_ERROR_FLOW_ID_IS_NOT_CONFIGURED ;

    xgpon_error =  ag_drv_ngpon_rx_assign_flow_get (xi_flow_id, &ngpon_flow );
    if (PON_NO_ERROR != xgpon_error) 
        return xgpon_error ;
    ngpon_flow.en = BDMF_FALSE ;
    xgpon_error =  ag_drv_ngpon_rx_assign_flow_set (xi_flow_id, &ngpon_flow );
    if (PON_NO_ERROR != xgpon_error) 
        return xgpon_error ;

    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].flow_id = NGPON_XGEM_FLOW_ID_UNASSIGNED ;
    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].xgem_port_id = NGPON_XGEM_PORT_ID_UNASSIGNED ;
    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].enable_flag = BDMF_FALSE ;
    g_xgpon_db.ds_xgem_flow_table[xi_flow_id].flow_configured = BDMF_FALSE ;

    return ( PON_NO_ERROR ) ;
}


PON_ERROR_DTE ngpon_remove_tcont ( PON_TCONT_ID_DTE xi_tcont_id )
{
    PON_ERROR_DTE       xgpon_error = PON_NO_ERROR ;
    uint32_t            alloc_index ;

    for ( alloc_index = 0 ; alloc_index < NGPON_MAX_NUM_OF_TCONT_IDS ; alloc_index ++ ) 
    {
        if ( ( g_xgpon_db.tconts_table [ alloc_index ].tcont_id == xi_tcont_id ) &&
             ( g_xgpon_db.tconts_table [ alloc_index ].config_tcont_valid_flag == BDMF_TRUE ) )
                break;
    }

    if ( alloc_index >= NGPON_MAX_NUM_OF_TCONT_IDS ) 
        return NGPON_ERROR_TCONT_ID_IS_NOT_CONFIGURED ;


    if ( g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag == BDMF_TRUE )
    {
        /* Disable the alloc at the HW table  */
        xgpon_error =  ngpon_enable_alloc_id_by_index (alloc_index, BDMF_FALSE );
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                     "Unable to write alloc id- %d at table  due to Driver error !", g_xgpon_db.tconts_table [ alloc_index ].alloc_id ) ;
            return ( xgpon_error ) ;
        }

        /* Assign the Alloc to a Dummy TCONT (last Traffic TCONT so it can answer with idle XGEMs to allocations*/
        xgpon_error = ngpon_enable_alloc_id_and_tcont ( NGPON_DUMMY_TCONT_ID_VALUE ,
                                                        alloc_index, g_xgpon_db.tconts_table [ alloc_index ].alloc_id );
        if (PON_NO_ERROR != xgpon_error) 
        {
             p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                     "Unable to set_alloc_id at HW. error %lu",xgpon_error);
        }
        g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag = BDMF_FALSE ;
        p_log ( ge_onu_logger.sections.stack.debug.general_id,
                  "Added Alloc id %lu at TCONT Table Index %d for TCONT ID %d",
                   g_xgpon_db.tconts_table [ alloc_index ].alloc_id, alloc_index ,
                   NGPON_DUMMY_TCONT_ID_VALUE ) ;

         g_xgpon_db.tconts_table[alloc_index].tcont_id = NGPON_DUMMY_TCONT_ID_VALUE ;
    }
    else
    {
         g_xgpon_db.tconts_table[alloc_index].tcont_id = NGPON_TCONT_ID_NOT_ASSIGNED ;
         g_xgpon_db.tconts_table[alloc_index].alloc_id = NGPON_ALLOC_ID_NOT_ASSIGNED ;
    }

   g_xgpon_db.tconts_table [alloc_index].config_tcont_valid_flag = BDMF_FALSE ;
  

    return ( PON_NO_ERROR ) ;
}



PON_ERROR_DTE  ngpon_flush_traffic_alloc_table (void)
{
    uint32_t alloc_index ;
 
    /* Flush entries in  SW Rx allocation table */
    for (alloc_index = 0; alloc_index <= NGPON_TRAFFIC_TCONT_ID_HIGH; alloc_index++)
    {
      if (g_xgpon_db.tconts_table[alloc_index].config_tcont_valid_flag)
      {
 
          if ((NGPON_DEFAULT_TCONT_ID != g_xgpon_db.tconts_table[alloc_index].tcont_id) &&
             (0xFF != g_xgpon_db.tconts_table[alloc_index].tcont_id))
          {
                ngpon_callbacks.flush_tcont_callback(g_xgpon_db.tconts_table[alloc_index].tcont_id);
          }
       }
    }
    return PON_NO_ERROR ;
}



PON_ERROR_DTE  ngpon_clear_traffic_alloc_table ( void )
{
    PON_ERROR_DTE xgpon_error ;
    uint32_t alloc_index ;

    /* Clear SW Rx allocation table */
    for ( alloc_index = 0 ; alloc_index <= NGPON_TRAFFIC_TCONT_ID_HIGH ; alloc_index++ )
    {
      if (g_xgpon_db.tconts_table [ alloc_index ].config_tcont_valid_flag)
      {

          if ((NGPON_DEFAULT_TCONT_ID != g_xgpon_db.tconts_table[alloc_index].tcont_id) &&
              (0xFF != g_xgpon_db.tconts_table[alloc_index].tcont_id))
          {
                ngpon_callbacks.flush_tcont_callback(g_xgpon_db.tconts_table[alloc_index].tcont_id);
           }
       }

       
       /* Disable the alloc at the HW table  */

       xgpon_error = ngpon_enable_alloc_id_by_index (alloc_index, BDMF_FALSE) ;
       if (PON_NO_ERROR != xgpon_error) 
       {
            p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                    "Unable to Enable at HW. error %lu",xgpon_error);
       }

       g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag = BDMF_FALSE ;

       if ( g_xgpon_db.tconts_table [ alloc_index ].config_tcont_valid_flag == BDMF_FALSE )
       {
           g_xgpon_db.tconts_table [ alloc_index ].alloc_id = NGPON_ALLOC_ID_NOT_ASSIGNED ;
           g_xgpon_db.tconts_table [ alloc_index ].tcont_id = NGPON_TCONT_ID_NOT_ASSIGNED ;
       }
    }


    return ( PON_NO_ERROR ) ;
}


void  ngpon_clear_sw_tcont_table ( void )
{
    uint32_t alloc_index ;

    /* Clear SW Rx allocation table */
    for ( alloc_index = 0 ; alloc_index < NGPON_MAX_NUM_OF_TCONT_IDS ; alloc_index++ )
    {

       g_xgpon_db.tconts_table [ alloc_index ].assign_alloc_valid_flag = BDMF_FALSE ;
       g_xgpon_db.tconts_table [ alloc_index ].alloc_id = NGPON_ALLOC_ID_NOT_ASSIGNED ;
       g_xgpon_db.tconts_table [ alloc_index ].tcont_id = NGPON_TCONT_ID_NOT_ASSIGNED ;
       g_xgpon_db.tconts_table [ alloc_index ].config_tcont_valid_flag = BDMF_FALSE ;
    }
}


void  ngpon_clear_sw_xgem_flow_table ( void )
{
    uint32_t flow_index ;

    /* Clear SW Rx allocation table */
    for ( flow_index = 0 ; flow_index < NGPON_MAX_NUM_OF_XGEM_FLOW_IDS ; flow_index++ )
    {
       g_xgpon_db.ds_xgem_flow_table [ flow_index ].flow_id = NGPON_XGEM_FLOW_ID_UNASSIGNED ;
       g_xgpon_db.ds_xgem_flow_table [ flow_index ].xgem_port_id = NGPON_XGEM_PORT_ID_UNASSIGNED ;
       g_xgpon_db.ds_xgem_flow_table [ flow_index ].enable_flag = BDMF_FALSE ;
       g_xgpon_db.ds_xgem_flow_table [ flow_index ].flow_configured = BDMF_FALSE ;
       g_xgpon_db.ds_xgem_flow_table [ flow_index ].encryption_ring = 0;
       g_xgpon_db.ds_xgem_flow_table [ flow_index ].encryption = BDMF_FALSE;
    }
}

PON_ERROR_DTE  ngpon_clear_ranging_alloc_table ( void )
{
    PON_ERROR_DTE xgpon_error ;
    uint32_t      alloc_id_index ;

    xgpon_error = ag_drv_ngpon_rx_bwmap_rngng_allocid_en_set ( BDMF_FALSE ); 
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear RX rngng_allocid_en_set %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }
    /* Clear Ranging allocation table */
    for ( alloc_id_index = 0 ; alloc_id_index < NGPON_MAX_NUM_OF_RANGING_ALLOC_IDS ; alloc_id_index++ )
    {
        xgpon_error = ag_drv_ngpon_rx_bwmap_rngng_allocid_set ( alloc_id_index, 0 ); 
        if (xgpon_error != PON_NO_ERROR)
        {
           p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear Rx Ranging alloc id (rngng_allocid)  %d !", xgpon_error ) ;
           return ( xgpon_error ) ;
        }

        /* The SW TCONT table resereves TCONTs 0-39 for Traffic and TCONTs 40,41,42,43 for Ranging/SN */
        g_xgpon_db.tconts_table [ alloc_id_index + NGPON_MAX_NUM_OF_TRAFFIC_TCONT_IDS ].assign_alloc_valid_flag = BDMF_FALSE ;

        if ( g_xgpon_db.tconts_table [ alloc_id_index + NGPON_MAX_NUM_OF_TRAFFIC_TCONT_IDS ].config_tcont_valid_flag == BDMF_FALSE )
        {
            g_xgpon_db.tconts_table [ alloc_id_index + NGPON_MAX_NUM_OF_TRAFFIC_TCONT_IDS ].alloc_id = NGPON_ALLOC_ID_NOT_ASSIGNED ;
        }
    }
    return ( PON_NO_ERROR ) ;
}

/* Clear paramter set 5: according to 989.3 standart, section 12.1 and 987.3, 12.2.4 */
/* Discard burst profiles, ONU-ID, default Alloc-ID, 
   default XGEM Port ID, Assigned alloc-IDs, EqD */
PON_ERROR_DTE clear_onu_config_5(void)
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR;
    NGPON_PLOAM_TYPE ploam_index;

    xgpon_error = clear_default_alloc_id_and_default_xgem_flow_id();
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to clear_default_alloc_id_and_default_xgem_flow_id due to Driver error %d", 
            xgpon_error);
        return xgpon_error;
    }


    /* Clear Ranging allocation table */
    xgpon_error = ngpon_clear_ranging_alloc_table();
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
        "Unable to clear Rx Ranging alloc id and its valid bit due to Driver error %d", 
        xgpon_error);
        return xgpon_error;
    }    

    /* Clear Traffic allocation table */
    xgpon_error = ngpon_clear_traffic_alloc_table();
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
        "Unable to clear Rx Traffic alloc id and its valid bit due to Driver error %d", 
        xgpon_error);
        return xgpon_error;
    }

    /* Clear RX ONU ID */
    xgpon_error = clear_all_rx_onu_ids();
    if (xgpon_error != PON_NO_ERROR)
        return xgpon_error;


    /* Clear TX burst Profiles  */
    xgpon_error = clear_burst_profiles();
    if (xgpon_error != PON_NO_ERROR)
        return xgpon_error;
    
    /* Clear TX ONU ID (The 3 ONU indexes must have different ONU Ids)  */
    xgpon_error = clear_all_tx_onu_ids();
    if (xgpon_error != PON_NO_ERROR)
         return xgpon_error;

    /* Clear PLOAMs*/
    for (ploam_index = NGPON_PLOAM_TYPE_LOW; ploam_index <= NGPON_PLOAM_TYPE_HIGH; ploam_index++)
    {
        xgpon_error = ag_drv_ngpon_tx_cfg_plm_cfg_set(ploam_index, BDMF_FALSE, 
            BDMF_FALSE, BDMF_FALSE, BDMF_FALSE);
        if (xgpon_error != PON_NO_ERROR)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to disable Tx ploams due to Driver error %d", 
            xgpon_error);
            return xgpon_error;
        }
    }
    return PON_NO_ERROR;
}

/* Clear paramter set 7: according to 989.3 standart, section 12.1 */
/* Discard System profile and channels profiles */
void clear_onu_config_7(void)
{
    int i;
    NGPON_DS_SYSTEM_PROFILE_PLOAM system = {};
    NGPON_DS_CHANNEL_PROFILE_PLOAM channel = {};

    save_system_profile_to_db(&system);
    if ((g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGPON) &&
        (g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGS))
    {
       g_xgpon_db.link_params.system_profile.system_profile_received = BDMF_FALSE;
    }
    else
    {
       g_xgpon_db.link_params.system_profile.system_profile_received = BDMF_TRUE;
    }

    g_xgpon_db.link_params.ch_profile.channel_profile_num = 0 ;
    for (i=0; i<16; i++) 
    { 
        channel.twdm.control.index = i;
        channel.twdm.control.us_void_indicator = 1;
        channel.twdm.control.ds_void_indicator = 1;
        save_channel_profile_to_db(&channel);
    }

    if ((g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGPON) &&
        (g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGS))
    {
       g_xgpon_db.link_params.ch_profile.channel_profile_received = BDMF_FALSE;
    }
    else
    {
       g_xgpon_db.link_params.ch_profile.channel_profile_received = BDMF_TRUE;
    }
}

PON_ERROR_DTE clear_default_alloc_id_and_default_xgem_flow_id ( void )
{
    PON_ERROR_DTE xgpon_error;
    ngpon_rx_assign_flow default_flow;

    /* Disable Default ALLOC ID and associate it with ONU ID */
    xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_en1_set(0x0);
    if (PON_NO_ERROR != xgpon_error) 
    {
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                 "Unable to set trfc_allocid_en1. error %lu",xgpon_error);
         return xgpon_error ;
    }

    xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_en2_set(0x0);
    if (PON_NO_ERROR != xgpon_error) 
    {
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                 "Unable to set trfc_allocid_en2. error %lu",xgpon_error);
         return xgpon_error ;
    }
    /* to be on the safe side */
    xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_set(NGPON_DEFAULT_TCONT_ID, BDMF_FALSE);
    if (PON_NO_ERROR != xgpon_error) 
    {
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                 "Unable to set RX trfc_allocid for NGPON_DEFAULT_TCONT_ID . error %lu",xgpon_error);
         return xgpon_error ;
    }

    g_xgpon_db.tconts_table[NGPON_DEFAULT_TCONT_ID].assign_alloc_valid_flag = BDMF_FALSE ;
    g_xgpon_db.tconts_table[NGPON_DEFAULT_TCONT_ID].alloc_id = NGPON_ALLOC_ID_NOT_ASSIGNED ;
    g_xgpon_db.tconts_table[NGPON_DEFAULT_TCONT_ID].tcont_id = NGPON_TCONT_ID_NOT_ASSIGNED ;
    g_xgpon_db.tconts_table[NGPON_DEFAULT_TCONT_ID].config_tcont_valid_flag = BDMF_FALSE ;

    /* Disable Default XGEM flow and associate it with ONU ID */
    default_flow.portid    = NGPON_XGEM_PORT_ID_UNASSIGNED;
    default_flow.priority  = 0;
    default_flow.enc_ring  = 0;
    default_flow.crc_en    = BDMF_FALSE;
    default_flow.en        = BDMF_FALSE;
    xgpon_error =  ag_drv_ngpon_rx_assign_flow_set(NGPON_DEFAULT_XGEM_FLOW,&default_flow);
    if (PON_NO_ERROR != xgpon_error) 
    {
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                 "Unable to set_xgem_flow_config. error %lu",xgpon_error);
         return xgpon_error ;
    }
    g_xgpon_db.ds_xgem_flow_table[NGPON_DEFAULT_XGEM_FLOW].flow_id = NGPON_XGEM_FLOW_ID_UNASSIGNED ;
    g_xgpon_db.ds_xgem_flow_table[NGPON_DEFAULT_XGEM_FLOW].xgem_port_id = NGPON_XGEM_PORT_ID_UNASSIGNED ;
    g_xgpon_db.ds_xgem_flow_table[NGPON_DEFAULT_XGEM_FLOW].enable_flag = BDMF_FALSE ;
    g_xgpon_db.ds_xgem_flow_table[NGPON_DEFAULT_XGEM_FLOW].flow_configured = BDMF_FALSE ;

    /*
     *  AkivaS: Use of Flow-ID 255 ???
     */
#ifdef SIM_ENV
    /*
       Enable all TCONTs by default
    */

    xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_en1_set(0xFFFFFFFF);
    if ( BDMF_ERR_OK != xgpon_error ) 
    {
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                 "Unable to set trfc_allocid_en1. error %lu",xgpon_error);
         return xgpon_error ;
    }
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                  "====>rx_bwmap_trfc_allocid_en2_set sets mask to 0x%08x", 0xFFFFFFF);

       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Setting ag_drv_ngpon_rx_bwmap_trfc_allocid_en2_set() to 0x%X", 0xFF ) ;

    xgpon_error = ag_drv_ngpon_rx_bwmap_trfc_allocid_en2_set(0xFF);
    if ( BDMF_ERR_OK != xgpon_error ) 
    {
         p_log ( ge_onu_logger.sections.stack.downstream_ploam.general_id,
                 "Unable to set trfc_allocid_en2. error %lu",xgpon_error);
         return xgpon_error ;
    }
#endif

    return PON_NO_ERROR ;
}


/* Discard burst profiles parameters, ONU ID, 
   default Alloc-ID, default XGEM Port-ID (Paramter set 3). */
PON_ERROR_DTE clear_onu_config_3(void)
{
    PON_ERROR_DTE rc = 0;
    //#ifndef AKIVAS_DBG
    rc = clear_burst_profiles();
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to discard busrt profiles, error %d", rc);
        return rc;
    }

    rc = clear_default_alloc_id_and_default_xgem_flow_id();
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to discard default Alloc-ID or default XGEM Port-ID, error %d", rc);
        return rc;
    }

    rc = clear_all_rx_onu_ids();
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to discard Rx ONU ID, error %d", rc);
    }

    rc = clear_all_tx_onu_ids();
    if (rc != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to discard Tx ONU ID, error %d", rc);
    }
    //#endif
    return rc;
}


PON_ERROR_DTE clear_all_rx_onu_ids ( void )
{
    PON_ERROR_DTE xgpon_error ;

    /* Disable PRIMARY RX ONU ID */
    xgpon_error = ag_drv_ngpon_rx_ploam_trfc_set ( NGPON_ONU_INDEX_ONU_0,
                                                   NGPON_DEFAULT_RX_ONU_ID_VALUE ,
                                                   BDMF_FALSE);
    if (xgpon_error !=  PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to disable PRIMARY Rx ONU-ID and its valid bit (rx_ploam_trfc_set) %d !", xgpon_error ) ;
       return ( xgpon_error ) ;
    }
#if 0    /* not sure whether we need it ??? */
    /* Disable SECONDARY RX ONU ID */
    xgpon_error = ngpon_drv_rx_set_onu_id_params ( NGPON_ONU_INDEX_ONU_1,
                                                   NGPON_DEFAULT_RX_ONU_ID_VALUE ,
                                                   BDMF_FALSE);
    if (xgpon_error !=  PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to disable SECONDARY Rx ONU-ID and its valid bit (rx_ploam_trfc_set) %d !", xgpon_error ) ;
       return ( xgpon_error ) ;
    }
#endif
    /* Disable Broadcast Id and valid bit */
    xgpon_error = ag_drv_ngpon_rx_ploam_bcst_set ( NGPON_DEFAULT_RX_ONU_ID_VALUE, BDMF_FALSE ) ;
    if (xgpon_error !=  PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to disable Rx Broadcast ONU-ID and its valid bit (rx_ploam_bcst_set) %d !", xgpon_error ) ;
       return ( xgpon_error ) ;
    }

   if (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS)
   {
       /* Disable SECONDARY RX ONU ID used for Broadcast ONU ID 0x3FE [1022] */
       xgpon_error = ag_drv_ngpon_rx_ploam_trfc_set (NGPON_ONU_INDEX_ONU_1,
           XGS_BROADCAST_ONU_ID, BDMF_FALSE);
       if (xgpon_error !=  PON_NO_ERROR)
       {
          p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to disable SECONDARY Rx ONU-ID and its valid bit (rx_ploam_trfc_set) %d !", xgpon_error) ;
          return (xgpon_error) ;
       }
    }

    return PON_NO_ERROR ;
}


PON_ERROR_DTE clear_all_tx_onu_ids ( void )
{
    PON_ERROR_DTE xgpon_error ;

    /* Clear TX ONU ID (The 3 ONU indexes must have different ONU Ids)  */
    xgpon_error = ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_set ( NGPON_ONU_INDEX_ONU_0,
                                                              NGPON_DEFAULT_TX_ONU_INDEX_0_ONU_ID_VALUE,
                                                              NGPON_PLOAM_TYPE_NORMAL,
                                                              NGPON_PLOAM_TYPE_NORMAL );

    if (xgpon_error != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set_onu_id_to_ploam_mapping due to Driver error %d !", xgpon_error ) ;
       return ( xgpon_error ) ;
    }
    xgpon_error = ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_set ( NGPON_ONU_INDEX_ONU_1,
                                                              NGPON_DEFAULT_TX_ONU_INDEX_1_ONU_ID_VALUE,
                                                              NGPON_PLOAM_TYPE_REGISTRATION,
                                                              NGPON_PLOAM_TYPE_IDLE_OR_SN );
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set_onu_id_to_ploam_mapping due to Driver error %d !", xgpon_error ) ;
       return ( xgpon_error ) ;
    }
    xgpon_error =  ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_set ( NGPON_ONU_INDEX_ONU_BROADCAST,
                                                           NGPON_DEFAULT_TX_ONU_INDEX_2_ONU_ID_VALUE,
                                                           NGPON_PLOAM_TYPE_REGISTRATION,
                                                           NGPON_PLOAM_TYPE_IDLE_OR_SN );
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set_onu_id_to_ploam_mapping due to Driver error %d !", xgpon_error ) ;
       return ( xgpon_error ) ;
    }


    return PON_NO_ERROR ;
}

PON_ERROR_DTE clear_burst_profiles ( void )
{
    PON_ERROR_DTE                 xgpon_error = PON_NO_ERROR    ;
    NGPON_BURST_PROFILE_INDEX     profile_index  ;
    ngpon_tx_profile_profile_ctrl profile_ctrl = {0, 0, BDMF_FALSE, 0, BDMF_FALSE} ;

    /* Disable profile  */
    
            p_log(ge_onu_logger.sections.stack.downstream_ploam.burst_profile_message_id,
            "clear_burst_profiles()\n========>\n");

    /* Clear database */
    for ( profile_index = NGPON_BURST_PROFILE_INDEX_LOW ; profile_index <= NGPON_BURST_PROFILE_INDEX_HIGH ; profile_index++ )
    {
        g_xgpon_db.link_params.burst_profiles.profiles[profile_index].profile_enable = BDMF_FALSE ;

        xgpon_error =  ag_drv_ngpon_tx_profile_profile_ctrl_set (profile_index, &profile_ctrl ) ;
        if (xgpon_error != PON_NO_ERROR)
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to disable profile_index due to Driver error %d !", xgpon_error ) ;
            return ( xgpon_error ) ;
        }

    }

    g_xgpon_db.link_params.burst_profiles.burst_profile_received = BDMF_FALSE;

    return (PON_NO_ERROR) ;
}


PON_ERROR_DTE config_transceiver_dv ( void )
{

    PON_ERROR_DTE       xgpon_error ;
    ngpon_tx_cfg_dv_cfg tx_dv_cfg ;
    TRX_SIG_ACTIVE_POLARITY  tx_sd_polarity ;
    int                      rc ;
    int bus = -1;
 
    opticaldet_get_xpon_i2c_bus_num(&bus);

    rc = trx_get_tx_sd_polarity(bus, &tx_sd_polarity) ;
    if (rc != 0)
    {
      /*
       * Use default if there are troubles with determining optics type
       */
      tx_dv_cfg.dv_pol          = g_xgpon_db.physic_params.transceiver_dv_polarity ;
    }
    else
    {
      tx_dv_cfg.dv_pol          = (tx_sd_polarity == TRX_ACTIVE_LOW)? 1 : 0 ;
      g_xgpon_db.physic_params.transceiver_dv_polarity = tx_dv_cfg.dv_pol ;
    }

    tx_dv_cfg.dv_setup_len      = g_xgpon_db.physic_params.transceiver_dv_setup_length ;
    tx_dv_cfg.dv_hold_len       = g_xgpon_db.physic_params.transceiver_dv_hold_length ;
    tx_dv_cfg.dv_setup_pat_src  = g_xgpon_db.physic_params.transceiver_dv_setup_pattern_src ;
    tx_dv_cfg.dv_hold_pat_src   = g_xgpon_db.physic_params.transceiver_dv_hold_pattern_src ;

    if (g_xgpon_db.optics_type == BP_GPON_OPTICS_TYPE_PMD)
    {

    }

    xgpon_error =  ag_drv_ngpon_tx_cfg_dv_cfg_set ( &tx_dv_cfg ) ;

    if (xgpon_error != PON_NO_ERROR)
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx dv_polarity due to driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    xgpon_error = ag_drv_ngpon_tx_cfg_dv_setup_pat_set ( g_xgpon_db.physic_params.transceiver_dv_setup_pattern  ); 
    if (xgpon_error != PON_NO_ERROR)
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_setup_pattern ,  error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }
    xgpon_error = ag_drv_ngpon_tx_cfg_dv_hold_pat_set ( g_xgpon_db.physic_params.transceiver_dv_hold_pattern  ); 
    if (xgpon_error != PON_NO_ERROR)
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_dv_hold_pattern ,  error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }
    return PON_NO_ERROR ;
}

PON_ERROR_DTE config_transceiver_data ( void )
{
    PON_ERROR_DTE xgpon_error ;
    uint8_t       pattern_word_no ;

    xgpon_error = ag_drv_ngpon_tx_cfg_dat_pat_cfg_set ( g_xgpon_db.physic_params.transceiver_data_patten_type,
                                          g_xgpon_db.physic_params.transceiver_data_setup_length,
                                                        g_xgpon_db.physic_params.transceiver_data_hold_length ) ;
    if (xgpon_error != PON_NO_ERROR)
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_data_patten_type ,  error %d !", xgpon_error ) ;
        return (xgpon_error) ;
    }
    xgpon_error = ag_drv_ngpon_tx_cfg_dat_pat_set (  g_xgpon_db.physic_params.transceiver_data_pattern_reg ) ;
    if (xgpon_error != PON_NO_ERROR)
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_data_pattern ,  error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }
   
    for ( pattern_word_no = 0 ; pattern_word_no < NGPON_MAX_DAT_SETUP_PATTERN_LEN ; pattern_word_no ++ )
    {
       xgpon_error = ag_drv_ngpon_tx_cfg_dat_setup_pat_set ( pattern_word_no ,  g_xgpon_db.physic_params.transceiver_data_setup_pattern [ pattern_word_no ] ) ;
       if (xgpon_error != PON_NO_ERROR)
       {
          p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_data_setup_pattern ,  error %d !", xgpon_error ) ;
          return (xgpon_error) ;
       }
    }

    for ( pattern_word_no = 0 ; pattern_word_no < NGPON_MAX_DAT_HOLD_PATTERN_LEN ; pattern_word_no ++ )
    {
       xgpon_error = ag_drv_ngpon_tx_cfg_dat_hold_pat_set ( pattern_word_no ,  g_xgpon_db.physic_params.transceiver_data_hold_pattern [ pattern_word_no ] ) ;
       if (xgpon_error != PON_NO_ERROR)
       {
          p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure transceiver_data_hold_pattern ,  error %d !", xgpon_error ) ;
          return ( xgpon_error ) ;
       }
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE config_tcont_groups ( void )
{
    PON_ERROR_DTE xgpon_error ;
    PON_TCONT_ID_DTE tcont_id ;

    uint32_t  tx_queue_offset = 0 ;
    uint32_t  tx_queue_size = 0 ;
    uint32_t  tx_pd_size = 0 ;
    uint32_t  tx_queue_size_group_0 = 0 ;
    uint32_t  tx_queue_size_group_1 = 0 ;
    uint32_t  tx_queue_size_group_2 = 0 ;
    uint32_t  tx_queue_size_group_3 = 0 ;
    uint32_t  tx_pd_offset = 0 ;
 


    /* TCONT Front End buffer configuration - based groups */
    /* 1st Group - TCONT 0-7 */
    for ( tcont_id = NGPON_TX_FIFO_QUEUE_INDEX_LOW ; tcont_id <= NGPON_TX_FIFO_QUEUE_INDEX_HIGH ; tcont_id ++ )
    {
        /* TX FIFO size should be in units of words = 4 bytes */
        tx_queue_size = g_xgpon_db.tx_params.tx_tcont_0_7_queue_size[ tcont_id ] /BYTES_PER_WORD ;
        tx_pd_size    = g_xgpon_db.tx_params.tx_tcont_0_7_pd_size[ tcont_id ] ;

        p_log ( ge_onu_logger.sections.stack.debug.general_id,
                "TCONT ID %d tx_pd_offset %d (0x%X) pd_size %d , tx_queue_offset %d (0x%X) , queue_size %d ",
                tcont_id, tx_pd_offset, tx_pd_offset, tx_pd_size,
                tx_queue_offset, tx_queue_offset, tx_queue_size ) ;
        
        xgpon_error = ag_drv_ngpon_tx_cfg_pd_q_cfg_set ( tcont_id, tx_pd_offset, tx_pd_size ) ;

        if (xgpon_error != PON_NO_ERROR)
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx pd q due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }                
        xgpon_error = ag_drv_ngpon_tx_cfg_tx_fifo_q_cfg_set ( tcont_id, tx_queue_offset, tx_queue_size );

        if (xgpon_error != PON_NO_ERROR)
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }

        /* Update the buffer descriptor offset */
        tx_pd_offset += tx_pd_size ;

        /* Update the TX FIFO offset. It points to word of 4 bytes */
        tx_queue_offset += tx_queue_size ;
    }

    /* If the number of traffic TCONTs is 40 then we need to config TCONT groups 8-15, 16-23, 24-31, 32-39  */
    /* If the number of traffic TCONTs is only 8 then there is no need to config those groups */
    if ( NGPON_TRAFFIC_TCONT_ID_HIGH > tcont_id ) 
    {
         /*  TCONT Front End buffer configuration for TCONT 8 and up */
    
        /* Group 0 - TCONT 8-15 */
        /* TX FIFO group size ( units of 4 bytes ) */
        tx_queue_size_group_0 = g_xgpon_db.tx_params.tx_tcont_8_15_queue_size / BYTES_PER_WORD   ;
        tx_pd_offset          = NGPON_TX_TCONT_PD_SIZE * NGPON_TX_QUEUES_PER_GROUP * ( NGPON_TX_GROUP_TCONT_8_TO_TCONT_15 + 1 ) ;
        p_log ( ge_onu_logger.sections.stack.debug.general_id,
                "GROUP_TCONT_8_TO_TCONT_15 (index %d) tx_pd_offset %d tx_queue_offset %d queue_size %d (0x%X)",
                NGPON_TX_GROUP_TCONT_8_TO_TCONT_15, tx_pd_offset, tx_queue_offset, tx_queue_size_group_0, tx_queue_size_group_0 ) ;

        xgpon_error = ag_drv_ngpon_tx_cfg_tx_fifo_grp_cfg_set ( NGPON_TX_GROUP_TCONT_8_TO_TCONT_15, tx_queue_offset, tx_queue_size_group_0 ) ;
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx fifo group due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }
        xgpon_error = ag_drv_ngpon_tx_cfg_pd_grp_cfg_set ( NGPON_TX_GROUP_TCONT_8_TO_TCONT_15, tx_pd_offset ) ;
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx group pd due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }
        tx_queue_offset += tx_queue_size_group_0 * NGPON_TX_QUEUES_PER_GROUP ;
    
        /* Group 1 - TCONT 16-23 */
        /* TX FIFO group size ( units of 4 bytes ) */
        tx_queue_size_group_1 = g_xgpon_db.tx_params.tx_tcont_16_23_queue_size / BYTES_PER_WORD ;
        tx_pd_offset          = NGPON_TX_TCONT_PD_SIZE * NGPON_TX_QUEUES_PER_GROUP * ( NGPON_TX_GROUP_TCONT_16_TO_TCONT_23 + 1 ) ;
        p_log ( ge_onu_logger.sections.stack.debug.general_id,
                "NGPON_TX_GROUP_TCONT_16_TO_TCONT_23 (index %d) tx_pd_offset %d tx_queue_offset %d queue_size %d (0x%X)",
                 NGPON_TX_GROUP_TCONT_16_TO_TCONT_23,tx_pd_offset ,tx_queue_offset  ,tx_queue_size_group_1,tx_queue_size_group_1 ) ;
        xgpon_error = ag_drv_ngpon_tx_cfg_tx_fifo_grp_cfg_set ( NGPON_TX_GROUP_TCONT_16_TO_TCONT_23, tx_queue_offset,
                                                           tx_queue_size_group_1 ) ;
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx fifo group due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }
        xgpon_error = ag_drv_ngpon_tx_cfg_pd_grp_cfg_set ( NGPON_TX_GROUP_TCONT_16_TO_TCONT_23, tx_pd_offset ) ;
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx group pd due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }
        tx_queue_offset += tx_queue_size_group_1 * NGPON_TX_QUEUES_PER_GROUP ;
    
        /* Group 2 - TCONT 24-31 */
        /* TX FIFO group size ( units of 4 bytes ) */
        tx_queue_size_group_2 = g_xgpon_db.tx_params.tx_tcont_24_31_queue_size / BYTES_PER_WORD;
        tx_pd_offset = NGPON_TX_TCONT_PD_SIZE * NGPON_TX_QUEUES_PER_GROUP * ( NGPON_TX_GROUP_TCONT_24_TO_TCONT_31 + 1 ) ;
        p_log ( ge_onu_logger.sections.stack.debug.general_id,
                "NGPONXG_TX_GROUP_TCONT_24_TO_TCONT_31 (index %d) tx_pd_offset %d tx_queue_offset %d queue_size %d (0x%X)",
                 NGPON_TX_GROUP_TCONT_24_TO_TCONT_31, tx_pd_offset, tx_queue_offset, tx_queue_size_group_2, tx_queue_size_group_2 ) ;
        xgpon_error = ag_drv_ngpon_tx_cfg_tx_fifo_grp_cfg_set ( NGPON_TX_GROUP_TCONT_24_TO_TCONT_31, tx_queue_offset,
                                                           tx_queue_size_group_2 ) ;
    
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx fifo group due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }
        xgpon_error = ag_drv_ngpon_tx_cfg_pd_grp_cfg_set ( NGPON_TX_GROUP_TCONT_24_TO_TCONT_31, tx_pd_offset ) ;
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx group pd due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }
        tx_queue_offset += tx_queue_size_group_2 * NGPON_TX_QUEUES_PER_GROUP ;
    
        /* Group 3 - TCONT 32-39 */
        /* TX FIFO group size ( units of 4 bytes ) */
        tx_queue_size_group_3 = g_xgpon_db.tx_params.tx_tcont_32_39_queue_size /BYTES_PER_WORD ;
        tx_pd_offset = NGPON_TX_TCONT_PD_SIZE * NGPON_TX_QUEUES_PER_GROUP * ( NGPON_TX_GROUP_TCONT_32_TO_TCONT_39 + 1 ) ;
        p_log ( ge_onu_logger.sections.stack.debug.general_id,
                "NGPON_TX_GROUP_TCONT_32_TO_TCONT_39 (index %d) tx_pd_offset %d tx_queue_offset %d queue_size %d (0x%X)",
                 NGPON_TX_GROUP_TCONT_32_TO_TCONT_39, tx_pd_offset, tx_queue_offset, tx_queue_size_group_3, tx_queue_size_group_3 ) ;
        xgpon_error = ag_drv_ngpon_tx_cfg_tx_fifo_grp_cfg_set ( NGPON_TX_GROUP_TCONT_32_TO_TCONT_39, tx_queue_offset,
                                                           tx_queue_size_group_3 ) ;
    
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx fifo group due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }
        xgpon_error = ag_drv_ngpon_tx_cfg_pd_grp_cfg_set ( NGPON_TX_GROUP_TCONT_32_TO_TCONT_39, tx_pd_offset ) ;
        if ( xgpon_error != PON_NO_ERROR )
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx group pd due to driver error %d !", xgpon_error ) ;
            return( xgpon_error ) ;
        }

    }

    return PON_NO_ERROR ;
}

PON_ERROR_DTE ngpon_set_txfifo_queue_configuration(NGPON_TX_FIFO_CFG  *tx_fifo_cfg)
{
    int drv_error;

    /* Verify that the stack is active */
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }



     p_log(ge_onu_logger.sections.stack.debug.general_id,
         "txfifo_id %d txfifo_id %d tx_bd_offset %d bd_number %d tx_queue_offset %d queue_size %d ",
         tx_fifo_cfg->packet_descriptor_id, tx_fifo_cfg->txfifo_id, 
         tx_fifo_cfg->packet_descriptor_queue_base_address,
         tx_fifo_cfg->packet_descriptor_queue_size, 
         tx_fifo_cfg->tx_queue_offset, tx_fifo_cfg->tx_max_queue_size);

     drv_error = ag_drv_ngpon_tx_cfg_pd_q_cfg_set (tx_fifo_cfg->packet_descriptor_id, 
         tx_fifo_cfg->packet_descriptor_queue_base_address, tx_fifo_cfg->packet_descriptor_queue_size);
     if (drv_error != PON_NO_ERROR)
     {
         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
             "Unable to set packet descriptor configuration due to driver error %d", drv_error);
         return PON_ERROR_DRIVER_ERROR;
     }

     drv_error = ag_drv_ngpon_tx_cfg_tx_fifo_q_cfg_set (tx_fifo_cfg->txfifo_id, tx_fifo_cfg->tx_queue_offset, 
         tx_fifo_cfg->tx_max_queue_size);
     if (drv_error != PON_NO_ERROR)
    {
         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
             "Unable to set fifo configuration due to driver error %d", drv_error);
         return PON_ERROR_DRIVER_ERROR;
    }

     return PON_NO_ERROR;
}



PON_ERROR_DTE ngpon_get_txfifo_queue_configuration(uint8_t xi_packet_descriptor_id,
    PON_FIFO_ID_DTE xi_txfifo_id, NGPON_TX_FIFO_CFG  *tx_fifo_cfg)
{
    int drv_error;
    uint16_t tx_queue_size;

    /* Verify that the stack is active */
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }



     drv_error = ag_drv_ngpon_tx_cfg_pd_q_cfg_get (xi_packet_descriptor_id, 
        &tx_fifo_cfg->packet_descriptor_queue_base_address, &tx_fifo_cfg->packet_descriptor_queue_size);
     if (drv_error != PON_NO_ERROR)
     {
         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
             "Unable to get packet descriptor configuration due to driver error %d", drv_error);
         return PON_ERROR_DRIVER_ERROR;
     }

     drv_error = ag_drv_ngpon_tx_cfg_tx_fifo_q_cfg_get (xi_txfifo_id, 
        &tx_fifo_cfg->tx_queue_offset, &tx_queue_size);
     if (drv_error != PON_NO_ERROR)
    {
         p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
             "Unable to get fifo configuration due to driver error %d", drv_error);
         return PON_ERROR_DRIVER_ERROR;
    }


    /* TX FIFO queue size(units of 4 bytes) */
    tx_fifo_cfg->tx_max_queue_size = tx_queue_size * 4;
    tx_fifo_cfg->packet_descriptor_id = xi_packet_descriptor_id;
    tx_fifo_cfg->txfifo_id = xi_txfifo_id;

    return PON_NO_ERROR;
}




PON_ERROR_DTE ngpon_link_reset ( void )
{
#ifdef SIM_ENV
    int i;
#endif
    PON_ERROR_DTE                    xgpon_error ;
    ngpon_rx_gen_rcvrcfg             rx_gen_rcvrcfg ;
#ifdef CONFIG_BCM_GPON_TODD
    bdmf_boolean tod_arm, tod_en;
    uint8_t rdarm;
#endif
    uint32_t   dbru_init_val = 0;
    uint32_t   sync_conf = 0x202;

    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_reset() Entered") ;

    /* Check that the stack is active */
    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return ( NGPON_ERROR_SOFTWARE_NOT_INITIALIZED ) ;
    }

    /* Link already activated? */
    if ( g_xgpon_db.link_params.operation_sm.link_state == LINK_STATE_ACTIVE )
    {
        return ( NGPON_ERROR_LINK_IS_ALREADY_ACTIVE ) ;
    }

    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_get ( &rx_gen_rcvrcfg ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to get rx configuration due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }
    /* Disable Rx */
    rx_gen_rcvrcfg.rx_enable = BDMF_FALSE ;

    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_set ( &rx_gen_rcvrcfg ) ;

    if ( xgpon_error !=  PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to get rx configuration due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }
    rx_gen_rcvrcfg.desc_disable = g_xgpon_db.rx_params.rx_descrambler_disable ;
    rx_gen_rcvrcfg.loopback_enable = g_xgpon_db.rx_params.rx_loopback_enable ;
    rx_gen_rcvrcfg.tmf_enable = g_xgpon_db.rx_params.rx_tm_enable ;
    rx_gen_rcvrcfg.rx_data_invert = g_xgpon_db.rx_params.rx_din_polarity ;
    rx_gen_rcvrcfg.mac_mode = g_xgpon_db.rx_params.rx_mac_mode ;

    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_set ( &rx_gen_rcvrcfg ) ;
    if ( xgpon_error !=  PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to configure rx due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }

    xgpon_error = ag_drv_ngpon_rx_gen_sync_cfg_set( g_xgpon_db.physic_params.num_of_psyncs_for_lof_assertion, 
                                                    g_xgpon_db.physic_params.num_of_psyncs_for_lof_clear, 
                                                    g_xgpon_db.physic_params.ponid_acq_thr, 
                                                    g_xgpon_db.physic_params.ext_sync_holdover_enable ) ;

    if ( xgpon_error !=  PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure synchronizer_configuration due to driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }  
                    
    /* Disable XGEM hunt */
#if defined(CONFIG_BCM96858)
    NGPON_WRITE_32(0x80160f00,sync_conf);    
#elif defined(CONFIG_BCM96856)
    NGPON_WRITE_32(0x82db4f00,sync_conf);
#endif
                                                                      
    xgpon_error =  ag_drv_ngpon_rx_tod_cfg_set ( g_xgpon_db.rx_params.rx_8khz_phase_difference,
                                                 g_xgpon_db.rx_params.rx_8khz_increm_phase_change,
                                                 g_xgpon_db.rx_params.rx_8khz_clk_output_active_sync_required  ) ;

    if ( xgpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure 8KHZ clock due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }

    xgpon_error = ag_drv_ngpon_rx_tod_cong_thr_set ( g_xgpon_db.rx_params.rx_congestion_into_threshold,
                                                     g_xgpon_db.rx_params.rx_congestion_out_of_threshold ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure RX congestion thresholds due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }

    xgpon_error = ag_drv_ngpon_rx_gen_sync_psync_value_ms_set ( g_xgpon_db.physic_params.exp_psync_ms ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure Expected Psync (MS) due to driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    xgpon_error = ag_drv_ngpon_rx_gen_sync_psync_value_ls_set ( g_xgpon_db.physic_params.exp_psync_ls ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure Expected Psync (LS) due to driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }




    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_reset() -- Configure tx data polarity") ;

    /* Configure tx data polarity */
    xgpon_error = ag_drv_ngpon_tx_cfg_dat_pol_set (g_xgpon_db.tx_params.tx_data_out_polarity ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx data polarity due to driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_reset() -- Configure Tx DV") ;

    xgpon_error =  config_transceiver_dv();
    if ( xgpon_error != PON_NO_ERROR )
        return ( xgpon_error ) ;

    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_reset() -- Configure transciever data") ;

    xgpon_error =  config_transceiver_data();
    if ( xgpon_error != PON_NO_ERROR )
        return ( xgpon_error ) ;


    /* TCONT Front End buffer configuration - based groups */
    xgpon_error = config_tcont_groups();
    if ( xgpon_error != PON_NO_ERROR )
             return( xgpon_error ) ;

    xgpon_error = ag_drv_ngpon_tx_cfg_xgem_pyld_min_len_set ( g_xgpon_db.tx_params.tx_min_data_xgem_payload_len );
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx xgem_payload_min_len due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }
    xgpon_error = ag_drv_ngpon_tx_cfg_idle_xgem_pyld_len_set ( g_xgpon_db.tx_params.tx_min_idle_xgem_payload_len ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx idle_xgem_payload_len due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }  

    /* This function sets the 18 bits Options field of the XGEM header */
    xgpon_error = ag_drv_ngpon_tx_cfg_xgem_hdr_opt_set ( g_xgpon_db.tx_params.tx_xgem_header_options ); 
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx xgem_header_options due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }

    xgpon_error =  ag_drv_ngpon_tx_cfg_start_offset_set ( g_xgpon_db.tx_params.tx_timeline_offset ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx timeline_offset due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }
    /* DBR source is set to be HW and counters are flushed */
    xgpon_error = ag_drv_ngpon_tx_cfg_dbr_cfg_set ( NGPON_DBR_SRC_HW, BDMF_TRUE);
    if (xgpon_error != PON_NO_ERROR)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx DBR source as SW  due to driver error %d !", xgpon_error) ;
        return xgpon_error;
    }

    /***************************/
    /* Disable units !!! driver */
    /***************************/
    /* Disable Tx unit */
    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_reset() -- Disable Tx") ;

    xgpon_error = ngpon_enable_transmitter ( BDMF_FALSE ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Disable Tx due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    g_xgpon_db.tx_params.lbe_invert_bit = gpon_ngpon_get_lbe_invert_bit_val () ;

    /*
     * Disable output in gearbox
     */
    ngpon_wan_top_enable_transmitter(0) ;
    gpon_ngpon_wan_top_set_lbe_invert(g_xgpon_db.tx_params.lbe_invert_bit) ;
    config_transceiver_dv () ;

    /* RX Disable */
    rx_gen_rcvrcfg.rx_enable = BDMF_FALSE ;
    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_set ( &rx_gen_rcvrcfg ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to disable Rx unit due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    xgpon_error = disable_interrupts(true, true);
    if ( xgpon_error != PON_NO_ERROR )
        return ( xgpon_error ) ;

    xgpon_error = clear_interrupts () ;
    if ( xgpon_error != PON_NO_ERROR )
        return ( xgpon_error ) ;

    xgpon_error = enable_interrupts (true, true) ;
    if ( xgpon_error != PON_NO_ERROR )
        return ( xgpon_error ) ;

    /*
     *   Do we need to clear interrupts (???)
     */
    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_reset() -- Clear interrupts and counters") ;
   

    ngpon_clear_sw_xgem_flow_table() ;
    ngpon_clear_sw_tcont_table();

    xgpon_error = clear_onu_config_5 ();
    if ( xgpon_error != PON_NO_ERROR )
        return ( xgpon_error ) ;


    xgpon_error =  ag_drv_ngpon_tx_cfg_tcont_to_cnt_grp_map_set ( g_xgpon_db.tx_params.tcont_tx_counter_0,
                                                                  g_xgpon_db.tx_params.tcont_tx_counter_1, 
                                                                  g_xgpon_db.tx_params.tcont_tx_counter_2,
                                                                  g_xgpon_db.tx_params.tcont_tx_counter_3 );
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx tcont_to_counter_group due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }
    xgpon_error = ag_drv_ngpon_rx_bwmap_bwcnt_assoc_set ( g_xgpon_db.rx_params.tcont_rx_counter_0,    
                                                           g_xgpon_db.rx_params.tcont_rx_counter_1,    
                                                           g_xgpon_db.rx_params.tcont_rx_counter_2,    
                                                           g_xgpon_db.rx_params.tcont_rx_counter_3 );  
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure rx tcont_to_counter_group due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }
   
    xgpon_error = ag_drv_ngpon_tx_cfg_xgem_hdr_opt_set ( NGPON_DFLT_XGEM_HDR_OPTIONS ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure XGEM header options error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    /* Clear TX PM counters */
    xgpon_error = ngpon_get_all_tx_counters (& g_tx_counters);
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear Tx unit PM counters due to Api error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }
    /* Clear RX PM counters */
    xgpon_error = ngpon_get_all_rx_counters (& g_rx_counters);
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear Rx unit PM counters due to Api error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    /* Activation state machine control */
    g_xgpon_db.link_params.operation_sm.link_state = LINK_STATE_INACTIVE ;
    g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O1 ;



    xgpon_error = ag_drv_ngpon_rx_ploam_trfc_set (NGPON_ONU_INDEX_ONU_0, NGPON_SIM_ONU_1_ID, BDMF_TRUE);

    if ( xgpon_error != PON_NO_ERROR )
    {
       p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set hardcoded ONU ID #1 and to enable it  Driver error %d !", xgpon_error );
    }
    xgpon_error = ag_drv_ngpon_rx_ploam_trfc_set (NGPON_ONU_INDEX_ONU_1, NGPON_SIM_ONU_2_ID, BDMF_TRUE);

    if ( xgpon_error != PON_NO_ERROR )
    {
       p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set hardcoded ONU ID #2 and to enable it  Driver error %d !", xgpon_error );
    }
    xgpon_error = ag_drv_ngpon_rx_ploam_trfc_set (NGPON_ONU_INDEX_ONU_0, NGPON_SIM_ONU_3_ID, BDMF_TRUE);
    if ( xgpon_error != PON_NO_ERROR )
    {
       p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set hardcoded ONU ID #3 and to enable it  Driver error %d !", xgpon_error );
    }

    xgpon_error = ag_drv_ngpon_rx_ploam_gencfg_set (1);   /* No PLOAM filtering */
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set PLOAM filtering  Driver error %d !", xgpon_error );
    }


    xgpon_error = ag_drv_ngpon_rx_assign_last_flow_set (NGPON_LAST_FLOW_MASK);
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set Proprietary Access  Driver error %d !", xgpon_error);
    }

    xgpon_error = ag_drv_ngpon_rx_recording_cfg_set ( BDMF_FALSE, BDMF_FALSE, NGPON_ALLOC_ID_4_REC ) ;
    if (xgpon_error != PON_NO_ERROR)
    {
           p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to set BW recording  Driver error %d !", xgpon_error);
    }

    xgpon_error = ag_drv_ngpon_rx_recording_en_set (BDMF_FALSE);
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to enable BW recording  Driver error %d !", xgpon_error);
    }


    xgpon_error = ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_set ( BDMF_FALSE, BDMF_FALSE, BDMF_TRUE ) ;
    if (xgpon_error != PON_NO_ERROR)
    {
      p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure rogue ONU control  Driver error %d !", xgpon_error);
    }
    xgpon_error = ag_drv_ngpon_tx_cfg_rogue_level_time_set (NGPON_ROGUE_LEVEL_TIME_WINDOW);
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure rogue level time (window size)  Driver error %d !", xgpon_error ) ;
    }

    xgpon_error = ag_drv_ngpon_tx_cfg_rogue_diff_time_set (NGPON_ROGUE_DIFF_TIME_WINDOW);
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure rogue level time (window size)  Driver error %d !", xgpon_error ) ;
    }


    xgpon_error =  ag_drv_ngpon_tx_cfg_fragmentation_disable_set (BDMF_FALSE); 
    if (xgpon_error != PON_NO_ERROR)
    {
       p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure fragmentation state to Disable  Driver error %d !", xgpon_error);
    }

#ifdef CONFIG_BCM_GPON_TODD
    /* Enable ToD update */

    xgpon_error = ag_drv_ngpon_rx_tod_update_arm_get(&tod_arm, &tod_en, &rdarm);
    if (xgpon_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read AMD2 tod due to driver error %d", xgpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    xgpon_error = ag_drv_ngpon_rx_tod_update_arm_set(tod_arm, BDMF_TRUE, rdarm);
    if (xgpon_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to enable AMD2 tod due to driver error %d", xgpon_error);
        return PON_ERROR_DRIVER_ERROR;
    }
#endif

#if defined(CONFIG_BCM96858)
    NGPON_WRITE_32(0x80168398,dbru_init_val);   /* On TCONT flush operation DBRu counter will be set to 0 */
#elif defined(CONFIG_BCM96856)
    NGPON_WRITE_32(0x82db8398,dbru_init_val);   /* On TCONT flush operation DBRu counter will be set to 0 */
#endif
 
    p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_reset() -- Exit");

    return PON_NO_ERROR;
}


void rx_int_handler(void);


PON_ERROR_DTE ngpon_link_activate ( bdmf_boolean xi_initial_state_disable )
{
    PON_ERROR_DTE            xgpon_error ;
    ngpon_rx_gen_rcvrcfg     rx_gen_rcvrcfg ;
    ngpon_tx_cfg_func_en     tx_cfg_func_en;

    OPERATION_SM_PARAMS      operation_sm_params ;

    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_activate() -- Enter") ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return ( NGPON_ERROR_SOFTWARE_NOT_INITIALIZED ) ;
    }
    if ( g_xgpon_db.link_params.operation_sm.link_state != LINK_STATE_INACTIVE )
    {
        return ( NGPON_ERROR_LINK_IS_NOT_INACTIVE ) ;
    }


    /* Configure RX & TX */
    /* Clear ONU HW configuration */
    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_activate() -- Configure Rx and Tx" ) ;
    xgpon_error = clear_onu_config_5 ();
     if ( xgpon_error != PON_NO_ERROR )
         return ( xgpon_error ) ;


    /* Set DV Setup and hold */
    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_activate() -- Set DV setup and hold" ) ;

    xgpon_error =  config_transceiver_dv();
    if ( xgpon_error != PON_NO_ERROR )
        return ( xgpon_error ) ;


    /* Enable Rx unit */
    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_activate() -- Enable Rx" ) ;

    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_get ( &rx_gen_rcvrcfg ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to get rx (1) configuration due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }
    
    rx_gen_rcvrcfg.desc_disable = g_xgpon_db.rx_params.rx_descrambler_disable ;
    rx_gen_rcvrcfg.loopback_enable = g_xgpon_db.rx_params.rx_loopback_enable ;
    rx_gen_rcvrcfg.tmf_enable = g_xgpon_db.rx_params.rx_tm_enable ;
    rx_gen_rcvrcfg.rx_data_invert = g_xgpon_db.rx_params.rx_din_polarity ;
    rx_gen_rcvrcfg.mac_mode = g_xgpon_db.rx_params.rx_mac_mode ;

    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_set ( &rx_gen_rcvrcfg ) ;

    if ( xgpon_error !=  PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to configure rx (2) due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }

    xgpon_error  = ag_drv_ngpon_rx_gen_sync_frame_length_set ( g_xgpon_db.rx_params.rx_sync_frame_len ) ;
    if ( xgpon_error !=  PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to configure rx (3) due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }



    /* Initialization */
    {
        /* Activation state machine control */
        {
            g_xgpon_db.link_params.operation_sm.link_state = LINK_STATE_ACTIVE ;
            if ( xi_initial_state_disable == BDMF_TRUE )
            {
                g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O7 ;

                /* Set Broadcast ONU Id */
                xgpon_error = ag_drv_ngpon_rx_ploam_bcst_set ( BROADCAST_ONU_ID, BDMF_TRUE ) ;
                if ( xgpon_error != PON_NO_ERROR )
                {
                   p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to enable Rx Broadcast ONU-ID and its valid bit due to Driver error %d !", xgpon_error ) ;
                   return ( xgpon_error ) ;
                }
                if (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS)
                {
                    /* Enable SECONDARY RX ONU ID used for Broadcast ONU ID 0x3FE [1022] */
                    xgpon_error = ag_drv_ngpon_rx_ploam_trfc_set (NGPON_ONU_INDEX_ONU_1,
                       XGS_BROADCAST_ONU_ID,
                       BDMF_TRUE);
                   if (xgpon_error !=  PON_NO_ERROR)
                   {
                     p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to enable SECONDARY Rx ONU-ID and its valid bit (rx_ploam_trfc_set) %d !", xgpon_error) ;
                     return (xgpon_error) ;
                   }
                }
            }
            else
            {
                /* According to RX link sub state will be */
                
                g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O1 ;

                /* Set LODS to ON */
                operation_sm_params.lods_state = BDMF_TRUE ;
                p_operation_sm ( OPERATION_EVENT_TIMER_DS_SYNC_CHECK_EXPIRE, & operation_sm_params ) ;
            }

        }
    }

    /* TX is still Disabled - it'll become enable once we'll get Burst profile*/


    xgpon_error = ag_drv_ngpon_tx_cfg_frame_length_minus_1_set ( g_xgpon_db.tx_params.tx_frame_length ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure Tx frame length Minus 1 due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }
    memset (&tx_cfg_func_en, 0, sizeof(tx_cfg_func_en));
    tx_cfg_func_en.scrmblr_en    = g_xgpon_db.tx_params.tx_scrambler_enable ;
    tx_cfg_func_en.bip_en        = g_xgpon_db.tx_params.tx_enc_enable ;
    tx_cfg_func_en.fec_en        = BDMF_TRUE ;
    tx_cfg_func_en.enc_en        = g_xgpon_db.tx_params.tx_enc_enable ;

    xgpon_error = ag_drv_ngpon_tx_cfg_func_en_set ( & tx_cfg_func_en ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to configure tx_cfg_func_en  due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    {
       uint32_t reg;

#if defined(CONFIG_BCM96858)
#define TEMP_ADDRESS_FOR_NGPON_32 ( 0x80168004 )
#elif defined(CONFIG_BCM96856)
#define TEMP_ADDRESS_FOR_NGPON_32 ( 0x82db8004 )
#endif

       NGPON_READ_32(TEMP_ADDRESS_FOR_NGPON_32, reg);
       reg |= 0x100;
       NGPON_WRITE_32(TEMP_ADDRESS_FOR_NGPON_32, reg);

#undef TEMP_ADDRESS_FOR_NGPON_32
    }

    xgpon_error = ngpon_enable_transmitter ( BDMF_FALSE ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Disable Tx due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }
    g_xgpon_db.tx_params.lbe_invert_bit = gpon_ngpon_get_lbe_invert_bit_val () ; /* just to be on the safe side */
    /*
     * Disable output in gearbox
     */
    ngpon_wan_top_enable_transmitter(0) ;
    gpon_ngpon_wan_top_set_lbe_invert(g_xgpon_db.tx_params.lbe_invert_bit) ;
    config_transceiver_dv () ;

    /* RX becomes Enable */
    rx_gen_rcvrcfg.rx_enable = BDMF_TRUE ;
    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_set ( &rx_gen_rcvrcfg ) ;

    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_activate() -- Exit\n" ) ;

#if 0
    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "ngpon_link_activate() -- Push Activate SM to O2\n" ) ;

#if 1
    bdmf_usleep(250);
    rx_int_handler();
#endif
#endif
    return ( PON_NO_ERROR ) ;
}

PON_ERROR_DTE  ngpon_link_deactivate ( void )
{
    PON_ERROR_DTE            xgpon_error ;
    ngpon_rx_gen_rcvrcfg     rx_gen_rcvrcfg ;


    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
        return ( NGPON_ERROR_SOFTWARE_NOT_INITIALIZED ) ;
    if ( g_xgpon_db.link_params.operation_sm.link_state != LINK_STATE_ACTIVE )
        return ( NGPON_ERROR_LINK_IS_NOT_ACTIVE ) ;

    /* Stop TO1 timer */
    bdmf_timer_stop ( & g_xgpon_db.os_resources.to1_timer_id ) ;

    /* Stop TO2 timer */
    bdmf_timer_stop ( &  g_xgpon_db.os_resources.to2_timer_id ) ;

    /* Stop TK4 timer */
    bdmf_timer_stop ( &  g_xgpon_db.os_resources.tk4_timer_id ) ;

    /* Stop TK5 timer */
    bdmf_timer_stop ( & g_xgpon_db.os_resources.tk5_timer_id ) ;

    bdmf_timer_stop ( &g_xgpon_db.os_resources.ds_sync_check_timer_id ) ;

    bdmf_timer_stop (&g_xgpon_db.os_resources.toz_timer_id) ;

    bdmf_timer_stop (&g_xgpon_db.os_resources.to3_timer_id) ;

    bdmf_timer_stop (&g_xgpon_db.os_resources.to4_timer_id) ;

    bdmf_timer_stop (&g_xgpon_db.os_resources.to5_timer_id) ;

#ifdef G989_3_AMD1
    g_xgpon_db.os_resources.tcpi_timer_is_running = BDMF_FALSE;
    bdmf_timer_stop (&g_xgpon_db.os_resources.tcpi_timer_id);
#endif

    /* Disble interrupts */
    xgpon_error = clear_interrupts () ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to disable rx and tx interrupts due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;

     }

    /* Clear ONU config : Alloc IDs , XGEM flows , TMUS look up table  */
    xgpon_error =  clear_onu_config_5 ();
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to clear_onu_config due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    /* Disable TX Module */
    xgpon_error = ngpon_enable_transmitter ( BDMF_FALSE ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to Disable Tx due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }
    /*
     * Disable output in gearbox
     */
    ngpon_wan_top_enable_transmitter(0);

    /* Disable RX Module */
    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_get ( &rx_gen_rcvrcfg ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        /* Log */
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to get rx configuration due to driver error %d !", xgpon_error ) ;
        return( xgpon_error ) ;
    }    
    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_get ( &rx_gen_rcvrcfg ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to get Rx config due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }

    rx_gen_rcvrcfg.rx_enable = BDMF_FALSE ;
    xgpon_error = ag_drv_ngpon_rx_gen_rcvrcfg_set ( &rx_gen_rcvrcfg ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to disable Rx unit due to Driver error %d !", xgpon_error ) ;
        return ( xgpon_error ) ;
    }


    /* Report event "Link state transition" with state parameter */
    xgpon_error = send_message_to_pon_task ( PON_EVENT_LINK_STATE_TRANSITION ,
                                               g_xgpon_db.link_params.operation_sm.activation_state,
                                               OPERATION_STATE_O1 ) ;
    if ( xgpon_error != PON_NO_ERROR )
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to send user indication: error %d !", xgpon_error ) ;
    }


    /* Activation state machine control */
    g_xgpon_db.link_params.operation_sm.link_state = LINK_STATE_INACTIVE ;
    g_xgpon_db.link_params.operation_sm.activation_state = OPERATION_STATE_O1 ;


    return ( PON_NO_ERROR ) ;
}

static PON_ERROR_DTE ngpon_rx_set_bw_recording_cfg(bdmf_boolean rcd_stop_mode,
    NGPON_BW_RECORD_MODE rcd_mode, uint16_t specific_alloc, 
    bdmf_boolean rcd_enable) 
{
    int drv_error;

    if (!NGPON_BW_RECORD_MODE_IN_RANGE(rcd_mode))
        return BDMF_ERR_RANGE;

    if (rcd_enable)
    {
        drv_error = ag_drv_ngpon_rx_recording_cfg_set(rcd_stop_mode, rcd_mode, specific_alloc) ;
        if (drv_error != PON_NO_ERROR)
        {
            return drv_error;
        }
    }

    drv_error = ag_drv_ngpon_rx_recording_en_set(rcd_enable);
    if (drv_error != PON_NO_ERROR)
    {
        return drv_error;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_start_bw_recording(bdmf_boolean rcd_stop,
    uint32_t record_type, uint32_t alloc_id)
{
    int stack_error;

    /* Check GPON stack state */
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return NGPON_ERROR_SOFTWARE_NOT_INITIALIZED;
    }

    stack_error = ngpon_rx_set_bw_recording_cfg(rcd_stop, 
         record_type, alloc_id, BDMF_TRUE);

    return stack_error;
}

PON_ERROR_DTE ngpon_stop_bw_recording(void)
{
    PON_ERROR_DTE stack_error;

    /* Check GPON stack state */
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return NGPON_ERROR_SOFTWARE_NOT_INITIALIZED;
    }

    stack_error = ngpon_rx_set_bw_recording_cfg(0, 0, 0, BDMF_FALSE);

    return stack_error;
}

PON_ERROR_DTE ngpon_get_bw_recording_data(NGPON_ACCESS *accssess_array,
    int *const number_of_access_read)
{
    PON_ERROR_DTE drv_error;
    bdmf_boolean rcd_done;
    bdmf_boolean rcd_mismatch;
    bdmf_boolean ecd_empty;
    uint16_t rcd_last_ptr;
    uint32_t read_index;
    uint16_t starttime; 
    uint16_t allocid;
    ngpon_rx_bwrcd_mem_mem_lsb mem_lsb;

    /* Check GPON stack state */
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return NGPON_ERROR_SOFTWARE_NOT_INITIALIZED;
    }

    drv_error = ag_drv_ngpon_rx_recording_status_get(&rcd_done, 
        &rcd_mismatch, &ecd_empty, &rcd_last_ptr);
    if (drv_error != PON_NO_ERROR)
    {
        return drv_error;
    }
    if (rcd_mismatch)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "There was not enough space in the memory for entire map\n");
        return BDMF_ERR_NOMEM;
    }

    if (ecd_empty && (g_xgpon_db.onu_params.stack_mode != NGPON_MODE_XGS))  /* AkivaS - workaround for XGS ranging debug */
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
              "g_xgpon_db.onu_params.stack_mode = %d\n", g_xgpon_db.onu_params.stack_mode);
        return PON_NO_ERROR;
    }


    if (rcd_done)
    {
        for (read_index = 0; read_index <= rcd_last_ptr; read_index++)
        {
            drv_error = ag_drv_ngpon_rx_bwrcd_mem_mem_msb_get(read_index, 
                &starttime, &allocid);
            if (drv_error != PON_NO_ERROR) 
            { 
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Failed to read rcd data read index=%d MSB register due to driver error %d\n", 
                   read_index, drv_error);
                return drv_error;
            }

            drv_error = ag_drv_ngpon_rx_bwrcd_mem_mem_lsb_get(read_index, &mem_lsb);
            if (drv_error != PON_NO_ERROR) 
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Failed to read rcd data read index=%d LSB register due to driver error %d\n", 
                   read_index, drv_error);
                return drv_error;
            }
            
            accssess_array[read_index].starttime = starttime; 
            accssess_array[read_index].allocid   = allocid;
            accssess_array[read_index].sfc_ls    = mem_lsb.sfc_ls;
            accssess_array[read_index].hec_ok    = mem_lsb.hec_ok;
            accssess_array[read_index].bprofile  = mem_lsb.bprofile; 
            accssess_array[read_index].fwi       = mem_lsb.fwi;
            accssess_array[read_index].ploamu    = mem_lsb.ploamu;
            accssess_array[read_index].dbru      = mem_lsb.dbru;
            accssess_array[read_index].grantsize = mem_lsb.grantsize;
        }

        *number_of_access_read = read_index;
    }
    else
        *number_of_access_read = 0;

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   ngpon_read_bw_record                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   NGPON Rx - Read BW recording data                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Returns the recorded data and number of accesses recorded.               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   accesses_array - Array storing the recorded data                         */
/*                                                                            */
/*   access_read_num - Number of accesses in recorded data (1-64)             */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*     PON_NO_ERROR - No errors or faults                                     */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_read_bw_record(PON_ACCESS *record_data,
    int *const access_read_num)
{
    record_data->access_type = 1;
    return ngpon_get_bw_recording_data(record_data->ngpon_access, access_read_num);
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   ngpon_get_rogue_onu_detection_params                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   XGPON/NGPON2 Tx - Get Rogue ONU Detection Parameters                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_get_rogue_onu_detection_params(PON_TX_ROGUE_ONU_PARAMETERS *rogue_onu_params)
{
    PON_ERROR_DTE drv_error;

    drv_error =  ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_get(&rogue_onu_params->rogue_onu_level_clear, 
       &rogue_onu_params->rogue_onu_diff_clear, &rogue_onu_params->source_select) ;
    if (drv_error != PON_NO_ERROR) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to read rouge ONU parameters due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    drv_error = ag_drv_ngpon_tx_cfg_rogue_level_time_get(
        &rogue_onu_params->rogue_onu_level_threshold);        
    if (drv_error != PON_NO_ERROR) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to read rouge ONU level time due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    drv_error = ag_drv_ngpon_tx_cfg_rogue_diff_time_get(
        &rogue_onu_params->rogue_onu_diff_threshold);        
    if (drv_error != PON_NO_ERROR) 
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
/*   ngpon_set_rogue_onu_detection_params                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   XGPON/NGPON2 Tx - Set Rogue ONU Detection Parameters                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function Set the Rouge ONU detection parameters such as source      */
/*   detection thresholds, and clear counters                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   rogue_onu_parameters - Rogue ONU paramteres struct                       */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*   PON_NO_ERROR - No errors or faults                                       */
/*   PON_GENERAL_ERROR - General error                                        */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_set_rogue_onu_detection_params(const PON_TX_ROGUE_ONU_PARAMETERS *rogue_onu_parameters)
{
    PON_ERROR_DTE drv_error;

    /* Check the stack state */
    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    /*Set the rogue onu parameters*/
    drv_error = ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_set(rogue_onu_parameters->rogue_onu_level_clear, 
        rogue_onu_parameters->rogue_onu_diff_clear, rogue_onu_parameters->source_select);
    if (drv_error != PON_NO_ERROR) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to configure rouge detect parameters due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error =  ag_drv_ngpon_tx_cfg_rogue_level_time_set(
       rogue_onu_parameters->rogue_onu_level_threshold);
    if (drv_error != PON_NO_ERROR) 
    { 
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to configure rouge detect parameters (level time) due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_tx_cfg_rogue_diff_time_set(
        rogue_onu_parameters->rogue_onu_diff_threshold);
    if (drv_error != PON_NO_ERROR) 
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
           "Failed to configure rouge detect parameters due to driver error %d\n", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Save to database */
    g_xgpon_db.rogue_onu_detection_parameters.source_indication = rogue_onu_parameters->source_select ;
    g_xgpon_db.rogue_onu_detection_parameters.rogue_onu_level_threshold = rogue_onu_parameters->rogue_onu_level_threshold ;
    g_xgpon_db.rogue_onu_detection_parameters.rogue_onu_diff_threshold  = rogue_onu_parameters->rogue_onu_diff_threshold ;

    return PON_NO_ERROR;

}

PON_ERROR_DTE _ngpon_link_activate(bdmf_boolean xi_initial_state_disable)
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    xgpon_error = ngpon_link_activate(xi_initial_state_disable);
    if (PON_NO_ERROR != xgpon_error)
    {
        return PON_GENERAL_ERROR ;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE _ngpon_link_deactivate(void)
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR;

    xgpon_error = ngpon_link_deactivate();
    if (PON_NO_ERROR != xgpon_error)
    {
        return PON_GENERAL_ERROR;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_configure_onu_serial_number(PON_SERIAL_NUMBER_DTE sn)
{
    /* Link already activated? */
    if (g_xgpon_db.link_params.operation_sm.link_state == LINK_STATE_ACTIVE)
    {
        return PON_ERROR_INVALID_STATE;
    }

    p_log(ge_onu_logger.sections.stack.debug.general_id, 
        "Configured serial number %x %x %x %x %x %x %x %x\n", 
        sn.vendor_code[0], sn.vendor_code[1], sn.vendor_code[2], 
        sn.vendor_code[3], sn.serial_number[0], sn.serial_number[1],
        sn.serial_number[2], sn.serial_number[3]);

    memcpy(g_xgpon_db.onu_params.sn.serial_number, &sn, sizeof(NGPON_SERIAL_NUMBER));

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_configure_onu_password(PON_PASSWORD_DTE  xi_onu_password)
{
    uint32_t i;

    /* Link already activated? */
    if (g_xgpon_db.link_params.operation_sm.link_state == LINK_STATE_ACTIVE)
    {
        return (PON_ERROR_INVALID_STATE);
    }

    /* Copy password */
    for (i = 0; i < PON_PASSWORD_FIELD_SIZE; i++)
    {
        g_xgpon_db.onu_params.reg_id.reg_id[i] = xi_onu_password.password[i];
    }

#ifdef AKIVAS_DBG
    /* In bringup configuration OLT (Maple) expects all-zero Registration ID */
    for (i = 0; i < PON_PASSWORD_FIELD_SIZE; i++)
    {
        g_xgpon_db.onu_params.reg_id.reg_id[i] = 0;
    }

#endif
    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_set_gem_block_size(PON_GEM_BLOCK_SIZE_DTE xi_gem_block_size)
{
    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_dba_sr_process_initialize(uint32_t xi_sr_reporting_block,
    uint32_t xi_update_interval)
{
    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_dba_sr_process_terminate(void)
{
    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_configure_link_params ( rdpa_gpon_link_cfg_t * const xi_link_default_configuration )
{
    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    g_xgpon_db.rx_params.rx_din_polarity = xi_link_default_configuration -> rx_din_polarity;
    g_xgpon_db.tx_params.tx_pls_enable = xi_link_default_configuration -> transceiver_power_calibration_mode;
    g_xgpon_db.tx_params.tx_data_out_polarity = xi_link_default_configuration -> tx_data_polarity;

    g_xgpon_db.physic_params.transceiver_dv_hold_pattern = xi_link_default_configuration -> transceiver_dv_hold_pattern ;
    g_xgpon_db.physic_params.transceiver_dv_polarity = xi_link_default_configuration -> transceiver_dv_polarity ;
    g_xgpon_db.physic_params.transceiver_dv_setup_pattern = xi_link_default_configuration -> transceiver_dv_setup_pattern ;
    g_xgpon_db.physic_params.min_response_time = xi_link_default_configuration -> min_response_time ;
    g_xgpon_db.physic_params.num_of_psyncs_for_lof_assertion = xi_link_default_configuration -> number_of_psyncs_for_lof_assertion ;
    g_xgpon_db.physic_params.num_of_psyncs_for_lof_clear = xi_link_default_configuration -> number_of_psyncs_for_lof_clear ;

    g_xgpon_db.onu_params.ber_interval = xi_link_default_configuration -> ber_interval ;
    g_xgpon_db.onu_params.to1_timer_timeout = xi_link_default_configuration -> to1_timeout ;
    g_xgpon_db.onu_params.to2_timer_timeout = xi_link_default_configuration -> to2_timeout ;
    g_xgpon_db.onu_params.ber_threshold_for_sd_assertion = xi_link_default_configuration -> ber_threshold_for_sd_assertion ;
    g_xgpon_db.onu_params.ber_threshold_for_sf_assertion = xi_link_default_configuration -> ber_threshold_for_sf_assertion ;

    return ( PON_NO_ERROR ) ;
}

PON_ERROR_DTE ngpon_control_port_id_filter(PON_FLOW_ID_DTE xi_flow_id, bool xi_filter_status)
{
    PON_ERROR_DTE gpon_error ;

    gpon_error = ngpon_control_xgem_flow(xi_flow_id ,(bdmf_boolean) xi_filter_status);

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_modify_encryption_port_id_filter(PON_FLOW_ID_DTE xi_flow_id, bool xi_encryption_mode)
{
    p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Configuration of DS encryption not configurable in XGPON\n");
    return PON_GENERAL_ERROR;
}

PON_ERROR_DTE ngpon_configure_port_id_filter(PON_FLOW_ID_DTE xi_flow_id, PON_PORT_ID_DTE xi_port_id_filter, PON_PORT_ID_DTE xi_port_id_mask,
                                             bool xi_encryption_mode, PON_FLOW_TYPE_DTE xi_flow_type, PON_FLOW_PRIORITY_DTE xi_flow_priority, bool crcEnable)
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    xgpon_error = ngpon_config_xgem_flow(xi_flow_id, xi_port_id_filter, xi_flow_priority, (NGPON_ENCRYPTION_RING)xi_encryption_mode, (bdmf_boolean)crcEnable);
    if ( PON_NO_ERROR != xgpon_error )
        return PON_GENERAL_ERROR ;

    return ( PON_NO_ERROR ) ;
}

PON_ERROR_DTE ngpon_configure_tcont(PON_TCONT_ID_DTE xi_tcont_queue_id, PON_ALLOC_ID_DTE xi_alloc_id)
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    xgpon_error = ngpon_assign_tcont(xi_tcont_queue_id, xi_alloc_id);

    if ( PON_NO_ERROR != xgpon_error )
        return PON_GENERAL_ERROR ;

    return PON_NO_ERROR;
}

PON_ERROR_DTE _ngpon_remove_tcont ( PON_TCONT_ID_DTE  xi_tcont_id )
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    xgpon_error = ngpon_remove_tcont(xi_tcont_id);

    if ( PON_NO_ERROR != xgpon_error )
        return PON_GENERAL_ERROR ;

    return PON_NO_ERROR;
}

PON_ERROR_DTE _ngpon_link_reset ( void )
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    xgpon_error = ngpon_link_reset();

    if ( PON_NO_ERROR != xgpon_error )
        return PON_GENERAL_ERROR ;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_mask_rx_pon_interrupts ( void )
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    xgpon_error = disable_interrupts(true, false);

    if ( PON_NO_ERROR != xgpon_error )
        return PON_GENERAL_ERROR ;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_mask_tx_pon_interrupts ( void )
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    xgpon_error = disable_interrupts(false, true);

    if ( PON_NO_ERROR != xgpon_error )
        return PON_GENERAL_ERROR ;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_unmask_tx_pon_interrupts ( void )
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    xgpon_error = enable_interrupts(false, true);

    if ( PON_NO_ERROR != xgpon_error )
        return PON_GENERAL_ERROR ;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_unmask_rx_pon_interrupts ( void )
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    xgpon_error = enable_interrupts(true, false);

    if ( PON_NO_ERROR != xgpon_error )
        return PON_GENERAL_ERROR ;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_tx_set_tcont_to_counter_group_association(uint8_t tcont0, uint8_t tcont1, uint8_t tcont2, uint8_t tcont3)
{
    PON_ERROR_DTE xgpon_error = PON_NO_ERROR ;

    xgpon_error = ag_drv_ngpon_tx_cfg_tcont_to_cnt_grp_map_set(tcont0, tcont1, tcont2, tcont3);
    if (PON_NO_ERROR != xgpon_error)
    {
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_tx_set_flush_valid_bit(uint8_t tcont_id, bdmf_boolean flush_enable, bdmf_boolean flush_immediate)
{
    PON_ERROR_DTE drv_error;
    uint32_t   dbr_flush_mask;

    int high_tcont_indx = 0;

    /*
     *     Set T-CONT flush
     */
    drv_error = ag_drv_ngpon_tx_cfg_tcont_flush_set(tcont_id, flush_enable, flush_immediate);
    if (drv_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Failed to configure flush parameters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /*
     *   Set DBR flush
     */
    
    if (tcont_id < 32)
    {
#if defined(CONFIG_BCM96858)
#define TEMP_ADDRESS_FOR_NGPON_32 ( 0x80168390 )
#elif defined(CONFIG_BCM96856)
#define TEMP_ADDRESS_FOR_NGPON_32 ( 0x82db8390 )
#endif
        NGPON_READ_32(TEMP_ADDRESS_FOR_NGPON_32, dbr_flush_mask);
        if (flush_enable)
        {
            dbr_flush_mask |= (1 << tcont_id);
        }
        else
        {
            dbr_flush_mask &= ~(1 << tcont_id);
        }
        NGPON_WRITE_32(TEMP_ADDRESS_FOR_NGPON_32, dbr_flush_mask);
#undef TEMP_ADDRESS_FOR_NGPON_32
    }
    else
    {
#if defined(CONFIG_BCM96858)
#define TEMP_ADDRESS_FOR_NGPON_32 ( 0x80168394 )
#elif defined(CONFIG_BCM96856)
#define TEMP_ADDRESS_FOR_NGPON_32 ( 0x82db8394 )
#endif
        NGPON_READ_32(TEMP_ADDRESS_FOR_NGPON_32, dbr_flush_mask);
        high_tcont_indx = tcont_id - 32;
        if (flush_enable)
        {
            dbr_flush_mask |= (1 << high_tcont_indx);
        }
        else
        {
            dbr_flush_mask &= ~(1 << high_tcont_indx);
        }
        NGPON_WRITE_32(TEMP_ADDRESS_FOR_NGPON_32, dbr_flush_mask);
#undef TEMP_ADDRESS_FOR_NGPON_32
    }


    return drv_error;
}

PON_ERROR_DTE ngpon_tx_generate_misc_transmit(uint8_t prodc[16], uint8_t prcl,
    uint8_t brc, uint8_t prl, uint16_t msstart, uint16_t msstop, bdmf_boolean enable)
{
    int drv_error;
    uint16_t transceiver;
    PON_ERROR_DTE rc;

    ngpon_tx_cfg_func_en tx_status = {};
    ngpon_tx_profile_profile_ctrl profile_ctrl = {};
    uint32_t msb_preamble;
    uint32_t lsb_preamble;
    uint32_t alloc_cfg1;
    uint32_t start_size;


    static ngpon_tx_cfg_func_en def_tx_status;
    static uint8_t def_speed;
    static uint8_t def_standard;
    static uint8_t def_txen;
    static uint32_t def_start_size;
    static ngpon_tx_profile_profile_ctrl def_profile_ctrl;
    static uint32_t def_preamble0_value;
    static uint32_t def_preamble1_value;
    static uint32_t def_alloc_cfg1;

    BpGetGponOpticsType(&transceiver);

    drv_error = ag_drv_ngpon_tx_profile_profile_ctrl_get(0, &profile_ctrl);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read ngpon_tx_profile_profile_ctrl. error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    if (profile_ctrl.prof_en == 1)
    {
        profile_ctrl.prof_en = 0;

        drv_error = ag_drv_ngpon_tx_profile_profile_ctrl_set(0, &profile_ctrl);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set ngpon_tx_profile_profile_ctrl. error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        profile_ctrl.prof_en = 1;
    }

    if (enable)
    {

      ngpon_wan_top_enable_transmitter(1) ;

        drv_error = ag_drv_ngpon_tx_cfg_onu_tx_en_get(&def_txen, &def_standard, &def_speed);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to read ngpon_tx_cfg_onu_tx_en. error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_ngpon_tx_cfg_func_en_get(&def_tx_status);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to read ngpon_tx_cfg_func_en. error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        def_tx_status.reserved0 &= 0x1;

#if defined(CONFIG_BCM96858)
        NGPON_READ_32(0x8016b000, def_start_size);
        NGPON_READ_32(0x8016b004, def_alloc_cfg1);
#elif defined(CONFIG_BCM96856)
        NGPON_READ_32(0x82dbb000, def_start_size);
        NGPON_READ_32(0x82dbb004, def_alloc_cfg1);
#endif

        drv_error = ag_drv_ngpon_tx_profile_preamble_0_get(0, &def_preamble0_value);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to read ngpon_tx_profile_preamble_0. error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_ngpon_tx_profile_preamble_1_get(0, &def_preamble1_value);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to read ngpon_tx_profile_preamble_1. error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        memcpy(&def_profile_ctrl, &profile_ctrl, sizeof(ngpon_tx_profile_profile_ctrl));

        tx_status.reserved2 = 1;
        tx_status.scrmblr_en = 1;

        profile_ctrl.preamble_len = prcl;
        profile_ctrl.preamble_total_len = prcl * brc;
        profile_ctrl.prof_en = 1;

        if (BDMF_FALSE == g_xgpon_db.debug_params.swap_preamble_msb_lsb)
        {
            bytes_to_word (prodc , &msb_preamble);
            bytes_to_word (&prodc[4] , &lsb_preamble);
        }
        else
        {
            bytes_to_word (prodc , &lsb_preamble);
            bytes_to_word (&prodc[4] , &msb_preamble);
        }

        start_size = ((0x0000ffff & (msstop - msstart)) | (0xffff0000 & (msstart << 16)));
        alloc_cfg1 = def_alloc_cfg1 & 0xfffcffff;

        rc = misc_cfg(msb_preamble, lsb_preamble, &profile_ctrl, alloc_cfg1, start_size, &tx_status, def_speed, def_standard, 1);
        if (rc)
            return rc;
    }
    else
    {
      ngpon_wan_top_enable_transmitter(0);
        /* Restore default */
        rc = misc_cfg(def_preamble0_value, def_preamble0_value, &def_profile_ctrl, def_alloc_cfg1, def_start_size, &def_tx_status, def_speed, def_standard, 0);
        if (rc)
            return rc;
    }

    if (transceiver == BP_GPON_OPTICS_TYPE_PMD)
    {
        pmd_dev_enable_prbs_or_misc_mode(enable, 0);
    }

    return PON_NO_ERROR;
}

static PON_ERROR_DTE misc_cfg(uint32_t def_preamble0_value, uint32_t def_preamble1_value, ngpon_tx_profile_profile_ctrl *profile_ctrl, uint32_t alloc_cfg1,
                                                          uint32_t start_size, ngpon_tx_cfg_func_en *tx_status, uint8_t speed, uint8_t standard, uint8_t txen)
{
    int drv_error;

    drv_error = ag_drv_ngpon_tx_cfg_onu_tx_en_set(0, standard, speed);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to set ngpon_tx_cfg_onu_tx_en. error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

#if defined(CONFIG_BCM96858)
    NGPON_WRITE_32(0x8016b000, start_size);
    NGPON_WRITE_32(0x8016b004, alloc_cfg1);
#elif defined(CONFIG_BCM96856)
    NGPON_WRITE_32(0x82dbb000, start_size);
    NGPON_WRITE_32(0x82dbb004, alloc_cfg1);
#endif

    drv_error = ag_drv_ngpon_tx_profile_preamble_0_set(0, def_preamble0_value);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to set ngpon_tx_profile_preamble_0. error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_tx_profile_preamble_1_set(0, def_preamble1_value);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to set ngpon_tx_profile_preamble_1. error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

        drv_error = ag_drv_ngpon_tx_cfg_func_en_set(tx_status);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to set ngpon_tx_cfg_func_en. error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    if ((profile_ctrl->delimiter_len == 0) && (txen == 1))
    {
        profile_ctrl->delimiter_len = 8;

        drv_error = ag_drv_ngpon_tx_profile_delimiter_0_set(0, 0xb3bdd310);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set ngpon_tx_profile_delimiter_0. error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_ngpon_tx_profile_delimiter_1_set(0, 0xb2c50fa1);
        if (drv_error > 0)
        {
            p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set ngpon_tx_profile_delimiter_0. error %d.", drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    drv_error = ag_drv_ngpon_tx_profile_profile_ctrl_set(0, profile_ctrl);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to set ngpon_tx_profile_profile_ctrl. error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_tx_cfg_onu_tx_en_set(txen, standard, speed);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to set ngpon_tx_cfg_onu_tx_en. error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_link_status(LINK_STATE_DTE * const xo_state,
    LINK_SUB_STATE_DTE * const xo_sub_state, LINK_OPERATION_STATES_DTE * const xo_operational_state)
{
    uint8_t  sub_state_string_offset = 0;


    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }
    /*
     *   Sub-states of NGPON2 state machine are reported via "sub_state" parameter
     *
     * Since original intention was to present unknown/standby/operational/stop
     * NGPON2 state machine sub states are represented by values above rdpa_pon_oper_state_stop 
     * in rdpa_pon_sub_state enum - therefore offset is required
     */
    switch (g_xgpon_db.link_params.operation_sm.activation_state)
    {
       case OPERATION_STATE_O1:
         sub_state_string_offset = O1_SUB_STATE_STRINGS_IN_RDPA ;
         break;

      case OPERATION_STATE_O5:
         sub_state_string_offset = O5_SUB_STATE_STRINGS_IN_RDPA ;
         break;

      case OPERATION_STATE_O8:
         sub_state_string_offset = O8_SUB_STATE_STRINGS_IN_RDPA ;
         break;

      default:
        sub_state_string_offset = 0;
    }

    *xo_state = g_xgpon_db.link_params.operation_sm.link_state ;
    *xo_sub_state = g_xgpon_db.link_params.sub_sm_state + sub_state_string_offset ;
    *xo_operational_state = g_xgpon_db.link_params.operation_sm.activation_state ;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_link_parameters ( rdpa_gpon_link_cfg_t * const xo_parameters )
{
    rdpa_gpon_link_cfg_t link_parameters ;

    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    link_parameters.rx_din_polarity                    = g_xgpon_db.rx_params.rx_din_polarity;
    link_parameters.transceiver_power_calibration_mode = g_xgpon_db.tx_params.tx_pls_enable;
    link_parameters.tx_data_polarity                   = g_xgpon_db.tx_params.tx_data_out_polarity;

    link_parameters.transceiver_dv_hold_pattern           = g_xgpon_db.physic_params.transceiver_dv_hold_pattern;
    link_parameters.transceiver_dv_polarity               = g_xgpon_db.physic_params.transceiver_dv_polarity;
    link_parameters.transceiver_dv_setup_pattern          = g_xgpon_db.physic_params.transceiver_dv_setup_pattern;
    link_parameters.transceiver_power_calibration_pattern = 0; //g_xgpon_db.physic_params. ;
    link_parameters.transceiver_power_calibration_size    = 0; //g_xgpon_db.physic_params. ;
    link_parameters.min_response_time                     = g_xgpon_db.physic_params.min_response_time;
    link_parameters.number_of_psyncs_for_lof_assertion    = g_xgpon_db.physic_params.num_of_psyncs_for_lof_assertion;
    link_parameters.number_of_psyncs_for_lof_clear        = g_xgpon_db.physic_params.num_of_psyncs_for_lof_clear;

    link_parameters.ber_interval                   = g_xgpon_db.onu_params.ber_interval;
    link_parameters.to1_timeout                    = g_xgpon_db.onu_params.to1_timer_timeout;
    link_parameters.to2_timeout                    = g_xgpon_db.onu_params.to2_timer_timeout;
    link_parameters.ber_threshold_for_sd_assertion = g_xgpon_db.onu_params.ber_threshold_for_sd_assertion;
    link_parameters.ber_threshold_for_sf_assertion = g_xgpon_db.onu_params.ber_threshold_for_sf_assertion;

    /* Set output values */
    *xo_parameters = link_parameters ;

    return ( PON_NO_ERROR ) ;
}

static void reverse_8bytes(uint8_t v[])
{
    int i;
    uint8_t t;

    for ( i = 0; i < 8/2; i++ ) {
        t = v[i];
        v[i] = v[8-1-i];
        v[8-1-i] = t;
    }
}

PON_ERROR_DTE ngpon_get_burst_prof(rdpa_gpon_burst_prof_t * const xo_burst_prof, const int index)
{
    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }
    if ( ! g_xgpon_db.link_params.burst_profiles.profiles[index].profile_enable )
    {
        return PON_BURST_PROF_NOT_ENABLED_YET;
    }

    *xo_burst_prof = g_xgpon_db.link_params.burst_profiles.profiles[index];
    reverse_8bytes(xo_burst_prof->preamble); // for print order
    reverse_8bytes(xo_burst_prof->pon_tag); // for print order

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_onu_id ( uint16_t * const xo_onu_id )
{
    if ( g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    *xo_onu_id = g_xgpon_db.onu_params.onu_id;

    return ( PON_NO_ERROR ) ;
}

PON_ERROR_DTE ngpon_get_overhead_and_length(PON_TX_OVERHEAD_DTE  * const xo_pon_overhead, uint8_t * const xo_pon_overhead_length,
                                             uint8_t * const xo_pon_overhead_repetition_length, uint8_t * const xo_pon_overhead_repetition)
{

    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_transceiver_power_level(uint8_t * const xo_transceiver_power_level)
{
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Current power-level of the optical transceiver */
    /* No support in DB */
    *xo_transceiver_power_level = 0; //g_xgpon_db.onu_params.

    return ( PON_NO_ERROR ) ;
}

PON_ERROR_DTE ngpon_get_equalization_delay ( uint32_t * const xo_equalization_delay )
{
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    *xo_equalization_delay = g_xgpon_db.physic_params.eqd_in_bits;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_aes_encryption_key ( AES_KEY_DTE * xi_aes_encryption_key, uint8_t xi_fragment_index )
{
    if ( g_xgpon_db.link_params.operation_sm.activation_state != OPERATION_STATE_O5 &&
         g_xgpon_db.link_params.operation_sm.activation_state != OPERATION_STATE_O7 )
    {
        return PON_ERROR_INVALID_STATE;
    }

    memcpy(xi_aes_encryption_key, &g_xgpon_db.link_params.key_sm.new_key[xi_fragment_index], 
        sizeof(AES_KEY_DTE));

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_set_multicast_encryption_key (uint32_t key_index, uint8_t* encryption_key)
{
    uint32_t *encryption_key_p = (uint32_t *) encryption_key;
    ngpon_rx_enc_key_valid  key_valid;
    int i;
    
    PON_ERROR_DTE ngpon_error = PON_NO_ERROR;

    /* TODO: Decrypt with AES-ECB using the KEK */

    ngpon_error = ag_drv_ngpon_rx_enc_key_valid_get (&key_valid);
    if (ngpon_error != PON_NO_ERROR)
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to get key valid (rx) : error %d !", ngpon_error ) ;
    }

    /*  Write new key to Multicast_keyX and set as valid  */
    if (key_index == KEY_INDEX_FIRST_KEY) 
    {
        for (i=0; i<4; i++)
        {
            ngpon_error = ag_drv_ngpon_rx_enc_key1_mcst_set(i, htonl(*(encryption_key_p+i)));
            if (ngpon_error != PON_NO_ERROR)
            {
                p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write word %d of the key to rx: error %d !", i, ngpon_error) ;
            }
        }
        key_valid.key1_mcst = BDMF_TRUE ;
    }
    else
    {
        for (i=0 ; i < 4 ; i ++ )
        {
            ngpon_error = ag_drv_ngpon_rx_enc_key2_mcst_set(i, htonl(*(encryption_key_p+i)));
            if ( ngpon_error != PON_NO_ERROR )
            {
                p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to write word %d of the key to rx: error %d !", i, ngpon_error);
            }
        }
        key_valid.key2_mcst = BDMF_TRUE ;
    }

    ngpon_error = ag_drv_ngpon_rx_enc_key_valid_set (&key_valid);
    if (ngpon_error != PON_NO_ERROR)
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to set key valid (rx) : error %d !", ngpon_error ) ;
    }

    if (ngpon_error == PON_NO_ERROR)
    {
        /* Dump New key */
        p_log (ge_onu_logger.sections.stack.upstream_ploam.key_report_message_id,
                "New Key has been written to MAC RX :") ;
        dump_key (encryption_key, CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE);
    }
    return ngpon_error;
}


PON_ERROR_DTE ngpon_get_multicast_encryption_key (uint32_t key_index, uint8_t* encryption_key, uint8_t* mcast_key_valid)
{
   uint32_t *encryption_key_p = (uint32_t *) encryption_key;
   ngpon_rx_enc_key_valid  key_valid;
   int i;
   PON_ERROR_DTE ngpon_error = PON_NO_ERROR;

   ngpon_error = ag_drv_ngpon_rx_enc_key_valid_get (&key_valid);
   if (ngpon_error != PON_NO_ERROR)
   {
      p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id,
              "Unable to get key valid (rx) : error %d !", ngpon_error ) ;
   }

   /*  Read new key from Multicast_keyX */
   if (key_index == KEY_INDEX_FIRST_KEY) 
   {
      for (i=0; i<4; i++)
      {
         ngpon_error = ag_drv_ngpon_rx_enc_key1_mcst_get(i, encryption_key_p+i);
         if (ngpon_error != PON_NO_ERROR)
         {
           p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read word %d of the key to rx: error %d !", i, ngpon_error) ;
         }
      }
      *mcast_key_valid =  key_valid.key1_mcst;
   }
   else
   {
      for (i=0 ; i < 4 ; i ++ )
      {
         ngpon_error = ag_drv_ngpon_rx_enc_key2_mcst_get(i, encryption_key_p+i);
         if ( ngpon_error != PON_NO_ERROR )
         {
           p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read word %d of the key to rx: error %d !", i, ngpon_error);
         }
      }
      *mcast_key_valid = key_valid.key2_mcst;
   }

   *encryption_key = ntohl(*encryption_key);
    
   /* TODO: Encrypt with AES-ECB using the KEK */

   if (ngpon_error == PON_NO_ERROR)
   {
       /* Dump New key */
      p_log (ge_onu_logger.sections.stack.upstream_ploam.key_report_message_id,
          "Key has been read from MAC RX :") ;
      dump_key (encryption_key, CE_XGPON_DEFAULT_AES_KEY_BYTES_SIZE);
   }
   return ngpon_error;
}

static uint32_t ngpon_mask_bip_error_counter(uint32_t const xo_bip_errors)
{
    return (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGPON ||
            g_xgpon_db.onu_params.stack_mode == NGPON_MODE_XGS) ?
                0 : xo_bip_errors;
}

PON_ERROR_DTE ngpon_get_rxpon_pm_counters(PON_RXPON_PM_COUNTERS * const rxpon_pm_counters)
{
    int rc = PON_NO_ERROR;
    uint16_t cnt;
    int i;

    rc = ag_drv_ngpon_rx_pm_sync_lof_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read sync lof counter rc=%d !", rc);
    }
    rxpon_pm_counters->sync_lof_counter = cnt;


    rc = ag_drv_ngpon_rx_pm_bwmap_hec_err_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read bwmap hec err counter rc=%d !", rc);
    }
    rxpon_pm_counters->bwmap_hec_err_counter = cnt;


    rc = ag_drv_ngpon_rx_pm_bwmap_invalid_burst_series_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read invalid_burst_series counter rc=%d !", rc);
    }
    rxpon_pm_counters->bwmap_invalid_burst_series_counter = cnt;


    rc = ag_drv_ngpon_rx_pm_hlend_hec_err_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read hlend_hec_err counter rc=%d !", rc);
    }
    rxpon_pm_counters->hlend_hec_err_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_del_lcdg_get(&(rxpon_pm_counters->del_lcdg_counter));
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read del_lcdg counter rc=%d !", rc);
    }

    rc = ag_drv_ngpon_rx_pm_sync_ponid_hec_err_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read sync_ponid_hec_err counter rc=%d !", rc);
    }
    rxpon_pm_counters->sync_ponid_hec_err_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_del_pass_pkt_get(&(rxpon_pm_counters->del_pass_pkt_counter));
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read del_pass_pkt counter rc=%d !", rc);
    }

    rc = ag_drv_ngpon_rx_pm_bwmap_correct_get(&(rxpon_pm_counters->bwmap_correct_counter));
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read bwmap_correct counter rc=%d !", rc);
    }

    rc = ag_drv_ngpon_rx_pm_bwmap_bursts_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read bwmap_bursts counter rc=%d !", rc);
    }
    rxpon_pm_counters->bwmap_bursts_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_xhp_pass_pkt_get(&(rxpon_pm_counters->xhp_pass_pkt_counter));
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read xhp_pass_pkt counter rc=%d !", rc);
    }

    rc = ag_drv_ngpon_rx_pm_dec_cant_allocate_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read dec_cant_allocate counter rc=%d !", rc);
    }
    rxpon_pm_counters->dec_cant_allocate_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_dec_invalid_key_idx_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read counter rc=%d !", rc);
    }
    rxpon_pm_counters->dec_invalid_key_idx_counter = cnt;

    for (i=0; i<2; i++)
    {
        rc = ag_drv_ngpon_rx_pm_plp_valid_onuid_get(i, &cnt);
        if (rc != PON_NO_ERROR)
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read dec_invalid_key_idx counter rc=%d !", rc);
        }
        rxpon_pm_counters->plp_valid_onuid_counter[i] = cnt;
    }

    rc = ag_drv_ngpon_rx_pm_plp_valid_bcst_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read plp_valid_bcst counter rc=%d !", rc);
    }
    rxpon_pm_counters->plp_valid_bcst_counter = cnt;
    
    rc = ag_drv_ngpon_rx_pm_plp_mic_err_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read plp_mic_err counter rc=%d !", rc);
    }
    rxpon_pm_counters->mic_err_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_bwmap_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read bwmap_hec_fix counter rc=%d !", rc);
    }
    rxpon_pm_counters->bwmap_hec_fix_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_hlend_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read hlend_hec_fix counter rc=%d !", rc);
    }
    rxpon_pm_counters->hlend_hec_fix_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_xhp_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read xhp_hec_fix counter rc=%d !", rc);
    }
    rxpon_pm_counters->xhp_hec_fix_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_sync_sfc_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read sync_sfc_hec_fix counter rc=%d !", rc);
    }
    rxpon_pm_counters->sync_sfc_hec_fix_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_sync_ponid_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read sync_ponid_hec_fix counter rc=%d !", rc);
    }
    rxpon_pm_counters->sync_ponid_hec_fix_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_xgem_overrun_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read xgem_overrun counter rc=%d !", rc);
    }
    rxpon_pm_counters->xgem_overrun_counter = cnt;


    rc = ag_drv_ngpon_rx_pm_bwmap_discard_dis_tx_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read bwmap_discard_dis_tx counter rc=%d !", rc);
    }
    rxpon_pm_counters->bwmap_discard_dis_tx_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_fec_bit_err_get(&(rxpon_pm_counters->fec_bit_err_counter));
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read discard_dis_tx counter rc=%d !", rc);
    }

    rc = ag_drv_ngpon_rx_pm_fec_sym_err_get(&(rxpon_pm_counters->fec_sym_err_counter));
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read fec_sym_err counter rc=%d !", rc);
    }

    rc = ag_drv_ngpon_rx_pm_fec_cw_err_get(&(rxpon_pm_counters->fec_cw_err_counter));
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read counter rc=%d !", rc);
    }

    rc = ag_drv_ngpon_rx_pm_fec_uc_cw_get(&(rxpon_pm_counters->fec_uc_cw_counter));
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read fec_uc_cw counter rc=%d !", rc);
    }

    for (i=0; i<3; i++)
    {
        rc = ag_drv_ngpon_rx_pm_bwmap_num_alloc_tcont_get(i, &cnt);
        if (rc != PON_NO_ERROR)
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read bwmap_num_alloc_tcont counter rc=%d !", rc);
        }
        rxpon_pm_counters->bwmap_num_alloc_tcont_counter[i] = cnt;
    }

    for (i=0; i<3; i++)
    {
        rc = ag_drv_ngpon_rx_pm_bwmap_tot_bw_tcont_get(i, &(rxpon_pm_counters->bwmap_tot_bw_tcont_counter[i]));
        if (rc != PON_NO_ERROR)
        {
            p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read bwmap_tot_bw_tcont counter rc=%d !", rc);
        }

    }

    rc = ag_drv_ngpon_rx_pm_fs_bip_err_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read fs_bip_err counter rc=%d !", rc);
    }
    rxpon_pm_counters->fs_bip_err_counter = ngpon_mask_bip_error_counter(cnt);

    /* To be added later
    int ag_drv_ngpon_rx_pm_xhp_xgem_per_flow_get(uint32_t ngpon_rx_flow_id, uint32_t *cnt);
    int ag_drv_ngpon_rx_pm_xhp_bytes_per_flow_get(uint32_t ngpon_rx_flow_id, uint32_t *cnt);
    */
    

    return rc;
}

PON_ERROR_DTE ngpon_get_bip_error_counter( uint32_t * const xo_bip_errors)
{
    uint16_t bip_errors;
    int rc;

    if ( (rc = ag_drv_ngpon_rx_pm_fs_bip_err_get(&bip_errors)) != PON_NO_ERROR)
    {
        p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read fs_bip_err counter rc=%d !", rc);
    }
    *xo_bip_errors = ngpon_mask_bip_error_counter(bip_errors);
    return rc;
}

PON_ERROR_DTE ngpon_get_rxpon_ploam_counters ( PON_RX_PLOAM_COUNTERS_DTE * const xo_rxpon_ploam_counters )
{
    uint16_t counter0, counter1;

    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    ag_drv_ngpon_rx_pm_plp_valid_onuid_get(0, &counter0);
    ag_drv_ngpon_rx_pm_plp_valid_onuid_get(1, &counter1);

    xo_rxpon_ploam_counters->valid_onu_id_ploam_counter = counter0 + counter1;

    ag_drv_ngpon_rx_pm_plp_valid_bcst_get(&counter0);

    xo_rxpon_ploam_counters->valid_broadcast_ploam_counter = counter0;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_rx_unkonw_ploam_counter ( uint32_t * const  xo_rx_unknown_counters )
{
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* No support in H/W */
    *xo_rx_unknown_counters = 0; //g_xgpon_db. unknown_ploam.non_standard_rx_msg;

    return ( PON_NO_ERROR ) ;
}

PON_ERROR_DTE ngpon_get_txpon_pm_counters(uint16_t *const illegal_access_counter,
    uint32_t * const idle_ploam_counter, uint32_t * const tx_ploam_counter)
{
    int drv_error;
    uint32_t i;

    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    drv_error = ag_drv_ngpon_tx_perf_mon_ilgl_accs_cnt_get(illegal_access_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read illegal_access_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    ag_drv_ngpon_tx_perf_mon_req_plm_get(NGPON_PLOAM_TYPE_NORMAL, tx_ploam_counter);
    ag_drv_ngpon_tx_perf_mon_req_plm_get(NGPON_PLOAM_TYPE_REGISTRATION, &i);
    *tx_ploam_counter += i;
    ag_drv_ngpon_tx_perf_mon_req_plm_get(NGPON_PLOAM_TYPE_IDLE_OR_SN, idle_ploam_counter);

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_port_id_configuration(PON_FLOW_ID_DTE xi_flow_id, bool * const xo_filter_status, PON_PORT_ID_DTE * const xo_port_id_filter,
                                              PON_PORT_ID_DTE * const xo_port_id_mask, bool * const xo_encryption_mode, PON_FLOW_TYPE_DTE * const xo_flow_type,
                                              PON_FLOW_PRIORITY_DTE * const xo_flow_priority, bool * xo_crc_enable)
{
    /*
     * Since the encryption ring configuration is provided in the MIB format:
     *     0 - unencrypted
     *     1 - unicast encryption (bidirectional)
     *     2 - multicast encryption at this GEM
     *     3 - unicast encryption (downstream only)
     *  while the register is set to:
     *     0 - ONU#1 key set
     *     1 - ONU#2 key set
     *     2 - multicast key set
     *     3 - plain text (no encryption)
     *  the mapping is provided by the following table
     */
  static uint8_t reg_to_enc_ring_mapping[] = {   /* In case the encryption is enabled: */
      1, /* unicast_encryption */
      1, /* unicast encryption, we _always_ use keys of ONU #1 */
      2, /* multicast key set */
      0  /* unicast encryption, downstream only - but here the downstream is configured , we _always_ use keys of ONU #1 */
    } ;

    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    * xo_filter_status = g_xgpon_db.ds_xgem_flow_table[xi_flow_id].enable_flag;
    * xo_port_id_filter = g_xgpon_db.ds_xgem_flow_table[xi_flow_id].xgem_port_id;
    * xo_port_id_mask = g_xgpon_db.ds_xgem_flow_table[xi_flow_id].xgem_port_mask_id;
    * xo_encryption_mode = (g_xgpon_db.ds_xgem_flow_table[xi_flow_id].encryption)?
      reg_to_enc_ring_mapping[g_xgpon_db.ds_xgem_flow_table[xi_flow_id].encryption_ring] : 0 /* unencrypted */;
    * xo_flow_type = g_xgpon_db.ds_xgem_flow_table[xi_flow_id].flow_configured;
    * xo_flow_priority = g_xgpon_db.ds_xgem_flow_table[xi_flow_id].flow_priority;
    * xo_crc_enable = g_xgpon_db.ds_xgem_flow_table [ xi_flow_id ].crc_enable;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_encryption_error_counter(uint32_t *const xo_rx_encryption_error)
{
    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* No support in H/W */
    *xo_rx_encryption_error = 0;

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_tcont_configuration(PON_TCONT_ID_DTE xi_tcont_id, PON_TCONT_DTE * const xo_tcont_cfg )
{
    uint32_t index;

    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if ( !NGPON_TCONT_ID_IN_RANGE ( xi_tcont_id ) )
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR ;
    }

    for ( index = 0 ; index < NGPON_MAX_NUM_OF_TCONT_IDS ; index++ )
    {
        if (g_xgpon_db.tconts_table[index].config_tcont_valid_flag == BDMF_TRUE &&
            g_xgpon_db.tconts_table[index].tcont_id == xi_tcont_id)
        {
            xo_tcont_cfg->tcont_id = xi_tcont_id;
            xo_tcont_cfg->alloc_id = g_xgpon_db.tconts_table[index].alloc_id;
            xo_tcont_cfg->assign_alloc_valid_flag = g_xgpon_db.tconts_table[index].assign_alloc_valid_flag;
            xo_tcont_cfg->config_tcont_valid_flag = g_xgpon_db.tconts_table[index].config_tcont_valid_flag;
            xo_tcont_cfg->tcont_queue_size = g_xgpon_db.tconts_table[index].tcont_queue_size;
            break;
        }
    }
    if ( index == NGPON_MAX_NUM_OF_TCONT_IDS )
    {
        return ( PON_ERROR_TCONT_NOT_ASSIGNED );
    }

    return ( PON_NO_ERROR ) ;
}

PON_ERROR_DTE ngpon_get_tcont_queue_pm_counters(uint32_t xi_fifo_id, uint32_t * const xo_transmitted_idle_counter,
                                                uint32_t * const xo_transmitted_gem_counter, uint32_t * const xo_transmitted_packet_counter,
                                                uint16_t *const xo_requested_dbr_counter, uint16_t * const xo_valid_access_counter)
{
    PON_ERROR_DTE drv_error;

    if (g_software_state != NGPON_SOFTWARE_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (xi_fifo_id < 0 || xi_fifo_id > 3)
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR ;
    }

    drv_error = ag_drv_ngpon_tx_perf_mon_tx_xgem_idle_cnt_get(xi_fifo_id, xo_transmitted_idle_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read transmitted_idle_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_tx_perf_mon_tx_xgem_frm_cnt_get(xi_fifo_id, xo_transmitted_gem_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read transmitted_gem_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_tx_perf_mon_tx_pckt_cnt_get(xi_fifo_id, xo_transmitted_packet_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read transmitted_packet_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_tx_perf_mon_rx_accs_cnt_get(xi_fifo_id, xo_valid_access_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read valid_access_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_tx_perf_mon_req_dbr_cnt_get(xi_fifo_id, xo_requested_dbr_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read requested_dbr_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_ploam_crc_error_counter(PON_RXPON_REQUEST_DTE xi_request_type, uint32_t *const xo_ploam_crc_errors)
{
    uint16_t ploamerr;
    PON_ERROR_DTE drv_error;

    drv_error = ag_drv_ngpon_rx_pm_plp_mic_err_get(&ploamerr);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Failed to read CRC errror due to driver return Error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    switch (xi_request_type)
    {
        case CE_COUNTER_STACK_REQUEST:
            /* Calculate received counters values */
            * xo_ploam_crc_errors  = g_xgpon_db.rxpon_counters.stack_crc_counter + ploamerr;
            /* Clear counters */
            g_xgpon_db.rxpon_counters.stack_crc_counter = 0;
            /* Update gmp counters */
            g_xgpon_db.rxpon_counters.gmp_crc_counter += ploamerr;
            break;

        case CE_COUNTER_RDPA_REQUEST:
            /* Calculate received counters values */
            * xo_ploam_crc_errors = g_xgpon_db.rxpon_counters.gmp_crc_counter + ploamerr;
            /* Clear GEM counters */
            g_xgpon_db.rxpon_counters.gmp_crc_counter = 0;
            /* Update stack counters */
            g_xgpon_db.rxpon_counters.stack_crc_counter += ploamerr;
            break;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_rx_get_receiver_status(bdmf_boolean *lof_state, bdmf_boolean *fec_state, bdmf_boolean *lcdg_state)
{
    PON_ERROR_DTE drv_error;
    bdmf_boolean pon_id, fwi;
    uint32_t     reg;

    drv_error = ag_drv_ngpon_rx_gen_rcvrstat_get(lof_state, lcdg_state, &pon_id, &fwi);

    /* not supported in H/W for XG-PON and XGS */
    *fec_state = BDMF_TRUE;

    if ((g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_10G) ||
        (g_xgpon_db.onu_params.stack_mode == NGPON_MODE_NGPON2_2_5G))
    {
        NGPON_READ_32(0x80160f10, reg);

        *fec_state = ((reg & 0x3)  == 0x1)? BDMF_FALSE : BDMF_TRUE ;
    }
    
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read recevier status due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_rx_get_fec_counters(uint32_t *fec_corrected_bytes, 
    uint32_t *fec_corrected_codewords, uint32_t *fec_uncorrectable_codewords)
{
    PON_ERROR_DTE drv_error;

    drv_error = ag_drv_ngpon_rx_pm_fec_sym_err_get(fec_corrected_bytes);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read fec corrected bytes due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_rx_pm_fec_cw_err_get(fec_corrected_codewords);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read fec corrected codewords due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_rx_pm_fec_uc_cw_get(fec_uncorrectable_codewords);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read fec uncorrected words due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


PON_ERROR_DTE ngpon_rx_get_hec_counters(PON_RX_HEC_COUNTERS_DTE * const rxpon_hec_counter)
{
    int rc = PON_NO_ERROR;
    uint16_t cnt;

    rc = ag_drv_ngpon_rx_pm_bwmap_hec_err_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read bwmap hec err counter rc=%d !", rc);
    }
    rxpon_hec_counter->bwmap_hec_err_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_hlend_hec_err_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read hlend_hec_err counter rc=%d !", rc);
    }
    rxpon_hec_counter->hlend_hec_err_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_sync_ponid_hec_err_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read sync_ponid_hec_err counter rc=%d !", rc);
    }
    rxpon_hec_counter->sync_ponid_hec_err_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_bwmap_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read bwmap_hec_fix counter rc=%d !", rc);
    }
    rxpon_hec_counter->bwmap_hec_fix_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_hlend_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read hlend_hec_fix counter rc=%d !", rc);
    }
    rxpon_hec_counter->hlend_hec_fix_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_xhp_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read xhp_hec_fix counter rc=%d !", rc);
    }
    rxpon_hec_counter->xhp_hec_fix_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_sync_sfc_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read sync_sfc_hec_fix counter rc=%d !", rc);
    }
    rxpon_hec_counter->sync_sfc_hec_fix_counter = cnt;

    rc = ag_drv_ngpon_rx_pm_sync_ponid_hec_fix_get(&cnt);
    if (rc != PON_NO_ERROR)
    {
       p_log ( ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to read sync_ponid_hec_fix counter rc=%d !", rc);
    }
    rxpon_hec_counter->sync_ponid_hec_fix_counter = cnt;
    
    return rc;
}


PON_ERROR_DTE ngpon_tx_get_flush_valid_bit(uint8_t *tcont_id, bdmf_boolean *flush_enable,
                                           bdmf_boolean *flush_immediate, bdmf_boolean *flush_done)
{
    PON_ERROR_DTE drv_error;

    drv_error = ag_drv_ngpon_tx_cfg_tcont_flush_get(tcont_id, flush_enable, flush_immediate);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Failed to read flush parameters due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_ngpon_tx_status_tcont_flsh_done_get(flush_done);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Failed to read flush status due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE ngpon_get_pon_id_parameters (PON_ID_PARAMETERS *xo_pon_param)
{
    /*PON_ID_PARAMETERS pon_param;

    pon_param.pon_id_type = g_xgpon_db.onu_params.

    memcpy(xo_pon_param, &pon_param, sizeof(PON_ID_PARAMETERS));*/

    return PON_NO_ERROR;
}


#ifdef USE_BDMF_SHELL


extern PON_ERROR_DTE ngpon_modify_tcont (PON_TCONT_ID_DTE xi_tcont_id,
    PON_ALLOC_ID_DTE xi_new_alloc_id)
{
    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "\n\n ###### In ngpon_modify_tcont()\n");
    return PON_NO_ERROR;  
}
#endif

void ngpon_configure_omci_key (uint8_t *omci_ik_p)
{
  memcpy (g_xgpon_db.onu_params.omci_ik_in_bytes, omci_ik_p, NGPON_DEFAULT_KEY_SIZE);
}

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Check, whether the SFC_INDICATION is enabled - means that                */
/* somebody is expected to handle this interrupt                              */
/*                                                                            */
/******************************************************************************/
bdmf_boolean ngpon_is_sfc_indication_int_set ()
{
    PON_ERROR_DTE stack_error ;
    uint32_t ms_value;
    uint8_t  enable;
    uint8_t  holdover;

    

    stack_error = ag_drv_ngpon_rx_gen_sync_sfc_ind_get (&ms_value, &enable, &holdover) ;
    if (stack_error != PON_NO_ERROR)
    {
        return BDMF_FALSE ;
    }

    return enable ;
}

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Configure SYNC_SFC_IND Register and enable corresponding interrupt       */
/* somebody is expected to handle this interrupt                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_set_sfc_indication_int (uint32_t sfc_msb, uint32_t sfc_lsb)
{
    PON_ERROR_DTE stack_error ;
    uint32_t ms_value ;
    uint8_t  enable ;
    uint8_t  holdover ;


    /*
     *  Set 'enable' bit in SYNC_SFC_IND Register to 0
     */
    stack_error = ag_drv_ngpon_rx_gen_sync_sfc_ind_get (&ms_value, &enable, &holdover) ;
    if (stack_error != PON_NO_ERROR)
    {
        return PON_ERROR_DRIVER_ERROR ;
    }
    enable = 0 ; 
    stack_error = ag_drv_ngpon_rx_gen_sync_sfc_ind_set (ms_value, enable, holdover) ;
    if (stack_error != PON_NO_ERROR)
    {
        return PON_ERROR_DRIVER_ERROR ;
    }
    
    /*
     * Set MSB and LSB
     */
    stack_error = ag_drv_ngpon_rx_gen_sync_sfc_ind_ls_set (sfc_lsb) ;
    if (stack_error != PON_NO_ERROR)
    {
        return PON_ERROR_DRIVER_ERROR ;
    }


    enable = 0 ;
    holdover = 0;
    stack_error = ag_drv_ngpon_rx_gen_sync_sfc_ind_set (sfc_msb, enable, holdover) ;
    if (stack_error != PON_NO_ERROR)
    {
        return PON_ERROR_DRIVER_ERROR ;
    }
    

    enable = 1 ;
    holdover = 0;
    stack_error = ag_drv_ngpon_rx_gen_sync_sfc_ind_set (sfc_msb, enable, holdover) ;
    if (stack_error != PON_NO_ERROR)
    {
        return PON_ERROR_DRIVER_ERROR ;
    }

    /*
     * Enable interrupt
     */

    g_xgpon_db.rx_ier_vector.sfc_indication_mask = BDMF_TRUE ;

    stack_error = ag_drv_ngpon_rx_int_ier_set (&g_xgpon_db.rx_ier_vector) ;
    if ( stack_error != PON_NO_ERROR )
    {
        return ( stack_error ) ;
    }
     
    return PON_NO_ERROR ;
}

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   Configure SYNC_SFC_IND Register and enable corresponding interrupt       */
/* somebody is expected to handle this interrupt                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE ngpon_clear_sfc_indication_int ()
{
    PON_ERROR_DTE stack_error ;
    uint32_t ms_value ;
    uint8_t  enable ;
    uint8_t  holdover ;


    /*
     *  Set 'enable' bit in SYNC_SFC_IND Register to 0
     */
    stack_error = ag_drv_ngpon_rx_gen_sync_sfc_ind_get (&ms_value, &enable, &holdover) ;
    if (stack_error != PON_NO_ERROR)
    {
        return PON_ERROR_DRIVER_ERROR ;
    }
    enable = 0 ; 
    stack_error = ag_drv_ngpon_rx_gen_sync_sfc_ind_set (ms_value, enable, holdover) ;
    if (stack_error != PON_NO_ERROR)
    {
        return PON_ERROR_DRIVER_ERROR ;
    }
    /*
     * Disable interrupt
     */

    g_xgpon_db.rx_ier_vector.sfc_indication_mask = BDMF_FALSE ;

    stack_error = ag_drv_ngpon_rx_int_ier_set (&g_xgpon_db.rx_ier_vector) ;
    if ( stack_error != PON_NO_ERROR )
    {
        return ( stack_error ) ;
    }
     
    return PON_NO_ERROR ;

}
