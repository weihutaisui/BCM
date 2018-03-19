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

unsigned long GPON_TX_FIFO_STATISTICS_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130fa800,
#else
    0x80148800,
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

/* End of file BCM6858_A0GPON_TX_FIFO_STATISTICS.c */
