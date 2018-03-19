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
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_0_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_0_DBR_FIELD =
{
    "TX_TCONT_0_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_0_DBR",
    "DBR for T-CONT 0 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_0_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_0_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_0_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_1_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_1_DBR_FIELD =
{
    "TX_TCONT_1_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_1_DBR",
    "DBR for T-CONT 1 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_1_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_1_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_1_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_2_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_2_DBR_FIELD =
{
    "TX_TCONT_2_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_2_DBR",
    "DBR for T-CONT 2 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_2_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_2_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_2_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_3_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_3_DBR_FIELD =
{
    "TX_TCONT_3_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_3_DBR",
    "DBR for T-CONT 3 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_3_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_3_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_3_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_4_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_4_DBR_FIELD =
{
    "TX_TCONT_4_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_4_DBR",
    "DBR for T-CONT 4 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_4_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_4_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_4_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_5_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_5_DBR_FIELD =
{
    "TX_TCONT_5_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_5_DBR",
    "DBR for T-CONT 5 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_5_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_5_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_5_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_6_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_6_DBR_FIELD =
{
    "TX_TCONT_6_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_6_DBR",
    "DBR for T-CONT 6 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_6_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_6_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_6_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_7_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_7_DBR_FIELD =
{
    "TX_TCONT_7_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_7_DBR",
    "DBR for T-CONT 7 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_7_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_7_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_7_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_8_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_8_DBR_FIELD =
{
    "TX_TCONT_8_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_8_DBR",
    "DBR for T-CONT 8 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_8_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_8_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_8_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_9_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_9_DBR_FIELD =
{
    "TX_TCONT_9_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_9_DBR",
    "DBR for T-CONT 9 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_9_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_9_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_9_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_10_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_10_DBR_FIELD =
{
    "TX_TCONT_10_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_10_DBR",
    "DBR for T-CONT 10 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_10_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_10_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_10_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_11_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_11_DBR_FIELD =
{
    "TX_TCONT_11_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_11_DBR",
    "DBR for T-CONT 11 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_11_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_11_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_11_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_12_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_12_DBR_FIELD =
{
    "TX_TCONT_12_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_12_DBR",
    "DBR for T-CONT 12 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_12_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_12_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_12_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_13_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_13_DBR_FIELD =
{
    "TX_TCONT_13_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_13_DBR",
    "DBR for T-CONT 13 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_13_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_13_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_13_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_14_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_14_DBR_FIELD =
{
    "TX_TCONT_14_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_14_DBR",
    "DBR for T-CONT 14 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_14_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_14_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_14_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_15_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_15_DBR_FIELD =
{
    "TX_TCONT_15_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_15_DBR",
    "DBR for T-CONT 15 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_15_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_15_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_15_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_16_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_16_DBR_FIELD =
{
    "TX_TCONT_16_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_16_DBR",
    "DBR for T-CONT 16 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_16_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_16_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_16_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_17_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_17_DBR_FIELD =
{
    "TX_TCONT_17_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_17_DBR",
    "DBR for T-CONT 17 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_17_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_17_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_17_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_18_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_18_DBR_FIELD =
{
    "TX_TCONT_18_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_18_DBR",
    "DBR for T-CONT 18 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_18_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_18_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_18_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_19_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_19_DBR_FIELD =
{
    "TX_TCONT_19_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_19_DBR",
    "DBR for T-CONT 19 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_19_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_19_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_19_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_20_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_20_DBR_FIELD =
{
    "TX_TCONT_20_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_20_DBR",
    "DBR for T-CONT 20 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_20_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_20_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_20_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_21_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_21_DBR_FIELD =
{
    "TX_TCONT_21_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_21_DBR",
    "DBR for T-CONT 21 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_21_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_21_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_21_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_22_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_22_DBR_FIELD =
{
    "TX_TCONT_22_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_22_DBR",
    "DBR for T-CONT 22 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_22_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_22_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_22_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_23_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_23_DBR_FIELD =
{
    "TX_TCONT_23_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_23_DBR",
    "DBR for T-CONT 23 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_23_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_23_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_23_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_24_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_24_DBR_FIELD =
{
    "TX_TCONT_24_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_24_DBR",
    "DBR for T-CONT 24 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_24_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_24_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_24_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_25_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_25_DBR_FIELD =
{
    "TX_TCONT_25_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_25_DBR",
    "DBR for T-CONT 25 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_25_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_25_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_25_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_26_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_26_DBR_FIELD =
{
    "TX_TCONT_26_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_26_DBR",
    "DBR for T-CONT 26 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_26_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_26_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_26_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_27_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_27_DBR_FIELD =
{
    "TX_TCONT_27_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_27_DBR",
    "DBR for T-CONT 27 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_27_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_27_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_27_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_28_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_28_DBR_FIELD =
{
    "TX_TCONT_28_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_28_DBR",
    "DBR for T-CONT 28 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_28_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_28_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_28_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_29_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_29_DBR_FIELD =
{
    "TX_TCONT_29_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_29_DBR",
    "DBR for T-CONT 29 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_29_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_29_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_29_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_30_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_30_DBR_FIELD =
{
    "TX_TCONT_30_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_30_DBR",
    "DBR for T-CONT 30 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_30_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_30_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_30_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR0_TX_TCONT_31_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR0_TX_TCONT_31_DBR_FIELD =
{
    "TX_TCONT_31_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_31_DBR",
    "DBR for T-CONT 31 transmission  indication",
#endif
    NGPON_TX_INT_ISR0_TX_TCONT_31_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR0_TX_TCONT_31_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR0_TX_TCONT_31_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_0_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_0_DBR_FIELD =
{
    "TX_TCONT_0_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_0_DBR",
    "DBR for T-CONT 0 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_0_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_0_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_0_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_1_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_1_DBR_FIELD =
{
    "TX_TCONT_1_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_1_DBR",
    "DBR for T-CONT 1 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_1_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_1_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_1_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_2_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_2_DBR_FIELD =
{
    "TX_TCONT_2_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_2_DBR",
    "DBR for T-CONT 2 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_2_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_2_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_2_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_3_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_3_DBR_FIELD =
{
    "TX_TCONT_3_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_3_DBR",
    "DBR for T-CONT 3 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_3_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_3_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_3_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_4_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_4_DBR_FIELD =
{
    "TX_TCONT_4_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_4_DBR",
    "DBR for T-CONT 4 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_4_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_4_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_4_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_5_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_5_DBR_FIELD =
{
    "TX_TCONT_5_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_5_DBR",
    "DBR for T-CONT 5 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_5_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_5_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_5_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_6_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_6_DBR_FIELD =
{
    "TX_TCONT_6_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_6_DBR",
    "DBR for T-CONT 6 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_6_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_6_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_6_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_7_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_7_DBR_FIELD =
{
    "TX_TCONT_7_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_7_DBR",
    "DBR for T-CONT 7 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_7_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_7_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_7_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_8_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_8_DBR_FIELD =
{
    "TX_TCONT_8_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_8_DBR",
    "DBR for T-CONT 8 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_8_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_8_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_8_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_9_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_9_DBR_FIELD =
{
    "TX_TCONT_9_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_9_DBR",
    "DBR for T-CONT 9 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_9_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_9_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_9_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_10_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_10_DBR_FIELD =
{
    "TX_TCONT_10_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_10_DBR",
    "DBR for T-CONT 10 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_10_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_10_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_10_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_11_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_11_DBR_FIELD =
{
    "TX_TCONT_11_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_11_DBR",
    "DBR for T-CONT 11 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_11_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_11_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_11_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_12_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_12_DBR_FIELD =
{
    "TX_TCONT_12_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_12_DBR",
    "DBR for T-CONT 12 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_12_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_12_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_12_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_13_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_13_DBR_FIELD =
{
    "TX_TCONT_13_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_13_DBR",
    "DBR for T-CONT 13 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_13_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_13_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_13_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_14_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_14_DBR_FIELD =
{
    "TX_TCONT_14_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_14_DBR",
    "DBR for T-CONT 14 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_14_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_14_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_14_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_15_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_15_DBR_FIELD =
{
    "TX_TCONT_15_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_15_DBR",
    "DBR for T-CONT 15 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_15_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_15_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_15_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_16_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_16_DBR_FIELD =
{
    "TX_TCONT_16_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_16_DBR",
    "DBR for T-CONT 16 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_16_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_16_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_16_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_17_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_17_DBR_FIELD =
{
    "TX_TCONT_17_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_17_DBR",
    "DBR for T-CONT 17 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_17_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_17_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_17_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_18_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_18_DBR_FIELD =
{
    "TX_TCONT_18_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_18_DBR",
    "DBR for T-CONT 18 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_18_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_18_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_18_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_19_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_19_DBR_FIELD =
{
    "TX_TCONT_19_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_19_DBR",
    "DBR for T-CONT 19 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_19_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_19_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_19_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_20_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_20_DBR_FIELD =
{
    "TX_TCONT_20_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_20_DBR",
    "DBR for T-CONT 20 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_20_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_20_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_20_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_21_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_21_DBR_FIELD =
{
    "TX_TCONT_21_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_21_DBR",
    "DBR for T-CONT 21 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_21_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_21_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_21_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_22_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_22_DBR_FIELD =
{
    "TX_TCONT_22_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_22_DBR",
    "DBR for T-CONT 22 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_22_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_22_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_22_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_23_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_23_DBR_FIELD =
{
    "TX_TCONT_23_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_23_DBR",
    "DBR for T-CONT 23 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_23_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_23_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_23_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_24_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_24_DBR_FIELD =
{
    "TX_TCONT_24_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_24_DBR",
    "DBR for T-CONT 24 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_24_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_24_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_24_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_25_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_25_DBR_FIELD =
{
    "TX_TCONT_25_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_25_DBR",
    "DBR for T-CONT 25 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_25_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_25_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_25_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_26_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_26_DBR_FIELD =
{
    "TX_TCONT_26_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_26_DBR",
    "DBR for T-CONT 26 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_26_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_26_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_26_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_27_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_27_DBR_FIELD =
{
    "TX_TCONT_27_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_27_DBR",
    "DBR for T-CONT 27 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_27_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_27_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_27_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_28_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_28_DBR_FIELD =
{
    "TX_TCONT_28_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_28_DBR",
    "DBR for T-CONT 28 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_28_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_28_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_28_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_29_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_29_DBR_FIELD =
{
    "TX_TCONT_29_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_29_DBR",
    "DBR for T-CONT 29 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_29_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_29_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_29_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_30_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_30_DBR_FIELD =
{
    "TX_TCONT_30_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_30_DBR",
    "DBR for T-CONT 30 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_30_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_30_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_30_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM0_TX_TCONT_31_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM0_TX_TCONT_31_DBR_FIELD =
{
    "TX_TCONT_31_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_31_DBR",
    "DBR for T-CONT 31 transmission  indication",
#endif
    NGPON_TX_INT_ISM0_TX_TCONT_31_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM0_TX_TCONT_31_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM0_TX_TCONT_31_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_0_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_0_DBR_FIELD =
{
    "TX_TCONT_0_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_0_DBR",
    "DBR for T-CONT 0 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_0_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_0_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_0_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_1_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_1_DBR_FIELD =
{
    "TX_TCONT_1_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_1_DBR",
    "DBR for T-CONT 1 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_1_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_1_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_1_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_2_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_2_DBR_FIELD =
{
    "TX_TCONT_2_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_2_DBR",
    "DBR for T-CONT 2 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_2_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_2_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_2_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_3_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_3_DBR_FIELD =
{
    "TX_TCONT_3_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_3_DBR",
    "DBR for T-CONT 3 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_3_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_3_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_3_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_4_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_4_DBR_FIELD =
{
    "TX_TCONT_4_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_4_DBR",
    "DBR for T-CONT 4 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_4_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_4_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_4_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_5_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_5_DBR_FIELD =
{
    "TX_TCONT_5_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_5_DBR",
    "DBR for T-CONT 5 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_5_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_5_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_5_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_6_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_6_DBR_FIELD =
{
    "TX_TCONT_6_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_6_DBR",
    "DBR for T-CONT 6 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_6_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_6_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_6_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_7_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_7_DBR_FIELD =
{
    "TX_TCONT_7_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_7_DBR",
    "DBR for T-CONT 7 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_7_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_7_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_7_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_8_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_8_DBR_FIELD =
{
    "TX_TCONT_8_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_8_DBR",
    "DBR for T-CONT 8 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_8_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_8_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_8_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_9_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_9_DBR_FIELD =
{
    "TX_TCONT_9_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_9_DBR",
    "DBR for T-CONT 9 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_9_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_9_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_9_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_10_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_10_DBR_FIELD =
{
    "TX_TCONT_10_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_10_DBR",
    "DBR for T-CONT 10 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_10_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_10_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_10_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_11_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_11_DBR_FIELD =
{
    "TX_TCONT_11_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_11_DBR",
    "DBR for T-CONT 11 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_11_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_11_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_11_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_12_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_12_DBR_FIELD =
{
    "TX_TCONT_12_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_12_DBR",
    "DBR for T-CONT 12 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_12_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_12_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_12_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_13_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_13_DBR_FIELD =
{
    "TX_TCONT_13_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_13_DBR",
    "DBR for T-CONT 13 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_13_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_13_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_13_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_14_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_14_DBR_FIELD =
{
    "TX_TCONT_14_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_14_DBR",
    "DBR for T-CONT 14 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_14_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_14_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_14_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_15_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_15_DBR_FIELD =
{
    "TX_TCONT_15_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_15_DBR",
    "DBR for T-CONT 15 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_15_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_15_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_15_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_16_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_16_DBR_FIELD =
{
    "TX_TCONT_16_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_16_DBR",
    "DBR for T-CONT 16 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_16_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_16_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_16_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_17_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_17_DBR_FIELD =
{
    "TX_TCONT_17_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_17_DBR",
    "DBR for T-CONT 17 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_17_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_17_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_17_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_18_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_18_DBR_FIELD =
{
    "TX_TCONT_18_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_18_DBR",
    "DBR for T-CONT 18 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_18_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_18_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_18_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_19_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_19_DBR_FIELD =
{
    "TX_TCONT_19_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_19_DBR",
    "DBR for T-CONT 19 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_19_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_19_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_19_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_20_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_20_DBR_FIELD =
{
    "TX_TCONT_20_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_20_DBR",
    "DBR for T-CONT 20 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_20_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_20_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_20_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_21_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_21_DBR_FIELD =
{
    "TX_TCONT_21_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_21_DBR",
    "DBR for T-CONT 21 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_21_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_21_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_21_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_22_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_22_DBR_FIELD =
{
    "TX_TCONT_22_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_22_DBR",
    "DBR for T-CONT 22 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_22_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_22_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_22_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_23_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_23_DBR_FIELD =
{
    "TX_TCONT_23_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_23_DBR",
    "DBR for T-CONT 23 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_23_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_23_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_23_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_24_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_24_DBR_FIELD =
{
    "TX_TCONT_24_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_24_DBR",
    "DBR for T-CONT 24 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_24_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_24_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_24_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_25_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_25_DBR_FIELD =
{
    "TX_TCONT_25_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_25_DBR",
    "DBR for T-CONT 25 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_25_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_25_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_25_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_26_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_26_DBR_FIELD =
{
    "TX_TCONT_26_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_26_DBR",
    "DBR for T-CONT 26 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_26_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_26_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_26_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_27_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_27_DBR_FIELD =
{
    "TX_TCONT_27_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_27_DBR",
    "DBR for T-CONT 27 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_27_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_27_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_27_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_28_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_28_DBR_FIELD =
{
    "TX_TCONT_28_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_28_DBR",
    "DBR for T-CONT 28 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_28_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_28_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_28_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_29_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_29_DBR_FIELD =
{
    "TX_TCONT_29_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_29_DBR",
    "DBR for T-CONT 29 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_29_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_29_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_29_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_30_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_30_DBR_FIELD =
{
    "TX_TCONT_30_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_30_DBR",
    "DBR for T-CONT 30 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_30_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_30_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_30_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER0_TX_TCONT_31_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER0_TX_TCONT_31_DBR_FIELD =
{
    "TX_TCONT_31_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_31_DBR",
    "DBR for T-CONT 31 transmission  indication",
#endif
    NGPON_TX_INT_IER0_TX_TCONT_31_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER0_TX_TCONT_31_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER0_TX_TCONT_31_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_0_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_0_DBR_FIELD =
{
    "TX_TCONT_0_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_0_DBR",
    "DBR for T-CONT 0 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_0_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_0_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_0_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_1_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_1_DBR_FIELD =
{
    "TX_TCONT_1_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_1_DBR",
    "DBR for T-CONT 1 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_1_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_1_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_1_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_2_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_2_DBR_FIELD =
{
    "TX_TCONT_2_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_2_DBR",
    "DBR for T-CONT 2 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_2_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_2_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_2_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_3_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_3_DBR_FIELD =
{
    "TX_TCONT_3_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_3_DBR",
    "DBR for T-CONT 3 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_3_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_3_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_3_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_4_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_4_DBR_FIELD =
{
    "TX_TCONT_4_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_4_DBR",
    "DBR for T-CONT 4 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_4_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_4_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_4_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_5_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_5_DBR_FIELD =
{
    "TX_TCONT_5_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_5_DBR",
    "DBR for T-CONT 5 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_5_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_5_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_5_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_6_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_6_DBR_FIELD =
{
    "TX_TCONT_6_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_6_DBR",
    "DBR for T-CONT 6 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_6_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_6_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_6_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_7_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_7_DBR_FIELD =
{
    "TX_TCONT_7_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_7_DBR",
    "DBR for T-CONT 7 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_7_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_7_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_7_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_8_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_8_DBR_FIELD =
{
    "TX_TCONT_8_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_8_DBR",
    "DBR for T-CONT 8 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_8_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_8_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_8_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_9_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_9_DBR_FIELD =
{
    "TX_TCONT_9_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_9_DBR",
    "DBR for T-CONT 9 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_9_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_9_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_9_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_10_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_10_DBR_FIELD =
{
    "TX_TCONT_10_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_10_DBR",
    "DBR for T-CONT 10 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_10_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_10_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_10_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_11_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_11_DBR_FIELD =
{
    "TX_TCONT_11_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_11_DBR",
    "DBR for T-CONT 11 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_11_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_11_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_11_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_12_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_12_DBR_FIELD =
{
    "TX_TCONT_12_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_12_DBR",
    "DBR for T-CONT 12 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_12_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_12_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_12_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_13_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_13_DBR_FIELD =
{
    "TX_TCONT_13_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_13_DBR",
    "DBR for T-CONT 13 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_13_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_13_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_13_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_14_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_14_DBR_FIELD =
{
    "TX_TCONT_14_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_14_DBR",
    "DBR for T-CONT 14 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_14_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_14_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_14_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_15_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_15_DBR_FIELD =
{
    "TX_TCONT_15_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_15_DBR",
    "DBR for T-CONT 15 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_15_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_15_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_15_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_16_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_16_DBR_FIELD =
{
    "TX_TCONT_16_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_16_DBR",
    "DBR for T-CONT 16 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_16_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_16_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_16_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_17_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_17_DBR_FIELD =
{
    "TX_TCONT_17_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_17_DBR",
    "DBR for T-CONT 17 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_17_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_17_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_17_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_18_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_18_DBR_FIELD =
{
    "TX_TCONT_18_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_18_DBR",
    "DBR for T-CONT 18 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_18_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_18_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_18_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_19_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_19_DBR_FIELD =
{
    "TX_TCONT_19_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_19_DBR",
    "DBR for T-CONT 19 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_19_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_19_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_19_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_20_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_20_DBR_FIELD =
{
    "TX_TCONT_20_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_20_DBR",
    "DBR for T-CONT 20 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_20_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_20_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_20_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_21_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_21_DBR_FIELD =
{
    "TX_TCONT_21_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_21_DBR",
    "DBR for T-CONT 21 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_21_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_21_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_21_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_22_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_22_DBR_FIELD =
{
    "TX_TCONT_22_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_22_DBR",
    "DBR for T-CONT 22 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_22_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_22_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_22_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_23_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_23_DBR_FIELD =
{
    "TX_TCONT_23_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_23_DBR",
    "DBR for T-CONT 23 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_23_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_23_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_23_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_24_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_24_DBR_FIELD =
{
    "TX_TCONT_24_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_24_DBR",
    "DBR for T-CONT 24 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_24_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_24_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_24_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_25_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_25_DBR_FIELD =
{
    "TX_TCONT_25_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_25_DBR",
    "DBR for T-CONT 25 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_25_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_25_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_25_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_26_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_26_DBR_FIELD =
{
    "TX_TCONT_26_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_26_DBR",
    "DBR for T-CONT 26 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_26_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_26_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_26_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_27_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_27_DBR_FIELD =
{
    "TX_TCONT_27_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_27_DBR",
    "DBR for T-CONT 27 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_27_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_27_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_27_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_28_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_28_DBR_FIELD =
{
    "TX_TCONT_28_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_28_DBR",
    "DBR for T-CONT 28 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_28_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_28_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_28_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_29_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_29_DBR_FIELD =
{
    "TX_TCONT_29_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_29_DBR",
    "DBR for T-CONT 29 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_29_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_29_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_29_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_30_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_30_DBR_FIELD =
{
    "TX_TCONT_30_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_30_DBR",
    "DBR for T-CONT 30 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_30_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_30_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_30_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR0_TX_TCONT_31_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR0_TX_TCONT_31_DBR_FIELD =
{
    "TX_TCONT_31_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_31_DBR",
    "DBR for T-CONT 31 transmission  indication",
#endif
    NGPON_TX_INT_ITR0_TX_TCONT_31_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR0_TX_TCONT_31_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR0_TX_TCONT_31_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_PLM_0
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_PLM_0_FIELD =
{
    "TX_PLM_0",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_0_transmitted",
    "PLOAM was transmitted from buffer 0",
#endif
    NGPON_TX_INT_ISR1_TX_PLM_0_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_PLM_0_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_PLM_0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_PLM_1
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_PLM_1_FIELD =
{
    "TX_PLM_1",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_1_transmitted",
    "PLOAM was transmitted from buffer 1",
#endif
    NGPON_TX_INT_ISR1_TX_PLM_1_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_PLM_1_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_PLM_1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_PLM_2
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_PLM_2_FIELD =
{
    "TX_PLM_2",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_2_transmitted",
    "PLOAM was transmitted from buffer 2",
#endif
    NGPON_TX_INT_ISR1_TX_PLM_2_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_PLM_2_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_PLM_2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_ISR1_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_RESERVED0_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_FE_DATA_OVERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_FE_DATA_OVERUN_FIELD =
{
    "FE_DATA_OVERUN",
#if RU_INCLUDE_DESC
    "fe_data_overrun",
    "An overrun occurred in the data forntend buffer (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_ISR1_FE_DATA_OVERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_FE_DATA_OVERUN_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_FE_DATA_OVERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_PD_UNDERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_PD_UNDERUN_FIELD =
{
    "PD_UNDERUN",
#if RU_INCLUDE_DESC
    "pd_underrun",
    "A Packet Descriptor underrun occurred  (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_ISR1_PD_UNDERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_PD_UNDERUN_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_PD_UNDERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_PD_OVERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_PD_OVERUN_FIELD =
{
    "PD_OVERUN",
#if RU_INCLUDE_DESC
    "pd_overrun",
    "An overrun occurred in packet descriptor front end buffer (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_ISR1_PD_OVERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_PD_OVERUN_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_PD_OVERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_AF_ERR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_AF_ERR_FIELD =
{
    "AF_ERR",
#if RU_INCLUDE_DESC
    "Access_FIFO_error",
    "Access FIFO error. No consequent action is needed",
#endif
    NGPON_TX_INT_ISR1_AF_ERR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_AF_ERR_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_AF_ERR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_ROG_DIF
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_ROG_DIF_FIELD =
{
    "ROG_DIF",
#if RU_INCLUDE_DESC
    "rogue_ONU_diff",
    "Rogue ONU detected - Difference between generated TXEN and examined TXEN for longer than expected."
    "",
#endif
    NGPON_TX_INT_ISR1_ROG_DIF_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_ROG_DIF_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_ROG_DIF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_ROG_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_ROG_LEN_FIELD =
{
    "ROG_LEN",
#if RU_INCLUDE_DESC
    "rogue_ONU_length",
    "Rogue ONU detected - transmission longer than predefined time window",
#endif
    NGPON_TX_INT_ISR1_ROG_LEN_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_ROG_LEN_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_ROG_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_ISR1_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_RESERVED1_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_TCONT_32_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_TCONT_32_DBR_FIELD =
{
    "TX_TCONT_32_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_32_DBR",
    "DBR for T-CONT 32 transmission  indication",
#endif
    NGPON_TX_INT_ISR1_TX_TCONT_32_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_TCONT_32_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_TCONT_32_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_TCONT_33_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_TCONT_33_DBR_FIELD =
{
    "TX_TCONT_33_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_33_DBR",
    "DBR for T-CONT 33 transmission  indication",
#endif
    NGPON_TX_INT_ISR1_TX_TCONT_33_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_TCONT_33_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_TCONT_33_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_TCONT_34_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_TCONT_34_DBR_FIELD =
{
    "TX_TCONT_34_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_34_DBR",
    "DBR for T-CONT 34 transmission  indication",
#endif
    NGPON_TX_INT_ISR1_TX_TCONT_34_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_TCONT_34_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_TCONT_34_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_TCONT_35_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_TCONT_35_DBR_FIELD =
{
    "TX_TCONT_35_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_35_DBR",
    "DBR for T-CONT 35 transmission  indication",
#endif
    NGPON_TX_INT_ISR1_TX_TCONT_35_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_TCONT_35_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_TCONT_35_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_TCONT_36_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_TCONT_36_DBR_FIELD =
{
    "TX_TCONT_36_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_36_DBR",
    "DBR for T-CONT 36 transmission  indication",
#endif
    NGPON_TX_INT_ISR1_TX_TCONT_36_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_TCONT_36_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_TCONT_36_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_TCONT_37_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_TCONT_37_DBR_FIELD =
{
    "TX_TCONT_37_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_37_DBR",
    "DBR for T-CONT 37 transmission  indication",
#endif
    NGPON_TX_INT_ISR1_TX_TCONT_37_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_TCONT_37_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_TCONT_37_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_TCONT_38_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_TCONT_38_DBR_FIELD =
{
    "TX_TCONT_38_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_38_DBR",
    "DBR for T-CONT 38 transmission  indication",
#endif
    NGPON_TX_INT_ISR1_TX_TCONT_38_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_TCONT_38_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_TCONT_38_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_TX_TCONT_39_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_TX_TCONT_39_DBR_FIELD =
{
    "TX_TCONT_39_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_39_DBR",
    "DBR for T-CONT 39 transmission  indication",
#endif
    NGPON_TX_INT_ISR1_TX_TCONT_39_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_TX_TCONT_39_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_TX_TCONT_39_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISR1_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISR1_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_ISR1_RESERVED2_FIELD_MASK,
    0,
    NGPON_TX_INT_ISR1_RESERVED2_FIELD_WIDTH,
    NGPON_TX_INT_ISR1_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_PLM_0
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_PLM_0_FIELD =
{
    "TX_PLM_0",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_0_transmitted",
    "PLOAM was transmitted from buffer 0",
#endif
    NGPON_TX_INT_ISM1_TX_PLM_0_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_PLM_0_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_PLM_0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_PLM_1
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_PLM_1_FIELD =
{
    "TX_PLM_1",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_1_transmitted",
    "PLOAM was transmitted from buffer 1",
#endif
    NGPON_TX_INT_ISM1_TX_PLM_1_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_PLM_1_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_PLM_1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_PLM_2
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_PLM_2_FIELD =
{
    "TX_PLM_2",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_2_transmitted",
    "PLOAM was transmitted from buffer 2",
#endif
    NGPON_TX_INT_ISM1_TX_PLM_2_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_PLM_2_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_PLM_2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_ISM1_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_RESERVED0_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_FE_DATA_OVERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_FE_DATA_OVERUN_FIELD =
{
    "FE_DATA_OVERUN",
#if RU_INCLUDE_DESC
    "fe_data_overrun",
    "An overrun occurred in the data forntend buffer (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_ISM1_FE_DATA_OVERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_FE_DATA_OVERUN_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_FE_DATA_OVERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_PD_UNDERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_PD_UNDERUN_FIELD =
{
    "PD_UNDERUN",
#if RU_INCLUDE_DESC
    "pd_underrun",
    "A Packet Descriptor underrun occurred(fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_ISM1_PD_UNDERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_PD_UNDERUN_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_PD_UNDERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_PD_OVERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_PD_OVERUN_FIELD =
{
    "PD_OVERUN",
#if RU_INCLUDE_DESC
    "pd_overrun",
    "A Packet Descriptor overrun occurred. (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_ISM1_PD_OVERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_PD_OVERUN_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_PD_OVERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_AF_ERR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_AF_ERR_FIELD =
{
    "AF_ERR",
#if RU_INCLUDE_DESC
    "Access_FIFO_error",
    "Access FIFO error. No consequent action is needed",
#endif
    NGPON_TX_INT_ISM1_AF_ERR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_AF_ERR_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_AF_ERR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_ROG_DIF
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_ROG_DIF_FIELD =
{
    "ROG_DIF",
#if RU_INCLUDE_DESC
    "rogue_ONU_diff",
    "Rogue ONU detected - Difference between generated TXEN and examined TXEN for longer than expected."
    "",
#endif
    NGPON_TX_INT_ISM1_ROG_DIF_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_ROG_DIF_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_ROG_DIF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_ROG_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_ROG_LEN_FIELD =
{
    "ROG_LEN",
#if RU_INCLUDE_DESC
    "rogue_ONU_length",
    "Rogue ONU detected - transmission longer than predefined time window",
#endif
    NGPON_TX_INT_ISM1_ROG_LEN_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_ROG_LEN_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_ROG_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_ISM1_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_RESERVED1_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_TCONT_32_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_TCONT_32_DBR_FIELD =
{
    "TX_TCONT_32_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_32_DBR",
    "DBR for T-CONT 32 transmission  indication",
#endif
    NGPON_TX_INT_ISM1_TX_TCONT_32_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_TCONT_32_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_TCONT_32_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_TCONT_33_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_TCONT_33_DBR_FIELD =
{
    "TX_TCONT_33_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_33_DBR",
    "DBR for T-CONT 33 transmission  indication",
#endif
    NGPON_TX_INT_ISM1_TX_TCONT_33_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_TCONT_33_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_TCONT_33_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_TCONT_34_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_TCONT_34_DBR_FIELD =
{
    "TX_TCONT_34_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_34_DBR",
    "DBR for T-CONT 34 transmission  indication",
#endif
    NGPON_TX_INT_ISM1_TX_TCONT_34_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_TCONT_34_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_TCONT_34_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_TCONT_35_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_TCONT_35_DBR_FIELD =
{
    "TX_TCONT_35_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_35_DBR",
    "DBR for T-CONT 35 transmission  indication",
#endif
    NGPON_TX_INT_ISM1_TX_TCONT_35_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_TCONT_35_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_TCONT_35_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_TCONT_36_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_TCONT_36_DBR_FIELD =
{
    "TX_TCONT_36_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_36_DBR",
    "DBR for T-CONT 36 transmission  indication",
#endif
    NGPON_TX_INT_ISM1_TX_TCONT_36_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_TCONT_36_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_TCONT_36_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_TCONT_37_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_TCONT_37_DBR_FIELD =
{
    "TX_TCONT_37_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_36_DBR",
    "DBR for T-CONT 37 transmission  indication",
#endif
    NGPON_TX_INT_ISM1_TX_TCONT_37_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_TCONT_37_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_TCONT_37_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_TCONT_38_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_TCONT_38_DBR_FIELD =
{
    "TX_TCONT_38_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_38_DBR",
    "DBR for T-CONT 38 transmission  indication",
#endif
    NGPON_TX_INT_ISM1_TX_TCONT_38_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_TCONT_38_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_TCONT_38_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_TX_TCONT_39_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_TX_TCONT_39_DBR_FIELD =
{
    "TX_TCONT_39_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_39_DBR",
    "DBR for T-CONT 39 transmission  indication",
#endif
    NGPON_TX_INT_ISM1_TX_TCONT_39_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_TX_TCONT_39_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_TX_TCONT_39_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ISM1_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ISM1_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_ISM1_RESERVED2_FIELD_MASK,
    0,
    NGPON_TX_INT_ISM1_RESERVED2_FIELD_WIDTH,
    NGPON_TX_INT_ISM1_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_PLM_0
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_PLM_0_FIELD =
{
    "TX_PLM_0",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_0_transmitted",
    "PLOAM was transmitted from buffer 0",
#endif
    NGPON_TX_INT_IER1_TX_PLM_0_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_PLM_0_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_PLM_0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_PLM_1
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_PLM_1_FIELD =
{
    "TX_PLM_1",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_1_transmitted",
    "PLOAM was transmitted from buffer 1",
#endif
    NGPON_TX_INT_IER1_TX_PLM_1_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_PLM_1_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_PLM_1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_PLM_2
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_PLM_2_FIELD =
{
    "TX_PLM_2",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_2_transmitted",
    "PLOAM was transmitted from buffer 2",
#endif
    NGPON_TX_INT_IER1_TX_PLM_2_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_PLM_2_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_PLM_2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_IER1_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_RESERVED0_FIELD_WIDTH,
    NGPON_TX_INT_IER1_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_FE_DATA_OVERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_FE_DATA_OVERUN_FIELD =
{
    "FE_DATA_OVERUN",
#if RU_INCLUDE_DESC
    "fe_data_overrun",
    "A Data frontend buffer overrun occurred. (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_IER1_FE_DATA_OVERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_FE_DATA_OVERUN_FIELD_WIDTH,
    NGPON_TX_INT_IER1_FE_DATA_OVERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_PD_UNDERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_PD_UNDERUN_FIELD =
{
    "PD_UNDERUN",
#if RU_INCLUDE_DESC
    "pd_underrun",
    "A Packet Descriptor underrun occurred. (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_IER1_PD_UNDERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_PD_UNDERUN_FIELD_WIDTH,
    NGPON_TX_INT_IER1_PD_UNDERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_PD_OVERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_PD_OVERUN_FIELD =
{
    "PD_OVERUN",
#if RU_INCLUDE_DESC
    "pd_overrun",
    "A Packet Descriptor overrun occurred. (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_IER1_PD_OVERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_PD_OVERUN_FIELD_WIDTH,
    NGPON_TX_INT_IER1_PD_OVERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_AF_ERR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_AF_ERR_FIELD =
{
    "AF_ERR",
#if RU_INCLUDE_DESC
    "Access_FIFO_error",
    "Access FIFO error. No consequent action is needed",
#endif
    NGPON_TX_INT_IER1_AF_ERR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_AF_ERR_FIELD_WIDTH,
    NGPON_TX_INT_IER1_AF_ERR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_ROG_DIF
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_ROG_DIF_FIELD =
{
    "ROG_DIF",
#if RU_INCLUDE_DESC
    "rogue_ONU_diff",
    "Rogue ONU detected - Difference between generated TXEN and examined TXEN for longer than expected."
    "",
#endif
    NGPON_TX_INT_IER1_ROG_DIF_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_ROG_DIF_FIELD_WIDTH,
    NGPON_TX_INT_IER1_ROG_DIF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_ROG_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_ROG_LEN_FIELD =
{
    "ROG_LEN",
#if RU_INCLUDE_DESC
    "rogue_ONU_length",
    "Rogue ONU detected - transmission longer than predefined time window",
#endif
    NGPON_TX_INT_IER1_ROG_LEN_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_ROG_LEN_FIELD_WIDTH,
    NGPON_TX_INT_IER1_ROG_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_IER1_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_RESERVED1_FIELD_WIDTH,
    NGPON_TX_INT_IER1_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_TCONT_32_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_TCONT_32_DBR_FIELD =
{
    "TX_TCONT_32_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_32_DBR",
    "DBR for T-CONT 32 transmission  indication",
#endif
    NGPON_TX_INT_IER1_TX_TCONT_32_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_TCONT_32_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_TCONT_32_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_TCONT_33_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_TCONT_33_DBR_FIELD =
{
    "TX_TCONT_33_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_33_DBR",
    "DBR for T-CONT 33 transmission  indication",
#endif
    NGPON_TX_INT_IER1_TX_TCONT_33_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_TCONT_33_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_TCONT_33_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_TCONT_34_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_TCONT_34_DBR_FIELD =
{
    "TX_TCONT_34_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_34_DBR",
    "DBR for T-CONT 34 transmission  indication",
#endif
    NGPON_TX_INT_IER1_TX_TCONT_34_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_TCONT_34_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_TCONT_34_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_TCONT_35_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_TCONT_35_DBR_FIELD =
{
    "TX_TCONT_35_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_35_DBR",
    "DBR for T-CONT 35 transmission  indication",
#endif
    NGPON_TX_INT_IER1_TX_TCONT_35_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_TCONT_35_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_TCONT_35_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_TCONT_36_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_TCONT_36_DBR_FIELD =
{
    "TX_TCONT_36_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_36_DBR",
    "DBR for T-CONT 36 transmission  indication",
#endif
    NGPON_TX_INT_IER1_TX_TCONT_36_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_TCONT_36_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_TCONT_36_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_TCONT_37_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_TCONT_37_DBR_FIELD =
{
    "TX_TCONT_37_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_37_DBR",
    "DBR for T-CONT 37 transmission  indication",
#endif
    NGPON_TX_INT_IER1_TX_TCONT_37_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_TCONT_37_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_TCONT_37_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_TCONT_38_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_TCONT_38_DBR_FIELD =
{
    "TX_TCONT_38_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_38_DBR",
    "DBR for T-CONT 38 transmission  indication",
#endif
    NGPON_TX_INT_IER1_TX_TCONT_38_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_TCONT_38_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_TCONT_38_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_TX_TCONT_39_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_TX_TCONT_39_DBR_FIELD =
{
    "TX_TCONT_39_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_39_DBR",
    "DBR for T-CONT 39 transmission  indication",
#endif
    NGPON_TX_INT_IER1_TX_TCONT_39_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_TX_TCONT_39_DBR_FIELD_WIDTH,
    NGPON_TX_INT_IER1_TX_TCONT_39_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_IER1_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_IER1_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_IER1_RESERVED2_FIELD_MASK,
    0,
    NGPON_TX_INT_IER1_RESERVED2_FIELD_WIDTH,
    NGPON_TX_INT_IER1_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_PLM_0
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_PLM_0_FIELD =
{
    "TX_PLM_0",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_0_transmitted",
    "PLOAM was transmitted from buffer 0",
#endif
    NGPON_TX_INT_ITR1_TX_PLM_0_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_PLM_0_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_PLM_0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_PLM_1
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_PLM_1_FIELD =
{
    "TX_PLM_1",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_1_transmitted",
    "PLOAM was transmitted from buffer 1",
#endif
    NGPON_TX_INT_ITR1_TX_PLM_1_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_PLM_1_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_PLM_1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_PLM_2
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_PLM_2_FIELD =
{
    "TX_PLM_2",
#if RU_INCLUDE_DESC
    "PLOAM_buffer_2_transmitted",
    "PLOAM was transmitted from buffer 2",
#endif
    NGPON_TX_INT_ITR1_TX_PLM_2_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_PLM_2_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_PLM_2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_ITR1_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_RESERVED0_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_FE_DATA_OVERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_FE_DATA_OVERUN_FIELD =
{
    "FE_DATA_OVERUN",
#if RU_INCLUDE_DESC
    "fe_data_overrun",
    "A Data Frontend buffer overrun occurred. (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_ITR1_FE_DATA_OVERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_FE_DATA_OVERUN_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_FE_DATA_OVERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_PD_UNDERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_PD_UNDERUN_FIELD =
{
    "PD_UNDERUN",
#if RU_INCLUDE_DESC
    "pd_underrun",
    "A Packet Descriptor underrun occurred. (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_ITR1_PD_UNDERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_PD_UNDERUN_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_PD_UNDERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_PD_OVERUN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_PD_OVERUN_FIELD =
{
    "PD_OVERUN",
#if RU_INCLUDE_DESC
    "pd_overrun",
    "A Packet Descriptor overrun occurred. (fatal error a tx disable is required)",
#endif
    NGPON_TX_INT_ITR1_PD_OVERUN_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_PD_OVERUN_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_PD_OVERUN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_AF_ERR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_AF_ERR_FIELD =
{
    "AF_ERR",
#if RU_INCLUDE_DESC
    "Access_FIFO_error",
    "Access FIFO error. No consequent action is needed",
#endif
    NGPON_TX_INT_ITR1_AF_ERR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_AF_ERR_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_AF_ERR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_ROG_DIF
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_ROG_DIF_FIELD =
{
    "ROG_DIF",
#if RU_INCLUDE_DESC
    "rogue_ONU_diff",
    "Rogue ONU detected - Difference between generated TXEN and examined TXEN for longer than expected."
    "",
#endif
    NGPON_TX_INT_ITR1_ROG_DIF_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_ROG_DIF_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_ROG_DIF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_ROG_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_ROG_LEN_FIELD =
{
    "ROG_LEN",
#if RU_INCLUDE_DESC
    "rogue_ONU_length",
    "Rogue ONU detected - transmission longer than predefined time window",
#endif
    NGPON_TX_INT_ITR1_ROG_LEN_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_ROG_LEN_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_ROG_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_ITR1_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_RESERVED1_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_TCONT_32_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_TCONT_32_DBR_FIELD =
{
    "TX_TCONT_32_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_32_DBR",
    "DBR for T-CONT 32 transmission  indication",
#endif
    NGPON_TX_INT_ITR1_TX_TCONT_32_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_TCONT_32_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_TCONT_32_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_TCONT_33_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_TCONT_33_DBR_FIELD =
{
    "TX_TCONT_33_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_33_DBR",
    "DBR for T-CONT 33 transmission  indication",
#endif
    NGPON_TX_INT_ITR1_TX_TCONT_33_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_TCONT_33_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_TCONT_33_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_TCONT_34_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_TCONT_34_DBR_FIELD =
{
    "TX_TCONT_34_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_34_DBR",
    "DBR for T-CONT 34 transmission  indication",
#endif
    NGPON_TX_INT_ITR1_TX_TCONT_34_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_TCONT_34_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_TCONT_34_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_TCONT_35_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_TCONT_35_DBR_FIELD =
{
    "TX_TCONT_35_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_35_DBR",
    "DBR for T-CONT 35 transmission  indication",
#endif
    NGPON_TX_INT_ITR1_TX_TCONT_35_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_TCONT_35_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_TCONT_35_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_TCONT_36_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_TCONT_36_DBR_FIELD =
{
    "TX_TCONT_36_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_36_DBR",
    "DBR for T-CONT 36 transmission  indication",
#endif
    NGPON_TX_INT_ITR1_TX_TCONT_36_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_TCONT_36_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_TCONT_36_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_TCONT_37_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_TCONT_37_DBR_FIELD =
{
    "TX_TCONT_37_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_37_DBR",
    "DBR for T-CONT 37 transmission  indication",
#endif
    NGPON_TX_INT_ITR1_TX_TCONT_37_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_TCONT_37_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_TCONT_37_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_TCONT_38_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_TCONT_38_DBR_FIELD =
{
    "TX_TCONT_38_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_38_DBR",
    "DBR for T-CONT 38 transmission  indication",
#endif
    NGPON_TX_INT_ITR1_TX_TCONT_38_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_TCONT_38_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_TCONT_38_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_TX_TCONT_39_DBR
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_TX_TCONT_39_DBR_FIELD =
{
    "TX_TCONT_39_DBR",
#if RU_INCLUDE_DESC
    "transmit_TCONT_39_DBR",
    "DBR for T-CONT 39 transmission  indication",
#endif
    NGPON_TX_INT_ITR1_TX_TCONT_39_DBR_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_TX_TCONT_39_DBR_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_TX_TCONT_39_DBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_INT_ITR1_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_TX_INT_ITR1_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_INT_ITR1_RESERVED2_FIELD_MASK,
    0,
    NGPON_TX_INT_ITR1_RESERVED2_FIELD_WIDTH,
    NGPON_TX_INT_ITR1_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_TX_INT_ISR0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_INT_ISR0_FIELDS[] =
{
    &NGPON_TX_INT_ISR0_TX_TCONT_0_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_1_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_2_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_3_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_4_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_5_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_6_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_7_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_8_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_9_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_10_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_11_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_12_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_13_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_14_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_15_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_16_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_17_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_18_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_19_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_20_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_21_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_22_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_23_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_24_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_25_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_26_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_27_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_28_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_29_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_30_DBR_FIELD,
    &NGPON_TX_INT_ISR0_TX_TCONT_31_DBR_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_INT_ISR0_REG = 
{
    "ISR0",
#if RU_INCLUDE_DESC
    "INTERRUPT_STATUS_0 Register",
    "This register contains the current active transmitter interrupts. Each asserted bit represents an active interrupt source. The interrupt remains active until the software clears it by writing 1 to the corresponding bit."
    "Writing 0 to any bit has no effect.",
#endif
    NGPON_TX_INT_ISR0_REG_OFFSET,
    0,
    0,
    240,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    32,
    NGPON_TX_INT_ISR0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_INT_ISM0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_INT_ISM0_FIELDS[] =
{
    &NGPON_TX_INT_ISM0_TX_TCONT_0_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_1_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_2_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_3_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_4_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_5_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_6_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_7_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_8_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_9_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_10_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_11_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_12_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_13_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_14_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_15_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_16_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_17_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_18_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_19_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_20_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_21_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_22_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_23_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_24_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_25_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_26_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_27_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_28_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_29_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_30_DBR_FIELD,
    &NGPON_TX_INT_ISM0_TX_TCONT_31_DBR_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_INT_ISM0_REG = 
{
    "ISM0",
#if RU_INCLUDE_DESC
    "INTERRUPT_MASKED_STATUS_0 Register",
    "This register contains the current active transmitter interrupts that werent maked. Each asserted bit represents an active interrupt source. The interrupt remains active until the software clears it by writing 1 to the corresponding bit in the ISR0 register."
    "Writing 0 to any bit has no effect.",
#endif
    NGPON_TX_INT_ISM0_REG_OFFSET,
    0,
    0,
    241,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    32,
    NGPON_TX_INT_ISM0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_INT_IER0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_INT_IER0_FIELDS[] =
{
    &NGPON_TX_INT_IER0_TX_TCONT_0_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_1_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_2_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_3_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_4_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_5_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_6_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_7_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_8_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_9_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_10_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_11_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_12_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_13_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_14_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_15_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_16_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_17_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_18_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_19_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_20_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_21_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_22_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_23_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_24_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_25_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_26_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_27_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_28_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_29_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_30_DBR_FIELD,
    &NGPON_TX_INT_IER0_TX_TCONT_31_DBR_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_INT_IER0_REG = 
{
    "IER0",
#if RU_INCLUDE_DESC
    "INTERRUPT_ENABLE_0 Register",
    "Interrupts enable register, each bit masks an interrupt.",
#endif
    NGPON_TX_INT_IER0_REG_OFFSET,
    0,
    0,
    242,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    32,
    NGPON_TX_INT_IER0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_INT_ITR0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_INT_ITR0_FIELDS[] =
{
    &NGPON_TX_INT_ITR0_TX_TCONT_0_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_1_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_2_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_3_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_4_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_5_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_6_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_7_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_8_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_9_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_10_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_11_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_12_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_13_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_14_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_15_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_16_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_17_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_18_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_19_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_20_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_21_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_22_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_23_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_24_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_25_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_26_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_27_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_28_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_29_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_30_DBR_FIELD,
    &NGPON_TX_INT_ITR0_TX_TCONT_31_DBR_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_INT_ITR0_REG = 
{
    "ITR0",
#if RU_INCLUDE_DESC
    "INTERRUPT_TEST_0 Register",
    "This register enables testing by simulating interrupt sources. When the software sets a bit in the ITR, the corresponding bit in the ISR shows an active interrupt. The interrupt remains active until software clears the bit in the ITR"
    "",
#endif
    NGPON_TX_INT_ITR0_REG_OFFSET,
    0,
    0,
    243,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    32,
    NGPON_TX_INT_ITR0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_INT_ISR1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_INT_ISR1_FIELDS[] =
{
    &NGPON_TX_INT_ISR1_TX_PLM_0_FIELD,
    &NGPON_TX_INT_ISR1_TX_PLM_1_FIELD,
    &NGPON_TX_INT_ISR1_TX_PLM_2_FIELD,
    &NGPON_TX_INT_ISR1_RESERVED0_FIELD,
    &NGPON_TX_INT_ISR1_FE_DATA_OVERUN_FIELD,
    &NGPON_TX_INT_ISR1_PD_UNDERUN_FIELD,
    &NGPON_TX_INT_ISR1_PD_OVERUN_FIELD,
    &NGPON_TX_INT_ISR1_AF_ERR_FIELD,
    &NGPON_TX_INT_ISR1_ROG_DIF_FIELD,
    &NGPON_TX_INT_ISR1_ROG_LEN_FIELD,
    &NGPON_TX_INT_ISR1_RESERVED1_FIELD,
    &NGPON_TX_INT_ISR1_TX_TCONT_32_DBR_FIELD,
    &NGPON_TX_INT_ISR1_TX_TCONT_33_DBR_FIELD,
    &NGPON_TX_INT_ISR1_TX_TCONT_34_DBR_FIELD,
    &NGPON_TX_INT_ISR1_TX_TCONT_35_DBR_FIELD,
    &NGPON_TX_INT_ISR1_TX_TCONT_36_DBR_FIELD,
    &NGPON_TX_INT_ISR1_TX_TCONT_37_DBR_FIELD,
    &NGPON_TX_INT_ISR1_TX_TCONT_38_DBR_FIELD,
    &NGPON_TX_INT_ISR1_TX_TCONT_39_DBR_FIELD,
    &NGPON_TX_INT_ISR1_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_INT_ISR1_REG = 
{
    "ISR1",
#if RU_INCLUDE_DESC
    "INTERRUPT_STATUS_1 Register",
    "This register contains the current active transmitter interrupts. Each asserted bit represents an active interrupt source. The interrupt remains active until the software clears it by writing 1 to the corresponding bit."
    "Writing 0 to any bit has no effect.",
#endif
    NGPON_TX_INT_ISR1_REG_OFFSET,
    0,
    0,
    244,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    20,
    NGPON_TX_INT_ISR1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_INT_ISM1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_INT_ISM1_FIELDS[] =
{
    &NGPON_TX_INT_ISM1_TX_PLM_0_FIELD,
    &NGPON_TX_INT_ISM1_TX_PLM_1_FIELD,
    &NGPON_TX_INT_ISM1_TX_PLM_2_FIELD,
    &NGPON_TX_INT_ISM1_RESERVED0_FIELD,
    &NGPON_TX_INT_ISM1_FE_DATA_OVERUN_FIELD,
    &NGPON_TX_INT_ISM1_PD_UNDERUN_FIELD,
    &NGPON_TX_INT_ISM1_PD_OVERUN_FIELD,
    &NGPON_TX_INT_ISM1_AF_ERR_FIELD,
    &NGPON_TX_INT_ISM1_ROG_DIF_FIELD,
    &NGPON_TX_INT_ISM1_ROG_LEN_FIELD,
    &NGPON_TX_INT_ISM1_RESERVED1_FIELD,
    &NGPON_TX_INT_ISM1_TX_TCONT_32_DBR_FIELD,
    &NGPON_TX_INT_ISM1_TX_TCONT_33_DBR_FIELD,
    &NGPON_TX_INT_ISM1_TX_TCONT_34_DBR_FIELD,
    &NGPON_TX_INT_ISM1_TX_TCONT_35_DBR_FIELD,
    &NGPON_TX_INT_ISM1_TX_TCONT_36_DBR_FIELD,
    &NGPON_TX_INT_ISM1_TX_TCONT_37_DBR_FIELD,
    &NGPON_TX_INT_ISM1_TX_TCONT_38_DBR_FIELD,
    &NGPON_TX_INT_ISM1_TX_TCONT_39_DBR_FIELD,
    &NGPON_TX_INT_ISM1_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_INT_ISM1_REG = 
{
    "ISM1",
#if RU_INCLUDE_DESC
    "INTERRUPT_MASKED_STATUS_1 Register",
    "This register contains the current active transmitter interrupts that werent masked. Each asserted bit represents an active interrupt source. The interrupt remains active until the software clears it by writing 1 to the corresponding bit in the ISR1 register."
    "Writing 0 to any bit has no effect.",
#endif
    NGPON_TX_INT_ISM1_REG_OFFSET,
    0,
    0,
    245,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    20,
    NGPON_TX_INT_ISM1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_INT_IER1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_INT_IER1_FIELDS[] =
{
    &NGPON_TX_INT_IER1_TX_PLM_0_FIELD,
    &NGPON_TX_INT_IER1_TX_PLM_1_FIELD,
    &NGPON_TX_INT_IER1_TX_PLM_2_FIELD,
    &NGPON_TX_INT_IER1_RESERVED0_FIELD,
    &NGPON_TX_INT_IER1_FE_DATA_OVERUN_FIELD,
    &NGPON_TX_INT_IER1_PD_UNDERUN_FIELD,
    &NGPON_TX_INT_IER1_PD_OVERUN_FIELD,
    &NGPON_TX_INT_IER1_AF_ERR_FIELD,
    &NGPON_TX_INT_IER1_ROG_DIF_FIELD,
    &NGPON_TX_INT_IER1_ROG_LEN_FIELD,
    &NGPON_TX_INT_IER1_RESERVED1_FIELD,
    &NGPON_TX_INT_IER1_TX_TCONT_32_DBR_FIELD,
    &NGPON_TX_INT_IER1_TX_TCONT_33_DBR_FIELD,
    &NGPON_TX_INT_IER1_TX_TCONT_34_DBR_FIELD,
    &NGPON_TX_INT_IER1_TX_TCONT_35_DBR_FIELD,
    &NGPON_TX_INT_IER1_TX_TCONT_36_DBR_FIELD,
    &NGPON_TX_INT_IER1_TX_TCONT_37_DBR_FIELD,
    &NGPON_TX_INT_IER1_TX_TCONT_38_DBR_FIELD,
    &NGPON_TX_INT_IER1_TX_TCONT_39_DBR_FIELD,
    &NGPON_TX_INT_IER1_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_INT_IER1_REG = 
{
    "IER1",
#if RU_INCLUDE_DESC
    "INTERRUPT_ENABLE_1 Register",
    "Interrupts enable. Each bit can enable/ disable an interrupt",
#endif
    NGPON_TX_INT_IER1_REG_OFFSET,
    0,
    0,
    246,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    20,
    NGPON_TX_INT_IER1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_INT_ITR1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_INT_ITR1_FIELDS[] =
{
    &NGPON_TX_INT_ITR1_TX_PLM_0_FIELD,
    &NGPON_TX_INT_ITR1_TX_PLM_1_FIELD,
    &NGPON_TX_INT_ITR1_TX_PLM_2_FIELD,
    &NGPON_TX_INT_ITR1_RESERVED0_FIELD,
    &NGPON_TX_INT_ITR1_FE_DATA_OVERUN_FIELD,
    &NGPON_TX_INT_ITR1_PD_UNDERUN_FIELD,
    &NGPON_TX_INT_ITR1_PD_OVERUN_FIELD,
    &NGPON_TX_INT_ITR1_AF_ERR_FIELD,
    &NGPON_TX_INT_ITR1_ROG_DIF_FIELD,
    &NGPON_TX_INT_ITR1_ROG_LEN_FIELD,
    &NGPON_TX_INT_ITR1_RESERVED1_FIELD,
    &NGPON_TX_INT_ITR1_TX_TCONT_32_DBR_FIELD,
    &NGPON_TX_INT_ITR1_TX_TCONT_33_DBR_FIELD,
    &NGPON_TX_INT_ITR1_TX_TCONT_34_DBR_FIELD,
    &NGPON_TX_INT_ITR1_TX_TCONT_35_DBR_FIELD,
    &NGPON_TX_INT_ITR1_TX_TCONT_36_DBR_FIELD,
    &NGPON_TX_INT_ITR1_TX_TCONT_37_DBR_FIELD,
    &NGPON_TX_INT_ITR1_TX_TCONT_38_DBR_FIELD,
    &NGPON_TX_INT_ITR1_TX_TCONT_39_DBR_FIELD,
    &NGPON_TX_INT_ITR1_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_INT_ITR1_REG = 
{
    "ITR1",
#if RU_INCLUDE_DESC
    "INTERRUPT_TEST_1 Register",
    "This register enables testing by simulating interrupt sources. When the software sets a bit in the ITR, the corresponding bit in the ISR shows an active interrupt. The interrupt remains active until software clears the bit in the ITR",
#endif
    NGPON_TX_INT_ITR1_REG_OFFSET,
    0,
    0,
    247,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    20,
    NGPON_TX_INT_ITR1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_TX_INT
 ******************************************************************************/
static const ru_reg_rec *NGPON_TX_INT_REGS[] =
{
    &NGPON_TX_INT_ISR0_REG,
    &NGPON_TX_INT_ISM0_REG,
    &NGPON_TX_INT_IER0_REG,
    &NGPON_TX_INT_ITR0_REG,
    &NGPON_TX_INT_ISR1_REG,
    &NGPON_TX_INT_ISM1_REG,
    &NGPON_TX_INT_IER1_REG,
    &NGPON_TX_INT_ITR1_REG,
};

unsigned long NGPON_TX_INT_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x8016d000,
#elif defined(CONFIG_BCM96856)
    0x82dbd000,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_TX_INT_BLOCK = 
{
    "NGPON_TX_INT",
    NGPON_TX_INT_ADDRS,
    1,
    8,
    NGPON_TX_INT_REGS
};

/* End of file BCM6858_A0_NGPON_TX_INT.c */
