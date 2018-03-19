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

#ifndef _GPON_TX_GEN_CTRL_AG_H_
#define _GPON_TX_GEN_CTRL_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"


/**************************************************************************************************/
/* txen: Transmitter_enable - Enable the accesses transmission and tx fifo operation. If the flag */
/*        is deactivated during operation, then the  transmittor stops transmissions, flushes awa */
/*       iting accesses, and tx fifo queues. To ensure a correct gpon enabling sequence, tx unit  */
/*       must be enabled prior to rx unit.                                                        */
/* scren: Scrambler_enable - Enable the upstream scrambler operation. This register can be update */
/*        d only when tx is disabled.                                                             */
/* bipen: BIP8_enable - Enable BIP8 generation for upstream traffic. This register can be updated */
/*         only when tx is disabled.                                                              */
/* fecen: FEC_enable - Enable the upstream FEC encoder operation. This register can be updated on */
/*        ly when tx is disabled.                                                                 */
/* loopben: Loop_back_enable - Enable RX-TX loopback operation. This register can be updated only */
/*           when tx is disabled.                                                                 */
/* plsen: Power_level_sequence_enable - Enable transmitter append of PLS sequence to ranging tran */
/*        smissions. This register can be updated only when tx is enabled.                        */
/* tdmen: TDM_enable - Enable TDM port. This register can be updated only when tx is disabled.    */
/* misctxen: Misc_transmission_enable - Enable TX independent transmission according to access co */
/*           nfigured by mips. This register can be updated only when tx is disabled. For more in */
/*           formation regarding misc accesses, refer to registers MISCAC1 and MISCAC2            */
/* rngen: Ranging_enable - When this bit is set and an sn request/ranging access is received then */
/*         the ONU will transmit the ploam ranging buffer. This register can be updated when tx i */
/*        s enabled.                                                                              */
/**************************************************************************************************/
typedef struct
{
    bdmf_boolean txen;
    bdmf_boolean scren;
    bdmf_boolean bipen;
    bdmf_boolean fecen;
    bdmf_boolean loopben;
    bdmf_boolean plsen;
    bdmf_boolean tdmen;
    bdmf_boolean misctxen;
    bdmf_boolean rngen;
} gpon_tx_gen_ctrl_ten;

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdly_set(uint16_t bdly, uint8_t fdly, uint8_t sdly);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdly_get(uint16_t *bdly, uint8_t *fdly, uint8_t *sdly);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_ten_set(const gpon_tx_gen_ctrl_ten *ten);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_ten_get(gpon_tx_gen_ctrl_ten *ten);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_set(bdmf_boolean doutpol);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_get(bdmf_boolean *doutpol);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_nprep_set(uint8_t prep0, uint8_t prep1, uint8_t prep2, uint8_t prep3);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_nprep_get(uint8_t *prep0, uint8_t *prep1, uint8_t *prep2, uint8_t *prep3);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(uint8_t npvld, bdmf_boolean uplvd);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(uint8_t *npvld, bdmf_boolean *uplvd);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdbvld_set(uint32_t dbav);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdbvld_get(uint32_t *dbav);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdbconf_set(uint8_t dbav, bdmf_boolean dbr_gen_en, uint8_t divrate, bdmf_boolean dbflush);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdbconf_get(uint8_t *dbav, bdmf_boolean *dbr_gen_en, uint8_t *divrate, bdmf_boolean *dbflush);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tpclr_set(bdmf_boolean tpclrc);
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tpclr_get(bdmf_boolean *tpclrc);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_tx_gen_ctrl_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

