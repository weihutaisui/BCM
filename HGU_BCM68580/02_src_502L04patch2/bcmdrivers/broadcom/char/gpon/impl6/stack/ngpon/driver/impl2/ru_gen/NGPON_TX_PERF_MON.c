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
 * Field: NGPON_TX_PERF_MON_ILGL_ACCS_CNT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_ILGL_ACCS_CNT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Illegal access counter content. Freeze at max value",
#endif
    NGPON_TX_PERF_MON_ILGL_ACCS_CNT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_ILGL_ACCS_CNT_VALUE_FIELD_WIDTH,
    NGPON_TX_PERF_MON_ILGL_ACCS_CNT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PERF_MON_ILGL_ACCS_CNT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_ILGL_ACCS_CNT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_PERF_MON_ILGL_ACCS_CNT_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_ILGL_ACCS_CNT_RESERVED0_FIELD_WIDTH,
    NGPON_TX_PERF_MON_ILGL_ACCS_CNT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PERF_MON_RX_ACCS_CNT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_RX_ACCS_CNT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "contains the number of valid access records received by the transmitter relevant to the specific Tx queue. Freeze at max value",
#endif
    NGPON_TX_PERF_MON_RX_ACCS_CNT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_RX_ACCS_CNT_VALUE_FIELD_WIDTH,
    NGPON_TX_PERF_MON_RX_ACCS_CNT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PERF_MON_RX_ACCS_CNT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_RX_ACCS_CNT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_PERF_MON_RX_ACCS_CNT_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_RX_ACCS_CNT_RESERVED0_FIELD_WIDTH,
    NGPON_TX_PERF_MON_RX_ACCS_CNT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PERF_MON_REQ_DBR_CNT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_REQ_DBR_CNT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Details the number of DBRu reports requested from the Tx queue. Freeze at max value",
#endif
    NGPON_TX_PERF_MON_REQ_DBR_CNT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_REQ_DBR_CNT_VALUE_FIELD_WIDTH,
    NGPON_TX_PERF_MON_REQ_DBR_CNT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PERF_MON_REQ_DBR_CNT_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_REQ_DBR_CNT_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_TX_PERF_MON_REQ_DBR_CNT_RESERVED0_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_REQ_DBR_CNT_RESERVED0_FIELD_WIDTH,
    NGPON_TX_PERF_MON_REQ_DBR_CNT_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PERF_MON_TX_PCKT_CNT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_TX_PCKT_CNT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Transmitted packet counter of the relevant Tx queue. Freeze at max value",
#endif
    NGPON_TX_PERF_MON_TX_PCKT_CNT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_TX_PCKT_CNT_VALUE_FIELD_WIDTH,
    NGPON_TX_PERF_MON_TX_PCKT_CNT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Transmitted gem counter content per the relevant Tx queue. Freeze at max value",
#endif
    NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_VALUE_FIELD_WIDTH,
    NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Transmitted idle counter of the relevant Tx queue. Freeze at max value",
#endif
    NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_VALUE_FIELD_WIDTH,
    NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_TX_PERF_MON_REQ_PLM_VALUE
 ******************************************************************************/
const ru_field_rec NGPON_TX_PERF_MON_REQ_PLM_VALUE_FIELD =
{
    "VALUE",
#if RU_INCLUDE_DESC
    "value",
    "Counts the number of requested PLOAM per PLOAM buffer.",
#endif
    NGPON_TX_PERF_MON_REQ_PLM_VALUE_FIELD_MASK,
    0,
    NGPON_TX_PERF_MON_REQ_PLM_VALUE_FIELD_WIDTH,
    NGPON_TX_PERF_MON_REQ_PLM_VALUE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_TX_PERF_MON_ILGL_ACCS_CNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PERF_MON_ILGL_ACCS_CNT_FIELDS[] =
{
    &NGPON_TX_PERF_MON_ILGL_ACCS_CNT_VALUE_FIELD,
    &NGPON_TX_PERF_MON_ILGL_ACCS_CNT_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PERF_MON_ILGL_ACCS_CNT_REG = 
{
    "ILGL_ACCS_CNT",
#if RU_INCLUDE_DESC
    "ILLEGAL_ACCESS_COUNTER Register",
    "Number of illegal access received and discarded by the transmitter."
    "The counter doesn't wrap and it is RC.",
#endif
    NGPON_TX_PERF_MON_ILGL_ACCS_CNT_REG_OFFSET,
    0,
    0,
    233,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_PERF_MON_ILGL_ACCS_CNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PERF_MON_RX_ACCS_CNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PERF_MON_RX_ACCS_CNT_FIELDS[] =
{
    &NGPON_TX_PERF_MON_RX_ACCS_CNT_VALUE_FIELD,
    &NGPON_TX_PERF_MON_RX_ACCS_CNT_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PERF_MON_RX_ACCS_CNT_REG = 
{
    "RX_ACCS_CNT",
#if RU_INCLUDE_DESC
    "RECEIVED_ACCESS_COUNTER %i Register",
    "This register array contains the number of received access per Tx queue",
#endif
    NGPON_TX_PERF_MON_RX_ACCS_CNT_REG_OFFSET,
    NGPON_TX_PERF_MON_RX_ACCS_CNT_REG_RAM_CNT,
    4,
    234,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_PERF_MON_RX_ACCS_CNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PERF_MON_REQ_DBR_CNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PERF_MON_REQ_DBR_CNT_FIELDS[] =
{
    &NGPON_TX_PERF_MON_REQ_DBR_CNT_VALUE_FIELD,
    &NGPON_TX_PERF_MON_REQ_DBR_CNT_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PERF_MON_REQ_DBR_CNT_REG = 
{
    "REQ_DBR_CNT",
#if RU_INCLUDE_DESC
    "REQUESTED_DBR_COUNTER %i Register",
    "This register array contains the number of requested DBA reports per TX queue",
#endif
    NGPON_TX_PERF_MON_REQ_DBR_CNT_REG_OFFSET,
    NGPON_TX_PERF_MON_REQ_DBR_CNT_REG_RAM_CNT,
    4,
    235,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    NGPON_TX_PERF_MON_REQ_DBR_CNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PERF_MON_TX_PCKT_CNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PERF_MON_TX_PCKT_CNT_FIELDS[] =
{
    &NGPON_TX_PERF_MON_TX_PCKT_CNT_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PERF_MON_TX_PCKT_CNT_REG = 
{
    "TX_PCKT_CNT",
#if RU_INCLUDE_DESC
    "TRANSMITTED_PACKET_COUNTER %i Register",
    "This register array contains the number of transmitted packet per Tx queue",
#endif
    NGPON_TX_PERF_MON_TX_PCKT_CNT_REG_OFFSET,
    NGPON_TX_PERF_MON_TX_PCKT_CNT_REG_RAM_CNT,
    4,
    236,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_PERF_MON_TX_PCKT_CNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_FIELDS[] =
{
    &NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_REG = 
{
    "TX_XGEM_FRM_CNT",
#if RU_INCLUDE_DESC
    "TRANSMITTED_XGEM_FRAMES_COUNTER %i Register",
    "This register array contains the number of transmitted XGEM frames per TX queue (not including XGEM IDLE frames)",
#endif
    NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_REG_OFFSET,
    NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_REG_RAM_CNT,
    4,
    237,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_FIELDS[] =
{
    &NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_REG = 
{
    "TX_XGEM_IDLE_CNT",
#if RU_INCLUDE_DESC
    "TRANSMITTED_XGEM_IDLE_FRAMES_COUNTER %i Register",
    "Overall transmitted XGEM idle frame per Tx queue",
#endif
    NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_REG_OFFSET,
    NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_REG_RAM_CNT,
    4,
    238,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_TX_PERF_MON_REQ_PLM
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_TX_PERF_MON_REQ_PLM_FIELDS[] =
{
    &NGPON_TX_PERF_MON_REQ_PLM_VALUE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_TX_PERF_MON_REQ_PLM_REG = 
{
    "REQ_PLM",
#if RU_INCLUDE_DESC
    "REQUESTED_PLOAM_COUNTER %i Register",
    "Requested PLOAM counters array. Each counter corresponds to a PLOAM buffer.",
#endif
    NGPON_TX_PERF_MON_REQ_PLM_REG_OFFSET,
    NGPON_TX_PERF_MON_REQ_PLM_REG_RAM_CNT,
    4,
    239,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    NGPON_TX_PERF_MON_REQ_PLM_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_TX_PERF_MON
 ******************************************************************************/
static const ru_reg_rec *NGPON_TX_PERF_MON_REGS[] =
{
    &NGPON_TX_PERF_MON_ILGL_ACCS_CNT_REG,
    &NGPON_TX_PERF_MON_RX_ACCS_CNT_REG,
    &NGPON_TX_PERF_MON_REQ_DBR_CNT_REG,
    &NGPON_TX_PERF_MON_TX_PCKT_CNT_REG,
    &NGPON_TX_PERF_MON_TX_XGEM_FRM_CNT_REG,
    &NGPON_TX_PERF_MON_TX_XGEM_IDLE_CNT_REG,
    &NGPON_TX_PERF_MON_REQ_PLM_REG,
};

unsigned long NGPON_TX_PERF_MON_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x8016c000,
#elif defined(CONFIG_BCM96856)
    0x82dbc000,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_TX_PERF_MON_BLOCK = 
{
    "NGPON_TX_PERF_MON",
    NGPON_TX_PERF_MON_ADDRS,
    1,
    7,
    NGPON_TX_PERF_MON_REGS
};

/* End of file BCM6858_A0_NGPON_TX_PERF_MON.c */
