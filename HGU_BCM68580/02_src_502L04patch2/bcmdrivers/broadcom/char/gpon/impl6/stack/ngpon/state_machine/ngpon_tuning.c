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
#include "opticaldet.h"

/* driver */
#include "ngpon_db.h"
#include "aes_cmac.h"
#include "pon_sm_common.h"

#if defined(USE_LOGGER)
#include "gpon_logger.h"
#endif

#include "bcmsfp_i2c.h"


/******************************************************************************/
/*                                                                            */
/* Types and values definitions                                               */
/*                                                                            */
/******************************************************************************/
#define TRX_TX_CHN_CFG    0x70
#define TRX_RX_CHN_CFG    0x71
#define TRX_PASS_BASE     0x7B

static uint8_t   max_chan_num = 4 ;

/******************************************************************************/
/*                                                                            */
/* Functions Definitions                                                      */
/*                                                                            */
/******************************************************************************/

static PON_ERROR_DTE ngpon_tune_channel (int channel, int direction)
{
  uint8_t current_channel ;
  int rc ;
  uint8_t channel_to_set = channel ;
  int bus = -1;

  if ((channel < -1) || (channel >= max_chan_num))
  {
     p_log(ge_onu_logger.sections.stack.debug.general_id, "illegal channel %d", channel);
     return PON_ERROR_INVALID_PARAMETER ;
  }

  if ((direction != TRX_RX_CHN_CFG) && (direction != TRX_TX_CHN_CFG))
  {
     p_log(ge_onu_logger.sections.stack.debug.general_id, "illegal direction %d", direction);
     return PON_ERROR_INVALID_PARAMETER ;
  }

  opticaldet_get_xpon_i2c_bus_num(&bus);
  if (channel == -1) /* select next channel */
  {
    /*
       read current channel from transciever
    */
     rc = bcmsfp_read_byte(bus, 0, direction, &current_channel);
     channel_to_set = (current_channel + 1) % max_chan_num ;     /* channels are numbered in TRX 0..max_channel-1 */
     p_log(ge_onu_logger.sections.stack.pon_task.general_id, "-----> Tuning: current channel %d tuning to %d", current_channel, channel_to_set);
  } 

  rc = bcmsfp_write_byte(bus, 0, direction, channel_to_set);
  if (rc < 0)
  {
       p_log(ge_onu_logger.sections.stack.debug.general_id, "failed to select channel %d rc=%d", channel, rc);
       return PON_ERROR_INVALID_PARAMETER ;
  }
 
   return PON_NO_ERROR;
}



uint8_t ngpon_tune_get_current_rx_channel (void)
{
    uint8_t val;
    int  bus = -1;
    opticaldet_get_xpon_i2c_bus_num(&bus);
    bcmsfp_read_byte(bus, 0, TRX_RX_CHN_CFG, &val);
    return val+1;
}

uint8_t ngpon_tune_get_current_tx_channel (void)
{
    uint8_t val;
    int  bus = -1;
 
    opticaldet_get_xpon_i2c_bus_num(&bus);
    bcmsfp_read_byte(bus, 0, TRX_TX_CHN_CFG, &val);
    return val+1;
}


/*
 * Tune Rx  channel at NGPON2 tunable TRX
 * (envelope over ngpon_tune_channel() )
 *
 * 'channel' can be:
 * 0      -- select next channel
 * 1..4   -- select required channel
 *
 */
PON_ERROR_DTE ngpon_tune_rx (int rx_channel)
{
  int rx_channel_for_trx = rx_channel - 1 ;  /* TRX uses chn numbering 0..3, -1 is indication to go to next chn */
  return ngpon_tune_channel (rx_channel_for_trx, TRX_RX_CHN_CFG) ;
}


/*
 * Tune Tx  channel at NGPON2 tunable TRX
 * (envelope over ngpon_tune_channel() )
 *
 * 'channel' can be:
 * 0      -- select next channel
 * 1..4   -- select required channel
 *
 */
PON_ERROR_DTE ngpon_tune_tx (int tx_channel)
{
  int tx_channel_for_trx = tx_channel - 1 ;  /* TRX uses chn numbering 0..3, -1 is indication to set next chn */
  return ngpon_tune_channel (tx_channel_for_trx, TRX_TX_CHN_CFG) ;
}

void ngpon_tune_set_max_chn (uint8_t max_chn)
{
  max_chan_num = max_chn ;
}



