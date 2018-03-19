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

static unsigned long GPON_RX_PORT_ID_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80150200,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db1200,
#else
    #error "GPON_RX_AMD2 base address not defined"
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

/* End of file BCM6836_A0GPON_RX_PORT_ID.c */
