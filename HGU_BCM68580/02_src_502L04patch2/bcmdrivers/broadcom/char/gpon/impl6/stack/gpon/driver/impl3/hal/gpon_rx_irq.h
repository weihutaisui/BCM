/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.

    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.

:>
*/

#ifndef _GPON_RX_IRQ_AG_H_
#define _GPON_RX_IRQ_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"


/**************************************************************************************************/
/* rxfifo: Rx_FIFO_Full_IRQ - Indicates that the Rx FIFO is full.                                 */
/* tdmfifo: TDM_FIFO_Full_IRQ - Indicates that the TDM FIFO is full                               */
/* aes: AES_Error_IRQ - Indicates that no AES machines could be allocated to decrypt a fragment   */
/* accfifo: Access_FIFO_Full_IRQ - Indicates that the Access FIFO is full                         */
/* lofchng: LOF_state_change_IRQ - Indicates that the LOF alarm was asserted/de-asserted          */
/* lcdgchng: LCDG_state_change_IRQ - Indicates that the LCDG alarm was asserted/de-asserted       */
/* fecchng: FEC_State_change_IRQ - Indicates that FEC decoding was activated/de-activated         */
/* tod: TOD_update_IRQ - An update of the TOD counters occured                                    */
/* fwi: FWI_state_change - forced_wakeup_indication state change                                  */
/**************************************************************************************************/
typedef struct
{
    bdmf_boolean rxfifo;
    bdmf_boolean tdmfifo;
    bdmf_boolean aes;
    bdmf_boolean accfifo;
    bdmf_boolean lofchng;
    bdmf_boolean lcdgchng;
    bdmf_boolean fecchng;
    bdmf_boolean tod;
    bdmf_boolean fwi;
} gpon_rx_irq_grxisr;


/**************************************************************************************************/
/* rxfifo: Rx_FIFO_Full_IRQ_mask - Controls IRQ assertion due to Rx FIFO Full                     */
/* tdmfifo: TDM_FIFO_Full_IRQ_mask - Controls IRQ assertion due to TDM FIFO Full                  */
/* aes: AES_Error_IRQ_mask - Controls IRQ assertion due to AES allocation error                   */
/* accfifo: Access_FIFO_Full_IRQ_mask - Controls IRQ assertion due to Access FIFO Full errors     */
/* lofchng: LOF_change_IRQ_mask - Controls IRQ assertion due to LOF change                        */
/* lcdgchng: LCDG_change_IRQ_mask - Controls IRQ assertion due to LCDG change                     */
/* fecchng: FEC_state_change_IRQ_mask - Controls IRQ assertion due to FEC state change            */
/* tod: TOD_update_IRQ_mask - Controls IRQ assertion due to TOD update                            */
/* fwi: FWI_change_IRQ_mask - Controls IRQ assertion due to FWI change                            */
/**************************************************************************************************/
typedef struct
{
    bdmf_boolean rxfifo;
    bdmf_boolean tdmfifo;
    bdmf_boolean aes;
    bdmf_boolean accfifo;
    bdmf_boolean lofchng;
    bdmf_boolean lcdgchng;
    bdmf_boolean fecchng;
    bdmf_boolean tod;
    bdmf_boolean fwi;
} gpon_rx_irq_grxier;

bdmf_error_t ag_drv_gpon_rx_irq_grxisr_set(const gpon_rx_irq_grxisr *grxisr);
bdmf_error_t ag_drv_gpon_rx_irq_grxisr_get(gpon_rx_irq_grxisr *grxisr);
bdmf_error_t ag_drv_gpon_rx_irq_grxier_set(const gpon_rx_irq_grxier *grxier);
bdmf_error_t ag_drv_gpon_rx_irq_grxier_get(gpon_rx_irq_grxier *grxier);
bdmf_error_t ag_drv_gpon_rx_irq_grxitr_set(uint16_t ist);
bdmf_error_t ag_drv_gpon_rx_irq_grxitr_get(uint16_t *ist);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_rx_irq_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

