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
 * Field: NGPON_RX_PLP2_BCST_ONUID
 ******************************************************************************/
const ru_field_rec NGPON_RX_PLP2_BCST_ONUID_FIELD =
{
    "ONUID",
#if RU_INCLUDE_DESC
    "Broadcast_ONU_ID",
    "Sets the ONU_ID. This field can be changed only when the comparator is disabled.",
#endif
    NGPON_RX_PLP2_BCST_ONUID_FIELD_MASK,
    0,
    NGPON_RX_PLP2_BCST_ONUID_FIELD_WIDTH,
    NGPON_RX_PLP2_BCST_ONUID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PLP2_BCST_FILT_EN
 ******************************************************************************/
const ru_field_rec NGPON_RX_PLP2_BCST_FILT_EN_FIELD =
{
    "FILT_EN",
#if RU_INCLUDE_DESC
    "Broadcast_filter_enable",
    "Enable this ONU_ID. When the filter is disabled, PLOAM messages directed to this ONU_ID will not be passed on. This field can be changed on-the-fly during operation."
    "",
#endif
    NGPON_RX_PLP2_BCST_FILT_EN_FIELD_MASK,
    0,
    NGPON_RX_PLP2_BCST_FILT_EN_FIELD_WIDTH,
    NGPON_RX_PLP2_BCST_FILT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PLP2_BCST_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PLP2_BCST_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PLP2_BCST_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PLP2_BCST_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PLP2_BCST_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_PLP2_BCST
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PLP2_BCST_FIELDS[] =
{
    &NGPON_RX_PLP2_BCST_ONUID_FIELD,
    &NGPON_RX_PLP2_BCST_FILT_EN_FIELD,
    &NGPON_RX_PLP2_BCST_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PLP2_BCST_REG = 
{
    "BCST",
#if RU_INCLUDE_DESC
    "BROADCAST_ONU_ID %i Register",
    "Sets the ONU_ID for broadcast messages. Changing the ID is allowed only when it is disabled.",
#endif
    NGPON_RX_PLP2_BCST_REG_OFFSET,
    NGPON_RX_PLP2_BCST_REG_RAM_CNT,
    4,
    22,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    NGPON_RX_PLP2_BCST_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_PLP2
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_PLP2_REGS[] =
{
    &NGPON_RX_PLP2_BCST_REG,
};

static unsigned long NGPON_RX_PLP2_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x801600a8,
#elif defined(CONFIG_BCM96856)
    0x82db40a8,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_PLP2_BLOCK = 
{
    "NGPON_RX_PLP2",
    NGPON_RX_PLP2_ADDRS,
    1,
    1,
    NGPON_RX_PLP2_REGS
};

/* End of file BCM6858_B0_NGPON_RX_PLP2.c */
