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
 * Field: NGPON_RX_BWMAP_TRFC_ALLOCID_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_ALLOCID_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Alloc-ID for traffic",
#endif
    NGPON_RX_BWMAP_TRFC_ALLOCID_VALUE_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_ALLOCID_VALUE_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_ALLOCID_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_ALLOCID_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_ALLOCID_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_TRFC_ALLOCID_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_ALLOCID_RESERVED0_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_ALLOCID_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_ALLOCID_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_ALLOCID_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Alloc-ID for ranging",
#endif
    NGPON_RX_BWMAP_RNGNG_ALLOCID_VALUE_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_VALUE_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_ALLOCID_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_ALLOCID_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_RNGNG_ALLOCID_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_RESERVED0_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_EN
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_EN_FIELD =
{
    "EN",
#if RU_INCLUDE_DESC
    "Enable",
    "Enables #1-#32. Bit 0 enables comparator #1, bit 31 enables comparator #32."
    ""
    "0 = disable (access dropped)"
    "1 = enable",
#endif
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_EN_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_EN_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_EN
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_EN_FIELD =
{
    "EN",
#if RU_INCLUDE_DESC
    "Enable",
    "Enables #33-#40. Bit 0 enables comparator #33, bit 7 enables comparator #40."
    ""
    "0 = disable"
    "1 = enable",
#endif
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_EN_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_EN_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_RESERVED0_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_EN
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_EN_FIELD =
{
    "EN",
#if RU_INCLUDE_DESC
    "Enable",
    "Bit 0 enables comparator #1, bit 3 enables comparator #4."
    ""
    "0 = disable"
    "1 = enable",
#endif
    NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_EN_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_EN_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_RESERVED0_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A0_TNUM
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A0_TNUM_FIELD =
{
    "A0_TNUM",
#if RU_INCLUDE_DESC
    "A0_TNUM",
    "TCONT number associated to ALLOC_ID 0",
#endif
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A0_TNUM_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A0_TNUM_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A0_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED0_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A1_TNUM
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A1_TNUM_FIELD =
{
    "A1_TNUM",
#if RU_INCLUDE_DESC
    "A1_TNUM",
    "TCONT number associated to ALLOC_ID 1",
#endif
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A1_TNUM_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A1_TNUM_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A1_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED1_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A2_TNUM
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A2_TNUM_FIELD =
{
    "A2_TNUM",
#if RU_INCLUDE_DESC
    "A2_TNUM",
    "TCONT number associated to ALLOC_ID 2",
#endif
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A2_TNUM_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A2_TNUM_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A2_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED2_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED2_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A3_TNUM
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A3_TNUM_FIELD =
{
    "A3_TNUM",
#if RU_INCLUDE_DESC
    "A3_TNUM",
    "TCONT number associated to ALLOC_ID 3",
#endif
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A3_TNUM_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A3_TNUM_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A3_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED3
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED3_FIELD =
{
    "RESERVED3",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED3_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED3_FIELD_WIDTH,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A0_TNUM
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A0_TNUM_FIELD =
{
    "RNG_A0_TNUM",
#if RU_INCLUDE_DESC
    "RNG_A0_TNUM",
    "TCONT number associated to ranging ALLOC_ID 0",
#endif
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A0_TNUM_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A0_TNUM_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A0_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED0_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A1_TNUM
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A1_TNUM_FIELD =
{
    "RNG_A1_TNUM",
#if RU_INCLUDE_DESC
    "RNG_A1_TNUM",
    "TCONT number associated to ranging ALLOC_ID 1",
#endif
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A1_TNUM_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A1_TNUM_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A1_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED1_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A2_TNUM
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A2_TNUM_FIELD =
{
    "RNG_A2_TNUM",
#if RU_INCLUDE_DESC
    "RNG_A2_TNUM",
    "TCONT number associated to ranging ALLOC_ID 2",
#endif
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A2_TNUM_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A2_TNUM_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A2_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED2_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED2_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A3_TNUM
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A3_TNUM_FIELD =
{
    "RNG_A3_TNUM",
#if RU_INCLUDE_DESC
    "RNG_A3_TNUM",
    "TCONT number associated to ranging ALLOC_ID 3",
#endif
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A3_TNUM_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A3_TNUM_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A3_TNUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED3
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED3_FIELD =
{
    "RESERVED3",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED3_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED3_FIELD_WIDTH,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_BWCNT_ASSOC_CNT9_ASSOC
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_BWCNT_ASSOC_CNT9_ASSOC_FIELD =
{
    "CNT9_ASSOC",
#if RU_INCLUDE_DESC
    "CNT9_ASSOC",
    "To which TCONT to associate counter 9",
#endif
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT9_ASSOC_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT9_ASSOC_FIELD_WIDTH,
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT9_ASSOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED0_FIELD_WIDTH,
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_BWCNT_ASSOC_CNT10_ASSOC
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_BWCNT_ASSOC_CNT10_ASSOC_FIELD =
{
    "CNT10_ASSOC",
#if RU_INCLUDE_DESC
    "CNT10_ASSOC",
    "To which TCONT to associate counter 10",
#endif
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT10_ASSOC_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT10_ASSOC_FIELD_WIDTH,
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT10_ASSOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED1_FIELD_WIDTH,
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_BWCNT_ASSOC_CNT11_ASSOC
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_BWCNT_ASSOC_CNT11_ASSOC_FIELD =
{
    "CNT11_ASSOC",
#if RU_INCLUDE_DESC
    "CNT11_ASSOC",
    "To which TCONT to associate counter 11",
#endif
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT11_ASSOC_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT11_ASSOC_FIELD_WIDTH,
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT11_ASSOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED2_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED2_FIELD_WIDTH,
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_BWCNT_ASSOC_CNT12_ASSOC
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_BWCNT_ASSOC_CNT12_ASSOC_FIELD =
{
    "CNT12_ASSOC",
#if RU_INCLUDE_DESC
    "CNT12_ASSOC",
    "To which TCONT to associate counter 12",
#endif
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT12_ASSOC_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT12_ASSOC_FIELD_WIDTH,
    NGPON_RX_BWMAP_BWCNT_ASSOC_CNT12_ASSOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED3
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED3_FIELD =
{
    "RESERVED3",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED3_FIELD_MASK,
    0,
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED3_FIELD_WIDTH,
    NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_BWMAP_TRFC_ALLOCID
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWMAP_TRFC_ALLOCID_FIELDS[] =
{
    &NGPON_RX_BWMAP_TRFC_ALLOCID_VALUE_FIELD,
    &NGPON_RX_BWMAP_TRFC_ALLOCID_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWMAP_TRFC_ALLOCID_REG = 
{
    "TRFC_ALLOCID",
#if RU_INCLUDE_DESC
    "TRAFFIC_ALLOC_ID %i Register",
    "Alloc-IDs associated with traffic (#1-#40). Passed to TX unit for transmission.",
#endif
    NGPON_RX_BWMAP_TRFC_ALLOCID_REG_OFFSET,
    NGPON_RX_BWMAP_TRFC_ALLOCID_REG_RAM_CNT,
    4,
    34,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_BWMAP_TRFC_ALLOCID_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_BWMAP_RNGNG_ALLOCID
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWMAP_RNGNG_ALLOCID_FIELDS[] =
{
    &NGPON_RX_BWMAP_RNGNG_ALLOCID_VALUE_FIELD,
    &NGPON_RX_BWMAP_RNGNG_ALLOCID_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWMAP_RNGNG_ALLOCID_REG = 
{
    "RNGNG_ALLOCID",
#if RU_INCLUDE_DESC
    "RANGING_ALLOC_ID %i Register",
    "Alloc-IDs associated with non-traffic information (such as ranging). Passed to TX unit for transmission.",
#endif
    NGPON_RX_BWMAP_RNGNG_ALLOCID_REG_OFFSET,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_REG_RAM_CNT,
    4,
    35,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_BWMAP_TRFC_ALLOCID_EN1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_FIELDS[] =
{
    &NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_EN_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_REG = 
{
    "TRFC_ALLOCID_EN1",
#if RU_INCLUDE_DESC
    "TRAFFIC_ALLOC_ID_ENABLE1 Register",
    "Enable/disable the Alloc-ID comparators #1-32 (for TX)",
#endif
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_REG_OFFSET,
    0,
    0,
    36,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_BWMAP_TRFC_ALLOCID_EN2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_FIELDS[] =
{
    &NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_EN_FIELD,
    &NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_REG = 
{
    "TRFC_ALLOCID_EN2",
#if RU_INCLUDE_DESC
    "TRAFFIC_ALLOC_ID_ENABLE2 Register",
    "Enable/disable the Alloc-ID comparators #33-40 (for TX)",
#endif
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_REG_OFFSET,
    0,
    0,
    37,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_BWMAP_RNGNG_ALLOCID_EN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_FIELDS[] =
{
    &NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_EN_FIELD,
    &NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_REG = 
{
    "RNGNG_ALLOCID_EN",
#if RU_INCLUDE_DESC
    "RANGING_ALLOC_ID_ENABLE Register",
    "Enable/disable the ranging Alloc-ID comparators",
#endif
    NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_REG_OFFSET,
    0,
    0,
    38,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_BWMAP_TRFC_TCNT_ASSOC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_FIELDS[] =
{
    &NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A0_TNUM_FIELD,
    &NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED0_FIELD,
    &NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A1_TNUM_FIELD,
    &NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED1_FIELD,
    &NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A2_TNUM_FIELD,
    &NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED2_FIELD,
    &NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_A3_TNUM_FIELD,
    &NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_RESERVED3_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_REG = 
{
    "TRFC_TCNT_ASSOC",
#if RU_INCLUDE_DESC
    "ALLOC_ID_TO_TCONT_ASSOCIATION %i Register",
    "A vector that defines which TCONT is associated to each one of the ALLOC_ID which are configured in the TRFC_ALLOC_ID CONF register."
    "For each ALLOC_ID, there are 6 bits dedicated for configuring the associated queue number. The least significant byte in the first register defines the tcont for alloc-id #1, while the most significant byte of the last register in the array defines the tcont for alloc-id #40.",
#endif
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_REG_OFFSET,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_REG_RAM_CNT,
    4,
    39,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    8,
    NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_FIELDS[] =
{
    &NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A0_TNUM_FIELD,
    &NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED0_FIELD,
    &NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A1_TNUM_FIELD,
    &NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED1_FIELD,
    &NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A2_TNUM_FIELD,
    &NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED2_FIELD,
    &NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RNG_A3_TNUM_FIELD,
    &NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_RESERVED3_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_REG = 
{
    "RNGNG_TCNT_ASSOC",
#if RU_INCLUDE_DESC
    "RANGING_ID_TO_TCONT_ASSOCIATION Register",
    "A vector that defines which TCONT is associated to each one of the ALLOC_ID which are configured in the RNGNG_ALLOC_ID CONF register."
    "For each ALLOC_ID, there are 6 bits dedicated for configuring the associated queue number. The least significant byte defines the tcont for ranging alloc-id #1, while the most significant byte defines the tcont for ranging alloc-id #4.",
#endif
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_REG_OFFSET,
    0,
    0,
    40,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    8,
    NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_BWMAP_BWCNT_ASSOC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWMAP_BWCNT_ASSOC_FIELDS[] =
{
    &NGPON_RX_BWMAP_BWCNT_ASSOC_CNT9_ASSOC_FIELD,
    &NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED0_FIELD,
    &NGPON_RX_BWMAP_BWCNT_ASSOC_CNT10_ASSOC_FIELD,
    &NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED1_FIELD,
    &NGPON_RX_BWMAP_BWCNT_ASSOC_CNT11_ASSOC_FIELD,
    &NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED2_FIELD,
    &NGPON_RX_BWMAP_BWCNT_ASSOC_CNT12_ASSOC_FIELD,
    &NGPON_RX_BWMAP_BWCNT_ASSOC_RESERVED3_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWMAP_BWCNT_ASSOC_REG = 
{
    "BWCNT_ASSOC",
#if RU_INCLUDE_DESC
    "BANDWIDTH_COUNTERS_ASSOCIATION Register",
    "There is a set of 4 counters in the PM section which accumulate the accesses of a certain TCONT."
    "Each counter can be associated with any of the 40 TCONTs.",
#endif
    NGPON_RX_BWMAP_BWCNT_ASSOC_REG_OFFSET,
    0,
    0,
    41,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    8,
    NGPON_RX_BWMAP_BWCNT_ASSOC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_BWMAP
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_BWMAP_REGS[] =
{
    &NGPON_RX_BWMAP_TRFC_ALLOCID_REG,
    &NGPON_RX_BWMAP_RNGNG_ALLOCID_REG,
    &NGPON_RX_BWMAP_TRFC_ALLOCID_EN1_REG,
    &NGPON_RX_BWMAP_TRFC_ALLOCID_EN2_REG,
    &NGPON_RX_BWMAP_RNGNG_ALLOCID_EN_REG,
    &NGPON_RX_BWMAP_TRFC_TCNT_ASSOC_REG,
    &NGPON_RX_BWMAP_RNGNG_TCNT_ASSOC_REG,
    &NGPON_RX_BWMAP_BWCNT_ASSOC_REG,
};

unsigned long NGPON_RX_BWMAP_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80160100,
#elif defined(CONFIG_BCM96856)
    0x82db4100,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_BWMAP_BLOCK = 
{
    "NGPON_RX_BWMAP",
    NGPON_RX_BWMAP_ADDRS,
    1,
    8,
    NGPON_RX_BWMAP_REGS
};

/* End of file BCM6858_A0_NGPON_RX_BWMAP.c */
