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
 * Field: GPON_RX_CONGESTION_THRESH_INTO
 ******************************************************************************/
const ru_field_rec GPON_RX_CONGESTION_THRESH_INTO_FIELD =
{
    "INTO",
#if RU_INCLUDE_DESC
    "into_congestion",
    "Threshold for moving into congestion."
    ""
    "when crossed, a congestion signal will be raised towards the BBH RX",
#endif
    GPON_RX_CONGESTION_THRESH_INTO_FIELD_MASK,
    0,
    GPON_RX_CONGESTION_THRESH_INTO_FIELD_WIDTH,
    GPON_RX_CONGESTION_THRESH_INTO_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_CONGESTION_THRESH_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_CONGESTION_THRESH_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_CONGESTION_THRESH_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_CONGESTION_THRESH_RESERVED0_FIELD_WIDTH,
    GPON_RX_CONGESTION_THRESH_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_CONGESTION_THRESH_OUTOF
 ******************************************************************************/
const ru_field_rec GPON_RX_CONGESTION_THRESH_OUTOF_FIELD =
{
    "OUTOF",
#if RU_INCLUDE_DESC
    "out_of_congestion",
    "Threshold for moving out of congestion."
    ""
    "when crossed from above, the congestion signal towards the BBH RX will be de asserted",
#endif
    GPON_RX_CONGESTION_THRESH_OUTOF_FIELD_MASK,
    0,
    GPON_RX_CONGESTION_THRESH_OUTOF_FIELD_WIDTH,
    GPON_RX_CONGESTION_THRESH_OUTOF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_CONGESTION_THRESH_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_RX_CONGESTION_THRESH_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_CONGESTION_THRESH_RESERVED1_FIELD_MASK,
    0,
    GPON_RX_CONGESTION_THRESH_RESERVED1_FIELD_WIDTH,
    GPON_RX_CONGESTION_THRESH_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_CONGESTION_THRESH
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_CONGESTION_THRESH_FIELDS[] =
{
    &GPON_RX_CONGESTION_THRESH_INTO_FIELD,
    &GPON_RX_CONGESTION_THRESH_RESERVED0_FIELD,
    &GPON_RX_CONGESTION_THRESH_OUTOF_FIELD,
    &GPON_RX_CONGESTION_THRESH_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_CONGESTION_THRESH_REG = 
{
    "THRESH",
#if RU_INCLUDE_DESC
    "CONGESTION_THRESHOLDS Register",
    "into congestion and out of congestion thresholds."
    ""
    "The thresholds are in number of RX fifo entries."
    ""
    "Possible values: 0 - 63 decimal"
    ""
    "(each entry in the FIFO is of 8 bytes)",
#endif
    GPON_RX_CONGESTION_THRESH_REG_OFFSET,
    0,
    0,
    103,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_RX_CONGESTION_THRESH_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_CONGESTION
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_CONGESTION_REGS[] =
{
    &GPON_RX_CONGESTION_THRESH_REG,
};

unsigned long GPON_RX_CONGESTION_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80150130,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db1130,
#else
    #error "GPON_RX_CONGESTION base address not defined"
#endif
};

const ru_block_rec GPON_RX_CONGESTION_BLOCK = 
{
    "GPON_RX_CONGESTION",
    GPON_RX_CONGESTION_ADDRS,
    1,
    1,
    GPON_RX_CONGESTION_REGS
};

/* End of file BCM6836_A0GPON_RX_CONGESTION.c */
