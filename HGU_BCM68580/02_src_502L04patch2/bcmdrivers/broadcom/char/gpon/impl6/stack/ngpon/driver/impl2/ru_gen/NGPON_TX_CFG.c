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
 * Field: NGPON_TX_CFG_ONU_TX_EN_TRANSMITTER_ENABLE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_TX_EN_TRANSMITTER_ENABLE_FIELD =
{
    "TRANSMITTER_ENABLE",
#if RU_INCLUDE_DESC
    "Transmitter_enable",
    "Enable the accesses transmission and TX FIFO operation. If the flag is deactivated during operation, then the  transmittor stops transmissions, flushes awaiting accesses, and TX FIFO queues.",
#endif
    NGPON_TX_CFG_ONU_TX_EN_TRANSMITTER_ENABLE_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_TX_EN_TRANSMITTER_ENABLE_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_TX_EN_TRANSMITTER_ENABLE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ONU_TX_EN_STANDARD
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_TX_EN_STANDARD_FIELD =
{
    "STANDARD",
#if RU_INCLUDE_DESC
    "Standard",
    "0 - XGPON"
    "1 - NGPON2",
#endif
    NGPON_TX_CFG_ONU_TX_EN_STANDARD_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_TX_EN_STANDARD_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_TX_EN_STANDARD_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ONU_TX_EN_SPEED
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_TX_EN_SPEED_FIELD =
{
    "SPEED",
#if RU_INCLUDE_DESC
    "Speed",
    "0 - 2.5G"
    "1 - 10 G",
#endif
    NGPON_TX_CFG_ONU_TX_EN_SPEED_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_TX_EN_SPEED_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_TX_EN_SPEED_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ONU_TX_EN_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_TX_EN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_ONU_TX_EN_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_TX_EN_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_TX_EN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FUNC_EN_SCRMBLR_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FUNC_EN_SCRMBLR_EN_FIELD =
{
    "SCRMBLR_EN",
#if RU_INCLUDE_DESC
    "Scrambler_enable",
    "Enable the upstream scrambler operation. This register can be updated only when TX is disabled.",
#endif
    NGPON_TX_CFG_FUNC_EN_SCRMBLR_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_FUNC_EN_SCRMBLR_EN_FIELD_WIDTH,
    NGPON_TX_CFG_FUNC_EN_SCRMBLR_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FUNC_EN_BIP_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FUNC_EN_BIP_EN_FIELD =
{
    "BIP_EN",
#if RU_INCLUDE_DESC
    "BIP32_enable",
    "Enable BIP32 generation for upstream traffic. This register can be updated only when TX is disabled.",
#endif
    NGPON_TX_CFG_FUNC_EN_BIP_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_FUNC_EN_BIP_EN_FIELD_WIDTH,
    NGPON_TX_CFG_FUNC_EN_BIP_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FUNC_EN_FEC_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FUNC_EN_FEC_EN_FIELD =
{
    "FEC_EN",
#if RU_INCLUDE_DESC
    "FEC_enable",
    "Enable the upstream FEC encoder unit operation. FEC will be applied to the upstream burst only if the FEC_enable is set and the FEC is also enabled by the selected profile.This register can be updated only when TX is disabled.",
#endif
    NGPON_TX_CFG_FUNC_EN_FEC_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_FUNC_EN_FEC_EN_FIELD_WIDTH,
    NGPON_TX_CFG_FUNC_EN_FEC_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FUNC_EN_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FUNC_EN_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "Loop_back_enable",
    "Enable RX-TX loopback operation."
    ""
    "This register can be updated if TX is disabled or if the queue assign for the loopback is unused. The SW should configured the loopback queue in the ONU-TX and then also in the ONU-RX.",
#endif
    NGPON_TX_CFG_FUNC_EN_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_CFG_FUNC_EN_RESERVED1_FIELD_WIDTH,
    NGPON_TX_CFG_FUNC_EN_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FUNC_EN_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FUNC_EN_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "Misc_transmission_enable",
    "Enable TX independent transmission according to access configured by host. This register can be updated only when TX is disabled. For more information regarding misc accesses, refer to registers MISC_ALLOC_CFG_0 and MISC_ALLOC_CFG_1",
#endif
    NGPON_TX_CFG_FUNC_EN_RESERVED2_FIELD_MASK,
    0,
    NGPON_TX_CFG_FUNC_EN_RESERVED2_FIELD_WIDTH,
    NGPON_TX_CFG_FUNC_EN_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FUNC_EN_RESERVED3
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FUNC_EN_RESERVED3_FIELD =
{
    "RESERVED3",
#if RU_INCLUDE_DESC
    "Misc_data_enable",
    "Enable TX independent data according to access configured by host. This register can be updated only when TX is disabled. For more information regarding misc data, refer to registers MISC_DATA_CFG abd MISC_PD_CFG",
#endif
    NGPON_TX_CFG_FUNC_EN_RESERVED3_FIELD_MASK,
    0,
    NGPON_TX_CFG_FUNC_EN_RESERVED3_FIELD_WIDTH,
    NGPON_TX_CFG_FUNC_EN_RESERVED3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FUNC_EN_ENC_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FUNC_EN_ENC_EN_FIELD =
{
    "ENC_EN",
#if RU_INCLUDE_DESC
    "encrypt_enable",
    "If this bit isnt reset then encryption is applied according to the enc bit in the Packet descriptor.",
#endif
    NGPON_TX_CFG_FUNC_EN_ENC_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_FUNC_EN_ENC_EN_FIELD_WIDTH,
    NGPON_TX_CFG_FUNC_EN_ENC_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FUNC_EN_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FUNC_EN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_FUNC_EN_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_FUNC_EN_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_FUNC_EN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};


/******************************************************************************
 * Field: NGPON_TX_CFG_FUNC_EN_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FUNC_EN_S_FIELD =
{
    "S",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_FUNC_EN_S_FIELD_MASK,
    0,
    NGPON_TX_CFG_FUNC_EN_S_FIELD_WIDTH,
    NGPON_TX_CFG_FUNC_EN_S_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};


/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_POL_TX_DATA_OUT_POLARITY
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_POL_TX_DATA_OUT_POLARITY_FIELD =
{
    "TX_DATA_OUT_POLARITY",
#if RU_INCLUDE_DESC
    "TX_data_out_polarity",
    "When this bit is asserted, the output data bus is bitwise inverted",
#endif
    NGPON_TX_CFG_DAT_POL_TX_DATA_OUT_POLARITY_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_POL_TX_DATA_OUT_POLARITY_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_POL_TX_DATA_OUT_POLARITY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_POL_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_POL_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_DAT_POL_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_POL_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_POL_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_RNG_DLY_TX_WORD_DELAY
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_RNG_DLY_TX_WORD_DELAY_FIELD =
{
    "TX_WORD_DELAY",
#if RU_INCLUDE_DESC
    "Tx_Word_Delay",
    "Number of words delay between RX to TX frames",
#endif
    NGPON_TX_CFG_TX_RNG_DLY_TX_WORD_DELAY_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_RNG_DLY_TX_WORD_DELAY_FIELD_WIDTH,
    NGPON_TX_CFG_TX_RNG_DLY_TX_WORD_DELAY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_RNG_DLY_TX_FRAME_DELAY
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_RNG_DLY_TX_FRAME_DELAY_FIELD =
{
    "TX_FRAME_DELAY",
#if RU_INCLUDE_DESC
    "Tx_Frame_Delay",
    "Number of frames delay between RX to TX frames",
#endif
    NGPON_TX_CFG_TX_RNG_DLY_TX_FRAME_DELAY_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_RNG_DLY_TX_FRAME_DELAY_FIELD_WIDTH,
    NGPON_TX_CFG_TX_RNG_DLY_TX_FRAME_DELAY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_RNG_DLY_TX_BIT_DELAY
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_RNG_DLY_TX_BIT_DELAY_FIELD =
{
    "TX_BIT_DELAY",
#if RU_INCLUDE_DESC
    "Tx_Bit_Delay",
    "Number of bits delay between RX to TX frames",
#endif
    NGPON_TX_CFG_TX_RNG_DLY_TX_BIT_DELAY_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_RNG_DLY_TX_BIT_DELAY_FIELD_WIDTH,
    NGPON_TX_CFG_TX_RNG_DLY_TX_BIT_DELAY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_RNG_DLY_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_RNG_DLY_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TX_RNG_DLY_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_RNG_DLY_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_TX_RNG_DLY_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DV_CFG_DV_POL
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DV_CFG_DV_POL_FIELD =
{
    "DV_POL",
#if RU_INCLUDE_DESC
    "DV_polarity",
    "DV output polarity. This bit determines whether the bit pattern specified for DV signal is inverted or not.",
#endif
    NGPON_TX_CFG_DV_CFG_DV_POL_FIELD_MASK,
    0,
    NGPON_TX_CFG_DV_CFG_DV_POL_FIELD_WIDTH,
    NGPON_TX_CFG_DV_CFG_DV_POL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DV_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DV_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_DV_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_DV_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_DV_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DV_CFG_DV_SETUP_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DV_CFG_DV_SETUP_LEN_FIELD =
{
    "DV_SETUP_LEN",
#if RU_INCLUDE_DESC
    "DV_setup_length",
    "length of the DV setup in words."
    "The DV during the setup period is composed of (DV_SETUP_LEN-1) DV words of DV_POL followed by one DV setup word is according to the DV_HOLD_PAT_SRC and DV_POL configuartion.",
#endif
    NGPON_TX_CFG_DV_CFG_DV_SETUP_LEN_FIELD_MASK,
    0,
    NGPON_TX_CFG_DV_CFG_DV_SETUP_LEN_FIELD_WIDTH,
    NGPON_TX_CFG_DV_CFG_DV_SETUP_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DV_CFG_DV_HOLD_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DV_CFG_DV_HOLD_LEN_FIELD =
{
    "DV_HOLD_LEN",
#if RU_INCLUDE_DESC
    "DV_hold_length",
    "length of the DV hold in words."
    "The first DV word is according to the DV_HOLD_PAT_SRC and DV_POL and the (DV_HOLD_LEN-1) following DV words are according to the DV_POL.",
#endif
    NGPON_TX_CFG_DV_CFG_DV_HOLD_LEN_FIELD_MASK,
    0,
    NGPON_TX_CFG_DV_CFG_DV_HOLD_LEN_FIELD_WIDTH,
    NGPON_TX_CFG_DV_CFG_DV_HOLD_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DV_CFG_DV_SETUP_PAT_SRC
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DV_CFG_DV_SETUP_PAT_SRC_FIELD =
{
    "DV_SETUP_PAT_SRC",
#if RU_INCLUDE_DESC
    "DV_setup_pattern_source",
    "Select the DV pattern from the DV_SETUP_PAT register and according to the polarity configuration or only acording to the DV_polarity",
#endif
    NGPON_TX_CFG_DV_CFG_DV_SETUP_PAT_SRC_FIELD_MASK,
    0,
    NGPON_TX_CFG_DV_CFG_DV_SETUP_PAT_SRC_FIELD_WIDTH,
    NGPON_TX_CFG_DV_CFG_DV_SETUP_PAT_SRC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DV_CFG_DV_HOLD_PAT_SRC
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DV_CFG_DV_HOLD_PAT_SRC_FIELD =
{
    "DV_HOLD_PAT_SRC",
#if RU_INCLUDE_DESC
    "DV_hold_pattern_source",
    "Select the DV pattern from the DV_HOLD_PAT register and according to the polarity configuration or only acording to the DV_polarity",
#endif
    NGPON_TX_CFG_DV_CFG_DV_HOLD_PAT_SRC_FIELD_MASK,
    0,
    NGPON_TX_CFG_DV_CFG_DV_HOLD_PAT_SRC_FIELD_WIDTH,
    NGPON_TX_CFG_DV_CFG_DV_HOLD_PAT_SRC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DV_CFG_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DV_CFG_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_DV_CFG_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_CFG_DV_CFG_RESERVED1_FIELD_WIDTH,
    NGPON_TX_CFG_DV_CFG_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DV_SETUP_PAT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DV_SETUP_PAT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Pattern of DV setup bits before the PON overhead starts. This register represnts 32 DV bits and each one may be 0 - DV signal is off during this bit, 1 DV signal is on during this bit",
#endif
    NGPON_TX_CFG_DV_SETUP_PAT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_DV_SETUP_PAT_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_DV_SETUP_PAT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DV_HOLD_PAT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DV_HOLD_PAT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Pattern of DV hold bits that starts where the DATAs XGTC trailer ends. This register represnts 32 DV bits and each one may be  0 - DV signal is off during this bit, 1 DV signal is on during this bit",
#endif
    NGPON_TX_CFG_DV_HOLD_PAT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_DV_HOLD_PAT_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_DV_HOLD_PAT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_PAT_CFG_DAT_PAT_TYPE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_PAT_CFG_DAT_PAT_TYPE_FIELD =
{
    "DAT_PAT_TYPE",
#if RU_INCLUDE_DESC
    "data_patten_type",
    "Select btween data pattern of PRBS, Data from register or no data pattern.",
#endif
    NGPON_TX_CFG_DAT_PAT_CFG_DAT_PAT_TYPE_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_PAT_CFG_DAT_PAT_TYPE_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_PAT_CFG_DAT_PAT_TYPE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_PAT_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_PAT_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_DAT_PAT_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_PAT_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_PAT_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_PAT_CFG_SETUP_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_PAT_CFG_SETUP_LEN_FIELD =
{
    "SETUP_LEN",
#if RU_INCLUDE_DESC
    "Setup_length",
    "Setup pattern length (in bytes)",
#endif
    NGPON_TX_CFG_DAT_PAT_CFG_SETUP_LEN_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_PAT_CFG_SETUP_LEN_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_PAT_CFG_SETUP_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_PAT_CFG_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_PAT_CFG_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_DAT_PAT_CFG_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_PAT_CFG_RESERVED1_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_PAT_CFG_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_PAT_CFG_HOLD_LEN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_PAT_CFG_HOLD_LEN_FIELD =
{
    "HOLD_LEN",
#if RU_INCLUDE_DESC
    "Hold_length",
    "Hold pattern length",
#endif
    NGPON_TX_CFG_DAT_PAT_CFG_HOLD_LEN_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_PAT_CFG_HOLD_LEN_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_PAT_CFG_HOLD_LEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_PAT_CFG_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_PAT_CFG_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_DAT_PAT_CFG_RESERVED2_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_PAT_CFG_RESERVED2_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_PAT_CFG_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_PAT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_PAT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "pattern",
#endif
    NGPON_TX_CFG_DAT_PAT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_PAT_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_PAT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_SETUP_PAT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_SETUP_PAT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "data setup pattern",
#endif
    NGPON_TX_CFG_DAT_SETUP_PAT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_SETUP_PAT_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_SETUP_PAT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DAT_HOLD_PAT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DAT_HOLD_PAT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "data hold pattern",
#endif
    NGPON_TX_CFG_DAT_HOLD_PAT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_DAT_HOLD_PAT_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_DAT_HOLD_PAT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_ONU_ID
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_ONU_ID_FIELD =
{
    "ONU_ID",
#if RU_INCLUDE_DESC
    "ONU_ID",
    "Mapping of T-CONT to one of the three ONU-ID registers."
    "0 - First ONU-ID register"
    "1 - Second ONU-ID register"
    "2 - Third ONU-ID register",
#endif
    NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_ONU_ID_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_ONU_ID_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_ONU_ID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_FLUSH_FLSH_Q
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_FLUSH_FLSH_Q_FIELD =
{
    "FLSH_Q",
#if RU_INCLUDE_DESC
    "flush_queue",
    "The number of queue to be flushed",
#endif
    NGPON_TX_CFG_TCONT_FLUSH_FLSH_Q_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_FLUSH_FLSH_Q_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_FLUSH_FLSH_Q_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_FLUSH_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_FLUSH_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TCONT_FLUSH_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_FLUSH_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_FLUSH_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_FLUSH_FLSH_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_FLUSH_FLSH_EN_FIELD =
{
    "FLSH_EN",
#if RU_INCLUDE_DESC
    "flush_enable",
    "When asserted, the queue determined in FLSH_Q will be flushed",
#endif
    NGPON_TX_CFG_TCONT_FLUSH_FLSH_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_FLUSH_FLSH_EN_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_FLUSH_FLSH_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_FLUSH_FLSH_IMM
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_FLUSH_FLSH_IMM_FIELD =
{
    "FLSH_IMM",
#if RU_INCLUDE_DESC
    "flush_immediate",
    "When asserted, with flush valid, the flush will happen immediately without waiting for not transmitting",
#endif
    NGPON_TX_CFG_TCONT_FLUSH_FLSH_IMM_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_FLUSH_FLSH_IMM_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_FLUSH_FLSH_IMM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_FLUSH_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_FLUSH_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TCONT_FLUSH_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_FLUSH_RESERVED1_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_FLUSH_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_ONU_ID
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_ONU_ID_FIELD =
{
    "ONU_ID",
#if RU_INCLUDE_DESC
    "ONU_ID",
    "ONU-ID",
#endif
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_ONU_ID_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_ONU_ID_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_ONU_ID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_PLM_NUM
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_PLM_NUM_FIELD =
{
    "PLM_NUM",
#if RU_INCLUDE_DESC
    "PLOAM_number",
    "PLOAM number."
    "0 - Normal PLOAM buffer."
    "The rest number are flixable and use as the SW desicides.",
#endif
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_PLM_NUM_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_PLM_NUM_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_PLM_NUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED1_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_FLBK_PLM_NUM
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_FLBK_PLM_NUM_FIELD =
{
    "FLBK_PLM_NUM",
#if RU_INCLUDE_DESC
    "fallback_PLOAM_number",
    "When PLM_NUM isnt valid the FLBK_PLM_NUM will be sent",
#endif
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_FLBK_PLM_NUM_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_FLBK_PLM_NUM_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_FLBK_PLM_NUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED2_FIELD_MASK,
    0,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED2_FIELD_WIDTH,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_FIFO_GRP_CFG_BASE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_FIFO_GRP_CFG_BASE_FIELD =
{
    "BASE",
#if RU_INCLUDE_DESC
    "group_base_address",
    "base address of the group",
#endif
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_BASE_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_BASE_FIELD_WIDTH,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_BASE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_FIFO_GRP_CFG_SIZE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_FIFO_GRP_CFG_SIZE_FIELD =
{
    "SIZE",
#if RU_INCLUDE_DESC
    "group_size",
    "size of each queue in the group."
    "The size is the number of words and two LSB should be configured to zero (the resolution are 4 words)"
    "",
#endif
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_SIZE_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_SIZE_FIELD_WIDTH,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_SIZE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED1_FIELD_WIDTH,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_FIFO_Q_CFG_BASE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_FIFO_Q_CFG_BASE_FIELD =
{
    "BASE",
#if RU_INCLUDE_DESC
    "Tx_queue_base_address",
    "Offset of the TX queue within the 20Kbyte Tx FIFO.  The two LSB should be configured to zero the resolution are 4 words)",
#endif
    NGPON_TX_CFG_TX_FIFO_Q_CFG_BASE_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_BASE_FIELD_WIDTH,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_BASE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_FIFO_Q_CFG_SIZE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_FIFO_Q_CFG_SIZE_FIELD =
{
    "SIZE",
#if RU_INCLUDE_DESC
    "Tx_queue_size",
    "Size of TX queue within the 20K Tx FIFO."
    "The size is the number of words and two LSB should be configured to zero (the resolution are 4 words)"
    "",
#endif
    NGPON_TX_CFG_TX_FIFO_Q_CFG_SIZE_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_SIZE_FIELD_WIDTH,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_SIZE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED1_FIELD_WIDTH,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PD_Q_CFG_BASE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PD_Q_CFG_BASE_FIELD =
{
    "BASE",
#if RU_INCLUDE_DESC
    "Packet_descriptor_base_address_content",
    "This field contains the offset of the packet descriptor queue in the packet array. The base address of a queue must be equal or higher than the sum of all allocations to previous queues (each address represents one packet descriptor).",
#endif
    NGPON_TX_CFG_PD_Q_CFG_BASE_FIELD_MASK,
    0,
    NGPON_TX_CFG_PD_Q_CFG_BASE_FIELD_WIDTH,
    NGPON_TX_CFG_PD_Q_CFG_BASE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PD_Q_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PD_Q_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_PD_Q_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_PD_Q_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_PD_Q_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PD_Q_CFG_SIZE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PD_Q_CFG_SIZE_FIELD =
{
    "SIZE",
#if RU_INCLUDE_DESC
    "Packet_descriptor_size_content",
    "This field contains the number of packet descriptors allocated to the queue. (out of 128 packet descriptors in total)",
#endif
    NGPON_TX_CFG_PD_Q_CFG_SIZE_FIELD_MASK,
    0,
    NGPON_TX_CFG_PD_Q_CFG_SIZE_FIELD_WIDTH,
    NGPON_TX_CFG_PD_Q_CFG_SIZE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PD_Q_CFG_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PD_Q_CFG_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_PD_Q_CFG_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_CFG_PD_Q_CFG_RESERVED1_FIELD_WIDTH,
    NGPON_TX_CFG_PD_Q_CFG_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PD_GRP_CFG_BASE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PD_GRP_CFG_BASE_FIELD =
{
    "BASE",
#if RU_INCLUDE_DESC
    "base_address_of_the_group",
    "base address of the group",
#endif
    NGPON_TX_CFG_PD_GRP_CFG_BASE_FIELD_MASK,
    0,
    NGPON_TX_CFG_PD_GRP_CFG_BASE_FIELD_WIDTH,
    NGPON_TX_CFG_PD_GRP_CFG_BASE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PD_GRP_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PD_GRP_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_PD_GRP_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_PD_GRP_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_PD_GRP_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DBR_CFG_DBR_SRC
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DBR_CFG_DBR_SRC_FIELD =
{
    "DBR_SRC",
#if RU_INCLUDE_DESC
    "DBR_source",
    "When this bit is set, the SW will write to the DBR register array."
    "When this bit is reset, the HW will generate the DBR report by accumulating the data in the FE buffer, the data in the BBH and the data in the Runners queues as received in the Runner DBR reports."
    ""
    "If not set, the DBR generation should be done by SW",
#endif
    NGPON_TX_CFG_DBR_CFG_DBR_SRC_FIELD_MASK,
    0,
    NGPON_TX_CFG_DBR_CFG_DBR_SRC_FIELD_WIDTH,
    NGPON_TX_CFG_DBR_CFG_DBR_SRC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DBR_CFG_DBR_FLUSH
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DBR_CFG_DBR_FLUSH_FIELD =
{
    "DBR_FLUSH",
#if RU_INCLUDE_DESC
    "DBR_flush",
    "When set, the dbr counters are set to zero (only the counters of the ONU-TX tranmitission)",
#endif
    NGPON_TX_CFG_DBR_CFG_DBR_FLUSH_FIELD_MASK,
    0,
    NGPON_TX_CFG_DBR_CFG_DBR_FLUSH_FIELD_WIDTH,
    NGPON_TX_CFG_DBR_CFG_DBR_FLUSH_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DBR_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DBR_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_DBR_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_DBR_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_DBR_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DBR_BUFOCC
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DBR_BUFOCC_FIELD =
{
    "BUFOCC",
#if RU_INCLUDE_DESC
    "BufOcc",
    "The Buffer Occupancy (BufOcc) field is 3 bytes long and contains the total amount of SDU traffic, expressed in units of 4-byte words, aggregated across all the buffers associated with the Alloc-ID",
#endif
    NGPON_TX_CFG_DBR_BUFOCC_FIELD_MASK,
    0,
    NGPON_TX_CFG_DBR_BUFOCC_FIELD_WIDTH,
    NGPON_TX_CFG_DBR_BUFOCC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DBR_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DBR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_DBR_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_DBR_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_DBR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "The smallest value supported is 8 bytes. This is the configuration of the minimum XGEM payload length in bytes. When the data length available for transmission is smaller then the register value, an idle XGEM frame will be sent. An exception is if the available data for transmission is bigger then the configuration and the available space left in the allocation is smaller then the configuration. In this case the available BW will be used and an XGEM smaller then this configuration will be generated.",
#endif
    NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_0_Q_NUM
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_0_Q_NUM_FIELD =
{
    "GRP_0_Q_NUM",
#if RU_INCLUDE_DESC
    "TCONT_number_for_group_0",
    "TCONT number for counter group 0",
#endif
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_0_Q_NUM_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_0_Q_NUM_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_0_Q_NUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_1_Q_NUM
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_1_Q_NUM_FIELD =
{
    "GRP_1_Q_NUM",
#if RU_INCLUDE_DESC
    "TCONT_number_for_group_1",
    "TCONT number for counter group 1",
#endif
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_1_Q_NUM_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_1_Q_NUM_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_1_Q_NUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED1_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED1_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_2_Q_NUM
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_2_Q_NUM_FIELD =
{
    "GRP_2_Q_NUM",
#if RU_INCLUDE_DESC
    "TCONT_number_for_group_2",
    "TCONT number for counter group 2",
#endif
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_2_Q_NUM_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_2_Q_NUM_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_2_Q_NUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED2
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED2_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED2_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_3_Q_NUM
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_3_Q_NUM_FIELD =
{
    "GRP_3_Q_NUM",
#if RU_INCLUDE_DESC
    "TCONT_number_for_group_3",
    "TCONT number for counter group 3",
#endif
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_3_Q_NUM_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_3_Q_NUM_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_3_Q_NUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED3
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED3_FIELD =
{
    "RESERVED3",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED3_FIELD_MASK,
    0,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED3_FIELD_WIDTH,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_CFG_PRSSTNT
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_CFG_PRSSTNT_FIELD =
{
    "PRSSTNT",
#if RU_INCLUDE_DESC
    "persistent",
    "Configuration whether the PLOAM is persistent."
    "A Persistent PLOAM which is pointed directly (not choosen as a fallback PLOAM) is retransmited as long as it is valid. If it is used as a fallback PLOAM, then its validity isnt check."
    "If a persistent PLOAM is valid, tt stays valid after it is sent. It is not valid only if the SW reset its validity.",
#endif
    NGPON_TX_CFG_PLM_CFG_PRSSTNT_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_CFG_PRSSTNT_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_CFG_PRSSTNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_CFG_VALID
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_CFG_VALID_FIELD =
{
    "VALID",
#if RU_INCLUDE_DESC
    "valid",
    "HW Indication whether a PLOAM is valid."
    "For the first register in the array, PLOAM_CFG of PLOAM 0 FIFO, the valid is set as long as the FIFO isnt empty."
    "For the rest of the PLOAM buffers, the SW set/clear this bit and the HW clears it for a non persistent PLOAM that was transmitted.",
#endif
    NGPON_TX_CFG_PLM_CFG_VALID_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_CFG_VALID_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_CFG_VALID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_CFG_SW_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_CFG_SW_EN_FIELD =
{
    "SW_EN",
#if RU_INCLUDE_DESC
    "software_enable",
    "The SW use this bit to indicate to the HW that it wrote the coplete PLOAM. This bit is only used for PLOAM 0.By setting this bit the SW indicate to the HW that a new PLOAM 0 entry is ready and should be written to the PLOAM 0 FIFO.",
#endif
    NGPON_TX_CFG_PLM_CFG_SW_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_CFG_SW_EN_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_CFG_SW_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_CFG_USE_DEF
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_CFG_USE_DEF_FIELD =
{
    "USE_DEF",
#if RU_INCLUDE_DESC
    "use_default",
    "use this PLOAM as default",
#endif
    NGPON_TX_CFG_PLM_CFG_USE_DEF_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_CFG_USE_DEF_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_CFG_USE_DEF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_PLM_CFG_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_CFG_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_LEVEL_CLR
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_LEVEL_CLR_FIELD =
{
    "ROGUE_LEVEL_CLR",
#if RU_INCLUDE_DESC
    "rogue_level_clear",
    "clears the dv level counter",
#endif
    NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_LEVEL_CLR_FIELD_MASK,
    0,
    NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_LEVEL_CLR_FIELD_WIDTH,
    NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_LEVEL_CLR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DIFF_CLR
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DIFF_CLR_FIELD =
{
    "ROGUE_DIFF_CLR",
#if RU_INCLUDE_DESC
    "rogue_diff_clear",
    "clears the dv diff counter",
#endif
    NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DIFF_CLR_FIELD_MASK,
    0,
    NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DIFF_CLR_FIELD_WIDTH,
    NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DIFF_CLR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DV_SEL
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DV_SEL_FIELD =
{
    "ROGUE_DV_SEL",
#if RU_INCLUDE_DESC
    "rogue_dv_sel",
    "selects the source of the examined dv. 1 - from SerDes, 0 - from I/O pad",
#endif
    NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DV_SEL_FIELD_MASK,
    0,
    NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DV_SEL_FIELD_WIDTH,
    NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DV_SEL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ROGUE_ONU_CTRL_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ROGUE_ONU_CTRL_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_ROGUE_ONU_CTRL_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_ROGUE_ONU_CTRL_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_ROGUE_ONU_CTRL_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ROGUE_LEVEL_TIME_WINDOW_SIZE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD =
{
    "WINDOW_SIZE",
#if RU_INCLUDE_DESC
    "window_size",
    "window_size in clock cycles",
#endif
    NGPON_TX_CFG_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD_MASK,
    0,
    NGPON_TX_CFG_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD_WIDTH,
    NGPON_TX_CFG_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ROGUE_DIFF_TIME_WINDOW_SIZE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD =
{
    "WINDOW_SIZE",
#if RU_INCLUDE_DESC
    "window_size",
    "window_size in clock cycles",
#endif
    NGPON_TX_CFG_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD_MASK,
    0,
    NGPON_TX_CFG_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD_WIDTH,
    NGPON_TX_CFG_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_ROGUE_DIFF_TIME_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_ROGUE_DIFF_TIME_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_ROGUE_DIFF_TIME_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_ROGUE_DIFF_TIME_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_ROGUE_DIFF_TIME_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_LPB_Q_NUM_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_LPB_Q_NUM_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Mapping of LoopBack to queue number",
#endif
    NGPON_TX_CFG_LPB_Q_NUM_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_LPB_Q_NUM_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_LPB_Q_NUM_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_LPB_Q_NUM_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_LPB_Q_NUM_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_LPB_Q_NUM_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_LPB_Q_NUM_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_LPB_Q_NUM_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_XGEM_HDR_OPT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_XGEM_HDR_OPT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Value",
#endif
    NGPON_TX_CFG_XGEM_HDR_OPT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_XGEM_HDR_OPT_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_XGEM_HDR_OPT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_XGEM_HDR_OPT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_XGEM_HDR_OPT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_XGEM_HDR_OPT_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_XGEM_HDR_OPT_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_XGEM_HDR_OPT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_0_STAT_OCCUPY
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_0_STAT_OCCUPY_FIELD =
{
    "OCCUPY",
#if RU_INCLUDE_DESC
    "OCCUPIED",
    "The number of valid PLOAMs entries in the PLOAM 0 FIFO",
#endif
    NGPON_TX_CFG_PLM_0_STAT_OCCUPY_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_0_STAT_OCCUPY_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_0_STAT_OCCUPY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_0_STAT_RD_ADR
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_0_STAT_RD_ADR_FIELD =
{
    "RD_ADR",
#if RU_INCLUDE_DESC
    "rd_adress",
    "The address that then next PLOAM0 will be read from",
#endif
    NGPON_TX_CFG_PLM_0_STAT_RD_ADR_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_0_STAT_RD_ADR_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_0_STAT_RD_ADR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_0_STAT_WR_ADR
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_0_STAT_WR_ADR_FIELD =
{
    "WR_ADR",
#if RU_INCLUDE_DESC
    "wr_adress",
    "The address that then next PLOAM0 will be written to",
#endif
    NGPON_TX_CFG_PLM_0_STAT_WR_ADR_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_0_STAT_WR_ADR_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_0_STAT_WR_ADR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_0_STAT_AF
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_0_STAT_AF_FIELD =
{
    "AF",
#if RU_INCLUDE_DESC
    "almost_full",
    "Set when the FIFO has space for one more PLOAM",
#endif
    NGPON_TX_CFG_PLM_0_STAT_AF_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_0_STAT_AF_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_0_STAT_AF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_0_STAT_F
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_0_STAT_F_FIELD =
{
    "F",
#if RU_INCLUDE_DESC
    "full",
    "FIFO full indication",
#endif
    NGPON_TX_CFG_PLM_0_STAT_F_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_0_STAT_F_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_0_STAT_F_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_0_STAT_AE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_0_STAT_AE_FIELD =
{
    "AE",
#if RU_INCLUDE_DESC
    "almost_empty",
    "Set when the FIFO has only one valid PLOAM in it",
#endif
    NGPON_TX_CFG_PLM_0_STAT_AE_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_0_STAT_AE_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_0_STAT_AE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_0_STAT_E
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_0_STAT_E_FIELD =
{
    "E",
#if RU_INCLUDE_DESC
    "empty",
    "Set when the FIFO is empty",
#endif
    NGPON_TX_CFG_PLM_0_STAT_E_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_0_STAT_E_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_0_STAT_E_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_PLM_0_STAT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_PLM_0_STAT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_PLM_0_STAT_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_PLM_0_STAT_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_PLM_0_STAT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_LINE_RATE_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_LINE_RATE_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "2b00 2.5GHz"
    "2b01 5GHz"
    "2b10 10GHz"
    "2b11 Reserved",
#endif
    NGPON_TX_CFG_LINE_RATE_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_LINE_RATE_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_LINE_RATE_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_LINE_RATE_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_LINE_RATE_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_LINE_RATE_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_LINE_RATE_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_LINE_RATE_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_START_OFFSET_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_START_OFFSET_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "the value of the offset in cycles (words)",
#endif
    NGPON_TX_CFG_START_OFFSET_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_START_OFFSET_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_START_OFFSET_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_START_OFFSET_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_START_OFFSET_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_START_OFFSET_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_START_OFFSET_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_START_OFFSET_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "Value",
    "Sets the XGPON frame length",
#endif
    NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Number of payload words in an IDLE XGEM frame.",
#endif
    NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_VALUE_FIELD_MASK,
    0,
    NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_VALUE_FIELD_WIDTH,
    NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_BIG_BRST_FILT_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_BIG_BRST_FILT_EN_FIELD =
{
    "BIG_BRST_FILT_EN",
#if RU_INCLUDE_DESC
    "burst_too_big_filter_enable",
    "If this bit is set all the access that cause the accumulate grant sizes of the burst to exceeds frame size (9720) will not be executed.",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_BIG_BRST_FILT_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_BIG_BRST_FILT_EN_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_BIG_BRST_FILT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_BIG_GRNT_FILT_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_BIG_GRNT_FILT_EN_FIELD =
{
    "BIG_GRNT_FILT_EN",
#if RU_INCLUDE_DESC
    "grant_too_big_filter_enable",
    "When this bit is set access with grant size bigger then 9718 will not be executed.",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_BIG_GRNT_FILT_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_BIG_GRNT_FILT_EN_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_BIG_GRNT_FILT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_TOO_MANY_ACCS_FILT_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_TOO_MANY_ACCS_FILT_EN_FIELD =
{
    "TOO_MANY_ACCS_FILT_EN",
#if RU_INCLUDE_DESC
    "too_many_access_filter_enable",
    "If this bit is set all the access that fallows the 16th access of a burst will not be executed",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_TOO_MANY_ACCS_FILT_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_TOO_MANY_ACCS_FILT_EN_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_TOO_MANY_ACCS_FILT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_NOT_FIRST_ACCS_PLM_FILT_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_NOT_FIRST_ACCS_PLM_FILT_EN_FIELD =
{
    "NOT_FIRST_ACCS_PLM_FILT_EN",
#if RU_INCLUDE_DESC
    "not_first_access_ploam_filter_enable",
    "PLOAM indication can be set only in the first access of a burst."
    "If a PLOAM bit is set in any other burst access and this bit is set, that access will be filtered.",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_NOT_FIRST_ACCS_PLM_FILT_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_NOT_FIRST_ACCS_PLM_FILT_EN_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_NOT_FIRST_ACCS_PLM_FILT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_LATE_ACCS_FILT_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_LATE_ACCS_FILT_EN_FIELD =
{
    "LATE_ACCS_FILT_EN",
#if RU_INCLUDE_DESC
    "late_access_filter_enable",
    "If an access is read by the ONU-TX and its start time already passed and this bit is set, the access and all the fallowing burst access will not be executed.",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_LATE_ACCS_FILT_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_LATE_ACCS_FILT_EN_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_LATE_ACCS_FILT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_WRNG_GNT_SIZE_FILT_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_WRNG_GNT_SIZE_FILT_EN_FIELD =
{
    "WRNG_GNT_SIZE_FILT_EN",
#if RU_INCLUDE_DESC
    "wrong_grant_size_filter_enable",
    "Accesses with grat size of 2 or 3 are illegal. When this bit is set such access will be filter, not be executed.",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_WRNG_GNT_SIZE_FILT_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_WRNG_GNT_SIZE_FILT_EN_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_WRNG_GNT_SIZE_FILT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_PROF_FILT_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_PROF_FILT_EN_FIELD =
{
    "PROF_FILT_EN",
#if RU_INCLUDE_DESC
    "profile_filter_enable",
    "If this bit is set every access that its profile_en bit isnt set, will not be executed.",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_PROF_FILT_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_PROF_FILT_EN_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_PROF_FILT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_TRIM_TOO_BIG_GRNT_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_TRIM_TOO_BIG_GRNT_EN_FIELD =
{
    "TRIM_TOO_BIG_GRNT_EN",
#if RU_INCLUDE_DESC
    "trim_too_big_grant_enable",
    "When this bit is set, the allocation which crosses 9718 is truncated at 9718."
    ""
    "Next allocations are discarded."
    "",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_TRIM_TOO_BIG_GRNT_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_TRIM_TOO_BIG_GRNT_EN_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_TRIM_TOO_BIG_GRNT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_START_TIME_FILT_EN
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_START_TIME_FILT_EN_FIELD =
{
    "START_TIME_FILT_EN",
#if RU_INCLUDE_DESC
    "start_time_too_big_filter_enable",
    "When this bit is set access with start time bigger then 9719 will not be executed.",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_START_TIME_FILT_EN_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_START_TIME_FILT_EN_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_START_TIME_FILT_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_AF_ERR_FILT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_AF_ERR_FILT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_AF_ERR_FILT_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_AF_ERR_FILT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DYING_GASP_DG
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DYING_GASP_DG_FIELD =
{
    "DG",
#if RU_INCLUDE_DESC
    "dying_gasp",
    "when set to 1 indicates that this ONU is about to end its operation.",
#endif
    NGPON_TX_CFG_DYING_GASP_DG_FIELD_MASK,
    0,
    NGPON_TX_CFG_DYING_GASP_DG_FIELD_WIDTH,
    NGPON_TX_CFG_DYING_GASP_DG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_DYING_GASP_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_DYING_GASP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_DYING_GASP_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_DYING_GASP_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_DYING_GASP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FRAGMENTATION_DISABLE_FRGMENT_DIS
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FRAGMENTATION_DISABLE_FRGMENT_DIS_FIELD =
{
    "FRGMENT_DIS",
#if RU_INCLUDE_DESC
    "Fragmentation_disable",
    "0 - Enable fragmentation (normal operation mode)"
    "1 - Disable fragmentation",
#endif
    NGPON_TX_CFG_FRAGMENTATION_DISABLE_FRGMENT_DIS_FIELD_MASK,
    0,
    NGPON_TX_CFG_FRAGMENTATION_DISABLE_FRGMENT_DIS_FIELD_WIDTH,
    NGPON_TX_CFG_FRAGMENTATION_DISABLE_FRGMENT_DIS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_FRAGMENTATION_DISABLE_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_FRAGMENTATION_DISABLE_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_FRAGMENTATION_DISABLE_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_FRAGMENTATION_DISABLE_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_FRAGMENTATION_DISABLE_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_XGTC_HEADER_IND_7_1_BITS_7_1
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_XGTC_HEADER_IND_7_1_BITS_7_1_FIELD =
{
    "BITS_7_1",
#if RU_INCLUDE_DESC
    "bits_7_1",
    "configure bits [7:1] in the IND field of the XGTC header",
#endif
    NGPON_TX_CFG_XGTC_HEADER_IND_7_1_BITS_7_1_FIELD_MASK,
    0,
    NGPON_TX_CFG_XGTC_HEADER_IND_7_1_BITS_7_1_FIELD_WIDTH,
    NGPON_TX_CFG_XGTC_HEADER_IND_7_1_BITS_7_1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: NGPON_TX_CFG_XGTC_HEADER_IND_7_1_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_CFG_XGTC_HEADER_IND_7_1_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_CFG_XGTC_HEADER_IND_7_1_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_CFG_XGTC_HEADER_IND_7_1_RESERVED0_FIELD_WIDTH,
    NGPON_TX_CFG_XGTC_HEADER_IND_7_1_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_TX_CFG_ONU_TX_EN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_ONU_TX_EN_FIELDS[] =
{
    &NGPON_TX_CFG_ONU_TX_EN_TRANSMITTER_ENABLE_FIELD,
    &NGPON_TX_CFG_ONU_TX_EN_STANDARD_FIELD,
    &NGPON_TX_CFG_ONU_TX_EN_SPEED_FIELD,
    &NGPON_TX_CFG_ONU_TX_EN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_ONU_TX_EN_REG = 
{
    "ONU_TX_EN",
#if RU_INCLUDE_DESC
    "ONU_TX_ENABLE Register",
    "Using this register the SW can Enable/Disable the ONU-TX."
    "When the ONU-TX is disabled all the ONU-TX's internal state machines/timers/counters are reset to default values excluding configuration and PM registers.",
#endif
    NGPON_TX_CFG_ONU_TX_EN_REG_OFFSET,
    0,
    0,
    142,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_TX_CFG_ONU_TX_EN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_FUNC_EN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_FUNC_EN_FIELDS[] =
{
    &NGPON_TX_CFG_FUNC_EN_SCRMBLR_EN_FIELD,
    &NGPON_TX_CFG_FUNC_EN_BIP_EN_FIELD,
    &NGPON_TX_CFG_FUNC_EN_FEC_EN_FIELD,
    &NGPON_TX_CFG_FUNC_EN_RESERVED1_FIELD,
    &NGPON_TX_CFG_FUNC_EN_RESERVED2_FIELD,
    &NGPON_TX_CFG_FUNC_EN_RESERVED3_FIELD,
    &NGPON_TX_CFG_FUNC_EN_ENC_EN_FIELD,
    &NGPON_TX_CFG_FUNC_EN_RESERVED0_FIELD,
    &NGPON_TX_CFG_FUNC_EN_S_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_FUNC_EN_REG = 
{
    "FUNC_EN",
#if RU_INCLUDE_DESC
    "FUNCTION_ENABLE Register",
    "Enable/Disable of blocks sub modules functions",
#endif
    NGPON_TX_CFG_FUNC_EN_REG_OFFSET,
    0,
    0,
    143,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    9,
    NGPON_TX_CFG_FUNC_EN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DAT_POL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DAT_POL_FIELDS[] =
{
    &NGPON_TX_CFG_DAT_POL_TX_DATA_OUT_POLARITY_FIELD,
    &NGPON_TX_CFG_DAT_POL_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DAT_POL_REG = 
{
    "DAT_POL",
#if RU_INCLUDE_DESC
    "DATA_POLARITY Register",
    "This register configuration determind the polarity of the output data.",
#endif
    NGPON_TX_CFG_DAT_POL_REG_OFFSET,
    0,
    0,
    144,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_DAT_POL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_TX_RNG_DLY
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_TX_RNG_DLY_FIELDS[] =
{
    &NGPON_TX_CFG_TX_RNG_DLY_TX_WORD_DELAY_FIELD,
    &NGPON_TX_CFG_TX_RNG_DLY_TX_FRAME_DELAY_FIELD,
    &NGPON_TX_CFG_TX_RNG_DLY_TX_BIT_DELAY_FIELD,
    &NGPON_TX_CFG_TX_RNG_DLY_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_TX_RNG_DLY_REG = 
{
    "TX_RNG_DLY",
#if RU_INCLUDE_DESC
    "TX_RANGING_DELAY Register",
    "Configuration of the ranging delay up to bit resolution.",
#endif
    NGPON_TX_CFG_TX_RNG_DLY_REG_OFFSET,
    0,
    0,
    145,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_TX_CFG_TX_RNG_DLY_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DV_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DV_CFG_FIELDS[] =
{
    &NGPON_TX_CFG_DV_CFG_DV_POL_FIELD,
    &NGPON_TX_CFG_DV_CFG_RESERVED0_FIELD,
    &NGPON_TX_CFG_DV_CFG_DV_SETUP_LEN_FIELD,
    &NGPON_TX_CFG_DV_CFG_DV_HOLD_LEN_FIELD,
    &NGPON_TX_CFG_DV_CFG_DV_SETUP_PAT_SRC_FIELD,
    &NGPON_TX_CFG_DV_CFG_DV_HOLD_PAT_SRC_FIELD,
    &NGPON_TX_CFG_DV_CFG_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DV_CFG_REG = 
{
    "DV_CFG",
#if RU_INCLUDE_DESC
    "DV_CONFIGURATION Register",
    "DV is the data valid signal output to control the burst transceiver. The configuation allow addition of setup and hold extention before and after the US burst and configuation of the DV polarity."
    "This register can be modified only when TX is disabled.",
#endif
    NGPON_TX_CFG_DV_CFG_REG_OFFSET,
    0,
    0,
    146,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    7,
    NGPON_TX_CFG_DV_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DV_SETUP_PAT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DV_SETUP_PAT_FIELDS[] =
{
    &NGPON_TX_CFG_DV_SETUP_PAT_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DV_SETUP_PAT_REG = 
{
    "DV_SETUP_PAT",
#if RU_INCLUDE_DESC
    "DV_SETUP_PATTERN Register",
    "DV is the data valid signal output to control the burst transceiver. This register configures the pattern of this signal during the first word of setup. This register can be modified only when TX is disabled.",
#endif
    NGPON_TX_CFG_DV_SETUP_PAT_REG_OFFSET,
    0,
    0,
    147,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_CFG_DV_SETUP_PAT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DV_HOLD_PAT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DV_HOLD_PAT_FIELDS[] =
{
    &NGPON_TX_CFG_DV_HOLD_PAT_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DV_HOLD_PAT_REG = 
{
    "DV_HOLD_PAT",
#if RU_INCLUDE_DESC
    "DV_HOLD_PATTERN Register",
    "DV is the data valid signal output to control the burst transceiver. This register configure the pattern of this signal during the last word of hold time."
    "This register can be modified only when TX is disabled.",
#endif
    NGPON_TX_CFG_DV_HOLD_PAT_REG_OFFSET,
    0,
    0,
    148,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_CFG_DV_HOLD_PAT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DAT_PAT_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DAT_PAT_CFG_FIELDS[] =
{
    &NGPON_TX_CFG_DAT_PAT_CFG_DAT_PAT_TYPE_FIELD,
    &NGPON_TX_CFG_DAT_PAT_CFG_RESERVED0_FIELD,
    &NGPON_TX_CFG_DAT_PAT_CFG_SETUP_LEN_FIELD,
    &NGPON_TX_CFG_DAT_PAT_CFG_RESERVED1_FIELD,
    &NGPON_TX_CFG_DAT_PAT_CFG_HOLD_LEN_FIELD,
    &NGPON_TX_CFG_DAT_PAT_CFG_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DAT_PAT_CFG_REG = 
{
    "DAT_PAT_CFG",
#if RU_INCLUDE_DESC
    "DATA_PATTERN_CONFIGURATION Register",
    "This register contains the configurations for the data pattern when the DV is off.",
#endif
    NGPON_TX_CFG_DAT_PAT_CFG_REG_OFFSET,
    0,
    0,
    149,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    6,
    NGPON_TX_CFG_DAT_PAT_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DAT_PAT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DAT_PAT_FIELDS[] =
{
    &NGPON_TX_CFG_DAT_PAT_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DAT_PAT_REG = 
{
    "DAT_PAT",
#if RU_INCLUDE_DESC
    "DATA_PATTERN Register",
    "Pattern of the data while the DV is off and data pattern type is configured to be taken from register.",
#endif
    NGPON_TX_CFG_DAT_PAT_REG_OFFSET,
    0,
    0,
    150,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_CFG_DAT_PAT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DAT_SETUP_PAT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DAT_SETUP_PAT_FIELDS[] =
{
    &NGPON_TX_CFG_DAT_SETUP_PAT_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DAT_SETUP_PAT_REG = 
{
    "DAT_SETUP_PAT",
#if RU_INCLUDE_DESC
    "DATA_SETUP_PATTERN %i Register",
    "Set of 4 registers that togheter contains configuration of the data setup pattern. This pattern will be transmitted before the preamble if the data pattern is enabled. The setup pattern length is configured and may vary from 1 to 4 words (see previous register). If a length of one word is configured, the first register (the one with the lowest address offset) will be ansmitted. If the length is 4 bytes all the register set will be transmitted. Starting with the first one.",
#endif
    NGPON_TX_CFG_DAT_SETUP_PAT_REG_OFFSET,
    NGPON_TX_CFG_DAT_SETUP_PAT_REG_RAM_CNT,
    4,
    151,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_CFG_DAT_SETUP_PAT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DAT_HOLD_PAT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DAT_HOLD_PAT_FIELDS[] =
{
    &NGPON_TX_CFG_DAT_HOLD_PAT_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DAT_HOLD_PAT_REG = 
{
    "DAT_HOLD_PAT",
#if RU_INCLUDE_DESC
    "DATA_HOLD_PATTERN %i Register",
    "This set of registers contain configurable the data hold pattern. This pattern will be transmitted right after the last word of payload, XGTC trailer, if the data pattern is enabled."
    "The setup pattern length is configured and may vary from 1 to 4 words (see previous register). If a length of one word is configured, the first register (the one with the lowest address offset) will be ansmitted. If the length is 4 bytes all the register set will be transmitted. Starting with the first one.",
#endif
    NGPON_TX_CFG_DAT_HOLD_PAT_REG_OFFSET,
    NGPON_TX_CFG_DAT_HOLD_PAT_REG_RAM_CNT,
    4,
    152,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_CFG_DAT_HOLD_PAT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_FIELDS[] =
{
    &NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_ONU_ID_FIELD,
    &NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_REG = 
{
    "TCONT_TO_ONU_ID_MAP",
#if RU_INCLUDE_DESC
    "TCONT_TO_ONU_ID_MAPPING %i Register",
    "The register configures the translation of the T-CONT(n) to an ONU-ID, n=[0..43]. This translation is used togther with the ONU_ID to PLOAM yielding a translation of a T-CONT to a spcific PLOAM. This translation is needed because the OLT-TX requests a PLOAMu for a spcific Alloc-id (T-CONT)."
    ""
    "Note that only n=[0..39] has a frontend buffer.",
#endif
    NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_REG_OFFSET,
    NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_REG_RAM_CNT,
    4,
    153,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_TCONT_FLUSH
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_TCONT_FLUSH_FIELDS[] =
{
    &NGPON_TX_CFG_TCONT_FLUSH_FLSH_Q_FIELD,
    &NGPON_TX_CFG_TCONT_FLUSH_RESERVED0_FIELD,
    &NGPON_TX_CFG_TCONT_FLUSH_FLSH_EN_FIELD,
    &NGPON_TX_CFG_TCONT_FLUSH_FLSH_IMM_FIELD,
    &NGPON_TX_CFG_TCONT_FLUSH_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_TCONT_FLUSH_REG = 
{
    "TCONT_FLUSH",
#if RU_INCLUDE_DESC
    "SINGLE_TCONT_FLUSH Register",
    "Single TCONT flush to specific queue. Data in the queue will be disregarded, and its read and write pointers will be reset."
    "In order to avoid under-run in the TX fifos, the flush does not happen immediately, but is done when there is no transmission. When the flush is done, the HW will assert flush_done bit (see next register)."
    "If an immediate flush is desired (no waiting for no transmission), the flush immediate bit should be asserted along with the flush valid.",
#endif
    NGPON_TX_CFG_TCONT_FLUSH_REG_OFFSET,
    0,
    0,
    154,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    NGPON_TX_CFG_TCONT_FLUSH_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_ONU_ID_TO_PLM_MAP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_FIELDS[] =
{
    &NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_ONU_ID_FIELD,
    &NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED0_FIELD,
    &NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_PLM_NUM_FIELD,
    &NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED1_FIELD,
    &NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_FLBK_PLM_NUM_FIELD,
    &NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_REG = 
{
    "ONU_ID_TO_PLM_MAP",
#if RU_INCLUDE_DESC
    "ONU_ID_TO_PLOAM_MAPPING %i Register",
    "A 3 registers array. Each  register configures the mapping of an ONU-ID to a PLOAM buffer and fallback PLOAM buffer."
    "This translation is used togther with the ONU_ID to PLOAM yielding a translation of a T-CONT to a spcific PLOAM. This translation is needed because the OLT-TX requests a PLOAMu for a spcific Alloc-ID (ONU-RX translates the Alloc-ID to a T-CONT)",
#endif
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_REG_OFFSET,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_REG_RAM_CNT,
    4,
    155,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    6,
    NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_TX_FIFO_GRP_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_TX_FIFO_GRP_CFG_FIELDS[] =
{
    &NGPON_TX_CFG_TX_FIFO_GRP_CFG_BASE_FIELD,
    &NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED0_FIELD,
    &NGPON_TX_CFG_TX_FIFO_GRP_CFG_SIZE_FIELD,
    &NGPON_TX_CFG_TX_FIFO_GRP_CFG_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_TX_FIFO_GRP_CFG_REG = 
{
    "TX_FIFO_GRP_CFG",
#if RU_INCLUDE_DESC
    "TX_FIFO_GROUP_CONFIG %i Register",
    "Tconts 8-39 are bundled in 4 groups of 8 TCONTS each. The SW configure each groups base address and size. The Queues within each group are of the same size. This register array of 4 registers contain the configured base address of each group. The first register configures the address of group0 (TCONTS 8-15), and so on.",
#endif
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_REG_OFFSET,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_REG_RAM_CNT,
    4,
    156,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_TX_CFG_TX_FIFO_GRP_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_TX_FIFO_Q_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_TX_FIFO_Q_CFG_FIELDS[] =
{
    &NGPON_TX_CFG_TX_FIFO_Q_CFG_BASE_FIELD,
    &NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED0_FIELD,
    &NGPON_TX_CFG_TX_FIFO_Q_CFG_SIZE_FIELD,
    &NGPON_TX_CFG_TX_FIFO_Q_CFG_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_TX_FIFO_Q_CFG_REG = 
{
    "TX_FIFO_Q_CFG",
#if RU_INCLUDE_DESC
    "TX_FIFO_QUEUE_CONFIG %i Register",
    "Set of 8 registers, this array contains the Tx FIFO parameters of the first 8 (0 to 7)of the Tx FIFO queues. It gives the base address of each of the first 8 queues and the queues size. A queue size is in 4 bytes resolution and is up to 13 bits  (0-32Kbyte). The total size of all 40 queues must be smaller or equal to 20K. The base address must also be 4 bytes aligned."
    "This register can be modified only when TX is disabled.",
#endif
    NGPON_TX_CFG_TX_FIFO_Q_CFG_REG_OFFSET,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_REG_RAM_CNT,
    4,
    157,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_TX_CFG_TX_FIFO_Q_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_PD_Q_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_PD_Q_CFG_FIELDS[] =
{
    &NGPON_TX_CFG_PD_Q_CFG_BASE_FIELD,
    &NGPON_TX_CFG_PD_Q_CFG_RESERVED0_FIELD,
    &NGPON_TX_CFG_PD_Q_CFG_SIZE_FIELD,
    &NGPON_TX_CFG_PD_Q_CFG_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_PD_Q_CFG_REG = 
{
    "PD_Q_CFG",
#if RU_INCLUDE_DESC
    "PACKET_DESCRIPTOR_QUEUE_CONFIG %i Register",
    "This register array contains the packet descriptor parameters. Each of the eight entries in the array relates to one of the first eight Tx queues (ordered 0 to seven) and  details  the number of packet descriptor allocated to that queue (out of 128). This register can be modified only when TX is disabled.",
#endif
    NGPON_TX_CFG_PD_Q_CFG_REG_OFFSET,
    NGPON_TX_CFG_PD_Q_CFG_REG_RAM_CNT,
    4,
    158,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_TX_CFG_PD_Q_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_PD_GRP_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_PD_GRP_CFG_FIELDS[] =
{
    &NGPON_TX_CFG_PD_GRP_CFG_BASE_FIELD,
    &NGPON_TX_CFG_PD_GRP_CFG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_PD_GRP_CFG_REG = 
{
    "PD_GRP_CFG",
#if RU_INCLUDE_DESC
    "PACKET_DESCRIPTOR_GROUP_CONFIG %i Register",
    "Tconts 8-39 are bundled in 4 groups of 8 TCONTS each. The SW configure each groups base address for the packet descriptors queues. The packet descriptors Queues within each group are of the same size. The size is derived from the corresponding data queue size. The PD queue size is the corresponding data queue size divided by 64 bytes."
    "This register array of 4 registers contain the configured base address of each group. The first register configures the address of group0 (TCONTS 8-15), and so on.",
#endif
    NGPON_TX_CFG_PD_GRP_CFG_REG_OFFSET,
    NGPON_TX_CFG_PD_GRP_CFG_REG_RAM_CNT,
    4,
    159,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_PD_GRP_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DBR_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DBR_CFG_FIELDS[] =
{
    &NGPON_TX_CFG_DBR_CFG_DBR_SRC_FIELD,
    &NGPON_TX_CFG_DBR_CFG_DBR_FLUSH_FIELD,
    &NGPON_TX_CFG_DBR_CFG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DBR_CFG_REG = 
{
    "DBR_CFG",
#if RU_INCLUDE_DESC
    "DBR_CONFIG Register",
    "configuration of the DBR mechanism. HW/SW report."
    "This register can be updated when TX is enabled.",
#endif
    NGPON_TX_CFG_DBR_CFG_REG_OFFSET,
    0,
    0,
    160,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    NGPON_TX_CFG_DBR_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DBR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DBR_FIELDS[] =
{
    &NGPON_TX_CFG_DBR_BUFOCC_FIELD,
    &NGPON_TX_CFG_DBR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DBR_REG = 
{
    "DBR",
#if RU_INCLUDE_DESC
    "DBR %i Register",
    "SW DBR (BufOcc) report."
    "An array of 40 registers, one per allocation ID (T-CONT)."
    "These register can be modified only when the corresponding valid bit is inactive.",
#endif
    NGPON_TX_CFG_DBR_REG_OFFSET,
    NGPON_TX_CFG_DBR_REG_RAM_CNT,
    4,
    161,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_DBR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_XGEM_PYLD_MIN_LEN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_FIELDS[] =
{
    &NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_VALUE_FIELD,
    &NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_REG = 
{
    "XGEM_PYLD_MIN_LEN",
#if RU_INCLUDE_DESC
    "XGEM_PAYLOAD_MIN_LENGTH Register",
    "Configuration of the minimum XGEM payload length in bytes. When the data length available for transmission is smaller then the register value, an idle XGEM frame will be sent. An exception is if the available data for transmission is bigger then the configuration and the available space left in the allocation is smaller then the configuration. In this case the available BW will be used and an XGEM smaller then this configuration will be generated.",
#endif
    NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_REG_OFFSET,
    0,
    0,
    162,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_FIELDS[] =
{
    &NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_0_Q_NUM_FIELD,
    &NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED0_FIELD,
    &NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_1_Q_NUM_FIELD,
    &NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED1_FIELD,
    &NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_2_Q_NUM_FIELD,
    &NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED2_FIELD,
    &NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_GRP_3_Q_NUM_FIELD,
    &NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_RESERVED3_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_REG = 
{
    "TCONT_TO_CNT_GRP_MAP",
#if RU_INCLUDE_DESC
    "TCONT_TO_COUNTER_GROUP_MAPPING Register",
    "There are four sets of PM counters. Each set counts events that are related to one TCONT. This register determines which TCONT is associated with each one of the counters groups",
#endif
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_REG_OFFSET,
    0,
    0,
    163,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    8,
    NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_PLM_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_PLM_CFG_FIELDS[] =
{
    &NGPON_TX_CFG_PLM_CFG_PRSSTNT_FIELD,
    &NGPON_TX_CFG_PLM_CFG_VALID_FIELD,
    &NGPON_TX_CFG_PLM_CFG_SW_EN_FIELD,
    &NGPON_TX_CFG_PLM_CFG_USE_DEF_FIELD,
    &NGPON_TX_CFG_PLM_CFG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_PLM_CFG_REG = 
{
    "PLM_CFG",
#if RU_INCLUDE_DESC
    "PLOAM_CONFIG %i Register",
    "A set of 3 registers array. Each register reflects the valid and persitent bits of a PLOAM buf.",
#endif
    NGPON_TX_CFG_PLM_CFG_REG_OFFSET,
    NGPON_TX_CFG_PLM_CFG_REG_RAM_CNT,
    4,
    164,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    NGPON_TX_CFG_PLM_CFG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_ROGUE_ONU_CTRL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_ROGUE_ONU_CTRL_FIELDS[] =
{
    &NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_LEVEL_CLR_FIELD,
    &NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DIFF_CLR_FIELD,
    &NGPON_TX_CFG_ROGUE_ONU_CTRL_ROGUE_DV_SEL_FIELD,
    &NGPON_TX_CFG_ROGUE_ONU_CTRL_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_ROGUE_ONU_CTRL_REG = 
{
    "ROGUE_ONU_CTRL",
#if RU_INCLUDE_DESC
    "ROGUE_ONU_CONTROL Register",
    "enables the measurement of rogue ONU."
    ""
    "If TX_DV will be asserted for a configured time window, then the TX_DV will assert an interrupt.",
#endif
    NGPON_TX_CFG_ROGUE_ONU_CTRL_REG_OFFSET,
    0,
    0,
    165,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    NGPON_TX_CFG_ROGUE_ONU_CTRL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_ROGUE_LEVEL_TIME
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_ROGUE_LEVEL_TIME_FIELDS[] =
{
    &NGPON_TX_CFG_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_ROGUE_LEVEL_TIME_REG = 
{
    "ROGUE_LEVEL_TIME",
#if RU_INCLUDE_DESC
    "ROGUE_ONU_LEVEL_THRESHOLD Register",
    "num of cycles the DV is allowed to be active before considered as rogue",
#endif
    NGPON_TX_CFG_ROGUE_LEVEL_TIME_REG_OFFSET,
    0,
    0,
    166,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_CFG_ROGUE_LEVEL_TIME_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_ROGUE_DIFF_TIME
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_ROGUE_DIFF_TIME_FIELDS[] =
{
    &NGPON_TX_CFG_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD,
    &NGPON_TX_CFG_ROGUE_DIFF_TIME_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_ROGUE_DIFF_TIME_REG = 
{
    "ROGUE_DIFF_TIME",
#if RU_INCLUDE_DESC
    "ROGUE_DIFF_THRESHOLD Register",
    "num of cycles the examined DV is allowed to be different from gpon dv before being considered as rogue",
#endif
    NGPON_TX_CFG_ROGUE_DIFF_TIME_REG_OFFSET,
    0,
    0,
    167,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_ROGUE_DIFF_TIME_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_LPB_Q_NUM
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_LPB_Q_NUM_FIELDS[] =
{
    &NGPON_TX_CFG_LPB_Q_NUM_VALUE_FIELD,
    &NGPON_TX_CFG_LPB_Q_NUM_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_LPB_Q_NUM_REG = 
{
    "LPB_Q_NUM",
#if RU_INCLUDE_DESC
    "LOOPBACK_QUEUE_NUMBER Register",
    "This register maps the loopback operation mode to one out of the 40 Tx queues (while disabling the other queues). This register can be modified only when TX is disabled.",
#endif
    NGPON_TX_CFG_LPB_Q_NUM_REG_OFFSET,
    0,
    0,
    168,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_LPB_Q_NUM_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_XGEM_HDR_OPT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_XGEM_HDR_OPT_FIELDS[] =
{
    &NGPON_TX_CFG_XGEM_HDR_OPT_VALUE_FIELD,
    &NGPON_TX_CFG_XGEM_HDR_OPT_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_XGEM_HDR_OPT_REG = 
{
    "XGEM_HDR_OPT",
#if RU_INCLUDE_DESC
    "XGEM_HEADER_OPTIONS Register",
    "18 bits Options field of the XGEM header is taken from this register.",
#endif
    NGPON_TX_CFG_XGEM_HDR_OPT_REG_OFFSET,
    0,
    0,
    169,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_XGEM_HDR_OPT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_PLM_0_STAT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_PLM_0_STAT_FIELDS[] =
{
    &NGPON_TX_CFG_PLM_0_STAT_OCCUPY_FIELD,
    &NGPON_TX_CFG_PLM_0_STAT_RD_ADR_FIELD,
    &NGPON_TX_CFG_PLM_0_STAT_WR_ADR_FIELD,
    &NGPON_TX_CFG_PLM_0_STAT_AF_FIELD,
    &NGPON_TX_CFG_PLM_0_STAT_F_FIELD,
    &NGPON_TX_CFG_PLM_0_STAT_AE_FIELD,
    &NGPON_TX_CFG_PLM_0_STAT_E_FIELD,
    &NGPON_TX_CFG_PLM_0_STAT_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_PLM_0_STAT_REG = 
{
    "PLM_0_STAT",
#if RU_INCLUDE_DESC
    "PLOAM_0_FIFO_STATUS Register",
    "Status of the PLOAM 0 FIFO. Holds the number of PLOAM in the FIFO",
#endif
    NGPON_TX_CFG_PLM_0_STAT_REG_OFFSET,
    0,
    0,
    170,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    8,
    NGPON_TX_CFG_PLM_0_STAT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_LINE_RATE
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_LINE_RATE_FIELDS[] =
{
    &NGPON_TX_CFG_LINE_RATE_VALUE_FIELD,
    &NGPON_TX_CFG_LINE_RATE_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_LINE_RATE_REG = 
{
    "LINE_RATE",
#if RU_INCLUDE_DESC
    "LINE_RATE Register",
    "Configuration of the XGPON line rate."
    "2.5GHz / 5GHz / 10GHz",
#endif
    NGPON_TX_CFG_LINE_RATE_REG_OFFSET,
    0,
    0,
    171,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_LINE_RATE_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_START_OFFSET
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_START_OFFSET_FIELDS[] =
{
    &NGPON_TX_CFG_START_OFFSET_VALUE_FIELD,
    &NGPON_TX_CFG_START_OFFSET_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_START_OFFSET_REG = 
{
    "START_OFFSET",
#if RU_INCLUDE_DESC
    "TIMELINE_OFFSET Register",
    "configurable offset of the burst start time in cycles (words). The offset is needed so that events before start time will have positive timeline value. This register is for internal use",
#endif
    NGPON_TX_CFG_START_OFFSET_REG_OFFSET,
    0,
    0,
    172,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_START_OFFSET_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_FRAME_LENGTH_MINUS_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_FIELDS[] =
{
    &NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_VALUE_FIELD,
    &NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_REG = 
{
    "FRAME_LENGTH_MINUS_1",
#if RU_INCLUDE_DESC
    "FRAME_LENGTH_MINUS_1 Register",
    "Sets the number of cycles minus1 of an XGPON frame."
    "This value should be changed only when the transmitter is disabled.",
#endif
    NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_REG_OFFSET,
    0,
    0,
    173,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_FIELDS[] =
{
    &NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_VALUE_FIELD,
    &NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_REG = 
{
    "IDLE_XGEM_PYLD_LEN",
#if RU_INCLUDE_DESC
    "IDLE_XGEM_PAYLOAD_LENGTH Register",
    "Number of payload words in an IDLE XGEM frame. If an IDLE XGEM is transmitted and thre is enough space for this length then it is used. Else the payload of the IDLE XGEM will be according to available size.",
#endif
    NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_REG_OFFSET,
    0,
    0,
    174,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_AF_ERR_FILT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_AF_ERR_FILT_FIELDS[] =
{
    &NGPON_TX_CFG_AF_ERR_FILT_BIG_BRST_FILT_EN_FIELD,
    &NGPON_TX_CFG_AF_ERR_FILT_BIG_GRNT_FILT_EN_FIELD,
    &NGPON_TX_CFG_AF_ERR_FILT_TOO_MANY_ACCS_FILT_EN_FIELD,
    &NGPON_TX_CFG_AF_ERR_FILT_NOT_FIRST_ACCS_PLM_FILT_EN_FIELD,
    &NGPON_TX_CFG_AF_ERR_FILT_LATE_ACCS_FILT_EN_FIELD,
    &NGPON_TX_CFG_AF_ERR_FILT_WRNG_GNT_SIZE_FILT_EN_FIELD,
    &NGPON_TX_CFG_AF_ERR_FILT_PROF_FILT_EN_FIELD,
    &NGPON_TX_CFG_AF_ERR_FILT_TRIM_TOO_BIG_GRNT_EN_FIELD,
    &NGPON_TX_CFG_AF_ERR_FILT_START_TIME_FILT_EN_FIELD,
    &NGPON_TX_CFG_AF_ERR_FILT_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_AF_ERR_FILT_REG = 
{
    "AF_ERR_FILT",
#if RU_INCLUDE_DESC
    "ACCESS_ERROR_FILTER Register",
    "The ONU-TX check the legality of the accesses received from the ONU-RX."
    "Each error can cause the disabling of the execution of the access if the enable bit in this register is set.",
#endif
    NGPON_TX_CFG_AF_ERR_FILT_REG_OFFSET,
    0,
    0,
    175,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    10,
    NGPON_TX_CFG_AF_ERR_FILT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_DYING_GASP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_DYING_GASP_FIELDS[] =
{
    &NGPON_TX_CFG_DYING_GASP_DG_FIELD,
    &NGPON_TX_CFG_DYING_GASP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_DYING_GASP_REG = 
{
    "DYING_GASP",
#if RU_INCLUDE_DESC
    "DYING_GASP Register",
    "Indication that this ONU is about to end its operation.",
#endif
    NGPON_TX_CFG_DYING_GASP_REG_OFFSET,
    0,
    0,
    176,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_DYING_GASP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_FRAGMENTATION_DISABLE
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_FRAGMENTATION_DISABLE_FIELDS[] =
{
    &NGPON_TX_CFG_FRAGMENTATION_DISABLE_FRGMENT_DIS_FIELD,
    &NGPON_TX_CFG_FRAGMENTATION_DISABLE_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_FRAGMENTATION_DISABLE_REG = 
{
    "FRAGMENTATION_DISABLE",
#if RU_INCLUDE_DESC
    "FRAGMENTATION_DISABLE Register",
    "When fragmentation is stopped, ONU complete sending the trailing fragments of the packets that have started transmission already, but will not initiate fragmentation of any new packets."
    "If the length of a Packet scheduled for transmission (including XGEM header) exceeds the available allocation, idle XGEM are sent.",
#endif
    NGPON_TX_CFG_FRAGMENTATION_DISABLE_REG_OFFSET,
    0,
    0,
    177,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_FRAGMENTATION_DISABLE_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_CFG_XGTC_HEADER_IND_7_1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_CFG_XGTC_HEADER_IND_7_1_FIELDS[] =
{
    &NGPON_TX_CFG_XGTC_HEADER_IND_7_1_BITS_7_1_FIELD,
    &NGPON_TX_CFG_XGTC_HEADER_IND_7_1_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_CFG_XGTC_HEADER_IND_7_1_REG = 
{
    "XGTC_HEADER_IND_7_1",
#if RU_INCLUDE_DESC
    "XGTC_HEADER_IND Register",
    "configure the ind[7:1] field in the XGTC header (for future options)",
#endif
    NGPON_TX_CFG_XGTC_HEADER_IND_7_1_REG_OFFSET,
    0,
    0,
    178,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_CFG_XGTC_HEADER_IND_7_1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_TX_CFG
 ******************************************************************************/
static const ru_reg_rec *NGPON_TX_CFG_REGS[] =
{
    &NGPON_TX_CFG_ONU_TX_EN_REG,
    &NGPON_TX_CFG_FUNC_EN_REG,
    &NGPON_TX_CFG_DAT_POL_REG,
    &NGPON_TX_CFG_TX_RNG_DLY_REG,
    &NGPON_TX_CFG_DV_CFG_REG,
    &NGPON_TX_CFG_DV_SETUP_PAT_REG,
    &NGPON_TX_CFG_DV_HOLD_PAT_REG,
    &NGPON_TX_CFG_DAT_PAT_CFG_REG,
    &NGPON_TX_CFG_DAT_PAT_REG,
    &NGPON_TX_CFG_DAT_SETUP_PAT_REG,
    &NGPON_TX_CFG_DAT_HOLD_PAT_REG,
    &NGPON_TX_CFG_TCONT_TO_ONU_ID_MAP_REG,
    &NGPON_TX_CFG_TCONT_FLUSH_REG,
    &NGPON_TX_CFG_ONU_ID_TO_PLM_MAP_REG,
    &NGPON_TX_CFG_TX_FIFO_GRP_CFG_REG,
    &NGPON_TX_CFG_TX_FIFO_Q_CFG_REG,
    &NGPON_TX_CFG_PD_Q_CFG_REG,
    &NGPON_TX_CFG_PD_GRP_CFG_REG,
    &NGPON_TX_CFG_DBR_CFG_REG,
    &NGPON_TX_CFG_DBR_REG,
    &NGPON_TX_CFG_XGEM_PYLD_MIN_LEN_REG,
    &NGPON_TX_CFG_TCONT_TO_CNT_GRP_MAP_REG,
    &NGPON_TX_CFG_PLM_CFG_REG,
    &NGPON_TX_CFG_ROGUE_ONU_CTRL_REG,
    &NGPON_TX_CFG_ROGUE_LEVEL_TIME_REG,
    &NGPON_TX_CFG_ROGUE_DIFF_TIME_REG,
    &NGPON_TX_CFG_LPB_Q_NUM_REG,
    &NGPON_TX_CFG_XGEM_HDR_OPT_REG,
    &NGPON_TX_CFG_PLM_0_STAT_REG,
    &NGPON_TX_CFG_LINE_RATE_REG,
    &NGPON_TX_CFG_START_OFFSET_REG,
    &NGPON_TX_CFG_FRAME_LENGTH_MINUS_1_REG,
    &NGPON_TX_CFG_IDLE_XGEM_PYLD_LEN_REG,
    &NGPON_TX_CFG_AF_ERR_FILT_REG,
    &NGPON_TX_CFG_DYING_GASP_REG,
    &NGPON_TX_CFG_FRAGMENTATION_DISABLE_REG,
    &NGPON_TX_CFG_XGTC_HEADER_IND_7_1_REG,
};

static unsigned long NGPON_TX_CFG_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80168000,
#elif defined(CONFIG_BCM96856)
    0x82db8000,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_TX_CFG_BLOCK = 
{
    "NGPON_TX_CFG",
    NGPON_TX_CFG_ADDRS,
    1,
    37,
    NGPON_TX_CFG_REGS
};

/* End of file BCM6858_B0_NGPON_TX_CFG.c */
