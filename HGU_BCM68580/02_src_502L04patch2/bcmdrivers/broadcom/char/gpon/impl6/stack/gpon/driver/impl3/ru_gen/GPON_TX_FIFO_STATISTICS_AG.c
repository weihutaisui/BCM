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
 * Field: GPON_TX_FIFO_STATISTICS_TQCNTR_TQCNTC
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_STATISTICS_TQCNTR_TQCNTC_FIELD =
{
    "TQCNTC",
#if RU_INCLUDE_DESC
    "TX_queue_counter_content",
    "Number of valid bytes in the Tx queue",
#endif
    GPON_TX_FIFO_STATISTICS_TQCNTR_TQCNTC_FIELD_MASK,
    0,
    GPON_TX_FIFO_STATISTICS_TQCNTR_TQCNTC_FIELD_WIDTH,
    GPON_TX_FIFO_STATISTICS_TQCNTR_TQCNTC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_STATISTICS_TQCNTR_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_STATISTICS_TQCNTR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_FIFO_STATISTICS_TQCNTR_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_FIFO_STATISTICS_TQCNTR_RESERVED0_FIELD_WIDTH,
    GPON_TX_FIFO_STATISTICS_TQCNTR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_STATISTICS_TQPDCNTR_TQPDCNTC
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_STATISTICS_TQPDCNTR_TQPDCNTC_FIELD =
{
    "TQPDCNTC",
#if RU_INCLUDE_DESC
    "TX_PD_queue_counter_content",
    "Number of valid bytes in the Tx queue",
#endif
    GPON_TX_FIFO_STATISTICS_TQPDCNTR_TQPDCNTC_FIELD_MASK,
    0,
    GPON_TX_FIFO_STATISTICS_TQPDCNTR_TQPDCNTC_FIELD_WIDTH,
    GPON_TX_FIFO_STATISTICS_TQPDCNTR_TQPDCNTC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_STATISTICS_TQPDCNTR_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_STATISTICS_TQPDCNTR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_FIFO_STATISTICS_TQPDCNTR_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_FIFO_STATISTICS_TQPDCNTR_RESERVED0_FIELD_WIDTH,
    GPON_TX_FIFO_STATISTICS_TQPDCNTR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_TX_FIFO_STATISTICS_TQCNTR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_FIFO_STATISTICS_TQCNTR_FIELDS[] =
{
    &GPON_TX_FIFO_STATISTICS_TQCNTR_TQCNTC_FIELD,
    &GPON_TX_FIFO_STATISTICS_TQCNTR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_FIFO_STATISTICS_TQCNTR_REG = 
{
    "TQCNTR",
#if RU_INCLUDE_DESC
    "TX_QUEUE_COUNTERS %i Register",
    "This register array details the number of valid bytes in each of the forty Tx queues",
#endif
    GPON_TX_FIFO_STATISTICS_TQCNTR_REG_OFFSET,
    GPON_TX_FIFO_STATISTICS_TQCNTR_REG_RAM_CNT,
    4,
    76,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_FIFO_STATISTICS_TQCNTR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_FIFO_STATISTICS_TQPDCNTR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_FIFO_STATISTICS_TQPDCNTR_FIELDS[] =
{
    &GPON_TX_FIFO_STATISTICS_TQPDCNTR_TQPDCNTC_FIELD,
    &GPON_TX_FIFO_STATISTICS_TQPDCNTR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_FIFO_STATISTICS_TQPDCNTR_REG = 
{
    "TQPDCNTR",
#if RU_INCLUDE_DESC
    "TX_PD_QUEUE_COUNTERS %i Register",
    "This register array details the number of valid PDs in each of the forty Tx queues",
#endif
    GPON_TX_FIFO_STATISTICS_TQPDCNTR_REG_OFFSET,
    GPON_TX_FIFO_STATISTICS_TQPDCNTR_REG_RAM_CNT,
    4,
    77,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_FIFO_STATISTICS_TQPDCNTR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_TX_FIFO_STATISTICS
 ******************************************************************************/
static const ru_reg_rec *GPON_TX_FIFO_STATISTICS_REGS[] =
{
    &GPON_TX_FIFO_STATISTICS_TQCNTR_REG,
    &GPON_TX_FIFO_STATISTICS_TQPDCNTR_REG,
};

static unsigned long GPON_TX_FIFO_STATISTICS_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80148800,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db0800,
#else
    #error "GPON_TX_FIFO_STATISTICS base address not defined"
#endif
};

const ru_block_rec GPON_TX_FIFO_STATISTICS_BLOCK = 
{
    "GPON_TX_FIFO_STATISTICS",
    GPON_TX_FIFO_STATISTICS_ADDRS,
    1,
    2,
    GPON_TX_FIFO_STATISTICS_REGS
};

/* End of file BCM6836_A0GPON_TX_FIFO_STATISTICS.c */
