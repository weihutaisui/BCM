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
 * Field: GPON_TX_STATISTICS_COUNTERS_IAC_IACC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_IAC_IACC_FIELD =
{
    "IACC",
#if RU_INCLUDE_DESC
    "Illegal_access_counter_content",
    "Illegal access counter content. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_IAC_IACC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_IAC_IACC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_IAC_IACC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_IAC_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_IAC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_STATISTICS_COUNTERS_IAC_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_IAC_RESERVED0_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_IAC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_RAC_RACC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_RAC_RACC_FIELD =
{
    "RACC",
#if RU_INCLUDE_DESC
    "Received_access_counter_content",
    "contains the number of valid access records received by the transmitter relevant to the specific Tx queue. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_RAC_RACC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_RAC_RACC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_RAC_RACC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_RAC_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_RAC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_STATISTICS_COUNTERS_RAC_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_RAC_RESERVED0_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_RAC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_RDBC_RDBCC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_RDBC_RDBCC_FIELD =
{
    "RDBCC",
#if RU_INCLUDE_DESC
    "Requested_DBR_counter_content",
    "Details the number of DBA reports requested from the Tx queue. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_RDBC_RDBCC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_RDBC_RDBCC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_RDBC_RDBCC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_RDBC_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_RDBC_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_STATISTICS_COUNTERS_RDBC_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_RDBC_RESERVED0_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_RDBC_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_TPC_TPCC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_TPC_TPCC_FIELD =
{
    "TPCC",
#if RU_INCLUDE_DESC
    "Transmitted_packet_counter_content",
    "Transmitted packet counter of the relevant Tx queue. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_TPC_TPCC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_TPC_TPCC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_TPC_TPCC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_TGC_TGCC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_TGC_TGCC_FIELD =
{
    "TGCC",
#if RU_INCLUDE_DESC
    "Transmitted_gem_counter_content",
    "Transmitted gem counter content per the relevant Tx queue. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_TGC_TGCC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_TGC_TGCC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_TGC_TGCC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_TIC_TIC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_TIC_TIC_FIELD =
{
    "TIC",
#if RU_INCLUDE_DESC
    "Transmitted_idle_counter",
    "Transmitted idle counter of the relevant Tx queue. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_TIC_TIC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_TIC_TIC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_TIC_TIC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_RIPC_IPCC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_RIPC_IPCC_FIELD =
{
    "IPCC",
#if RU_INCLUDE_DESC
    "Idle_PLOAM_counter_content",
    "Requested idle PLOAM counter. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_RIPC_IPCC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_RIPC_IPCC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_RIPC_IPCC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_RNPC_RNPCC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_RNPC_RNPCC_FIELD =
{
    "RNPCC",
#if RU_INCLUDE_DESC
    "Requested_normal_PLOAM_counter_content",
    "Requested Normal PLOAM counter content. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_RNPC_RNPCC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_RNPC_RNPCC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_RNPC_RNPCC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_RSPC_RUPCC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_RSPC_RUPCC_FIELD =
{
    "RUPCC",
#if RU_INCLUDE_DESC
    "Requested_urgent_PLOAM_counter_content",
    "requested urgent PLOAM counter content. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_RSPC_RUPCC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_RSPC_RUPCC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_RSPC_RUPCC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_STATISTICS_COUNTERS_RSPC_RRPCC
 ******************************************************************************/
const ru_field_rec GPON_TX_STATISTICS_COUNTERS_RSPC_RRPCC_FIELD =
{
    "RRPCC",
#if RU_INCLUDE_DESC
    "Request_ranging_PLOAM_counter_content",
    "Request ranging PLOAM counter content. Freeze at max value",
#endif
    GPON_TX_STATISTICS_COUNTERS_RSPC_RRPCC_FIELD_MASK,
    0,
    GPON_TX_STATISTICS_COUNTERS_RSPC_RRPCC_FIELD_WIDTH,
    GPON_TX_STATISTICS_COUNTERS_RSPC_RRPCC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_TX_STATISTICS_COUNTERS_IAC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_STATISTICS_COUNTERS_IAC_FIELDS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_IAC_IACC_FIELD,
    &GPON_TX_STATISTICS_COUNTERS_IAC_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_STATISTICS_COUNTERS_IAC_REG = 
{
    "IAC",
#if RU_INCLUDE_DESC
    "ILLEGAL_ACCESS_COUNTER Register",
    "Number of illegal access received and discarded by the transmitter",
#endif
    GPON_TX_STATISTICS_COUNTERS_IAC_REG_OFFSET,
    0,
    0,
    78,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_STATISTICS_COUNTERS_IAC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_STATISTICS_COUNTERS_RAC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_STATISTICS_COUNTERS_RAC_FIELDS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_RAC_RACC_FIELD,
    &GPON_TX_STATISTICS_COUNTERS_RAC_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_STATISTICS_COUNTERS_RAC_REG = 
{
    "RAC",
#if RU_INCLUDE_DESC
    "RECEIVED_ACCESS_COUNTER %i Register",
    "This register array contains the number of received access per Tx queue",
#endif
    GPON_TX_STATISTICS_COUNTERS_RAC_REG_OFFSET,
    GPON_TX_STATISTICS_COUNTERS_RAC_REG_RAM_CNT,
    4,
    79,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_STATISTICS_COUNTERS_RAC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_STATISTICS_COUNTERS_RDBC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_STATISTICS_COUNTERS_RDBC_FIELDS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_RDBC_RDBCC_FIELD,
    &GPON_TX_STATISTICS_COUNTERS_RDBC_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_STATISTICS_COUNTERS_RDBC_REG = 
{
    "RDBC",
#if RU_INCLUDE_DESC
    "REQUESTED_DBR_COUNTER %i Register",
    "This register array contains the number of requested DBA reports per TX queue",
#endif
    GPON_TX_STATISTICS_COUNTERS_RDBC_REG_OFFSET,
    GPON_TX_STATISTICS_COUNTERS_RDBC_REG_RAM_CNT,
    4,
    80,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_STATISTICS_COUNTERS_RDBC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_STATISTICS_COUNTERS_TPC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_STATISTICS_COUNTERS_TPC_FIELDS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_TPC_TPCC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_STATISTICS_COUNTERS_TPC_REG = 
{
    "TPC",
#if RU_INCLUDE_DESC
    "TRANSMITTED_PACKET_COUNTER %i Register",
    "This register array contains the number of transmitted packet per Tx queue",
#endif
    GPON_TX_STATISTICS_COUNTERS_TPC_REG_OFFSET,
    GPON_TX_STATISTICS_COUNTERS_TPC_REG_RAM_CNT,
    4,
    81,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_STATISTICS_COUNTERS_TPC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_STATISTICS_COUNTERS_TGC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_STATISTICS_COUNTERS_TGC_FIELDS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_TGC_TGCC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_STATISTICS_COUNTERS_TGC_REG = 
{
    "TGC",
#if RU_INCLUDE_DESC
    "TRANSMITTED_GEM_FRAGMENTS_COUNTER %i Register",
    "This register array contains the number of transmitted gem fragments per Tx queue",
#endif
    GPON_TX_STATISTICS_COUNTERS_TGC_REG_OFFSET,
    GPON_TX_STATISTICS_COUNTERS_TGC_REG_RAM_CNT,
    4,
    82,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_STATISTICS_COUNTERS_TGC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_STATISTICS_COUNTERS_TIC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_STATISTICS_COUNTERS_TIC_FIELDS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_TIC_TIC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_STATISTICS_COUNTERS_TIC_REG = 
{
    "TIC",
#if RU_INCLUDE_DESC
    "TRANSMITTED_IDLE_COUNTER %i Register",
    "Overall transmitted idle gem per Tx queue",
#endif
    GPON_TX_STATISTICS_COUNTERS_TIC_REG_OFFSET,
    GPON_TX_STATISTICS_COUNTERS_TIC_REG_RAM_CNT,
    4,
    83,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_STATISTICS_COUNTERS_TIC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_STATISTICS_COUNTERS_RIPC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_STATISTICS_COUNTERS_RIPC_FIELDS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_RIPC_IPCC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_STATISTICS_COUNTERS_RIPC_REG = 
{
    "RIPC",
#if RU_INCLUDE_DESC
    "REQUESTED_IDLE_PLOAM_COUNTER Register",
    "Requested idle PLOAM counter",
#endif
    GPON_TX_STATISTICS_COUNTERS_RIPC_REG_OFFSET,
    0,
    0,
    84,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_STATISTICS_COUNTERS_RIPC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_STATISTICS_COUNTERS_RNPC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_STATISTICS_COUNTERS_RNPC_FIELDS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_RNPC_RNPCC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_STATISTICS_COUNTERS_RNPC_REG = 
{
    "RNPC",
#if RU_INCLUDE_DESC
    "REQUESTED_NORMAL_PLOAM_COUNTER Register",
    "Requested Normal PLOAM counter",
#endif
    GPON_TX_STATISTICS_COUNTERS_RNPC_REG_OFFSET,
    0,
    0,
    85,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_STATISTICS_COUNTERS_RNPC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_STATISTICS_COUNTERS_RSPC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_STATISTICS_COUNTERS_RSPC_FIELDS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_RSPC_RUPCC_FIELD,
    &GPON_TX_STATISTICS_COUNTERS_RSPC_RRPCC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_STATISTICS_COUNTERS_RSPC_REG = 
{
    "RSPC",
#if RU_INCLUDE_DESC
    "REQUESTED_SPECIAL_PLOAM_COUNTER Register",
    "Requested special PLOAM counter. Including urgent PLOAM counter & ranging PLOAM counter",
#endif
    GPON_TX_STATISTICS_COUNTERS_RSPC_REG_OFFSET,
    0,
    0,
    86,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_STATISTICS_COUNTERS_RSPC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_TX_STATISTICS_COUNTERS
 ******************************************************************************/
static const ru_reg_rec *GPON_TX_STATISTICS_COUNTERS_REGS[] =
{
    &GPON_TX_STATISTICS_COUNTERS_IAC_REG,
    &GPON_TX_STATISTICS_COUNTERS_RAC_REG,
    &GPON_TX_STATISTICS_COUNTERS_RDBC_REG,
    &GPON_TX_STATISTICS_COUNTERS_TPC_REG,
    &GPON_TX_STATISTICS_COUNTERS_TGC_REG,
    &GPON_TX_STATISTICS_COUNTERS_TIC_REG,
    &GPON_TX_STATISTICS_COUNTERS_RIPC_REG,
    &GPON_TX_STATISTICS_COUNTERS_RNPC_REG,
    &GPON_TX_STATISTICS_COUNTERS_RSPC_REG,
};

static unsigned long GPON_TX_STATISTICS_COUNTERS_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80148a00,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db0a00,
#else
    #error "GPON_TX_STATISTICS_COUNTER base address not defined"
#endif
};

const ru_block_rec GPON_TX_STATISTICS_COUNTERS_BLOCK = 
{
    "GPON_TX_STATISTICS_COUNTERS",
    GPON_TX_STATISTICS_COUNTERS_ADDRS,
    1,
    9,
    GPON_TX_STATISTICS_COUNTERS_REGS
};

/* End of file BCM6836_A0GPON_TX_STATISTICS_COUNTERS.c */
