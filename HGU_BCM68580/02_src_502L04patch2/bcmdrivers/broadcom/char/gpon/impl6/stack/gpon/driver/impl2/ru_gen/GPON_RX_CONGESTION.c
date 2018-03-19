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
#ifndef CONFIG_BCM96858
    0x130f9130,
#else
    0x80150130
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

/* End of file BCM6858_A0GPON_RX_CONGESTION.c */
