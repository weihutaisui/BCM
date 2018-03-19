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
 * Field: NGPON_RX_GEN_RCVRSTAT_FRAME_SYNC_STATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRSTAT_FRAME_SYNC_STATE_FIELD =
{
    "FRAME_SYNC_STATE",
#if RU_INCLUDE_DESC
    "frame_sync_state",
    "Indicates the state of the frame synchronizer",
#endif
    NGPON_RX_GEN_RCVRSTAT_FRAME_SYNC_STATE_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRSTAT_FRAME_SYNC_STATE_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRSTAT_FRAME_SYNC_STATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRSTAT_XGEM_SYNC_STATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRSTAT_XGEM_SYNC_STATE_FIELD =
{
    "XGEM_SYNC_STATE",
#if RU_INCLUDE_DESC
    "xgem_sync_state",
    "Indicates the state of the XGEM synchronizer",
#endif
    NGPON_RX_GEN_RCVRSTAT_XGEM_SYNC_STATE_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRSTAT_XGEM_SYNC_STATE_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRSTAT_XGEM_SYNC_STATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRSTAT_PONID_CONSIST_STATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRSTAT_PONID_CONSIST_STATE_FIELD =
{
    "PONID_CONSIST_STATE",
#if RU_INCLUDE_DESC
    "ponid_consist_state",
    "PON-ID tracker consistency state",
#endif
    NGPON_RX_GEN_RCVRSTAT_PONID_CONSIST_STATE_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRSTAT_PONID_CONSIST_STATE_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRSTAT_PONID_CONSIST_STATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRSTAT_FWI_STATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRSTAT_FWI_STATE_FIELD =
{
    "FWI_STATE",
#if RU_INCLUDE_DESC
    "fwi_state",
    "State of force wakeup field in BWMAPs",
#endif
    NGPON_RX_GEN_RCVRSTAT_FWI_STATE_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRSTAT_FWI_STATE_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRSTAT_FWI_STATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRSTAT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRSTAT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_GEN_RCVRSTAT_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRSTAT_RESERVED0_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRSTAT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRCFG_RX_ENABLE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRCFG_RX_ENABLE_FIELD =
{
    "RX_ENABLE",
#if RU_INCLUDE_DESC
    "rx_enable",
    "Enables/disables the entire receiver unit. This field can be changed on-the-fly during operation. Changes take effect on frame boundaries. Wait at least 1msec between changes."
    ""
    "Configuration, status and statistics are not lost on disable.",
#endif
    NGPON_RX_GEN_RCVRCFG_RX_ENABLE_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRCFG_RX_ENABLE_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRCFG_RX_ENABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRCFG_DESC_DISABLE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRCFG_DESC_DISABLE_FIELD =
{
    "DESC_DISABLE",
#if RU_INCLUDE_DESC
    "desc_disable",
    "Bypasses the descrambler so that the incoming data is passed as-is to the internal units without descrambling. This field can be changed on-the-fly during operation and takes effect on frame boundaries.",
#endif
    NGPON_RX_GEN_RCVRCFG_DESC_DISABLE_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRCFG_DESC_DISABLE_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRCFG_DESC_DISABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRCFG_MAC_MODE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRCFG_MAC_MODE_FIELD =
{
    "MAC_MODE",
#if RU_INCLUDE_DESC
    "mac_mode",
    "Selects MAC mode: XGPON1 (G.987) or NGPON2 (G.989) and rate."
    "It is only allowed to change this field when the MAC is disabled."
    "",
#endif
    NGPON_RX_GEN_RCVRCFG_MAC_MODE_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRCFG_MAC_MODE_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRCFG_MAC_MODE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRCFG_LOOPBACK_ENABLE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRCFG_LOOPBACK_ENABLE_FIELD =
{
    "LOOPBACK_ENABLE",
#if RU_INCLUDE_DESC
    "loopback_enable",
    "Reserved for loopback. This field can be changed on-the-fly during operation."
    ""
    "Loopback operation requires TM FIFO to be enabled.",
#endif
    NGPON_RX_GEN_RCVRCFG_LOOPBACK_ENABLE_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRCFG_LOOPBACK_ENABLE_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRCFG_LOOPBACK_ENABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRCFG_TMF_ENABLE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRCFG_TMF_ENABLE_FIELD =
{
    "TMF_ENABLE",
#if RU_INCLUDE_DESC
    "tmf_enable",
    "Enables/disables TM interface. This field can be changed on-the-fly during operation.",
#endif
    NGPON_RX_GEN_RCVRCFG_TMF_ENABLE_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRCFG_TMF_ENABLE_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRCFG_TMF_ENABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRCFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRCFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_GEN_RCVRCFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRCFG_RESERVED0_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRCFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRCFG_RX_DATA_INVERT
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRCFG_RX_DATA_INVERT_FIELD =
{
    "RX_DATA_INVERT",
#if RU_INCLUDE_DESC
    "rx_data_invert",
    "This bit controls the polarity of the GPON RX data input.",
#endif
    NGPON_RX_GEN_RCVRCFG_RX_DATA_INVERT_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRCFG_RX_DATA_INVERT_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRCFG_RX_DATA_INVERT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRCFG_K_8B10B
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRCFG_K_8B10B_FIELD =
{
    "K_8B10B",
#if RU_INCLUDE_DESC
    "k_8b10b",
    "Process K characters for 8b/10b mode."
    "It is only allowed to change this field when the MAC is disabled."
    "",
#endif
    NGPON_RX_GEN_RCVRCFG_K_8B10B_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRCFG_K_8B10B_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRCFG_K_8B10B_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RCVRCFG_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RCVRCFG_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_GEN_RCVRCFG_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_GEN_RCVRCFG_RESERVED1_FIELD_WIDTH,
    NGPON_RX_GEN_RCVRCFG_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_CFG_SYNC_LOSS_THR
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_CFG_SYNC_LOSS_THR_FIELD =
{
    "SYNC_LOSS_THR",
#if RU_INCLUDE_DESC
    "sync_loss_thr",
    "Number of consecutive incorrect PSync/SFC values required for assertion of the LOF alarm. This value should be changed only when the receiver is disabled."
    "The Nth frame is the last one to be processed.",
#endif
    NGPON_RX_GEN_SYNC_CFG_SYNC_LOSS_THR_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_CFG_SYNC_LOSS_THR_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_CFG_SYNC_LOSS_THR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_CFG_SYNC_ACQ_THR
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_CFG_SYNC_ACQ_THR_FIELD =
{
    "SYNC_ACQ_THR",
#if RU_INCLUDE_DESC
    "sync_acq_thr",
    "Number of consecutive correct PSync and SFC values required for de-assertion of the LOF alarm. This value should be changed only when the receiver is disabled.",
#endif
    NGPON_RX_GEN_SYNC_CFG_SYNC_ACQ_THR_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_CFG_SYNC_ACQ_THR_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_CFG_SYNC_ACQ_THR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_CFG_PONID_ACQ_THR
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_CFG_PONID_ACQ_THR_FIELD =
{
    "PONID_ACQ_THR",
#if RU_INCLUDE_DESC
    "ponid_acq_thr",
    "Number of consecutive consistent PON-ID values to adopt a new value, minus one. This value should be changed only when the receiver is disabled.",
#endif
    NGPON_RX_GEN_SYNC_CFG_PONID_ACQ_THR_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_CFG_PONID_ACQ_THR_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_CFG_PONID_ACQ_THR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_GEN_SYNC_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_CFG_EXT_SYNC_HOLDOVER
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_CFG_EXT_SYNC_HOLDOVER_FIELD =
{
    "EXT_SYNC_HOLDOVER",
#if RU_INCLUDE_DESC
    "ext_sync_holdover_enable",
    "Enable holdover mode of the start of frame signal output (does not survive negation and re-assertion of rx_enable).",
#endif
    NGPON_RX_GEN_SYNC_CFG_EXT_SYNC_HOLDOVER_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_CFG_EXT_SYNC_HOLDOVER_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_CFG_EXT_SYNC_HOLDOVER_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_CFG_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_CFG_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_GEN_SYNC_CFG_RESERVED1_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_CFG_RESERVED1_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_CFG_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_STAT_SYNC_CUR_ALIGN
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_STAT_SYNC_CUR_ALIGN_FIELD =
{
    "SYNC_CUR_ALIGN",
#if RU_INCLUDE_DESC
    "sync_cur_align",
    "Current bit alignment",
#endif
    NGPON_RX_GEN_SYNC_STAT_SYNC_CUR_ALIGN_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_STAT_SYNC_CUR_ALIGN_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_STAT_SYNC_CUR_ALIGN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_STAT_R1
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_STAT_R1_FIELD =
{
    "R1",
#if RU_INCLUDE_DESC
    "R1",
    "R1",
#endif
    NGPON_RX_GEN_SYNC_STAT_R1_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_STAT_R1_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_STAT_R1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_STAT_SYNC_MATCH_CTR
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_STAT_SYNC_MATCH_CTR_FIELD =
{
    "SYNC_MATCH_CTR",
#if RU_INCLUDE_DESC
    "sync_match_ctr",
    "Sync machine match counter",
#endif
    NGPON_RX_GEN_SYNC_STAT_SYNC_MATCH_CTR_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_STAT_SYNC_MATCH_CTR_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_STAT_SYNC_MATCH_CTR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_STAT_SYNC_MISMATCH_CTR
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_STAT_SYNC_MISMATCH_CTR_FIELD =
{
    "SYNC_MISMATCH_CTR",
#if RU_INCLUDE_DESC
    "sync_mismatch_ctr",
    "Sync machine mismatch counter",
#endif
    NGPON_RX_GEN_SYNC_STAT_SYNC_MISMATCH_CTR_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_STAT_SYNC_MISMATCH_CTR_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_STAT_SYNC_MISMATCH_CTR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_STAT_R2
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_STAT_R2_FIELD =
{
    "R2",
#if RU_INCLUDE_DESC
    "R2",
    "R2",
#endif
    NGPON_RX_GEN_SYNC_STAT_R2_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_STAT_R2_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_STAT_R2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Sets the most significant half of the expected PSync word",
#endif
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_VALUE_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Sets the least significant half of the expected PSync word. This value should be changed only when the receiver is disabled.",
#endif
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_VALUE_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_FRAME_LENGTH_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_FRAME_LENGTH_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Sets the XGPON frame length."
    "10G: 0x97e0"
    "2.5G: 0x25f8",
#endif
    NGPON_RX_GEN_SYNC_FRAME_LENGTH_VALUE_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_FRAME_LENGTH_VALUE_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_FRAME_LENGTH_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_FRAME_LENGTH_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_FRAME_LENGTH_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_GEN_SYNC_FRAME_LENGTH_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_FRAME_LENGTH_RESERVED0_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_FRAME_LENGTH_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_RANDOM_SEED_RANDOM_SEED
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_RANDOM_SEED_RANDOM_SEED_FIELD =
{
    "RANDOM_SEED",
#if RU_INCLUDE_DESC
    "Random_Seed",
    "Random seed",
#endif
    NGPON_RX_GEN_RANDOM_SEED_RANDOM_SEED_FIELD_MASK,
    0,
    NGPON_RX_GEN_RANDOM_SEED_RANDOM_SEED_FIELD_WIDTH,
    NGPON_RX_GEN_RANDOM_SEED_RANDOM_SEED_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PONID_MS_ADMIN_LABEL_MSB
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PONID_MS_ADMIN_LABEL_MSB_FIELD =
{
    "ADMIN_LABEL_MSB",
#if RU_INCLUDE_DESC
    "ADMIN_LABEL_MSB",
    "Administrative label - most significant bits",
#endif
    NGPON_RX_GEN_SYNC_PONID_MS_ADMIN_LABEL_MSB_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PONID_MS_ADMIN_LABEL_MSB_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PONID_MS_ADMIN_LABEL_MSB_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PONID_MS_PIT_RSVD
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PONID_MS_PIT_RSVD_FIELD =
{
    "PIT_RSVD",
#if RU_INCLUDE_DESC
    "PIT_RSVD",
    "PON-ID type - reserved bits",
#endif
    NGPON_RX_GEN_SYNC_PONID_MS_PIT_RSVD_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PONID_MS_PIT_RSVD_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PONID_MS_PIT_RSVD_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PONID_MS_PIT_ODN_CLASS
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PONID_MS_PIT_ODN_CLASS_FIELD =
{
    "PIT_ODN_CLASS",
#if RU_INCLUDE_DESC
    "PIT_ODN_CLASS",
    "PON-ID type - ODN class",
#endif
    NGPON_RX_GEN_SYNC_PONID_MS_PIT_ODN_CLASS_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PONID_MS_PIT_ODN_CLASS_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PONID_MS_PIT_ODN_CLASS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PONID_MS_PIT_RE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PONID_MS_PIT_RE_FIELD =
{
    "PIT_RE",
#if RU_INCLUDE_DESC
    "PIT_RE",
    "PON-ID type - indicates whether the transmit optical level (TOL) field contains the launch power of the OLT or of a reach extender",
#endif
    NGPON_RX_GEN_SYNC_PONID_MS_PIT_RE_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PONID_MS_PIT_RE_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PONID_MS_PIT_RE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PONID_MS_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PONID_MS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_GEN_SYNC_PONID_MS_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PONID_MS_RESERVED0_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PONID_MS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PONID_LS_TOL
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PONID_LS_TOL_FIELD =
{
    "TOL",
#if RU_INCLUDE_DESC
    "TOL",
    "Transmit optical level",
#endif
    NGPON_RX_GEN_SYNC_PONID_LS_TOL_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PONID_LS_TOL_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PONID_LS_TOL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PONID_LS_DWLCH_ID
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PONID_LS_DWLCH_ID_FIELD =
{
    "DWLCH_ID",
#if RU_INCLUDE_DESC
    "DWLCH_ID",
    "Downstream wavelength channel ID",
#endif
    NGPON_RX_GEN_SYNC_PONID_LS_DWLCH_ID_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PONID_LS_DWLCH_ID_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PONID_LS_DWLCH_ID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_PONID_LS_ADMIN_LABEL_LSB
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_PONID_LS_ADMIN_LABEL_LSB_FIELD =
{
    "ADMIN_LABEL_LSB",
#if RU_INCLUDE_DESC
    "ADMIN_LABEL_LSB",
    "Administrative label - least significant bits",
#endif
    NGPON_RX_GEN_SYNC_PONID_LS_ADMIN_LABEL_LSB_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_PONID_LS_ADMIN_LABEL_LSB_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_PONID_LS_ADMIN_LABEL_LSB_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_SFC_MS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_SFC_MS_VALUE_FIELD =
{
    "MS_VALUE",
#if RU_INCLUDE_DESC
    "MS_Value",
    "The most significant half of the superframe counter",
#endif
    NGPON_RX_GEN_SYNC_SFC_MS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_SFC_MS_VALUE_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_SFC_MS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_SFC_VALID
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_SFC_VALID_FIELD =
{
    "VALID",
#if RU_INCLUDE_DESC
    "Valid",
    "Reports whether the last received frame had a correct or wrong PSync and SFC values.",
#endif
    NGPON_RX_GEN_SYNC_SFC_VALID_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_SFC_VALID_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_SFC_VALID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_SFC_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_SFC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_GEN_SYNC_SFC_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_SFC_RESERVED0_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_SFC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_SFC_LS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_SFC_LS_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "The least significant half of the superframe counter",
#endif
    NGPON_RX_GEN_SYNC_SFC_LS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_SFC_LS_VALUE_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_SFC_LS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_SFC_IND_MS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_SFC_IND_MS_VALUE_FIELD =
{
    "MS_VALUE",
#if RU_INCLUDE_DESC
    "MS_Value",
    "The most significant half of the superframe counter",
#endif
    NGPON_RX_GEN_SYNC_SFC_IND_MS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_SFC_IND_MS_VALUE_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_SFC_IND_MS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_SFC_IND_ENABLE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_SFC_IND_ENABLE_FIELD =
{
    "ENABLE",
#if RU_INCLUDE_DESC
    "Enable",
    "Enable SFC indication processing."
    ""
    "Changing the set SFC is allowed only when this bit is disabled.",
#endif
    NGPON_RX_GEN_SYNC_SFC_IND_ENABLE_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_SFC_IND_ENABLE_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_SFC_IND_ENABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_SFC_IND_HOLDOVER
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_SFC_IND_HOLDOVER_FIELD =
{
    "HOLDOVER",
#if RU_INCLUDE_DESC
    "Holdover",
    "Enable holdover mode",
#endif
    NGPON_RX_GEN_SYNC_SFC_IND_HOLDOVER_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_SFC_IND_HOLDOVER_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_SFC_IND_HOLDOVER_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_SFC_IND_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_SFC_IND_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_GEN_SYNC_SFC_IND_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_SFC_IND_RESERVED0_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_SFC_IND_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_SYNC_SFC_IND_LS_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_SYNC_SFC_IND_LS_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "The least significant half of the superframe counter",
#endif
    NGPON_RX_GEN_SYNC_SFC_IND_LS_VALUE_FIELD_MASK,
    0,
    NGPON_RX_GEN_SYNC_SFC_IND_LS_VALUE_FIELD_WIDTH,
    NGPON_RX_GEN_SYNC_SFC_IND_LS_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_VERSION_LLD_MINOR
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_VERSION_LLD_MINOR_FIELD =
{
    "LLD_MINOR",
#if RU_INCLUDE_DESC
    "LLD_MINOR",
    "LLD minor version",
#endif
    NGPON_RX_GEN_VERSION_LLD_MINOR_FIELD_MASK,
    0,
    NGPON_RX_GEN_VERSION_LLD_MINOR_FIELD_WIDTH,
    NGPON_RX_GEN_VERSION_LLD_MINOR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_VERSION_LLD_MAJOR
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_VERSION_LLD_MAJOR_FIELD =
{
    "LLD_MAJOR",
#if RU_INCLUDE_DESC
    "LLD_MAJOR",
    "LLD major version",
#endif
    NGPON_RX_GEN_VERSION_LLD_MAJOR_FIELD_MASK,
    0,
    NGPON_RX_GEN_VERSION_LLD_MAJOR_FIELD_WIDTH,
    NGPON_RX_GEN_VERSION_LLD_MAJOR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_GEN_VERSION_DATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_GEN_VERSION_DATE_FIELD =
{
    "DATE",
#if RU_INCLUDE_DESC
    "DATE",
    "Release date DDMMYY (BCD)",
#endif
    NGPON_RX_GEN_VERSION_DATE_FIELD_MASK,
    0,
    NGPON_RX_GEN_VERSION_DATE_FIELD_WIDTH,
    NGPON_RX_GEN_VERSION_DATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_GEN_RCVRSTAT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_RCVRSTAT_FIELDS[] =
{
    &NGPON_RX_GEN_RCVRSTAT_FRAME_SYNC_STATE_FIELD,
    &NGPON_RX_GEN_RCVRSTAT_XGEM_SYNC_STATE_FIELD,
    &NGPON_RX_GEN_RCVRSTAT_PONID_CONSIST_STATE_FIELD,
    &NGPON_RX_GEN_RCVRSTAT_FWI_STATE_FIELD,
    &NGPON_RX_GEN_RCVRSTAT_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_RCVRSTAT_REG = 
{
    "RCVRSTAT",
#if RU_INCLUDE_DESC
    "RECEIVER_STATUS Register",
    "This register shows the status of different receiver sub-units",
#endif
    NGPON_RX_GEN_RCVRSTAT_REG_OFFSET,
    0,
    0,
    0,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    NGPON_RX_GEN_RCVRSTAT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_RCVRCFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_RCVRCFG_FIELDS[] =
{
    &NGPON_RX_GEN_RCVRCFG_RX_ENABLE_FIELD,
    &NGPON_RX_GEN_RCVRCFG_DESC_DISABLE_FIELD,
    &NGPON_RX_GEN_RCVRCFG_MAC_MODE_FIELD,
    &NGPON_RX_GEN_RCVRCFG_LOOPBACK_ENABLE_FIELD,
    &NGPON_RX_GEN_RCVRCFG_TMF_ENABLE_FIELD,
    &NGPON_RX_GEN_RCVRCFG_RESERVED0_FIELD,
    &NGPON_RX_GEN_RCVRCFG_RX_DATA_INVERT_FIELD,
    &NGPON_RX_GEN_RCVRCFG_K_8B10B_FIELD,
    &NGPON_RX_GEN_RCVRCFG_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_RCVRCFG_REG = 
{
    "RCVRCFG",
#if RU_INCLUDE_DESC
    "RECEIVER_CONFIG Register",
    "This register controls general parameters of the receiver",
#endif
    NGPON_RX_GEN_RCVRCFG_REG_OFFSET,
    0,
    0,
    1,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    9,
    NGPON_RX_GEN_RCVRCFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_CFG_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_CFG_SYNC_LOSS_THR_FIELD,
    &NGPON_RX_GEN_SYNC_CFG_SYNC_ACQ_THR_FIELD,
    &NGPON_RX_GEN_SYNC_CFG_PONID_ACQ_THR_FIELD,
    &NGPON_RX_GEN_SYNC_CFG_RESERVED0_FIELD,
    &NGPON_RX_GEN_SYNC_CFG_EXT_SYNC_HOLDOVER_FIELD,
    &NGPON_RX_GEN_SYNC_CFG_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_CFG_REG = 
{
    "SYNC_CFG",
#if RU_INCLUDE_DESC
    "SYNCHRONIZER_CONFIG Register",
    "This register controls parameters of the frame synchronizer",
#endif
    NGPON_RX_GEN_SYNC_CFG_REG_OFFSET,
    0,
    0,
    2,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    6,
    NGPON_RX_GEN_SYNC_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_STAT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_STAT_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_STAT_SYNC_CUR_ALIGN_FIELD,
    &NGPON_RX_GEN_SYNC_STAT_R1_FIELD,
    &NGPON_RX_GEN_SYNC_STAT_SYNC_MATCH_CTR_FIELD,
    &NGPON_RX_GEN_SYNC_STAT_SYNC_MISMATCH_CTR_FIELD,
    &NGPON_RX_GEN_SYNC_STAT_R2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_STAT_REG = 
{
    "SYNC_STAT",
#if RU_INCLUDE_DESC
    "SYNCHRONIZER_STATUS Register",
    "This register shows the status of the frame synchronizer",
#endif
    NGPON_RX_GEN_SYNC_STAT_REG_OFFSET,
    0,
    0,
    3,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    NGPON_RX_GEN_SYNC_STAT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_REG = 
{
    "SYNC_PSYNC_VALUE_MS",
#if RU_INCLUDE_DESC
    "PSYNC_VALUE_MS Register",
    "Sets the most significant half of the expected PSync word. This value should be changed only when the receiver is disabled.",
#endif
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_REG_OFFSET,
    0,
    0,
    4,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_REG = 
{
    "SYNC_PSYNC_VALUE_LS",
#if RU_INCLUDE_DESC
    "PSYNC_VALUE_LS Register",
    "Sets the least significant half of the expected PSync word",
#endif
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_REG_OFFSET,
    0,
    0,
    5,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_FRAME_LENGTH
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_FRAME_LENGTH_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_FRAME_LENGTH_VALUE_FIELD,
    &NGPON_RX_GEN_SYNC_FRAME_LENGTH_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_FRAME_LENGTH_REG = 
{
    "SYNC_FRAME_LENGTH",
#if RU_INCLUDE_DESC
    "FRAME_LENGTH Register",
    "Sets the frame length. For XGPON only, the set length must be 24 + (N x 248), N integer."
    "This value should be changed only when the receiver is disabled. Changing RCVRCFG.MAC_MODE modifies this register.",
#endif
    NGPON_RX_GEN_SYNC_FRAME_LENGTH_REG_OFFSET,
    0,
    0,
    6,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_RX_GEN_SYNC_FRAME_LENGTH_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_RANDOM_SEED
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_RANDOM_SEED_FIELDS[] =
{
    &NGPON_RX_GEN_RANDOM_SEED_RANDOM_SEED_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_RANDOM_SEED_REG = 
{
    "RANDOM_SEED",
#if RU_INCLUDE_DESC
    "RANDOM_SEED Register",
    "This register contains a random 32 bit number which changes constantly",
#endif
    NGPON_RX_GEN_RANDOM_SEED_REG_OFFSET,
    0,
    0,
    7,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_GEN_RANDOM_SEED_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_PONID_MS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_PONID_MS_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_PONID_MS_ADMIN_LABEL_MSB_FIELD,
    &NGPON_RX_GEN_SYNC_PONID_MS_PIT_RSVD_FIELD,
    &NGPON_RX_GEN_SYNC_PONID_MS_PIT_ODN_CLASS_FIELD,
    &NGPON_RX_GEN_SYNC_PONID_MS_PIT_RE_FIELD,
    &NGPON_RX_GEN_SYNC_PONID_MS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_PONID_MS_REG = 
{
    "SYNC_PONID_MS",
#if RU_INCLUDE_DESC
    "PONID_MS Register",
    "Shows the most significant half of the OC field (operational control). Reading this register latches the whole OC field (operational control).",
#endif
    NGPON_RX_GEN_SYNC_PONID_MS_REG_OFFSET,
    0,
    0,
    8,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    NGPON_RX_GEN_SYNC_PONID_MS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_PONID_LS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_PONID_LS_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_PONID_LS_TOL_FIELD,
    &NGPON_RX_GEN_SYNC_PONID_LS_DWLCH_ID_FIELD,
    &NGPON_RX_GEN_SYNC_PONID_LS_ADMIN_LABEL_LSB_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_PONID_LS_REG = 
{
    "SYNC_PONID_LS",
#if RU_INCLUDE_DESC
    "PONID_LS Register",
    "Shows the least significant half of the PON-ID (operational control). Value is latched when MS part is read.",
#endif
    NGPON_RX_GEN_SYNC_PONID_LS_REG_OFFSET,
    0,
    0,
    9,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    NGPON_RX_GEN_SYNC_PONID_LS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_SFC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_SFC_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_SFC_MS_VALUE_FIELD,
    &NGPON_RX_GEN_SYNC_SFC_VALID_FIELD,
    &NGPON_RX_GEN_SYNC_SFC_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_SFC_REG = 
{
    "SYNC_SFC",
#if RU_INCLUDE_DESC
    "SFC Register",
    "Shows the most significant half of the superframe counter and valid information. Reading this register latches the whole SFC value.",
#endif
    NGPON_RX_GEN_SYNC_SFC_REG_OFFSET,
    0,
    0,
    10,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    NGPON_RX_GEN_SYNC_SFC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_SFC_LS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_SFC_LS_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_SFC_LS_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_SFC_LS_REG = 
{
    "SYNC_SFC_LS",
#if RU_INCLUDE_DESC
    "SFC_LS Register",
    "Shows the least significant half of the superframe counter. Value is latched when MS part is read.",
#endif
    NGPON_RX_GEN_SYNC_SFC_LS_REG_OFFSET,
    0,
    0,
    11,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_GEN_SYNC_SFC_LS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_SFC_IND
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_SFC_IND_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_SFC_IND_MS_VALUE_FIELD,
    &NGPON_RX_GEN_SYNC_SFC_IND_ENABLE_FIELD,
    &NGPON_RX_GEN_SYNC_SFC_IND_HOLDOVER_FIELD,
    &NGPON_RX_GEN_SYNC_SFC_IND_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_SFC_IND_REG = 
{
    "SYNC_SFC_IND",
#if RU_INCLUDE_DESC
    "SYNC_SFC_IND Register",
    "Sets the most significant half of the required superframe counter for SFC_IND interrupt. When a frame with the set SFC is reached, an interrupt is asserted."
    ""
    "Changing the set SFC is allowed only when the indication mechanism is disabled.",
#endif
    NGPON_RX_GEN_SYNC_SFC_IND_REG_OFFSET,
    0,
    0,
    12,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_RX_GEN_SYNC_SFC_IND_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_SYNC_SFC_IND_LS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_SYNC_SFC_IND_LS_FIELDS[] =
{
    &NGPON_RX_GEN_SYNC_SFC_IND_LS_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_SYNC_SFC_IND_LS_REG = 
{
    "SYNC_SFC_IND_LS",
#if RU_INCLUDE_DESC
    "SYNC_SFC_IND_LS Register",
    "Sets the least significant half of the required superframe counter for SFC_IND interrupt. When a frame with the set SFC is reached, an interrupt is asserted."
    ""
    "Changing the set SFC is allowed only when the indication mechanism is disabled."
    "",
#endif
    NGPON_RX_GEN_SYNC_SFC_IND_LS_REG_OFFSET,
    0,
    0,
    13,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_RX_GEN_SYNC_SFC_IND_LS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_GEN_VERSION
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_GEN_VERSION_FIELDS[] =
{
    &NGPON_RX_GEN_VERSION_LLD_MINOR_FIELD,
    &NGPON_RX_GEN_VERSION_LLD_MAJOR_FIELD,
    &NGPON_RX_GEN_VERSION_DATE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_GEN_VERSION_REG = 
{
    "VERSION",
#if RU_INCLUDE_DESC
    "VESRION Register",
    "Version of module",
#endif
    NGPON_RX_GEN_VERSION_REG_OFFSET,
    0,
    0,
    14,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    NGPON_RX_GEN_VERSION_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_GEN
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_GEN_REGS[] =
{
    &NGPON_RX_GEN_RCVRSTAT_REG,
    &NGPON_RX_GEN_RCVRCFG_REG,
    &NGPON_RX_GEN_SYNC_CFG_REG,
    &NGPON_RX_GEN_SYNC_STAT_REG,
    &NGPON_RX_GEN_SYNC_PSYNC_VALUE_MS_REG,
    &NGPON_RX_GEN_SYNC_PSYNC_VALUE_LS_REG,
    &NGPON_RX_GEN_SYNC_FRAME_LENGTH_REG,
    &NGPON_RX_GEN_RANDOM_SEED_REG,
    &NGPON_RX_GEN_SYNC_PONID_MS_REG,
    &NGPON_RX_GEN_SYNC_PONID_LS_REG,
    &NGPON_RX_GEN_SYNC_SFC_REG,
    &NGPON_RX_GEN_SYNC_SFC_LS_REG,
    &NGPON_RX_GEN_SYNC_SFC_IND_REG,
    &NGPON_RX_GEN_SYNC_SFC_IND_LS_REG,
    &NGPON_RX_GEN_VERSION_REG,
};

unsigned long NGPON_RX_GEN_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80160000,
#elif defined(CONFIG_BCM96856)
    0x82db4000,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_GEN_BLOCK = 
{
    "NGPON_RX_GEN",
    NGPON_RX_GEN_ADDRS,
    1,
    15,
    NGPON_RX_GEN_REGS
};

/* End of file BCM6858_A0_NGPON_RX_GEN.c */
