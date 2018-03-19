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
 * Field: NGPON_TX_PROFILE_PROFILE_CTRL_DELIMITER_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_PROFILE_CTRL_DELIMITER_LEN_FIELD =
{
    "DELIMITER_LEN",
#if RU_INCLUDE_DESC
    "delimiter_length",
    "delimiter length in octets; size allowed are 4/8.",
#endif
    NGPON_TX_PROFILE_PROFILE_CTRL_DELIMITER_LEN_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_PROFILE_CTRL_DELIMITER_LEN_FIELD_WIDTH,
    NGPON_TX_PROFILE_PROFILE_CTRL_DELIMITER_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_LEN_FIELD =
{
    "PREAMBLE_LEN",
#if RU_INCLUDE_DESC
    "preamble_length",
    "preamble length in octets; four-bit integer; range 1-8.",
#endif
    NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_LEN_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_LEN_FIELD_WIDTH,
    NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED0_FIELD_WIDTH,
    NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_PROFILE_CTRL_FEC_IND
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_PROFILE_CTRL_FEC_IND_FIELD =
{
    "FEC_IND",
#if RU_INCLUDE_DESC
    "FEC_IND",
    "Enable for fec insertion",
#endif
    NGPON_TX_PROFILE_PROFILE_CTRL_FEC_IND_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_PROFILE_CTRL_FEC_IND_FIELD_WIDTH,
    NGPON_TX_PROFILE_PROFILE_CTRL_FEC_IND_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_TOTAL_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_TOTAL_LEN_FIELD =
{
    "PREAMBLE_TOTAL_LEN",
#if RU_INCLUDE_DESC
    "preamble_total_length",
    "preamble total length = preamble_length * preambel_reapt",
#endif
    NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_TOTAL_LEN_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_TOTAL_LEN_FIELD_WIDTH,
    NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_TOTAL_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED1_FIELD_WIDTH,
    NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_PROFILE_CTRL_PROF_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_PROFILE_CTRL_PROF_EN_FIELD =
{
    "PROF_EN",
#if RU_INCLUDE_DESC
    "profile_en",
    "If this bit isnt set and the profile_filter_en  bit in the AF_ERR_FILT  register is set, every access of this profile will be filtered",
#endif
    NGPON_TX_PROFILE_PROFILE_CTRL_PROF_EN_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_PROFILE_CTRL_PROF_EN_FIELD_WIDTH,
    NGPON_TX_PROFILE_PROFILE_CTRL_PROF_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_PREAMBLE_0_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_PREAMBLE_0_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "The two registers of the array together holds the preamble value",
#endif
    NGPON_TX_PROFILE_PREAMBLE_0_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_PREAMBLE_0_VALUE_FIELD_WIDTH,
    NGPON_TX_PROFILE_PREAMBLE_0_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_PREAMBLE_1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_PREAMBLE_1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "The two registers of the array together holds the preamble value",
#endif
    NGPON_TX_PROFILE_PREAMBLE_1_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_PREAMBLE_1_VALUE_FIELD_WIDTH,
    NGPON_TX_PROFILE_PREAMBLE_1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_DELIMITER_0_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_DELIMITER_0_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "the two arrays register together holds the Deliminter value",
#endif
    NGPON_TX_PROFILE_DELIMITER_0_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_DELIMITER_0_VALUE_FIELD_WIDTH,
    NGPON_TX_PROFILE_DELIMITER_0_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_DELIMITER_1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_DELIMITER_1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "the two arrays register together holds the Deliminter value",
#endif
    NGPON_TX_PROFILE_DELIMITER_1_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_DELIMITER_1_VALUE_FIELD_WIDTH,
    NGPON_TX_PROFILE_DELIMITER_1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_FEC_TYPE_TYPE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_FEC_TYPE_TYPE_FIELD =
{
    "TYPE",
#if RU_INCLUDE_DESC
    "TYPE",
    "2b00 = RS(248,232)"
    "2b01 = RS(248,216)"
    "2b10 = RS(248,200)"
    "2b11 = Reserved",
#endif
    NGPON_TX_PROFILE_FEC_TYPE_TYPE_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_FEC_TYPE_TYPE_FIELD_WIDTH,
    NGPON_TX_PROFILE_FEC_TYPE_TYPE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PROFILE_FEC_TYPE_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_PROFILE_FEC_TYPE_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_PROFILE_FEC_TYPE_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_PROFILE_FEC_TYPE_RESERVED0_FIELD_WIDTH,
    NGPON_TX_PROFILE_FEC_TYPE_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_TX_PROFILE_PROFILE_CTRL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PROFILE_PROFILE_CTRL_FIELDS[] =
{
    &NGPON_TX_PROFILE_PROFILE_CTRL_DELIMITER_LEN_FIELD,
    &NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_LEN_FIELD,
    &NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED0_FIELD,
    &NGPON_TX_PROFILE_PROFILE_CTRL_FEC_IND_FIELD,
    &NGPON_TX_PROFILE_PROFILE_CTRL_PREAMBLE_TOTAL_LEN_FIELD,
    &NGPON_TX_PROFILE_PROFILE_CTRL_RESERVED1_FIELD,
    &NGPON_TX_PROFILE_PROFILE_CTRL_PROF_EN_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PROFILE_PROFILE_CTRL_REG = 
{
    "PROFILE_CTRL",
#if RU_INCLUDE_DESC
    "CONTROLS Register",
    "control fields of the profile"
    "fec_en, delimiter_lenght, preamble_length and preamble_repeat_count",
#endif
    NGPON_TX_PROFILE_PROFILE_CTRL_REG_OFFSET,
    NGPON_TX_PROFILE_PROFILE_CTRL_REG_RAM_CNT,
    24,
    174,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    7,
    NGPON_TX_PROFILE_PROFILE_CTRL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PROFILE_PREAMBLE_0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PROFILE_PREAMBLE_0_FIELDS[] =
{
    &NGPON_TX_PROFILE_PREAMBLE_0_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PROFILE_PREAMBLE_0_REG = 
{
    "PREAMBLE_0",
#if RU_INCLUDE_DESC
    "PREAMBLE %i Register",
    "A register array of two registers that together holds 8 bytes of the preamble."
    "the first register (smaller address) holds the most significant bytes of the preamble."
    "",
#endif
    NGPON_TX_PROFILE_PREAMBLE_0_REG_OFFSET,
    NGPON_TX_PROFILE_PREAMBLE_0_REG_RAM_CNT,
    24,
    175,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_PROFILE_PREAMBLE_0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PROFILE_PREAMBLE_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PROFILE_PREAMBLE_1_FIELDS[] =
{
    &NGPON_TX_PROFILE_PREAMBLE_1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PROFILE_PREAMBLE_1_REG = 
{
    "PREAMBLE_1",
#if RU_INCLUDE_DESC
    "PREAMBLE %i Register",
    "A register array of two registers that together holds 8 bytes of the preamble."
    "the first register (smaller address) holds the most significant bytes of the preamble."
    "",
#endif
    NGPON_TX_PROFILE_PREAMBLE_1_REG_OFFSET,
    NGPON_TX_PROFILE_PREAMBLE_1_REG_RAM_CNT,
    24,
    176,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_PROFILE_PREAMBLE_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PROFILE_DELIMITER_0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PROFILE_DELIMITER_0_FIELDS[] =
{
    &NGPON_TX_PROFILE_DELIMITER_0_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PROFILE_DELIMITER_0_REG = 
{
    "DELIMITER_0",
#if RU_INCLUDE_DESC
    "DELIMITER %i Register",
    "A register array of two registers that together holds 8 bytes of the delimiter."
    "the first register (smaller address) holds the delimiters most significant bytes."
    "Address offset  | delimiterReg"
    "----------------|---------------"
    "0         | D7_D6_D5_D4"
    "----------------|---------------"
    "1         | D3_D2_D1_D0"
    "----------------|---------------"
    ""
    "Output to the PON"
    "First byte --------------> Last byte"
    "D7,D6,D5,D4,D3,D2,D1,D0   (if dlmg_delimiter_length=8)"
    "D3,D2,D1,D0               (if dlmg_delimiter_length=4)"
    ""
    "If the requested delimiter length is 0,1,2,3,5,6,7 then SW will write the end of the Preamble bytes into the delimiter registers in order to complete it to 4/8 bytes."
    "",
#endif
    NGPON_TX_PROFILE_DELIMITER_0_REG_OFFSET,
    NGPON_TX_PROFILE_DELIMITER_0_REG_RAM_CNT,
    24,
    177,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_PROFILE_DELIMITER_0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PROFILE_DELIMITER_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PROFILE_DELIMITER_1_FIELDS[] =
{
    &NGPON_TX_PROFILE_DELIMITER_1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PROFILE_DELIMITER_1_REG = 
{
    "DELIMITER_1",
#if RU_INCLUDE_DESC
    "DELIMITER %i Register",
    "A register array of two registers that together holds 8 bytes of the delimiter."
    "the first register (smaller address) holds the delimiters most significant bytes."
    "Address offset  | delimiterReg"
    "----------------|---------------"
    "0         | D7_D6_D5_D4"
    "----------------|---------------"
    "1         | D3_D2_D1_D0"
    "----------------|---------------"
    ""
    "Output to the PON"
    "First byte --------------> Last byte"
    "D7,D6,D5,D4,D3,D2,D1,D0   (if dlmg_delimiter_length=8)"
    "D3,D2,D1,D0               (if dlmg_delimiter_length=4)"
    ""
    "If the requested delimiter length is 0,1,2,3,5,6,7 then SW will write the end of the Preamble bytes into the delimiter registers in order to complete it to 4/8 bytes."
    "",
#endif
    NGPON_TX_PROFILE_DELIMITER_1_REG_OFFSET,
    NGPON_TX_PROFILE_DELIMITER_1_REG_RAM_CNT,
    24,
    178,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_PROFILE_DELIMITER_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PROFILE_FEC_TYPE
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PROFILE_FEC_TYPE_FIELDS[] =
{
    &NGPON_TX_PROFILE_FEC_TYPE_TYPE_FIELD,
    &NGPON_TX_PROFILE_FEC_TYPE_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PROFILE_FEC_TYPE_REG = 
{
    "FEC_TYPE",
#if RU_INCLUDE_DESC
    "FEC_TYPE Register",
    "FEC type configuration."
    "Choose between:"
    "RS(248,232)"
    "RS(248,216)"
    "RS(248,200)",
#endif
    NGPON_TX_PROFILE_FEC_TYPE_REG_OFFSET,
    NGPON_TX_PROFILE_FEC_TYPE_REG_RAM_CNT,
    24,
    179,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_PROFILE_FEC_TYPE_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_TX_PROFILE
 ******************************************************************************/
static const ru_reg_rec *NGPON_TX_PROFILE_REGS[] =
{
    &NGPON_TX_PROFILE_PROFILE_CTRL_REG,
    &NGPON_TX_PROFILE_PREAMBLE_0_REG,
    &NGPON_TX_PROFILE_PREAMBLE_1_REG,
    &NGPON_TX_PROFILE_DELIMITER_0_REG,
    &NGPON_TX_PROFILE_DELIMITER_1_REG,
    &NGPON_TX_PROFILE_FEC_TYPE_REG,
};

unsigned long NGPON_TX_PROFILE_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80168600,
#elif defined(CONFIG_BCM96856)
    0x82db8600,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_TX_PROFILE_BLOCK = 
{
    "NGPON_TX_PROFILE",
    NGPON_TX_PROFILE_ADDRS,
    1,
    6,
    NGPON_TX_PROFILE_REGS
};

/* End of file BCM6858_A0_NGPON_TX_PROFILE.c */
