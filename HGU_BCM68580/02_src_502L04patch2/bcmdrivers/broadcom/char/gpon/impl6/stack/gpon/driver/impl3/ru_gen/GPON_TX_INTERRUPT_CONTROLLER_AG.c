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
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_TDMIRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_TDMIRQ_FIELD =
{
    "TDMIRQ",
#if RU_INCLUDE_DESC
    "TDM_FIFO_error",
    "TDM FIFO overrun. No consequent action is needed.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_TDMIRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_TDMIRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_TDMIRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_ACCIRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_ACCIRQ_FIELD =
{
    "ACCIRQ",
#if RU_INCLUDE_DESC
    "Access_FIFO_error",
    "Access FIFO error. No consequent action is needed",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_ACCIRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_ACCIRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_ACCIRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_NPLMIRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_NPLMIRQ_FIELD =
{
    "NPLMIRQ",
#if RU_INCLUDE_DESC
    "Normal_PLOAM_transmitted",
    "Normal PLOAM has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_NPLMIRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_NPLMIRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_NPLMIRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_UPLMIRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_UPLMIRQ_FIELD =
{
    "UPLMIRQ",
#if RU_INCLUDE_DESC
    "Urgent_PLOAM_transmitted",
    "Urgent PLOAM has been transmitted.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_UPLMIRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_UPLMIRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_UPLMIRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_IPLMIRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_IPLMIRQ_FIELD =
{
    "IPLMIRQ",
#if RU_INCLUDE_DESC
    "Idle_PLOAM_transmitted",
    "Idle PLOAM has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_IPLMIRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_IPLMIRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_IPLMIRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_RPLMIRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_RPLMIRQ_FIELD =
{
    "RPLMIRQ",
#if RU_INCLUDE_DESC
    "Ranging_PLOAM_transmitted",
    "Ranging PLOAM has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_RPLMIRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_RPLMIRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_RPLMIRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB0IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB0IRQ_FIELD =
{
    "DB0IRQ",
#if RU_INCLUDE_DESC
    "DBA0_transmitted",
    "DBA0 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB0IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB0IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB0IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB1IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB1IRQ_FIELD =
{
    "DB1IRQ",
#if RU_INCLUDE_DESC
    "DBA1_transmitted",
    "DBA1 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB1IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB1IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB1IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB2IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB2IRQ_FIELD =
{
    "DB2IRQ",
#if RU_INCLUDE_DESC
    "DBA2_transmitted",
    "DBA1 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB2IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB2IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB2IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB3IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB3IRQ_FIELD =
{
    "DB3IRQ",
#if RU_INCLUDE_DESC
    "DBA3_transmitted",
    "DBA3 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB3IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB3IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB3IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB4IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB4IRQ_FIELD =
{
    "DB4IRQ",
#if RU_INCLUDE_DESC
    "DBA4_transmitted",
    "DBA4 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB4IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB4IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB4IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB5IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB5IRQ_FIELD =
{
    "DB5IRQ",
#if RU_INCLUDE_DESC
    "DBA5_transmitted",
    "DBA5 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB5IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB5IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB5IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB6IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB6IRQ_FIELD =
{
    "DB6IRQ",
#if RU_INCLUDE_DESC
    "DBA6_transmitted",
    "DBA6 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB6IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB6IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB6IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB7IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB7IRQ_FIELD =
{
    "DB7IRQ",
#if RU_INCLUDE_DESC
    "DBA7_transmitted",
    "DBA7 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB7IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB7IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB7IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB8IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB8IRQ_FIELD =
{
    "DB8IRQ",
#if RU_INCLUDE_DESC
    "DBA8_transmitted",
    "DBA8 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB8IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB8IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB8IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB9IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB9IRQ_FIELD =
{
    "DB9IRQ",
#if RU_INCLUDE_DESC
    "DBA9_transmitted",
    "DBA9 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB9IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB9IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB9IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB10IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB10IRQ_FIELD =
{
    "DB10IRQ",
#if RU_INCLUDE_DESC
    "DBA10_transmitted",
    "DBA10 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB10IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB10IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB10IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB11IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB11IRQ_FIELD =
{
    "DB11IRQ",
#if RU_INCLUDE_DESC
    "DBA11_transmitted",
    "DBA11 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB11IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB11IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB11IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB12IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB12IRQ_FIELD =
{
    "DB12IRQ",
#if RU_INCLUDE_DESC
    "DBA12_transmitted",
    "DBA12 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB12IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB12IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB12IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB13IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB13IRQ_FIELD =
{
    "DB13IRQ",
#if RU_INCLUDE_DESC
    "DBA13_transmitted",
    "DBA13 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB13IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB13IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB13IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB14IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB14IRQ_FIELD =
{
    "DB14IRQ",
#if RU_INCLUDE_DESC
    "DBA14_transmitted",
    "DBA14 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB14IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB14IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB14IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB15IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB15IRQ_FIELD =
{
    "DB15IRQ",
#if RU_INCLUDE_DESC
    "DBA15_transmitted",
    "DBA15 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB15IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB15IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB15IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB16IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB16IRQ_FIELD =
{
    "DB16IRQ",
#if RU_INCLUDE_DESC
    "DBA16_transmitted",
    "DBA16 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB16IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB16IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB16IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB17IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB17IRQ_FIELD =
{
    "DB17IRQ",
#if RU_INCLUDE_DESC
    "DBA17_transmitted",
    "DBA17 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB17IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB17IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB17IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB18IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB18IRQ_FIELD =
{
    "DB18IRQ",
#if RU_INCLUDE_DESC
    "DBA18_transmitted",
    "DBA18 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB18IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB18IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB18IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB19IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB19IRQ_FIELD =
{
    "DB19IRQ",
#if RU_INCLUDE_DESC
    "DBA19_transmitted",
    "DBA19 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB19IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB19IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB19IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB20IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB20IRQ_FIELD =
{
    "DB20IRQ",
#if RU_INCLUDE_DESC
    "DBA20_transmitted",
    "DBA20 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB20IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB20IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB20IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB21IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB21IRQ_FIELD =
{
    "DB21IRQ",
#if RU_INCLUDE_DESC
    "DBA21_transmitted",
    "DBA21 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB21IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB21IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB21IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB22IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB22IRQ_FIELD =
{
    "DB22IRQ",
#if RU_INCLUDE_DESC
    "DBA22_transmitted",
    "DBA22 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB22IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB22IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB22IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB23IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB23IRQ_FIELD =
{
    "DB23IRQ",
#if RU_INCLUDE_DESC
    "DBA23_transmitted",
    "DBA23 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB23IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB23IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB23IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB24IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB24IRQ_FIELD =
{
    "DB24IRQ",
#if RU_INCLUDE_DESC
    "DBA24_transmitted",
    "DBA24 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB24IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB24IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB24IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB25IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB25IRQ_FIELD =
{
    "DB25IRQ",
#if RU_INCLUDE_DESC
    "DBA25_transmitted",
    "DBA25 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB25IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB25IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB25IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB26IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB26IRQ_FIELD =
{
    "DB26IRQ",
#if RU_INCLUDE_DESC
    "DBA26_transmitted",
    "DBA26 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB26IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB26IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB26IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB27IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB27IRQ_FIELD =
{
    "DB27IRQ",
#if RU_INCLUDE_DESC
    "DBA27_transmitted",
    "DBA27 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB27IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB27IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB27IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB28IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB28IRQ_FIELD =
{
    "DB28IRQ",
#if RU_INCLUDE_DESC
    "DBA28_transmitted",
    "DBA28 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB28IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB28IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB28IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB29IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB29IRQ_FIELD =
{
    "DB29IRQ",
#if RU_INCLUDE_DESC
    "DBA29_transmitted",
    "DBA29 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB29IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB29IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB29IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB30IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB30IRQ_FIELD =
{
    "DB30IRQ",
#if RU_INCLUDE_DESC
    "DBA30_transmitted",
    "DBA30 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB30IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB30IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB30IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB31IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB31IRQ_FIELD =
{
    "DB31IRQ",
#if RU_INCLUDE_DESC
    "DBA31transmitted",
    "DBA31 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB31IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB31IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB31IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB32IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB32IRQ_FIELD =
{
    "DB32IRQ",
#if RU_INCLUDE_DESC
    "DBA32_transmitted",
    "DBA32 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB32IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB32IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB32IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB33IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB33IRQ_FIELD =
{
    "DB33IRQ",
#if RU_INCLUDE_DESC
    "DBA33_transmitted",
    "DBA33 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB33IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB33IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB33IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB34IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB34IRQ_FIELD =
{
    "DB34IRQ",
#if RU_INCLUDE_DESC
    "DBA34_transmitted",
    "DBA34 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB34IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB34IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB34IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB35IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB35IRQ_FIELD =
{
    "DB35IRQ",
#if RU_INCLUDE_DESC
    "DBA35_transmitted",
    "DBA35 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB35IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB35IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB35IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB36IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB36IRQ_FIELD =
{
    "DB36IRQ",
#if RU_INCLUDE_DESC
    "DBA36_transmitted",
    "DBA36 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB36IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB36IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB36IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB37IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB37IRQ_FIELD =
{
    "DB37IRQ",
#if RU_INCLUDE_DESC
    "DBA37_transmitted",
    "DBA37 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB37IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB37IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB37IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB38IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB38IRQ_FIELD =
{
    "DB38IRQ",
#if RU_INCLUDE_DESC
    "DBA38_transmitted",
    "DBA38 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB38IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB38IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB38IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB39IRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB39IRQ_FIELD =
{
    "DB39IRQ",
#if RU_INCLUDE_DESC
    "DBA39_transmitted",
    "DBA39 has been transmitted indication",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB39IRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB39IRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB39IRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_OVRIRQ
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_OVRIRQ_FIELD =
{
    "OVRIRQ",
#if RU_INCLUDE_DESC
    "Overrun_occurred",
    "An overrun occurred in non tdm queue. (fatal error a tx disable is required)",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_OVRIRQ_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_OVRIRQ_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_OVRIRQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_LEVEL
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_LEVEL_FIELD =
{
    "ROGUE_ONU_LEVEL",
#if RU_INCLUDE_DESC
    "rogue_onu_level",
    "Rogue ONU detected - transmission longer than predefined time window.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_LEVEL_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_LEVEL_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_LEVEL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_DIFF
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_DIFF_FIELD =
{
    "ROGUE_ONU_DIFF",
#if RU_INCLUDE_DESC
    "rogue_onu_diff",
    "Rogue ONU detected - Difference between generated TXEN and examined TXEN for longer than expected.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_DIFF_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_DIFF_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_DIFF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXISR2_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_RESERVED0_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXIER0_IEM
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXIER0_IEM_FIELD =
{
    "IEM",
#if RU_INCLUDE_DESC
    "Interrupt_enable_mask",
    "Each bit in the mask controls the corresponding interrupt source in the IER",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXIER0_IEM_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER0_IEM_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER0_IEM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXIER1_IEM
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXIER1_IEM_FIELD =
{
    "IEM",
#if RU_INCLUDE_DESC
    "Interrupt_enable_mask",
    "Each bit in the mask controls the corresponding interrupt source in the IER",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXIER1_IEM_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER1_IEM_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER1_IEM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXIER2_IEM
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXIER2_IEM_FIELD =
{
    "IEM",
#if RU_INCLUDE_DESC
    "Interrupt_enable_mask",
    "Each bit in the mask controls the corresponding interrupt source in the IER",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXIER2_IEM_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER2_IEM_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER2_IEM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXIER2_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXIER2_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXIER2_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER2_RESERVED0_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER2_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXITR0_IST
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXITR0_IST_FIELD =
{
    "IST",
#if RU_INCLUDE_DESC
    "Interrupt_simulation_test",
    "Each bit in the mask tests the corresponding interrupt source in the ISR",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXITR0_IST_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR0_IST_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR0_IST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXITR1_IST
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXITR1_IST_FIELD =
{
    "IST",
#if RU_INCLUDE_DESC
    "Interrupt_simulation_test",
    "Each bit in the mask tests the corresponding interrupt source in the ISR",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXITR1_IST_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR1_IST_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR1_IST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXITR2_IST
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXITR2_IST_FIELD =
{
    "IST",
#if RU_INCLUDE_DESC
    "Interrupt_simulation_test",
    "Each bit in the mask tests the corresponding interrupt source in the ISR",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXITR2_IST_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR2_IST_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR2_IST_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_INTERRUPT_CONTROLLER_TXITR2_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_INTERRUPT_CONTROLLER_TXITR2_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXITR2_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR2_RESERVED0_FIELD_WIDTH,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR2_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_TX_INTERRUPT_CONTROLLER_TXISR0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_INTERRUPT_CONTROLLER_TXISR0_FIELDS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_TDMIRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_ACCIRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_NPLMIRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_UPLMIRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_IPLMIRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_RPLMIRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB0IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB1IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB2IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB3IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB4IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB5IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB6IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB7IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB8IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB9IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB10IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB11IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_DB12IRQ_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR0_REG = 
{
    "TXISR0",
#if RU_INCLUDE_DESC
    "INTERRUPT_STATUS_REGISTER_0 Register",
    "This register contains the current active transmitter interrupts. Each asserted bit represents an active interrupt source. The interrupt remains active until the software clears it by writing 1 to the corresponding bit."
    "The dbr interrupts has two bits, indicating, not only interrupt, but the type of dbr (01 - 0ne byte, 10 -  two bytes, 11 - 4 bytes dbr)."
    "Writing 0 to any bit has no effect.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_REG_OFFSET,
    0,
    0,
    8,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    19,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_INTERRUPT_CONTROLLER_TXISR1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_INTERRUPT_CONTROLLER_TXISR1_FIELDS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB13IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB14IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB15IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB16IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB17IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB18IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB19IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB20IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB21IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB22IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB23IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB24IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB25IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB26IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB27IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_DB28IRQ_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR1_REG = 
{
    "TXISR1",
#if RU_INCLUDE_DESC
    "INTERRUPT_STATUS_REGISTER_1 Register",
    "This register contains the current active transmitter interrupts. Each asserted bit represents an active interrupt source. The interrupt remains active until the software clears it by writing 1 to the corresponding bit."
    "The dbr interrupts has two bits, indicating, not only interrupt, but the type of dbr (01 - 0ne byte, 10 -  two bytes, 11 - 4 bytes dbr)."
    "Writing 0 to any bit has no effect.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_REG_OFFSET,
    0,
    0,
    9,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    16,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_INTERRUPT_CONTROLLER_TXISR2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_INTERRUPT_CONTROLLER_TXISR2_FIELDS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB29IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB30IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB31IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB32IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB33IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB34IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB35IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB36IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB37IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB38IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_DB39IRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_OVRIRQ_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_LEVEL_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_ROGUE_ONU_DIFF_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_INTERRUPT_CONTROLLER_TXISR2_REG = 
{
    "TXISR2",
#if RU_INCLUDE_DESC
    "INTERRUPT_STATUS_REGISTER_2 Register",
    "This register contains the current active transmitter interrupts. Each asserted bit represents an active interrupt source. The interrupt remains active until the software clears it by writing 1 to the corresponding bit."
    "The dbr interrupts has two bits, indicating, not only interrupt, but the type of dbr (01 - 0ne byte, 10 -  two bytes, 11 - 4 bytes dbr)."
    "Writing 0 to any bit has no effect.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_REG_OFFSET,
    0,
    0,
    10,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    15,
    GPON_TX_INTERRUPT_CONTROLLER_TXISR2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_INTERRUPT_CONTROLLER_TXIER0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_INTERRUPT_CONTROLLER_TXIER0_FIELDS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXIER0_IEM_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_INTERRUPT_CONTROLLER_TXIER0_REG = 
{
    "TXIER0",
#if RU_INCLUDE_DESC
    "INTERRUPT_ENABLE_REGISTER_0 Register",
    "This register provides an enable mask for each of the interrupt sources depicted in the ISR register.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXIER0_REG_OFFSET,
    0,
    0,
    11,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_INTERRUPT_CONTROLLER_TXIER1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_INTERRUPT_CONTROLLER_TXIER1_FIELDS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXIER1_IEM_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_INTERRUPT_CONTROLLER_TXIER1_REG = 
{
    "TXIER1",
#if RU_INCLUDE_DESC
    "INTERRUPT_ENABLE_REGISTER_1 Register",
    "This register provides an enable mask for each of the interrupt sources depicted in the ISR register.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXIER1_REG_OFFSET,
    0,
    0,
    12,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_INTERRUPT_CONTROLLER_TXIER2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_INTERRUPT_CONTROLLER_TXIER2_FIELDS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXIER2_IEM_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXIER2_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_INTERRUPT_CONTROLLER_TXIER2_REG = 
{
    "TXIER2",
#if RU_INCLUDE_DESC
    "INTERRUPT_ENABLE_REGISTER_2 Register",
    "This register provides an enable mask for each of the interrupt sources depicted in the ISR register.",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXIER2_REG_OFFSET,
    0,
    0,
    13,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_INTERRUPT_CONTROLLER_TXIER2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_INTERRUPT_CONTROLLER_TXITR0
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_INTERRUPT_CONTROLLER_TXITR0_FIELDS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXITR0_IST_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_INTERRUPT_CONTROLLER_TXITR0_REG = 
{
    "TXITR0",
#if RU_INCLUDE_DESC
    "INTERRUPT_TEST_REGISTER_0 Register",
    "This register enables testing by simulating interrupt sources. When the software sets a bit in the ITR, the corresponding bit in the ISR shows an active interrupt. The interrupt remains active until software clears the bit in the ITR",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXITR0_REG_OFFSET,
    0,
    0,
    14,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR0_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_INTERRUPT_CONTROLLER_TXITR1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_INTERRUPT_CONTROLLER_TXITR1_FIELDS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXITR1_IST_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_INTERRUPT_CONTROLLER_TXITR1_REG = 
{
    "TXITR1",
#if RU_INCLUDE_DESC
    "INTERRUPT_TEST_REGISTER_1 Register",
    "This register enables testing by simulating interrupt sources. When the software sets a bit in the ITR, the corresponding bit in the ISR shows an active interrupt. The interrupt remains active until software clears the bit in the ITR",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXITR1_REG_OFFSET,
    0,
    0,
    15,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_INTERRUPT_CONTROLLER_TXITR2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_INTERRUPT_CONTROLLER_TXITR2_FIELDS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXITR2_IST_FIELD,
    &GPON_TX_INTERRUPT_CONTROLLER_TXITR2_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_INTERRUPT_CONTROLLER_TXITR2_REG = 
{
    "TXITR2",
#if RU_INCLUDE_DESC
    "INTERRUPT_TEST_REGISTER_2 Register",
    "This register enables testing by simulating interrupt sources. When the software sets a bit in the ITR, the corresponding bit in the ISR shows an active interrupt. The interrupt remains active until software clears the bit in the ITR",
#endif
    GPON_TX_INTERRUPT_CONTROLLER_TXITR2_REG_OFFSET,
    0,
    0,
    16,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_INTERRUPT_CONTROLLER_TXITR2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_TX_INTERRUPT_CONTROLLER
 ******************************************************************************/
static const ru_reg_rec *GPON_TX_INTERRUPT_CONTROLLER_REGS[] =
{
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR0_REG,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR1_REG,
    &GPON_TX_INTERRUPT_CONTROLLER_TXISR2_REG,
    &GPON_TX_INTERRUPT_CONTROLLER_TXIER0_REG,
    &GPON_TX_INTERRUPT_CONTROLLER_TXIER1_REG,
    &GPON_TX_INTERRUPT_CONTROLLER_TXIER2_REG,
    &GPON_TX_INTERRUPT_CONTROLLER_TXITR0_REG,
    &GPON_TX_INTERRUPT_CONTROLLER_TXITR1_REG,
    &GPON_TX_INTERRUPT_CONTROLLER_TXITR2_REG,
};

static unsigned long GPON_TX_INTERRUPT_CONTROLLER_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80148024,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db0024,
#else
    #error "GPON_TX_INTERRUPT_CONTROLLER base address not defined"
#endif
};

const ru_block_rec GPON_TX_INTERRUPT_CONTROLLER_BLOCK = 
{
    "GPON_TX_INTERRUPT_CONTROLLER",
    GPON_TX_INTERRUPT_CONTROLLER_ADDRS,
    1,
    9,
    GPON_TX_INTERRUPT_CONTROLLER_REGS
};

/* End of file BCM6836_A0GPON_TX_INTERRUPT_CONTROLLER.c */
