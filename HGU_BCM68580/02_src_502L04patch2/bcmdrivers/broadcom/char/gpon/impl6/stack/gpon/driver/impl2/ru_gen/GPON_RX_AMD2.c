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
 * Field: GPON_RX_AMD2_TOD_SF_UP_UP_SF
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TOD_SF_UP_UP_SF_FIELD =
{
    "UP_SF",
#if RU_INCLUDE_DESC
    "update_sf",
    "the SF in which the update will take place",
#endif
    GPON_RX_AMD2_TOD_SF_UP_UP_SF_FIELD_MASK,
    0,
    GPON_RX_AMD2_TOD_SF_UP_UP_SF_FIELD_WIDTH,
    GPON_RX_AMD2_TOD_SF_UP_UP_SF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TOD_SF_UP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TOD_SF_UP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_AMD2_TOD_SF_UP_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_AMD2_TOD_SF_UP_RESERVED0_FIELD_WIDTH,
    GPON_RX_AMD2_TOD_SF_UP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TOD_UP_VAL_TOD_VAL
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TOD_UP_VAL_TOD_VAL_FIELD =
{
    "TOD_VAL",
#if RU_INCLUDE_DESC
    "tod_val",
    "The Time of Day value",
#endif
    GPON_RX_AMD2_TOD_UP_VAL_TOD_VAL_FIELD_MASK,
    0,
    GPON_RX_AMD2_TOD_UP_VAL_TOD_VAL_FIELD_WIDTH,
    GPON_RX_AMD2_TOD_UP_VAL_TOD_VAL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_UP_ARM_UPARM
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_UP_ARM_UPARM_FIELD =
{
    "UPARM",
#if RU_INCLUDE_DESC
    "update_arm",
    "Arms the update. When the SW is done updating the update values, it should set the arm bit. This enables configuring the update frame number to be like the current frame number without causing a false update.",
#endif
    GPON_RX_AMD2_UP_ARM_UPARM_FIELD_MASK,
    0,
    GPON_RX_AMD2_UP_ARM_UPARM_FIELD_WIDTH,
    GPON_RX_AMD2_UP_ARM_UPARM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_UP_ARM_EN
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_UP_ARM_EN_FIELD =
{
    "EN",
#if RU_INCLUDE_DESC
    "update_en",
    "Enables the update of the TOD and TS.",
#endif
    GPON_RX_AMD2_UP_ARM_EN_FIELD_MASK,
    0,
    GPON_RX_AMD2_UP_ARM_EN_FIELD_WIDTH,
    GPON_RX_AMD2_UP_ARM_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_UP_ARM_RDARM
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_UP_ARM_RDARM_FIELD =
{
    "RDARM",
#if RU_INCLUDE_DESC
    "rd_arm",
    "Arms the read of the TOD and TS, meaning latches the value of these register and make them available for SW read through the read val registers",
#endif
    GPON_RX_AMD2_UP_ARM_RDARM_FIELD_MASK,
    0,
    GPON_RX_AMD2_UP_ARM_RDARM_FIELD_WIDTH,
    GPON_RX_AMD2_UP_ARM_RDARM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_UP_ARM_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_UP_ARM_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_AMD2_UP_ARM_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_AMD2_UP_ARM_RESERVED0_FIELD_WIDTH,
    GPON_RX_AMD2_UP_ARM_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TOD_CYC_IN_SEC_CYC_IN_SEC
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TOD_CYC_IN_SEC_CYC_IN_SEC_FIELD =
{
    "CYC_IN_SEC",
#if RU_INCLUDE_DESC
    "cycles_in_sec",
    "Number of GPON clock cycles in one second - 1",
#endif
    GPON_RX_AMD2_TOD_CYC_IN_SEC_CYC_IN_SEC_FIELD_MASK,
    0,
    GPON_RX_AMD2_TOD_CYC_IN_SEC_CYC_IN_SEC_FIELD_WIDTH,
    GPON_RX_AMD2_TOD_CYC_IN_SEC_CYC_IN_SEC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TOD_CYC_IN_SEC_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TOD_CYC_IN_SEC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_AMD2_TOD_CYC_IN_SEC_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_AMD2_TOD_CYC_IN_SEC_RESERVED0_FIELD_WIDTH,
    GPON_RX_AMD2_TOD_CYC_IN_SEC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TOD_OPPS_DUTY_DUTY_CYCLE
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TOD_OPPS_DUTY_DUTY_CYCLE_FIELD =
{
    "DUTY_CYCLE",
#if RU_INCLUDE_DESC
    "duty_cycle",
    "Number of GPON clock cycles in which the 1pps clock will be asserted high.",
#endif
    GPON_RX_AMD2_TOD_OPPS_DUTY_DUTY_CYCLE_FIELD_MASK,
    0,
    GPON_RX_AMD2_TOD_OPPS_DUTY_DUTY_CYCLE_FIELD_WIDTH,
    GPON_RX_AMD2_TOD_OPPS_DUTY_DUTY_CYCLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TOD_OPPS_DUTY_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TOD_OPPS_DUTY_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_AMD2_TOD_OPPS_DUTY_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_AMD2_TOD_OPPS_DUTY_RESERVED0_FIELD_WIDTH,
    GPON_RX_AMD2_TOD_OPPS_DUTY_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TOD_OPPS_DUTY_OPPS_EN
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TOD_OPPS_DUTY_OPPS_EN_FIELD =
{
    "OPPS_EN",
#if RU_INCLUDE_DESC
    "opps_en",
    "Enable bit for generating 1pps clock",
#endif
    GPON_RX_AMD2_TOD_OPPS_DUTY_OPPS_EN_FIELD_MASK,
    0,
    GPON_RX_AMD2_TOD_OPPS_DUTY_OPPS_EN_FIELD_WIDTH,
    GPON_RX_AMD2_TOD_OPPS_DUTY_OPPS_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_NANOS_PER_CLK_NS_IN_CLK
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_NANOS_PER_CLK_NS_IN_CLK_FIELD =
{
    "NS_IN_CLK",
#if RU_INCLUDE_DESC
    "nanos_per_clk",
    "Number of full nano seconds per GPON CLOCK",
#endif
    GPON_RX_AMD2_NANOS_PER_CLK_NS_IN_CLK_FIELD_MASK,
    0,
    GPON_RX_AMD2_NANOS_PER_CLK_NS_IN_CLK_FIELD_WIDTH,
    GPON_RX_AMD2_NANOS_PER_CLK_NS_IN_CLK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_NANOS_PER_CLK_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_NANOS_PER_CLK_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_AMD2_NANOS_PER_CLK_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_AMD2_NANOS_PER_CLK_RESERVED0_FIELD_WIDTH,
    GPON_RX_AMD2_NANOS_PER_CLK_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_NSPARTINC_NSPINC
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_NSPARTINC_NSPINC_FIELD =
{
    "NSPINC",
#if RU_INCLUDE_DESC
    "nanos_parts_inc",
    "Number of ns parts to be added per GPON CLOCK",
#endif
    GPON_RX_AMD2_NSPARTINC_NSPINC_FIELD_MASK,
    0,
    GPON_RX_AMD2_NSPARTINC_NSPINC_FIELD_WIDTH,
    GPON_RX_AMD2_NSPARTINC_NSPINC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_NSPARTINC_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_NSPARTINC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_AMD2_NSPARTINC_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_AMD2_NSPARTINC_RESERVED0_FIELD_WIDTH,
    GPON_RX_AMD2_NSPARTINC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_NSP_IN_NANO_NSP_IN_NANO
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_NSP_IN_NANO_NSP_IN_NANO_FIELD =
{
    "NSP_IN_NANO",
#if RU_INCLUDE_DESC
    "nanos_parts_in_nano",
    "Number of ns parts per nano",
#endif
    GPON_RX_AMD2_NSP_IN_NANO_NSP_IN_NANO_FIELD_MASK,
    0,
    GPON_RX_AMD2_NSP_IN_NANO_NSP_IN_NANO_FIELD_WIDTH,
    GPON_RX_AMD2_NSP_IN_NANO_NSP_IN_NANO_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_NSP_IN_NANO_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_NSP_IN_NANO_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_AMD2_NSP_IN_NANO_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_AMD2_NSP_IN_NANO_RESERVED0_FIELD_WIDTH,
    GPON_RX_AMD2_NSP_IN_NANO_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TS_UP_VAL_TS_VAL
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TS_UP_VAL_TS_VAL_FIELD =
{
    "TS_VAL",
#if RU_INCLUDE_DESC
    "ts_val",
    "The Time stamp value",
#endif
    GPON_RX_AMD2_TS_UP_VAL_TS_VAL_FIELD_MASK,
    0,
    GPON_RX_AMD2_TS_UP_VAL_TS_VAL_FIELD_WIDTH,
    GPON_RX_AMD2_TS_UP_VAL_TS_VAL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TOD_RD_VAL_TS_VAL
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TOD_RD_VAL_TS_VAL_FIELD =
{
    "TS_VAL",
#if RU_INCLUDE_DESC
    "ts_val",
    "The Time stamp value",
#endif
    GPON_RX_AMD2_TOD_RD_VAL_TS_VAL_FIELD_MASK,
    0,
    GPON_RX_AMD2_TOD_RD_VAL_TS_VAL_FIELD_WIDTH,
    GPON_RX_AMD2_TOD_RD_VAL_TS_VAL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_AMD2_TS_RD_VAL_TS_VAL
 ******************************************************************************/
const ru_field_rec GPON_RX_AMD2_TS_RD_VAL_TS_VAL_FIELD =
{
    "TS_VAL",
#if RU_INCLUDE_DESC
    "ts_val",
    "The Time stamp value",
#endif
    GPON_RX_AMD2_TS_RD_VAL_TS_VAL_FIELD_MASK,
    0,
    GPON_RX_AMD2_TS_RD_VAL_TS_VAL_FIELD_WIDTH,
    GPON_RX_AMD2_TS_RD_VAL_TS_VAL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_AMD2_TOD_SF_UP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_TOD_SF_UP_FIELDS[] =
{
    &GPON_RX_AMD2_TOD_SF_UP_UP_SF_FIELD,
    &GPON_RX_AMD2_TOD_SF_UP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_TOD_SF_UP_REG = 
{
    "TOD_SF_UP",
#if RU_INCLUDE_DESC
    "TOD_UPDATE_TIME Register",
    "The update mechanism of the TOD clock counter is similar to the AES key switch mechanism. The SW indicates the frame number in which there will be update, the update value and assert ARM bit. The HW will update the TOD accordingly. This register is setting the frame number in which the update will take place.",
#endif
    GPON_RX_AMD2_TOD_SF_UP_REG_OFFSET,
    0,
    0,
    354,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_AMD2_TOD_SF_UP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_TOD_UP_VAL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_TOD_UP_VAL_FIELDS[] =
{
    &GPON_RX_AMD2_TOD_UP_VAL_TOD_VAL_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_TOD_UP_VAL_REG = 
{
    "TOD_UP_VAL",
#if RU_INCLUDE_DESC
    "TOD_UPDATE_VAL %i Register",
    "This 2 registers hold the value of the TOD which the TOD counter should be updated to. The 30 lsb hold the nano seconds value and the 34 msb hold seconds value."
    "The first register in the array is the lsb.",
#endif
    GPON_RX_AMD2_TOD_UP_VAL_REG_OFFSET,
    GPON_RX_AMD2_TOD_UP_VAL_REG_RAM_CNT,
    4,
    355,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_AMD2_TOD_UP_VAL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_UP_ARM
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_UP_ARM_FIELDS[] =
{
    &GPON_RX_AMD2_UP_ARM_UPARM_FIELD,
    &GPON_RX_AMD2_UP_ARM_EN_FIELD,
    &GPON_RX_AMD2_UP_ARM_RDARM_FIELD,
    &GPON_RX_AMD2_UP_ARM_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_UP_ARM_REG = 
{
    "UP_ARM",
#if RU_INCLUDE_DESC
    "UPDATE_ARM Register",
    "The arm bit is used by the SW for indicating the update values are valid. The enable bit is used to disable an update which is already armed.",
#endif
    GPON_RX_AMD2_UP_ARM_REG_OFFSET,
    0,
    0,
    356,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_RX_AMD2_UP_ARM_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_TOD_CYC_IN_SEC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_TOD_CYC_IN_SEC_FIELDS[] =
{
    &GPON_RX_AMD2_TOD_CYC_IN_SEC_CYC_IN_SEC_FIELD,
    &GPON_RX_AMD2_TOD_CYC_IN_SEC_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_TOD_CYC_IN_SEC_REG = 
{
    "TOD_CYC_IN_SEC",
#if RU_INCLUDE_DESC
    "TOD_NANOS_IN_SEC Register",
    "The TOD counter counts GPON clock cycles. This register holds the number of nano seconds in a complete second - 1. When the Tod counter reaches this value it will add 1 to the TOD seconds counter and will generate 1PPs clock pulse."
    "In real life scenarios should be configured to 1,000,000,000 (0x3b9aca00)"
    "",
#endif
    GPON_RX_AMD2_TOD_CYC_IN_SEC_REG_OFFSET,
    0,
    0,
    357,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_AMD2_TOD_CYC_IN_SEC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_TOD_OPPS_DUTY
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_TOD_OPPS_DUTY_FIELDS[] =
{
    &GPON_RX_AMD2_TOD_OPPS_DUTY_DUTY_CYCLE_FIELD,
    &GPON_RX_AMD2_TOD_OPPS_DUTY_RESERVED0_FIELD,
    &GPON_RX_AMD2_TOD_OPPS_DUTY_OPPS_EN_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_TOD_OPPS_DUTY_REG = 
{
    "TOD_OPPS_DUTY",
#if RU_INCLUDE_DESC
    "TOD_OPPS_DUTY_CYCLE Register",
    "this register holds the number of GPON clock cycles in which the 1pps clock will be asserted high. The clock will be asserted low for:"
    "cycles_in_se - duty_cycle."
    ""
    "for cycles_in_sec description see previos register",
#endif
    GPON_RX_AMD2_TOD_OPPS_DUTY_REG_OFFSET,
    0,
    0,
    358,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_RX_AMD2_TOD_OPPS_DUTY_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_NANOS_PER_CLK
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_NANOS_PER_CLK_FIELDS[] =
{
    &GPON_RX_AMD2_NANOS_PER_CLK_NS_IN_CLK_FIELD,
    &GPON_RX_AMD2_NANOS_PER_CLK_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_NANOS_PER_CLK_REG = 
{
    "NANOS_PER_CLK",
#if RU_INCLUDE_DESC
    "NANOS_PER_CLK Register",
    "number of complete nano seconds to add to the TOD counter per cycle."
    "For GPON it should be 6, as the lock cycle is 6.43ns",
#endif
    GPON_RX_AMD2_NANOS_PER_CLK_REG_OFFSET,
    0,
    0,
    359,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_AMD2_NANOS_PER_CLK_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_NSPARTINC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_NSPARTINC_FIELDS[] =
{
    &GPON_RX_AMD2_NSPARTINC_NSPINC_FIELD,
    &GPON_RX_AMD2_NSPARTINC_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_NSPARTINC_REG = 
{
    "NSPARTINC",
#if RU_INCLUDE_DESC
    "NANO_PARTS_INC Register",
    "number of parts of nano seconds to add to the TOD counter per cycle."
    "For GPON it should be 209, in order to achieve 0.43 by dividing 209/486.",
#endif
    GPON_RX_AMD2_NSPARTINC_REG_OFFSET,
    0,
    0,
    360,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_AMD2_NSPARTINC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_NSP_IN_NANO
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_NSP_IN_NANO_FIELDS[] =
{
    &GPON_RX_AMD2_NSP_IN_NANO_NSP_IN_NANO_FIELD,
    &GPON_RX_AMD2_NSP_IN_NANO_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_NSP_IN_NANO_REG = 
{
    "NSP_IN_NANO",
#if RU_INCLUDE_DESC
    "NANO_PARTS_IN_NANO Register",
    "number of parts of nano seconds per_nano."
    "For GPON it should be 486, in order to achieve 0.43 by dividing 209/486.",
#endif
    GPON_RX_AMD2_NSP_IN_NANO_REG_OFFSET,
    0,
    0,
    361,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_AMD2_NSP_IN_NANO_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_TS_UP_VAL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_TS_UP_VAL_FIELDS[] =
{
    &GPON_RX_AMD2_TS_UP_VAL_TS_VAL_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_TS_UP_VAL_REG = 
{
    "TS_UP_VAL",
#if RU_INCLUDE_DESC
    "TS_UPDATE_VAL %i Register",
    "This 2 registers hold the value of the TS which the TS counter should be updated to. 48 bits in nano seconds."
    "The first register in the array is the lsb.",
#endif
    GPON_RX_AMD2_TS_UP_VAL_REG_OFFSET,
    GPON_RX_AMD2_TS_UP_VAL_REG_RAM_CNT,
    4,
    362,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_AMD2_TS_UP_VAL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_TOD_RD_VAL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_TOD_RD_VAL_FIELDS[] =
{
    &GPON_RX_AMD2_TOD_RD_VAL_TS_VAL_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_TOD_RD_VAL_REG = 
{
    "TOD_RD_VAL",
#if RU_INCLUDE_DESC
    "TOD_READ_VAL %i Register",
    "Current 64 bit TOD value."
    "first reg is the lsb.",
#endif
    GPON_RX_AMD2_TOD_RD_VAL_REG_OFFSET,
    GPON_RX_AMD2_TOD_RD_VAL_REG_RAM_CNT,
    4,
    363,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_AMD2_TOD_RD_VAL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_AMD2_TS_RD_VAL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_AMD2_TS_RD_VAL_FIELDS[] =
{
    &GPON_RX_AMD2_TS_RD_VAL_TS_VAL_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_AMD2_TS_RD_VAL_REG = 
{
    "TS_RD_VAL",
#if RU_INCLUDE_DESC
    "TS_READ_VAL %i Register",
    "Current 48 bit TS value."
    "first reg is the lsb.",
#endif
    GPON_RX_AMD2_TS_RD_VAL_REG_OFFSET,
    GPON_RX_AMD2_TS_RD_VAL_REG_RAM_CNT,
    4,
    364,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_AMD2_TS_RD_VAL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_AMD2
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_AMD2_REGS[] =
{
    &GPON_RX_AMD2_TOD_SF_UP_REG,
    &GPON_RX_AMD2_TOD_UP_VAL_REG,
    &GPON_RX_AMD2_UP_ARM_REG,
    &GPON_RX_AMD2_TOD_CYC_IN_SEC_REG,
    &GPON_RX_AMD2_TOD_OPPS_DUTY_REG,
    &GPON_RX_AMD2_NANOS_PER_CLK_REG,
    &GPON_RX_AMD2_NSPARTINC_REG,
    &GPON_RX_AMD2_NSP_IN_NANO_REG,
    &GPON_RX_AMD2_TS_UP_VAL_REG,
    &GPON_RX_AMD2_TOD_RD_VAL_REG,
    &GPON_RX_AMD2_TS_RD_VAL_REG,
};

static unsigned long GPON_RX_AMD2_ADDRS[] =
{
    0x80150800,
};

const ru_block_rec GPON_RX_AMD2_BLOCK = 
{
    "GPON_RX_AMD2",
    GPON_RX_AMD2_ADDRS,
    1,
    11,
    GPON_RX_AMD2_REGS
};

/* End of file BCM6858_B0GPON_RX_AMD2.c */
