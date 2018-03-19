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
 * Field: NGPON_RX_TOD_CFG_REQ_PHASE
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_CFG_REQ_PHASE_FIELD =
{
    "REQ_PHASE",
#if RU_INCLUDE_DESC
    "Phase_difference",
    "The desired phase difference between the free-running 8kHz output clock, and the incoming frame stream. When the phase difference is 0, the rising edge of the clock will coincide with the frame start."
    "When it is 1, the rising edge of the clock will be delayed by one internal clock cycle after the frame start."
    "",
#endif
    NGPON_RX_TOD_CFG_REQ_PHASE_FIELD_MASK,
    0,
    NGPON_RX_TOD_CFG_REQ_PHASE_FIELD_WIDTH,
    NGPON_RX_TOD_CFG_REQ_PHASE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_CFG_INCR
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_CFG_INCR_FIELD =
{
    "INCR",
#if RU_INCLUDE_DESC
    "Incremental_phase_change",
    "When negated, the phase lock mechanism is disabled, and the 8kHz clock changes immediately. When asserted, the clock changes incrementally.",
#endif
    NGPON_RX_TOD_CFG_INCR_FIELD_MASK,
    0,
    NGPON_RX_TOD_CFG_INCR_FIELD_WIDTH,
    NGPON_RX_TOD_CFG_INCR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_CFG_LOF_BEH
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_CFG_LOF_BEH_FIELD =
{
    "LOF_BEH",
#if RU_INCLUDE_DESC
    "LOF_behaviour",
    "When negated, clock output is active regardless of LOF state. When asserted, output clock is active only when RX is synchronized to the incoming data stream.",
#endif
    NGPON_RX_TOD_CFG_LOF_BEH_FIELD_MASK,
    0,
    NGPON_RX_TOD_CFG_LOF_BEH_FIELD_WIDTH,
    NGPON_RX_TOD_CFG_LOF_BEH_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_TOD_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_RX_TOD_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_CONG_THR_ASSERT
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_CONG_THR_ASSERT_FIELD =
{
    "ASSERT",
#if RU_INCLUDE_DESC
    "assert",
    "Threshold for moving into congestion."
    ""
    "When crossed, a congestion signal will be raised towards the TM",
#endif
    NGPON_RX_TOD_CONG_THR_ASSERT_FIELD_MASK,
    0,
    NGPON_RX_TOD_CONG_THR_ASSERT_FIELD_WIDTH,
    NGPON_RX_TOD_CONG_THR_ASSERT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_CONG_THR_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_CONG_THR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_CONG_THR_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_TOD_CONG_THR_RESERVED0_FIELD_WIDTH,
    NGPON_RX_TOD_CONG_THR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_CONG_THR_NEGATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_CONG_THR_NEGATE_FIELD =
{
    "NEGATE",
#if RU_INCLUDE_DESC
    "negate",
    "Threshold for moving out of congestion."
    ""
    "when crossed from above, the congestion signal towards the TM will be negated.",
#endif
    NGPON_RX_TOD_CONG_THR_NEGATE_FIELD_MASK,
    0,
    NGPON_RX_TOD_CONG_THR_NEGATE_FIELD_WIDTH,
    NGPON_RX_TOD_CONG_THR_NEGATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_CONG_THR_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_CONG_THR_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_CONG_THR_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_TOD_CONG_THR_RESERVED1_FIELD_WIDTH,
    NGPON_RX_TOD_CONG_THR_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_UPDATE_TIME_LS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_UPDATE_TIME_LS_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "the SFC in which the update will take place",
#endif
    NGPON_RX_TOD_UPDATE_TIME_LS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_TOD_UPDATE_TIME_LS_VALUE_FIELD_WIDTH,
    NGPON_RX_TOD_UPDATE_TIME_LS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_UPDATE_TIME_MS_MS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_UPDATE_TIME_MS_MS_VALUE_FIELD =
{
    "MS_VALUE",
#if RU_INCLUDE_DESC
    "ms_value",
    "the SFC in which the update will take place",
#endif
    NGPON_RX_TOD_UPDATE_TIME_MS_MS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_TOD_UPDATE_TIME_MS_MS_VALUE_FIELD_WIDTH,
    NGPON_RX_TOD_UPDATE_TIME_MS_MS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_UPDATE_TIME_MS_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_UPDATE_TIME_MS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_UPDATE_TIME_MS_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_TOD_UPDATE_TIME_MS_RESERVED0_FIELD_WIDTH,
    NGPON_RX_TOD_UPDATE_TIME_MS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_UPDATE_VAL_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_UPDATE_VAL_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "The Time of Day value",
#endif
    NGPON_RX_TOD_UPDATE_VAL_VALUE_FIELD_MASK,
    0,
    NGPON_RX_TOD_UPDATE_VAL_VALUE_FIELD_WIDTH,
    NGPON_RX_TOD_UPDATE_VAL_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_UPDATE_ARM_ARM
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_UPDATE_ARM_ARM_FIELD =
{
    "ARM",
#if RU_INCLUDE_DESC
    "arm",
    "Arms the update. When the SW is done updating the update values, it should set the arm bit. This enables configuring the update frame number to be like the current frame number without causing a false update.",
#endif
    NGPON_RX_TOD_UPDATE_ARM_ARM_FIELD_MASK,
    0,
    NGPON_RX_TOD_UPDATE_ARM_ARM_FIELD_WIDTH,
    NGPON_RX_TOD_UPDATE_ARM_ARM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_UPDATE_ARM_EN
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_UPDATE_ARM_EN_FIELD =
{
    "EN",
#if RU_INCLUDE_DESC
    "update_en",
    "Enables the update of the TOD and TS.",
#endif
    NGPON_RX_TOD_UPDATE_ARM_EN_FIELD_MASK,
    0,
    NGPON_RX_TOD_UPDATE_ARM_EN_FIELD_WIDTH,
    NGPON_RX_TOD_UPDATE_ARM_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_UPDATE_ARM_RDARM
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_UPDATE_ARM_RDARM_FIELD =
{
    "RDARM",
#if RU_INCLUDE_DESC
    "rd_arm",
    "Arms the read of the TOD and TS, meaning latches the value of these register and make them available for SW read through the read val registers",
#endif
    NGPON_RX_TOD_UPDATE_ARM_RDARM_FIELD_MASK,
    0,
    NGPON_RX_TOD_UPDATE_ARM_RDARM_FIELD_WIDTH,
    NGPON_RX_TOD_UPDATE_ARM_RDARM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_UPDATE_ARM_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_UPDATE_ARM_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_UPDATE_ARM_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_TOD_UPDATE_ARM_RESERVED0_FIELD_WIDTH,
    NGPON_RX_TOD_UPDATE_ARM_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_NANO_IN_SECS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_NANO_IN_SECS_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Number of GPON clock cycles in one second - 1",
#endif
    NGPON_RX_TOD_NANO_IN_SECS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_TOD_NANO_IN_SECS_VALUE_FIELD_WIDTH,
    NGPON_RX_TOD_NANO_IN_SECS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_NANO_IN_SECS_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_NANO_IN_SECS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_NANO_IN_SECS_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_TOD_NANO_IN_SECS_RESERVED0_FIELD_WIDTH,
    NGPON_RX_TOD_NANO_IN_SECS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_OPPS_DUTY_CYCLE_TIME
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_OPPS_DUTY_CYCLE_TIME_FIELD =
{
    "DUTY_CYCLE_TIME",
#if RU_INCLUDE_DESC
    "duty_cycle_time",
    "Number of GPON clock cycles in which the 1pps clock will be asserted high.",
#endif
    NGPON_RX_TOD_OPPS_DUTY_CYCLE_TIME_FIELD_MASK,
    0,
    NGPON_RX_TOD_OPPS_DUTY_CYCLE_TIME_FIELD_WIDTH,
    NGPON_RX_TOD_OPPS_DUTY_CYCLE_TIME_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_OPPS_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_OPPS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_OPPS_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_TOD_OPPS_RESERVED0_FIELD_WIDTH,
    NGPON_RX_TOD_OPPS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_OPPS_EN
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_OPPS_EN_FIELD =
{
    "EN",
#if RU_INCLUDE_DESC
    "en",
    "Enable bit for generating 1pps clock",
#endif
    NGPON_RX_TOD_OPPS_EN_FIELD_MASK,
    0,
    NGPON_RX_TOD_OPPS_EN_FIELD_WIDTH,
    NGPON_RX_TOD_OPPS_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_NANOS_PER_CYCLE_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_NANOS_PER_CYCLE_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Number of full nano seconds per GPON CLOCK",
#endif
    NGPON_RX_TOD_NANOS_PER_CYCLE_VALUE_FIELD_MASK,
    0,
    NGPON_RX_TOD_NANOS_PER_CYCLE_VALUE_FIELD_WIDTH,
    NGPON_RX_TOD_NANOS_PER_CYCLE_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_NANOS_PER_CYCLE_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_NANOS_PER_CYCLE_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_NANOS_PER_CYCLE_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_TOD_NANOS_PER_CYCLE_RESERVED0_FIELD_WIDTH,
    NGPON_RX_TOD_NANOS_PER_CYCLE_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_PARTIAL_NS_INC_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_PARTIAL_NS_INC_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Number of ns parts to be added per GPON CLOCK",
#endif
    NGPON_RX_TOD_PARTIAL_NS_INC_VALUE_FIELD_MASK,
    0,
    NGPON_RX_TOD_PARTIAL_NS_INC_VALUE_FIELD_WIDTH,
    NGPON_RX_TOD_PARTIAL_NS_INC_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_PARTIAL_NS_INC_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_PARTIAL_NS_INC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_PARTIAL_NS_INC_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_TOD_PARTIAL_NS_INC_RESERVED0_FIELD_WIDTH,
    NGPON_RX_TOD_PARTIAL_NS_INC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_PARTIAL_NS_IN_NANO_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_PARTIAL_NS_IN_NANO_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Number of ns parts per nano",
#endif
    NGPON_RX_TOD_PARTIAL_NS_IN_NANO_VALUE_FIELD_MASK,
    0,
    NGPON_RX_TOD_PARTIAL_NS_IN_NANO_VALUE_FIELD_WIDTH,
    NGPON_RX_TOD_PARTIAL_NS_IN_NANO_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_PARTIAL_NS_IN_NANO_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_PARTIAL_NS_IN_NANO_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_TOD_PARTIAL_NS_IN_NANO_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_TOD_PARTIAL_NS_IN_NANO_RESERVED0_FIELD_WIDTH,
    NGPON_RX_TOD_PARTIAL_NS_IN_NANO_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_TS_UP_VAL_TS_VAL
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_TS_UP_VAL_TS_VAL_FIELD =
{
    "TS_VAL",
#if RU_INCLUDE_DESC
    "ts_val",
    "The Time stamp value",
#endif
    NGPON_RX_TOD_TS_UP_VAL_TS_VAL_FIELD_MASK,
    0,
    NGPON_RX_TOD_TS_UP_VAL_TS_VAL_FIELD_WIDTH,
    NGPON_RX_TOD_TS_UP_VAL_TS_VAL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_TOD_RD_VAL_TS_VAL
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_TOD_RD_VAL_TS_VAL_FIELD =
{
    "TS_VAL",
#if RU_INCLUDE_DESC
    "ts_val",
    "The Time stamp value",
#endif
    NGPON_RX_TOD_TOD_RD_VAL_TS_VAL_FIELD_MASK,
    0,
    NGPON_RX_TOD_TOD_RD_VAL_TS_VAL_FIELD_WIDTH,
    NGPON_RX_TOD_TOD_RD_VAL_TS_VAL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_TOD_TS_RD_VAL_TS_VAL
 ******************************************************************************/
const ru_field_rec NGPON_RX_TOD_TS_RD_VAL_TS_VAL_FIELD =
{
    "TS_VAL",
#if RU_INCLUDE_DESC
    "ts_val",
    "The Time stamp value",
#endif
    NGPON_RX_TOD_TS_RD_VAL_TS_VAL_FIELD_MASK,
    0,
    NGPON_RX_TOD_TS_RD_VAL_TS_VAL_FIELD_WIDTH,
    NGPON_RX_TOD_TS_RD_VAL_TS_VAL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_TOD_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_CFG_FIELDS[] =
{
    &NGPON_RX_TOD_CFG_REQ_PHASE_FIELD,
    &NGPON_RX_TOD_CFG_INCR_FIELD,
    &NGPON_RX_TOD_CFG_LOF_BEH_FIELD,
    &NGPON_RX_TOD_CFG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_CFG_REG = 
{
    "CFG",
#if RU_INCLUDE_DESC
    "CONFIGURATION Register",
    "This register provides control over the 8kHz clock phase lock mechanism."
    "It enables/disables the phase lock mechanism and controls the desired phase difference between the output free-running 8kHz clock and the input frames stream."
    "",
#endif
    NGPON_RX_TOD_CFG_REG_OFFSET,
    0,
    0,
    23,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_RX_TOD_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_CONG_THR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_CONG_THR_FIELDS[] =
{
    &NGPON_RX_TOD_CONG_THR_ASSERT_FIELD,
    &NGPON_RX_TOD_CONG_THR_RESERVED0_FIELD,
    &NGPON_RX_TOD_CONG_THR_NEGATE_FIELD,
    &NGPON_RX_TOD_CONG_THR_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_CONG_THR_REG = 
{
    "CONG_THR",
#if RU_INCLUDE_DESC
    "CONGESTION_THRESHOLDS Register",
    "Into congestion and out of congestion thresholds."
    ""
    "The thresholds are in number of RX fifo entries (each entry in the FIFO is of 8 bytes)."
    ""
    "Can only be changed when the module is disabled.",
#endif
    NGPON_RX_TOD_CONG_THR_REG_OFFSET,
    0,
    0,
    24,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_RX_TOD_CONG_THR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_UPDATE_TIME_LS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_UPDATE_TIME_LS_FIELDS[] =
{
    &NGPON_RX_TOD_UPDATE_TIME_LS_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_UPDATE_TIME_LS_REG = 
{
    "UPDATE_TIME_LS",
#if RU_INCLUDE_DESC
    "TOD_UPDATE_TIME_LS Register",
    "The update mechanism of the TOD clock counter is similar to the GPON AES key switch mechanism. The SW indicates the frame number in which there will be update, the update value and assert ARM bit. The HW will update the TOD accordingly. This register is setting the frame number in which the update will take place.",
#endif
    NGPON_RX_TOD_UPDATE_TIME_LS_REG_OFFSET,
    0,
    0,
    25,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_TOD_UPDATE_TIME_LS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_UPDATE_TIME_MS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_UPDATE_TIME_MS_FIELDS[] =
{
    &NGPON_RX_TOD_UPDATE_TIME_MS_MS_VALUE_FIELD,
    &NGPON_RX_TOD_UPDATE_TIME_MS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_UPDATE_TIME_MS_REG = 
{
    "UPDATE_TIME_MS",
#if RU_INCLUDE_DESC
    "TOD_UPDATE_TIME_MS Register",
    "The update mechanism of the TOD clock counter is similar to the GPON AES key switch mechanism. The SW indicates the frame number in which there will be update, the update value and assert ARM bit. The HW will update the TOD accordingly. This register is setting the frame number in which the update will take place.",
#endif
    NGPON_RX_TOD_UPDATE_TIME_MS_REG_OFFSET,
    0,
    0,
    26,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_TOD_UPDATE_TIME_MS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_UPDATE_VAL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_UPDATE_VAL_FIELDS[] =
{
    &NGPON_RX_TOD_UPDATE_VAL_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_UPDATE_VAL_REG = 
{
    "UPDATE_VAL",
#if RU_INCLUDE_DESC
    "TOD_UPDATE_VAL %i Register",
    "This 2 registers hold the value of the TOD which the TOD counter should be updated to. The 30 lsb hold the nano seconds value and the 34 msb hold seconds value."
    "The first register in the array is the lsb.",
#endif
    NGPON_RX_TOD_UPDATE_VAL_REG_OFFSET,
    NGPON_RX_TOD_UPDATE_VAL_REG_RAM_CNT,
    4,
    27,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_TOD_UPDATE_VAL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_UPDATE_ARM
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_UPDATE_ARM_FIELDS[] =
{
    &NGPON_RX_TOD_UPDATE_ARM_ARM_FIELD,
    &NGPON_RX_TOD_UPDATE_ARM_EN_FIELD,
    &NGPON_RX_TOD_UPDATE_ARM_RDARM_FIELD,
    &NGPON_RX_TOD_UPDATE_ARM_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_UPDATE_ARM_REG = 
{
    "UPDATE_ARM",
#if RU_INCLUDE_DESC
    "TOD_UPDATE_ARM Register",
    "The arm bit is used by the SW for indicating the update values are valid. The enable bit is used to disable an update which is already armed.",
#endif
    NGPON_RX_TOD_UPDATE_ARM_REG_OFFSET,
    0,
    0,
    28,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_RX_TOD_UPDATE_ARM_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_NANO_IN_SECS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_NANO_IN_SECS_FIELDS[] =
{
    &NGPON_RX_TOD_NANO_IN_SECS_VALUE_FIELD,
    &NGPON_RX_TOD_NANO_IN_SECS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_NANO_IN_SECS_REG = 
{
    "NANO_IN_SECS",
#if RU_INCLUDE_DESC
    "TOD_CLOCKS_IN_SEC Register",
    "The TOD counter counts GPON clock cycles. This register holds the number of GPON clock cycles in one second - 1."
    "When the Tod counter reaches this value it will add 1 to the TOD seconds counter and will generate 1PPs clock pulse."
    "",
#endif
    NGPON_RX_TOD_NANO_IN_SECS_REG_OFFSET,
    0,
    0,
    29,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_TOD_NANO_IN_SECS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_OPPS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_OPPS_FIELDS[] =
{
    &NGPON_RX_TOD_OPPS_DUTY_CYCLE_TIME_FIELD,
    &NGPON_RX_TOD_OPPS_RESERVED0_FIELD,
    &NGPON_RX_TOD_OPPS_EN_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_OPPS_REG = 
{
    "OPPS",
#if RU_INCLUDE_DESC
    "OPPS Register",
    "1PPS clock configuration:"
    ""
    "this register holds the enable and the number of GPON clock cycles in which the 1pps clock will be asserted high. The clock will be asserted low for:"
    "clocks_in_sec - duty_cycle."
    ""
    "for clocks_in_sec description see previous register",
#endif
    NGPON_RX_TOD_OPPS_REG_OFFSET,
    0,
    0,
    30,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    NGPON_RX_TOD_OPPS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_NANOS_PER_CYCLE
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_NANOS_PER_CYCLE_FIELDS[] =
{
    &NGPON_RX_TOD_NANOS_PER_CYCLE_VALUE_FIELD,
    &NGPON_RX_TOD_NANOS_PER_CYCLE_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_NANOS_PER_CYCLE_REG = 
{
    "NANOS_PER_CYCLE",
#if RU_INCLUDE_DESC
    "NANOS_PER_CYCLE Register",
    "number of complete nano seconds to add to the TOD counter per cycle."
    "",
#endif
    NGPON_RX_TOD_NANOS_PER_CYCLE_REG_OFFSET,
    0,
    0,
    31,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_TOD_NANOS_PER_CYCLE_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_PARTIAL_NS_INC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_PARTIAL_NS_INC_FIELDS[] =
{
    &NGPON_RX_TOD_PARTIAL_NS_INC_VALUE_FIELD,
    &NGPON_RX_TOD_PARTIAL_NS_INC_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_PARTIAL_NS_INC_REG = 
{
    "PARTIAL_NS_INC",
#if RU_INCLUDE_DESC
    "PARTIAL_NS_INC Register",
    "number of parts of nano seconds to add to the TOD counter per cycle."
    "",
#endif
    NGPON_RX_TOD_PARTIAL_NS_INC_REG_OFFSET,
    0,
    0,
    32,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_TOD_PARTIAL_NS_INC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_PARTIAL_NS_IN_NANO
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_PARTIAL_NS_IN_NANO_FIELDS[] =
{
    &NGPON_RX_TOD_PARTIAL_NS_IN_NANO_VALUE_FIELD,
    &NGPON_RX_TOD_PARTIAL_NS_IN_NANO_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_PARTIAL_NS_IN_NANO_REG = 
{
    "PARTIAL_NS_IN_NANO",
#if RU_INCLUDE_DESC
    "PARTIAL_NS_IN_NANO Register",
    "number of parts of nano seconds per_nano."
    "",
#endif
    NGPON_RX_TOD_PARTIAL_NS_IN_NANO_REG_OFFSET,
    0,
    0,
    33,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_TOD_PARTIAL_NS_IN_NANO_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_TS_UP_VAL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_TS_UP_VAL_FIELDS[] =
{
    &NGPON_RX_TOD_TS_UP_VAL_TS_VAL_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_TS_UP_VAL_REG = 
{
    "TS_UP_VAL",
#if RU_INCLUDE_DESC
    "TS_UPDATE_VAL %i Register",
    "This 2 registers hold the value of the TS which the TS counter should be updated to. 48 bits in nano seconds."
    "The first register in the array is the lsb.",
#endif
    NGPON_RX_TOD_TS_UP_VAL_REG_OFFSET,
    NGPON_RX_TOD_TS_UP_VAL_REG_RAM_CNT,
    4,
    34,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_TOD_TS_UP_VAL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_TOD_RD_VAL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_TOD_RD_VAL_FIELDS[] =
{
    &NGPON_RX_TOD_TOD_RD_VAL_TS_VAL_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_TOD_RD_VAL_REG = 
{
    "TOD_RD_VAL",
#if RU_INCLUDE_DESC
    "TOD_READ_VAL %i Register",
    "Current 64 bit TOD value."
    "first reg is the lsb.",
#endif
    NGPON_RX_TOD_TOD_RD_VAL_REG_OFFSET,
    NGPON_RX_TOD_TOD_RD_VAL_REG_RAM_CNT,
    4,
    35,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_TOD_TOD_RD_VAL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_TOD_TS_RD_VAL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_TOD_TS_RD_VAL_FIELDS[] =
{
    &NGPON_RX_TOD_TS_RD_VAL_TS_VAL_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_TOD_TS_RD_VAL_REG = 
{
    "TS_RD_VAL",
#if RU_INCLUDE_DESC
    "TS_READ_VAL %i Register",
    "Current 48 bit TS value."
    "first reg is the lsb.",
#endif
    NGPON_RX_TOD_TS_RD_VAL_REG_OFFSET,
    NGPON_RX_TOD_TS_RD_VAL_REG_RAM_CNT,
    4,
    36,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_TOD_TS_RD_VAL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_TOD
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_TOD_REGS[] =
{
    &NGPON_RX_TOD_CFG_REG,
    &NGPON_RX_TOD_CONG_THR_REG,
    &NGPON_RX_TOD_UPDATE_TIME_LS_REG,
    &NGPON_RX_TOD_UPDATE_TIME_MS_REG,
    &NGPON_RX_TOD_UPDATE_VAL_REG,
    &NGPON_RX_TOD_UPDATE_ARM_REG,
    &NGPON_RX_TOD_NANO_IN_SECS_REG,
    &NGPON_RX_TOD_OPPS_REG,
    &NGPON_RX_TOD_NANOS_PER_CYCLE_REG,
    &NGPON_RX_TOD_PARTIAL_NS_INC_REG,
    &NGPON_RX_TOD_PARTIAL_NS_IN_NANO_REG,
    &NGPON_RX_TOD_TS_UP_VAL_REG,
    &NGPON_RX_TOD_TOD_RD_VAL_REG,
    &NGPON_RX_TOD_TS_RD_VAL_REG,
};

static unsigned long NGPON_RX_TOD_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x801600a0,
#elif defined(CONFIG_BCM96856)
    0x82db40c0,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_TOD_BLOCK = 
{
    "NGPON_RX_TOD",
    NGPON_RX_TOD_ADDRS,
    1,
    14,
    NGPON_RX_TOD_REGS
};

/* End of file BCM6858_B0_NGPON_RX_TOD.c */
