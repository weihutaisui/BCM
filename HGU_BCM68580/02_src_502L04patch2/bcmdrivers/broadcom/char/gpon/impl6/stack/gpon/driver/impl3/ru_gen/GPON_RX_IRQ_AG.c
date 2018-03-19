/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.

    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.

:>
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

static unsigned long GPON_RX_IRQ_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80150010,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db1010,
#else
    #error "GPON_RX_IRQ base address not defined"
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

/* End of file BCM6836_A0GPON_RX_IRQ.c */
