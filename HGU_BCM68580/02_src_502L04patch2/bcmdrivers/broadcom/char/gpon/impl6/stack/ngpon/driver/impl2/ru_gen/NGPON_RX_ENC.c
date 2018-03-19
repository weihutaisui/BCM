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
 * Field: NGPON_RX_ENC_KEY_VALID_KEY1_ONU1
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY_VALID_KEY1_ONU1_FIELD =
{
    "KEY1_ONU1",
#if RU_INCLUDE_DESC
    "key1_onu1_valid",
    "Unicast key #1 ONU-ID #1 valid",
#endif
    NGPON_RX_ENC_KEY_VALID_KEY1_ONU1_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY_VALID_KEY1_ONU1_FIELD_WIDTH,
    NGPON_RX_ENC_KEY_VALID_KEY1_ONU1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY_VALID_KEY2_ONU1
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY_VALID_KEY2_ONU1_FIELD =
{
    "KEY2_ONU1",
#if RU_INCLUDE_DESC
    "key2_onu1_valid",
    "Unicast key #2 ONU-ID #1 valid",
#endif
    NGPON_RX_ENC_KEY_VALID_KEY2_ONU1_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY_VALID_KEY2_ONU1_FIELD_WIDTH,
    NGPON_RX_ENC_KEY_VALID_KEY2_ONU1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY_VALID_KEY1_ONU2
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY_VALID_KEY1_ONU2_FIELD =
{
    "KEY1_ONU2",
#if RU_INCLUDE_DESC
    "key1_onu2_valid",
    "Unicast key #1 ONU-ID #2 valid",
#endif
    NGPON_RX_ENC_KEY_VALID_KEY1_ONU2_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY_VALID_KEY1_ONU2_FIELD_WIDTH,
    NGPON_RX_ENC_KEY_VALID_KEY1_ONU2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY_VALID_KEY2_ONU2
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY_VALID_KEY2_ONU2_FIELD =
{
    "KEY2_ONU2",
#if RU_INCLUDE_DESC
    "key2_onu2_valid",
    "Unicast key #2 ONU-ID #2 valid",
#endif
    NGPON_RX_ENC_KEY_VALID_KEY2_ONU2_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY_VALID_KEY2_ONU2_FIELD_WIDTH,
    NGPON_RX_ENC_KEY_VALID_KEY2_ONU2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY_VALID_KEY1_MCST
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY_VALID_KEY1_MCST_FIELD =
{
    "KEY1_MCST",
#if RU_INCLUDE_DESC
    "key1_mcst_valid",
    "Multicast key #1 valid",
#endif
    NGPON_RX_ENC_KEY_VALID_KEY1_MCST_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY_VALID_KEY1_MCST_FIELD_WIDTH,
    NGPON_RX_ENC_KEY_VALID_KEY1_MCST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY_VALID_KEY2_MCST
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY_VALID_KEY2_MCST_FIELD =
{
    "KEY2_MCST",
#if RU_INCLUDE_DESC
    "key2_mcst_valid",
    "Multicast key #2 valid",
#endif
    NGPON_RX_ENC_KEY_VALID_KEY2_MCST_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY_VALID_KEY2_MCST_FIELD_WIDTH,
    NGPON_RX_ENC_KEY_VALID_KEY2_MCST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY_VALID_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY_VALID_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_ENC_KEY_VALID_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY_VALID_RESERVED0_FIELD_WIDTH,
    NGPON_RX_ENC_KEY_VALID_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU1
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD =
{
    "PLOAMIK_ONU1",
#if RU_INCLUDE_DESC
    "ploamik1_load",
    "Load PLOAM-IK and CMAC K2 for ONU-ID #1",
#endif
    NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD_MASK,
    0,
    NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD_WIDTH,
    NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU2
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD =
{
    "PLOAMIK_ONU2",
#if RU_INCLUDE_DESC
    "ploamik2_load",
    "Load PLOAM-IK and CMAC K2 for ONU-ID #2",
#endif
    NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD_MASK,
    0,
    NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD_WIDTH,
    NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_BCST
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD =
{
    "PLOAMIK_BCST",
#if RU_INCLUDE_DESC
    "ploamikb_load",
    "Load PLOAM-IK and CMAC K2 for broadcast ONU-ID",
#endif
    NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD_MASK,
    0,
    NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD_WIDTH,
    NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_PLOAM_LOAD_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_PLOAM_LOAD_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_ENC_PLOAM_LOAD_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_ENC_PLOAM_LOAD_RESERVED0_FIELD_WIDTH,
    NGPON_RX_ENC_PLOAM_LOAD_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY1_ONU1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY1_ONU1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_RX_ENC_KEY1_ONU1_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY1_ONU1_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_KEY1_ONU1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY2_ONU1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY2_ONU1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_RX_ENC_KEY2_ONU1_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY2_ONU1_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_KEY2_ONU1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY1_ONU2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY1_ONU2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_RX_ENC_KEY1_ONU2_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY1_ONU2_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_KEY1_ONU2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY2_ONU2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY2_ONU2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_RX_ENC_KEY2_ONU2_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY2_ONU2_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_KEY2_ONU2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_PLOAMIK_ONU1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_PLOAMIK_ONU1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_RX_ENC_PLOAMIK_ONU1_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_PLOAMIK_ONU1_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_PLOAMIK_ONU1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_PLOAMIK_ONU2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_PLOAMIK_ONU2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_RX_ENC_PLOAMIK_ONU2_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_PLOAMIK_ONU2_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_PLOAMIK_ONU2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_PLOAMIK_BCST_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_PLOAMIK_BCST_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_RX_ENC_PLOAMIK_BCST_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_PLOAMIK_BCST_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_PLOAMIK_BCST_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_ONU1_0_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_ONU1_0_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_ONU1_0_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_ONU1_0_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_ONU1_0_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_ONU1_1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_ONU1_1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_ONU1_1_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_ONU1_1_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_ONU1_1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_ONU1_2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_ONU1_2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_ONU1_2_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_ONU1_2_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_ONU1_2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_ONU1_3_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_ONU1_3_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_ONU1_3_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_ONU1_3_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_ONU1_3_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_ONU2_0_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_ONU2_0_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_ONU2_0_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_ONU2_0_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_ONU2_0_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_ONU2_1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_ONU2_1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_ONU2_1_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_ONU2_1_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_ONU2_1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_ONU2_2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_ONU2_2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_ONU2_2_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_ONU2_2_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_ONU2_2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_ONU2_3_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_ONU2_3_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "rValue",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_ONU2_3_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_ONU2_3_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_ONU2_3_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_BCST_0_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_BCST_0_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_BCST_0_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_BCST_0_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_BCST_0_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_BCST_1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_BCST_1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_BCST_1_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_BCST_1_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_BCST_1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_BCST_2_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_BCST_2_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_BCST_2_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_BCST_2_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_BCST_2_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_BCST_3_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_BCST_3_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_BCST_3_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_BCST_3_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_BCST_3_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY1_MCST_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY1_MCST_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_RX_ENC_KEY1_MCST_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY1_MCST_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_KEY1_MCST_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_KEY2_MCST_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_KEY2_MCST_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Decryption key",
#endif
    NGPON_RX_ENC_KEY2_MCST_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_KEY2_MCST_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_KEY2_MCST_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_PLOAMIK_LOOKUP_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_PLOAMIK_LOOKUP_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Value",
#endif
    NGPON_RX_ENC_PLOAMIK_LOOKUP_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_PLOAMIK_LOOKUP_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_PLOAMIK_LOOKUP_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_ENC_CMACK2_LOOKUP_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_ENC_CMACK2_LOOKUP_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "CMAC K2 value",
#endif
    NGPON_RX_ENC_CMACK2_LOOKUP_VALUE_FIELD_MASK,
    0,
    NGPON_RX_ENC_CMACK2_LOOKUP_VALUE_FIELD_WIDTH,
    NGPON_RX_ENC_CMACK2_LOOKUP_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_ENC_KEY_VALID
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_KEY_VALID_FIELDS[] =
{
    &NGPON_RX_ENC_KEY_VALID_KEY1_ONU1_FIELD,
    &NGPON_RX_ENC_KEY_VALID_KEY2_ONU1_FIELD,
    &NGPON_RX_ENC_KEY_VALID_KEY1_ONU2_FIELD,
    &NGPON_RX_ENC_KEY_VALID_KEY2_ONU2_FIELD,
    &NGPON_RX_ENC_KEY_VALID_KEY1_MCST_FIELD,
    &NGPON_RX_ENC_KEY_VALID_KEY2_MCST_FIELD,
    &NGPON_RX_ENC_KEY_VALID_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_KEY_VALID_REG = 
{
    "KEY_VALID",
#if RU_INCLUDE_DESC
    "KEY_VALIDITY Register",
    "Valid flag for each key",
#endif
    NGPON_RX_ENC_KEY_VALID_REG_OFFSET,
    0,
    0,
    44,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    7,
    NGPON_RX_ENC_KEY_VALID_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_PLOAM_LOAD
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_PLOAM_LOAD_FIELDS[] =
{
    &NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU1_FIELD,
    &NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_ONU2_FIELD,
    &NGPON_RX_ENC_PLOAM_LOAD_PLOAMIK_BCST_FIELD,
    &NGPON_RX_ENC_PLOAM_LOAD_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_PLOAM_LOAD_REG = 
{
    "PLOAM_LOAD",
#if RU_INCLUDE_DESC
    "PLOAM_KEY_LOAD Register",
    "Load commands for PLOAM keys."
    "Writing 1 to a bit triggers copying the information from a configuration register to a shadow register, which is used for the actual decryption. Writing 0 is ignored. After the actual copying is done, hardware changes the bit back to 0.",
#endif
    NGPON_RX_ENC_PLOAM_LOAD_REG_OFFSET,
    0,
    0,
    45,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_RX_ENC_PLOAM_LOAD_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_KEY1_ONU1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_KEY1_ONU1_FIELDS[] =
{
    &NGPON_RX_ENC_KEY1_ONU1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_KEY1_ONU1_REG = 
{
    "KEY1_ONU1",
#if RU_INCLUDE_DESC
    "UNICAST_KEY1_ONU1 %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    "These registers must be changed only when the key is not valid.",
#endif
    NGPON_RX_ENC_KEY1_ONU1_REG_OFFSET,
    NGPON_RX_ENC_KEY1_ONU1_REG_RAM_CNT,
    4,
    46,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_KEY1_ONU1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_KEY2_ONU1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_KEY2_ONU1_FIELDS[] =
{
    &NGPON_RX_ENC_KEY2_ONU1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_KEY2_ONU1_REG = 
{
    "KEY2_ONU1",
#if RU_INCLUDE_DESC
    "UNICAST_KEY2_ONU1 %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    "These registers must be changed only when the key is not valid.",
#endif
    NGPON_RX_ENC_KEY2_ONU1_REG_OFFSET,
    NGPON_RX_ENC_KEY2_ONU1_REG_RAM_CNT,
    4,
    47,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_KEY2_ONU1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_KEY1_ONU2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_KEY1_ONU2_FIELDS[] =
{
    &NGPON_RX_ENC_KEY1_ONU2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_KEY1_ONU2_REG = 
{
    "KEY1_ONU2",
#if RU_INCLUDE_DESC
    "UNICAST_KEY1_ONU2 %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    "These registers must be changed only when the key is not valid.",
#endif
    NGPON_RX_ENC_KEY1_ONU2_REG_OFFSET,
    NGPON_RX_ENC_KEY1_ONU2_REG_RAM_CNT,
    4,
    48,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_KEY1_ONU2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_KEY2_ONU2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_KEY2_ONU2_FIELDS[] =
{
    &NGPON_RX_ENC_KEY2_ONU2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_KEY2_ONU2_REG = 
{
    "KEY2_ONU2",
#if RU_INCLUDE_DESC
    "UNICAST_KEY2_ONU2 %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    "These registers must be changed only when the key is not valid.",
#endif
    NGPON_RX_ENC_KEY2_ONU2_REG_OFFSET,
    NGPON_RX_ENC_KEY2_ONU2_REG_RAM_CNT,
    4,
    49,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_KEY2_ONU2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_PLOAMIK_ONU1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_PLOAMIK_ONU1_FIELDS[] =
{
    &NGPON_RX_ENC_PLOAMIK_ONU1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_PLOAMIK_ONU1_REG = 
{
    "PLOAMIK_ONU1",
#if RU_INCLUDE_DESC
    "PLOAM_IK_ONU1 %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_PLOAMIK_ONU1_REG_OFFSET,
    NGPON_RX_ENC_PLOAMIK_ONU1_REG_RAM_CNT,
    4,
    50,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_PLOAMIK_ONU1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_PLOAMIK_ONU2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_PLOAMIK_ONU2_FIELDS[] =
{
    &NGPON_RX_ENC_PLOAMIK_ONU2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_PLOAMIK_ONU2_REG = 
{
    "PLOAMIK_ONU2",
#if RU_INCLUDE_DESC
    "PLOAM_IK_ONU2 %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_PLOAMIK_ONU2_REG_OFFSET,
    NGPON_RX_ENC_PLOAMIK_ONU2_REG_RAM_CNT,
    4,
    51,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_PLOAMIK_ONU2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_PLOAMIK_BCST
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_PLOAMIK_BCST_FIELDS[] =
{
    &NGPON_RX_ENC_PLOAMIK_BCST_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_PLOAMIK_BCST_REG = 
{
    "PLOAMIK_BCST",
#if RU_INCLUDE_DESC
    "PLOAM_IK_BCST %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_PLOAMIK_BCST_REG_OFFSET,
    NGPON_RX_ENC_PLOAMIK_BCST_REG_RAM_CNT,
    4,
    52,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_PLOAMIK_BCST_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_ONU1_0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_ONU1_0_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_ONU1_0_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_ONU1_0_REG = 
{
    "CMACK2_ONU1_0",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU1_1 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 1 - MSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_ONU1_0_REG_OFFSET,
    0,
    0,
    53,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_ONU1_0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_ONU1_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_ONU1_1_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_ONU1_1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_ONU1_1_REG = 
{
    "CMACK2_ONU1_1",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU1_2 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 2."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_ONU1_1_REG_OFFSET,
    0,
    0,
    54,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_ONU1_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_ONU1_2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_ONU1_2_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_ONU1_2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_ONU1_2_REG = 
{
    "CMACK2_ONU1_2",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU1_3 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 3."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD  is written.",
#endif
    NGPON_RX_ENC_CMACK2_ONU1_2_REG_OFFSET,
    0,
    0,
    55,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_ONU1_2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_ONU1_3
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_ONU1_3_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_ONU1_3_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_ONU1_3_REG = 
{
    "CMACK2_ONU1_3",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU1_4 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 4 - LSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_ONU1_3_REG_OFFSET,
    0,
    0,
    56,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_ONU1_3_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_ONU2_0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_ONU2_0_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_ONU2_0_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_ONU2_0_REG = 
{
    "CMACK2_ONU2_0",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU2_1 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 1 - MSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_ONU2_0_REG_OFFSET,
    0,
    0,
    57,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_ONU2_0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_ONU2_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_ONU2_1_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_ONU2_1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_ONU2_1_REG = 
{
    "CMACK2_ONU2_1",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU2_2 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 2."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_ONU2_1_REG_OFFSET,
    0,
    0,
    58,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_ONU2_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_ONU2_2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_ONU2_2_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_ONU2_2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_ONU2_2_REG = 
{
    "CMACK2_ONU2_2",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU2_3 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 3."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_ONU2_2_REG_OFFSET,
    0,
    0,
    59,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_ONU2_2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_ONU2_3
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_ONU2_3_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_ONU2_3_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_ONU2_3_REG = 
{
    "CMACK2_ONU2_3",
#if RU_INCLUDE_DESC
    "CMAC_K2_ONU2_4 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 4 - LSB."
    ""
    "Update takes place only when the appropriate bit in PLOAM_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_ONU2_3_REG_OFFSET,
    0,
    0,
    60,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_ONU2_3_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_BCST_0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_BCST_0_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_BCST_0_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_BCST_0_REG = 
{
    "CMACK2_BCST_0",
#if RU_INCLUDE_DESC
    "CMAC_K2_BCST_1 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 1 - MSB."
    ""
    "Update takes place only when the appropriate bit in BCST_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_BCST_0_REG_OFFSET,
    0,
    0,
    61,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_BCST_0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_BCST_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_BCST_1_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_BCST_1_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_BCST_1_REG = 
{
    "CMACK2_BCST_1",
#if RU_INCLUDE_DESC
    "CMAC_K2_BCST_2 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 2."
    ""
    "Update takes place only when the appropriate bit in BCST_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_BCST_1_REG_OFFSET,
    0,
    0,
    62,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_BCST_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_BCST_2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_BCST_2_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_BCST_2_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_BCST_2_REG = 
{
    "CMACK2_BCST_2",
#if RU_INCLUDE_DESC
    "CMAC_K2_BCST_3 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 3."
    ""
    "Update takes place only when the appropriate bit in BCST_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_BCST_2_REG_OFFSET,
    0,
    0,
    63,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_BCST_2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_BCST_3
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_BCST_3_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_BCST_3_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_BCST_3_REG = 
{
    "CMACK2_BCST_3",
#if RU_INCLUDE_DESC
    "CMAC_K2_BCST_4 Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Part 4 - LSB."
    ""
    "Update takes place only when the appropriate bit in BCST_LOAD is written.",
#endif
    NGPON_RX_ENC_CMACK2_BCST_3_REG_OFFSET,
    0,
    0,
    64,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_BCST_3_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_KEY1_MCST
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_KEY1_MCST_FIELDS[] =
{
    &NGPON_RX_ENC_KEY1_MCST_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_KEY1_MCST_REG = 
{
    "KEY1_MCST",
#if RU_INCLUDE_DESC
    "MULTICAST_KEY1 %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    "These registers must be changed only when the key is not valid.",
#endif
    NGPON_RX_ENC_KEY1_MCST_REG_OFFSET,
    NGPON_RX_ENC_KEY1_MCST_REG_RAM_CNT,
    4,
    65,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_KEY1_MCST_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_KEY2_MCST
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_KEY2_MCST_FIELDS[] =
{
    &NGPON_RX_ENC_KEY2_MCST_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_KEY2_MCST_REG = 
{
    "KEY2_MCST",
#if RU_INCLUDE_DESC
    "MULTICAST_KEY2 %i Register",
    "Sets the value of the 128 bit decryption key. Array of 4 32 bit registers. Register 0 in the array sets the MSB, register 3 sets the LSB."
    "These registers must be changed only when the key is not valid.",
#endif
    NGPON_RX_ENC_KEY2_MCST_REG_OFFSET,
    NGPON_RX_ENC_KEY2_MCST_REG_RAM_CNT,
    4,
    66,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_KEY2_MCST_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_PLOAMIK_LOOKUP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_PLOAMIK_LOOKUP_FIELDS[] =
{
    &NGPON_RX_ENC_PLOAMIK_LOOKUP_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_PLOAMIK_LOOKUP_REG = 
{
    "PLOAMIK_LOOKUP",
#if RU_INCLUDE_DESC
    "PLOAM_IK_LOOKUP %i Register",
    "Shows the active PLOAM-IKs currently in use. Array of 3x4 32 bit registers. Register 0 in the array is ONU-ID #1 MSB, register 3 is ONU-ID #1 LSB, registers 4-7 are ONU-ID #2 and registers 8-11 belong to the broadcast ONU-ID."
    "",
#endif
    NGPON_RX_ENC_PLOAMIK_LOOKUP_REG_OFFSET,
    NGPON_RX_ENC_PLOAMIK_LOOKUP_REG_RAM_CNT,
    4,
    67,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_PLOAMIK_LOOKUP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_ENC_CMACK2_LOOKUP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_ENC_CMACK2_LOOKUP_FIELDS[] =
{
    &NGPON_RX_ENC_CMACK2_LOOKUP_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_ENC_CMACK2_LOOKUP_REG = 
{
    "CMACK2_LOOKUP",
#if RU_INCLUDE_DESC
    "CMAC_K2_LOOKUP %i Register",
    "Shows the active CMAC K2 values currently in use. Array of 3x4 32 bit registers. Register 0 in the array is ONU-ID #1 MSB, register 3 is ONU-ID #1 LSB, registers 4-7 are ONU-ID #2 and registers 8-11 belong to the broadcast ONU-ID."
    "",
#endif
    NGPON_RX_ENC_CMACK2_LOOKUP_REG_OFFSET,
    NGPON_RX_ENC_CMACK2_LOOKUP_REG_RAM_CNT,
    4,
    68,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_ENC_CMACK2_LOOKUP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_ENC
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_ENC_REGS[] =
{
    &NGPON_RX_ENC_KEY_VALID_REG,
    &NGPON_RX_ENC_PLOAM_LOAD_REG,
    &NGPON_RX_ENC_KEY1_ONU1_REG,
    &NGPON_RX_ENC_KEY2_ONU1_REG,
    &NGPON_RX_ENC_KEY1_ONU2_REG,
    &NGPON_RX_ENC_KEY2_ONU2_REG,
    &NGPON_RX_ENC_PLOAMIK_ONU1_REG,
    &NGPON_RX_ENC_PLOAMIK_ONU2_REG,
    &NGPON_RX_ENC_PLOAMIK_BCST_REG,
    &NGPON_RX_ENC_CMACK2_ONU1_0_REG,
    &NGPON_RX_ENC_CMACK2_ONU1_1_REG,
    &NGPON_RX_ENC_CMACK2_ONU1_2_REG,
    &NGPON_RX_ENC_CMACK2_ONU1_3_REG,
    &NGPON_RX_ENC_CMACK2_ONU2_0_REG,
    &NGPON_RX_ENC_CMACK2_ONU2_1_REG,
    &NGPON_RX_ENC_CMACK2_ONU2_2_REG,
    &NGPON_RX_ENC_CMACK2_ONU2_3_REG,
    &NGPON_RX_ENC_CMACK2_BCST_0_REG,
    &NGPON_RX_ENC_CMACK2_BCST_1_REG,
    &NGPON_RX_ENC_CMACK2_BCST_2_REG,
    &NGPON_RX_ENC_CMACK2_BCST_3_REG,
    &NGPON_RX_ENC_KEY1_MCST_REG,
    &NGPON_RX_ENC_KEY2_MCST_REG,
    &NGPON_RX_ENC_PLOAMIK_LOOKUP_REG,
    &NGPON_RX_ENC_CMACK2_LOOKUP_REG,
};

unsigned long NGPON_RX_ENC_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80160700,
#elif defined(CONFIG_BCM96856)
    0x82db4700,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_ENC_BLOCK = 
{
    "NGPON_RX_ENC",
    NGPON_RX_ENC_ADDRS,
    1,
    25,
    NGPON_RX_ENC_REGS
};

/* End of file BCM6858_A0_NGPON_RX_ENC.c */
