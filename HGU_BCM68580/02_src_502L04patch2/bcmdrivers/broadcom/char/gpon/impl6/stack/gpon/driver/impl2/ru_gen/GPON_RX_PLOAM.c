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
 * Field: GPON_RX_PLOAM_ONU_ID_ONU_ID
 ******************************************************************************/
const ru_field_rec GPON_RX_PLOAM_ONU_ID_ONU_ID_FIELD =
{
    "ONU_ID",
#if RU_INCLUDE_DESC
    "ONU_ID",
    "Sets the ONU_ID."
    "This field can be changed only when the comparator is disabled.",
#endif
    GPON_RX_PLOAM_ONU_ID_ONU_ID_FIELD_MASK,
    0,
    GPON_RX_PLOAM_ONU_ID_ONU_ID_FIELD_WIDTH,
    GPON_RX_PLOAM_ONU_ID_ONU_ID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PLOAM_ONU_ID_DISABLE
 ******************************************************************************/
const ru_field_rec GPON_RX_PLOAM_ONU_ID_DISABLE_FIELD =
{
    "DISABLE",
#if RU_INCLUDE_DESC
    "ONU_ID_Disable",
    "Disables this ONU_ID. PLOAM messages directed to this ONU_ID will not be passed on."
    "This field can be changed on-the-fly during operation.",
#endif
    GPON_RX_PLOAM_ONU_ID_DISABLE_FIELD_MASK,
    0,
    GPON_RX_PLOAM_ONU_ID_DISABLE_FIELD_WIDTH,
    GPON_RX_PLOAM_ONU_ID_DISABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PLOAM_ONU_ID_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_PLOAM_ONU_ID_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_PLOAM_ONU_ID_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_PLOAM_ONU_ID_RESERVED0_FIELD_WIDTH,
    GPON_RX_PLOAM_ONU_ID_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PLOAM_PARAMS_CRC_OVERRD
 ******************************************************************************/
const ru_field_rec GPON_RX_PLOAM_PARAMS_CRC_OVERRD_FIELD =
{
    "CRC_OVERRD",
#if RU_INCLUDE_DESC
    "CRC_Override",
    "Allows messages with bad CRC to pass the filtering."
    "This field can be changed only while the receiver is disabled.",
#endif
    GPON_RX_PLOAM_PARAMS_CRC_OVERRD_FIELD_MASK,
    0,
    GPON_RX_PLOAM_PARAMS_CRC_OVERRD_FIELD_WIDTH,
    GPON_RX_PLOAM_PARAMS_CRC_OVERRD_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PLOAM_PARAMS_IDLE_FILTER
 ******************************************************************************/
const ru_field_rec GPON_RX_PLOAM_PARAMS_IDLE_FILTER_FIELD =
{
    "IDLE_FILTER",
#if RU_INCLUDE_DESC
    "Idle_PLOAM_filtering_enable",
    "Allows filtering of Idle PLOAM messages."
    "This field can be changed only while the receiver is disabled.",
#endif
    GPON_RX_PLOAM_PARAMS_IDLE_FILTER_FIELD_MASK,
    0,
    GPON_RX_PLOAM_PARAMS_IDLE_FILTER_FIELD_WIDTH,
    GPON_RX_PLOAM_PARAMS_IDLE_FILTER_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PLOAM_PARAMS_FAPL
 ******************************************************************************/
const ru_field_rec GPON_RX_PLOAM_PARAMS_FAPL_FIELD =
{
    "FAPL",
#if RU_INCLUDE_DESC
    "forward_all_ploam_messages",
    "enable forwarding all PLOAM messages, regardless of ONU ID",
#endif
    GPON_RX_PLOAM_PARAMS_FAPL_FIELD_MASK,
    0,
    GPON_RX_PLOAM_PARAMS_FAPL_FIELD_WIDTH,
    GPON_RX_PLOAM_PARAMS_FAPL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PLOAM_PARAMS_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_PLOAM_PARAMS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_PLOAM_PARAMS_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_PLOAM_PARAMS_RESERVED0_FIELD_WIDTH,
    GPON_RX_PLOAM_PARAMS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_PLOAM_ONU_ID
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PLOAM_ONU_ID_FIELDS[] =
{
    &GPON_RX_PLOAM_ONU_ID_ONU_ID_FIELD,
    &GPON_RX_PLOAM_ONU_ID_DISABLE_FIELD,
    &GPON_RX_PLOAM_ONU_ID_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PLOAM_ONU_ID_REG = 
{
    "ONU_ID",
#if RU_INCLUDE_DESC
    "ONU_ID_SETTINGS %i Register",
    "Sets the ONU_ID for the ONU."
    "Two registers are provided, and each can be disabled independently."
    "The first register is used for the ONU_ID. The second register is used for assigning the broadcast address.",
#endif
    GPON_RX_PLOAM_ONU_ID_REG_OFFSET,
    GPON_RX_PLOAM_ONU_ID_REG_RAM_CNT,
    4,
    94,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_RX_PLOAM_ONU_ID_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PLOAM_PARAMS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PLOAM_PARAMS_FIELDS[] =
{
    &GPON_RX_PLOAM_PARAMS_CRC_OVERRD_FIELD,
    &GPON_RX_PLOAM_PARAMS_IDLE_FILTER_FIELD,
    &GPON_RX_PLOAM_PARAMS_FAPL_FIELD,
    &GPON_RX_PLOAM_PARAMS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PLOAM_PARAMS_REG = 
{
    "PARAMS",
#if RU_INCLUDE_DESC
    "PLOAM_PROCESSOR_PARAMETERS Register",
    "This register allows setting of the PLOAM Processor operation parameters.",
#endif
    GPON_RX_PLOAM_PARAMS_REG_OFFSET,
    0,
    0,
    95,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_RX_PLOAM_PARAMS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_PLOAM
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_PLOAM_REGS[] =
{
    &GPON_RX_PLOAM_ONU_ID_REG,
    &GPON_RX_PLOAM_PARAMS_REG,
};

unsigned long GPON_RX_PLOAM_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130f9020,
#else
    0x80150020,
#endif
};

const ru_block_rec GPON_RX_PLOAM_BLOCK = 
{
    "GPON_RX_PLOAM",
    GPON_RX_PLOAM_ADDRS,
    1,
    2,
    GPON_RX_PLOAM_REGS
};

/* End of file BCM6858_A0GPON_RX_PLOAM.c */
