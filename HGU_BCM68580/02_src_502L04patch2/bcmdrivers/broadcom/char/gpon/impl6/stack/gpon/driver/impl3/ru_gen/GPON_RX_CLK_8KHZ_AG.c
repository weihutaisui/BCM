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
 * Field: GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF
 ******************************************************************************/
const ru_field_rec GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD =
{
    "PHSDIFF",
#if RU_INCLUDE_DESC
    "Phase_difference",
    "The desired phase difference between the free-running 8kHz output clock, and the incoming frame stream. When phase difference is 0, the rising edge of the clock will be synced to the frame start.",
#endif
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD_MASK,
    0,
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD_WIDTH,
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS
 ******************************************************************************/
const ru_field_rec GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD =
{
    "PHSLOCKDIS",
#if RU_INCLUDE_DESC
    "Phase_lock_mechanism_disable",
    "When asserted, the phase lock mechanism is disabled, and the 8kHz clock will run regardless of the incoming frames state.",
#endif
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD_MASK,
    0,
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD_WIDTH,
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD_WIDTH,
    GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_CLK_8KHZ_CLK8KCONF
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_CLK_8KHZ_CLK8KCONF_FIELDS[] =
{
    &GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD,
    &GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD,
    &GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_CLK_8KHZ_CLK8KCONF_REG = 
{
    "CLK8KCONF",
#if RU_INCLUDE_DESC
    "CLOCK_8KHZ_PHASE_LOCK_MECHANISM Register",
    "This register provide control over the 8KHz clock phase lock mechanism."
    "It enables/disables the phase lock mechanism and controls the desired phase difference between the output free-running 8KHz clock and the input frames stream."
    "When the desired phase difference is 0, the rising edge of the output 8KHz clock will be synced to the frame start."
    "The desired phase difference is in resolution of GPON clock cycles."
    "If one will configure the phase difference to be 1, the rising edge of the clock will be one GPON clock cycle before the frame start."
    "",
#endif
    GPON_RX_CLK_8KHZ_CLK8KCONF_REG_OFFSET,
    0,
    0,
    102,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_RX_CLK_8KHZ_CLK8KCONF_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_CLK_8KHZ
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_CLK_8KHZ_REGS[] =
{
    &GPON_RX_CLK_8KHZ_CLK8KCONF_REG,
};

unsigned long GPON_RX_CLK_8KHZ_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80150120,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db1120,
#else
    #error "GPON_RX_CLK_8KHZ base address not defined"
#endif
};

const ru_block_rec GPON_RX_CLK_8KHZ_BLOCK = 
{
    "GPON_RX_CLK_8KHZ",
    GPON_RX_CLK_8KHZ_ADDRS,
    1,
    1,
    GPON_RX_CLK_8KHZ_REGS
};

/* End of file BCM6836_A0GPON_RX_CLK_8KHZ.c */
