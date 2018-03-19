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
 * Field: GPON_TX_FIFO_CONFIGURATION_PDP_PDBC
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_PDP_PDBC_FIELD =
{
    "PDBC",
#if RU_INCLUDE_DESC
    "Packet_descriptor_base_address_content",
    "This field contains the offset of the packet descriptor queue in the packet array. The base address of a queue must be equal or higher than the sum of all allocations to previous queues (each address represents one packet descriptor).",
#endif
    GPON_TX_FIFO_CONFIGURATION_PDP_PDBC_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_PDP_PDBC_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_PDP_PDBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_PDP_PDSC
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_PDP_PDSC_FIELD =
{
    "PDSC",
#if RU_INCLUDE_DESC
    "Packet_descriptor_size_content",
    "This field contains the number of packet descriptors allocated to the queue. (out of 128 packet descriptors in total)",
#endif
    GPON_TX_FIFO_CONFIGURATION_PDP_PDSC_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_PDP_PDSC_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_PDP_PDSC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_PDP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_PDP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_FIFO_CONFIGURATION_PDP_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_PDP_RESERVED0_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_PDP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_PDPGRP_PDBC_GRP
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_PDPGRP_PDBC_GRP_FIELD =
{
    "PDBC_GRP",
#if RU_INCLUDE_DESC
    "base_address_of_the_group",
    "base address of the group",
#endif
    GPON_TX_FIFO_CONFIGURATION_PDPGRP_PDBC_GRP_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_PDPGRP_PDBC_GRP_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_PDPGRP_PDBC_GRP_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_PDPGRP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_PDPGRP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_FIFO_CONFIGURATION_PDPGRP_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_PDPGRP_RESERVED0_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_PDPGRP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TQP_TQBC
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TQP_TQBC_FIELD =
{
    "TQBC",
#if RU_INCLUDE_DESC
    "Tx_queue_base_address_content",
    "Offset of the TX queue within the 8K Tx FIFO (11 msb out of 13 bits address, meaning 4 byte resolution).",
#endif
    GPON_TX_FIFO_CONFIGURATION_TQP_TQBC_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TQP_TQBC_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TQP_TQBC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TQP_TQSC
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TQP_TQSC_FIELD =
{
    "TQSC",
#if RU_INCLUDE_DESC
    "Tx_queue_size_content",
    "Size of TX queue within the 20K Tx FIFO",
#endif
    GPON_TX_FIFO_CONFIGURATION_TQP_TQSC_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TQP_TQSC_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TQP_TQSC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TQP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TQP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_FIFO_CONFIGURATION_TQP_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TQP_RESERVED0_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TQP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQB
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQB_FIELD =
{
    "TDQB",
#if RU_INCLUDE_DESC
    "base_address_of_the_group",
    "base address of the group",
#endif
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQB_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQB_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQB_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQS
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQS_FIELD =
{
    "TDQS",
#if RU_INCLUDE_DESC
    "size_of_the_group",
    "size of each queue in the group",
#endif
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQS_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQS_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TDQPGRP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TDQPGRP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_RESERVED0_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPID
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPID_FIELD =
{
    "TDQPID",
#if RU_INCLUDE_DESC
    "TDM_queue_PORT_ID",
    "TDM queue PORT ID (GEM PORT-ID for TDM)",
#endif
    GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPID_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPID_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TDQP_TDQ
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TDQP_TDQ_FIELD =
{
    "TDQ",
#if RU_INCLUDE_DESC
    "TDM_queue",
    "TDM queue number (one of the eight TX queues)",
#endif
    GPON_TX_FIFO_CONFIGURATION_TDQP_TDQ_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TDQP_TDQ_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TDQP_TDQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPTI
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPTI_FIELD =
{
    "TDQPTI",
#if RU_INCLUDE_DESC
    "TDM_PTI_bits",
    "2 MS PTI bits for TDM port ID.",
#endif
    GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPTI_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPTI_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPTI_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TDQP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TDQP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_FIFO_CONFIGURATION_TDQP_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TDQP_RESERVED0_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TDQP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_LBP_LBQN
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_LBP_LBQN_FIELD =
{
    "LBQN",
#if RU_INCLUDE_DESC
    "LoopBack_queue_number",
    "Mapping of LoopBack to queue number",
#endif
    GPON_TX_FIFO_CONFIGURATION_LBP_LBQN_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_LBP_LBQN_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_LBP_LBQN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_LBP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_LBP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_FIFO_CONFIGURATION_LBP_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_LBP_RESERVED0_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_LBP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TTH_TDMMIN
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TTH_TDMMIN_FIELD =
{
    "TDMMIN",
#if RU_INCLUDE_DESC
    "TDM_threshold_min",
    "TDM threshold min",
#endif
    GPON_TX_FIFO_CONFIGURATION_TTH_TDMMIN_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TTH_TDMMIN_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TTH_TDMMIN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TTH_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TTH_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_FIFO_CONFIGURATION_TTH_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TTH_RESERVED0_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TTH_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TTH_TDMMAX
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TTH_TDMMAX_FIELD =
{
    "TDMMAX",
#if RU_INCLUDE_DESC
    "TDM_threshold_max",
    "TDM threshold max",
#endif
    GPON_TX_FIFO_CONFIGURATION_TTH_TDMMAX_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TTH_TDMMAX_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TTH_TDMMAX_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_FIFO_CONFIGURATION_TTH_TTHEN
 ******************************************************************************/
const ru_field_rec GPON_TX_FIFO_CONFIGURATION_TTH_TTHEN_FIELD =
{
    "TTHEN",
#if RU_INCLUDE_DESC
    "TDM_threshold_enable",
    "TDM threshold enable",
#endif
    GPON_TX_FIFO_CONFIGURATION_TTH_TTHEN_FIELD_MASK,
    0,
    GPON_TX_FIFO_CONFIGURATION_TTH_TTHEN_FIELD_WIDTH,
    GPON_TX_FIFO_CONFIGURATION_TTH_TTHEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_TX_FIFO_CONFIGURATION_PDP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_FIFO_CONFIGURATION_PDP_FIELDS[] =
{
    &GPON_TX_FIFO_CONFIGURATION_PDP_PDBC_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_PDP_PDSC_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_PDP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_FIFO_CONFIGURATION_PDP_REG = 
{
    "PDP",
#if RU_INCLUDE_DESC
    "PACKET_DESCRIPTOR_PARAMETERS %i Register",
    "This register array contains the packet descriptor parameters. Each of the eight entries in the array relates to one of the first eight Tx queues (ordered 0 to seven) and  details  the number of packet descriptor allocated to that queue (out of 128). This register can be modified only when TX is disabled.",
#endif
    GPON_TX_FIFO_CONFIGURATION_PDP_REG_OFFSET,
    GPON_TX_FIFO_CONFIGURATION_PDP_REG_RAM_CNT,
    4,
    69,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_TX_FIFO_CONFIGURATION_PDP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_FIFO_CONFIGURATION_PDPGRP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_FIFO_CONFIGURATION_PDPGRP_FIELDS[] =
{
    &GPON_TX_FIFO_CONFIGURATION_PDPGRP_PDBC_GRP_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_PDPGRP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_FIFO_CONFIGURATION_PDPGRP_REG = 
{
    "PDPGRP",
#if RU_INCLUDE_DESC
    "PACKET_DESCRIPTOR_PARAMETERS_FOR_TCONT_GROUPS_0-3 %i Register",
    "Tconts 8-39 are bundled in 4 groups of 8 TCONTS each. The SW configure each groups base address for the packet descriptors queues. The packet descriptors Queues within each group are of the same size. The size is derived from the corresponding data queue size. The PD queue size is the corresponding data queue size divided by 64 bytes."
    "This register array of 4 registers contain the configured base address of each group. The first register configures the address of group0 (TCONTS 8-15), and so on.",
#endif
    GPON_TX_FIFO_CONFIGURATION_PDPGRP_REG_OFFSET,
    GPON_TX_FIFO_CONFIGURATION_PDPGRP_REG_RAM_CNT,
    4,
    70,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_FIFO_CONFIGURATION_PDPGRP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_FIFO_CONFIGURATION_TQP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_FIFO_CONFIGURATION_TQP_FIELDS[] =
{
    &GPON_TX_FIFO_CONFIGURATION_TQP_TQBC_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TQP_TQSC_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TQP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_FIFO_CONFIGURATION_TQP_REG = 
{
    "TQP",
#if RU_INCLUDE_DESC
    "TX_FIFO_PARAMETER_ARRAY %i Register",
    "This register array contains the Tx FIFO parameters of the first 8 of the Tx FIFO queues. It gives the base address of each of the first 8 queues and the queues size. A queue size is in 4 bytes resolution and is up to 13 bits  (0-32Kbyte). The total size of all 40 queues must be smaller or equal to 20K. The base address must also be 4 bytes aligned."
    "This register can be modified only when TX is disabled.",
#endif
    GPON_TX_FIFO_CONFIGURATION_TQP_REG_OFFSET,
    GPON_TX_FIFO_CONFIGURATION_TQP_REG_RAM_CNT,
    4,
    71,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_TX_FIFO_CONFIGURATION_TQP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_FIFO_CONFIGURATION_TDQPGRP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_FIFO_CONFIGURATION_TDQPGRP_FIELDS[] =
{
    &GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQB_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TDQPGRP_TDQS_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TDQPGRP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_FIFO_CONFIGURATION_TDQPGRP_REG = 
{
    "TDQPGRP",
#if RU_INCLUDE_DESC
    "DATA_QUEUES_PARAMETERS_FOR_TCONT_GROUPS_0-3 %i Register",
    "Tconts 8-39 are bundled in 4 groups of 8 TCONTS each. The SW configure each groups base address and size. The Queues within each group are of the same size. This register array of 4 registers contain the configured base address of each group. The first register configures the address of group0 (TCONTS 8-15), and so on.",
#endif
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_REG_OFFSET,
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_REG_RAM_CNT,
    4,
    72,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_TX_FIFO_CONFIGURATION_TDQPGRP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_FIFO_CONFIGURATION_TDQP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_FIFO_CONFIGURATION_TDQP_FIELDS[] =
{
    &GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPID_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TDQP_TDQ_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TDQP_TDQPTI_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TDQP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_FIFO_CONFIGURATION_TDQP_REG = 
{
    "TDQP",
#if RU_INCLUDE_DESC
    "TX_FIFO_TDM_QUEUE_PARAMETERS Register",
    "This register maps TX TDM flow to one of the forty TX FIFO queues. The mapping including a single PORT ID, and a queue number. This register can be modified only when TX is disabled.",
#endif
    GPON_TX_FIFO_CONFIGURATION_TDQP_REG_OFFSET,
    0,
    0,
    73,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_TX_FIFO_CONFIGURATION_TDQP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_FIFO_CONFIGURATION_LBP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_FIFO_CONFIGURATION_LBP_FIELDS[] =
{
    &GPON_TX_FIFO_CONFIGURATION_LBP_LBQN_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_LBP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_FIFO_CONFIGURATION_LBP_REG = 
{
    "LBP",
#if RU_INCLUDE_DESC
    "LOOPBACK_PARAMETERS Register",
    "This register maps the loopback operation mode to one out of the 40 Tx queues (while disabling the other queues). This register can be modified only when TX is disabled.",
#endif
    GPON_TX_FIFO_CONFIGURATION_LBP_REG_OFFSET,
    0,
    0,
    74,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_FIFO_CONFIGURATION_LBP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_FIFO_CONFIGURATION_TTH
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_FIFO_CONFIGURATION_TTH_FIELDS[] =
{
    &GPON_TX_FIFO_CONFIGURATION_TTH_TDMMIN_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TTH_RESERVED0_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TTH_TDMMAX_FIELD,
    &GPON_TX_FIFO_CONFIGURATION_TTH_TTHEN_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_FIFO_CONFIGURATION_TTH_REG = 
{
    "TTH",
#if RU_INCLUDE_DESC
    "TX_THRESHOLD Register",
    "If TDM threshold is enabled then if the requested TDM contains more than min threshold valid entries, transmission will be enabled from TDM queue given a request. The TDM access size will be the smallest of access size or num of bytes in queue or TDM max threshold. This means that if there is more data in the queue than max threshold, max threshold bytes of data will be transmitted (if the access is large enough)."
    "If the thresholds are disabled, the only condition for transmission is that the queue contains more than min threshold only and there is access for that queue. This register can be modified only when TX is disabled.",
#endif
    GPON_TX_FIFO_CONFIGURATION_TTH_REG_OFFSET,
    0,
    0,
    75,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_TX_FIFO_CONFIGURATION_TTH_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_TX_FIFO_CONFIGURATION
 ******************************************************************************/
static const ru_reg_rec *GPON_TX_FIFO_CONFIGURATION_REGS[] =
{
    &GPON_TX_FIFO_CONFIGURATION_PDP_REG,
    &GPON_TX_FIFO_CONFIGURATION_PDPGRP_REG,
    &GPON_TX_FIFO_CONFIGURATION_TQP_REG,
    &GPON_TX_FIFO_CONFIGURATION_TDQPGRP_REG,
    &GPON_TX_FIFO_CONFIGURATION_TDQP_REG,
    &GPON_TX_FIFO_CONFIGURATION_LBP_REG,
    &GPON_TX_FIFO_CONFIGURATION_TTH_REG,
};

static unsigned long GPON_TX_FIFO_CONFIGURATION_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80148500,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db0500,
#else
    #error "GPON_TX_FIFO_CONFIGURATION base address not defined"
#endif
};

const ru_block_rec GPON_TX_FIFO_CONFIGURATION_BLOCK = 
{
    "GPON_TX_FIFO_CONFIGURATION",
    GPON_TX_FIFO_CONFIGURATION_ADDRS,
    1,
    7,
    GPON_TX_FIFO_CONFIGURATION_REGS
};

/* End of file BCM6836_A0GPON_TX_FIFO_CONFIGURATION.c */
