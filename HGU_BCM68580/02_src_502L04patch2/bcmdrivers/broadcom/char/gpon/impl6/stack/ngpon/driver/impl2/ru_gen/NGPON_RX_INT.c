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
 * Field: NGPON_RX_INT_ISR_TM_FIFO_OVF
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_TM_FIFO_OVF_FIELD =
{
    "TM_FIFO_OVF",
#if RU_INCLUDE_DESC
    "tm_fifo_ovf",
    "TM FIFO overflow",
#endif
    NGPON_RX_INT_ISR_TM_FIFO_OVF_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_TM_FIFO_OVF_FIELD_WIDTH,
    NGPON_RX_INT_ISR_TM_FIFO_OVF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_BW_FIFO_OVF
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_BW_FIFO_OVF_FIELD =
{
    "BW_FIFO_OVF",
#if RU_INCLUDE_DESC
    "bw_fifo_ovf",
    "Access FIFO overflow",
#endif
    NGPON_RX_INT_ISR_BW_FIFO_OVF_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_BW_FIFO_OVF_FIELD_WIDTH,
    NGPON_RX_INT_ISR_BW_FIFO_OVF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_AES_FAIL
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_AES_FAIL_FIELD =
{
    "AES_FAIL",
#if RU_INCLUDE_DESC
    "aes_fail",
    "AES fail to allocate error (no AES machines could be allocated to decrypt a fragment)",
#endif
    NGPON_RX_INT_ISR_AES_FAIL_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_AES_FAIL_FIELD_WIDTH,
    NGPON_RX_INT_ISR_AES_FAIL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_FWI_STATE_CHANGE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_FWI_STATE_CHANGE_FIELD =
{
    "FWI_STATE_CHANGE",
#if RU_INCLUDE_DESC
    "fwi_state_change",
    "Assertion or negation of FWI in received BWMAPs",
#endif
    NGPON_RX_INT_ISR_FWI_STATE_CHANGE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_FWI_STATE_CHANGE_FIELD_WIDTH,
    NGPON_RX_INT_ISR_FWI_STATE_CHANGE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_LOF_STATE_CHANGE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_LOF_STATE_CHANGE_FIELD =
{
    "LOF_STATE_CHANGE",
#if RU_INCLUDE_DESC
    "lof_state_change",
    "Acquire or loss of frame synchronization",
#endif
    NGPON_RX_INT_ISR_LOF_STATE_CHANGE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_LOF_STATE_CHANGE_FIELD_WIDTH,
    NGPON_RX_INT_ISR_LOF_STATE_CHANGE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_LCDG_STATE_CHANGE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_LCDG_STATE_CHANGE_FIELD =
{
    "LCDG_STATE_CHANGE",
#if RU_INCLUDE_DESC
    "lcdg_state_change",
    "Assertion or negation of loss of XGEM frame delineation signal",
#endif
    NGPON_RX_INT_ISR_LCDG_STATE_CHANGE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_LCDG_STATE_CHANGE_FIELD_WIDTH,
    NGPON_RX_INT_ISR_LCDG_STATE_CHANGE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_LB_FIFO_OVF
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_LB_FIFO_OVF_FIELD =
{
    "LB_FIFO_OVF",
#if RU_INCLUDE_DESC
    "lb_fifo_ovf",
    "LB FIFO overflow",
#endif
    NGPON_RX_INT_ISR_LB_FIFO_OVF_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_LB_FIFO_OVF_FIELD_WIDTH,
    NGPON_RX_INT_ISR_LB_FIFO_OVF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_TOD_UPDATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_TOD_UPDATE_FIELD =
{
    "TOD_UPDATE",
#if RU_INCLUDE_DESC
    "tod_update",
    "An update of the TOD counters occured",
#endif
    NGPON_RX_INT_ISR_TOD_UPDATE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_TOD_UPDATE_FIELD_WIDTH,
    NGPON_RX_INT_ISR_TOD_UPDATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_BW_DIS_TX
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_BW_DIS_TX_FIELD =
{
    "BW_DIS_TX",
#if RU_INCLUDE_DESC
    "bw_dis_tx",
    "Bandwidth allocation to a disabled transmitter",
#endif
    NGPON_RX_INT_ISR_BW_DIS_TX_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_BW_DIS_TX_FIELD_WIDTH,
    NGPON_RX_INT_ISR_BW_DIS_TX_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_INV_KEY
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_INV_KEY_FIELD =
{
    "INV_KEY",
#if RU_INCLUDE_DESC
    "inv_key",
    "Incoming XGEM with a non-valid key index",
#endif
    NGPON_RX_INT_ISR_INV_KEY_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_INV_KEY_FIELD_WIDTH,
    NGPON_RX_INT_ISR_INV_KEY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_BWMAP_REC_DONE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_BWMAP_REC_DONE_FIELD =
{
    "BWMAP_REC_DONE",
#if RU_INCLUDE_DESC
    "bwmap_rec_done",
    "Bandwidth map recorder done",
#endif
    NGPON_RX_INT_ISR_BWMAP_REC_DONE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_BWMAP_REC_DONE_FIELD_WIDTH,
    NGPON_RX_INT_ISR_BWMAP_REC_DONE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_PONID_INCONSIST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_PONID_INCONSIST_FIELD =
{
    "PONID_INCONSIST",
#if RU_INCLUDE_DESC
    "ponid_inconsist",
    "First PON-ID inconsistency. Note that a new PON-ID value is available for read only after it has stabilized.",
#endif
    NGPON_RX_INT_ISR_PONID_INCONSIST_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_PONID_INCONSIST_FIELD_WIDTH,
    NGPON_RX_INT_ISR_PONID_INCONSIST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_INV_PORTID_CHANGE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_INV_PORTID_CHANGE_FIELD =
{
    "INV_PORTID_CHANGE",
#if RU_INCLUDE_DESC
    "inv_portid_change",
    "Invalid Port-ID between Ethernet fragments",
#endif
    NGPON_RX_INT_ISR_INV_PORTID_CHANGE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_INV_PORTID_CHANGE_FIELD_WIDTH,
    NGPON_RX_INT_ISR_INV_PORTID_CHANGE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_FEC_FATAL_ERROR
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_FEC_FATAL_ERROR_FIELD =
{
    "FEC_FATAL_ERROR",
#if RU_INCLUDE_DESC
    "fec_fatal_error",
    "Fatal error in FEC block (check map)",
#endif
    NGPON_RX_INT_ISR_FEC_FATAL_ERROR_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_FEC_FATAL_ERROR_FIELD_WIDTH,
    NGPON_RX_INT_ISR_FEC_FATAL_ERROR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_SFC_INDICATION
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_SFC_INDICATION_FIELD =
{
    "SFC_INDICATION",
#if RU_INCLUDE_DESC
    "sfc_indication",
    "Indicates the start of the superframe set at gen.SYNC_SFC_IND",
#endif
    NGPON_RX_INT_ISR_SFC_INDICATION_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_SFC_INDICATION_FIELD_WIDTH,
    NGPON_RX_INT_ISR_SFC_INDICATION_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_AMCC_GRANT
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_AMCC_GRANT_FIELD =
{
    "AMCC_GRANT",
#if RU_INCLUDE_DESC
    "amcc_grant",
    "Reserved for AMCC grant indication",
#endif
    NGPON_RX_INT_ISR_AMCC_GRANT_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_AMCC_GRANT_FIELD_WIDTH,
    NGPON_RX_INT_ISR_AMCC_GRANT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_DWBA
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_DWBA_FIELD =
{
    "DWBA",
#if RU_INCLUDE_DESC
    "dwba",
    "Reserved for DWBA",
#endif
    NGPON_RX_INT_ISR_DWBA_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_DWBA_FIELD_WIDTH,
    NGPON_RX_INT_ISR_DWBA_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISR_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_INT_ISR_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_INT_ISR_RESERVED0_FIELD_WIDTH,
    NGPON_RX_INT_ISR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_TM_FIFO_OVF
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_TM_FIFO_OVF_FIELD =
{
    "TM_FIFO_OVF",
#if RU_INCLUDE_DESC
    "tm_fifo_ovf",
    "TM FIFO overflow",
#endif
    NGPON_RX_INT_ISM_TM_FIFO_OVF_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_TM_FIFO_OVF_FIELD_WIDTH,
    NGPON_RX_INT_ISM_TM_FIFO_OVF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_BW_FIFO_OVF
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_BW_FIFO_OVF_FIELD =
{
    "BW_FIFO_OVF",
#if RU_INCLUDE_DESC
    "bw_fifo_ovf",
    "Access FIFO overflow",
#endif
    NGPON_RX_INT_ISM_BW_FIFO_OVF_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_BW_FIFO_OVF_FIELD_WIDTH,
    NGPON_RX_INT_ISM_BW_FIFO_OVF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_AES_FAIL
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_AES_FAIL_FIELD =
{
    "AES_FAIL",
#if RU_INCLUDE_DESC
    "aes_fail",
    "AES fail to allocate error (no AES machines could be allocated to decrypt a fragment)",
#endif
    NGPON_RX_INT_ISM_AES_FAIL_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_AES_FAIL_FIELD_WIDTH,
    NGPON_RX_INT_ISM_AES_FAIL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_FWI_STATE_CHANGE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_FWI_STATE_CHANGE_FIELD =
{
    "FWI_STATE_CHANGE",
#if RU_INCLUDE_DESC
    "fwi_state_change",
    "Assertion or negation of FWI in received BWMAPs",
#endif
    NGPON_RX_INT_ISM_FWI_STATE_CHANGE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_FWI_STATE_CHANGE_FIELD_WIDTH,
    NGPON_RX_INT_ISM_FWI_STATE_CHANGE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_LOF_STATE_CHANGE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_LOF_STATE_CHANGE_FIELD =
{
    "LOF_STATE_CHANGE",
#if RU_INCLUDE_DESC
    "lof_state_change",
    "Acquire or loss of frame synchronization",
#endif
    NGPON_RX_INT_ISM_LOF_STATE_CHANGE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_LOF_STATE_CHANGE_FIELD_WIDTH,
    NGPON_RX_INT_ISM_LOF_STATE_CHANGE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_LCDG_STATE_CHANGE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_LCDG_STATE_CHANGE_FIELD =
{
    "LCDG_STATE_CHANGE",
#if RU_INCLUDE_DESC
    "lcdg_state_change",
    "Assertion or negation of loss of XGEM frame delineation signal",
#endif
    NGPON_RX_INT_ISM_LCDG_STATE_CHANGE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_LCDG_STATE_CHANGE_FIELD_WIDTH,
    NGPON_RX_INT_ISM_LCDG_STATE_CHANGE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_LB_FIFO_OVF
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_LB_FIFO_OVF_FIELD =
{
    "LB_FIFO_OVF",
#if RU_INCLUDE_DESC
    "lb_fifo_ovf",
    "LB FIFO overflow",
#endif
    NGPON_RX_INT_ISM_LB_FIFO_OVF_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_LB_FIFO_OVF_FIELD_WIDTH,
    NGPON_RX_INT_ISM_LB_FIFO_OVF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_TOD_UPDATE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_TOD_UPDATE_FIELD =
{
    "TOD_UPDATE",
#if RU_INCLUDE_DESC
    "tod_update",
    "An update of the TOD counters occured",
#endif
    NGPON_RX_INT_ISM_TOD_UPDATE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_TOD_UPDATE_FIELD_WIDTH,
    NGPON_RX_INT_ISM_TOD_UPDATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_BW_DIS_TX
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_BW_DIS_TX_FIELD =
{
    "BW_DIS_TX",
#if RU_INCLUDE_DESC
    "bw_dis_tx",
    "Bandwidth allocation to a disabled transmitter",
#endif
    NGPON_RX_INT_ISM_BW_DIS_TX_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_BW_DIS_TX_FIELD_WIDTH,
    NGPON_RX_INT_ISM_BW_DIS_TX_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_INV_KEY
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_INV_KEY_FIELD =
{
    "INV_KEY",
#if RU_INCLUDE_DESC
    "inv_key",
    "Incoming XGEM with a non-valid key index",
#endif
    NGPON_RX_INT_ISM_INV_KEY_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_INV_KEY_FIELD_WIDTH,
    NGPON_RX_INT_ISM_INV_KEY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_BWMAP_REC_DONE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_BWMAP_REC_DONE_FIELD =
{
    "BWMAP_REC_DONE",
#if RU_INCLUDE_DESC
    "bwmap_rec_done",
    "Bandwidth map recorder done",
#endif
    NGPON_RX_INT_ISM_BWMAP_REC_DONE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_BWMAP_REC_DONE_FIELD_WIDTH,
    NGPON_RX_INT_ISM_BWMAP_REC_DONE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_PONID_INCONSIST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_PONID_INCONSIST_FIELD =
{
    "PONID_INCONSIST",
#if RU_INCLUDE_DESC
    "ponid_inconsist",
    "First PON-ID inconsistency",
#endif
    NGPON_RX_INT_ISM_PONID_INCONSIST_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_PONID_INCONSIST_FIELD_WIDTH,
    NGPON_RX_INT_ISM_PONID_INCONSIST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_INV_PORTID_CHANGE
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_INV_PORTID_CHANGE_FIELD =
{
    "INV_PORTID_CHANGE",
#if RU_INCLUDE_DESC
    "inv_portid_change",
    "Invalid Port-ID between Ethernet fragments",
#endif
    NGPON_RX_INT_ISM_INV_PORTID_CHANGE_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_INV_PORTID_CHANGE_FIELD_WIDTH,
    NGPON_RX_INT_ISM_INV_PORTID_CHANGE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_FEC_FATAL_ERROR
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_FEC_FATAL_ERROR_FIELD =
{
    "FEC_FATAL_ERROR",
#if RU_INCLUDE_DESC
    "fec_fatal_error",
    "Fatal error in FEC block (check map)",
#endif
    NGPON_RX_INT_ISM_FEC_FATAL_ERROR_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_FEC_FATAL_ERROR_FIELD_WIDTH,
    NGPON_RX_INT_ISM_FEC_FATAL_ERROR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_SFC_INDICATION
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_SFC_INDICATION_FIELD =
{
    "SFC_INDICATION",
#if RU_INCLUDE_DESC
    "sfc_indication",
    "Indicates the start of the superframe set at gen.SYNC_SFC_IND",
#endif
    NGPON_RX_INT_ISM_SFC_INDICATION_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_SFC_INDICATION_FIELD_WIDTH,
    NGPON_RX_INT_ISM_SFC_INDICATION_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_AMCC_GRANT
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_AMCC_GRANT_FIELD =
{
    "AMCC_GRANT",
#if RU_INCLUDE_DESC
    "amcc_grant",
    "Reserved for AMCC grant indication",
#endif
    NGPON_RX_INT_ISM_AMCC_GRANT_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_AMCC_GRANT_FIELD_WIDTH,
    NGPON_RX_INT_ISM_AMCC_GRANT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_DWBA
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_DWBA_FIELD =
{
    "DWBA",
#if RU_INCLUDE_DESC
    "dwba",
    "Reserved for DWBA",
#endif
    NGPON_RX_INT_ISM_DWBA_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_DWBA_FIELD_WIDTH,
    NGPON_RX_INT_ISM_DWBA_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ISM_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ISM_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_INT_ISM_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_INT_ISM_RESERVED0_FIELD_WIDTH,
    NGPON_RX_INT_ISM_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_TM_FIFO_OVF_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_TM_FIFO_OVF_MASK_FIELD =
{
    "TM_FIFO_OVF_MASK",
#if RU_INCLUDE_DESC
    "tm_fifo_ovf_mask",
    "TM FIFO overflow mask",
#endif
    NGPON_RX_INT_IER_TM_FIFO_OVF_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_TM_FIFO_OVF_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_TM_FIFO_OVF_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_BW_FIFO_OVF_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_BW_FIFO_OVF_MASK_FIELD =
{
    "BW_FIFO_OVF_MASK",
#if RU_INCLUDE_DESC
    "bw_fifo_ovf_mask",
    "Access FIFO overflow mask",
#endif
    NGPON_RX_INT_IER_BW_FIFO_OVF_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_BW_FIFO_OVF_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_BW_FIFO_OVF_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_AES_FAIL_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_AES_FAIL_MASK_FIELD =
{
    "AES_FAIL_MASK",
#if RU_INCLUDE_DESC
    "aes_fail_mask",
    "AES fail to allocate error mask",
#endif
    NGPON_RX_INT_IER_AES_FAIL_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_AES_FAIL_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_AES_FAIL_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_FWI_STATE_CHANGE_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_FWI_STATE_CHANGE_MASK_FIELD =
{
    "FWI_STATE_CHANGE_MASK",
#if RU_INCLUDE_DESC
    "fwi_state_change_mask",
    "FWI state change mask",
#endif
    NGPON_RX_INT_IER_FWI_STATE_CHANGE_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_FWI_STATE_CHANGE_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_FWI_STATE_CHANGE_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_LOF_STATE_CHANGE_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_LOF_STATE_CHANGE_MASK_FIELD =
{
    "LOF_STATE_CHANGE_MASK",
#if RU_INCLUDE_DESC
    "lof_state_change_mask",
    "Acquire or loss of frame synchronization mask",
#endif
    NGPON_RX_INT_IER_LOF_STATE_CHANGE_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_LOF_STATE_CHANGE_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_LOF_STATE_CHANGE_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_LCDG_STATE_CHANGE_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_LCDG_STATE_CHANGE_MASK_FIELD =
{
    "LCDG_STATE_CHANGE_MASK",
#if RU_INCLUDE_DESC
    "lcdg_state_change_mask",
    "Assertion or negation of loss of XGEM LCDG mask",
#endif
    NGPON_RX_INT_IER_LCDG_STATE_CHANGE_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_LCDG_STATE_CHANGE_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_LCDG_STATE_CHANGE_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_LB_FIFO_OVF_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_LB_FIFO_OVF_MASK_FIELD =
{
    "LB_FIFO_OVF_MASK",
#if RU_INCLUDE_DESC
    "lb_fifo_ovf_mask",
    "LB FIFO overflow mask",
#endif
    NGPON_RX_INT_IER_LB_FIFO_OVF_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_LB_FIFO_OVF_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_LB_FIFO_OVF_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_TOD_UPDATE_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_TOD_UPDATE_MASK_FIELD =
{
    "TOD_UPDATE_MASK",
#if RU_INCLUDE_DESC
    "tod_update_mask",
    "TOD counters update mask",
#endif
    NGPON_RX_INT_IER_TOD_UPDATE_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_TOD_UPDATE_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_TOD_UPDATE_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_BW_DIS_TX_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_BW_DIS_TX_MASK_FIELD =
{
    "BW_DIS_TX_MASK",
#if RU_INCLUDE_DESC
    "bw_dis_tx_mask",
    "Bandwidth allocation to a disabled transmitter mask",
#endif
    NGPON_RX_INT_IER_BW_DIS_TX_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_BW_DIS_TX_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_BW_DIS_TX_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_INV_KEY_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_INV_KEY_MASK_FIELD =
{
    "INV_KEY_MASK",
#if RU_INCLUDE_DESC
    "inv_key_mask",
    "Incoming XGEM with a non-valid key index mask",
#endif
    NGPON_RX_INT_IER_INV_KEY_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_INV_KEY_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_INV_KEY_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_BWMAP_REC_DONE_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_BWMAP_REC_DONE_MASK_FIELD =
{
    "BWMAP_REC_DONE_MASK",
#if RU_INCLUDE_DESC
    "bwmap_rec_done_mask",
    "Bandwidth map recorder done mask",
#endif
    NGPON_RX_INT_IER_BWMAP_REC_DONE_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_BWMAP_REC_DONE_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_BWMAP_REC_DONE_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_PONID_INCONSIST_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_PONID_INCONSIST_MASK_FIELD =
{
    "PONID_INCONSIST_MASK",
#if RU_INCLUDE_DESC
    "ponid_inconsist_mask",
    "First PON-ID inconsistency mask",
#endif
    NGPON_RX_INT_IER_PONID_INCONSIST_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_PONID_INCONSIST_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_PONID_INCONSIST_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_INV_PORTID_CHANGE_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_INV_PORTID_CHANGE_MASK_FIELD =
{
    "INV_PORTID_CHANGE_MASK",
#if RU_INCLUDE_DESC
    "inv_portid_change_mask",
    "Invalid Port-ID between Ethernet fragments mask",
#endif
    NGPON_RX_INT_IER_INV_PORTID_CHANGE_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_INV_PORTID_CHANGE_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_INV_PORTID_CHANGE_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_FEC_FATAL_ERROR_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_FEC_FATAL_ERROR_MASK_FIELD =
{
    "FEC_FATAL_ERROR_MASK",
#if RU_INCLUDE_DESC
    "fec_fatal_error_mask",
    "FEC fatal error mask",
#endif
    NGPON_RX_INT_IER_FEC_FATAL_ERROR_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_FEC_FATAL_ERROR_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_FEC_FATAL_ERROR_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_SFC_INDICATION_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_SFC_INDICATION_MASK_FIELD =
{
    "SFC_INDICATION_MASK",
#if RU_INCLUDE_DESC
    "sfc_indication_mask",
    "Mask of indication of the start of the superframe set at gen.SYNC_SFC_IND",
#endif
    NGPON_RX_INT_IER_SFC_INDICATION_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_SFC_INDICATION_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_SFC_INDICATION_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_AMCC_GRANT_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_AMCC_GRANT_MASK_FIELD =
{
    "AMCC_GRANT_MASK",
#if RU_INCLUDE_DESC
    "amcc_grant_mask",
    "Mask of AMCC grant indication",
#endif
    NGPON_RX_INT_IER_AMCC_GRANT_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_AMCC_GRANT_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_AMCC_GRANT_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_DWBA_MASK
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_DWBA_MASK_FIELD =
{
    "DWBA_MASK",
#if RU_INCLUDE_DESC
    "dwba_mask",
    "Reserved for DWBA",
#endif
    NGPON_RX_INT_IER_DWBA_MASK_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_DWBA_MASK_FIELD_WIDTH,
    NGPON_RX_INT_IER_DWBA_MASK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_IER_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_IER_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_INT_IER_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_INT_IER_RESERVED0_FIELD_WIDTH,
    NGPON_RX_INT_IER_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_TM_FIFO_OVF_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_TM_FIFO_OVF_TEST_FIELD =
{
    "TM_FIFO_OVF_TEST",
#if RU_INCLUDE_DESC
    "tm_fifo_ovf_test",
    "TM FIFO overflow test",
#endif
    NGPON_RX_INT_ITR_TM_FIFO_OVF_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_TM_FIFO_OVF_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_TM_FIFO_OVF_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_BW_FIFO_OVF_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_BW_FIFO_OVF_TEST_FIELD =
{
    "BW_FIFO_OVF_TEST",
#if RU_INCLUDE_DESC
    "bw_fifo_ovf_test",
    "Access FIFO overflow test",
#endif
    NGPON_RX_INT_ITR_BW_FIFO_OVF_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_BW_FIFO_OVF_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_BW_FIFO_OVF_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_AES_FAIL_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_AES_FAIL_TEST_FIELD =
{
    "AES_FAIL_TEST",
#if RU_INCLUDE_DESC
    "aes_fail_test",
    "AES fail to allocate error test",
#endif
    NGPON_RX_INT_ITR_AES_FAIL_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_AES_FAIL_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_AES_FAIL_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_FWI_STATE_CHANGE_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_FWI_STATE_CHANGE_TEST_FIELD =
{
    "FWI_STATE_CHANGE_TEST",
#if RU_INCLUDE_DESC
    "fwi_state_change_test",
    "FWI state change test",
#endif
    NGPON_RX_INT_ITR_FWI_STATE_CHANGE_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_FWI_STATE_CHANGE_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_FWI_STATE_CHANGE_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_LOF_STATE_CHANGE_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_LOF_STATE_CHANGE_TEST_FIELD =
{
    "LOF_STATE_CHANGE_TEST",
#if RU_INCLUDE_DESC
    "lof_state_change_test",
    "Acquire or loss of frame synchronization test",
#endif
    NGPON_RX_INT_ITR_LOF_STATE_CHANGE_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_LOF_STATE_CHANGE_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_LOF_STATE_CHANGE_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_LCDG_STATE_CHANGE_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_LCDG_STATE_CHANGE_TEST_FIELD =
{
    "LCDG_STATE_CHANGE_TEST",
#if RU_INCLUDE_DESC
    "lcdg_state_change_test",
    "Assertion or negation of loss of XGEM LCDG test",
#endif
    NGPON_RX_INT_ITR_LCDG_STATE_CHANGE_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_LCDG_STATE_CHANGE_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_LCDG_STATE_CHANGE_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_LB_FIFO_OVF_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_LB_FIFO_OVF_TEST_FIELD =
{
    "LB_FIFO_OVF_TEST",
#if RU_INCLUDE_DESC
    "lb_fifo_ovf_test",
    "LB FIFO overflow test",
#endif
    NGPON_RX_INT_ITR_LB_FIFO_OVF_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_LB_FIFO_OVF_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_LB_FIFO_OVF_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_TOD_UPDATE_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_TOD_UPDATE_TEST_FIELD =
{
    "TOD_UPDATE_TEST",
#if RU_INCLUDE_DESC
    "tod_update_test",
    "TOD counters update test",
#endif
    NGPON_RX_INT_ITR_TOD_UPDATE_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_TOD_UPDATE_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_TOD_UPDATE_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_BW_DIS_TX_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_BW_DIS_TX_TEST_FIELD =
{
    "BW_DIS_TX_TEST",
#if RU_INCLUDE_DESC
    "bw_dis_tx_test",
    "Bandwidth allocation to a disabled transmitter test",
#endif
    NGPON_RX_INT_ITR_BW_DIS_TX_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_BW_DIS_TX_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_BW_DIS_TX_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_INV_KEY_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_INV_KEY_TEST_FIELD =
{
    "INV_KEY_TEST",
#if RU_INCLUDE_DESC
    "inv_key_test",
    "Incoming XGEM with a non-valid key index test",
#endif
    NGPON_RX_INT_ITR_INV_KEY_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_INV_KEY_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_INV_KEY_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_BWMAP_REC_DONE_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_BWMAP_REC_DONE_TEST_FIELD =
{
    "BWMAP_REC_DONE_TEST",
#if RU_INCLUDE_DESC
    "bwmap_rec_done_test",
    "Bandwidth map recorder done test",
#endif
    NGPON_RX_INT_ITR_BWMAP_REC_DONE_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_BWMAP_REC_DONE_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_BWMAP_REC_DONE_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_PONID_INCONSIST_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_PONID_INCONSIST_TEST_FIELD =
{
    "PONID_INCONSIST_TEST",
#if RU_INCLUDE_DESC
    "ponid_inconsist_test",
    "First PON-ID inconsistency test",
#endif
    NGPON_RX_INT_ITR_PONID_INCONSIST_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_PONID_INCONSIST_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_PONID_INCONSIST_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_INV_PORTID_CHANGE_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_INV_PORTID_CHANGE_TEST_FIELD =
{
    "INV_PORTID_CHANGE_TEST",
#if RU_INCLUDE_DESC
    "inv_portid_change_test",
    "Invalid Port-ID between Ethernet fragments test",
#endif
    NGPON_RX_INT_ITR_INV_PORTID_CHANGE_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_INV_PORTID_CHANGE_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_INV_PORTID_CHANGE_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_FEC_FATAL_ERROR_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_FEC_FATAL_ERROR_TEST_FIELD =
{
    "FEC_FATAL_ERROR_TEST",
#if RU_INCLUDE_DESC
    "fec_fatal_error_test",
    "FEC fatal error test",
#endif
    NGPON_RX_INT_ITR_FEC_FATAL_ERROR_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_FEC_FATAL_ERROR_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_FEC_FATAL_ERROR_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_SFC_INDICATION_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_SFC_INDICATION_TEST_FIELD =
{
    "SFC_INDICATION_TEST",
#if RU_INCLUDE_DESC
    "sfc_indication_test",
    "Test of indication of the start of the superframe set at gen.SYNC_SFC_IND",
#endif
    NGPON_RX_INT_ITR_SFC_INDICATION_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_SFC_INDICATION_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_SFC_INDICATION_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_AMCC_GRANT_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_AMCC_GRANT_TEST_FIELD =
{
    "AMCC_GRANT_TEST",
#if RU_INCLUDE_DESC
    "amcc_grant_test",
    "Test of AMCC grant indication",
#endif
    NGPON_RX_INT_ITR_AMCC_GRANT_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_AMCC_GRANT_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_AMCC_GRANT_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_DWBA_TEST
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_DWBA_TEST_FIELD =
{
    "DWBA_TEST",
#if RU_INCLUDE_DESC
    "dwba_test",
    "Reserved for DWBA",
#endif
    NGPON_RX_INT_ITR_DWBA_TEST_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_DWBA_TEST_FIELD_WIDTH,
    NGPON_RX_INT_ITR_DWBA_TEST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_RX_INT_ITR_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_INT_ITR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_INT_ITR_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_INT_ITR_RESERVED0_FIELD_WIDTH,
    NGPON_RX_INT_ITR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_INT_ISR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_INT_ISR_FIELDS[] =
{
    &NGPON_RX_INT_ISR_TM_FIFO_OVF_FIELD,
    &NGPON_RX_INT_ISR_BW_FIFO_OVF_FIELD,
    &NGPON_RX_INT_ISR_AES_FAIL_FIELD,
    &NGPON_RX_INT_ISR_FWI_STATE_CHANGE_FIELD,
    &NGPON_RX_INT_ISR_LOF_STATE_CHANGE_FIELD,
    &NGPON_RX_INT_ISR_LCDG_STATE_CHANGE_FIELD,
    &NGPON_RX_INT_ISR_LB_FIFO_OVF_FIELD,
    &NGPON_RX_INT_ISR_TOD_UPDATE_FIELD,
    &NGPON_RX_INT_ISR_BW_DIS_TX_FIELD,
    &NGPON_RX_INT_ISR_INV_KEY_FIELD,
    &NGPON_RX_INT_ISR_BWMAP_REC_DONE_FIELD,
    &NGPON_RX_INT_ISR_PONID_INCONSIST_FIELD,
    &NGPON_RX_INT_ISR_INV_PORTID_CHANGE_FIELD,
    &NGPON_RX_INT_ISR_FEC_FATAL_ERROR_FIELD,
    &NGPON_RX_INT_ISR_SFC_INDICATION_FIELD,
    &NGPON_RX_INT_ISR_AMCC_GRANT_FIELD,
    &NGPON_RX_INT_ISR_DWBA_FIELD,
    &NGPON_RX_INT_ISR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_INT_ISR_REG = 
{
    "ISR",
#if RU_INCLUDE_DESC
    "XGRX_ISR Register",
    "Interrupt status register. The interrupt status bit remains set until written 1 to clear by SW. Writing 0 to any bit has no effect.",
#endif
    NGPON_RX_INT_ISR_REG_OFFSET,
    0,
    0,
    15,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    18,
    NGPON_RX_INT_ISR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_INT_ISM
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_INT_ISM_FIELDS[] =
{
    &NGPON_RX_INT_ISM_TM_FIFO_OVF_FIELD,
    &NGPON_RX_INT_ISM_BW_FIFO_OVF_FIELD,
    &NGPON_RX_INT_ISM_AES_FAIL_FIELD,
    &NGPON_RX_INT_ISM_FWI_STATE_CHANGE_FIELD,
    &NGPON_RX_INT_ISM_LOF_STATE_CHANGE_FIELD,
    &NGPON_RX_INT_ISM_LCDG_STATE_CHANGE_FIELD,
    &NGPON_RX_INT_ISM_LB_FIFO_OVF_FIELD,
    &NGPON_RX_INT_ISM_TOD_UPDATE_FIELD,
    &NGPON_RX_INT_ISM_BW_DIS_TX_FIELD,
    &NGPON_RX_INT_ISM_INV_KEY_FIELD,
    &NGPON_RX_INT_ISM_BWMAP_REC_DONE_FIELD,
    &NGPON_RX_INT_ISM_PONID_INCONSIST_FIELD,
    &NGPON_RX_INT_ISM_INV_PORTID_CHANGE_FIELD,
    &NGPON_RX_INT_ISM_FEC_FATAL_ERROR_FIELD,
    &NGPON_RX_INT_ISM_SFC_INDICATION_FIELD,
    &NGPON_RX_INT_ISM_AMCC_GRANT_FIELD,
    &NGPON_RX_INT_ISM_DWBA_FIELD,
    &NGPON_RX_INT_ISM_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_INT_ISM_REG = 
{
    "ISM",
#if RU_INCLUDE_DESC
    "XGRX_ISM Register",
    "Interrupt status after masking register.",
#endif
    NGPON_RX_INT_ISM_REG_OFFSET,
    0,
    0,
    16,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    18,
    NGPON_RX_INT_ISM_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_INT_IER
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_INT_IER_FIELDS[] =
{
    &NGPON_RX_INT_IER_TM_FIFO_OVF_MASK_FIELD,
    &NGPON_RX_INT_IER_BW_FIFO_OVF_MASK_FIELD,
    &NGPON_RX_INT_IER_AES_FAIL_MASK_FIELD,
    &NGPON_RX_INT_IER_FWI_STATE_CHANGE_MASK_FIELD,
    &NGPON_RX_INT_IER_LOF_STATE_CHANGE_MASK_FIELD,
    &NGPON_RX_INT_IER_LCDG_STATE_CHANGE_MASK_FIELD,
    &NGPON_RX_INT_IER_LB_FIFO_OVF_MASK_FIELD,
    &NGPON_RX_INT_IER_TOD_UPDATE_MASK_FIELD,
    &NGPON_RX_INT_IER_BW_DIS_TX_MASK_FIELD,
    &NGPON_RX_INT_IER_INV_KEY_MASK_FIELD,
    &NGPON_RX_INT_IER_BWMAP_REC_DONE_MASK_FIELD,
    &NGPON_RX_INT_IER_PONID_INCONSIST_MASK_FIELD,
    &NGPON_RX_INT_IER_INV_PORTID_CHANGE_MASK_FIELD,
    &NGPON_RX_INT_IER_FEC_FATAL_ERROR_MASK_FIELD,
    &NGPON_RX_INT_IER_SFC_INDICATION_MASK_FIELD,
    &NGPON_RX_INT_IER_AMCC_GRANT_MASK_FIELD,
    &NGPON_RX_INT_IER_DWBA_MASK_FIELD,
    &NGPON_RX_INT_IER_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_INT_IER_REG = 
{
    "IER",
#if RU_INCLUDE_DESC
    "XGRX_IER Register",
    "This register provides an enable mask for each of the interrupt sources in XGRXISR. It can be updated on-the-fly.",
#endif
    NGPON_RX_INT_IER_REG_OFFSET,
    0,
    0,
    17,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    18,
    NGPON_RX_INT_IER_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_INT_ITR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_INT_ITR_FIELDS[] =
{
    &NGPON_RX_INT_ITR_TM_FIFO_OVF_TEST_FIELD,
    &NGPON_RX_INT_ITR_BW_FIFO_OVF_TEST_FIELD,
    &NGPON_RX_INT_ITR_AES_FAIL_TEST_FIELD,
    &NGPON_RX_INT_ITR_FWI_STATE_CHANGE_TEST_FIELD,
    &NGPON_RX_INT_ITR_LOF_STATE_CHANGE_TEST_FIELD,
    &NGPON_RX_INT_ITR_LCDG_STATE_CHANGE_TEST_FIELD,
    &NGPON_RX_INT_ITR_LB_FIFO_OVF_TEST_FIELD,
    &NGPON_RX_INT_ITR_TOD_UPDATE_TEST_FIELD,
    &NGPON_RX_INT_ITR_BW_DIS_TX_TEST_FIELD,
    &NGPON_RX_INT_ITR_INV_KEY_TEST_FIELD,
    &NGPON_RX_INT_ITR_BWMAP_REC_DONE_TEST_FIELD,
    &NGPON_RX_INT_ITR_PONID_INCONSIST_TEST_FIELD,
    &NGPON_RX_INT_ITR_INV_PORTID_CHANGE_TEST_FIELD,
    &NGPON_RX_INT_ITR_FEC_FATAL_ERROR_TEST_FIELD,
    &NGPON_RX_INT_ITR_SFC_INDICATION_TEST_FIELD,
    &NGPON_RX_INT_ITR_AMCC_GRANT_TEST_FIELD,
    &NGPON_RX_INT_ITR_DWBA_TEST_FIELD,
    &NGPON_RX_INT_ITR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_INT_ITR_REG = 
{
    "ITR",
#if RU_INCLUDE_DESC
    "XGRX_ITR Register",
    "This write-only register enables  interrupt testing by setting its different sources. Writing 1 to a bit in this register sets the same bit in XGRXISR. Read always returns 0.",
#endif
    NGPON_RX_INT_ITR_REG_OFFSET,
    0,
    0,
    18,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    18,
    NGPON_RX_INT_ITR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_INT
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_INT_REGS[] =
{
    &NGPON_RX_INT_ISR_REG,
    &NGPON_RX_INT_ISM_REG,
    &NGPON_RX_INT_IER_REG,
    &NGPON_RX_INT_ITR_REG,
};

unsigned long NGPON_RX_INT_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80160080,
#elif defined(CONFIG_BCM96856)
    0x82db4080,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_INT_BLOCK = 
{
    "NGPON_RX_INT",
    NGPON_RX_INT_ADDRS,
    1,
    4,
    NGPON_RX_INT_REGS
};

/* End of file BCM6858_A0_NGPON_RX_INT.c */
