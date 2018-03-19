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
 * Field: NGPON_RX_PM_SYNC_LOF_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_SYNC_LOF_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_SYNC_LOF_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_SYNC_LOF_CNT_FIELD_WIDTH,
    NGPON_RX_PM_SYNC_LOF_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_SYNC_LOF_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_SYNC_LOF_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_SYNC_LOF_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_SYNC_LOF_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_SYNC_LOF_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_HEC_ERR_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_HEC_ERR_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_BWMAP_HEC_ERR_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_HEC_ERR_CNT_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_HEC_ERR_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_HEC_ERR_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_HEC_ERR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_BWMAP_HEC_ERR_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_HEC_ERR_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_HEC_ERR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_CNT_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_HLEND_HEC_ERR_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_HLEND_HEC_ERR_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_HLEND_HEC_ERR_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_HLEND_HEC_ERR_CNT_FIELD_WIDTH,
    NGPON_RX_PM_HLEND_HEC_ERR_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_HLEND_HEC_ERR_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_HLEND_HEC_ERR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_HLEND_HEC_ERR_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_HLEND_HEC_ERR_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_HLEND_HEC_ERR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_DEL_LCDG_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_DEL_LCDG_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_DEL_LCDG_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_DEL_LCDG_CNT_FIELD_WIDTH,
    NGPON_RX_PM_DEL_LCDG_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_SYNC_PONID_HEC_ERR_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_SYNC_PONID_HEC_ERR_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_SYNC_PONID_HEC_ERR_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_SYNC_PONID_HEC_ERR_CNT_FIELD_WIDTH,
    NGPON_RX_PM_SYNC_PONID_HEC_ERR_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_SYNC_PONID_HEC_ERR_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_SYNC_PONID_HEC_ERR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_SYNC_PONID_HEC_ERR_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_SYNC_PONID_HEC_ERR_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_SYNC_PONID_HEC_ERR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_DEL_PASS_PKT_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_DEL_PASS_PKT_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_DEL_PASS_PKT_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_DEL_PASS_PKT_CNT_FIELD_WIDTH,
    NGPON_RX_PM_DEL_PASS_PKT_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_CORRECT_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_CORRECT_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_BWMAP_CORRECT_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_CORRECT_CNT_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_CORRECT_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_BURSTS_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_BURSTS_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_BWMAP_BURSTS_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_BURSTS_CNT_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_BURSTS_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_BURSTS_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_BURSTS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_BWMAP_BURSTS_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_BURSTS_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_BURSTS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_XHP_PASS_PKT_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_XHP_PASS_PKT_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_XHP_PASS_PKT_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_XHP_PASS_PKT_CNT_FIELD_WIDTH,
    NGPON_RX_PM_XHP_PASS_PKT_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_DEC_CANT_ALLOCATE_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_DEC_CANT_ALLOCATE_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_DEC_CANT_ALLOCATE_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_DEC_CANT_ALLOCATE_CNT_FIELD_WIDTH,
    NGPON_RX_PM_DEC_CANT_ALLOCATE_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_DEC_CANT_ALLOCATE_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_DEC_CANT_ALLOCATE_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_DEC_CANT_ALLOCATE_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_DEC_CANT_ALLOCATE_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_DEC_CANT_ALLOCATE_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_DEC_INVALID_KEY_IDX_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_DEC_INVALID_KEY_IDX_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_DEC_INVALID_KEY_IDX_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_DEC_INVALID_KEY_IDX_CNT_FIELD_WIDTH,
    NGPON_RX_PM_DEC_INVALID_KEY_IDX_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_DEC_INVALID_KEY_IDX_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_DEC_INVALID_KEY_IDX_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_DEC_INVALID_KEY_IDX_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_DEC_INVALID_KEY_IDX_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_DEC_INVALID_KEY_IDX_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_PLP_VALID_ONUID_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_PLP_VALID_ONUID_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_PLP_VALID_ONUID_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_PLP_VALID_ONUID_CNT_FIELD_WIDTH,
    NGPON_RX_PM_PLP_VALID_ONUID_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_PLP_VALID_ONUID_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_PLP_VALID_ONUID_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_PLP_VALID_ONUID_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_PLP_VALID_ONUID_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_PLP_VALID_ONUID_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_PLP_VALID_BCST_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_PLP_VALID_BCST_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_PLP_VALID_BCST_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_PLP_VALID_BCST_CNT_FIELD_WIDTH,
    NGPON_RX_PM_PLP_VALID_BCST_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_PLP_VALID_BCST_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_PLP_VALID_BCST_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_PLP_VALID_BCST_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_PLP_VALID_BCST_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_PLP_VALID_BCST_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_PLP_MIC_ERR_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_PLP_MIC_ERR_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_PLP_MIC_ERR_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_PLP_MIC_ERR_CNT_FIELD_WIDTH,
    NGPON_RX_PM_PLP_MIC_ERR_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_PLP_MIC_ERR_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_PLP_MIC_ERR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_PLP_MIC_ERR_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_PLP_MIC_ERR_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_PLP_MIC_ERR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_HEC_FIX_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_HEC_FIX_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_BWMAP_HEC_FIX_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_HEC_FIX_CNT_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_HEC_FIX_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_HEC_FIX_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_HEC_FIX_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_BWMAP_HEC_FIX_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_HEC_FIX_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_HEC_FIX_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_HLEND_HEC_FIX_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_HLEND_HEC_FIX_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_HLEND_HEC_FIX_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_HLEND_HEC_FIX_CNT_FIELD_WIDTH,
    NGPON_RX_PM_HLEND_HEC_FIX_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_HLEND_HEC_FIX_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_HLEND_HEC_FIX_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_HLEND_HEC_FIX_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_HLEND_HEC_FIX_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_HLEND_HEC_FIX_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_XHP_HEC_FIX_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_XHP_HEC_FIX_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_XHP_HEC_FIX_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_XHP_HEC_FIX_CNT_FIELD_WIDTH,
    NGPON_RX_PM_XHP_HEC_FIX_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_XHP_HEC_FIX_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_XHP_HEC_FIX_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_XHP_HEC_FIX_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_XHP_HEC_FIX_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_XHP_HEC_FIX_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_SYNC_SFC_HEC_FIX_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_SYNC_SFC_HEC_FIX_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_SYNC_SFC_HEC_FIX_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_SYNC_SFC_HEC_FIX_CNT_FIELD_WIDTH,
    NGPON_RX_PM_SYNC_SFC_HEC_FIX_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_SYNC_SFC_HEC_FIX_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_SYNC_SFC_HEC_FIX_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_SYNC_SFC_HEC_FIX_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_SYNC_SFC_HEC_FIX_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_SYNC_SFC_HEC_FIX_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_SYNC_PONID_HEC_FIX_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_SYNC_PONID_HEC_FIX_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_SYNC_PONID_HEC_FIX_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_SYNC_PONID_HEC_FIX_CNT_FIELD_WIDTH,
    NGPON_RX_PM_SYNC_PONID_HEC_FIX_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_SYNC_PONID_HEC_FIX_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_SYNC_PONID_HEC_FIX_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_SYNC_PONID_HEC_FIX_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_SYNC_PONID_HEC_FIX_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_SYNC_PONID_HEC_FIX_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_XGEM_OVERRUN_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_XGEM_OVERRUN_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_XGEM_OVERRUN_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_XGEM_OVERRUN_CNT_FIELD_WIDTH,
    NGPON_RX_PM_XGEM_OVERRUN_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_XGEM_OVERRUN_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_XGEM_OVERRUN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_XGEM_OVERRUN_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_XGEM_OVERRUN_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_XGEM_OVERRUN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_CNT_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_FEC_BIT_ERR_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_FEC_BIT_ERR_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_FEC_BIT_ERR_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_FEC_BIT_ERR_CNT_FIELD_WIDTH,
    NGPON_RX_PM_FEC_BIT_ERR_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_FEC_SYM_ERR_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_FEC_SYM_ERR_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_FEC_SYM_ERR_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_FEC_SYM_ERR_CNT_FIELD_WIDTH,
    NGPON_RX_PM_FEC_SYM_ERR_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_FEC_CW_ERR_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_FEC_CW_ERR_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_FEC_CW_ERR_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_FEC_CW_ERR_CNT_FIELD_WIDTH,
    NGPON_RX_PM_FEC_CW_ERR_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_FEC_UC_CW_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_FEC_UC_CW_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_FEC_UC_CW_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_FEC_UC_CW_CNT_FIELD_WIDTH,
    NGPON_RX_PM_FEC_UC_CW_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_CNT_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_BWMAP_TOT_BW_TCONT_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_BWMAP_TOT_BW_TCONT_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_BWMAP_TOT_BW_TCONT_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_BWMAP_TOT_BW_TCONT_CNT_FIELD_WIDTH,
    NGPON_RX_PM_BWMAP_TOT_BW_TCONT_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_FS_BIP_ERR_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_FS_BIP_ERR_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_FS_BIP_ERR_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_FS_BIP_ERR_CNT_FIELD_WIDTH,
    NGPON_RX_PM_FS_BIP_ERR_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_FS_BIP_ERR_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_FS_BIP_ERR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PM_FS_BIP_ERR_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PM_FS_BIP_ERR_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PM_FS_BIP_ERR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_XHP_XGEM_PER_FLOW_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_XHP_XGEM_PER_FLOW_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_XHP_XGEM_PER_FLOW_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_XHP_XGEM_PER_FLOW_CNT_FIELD_WIDTH,
    NGPON_RX_PM_XHP_XGEM_PER_FLOW_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PM_XHP_BYTES_PER_FLOW_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PM_XHP_BYTES_PER_FLOW_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PM_XHP_BYTES_PER_FLOW_CNT_FIELD_MASK,
    0,
    NGPON_RX_PM_XHP_BYTES_PER_FLOW_CNT_FIELD_WIDTH,
    NGPON_RX_PM_XHP_BYTES_PER_FLOW_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_PM_SYNC_LOF
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_SYNC_LOF_FIELDS[] =
{
    &NGPON_RX_PM_SYNC_LOF_CNT_FIELD,
    &NGPON_RX_PM_SYNC_LOF_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_SYNC_LOF_REG = 
{
    "SYNC_LOF",
#if RU_INCLUDE_DESC
    "SYNC_LOF Register",
    "Number of LOFs",
#endif
    NGPON_RX_PM_SYNC_LOF_REG_OFFSET,
    0,
    0,
    100,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_SYNC_LOF_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_BWMAP_HEC_ERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_BWMAP_HEC_ERR_FIELDS[] =
{
    &NGPON_RX_PM_BWMAP_HEC_ERR_CNT_FIELD,
    &NGPON_RX_PM_BWMAP_HEC_ERR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_BWMAP_HEC_ERR_REG = 
{
    "BWMAP_HEC_ERR",
#if RU_INCLUDE_DESC
    "BWMAP_HEC_ERR Register",
    "Number of US BWMAP allocations discarded due to HEC errors",
#endif
    NGPON_RX_PM_BWMAP_HEC_ERR_REG_OFFSET,
    0,
    0,
    101,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_BWMAP_HEC_ERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_FIELDS[] =
{
    &NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_CNT_FIELD,
    &NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_REG = 
{
    "BWMAP_INVALID_BURST_SERIES",
#if RU_INCLUDE_DESC
    "BWMAP_INVALID_BURST_SERIES Register",
    "Discarded US BWMAP allocations due to invalid burst construction (after T-CONT filtering) - caused by burst continue (StartTime=FFFF) when the previous BWMAP did not pass filtering (was not directed to us)",
#endif
    NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_REG_OFFSET,
    0,
    0,
    102,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_HLEND_HEC_ERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_HLEND_HEC_ERR_FIELDS[] =
{
    &NGPON_RX_PM_HLEND_HEC_ERR_CNT_FIELD,
    &NGPON_RX_PM_HLEND_HEC_ERR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_HLEND_HEC_ERR_REG = 
{
    "HLEND_HEC_ERR",
#if RU_INCLUDE_DESC
    "HLEND_HEC_ERR Register",
    "Number of dropped frames due to HEC errors in HLend field",
#endif
    NGPON_RX_PM_HLEND_HEC_ERR_REG_OFFSET,
    0,
    0,
    103,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_HLEND_HEC_ERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_DEL_LCDG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_DEL_LCDG_FIELDS[] =
{
    &NGPON_RX_PM_DEL_LCDG_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_DEL_LCDG_REG = 
{
    "DEL_LCDG",
#if RU_INCLUDE_DESC
    "DEL_LCDG Register",
    "Number of LCDGs",
#endif
    NGPON_RX_PM_DEL_LCDG_REG_OFFSET,
    0,
    0,
    104,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_DEL_LCDG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_SYNC_PONID_HEC_ERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_SYNC_PONID_HEC_ERR_FIELDS[] =
{
    &NGPON_RX_PM_SYNC_PONID_HEC_ERR_CNT_FIELD,
    &NGPON_RX_PM_SYNC_PONID_HEC_ERR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_SYNC_PONID_HEC_ERR_REG = 
{
    "SYNC_PONID_HEC_ERR",
#if RU_INCLUDE_DESC
    "SYNC_PONID_HEC_ERR Register",
    "Number of ignored PON-ID fields due to HEC errors (sync mode only)",
#endif
    NGPON_RX_PM_SYNC_PONID_HEC_ERR_REG_OFFSET,
    0,
    0,
    105,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_SYNC_PONID_HEC_ERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_DEL_PASS_PKT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_DEL_PASS_PKT_FIELDS[] =
{
    &NGPON_RX_PM_DEL_PASS_PKT_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_DEL_PASS_PKT_REG = 
{
    "DEL_PASS_PKT",
#if RU_INCLUDE_DESC
    "DEL_PASS_PKT Register",
    "Number of XGEM frames with non zero PLI before Port-ID filtering",
#endif
    NGPON_RX_PM_DEL_PASS_PKT_REG_OFFSET,
    0,
    0,
    106,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_DEL_PASS_PKT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_BWMAP_CORRECT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_BWMAP_CORRECT_FIELDS[] =
{
    &NGPON_RX_PM_BWMAP_CORRECT_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_BWMAP_CORRECT_REG = 
{
    "BWMAP_CORRECT",
#if RU_INCLUDE_DESC
    "BWMAP_CORRECT Register",
    "Number of correct US BWMAP allocations (before filtering)",
#endif
    NGPON_RX_PM_BWMAP_CORRECT_REG_OFFSET,
    0,
    0,
    107,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_BWMAP_CORRECT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_BWMAP_BURSTS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_BWMAP_BURSTS_FIELDS[] =
{
    &NGPON_RX_PM_BWMAP_BURSTS_CNT_FIELD,
    &NGPON_RX_PM_BWMAP_BURSTS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_BWMAP_BURSTS_REG = 
{
    "BWMAP_BURSTS",
#if RU_INCLUDE_DESC
    "BWMAP_BURSTS Register",
    "Number of US BWMAP burst series (after filtering; StartTime != 0xFFFF). These bursts will be passed to ONU-TX if it is enabled.",
#endif
    NGPON_RX_PM_BWMAP_BURSTS_REG_OFFSET,
    0,
    0,
    108,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_BWMAP_BURSTS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_XHP_PASS_PKT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_XHP_PASS_PKT_FIELDS[] =
{
    &NGPON_RX_PM_XHP_PASS_PKT_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_XHP_PASS_PKT_REG = 
{
    "XHP_PASS_PKT",
#if RU_INCLUDE_DESC
    "XHP_PASS_PKT Register",
    "Number of XGEM frames which passed the Port-ID filtering",
#endif
    NGPON_RX_PM_XHP_PASS_PKT_REG_OFFSET,
    0,
    0,
    109,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_XHP_PASS_PKT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_DEC_CANT_ALLOCATE
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_DEC_CANT_ALLOCATE_FIELDS[] =
{
    &NGPON_RX_PM_DEC_CANT_ALLOCATE_CNT_FIELD,
    &NGPON_RX_PM_DEC_CANT_ALLOCATE_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_DEC_CANT_ALLOCATE_REG = 
{
    "DEC_CANT_ALLOCATE",
#if RU_INCLUDE_DESC
    "DEC_CANT_ALLOCATE Register",
    "Number of times the cipher mechanism couldnt allocate a decryption machine",
#endif
    NGPON_RX_PM_DEC_CANT_ALLOCATE_REG_OFFSET,
    0,
    0,
    110,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_DEC_CANT_ALLOCATE_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_DEC_INVALID_KEY_IDX
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_DEC_INVALID_KEY_IDX_FIELDS[] =
{
    &NGPON_RX_PM_DEC_INVALID_KEY_IDX_CNT_FIELD,
    &NGPON_RX_PM_DEC_INVALID_KEY_IDX_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_DEC_INVALID_KEY_IDX_REG = 
{
    "DEC_INVALID_KEY_IDX",
#if RU_INCLUDE_DESC
    "DEC_INVALID_KEY_IDX Register",
    "Number of XGEM frames with a non-valid decryption key index",
#endif
    NGPON_RX_PM_DEC_INVALID_KEY_IDX_REG_OFFSET,
    0,
    0,
    111,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_DEC_INVALID_KEY_IDX_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_PLP_VALID_ONUID
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_PLP_VALID_ONUID_FIELDS[] =
{
    &NGPON_RX_PM_PLP_VALID_ONUID_CNT_FIELD,
    &NGPON_RX_PM_PLP_VALID_ONUID_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_PLP_VALID_ONUID_REG = 
{
    "PLP_VALID_ONUID",
#if RU_INCLUDE_DESC
    "PLP_VALID_ONUID %i Register",
    "Number of valid received PLOAM messages that passed the ONU ID filter (per filter)",
#endif
    NGPON_RX_PM_PLP_VALID_ONUID_REG_OFFSET,
    NGPON_RX_PM_PLP_VALID_ONUID_REG_RAM_CNT,
    4,
    112,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_PLP_VALID_ONUID_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_PLP_VALID_BCST
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_PLP_VALID_BCST_FIELDS[] =
{
    &NGPON_RX_PM_PLP_VALID_BCST_CNT_FIELD,
    &NGPON_RX_PM_PLP_VALID_BCST_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_PLP_VALID_BCST_REG = 
{
    "PLP_VALID_BCST",
#if RU_INCLUDE_DESC
    "PLP_VALID_BCST Register",
    "Number of valid received PLOAM messages that passed the BROADCAST ONU ID filter",
#endif
    NGPON_RX_PM_PLP_VALID_BCST_REG_OFFSET,
    0,
    0,
    113,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_PLP_VALID_BCST_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_PLP_MIC_ERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_PLP_MIC_ERR_FIELDS[] =
{
    &NGPON_RX_PM_PLP_MIC_ERR_CNT_FIELD,
    &NGPON_RX_PM_PLP_MIC_ERR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_PLP_MIC_ERR_REG = 
{
    "PLP_MIC_ERR",
#if RU_INCLUDE_DESC
    "PLP_MIC_ERR Register",
    "Number of PLOAM messages with MIC errors (after filter)",
#endif
    NGPON_RX_PM_PLP_MIC_ERR_REG_OFFSET,
    0,
    0,
    114,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_PLP_MIC_ERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_BWMAP_HEC_FIX
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_BWMAP_HEC_FIX_FIELDS[] =
{
    &NGPON_RX_PM_BWMAP_HEC_FIX_CNT_FIELD,
    &NGPON_RX_PM_BWMAP_HEC_FIX_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_BWMAP_HEC_FIX_REG = 
{
    "BWMAP_HEC_FIX",
#if RU_INCLUDE_DESC
    "BWMAP_HEC_FIX Register",
    "Number of corrected HEC errors in the US BWMAP field",
#endif
    NGPON_RX_PM_BWMAP_HEC_FIX_REG_OFFSET,
    0,
    0,
    115,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_BWMAP_HEC_FIX_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_HLEND_HEC_FIX
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_HLEND_HEC_FIX_FIELDS[] =
{
    &NGPON_RX_PM_HLEND_HEC_FIX_CNT_FIELD,
    &NGPON_RX_PM_HLEND_HEC_FIX_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_HLEND_HEC_FIX_REG = 
{
    "HLEND_HEC_FIX",
#if RU_INCLUDE_DESC
    "HLEND_HEC_FIX Register",
    "Number of corrected HEC errors in the HLend field",
#endif
    NGPON_RX_PM_HLEND_HEC_FIX_REG_OFFSET,
    0,
    0,
    116,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_HLEND_HEC_FIX_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_XHP_HEC_FIX
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_XHP_HEC_FIX_FIELDS[] =
{
    &NGPON_RX_PM_XHP_HEC_FIX_CNT_FIELD,
    &NGPON_RX_PM_XHP_HEC_FIX_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_XHP_HEC_FIX_REG = 
{
    "XHP_HEC_FIX",
#if RU_INCLUDE_DESC
    "XHP_HEC_FIX Register",
    "Number of corrected HEC errors in the XGEM header",
#endif
    NGPON_RX_PM_XHP_HEC_FIX_REG_OFFSET,
    0,
    0,
    117,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_XHP_HEC_FIX_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_SYNC_SFC_HEC_FIX
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_SYNC_SFC_HEC_FIX_FIELDS[] =
{
    &NGPON_RX_PM_SYNC_SFC_HEC_FIX_CNT_FIELD,
    &NGPON_RX_PM_SYNC_SFC_HEC_FIX_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_SYNC_SFC_HEC_FIX_REG = 
{
    "SYNC_SFC_HEC_FIX",
#if RU_INCLUDE_DESC
    "SYNC_SFC_HEC_FIX Register",
    "Number of corrected HEC errors in the superframe field",
#endif
    NGPON_RX_PM_SYNC_SFC_HEC_FIX_REG_OFFSET,
    0,
    0,
    118,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_SYNC_SFC_HEC_FIX_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_SYNC_PONID_HEC_FIX
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_SYNC_PONID_HEC_FIX_FIELDS[] =
{
    &NGPON_RX_PM_SYNC_PONID_HEC_FIX_CNT_FIELD,
    &NGPON_RX_PM_SYNC_PONID_HEC_FIX_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_SYNC_PONID_HEC_FIX_REG = 
{
    "SYNC_PONID_HEC_FIX",
#if RU_INCLUDE_DESC
    "SYNC_PONID_HEC_FIX Register",
    "Number of corrected HEC errors in the PON-ID field",
#endif
    NGPON_RX_PM_SYNC_PONID_HEC_FIX_REG_OFFSET,
    0,
    0,
    119,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_SYNC_PONID_HEC_FIX_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_XGEM_OVERRUN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_XGEM_OVERRUN_FIELDS[] =
{
    &NGPON_RX_PM_XGEM_OVERRUN_CNT_FIELD,
    &NGPON_RX_PM_XGEM_OVERRUN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_XGEM_OVERRUN_REG = 
{
    "XGEM_OVERRUN",
#if RU_INCLUDE_DESC
    "XGEM_OVERRUN Register",
    "Number of XGEM overruns between XGPON frames (decoded PLI exceeds XGTC partition)",
#endif
    NGPON_RX_PM_XGEM_OVERRUN_REG_OFFSET,
    0,
    0,
    120,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_XGEM_OVERRUN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_BWMAP_DISCARD_DIS_TX
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_FIELDS[] =
{
    &NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_CNT_FIELD,
    &NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_REG = 
{
    "BWMAP_DISCARD_DIS_TX",
#if RU_INCLUDE_DESC
    "BWMAP_DISCARD_DIS_TX Register",
    "Number of US BWMAP bursts discarded because the transmitter is disabled (after filtering)",
#endif
    NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_REG_OFFSET,
    0,
    0,
    121,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_FEC_BIT_ERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_FEC_BIT_ERR_FIELDS[] =
{
    &NGPON_RX_PM_FEC_BIT_ERR_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_FEC_BIT_ERR_REG = 
{
    "FEC_BIT_ERR",
#if RU_INCLUDE_DESC
    "FEC_BIT_ERR Register",
    "Number of corrected FEC bits",
#endif
    NGPON_RX_PM_FEC_BIT_ERR_REG_OFFSET,
    0,
    0,
    122,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_FEC_BIT_ERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_FEC_SYM_ERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_FEC_SYM_ERR_FIELDS[] =
{
    &NGPON_RX_PM_FEC_SYM_ERR_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_FEC_SYM_ERR_REG = 
{
    "FEC_SYM_ERR",
#if RU_INCLUDE_DESC
    "FEC_SYM_ERR Register",
    "Number of corrected FEC symbols (bytes)",
#endif
    NGPON_RX_PM_FEC_SYM_ERR_REG_OFFSET,
    0,
    0,
    123,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_FEC_SYM_ERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_FEC_CW_ERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_FEC_CW_ERR_FIELDS[] =
{
    &NGPON_RX_PM_FEC_CW_ERR_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_FEC_CW_ERR_REG = 
{
    "FEC_CW_ERR",
#if RU_INCLUDE_DESC
    "FEC_CW_ERR Register",
    "Number of corrected FEC code words",
#endif
    NGPON_RX_PM_FEC_CW_ERR_REG_OFFSET,
    0,
    0,
    124,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_FEC_CW_ERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_FEC_UC_CW
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_FEC_UC_CW_FIELDS[] =
{
    &NGPON_RX_PM_FEC_UC_CW_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_FEC_UC_CW_REG = 
{
    "FEC_UC_CW",
#if RU_INCLUDE_DESC
    "FEC_UC_CW Register",
    "Number of uncorrectable FEC code words",
#endif
    NGPON_RX_PM_FEC_UC_CW_REG_OFFSET,
    0,
    0,
    125,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_FEC_UC_CW_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_FIELDS[] =
{
    &NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_CNT_FIELD,
    &NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_REG = 
{
    "BWMAP_NUM_ALLOC_TCONT",
#if RU_INCLUDE_DESC
    "BWMAP_NUM_ALLOC_TCONT %i Register",
    "Number of valid allocations received per T-CONT. The first eight counters are for TCONTs 0-7 and the last four are configurable in bwmap filters sections. Burst series are counted as the number of discrete allocations.",
#endif
    NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_REG_OFFSET,
    NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_REG_RAM_CNT,
    4,
    126,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_BWMAP_TOT_BW_TCONT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_BWMAP_TOT_BW_TCONT_FIELDS[] =
{
    &NGPON_RX_PM_BWMAP_TOT_BW_TCONT_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_BWMAP_TOT_BW_TCONT_REG = 
{
    "BWMAP_TOT_BW_TCONT",
#if RU_INCLUDE_DESC
    "BWMAP_TOT_BW_TCONT %i Register",
    "Total bandwidth allocated per T-CONT (in units of 4 bytes); calculated by summing grant sizes of valid allocations. The first eight counters are for TCONTs 0-7 and the last four are configurable in bwmap filters sections.",
#endif
    NGPON_RX_PM_BWMAP_TOT_BW_TCONT_REG_OFFSET,
    NGPON_RX_PM_BWMAP_TOT_BW_TCONT_REG_RAM_CNT,
    4,
    127,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_BWMAP_TOT_BW_TCONT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_FS_BIP_ERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_FS_BIP_ERR_FIELDS[] =
{
    &NGPON_RX_PM_FS_BIP_ERR_CNT_FIELD,
    &NGPON_RX_PM_FS_BIP_ERR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_FS_BIP_ERR_REG = 
{
    "FS_BIP_ERR",
#if RU_INCLUDE_DESC
    "FS_BIP_ERR Register",
    "Number of BIP errors in downstream FS frames (valid in NGPON2 mode only)",
#endif
    NGPON_RX_PM_FS_BIP_ERR_REG_OFFSET,
    0,
    0,
    128,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PM_FS_BIP_ERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_XHP_XGEM_PER_FLOW
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_XHP_XGEM_PER_FLOW_FIELDS[] =
{
    &NGPON_RX_PM_XHP_XGEM_PER_FLOW_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_XHP_XGEM_PER_FLOW_REG = 
{
    "XHP_XGEM_PER_FLOW",
#if RU_INCLUDE_DESC
    "XHP_XGEM_PER_FLOW %i Register",
    "Number of XGEM frames per flow",
#endif
    NGPON_RX_PM_XHP_XGEM_PER_FLOW_REG_OFFSET,
    NGPON_RX_PM_XHP_XGEM_PER_FLOW_REG_RAM_CNT,
    4,
    129,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_XHP_XGEM_PER_FLOW_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PM_XHP_BYTES_PER_FLOW
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PM_XHP_BYTES_PER_FLOW_FIELDS[] =
{
    &NGPON_RX_PM_XHP_BYTES_PER_FLOW_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PM_XHP_BYTES_PER_FLOW_REG = 
{
    "XHP_BYTES_PER_FLOW",
#if RU_INCLUDE_DESC
    "XHP_BYTES_PER_FLOW %i Register",
    "Number of bytes received per flow",
#endif
    NGPON_RX_PM_XHP_BYTES_PER_FLOW_REG_OFFSET,
    NGPON_RX_PM_XHP_BYTES_PER_FLOW_REG_RAM_CNT,
    4,
    130,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PM_XHP_BYTES_PER_FLOW_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_PM
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_PM_REGS[] =
{
    &NGPON_RX_PM_SYNC_LOF_REG,
    &NGPON_RX_PM_BWMAP_HEC_ERR_REG,
    &NGPON_RX_PM_BWMAP_INVALID_BURST_SERIES_REG,
    &NGPON_RX_PM_HLEND_HEC_ERR_REG,
    &NGPON_RX_PM_DEL_LCDG_REG,
    &NGPON_RX_PM_SYNC_PONID_HEC_ERR_REG,
    &NGPON_RX_PM_DEL_PASS_PKT_REG,
    &NGPON_RX_PM_BWMAP_CORRECT_REG,
    &NGPON_RX_PM_BWMAP_BURSTS_REG,
    &NGPON_RX_PM_XHP_PASS_PKT_REG,
    &NGPON_RX_PM_DEC_CANT_ALLOCATE_REG,
    &NGPON_RX_PM_DEC_INVALID_KEY_IDX_REG,
    &NGPON_RX_PM_PLP_VALID_ONUID_REG,
    &NGPON_RX_PM_PLP_VALID_BCST_REG,
    &NGPON_RX_PM_PLP_MIC_ERR_REG,
    &NGPON_RX_PM_BWMAP_HEC_FIX_REG,
    &NGPON_RX_PM_HLEND_HEC_FIX_REG,
    &NGPON_RX_PM_XHP_HEC_FIX_REG,
    &NGPON_RX_PM_SYNC_SFC_HEC_FIX_REG,
    &NGPON_RX_PM_SYNC_PONID_HEC_FIX_REG,
    &NGPON_RX_PM_XGEM_OVERRUN_REG,
    &NGPON_RX_PM_BWMAP_DISCARD_DIS_TX_REG,
    &NGPON_RX_PM_FEC_BIT_ERR_REG,
    &NGPON_RX_PM_FEC_SYM_ERR_REG,
    &NGPON_RX_PM_FEC_CW_ERR_REG,
    &NGPON_RX_PM_FEC_UC_CW_REG,
    &NGPON_RX_PM_BWMAP_NUM_ALLOC_TCONT_REG,
    &NGPON_RX_PM_BWMAP_TOT_BW_TCONT_REG,
    &NGPON_RX_PM_FS_BIP_ERR_REG,
    &NGPON_RX_PM_XHP_XGEM_PER_FLOW_REG,
    &NGPON_RX_PM_XHP_BYTES_PER_FLOW_REG,
};

unsigned long NGPON_RX_PM_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80161000,
#elif defined(CONFIG_BCM96856)
    0x82db5000,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_PM_BLOCK = 
{
    "NGPON_RX_PM",
    NGPON_RX_PM_ADDRS,
    1,
    31,
    NGPON_RX_PM_REGS
};

/* End of file BCM6858_A0_NGPON_RX_PM.c */
