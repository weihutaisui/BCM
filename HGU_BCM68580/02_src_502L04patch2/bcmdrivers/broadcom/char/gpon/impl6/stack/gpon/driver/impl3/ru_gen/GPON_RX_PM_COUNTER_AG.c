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
 * Field: GPON_RX_PM_COUNTER_BWMAPERR_BWMAPERR
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_BWMAPERR_BWMAPERR_FIELD =
{
    "BWMAPERR",
#if RU_INCLUDE_DESC
    "US_BW_Map_CRC_Errors",
    "Counts the number of CRC errors detected in the US BW Map",
#endif
    GPON_RX_PM_COUNTER_BWMAPERR_BWMAPERR_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_BWMAPERR_BWMAPERR_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_BWMAPERR_BWMAPERR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_PLOAMERR_PLOAMERR
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_PLOAMERR_PLOAMERR_FIELD =
{
    "PLOAMERR",
#if RU_INCLUDE_DESC
    "PLOAM_CRC_Errors",
    "Number of PLOAM CRC errors detected",
#endif
    GPON_RX_PM_COUNTER_PLOAMERR_PLOAMERR_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_PLOAMERR_PLOAMERR_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_PLOAMERR_PLOAMERR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_PLENDERR_PLENDERR
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_PLENDERR_PLENDERR_FIELD =
{
    "PLENDERR",
#if RU_INCLUDE_DESC
    "Plend_CRC_Errors",
    "Number of Plend CRC errors detecter",
#endif
    GPON_RX_PM_COUNTER_PLENDERR_PLENDERR_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_PLENDERR_PLENDERR_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_PLENDERR_PLENDERR_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_SFMISMATCH_SFMISMATCH
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_SFMISMATCH_SFMISMATCH_FIELD =
{
    "SFMISMATCH",
#if RU_INCLUDE_DESC
    "Superframe_counter_mismatches",
    "Counts the number of invalid SF counter values",
#endif
    GPON_RX_PM_COUNTER_SFMISMATCH_SFMISMATCH_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_SFMISMATCH_SFMISMATCH_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_SFMISMATCH_SFMISMATCH_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_BIP_BIP
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_BIP_BIP_FIELD =
{
    "BIP",
#if RU_INCLUDE_DESC
    "BIP_errors",
    "Number of BIP errors detected",
#endif
    GPON_RX_PM_COUNTER_BIP_BIP_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_BIP_BIP_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_BIP_BIP_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_GEMFRAG_GEMFRAG
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_GEMFRAG_GEMFRAG_FIELD =
{
    "GEMFRAG",
#if RU_INCLUDE_DESC
    "GEM_fragments",
    "Number of non-idle GEM fragments received (before filtering)",
#endif
    GPON_RX_PM_COUNTER_GEMFRAG_GEMFRAG_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_GEMFRAG_GEMFRAG_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_GEMFRAG_GEMFRAG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_ETHPACK_ETHPACK
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_ETHPACK_ETHPACK_FIELD =
{
    "ETHPACK",
#if RU_INCLUDE_DESC
    "Ethernet_packets",
    "Number of Ethernet packets which passed the Port-ID filtering",
#endif
    GPON_RX_PM_COUNTER_ETHPACK_ETHPACK_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_ETHPACK_ETHPACK_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_ETHPACK_ETHPACK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_TDMFRAG_TDMFRAG
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_TDMFRAG_TDMFRAG_FIELD =
{
    "TDMFRAG",
#if RU_INCLUDE_DESC
    "TDM_fragments",
    "Number of TDM fragments which passed the Port-ID filtering",
#endif
    GPON_RX_PM_COUNTER_TDMFRAG_TDMFRAG_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_TDMFRAG_TDMFRAG_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_TDMFRAG_TDMFRAG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_SYMERROR_SYMERRORS
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_SYMERROR_SYMERRORS_FIELD =
{
    "SYMERRORS",
#if RU_INCLUDE_DESC
    "Number_of_symbol_errors_the_FEC_corrected",
    "Number of errored symbols the FEC decoder corrected",
#endif
    GPON_RX_PM_COUNTER_SYMERROR_SYMERRORS_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_SYMERROR_SYMERRORS_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_SYMERROR_SYMERRORS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_BITERROR_BITERRORS
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_BITERROR_BITERRORS_FIELD =
{
    "BITERRORS",
#if RU_INCLUDE_DESC
    "Corrected_bit_errors",
    "Counts the number of bit errors the FEC block corrected.",
#endif
    GPON_RX_PM_COUNTER_BITERROR_BITERRORS_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_BITERROR_BITERRORS_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_BITERROR_BITERRORS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_AESERROR_AESERRCNT
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_AESERROR_AESERRCNT_FIELD =
{
    "AESERRCNT",
#if RU_INCLUDE_DESC
    "AES_allocation_error_count",
    "Number of times the cipher mechanism could not allocate a free AES machine.",
#endif
    GPON_RX_PM_COUNTER_AESERROR_AESERRCNT_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_AESERROR_AESERRCNT_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_AESERROR_AESERRCNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_TDMSTATS_RCVBYTES
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_TDMSTATS_RCVBYTES_FIELD =
{
    "RCVBYTES",
#if RU_INCLUDE_DESC
    "Number_of_TDM_received_bytes",
    "Counts the number of bytes correctly received by the TDM interface from GEM fragments.",
#endif
    GPON_RX_PM_COUNTER_TDMSTATS_RCVBYTES_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_TDMSTATS_RCVBYTES_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_TDMSTATS_RCVBYTES_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_ETHFRAG_ETHFRAG
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_ETHFRAG_ETHFRAG_FIELD =
{
    "ETHFRAG",
#if RU_INCLUDE_DESC
    "Number_of_Ethernet_fragments",
    "Counts the number of Ethernet fragments which passed Port-ID filtering",
#endif
    GPON_RX_PM_COUNTER_ETHFRAG_ETHFRAG_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_ETHFRAG_ETHFRAG_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_ETHFRAG_ETHFRAG_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_ETHBYTECNT_ETHBYTECNT
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_ETHBYTECNT_ETHBYTECNT_FIELD =
{
    "ETHBYTECNT",
#if RU_INCLUDE_DESC
    "Ethernet_Byte_Count",
    "Counts the total number of Ethernet bytes received.",
#endif
    GPON_RX_PM_COUNTER_ETHBYTECNT_ETHBYTECNT_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_ETHBYTECNT_ETHBYTECNT_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_ETHBYTECNT_ETHBYTECNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_FECUCCWCNT_FECUCCWCNT
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_FECUCCWCNT_FECUCCWCNT_FIELD =
{
    "FECUCCWCNT",
#if RU_INCLUDE_DESC
    "FEC_Uncorrectable_codewords_counter",
    "Counts the number of FEC codewords that the decoder was unable to correct",
#endif
    GPON_RX_PM_COUNTER_FECUCCWCNT_FECUCCWCNT_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_FECUCCWCNT_FECUCCWCNT_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_FECUCCWCNT_FECUCCWCNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_FECUCCWCNT_R1
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_FECUCCWCNT_R1_FIELD =
{
    "R1",
#if RU_INCLUDE_DESC
    "R",
    "Reserved",
#endif
    GPON_RX_PM_COUNTER_FECUCCWCNT_R1_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_FECUCCWCNT_R1_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_FECUCCWCNT_R1_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_FECCCWCNT_FECCCWCNT
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_FECCCWCNT_FECCCWCNT_FIELD =
{
    "FECCCWCNT",
#if RU_INCLUDE_DESC
    "FEC_correctable_codewords_counter",
    "Counts the number of codewords which contained at least one error and the FEC decoder succeeded to decode",
#endif
    GPON_RX_PM_COUNTER_FECCCWCNT_FECCCWCNT_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_FECCCWCNT_FECCCWCNT_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_FECCCWCNT_FECCCWCNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_PLONUCNT_PL_ONU_CNT
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_PLONUCNT_PL_ONU_CNT_FIELD =
{
    "PL_ONU_CNT",
#if RU_INCLUDE_DESC
    "ploam_onuid_cnt",
    "counts the number of valid received PLOAM messages passed the ONU ID filter",
#endif
    GPON_RX_PM_COUNTER_PLONUCNT_PL_ONU_CNT_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_PLONUCNT_PL_ONU_CNT_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_PLONUCNT_PL_ONU_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_PLBCSTCNT_PL_BCST_CNT
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_PLBCSTCNT_PL_BCST_CNT_FIELD =
{
    "PL_BCST_CNT",
#if RU_INCLUDE_DESC
    "ploam_broadcast_cnt",
    "counts the number of valid received PLOAM messages passed the BROADCAST ONU ID filter",
#endif
    GPON_RX_PM_COUNTER_PLBCSTCNT_PL_BCST_CNT_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_PLBCSTCNT_PL_BCST_CNT_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_PLBCSTCNT_PL_BCST_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: GPON_RX_PM_COUNTER_BW_CNT_BW_CNT
 ******************************************************************************/
const ru_field_rec GPON_RX_PM_COUNTER_BW_CNT_BW_CNT_FIELD =
{
    "BW_CNT",
#if RU_INCLUDE_DESC
    "bandwidth_count",
    "accumulates the BW accesses allocated by the OLT to a certain TCONT as received in the BW map."
    ""
    "",
#endif
    GPON_RX_PM_COUNTER_BW_CNT_BW_CNT_FIELD_MASK,
    0,
    GPON_RX_PM_COUNTER_BW_CNT_BW_CNT_FIELD_WIDTH,
    GPON_RX_PM_COUNTER_BW_CNT_BW_CNT_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_BWMAPERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_BWMAPERR_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_BWMAPERR_BWMAPERR_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_BWMAPERR_REG = 
{
    "BWMAPERR",
#if RU_INCLUDE_DESC
    "CRC_ERRORS_IN_THE_US_BW_MAP_FIELD Register",
    "Counts the number of CRC errors in the US BW Map field",
#endif
    GPON_RX_PM_COUNTER_BWMAPERR_REG_OFFSET,
    0,
    0,
    120,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_BWMAPERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_PLOAMERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_PLOAMERR_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_PLOAMERR_PLOAMERR_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_PLOAMERR_REG = 
{
    "PLOAMERR",
#if RU_INCLUDE_DESC
    "CRC_ERRORS_IN_PLOAM_FIELD Register",
    "Counts the number of CRC erros in the PLOAM field",
#endif
    GPON_RX_PM_COUNTER_PLOAMERR_REG_OFFSET,
    0,
    0,
    121,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_PLOAMERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_PLENDERR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_PLENDERR_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_PLENDERR_PLENDERR_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_PLENDERR_REG = 
{
    "PLENDERR",
#if RU_INCLUDE_DESC
    "CRC_ERRORS_IN_PLEND_FIELD Register",
    "Counts the number of CRC Errors in the Plend field",
#endif
    GPON_RX_PM_COUNTER_PLENDERR_REG_OFFSET,
    0,
    0,
    122,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_PLENDERR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_SFMISMATCH
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_SFMISMATCH_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_SFMISMATCH_SFMISMATCH_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_SFMISMATCH_REG = 
{
    "SFMISMATCH",
#if RU_INCLUDE_DESC
    "SUPERFRAME_COUNTER_MISMATCH_COUNTER Register",
    "Counts the number of invalid (non-consecutive) superframe counter values",
#endif
    GPON_RX_PM_COUNTER_SFMISMATCH_REG_OFFSET,
    0,
    0,
    123,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_SFMISMATCH_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_BIP
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_BIP_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_BIP_BIP_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_BIP_REG = 
{
    "BIP",
#if RU_INCLUDE_DESC
    "BIP_ERROR_COUNTER Register",
    "Counts the number of BIP errors detected since the counter was last read",
#endif
    GPON_RX_PM_COUNTER_BIP_REG_OFFSET,
    0,
    0,
    124,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_BIP_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_GEMFRAG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_GEMFRAG_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_GEMFRAG_GEMFRAG_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_GEMFRAG_REG = 
{
    "GEMFRAG",
#if RU_INCLUDE_DESC
    "NON_IDLE_GEM_FRAGMENTS_RECEIVED Register",
    "Counts the total number of non-idle GEM fragments received (before filtering)",
#endif
    GPON_RX_PM_COUNTER_GEMFRAG_REG_OFFSET,
    0,
    0,
    125,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_GEMFRAG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_ETHPACK
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_ETHPACK_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_ETHPACK_ETHPACK_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_ETHPACK_REG = 
{
    "ETHPACK",
#if RU_INCLUDE_DESC
    "ETHERNET_PACKETS_DIRECTED_AT_ONU Register",
    "Counts the number of Ethernet packets which passed the Port-ID filtering",
#endif
    GPON_RX_PM_COUNTER_ETHPACK_REG_OFFSET,
    0,
    0,
    126,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_ETHPACK_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_TDMFRAG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_TDMFRAG_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_TDMFRAG_TDMFRAG_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_TDMFRAG_REG = 
{
    "TDMFRAG",
#if RU_INCLUDE_DESC
    "TDM_FRAGMENTS_DIRECTED_AT_ONU Register",
    "Counts the number of TDM fragments which passed the Port-ID filtering",
#endif
    GPON_RX_PM_COUNTER_TDMFRAG_REG_OFFSET,
    0,
    0,
    127,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_TDMFRAG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_SYMERROR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_SYMERROR_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_SYMERROR_SYMERRORS_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_SYMERROR_REG = 
{
    "SYMERROR",
#if RU_INCLUDE_DESC
    "FEC_SYMBOL_ERRORS_CORRECTED Register",
    "Counts the number of bytes the FEC decoder corrected. This counter will freeze when it crosses 0xFFFF_FFF0.",
#endif
    GPON_RX_PM_COUNTER_SYMERROR_REG_OFFSET,
    0,
    0,
    128,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_SYMERROR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_BITERROR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_BITERROR_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_BITERROR_BITERRORS_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_BITERROR_REG = 
{
    "BITERROR",
#if RU_INCLUDE_DESC
    "FEC_BIT_ERRORS_CORRECTED Register",
    "Counts the number of bit errors the FEC block corrected. This counter will freeze when it crosses 0xFFFF_FF00.",
#endif
    GPON_RX_PM_COUNTER_BITERROR_REG_OFFSET,
    0,
    0,
    129,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_BITERROR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_AESERROR
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_AESERROR_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_AESERROR_AESERRCNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_AESERROR_REG = 
{
    "AESERROR",
#if RU_INCLUDE_DESC
    "AES_MACHINE_ALLOCATION_ERRORS Register",
    "counts the number of times the cipher mechanism couldnt allocate a decryption machine",
#endif
    GPON_RX_PM_COUNTER_AESERROR_REG_OFFSET,
    0,
    0,
    130,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_AESERROR_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_TDMSTATS
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_TDMSTATS_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_TDMSTATS_RCVBYTES_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_TDMSTATS_REG = 
{
    "TDMSTATS",
#if RU_INCLUDE_DESC
    "TDM_STATISTICS Register",
    "counts the number of bytes correctly received by the TDM interface from GEM fragments. This counter will freeze when it crosses 0xFFFF_FFFC.",
#endif
    GPON_RX_PM_COUNTER_TDMSTATS_REG_OFFSET,
    0,
    0,
    131,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_TDMSTATS_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_ETHFRAG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_ETHFRAG_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_ETHFRAG_ETHFRAG_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_ETHFRAG_REG = 
{
    "ETHFRAG",
#if RU_INCLUDE_DESC
    "NUMBER_OF_ETHERNET_FRAGMENTS Register",
    "Counts the number of Ethernet fragments which passed Port-ID filtering",
#endif
    GPON_RX_PM_COUNTER_ETHFRAG_REG_OFFSET,
    0,
    0,
    132,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_ETHFRAG_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_ETHBYTECNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_ETHBYTECNT_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_ETHBYTECNT_ETHBYTECNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_ETHBYTECNT_REG = 
{
    "ETHBYTECNT",
#if RU_INCLUDE_DESC
    "TOTAL_NUMBER_OF_ETHERNET_BYTES_RECEIVED_AFTER_FIL Register",
    "Counts the number of total Ethernet bytes received after filtering. This counter will freeze when it crosses 0xFFFF_F",
#endif
    GPON_RX_PM_COUNTER_ETHBYTECNT_REG_OFFSET,
    0,
    0,
    133,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_ETHBYTECNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_FECUCCWCNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_FECUCCWCNT_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_FECUCCWCNT_FECUCCWCNT_FIELD,
    &GPON_RX_PM_COUNTER_FECUCCWCNT_R1_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_FECUCCWCNT_REG = 
{
    "FECUCCWCNT",
#if RU_INCLUDE_DESC
    "FEC_UNCORRECTABLE_CODEWORD_COUNTER Register",
    "Counts the number of codewords the FEC decoder was unable to correct",
#endif
    GPON_RX_PM_COUNTER_FECUCCWCNT_REG_OFFSET,
    0,
    0,
    134,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    2,
    GPON_RX_PM_COUNTER_FECUCCWCNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_FECCCWCNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_FECCCWCNT_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_FECCCWCNT_FECCCWCNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_FECCCWCNT_REG = 
{
    "FECCCWCNT",
#if RU_INCLUDE_DESC
    "FEC_CORRECTABLE_CODEWORD_COUNTER Register",
    "Counts the number of codewords which contained at least one error and the FEC decoder succeeded to decode",
#endif
    GPON_RX_PM_COUNTER_FECCCWCNT_REG_OFFSET,
    0,
    0,
    135,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_FECCCWCNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_PLONUCNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_PLONUCNT_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_PLONUCNT_PL_ONU_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_PLONUCNT_REG = 
{
    "PLONUCNT",
#if RU_INCLUDE_DESC
    "VALID_PLOAM_CNT_ONUID Register",
    "counts the number of valid received PLOAM messages passed the ONU ID filter",
#endif
    GPON_RX_PM_COUNTER_PLONUCNT_REG_OFFSET,
    0,
    0,
    136,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_PLONUCNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_PLBCSTCNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_PLBCSTCNT_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_PLBCSTCNT_PL_BCST_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_PLBCSTCNT_REG = 
{
    "PLBCSTCNT",
#if RU_INCLUDE_DESC
    "VALID_PLOAM_CNT_BROADCAST Register",
    "counts the number of valid received PLOAM messages passed the BROADCAST ONU ID filter",
#endif
    GPON_RX_PM_COUNTER_PLBCSTCNT_REG_OFFSET,
    0,
    0,
    137,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_PLBCSTCNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: GPON_RX_PM_COUNTER_BW_CNT
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *GPON_RX_PM_COUNTER_BW_CNT_FIELDS[] =
{
    &GPON_RX_PM_COUNTER_BW_CNT_BW_CNT_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec GPON_RX_PM_COUNTER_BW_CNT_REG = 
{
    "BW_CNT",
#if RU_INCLUDE_DESC
    "BANDWIDTH_ALLOC_CNT %i Register",
    "set of 4 counters."
    "Each counter can be associated to each one of the 40 TCONTs"
    "Each counter accumaltes the allocations received to its associated TCONT (cnter = cntr + sstop - sstart)",
#endif
    GPON_RX_PM_COUNTER_BW_CNT_REG_OFFSET,
    GPON_RX_PM_COUNTER_BW_CNT_REG_RAM_CNT,
    4,
    138,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    1,
    GPON_RX_PM_COUNTER_BW_CNT_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: GPON_RX_PM_COUNTER
 ******************************************************************************/
static const ru_reg_rec *GPON_RX_PM_COUNTER_REGS[] =
{
    &GPON_RX_PM_COUNTER_BWMAPERR_REG,
    &GPON_RX_PM_COUNTER_PLOAMERR_REG,
    &GPON_RX_PM_COUNTER_PLENDERR_REG,
    &GPON_RX_PM_COUNTER_SFMISMATCH_REG,
    &GPON_RX_PM_COUNTER_BIP_REG,
    &GPON_RX_PM_COUNTER_GEMFRAG_REG,
    &GPON_RX_PM_COUNTER_ETHPACK_REG,
    &GPON_RX_PM_COUNTER_TDMFRAG_REG,
    &GPON_RX_PM_COUNTER_SYMERROR_REG,
    &GPON_RX_PM_COUNTER_BITERROR_REG,
    &GPON_RX_PM_COUNTER_AESERROR_REG,
    &GPON_RX_PM_COUNTER_TDMSTATS_REG,
    &GPON_RX_PM_COUNTER_ETHFRAG_REG,
    &GPON_RX_PM_COUNTER_ETHBYTECNT_REG,
    &GPON_RX_PM_COUNTER_FECUCCWCNT_REG,
    &GPON_RX_PM_COUNTER_FECCCWCNT_REG,
    &GPON_RX_PM_COUNTER_PLONUCNT_REG,
    &GPON_RX_PM_COUNTER_PLBCSTCNT_REG,
    &GPON_RX_PM_COUNTER_BW_CNT_REG,
};

unsigned long GPON_RX_PM_COUNTER_ADDRS[] =
{
#if defined(CONFIG_BCM963158)
    0x80150a00,
#elif defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856)
    0x82db1a00,
#else
    #error "GPON_RX_PM_COUNTER base address not defined"
#endif
};

const ru_block_rec GPON_RX_PM_COUNTER_BLOCK = 
{
    "GPON_RX_PM_COUNTER",
    GPON_RX_PM_COUNTER_ADDRS,
    1,
    19,
    GPON_RX_PM_COUNTER_REGS
};

/* End of file BCM6836_A0GPON_RX_PM_COUNTER.c */
