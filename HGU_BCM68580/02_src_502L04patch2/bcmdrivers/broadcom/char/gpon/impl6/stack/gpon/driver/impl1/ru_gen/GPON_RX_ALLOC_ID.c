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
 * Field: GPON_RX_ALLOC_ID_RANG_ALC_ID_ALLOC_ID
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_RANG_ALC_ID_ALLOC_ID_FIELD =
{
    "ALLOC_ID",
#if RU_INCLUDE_DESC
    "Alloc-ID",
    "Defines the ranging Alloc-ID."
    "This field can be changed only while the corresponding fiter is disabled (via the ALLOCID_EN register).",
#endif
    GPON_RX_ALLOC_ID_RANG_ALC_ID_ALLOC_ID_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_ALLOC_ID_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_ALLOC_ID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_RANG_ALC_ID_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_RANG_ALC_ID_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_RANG_ALC_ID_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_RESERVED0_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_ID_CFG_ALLOC_ID
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_ID_CFG_ALLOC_ID_FIELD =
{
    "ALLOC_ID",
#if RU_INCLUDE_DESC
    "ALLOC_ID",
    "Alloc-ID assignment."
    "This field can be changed only while the corresponding fiter is disabled (via the ALLOCID_EN register).",
#endif
    GPON_RX_ALLOC_ID_ALC_ID_CFG_ALLOC_ID_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_ID_CFG_ALLOC_ID_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_ID_CFG_ALLOC_ID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_ID_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_ID_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_ALC_ID_CFG_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_ID_CFG_RESERVED0_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_ID_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_ALLOC_EN
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_ALLOC_EN_FIELD =
{
    "ALLOC_EN",
#if RU_INCLUDE_DESC
    "Alloc_ID_enable",
    "Enable the ranging alloc-ID comparator. May be changed on the fly.",
#endif
    GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_ALLOC_EN_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_ALLOC_EN_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_ALLOC_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_RESERVED0_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_ID_EN_ALLOC_EN
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_ID_EN_ALLOC_EN_FIELD =
{
    "ALLOC_EN",
#if RU_INCLUDE_DESC
    "Alloc-ID_Enable",
    "Enables/Disables the Alloc-ID comparators. Every bit corresponds to one Alloc-ID comparator."
    "This field can be changed on-the-fly during operation.",
#endif
    GPON_RX_ALLOC_ID_ALC_ID_EN_ALLOC_EN_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_ID_EN_ALLOC_EN_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_ID_EN_ALLOC_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_ID_EN_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_ID_EN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_ALC_ID_EN_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_ID_EN_RESERVED0_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_ID_EN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A0_TNUM
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A0_TNUM_FIELD =
{
    "A0_TNUM",
#if RU_INCLUDE_DESC
    "ALLOC_ID_0_TCONT_number",
    "TCONT number associated to ALLOC_ID 0",
#endif
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A0_TNUM_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A0_TNUM_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A0_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED0_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A1_TNUM
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A1_TNUM_FIELD =
{
    "A1_TNUM",
#if RU_INCLUDE_DESC
    "ALLOC_ID_1_TCONT_number",
    "TCONT number associated to ALLOC_ID 1",
#endif
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A1_TNUM_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A1_TNUM_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A1_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED1_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED1_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A2_TNUM
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A2_TNUM_FIELD =
{
    "A2_TNUM",
#if RU_INCLUDE_DESC
    "ALLOC_ID_2_TCONT_number",
    "TCONT number associated to ALLOC_ID 2",
#endif
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A2_TNUM_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A2_TNUM_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A2_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED2
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED2_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED2_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A3_TNUM
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A3_TNUM_FIELD =
{
    "A3_TNUM",
#if RU_INCLUDE_DESC
    "ALLOC_ID_3_TCONT_number",
    "TCONT number associated to ALLOC_ID 3",
#endif
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A3_TNUM_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A3_TNUM_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A3_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED3
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED3_FIELD =
{
    "RESERVED3",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED3_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED3_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT0_ASSOC
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT0_ASSOC_FIELD =
{
    "CNT0_ASSOC",
#if RU_INCLUDE_DESC
    "cnt0_assoc",
    "To which TCONT to associate counter 0",
#endif
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT0_ASSOC_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT0_ASSOC_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT0_ASSOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED0_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT1_ASSOC
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT1_ASSOC_FIELD =
{
    "CNT1_ASSOC",
#if RU_INCLUDE_DESC
    "cnt1_assoc",
    "To which TCONT to associate counter 1",
#endif
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT1_ASSOC_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT1_ASSOC_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT1_ASSOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED1_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED1_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT2_ASSOC
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT2_ASSOC_FIELD =
{
    "CNT2_ASSOC",
#if RU_INCLUDE_DESC
    "cnt2_assoc",
    "To which TCONT to associate counter 2",
#endif
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT2_ASSOC_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT2_ASSOC_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT2_ASSOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED2
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED2_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED2_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT3_ASSOC
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT3_ASSOC_FIELD =
{
    "CNT3_ASSOC",
#if RU_INCLUDE_DESC
    "cnt3_assoc",
    "To which TCONT to associate counter 3",
#endif
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT3_ASSOC_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT3_ASSOC_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT3_ASSOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED3
 ******************************************************************************/
const ru_field_rec GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED3_FIELD =
{
    "RESERVED3",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED3_FIELD_MASK,
    0,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED3_FIELD_WIDTH,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_ALLOC_ID_RANG_ALC_ID
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_ALLOC_ID_RANG_ALC_ID_FIELDS[] =
{
    &GPON_RX_ALLOC_ID_RANG_ALC_ID_ALLOC_ID_FIELD,
    &GPON_RX_ALLOC_ID_RANG_ALC_ID_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_ALLOC_ID_RANG_ALC_ID_REG = 
{
    "RANG_ALC_ID",
#if RU_INCLUDE_DESC
    "RANGING_ALLOC_ID_CONFIGURATION Register",
    "Assigns Alloc-IDs used during ranging (first the broadcast Alloc-ID is assigned and then the ONU-ID is assigned). After ranging, this Alloc-ID is disabled.",
#endif
    GPON_RX_ALLOC_ID_RANG_ALC_ID_REG_OFFSET,
    0,
    0,
    96,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_ALLOC_ID_ALC_ID_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_ALLOC_ID_ALC_ID_CFG_FIELDS[] =
{
    &GPON_RX_ALLOC_ID_ALC_ID_CFG_ALLOC_ID_FIELD,
    &GPON_RX_ALLOC_ID_ALC_ID_CFG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_ALLOC_ID_ALC_ID_CFG_REG = 
{
    "ALC_ID_CFG",
#if RU_INCLUDE_DESC
    "ALLOC_ID_CONFIGURATION %i Register",
    "40 registers are provided to allow assignment of up to 40 Alloc-IDs per ONU.",
#endif
    GPON_RX_ALLOC_ID_ALC_ID_CFG_REG_OFFSET,
    GPON_RX_ALLOC_ID_ALC_ID_CFG_REG_RAM_CNT,
    4,
    97,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_ALLOC_ID_ALC_ID_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_ALLOC_ID_RANG_ALC_ID_EN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_FIELDS[] =
{
    &GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_ALLOC_EN_FIELD,
    &GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_REG = 
{
    "RANG_ALC_ID_EN",
#if RU_INCLUDE_DESC
    "RANGING_ALLOC_ID_ENABLE Register",
    "This register is used to enable/disable the ranging Alloc-ID comprator",
#endif
    GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_REG_OFFSET,
    0,
    0,
    98,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_ALLOC_ID_ALC_ID_EN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_ALLOC_ID_ALC_ID_EN_FIELDS[] =
{
    &GPON_RX_ALLOC_ID_ALC_ID_EN_ALLOC_EN_FIELD,
    &GPON_RX_ALLOC_ID_ALC_ID_EN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_ALLOC_ID_ALC_ID_EN_REG = 
{
    "ALC_ID_EN",
#if RU_INCLUDE_DESC
    "ALLOC_ID_ENABLE %i Register",
    "This register is used to enable/disable the Alloc-ID comprators",
#endif
    GPON_RX_ALLOC_ID_ALC_ID_EN_REG_OFFSET,
    GPON_RX_ALLOC_ID_ALC_ID_EN_REG_RAM_CNT,
    4,
    99,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_ALLOC_ID_ALC_ID_EN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_FIELDS[] =
{
    &GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A0_TNUM_FIELD,
    &GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED0_FIELD,
    &GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A1_TNUM_FIELD,
    &GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED1_FIELD,
    &GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A2_TNUM_FIELD,
    &GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED2_FIELD,
    &GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_A3_TNUM_FIELD,
    &GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_RESERVED3_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_REG = 
{
    "ALC_TCNT_ASSOC",
#if RU_INCLUDE_DESC
    "ALLOC_ID_TO_TCONT_ASSOCIATION %i Register",
    "A vector that defines which TCONT is associated to each one of the ALLOC_ID which are configured in the ALLOC_ID CONF register."
    "For each ALLOC_ID, there are 6 bits dedicated for configuring the associated queue number. The least significant byte in the first register defines the alloc-id for tcont0, while the most significant byte of the last register in the array defines the alloc-id for tcont 39",
#endif
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_REG_OFFSET,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_REG_RAM_CNT,
    4,
    100,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    8,
    GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_ALLOC_ID_BW_CNT_ASSOC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_ALLOC_ID_BW_CNT_ASSOC_FIELDS[] =
{
    &GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT0_ASSOC_FIELD,
    &GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED0_FIELD,
    &GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT1_ASSOC_FIELD,
    &GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED1_FIELD,
    &GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT2_ASSOC_FIELD,
    &GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED2_FIELD,
    &GPON_RX_ALLOC_ID_BW_CNT_ASSOC_CNT3_ASSOC_FIELD,
    &GPON_RX_ALLOC_ID_BW_CNT_ASSOC_RESERVED3_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_ALLOC_ID_BW_CNT_ASSOC_REG = 
{
    "BW_CNT_ASSOC",
#if RU_INCLUDE_DESC
    "BANDWIDTH_COUNTERS_ASSOCIATION Register",
    "set of 4 counters which accumulate the accesses of a certain TCONT."
    "Each counter can be associated with any of the 40 TCONTs.",
#endif
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_REG_OFFSET,
    0,
    0,
    101,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    8,
    GPON_RX_ALLOC_ID_BW_CNT_ASSOC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_ALLOC_ID
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_ALLOC_ID_REGS[] =
{
    &GPON_RX_ALLOC_ID_RANG_ALC_ID_REG,
    &GPON_RX_ALLOC_ID_ALC_ID_CFG_REG,
    &GPON_RX_ALLOC_ID_RANG_ALC_ID_EN_REG,
    &GPON_RX_ALLOC_ID_ALC_ID_EN_REG,
    &GPON_RX_ALLOC_ID_ALC_TCNT_ASSOC_REG,
    &GPON_RX_ALLOC_ID_BW_CNT_ASSOC_REG,
};

unsigned long GPON_RX_ALLOC_ID_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130f9030,
#else
    0x80150030,
#endif
};

const ru_block_rec GPON_RX_ALLOC_ID_BLOCK = 
{
    "GPON_RX_ALLOC_ID",
    GPON_RX_ALLOC_ID_ADDRS,
    1,
    6,
    GPON_RX_ALLOC_ID_REGS
};

/* End of file BCM6858_A0GPON_RX_ALLOC_ID.c */
