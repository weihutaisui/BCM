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

#include "ru.h"
#include "GPON_BLOCKS.h"

#if RU_INCLUDE_FIELD_DB
/******************************************************************************
 * Field: GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_DBG_SEL
 ******************************************************************************/
const ru_field_rec GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_DBG_SEL_FIELD =
{
    "DBG_SEL",
#if RU_INCLUDE_DESC
    "Debug_Select",
    "selects a debug vector",
#endif
    GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_DBG_SEL_FIELD_MASK,
    0,
    GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_DBG_SEL_FIELD_WIDTH,
    GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_DBG_SEL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_RESERVED0_FIELD_WIDTH,
    GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_FIELDS[] =
{
    &GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_DBG_SEL_FIELD,
    &GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_REG = 
{
    "DBG_SEL",
#if RU_INCLUDE_DESC
    "GPON_DEBUG_BUS_SELECTOR Register",
    "Selects one of many debug vectors which the GPON block will output",
#endif
    GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_REG_OFFSET,
    0,
    0,
    147,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_GRX_DEBUG_DBG_SEL
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_GRX_DEBUG_DBG_SEL_REGS[] =
{
    &GPON_RX_GRX_DEBUG_DBG_SEL_DBG_SEL_REG,
};

static unsigned long GPON_RX_GRX_DEBUG_DBG_SEL_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80150b00,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db1b00,
#else
    #error "GPON_RX_GRX_DEBUG_DBG_SEL base address not defined"
#endif
};

const ru_block_rec GPON_RX_GRX_DEBUG_DBG_SEL_BLOCK = 
{
    "GPON_RX_GRX_DEBUG_DBG_SEL",
    GPON_RX_GRX_DEBUG_DBG_SEL_ADDRS,
    1,
    1,
    GPON_RX_GRX_DEBUG_DBG_SEL_REGS
};

/* End of file BCM6836_A0GPON_RX_GRX_DEBUG_DBG_SEL.c */
