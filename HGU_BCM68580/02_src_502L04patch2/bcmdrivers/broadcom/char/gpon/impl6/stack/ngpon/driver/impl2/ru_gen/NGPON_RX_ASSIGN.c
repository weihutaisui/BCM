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
 * Field: NGPON_RX_ASSIGN_FLOW_PORTID
 ******************************************************************************/
const ru_field_rec NGPON_RX_ASSIGN_FLOW_PORTID_FIELD =
{
    "PORTID",
#if RU_INCLUDE_DESC
    "PORTID",
    "Sets the Port-ID for this Flow-ID."
    "This field can be changed only while the filter is disabled (via the PORTID_EN register).",
#endif
    NGPON_RX_ASSIGN_FLOW_PORTID_FIELD_MASK,
    0,
    NGPON_RX_ASSIGN_FLOW_PORTID_FIELD_WIDTH,
    NGPON_RX_ASSIGN_FLOW_PORTID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ASSIGN_FLOW_PRIORITY
 ******************************************************************************/
const ru_field_rec NGPON_RX_ASSIGN_FLOW_PRIORITY_FIELD =
{
    "PRIORITY",
#if RU_INCLUDE_DESC
    "PRIORITY",
    "Indicates the priority of the flow."
    "This field can be changed only while the filter is disabled (via the PORTID_EN register).",
#endif
    NGPON_RX_ASSIGN_FLOW_PRIORITY_FIELD_MASK,
    0,
    NGPON_RX_ASSIGN_FLOW_PRIORITY_FIELD_WIDTH,
    NGPON_RX_ASSIGN_FLOW_PRIORITY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ASSIGN_FLOW_ENC_RING
 ******************************************************************************/
const ru_field_rec NGPON_RX_ASSIGN_FLOW_ENC_RING_FIELD =
{
    "ENC_RING",
#if RU_INCLUDE_DESC
    "ENC_RING",
    "Sets the encryption keyring to be used. The payload is decrypted using the key indexed by the XGEM header from the selected ring. This field can be changed only while the filter is disabled (via the PORTID_EN register).",
#endif
    NGPON_RX_ASSIGN_FLOW_ENC_RING_FIELD_MASK,
    0,
    NGPON_RX_ASSIGN_FLOW_ENC_RING_FIELD_WIDTH,
    NGPON_RX_ASSIGN_FLOW_ENC_RING_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ASSIGN_FLOW_CRC_EN
 ******************************************************************************/
const ru_field_rec NGPON_RX_ASSIGN_FLOW_CRC_EN_FIELD =
{
    "CRC_EN",
#if RU_INCLUDE_DESC
    "CRC_EN",
    "Select whether Ethernet CRC calculation is done or not.",
#endif
    NGPON_RX_ASSIGN_FLOW_CRC_EN_FIELD_MASK,
    0,
    NGPON_RX_ASSIGN_FLOW_CRC_EN_FIELD_WIDTH,
    NGPON_RX_ASSIGN_FLOW_CRC_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ASSIGN_FLOW_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_ASSIGN_FLOW_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_ASSIGN_FLOW_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_ASSIGN_FLOW_RESERVED0_FIELD_WIDTH,
    NGPON_RX_ASSIGN_FLOW_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ASSIGN_FLOW_EN
 ******************************************************************************/
const ru_field_rec NGPON_RX_ASSIGN_FLOW_EN_FIELD =
{
    "EN",
#if RU_INCLUDE_DESC
    "Enable",
    "Select whether the filter is enabled or not. When the filter is disabled, traffic is blocked. Configuration changes are allowed only in the disable state. Re-enabling the filter should be done as a separate command.",
#endif
    NGPON_RX_ASSIGN_FLOW_EN_FIELD_MASK,
    0,
    NGPON_RX_ASSIGN_FLOW_EN_FIELD_WIDTH,
    NGPON_RX_ASSIGN_FLOW_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ASSIGN_FLOW_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_ASSIGN_FLOW_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_ASSIGN_FLOW_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_ASSIGN_FLOW_RESERVED1_FIELD_WIDTH,
    NGPON_RX_ASSIGN_FLOW_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ASSIGN_LAST_FLOW_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_ASSIGN_LAST_FLOW_MASK_FIELD =
{
    "MASK",
#if RU_INCLUDE_DESC
    "Mask",
    "A mask for the Port-ID value in Flow-ID 255."
    "A 0 in the mask indicates that this bit is not checked in the comparison."
    "This field can be changed only while Flow-ID 255 filter is disabled (via the PORTID_EN register).",
#endif
    NGPON_RX_ASSIGN_LAST_FLOW_MASK_FIELD_MASK,
    0,
    NGPON_RX_ASSIGN_LAST_FLOW_MASK_FIELD_WIDTH,
    NGPON_RX_ASSIGN_LAST_FLOW_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ASSIGN_LAST_FLOW_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_ASSIGN_LAST_FLOW_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_ASSIGN_LAST_FLOW_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_ASSIGN_LAST_FLOW_RESERVED0_FIELD_WIDTH,
    NGPON_RX_ASSIGN_LAST_FLOW_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_ASSIGN_FLOW
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ASSIGN_FLOW_FIELDS[] =
{
    &NGPON_RX_ASSIGN_FLOW_PORTID_FIELD,
    &NGPON_RX_ASSIGN_FLOW_PRIORITY_FIELD,
    &NGPON_RX_ASSIGN_FLOW_ENC_RING_FIELD,
    &NGPON_RX_ASSIGN_FLOW_CRC_EN_FIELD,
    &NGPON_RX_ASSIGN_FLOW_RESERVED0_FIELD,
    &NGPON_RX_ASSIGN_FLOW_EN_FIELD,
    &NGPON_RX_ASSIGN_FLOW_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ASSIGN_FLOW_REG = 
{
    "FLOW",
#if RU_INCLUDE_DESC
    "FLOW_CONFIGURATION %i Register",
    "These registers assign Port-IDs to the ONU. Each Port-ID has a set of parameters associated with it. The number of the register is the Flow-ID the Port-ID is assigned to. Register 255 also has a mask assigned with it (see register PORTIDMSK).",
#endif
    NGPON_RX_ASSIGN_FLOW_REG_OFFSET,
    NGPON_RX_ASSIGN_FLOW_REG_RAM_CNT,
    4,
    42,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    7,
    NGPON_RX_ASSIGN_FLOW_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ASSIGN_LAST_FLOW
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ASSIGN_LAST_FLOW_FIELDS[] =
{
    &NGPON_RX_ASSIGN_LAST_FLOW_MASK_FIELD,
    &NGPON_RX_ASSIGN_LAST_FLOW_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ASSIGN_LAST_FLOW_REG = 
{
    "LAST_FLOW",
#if RU_INCLUDE_DESC
    "LAST_FLOW Register",
    "This register allows Flow-ID 255 to contains multiple Port-IDs by masking several (or all) bits of the Port-ID field before comparison.",
#endif
    NGPON_RX_ASSIGN_LAST_FLOW_REG_OFFSET,
    0,
    0,
    43,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_ASSIGN_LAST_FLOW_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_ASSIGN
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_ASSIGN_REGS[] =
{
    &NGPON_RX_ASSIGN_FLOW_REG,
    &NGPON_RX_ASSIGN_LAST_FLOW_REG,
};

unsigned long NGPON_RX_ASSIGN_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80160200,
#elif defined(CONFIG_BCM96856)
    0x82db4200,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_ASSIGN_BLOCK = 
{
    "NGPON_RX_ASSIGN",
    NGPON_RX_ASSIGN_ADDRS,
    1,
    2,
    NGPON_RX_ASSIGN_REGS
};

/* End of file BCM6858_A0_NGPON_RX_ASSIGN.c */
