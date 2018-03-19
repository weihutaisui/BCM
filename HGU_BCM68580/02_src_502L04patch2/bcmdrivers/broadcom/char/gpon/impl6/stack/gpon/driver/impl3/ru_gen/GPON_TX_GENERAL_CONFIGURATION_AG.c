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
 * Field: GPON_TX_GENERAL_CONFIGURATION_ONUID_ONUID
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ONUID_ONUID_FIELD =
{
    "ONUID",
#if RU_INCLUDE_DESC
    "ONU_ID",
    "Specifies the ONU ID",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ONUID_ONUID_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ONUID_ONUID_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ONUID_ONUID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ONUID_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ONUID_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ONUID_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ONUID_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ONUID_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_IND_TWT2
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_IND_TWT2_FIELD =
{
    "TWT2",
#if RU_INCLUDE_DESC
    "Traffic_waiting_TCONT2",
    "Traffic waiting on TCONT type 2 indication bit",
#endif
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT2_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT2_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_IND_TWT3
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_IND_TWT3_FIELD =
{
    "TWT3",
#if RU_INCLUDE_DESC
    "Traffic_waiting_TCONT3",
    "Traffic waiting on TCONT type 3 indication",
#endif
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT3_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT3_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_IND_TWT4
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_IND_TWT4_FIELD =
{
    "TWT4",
#if RU_INCLUDE_DESC
    "Traffic_waiting_TCONT4",
    "Traffic waiting on TCONT type 4 indication",
#endif
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT4_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT4_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT4_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_IND_TWT5
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_IND_TWT5_FIELD =
{
    "TWT5",
#if RU_INCLUDE_DESC
    "Traffic_waiting_TCONT5",
    "Traffic waiting on TCONT type 5 indication",
#endif
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT5_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT5_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_IND_TWT5_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_IND_RDI
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_IND_RDI_FIELD =
{
    "RDI",
#if RU_INCLUDE_DESC
    "Remote_defects_status",
    "The RDI field is asserted by the software, if the OLT transmission is received with defects at the ONU",
#endif
    GPON_TX_GENERAL_CONFIGURATION_IND_RDI_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_IND_RDI_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_IND_RDI_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED1_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED1_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED2
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED2_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED2_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PROD_PRODC
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PROD_PRODC_FIELD =
{
    "PRODC",
#if RU_INCLUDE_DESC
    "Overhead_content",
    "Preamble overhead content",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PROD_PRODC_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PROD_PRODC_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PROD_PRODC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PROD_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PROD_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PROD_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PROD_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PROD_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PRODS_PRCL
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PRODS_PRCL_FIELD =
{
    "PRCL",
#if RU_INCLUDE_DESC
    "Preamble_length",
    "The configured preamble length",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PRODS_PRCL_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PRODS_PRCL_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PRODS_PRCL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PRODS_PRL
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PRODS_PRL_FIELD =
{
    "PRL",
#if RU_INCLUDE_DESC
    "preamble_repetition_length",
    "One byte preamble repetition length",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PRODS_PRL_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PRODS_PRL_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PRODS_PRL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PRODS_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PRODS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PRODS_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PRODS_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PRODS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PRRB_BRC
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PRRB_BRC_FIELD =
{
    "BRC",
#if RU_INCLUDE_DESC
    "Byte_repetition_content",
    "Byte repetition content",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PRRB_BRC_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PRRB_BRC_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PRRB_BRC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PRRB_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PRRB_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PRRB_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PRRB_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PRRB_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DELC_DELOC
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DELC_DELOC_FIELD =
{
    "DELOC",
#if RU_INCLUDE_DESC
    "Delimiter_content",
    "Delimiter content",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DELC_DELOC_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DELC_DELOC_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DELC_DELOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTOP
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTOP_FIELD =
{
    "MSSTOP",
#if RU_INCLUDE_DESC
    "Msstop",
    "Misc sstop",
#endif
    GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTOP_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTOP_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTOP_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTART
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTART_FIELD =
{
    "MSSTART",
#if RU_INCLUDE_DESC
    "Msstart",
    "Misc sstart",
#endif
    GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTART_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTART_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTART_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_MISCAC2_PLOAMU
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_MISCAC2_PLOAMU_FIELD =
{
    "PLOAMU",
#if RU_INCLUDE_DESC
    "PLOAMu",
    "Ploam up stream flag",
#endif
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_PLOAMU_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_PLOAMU_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_PLOAMU_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_MISCAC2_DBRU
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_MISCAC2_DBRU_FIELD =
{
    "DBRU",
#if RU_INCLUDE_DESC
    "DBRu",
    "DBRu flag",
#endif
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_DBRU_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_DBRU_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_DBRU_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_MISCAC2_FECU
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_MISCAC2_FECU_FIELD =
{
    "FECU",
#if RU_INCLUDE_DESC
    "FECu",
    "FECu flag",
#endif
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_FECU_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_FECU_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_FECU_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_MISCAC2_QU
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_MISCAC2_QU_FIELD =
{
    "QU",
#if RU_INCLUDE_DESC
    "Queue",
    "Queue num up stream",
#endif
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_QU_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_QU_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_QU_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_MISCAC2_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_MISCAC2_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ESALL_ESA
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ESALL_ESA_FIELD =
{
    "ESA",
#if RU_INCLUDE_DESC
    "enable_short_allocations",
    "When asserted, all allocations will be allowed, regardless of their length. Even illegal allocations, shorter than 12 bytes, or shorter than ploam+dbr will be allowed.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ESALL_ESA_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ESALL_ESA_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ESALL_ESA_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ESALL_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ESALL_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ESALL_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ESALL_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ESALL_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP0_FIELD =
{
    "QFORGRP0",
#if RU_INCLUDE_DESC
    "TCONT_number_for_group_0",
    "TCONT number for counter group 0",
#endif
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP1
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP1_FIELD =
{
    "QFORGRP1",
#if RU_INCLUDE_DESC
    "TCONT_number_for_group_1",
    "TCONT number for counter group 1",
#endif
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP1_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP1_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED1_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED1_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP2
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP2_FIELD =
{
    "QFORGRP2",
#if RU_INCLUDE_DESC
    "TCONT_number_for_group_2",
    "TCONT number for counter group 2",
#endif
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP2_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP2_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED2
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED2_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED2_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP3
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP3_FIELD =
{
    "QFORGRP3",
#if RU_INCLUDE_DESC
    "TCONT_number_for_group_3",
    "TCONT number for counter group 3",
#endif
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP3_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP3_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED3
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED3_FIELD =
{
    "RESERVED3",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED3_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED3_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED3_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PLSS_PLSPAT
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PLSS_PLSPAT_FIELD =
{
    "PLSPAT",
#if RU_INCLUDE_DESC
    "PLS_pattern",
    "PLS pattern to repeat",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PLSS_PLSPAT_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PLSS_PLSPAT_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PLSS_PLSPAT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PLSRS_PLSR
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PLSRS_PLSR_FIELD =
{
    "PLSR",
#if RU_INCLUDE_DESC
    "PLS_repetition",
    "Number of times to repeat on PLS pattern",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PLSRS_PLSR_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PLSRS_PLSR_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PLSRS_PLSR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_PLSRS_R1
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_PLSRS_R1_FIELD =
{
    "R1",
#if RU_INCLUDE_DESC
    "R",
    "Reserved",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PLSRS_R1_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_PLSRS_R1_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_PLSRS_R1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPENUM
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPENUM_FIELD =
{
    "BIPENUM",
#if RU_INCLUDE_DESC
    "Bip_error_num",
    "Number of bursts in which the HW bip value will be xored with the BIP mask",
#endif
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPENUM_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPENUM_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPENUM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPM
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPM_FIELD =
{
    "BIPM",
#if RU_INCLUDE_DESC
    "Bip_error_mask",
    "BIP error mask",
#endif
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPM_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPM_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEV
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEV_FIELD =
{
    "BIPEV",
#if RU_INCLUDE_DESC
    "Bip_error_valid",
    "BIP error valid. This bit is write clear. When read, will indicate error status.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEV_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEV_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEV_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEF
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEF_FIELD =
{
    "BIPEF",
#if RU_INCLUDE_DESC
    "BIP_error_force",
    "BIP error force",
#endif
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEF_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEF_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEF_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_BIPS_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_BIPS_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_BIPS_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_CRCOFF_CRCOC
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_CRCOFF_CRCOC_FIELD =
{
    "CRCOC",
#if RU_INCLUDE_DESC
    "CRC_offset_content",
    "CRC offset start of calc",
#endif
    GPON_TX_GENERAL_CONFIGURATION_CRCOFF_CRCOC_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_CRCOFF_CRCOC_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_CRCOFF_CRCOC_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_CRCOFF_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_CRCOFF_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_CRCOFF_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_CRCOFF_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_CRCOFF_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLQ
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLQ_FIELD =
{
    "FLQ",
#if RU_INCLUDE_DESC
    "flush_queue",
    "The number of queue to be flushed",
#endif
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLQ_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLQ_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLQ_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLEN_FIELD =
{
    "FLEN",
#if RU_INCLUDE_DESC
    "flush_enable",
    "When asserted, the queue determined in FLQ will be flushed",
#endif
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLEN_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLEN_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLIMM
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLIMM_FIELD =
{
    "FLIMM",
#if RU_INCLUDE_DESC
    "flush_immediate",
    "When asserted, with flush valid, the flush will happen immediately without waiting for not transmitting",
#endif
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLIMM_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLIMM_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLIMM_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED1_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED1_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_FLSHDN_FLDN
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_FLSHDN_FLDN_FIELD =
{
    "FLDN",
#if RU_INCLUDE_DESC
    "flush_done",
    "flush done indication",
#endif
    GPON_TX_GENERAL_CONFIGURATION_FLSHDN_FLDN_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_FLSHDN_FLDN_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_FLSHDN_FLDN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_FLSHDN_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_FLSHDN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_FLSHDN_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_FLSHDN_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_FLSHDN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_RDYIND_SET
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_RDYIND_SET_FIELD =
{
    "SET",
#if RU_INCLUDE_DESC
    "indications_update_set",
    "triggers the process of updating",
#endif
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_SET_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_SET_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_SET_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_RDYIND_RDY
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_RDYIND_RDY_FIELD =
{
    "RDY",
#if RU_INCLUDE_DESC
    "indicatios_update_done",
    "HW indcation that the update is done",
#endif
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_RDY_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_RDY_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_RDY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED1_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED1_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVPOL
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVPOL_FIELD =
{
    "DVPOL",
#if RU_INCLUDE_DESC
    "DV_polarity",
    "DV output polarity. This bit determines whether the bit pattern specified for DV signal is inverted or not.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVPOL_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVPOL_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVPOL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVEXTEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVEXTEN_FIELD =
{
    "DVEXTEN",
#if RU_INCLUDE_DESC
    "DV_extension_enable",
    "Enables the DV setup and hold pattern extension from two bytes to three bytes each",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVEXTEN_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVEXTEN_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVEXTEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED1_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED1_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DVSTP_DVSTU
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DVSTP_DVSTU_FIELD =
{
    "DVSTU",
#if RU_INCLUDE_DESC
    "DV_setup_bits",
    "Pattern of DV setup bits before the PON overhead starts. 0 - DV signal is off during this bit, 1 DV signal is on during this bit",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVSTP_DVSTU_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DVSTP_DVSTU_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DVSTP_DVSTU_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DVSTP_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DVSTP_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVSTP_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DVSTP_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DVSTP_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DVHLD_DVHLD
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DVHLD_DVHLD_FIELD =
{
    "DVHLD",
#if RU_INCLUDE_DESC
    "DV_hold_bits",
    "Pattern of DV hold bits following the burst transmission end. 0 - DV signal is off during this bit, 1 DV signal is on during this bit",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVHLD_DVHLD_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DVHLD_DVHLD_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DVHLD_DVHLD_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DVHLD_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DVHLD_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVHLD_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DVHLD_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DVHLD_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DPEN_DPEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DPEN_DPEN_FIELD =
{
    "DPEN",
#if RU_INCLUDE_DESC
    "data_pattern_enable",
    "Enables the data pattern, setup and hold transmissions.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DPEN_DPEN_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_DPEN_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_DPEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DPEN_PRBS
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DPEN_PRBS_FIELD =
{
    "PRBS",
#if RU_INCLUDE_DESC
    "prbs_pattern_enable",
    "Enables a PRBS pattern on the data bus while the tx_en signal is de-activated.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DPEN_PRBS_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_PRBS_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_PRBS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DPEN_STPLEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DPEN_STPLEN_FIELD =
{
    "STPLEN",
#if RU_INCLUDE_DESC
    "Setup_length",
    "Setup pattern length (in bytes)",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DPEN_STPLEN_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_STPLEN_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_STPLEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED1
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED1_FIELD =
{
    "RESERVED1",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED1_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED1_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DPEN_HLDLEN
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DPEN_HLDLEN_FIELD =
{
    "HLDLEN",
#if RU_INCLUDE_DESC
    "Hold_length",
    "Hold pattern length",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DPEN_HLDLEN_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_HLDLEN_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_HLDLEN_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED2
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED2_FIELD =
{
    "RESERVED2",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED2_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED2_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED2_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DSTP_STPPAT
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DSTP_STPPAT_FIELD =
{
    "STPPAT",
#if RU_INCLUDE_DESC
    "data_setup_pattern",
    "Data setup pattern.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DSTP_STPPAT_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DSTP_STPPAT_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DSTP_STPPAT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_DHLD_HLDPAT
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_DHLD_HLDPAT_FIELD =
{
    "HLDPAT",
#if RU_INCLUDE_DESC
    "data_hold_pattern",
    "Data hold pattern.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DHLD_HLDPAT_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_DHLD_HLDPAT_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_DHLD_HLDPAT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_LEVEL_CLR
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_LEVEL_CLR_FIELD =
{
    "ROGUE_LEVEL_CLR",
#if RU_INCLUDE_DESC
    "rogue_level_clear",
    "clears the dv level counter",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_LEVEL_CLR_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_LEVEL_CLR_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_LEVEL_CLR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DIFF_CLR
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DIFF_CLR_FIELD =
{
    "ROGUE_DIFF_CLR",
#if RU_INCLUDE_DESC
    "rogue_diff_clear",
    "clears the dv diff counter",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DIFF_CLR_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DIFF_CLR_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DIFF_CLR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DV_SEL
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DV_SEL_FIELD =
{
    "ROGUE_DV_SEL",
#if RU_INCLUDE_DESC
    "rogue_dv_sel",
    "selects the source of the examined dv. 1 - from SerDes, 0 - from I/O pad",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DV_SEL_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DV_SEL_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DV_SEL_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_WINDOW_SIZE
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD =
{
    "WINDOW_SIZE",
#if RU_INCLUDE_DESC
    "window_size",
    "Window_size in clock cycles.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_WINDOW_SIZE
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD =
{
    "WINDOW_SIZE",
#if RU_INCLUDE_DESC
    "window_size",
    "Window_size in clock cycles.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_RESERVED0
 ******************************************************************************/
const ru_field_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_RESERVED0_FIELD_MASK,
    0,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_RESERVED0_FIELD_WIDTH,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_ONUID
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_ONUID_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_ONUID_ONUID_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_ONUID_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_ONUID_REG = 
{
    "ONUID",
#if RU_INCLUDE_DESC
    "ONU_ID Register",
    "Specify the ONU ID. This register can be modified while TX is enabled.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ONUID_REG_OFFSET,
    0,
    0,
    17,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_ONUID_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_IND
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_IND_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED0_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_IND_TWT2_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_IND_TWT3_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_IND_TWT4_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_IND_TWT5_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_IND_RDI_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED1_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_IND_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_IND_REG = 
{
    "IND",
#if RU_INCLUDE_DESC
    "INDICATION_FIELD Register",
    "Specifies the IND field in the PON upstream. This register can be modified while TX is enabled.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_IND_REG_OFFSET,
    0,
    0,
    18,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    8,
    GPON_TX_GENERAL_CONFIGURATION_IND_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_PROD
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_PROD_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_PROD_PRODC_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_PROD_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_PROD_REG = 
{
    "PROD",
#if RU_INCLUDE_DESC
    "PREAMBLE_OVERHEAD %i Register",
    "This register array of 16 bytes defines a configurable pattern of the preamble overhead. This register can be modified when TX is enabled in space of one frame from last allocation to that ONU",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PROD_REG_OFFSET,
    GPON_TX_GENERAL_CONFIGURATION_PROD_REG_RAM_CNT,
    4,
    19,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_PROD_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_PRODS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_PRODS_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_PRODS_PRCL_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_PRODS_PRL_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_PRODS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_PRODS_REG = 
{
    "PRODS",
#if RU_INCLUDE_DESC
    "PREAMBLE_OVERHEAD_SPECIFICATIONS Register",
    "This register configures the preamble config lengths which consist of a pattern up to 16 bytes and a repetition byte. This register can be modified when TX is enabled in space of one frame from last allocation to that ONU",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PRODS_REG_OFFSET,
    0,
    0,
    20,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    GPON_TX_GENERAL_CONFIGURATION_PRODS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_PRRB
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_PRRB_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_PRRB_BRC_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_PRRB_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_PRRB_REG = 
{
    "PRRB",
#if RU_INCLUDE_DESC
    "PREAMBLE_REPETITION_BYTE Register",
    "This register defines the byte repetition content. This register can be modified when TX is enabled in space of one frame from last allocation to that ONU",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PRRB_REG_OFFSET,
    0,
    0,
    21,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_PRRB_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_DELC
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_DELC_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_DELC_DELOC_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_DELC_REG = 
{
    "DELC",
#if RU_INCLUDE_DESC
    "DELIMITER_CONTENT Register",
    "This register defines the delimiter content. This register can be modified when TX is enabled in space of one frame from last allocation to that ONU. The delimiter containing 4 bytes is transmitted from Lsbyte to Msbyte",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DELC_REG_OFFSET,
    0,
    0,
    22,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_GENERAL_CONFIGURATION_DELC_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_MISCAC1
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_MISCAC1_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTOP_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_MISCAC1_MSSTART_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_MISCAC1_REG = 
{
    "MISCAC1",
#if RU_INCLUDE_DESC
    "MISC_ACCESS_CONTENT1 Register",
    "This register defines sstart & sstop of misc access. This register can be updated only when TX is disabled.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_MISCAC1_REG_OFFSET,
    0,
    0,
    23,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC1_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_MISCAC2
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_MISCAC2_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_MISCAC2_PLOAMU_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_MISCAC2_DBRU_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_MISCAC2_FECU_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_MISCAC2_QU_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_MISCAC2_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_MISCAC2_REG = 
{
    "MISCAC2",
#if RU_INCLUDE_DESC
    "MISC_ACCESS_CONTENT2 Register",
    "This register defines misc access flags and queue. This register can be updated only when TX is disabled.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_REG_OFFSET,
    0,
    0,
    24,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    GPON_TX_GENERAL_CONFIGURATION_MISCAC2_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_ESALL
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_ESALL_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_ESALL_ESA_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_ESALL_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_ESALL_REG = 
{
    "ESALL",
#if RU_INCLUDE_DESC
    "ENABLE_SHORT_ALLOCATIONS Register",
    "When asserted, allocations shorter than 16 bytes will be allowed",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ESALL_REG_OFFSET,
    0,
    0,
    25,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_ESALL_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_QFORCNTR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP0_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED0_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP1_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED1_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP2_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED2_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_QFORGRP3_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_RESERVED3_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_REG = 
{
    "QFORCNTR",
#if RU_INCLUDE_DESC
    "TCONT_TO_COUNTER_GROUP_ASSOCIATION Register",
    "There are four sets of PM counters. Each set counts events that are related to one TCONT. This register determines which TCONT is associated with each one of the counters groups",
#endif
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_REG_OFFSET,
    0,
    0,
    26,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    8,
    GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_PLSS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_PLSS_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_PLSS_PLSPAT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_PLSS_REG = 
{
    "PLSS",
#if RU_INCLUDE_DESC
    "PLS_SPECIFICATION Register",
    "PLS specifications including PLS pattern. This register can be modified when TX is enabled in space of one frame from last allocation to that ONU",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PLSS_REG_OFFSET,
    0,
    0,
    27,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_GENERAL_CONFIGURATION_PLSS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_PLSRS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_PLSRS_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_PLSRS_PLSR_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_PLSRS_R1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_PLSRS_REG = 
{
    "PLSRS",
#if RU_INCLUDE_DESC
    "PLS_REPETITION_SPECIFICATIONS Register",
    "This register defines the number of repetitions on PLSS register. This register can be modified when TX is enabled in space of one frame from last allocation to that ONU",
#endif
    GPON_TX_GENERAL_CONFIGURATION_PLSRS_REG_OFFSET,
    0,
    0,
    28,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_PLSRS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_BIPS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_BIPS_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPENUM_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPM_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEV_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_BIPS_BIPEF_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_BIPS_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_BIPS_REG = 
{
    "BIPS",
#if RU_INCLUDE_DESC
    "BIP_SPECIFICATIONS Register",
    "Enables injection of bip errors using a bip mask. The SW has two options of operation. One is to use BIPEF (Bip Error Force) field. The SW can set/clear this bit. When set to  one the BIP error will be set in each burst BIP. The second option is to use BIPEV field. The SW can only set this field to one and the HW can only clear this bit to zero after BIPENUN burst errors were injected. This register can be modified only when TX is disabled.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_BIPS_REG_OFFSET,
    0,
    0,
    29,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    GPON_TX_GENERAL_CONFIGURATION_BIPS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_CRCOFF
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_CRCOFF_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_CRCOFF_CRCOC_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_CRCOFF_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_CRCOFF_REG = 
{
    "CRCOFF",
#if RU_INCLUDE_DESC
    "CRC_OFFSET Register",
    "This register defines the offset from a packet start from which crc calculation is applied to the transmitted packet when crc32 is enabled. This register can be modified only when TX is disabled.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_CRCOFF_REG_OFFSET,
    0,
    0,
    30,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_CRCOFF_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_FLUSH
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_FLUSH_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLQ_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED0_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLEN_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_FLUSH_FLIMM_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_FLUSH_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_FLUSH_REG = 
{
    "FLUSH",
#if RU_INCLUDE_DESC
    "SINGLE_TCONT_FLUSH Register",
    "This register enables single TCONT flush. When configuring the queue number, and setting the valid bit, the new data for the queue will be disregarded, and its read and write pointers will be reset."
    "In order to avoid under-run in the TX fifos, the flush does not happen immediately, but is done when there is no transmission. When the flush is done, the HW will assert flush_done bit (see next register)."
    "If an immediate flush is desired (no waiting for no transmission), the flush immediate bit should be asserted along with the flush valid.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_REG_OFFSET,
    0,
    0,
    31,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    5,
    GPON_TX_GENERAL_CONFIGURATION_FLUSH_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_FLSHDN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_FLSHDN_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_FLSHDN_FLDN_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_FLSHDN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_FLSHDN_REG = 
{
    "FLSHDN",
#if RU_INCLUDE_DESC
    "FLUSH_DONE Register",
    "This register contains the HW indication to SW that the flush was done."
    "In order to initiate flush process the SW asserts flush valid and flush_q fields in the previous register. When the flush is done (immediately or gracefully) the HW asserts the flush done bit. This bit will stay asserted until the SW de-assert flush valid. In this time, between the assertion of flush done and de-assertion of flush valid, the flush continues, meaning new data for the flushed queue will be disregarded, and IDLE GEMs will be transmitted if the flushed queue will receive accesses.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_FLSHDN_REG_OFFSET,
    0,
    0,
    32,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_FLSHDN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_RDYIND
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_RDYIND_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_RDYIND_SET_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED0_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_RDYIND_RDY_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_RDYIND_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_RDYIND_REG = 
{
    "RDYIND",
#if RU_INCLUDE_DESC
    "BUFFER_READY_INDICATIONS_UPDATE_SET Register",
    "This register contains the control of the ready indications update mechanism. Assertion of the set bit will activate a process of updating the indications of number of buffers ready per queue in the front end fifo. The updating process will be triggered every time the set bit will change its state from 0 to 1. When the update is done, the HW will assert the RDY bit.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_REG_OFFSET,
    0,
    0,
    33,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_TX_GENERAL_CONFIGURATION_RDYIND_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_DVSLP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_DVSLP_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVPOL_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED0_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DVSLP_DVEXTEN_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DVSLP_RESERVED1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_DVSLP_REG = 
{
    "DVSLP",
#if RU_INCLUDE_DESC
    "DV_SPECIFICATIONS_POLARITY_AND_EXTENSION_ENABLE Register",
    "DV is the data valid signal output to control the burst transceiver. If DVPOL register  is set to one then DV value when not in burst is zero and during burst is one and vice versa. DVPOL doesn't affect DV value in setup (2 bytes pattern) and hold time (2 bytes pattern). During setup and hold DV value is set according to DVSTP and DVHLD registers."
    "This register contains also the enable bit for the DV setup and hold extension."
    "This register can be modified only when TX is disabled.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_REG_OFFSET,
    0,
    0,
    34,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_TX_GENERAL_CONFIGURATION_DVSLP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_DVSTP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_DVSTP_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_DVSTP_DVSTU_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DVSTP_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_DVSTP_REG = 
{
    "DVSTP",
#if RU_INCLUDE_DESC
    "DV_SPECIFICATIONS_SETUP_PATTERN Register",
    "DV is the data valid signal output to control the burst transceiver. This register configure the pattern of this signal during setup time prior to preamble transmission."
    "The default setup pattern length is of 2 bytes, when bits 15:8 are transmitted first, and then bits 7:0."
    "he setup pattern can be extended (see previous register) to 3 bytes, where bits 23:16 will be the first to be transmitted and 7:0 are last."
    "This register can be modified only when TX is disabled.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVSTP_REG_OFFSET,
    0,
    0,
    35,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_DVSTP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_DVHLD
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_DVHLD_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_DVHLD_DVHLD_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DVHLD_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_DVHLD_REG = 
{
    "DVHLD",
#if RU_INCLUDE_DESC
    "DV_SPECIFICATIONS_HOLD_PATTERN Register",
    "DV is the data valid signal output to control the burst transceiver. This register configure the hold pattern of this signal during hold time following burst transmission end."
    "The hold pattern can be also extended (see previous register) in the same manner as setup."
    "This register can be modified only when TX is disabled.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DVHLD_REG_OFFSET,
    0,
    0,
    36,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_DVHLD_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_DPEN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_DPEN_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_DPEN_DPEN_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DPEN_PRBS_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED0_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DPEN_STPLEN_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED1_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DPEN_HLDLEN_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_DPEN_RESERVED2_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_DPEN_REG = 
{
    "DPEN",
#if RU_INCLUDE_DESC
    "DATA_PATTERN_ENABLE Register",
    "This register contains the configurations for the data pattern feature."
    "When the data pattern enable bit is asserted, the data bus will have the value of 0xAA in times where there is no transimission (dv is off). A setup and hold patterns will also be transmitted. The setup will be transmitted before the preamble and the hold will be transmitted right after the last byte of payload. The setup and hold patterns are configured in the next register, but their length is configured in this. The length for both may vary between 1 to 4 bytes.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DPEN_REG_OFFSET,
    0,
    0,
    37,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    7,
    GPON_TX_GENERAL_CONFIGURATION_DPEN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_DSTP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_DSTP_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_DSTP_STPPAT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_DSTP_REG = 
{
    "DSTP",
#if RU_INCLUDE_DESC
    "DATA_SETUP_PATTERN Register",
    "This register contains configurable the data setup pattern. This pattern will be transmitted before the preamble if the data pattern is enabled. The setup pattern length is configured and may vary from 1 to 4 bytes (see previous register). If a length of one byte is configured, bits 7:0 of this register will be transmitted. If the length is 4 bytes, bits 7:0 will be transmitted first, and 31:24 will be transmitted last.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DSTP_REG_OFFSET,
    0,
    0,
    38,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_GENERAL_CONFIGURATION_DSTP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_DHLD
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_DHLD_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_DHLD_HLDPAT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_DHLD_REG = 
{
    "DHLD",
#if RU_INCLUDE_DESC
    "DATA_HOLD_PATTERN Register",
    "This register contains configurable the data hold pattern. This pattern will be transmitted right after the last byte of payload if the data pattern is enabled. The hold pattern length is configured and may vary from 1 to 4 bytes (see previous register). If a length of one byte is configured, bits 7:0 of this register will be transmitted. If the length is 4 bytes, bits 7:0 will be transmitted first, and 31:24 will be transmitted last.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_DHLD_REG_OFFSET,
    0,
    0,
    39,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_GENERAL_CONFIGURATION_DHLD_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_LEVEL_CLR_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DIFF_CLR_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_ROGUE_DV_SEL_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_REG = 
{
    "ROGUE_ONU_EN",
#if RU_INCLUDE_DESC
    "ROGUE_ONU_EN Register",
    "enables the measurement of rogue ONU."
    ""
    "If TX_DV will be asserted for a configured time window, then the TX_DV will be shut down and an interrupt will be asserted.",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_REG_OFFSET,
    0,
    0,
    40,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    4,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_WINDOW_SIZE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_REG = 
{
    "ROGUE_LEVEL_TIME",
#if RU_INCLUDE_DESC
    "ROGUE_ONU_LEVEL_THRESHOLD Register",
    "num of cycles the dv is allowed to be active before considered as rogue",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_REG_OFFSET,
    0,
    0,
    41,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_FIELDS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_WINDOW_SIZE_FIELD,
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_REG = 
{
    "ROGUE_DIFF_TIME",
#if RU_INCLUDE_DESC
    "ROGUE_DIFF_THRESHOLD Register",
    "num of cycles the examined dv is allowed to be different from gpon dv before being considered as rogue",
#endif
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_REG_OFFSET,
    0,
    0,
    42,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_TX_GENERAL_CONFIGURATION
 ******************************************************************************/
static const ru_reg_rec *GPON_TX_GENERAL_CONFIGURATION_REGS[] =
{
    &GPON_TX_GENERAL_CONFIGURATION_ONUID_REG,
    &GPON_TX_GENERAL_CONFIGURATION_IND_REG,
    &GPON_TX_GENERAL_CONFIGURATION_PROD_REG,
    &GPON_TX_GENERAL_CONFIGURATION_PRODS_REG,
    &GPON_TX_GENERAL_CONFIGURATION_PRRB_REG,
    &GPON_TX_GENERAL_CONFIGURATION_DELC_REG,
    &GPON_TX_GENERAL_CONFIGURATION_MISCAC1_REG,
    &GPON_TX_GENERAL_CONFIGURATION_MISCAC2_REG,
    &GPON_TX_GENERAL_CONFIGURATION_ESALL_REG,
    &GPON_TX_GENERAL_CONFIGURATION_QFORCNTR_REG,
    &GPON_TX_GENERAL_CONFIGURATION_PLSS_REG,
    &GPON_TX_GENERAL_CONFIGURATION_PLSRS_REG,
    &GPON_TX_GENERAL_CONFIGURATION_BIPS_REG,
    &GPON_TX_GENERAL_CONFIGURATION_CRCOFF_REG,
    &GPON_TX_GENERAL_CONFIGURATION_FLUSH_REG,
    &GPON_TX_GENERAL_CONFIGURATION_FLSHDN_REG,
    &GPON_TX_GENERAL_CONFIGURATION_RDYIND_REG,
    &GPON_TX_GENERAL_CONFIGURATION_DVSLP_REG,
    &GPON_TX_GENERAL_CONFIGURATION_DVSTP_REG,
    &GPON_TX_GENERAL_CONFIGURATION_DVHLD_REG,
    &GPON_TX_GENERAL_CONFIGURATION_DPEN_REG,
    &GPON_TX_GENERAL_CONFIGURATION_DSTP_REG,
    &GPON_TX_GENERAL_CONFIGURATION_DHLD_REG,
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_ONU_EN_REG,
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_LEVEL_TIME_REG,
    &GPON_TX_GENERAL_CONFIGURATION_ROGUE_DIFF_TIME_REG,
};

static unsigned long GPON_TX_GENERAL_CONFIGURATION_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80148050,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db0050,
#else
    #error "GPON_TX_GENERAL_CONFIGURATION base address not defined"
#endif
};

const ru_block_rec GPON_TX_GENERAL_CONFIGURATION_BLOCK = 
{
    "GPON_TX_GENERAL_CONFIGURATION",
    GPON_TX_GENERAL_CONFIGURATION_ADDRS,
    1,
    26,
    GPON_TX_GENERAL_CONFIGURATION_REGS
};

/* End of file BCM6836_A0GPON_TX_GENERAL_CONFIGURATION.c */
