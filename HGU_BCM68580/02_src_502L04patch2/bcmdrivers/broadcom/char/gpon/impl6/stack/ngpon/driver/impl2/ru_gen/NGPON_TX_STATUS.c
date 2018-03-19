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
#include "NGPON_BLOCKS.h"

#if RU_INCLUDE_FIELD_DB
/******************************************************************************
 * Field: NGPON_TX_STATUS_TCONT_FLSH_DONE_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_STATUS_TCONT_FLSH_DONE_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "flush done indication",
#endif
    NGPON_TX_STATUS_TCONT_FLSH_DONE_VALUE_FIELD_MASK,
    0,
    NGPON_TX_STATUS_TCONT_FLSH_DONE_VALUE_FIELD_WIDTH,
    NGPON_TX_STATUS_TCONT_FLSH_DONE_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_STATUS_TCONT_FLSH_DONE_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_STATUS_TCONT_FLSH_DONE_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_STATUS_TCONT_FLSH_DONE_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_STATUS_TCONT_FLSH_DONE_RESERVED0_FIELD_WIDTH,
    NGPON_TX_STATUS_TCONT_FLSH_DONE_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_STATUS_TX_Q_AVAIL_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_STATUS_TX_Q_AVAIL_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Number of valid entries (words) in the Tx queue. The last entry of a packet may be partial (not all bytes are valid)",
#endif
    NGPON_TX_STATUS_TX_Q_AVAIL_VALUE_FIELD_MASK,
    0,
    NGPON_TX_STATUS_TX_Q_AVAIL_VALUE_FIELD_WIDTH,
    NGPON_TX_STATUS_TX_Q_AVAIL_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_STATUS_TX_Q_AVAIL_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_STATUS_TX_Q_AVAIL_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_STATUS_TX_Q_AVAIL_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_STATUS_TX_Q_AVAIL_RESERVED0_FIELD_WIDTH,
    NGPON_TX_STATUS_TX_Q_AVAIL_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_STATUS_TX_PD_AVAIL_TQPDCNTC
 ******************************************************************************/
const ru_field_rec NGPON_TX_STATUS_TX_PD_AVAIL_TQPDCNTC_FIELD =
{
    "TQPDCNTC",
#if RU_INCLUDE_DESC
    "TX_PD_queue_counter_content",
    "Number of valid entries in the packet descrioptor queue queue",
#endif
    NGPON_TX_STATUS_TX_PD_AVAIL_TQPDCNTC_FIELD_MASK,
    0,
    NGPON_TX_STATUS_TX_PD_AVAIL_TQPDCNTC_FIELD_WIDTH,
    NGPON_TX_STATUS_TX_PD_AVAIL_TQPDCNTC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_STATUS_TX_PD_AVAIL_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_STATUS_TX_PD_AVAIL_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_STATUS_TX_PD_AVAIL_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_STATUS_TX_PD_AVAIL_RESERVED0_FIELD_WIDTH,
    NGPON_TX_STATUS_TX_PD_AVAIL_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_TX_STATUS_TCONT_FLSH_DONE
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_STATUS_TCONT_FLSH_DONE_FIELDS[] =
{
    &NGPON_TX_STATUS_TCONT_FLSH_DONE_VALUE_FIELD,
    &NGPON_TX_STATUS_TCONT_FLSH_DONE_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_STATUS_TCONT_FLSH_DONE_REG = 
{
    "TCONT_FLSH_DONE",
#if RU_INCLUDE_DESC
    "TCONT_FLUSH_DONE Register",
    "HW indication that the flush was done. In order to initiate the flush. When the flush is done (immediately or gracefully) the HW asserts the flush done bit. This bit will stay asserted until the SW de-assert flush valid. In this time, between the assertion of flush done and de-assertion of flush valid, the flush continues, meaning new data for the flushed queue will be disregarded, and IDLE GEMs will be transmitted if the flushed queue will receive accesses.",
#endif
    NGPON_TX_STATUS_TCONT_FLSH_DONE_REG_OFFSET,
    0,
    0,
    214,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_STATUS_TCONT_FLSH_DONE_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_STATUS_TX_Q_AVAIL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_STATUS_TX_Q_AVAIL_FIELDS[] =
{
    &NGPON_TX_STATUS_TX_Q_AVAIL_VALUE_FIELD,
    &NGPON_TX_STATUS_TX_Q_AVAIL_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_STATUS_TX_Q_AVAIL_REG = 
{
    "TX_Q_AVAIL",
#if RU_INCLUDE_DESC
    "TX_QUEUE_AVAILABLE %i Register",
    "This register array details the number of available valid entries in each of the forty TX queues",
#endif
    NGPON_TX_STATUS_TX_Q_AVAIL_REG_OFFSET,
    NGPON_TX_STATUS_TX_Q_AVAIL_REG_RAM_CNT,
    4,
    215,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_STATUS_TX_Q_AVAIL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_STATUS_TX_PD_AVAIL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_STATUS_TX_PD_AVAIL_FIELDS[] =
{
    &NGPON_TX_STATUS_TX_PD_AVAIL_TQPDCNTC_FIELD,
    &NGPON_TX_STATUS_TX_PD_AVAIL_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_STATUS_TX_PD_AVAIL_REG = 
{
    "TX_PD_AVAIL",
#if RU_INCLUDE_DESC
    "TX_PD_QUEUE_AVAILABLE %i Register",
    "This register array details the number of valid PDs in each of the forty Tx queues",
#endif
    NGPON_TX_STATUS_TX_PD_AVAIL_REG_OFFSET,
    NGPON_TX_STATUS_TX_PD_AVAIL_REG_RAM_CNT,
    4,
    216,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_STATUS_TX_PD_AVAIL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_TX_STATUS
 ******************************************************************************/
static const ru_reg_rec *NGPON_TX_STATUS_REGS[] =
{
    &NGPON_TX_STATUS_TCONT_FLSH_DONE_REG,
    &NGPON_TX_STATUS_TX_Q_AVAIL_REG,
    &NGPON_TX_STATUS_TX_PD_AVAIL_REG,
};

unsigned long NGPON_TX_STATUS_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x8016a00c,
#elif defined(CONFIG_BCM96856)
    0x82dba00c,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_TX_STATUS_BLOCK = 
{
    "NGPON_TX_STATUS",
    NGPON_TX_STATUS_ADDRS,
    1,
    3,
    NGPON_TX_STATUS_REGS
};

/* End of file BCM6858_A0_NGPON_TX_STATUS.c */
