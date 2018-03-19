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
 * Field: GPON_RX_SHORT_FRAME_SHORT0_CWFORIF
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT0_CWFORIF_FIELD =
{
    "CWFORIF",
#if RU_INCLUDE_DESC
    "cw_for_if",
    "num of codewords for if counter module."
    "number of full codewords of 255 bytes -1",
#endif
    GPON_RX_SHORT_FRAME_SHORT0_CWFORIF_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT0_CWFORIF_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT0_CWFORIF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT0_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT0_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_SHORT_FRAME_SHORT0_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT0_RESERVED0_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT0_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT0_CYCINFR
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT0_CYCINFR_FIELD =
{
    "CYCINFR",
#if RU_INCLUDE_DESC
    "cycles_in_frame",
    "num of cycles in the chort frame -1",
#endif
    GPON_RX_SHORT_FRAME_SHORT0_CYCINFR_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT0_CYCINFR_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT0_CYCINFR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT0_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT0_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_SHORT_FRAME_SHORT0_RESERVED1_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT0_RESERVED1_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT0_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT1_ZPCR0
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT1_ZPCR0_FIELD =
{
    "ZPCR0",
#if RU_INCLUDE_DESC
    "zeropad_clock_rate0",
    "zeropad_clock_rate0 - dont change",
#endif
    GPON_RX_SHORT_FRAME_SHORT1_ZPCR0_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT1_ZPCR0_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT1_ZPCR0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT1_ZPCR1
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT1_ZPCR1_FIELD =
{
    "ZPCR1",
#if RU_INCLUDE_DESC
    "zeropad_clock_rate1",
    "zeropad_clock_rate1 - number of zero pad bytes in last codeword - 1",
#endif
    GPON_RX_SHORT_FRAME_SHORT1_ZPCR1_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT1_ZPCR1_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT1_ZPCR1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT1_CWINFR
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT1_CWINFR_FIELD =
{
    "CWINFR",
#if RU_INCLUDE_DESC
    "cw_in_frame",
    "num of fuill codewords in the chort frame",
#endif
    GPON_RX_SHORT_FRAME_SHORT1_CWINFR_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT1_CWINFR_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT1_CWINFR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT1_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT1_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_SHORT_FRAME_SHORT1_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT1_RESERVED0_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT1_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT2_BYTESCW1
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT2_BYTESCW1_FIELD =
{
    "BYTESCW1",
#if RU_INCLUDE_DESC
    "bytes_in_last_cw_clk_rate1",
    "bytes in last cw clock_rate1 - number of bytes in last codeword -1",
#endif
    GPON_RX_SHORT_FRAME_SHORT2_BYTESCW1_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT2_BYTESCW1_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT2_BYTESCW1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT2_BYTESCW0
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT2_BYTESCW0_FIELD =
{
    "BYTESCW0",
#if RU_INCLUDE_DESC
    "bytes_in_last_cw_clk_rate0",
    "bytes in last cw clock_rate0 - dont change",
#endif
    GPON_RX_SHORT_FRAME_SHORT2_BYTESCW0_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT2_BYTESCW0_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT2_BYTESCW0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_SHORT_FRAME_SHORT2_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_SHORT_FRAME_SHORT2_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_SHORT_FRAME_SHORT2_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_SHORT_FRAME_SHORT2_RESERVED0_FIELD_WIDTH,
    GPON_RX_SHORT_FRAME_SHORT2_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_SHORT_FRAME_SHORT0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_SHORT_FRAME_SHORT0_FIELDS[] =
{
    &GPON_RX_SHORT_FRAME_SHORT0_CWFORIF_FIELD,
    &GPON_RX_SHORT_FRAME_SHORT0_RESERVED0_FIELD,
    &GPON_RX_SHORT_FRAME_SHORT0_CYCINFR_FIELD,
    &GPON_RX_SHORT_FRAME_SHORT0_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_SHORT_FRAME_SHORT0_REG = 
{
    "SHORT0",
#if RU_INCLUDE_DESC
    "SHORT_FRAME_0 Register",
    "short frame config",
#endif
    GPON_RX_SHORT_FRAME_SHORT0_REG_OFFSET,
    0,
    0,
    144,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_RX_SHORT_FRAME_SHORT0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_SHORT_FRAME_SHORT1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_SHORT_FRAME_SHORT1_FIELDS[] =
{
    &GPON_RX_SHORT_FRAME_SHORT1_ZPCR0_FIELD,
    &GPON_RX_SHORT_FRAME_SHORT1_ZPCR1_FIELD,
    &GPON_RX_SHORT_FRAME_SHORT1_CWINFR_FIELD,
    &GPON_RX_SHORT_FRAME_SHORT1_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_SHORT_FRAME_SHORT1_REG = 
{
    "SHORT1",
#if RU_INCLUDE_DESC
    "SHORT_FRAME_1 Register",
    "short frame config",
#endif
    GPON_RX_SHORT_FRAME_SHORT1_REG_OFFSET,
    0,
    0,
    145,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_RX_SHORT_FRAME_SHORT1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_SHORT_FRAME_SHORT2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_SHORT_FRAME_SHORT2_FIELDS[] =
{
    &GPON_RX_SHORT_FRAME_SHORT2_BYTESCW1_FIELD,
    &GPON_RX_SHORT_FRAME_SHORT2_BYTESCW0_FIELD,
    &GPON_RX_SHORT_FRAME_SHORT2_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_SHORT_FRAME_SHORT2_REG = 
{
    "SHORT2",
#if RU_INCLUDE_DESC
    "SHORT_FRAME_2 Register",
    "short frame config",
#endif
    GPON_RX_SHORT_FRAME_SHORT2_REG_OFFSET,
    0,
    0,
    146,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_RX_SHORT_FRAME_SHORT2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_SHORT_FRAME
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_SHORT_FRAME_REGS[] =
{
    &GPON_RX_SHORT_FRAME_SHORT0_REG,
    &GPON_RX_SHORT_FRAME_SHORT1_REG,
    &GPON_RX_SHORT_FRAME_SHORT2_REG,
};

static unsigned long GPON_RX_SHORT_FRAME_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80150aa0,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db1aa0,
#else
    #error "GPON_RX_SHORT_FRAME base address not defined"
#endif
};

const ru_block_rec GPON_RX_SHORT_FRAME_BLOCK = 
{
    "GPON_RX_SHORT_FRAME",
    GPON_RX_SHORT_FRAME_ADDRS,
    1,
    3,
    GPON_RX_SHORT_FRAME_REGS
};

/* End of file BCM6836_A0GPON_RX_SHORT_FRAME.c */
