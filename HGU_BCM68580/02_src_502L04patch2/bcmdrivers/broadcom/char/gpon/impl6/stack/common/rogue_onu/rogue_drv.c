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
#include "bdmf_dev.h"
#include "rdpa_gpon.h"
#include "rogue_drv.h"
#include "boardparms.h"
#include "opticaldet.h"

ROGUE_ONU_DETECT_CALLBACK_FUNC gl_rogue_onu_detect_cb = {NULL, NULL, NULL} ;

/* Define the pin physical for print only */
#if defined(CONFIG_BCM96858) || defined(CONFIG_BCM96846)
#define ROGUE_ONU_PIN 56
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96856)
#define ROGUE_ONU_PIN 62
#else
#define ROGUE_ONU_PIN 51
#endif

static int cfg_rogue_hw_enable(rdpa_rogue_onu_t *cfg)
{
    PON_TX_ROGUE_ONU_PARAMETERS hw_rogue_param;
    unsigned short board_param_pin;
    int rogue_onu_diff=0;
    int rogue_onu_level=0;
    int rc;

    if (cfg->enable)
    {
        rc = (*gl_rogue_onu_detect_cb.get_rogue_onu_detection_params)(&hw_rogue_param);
        if (rc)
            return rc;

        hw_rogue_param.source_select = 0 ;  

        switch (cfg->mode)
        {
        case rdpa_rogue_tx_monitor_mode:
            /*write enable to the rogue diff interrupt*/
            rogue_onu_diff = 1;

            /* update the clock cycle */
            hw_rogue_param.rogue_onu_diff_threshold = cfg->clock_cycle;

            /*clear the diff counter*/
            hw_rogue_param.rogue_onu_diff_clear = 0;

            break;
        case rdpa_rogue_tx_fault_mode:
            /*write enable to the rogue level interrupt*/
            rogue_onu_level = 1;

            /* update the clock cycle */
            hw_rogue_param.rogue_onu_level_threshold = cfg->clock_cycle ;

            /*clear the level counter*/
            hw_rogue_param.rogue_onu_level_clear = 0 ;

            break;
        default:
            break;
        }

        bdmf_lock();
 
        /* Check if Rogue pin is configured in board params and write the value to object */
        rc = BpGetRogueOnuEn(&board_param_pin);
        if (rc)
            goto label_unlock_critical_section;
        else
            cfg->gpio_pin = ROGUE_ONU_PIN;

        rc = (*gl_rogue_onu_detect_cb.set_rogue_onu_detection_params)(&hw_rogue_param);
        if (rc)
            goto label_unlock_critical_section;

        rc = (*gl_rogue_onu_detect_cb.cfg_rogue_onu_interrupts)(ROGUE_ONU_TX_INT_UNMASK, rogue_onu_diff, 
           rogue_onu_level);
        if (rc)
            goto label_unlock_critical_section;

        /* Clear rogue status bit*/
        rc = (*gl_rogue_onu_detect_cb.cfg_rogue_onu_interrupts)(ROGUE_ONU_TX_INT_CLEAR, rogue_onu_diff, 
           rogue_onu_level);

        
        goto label_unlock_critical_section;
    }
    else
    {
        hw_rogue_param.source_select = 1 ;  

        if (cfg->mode == rdpa_rogue_tx_monitor_mode)
            rogue_onu_diff = 1 ;
        else
            rogue_onu_level = 1 ;

        hw_rogue_param.rogue_onu_diff_clear = rogue_onu_diff;
        hw_rogue_param.rogue_onu_level_clear = rogue_onu_level ;
        hw_rogue_param.rogue_onu_diff_threshold = cfg->clock_cycle;   /*  Reasonable default for 'disable' */
        hw_rogue_param.rogue_onu_level_threshold = cfg->clock_cycle;  /*  Reasonable default for 'disable' */

        bdmf_lock();

        rc = (*gl_rogue_onu_detect_cb.set_rogue_onu_detection_params)(&hw_rogue_param);
        if (rc)
            goto label_unlock_critical_section;

        /*mask onu diff/rogue level interrupt*/
        rc = (*gl_rogue_onu_detect_cb.cfg_rogue_onu_interrupts)(ROGUE_ONU_TX_INT_MASK, rogue_onu_diff, 
           rogue_onu_level);
        goto label_unlock_critical_section;
    }

label_unlock_critical_section:
    bdmf_unlock();
    return rc;
}


/* Generic rogue onu configuration func */
int rogue_onu_detect_cfg(void *cfg, void *prev_cfg)
{
   cfg_rogue_hw_enable((rdpa_rogue_onu_t *)cfg);
   return 0;
}

uint8_t gpon_ngpon_get_lbe_invert_bit_val(void)
{
#if defined(CONFIG_BCM96858)   
   TRX_SIG_ACTIVE_POLARITY lbe_polarity ;
   TRX_SIG_ACTIVE_POLARITY tx_sd_polarity ;
   int                     rc1 ;
   int                     rc2 ;
   uint8_t                 lbe_pol_val ;
   uint8_t                 tx_sd_pol_val ;
   int                     bus = -1;

   opticaldet_get_xpon_i2c_bus_num(&bus);
   rc1 = trx_get_lbe_polarity(bus, &lbe_polarity) ;
   rc2 = trx_get_tx_sd_polarity(bus, &tx_sd_polarity) ;

   lbe_pol_val = (lbe_polarity == TRX_ACTIVE_LOW)? 0 : 1 ;
   tx_sd_pol_val = (tx_sd_polarity == TRX_ACTIVE_LOW)? 0 : 1 ;

   if ((rc1 == 0) && (rc2 == 0))
   {
      return lbe_pol_val ^ tx_sd_pol_val ;
   }
   else
   {
     return (rc1 == 0)? lbe_pol_val : 0 ; 
   }
#else
   return -1 ;   /* Nobody supposed to use this function for 6848 */
#endif
}



