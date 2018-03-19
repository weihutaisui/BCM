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
 * Field: GPON_RX_BWMAP_RECORD_CONFIG_RCD_STOP_ON_MAP_END
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_CONFIG_RCD_STOP_ON_MAP_END_FIELD =
{
    "RCD_STOP_ON_MAP_END",
#if RU_INCLUDE_DESC
    "rcd_stop_on_map_end",
    "Whether to stop on map end or to stop when memory is full."
    ""
    "If asserted, only one map will be recorded and the recording will be stopped at the end of the first map after recording enable.",
#endif
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_STOP_ON_MAP_END_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_STOP_ON_MAP_END_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_STOP_ON_MAP_END_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_FIELD =
{
    "RCD_ALL",
#if RU_INCLUDE_DESC
    "rcd_all",
    "Record all accesses (do not filter any of the accesses according to Alloc-ID for recording)",
#endif
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_ONU
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_ONU_FIELD =
{
    "RCD_ALL_ONU",
#if RU_INCLUDE_DESC
    "rcd_all_onu",
    "Record only the accesses which are directed to one of the 40 TCONTs of the ONU.",
#endif
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_ONU_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_ONU_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_ONU_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_CONFIG_RCD_SPECIFIC_ALLOC
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_CONFIG_RCD_SPECIFIC_ALLOC_FIELD =
{
    "RCD_SPECIFIC_ALLOC",
#if RU_INCLUDE_DESC
    "rcd_specific_alloc",
    "Record only accesses of a specific Alloc-ID as configured in the next field.",
#endif
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_SPECIFIC_ALLOC_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_SPECIFIC_ALLOC_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_CONFIG_RCD_SPECIFIC_ALLOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_CONFIG_SECIFIC_ALLOC
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_CONFIG_SECIFIC_ALLOC_FIELD =
{
    "SECIFIC_ALLOC",
#if RU_INCLUDE_DESC
    "secific_alloc",
    "Configurable Alloc-ID to record the access which are directed to."
    ""
    "Used only when rcd_specific_alloc bit is asserted.",
#endif
    GPON_RX_BWMAP_RECORD_CONFIG_SECIFIC_ALLOC_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_CONFIG_SECIFIC_ALLOC_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_CONFIG_SECIFIC_ALLOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_CONFIG_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_CONFIG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_BWMAP_RECORD_CONFIG_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_CONFIG_RESERVED0_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_CONFIG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_ENABLE_RCD_ENABLE
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_ENABLE_RCD_ENABLE_FIELD =
{
    "RCD_ENABLE",
#if RU_INCLUDE_DESC
    "rcd_enable",
    "record enable strobe."
    ""
    "The recording will start in the beginning of the next bwmap after assertion and will continue until recording is done."
    ""
    "this bit should stay asserted during recording."
    ""
    "For restarting, the SW should deassert and assert again this bit",
#endif
    GPON_RX_BWMAP_RECORD_ENABLE_RCD_ENABLE_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_ENABLE_RCD_ENABLE_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_ENABLE_RCD_ENABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_ENABLE_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_ENABLE_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_BWMAP_RECORD_ENABLE_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_ENABLE_RESERVED0_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_ENABLE_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_STATUS_RCD_DONE
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_STATUS_RCD_DONE_FIELD =
{
    "RCD_DONE",
#if RU_INCLUDE_DESC
    "rcd_done",
    "recording done. The SW may start reading the results."
    ""
    "Will be self cleared when the SW will restart the recording.",
#endif
    GPON_RX_BWMAP_RECORD_STATUS_RCD_DONE_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_STATUS_RCD_DONE_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_STATUS_RCD_DONE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_STATUS_RCD_MISMATCH
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_STATUS_RCD_MISMATCH_FIELD =
{
    "RCD_MISMATCH",
#if RU_INCLUDE_DESC
    "rcd_mismatch",
    "a mismatch has occured when recording, meaning there was not enough space in the memory for entire map.",
#endif
    GPON_RX_BWMAP_RECORD_STATUS_RCD_MISMATCH_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_STATUS_RCD_MISMATCH_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_STATUS_RCD_MISMATCH_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_STATUS_ECD_EMPTY
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_STATUS_ECD_EMPTY_FIELD =
{
    "ECD_EMPTY",
#if RU_INCLUDE_DESC
    "rcd_empty",
    "indicates no access was recorded. Used in order to distinguish between empty and full (if 0 or 64 accesses were recorded, the pointer will point to 0).",
#endif
    GPON_RX_BWMAP_RECORD_STATUS_ECD_EMPTY_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_STATUS_ECD_EMPTY_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_STATUS_ECD_EMPTY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_STATUS_RCD_LAST_PTR
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_STATUS_RCD_LAST_PTR_FIELD =
{
    "RCD_LAST_PTR",
#if RU_INCLUDE_DESC
    "rcd_last_ptr",
    "the pointer to the last address which was recorded. Indicates haw many entries are valid in the RAM (from 0 to the pointer)."
    ""
    "Possible values: 0 to 3f",
#endif
    GPON_RX_BWMAP_RECORD_STATUS_RCD_LAST_PTR_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_STATUS_RCD_LAST_PTR_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_STATUS_RCD_LAST_PTR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_STATUS_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_STATUS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_BWMAP_RECORD_STATUS_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_STATUS_RESERVED0_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_STATUS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_READ_IF_READ_ADDR
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_READ_IF_READ_ADDR_FIELD =
{
    "READ_ADDR",
#if RU_INCLUDE_DESC
    "read_addr",
    "read address within the BWmap recording RAM."
    ""
    "Valid values: 0 - 63."
    ""
    "Each address indicates a different access.",
#endif
    GPON_RX_BWMAP_RECORD_READ_IF_READ_ADDR_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_READ_IF_READ_ADDR_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_READ_IF_READ_ADDR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_READ_IF_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_READ_IF_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_BWMAP_RECORD_READ_IF_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_READ_IF_RESERVED0_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_READ_IF_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_BWMAP_RECORD_RDATA_RCD_DATA
 ******************************************************************************/
const ru_field_rec GPON_RX_BWMAP_RECORD_RDATA_RCD_DATA_FIELD =
{
    "RCD_DATA",
#if RU_INCLUDE_DESC
    "rcd_data",
    "read data."
    ""
    "bits 63:32 are located at the lower address"
    "bits 31:0 at the higher address",
#endif
    GPON_RX_BWMAP_RECORD_RDATA_RCD_DATA_FIELD_MASK,
    0,
    GPON_RX_BWMAP_RECORD_RDATA_RCD_DATA_FIELD_WIDTH,
    GPON_RX_BWMAP_RECORD_RDATA_RCD_DATA_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_BWMAP_RECORD_CONFIG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_BWMAP_RECORD_CONFIG_FIELDS[] =
{
    &GPON_RX_BWMAP_RECORD_CONFIG_RCD_STOP_ON_MAP_END_FIELD,
    &GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_FIELD,
    &GPON_RX_BWMAP_RECORD_CONFIG_RCD_ALL_ONU_FIELD,
    &GPON_RX_BWMAP_RECORD_CONFIG_RCD_SPECIFIC_ALLOC_FIELD,
    &GPON_RX_BWMAP_RECORD_CONFIG_SECIFIC_ALLOC_FIELD,
    &GPON_RX_BWMAP_RECORD_CONFIG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_BWMAP_RECORD_CONFIG_REG = 
{
    "CONFIG",
#if RU_INCLUDE_DESC
    "BW_RCD_CONFIGURATION Register",
    "the configurations of the BWmap recording mechanism:"
    ""
    "Which allocs to record, when to stop, etc."
    "",
#endif
    GPON_RX_BWMAP_RECORD_CONFIG_REG_OFFSET,
    0,
    0,
    139,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    6,
    GPON_RX_BWMAP_RECORD_CONFIG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_BWMAP_RECORD_ENABLE
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_BWMAP_RECORD_ENABLE_FIELDS[] =
{
    &GPON_RX_BWMAP_RECORD_ENABLE_RCD_ENABLE_FIELD,
    &GPON_RX_BWMAP_RECORD_ENABLE_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_BWMAP_RECORD_ENABLE_REG = 
{
    "ENABLE",
#if RU_INCLUDE_DESC
    "BW_RCD_EN Register",
    "record enable",
#endif
    GPON_RX_BWMAP_RECORD_ENABLE_REG_OFFSET,
    0,
    0,
    140,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_BWMAP_RECORD_ENABLE_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_BWMAP_RECORD_STATUS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_BWMAP_RECORD_STATUS_FIELDS[] =
{
    &GPON_RX_BWMAP_RECORD_STATUS_RCD_DONE_FIELD,
    &GPON_RX_BWMAP_RECORD_STATUS_RCD_MISMATCH_FIELD,
    &GPON_RX_BWMAP_RECORD_STATUS_ECD_EMPTY_FIELD,
    &GPON_RX_BWMAP_RECORD_STATUS_RCD_LAST_PTR_FIELD,
    &GPON_RX_BWMAP_RECORD_STATUS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_BWMAP_RECORD_STATUS_REG = 
{
    "STATUS",
#if RU_INCLUDE_DESC
    "BW_RCD_RESULT Register",
    "indications on the recording result:"
    "recording done"
    "mismatch"
    "write pointer (haw many entries are valid in the ram)",
#endif
    GPON_RX_BWMAP_RECORD_STATUS_REG_OFFSET,
    0,
    0,
    141,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    GPON_RX_BWMAP_RECORD_STATUS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_BWMAP_RECORD_READ_IF
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_BWMAP_RECORD_READ_IF_FIELDS[] =
{
    &GPON_RX_BWMAP_RECORD_READ_IF_READ_ADDR_FIELD,
    &GPON_RX_BWMAP_RECORD_READ_IF_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_BWMAP_RECORD_READ_IF_REG = 
{
    "READ_IF",
#if RU_INCLUDE_DESC
    "BW_RCD_RD_IF Register",
    "SW read interface."
    ""
    "which address within the recordnig memory to read",
#endif
    GPON_RX_BWMAP_RECORD_READ_IF_REG_OFFSET,
    0,
    0,
    142,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_BWMAP_RECORD_READ_IF_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_BWMAP_RECORD_RDATA
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_BWMAP_RECORD_RDATA_FIELDS[] =
{
    &GPON_RX_BWMAP_RECORD_RDATA_RCD_DATA_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_BWMAP_RECORD_RDATA_REG = 
{
    "RDATA",
#if RU_INCLUDE_DESC
    "BW_RCD_RDATA %i Register",
    "recording read data."
    ""
    "2 registers - 64bit data."
    ""
    "each entry describes one access as recorded.",
#endif
    GPON_RX_BWMAP_RECORD_RDATA_REG_OFFSET,
    GPON_RX_BWMAP_RECORD_RDATA_REG_RAM_CNT,
    4,
    143,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_BWMAP_RECORD_RDATA_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_BWMAP_RECORD
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_BWMAP_RECORD_REGS[] =
{
    &GPON_RX_BWMAP_RECORD_CONFIG_REG,
    &GPON_RX_BWMAP_RECORD_ENABLE_REG,
    &GPON_RX_BWMAP_RECORD_STATUS_REG,
    &GPON_RX_BWMAP_RECORD_READ_IF_REG,
    &GPON_RX_BWMAP_RECORD_RDATA_REG,
};

unsigned long GPON_RX_BWMAP_RECORD_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130f9a80,
#else
    0x80150a80,
#endif
};

const ru_block_rec GPON_RX_BWMAP_RECORD_BLOCK = 
{
    "GPON_RX_BWMAP_RECORD",
    GPON_RX_BWMAP_RECORD_ADDRS,
    1,
    5,
    GPON_RX_BWMAP_RECORD_REGS
};

/* End of file BCM6858_A0GPON_RX_BWMAP_RECORD.c */
