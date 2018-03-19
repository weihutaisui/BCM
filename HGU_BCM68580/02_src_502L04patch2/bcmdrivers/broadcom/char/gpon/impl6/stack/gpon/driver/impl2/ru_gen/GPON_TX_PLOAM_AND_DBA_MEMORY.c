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
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RPLMC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RPLMC_FIELD =
{
    "RPLMC",
#if RU_INCLUDE_DESC
    "Ranging_PLOAM_content",
    "SN/Ranging PLOAM content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RPLMC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RPLMC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RPLMC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_IPLC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_IPLC_FIELD =
{
    "IPLC",
#if RU_INCLUDE_DESC
    "Idle_PLOAM_content",
    "Idle PLOAM content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_IPLC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_IPLC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_IPLC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_UPLMC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_UPLMC_FIELD =
{
    "UPLMC",
#if RU_INCLUDE_DESC
    "Urgent_PLOAM_content",
    "Urgent PLOAM content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_UPLMC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_UPLMC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_UPLMC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_NPLMC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_NPLMC_FIELD =
{
    "NPLMC",
#if RU_INCLUDE_DESC
    "Normal_PLOAM_content",
    "Normal PLOAM content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_NPLMC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_NPLMC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_NPLMC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_NPLMC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_NPLMC_FIELD =
{
    "NPLMC",
#if RU_INCLUDE_DESC
    "Normal_PLOAM_content",
    "Normal PLAOM content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_NPLMC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_NPLMC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_NPLMC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_NPLMC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_NPLMC_FIELD =
{
    "NPLMC",
#if RU_INCLUDE_DESC
    "Normal_PLOAM_bits",
    "Normal PLOAM bits",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_NPLMC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_NPLMC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_NPLMC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_NPLMC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_NPLMC_FIELD =
{
    "NPLMC",
#if RU_INCLUDE_DESC
    "Normal_PLOAM_content",
    "Bits for normal PLOAM.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_NPLMC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_NPLMC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_NPLMC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_IDBCC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_IDBCC_FIELD =
{
    "IDBCC",
#if RU_INCLUDE_DESC
    "Idle_DBR_content",
    "Idle DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_IDBCC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_IDBCC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_IDBCC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_IDBR
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_IDBR_FIELD =
{
    "IDBR",
#if RU_INCLUDE_DESC
    "Idle_DBR",
    "last byte of idle DBR",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_IDBR_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_IDBR_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_IDBR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_NDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_NDBC_FIELD =
{
    "NDBC",
#if RU_INCLUDE_DESC
    "Normal_DBR_content",
    "Normal DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_NDBC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_NDBC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_NDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_NDBCBYTE5
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_NDBCBYTE5_FIELD =
{
    "NDBCBYTE5",
#if RU_INCLUDE_DESC
    "Normal_DBR_byte_5_content",
    "the last byte of the DBR",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_NDBCBYTE5_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_NDBCBYTE5_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_NDBCBYTE5_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_NDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_NDBC_FIELD =
{
    "NDBC",
#if RU_INCLUDE_DESC
    "Normal_DBR_content",
    "Normal DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_NDBC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_NDBC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_NDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_NDBCBYTE5
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_NDBCBYTE5_FIELD =
{
    "NDBCBYTE5",
#if RU_INCLUDE_DESC
    "Normal_DBR_byte_5_content",
    "The last byte of the DBR.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_NDBCBYTE5_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_NDBCBYTE5_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_NDBCBYTE5_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_NDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_NDBC_FIELD =
{
    "NDBC",
#if RU_INCLUDE_DESC
    "Normal_DBR_content",
    "Normal DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_NDBC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_NDBC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_NDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_NDBCBYTE5
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_NDBCBYTE5_FIELD =
{
    "NDBCBYTE5",
#if RU_INCLUDE_DESC
    "Normal_DBR_byte_5_content",
    "the last byte of the DBR",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_NDBCBYTE5_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_NDBCBYTE5_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_NDBCBYTE5_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_NDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_NDBC_FIELD =
{
    "NDBC",
#if RU_INCLUDE_DESC
    "Normal_DBR_content",
    "Normal DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_NDBC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_NDBC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_NDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_NDBCBYTE5
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_NDBCBYTE5_FIELD =
{
    "NDBCBYTE5",
#if RU_INCLUDE_DESC
    "Normal_DBR_byte_5_content",
    "the last byte of the DBR",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_NDBCBYTE5_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_NDBCBYTE5_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_NDBCBYTE5_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_NDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_NDBC_FIELD =
{
    "NDBC",
#if RU_INCLUDE_DESC
    "Normal_DBR_content",
    "Normal DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_NDBC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_NDBC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_NDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_NDBCBYTE5
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_NDBCBYTE5_FIELD =
{
    "NDBCBYTE5",
#if RU_INCLUDE_DESC
    "Normal_DBR_byte_5_content",
    "the last byte of the DBR",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_NDBCBYTE5_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_NDBCBYTE5_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_NDBCBYTE5_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_NDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_NDBC_FIELD =
{
    "NDBC",
#if RU_INCLUDE_DESC
    "Normal_DBR_content",
    "Normal DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_NDBC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_NDBC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_NDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_NDBCBYTE5
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_NDBCBYTE5_FIELD =
{
    "NDBCBYTE5",
#if RU_INCLUDE_DESC
    "Normal_DBR_byte_5_content",
    "the last byte of the DBR",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_NDBCBYTE5_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_NDBCBYTE5_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_NDBCBYTE5_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_NDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_NDBC_FIELD =
{
    "NDBC",
#if RU_INCLUDE_DESC
    "Normal_DBR_content",
    "Normal DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_NDBC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_NDBC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_NDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_NDBCBYTE5
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_NDBCBYTE5_FIELD =
{
    "NDBCBYTE5",
#if RU_INCLUDE_DESC
    "Normal_DBR_byte_5_content",
    "the last byte of the DBR",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_NDBCBYTE5_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_NDBCBYTE5_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_NDBCBYTE5_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_NDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_NDBC_FIELD =
{
    "NDBC",
#if RU_INCLUDE_DESC
    "Normal_DBR_content",
    "Normal DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_NDBC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_NDBC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_NDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_NDBCBYTE5
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_NDBCBYTE5_FIELD =
{
    "NDBCBYTE5",
#if RU_INCLUDE_DESC
    "Normal_DBR_byte_5_content",
    "the last byte of the DBR",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_NDBCBYTE5_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_NDBCBYTE5_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_NDBCBYTE5_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_NDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_NDBC_FIELD =
{
    "NDBC",
#if RU_INCLUDE_DESC
    "Normal_DBR_content",
    "Normal DBR content",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_NDBC_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_NDBC_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_NDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_RESERVED0_FIELD_WIDTH,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RPLMC_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_REG = 
{
    "RPLM",
#if RU_INCLUDE_DESC
    "RANGING_PLOAM %i Register",
    "This is an array of 13 bytes for ranging PLOAM buffer. This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_REG_OFFSET,
    GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_REG_RAM_CNT,
    4,
    43,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_IPLC_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_REG = 
{
    "IPLM",
#if RU_INCLUDE_DESC
    "IDLE_PLOAM %i Register",
    "This is an array of 13 bytes for idle PLOAM buffer. This register can be modified only when TX is disabled.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_REG_OFFSET,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_REG_RAM_CNT,
    4,
    44,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_UPLMC_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_REG = 
{
    "UPLM",
#if RU_INCLUDE_DESC
    "URGENT_PLOAM %i Register",
    "This is an array of 13 bytes for urgent PLOAM buffer. This register array can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_REG_OFFSET,
    GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_REG_RAM_CNT,
    4,
    45,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_NPLMC_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_REG = 
{
    "NPLM3",
#if RU_INCLUDE_DESC
    "NORMAL_PLOAM3 %i Register",
    "This is an array of 13 bytes for normal PLOAM3 buffer. This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_REG_OFFSET,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_REG_RAM_CNT,
    4,
    46,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_NPLMC_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_REG = 
{
    "NPLM2",
#if RU_INCLUDE_DESC
    "NORMAL_PLOAM2 %i Register",
    "This is an array of 13 bytes for normal PLOAM2 buffer. This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_REG_OFFSET,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_REG_RAM_CNT,
    4,
    47,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_NPLMC_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_REG = 
{
    "NPLM1",
#if RU_INCLUDE_DESC
    "NORMAL_PLOAM1 %i Register",
    "This is an array of 13 bytes for normal PLOAM1 buffer. This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_REG_OFFSET,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_REG_RAM_CNT,
    4,
    48,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_NPLMC_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_REG = 
{
    "NPLM0",
#if RU_INCLUDE_DESC
    "NORMAL_PLOAM0 %i Register",
    "This is an array of 13 bytes for normal PLOAM0 buffer. Only 13 bytes are used. The transmitter will transmit this buffer if",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_REG_OFFSET,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_REG_RAM_CNT,
    4,
    49,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_IDBCC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_REG = 
{
    "IDBR",
#if RU_INCLUDE_DESC
    "IDLE_DBR Register",
    "This is a register which contains the 4 first bytes details the idle DBR buffer. modified only when TX is disabled.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_REG_OFFSET,
    0,
    0,
    50,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_IDBR_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_REG = 
{
    "IDBRBYTE5",
#if RU_INCLUDE_DESC
    "IDLE_DBR_LAST_BYTE Register",
    "The last byte of the Idle DBR",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_REG_OFFSET,
    0,
    0,
    51,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_NDBC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_REG = 
{
    "NDBR0",
#if RU_INCLUDE_DESC
    "NORMAL_DBR0 Register",
    "4 first bytes for DBR0.This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_REG_OFFSET,
    0,
    0,
    52,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_NDBCBYTE5_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_REG = 
{
    "NDBR0BYTE5",
#if RU_INCLUDE_DESC
    "NORMAL_DBR0 Register",
    "The last byte of DBR0",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_REG_OFFSET,
    0,
    0,
    53,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_NDBC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_REG = 
{
    "NDBR1",
#if RU_INCLUDE_DESC
    "NORMAL_DBR1 Register",
    "4 first bytes for DBR1.This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_REG_OFFSET,
    0,
    0,
    54,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_NDBCBYTE5_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_REG = 
{
    "NDBR1BYTE5",
#if RU_INCLUDE_DESC
    "NORMAL_DBR1 Register",
    "The last byte of DBR1",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_REG_OFFSET,
    0,
    0,
    55,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_NDBC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_REG = 
{
    "NDBR2",
#if RU_INCLUDE_DESC
    "NORMAL_DBR2 Register",
    "4 first bytes for DBR2.This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_REG_OFFSET,
    0,
    0,
    56,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_NDBCBYTE5_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_REG = 
{
    "NDBR2BYTE5",
#if RU_INCLUDE_DESC
    "NORMAL_DBR2 Register",
    "The last byte of DBR2",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_REG_OFFSET,
    0,
    0,
    57,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_NDBC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_REG = 
{
    "NDBR3",
#if RU_INCLUDE_DESC
    "NORMAL_DBR3 Register",
    "4 first bytes for DBR3.This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_REG_OFFSET,
    0,
    0,
    58,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_NDBCBYTE5_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_REG = 
{
    "NDBR3BYTE5",
#if RU_INCLUDE_DESC
    "NORMAL_DBR3 Register",
    "The last byte of DBR3",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_REG_OFFSET,
    0,
    0,
    59,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_NDBC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_REG = 
{
    "NDBR4",
#if RU_INCLUDE_DESC
    "NORMAL_DBR4 Register",
    "4 first bytes for DBR4.This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_REG_OFFSET,
    0,
    0,
    60,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_NDBCBYTE5_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_REG = 
{
    "NDBR4BYTE5",
#if RU_INCLUDE_DESC
    "NORMAL_DBR4 Register",
    "The last byte of DBR4",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_REG_OFFSET,
    0,
    0,
    61,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_NDBC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_REG = 
{
    "NDBR5",
#if RU_INCLUDE_DESC
    "NORMAL_DBR5 Register",
    "4 first bytes for DBR5.This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_REG_OFFSET,
    0,
    0,
    62,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_NDBCBYTE5_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_REG = 
{
    "NDBR5BYTE5",
#if RU_INCLUDE_DESC
    "NORMAL_DBR5 Register",
    "The last byte of DBR5",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_REG_OFFSET,
    0,
    0,
    63,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_NDBC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_REG = 
{
    "NDBR6",
#if RU_INCLUDE_DESC
    "NORMAL_DBR6 Register",
    "4 first bytes for DBR6.This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_REG_OFFSET,
    0,
    0,
    64,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_NDBCBYTE5_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_REG = 
{
    "NDBR6BYTE5",
#if RU_INCLUDE_DESC
    "NORMAL_DBR6 Register",
    "The last byte of DBR6",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_REG_OFFSET,
    0,
    0,
    65,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_NDBC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_REG = 
{
    "NDBR7",
#if RU_INCLUDE_DESC
    "NORMAL_DBR7 Register",
    "4 first bytes for DBR7.This register can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_REG_OFFSET,
    0,
    0,
    66,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_NDBCBYTE5_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_REG = 
{
    "NDBR7BYTE5",
#if RU_INCLUDE_DESC
    "NORMAL_DBR7 Register",
    "The last byte of DBR7",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_REG_OFFSET,
    0,
    0,
    67,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_FIELDS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_NDBC_FIELD,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_REG = 
{
    "NDBR8TO39",
#if RU_INCLUDE_DESC
    "NORMAL_DBR_REGISTERS_FOR_TCONTS_8_TO_39 %i Register",
    "3 bytes DBR registers for DBR 8 to 39 These registers can be modified only when the corresponding valid bit is inactive.",
#endif
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_REG_OFFSET,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_REG_RAM_CNT,
    4,
    68,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_TX_PLOAM_AND_DBA_MEMORY
 ******************************************************************************/
static const ru_reg_rec *GPON_TX_PLOAM_AND_DBA_MEMORY_REGS[] =
{
    &GPON_TX_PLOAM_AND_DBA_MEMORY_RPLM_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_IPLM_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_UPLM_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM3_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM2_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM1_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NPLM0_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_IDBR_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_IDBRBYTE5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR0BYTE5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR1BYTE5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR2BYTE5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR3BYTE5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR4BYTE5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR5BYTE5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR6BYTE5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR7BYTE5_REG,
    &GPON_TX_PLOAM_AND_DBA_MEMORY_NDBR8TO39_REG,
};

unsigned long GPON_TX_PLOAM_AND_DBA_MEMORY_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130fa200,
#else
    0x80148200,
#endif
};

const ru_block_rec GPON_TX_PLOAM_AND_DBA_MEMORY_BLOCK = 
{
    "GPON_TX_PLOAM_AND_DBA_MEMORY",
    GPON_TX_PLOAM_AND_DBA_MEMORY_ADDRS,
    1,
    26,
    GPON_TX_PLOAM_AND_DBA_MEMORY_REGS
};

/* End of file BCM6858_A0GPON_TX_PLOAM_AND_DBA_MEMORY.c */
