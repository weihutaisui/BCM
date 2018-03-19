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

#ifndef _GPON_RX_GENERAL_CONFIG_AG_H_
#define _GPON_RX_GENERAL_CONFIG_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"


/**************************************************************************************************/
/* des_disable: Descrambler_Disable - Bypasses the descrambler so that the incoming data is passe */
/*              d as-is to the internal units.This field can be changed on-the-fly during operati */
/*              on.                                                                               */
/* fec_disable: FEC_Disable - Disables the FEC decoder even if DS FEC is enabled.This field  can  */
/*              be changed only while the receiver is disabled.                                   */
/* rx_disable: Receiver_Enable - Enables/Disables the entire receiver unit.This field  can be cha */
/*             nged on-the-fly during operation. Changes go into effect on frame boundaries.      */
/* loopback_enable: Loopback_Enable - Enables/Disables the RX-TX loopback.This field can be chang */
/*                  ed on-the-fly during operation.                                               */
/* fec_force: FEC_Force - Forces FEC decoding on DS regardless of the bit in the Ident field.This */
/*             field can be changed only while the receiver is disabled.                          */
/* fec_st_disc: FEC_State_Disconnect - Prevents the RX Unit from making the FEC transitions auton */
/*              omously.This field should not be used (always write 1 to this field).             */
/* squelch_dis: Squelch_Disable - Neutralizes the FEC State Transition fix.This field should not  */
/*              be used (always write 1 to this field).                                           */
/* sop_reset: SOP_Reset - Resets all flows to SOP (Start Of Packet) State. Should only be applied */
/*             when the module is in LOF or Rx_Disable states.This field should not be used (alwa */
/*            ys write 0 to this field).                                                          */
/* din_polarity: RX_data_in_polarity - This bit controls the polarity of the GPON RX data input   */
/**************************************************************************************************/
typedef struct
{
    bdmf_boolean des_disable;
    bdmf_boolean fec_disable;
    bdmf_boolean rx_disable;
    bdmf_boolean loopback_enable;
    bdmf_boolean fec_force;
    bdmf_boolean fec_st_disc;
    bdmf_boolean squelch_dis;
    bdmf_boolean sop_reset;
    bdmf_boolean din_polarity;
} gpon_rx_general_config_rcvr_config;

bdmf_error_t ag_drv_gpon_rx_general_config_rcvr_status_get(bdmf_boolean *lof, bdmf_boolean *fec_state, bdmf_boolean *lcdg_state, uint8_t *bit_align);
bdmf_error_t ag_drv_gpon_rx_general_config_rcvr_config_set(const gpon_rx_general_config_rcvr_config *rcvr_config);
bdmf_error_t ag_drv_gpon_rx_general_config_rcvr_config_get(gpon_rx_general_config_rcvr_config *rcvr_config);
bdmf_error_t ag_drv_gpon_rx_general_config_lof_params_set(uint8_t delta, uint8_t alpha);
bdmf_error_t ag_drv_gpon_rx_general_config_lof_params_get(uint8_t *delta, uint8_t *alpha);
bdmf_error_t ag_drv_gpon_rx_general_config_randomsd_get(uint32_t *randomsd);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_rx_general_config_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

