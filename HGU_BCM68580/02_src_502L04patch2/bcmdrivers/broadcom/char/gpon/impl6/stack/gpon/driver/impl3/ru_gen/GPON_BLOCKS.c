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

#include "GPON_BLOCKS.h"

/******************************************************************************
 * Chip: BCM6836_A0
 ******************************************************************************/
const ru_block_rec *RU_GPON_BLOCKS[] =
{
    &GPON_TX_GEN_CTRL_BLOCK,
    &GPON_TX_INTERRUPT_CONTROLLER_BLOCK,
    &GPON_TX_GENERAL_CONFIGURATION_BLOCK,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_BLOCK,
    &GPON_TX_FIFO_CONFIGURATION_BLOCK,
    &GPON_TX_FIFO_STATISTICS_BLOCK,
    &GPON_TX_STATISTICS_COUNTERS_BLOCK,
    &GPON_RX_GENERAL_CONFIG_BLOCK,
    &GPON_RX_IRQ_BLOCK,
    &GPON_RX_PLOAM_BLOCK,
    &GPON_RX_ALLOC_ID_BLOCK,
    &GPON_RX_CLK_8KHZ_BLOCK,
    &GPON_RX_CONGESTION_BLOCK,
    &GPON_RX_PORT_ID_BLOCK,
    &GPON_RX_ENCRYPTION_BLOCK,
    &GPON_RX_AMD2_BLOCK,
    &GPON_RX_PM_COUNTER_BLOCK,
    &GPON_RX_BWMAP_RECORD_BLOCK,
    &GPON_RX_SHORT_FRAME_BLOCK,
    &GPON_RX_GRX_DEBUG_DBG_SEL_BLOCK,
    NULL
};

/* End of file BCM6836_A0.c */
