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
 * Field: NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_SYNC
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_SYNC_FIELD =
{
    "PSYNC_TOL_SYNC",
#if RU_INCLUDE_DESC
    "sync_psync_tol_sync",
    "PSync tolerance in sync state",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_SYNC_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_SYNC_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_SYNC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_OTHER
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_OTHER_FIELD =
{
    "PSYNC_TOL_OTHER",
#if RU_INCLUDE_DESC
    "sync_psync_tol_other",
    "PSync tolerance in states other than sync",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_OTHER_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_OTHER_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_OTHER_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_SYNC_CFG_SFC_MSB_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_SYNC_CFG_SFC_MSB_MASK_FIELD =
{
    "SFC_MSB_MASK",
#if RU_INCLUDE_DESC
    "sync_sfc_msb_mask",
    "Mask MSB of superframe counter",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_SFC_MSB_MASK_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_SYNC_CFG_SFC_MSB_MASK_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_SYNC_CFG_SFC_MSB_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_DISABLE_HUNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_DISABLE_HUNT_FIELD =
{
    "XGD_DISABLE_HUNT",
#if RU_INCLUDE_DESC
    "xgd_disable_hunt",
    "Disable hunt mode in XGEM delineator - if cell delineation is lost, whole XGTC frame is discarded",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_DISABLE_HUNT_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_DISABLE_HUNT_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_DISABLE_HUNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_STRICT_IDLE_PLI
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_STRICT_IDLE_PLI_FIELD =
{
    "XGD_STRICT_IDLE_PLI",
#if RU_INCLUDE_DESC
    "xgd_strict_idle_pli",
    "Strict handling of idles: if PLI %% 4 != 0, XGEM frame is considered invalid",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_STRICT_IDLE_PLI_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_STRICT_IDLE_PLI_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_STRICT_IDLE_PLI_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_SYNC_CFG_SYNC_PSYNC_SRC
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_SYNC_CFG_SYNC_PSYNC_SRC_FIELD =
{
    "SYNC_PSYNC_SRC",
#if RU_INCLUDE_DESC
    "sync_psync_src",
    "Source of PSYNC indication",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_SYNC_PSYNC_SRC_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_SYNC_CFG_SYNC_PSYNC_SRC_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_SYNC_CFG_SYNC_PSYNC_SRC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_FEC_FORCE_STATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_FEC_FORCE_STATE_FIELD =
{
    "STATE",
#if RU_INCLUDE_DESC
    "fec_force_state",
    "FEC force. Effective on pre-sync state and when the receiver is disabled.",
#endif
    NGPON_RX_PROPRIETARY_FEC_FORCE_STATE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_FEC_FORCE_STATE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_FEC_FORCE_STATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_FEC_FORCE_TYPE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_FEC_FORCE_TYPE_FIELD =
{
    "TYPE",
#if RU_INCLUDE_DESC
    "fec_force_type",
    "Force a specific FEC type (applies only if fec_force_state is FORCE_ON)",
#endif
    NGPON_RX_PROPRIETARY_FEC_FORCE_TYPE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_FEC_FORCE_TYPE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_FEC_FORCE_TYPE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_FEC_FORCE_IGNORE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_FEC_FORCE_IGNORE_FIELD =
{
    "IGNORE",
#if RU_INCLUDE_DESC
    "fec_ignore",
    "Ignore parity bytes",
#endif
    NGPON_RX_PROPRIETARY_FEC_FORCE_IGNORE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_FEC_FORCE_IGNORE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_FEC_FORCE_IGNORE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_FEC_FORCE_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_FEC_FORCE_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_FEC_FORCE_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_FEC_FORCE_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_FEC_FORCE_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_ACCESS_ENABLE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_ACCESS_ENABLE_FIELD =
{
    "ENABLE",
#if RU_INCLUDE_DESC
    "prop_access_enable",
    "Enable processing of the proprietary access",
#endif
    NGPON_RX_PROPRIETARY_ACCESS_ENABLE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_ACCESS_ENABLE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_ACCESS_ENABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_ACCESS_RECORD
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_ACCESS_RECORD_FIELD =
{
    "RECORD",
#if RU_INCLUDE_DESC
    "prop_access_record",
    "Enable recording (and counting) of the proprietary access",
#endif
    NGPON_RX_PROPRIETARY_ACCESS_RECORD_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_ACCESS_RECORD_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_ACCESS_RECORD_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_ACCESS_FORCE_BIP
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_ACCESS_FORCE_BIP_FIELD =
{
    "FORCE_BIP",
#if RU_INCLUDE_DESC
    "prop_access_force_bip",
    "Force BIP processing (normally its processed only when FEC is off)",
#endif
    NGPON_RX_PROPRIETARY_ACCESS_FORCE_BIP_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_ACCESS_FORCE_BIP_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_ACCESS_FORCE_BIP_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_ACCESS_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_ACCESS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_ACCESS_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_ACCESS_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_ACCESS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_ACCESS_ALLOCID
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_ACCESS_ALLOCID_FIELD =
{
    "ALLOCID",
#if RU_INCLUDE_DESC
    "prop_access_allocid",
    "Alloc-ID of the proprietary access",
#endif
    NGPON_RX_PROPRIETARY_ACCESS_ALLOCID_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_ACCESS_ALLOCID_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_ACCESS_ALLOCID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_ACCESS_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_ACCESS_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_ACCESS_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_ACCESS_RESERVED1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_ACCESS_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_DBG_VECT_SELECT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_DBG_VECT_SELECT_FIELD =
{
    "SELECT",
#if RU_INCLUDE_DESC
    "debug_vector_sel",
    "Debug vector select",
#endif
    NGPON_RX_PROPRIETARY_DBG_VECT_SELECT_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_DBG_VECT_SELECT_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_DBG_VECT_SELECT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_DBG_VECT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_DBG_VECT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_DBG_VECT_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_DBG_VECT_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_DBG_VECT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_TYPE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_TYPE_FIELD =
{
    "FEC_TYPE",
#if RU_INCLUDE_DESC
    "dbg_fec_type",
    "FEC type detected",
#endif
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_TYPE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_TYPE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_TYPE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_DBG_GENSTAT_SYNC_STATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_DBG_GENSTAT_SYNC_STATE_FIELD =
{
    "SYNC_STATE",
#if RU_INCLUDE_DESC
    "dbg_sync_state",
    "Frame synchronizer state",
#endif
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_SYNC_STATE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_SYNC_STATE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_SYNC_STATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_DBG_GENSTAT_PIPE_EMPTY
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_DBG_GENSTAT_PIPE_EMPTY_FIELD =
{
    "PIPE_EMPTY",
#if RU_INCLUDE_DESC
    "dbg_pipe_empty",
    "Data pipeline empty indication",
#endif
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_PIPE_EMPTY_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_PIPE_EMPTY_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_PIPE_EMPTY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_ON_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_ON_CNT_FIELD =
{
    "FEC_IND_ON_CNT",
#if RU_INCLUDE_DESC
    "fec_ind_on_cnt",
    "FEC indication on count",
#endif
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_ON_CNT_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_ON_CNT_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_ON_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_OFF_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_OFF_CNT_FIELD =
{
    "FEC_IND_OFF_CNT",
#if RU_INCLUDE_DESC
    "fec_ind_off_cnt",
    "FEC indication off count",
#endif
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_OFF_CNT_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_OFF_CNT_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_OFF_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_PLP_CFG_RELAXED_PLOAMS_PER_FRAME
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_PLP_CFG_RELAXED_PLOAMS_PER_FRAME_FIELD =
{
    "RELAXED_PLOAMS_PER_FRAME",
#if RU_INCLUDE_DESC
    "relaxed_ploams_per_frame",
    "Disable forcing of a single PLOAM per ONU/broadcast per frame.",
#endif
    NGPON_RX_PROPRIETARY_PLP_CFG_RELAXED_PLOAMS_PER_FRAME_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_PLP_CFG_RELAXED_PLOAMS_PER_FRAME_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_PLP_CFG_RELAXED_PLOAMS_PER_FRAME_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_PLP_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_PLP_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_PLP_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_PLP_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_PLP_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_PM_BIP_ERR_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_PM_BIP_ERR_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PROPRIETARY_PM_BIP_ERR_CNT_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_PM_BIP_ERR_CNT_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_PM_BIP_ERR_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_XGEM_ENC_STYLE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_XGEM_ENC_STYLE_FIELD =
{
    "XGEM_ENC_STYLE",
#if RU_INCLUDE_DESC
    "xgem_enc_style",
    "XGEM AES counter style",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_XGEM_ENC_STYLE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_XGEM_ENC_STYLE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_XGEM_ENC_STYLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_SYNC
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_SYNC_FIELD =
{
    "DISABLE_HECFIX_SYNC",
#if RU_INCLUDE_DESC
    "disable_hecfix_sync",
    "Disable HEC correction for SFC, PON-ID and prop access",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_SYNC_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_SYNC_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_SYNC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGTCH
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGTCH_FIELD =
{
    "DISABLE_HECFIX_XGTCH",
#if RU_INCLUDE_DESC
    "disable_hecfix_xgtch",
    "Disable HEC correction for XGTC header fields - HLend and BWMAPs",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGTCH_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGTCH_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGTCH_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGEMH
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGEMH_FIELD =
{
    "DISABLE_HECFIX_XGEMH",
#if RU_INCLUDE_DESC
    "disable_hecfix_xgemh",
    "Disable HEC correction for XGEM headers",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGEMH_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGEMH_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGEMH_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_SYNC
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_SYNC_FIELD =
{
    "DISABLE_HECDET_SYNC",
#if RU_INCLUDE_DESC
    "disable_hecdet_sync",
    "Disable HEC detection for SFC, PON-ID and prop access (all structures are considered valid)",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_SYNC_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_SYNC_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_SYNC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGTCH
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGTCH_FIELD =
{
    "DISABLE_HECDET_XGTCH",
#if RU_INCLUDE_DESC
    "disable_hecdet_xgtch",
    "Disable HEC detection for XGTC header fields - HLend and BWMAPs (all structures are considered valid)",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGTCH_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGTCH_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGTCH_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGEMH
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGEMH_FIELD =
{
    "DISABLE_HECDET_XGEMH",
#if RU_INCLUDE_DESC
    "disable_hecdet_xgemh",
    "Disable HEC detection for XGEM headers (all structures are considered valid)",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGEMH_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGEMH_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGEMH_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_XGEM_DEL_RESYNC_PROC
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_XGEM_DEL_RESYNC_PROC_FIELD =
{
    "XGEM_DEL_RESYNC_PROC",
#if RU_INCLUDE_DESC
    "xgem_del_resync_proc",
    "First XGEM to process after sync re-acquire",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_XGEM_DEL_RESYNC_PROC_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_XGEM_DEL_RESYNC_PROC_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_XGEM_DEL_RESYNC_PROC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_DATA_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_DATA_MASK_FIELD =
{
    "BWMAP_DATA_MASK",
#if RU_INCLUDE_DESC
    "bwmap_data_mask",
    "Force reserved bits in BWMAP record to zero at TX interface (only applies in 14 bit mode)",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_DATA_MASK_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_DATA_MASK_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_DATA_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_14BIT_MODE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_14BIT_MODE_FIELD =
{
    "BWMAP_14BIT_MODE",
#if RU_INCLUDE_DESC
    "bwmap_14bit_mode",
    "Use 14 bit mode for StartTime and GrantSize",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_14BIT_MODE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_14BIT_MODE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_14BIT_MODE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_FORCE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_FORCE_FIELD =
{
    "HLEND_BIP_FORCE",
#if RU_INCLUDE_DESC
    "hlend_bip_force",
    "Force BIP mode",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_FORCE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_FORCE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_FORCE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_STATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_STATE_FIELD =
{
    "HLEND_BIP_STATE",
#if RU_INCLUDE_DESC
    "hlend_bip_state",
    "BIP state (applicable when hlend_bip_force is set)",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_STATE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_STATE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_STATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_SERIES_FWI_IGNORE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_SERIES_FWI_IGNORE_FIELD =
{
    "BWMAP_SERIES_FWI_IGNORE",
#if RU_INCLUDE_DESC
    "bwmap_series_fwi_ignore",
    "Ignore FWI bit on burst series",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_SERIES_FWI_IGNORE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_SERIES_FWI_IGNORE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_SERIES_FWI_IGNORE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_CHICKEN_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_CHICKEN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_CHICKEN_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_CHICKEN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_PONID_MASK_MSB_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_PONID_MASK_MSB_MASK_FIELD =
{
    "MASK",
#if RU_INCLUDE_DESC
    "mask",
    "Mask register",
#endif
    NGPON_RX_PROPRIETARY_PONID_MASK_MSB_MASK_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_PONID_MASK_MSB_MASK_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_PONID_MASK_MSB_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_PONID_MASK_MSB_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_PONID_MASK_MSB_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_PONID_MASK_MSB_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_PONID_MASK_MSB_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_PONID_MASK_MSB_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_PONID_MASK_LSB_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_PONID_MASK_LSB_MASK_FIELD =
{
    "MASK",
#if RU_INCLUDE_DESC
    "mask",
    "Mask register",
#endif
    NGPON_RX_PROPRIETARY_PONID_MASK_LSB_MASK_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_PONID_MASK_LSB_MASK_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_PONID_MASK_LSB_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_IDLE_PORTID_IDLE_PORTID
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_IDLE_PORTID_IDLE_PORTID_FIELD =
{
    "IDLE_PORTID",
#if RU_INCLUDE_DESC
    "idle_portid",
    "Idle Port-ID value",
#endif
    NGPON_RX_PROPRIETARY_IDLE_PORTID_IDLE_PORTID_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_IDLE_PORTID_IDLE_PORTID_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_IDLE_PORTID_IDLE_PORTID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_IDLE_PORTID_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_IDLE_PORTID_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_IDLE_PORTID_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_IDLE_PORTID_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_IDLE_PORTID_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS1_FIELD =
{
    "FEC_BIT_POS1",
#if RU_INCLUDE_DESC
    "FEC_BIT_POS1",
    "Bit position in OC structure (without HEC) which indicates FEC on/off status"
    ""
    "Valid values: 0-50",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL1_FIELD =
{
    "FEC_ON_VAL1",
#if RU_INCLUDE_DESC
    "FEC_ON_VAL1",
    "Bit value which indicates FEC on",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN1_FIELD =
{
    "FEC_COMP_EN1",
#if RU_INCLUDE_DESC
    "FEC_COMP_EN1",
    "Enable comparator (AND relationship)",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS2_FIELD =
{
    "FEC_BIT_POS2",
#if RU_INCLUDE_DESC
    "FEC_BIT_POS2",
    "Bit position in OC structure (without HEC) which indicates FEC on/off status"
    ""
    "Valid values: 0-50",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL2_FIELD =
{
    "FEC_ON_VAL2",
#if RU_INCLUDE_DESC
    "FEC_ON_VAL2",
    "Bit value which indicates FEC on",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN2_FIELD =
{
    "FEC_COMP_EN2",
#if RU_INCLUDE_DESC
    "FEC_COMP_EN2",
    "Enable comparator (AND relationship)",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FECTYPE_STABILITY_THR
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FECTYPE_STABILITY_THR_FIELD =
{
    "FECTYPE_STABILITY_THR",
#if RU_INCLUDE_DESC
    "FECTYPE_STABILITY_THR",
    "FEC type stability threshold"
    "(0 = feature disabled, acquisition is only done before sync)",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FECTYPE_STABILITY_THR_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FECTYPE_STABILITY_THR_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FECTYPE_STABILITY_THR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_OC_SOURCE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_OC_SOURCE_FIELD =
{
    "OC_SOURCE",
#if RU_INCLUDE_DESC
    "OC_SOURCE",
    "OC structure source for comparison",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_OC_SOURCE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_OC_SOURCE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_OC_SOURCE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_ALLOW_OTF_CHANGE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_ALLOW_OTF_CHANGE_FIELD =
{
    "ALLOW_OTF_CHANGE",
#if RU_INCLUDE_DESC
    "ALLOW_OTF_CHANGE",
    "Allow FEC type changes while data is processed (for FEC stability mode only; normally not enabled)",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_ALLOW_OTF_CHANGE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_ALLOW_OTF_CHANGE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_ALLOW_OTF_CHANGE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL1_FIELD =
{
    "FEC_OFF_VAL1",
#if RU_INCLUDE_DESC
    "FEC_OFF_VAL1",
    "Bit value which indicates FEC off",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL2_FIELD =
{
    "FEC_OFF_VAL2",
#if RU_INCLUDE_DESC
    "FEC_OFF_VAL2",
    "Bit value which indicates FEC off",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_STARTTIME
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_STARTTIME_FIELD =
{
    "STARTTIME",
#if RU_INCLUDE_DESC
    "StartTime",
    "StartTime",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_STARTTIME_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_STARTTIME_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_STARTTIME_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_GRANTSIZE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_GRANTSIZE_FIELD =
{
    "GRANTSIZE",
#if RU_INCLUDE_DESC
    "GrantSize",
    "GrantSize",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_GRANTSIZE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_GRANTSIZE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_GRANTSIZE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_PROC_ENABLE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_PROC_ENABLE_FIELD =
{
    "PROC_ENABLE",
#if RU_INCLUDE_DESC
    "proc_enable",
    "Enable processing of AMCC grant",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_PROC_ENABLE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_PROC_ENABLE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_PROC_ENABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_AMCC_SERIES_COND
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_AMCC_SERIES_COND_FIELD =
{
    "AMCC_SERIES_COND",
#if RU_INCLUDE_DESC
    "amcc_series_cond",
    "Add AMCC grant as a negative condition for burst series",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_AMCC_SERIES_COND_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_AMCC_SERIES_COND_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_AMCC_SERIES_COND_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FIRST_ONLY
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FIRST_ONLY_FIELD =
{
    "FIRST_ONLY",
#if RU_INCLUDE_DESC
    "first_only",
    "AMCC grant is only handled when it is the first allocation in a bandwidth map",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FIRST_ONLY_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FIRST_ONLY_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FIRST_ONLY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FLAGS
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FLAGS_FIELD =
{
    "FLAGS",
#if RU_INCLUDE_DESC
    "flags",
    "AMCC flags (DBRu, PLOAMu) value",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FLAGS_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FLAGS_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FLAGS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_COMP_FLAGS
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_COMP_FLAGS_FIELD =
{
    "COMP_FLAGS",
#if RU_INCLUDE_DESC
    "comp_flags",
    "Compare flags (DBRu, PLOAMu) for AMCC access (otherwise, these bits are ignored)",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_COMP_FLAGS_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_COMP_FLAGS_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_COMP_FLAGS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_ALLOCID
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_ALLOCID_FIELD =
{
    "ALLOCID",
#if RU_INCLUDE_DESC
    "AllocID",
    "AllocID",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_ALLOCID_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_ALLOCID_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_ALLOCID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_K_TABLE1_D1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_K_TABLE1_D1_FIELD =
{
    "D1",
#if RU_INCLUDE_DESC
    "D1",
    "Data value",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE1_D1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_K_TABLE1_D1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_K_TABLE1_D1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_K_TABLE1_K1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_K_TABLE1_K1_FIELD =
{
    "K1",
#if RU_INCLUDE_DESC
    "K1",
    "K value",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE1_K1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_K_TABLE1_K1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_K_TABLE1_K1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_K_TABLE1_D2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_K_TABLE1_D2_FIELD =
{
    "D2",
#if RU_INCLUDE_DESC
    "D2",
    "Data value",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE1_D2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_K_TABLE1_D2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_K_TABLE1_D2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_K_TABLE1_K2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_K_TABLE1_K2_FIELD =
{
    "K2",
#if RU_INCLUDE_DESC
    "K2",
    "K value",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE1_K2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_K_TABLE1_K2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_K_TABLE1_K2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_K_TABLE2_D1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_K_TABLE2_D1_FIELD =
{
    "D1",
#if RU_INCLUDE_DESC
    "D1",
    "Data value",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE2_D1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_K_TABLE2_D1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_K_TABLE2_D1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_K_TABLE2_K1
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_K_TABLE2_K1_FIELD =
{
    "K1",
#if RU_INCLUDE_DESC
    "K1",
    "K value",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE2_K1_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_K_TABLE2_K1_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_K_TABLE2_K1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_K_TABLE2_D2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_K_TABLE2_D2_FIELD =
{
    "D2",
#if RU_INCLUDE_DESC
    "D2",
    "Data value",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE2_D2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_K_TABLE2_D2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_K_TABLE2_D2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_K_TABLE2_K2
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_K_TABLE2_K2_FIELD =
{
    "K2",
#if RU_INCLUDE_DESC
    "K2",
    "K value",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE2_K2_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_K_TABLE2_K2_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_K_TABLE2_K2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_UNEXPECTED_K_CNT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_UNEXPECTED_K_CNT_FIELD =
{
    "CNT",
#if RU_INCLUDE_DESC
    "cnt",
    "Count",
#endif
    NGPON_RX_PROPRIETARY_UNEXPECTED_K_CNT_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_UNEXPECTED_K_CNT_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_UNEXPECTED_K_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_UNEXPECTED_K_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_UNEXPECTED_K_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_UNEXPECTED_K_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_UNEXPECTED_K_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_UNEXPECTED_K_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_KEY_SELECT
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_KEY_SELECT_FIELD =
{
    "KEY_SELECT",
#if RU_INCLUDE_DESC
    "key_select",
    "0 = use ONU specific key for directed messages and broadcast key for broadcast messages"
    "1 = always use broadcast key",
#endif
    NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_KEY_SELECT_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_KEY_SELECT_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_KEY_SELECT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Value",
#endif
    NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_VALUE_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_RESERVED0_FIELD_WIDTH,
    NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_SYNC_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_SYNC_CFG_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_SYNC_FIELD,
    &NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED0_FIELD,
    &NGPON_RX_PROPRIETARY_SYNC_CFG_PSYNC_TOL_OTHER_FIELD,
    &NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED1_FIELD,
    &NGPON_RX_PROPRIETARY_SYNC_CFG_SFC_MSB_MASK_FIELD,
    &NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_DISABLE_HUNT_FIELD,
    &NGPON_RX_PROPRIETARY_SYNC_CFG_XGD_STRICT_IDLE_PLI_FIELD,
    &NGPON_RX_PROPRIETARY_SYNC_CFG_SYNC_PSYNC_SRC_FIELD,
    &NGPON_RX_PROPRIETARY_SYNC_CFG_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_SYNC_CFG_REG = 
{
    "SYNC_CFG",
#if RU_INCLUDE_DESC
    "SYNC_ADDITIONAL_PARAMS Register",
    "Additional parameters for the frame synchronizer and XGEM delineator",
#endif
    NGPON_RX_PROPRIETARY_SYNC_CFG_REG_OFFSET,
    0,
    0,
    85,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    9,
    NGPON_RX_PROPRIETARY_SYNC_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_FEC_FORCE
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_FEC_FORCE_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_FEC_FORCE_STATE_FIELD,
    &NGPON_RX_PROPRIETARY_FEC_FORCE_TYPE_FIELD,
    &NGPON_RX_PROPRIETARY_FEC_FORCE_IGNORE_FIELD,
    &NGPON_RX_PROPRIETARY_FEC_FORCE_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_FEC_FORCE_REG = 
{
    "FEC_FORCE",
#if RU_INCLUDE_DESC
    "FEC_FORCE Register",
    "Force FEC parameters. Can only be changed when the module is disabled.",
#endif
    NGPON_RX_PROPRIETARY_FEC_FORCE_REG_OFFSET,
    0,
    0,
    86,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_RX_PROPRIETARY_FEC_FORCE_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_ACCESS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_ACCESS_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_ACCESS_ENABLE_FIELD,
    &NGPON_RX_PROPRIETARY_ACCESS_RECORD_FIELD,
    &NGPON_RX_PROPRIETARY_ACCESS_FORCE_BIP_FIELD,
    &NGPON_RX_PROPRIETARY_ACCESS_RESERVED0_FIELD,
    &NGPON_RX_PROPRIETARY_ACCESS_ALLOCID_FIELD,
    &NGPON_RX_PROPRIETARY_ACCESS_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_ACCESS_REG = 
{
    "ACCESS",
#if RU_INCLUDE_DESC
    "PROPRIETARY_ACCESS_PARAMS Register",
    "Parameters of the proprietary access",
#endif
    NGPON_RX_PROPRIETARY_ACCESS_REG_OFFSET,
    0,
    0,
    87,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    6,
    NGPON_RX_PROPRIETARY_ACCESS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_DBG_VECT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_DBG_VECT_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_DBG_VECT_SELECT_FIELD,
    &NGPON_RX_PROPRIETARY_DBG_VECT_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_DBG_VECT_REG = 
{
    "DBG_VECT",
#if RU_INCLUDE_DESC
    "DEBUG Register",
    "Debug vector select",
#endif
    NGPON_RX_PROPRIETARY_DBG_VECT_REG_OFFSET,
    0,
    0,
    88,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PROPRIETARY_DBG_VECT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_DBG_GENSTAT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_DBG_GENSTAT_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_TYPE_FIELD,
    &NGPON_RX_PROPRIETARY_DBG_GENSTAT_SYNC_STATE_FIELD,
    &NGPON_RX_PROPRIETARY_DBG_GENSTAT_PIPE_EMPTY_FIELD,
    &NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED0_FIELD,
    &NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_ON_CNT_FIELD,
    &NGPON_RX_PROPRIETARY_DBG_GENSTAT_FEC_IND_OFF_CNT_FIELD,
    &NGPON_RX_PROPRIETARY_DBG_GENSTAT_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_DBG_GENSTAT_REG = 
{
    "DBG_GENSTAT",
#if RU_INCLUDE_DESC
    "GENERAL_STATUS Register",
    "General status for debug",
#endif
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_REG_OFFSET,
    0,
    0,
    89,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    7,
    NGPON_RX_PROPRIETARY_DBG_GENSTAT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_PLP_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_PLP_CFG_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_PLP_CFG_RELAXED_PLOAMS_PER_FRAME_FIELD,
    &NGPON_RX_PROPRIETARY_PLP_CFG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_PLP_CFG_REG = 
{
    "PLP_CFG",
#if RU_INCLUDE_DESC
    "PLP_CFG Register",
    "Additional configuration for PLOAM processor",
#endif
    NGPON_RX_PROPRIETARY_PLP_CFG_REG_OFFSET,
    0,
    0,
    90,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PROPRIETARY_PLP_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_PM_BIP_ERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_PM_BIP_ERR_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_PM_BIP_ERR_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_PM_BIP_ERR_REG = 
{
    "PM_BIP_ERR",
#if RU_INCLUDE_DESC
    "PM_BIP_ERR Register",
    "Number of BIP Errors (counted only when the module is synchronized on the incoming data stream)",
#endif
    NGPON_RX_PROPRIETARY_PM_BIP_ERR_REG_OFFSET,
    0,
    0,
    91,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PROPRIETARY_PM_BIP_ERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_CHICKEN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_CHICKEN_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_CHICKEN_XGEM_ENC_STYLE_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_SYNC_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGTCH_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECFIX_XGEMH_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_SYNC_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGTCH_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_DISABLE_HECDET_XGEMH_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_XGEM_DEL_RESYNC_PROC_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_DATA_MASK_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_14BIT_MODE_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_FORCE_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_HLEND_BIP_STATE_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_BWMAP_SERIES_FWI_IGNORE_FIELD,
    &NGPON_RX_PROPRIETARY_CHICKEN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_CHICKEN_REG = 
{
    "CHICKEN",
#if RU_INCLUDE_DESC
    "CHICKEN Register",
    "Debug options to toggle optional behaviours",
#endif
    NGPON_RX_PROPRIETARY_CHICKEN_REG_OFFSET,
    0,
    0,
    92,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    14,
    NGPON_RX_PROPRIETARY_CHICKEN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_PONID_MASK_MSB
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_PONID_MASK_MSB_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_PONID_MASK_MSB_MASK_FIELD,
    &NGPON_RX_PROPRIETARY_PONID_MASK_MSB_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_PONID_MASK_MSB_REG = 
{
    "PONID_MASK_MSB",
#if RU_INCLUDE_DESC
    "PONID_MASK_MSB Register",
    "Mask for PON-ID consistency detection mechanism (MSB)",
#endif
    NGPON_RX_PROPRIETARY_PONID_MASK_MSB_REG_OFFSET,
    0,
    0,
    93,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PROPRIETARY_PONID_MASK_MSB_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_PONID_MASK_LSB
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_PONID_MASK_LSB_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_PONID_MASK_LSB_MASK_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_PONID_MASK_LSB_REG = 
{
    "PONID_MASK_LSB",
#if RU_INCLUDE_DESC
    "PONID_MASK_LSB Register",
    "Mask for PON-ID consistency detection mechanism (LSB)",
#endif
    NGPON_RX_PROPRIETARY_PONID_MASK_LSB_REG_OFFSET,
    0,
    0,
    94,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PROPRIETARY_PONID_MASK_LSB_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_IDLE_PORTID
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_IDLE_PORTID_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_IDLE_PORTID_IDLE_PORTID_FIELD,
    &NGPON_RX_PROPRIETARY_IDLE_PORTID_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_IDLE_PORTID_REG = 
{
    "IDLE_PORTID",
#if RU_INCLUDE_DESC
    "IDLE_PORTID Register",
    "Idle Port-ID value for XGEM delineation",
#endif
    NGPON_RX_PROPRIETARY_IDLE_PORTID_REG_OFFSET,
    0,
    0,
    95,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PROPRIETARY_IDLE_PORTID_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS1_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL1_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN1_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_BIT_POS2_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_ON_VAL2_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_COMP_EN2_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FECTYPE_STABILITY_THR_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_OC_SOURCE_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_ALLOW_OTF_CHANGE_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL1_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FEC_OFF_VAL2_FIELD,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_REG = 
{
    "NGPON2_FEC_PARAMS",
#if RU_INCLUDE_DESC
    "NGPON2_FEC_PARAMS Register",
    "FEC on/off autodetect parameters for NGPON2 (based on OC structure)",
#endif
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_REG_OFFSET,
    0,
    0,
    96,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    12,
    NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_STARTTIME_FIELD,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_GRANTSIZE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_REG = 
{
    "AMCC_GRANT_PARAMS",
#if RU_INCLUDE_DESC
    "AMCC_GRANT_PARAMS Register",
    "Parameters of the AMCC grant",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_REG_OFFSET,
    0,
    0,
    97,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_PROC_ENABLE_FIELD,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_AMCC_SERIES_COND_FIELD,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FIRST_ONLY_FIELD,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED0_FIELD,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FLAGS_FIELD,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_COMP_FLAGS_FIELD,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED1_FIELD,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_ALLOCID_FIELD,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_REG = 
{
    "AMCC_GRANT_PARAMS2",
#if RU_INCLUDE_DESC
    "AMCC_GRANT_PARAMS2 Register",
    "AMCC grant processing",
#endif
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_REG_OFFSET,
    0,
    0,
    98,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    9,
    NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_K_TABLE1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_K_TABLE1_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_K_TABLE1_D1_FIELD,
    &NGPON_RX_PROPRIETARY_K_TABLE1_K1_FIELD,
    &NGPON_RX_PROPRIETARY_K_TABLE1_D2_FIELD,
    &NGPON_RX_PROPRIETARY_K_TABLE1_K2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_K_TABLE1_REG = 
{
    "K_TABLE1",
#if RU_INCLUDE_DESC
    "K_TABLE1 Register",
    "Translation table for K values",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE1_REG_OFFSET,
    0,
    0,
    99,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_RX_PROPRIETARY_K_TABLE1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_K_TABLE2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_K_TABLE2_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_K_TABLE2_D1_FIELD,
    &NGPON_RX_PROPRIETARY_K_TABLE2_K1_FIELD,
    &NGPON_RX_PROPRIETARY_K_TABLE2_D2_FIELD,
    &NGPON_RX_PROPRIETARY_K_TABLE2_K2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_K_TABLE2_REG = 
{
    "K_TABLE2",
#if RU_INCLUDE_DESC
    "K_TABLE2 Register",
    "Translation table for K values",
#endif
    NGPON_RX_PROPRIETARY_K_TABLE2_REG_OFFSET,
    0,
    0,
    100,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_RX_PROPRIETARY_K_TABLE2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_UNEXPECTED_K
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_UNEXPECTED_K_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_UNEXPECTED_K_CNT_FIELD,
    &NGPON_RX_PROPRIETARY_UNEXPECTED_K_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_UNEXPECTED_K_REG = 
{
    "UNEXPECTED_K",
#if RU_INCLUDE_DESC
    "UNEXPECTED_K Register",
    "Number of unexpected/missing K characters while not in hunt state",
#endif
    NGPON_RX_PROPRIETARY_UNEXPECTED_K_REG_OFFSET,
    0,
    0,
    101,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PROPRIETARY_UNEXPECTED_K_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_KEY_SELECT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_REG = 
{
    "PLOAM_USE_DEFAULT_KEY",
#if RU_INCLUDE_DESC
    "PLOAM_USE_DEFAULT_KEY Register",
    "Use default key for PLOAM MIC verification according to message type ID"
    ""
    "LSB corresponds to message type ID 0x1, MSB corresponds to message type ID 0x20",
#endif
    NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_REG_OFFSET,
    0,
    0,
    102,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_FIELDS[] =
{
    &NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_VALUE_FIELD,
    &NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_REG = 
{
    "TOD_MAX_DIFF_SECS",
#if RU_INCLUDE_DESC
    "TOD_MAX_DIFF_SECS Register",
    "Reserved",
#endif
    NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_REG_OFFSET,
    0,
    0,
    103,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_PROPRIETARY
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_PROPRIETARY_REGS[] =
{
    &NGPON_RX_PROPRIETARY_SYNC_CFG_REG,
    &NGPON_RX_PROPRIETARY_FEC_FORCE_REG,
    &NGPON_RX_PROPRIETARY_ACCESS_REG,
    &NGPON_RX_PROPRIETARY_DBG_VECT_REG,
    &NGPON_RX_PROPRIETARY_DBG_GENSTAT_REG,
    &NGPON_RX_PROPRIETARY_PLP_CFG_REG,
    &NGPON_RX_PROPRIETARY_PM_BIP_ERR_REG,
    &NGPON_RX_PROPRIETARY_CHICKEN_REG,
    &NGPON_RX_PROPRIETARY_PONID_MASK_MSB_REG,
    &NGPON_RX_PROPRIETARY_PONID_MASK_LSB_REG,
    &NGPON_RX_PROPRIETARY_IDLE_PORTID_REG,
    &NGPON_RX_PROPRIETARY_NGPON2_FEC_PARAMS_REG,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS_REG,
    &NGPON_RX_PROPRIETARY_AMCC_GRANT_PARAMS2_REG,
    &NGPON_RX_PROPRIETARY_K_TABLE1_REG,
    &NGPON_RX_PROPRIETARY_K_TABLE2_REG,
    &NGPON_RX_PROPRIETARY_UNEXPECTED_K_REG,
    &NGPON_RX_PROPRIETARY_PLOAM_USE_DEFAULT_KEY_REG,
    &NGPON_RX_PROPRIETARY_TOD_MAX_DIFF_SECS_REG,
};

static unsigned long NGPON_RX_PROPRIETARY_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80160f00,
#elif defined(CONFIG_BCM96856)
    0x82db4f00,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_PROPRIETARY_BLOCK = 
{
    "NGPON_RX_PROPRIETARY",
    NGPON_RX_PROPRIETARY_ADDRS,
    1,
    19,
    NGPON_RX_PROPRIETARY_REGS
};

/* End of file BCM6858_B0_NGPON_RX_PROPRIETARY.c */
