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
 * Field: GPON_RX_IRQ_GRXISR_RXFIFO
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_RXFIFO_FIELD =
{
    "RXFIFO",
#if RU_INCLUDE_DESC
    "Rx_FIFO_Full_IRQ",
    "Indicates that the Rx FIFO is full.",
#endif
    GPON_RX_IRQ_GRXISR_RXFIFO_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_RXFIFO_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_RXFIFO_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXISR_TDMFIFO
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_TDMFIFO_FIELD =
{
    "TDMFIFO",
#if RU_INCLUDE_DESC
    "TDM_FIFO_Full_IRQ",
    "Indicates that the TDM FIFO is full",
#endif
    GPON_RX_IRQ_GRXISR_TDMFIFO_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_TDMFIFO_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_TDMFIFO_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXISR_AES
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_AES_FIELD =
{
    "AES",
#if RU_INCLUDE_DESC
    "AES_Error_IRQ",
    "Indicates that no AES machines could be allocated to decrypt a fragment",
#endif
    GPON_RX_IRQ_GRXISR_AES_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_AES_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_AES_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXISR_ACCFIFO
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_ACCFIFO_FIELD =
{
    "ACCFIFO",
#if RU_INCLUDE_DESC
    "Access_FIFO_Full_IRQ",
    "Indicates that the Access FIFO is full",
#endif
    GPON_RX_IRQ_GRXISR_ACCFIFO_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_ACCFIFO_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_ACCFIFO_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXISR_LOFCHNG
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_LOFCHNG_FIELD =
{
    "LOFCHNG",
#if RU_INCLUDE_DESC
    "LOF_state_change_IRQ",
    "Indicates that the LOF alarm was asserted/de-asserted",
#endif
    GPON_RX_IRQ_GRXISR_LOFCHNG_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_LOFCHNG_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_LOFCHNG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXISR_LCDGCHNG
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_LCDGCHNG_FIELD =
{
    "LCDGCHNG",
#if RU_INCLUDE_DESC
    "LCDG_state_change_IRQ",
    "Indicates that the LCDG alarm was asserted/de-asserted",
#endif
    GPON_RX_IRQ_GRXISR_LCDGCHNG_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_LCDGCHNG_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_LCDGCHNG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXISR_FECCHNG
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_FECCHNG_FIELD =
{
    "FECCHNG",
#if RU_INCLUDE_DESC
    "FEC_State_change_IRQ",
    "Indicates that FEC decoding was activated/de-activated",
#endif
    GPON_RX_IRQ_GRXISR_FECCHNG_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_FECCHNG_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_FECCHNG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXISR_TOD
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_TOD_FIELD =
{
    "TOD",
#if RU_INCLUDE_DESC
    "TOD_update_IRQ",
    "An update of the TOD counters occured",
#endif
    GPON_RX_IRQ_GRXISR_TOD_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_TOD_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_TOD_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXISR_FWI
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_FWI_FIELD =
{
    "FWI",
#if RU_INCLUDE_DESC
    "FWI_state_change",
    "forced_wakeup_indication state change",
#endif
    GPON_RX_IRQ_GRXISR_FWI_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_FWI_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_FWI_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXISR_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXISR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_IRQ_GRXISR_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXISR_RESERVED0_FIELD_WIDTH,
    GPON_RX_IRQ_GRXISR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_RXFIFO
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_RXFIFO_FIELD =
{
    "RXFIFO",
#if RU_INCLUDE_DESC
    "Rx_FIFO_Full_IRQ_mask",
    "Controls IRQ assertion due to Rx FIFO Full",
#endif
    GPON_RX_IRQ_GRXIER_RXFIFO_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_RXFIFO_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_RXFIFO_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_TDMFIFO
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_TDMFIFO_FIELD =
{
    "TDMFIFO",
#if RU_INCLUDE_DESC
    "TDM_FIFO_Full_IRQ_mask",
    "Controls IRQ assertion due to TDM FIFO Full",
#endif
    GPON_RX_IRQ_GRXIER_TDMFIFO_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_TDMFIFO_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_TDMFIFO_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_AES
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_AES_FIELD =
{
    "AES",
#if RU_INCLUDE_DESC
    "AES_Error_IRQ_mask",
    "Controls IRQ assertion due to AES allocation error",
#endif
    GPON_RX_IRQ_GRXIER_AES_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_AES_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_AES_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_ACCFIFO
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_ACCFIFO_FIELD =
{
    "ACCFIFO",
#if RU_INCLUDE_DESC
    "Access_FIFO_Full_IRQ_mask",
    "Controls IRQ assertion due to Access FIFO Full errors",
#endif
    GPON_RX_IRQ_GRXIER_ACCFIFO_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_ACCFIFO_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_ACCFIFO_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_LOFCHNG
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_LOFCHNG_FIELD =
{
    "LOFCHNG",
#if RU_INCLUDE_DESC
    "LOF_change_IRQ_mask",
    "Controls IRQ assertion due to LOF change",
#endif
    GPON_RX_IRQ_GRXIER_LOFCHNG_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_LOFCHNG_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_LOFCHNG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_LCDGCHNG
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_LCDGCHNG_FIELD =
{
    "LCDGCHNG",
#if RU_INCLUDE_DESC
    "LCDG_change_IRQ_mask",
    "Controls IRQ assertion due to LCDG change",
#endif
    GPON_RX_IRQ_GRXIER_LCDGCHNG_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_LCDGCHNG_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_LCDGCHNG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_FECCHNG
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_FECCHNG_FIELD =
{
    "FECCHNG",
#if RU_INCLUDE_DESC
    "FEC_state_change_IRQ_mask",
    "Controls IRQ assertion due to FEC state change",
#endif
    GPON_RX_IRQ_GRXIER_FECCHNG_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_FECCHNG_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_FECCHNG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_TOD
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_TOD_FIELD =
{
    "TOD",
#if RU_INCLUDE_DESC
    "TOD_update_IRQ_mask",
    "Controls IRQ assertion due to TOD update",
#endif
    GPON_RX_IRQ_GRXIER_TOD_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_TOD_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_TOD_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_FWI
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_FWI_FIELD =
{
    "FWI",
#if RU_INCLUDE_DESC
    "FWI_change_IRQ_mask",
    "Controls IRQ assertion due to FWI change",
#endif
    GPON_RX_IRQ_GRXIER_FWI_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_FWI_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_FWI_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXIER_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXIER_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_IRQ_GRXIER_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXIER_RESERVED0_FIELD_WIDTH,
    GPON_RX_IRQ_GRXIER_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXITR_IST
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXITR_IST_FIELD =
{
    "IST",
#if RU_INCLUDE_DESC
    "Interrupt_simulation_test",
    "Each bit in the mask tests the corresponding interrupt source in the ISR. A value of 1 in a bit simulates an active interrupt.",
#endif
    GPON_RX_IRQ_GRXITR_IST_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXITR_IST_FIELD_WIDTH,
    GPON_RX_IRQ_GRXITR_IST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_RX_IRQ_GRXITR_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_RX_IRQ_GRXITR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_RX_IRQ_GRXITR_RESERVED0_FIELD_MASK,
    0,
    GPON_RX_IRQ_GRXITR_RESERVED0_FIELD_WIDTH,
    GPON_RX_IRQ_GRXITR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_IRQ_GRXISR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_IRQ_GRXISR_FIELDS[] =
{
    &GPON_RX_IRQ_GRXISR_RXFIFO_FIELD,
    &GPON_RX_IRQ_GRXISR_TDMFIFO_FIELD,
    &GPON_RX_IRQ_GRXISR_AES_FIELD,
    &GPON_RX_IRQ_GRXISR_ACCFIFO_FIELD,
    &GPON_RX_IRQ_GRXISR_LOFCHNG_FIELD,
    &GPON_RX_IRQ_GRXISR_LCDGCHNG_FIELD,
    &GPON_RX_IRQ_GRXISR_FECCHNG_FIELD,
    &GPON_RX_IRQ_GRXISR_TOD_FIELD,
    &GPON_RX_IRQ_GRXISR_FWI_FIELD,
    &GPON_RX_IRQ_GRXISR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_IRQ_GRXISR_REG = 
{
    "GRXISR",
#if RU_INCLUDE_DESC
    "INTERRUPT_STATUS_REGISTER Register",
    "This register contains the current active receiver  interrupts. Each asserted bit represents an active interrupt source. The inetrrupt remains active until the software clears it by writing 1 to the corresponding bit. Writing 0 to any bit has no effect."
    "All the field in this register can be changed on-the-fly during operation.",
#endif
    GPON_RX_IRQ_GRXISR_REG_OFFSET,
    0,
    0,
    91,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    10,
    GPON_RX_IRQ_GRXISR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_IRQ_GRXIER
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_IRQ_GRXIER_FIELDS[] =
{
    &GPON_RX_IRQ_GRXIER_RXFIFO_FIELD,
    &GPON_RX_IRQ_GRXIER_TDMFIFO_FIELD,
    &GPON_RX_IRQ_GRXIER_AES_FIELD,
    &GPON_RX_IRQ_GRXIER_ACCFIFO_FIELD,
    &GPON_RX_IRQ_GRXIER_LOFCHNG_FIELD,
    &GPON_RX_IRQ_GRXIER_LCDGCHNG_FIELD,
    &GPON_RX_IRQ_GRXIER_FECCHNG_FIELD,
    &GPON_RX_IRQ_GRXIER_TOD_FIELD,
    &GPON_RX_IRQ_GRXIER_FWI_FIELD,
    &GPON_RX_IRQ_GRXIER_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_IRQ_GRXIER_REG = 
{
    "GRXIER",
#if RU_INCLUDE_DESC
    "INTERRUPT_ENABLE_REGISTER Register",
    "This register provides an enable mask for each of the interrupt sources depicted in the ISR register. Writing 1 to a bit in this register enables assertion of the IRQ line when the corresponding interrupt is active. Writing 0 to a bit in this register masks assertion of the IRQ line when the corresponding interrupt is active."
    "All the fields in this register can be changed on-the-fly during operation.",
#endif
    GPON_RX_IRQ_GRXIER_REG_OFFSET,
    0,
    0,
    92,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    10,
    GPON_RX_IRQ_GRXIER_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_IRQ_GRXITR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_IRQ_GRXITR_FIELDS[] =
{
    &GPON_RX_IRQ_GRXITR_IST_FIELD,
    &GPON_RX_IRQ_GRXITR_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_IRQ_GRXITR_REG = 
{
    "GRXITR",
#if RU_INCLUDE_DESC
    "INTERRUPT_TEST_REGISTER Register",
    "This register enables testing by simulating interrupt sources. When the software sets a bit in the ITR, the corresponding bit in the ISR shows an active interrupt. The interrupt remains active untill software clears the bit in the ITR and the ISR."
    "All the fields in this register can be changed on-the-fly during operation.",
#endif
    GPON_RX_IRQ_GRXITR_REG_OFFSET,
    0,
    0,
    93,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_IRQ_GRXITR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_IRQ
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_IRQ_REGS[] =
{
    &GPON_RX_IRQ_GRXISR_REG,
    &GPON_RX_IRQ_GRXIER_REG,
    &GPON_RX_IRQ_GRXITR_REG,
};

unsigned long GPON_RX_IRQ_ADDRS[] =
{
#ifndef CONFIG_BCM96858
    0x130f9010,
#else
    0x80150010,
#endif
};

const ru_block_rec GPON_RX_IRQ_BLOCK = 
{
    "GPON_RX_IRQ",
    GPON_RX_IRQ_ADDRS,
    1,
    3,
    GPON_RX_IRQ_REGS
};

/* End of file BCM6858_A0GPON_RX_IRQ.c */
