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

static unsigned long GPON_RX_PLOAM_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80150020,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db1020,
#else
    #error "GPON_RX_PLOAM base address not defined"
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

/* End of file BCM6836_A0GPON_RX_PLOAM.c */
