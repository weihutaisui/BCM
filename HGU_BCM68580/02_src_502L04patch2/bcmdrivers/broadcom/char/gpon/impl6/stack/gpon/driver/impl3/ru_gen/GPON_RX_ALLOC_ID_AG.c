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
#if defined(CONFIG_BCM963158)
    0x80150030,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db1030,
#else
    #error "GPON_RX_ALLOC_ID base address not defined"	
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

/* End of file BCM6836_A0GPON_RX_ALLOC_ID.c */
