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
 * Field: GPON_RX_PORT_ID_FULLCFG_PORTID
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_FULLCFG_PORTID_FIELD =
{
    "PORTID",
#if RU_INCLUDE_DESC
    "Port-ID",
    "Sets the Port-ID for this Flow-ID."
    "This field can be changed only while the filter is disabled (via the PORTIDDIS register).",
#endif
    GPON_RX_PORT_ID_FULLCFG_PORTID_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_FULLCFG_PORTID_FIELD_WIDTH,
    GPON_RX_PORT_ID_FULLCFG_PORTID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_FULLCFG_ENCRYPT
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_FULLCFG_ENCRYPT_FIELD =
{
    "ENCRYPT",
#if RU_INCLUDE_DESC
    "Encryption_State",
    "Indicates whether the Port-ID is encrypted and should be decrypted in the ONU."
    "This field can be changed on-the-fly during operation. Change will take effect at frame boundaries.",
#endif
    GPON_RX_PORT_ID_FULLCFG_ENCRYPT_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_FULLCFG_ENCRYPT_FIELD_WIDTH,
    GPON_RX_PORT_ID_FULLCFG_ENCRYPT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_FULLCFG_TYPE
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_FULLCFG_TYPE_FIELD =
{
    "TYPE",
#if RU_INCLUDE_DESC
    "Flow_Type",
    "Indicates the Flows type (Ethernet or TDM)."
    "This field can be changed only while the filter is disabled (via the PORTIDDIS register).",
#endif
    GPON_RX_PORT_ID_FULLCFG_TYPE_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_FULLCFG_TYPE_FIELD_WIDTH,
    GPON_RX_PORT_ID_FULLCFG_TYPE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_FULLCFG_PRIORITY
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_FULLCFG_PRIORITY_FIELD =
{
    "PRIORITY",
#if RU_INCLUDE_DESC
    "Flow_Priority",
    "Indicates the Flows priority."
    "This field can be changed only while the filter is disabled (via the PORTIDDIS register).",
#endif
    GPON_RX_PORT_ID_FULLCFG_PRIORITY_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_FULLCFG_PRIORITY_FIELD_WIDTH,
    GPON_RX_PORT_ID_FULLCFG_PRIORITY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_FULLCFG_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_FULLCFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_PORT_ID_FULLCFG_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_FULLCFG_RESERVED0_FIELD_WIDTH,
    GPON_RX_PORT_ID_FULLCFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_CFG_PORTID
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_CFG_PORTID_FIELD =
{
    "PORTID",
#if RU_INCLUDE_DESC
    "Port-ID",
    "Sets the Port-ID for this Flow-ID."
    "This field can be changed only while the filter is disabled (via the PORTIDDIS register).",
#endif
    GPON_RX_PORT_ID_CFG_PORTID_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_CFG_PORTID_FIELD_WIDTH,
    GPON_RX_PORT_ID_CFG_PORTID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_CFG_ENCRYPT
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_CFG_ENCRYPT_FIELD =
{
    "ENCRYPT",
#if RU_INCLUDE_DESC
    "Encryption_State",
    "Indicates whether the Port-ID is encrypted and should be decrypted in the ONU."
    "This field can be changed on-the-fly during operation. Change will take effect at frame boundaries.",
#endif
    GPON_RX_PORT_ID_CFG_ENCRYPT_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_CFG_ENCRYPT_FIELD_WIDTH,
    GPON_RX_PORT_ID_CFG_ENCRYPT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_PORT_ID_CFG_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_CFG_RESERVED0_FIELD_WIDTH,
    GPON_RX_PORT_ID_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_DISABLE_PORTIDDIS
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_DISABLE_PORTIDDIS_FIELD =
{
    "PORTIDDIS",
#if RU_INCLUDE_DESC
    "Port-ID_Disable",
    "Each bit corresponds to one Flow-ID and allows disabling of that Flow-ID."
    "Setting a bit to 1 disables that Flow-ID."
    "This field can be changed on-the-fly during operation. Change will take effect at frame boundaries.",
#endif
    GPON_RX_PORT_ID_DISABLE_PORTIDDIS_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_DISABLE_PORTIDDIS_FIELD_WIDTH,
    GPON_RX_PORT_ID_DISABLE_PORTIDDIS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_MASK_PORTIDMSK
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_MASK_PORTIDMSK_FIELD =
{
    "PORTIDMSK",
#if RU_INCLUDE_DESC
    "Port-ID_Mask",
    "A mask for the Port-ID value in Flow-ID #31."
    "A 0 in the mask indicates that this bit is not checked in the comparison."
    "This field can be changed only while Flow-ID #31s filter is disabled (via the PORTIDDIS register).",
#endif
    GPON_RX_PORT_ID_MASK_PORTIDMSK_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_MASK_PORTIDMSK_FIELD_WIDTH,
    GPON_RX_PORT_ID_MASK_PORTIDMSK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_MASK_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_MASK_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_PORT_ID_MASK_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_MASK_RESERVED0_FIELD_WIDTH,
    GPON_RX_PORT_ID_MASK_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_LASTIND_PTILAST
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_LASTIND_PTILAST_FIELD =
{
    "PTILAST",
#if RU_INCLUDE_DESC
    "PTI_Last_Indication",
    "This mask indicates which PTI codepoints mark the end of packets."
    "This field can be changed only while the receiver is disabled.",
#endif
    GPON_RX_PORT_ID_LASTIND_PTILAST_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_LASTIND_PTILAST_FIELD_WIDTH,
    GPON_RX_PORT_ID_LASTIND_PTILAST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_PORT_ID_LASTIND_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_PORT_ID_LASTIND_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_PORT_ID_LASTIND_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_PORT_ID_LASTIND_RESERVED0_FIELD_WIDTH,
    GPON_RX_PORT_ID_LASTIND_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_PORT_ID_FULLCFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PORT_ID_FULLCFG_FIELDS[] =
{
    &GPON_RX_PORT_ID_FULLCFG_PORTID_FIELD,
    &GPON_RX_PORT_ID_FULLCFG_ENCRYPT_FIELD,
    &GPON_RX_PORT_ID_FULLCFG_TYPE_FIELD,
    &GPON_RX_PORT_ID_FULLCFG_PRIORITY_FIELD,
    &GPON_RX_PORT_ID_FULLCFG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PORT_ID_FULLCFG_REG = 
{
    "FULLCFG",
#if RU_INCLUDE_DESC
    "PORTID_FULL_CONFIGURATION %i Register",
    "These 32 registers assign 32 Port-IDs ( from the possible 4096) to the ONU. Each Port-ID has a set of parameters associated with it. The number of the register is the Flow-ID the Port-ID is assigned to. Register 31 also has a mask assigned with it (see register PORTIDMSK). These registers along with the following 224 registers, enable defining up to 256 different port-ids.",
#endif
    GPON_RX_PORT_ID_FULLCFG_REG_OFFSET,
    GPON_RX_PORT_ID_FULLCFG_REG_RAM_CNT,
    4,
    104,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    GPON_RX_PORT_ID_FULLCFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PORT_ID_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PORT_ID_CFG_FIELDS[] =
{
    &GPON_RX_PORT_ID_CFG_PORTID_FIELD,
    &GPON_RX_PORT_ID_CFG_ENCRYPT_FIELD,
    &GPON_RX_PORT_ID_CFG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PORT_ID_CFG_REG = 
{
    "CFG",
#if RU_INCLUDE_DESC
    "PORTID_CONFIGURATION %i Register",
    "These 224 registers along with the former 32, assign 256 Port-IDs ( from the possible 4096) to the ONU. Each Port-ID has a set of parameters associated with it. While the former 32 are fully configurable, these 224 registers has strict parameters: the type is always gem and the priority is low.",
#endif
    GPON_RX_PORT_ID_CFG_REG_OFFSET,
    GPON_RX_PORT_ID_CFG_REG_RAM_CNT,
    4,
    105,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_RX_PORT_ID_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PORT_ID_DISABLE
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PORT_ID_DISABLE_FIELDS[] =
{
    &GPON_RX_PORT_ID_DISABLE_PORTIDDIS_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PORT_ID_DISABLE_REG = 
{
    "DISABLE",
#if RU_INCLUDE_DESC
    "PORTID_DISABLE %i Register",
    "These registers allow disabling flows. The first allows disabling flows 0 to 31. The second allows disabling flows 32-63, and so on.",
#endif
    GPON_RX_PORT_ID_DISABLE_REG_OFFSET,
    GPON_RX_PORT_ID_DISABLE_REG_RAM_CNT,
    4,
    106,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PORT_ID_DISABLE_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PORT_ID_MASK
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PORT_ID_MASK_FIELDS[] =
{
    &GPON_RX_PORT_ID_MASK_PORTIDMSK_FIELD,
    &GPON_RX_PORT_ID_MASK_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PORT_ID_MASK_REG = 
{
    "MASK",
#if RU_INCLUDE_DESC
    "PORTID_MASK Register",
    "This registers allows Flow-ID #31 to contains multiple Port-IDs by masking several (or all) bits of the Port-ID field before comparing it",
#endif
    GPON_RX_PORT_ID_MASK_REG_OFFSET,
    0,
    0,
    107,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_PORT_ID_MASK_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PORT_ID_LASTIND
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PORT_ID_LASTIND_FIELDS[] =
{
    &GPON_RX_PORT_ID_LASTIND_PTILAST_FIELD,
    &GPON_RX_PORT_ID_LASTIND_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PORT_ID_LASTIND_REG = 
{
    "LASTIND",
#if RU_INCLUDE_DESC
    "LAST_FRAGMENT_INDICATION_MASK Register",
    "The mask indicates which of the 8 possible PTI codepoints indicate that the fragment contains end of packet",
#endif
    GPON_RX_PORT_ID_LASTIND_REG_OFFSET,
    0,
    0,
    108,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_PORT_ID_LASTIND_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_PORT_ID
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_PORT_ID_REGS[] =
{
    &GPON_RX_PORT_ID_FULLCFG_REG,
    &GPON_RX_PORT_ID_CFG_REG,
    &GPON_RX_PORT_ID_DISABLE_REG,
    &GPON_RX_PORT_ID_MASK_REG,
    &GPON_RX_PORT_ID_LASTIND_REG,
};

unsigned long GPON_RX_PORT_ID_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130f9200,
#else
    0x80150200,
#endif
};

const ru_block_rec GPON_RX_PORT_ID_BLOCK = 
{
    "GPON_RX_PORT_ID",
    GPON_RX_PORT_ID_ADDRS,
    1,
    5,
    GPON_RX_PORT_ID_REGS
};

/* End of file BCM6858_A0GPON_RX_PORT_ID.c */
