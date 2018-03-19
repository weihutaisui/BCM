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
 * Field: NGPON_TX_ENC_KEY_CFG_ONU1_KEY_1_VALID
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_KEY_CFG_ONU1_KEY_1_VALID_FIELD =
{
    "KEY_1_VALID",
#if RU_INCLUDE_DESC
    "Key_1_valid",
    "Valid indication for the first key",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU1_KEY_1_VALID_FIELD_MASK,
    0,
    NGPON_TX_ENC_KEY_CFG_ONU1_KEY_1_VALID_FIELD_WIDTH,
    NGPON_TX_ENC_KEY_CFG_ONU1_KEY_1_VALID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_KEY_CFG_ONU1_KEY_2_VALID
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_KEY_CFG_ONU1_KEY_2_VALID_FIELD =
{
    "KEY_2_VALID",
#if RU_INCLUDE_DESC
    "Key_2_valid",
    "Valid indication for the second key",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU1_KEY_2_VALID_FIELD_MASK,
    0,
    NGPON_TX_ENC_KEY_CFG_ONU1_KEY_2_VALID_FIELD_WIDTH,
    NGPON_TX_ENC_KEY_CFG_ONU1_KEY_2_VALID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_KEY_CFG_ONU1_KEY_SEL
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_KEY_CFG_ONU1_KEY_SEL_FIELD =
{
    "KEY_SEL",
#if RU_INCLUDE_DESC
    "Key_select",
    "Configuration which key to be used 0 or 1",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU1_KEY_SEL_FIELD_MASK,
    0,
    NGPON_TX_ENC_KEY_CFG_ONU1_KEY_SEL_FIELD_WIDTH,
    NGPON_TX_ENC_KEY_CFG_ONU1_KEY_SEL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_KEY_CFG_ONU1_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_KEY_CFG_ONU1_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU1_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_ENC_KEY_CFG_ONU1_RESERVED0_FIELD_WIDTH,
    NGPON_TX_ENC_KEY_CFG_ONU1_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_KEY_CFG_ONU2_KEY_1_VALID
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_KEY_CFG_ONU2_KEY_1_VALID_FIELD =
{
    "KEY_1_VALID",
#if RU_INCLUDE_DESC
    "Key_1_valid",
    "Valid indication for the first key",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU2_KEY_1_VALID_FIELD_MASK,
    0,
    NGPON_TX_ENC_KEY_CFG_ONU2_KEY_1_VALID_FIELD_WIDTH,
    NGPON_TX_ENC_KEY_CFG_ONU2_KEY_1_VALID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_KEY_CFG_ONU2_KEY_2_VALID
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_KEY_CFG_ONU2_KEY_2_VALID_FIELD =
{
    "KEY_2_VALID",
#if RU_INCLUDE_DESC
    "Key_2_valid",
    "Valid indication for the second key",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU2_KEY_2_VALID_FIELD_MASK,
    0,
    NGPON_TX_ENC_KEY_CFG_ONU2_KEY_2_VALID_FIELD_WIDTH,
    NGPON_TX_ENC_KEY_CFG_ONU2_KEY_2_VALID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_KEY_CFG_ONU2_KEY_SEL
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_KEY_CFG_ONU2_KEY_SEL_FIELD =
{
    "KEY_SEL",
#if RU_INCLUDE_DESC
    "Key_select",
    "Configuration which key to be used 0 or 1",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU2_KEY_SEL_FIELD_MASK,
    0,
    NGPON_TX_ENC_KEY_CFG_ONU2_KEY_SEL_FIELD_WIDTH,
    NGPON_TX_ENC_KEY_CFG_ONU2_KEY_SEL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_KEY_CFG_ONU2_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_KEY_CFG_ONU2_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU2_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_ENC_KEY_CFG_ONU2_RESERVED0_FIELD_WIDTH,
    NGPON_TX_ENC_KEY_CFG_ONU2_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Value",
#endif
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Value",
#endif
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Value",
#endif
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Value",
#endif
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU1
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD =
{
    "PLOAMIK_ONU1",
#if RU_INCLUDE_DESC
    "ploamik1_load",
    "Load PLOAM-IK and CMAC K2 for ONU-ID #1",
#endif
    NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD_MASK,
    0,
    NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD_WIDTH,
    NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU2
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD =
{
    "PLOAMIK_ONU2",
#if RU_INCLUDE_DESC
    "ploamik2_load",
    "Load PLOAM-IK and CMAC K2 for ONU-ID #2",
#endif
    NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD_MASK,
    0,
    NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD_WIDTH,
    NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_BCST
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD =
{
    "PLOAMIK_BCST",
#if RU_INCLUDE_DESC
    "ploamikb_load",
    "Load PLOAM-IK and CMAC K2 for broadcast ONU-ID",
#endif
    NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD_MASK,
    0,
    NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD_WIDTH,
    NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PLOAM_LOAD_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PLOAM_LOAD_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_ENC_PLOAM_LOAD_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_ENC_PLOAM_LOAD_RESERVED0_FIELD_WIDTH,
    NGPON_TX_ENC_PLOAM_LOAD_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PLOAMIK_ONU1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PLOAMIK_ONU1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_TX_ENC_PLOAMIK_ONU1_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_PLOAMIK_ONU1_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_PLOAMIK_ONU1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PLOAMIK_ONU2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PLOAMIK_ONU2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_TX_ENC_PLOAMIK_ONU2_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_PLOAMIK_ONU2_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_PLOAMIK_ONU2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_PLOAMIK_DFLT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_PLOAMIK_DFLT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_TX_ENC_PLOAMIK_DFLT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_PLOAMIK_DFLT_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_PLOAMIK_DFLT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_ONU1_0_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_ONU1_0_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU1_0_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_ONU1_0_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_ONU1_0_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_ONU1_1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_ONU1_1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU1_1_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_ONU1_1_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_ONU1_1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_ONU1_2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_ONU1_2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU1_2_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_ONU1_2_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_ONU1_2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_ONU1_3_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_ONU1_3_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU1_3_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_ONU1_3_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_ONU1_3_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_ONU2_0_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_ONU2_0_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU2_0_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_ONU2_0_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_ONU2_0_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_ONU2_1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_ONU2_1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU2_1_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_ONU2_1_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_ONU2_1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_ONU2_2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_ONU2_2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU2_2_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_ONU2_2_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_ONU2_2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_ONU2_3_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_ONU2_3_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU2_3_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_ONU2_3_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_ONU2_3_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_DFLT_0_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_DFLT_0_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_DFLT_0_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_DFLT_0_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_DFLT_0_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_DFLT_1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_DFLT_1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_DFLT_1_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_DFLT_1_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_DFLT_1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_DFLT_2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_DFLT_2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_DFLT_2_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_DFLT_2_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_DFLT_2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_ENC_CMAC_K2_DFLT_3_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_ENC_CMAC_K2_DFLT_3_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_TX_ENC_CMAC_K2_DFLT_3_VALUE_FIELD_MASK,
    0,
    NGPON_TX_ENC_CMAC_K2_DFLT_3_VALUE_FIELD_WIDTH,
    NGPON_TX_ENC_CMAC_K2_DFLT_3_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_TX_ENC_KEY_CFG_ONU1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_KEY_CFG_ONU1_FIELDS[] =
{
    &NGPON_TX_ENC_KEY_CFG_ONU1_KEY_1_VALID_FIELD,
    &NGPON_TX_ENC_KEY_CFG_ONU1_KEY_2_VALID_FIELD,
    &NGPON_TX_ENC_KEY_CFG_ONU1_KEY_SEL_FIELD,
    &NGPON_TX_ENC_KEY_CFG_ONU1_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_KEY_CFG_ONU1_REG = 
{
    "KEY_CFG_ONU1",
#if RU_INCLUDE_DESC
    "ENCRYPTION_KEY_CONFIGURATION_ONU1 Register",
    "Encryption key configuration."
    "Configuration which key to be used and validity of the keys."
    "used for onu-id #1",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU1_REG_OFFSET,
    0,
    0,
    191,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_TX_ENC_KEY_CFG_ONU1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_KEY_CFG_ONU2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_KEY_CFG_ONU2_FIELDS[] =
{
    &NGPON_TX_ENC_KEY_CFG_ONU2_KEY_1_VALID_FIELD,
    &NGPON_TX_ENC_KEY_CFG_ONU2_KEY_2_VALID_FIELD,
    &NGPON_TX_ENC_KEY_CFG_ONU2_KEY_SEL_FIELD,
    &NGPON_TX_ENC_KEY_CFG_ONU2_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_KEY_CFG_ONU2_REG = 
{
    "KEY_CFG_ONU2",
#if RU_INCLUDE_DESC
    "ENCRYPTION_KEY_CONFIGURATION_ONU2 Register",
    "Encryption key configuration."
    "Configuration which key to be used and validity of the keys."
    "used for onu-id #2",
#endif
    NGPON_TX_ENC_KEY_CFG_ONU2_REG_OFFSET,
    0,
    0,
    192,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_TX_ENC_KEY_CFG_ONU2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_FIELDS[] =
{
    &NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_REG = 
{
    "PYLD_ENC_KEY1_ONU1",
#if RU_INCLUDE_DESC
    "PAYLOAD_ENCRYPTION_KEY1_ONU1 %i Register",
    "First key for XGEM payload encryption."
    "used for onu-id #1",
#endif
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_REG_OFFSET,
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_REG_RAM_CNT,
    4,
    193,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_FIELDS[] =
{
    &NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_REG = 
{
    "PYLD_ENC_KEY2_ONU1",
#if RU_INCLUDE_DESC
    "PAYLOAD_ENCRYPTION_KEY2_ONU1 %i Register",
    "Second key for XGEM payload encryption."
    "used for onu-id #1",
#endif
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_REG_OFFSET,
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_REG_RAM_CNT,
    4,
    194,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_FIELDS[] =
{
    &NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_REG = 
{
    "PYLD_ENC_KEY1_ONU2",
#if RU_INCLUDE_DESC
    "PAYLOAD_ENCRYPTION_KEY1_ONU2 %i Register",
    "First key for XGEM payload encryption."
    "used for onu-id #2",
#endif
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_REG_OFFSET,
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_REG_RAM_CNT,
    4,
    195,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_FIELDS[] =
{
    &NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_REG = 
{
    "PYLD_ENC_KEY2_ONU2",
#if RU_INCLUDE_DESC
    "PAYLOAD_ENCRYPTION_KEY2_ONU2 %i Register",
    "Second key for XGEM payload encryption."
    "used for onu-id #2",
#endif
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_REG_OFFSET,
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_REG_RAM_CNT,
    4,
    196,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_PLOAM_LOAD
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_PLOAM_LOAD_FIELDS[] =
{
    &NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD,
    &NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD,
    &NGPON_TX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD,
    &NGPON_TX_ENC_PLOAM_LOAD_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_PLOAM_LOAD_REG = 
{
    "PLOAM_LOAD",
#if RU_INCLUDE_DESC
    "PLOAM_KEY_LOAD Register",
    "Load commands for PLOAM keys."
    "Writing 1 to a bit triggers copying the information from a configuration register to a shadow register, which is used for the actual Encryption. Writing 0 is ignored. After the actual copying is done, hardware changes the bit back to 0.",
#endif
    NGPON_TX_ENC_PLOAM_LOAD_REG_OFFSET,
    0,
    0,
    197,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_TX_ENC_PLOAM_LOAD_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_PLOAMIK_ONU1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_PLOAMIK_ONU1_FIELDS[] =
{
    &NGPON_TX_ENC_PLOAMIK_ONU1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_PLOAMIK_ONU1_REG = 
{
    "PLOAMIK_ONU1",
#if RU_INCLUDE_DESC
    "PLOAM_IK_ONU1 %i Register",
    "Sets the value of the 128 bit encryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_PLOAMIK_ONU1_REG_OFFSET,
    NGPON_TX_ENC_PLOAMIK_ONU1_REG_RAM_CNT,
    4,
    198,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_PLOAMIK_ONU1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_PLOAMIK_ONU2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_PLOAMIK_ONU2_FIELDS[] =
{
    &NGPON_TX_ENC_PLOAMIK_ONU2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_PLOAMIK_ONU2_REG = 
{
    "PLOAMIK_ONU2",
#if RU_INCLUDE_DESC
    "PLOAM_IK_ONU2 %i Register",
    "Sets the value of the 128 bit encryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_PLOAMIK_ONU2_REG_OFFSET,
    NGPON_TX_ENC_PLOAMIK_ONU2_REG_RAM_CNT,
    4,
    199,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_PLOAMIK_ONU2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_PLOAMIK_DFLT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_PLOAMIK_DFLT_FIELDS[] =
{
    &NGPON_TX_ENC_PLOAMIK_DFLT_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_PLOAMIK_DFLT_REG = 
{
    "PLOAMIK_DFLT",
#if RU_INCLUDE_DESC
    "PLOAM_IK_DEFAULT %i Register",
    "Sets the value of the 128 bit encryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_PLOAMIK_DFLT_REG_OFFSET,
    NGPON_TX_ENC_PLOAMIK_DFLT_REG_RAM_CNT,
    4,
    200,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_PLOAMIK_DFLT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_ONU1_0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_ONU1_0_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_ONU1_0_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_ONU1_0_REG = 
{
    "CMAC_K2_ONU1_0",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU1_0 Register",
    "Sets the value of the 1st 32 bits ouf of the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU1_0_REG_OFFSET,
    0,
    0,
    201,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_ONU1_0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_ONU1_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_ONU1_1_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_ONU1_1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_ONU1_1_REG = 
{
    "CMAC_K2_ONU1_1",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU1_1 Register",
    "Sets the value of the 2nd 32 bits ouf of the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU1_1_REG_OFFSET,
    0,
    0,
    202,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_ONU1_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_ONU1_2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_ONU1_2_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_ONU1_2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_ONU1_2_REG = 
{
    "CMAC_K2_ONU1_2",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU1_2 Register",
    "Sets the value of the 3rd 32 bits ouf of the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU1_2_REG_OFFSET,
    0,
    0,
    203,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_ONU1_2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_ONU1_3
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_ONU1_3_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_ONU1_3_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_ONU1_3_REG = 
{
    "CMAC_K2_ONU1_3",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU1_3 Register",
    "Sets the value of the 4th 32 bits ouf of the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU1_3_REG_OFFSET,
    0,
    0,
    204,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_ONU1_3_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_ONU2_0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_ONU2_0_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_ONU2_0_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_ONU2_0_REG = 
{
    "CMAC_K2_ONU2_0",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU2_0 Register",
    "Sets the value of the 1st 32 bits ouf of the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU2_0_REG_OFFSET,
    0,
    0,
    205,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_ONU2_0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_ONU2_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_ONU2_1_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_ONU2_1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_ONU2_1_REG = 
{
    "CMAC_K2_ONU2_1",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU2_1 Register",
    "Sets the value of the 2nd 32 bits ouf of the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU2_1_REG_OFFSET,
    0,
    0,
    206,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_ONU2_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_ONU2_2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_ONU2_2_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_ONU2_2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_ONU2_2_REG = 
{
    "CMAC_K2_ONU2_2",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU2_2 Register",
    "Sets the value of the 3rd 32 bits ouf of the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU2_2_REG_OFFSET,
    0,
    0,
    207,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_ONU2_2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_ONU2_3
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_ONU2_3_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_ONU2_3_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_ONU2_3_REG = 
{
    "CMAC_K2_ONU2_3",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU2_3 Register",
    "Sets the value of the 4th 32 bits ouf of the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_ONU2_3_REG_OFFSET,
    0,
    0,
    208,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_ONU2_3_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_DFLT_0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_DFLT_0_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_DFLT_0_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_DFLT_0_REG = 
{
    "CMAC_K2_DFLT_0",
#if RU_INCLUDE_DESC
    "CMAC_K2_DEFAULT_0 Register",
    "Sets the value of 1st 32 bits of the the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_DFLT_0_REG_OFFSET,
    0,
    0,
    209,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_DFLT_0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_DFLT_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_DFLT_1_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_DFLT_1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_DFLT_1_REG = 
{
    "CMAC_K2_DFLT_1",
#if RU_INCLUDE_DESC
    "CMAC_K2_DEFAULT_1 Register",
    "Sets the value of 2nd 32 bits of the the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_DFLT_1_REG_OFFSET,
    0,
    0,
    210,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_DFLT_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_DFLT_2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_DFLT_2_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_DFLT_2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_DFLT_2_REG = 
{
    "CMAC_K2_DFLT_2",
#if RU_INCLUDE_DESC
    "CMAC_K2_DEFAULT_2 Register",
    "Sets the value of 3rd 32 bits of the the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_DFLT_2_REG_OFFSET,
    0,
    0,
    211,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_DFLT_2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_ENC_CMAC_K2_DFLT_3
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_ENC_CMAC_K2_DFLT_3_FIELDS[] =
{
    &NGPON_TX_ENC_CMAC_K2_DFLT_3_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_ENC_CMAC_K2_DFLT_3_REG = 
{
    "CMAC_K2_DFLT_3",
#if RU_INCLUDE_DESC
    "CMAC_K2_DEFAULT_3 Register",
    "Sets the value of 4th 32 bits of the the 128 bit encryption key."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_TX_ENC_CMAC_K2_DFLT_3_REG_OFFSET,
    0,
    0,
    212,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_ENC_CMAC_K2_DFLT_3_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_TX_ENC
 ******************************************************************************/
static const ru_reg_rec *NGPON_TX_ENC_REGS[] =
{
    &NGPON_TX_ENC_KEY_CFG_ONU1_REG,
    &NGPON_TX_ENC_KEY_CFG_ONU2_REG,
    &NGPON_TX_ENC_PYLD_ENC_KEY1_ONU1_REG,
    &NGPON_TX_ENC_PYLD_ENC_KEY2_ONU1_REG,
    &NGPON_TX_ENC_PYLD_ENC_KEY1_ONU2_REG,
    &NGPON_TX_ENC_PYLD_ENC_KEY2_ONU2_REG,
    &NGPON_TX_ENC_PLOAM_LOAD_REG,
    &NGPON_TX_ENC_PLOAMIK_ONU1_REG,
    &NGPON_TX_ENC_PLOAMIK_ONU2_REG,
    &NGPON_TX_ENC_PLOAMIK_DFLT_REG,
    &NGPON_TX_ENC_CMAC_K2_ONU1_0_REG,
    &NGPON_TX_ENC_CMAC_K2_ONU1_1_REG,
    &NGPON_TX_ENC_CMAC_K2_ONU1_2_REG,
    &NGPON_TX_ENC_CMAC_K2_ONU1_3_REG,
    &NGPON_TX_ENC_CMAC_K2_ONU2_0_REG,
    &NGPON_TX_ENC_CMAC_K2_ONU2_1_REG,
    &NGPON_TX_ENC_CMAC_K2_ONU2_2_REG,
    &NGPON_TX_ENC_CMAC_K2_ONU2_3_REG,
    &NGPON_TX_ENC_CMAC_K2_DFLT_0_REG,
    &NGPON_TX_ENC_CMAC_K2_DFLT_1_REG,
    &NGPON_TX_ENC_CMAC_K2_DFLT_2_REG,
    &NGPON_TX_ENC_CMAC_K2_DFLT_3_REG,
};

unsigned long NGPON_TX_ENC_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80169000,
#elif defined(CONFIG_BCM96856)
    0x82db9000,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_TX_ENC_BLOCK = 
{
    "NGPON_TX_ENC",
    NGPON_TX_ENC_ADDRS,
    1,
    23,
    NGPON_TX_ENC_REGS
};

/* End of file BCM6858_A0_NGPON_TX_ENC.c */
