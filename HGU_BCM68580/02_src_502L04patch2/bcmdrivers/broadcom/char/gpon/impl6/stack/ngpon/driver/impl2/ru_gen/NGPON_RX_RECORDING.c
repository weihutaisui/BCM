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
 * Field: NGPON_RX_RECORDING_TRIG_FIRST_TRIG
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_TRIG_FIRST_TRIG_FIELD =
{
    "FIRST_TRIG",
#if RU_INCLUDE_DESC
    "first_trig",
    "Trigger for the first segment recorded",
#endif
    NGPON_RX_RECORDING_TRIG_FIRST_TRIG_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_TRIG_FIRST_TRIG_FIELD_WIDTH,
    NGPON_RX_RECORDING_TRIG_FIRST_TRIG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_TRIG_NEXT_TRIG
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_TRIG_NEXT_TRIG_FIELD =
{
    "NEXT_TRIG",
#if RU_INCLUDE_DESC
    "next_trig",
    "Trigger for additional segments (applicable if segment size < memory size)",
#endif
    NGPON_RX_RECORDING_TRIG_NEXT_TRIG_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_TRIG_NEXT_TRIG_FIELD_WIDTH,
    NGPON_RX_RECORDING_TRIG_NEXT_TRIG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_TRIG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_TRIG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_RECORDING_TRIG_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_TRIG_RESERVED0_FIELD_WIDTH,
    NGPON_RX_RECORDING_TRIG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_TRIG_TRIG_DELAY
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_TRIG_TRIG_DELAY_FIELD =
{
    "TRIG_DELAY",
#if RU_INCLUDE_DESC
    "trig_delay",
    "Delay recording start for a set number of words (applies to all triggers). If shorter frame size is used, this field is limited to 2*(frame_size_in_words-1)",
#endif
    NGPON_RX_RECORDING_TRIG_TRIG_DELAY_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_TRIG_TRIG_DELAY_FIELD_WIDTH,
    NGPON_RX_RECORDING_TRIG_TRIG_DELAY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_CFG_STOP
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_CFG_STOP_FIELD =
{
    "STOP",
#if RU_INCLUDE_DESC
    "RCD_STOP_ON_MAP_END",
    "Whether to stop on map end or to stop when memory is full."
    ""
    "If asserted, only one map (that is, bandwidth allocation structures from a single frame) will be recorded and the recording will be stopped at the end of the first map (first frame) after recording enable (*)."
    ""
    "This field can be changed during runtime."
    ""
    "(*) Actually, at the start of the following frame in which MAC is at synchronized state.",
#endif
    NGPON_RX_RECORDING_CFG_STOP_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_CFG_STOP_FIELD_WIDTH,
    NGPON_RX_RECORDING_CFG_STOP_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_CFG_FILT
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_CFG_FILT_FIELD =
{
    "FILT",
#if RU_INCLUDE_DESC
    "RCD_FILT",
    "Alloc-ID filter selection. Cannot be changed when the recorder is active.",
#endif
    NGPON_RX_RECORDING_CFG_FILT_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_CFG_FILT_FIELD_WIDTH,
    NGPON_RX_RECORDING_CFG_FILT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_RECORDING_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_RX_RECORDING_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_CFG_SPEC
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_CFG_SPEC_FIELD =
{
    "SPEC",
#if RU_INCLUDE_DESC
    "RCD_SPEC",
    "Configurable alloc id to record the access which are directed to."
    ""
    "Used only when rcd_filt mode is single",
#endif
    NGPON_RX_RECORDING_CFG_SPEC_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_CFG_SPEC_FIELD_WIDTH,
    NGPON_RX_RECORDING_CFG_SPEC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_CFG_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_CFG_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_RECORDING_CFG_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_CFG_RESERVED1_FIELD_WIDTH,
    NGPON_RX_RECORDING_CFG_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_EN_EN
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_EN_EN_FIELD =
{
    "EN",
#if RU_INCLUDE_DESC
    "bwrcd_en",
    "Record enable strobe."
    ""
    "The recording will start at the next frame after assertion and will continue until recording is done or zero is written."
    ""
    "Reading this bit indicates the status of the recorder."
    ""
    "Note that the recorder is not turned off when the module is disabled (recording is paused and continues after synchronization is reacquired; interrupt is only generated when the next valid frame is received).",
#endif
    NGPON_RX_RECORDING_EN_EN_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_EN_EN_FIELD_WIDTH,
    NGPON_RX_RECORDING_EN_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_EN_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_EN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_RECORDING_EN_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_EN_RESERVED0_FIELD_WIDTH,
    NGPON_RX_RECORDING_EN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_STATUS_DONE
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_STATUS_DONE_FIELD =
{
    "DONE",
#if RU_INCLUDE_DESC
    "bwrcd_done",
    "Recording done. The SW may start reading the results."
    ""
    "Will be self cleared when the SW restarts the recording.",
#endif
    NGPON_RX_RECORDING_STATUS_DONE_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_STATUS_DONE_FIELD_WIDTH,
    NGPON_RX_RECORDING_STATUS_DONE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_STATUS_OOM
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_STATUS_OOM_FIELD =
{
    "OOM",
#if RU_INCLUDE_DESC
    "bwrcd_out_of_mem",
    "Not enough memory for a single frame (only when configured to stop at end of frame)",
#endif
    NGPON_RX_RECORDING_STATUS_OOM_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_STATUS_OOM_FIELD_WIDTH,
    NGPON_RX_RECORDING_STATUS_OOM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_STATUS_EMPTY
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_STATUS_EMPTY_FIELD =
{
    "EMPTY",
#if RU_INCLUDE_DESC
    "bwrcd_empty",
    "Indicates no access was recorded. Used in order to distinguish between empty and full.",
#endif
    NGPON_RX_RECORDING_STATUS_EMPTY_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_STATUS_EMPTY_FIELD_WIDTH,
    NGPON_RX_RECORDING_STATUS_EMPTY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_STATUS_LAST_PTR
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_STATUS_LAST_PTR_FIELD =
{
    "LAST_PTR",
#if RU_INCLUDE_DESC
    "bwrcd_last_ptr",
    "Points to the last address which was recorded. Indicaes haw many entries are valid in the RAM (from 0 to the pointer)."
    "",
#endif
    NGPON_RX_RECORDING_STATUS_LAST_PTR_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_STATUS_LAST_PTR_FIELD_WIDTH,
    NGPON_RX_RECORDING_STATUS_LAST_PTR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_STATUS_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_STATUS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_RECORDING_STATUS_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_STATUS_RESERVED0_FIELD_WIDTH,
    NGPON_RX_RECORDING_STATUS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_RECORDING_MEM_DATA_DATA
 ******************************************************************************/
const ru_field_rec NGPON_RX_RECORDING_MEM_DATA_DATA_FIELD =
{
    "DATA",
#if RU_INCLUDE_DESC
    "Data",
    "Data",
#endif
    NGPON_RX_RECORDING_MEM_DATA_DATA_FIELD_MASK,
    0,
    NGPON_RX_RECORDING_MEM_DATA_DATA_FIELD_WIDTH,
    NGPON_RX_RECORDING_MEM_DATA_DATA_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_RECORDING_TRIG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_RECORDING_TRIG_FIELDS[] =
{
    &NGPON_RX_RECORDING_TRIG_FIRST_TRIG_FIELD,
    &NGPON_RX_RECORDING_TRIG_NEXT_TRIG_FIELD,
    &NGPON_RX_RECORDING_TRIG_RESERVED0_FIELD,
    &NGPON_RX_RECORDING_TRIG_TRIG_DELAY_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_RECORDING_TRIG_REG = 
{
    "TRIG",
#if RU_INCLUDE_DESC
    "LLRCD_TRIG Register",
    "Trigger configuration (recording starts when trigger condition is met)",
#endif
    NGPON_RX_RECORDING_TRIG_REG_OFFSET,
    0,
    0,
    133,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_RX_RECORDING_TRIG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_RECORDING_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_RECORDING_CFG_FIELDS[] =
{
    &NGPON_RX_RECORDING_CFG_STOP_FIELD,
    &NGPON_RX_RECORDING_CFG_FILT_FIELD,
    &NGPON_RX_RECORDING_CFG_RESERVED0_FIELD,
    &NGPON_RX_RECORDING_CFG_SPEC_FIELD,
    &NGPON_RX_RECORDING_CFG_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_RECORDING_CFG_REG = 
{
    "CFG",
#if RU_INCLUDE_DESC
    "BWRCD_CFG Register",
    "Configuration of the BWmap recording mechanism:"
    ""
    "Which allocs to record, when to stop, etc."
    "",
#endif
    NGPON_RX_RECORDING_CFG_REG_OFFSET,
    0,
    0,
    134,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    NGPON_RX_RECORDING_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_RECORDING_EN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_RECORDING_EN_FIELDS[] =
{
    &NGPON_RX_RECORDING_EN_EN_FIELD,
    &NGPON_RX_RECORDING_EN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_RECORDING_EN_REG = 
{
    "EN",
#if RU_INCLUDE_DESC
    "BWRCD_EN Register",
    "Record enable",
#endif
    NGPON_RX_RECORDING_EN_REG_OFFSET,
    0,
    0,
    135,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_RECORDING_EN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_RECORDING_STATUS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_RECORDING_STATUS_FIELDS[] =
{
    &NGPON_RX_RECORDING_STATUS_DONE_FIELD,
    &NGPON_RX_RECORDING_STATUS_OOM_FIELD,
    &NGPON_RX_RECORDING_STATUS_EMPTY_FIELD,
    &NGPON_RX_RECORDING_STATUS_LAST_PTR_FIELD,
    &NGPON_RX_RECORDING_STATUS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_RECORDING_STATUS_REG = 
{
    "STATUS",
#if RU_INCLUDE_DESC
    "BWRCD_STATUS Register",
    "Status indications",
#endif
    NGPON_RX_RECORDING_STATUS_REG_OFFSET,
    0,
    0,
    136,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    NGPON_RX_RECORDING_STATUS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_RECORDING_MEM_DATA
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_RECORDING_MEM_DATA_FIELDS[] =
{
    &NGPON_RX_RECORDING_MEM_DATA_DATA_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_RECORDING_MEM_DATA_REG = 
{
    "MEM_DATA",
#if RU_INCLUDE_DESC
    "DATA Register",
    "Data",
#endif
    NGPON_RX_RECORDING_MEM_DATA_REG_OFFSET,
    NGPON_RX_RECORDING_MEM_DATA_REG_RAM_CNT,
    4,
    137,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_RECORDING_MEM_DATA_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_RECORDING
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_RECORDING_REGS[] =
{
    &NGPON_RX_RECORDING_TRIG_REG,
    &NGPON_RX_RECORDING_CFG_REG,
    &NGPON_RX_RECORDING_EN_REG,
    &NGPON_RX_RECORDING_STATUS_REG,
    &NGPON_RX_RECORDING_MEM_DATA_REG,
};

unsigned long NGPON_RX_RECORDING_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80161fd4,
#elif defined(CONFIG_BCM96856)
    0x82db5fd0,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_RECORDING_BLOCK = 
{
    "NGPON_RX_RECORDING",
    NGPON_RX_RECORDING_ADDRS,
    1,
    5,
    NGPON_RX_RECORDING_REGS
};

/* End of file BCM6858_A0_NGPON_RX_RECORDING.c */
