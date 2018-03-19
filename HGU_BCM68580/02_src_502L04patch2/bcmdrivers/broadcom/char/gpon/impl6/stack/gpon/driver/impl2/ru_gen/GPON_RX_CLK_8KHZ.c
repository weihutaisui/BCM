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
 * Field: GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF
 ******************************************************************************/
const ru_field_rec GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD =
{
    "PHSDIFF",
#if RU_INCLUDE_DESC
    "Phase_difference",
    "The desired phase difference between the free-running 8kHz output clock, and the incoming frame stream. When phase difference is 0, the rising edge of the clock will be synced to the frame start.",
#endif
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD_MASK,
    0,
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD_WIDTH,
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS
 ******************************************************************************/
const ru_field_rec GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD =
{
    "PHSLOCKDIS",
#if RU_INCLUDE_DESC
    "Phase_lock_mechanism_disable",
    "When asserted, the phase lock mechanism is disabled, and the 8kHz clock will run regardless of the incoming frames state.",
#endif
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD_MASK,
    0,
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD_WIDTH,
    GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD_WIDTH,
    GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_CLK_8KHZ_CLK8KCONF
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_CLK_8KHZ_CLK8KCONF_FIELDS[] =
{
    &GPON_RX_CLK_8KHZ_CLK8KCONF_PHSDIFF_FIELD,
    &GPON_RX_CLK_8KHZ_CLK8KCONF_PHSLOCKDIS_FIELD,
    &GPON_RX_CLK_8KHZ_CLK8KCONF_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_CLK_8KHZ_CLK8KCONF_REG = 
{
    "CLK8KCONF",
#if RU_INCLUDE_DESC
    "CLOCK_8KHZ_PHASE_LOCK_MECHANISM Register",
    "This register provide control over the 8KHz clock phase lock mechanism."
    "It enables/disables the phase lock mechanism and controls the desired phase difference between the output free-running 8KHz clock and the input frames stream."
    "When the desired phase difference is 0, the rising edge of the output 8KHz clock will be synced to the frame start."
    "The desired phase difference is in resolution of GPON clock cycles."
    "If one will configure the phase difference to be 1, the rising edge of the clock will be one GPON clock cycle before the frame start."
    "",
#endif
    GPON_RX_CLK_8KHZ_CLK8KCONF_REG_OFFSET,
    0,
    0,
    102,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_RX_CLK_8KHZ_CLK8KCONF_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_CLK_8KHZ
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_CLK_8KHZ_REGS[] =
{
    &GPON_RX_CLK_8KHZ_CLK8KCONF_REG,
};

unsigned long GPON_RX_CLK_8KHZ_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130f9120,
#else
    0x80150120,
#endif
};

const ru_block_rec GPON_RX_CLK_8KHZ_BLOCK = 
{
    "GPON_RX_CLK_8KHZ",
    GPON_RX_CLK_8KHZ_ADDRS,
    1,
    1,
    GPON_RX_CLK_8KHZ_REGS
};

/* End of file BCM6858_A0GPON_RX_CLK_8KHZ.c */
