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
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_BDLY
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_BDLY_FIELD =
{
    "BDLY",
#if RU_INCLUDE_DESC
    "Tx_Byte_Delay",
    "Number of bytes delay between RX to TX frames",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_BDLY_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_BDLY_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_BDLY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED0_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_FDLY
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_FDLY_FIELD =
{
    "FDLY",
#if RU_INCLUDE_DESC
    "Tx_Frame_Delay",
    "Number of frames delay between RX to TX frames",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_FDLY_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_FDLY_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_FDLY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_SDLY
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_SDLY_FIELD =
{
    "SDLY",
#if RU_INCLUDE_DESC
    "Tx_Bit_Delay",
    "Number of bits delay between RX to TX frames",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_SDLY_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_SDLY_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_SDLY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED1_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED1_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TXEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TXEN_FIELD =
{
    "TXEN",
#if RU_INCLUDE_DESC
    "Transmitter_enable",
    "Enable the accesses transmission and tx fifo operation. If the flag is deactivated during operation, then the  transmittor stops transmissions, flushes awaiting accesses, and tx fifo queues. To ensure a correct gpon enabling sequence, tx unit must be enabled prior to rx unit.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TXEN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TXEN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TXEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_SCREN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_SCREN_FIELD =
{
    "SCREN",
#if RU_INCLUDE_DESC
    "Scrambler_enable",
    "Enable the upstream scrambler operation. This register can be updated only when tx is disabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_SCREN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_SCREN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_SCREN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_BIPEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_BIPEN_FIELD =
{
    "BIPEN",
#if RU_INCLUDE_DESC
    "BIP8_enable",
    "Enable BIP8 generation for upstream traffic. This register can be updated only when tx is disabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_BIPEN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_BIPEN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_BIPEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_FECEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_FECEN_FIELD =
{
    "FECEN",
#if RU_INCLUDE_DESC
    "FEC_enable",
    "Enable the upstream FEC encoder operation. This register can be updated only when tx is disabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_FECEN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_FECEN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_FECEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_LOOPBEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_LOOPBEN_FIELD =
{
    "LOOPBEN",
#if RU_INCLUDE_DESC
    "Loop_back_enable",
    "Enable RX-TX loopback operation. This register can be updated only when tx is disabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_LOOPBEN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_LOOPBEN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_LOOPBEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_PLSEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_PLSEN_FIELD =
{
    "PLSEN",
#if RU_INCLUDE_DESC
    "Power_level_sequence_enable",
    "Enable transmitter append of PLS sequence to ranging transmissions. This register can be updated only when tx is enabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_PLSEN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_PLSEN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_PLSEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TDMEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TDMEN_FIELD =
{
    "TDMEN",
#if RU_INCLUDE_DESC
    "TDM_enable",
    "Enable TDM port. This register can be updated only when tx is disabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TDMEN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TDMEN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TDMEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_MISCTXEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_MISCTXEN_FIELD =
{
    "MISCTXEN",
#if RU_INCLUDE_DESC
    "Misc_transmission_enable",
    "Enable TX independent transmission according to access configured by mips. This register can be updated only when tx is disabled. For more information regarding misc accesses, refer to registers MISCAC1 and MISCAC2",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_MISCTXEN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_MISCTXEN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_MISCTXEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RNGEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RNGEN_FIELD =
{
    "RNGEN",
#if RU_INCLUDE_DESC
    "Ranging_enable",
    "When this bit is set and an sn request/ranging access is received then the ONU will transmit the ploam ranging buffer. This register can be updated when tx is enabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RNGEN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RNGEN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RNGEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RESERVED0_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_DOUTPOL
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_DOUTPOL_FIELD =
{
    "DOUTPOL",
#if RU_INCLUDE_DESC
    "TX_data_out_polarity",
    "When this bit is asserted, the output data bus is bitwise inverted",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_DOUTPOL_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_DOUTPOL_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_DOUTPOL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_RESERVED0_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP0
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP0_FIELD =
{
    "PREP0",
#if RU_INCLUDE_DESC
    "Ploam_repetition_0",
    "This field contains the number of times in which a single normal ploam transmission will be repeated for ploam 0 before transmitter will clear the ploam buffer valid indication. This register can be updated only when the corresponding valid bit (see TPVLD reg) is inactive.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP0_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP0_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP1
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP1_FIELD =
{
    "PREP1",
#if RU_INCLUDE_DESC
    "Ploam_repetition_1",
    "This field contains the number of times in which a single normal ploam transmission will be repeated for ploam 1 before transmitter will clear the ploam buffer valid indication. This register can be updated only when the corresponding valid bit (see TPVLD reg) is inactive.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP1_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP1_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP2
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP2_FIELD =
{
    "PREP2",
#if RU_INCLUDE_DESC
    "Ploam_repetition_2",
    "This field contains the number of times in which a single normal ploam transmission will be repeated for ploam 2 before transmitter will clear the ploam buffer valid indication. This register can be updated only when the corresponding valid bit (see TPVLD reg) is inactive.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP2_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP2_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP3
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP3_FIELD =
{
    "PREP3",
#if RU_INCLUDE_DESC
    "Ploam_repetition_3",
    "This field contains the number of times in which a single normal ploam transmission will be repeated for ploam 3 before transmitter will clear the ploam buffer valid indication. This register can be updated only when the corresponding valid bit (see TPVLD reg) is inactive.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP3_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP3_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_RESERVED0_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_NPVLD
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_NPVLD_FIELD =
{
    "NPVLD",
#if RU_INCLUDE_DESC
    "Normal_PLOAM_valid",
    "When set this register informs the transmitter that normal PLOAM buffer is ready for transmission. The register can be updated by SW only when the appropriate valid bit is inactive.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_NPVLD_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_NPVLD_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_NPVLD_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_UPLVD
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_UPLVD_FIELD =
{
    "UPLVD",
#if RU_INCLUDE_DESC
    "Urgent_PLOAM_valid",
    "When set, this flag informs the transmitter that urgent PLOAM buffer is ready for transmission. The register can be updated by SW only when the appropriate valid bit is inactive.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_UPLVD_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_UPLVD_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_UPLVD_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_RESERVED0_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_DBAV
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_DBAV_FIELD =
{
    "DBAV",
#if RU_INCLUDE_DESC
    "DBA_valid",
    "When set, this field informs the transmitter that DBA0 status report is ready for transmission.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_DBAV_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_DBAV_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_DBAV_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBAV
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBAV_FIELD =
{
    "DBAV",
#if RU_INCLUDE_DESC
    "DBRvalid",
    "handshake signals for DBR update for tconts 32-39(see register TDBVLD)",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBAV_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBAV_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBAV_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBR_GEN_EN
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBR_GEN_EN_FIELD =
{
    "DBR_GEN_EN",
#if RU_INCLUDE_DESC
    "report_gen_en",
    "When this bit is set, the HW will generate the DBR report by accumulating the data in the FE buffer, the data in the BBH and the data in the Runners queues as received in the Runner DBR reports."
    ""
    "If not set, the DBR generation should be done by SW",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBR_GEN_EN_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBR_GEN_EN_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBR_GEN_EN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DIVRATE
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DIVRATE_FIELD =
{
    "DIVRATE",
#if RU_INCLUDE_DESC
    "divide_ratio",
    "selects the resolution of the HW DBR report."
    ""
    "Possible values:"
    "2b00: 32bytes"
    "2b01: 48bytes"
    "2b10: 64bytes"
    "2b11: reserved",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DIVRATE_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DIVRATE_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DIVRATE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBFLUSH
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBFLUSH_FIELD =
{
    "DBFLUSH",
#if RU_INCLUDE_DESC
    "DBR_flush",
    "When set, the dbr counters are set to zero",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBFLUSH_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBFLUSH_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBFLUSH_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_RESERVED0_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_TPCLRC
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_TPCLRC_FIELD =
{
    "TPCLRC",
#if RU_INCLUDE_DESC
    "TX_ploam_clear_conetent",
    "Clears NPREP and TPVLD regs",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_TPCLRC_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_TPCLRC_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_TPCLRC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_RESERVED0_FIELD_WIDTH,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_FIELDS[] =
{
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_BDLY_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED0_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_FDLY_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_SDLY_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_REG = 
{
    "TX_GENERAL_TRANSMITTER_CONTROL_TDLY",
#if RU_INCLUDE_DESC
    "TX_RANGING_DELAY Register",
    "This register contains the Ted - Transmit equalization delay, which is the total delay from the RX frame to the TX frame. This value is determined by the OLT during the ranging process. The software should translate the number to frames, bytes & bits, and then configure the register. This register may be modified while TX is enabled given a space of 3 frames from last allocation for that ONU",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_REG_OFFSET,
    0,
    0,
    0,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_FIELDS[] =
{
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TXEN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_SCREN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_BIPEN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_FECEN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_LOOPBEN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_PLSEN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_TDMEN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_MISCTXEN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RNGEN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_REG = 
{
    "TX_GENERAL_TRANSMITTER_CONTROL_TEN",
#if RU_INCLUDE_DESC
    "TX_ENABLE_REGISTER Register",
    "This register contains the enable-bits for the different transmittor functions"
    "Writing the same value to one of the bits is allowed and does not affect functionality.  For example, if BIP is enabled, and the SW writes 1 to this bit again, the BIP will stay enabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_REG_OFFSET,
    0,
    0,
    1,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    10,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_FIELDS[] =
{
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_DOUTPOL_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_REG = 
{
    "TX_GENERAL_TRANSMITTER_CONTROL_POL",
#if RU_INCLUDE_DESC
    "DATA_POLARITY Register",
    "This resister is used to determine the polarity of the transmitted data.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_REG_OFFSET,
    0,
    0,
    2,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_FIELDS[] =
{
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP0_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP1_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP2_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_PREP3_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_REG = 
{
    "TX_GENERAL_TRANSMITTER_CONTROL_NPREP",
#if RU_INCLUDE_DESC
    "NORMAL_PLOAM_REPETITION Register",
    "This register enables configuring  the number of times in which a single normal ploam transmission will be repeated before clearing the matching valid indication in TPVLD register. The actual number of ploam transmission repetition will be the configured value + 1."
    "Updating the register is possible only when the matching valid bit is cleared.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_REG_OFFSET,
    0,
    0,
    3,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_FIELDS[] =
{
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_NPVLD_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_UPLVD_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_REG = 
{
    "TX_GENERAL_TRANSMITTER_CONTROL_TPVLD",
#if RU_INCLUDE_DESC
    "TX_PLOAM_VALID_INFORMATION Register",
    "This register enables the hardware software handshake for sending PLOAM buffers. The software asserts a valid bit when a write cycle with value 1 to the appropriate valid bit. (SW write cycle with 0 value does not affect the register). The hardware clears the appropriate bit  to 0 value  after transmitting the buffer  NPREP+1 transmissions  or once if NPREP is set to zero (see also  NPREP reg for details).",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_REG_OFFSET,
    0,
    0,
    4,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_FIELDS[] =
{
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_DBAV_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_REG = 
{
    "TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD",
#if RU_INCLUDE_DESC
    "TX_DBR_VALID_INFORMATION Register",
    "This register enables the hardware software handshake for sending DBA buffers. The SW asserts/deasserts  the valid bits. Each bit corresponds to one tcont. Lsb - tcont0, msb tcont31.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_REG_OFFSET,
    0,
    0,
    5,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_FIELDS[] =
{
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBAV_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBR_GEN_EN_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DIVRATE_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_DBFLUSH_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_REG = 
{
    "TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF",
#if RU_INCLUDE_DESC
    "TX_DBR_CONFIGURATION Register",
    "This register contains some configuration bits regarding the DBR mechanism, and the rest of the valid bits for tconts 32-39."
    "The RDBAEN (bit16) is configuring which source update the DBA registers. When de-asserted, the DBA registers are under mips responsibility. When asserted, the Runner is responsible for updating the DBA registers. In this case, the MIPS may only read these registers. This bit should not be configured on the fly."
    "The stop index is used to determine the size of the report, when the runner is updating the DBR. It is used for enabling work with less than 40 TCONTS, without updating a report for all the 40 TCONTS on every update."
    "The flush bit is used to reset the DBR pointers used by the hw when the runner updates the DBR."
    "This register can be updated when TX is enabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_REG_OFFSET,
    0,
    0,
    6,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_FIELDS[] =
{
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_TPCLRC_FIELD,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_REG = 
{
    "TX_GENERAL_TRANSMITTER_CONTROL_TPCLR",
#if RU_INCLUDE_DESC
    "TX_PLOAM_CLEAR_CONTENT Register",
    "The SW may use this register in order to  clear TPVLD and NPREP reg.  This register can be updated when TX is enabled.",
#endif
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_REG_OFFSET,
    0,
    0,
    7,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_TX_GEN_CTRL
 ******************************************************************************/
static const ru_reg_rec *GPON_TX_GEN_CTRL_REGS[] =
{
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDLY_REG,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TEN_REG,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_POL_REG,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_NPREP_REG,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPVLD_REG,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD_REG,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF_REG,
    &GPON_TX_GEN_CTRL_TX_GENERAL_TRANSMITTER_CONTROL_TPCLR_REG,
};

unsigned long GPON_TX_GEN_CTRL_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130fa000,
#else
    0x80148000,
#endif
};

const ru_block_rec GPON_TX_GEN_CTRL_BLOCK = 
{
    "GPON_TX_GEN_CTRL",
    GPON_TX_GEN_CTRL_ADDRS,
    1,
    8,
    GPON_TX_GEN_CTRL_REGS
};

/* End of file BCM6858_A0GPON_TX_GEN_CTRL.c */
