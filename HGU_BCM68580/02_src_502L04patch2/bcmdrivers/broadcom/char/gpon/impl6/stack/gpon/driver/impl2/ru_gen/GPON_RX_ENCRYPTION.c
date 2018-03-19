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
 * Field: GPON_RX_ENCRYPTION_SF_CNTR_SF_CNTR
 ******************************************************************************/
const ru_field_rec GPON_RX_ENCRYPTION_SF_CNTR_SF_CNTR_FIELD =
{
    "SF_CNTR",
#if RU_INCLUDE_DESC
    "Superframe_Counter",
    "Shows the value of the superframe counter",
#endif
    GPON_RX_ENCRYPTION_SF_CNTR_SF_CNTR_FIELD_MASK,
    0,
    GPON_RX_ENCRYPTION_SF_CNTR_SF_CNTR_FIELD_WIDTH,
    GPON_RX_ENCRYPTION_SF_CNTR_SF_CNTR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_ENCRYPTION_SF_CNTR_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_ENCRYPTION_SF_CNTR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ENCRYPTION_SF_CNTR_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_ENCRYPTION_SF_CNTR_RESERVED0_FIELD_WIDTH,
    GPON_RX_ENCRYPTION_SF_CNTR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_ENCRYPTION_KEY_KEY
 ******************************************************************************/
const ru_field_rec GPON_RX_ENCRYPTION_KEY_KEY_FIELD =
{
    "KEY",
#if RU_INCLUDE_DESC
    "Key",
    "Decryption key",
#endif
    GPON_RX_ENCRYPTION_KEY_KEY_FIELD_MASK,
    0,
    GPON_RX_ENCRYPTION_KEY_KEY_FIELD_WIDTH,
    GPON_RX_ENCRYPTION_KEY_KEY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ENCRYPTION_SWITCH_TIME_SW_TIME
 ******************************************************************************/
const ru_field_rec GPON_RX_ENCRYPTION_SWITCH_TIME_SW_TIME_FIELD =
{
    "SW_TIME",
#if RU_INCLUDE_DESC
    "Switch_Time",
    "The superframe counter value at which the new key is put to use."
    "This field should not be changed while waiting for key switch (ARM bit is set).",
#endif
    GPON_RX_ENCRYPTION_SWITCH_TIME_SW_TIME_FIELD_MASK,
    0,
    GPON_RX_ENCRYPTION_SWITCH_TIME_SW_TIME_FIELD_WIDTH,
    GPON_RX_ENCRYPTION_SWITCH_TIME_SW_TIME_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ENCRYPTION_SWITCH_TIME_ARM
 ******************************************************************************/
const ru_field_rec GPON_RX_ENCRYPTION_SWITCH_TIME_ARM_FIELD =
{
    "ARM",
#if RU_INCLUDE_DESC
    "Switch_mechanism_arm",
    "Allows putting the new key to use when the received superframe counter matches the SW_TIME."
    "The mechanism is armed by writing 1 to this bit."
    "After arming, the bit is cleared by hardware right after the new key was put to use. SW can monitor this bit to determine when the switch occurred - after the switch occurred the bit will be cleared to 0."
    "This bit should be set only after the KEY registers were updated and the SWITCH_TIME field is valid.",
#endif
    GPON_RX_ENCRYPTION_SWITCH_TIME_ARM_FIELD_MASK,
    0,
    GPON_RX_ENCRYPTION_SWITCH_TIME_ARM_FIELD_WIDTH,
    GPON_RX_ENCRYPTION_SWITCH_TIME_ARM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ENCRYPTION_SWITCH_TIME_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_ENCRYPTION_SWITCH_TIME_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ENCRYPTION_SWITCH_TIME_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_ENCRYPTION_SWITCH_TIME_RESERVED0_FIELD_WIDTH,
    GPON_RX_ENCRYPTION_SWITCH_TIME_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_ENCRYPTION_SF_CNTR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_ENCRYPTION_SF_CNTR_FIELDS[] =
{
    &GPON_RX_ENCRYPTION_SF_CNTR_SF_CNTR_FIELD,
    &GPON_RX_ENCRYPTION_SF_CNTR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_ENCRYPTION_SF_CNTR_REG = 
{
    "SF_CNTR",
#if RU_INCLUDE_DESC
    "SUPERFRAME_COUNTER Register",
    "Shows the value of the received superframe counter",
#endif
    GPON_RX_ENCRYPTION_SF_CNTR_REG_OFFSET,
    0,
    0,
    109,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_ENCRYPTION_SF_CNTR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_ENCRYPTION_KEY
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_ENCRYPTION_KEY_FIELDS[] =
{
    &GPON_RX_ENCRYPTION_KEY_KEY_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_ENCRYPTION_KEY_REG = 
{
    "KEY",
#if RU_INCLUDE_DESC
    "DECRYPTION_KEY %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bir registers. Registr 0 in the array sets the MSB, register 3 sets the LSB."
    "These registers should not be changed while waiting for a key-switch (the ARM bit in the SWITCH_TIME register is set).",
#endif
    GPON_RX_ENCRYPTION_KEY_REG_OFFSET,
    GPON_RX_ENCRYPTION_KEY_REG_RAM_CNT,
    4,
    110,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_ENCRYPTION_KEY_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_ENCRYPTION_SWITCH_TIME
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_ENCRYPTION_SWITCH_TIME_FIELDS[] =
{
    &GPON_RX_ENCRYPTION_SWITCH_TIME_SW_TIME_FIELD,
    &GPON_RX_ENCRYPTION_SWITCH_TIME_ARM_FIELD,
    &GPON_RX_ENCRYPTION_SWITCH_TIME_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_ENCRYPTION_SWITCH_TIME_REG = 
{
    "SWITCH_TIME",
#if RU_INCLUDE_DESC
    "KEY_SWITCH_TIME Register",
    "Sets the superframe counter value at which the new key is put to use",
#endif
    GPON_RX_ENCRYPTION_SWITCH_TIME_REG_OFFSET,
    0,
    0,
    111,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_RX_ENCRYPTION_SWITCH_TIME_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_ENCRYPTION
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_ENCRYPTION_REGS[] =
{
    &GPON_RX_ENCRYPTION_SF_CNTR_REG,
    &GPON_RX_ENCRYPTION_KEY_REG,
    &GPON_RX_ENCRYPTION_SWITCH_TIME_REG,
};

unsigned long GPON_RX_ENCRYPTION_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130f9700,
#else
    0x80150700,
#endif
};

const ru_block_rec GPON_RX_ENCRYPTION_BLOCK = 
{
    "GPON_RX_ENCRYPTION",
    GPON_RX_ENCRYPTION_ADDRS,
    1,
    3,
    GPON_RX_ENCRYPTION_REGS
};

/* End of file BCM6858_A0GPON_RX_ENCRYPTION.c */
