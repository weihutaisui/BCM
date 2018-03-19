/***************************************************************************
 *
 *     Copyright (c) 2008-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *  Description: Generated libmoca functions - DO NOT EDIT
 *
<:label-BRCM:2017:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 ***************************************************************************/

struct moca_ie_names {
   uint32_t ie_type;
   const char *name;
};

const struct moca_ie_names moca_ie_names[] = {

   // Group node
   {IE_PREFERRED_NC, "PREFERRED_NC"},
   {IE_SINGLE_CHANNEL_OPERATION, "SINGLE_CHANNEL_OPERATION"},
   {IE_CONTINUOUS_POWER_TX_MODE, "CONTINUOUS_POWER_TX_MODE"},
   {IE_CONTINUOUS_RX_MODE_ATTN, "CONTINUOUS_RX_MODE_ATTN"},
   {IE_LOF, "LOF"},
   {IE_MAX_NBAS_PRIMARY, "MAX_NBAS_PRIMARY"},
   {IE_PS_SWCH_TX1, "PS_SWCH_TX1"},
   {IE_PS_SWCH_TX2, "PS_SWCH_TX2"},
   {IE_PS_SWCH_RX1, "PS_SWCH_RX1"},
   {IE_PS_SWCH_RX2, "PS_SWCH_RX2"},
   {IE_PS_SWCH_RX3, "PS_SWCH_RX3"},
   {IE_PRIM_CH_OFFS, "PRIM_CH_OFFS"},
   {IE_SEC_CH_OFFS, "SEC_CH_OFFS"},
   {IE_NON_DEF_SEQ_NUM, "NON_DEF_SEQ_NUM"},
   {IE_BONDING, "BONDING"},
   {IE_LISTENING_FREQ_MASK, "LISTENING_FREQ_MASK"},
   {IE_LISTENING_DURATION, "LISTENING_DURATION"},
   {IE_LIMIT_TRAFFIC, "LIMIT_TRAFFIC"},
   {IE_REMOTE_MAN, "REMOTE_MAN"},
   {IE_C4_MOCA20_EN, "C4_MOCA20_EN"},
   {IE_POWER_SAVE_MECHANISM_DIS, "POWER_SAVE_MECHANISM_DIS"},
   {IE_PSM_CONFIG, "PSM_CONFIG"},
   {IE_USE_EXT_DATA_MEM, "USE_EXT_DATA_MEM"},
   {IE_AIF_MODE, "AIF_MODE"},
   {IE_PROF_PAD_CTRL_DEG_6802C0_BONDING, "PROF_PAD_CTRL_DEG_6802C0_BONDING"},
   {IE_PROP_BONDING_COMPATIBILITY_MODE, "PROP_BONDING_COMPATIBILITY_MODE"},
   {IE_RDEG_3450, "RDEG_3450"},
   {IE_PHY_CLOCK, "PHY_CLOCK"},
   {IE_MAX_NBAS_SECONDARY, "MAX_NBAS_SECONDARY"},
   {IE_MAC_ADDR, "MAC_ADDR"},
   {IE_NODE_STATUS, "NODE_STATUS"},
   {IE_START_MOCA_CORE, "START_MOCA_CORE"},
   {IE_DOWNLOAD_TO_PACKET_RAM_DONE, "DOWNLOAD_TO_PACKET_RAM_DONE"},
   {IE_BEACON_CHANNEL_SET, "BEACON_CHANNEL_SET"},
   {IE_FW_VERSION, "FW_VERSION"},
   {IE_SHUTDOWN, "SHUTDOWN"},
   {IE_MAX_TX_POWER_TUNE, "MAX_TX_POWER_TUNE"},
   {IE_MAX_TX_POWER_TUNE_SEC_CH, "MAX_TX_POWER_TUNE_SEC_CH"},
   {IE_RX_POWER_TUNE, "RX_POWER_TUNE"},
   {IE_MOCAD_FORWARDING_RX_MAC, "MOCAD_FORWARDING_RX_MAC"},
   {IE_MOCAD_FORWARDING_RX_ACK, "MOCAD_FORWARDING_RX_ACK"},
   {IE_MOCAD_FORWARDING_TX_ALLOC, "MOCAD_FORWARDING_TX_ALLOC"},
   {IE_MOCAD_FORWARDING_TX_SEND, "MOCAD_FORWARDING_TX_SEND"},
   {IE_IMPEDANCE_MODE_BONDING, "IMPEDANCE_MODE_BONDING"},
   {IE_REWORK_6802, "REWORK_6802"},
   {IE_PROF_PAD_CTRL_DEG_6802C0_SINGLE, "PROF_PAD_CTRL_DEG_6802C0_SINGLE"},
   {IE_CORE_READY, "CORE_READY"},
   {IE_POWER_UP_STATUS, "POWER_UP_STATUS"},
   {IE_NEW_LOF, "NEW_LOF"},
   {IE_ADMISSION_COMPLETED, "ADMISSION_COMPLETED"},
   {IE_TPCAP_DONE, "TPCAP_DONE"},
   {IE_MOCAD_FORWARDING_RX_PACKET, "MOCAD_FORWARDING_RX_PACKET"},
   {IE_MOCAD_FORWARDING_TX_ACK, "MOCAD_FORWARDING_TX_ACK"},
   {IE_PR_DEGRADATION, "PR_DEGRADATION"},
   {IE_START, "START"},
   {IE_STOP, "STOP"},
   {IE_DRV_INFO, "DRV_INFO"},
   {IE_WDT, "WDT"},
   {IE_ABORT, "ABORT"},
   {IE_MISCVAL, "MISCVAL"},
   {IE_EN_CAPABLE, "EN_CAPABLE"},
   {IE_RESTORE_DEFAULTS, "RESTORE_DEFAULTS"},
   {IE_MOCAD_VERSION, "MOCAD_VERSION"},
   {IE_RESTART, "RESTART"},
   {IE_LOF_UPDATE, "LOF_UPDATE"},
   {IE_PRIMARY_CH_OFFSET, "PRIMARY_CH_OFFSET"},
   {IE_ASSERTTEXT, "ASSERTTEXT"},
   {IE_WDOG_ENABLE, "WDOG_ENABLE"},
   {IE_MISCVAL2, "MISCVAL2"},
   {IE_MR_SEQ_NUM, "MR_SEQ_NUM"},
   {IE_SECONDARY_CH_OFFSET, "SECONDARY_CH_OFFSET"},
   {IE_COF, "COF"},
   {IE_CONFIG_FLAGS, "CONFIG_FLAGS"},
   {IE_AMP_TYPE, "AMP_TYPE"},
   {IE_PM_NOTIFICATION, "PM_NOTIFICATION"},

   // Group phy
   {IE_TPC_EN, "TPC_EN"},
   {IE_MAX_TX_POWER, "MAX_TX_POWER"},
   {IE_BEACON_PWR_REDUCTION, "BEACON_PWR_REDUCTION"},
   {IE_BEACON_PWR_REDUCTION_EN, "BEACON_PWR_REDUCTION_EN"},
   {IE_BO_MODE, "BO_MODE"},
   {IE_QAM256_CAPABILITY, "QAM256_CAPABILITY"},
   {IE_OTF_EN, "OTF_EN"},
   {IE_STAR_TOPOLOGY_EN, "STAR_TOPOLOGY_EN"},
   {IE_OFDMA_EN, "OFDMA_EN"},
   {IE_MIN_BW_ALARM_THRESHOLD, "MIN_BW_ALARM_THRESHOLD"},
   {IE_EN_MAX_RATE_IN_MAX_BO, "EN_MAX_RATE_IN_MAX_BO"},
   {IE_TARGET_PHY_RATE_QAM128, "TARGET_PHY_RATE_QAM128"},
   {IE_TARGET_PHY_RATE_QAM256, "TARGET_PHY_RATE_QAM256"},
   {IE_SAPM_EN, "SAPM_EN"},
   {IE_ARPL_TH_50, "ARPL_TH_50"},
   {IE_RLAPM_EN, "RLAPM_EN"},
   {IE_FREQ_SHIFT, "FREQ_SHIFT"},
   {IE_MAX_PHY_RATE, "MAX_PHY_RATE"},
   {IE_BANDWIDTH, "BANDWIDTH"},
   {IE_ARPL_TH_100, "ARPL_TH_100"},
   {IE_ADC_MODE, "ADC_MODE"},
   {IE_MAX_PHY_RATE_TURBO, "MAX_PHY_RATE_TURBO"},
   {IE_CP_CONST, "CP_CONST"},
   {IE_PREAMBLE_UC_CONST, "PREAMBLE_UC_CONST"},
   {IE_CP_MARGIN_INCREASE, "CP_MARGIN_INCREASE"},
   {IE_AC_CC_SHIFT, "AC_CC_SHIFT"},
   {IE_MFC_TH_INCREASE, "MFC_TH_INCREASE"},
   {IE_AGC_CONST_EN, "AGC_CONST_EN"},
   {IE_AGC_CONST_ADDRESS, "AGC_CONST_ADDRESS"},
   {IE_MIN_BO_INSERT_2_BFM_LOCK, "MIN_BO_INSERT_2_BFM_LOCK"},
   {IE_MIN_SNR_AVG_DB_2_BFM_LOCK, "MIN_SNR_AVG_DB_2_BFM_LOCK"},
   {IE_MAX_PHY_RATE_50M, "MAX_PHY_RATE_50M"},
   {IE_MAX_CONSTELLATION_ALL, "MAX_CONSTELLATION_ALL"},
   {IE_MAX_CONSTELLATION, "MAX_CONSTELLATION"},
   {IE_SIG_Y_DONE, "SIG_Y_DONE"},
   {IE_SNR_MARGIN_TABLE_RS, "SNR_MARGIN_TABLE_RS"},
   {IE_SAPM_TABLE_FW_50, "SAPM_TABLE_FW_50"},
   {IE_RLAPM_TABLE_50, "RLAPM_TABLE_50"},
   {IE_PHY_STATUS, "PHY_STATUS"},
   {IE_SNR_MARGIN_TABLE_LDPC, "SNR_MARGIN_TABLE_LDPC"},
   {IE_SNR_MARGIN_TABLE_LDPC_SEC_CH, "SNR_MARGIN_TABLE_LDPC_SEC_CH"},
   {IE_SNR_MARGIN_TABLE_LDPC_PRE5, "SNR_MARGIN_TABLE_LDPC_PRE5"},
   {IE_SNR_MARGIN_TABLE_OFDMA, "SNR_MARGIN_TABLE_OFDMA"},
   {IE_SAPM_TABLE_FW_100, "SAPM_TABLE_FW_100"},
   {IE_RLAPM_TABLE_100, "RLAPM_TABLE_100"},
   {IE_CPLENGTH, "CPLENGTH"},
   {IE_RF_CALIB_DATA, "RF_CALIB_DATA"},
   {IE_PROBE_2_RESULTS, "PROBE_2_RESULTS"},
   {IE_SAPM_TABLE_FW_SEC, "SAPM_TABLE_FW_SEC"},
   {IE_RX_GAIN_PARAMS, "RX_GAIN_PARAMS"},
   {IE_RX_GAIN_AGC_TABLE, "RX_GAIN_AGC_TABLE"},
   {IE_TX_POWER_PARAMS, "TX_POWER_PARAMS"},
   {IE_SNR_MARGIN_TABLE_LDPC_PRI_CH, "SNR_MARGIN_TABLE_LDPC_PRI_CH"},
   {IE_SNR_MARGIN_TABLE_PRE5_PRI_CH, "SNR_MARGIN_TABLE_PRE5_PRI_CH"},
   {IE_SNR_MARGIN_TABLE_PRE5_SEC_CH, "SNR_MARGIN_TABLE_PRE5_SEC_CH"},
   {IE_SIG_Y_READY, "SIG_Y_READY"},
   {IE_NEW_RF_CALIB_DATA, "NEW_RF_CALIB_DATA"},
   {IE_NEW_PROBE_2_RESULTS, "NEW_PROBE_2_RESULTS"},
   {IE_NV_CAL_ENABLE, "NV_CAL_ENABLE"},
   {IE_RLAPM_CAP_50, "RLAPM_CAP_50"},
   {IE_SNR_MARGIN_RS, "SNR_MARGIN_RS"},
   {IE_SNR_MARGIN_LDPC, "SNR_MARGIN_LDPC"},
   {IE_SNR_MARGIN_LDPC_SEC_CH, "SNR_MARGIN_LDPC_SEC_CH"},
   {IE_SNR_MARGIN_LDPC_PRE5, "SNR_MARGIN_LDPC_PRE5"},
   {IE_SNR_MARGIN_OFDMA, "SNR_MARGIN_OFDMA"},
   {IE_RLAPM_CAP_100, "RLAPM_CAP_100"},
   {IE_SAPM_TABLE_50, "SAPM_TABLE_50"},
   {IE_SAPM_TABLE_100, "SAPM_TABLE_100"},
   {IE_NV_CAL_CLEAR, "NV_CAL_CLEAR"},
   {IE_SAPM_TABLE_SEC, "SAPM_TABLE_SEC"},
   {IE_AMP_REG, "AMP_REG"},
   {IE_SNR_MARGIN_LDPC_PRI_CH, "SNR_MARGIN_LDPC_PRI_CH"},
   {IE_SNR_MARGIN_PRE5_PRI_CH, "SNR_MARGIN_PRE5_PRI_CH"},
   {IE_SNR_MARGIN_PRE5_SEC_CH, "SNR_MARGIN_PRE5_SEC_CH"},

   // Group mac_layer
   {IE_MAX_FRAME_SIZE, "MAX_FRAME_SIZE"},
   {IE_MIN_AGGR_WAITING_TIME, "MIN_AGGR_WAITING_TIME"},
   {IE_SELECTIVE_RR, "SELECTIVE_RR"},
   {IE_FRAGMENTATION, "FRAGMENTATION"},
   {IE_IFG_MOCA20, "IFG_MOCA20"},
   {IE_MAP_SEED, "MAP_SEED"},
   {IE_MAX_TRANSMIT_TIME, "MAX_TRANSMIT_TIME"},
   {IE_MAX_PKT_AGGR, "MAX_PKT_AGGR"},
   {IE_PROBE_REQUEST, "PROBE_REQUEST"},
   {IE_RTR_CONFIG, "RTR_CONFIG"},
   {IE_TLP_MODE, "TLP_MODE"},
   {IE_MAX_PKT_AGGR_BONDING, "MAX_PKT_AGGR_BONDING"},

   // Group forwarding
   {IE_MULTICAST_MODE, "MULTICAST_MODE"},
   {IE_LOW_PRI_Q_NUM, "LOW_PRI_Q_NUM"},
   {IE_EGR_MC_FILTER_EN, "EGR_MC_FILTER_EN"},
   {IE_FC_MODE, "FC_MODE"},
   {IE_PQOS_MAX_PACKET_SIZE, "PQOS_MAX_PACKET_SIZE"},
   {IE_PER_MODE, "PER_MODE"},
   {IE_POLICING_EN, "POLICING_EN"},
   {IE_PQOS_EGRESS_NUMFLOWS, "PQOS_EGRESS_NUMFLOWS"},
   {IE_ORR_EN, "ORR_EN"},
   {IE_BRCMTAG_ENABLE, "BRCMTAG_ENABLE"},
   {IE_PRIORITY_ALLOCATIONS, "PRIORITY_ALLOCATIONS"},
   {IE_EGR_MC_ADDR_FILTER, "EGR_MC_ADDR_FILTER"},
   {IE_PQOS_CREATE_REQUEST, "PQOS_CREATE_REQUEST"},
   {IE_PQOS_INGR_ADD_FLOW, "PQOS_INGR_ADD_FLOW"},
   {IE_PQOS_UPDATE_REQUEST, "PQOS_UPDATE_REQUEST"},
   {IE_PQOS_INGR_UPDATE, "PQOS_INGR_UPDATE"},
   {IE_PQOS_DELETE_REQUEST, "PQOS_DELETE_REQUEST"},
   {IE_PQOS_INGR_DELETE, "PQOS_INGR_DELETE"},
   {IE_PQOS_LIST_REQUEST, "PQOS_LIST_REQUEST"},
   {IE_PQOS_QUERY_REQUEST, "PQOS_QUERY_REQUEST"},
   {IE_PQOS_MAINTENANCE_START, "PQOS_MAINTENANCE_START"},
   {IE_UC_FWD, "UC_FWD"},
   {IE_MC_FWD, "MC_FWD"},
   {IE_SRC_ADDR, "SRC_ADDR"},
   {IE_MAC_AGING, "MAC_AGING"},
   {IE_LOOPBACK_EN, "LOOPBACK_EN"},
   {IE_MCFILTER_ENABLE, "MCFILTER_ENABLE"},
   {IE_MCFILTER_ADDENTRY, "MCFILTER_ADDENTRY"},
   {IE_MCFILTER_DELENTRY, "MCFILTER_DELENTRY"},
   {IE_PAUSE_FC_EN, "PAUSE_FC_EN"},
   {IE_STAG_PRIORITY, "STAG_PRIORITY"},
   {IE_STAG_REMOVAL, "STAG_REMOVAL"},
   {IE_EPORT_LINK_STATE, "EPORT_LINK_STATE"},
   {IE_UCFWD_UPDATE, "UCFWD_UPDATE"},
   {IE_PQOS_CREATE_RESPONSE, "PQOS_CREATE_RESPONSE"},
   {IE_PQOS_CREATE_COMPLETE, "PQOS_CREATE_COMPLETE"},
   {IE_PQOS_UPDATE_RESPONSE, "PQOS_UPDATE_RESPONSE"},
   {IE_PQOS_UPDATE_COMPLETE, "PQOS_UPDATE_COMPLETE"},
   {IE_PQOS_DELETE_RESPONSE, "PQOS_DELETE_RESPONSE"},
   {IE_PQOS_DELETE_COMPLETE, "PQOS_DELETE_COMPLETE"},
   {IE_PQOS_LIST_RESPONSE, "PQOS_LIST_RESPONSE"},
   {IE_PQOS_QUERY_RESPONSE, "PQOS_QUERY_RESPONSE"},
   {IE_PQOS_MAINTENANCE_COMPLETE, "PQOS_MAINTENANCE_COMPLETE"},
   {IE_PQOS_CREATE_FLOW, "PQOS_CREATE_FLOW"},
   {IE_PQOS_UPDATE_FLOW, "PQOS_UPDATE_FLOW"},
   {IE_PQOS_DELETE_FLOW, "PQOS_DELETE_FLOW"},
   {IE_PQOS_LIST, "PQOS_LIST"},
   {IE_PQOS_QUERY, "PQOS_QUERY"},
   {IE_PQOS_STATUS, "PQOS_STATUS"},
   {IE_MCFILTER_CLEAR_TABLE, "MCFILTER_CLEAR_TABLE"},
   {IE_MCFILTER_TABLE, "MCFILTER_TABLE"},
   {IE_HOST_QOS, "HOST_QOS"},

   // Group network
   {IE_OOO_LMO_THRESHOLD, "OOO_LMO_THRESHOLD"},
   {IE_TABOO_CHANNELS, "TABOO_CHANNELS"},
   {IE_FMR_REQUEST, "FMR_REQUEST"},
   {IE_MR_REQUEST, "MR_REQUEST"},
   {IE_GEN_NODE_STATUS, "GEN_NODE_STATUS"},
   {IE_GEN_NODE_EXT_STATUS, "GEN_NODE_EXT_STATUS"},
   {IE_NODE_STATS, "NODE_STATS"},
   {IE_NODE_STATS_EXT, "NODE_STATS_EXT"},
   {IE_NETWORK_STATUS, "NETWORK_STATUS"},
   {IE_OOO_LMO, "OOO_LMO"},
   {IE_START_ULMO, "START_ULMO"},
   {IE_DD_REQUEST, "DD_REQUEST"},
   {IE_FMR20_REQUEST, "FMR20_REQUEST"},
   {IE_RXD_LMO_REQUEST, "RXD_LMO_REQUEST"},
   {IE_OFDMA_DEFINITION_TABLE, "OFDMA_DEFINITION_TABLE"},
   {IE_OFDMA_ASSIGNMENT_TABLE, "OFDMA_ASSIGNMENT_TABLE"},
   {IE_START_ACA, "START_ACA"},
   {IE_ADM_STATS, "ADM_STATS"},
   {IE_ADMISSION_STATUS, "ADMISSION_STATUS"},
   {IE_LIMITED_BW, "LIMITED_BW"},
   {IE_LMO_INFO, "LMO_INFO"},
   {IE_TOPOLOGY_CHANGED, "TOPOLOGY_CHANGED"},
   {IE_MOCA_VERSION_CHANGED, "MOCA_VERSION_CHANGED"},
   {IE_MOCA_RESET_REQUEST, "MOCA_RESET_REQUEST"},
   {IE_NC_ID_CHANGED, "NC_ID_CHANGED"},
   {IE_FMR_RESPONSE, "FMR_RESPONSE"},
   {IE_MR_RESPONSE, "MR_RESPONSE"},
   {IE_MR_COMPLETE, "MR_COMPLETE"},
   {IE_CPU_CHECK, "CPU_CHECK"},
   {IE_DD_RESPONSE, "DD_RESPONSE"},
   {IE_FMR20_RESPONSE, "FMR20_RESPONSE"},
   {IE_PKTRAM_FILL, "PKTRAM_FILL"},
   {IE_MR_EVENT, "MR_EVENT"},
   {IE_HOSTLESS_MODE_RESPONSE, "HOSTLESS_MODE_RESPONSE"},
   {IE_WAKEUP_NODE_RESPONSE, "WAKEUP_NODE_RESPONSE"},
   {IE_ACA_RESULT_1, "ACA_RESULT_1"},
   {IE_ACA_RESULT_2, "ACA_RESULT_2"},
   {IE_ACA, "ACA"},
   {IE_FMR_INIT, "FMR_INIT"},
   {IE_MOCA_RESET, "MOCA_RESET"},
   {IE_DD_INIT, "DD_INIT"},
   {IE_FMR_20, "FMR_20"},
   {IE_ERROR_STATS, "ERROR_STATS"},
   {IE_HOSTLESS_MODE, "HOSTLESS_MODE"},
   {IE_WAKEUP_NODE, "WAKEUP_NODE"},
   {IE_LAST_MR_EVENTS, "LAST_MR_EVENTS"},

   // Group intfc
   {IE_RF_BAND, "RF_BAND"},
   {IE_RF_SWITCH, "RF_SWITCH"},
   {IE_IF_ACCESS_EN, "IF_ACCESS_EN"},
   {IE_LED_MODE, "LED_MODE"},
   {IE_GEN_STATS, "GEN_STATS"},
   {IE_INTERFACE_STATUS, "INTERFACE_STATUS"},
   {IE_RESET_CORE_STATS, "RESET_CORE_STATS"},
   {IE_IF_ACCESS_TABLE, "IF_ACCESS_TABLE"},
   {IE_LINK_UP_STATE, "LINK_UP_STATE"},
   {IE_NEW_RF_BAND, "NEW_RF_BAND"},
   {IE_EXT_OCTET_COUNT, "EXT_OCTET_COUNT"},
   {IE_RESET_STATS, "RESET_STATS"},

   // Group power_mgmt
   {IE_M1_TX_POWER_VARIATION, "M1_TX_POWER_VARIATION"},
   {IE_NC_LISTENING_INTERVAL, "NC_LISTENING_INTERVAL"},
   {IE_NC_HEARTBEAT_INTERVAL, "NC_HEARTBEAT_INTERVAL"},
   {IE_WOM_MODE_INTERNAL, "WOM_MODE_INTERNAL"},
   {IE_WOM_MAGIC_ENABLE, "WOM_MAGIC_ENABLE"},
   {IE_PM_RESTORE_ON_LINK_DOWN, "PM_RESTORE_ON_LINK_DOWN"},
   {IE_POWER_STATE, "POWER_STATE"},
   {IE_HOSTLESS_MODE_REQUEST, "HOSTLESS_MODE_REQUEST"},
   {IE_WAKEUP_NODE_REQUEST, "WAKEUP_NODE_REQUEST"},
   {IE_NODE_POWER_STATE, "NODE_POWER_STATE"},
   {IE_FILTER_M2_DATA_WAKEUP, "FILTER_M2_DATA_WAKEUP"},
   {IE_WOM_PATTERN, "WOM_PATTERN"},
   {IE_WOM_IP, "WOM_IP"},
   {IE_WOM_MAGIC_MAC, "WOM_MAGIC_MAC"},
   {IE_STANDBY_POWER_STATE, "STANDBY_POWER_STATE"},
   {IE_WOM_MODE, "WOM_MODE"},
   {IE_POWER_STATE_RSP, "POWER_STATE_RSP"},
   {IE_POWER_STATE_EVENT, "POWER_STATE_EVENT"},
   {IE_POWER_STATE_CAP, "POWER_STATE_CAP"},
   {IE_WOL, "WOL"},
   {IE_PS_CMD, "PS_CMD"},
   {IE_POWER_STATE_CAPABILITIES, "POWER_STATE_CAPABILITIES"},
   {IE_LAST_PS_EVENT_CODE, "LAST_PS_EVENT_CODE"},

   // Group security
   {IE_PRIVACY_EN, "PRIVACY_EN"},
   {IE_PMK_EXCHANGE_INTERVAL, "PMK_EXCHANGE_INTERVAL"},
   {IE_TEK_EXCHANGE_INTERVAL, "TEK_EXCHANGE_INTERVAL"},
   {IE_AES_EXCHANGE_INTERVAL, "AES_EXCHANGE_INTERVAL"},
   {IE_MMK_KEY, "MMK_KEY"},
   {IE_PMK_INITIAL_KEY, "PMK_INITIAL_KEY"},
   {IE_AES_MM_KEY, "AES_MM_KEY"},
   {IE_AES_PM_KEY, "AES_PM_KEY"},
   {IE_CURRENT_KEYS, "CURRENT_KEYS"},
   {IE_NETWORK_PASSWORD, "NETWORK_PASSWORD"},
   {IE_PERMANENT_SALT, "PERMANENT_SALT"},
   {IE_AES_PMK_INITIAL_KEY, "AES_PMK_INITIAL_KEY"},
   {IE_KEY_CHANGED, "KEY_CHANGED"},
   {IE_KEY_TIMES, "KEY_TIMES"},
   {IE_PASSWORD, "PASSWORD"},

   // Group debug
   {IE_MTM_EN, "MTM_EN"},
   {IE_CONST_RX_SUBMODE, "CONST_RX_SUBMODE"},
   {IE_CIR_PRINTS, "CIR_PRINTS"},
   {IE_SNR_PRINTS, "SNR_PRINTS"},
   {IE_MMP_VERSION, "MMP_VERSION"},
   {IE_SIGMA2_PRINTS, "SIGMA2_PRINTS"},
   {IE_BAD_PROBE_PRINTS, "BAD_PROBE_PRINTS"},
   {IE_CONST_TX_PARAMS, "CONST_TX_PARAMS"},
   {IE_GMII_TRAP_HEADER, "GMII_TRAP_HEADER"},
   {IE_LED_STATUS, "LED_STATUS"},
   {IE_MOCA_CORE_TRACE_ENABLE, "MOCA_CORE_TRACE_ENABLE"},
   {IE_ERROR, "ERROR"},
   {IE_FW_PRINTF, "FW_PRINTF"},
   {IE_WARNING, "WARNING"},
   {IE_ERROR_LOOKUP, "ERROR_LOOKUP"},
   {IE_WARNING_LOOKUP, "WARNING_LOOKUP"},
   {IE_ERROR_TO_MASK, "ERROR_TO_MASK"},
   {IE_FW_FILE, "FW_FILE"},
   {IE_VERBOSE, "VERBOSE"},
   {IE_DONT_START_MOCA, "DONT_START_MOCA"},
   {IE_NO_RTT, "NO_RTT"},
   {IE_MOCAD_PRINTF, "MOCAD_PRINTF"},

   // Group lab
   {IE_LAB_MODE, "LAB_MODE"},
   {IE_NC_MODE, "NC_MODE"},
   {IE_RX_TX_PACKETS_PER_QM, "RX_TX_PACKETS_PER_QM"},
   {IE_EXTRA_RX_PACKETS_PER_QM, "EXTRA_RX_PACKETS_PER_QM"},
   {IE_TARGET_PHY_RATE_20, "TARGET_PHY_RATE_20"},
   {IE_TARGET_PHY_RATE_20_TURBO, "TARGET_PHY_RATE_20_TURBO"},
   {IE_TURBO_EN, "TURBO_EN"},
   {IE_RES1, "RES1"},
   {IE_RES2, "RES2"},
   {IE_RES3, "RES3"},
   {IE_RES4, "RES4"},
   {IE_RES5, "RES5"},
   {IE_RES6, "RES6"},
   {IE_RES7, "RES7"},
   {IE_RES8, "RES8"},
   {IE_RES9, "RES9"},
   {IE_INIT1, "INIT1"},
   {IE_INIT2, "INIT2"},
   {IE_INIT3, "INIT3"},
   {IE_INIT4, "INIT4"},
   {IE_INIT5, "INIT5"},
   {IE_INIT6, "INIT6"},
   {IE_INIT7, "INIT7"},
   {IE_INIT8, "INIT8"},
   {IE_INIT9, "INIT9"},
   {IE_LAB_SNR_GRAPH_SET, "LAB_SNR_GRAPH_SET"},
   {IE_BLOCK_NACK_RATE, "BLOCK_NACK_RATE"},
   {IE_RES10, "RES10"},
   {IE_RES11, "RES11"},
   {IE_RES12, "RES12"},
   {IE_RES13, "RES13"},
   {IE_RES14, "RES14"},
   {IE_RES15, "RES15"},
   {IE_RES16, "RES16"},
   {IE_RES17, "RES17"},
   {IE_RES18, "RES18"},
   {IE_RES19, "RES19"},
   {IE_RES20, "RES20"},
   {IE_TARGET_PHY_RATE_20_TURBO_VLPER, "TARGET_PHY_RATE_20_TURBO_VLPER"},
   {IE_TARGET_PHY_RATE_20_SEC_CH, "TARGET_PHY_RATE_20_SEC_CH"},
   {IE_TARGET_PHY_RATE_20_TURBO_SEC_CH, "TARGET_PHY_RATE_20_TURBO_SEC_CH"},
   {IE_TARGET_PHY_RATE_20_TURBO_VLPER_SEC_CH, "TARGET_PHY_RATE_20_TURBO_VLPER_SEC_CH"},
   {IE_CAP_PHY_RATE_EN, "CAP_PHY_RATE_EN"},
   {IE_CAP_TARGET_PHY_RATE, "CAP_TARGET_PHY_RATE"},
   {IE_CAP_SNR_BASE_MARGIN, "CAP_SNR_BASE_MARGIN"},
   {IE_MAP_CAPTURE, "MAP_CAPTURE"},
   {IE_LAB_PILOTS, "LAB_PILOTS"},
   {IE_LAB_IQ_DIAGRAM_SET, "LAB_IQ_DIAGRAM_SET"},
   {IE_LAB_REGISTER, "LAB_REGISTER"},
   {IE_LAB_CMD, "LAB_CMD"},
   {IE_LAB_TPCAP, "LAB_TPCAP"},
   {IE_HOST_POOL, "HOST_POOL"},
   {IE_FORCE_HANDOFF, "FORCE_HANDOFF"},
   {IE_TPCAP_CAPTURE_TIME, "TPCAP_CAPTURE_TIME"},
   {IE_LAB_PRINTF, "LAB_PRINTF"},
   {IE_LAB_PRINTF_CODES, "LAB_PRINTF_CODES"},
   {IE_ASSERT, "ASSERT"},
   {IE_MIPS_EXCEPTION, "MIPS_EXCEPTION"},
   {IE_DRV_PRINTF, "DRV_PRINTF"},
   {IE_LAB_PRINTF_CODES_LOOKUP, "LAB_PRINTF_CODES_LOOKUP"},
   {IE_MESSAGE, "MESSAGE"},
   {IE_SNR_DATA, "SNR_DATA"},
   {IE_IQ_DATA, "IQ_DATA"},
   {IE_CIR_DATA, "CIR_DATA"},
   {IE_ASSERT_RESTART, "ASSERT_RESTART"},
   {IE_MOCA_CPU_FREQ, "MOCA_CPU_FREQ"},

   // Group mps
   {IE_MPS_EN, "MPS_EN"},
   {IE_MPS_PRIVACY_RECEIVE, "MPS_PRIVACY_RECEIVE"},
   {IE_MPS_PRIVACY_DOWN, "MPS_PRIVACY_DOWN"},
   {IE_MPS_WALK_TIME, "MPS_WALK_TIME"},
   {IE_MPS_UNPAIRED_TIME, "MPS_UNPAIRED_TIME"},
   {IE_MPS_STATE, "MPS_STATE"},
   {IE_MPS_INIT_SCAN_PAYLOAD, "MPS_INIT_SCAN_PAYLOAD"},
   {IE_MPS_TRIG, "MPS_TRIG"},
   {IE_MPS_DATA, "MPS_DATA"},
   {IE_MPS_KEY, "MPS_KEY"},
   {IE_MPS_PRIVACY_CHANGED, "MPS_PRIVACY_CHANGED"},
   {IE_MPS_TRIGGER, "MPS_TRIGGER"},
   {IE_MPS_PAIR_FAIL, "MPS_PAIR_FAIL"},
   {IE_INIT_SCAN_REC, "INIT_SCAN_REC"},
   {IE_MPS_REQUEST_MPSKEY, "MPS_REQUEST_MPSKEY"},
   {IE_MPS_ADMISSION_NOCHANGE, "MPS_ADMISSION_NOCHANGE"},
   {IE_MPS_BUTTON_PRESS, "MPS_BUTTON_PRESS"},
   {IE_MPS_RESET, "MPS_RESET"},
   {IE_PRIVACY_DEFAULTS, "PRIVACY_DEFAULTS"},
   {IE_MPS_DATA_READY, "MPS_DATA_READY"},
};

const char *moca_ie_name(uint32_t ie_type)
{
   unsigned int i;
   for(i=0; i<sizeof(moca_ie_names)/sizeof(struct moca_ie_names); i++) {
      if (moca_ie_names[i].ie_type == ie_type) return moca_ie_names[i].name;
   }
   return NULL;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_preferred_nc_defaults(uint32_t *preferred_nc, uint32_t flags)
{
   if ((flags & MOCA_BAND_E_FLAG)) {
      *preferred_nc = MOCA_PREFERRED_NC_BAND_E_DEF;
   }
   else if ((flags & MOCA_BAND_F_FLAG)) {
      *preferred_nc = MOCA_PREFERRED_NC_BAND_F_DEF;
   }
   else {
      *preferred_nc = MOCA_PREFERRED_NC_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_single_channel_operation_defaults(uint32_t *single_channel_operation, uint32_t flags)
{
   if ((flags & MOCA_BAND_GENERIC_FLAG)) {
      *single_channel_operation = MOCA_SINGLE_CHANNEL_OPERATION_BAND_GENERIC_DEF;
   }
   else {
      *single_channel_operation = MOCA_SINGLE_CHANNEL_OPERATION_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_continuous_power_tx_mode_defaults(uint32_t *continuous_power_tx_mode, uint32_t flags)
{
   *continuous_power_tx_mode = MOCA_CONTINUOUS_POWER_TX_MODE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_continuous_rx_mode_attn_defaults(int32_t *continuous_rx_mode_attn, uint32_t flags)
{
   *continuous_rx_mode_attn = MOCA_CONTINUOUS_RX_MODE_ATTN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_lof_defaults(uint32_t *lof, uint32_t flags)
{
   if ((flags & MOCA_BAND_EX_D_FLAG)) {
      *lof = MOCA_LOF_BAND_EX_D_DEF;
   }
   else if ((flags & MOCA_BAND_D_LOW_FLAG)) {
      *lof = MOCA_LOF_BAND_D_LOW_DEF;
   }
   else if ((flags & MOCA_BAND_GENERIC_FLAG)) {
      *lof = MOCA_LOF_BAND_GENERIC_DEF;
   }
   else if ((flags & MOCA_BAND_D_HIGH_FLAG)) {
      *lof = MOCA_LOF_BAND_D_HIGH_DEF;
   }
   else if ((flags & MOCA_BAND_E_FLAG)) {
      *lof = MOCA_LOF_BAND_E_DEF;
   }
   else if ((flags & MOCA_BAND_F_FLAG)) {
      *lof = MOCA_LOF_BAND_F_DEF;
   }
   else if ((flags & MOCA_BAND_C4_FLAG)) {
      *lof = MOCA_LOF_BAND_C4_DEF;
   }
   else if ((flags & MOCA_BAND_H_FLAG)) {
      *lof = MOCA_LOF_BAND_H_DEF;
   }
   else {
      *lof = MOCA_LOF_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_nbas_primary_defaults(uint32_t *max_nbas_primary, uint32_t flags)
{
   *max_nbas_primary = MOCA_MAX_NBAS_PRIMARY_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_tx1_defaults(uint32_t *ps_swch_tx1, uint32_t flags)
{
   *ps_swch_tx1 = MOCA_PS_SWCH_TX1_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_tx2_defaults(uint32_t *ps_swch_tx2, uint32_t flags)
{
   *ps_swch_tx2 = MOCA_PS_SWCH_TX2_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_rx1_defaults(uint32_t *ps_swch_rx1, uint32_t flags)
{
   *ps_swch_rx1 = MOCA_PS_SWCH_RX1_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_rx2_defaults(uint32_t *ps_swch_rx2, uint32_t flags)
{
   *ps_swch_rx2 = MOCA_PS_SWCH_RX2_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_rx3_defaults(uint32_t *ps_swch_rx3, uint32_t flags)
{
   *ps_swch_rx3 = MOCA_PS_SWCH_RX3_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_bonding_defaults(uint32_t *bonding, uint32_t flags)
{
   if ((flags & MOCA_BONDING_SUPPORTED_FLAG)) {
      *bonding = MOCA_BONDING_BONDING_SUPPORTED_DEF;
   }
   else {
      *bonding = MOCA_BONDING_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_listening_freq_mask_defaults(uint32_t *listening_freq_mask, uint32_t flags)
{
   *listening_freq_mask = MOCA_LISTENING_FREQ_MASK_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_listening_duration_defaults(uint32_t *listening_duration, uint32_t flags)
{
   *listening_duration = MOCA_LISTENING_DURATION_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_limit_traffic_defaults(uint32_t *limit_traffic, uint32_t flags)
{
   *limit_traffic = MOCA_LIMIT_TRAFFIC_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_remote_man_defaults(uint32_t *remote_man, uint32_t flags)
{
   if ((flags & MOCA_STANDALONE_FLAG) &&
       (flags & MOCA_6802B0_FLAG)) {
      *remote_man = MOCA_REMOTE_MAN_STANDALONE_6802B0_DEF;
   }
   else if ((flags & MOCA_STANDALONE_FLAG) &&
            (flags & MOCA_6802C0_FLAG)) {
      *remote_man = MOCA_REMOTE_MAN_STANDALONE_6802C0_DEF;
   }
   else {
      *remote_man = MOCA_REMOTE_MAN_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_c4_moca20_en_defaults(uint32_t *c4_moca20_en, uint32_t flags)
{
   *c4_moca20_en = MOCA_C4_MOCA20_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_power_save_mechanism_dis_defaults(uint32_t *power_save_mechanism_dis, uint32_t flags)
{
   *power_save_mechanism_dis = MOCA_POWER_SAVE_MECHANISM_DIS_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_psm_config_defaults(uint32_t *psm_config, uint32_t flags)
{
   if ((flags & MOCA_7425_FLAG)) {
      *psm_config = MOCA_PSM_CONFIG_7425_DEF;
   }
   else if ((flags & MOCA_28NM_FLAG)) {
      *psm_config = MOCA_PSM_CONFIG_28NM_DEF;
   }
   else {
      *psm_config = MOCA_PSM_CONFIG_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_use_ext_data_mem_defaults(uint32_t *use_ext_data_mem, uint32_t flags)
{
   *use_ext_data_mem = MOCA_USE_EXT_DATA_MEM_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_aif_mode_defaults(uint32_t *aif_mode, uint32_t flags)
{
   *aif_mode = MOCA_AIF_MODE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_prof_pad_ctrl_deg_6802c0_bonding_defaults(uint32_t *prof_pad_ctrl_deg_6802c0_bonding, uint32_t flags)
{
   if ((flags & MOCA_BONDING_SUPPORTED_FLAG)) {
      *prof_pad_ctrl_deg_6802c0_bonding = MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_BONDING_SUPPORTED_DEF;
   }
   else {
      *prof_pad_ctrl_deg_6802c0_bonding = MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_prop_bonding_compatibility_mode_defaults(uint32_t *prop_bonding_compatibility_mode, uint32_t flags)
{
   if ((flags & MOCA_BONDING_SUPPORTED_FLAG)) {
      *prop_bonding_compatibility_mode = MOCA_PROP_BONDING_COMPATIBILITY_MODE_BONDING_SUPPORTED_DEF;
   }
   else {
      *prop_bonding_compatibility_mode = MOCA_PROP_BONDING_COMPATIBILITY_MODE_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rdeg_3450_defaults(uint32_t *rdeg_3450, uint32_t flags)
{
   *rdeg_3450 = MOCA_RDEG_3450_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_phy_clock_defaults(uint32_t *phy_clock, uint32_t flags)
{
   if ((flags & MOCA_3390B0_FLAG)) {
      *phy_clock = MOCA_PHY_CLOCK_3390B0_DEF;
   }
   else {
      *phy_clock = MOCA_PHY_CLOCK_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_nbas_secondary_defaults(uint32_t *max_nbas_secondary, uint32_t flags)
{
   *max_nbas_secondary = MOCA_MAX_NBAS_SECONDARY_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mac_addr_defaults(struct moca_mac_addr * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

}

MOCALIB_GEN_SET_FUNCTION void moca_set_beacon_channel_set_defaults(uint32_t *beacon_channel_set, uint32_t flags)
{
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_tx_power_tune_defaults(struct moca_max_tx_power_tune * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 86; i++) {
      in->offset[i] = MOCA_MAX_TX_POWER_TUNE_OFFSET_DEF;
   }
   if (flags & MOCA_7425B0_FLAG) {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else if (flags & MOCA_7445D0_FLAG) {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  2 ;
      }
   }
   else if (flags & MOCA_28NM_FLAG) {
      for (i = 20; i <= 24; i++) {
         in->offset[i] =  6 ;
      }
      in->offset[25] =  5 ;
      for (i = 28; i <= 33; i++) {
         in->offset[i] =  4 ;
      }
      for (i = 39; i <= 41; i++) {
         in->offset[i] =  0 ;
      }
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else if (flags & MOCA_7428B0_FLAG) {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
      for (i = 20; i <= 25; i++) {
         in->offset[i] =  2 ;
      }
      for (i = 28; i <= 33; i++) {
         in->offset[i] =  1 ;
      }
   }
   else if (flags & MOCA_7435B0_FLAG) {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else if (flags & MOCA_74371B0_FLAG) {
      for (i = 46; i <= 48; i++) {
         in->offset[i] =  3 ;
      }
      for (i = 49; i <= 55; i++) {
         in->offset[i] =  4 ;
      }
      for (i = 56; i <= 57; i++) {
         in->offset[i] =  5 ;
      }
      for (i = 58; i <= 60; i++) {
         in->offset[i] =  4 ;
      }
      for (i = 61; i <= 63; i++) {
         in->offset[i] =  3 ;
      }
      for (i = 64; i <= 65; i++) {
         in->offset[i] =  2 ;
      }
   }
   else if (flags & MOCA_6802C0_FLAG) {
      for (i = 20; i <= 25; i++) {
         in->offset[i] =  3 ;
      }
      for (i = 28; i <= 33; i++) {
         in->offset[i] =  2 ;
      }
      for (i = 39; i <= 41; i++) {
         in->offset[i] =  1 ;
      }
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else if (flags & MOCA_6803C0_FLAG) {
      for (i = 20; i <= 25; i++) {
         in->offset[i] =  3 ;
      }
      for (i = 28; i <= 33; i++) {
         in->offset[i] =  2 ;
      }
      for (i = 39; i <= 41; i++) {
         in->offset[i] =  1 ;
      }
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  2;
      }
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_max_tx_power_tune_swap(struct moca_max_tx_power_tune * x)
{
   x->padding = BE16(x->padding);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_tx_power_tune_sec_ch_defaults(struct moca_max_tx_power_tune_sec_ch * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 86; i++) {
      in->offset[i] = MOCA_MAX_TX_POWER_TUNE_SEC_CH_OFFSET_DEF;
   }
   if (flags & MOCA_7425B0_FLAG) {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else if (flags & MOCA_7445D0_FLAG) {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  2 ;
      }
   }
   else if (flags & MOCA_28NM_FLAG) {
      for (i = 20; i <= 24; i++) {
         in->offset[i] =  6 ;
      }
      in->offset[25] =  5 ;
      for (i = 28; i <= 33; i++) {
         in->offset[i] =  4 ;
      }
      for (i = 39; i <= 41; i++) {
         in->offset[i] =  0 ;
      }
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else if (flags & MOCA_7428B0_FLAG) {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
      for (i = 20; i <= 25; i++) {
         in->offset[i] =  2 ;
      }
      for (i = 28; i <= 33; i++) {
         in->offset[i] =  1 ;
      }
   }
   else if (flags & MOCA_7435B0_FLAG) {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else if (flags & MOCA_74371B0_FLAG) {
      for (i = 46; i <= 48; i++) {
         in->offset[i] =  3 ;
      }
      for (i = 49; i <= 55; i++) {
         in->offset[i] =  4 ;
      }
      for (i = 56; i <= 57; i++) {
         in->offset[i] =  5 ;
      }
      for (i = 58; i <= 60; i++) {
         in->offset[i] =  4 ;
      }
      for (i = 61; i <= 63; i++) {
         in->offset[i] =  3 ;
      }
      for (i = 64; i <= 65; i++) {
         in->offset[i] =  2 ;
      }
   }
   else if (flags & MOCA_6803C0_FLAG) {
      for (i = 20; i <= 25; i++) {
         in->offset[i] =  3 ;
      }
      for (i = 28; i <= 33; i++) {
         in->offset[i] =  2 ;
      }
      for (i = 39; i <= 41; i++) {
         in->offset[i] =  1 ;
      }
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else if (flags & MOCA_6802C0_FLAG) {
      for (i = 20; i <= 25; i++) {
         in->offset[i] =  3 ;
      }
      for (i = 28; i <= 33; i++) {
         in->offset[i] =  2 ;
      }
      for (i = 39; i <= 41; i++) {
         in->offset[i] =  1 ;
      }
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  0 ;
      }
   }
   else {
      for (i = 46; i <= 65; i++) {
         in->offset[i] =  2;
      }
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_max_tx_power_tune_sec_ch_swap(struct moca_max_tx_power_tune_sec_ch * x)
{
   x->padding = BE16(x->padding);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rx_power_tune_defaults(struct moca_rx_power_tune * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 86; i++) {
      in->offset[i] = MOCA_RX_POWER_TUNE_OFFSET_DEF;
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_rx_power_tune_swap(struct moca_rx_power_tune * x)
{
   x->padding = BE16(x->padding);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_impedance_mode_bonding_defaults(uint32_t *impedance_mode_bonding, uint32_t flags)
{
   if ((flags & MOCA_BONDING_SUPPORTED_FLAG)) {
      *impedance_mode_bonding = MOCA_IMPEDANCE_MODE_BONDING_BONDING_SUPPORTED_DEF;
   }
   else {
      *impedance_mode_bonding = MOCA_IMPEDANCE_MODE_BONDING_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rework_6802_defaults(uint32_t *rework_6802, uint32_t flags)
{
   *rework_6802 = MOCA_REWORK_6802_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_prof_pad_ctrl_deg_6802c0_single_defaults(struct moca_prof_pad_ctrl_deg_6802c0_single * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 86; i++) {
      in->offset[i] = MOCA_PROF_PAD_CTRL_DEG_6802C0_SINGLE_OFFSET_DEF;
   }
   if (flags & MOCA_6802C0_FLAG) {
      for (i = 46; i <= 53; i++) {
         in->offset[i] =  11 ;
      }
      for (i = 54; i <= 65; i++) {
         in->offset[i] =  11 ;
      }
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_prof_pad_ctrl_deg_6802c0_single_swap(struct moca_prof_pad_ctrl_deg_6802c0_single * x)
{
   x->padding = BE16(x->padding);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_core_ready_swap(struct moca_core_ready * x)
{
   x->syncVersion = BE32(x->syncVersion);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_drv_info_swap(struct moca_drv_info * x)
{
   x->version = BE32(x->version);
   x->build_number = BE32(x->build_number);
   x->hw_rev = BE32(x->hw_rev);
   x->uptime = BE32(x->uptime);
   x->link_uptime = BE32(x->link_uptime);
   x->core_uptime = BE32(x->core_uptime);
   x->rf_band = BE32(x->rf_band);
   x->chip_id = BE32(x->chip_id);
   x->reset_count = BE32(x->reset_count);
   x->link_up_count = BE32(x->link_up_count);
   x->link_down_count = BE32(x->link_down_count);
   x->topology_change_count = BE32(x->topology_change_count);
   x->assert_count = BE32(x->assert_count);
   x->last_assert_num = BE32(x->last_assert_num);
   x->wdog_count = BE32(x->wdog_count);
   x->restart_history = BE32(x->restart_history);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_miscval_defaults(uint32_t *miscval, uint32_t flags)
{
}

MOCALIB_GEN_SET_FUNCTION void moca_set_en_capable_defaults(uint32_t *en_capable, uint32_t flags)
{
   *en_capable = MOCA_EN_CAPABLE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_lof_update_defaults(uint32_t *lof_update, uint32_t flags)
{
   *lof_update = MOCA_LOF_UPDATE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_primary_ch_offset_defaults(int32_t *primary_ch_offset, uint32_t flags)
{
   *primary_ch_offset = MOCA_PRIMARY_CH_OFFSET_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_assertText_defaults(uint32_t *assertText, uint32_t flags)
{
   *assertText = MOCA_ASSERTTEXT_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_wdog_enable_defaults(uint32_t *wdog_enable, uint32_t flags)
{
   *wdog_enable = MOCA_WDOG_ENABLE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_miscval2_defaults(uint32_t *miscval2, uint32_t flags)
{
}

MOCALIB_GEN_SET_FUNCTION void moca_set_secondary_ch_offset_defaults(int32_t *secondary_ch_offset, uint32_t flags)
{
   if ((flags & MOCA_BAND_GENERIC_FLAG)) {
      *secondary_ch_offset = MOCA_SECONDARY_CH_OFFSET_BAND_GENERIC_DEF;
   }
   else {
      *secondary_ch_offset = MOCA_SECONDARY_CH_OFFSET_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_amp_type_defaults(uint32_t *amp_type, uint32_t flags)
{
   *amp_type = MOCA_AMP_TYPE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_tpc_en_defaults(uint32_t *tpc_en, uint32_t flags)
{
   if ((flags & MOCA_BAND_E_FLAG)) {
      *tpc_en = MOCA_TPC_EN_BAND_E_DEF;
   }
   else if ((flags & MOCA_BAND_F_FLAG)) {
      *tpc_en = MOCA_TPC_EN_BAND_F_DEF;
   }
   else if ((flags & MOCA_BAND_H_FLAG)) {
      *tpc_en = MOCA_TPC_EN_BAND_H_DEF;
   }
   else {
      *tpc_en = MOCA_TPC_EN_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_tx_power_defaults(int32_t *max_tx_power, uint32_t flags)
{
   *max_tx_power = MOCA_MAX_TX_POWER_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_beacon_pwr_reduction_defaults(uint32_t *beacon_pwr_reduction, uint32_t flags)
{
   *beacon_pwr_reduction = MOCA_BEACON_PWR_REDUCTION_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_beacon_pwr_reduction_en_defaults(uint32_t *beacon_pwr_reduction_en, uint32_t flags)
{
   if ((flags & MOCA_6816_FLAG)) {
      *beacon_pwr_reduction_en = MOCA_BEACON_PWR_REDUCTION_EN_6816_DEF;
   }
   else if ((flags & MOCA_BAND_E_FLAG)) {
      *beacon_pwr_reduction_en = MOCA_BEACON_PWR_REDUCTION_EN_BAND_E_DEF;
   }
   else if ((flags & MOCA_BAND_F_FLAG)) {
      *beacon_pwr_reduction_en = MOCA_BEACON_PWR_REDUCTION_EN_BAND_F_DEF;
   }
   else if ((flags & MOCA_7xxx_FLAG)) {
      *beacon_pwr_reduction_en = MOCA_BEACON_PWR_REDUCTION_EN_7xxx_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_bo_mode_defaults(uint32_t *bo_mode, uint32_t flags)
{
   *bo_mode = MOCA_BO_MODE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_qam256_capability_defaults(uint32_t *qam256_capability, uint32_t flags)
{
   *qam256_capability = MOCA_QAM256_CAPABILITY_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_otf_en_defaults(uint32_t *otf_en, uint32_t flags)
{
   *otf_en = MOCA_OTF_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_star_topology_en_defaults(uint32_t *star_topology_en, uint32_t flags)
{
   *star_topology_en = MOCA_STAR_TOPOLOGY_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ofdma_en_defaults(uint32_t *ofdma_en, uint32_t flags)
{
   *ofdma_en = MOCA_OFDMA_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_min_bw_alarm_threshold_defaults(uint32_t *min_bw_alarm_threshold, uint32_t flags)
{
   *min_bw_alarm_threshold = MOCA_MIN_BW_ALARM_THRESHOLD_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_en_max_rate_in_max_bo_defaults(uint32_t *en_max_rate_in_max_bo, uint32_t flags)
{
   *en_max_rate_in_max_bo = MOCA_EN_MAX_RATE_IN_MAX_BO_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_target_phy_rate_qam128_defaults(uint32_t *target_phy_rate_qam128, uint32_t flags)
{
   *target_phy_rate_qam128 = MOCA_TARGET_PHY_RATE_QAM128_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_target_phy_rate_qam256_defaults(uint32_t *target_phy_rate_qam256, uint32_t flags)
{
   *target_phy_rate_qam256 = MOCA_TARGET_PHY_RATE_QAM256_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_sapm_en_defaults(uint32_t *sapm_en, uint32_t flags)
{
   *sapm_en = MOCA_SAPM_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_arpl_th_50_defaults(int32_t *arpl_th_50, uint32_t flags)
{
   *arpl_th_50 = MOCA_ARPL_TH_50_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rlapm_en_defaults(uint32_t *rlapm_en, uint32_t flags)
{
   if ((flags & MOCA_BAND_E_FLAG)) {
      *rlapm_en = MOCA_RLAPM_EN_BAND_E_DEF;
   }
   else if ((flags & MOCA_BAND_F_FLAG)) {
      *rlapm_en = MOCA_RLAPM_EN_BAND_F_DEF;
   }
   else {
      *rlapm_en = MOCA_RLAPM_EN_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_freq_shift_defaults(uint32_t *freq_shift, uint32_t flags)
{
   *freq_shift = MOCA_FREQ_SHIFT_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_phy_rate_defaults(uint32_t *max_phy_rate, uint32_t flags)
{
   if ((flags & MOCA_7425_FLAG)) {
      *max_phy_rate = MOCA_MAX_PHY_RATE_7425_DEF;
   }
   else {
      *max_phy_rate = MOCA_MAX_PHY_RATE_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_bandwidth_defaults(uint32_t *bandwidth, uint32_t flags)
{
   *bandwidth = MOCA_BANDWIDTH_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_arpl_th_100_defaults(int32_t *arpl_th_100, uint32_t flags)
{
   *arpl_th_100 = MOCA_ARPL_TH_100_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_adc_mode_defaults(uint32_t *adc_mode, uint32_t flags)
{
   *adc_mode = MOCA_ADC_MODE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_phy_rate_turbo_defaults(uint32_t *max_phy_rate_turbo, uint32_t flags)
{
   if ((flags & MOCA_7425_FLAG)) {
      *max_phy_rate_turbo = MOCA_MAX_PHY_RATE_TURBO_7425_DEF;
   }
   else {
      *max_phy_rate_turbo = MOCA_MAX_PHY_RATE_TURBO_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_cp_const_defaults(uint32_t *cp_const, uint32_t flags)
{
   *cp_const = MOCA_CP_CONST_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_preamble_uc_const_defaults(uint32_t *preamble_uc_const, uint32_t flags)
{
   *preamble_uc_const = MOCA_PREAMBLE_UC_CONST_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_cp_margin_increase_defaults(uint32_t *cp_margin_increase, uint32_t flags)
{
   *cp_margin_increase = MOCA_CP_MARGIN_INCREASE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ac_cc_shift_defaults(uint32_t *ac_cc_shift, uint32_t flags)
{
   *ac_cc_shift = MOCA_AC_CC_SHIFT_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mfc_th_increase_defaults(uint32_t *mfc_th_increase, uint32_t flags)
{
   *mfc_th_increase = MOCA_MFC_TH_INCREASE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_agc_const_en_defaults(uint32_t *agc_const_en, uint32_t flags)
{
   *agc_const_en = MOCA_AGC_CONST_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_agc_const_address_defaults(uint32_t *agc_const_address, uint32_t flags)
{
   *agc_const_address = MOCA_AGC_CONST_ADDRESS_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_min_bo_insert_2_bfm_lock_defaults(uint32_t *min_bo_insert_2_bfm_lock, uint32_t flags)
{
   *min_bo_insert_2_bfm_lock = MOCA_MIN_BO_INSERT_2_BFM_LOCK_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_min_snr_avg_db_2_bfm_lock_defaults(uint32_t *min_snr_avg_db_2_bfm_lock, uint32_t flags)
{
   *min_snr_avg_db_2_bfm_lock = MOCA_MIN_SNR_AVG_DB_2_BFM_LOCK_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_phy_rate_50M_defaults(uint32_t *max_phy_rate_50M, uint32_t flags)
{
   *max_phy_rate_50M = MOCA_MAX_PHY_RATE_50M_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_constellation_all_defaults(uint32_t *max_constellation_all, uint32_t flags)
{
   *max_constellation_all = MOCA_MAX_CONSTELLATION_ALL_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_constellation_defaults(struct moca_max_constellation * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

   in->p2p_limit_50 = MOCA_MAX_CONSTELLATION_P2P_LIMIT_50_DEF;
   in->gcd_limit_50 = MOCA_MAX_CONSTELLATION_GCD_LIMIT_50_DEF;
   in->p2p_limit_100 = MOCA_MAX_CONSTELLATION_P2P_LIMIT_100_DEF;
   in->gcd_limit_100 = MOCA_MAX_CONSTELLATION_GCD_LIMIT_100_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_rs_defaults(struct moca_snr_margin_table_rs * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 22; i++) {
      in->mgntable[i] = MOCA_SNR_MARGIN_TABLE_RS_MGNTABLE_DEF;
   }
      in->mgntable[0] =  (int) (256 * 9.5);
      in->mgntable[1] =  (int) (256 * 12.5);
      in->mgntable[2] =  (int) (256 * 15.5);
      in->mgntable[3] =  (int) (256 * 19);
      in->mgntable[4] =  (int) (256 * 22.5);
      in->mgntable[5] =  (int) (256 * 25.5);
      in->mgntable[6] =  (int) (256 * 29.5);
      in->mgntable[7] =  (int) (256 * 34);
      in->mgntable[8] =  (int) (256 * 37);
      in->mgntable[9] =  (int) (256 * 40);
      in->mgntable[10] =  0;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_rs_swap(struct moca_snr_margin_table_rs * x)
{
   int i;
   for (i = 0; i < 22; i++) {
      x->mgntable[i] = BE16(x->mgntable[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rlapm_table_50_defaults(struct moca_rlapm_table_50 * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 66; i++) {
      in->rlapmtable[i] = MOCA_RLAPM_TABLE_50_RLAPMTABLE_DEF;
   }
      in->rlapmtable[26] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[27] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[28] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[29] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[30] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[31] =  (unsigned char) (1.0  *2) ;
      in->rlapmtable[32] =  (unsigned char) (1.5  *2) ;
      in->rlapmtable[33] =  (unsigned char) (1.0  *2) ;
      in->rlapmtable[34] =  (unsigned char) (1.5  *2) ;
      in->rlapmtable[35] =  (unsigned char) (1.5  *2) ;
      in->rlapmtable[36] =  (unsigned char) (2.0  *2) ;
      in->rlapmtable[37] =  (unsigned char) (2.5  *2) ;
      in->rlapmtable[38] =  (unsigned char) (3.0  *2) ;
      in->rlapmtable[39] =  (unsigned char) (3.5  *2) ;
      in->rlapmtable[40] =  (unsigned char) (4.0  *2) ;
      in->rlapmtable[41] =  (unsigned char) (4.5  *2) ;
      in->rlapmtable[42] =  (unsigned char) (4.5 *2) ;
      in->rlapmtable[43] =  (unsigned char) (6.5 *2) ;
      in->rlapmtable[44] =  (unsigned char) (7.5 *2) ;
      in->rlapmtable[45] =  (unsigned char) (9.5 *2) ;
      in->rlapmtable[46] =  (unsigned char) (9.5 *2) ;
      in->rlapmtable[47] =  (unsigned char) (10.5 *2) ;
      in->rlapmtable[48] =  (unsigned char) (10.5 *2) ;
      in->rlapmtable[49] =  (unsigned char) (11.0 *2) ;
      in->rlapmtable[50] =  (unsigned char) (12.0 *2) ;
      in->rlapmtable[51] =  (unsigned char) (12.0 *2) ;
      in->rlapmtable[52] =  (unsigned char) (12.0 *2) ;
      in->rlapmtable[53] =  (unsigned char) (12.0 *2) ;
      in->rlapmtable[54] =  (unsigned char) (13.0 *2) ;
      in->rlapmtable[55] =  (unsigned char) (13.5 *2) ;
      in->rlapmtable[56] =  (unsigned char) (13.5 *2) ;
      in->rlapmtable[57] =  (unsigned char) (13.5 *2) ;
      in->rlapmtable[58] =  (unsigned char) (13.5 *2) ;
      in->rlapmtable[59] =  (unsigned char) (13.5 *2) ;
      in->rlapmtable[60] =  (unsigned char) (14.0 *2) ;
      in->rlapmtable[61] =  (unsigned char) (15.0 *2) ;
      for (i = 62; i <= 65; i++) {
         in->rlapmtable[i] =  (unsigned char) (16.0 *2);
      }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ldpc_defaults(struct moca_snr_margin_table_ldpc * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 22; i++) {
      in->mgntable[i] = MOCA_SNR_MARGIN_TABLE_LDPC_MGNTABLE_DEF;
   }
      in->mgntable[0] =  (int) (256 * (3.8   + 1.8 + 1));
      in->mgntable[1] =  (int) (256 * (6.8   + 1.8 + 1));
      in->mgntable[2] =  (int) (256 * (10.8  + 1.8 + 1));
      in->mgntable[3] =  (int) (256 * (13.3  + 1.8 + 1));
      in->mgntable[4] =  (int) (256 * (16.5  + 1.8 + 1));
      in->mgntable[5] =  (int) (256 * (19.1  + 1.8 + 1));
      in->mgntable[6] =  (int) (256 * (22    + 1.8 + 1));
      in->mgntable[7] =  (int) (256 * (24.6  + 1.8 + 1));
      in->mgntable[8] =  (int) (256 * (27.4  + 1.8 + 1 + 1));
      in->mgntable[9] =  (int) (256 * (30.2  + 1.8 + 1 + 2));
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ldpc_swap(struct moca_snr_margin_table_ldpc * x)
{
   int i;
   for (i = 0; i < 22; i++) {
      x->mgntable[i] = BE16(x->mgntable[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ldpc_sec_ch_defaults(struct moca_snr_margin_table_ldpc_sec_ch * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 22; i++) {
      in->mgntable[i] = MOCA_SNR_MARGIN_TABLE_LDPC_SEC_CH_MGNTABLE_DEF;
   }
      in->mgntable[0] =  (int) (256 * (3.8   + 1.8 + 1));
      in->mgntable[1] =  (int) (256 * (6.8   + 1.8 + 1));
      in->mgntable[2] =  (int) (256 * (10.8  + 1.8 + 1));
      in->mgntable[3] =  (int) (256 * (13.3  + 1.8 + 1));
      in->mgntable[4] =  (int) (256 * (16.5  + 1.8 + 1));
      in->mgntable[5] =  (int) (256 * (19.1  + 1.8 + 1));
      in->mgntable[6] =  (int) (256 * (22    + 1.8 + 1));
      in->mgntable[7] =  (int) (256 * (24.6  + 1.8 + 1));
      in->mgntable[8] =  (int) (256 * (27.4  + 1.8 + 1 + 1));
      in->mgntable[9] =  (int) (256 * (30.2  + 1.8 + 1 + 2));
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ldpc_sec_ch_swap(struct moca_snr_margin_table_ldpc_sec_ch * x)
{
   int i;
   for (i = 0; i < 22; i++) {
      x->mgntable[i] = BE16(x->mgntable[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ldpc_pre5_defaults(struct moca_snr_margin_table_ldpc_pre5 * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 22; i++) {
      in->mgntable[i] = MOCA_SNR_MARGIN_TABLE_LDPC_PRE5_MGNTABLE_DEF;
   }
      in->mgntable[0] =  (int) (256 * (3.8   + 1.8 + 2.2));
      in->mgntable[1] =  (int) (256 * (6.8   + 1.8 + 2.2));
      in->mgntable[2] =  (int) (256 * (10.8  + 1.8 + 2.2));
      in->mgntable[3] =  (int) (256 * (13.3  + 1.8 + 2.2));
      in->mgntable[4] =  (int) (256 * (16.5  + 1.8 + 2.2));
      in->mgntable[5] =  (int) (256 * (19.1  + 1.8 + 2.2));
      in->mgntable[6] =  (int) (256 * (22    + 1.8 + 2.2));
      in->mgntable[7] =  (int) (256 * (24.6  + 1.8 + 2.2));
      in->mgntable[8] =  (int) (256 * (27.4  + 1.8 + 2.2 + 1));
      in->mgntable[9] =  (int) (256 * (30.2  + 1.8 + 2.2 + 2));
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ldpc_pre5_swap(struct moca_snr_margin_table_ldpc_pre5 * x)
{
   int i;
   for (i = 0; i < 22; i++) {
      x->mgntable[i] = BE16(x->mgntable[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ofdma_defaults(struct moca_snr_margin_table_ofdma * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 22; i++) {
      in->mgntable[i] = MOCA_SNR_MARGIN_TABLE_OFDMA_MGNTABLE_DEF;
   }
      in->mgntable[0] =  (int) (256 * (4.2   + 1.8));
      in->mgntable[1] =  (int) (256 * (7     + 1.8));
      in->mgntable[2] =  (int) (256 * (11.8  + 1.8));
      in->mgntable[3] =  (int) (256 * (13.8  + 1.8));
      in->mgntable[4] =  (int) (256 * (16.8  + 1.8));
      in->mgntable[5] =  (int) (256 * (19.8  + 1.8));
      in->mgntable[6] =  (int) (256 * (22.6  + 1.8));
      in->mgntable[7] =  (int) (256 * (25.4  + 1.8));
      in->mgntable[8] =  (int) (256 * (28    + 1.8));
      in->mgntable[9] =  (int) (256 * (31    + 1.8));
      in->mgntable[10] =  0;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ofdma_swap(struct moca_snr_margin_table_ofdma * x)
{
   int i;
   for (i = 0; i < 22; i++) {
      x->mgntable[i] = BE16(x->mgntable[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rlapm_table_100_defaults(struct moca_rlapm_table_100 * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 66; i++) {
      in->rlapmtable[i] = MOCA_RLAPM_TABLE_100_RLAPMTABLE_DEF;
   }
      in->rlapmtable[21] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[22] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[23] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[24] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[25] =  (unsigned char) (0.5  *2) ;
      in->rlapmtable[26] =  (unsigned char) (1.0  *2) ;
      in->rlapmtable[27] =  (unsigned char) (1.0  *2) ;
      in->rlapmtable[28] =  (unsigned char) (1.0  *2) ;
      in->rlapmtable[29] =  (unsigned char) (1.5  *2) ;
      in->rlapmtable[30] =  (unsigned char) (1.5  *2) ;
      in->rlapmtable[31] =  (unsigned char) (2.0  *2) ;
      in->rlapmtable[32] =  (unsigned char) (2.5  *2) ;
      in->rlapmtable[33] =  (unsigned char) (3.0  *2) ;
      in->rlapmtable[34] =  (unsigned char) (3.5  *2) ;
      in->rlapmtable[35] =  (unsigned char) (4.0  *2) ;
      in->rlapmtable[36] =  (unsigned char) (4.5  *2) ;
      in->rlapmtable[37] =  (unsigned char) (5.5  *2) ;
      in->rlapmtable[38] =  (unsigned char) (6.5  *2) ;
      in->rlapmtable[39] =  (unsigned char) (7.5  *2) ;
      in->rlapmtable[40] =  (unsigned char) (8.5  *2) ;
      in->rlapmtable[41] =  (unsigned char) (9.5  *2) ;
      in->rlapmtable[42] =  (unsigned char) (10.5 *2) ;
      in->rlapmtable[43] =  (unsigned char) (10.5 *2) ;
      in->rlapmtable[44] =  (unsigned char) (11.5 *2) ;
      in->rlapmtable[45] =  (unsigned char) (12.5 *2) ;
      in->rlapmtable[46] =  (unsigned char) (13.5 *2) ;
      in->rlapmtable[47] =  (unsigned char) (14.5 *2) ;
      in->rlapmtable[48] =  (unsigned char) (14.5 *2) ;
      in->rlapmtable[49] =  (unsigned char) (15.0 *2) ;
      in->rlapmtable[50] =  (unsigned char) (15.0 *2) ;
      in->rlapmtable[51] =  (unsigned char) (15.0 *2) ;
      for (i = 52; i <= 65; i++) {
         in->rlapmtable[i] =  (unsigned char) (16.0 *2);
      }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_rf_calib_data_swap(struct moca_rf_calib_data * x)
{
   x->valid = BE32(x->valid);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_tx_power_params_swap(struct moca_tx_power_params * x)
{
   int i;
   x->channelMode = BE32(x->channelMode);
   x->channel = BE32(x->channel);
   x->user_reduce_power = BE32(x->user_reduce_power);
   x->channel_reduce_tune = BE32(x->channel_reduce_tune);
   x->tx_digital_gain = BE32(x->tx_digital_gain);
   x->pad_ctrl_deg = BE32(x->pad_ctrl_deg);
   x->pa_ctrl_reg = BE32(x->pa_ctrl_reg);
   x->is3451 = BE32(x->is3451);
   x->table_max_index = BE32(x->table_max_index);
   for (i = 0; i < 62; i++) {
      x->tx_table[i] = BE16(x->tx_table[i]);
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_tx_power_params_in_swap(struct moca_tx_power_params_in * x)
{
   x->channelMode = BE32(x->channelMode);
   x->txTableIndex = BE32(x->txTableIndex);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ldpc_pri_ch_defaults(struct moca_snr_margin_table_ldpc_pri_ch * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 22; i++) {
      in->mgntable[i] = MOCA_SNR_MARGIN_TABLE_LDPC_PRI_CH_MGNTABLE_DEF;
   }
      in->mgntable[0] =  (int) (256 * (3.8   + 1.8 + 1));
      in->mgntable[1] =  (int) (256 * (6.8   + 1.8 + 1));
      in->mgntable[2] =  (int) (256 * (10.8  + 1.8 + 1));
      in->mgntable[3] =  (int) (256 * (13.3  + 1.8 + 1));
      in->mgntable[4] =  (int) (256 * (16.5  + 1.8 + 1));
      in->mgntable[5] =  (int) (256 * (19.1  + 1.8 + 1));
      in->mgntable[6] =  (int) (256 * (22    + 1.8 + 1));
      in->mgntable[7] =  (int) (256 * (24.6  + 1.8 + 1));
      in->mgntable[8] =  (int) (256 * (27.4  + 1.8 + 1 + 1));
      in->mgntable[9] =  (int) (256 * (30.2  + 1.8 + 1 + 2));
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ldpc_pri_ch_swap(struct moca_snr_margin_table_ldpc_pri_ch * x)
{
   int i;
   for (i = 0; i < 22; i++) {
      x->mgntable[i] = BE16(x->mgntable[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_pre5_pri_ch_defaults(struct moca_snr_margin_table_pre5_pri_ch * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 22; i++) {
      in->mgntable[i] = MOCA_SNR_MARGIN_TABLE_PRE5_PRI_CH_MGNTABLE_DEF;
   }
      in->mgntable[0] =  (int) (256 * (3.8   + 1.8 + 2.2));
      in->mgntable[1] =  (int) (256 * (6.8   + 1.8 + 2.2));
      in->mgntable[2] =  (int) (256 * (10.8  + 1.8 + 2.2));
      in->mgntable[3] =  (int) (256 * (13.3  + 1.8 + 2.2));
      in->mgntable[4] =  (int) (256 * (16.5  + 1.8 + 2.2));
      in->mgntable[5] =  (int) (256 * (19.1  + 1.8 + 2.2));
      in->mgntable[6] =  (int) (256 * (22    + 1.8 + 2.2));
      in->mgntable[7] =  (int) (256 * (24.6  + 1.8 + 2.2));
      in->mgntable[8] =  (int) (256 * (27.4  + 1.8 + 2.2 + 1));
      in->mgntable[9] =  (int) (256 * (30.2  + 1.8 + 2.2 + 2));
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_pre5_pri_ch_swap(struct moca_snr_margin_table_pre5_pri_ch * x)
{
   int i;
   for (i = 0; i < 22; i++) {
      x->mgntable[i] = BE16(x->mgntable[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_pre5_sec_ch_defaults(struct moca_snr_margin_table_pre5_sec_ch * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 22; i++) {
      in->mgntable[i] = MOCA_SNR_MARGIN_TABLE_PRE5_SEC_CH_MGNTABLE_DEF;
   }
      in->mgntable[0] =  (int) (256 * (3.8   + 1.8 + 2.2));
      in->mgntable[1] =  (int) (256 * (6.8   + 1.8 + 2.2));
      in->mgntable[2] =  (int) (256 * (10.8  + 1.8 + 2.2));
      in->mgntable[3] =  (int) (256 * (13.3  + 1.8 + 2.2));
      in->mgntable[4] =  (int) (256 * (16.5  + 1.8 + 2.2));
      in->mgntable[5] =  (int) (256 * (19.1  + 1.8 + 2.2));
      in->mgntable[6] =  (int) (256 * (22    + 1.8 + 2.2));
      in->mgntable[7] =  (int) (256 * (24.6  + 1.8 + 2.2));
      in->mgntable[8] =  (int) (256 * (27.4  + 1.8 + 2.2 + 1));
      in->mgntable[9] =  (int) (256 * (30.2  + 1.8 + 2.2 + 2));
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_pre5_sec_ch_swap(struct moca_snr_margin_table_pre5_sec_ch * x)
{
   int i;
   for (i = 0; i < 22; i++) {
      x->mgntable[i] = BE16(x->mgntable[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_nv_cal_enable_defaults(uint32_t *nv_cal_enable, uint32_t flags)
{
   *nv_cal_enable = MOCA_NV_CAL_ENABLE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rlapm_cap_50_defaults(uint32_t *rlapm_cap_50, uint32_t flags)
{
   *rlapm_cap_50 = MOCA_RLAPM_CAP_50_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_rs_defaults(struct moca_snr_margin_rs * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   in->base_margin = MOCA_SNR_MARGIN_RS_BASE_MARGIN_DEF;
   for (i = 0; i < 10; i++) {
      in->offsets[i] = MOCA_SNR_MARGIN_RS_OFFSETS_DEF;
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_rs_swap(struct moca_snr_margin_rs * x)
{
   int i;
   x->base_margin = BE32(x->base_margin);
   for (i = 0; i < 10; i++) {
      x->offsets[i] = BE16(x->offsets[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_ldpc_defaults(struct moca_snr_margin_ldpc * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   in->base_margin = MOCA_SNR_MARGIN_LDPC_BASE_MARGIN_DEF;
   for (i = 0; i < 10; i++) {
      in->offsets[i] = MOCA_SNR_MARGIN_LDPC_OFFSETS_DEF;
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_ldpc_swap(struct moca_snr_margin_ldpc * x)
{
   int i;
   x->base_margin = BE32(x->base_margin);
   for (i = 0; i < 10; i++) {
      x->offsets[i] = BE16(x->offsets[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_ldpc_sec_ch_defaults(struct moca_snr_margin_ldpc_sec_ch * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   in->base_margin = MOCA_SNR_MARGIN_LDPC_SEC_CH_BASE_MARGIN_DEF;
   for (i = 0; i < 10; i++) {
      in->offsets[i] = MOCA_SNR_MARGIN_LDPC_SEC_CH_OFFSETS_DEF;
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_ldpc_sec_ch_swap(struct moca_snr_margin_ldpc_sec_ch * x)
{
   int i;
   x->base_margin = BE32(x->base_margin);
   for (i = 0; i < 10; i++) {
      x->offsets[i] = BE16(x->offsets[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_ldpc_pre5_defaults(struct moca_snr_margin_ldpc_pre5 * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   in->base_margin = MOCA_SNR_MARGIN_LDPC_PRE5_BASE_MARGIN_DEF;
   for (i = 0; i < 10; i++) {
      in->offsets[i] = MOCA_SNR_MARGIN_LDPC_PRE5_OFFSETS_DEF;
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_ldpc_pre5_swap(struct moca_snr_margin_ldpc_pre5 * x)
{
   int i;
   x->base_margin = BE32(x->base_margin);
   for (i = 0; i < 10; i++) {
      x->offsets[i] = BE16(x->offsets[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_ofdma_defaults(struct moca_snr_margin_ofdma * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   in->base_margin = MOCA_SNR_MARGIN_OFDMA_BASE_MARGIN_DEF;
   for (i = 0; i < 10; i++) {
      in->offsets[i] = MOCA_SNR_MARGIN_OFDMA_OFFSETS_DEF;
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_ofdma_swap(struct moca_snr_margin_ofdma * x)
{
   int i;
   x->base_margin = BE32(x->base_margin);
   for (i = 0; i < 10; i++) {
      x->offsets[i] = BE16(x->offsets[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rlapm_cap_100_defaults(uint32_t *rlapm_cap_100, uint32_t flags)
{
   *rlapm_cap_100 = MOCA_RLAPM_CAP_100_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_sapm_table_50_defaults(struct moca_sapm_table_50 * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 256; i++) {
      in->val[i] = MOCA_SAPM_TABLE_50_VAL_DEF;
   }
      in->val[41] =  4 *2 ;
      in->val[42] =  4 *2 ;
      in->val[43] =  4 *2 ;
      in->val[44] =  4 *2 ;
      in->val[45] =  4 *2 ;
      in->val[46] =  4 *2 ;
      in->val[47] =  4 *2 ;
      in->val[48] =  4 *2 ;
      in->val[49] =  7 *2 ;
      in->val[50] =  7 *2 ;
      in->val[51] =  7 *2 ;
      in->val[52] =  7 *2 ;
      in->val[53] =  7 *2 ;
      in->val[54] =  7 *2 ;
      in->val[55] =  7 *2 ;
      in->val[56] =  7 *2 ;
      in->val[57] =  60 *2 ;
      in->val[58] =  60 *2 ;
      in->val[59] =  60 *2 ;
      in->val[60] =  60 *2 ;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_sapm_table_100_defaults(struct moca_sapm_table_100 * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 512; i++) {
      in->val[i] = MOCA_SAPM_TABLE_100_VAL_DEF;
   }
      in->val[41] =  4 *2 ;
      in->val[42] =  4 *2 ;
      in->val[43] =  4 *2 ;
      in->val[44] =  4 *2 ;
      in->val[45] =  4 *2 ;
      in->val[46] =  4 *2 ;
      in->val[47] =  4 *2 ;
      in->val[48] =  4 *2 ;
      in->val[49] =  7 *2 ;
      in->val[50] =  7 *2 ;
      in->val[51] =  7 *2 ;
      in->val[52] =  7 *2 ;
      in->val[53] =  7 *2 ;
      in->val[54] =  7 *2 ;
      in->val[55] =  7 *2 ;
      in->val[56] =  7 *2 ;
      in->val[57] =  60 *2 ;
      in->val[58] =  60 *2 ;
      in->val[59] =  60 *2 ;
      in->val[60] =  60 *2 ;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_sapm_table_sec_defaults(struct moca_sapm_table_sec * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 512; i++) {
      in->val[i] = MOCA_SAPM_TABLE_SEC_VAL_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_ldpc_pri_ch_defaults(struct moca_snr_margin_ldpc_pri_ch * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   in->base_margin = MOCA_SNR_MARGIN_LDPC_PRI_CH_BASE_MARGIN_DEF;
   for (i = 0; i < 10; i++) {
      in->offsets[i] = MOCA_SNR_MARGIN_LDPC_PRI_CH_OFFSETS_DEF;
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_ldpc_pri_ch_swap(struct moca_snr_margin_ldpc_pri_ch * x)
{
   int i;
   x->base_margin = BE32(x->base_margin);
   for (i = 0; i < 10; i++) {
      x->offsets[i] = BE16(x->offsets[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_pre5_pri_ch_defaults(struct moca_snr_margin_pre5_pri_ch * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   in->base_margin = MOCA_SNR_MARGIN_PRE5_PRI_CH_BASE_MARGIN_DEF;
   for (i = 0; i < 10; i++) {
      in->offsets[i] = MOCA_SNR_MARGIN_PRE5_PRI_CH_OFFSETS_DEF;
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_pre5_pri_ch_swap(struct moca_snr_margin_pre5_pri_ch * x)
{
   int i;
   x->base_margin = BE32(x->base_margin);
   for (i = 0; i < 10; i++) {
      x->offsets[i] = BE16(x->offsets[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_pre5_sec_ch_defaults(struct moca_snr_margin_pre5_sec_ch * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   in->base_margin = MOCA_SNR_MARGIN_PRE5_SEC_CH_BASE_MARGIN_DEF;
   for (i = 0; i < 10; i++) {
      in->offsets[i] = MOCA_SNR_MARGIN_PRE5_SEC_CH_OFFSETS_DEF;
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_pre5_sec_ch_swap(struct moca_snr_margin_pre5_sec_ch * x)
{
   int i;
   x->base_margin = BE32(x->base_margin);
   for (i = 0; i < 10; i++) {
      x->offsets[i] = BE16(x->offsets[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_frame_size_defaults(uint32_t *max_frame_size, uint32_t flags)
{
   *max_frame_size = MOCA_MAX_FRAME_SIZE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_min_aggr_waiting_time_defaults(uint32_t *min_aggr_waiting_time, uint32_t flags)
{
   *min_aggr_waiting_time = MOCA_MIN_AGGR_WAITING_TIME_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_selective_rr_defaults(uint32_t *selective_rr, uint32_t flags)
{
   *selective_rr = MOCA_SELECTIVE_RR_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_fragmentation_defaults(uint32_t *fragmentation, uint32_t flags)
{
   *fragmentation = MOCA_FRAGMENTATION_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ifg_moca20_defaults(uint32_t *ifg_moca20, uint32_t flags)
{
   *ifg_moca20 = MOCA_IFG_MOCA20_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_map_seed_defaults(uint32_t *map_seed, uint32_t flags)
{
   *map_seed = MOCA_MAP_SEED_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_transmit_time_defaults(uint32_t *max_transmit_time, uint32_t flags)
{
   *max_transmit_time = MOCA_MAX_TRANSMIT_TIME_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_pkt_aggr_defaults(uint32_t *max_pkt_aggr, uint32_t flags)
{
   *max_pkt_aggr = MOCA_MAX_PKT_AGGR_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_probe_request_defaults(struct moca_probe_request * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

   in->probe_type = MOCA_PROBE_REQUEST_PROBE_TYPE_DEF;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_probe_request_swap(struct moca_probe_request * x)
{
   x->probe_type = BE16(x->probe_type);
   x->timeslots = BE16(x->timeslots);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rtr_config_defaults(struct moca_rtr_config * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

   in->low = MOCA_RTR_CONFIG_LOW_DEF;
   in->med = MOCA_RTR_CONFIG_MED_DEF;
   in->high = MOCA_RTR_CONFIG_HIGH_DEF;
   in->bg = MOCA_RTR_CONFIG_BG_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_tlp_mode_defaults(uint32_t *tlp_mode, uint32_t flags)
{
   *tlp_mode = MOCA_TLP_MODE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_max_pkt_aggr_bonding_defaults(uint32_t *max_pkt_aggr_bonding, uint32_t flags)
{
   *max_pkt_aggr_bonding = MOCA_MAX_PKT_AGGR_BONDING_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_multicast_mode_defaults(uint32_t *multicast_mode, uint32_t flags)
{
   *multicast_mode = MOCA_MULTICAST_MODE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_low_pri_q_num_defaults(uint32_t *low_pri_q_num, uint32_t flags)
{
   if ((flags & MOCA_6816_FLAG)) {
      *low_pri_q_num = MOCA_LOW_PRI_Q_NUM_6816_DEF;
   }
   else {
      *low_pri_q_num = MOCA_LOW_PRI_Q_NUM_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_egr_mc_filter_en_defaults(uint32_t *egr_mc_filter_en, uint32_t flags)
{
   *egr_mc_filter_en = MOCA_EGR_MC_FILTER_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_fc_mode_defaults(uint32_t *fc_mode, uint32_t flags)
{
   if ((flags & MOCA_FC_CAPABLE_CHIP_FLAG)) {
      *fc_mode = MOCA_FC_MODE_FC_CAPABLE_CHIP_DEF;
   }
   else {
      *fc_mode = MOCA_FC_MODE_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_pqos_max_packet_size_defaults(uint32_t *pqos_max_packet_size, uint32_t flags)
{
   *pqos_max_packet_size = MOCA_PQOS_MAX_PACKET_SIZE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_per_mode_defaults(uint32_t *per_mode, uint32_t flags)
{
   if ((flags & MOCA_BAND_E_FLAG)) {
      *per_mode = MOCA_PER_MODE_BAND_E_DEF;
   }
   else if ((flags & MOCA_BAND_H_FLAG)) {
      *per_mode = MOCA_PER_MODE_BAND_H_DEF;
   }
   else {
      *per_mode = MOCA_PER_MODE_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_policing_en_defaults(uint32_t *policing_en, uint32_t flags)
{
   *policing_en = MOCA_POLICING_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_orr_en_defaults(uint32_t *orr_en, uint32_t flags)
{
   *orr_en = MOCA_ORR_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_brcmtag_enable_defaults(uint32_t *brcmtag_enable, uint32_t flags)
{
   *brcmtag_enable = MOCA_BRCMTAG_ENABLE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_priority_allocations_defaults(struct moca_priority_allocations * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

   if ((flags & MOCA_7408_FLAG)) {
      in->reservation_pqos = MOCA_PRIORITY_ALLOCATIONS_RESERVATION_PQOS_7408_DEF;
   }
   else {
      in->reservation_pqos = MOCA_PRIORITY_ALLOCATIONS_RESERVATION_PQOS_DEF;
   }
   if ((flags & MOCA_7408_FLAG)) {
      in->reservation_high = MOCA_PRIORITY_ALLOCATIONS_RESERVATION_HIGH_7408_DEF;
   }
   else {
      in->reservation_high = MOCA_PRIORITY_ALLOCATIONS_RESERVATION_HIGH_DEF;
   }
   if ((flags & MOCA_7408_FLAG)) {
      in->reservation_med = MOCA_PRIORITY_ALLOCATIONS_RESERVATION_MED_7408_DEF;
   }
   else {
      in->reservation_med = MOCA_PRIORITY_ALLOCATIONS_RESERVATION_MED_DEF;
   }
   if ((flags & MOCA_7408_FLAG)) {
      in->reservation_low = MOCA_PRIORITY_ALLOCATIONS_RESERVATION_LOW_7408_DEF;
   }
   else {
      in->reservation_low = MOCA_PRIORITY_ALLOCATIONS_RESERVATION_LOW_DEF;
   }
   in->limitation_pqos = MOCA_PRIORITY_ALLOCATIONS_LIMITATION_PQOS_DEF;
   in->limitation_high = MOCA_PRIORITY_ALLOCATIONS_LIMITATION_HIGH_DEF;
   in->limitation_med = MOCA_PRIORITY_ALLOCATIONS_LIMITATION_MED_DEF;
   in->limitation_low = MOCA_PRIORITY_ALLOCATIONS_LIMITATION_LOW_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_egr_mc_addr_filter_defaults(struct moca_egr_mc_addr_filter * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

}

MOCALIB_GEN_SWAP_FUNCTION void moca_egr_mc_addr_filter_swap(struct moca_egr_mc_addr_filter * x)
{
   x->entryid = BE32(x->entryid);
   x->valid = BE32(x->valid);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_egr_mc_addr_filter_set_int_swap(struct moca_egr_mc_addr_filter_set_int * x)
{
   x->entryid = BE32(x->entryid);
   x->valid = BE32(x->valid);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_egr_mc_addr_filter_set_swap(struct moca_egr_mc_addr_filter_set * x)
{
   x->entryid = BE32(x->entryid);
   x->valid = BE32(x->valid);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_request_int_swap(struct moca_pqos_create_request_int * x)
{
   x->vendorId = BE16(x->vendorId);
   x->wave0Nodemask = BE32(x->wave0Nodemask);
   x->tPacketSize = BE16(x->tPacketSize);
   x->flowTag = BE32(x->flowTag);
   x->tPeakDataRate = BE32(x->tPeakDataRate);
   x->tLeaseTime = BE32(x->tLeaseTime);
   x->proto_rule_vlan_dscp = BE16(x->proto_rule_vlan_dscp);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_request_swap(struct moca_pqos_create_request * x)
{
   x->wave0Nodemask = BE32(x->wave0Nodemask);
   x->tPacketSize = BE16(x->tPacketSize);
   x->flowTag = BE32(x->flowTag);
   x->tPeakDataRate = BE32(x->tPeakDataRate);
   x->tLeaseTime = BE32(x->tLeaseTime);
   x->proto_rule_vlan_dscp = BE16(x->proto_rule_vlan_dscp);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_ingr_add_flow_int_swap(struct moca_pqos_ingr_add_flow_int * x)
{
   x->flowtag = BE32(x->flowtag);
   x->qtag = BE32(x->qtag);
   x->tpeakdatarate = BE32(x->tpeakdatarate);
   x->tpacketsize = BE32(x->tpacketsize);
   x->tburstsize = BE32(x->tburstsize);
   x->tleasetime = BE32(x->tleasetime);
   x->egressnodeid = BE32(x->egressnodeid);
   x->flowvlanid = BE32(x->flowvlanid);
   x->committedstps = BE32(x->committedstps);
   x->committedtxps = BE32(x->committedtxps);
   x->dest_flow_id = BE32(x->dest_flow_id);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_ingr_add_flow_swap(struct moca_pqos_ingr_add_flow * x)
{
   x->flowtag = BE32(x->flowtag);
   x->qtag = BE32(x->qtag);
   x->tpeakdatarate = BE32(x->tpeakdatarate);
   x->tpacketsize = BE32(x->tpacketsize);
   x->tburstsize = BE32(x->tburstsize);
   x->tleasetime = BE32(x->tleasetime);
   x->egressnodeid = BE32(x->egressnodeid);
   x->flowvlanid = BE32(x->flowvlanid);
   x->committedstps = BE32(x->committedstps);
   x->committedtxps = BE32(x->committedtxps);
   x->dest_flow_id = BE32(x->dest_flow_id);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_request_int_swap(struct moca_pqos_update_request_int * x)
{
   x->vendorId = BE16(x->vendorId);
   x->wave0Nodemask = BE32(x->wave0Nodemask);
   x->tPacketSize = BE16(x->tPacketSize);
   x->flowTag = BE32(x->flowTag);
   x->tPeakDataRate = BE32(x->tPeakDataRate);
   x->tLeaseTime = BE32(x->tLeaseTime);
   x->proto_rule_vlan_dscp = BE16(x->proto_rule_vlan_dscp);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_request_swap(struct moca_pqos_update_request * x)
{
   x->wave0Nodemask = BE32(x->wave0Nodemask);
   x->tPacketSize = BE16(x->tPacketSize);
   x->flowTag = BE32(x->flowTag);
   x->tPeakDataRate = BE32(x->tPeakDataRate);
   x->tLeaseTime = BE32(x->tLeaseTime);
   x->proto_rule_vlan_dscp = BE16(x->proto_rule_vlan_dscp);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_ingr_update_int_swap(struct moca_pqos_ingr_update_int * x)
{
   x->flowtag = BE32(x->flowtag);
   x->tpeakdatarate = BE32(x->tpeakdatarate);
   x->tpacketsize = BE32(x->tpacketsize);
   x->tburstsize = BE32(x->tburstsize);
   x->tleasetime = BE32(x->tleasetime);
   x->committedstps = BE32(x->committedstps);
   x->committedtxps = BE32(x->committedtxps);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_ingr_update_swap(struct moca_pqos_ingr_update * x)
{
   x->flowtag = BE32(x->flowtag);
   x->tpeakdatarate = BE32(x->tpeakdatarate);
   x->tpacketsize = BE32(x->tpacketsize);
   x->tburstsize = BE32(x->tburstsize);
   x->tleasetime = BE32(x->tleasetime);
   x->committedstps = BE32(x->committedstps);
   x->committedtxps = BE32(x->committedtxps);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_request_int_swap(struct moca_pqos_delete_request_int * x)
{
   x->vendorId = BE16(x->vendorId);
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_request_swap(struct moca_pqos_delete_request * x)
{
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_list_request_int_swap(struct moca_pqos_list_request_int * x)
{
   x->vendorId = BE16(x->vendorId);
   x->wave0Nodemask = BE32(x->wave0Nodemask);
   x->flowStartIndex = BE32(x->flowStartIndex);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_list_request_swap(struct moca_pqos_list_request * x)
{
   x->wave0Nodemask = BE32(x->wave0Nodemask);
   x->flowStartIndex = BE32(x->flowStartIndex);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_query_request_int_swap(struct moca_pqos_query_request_int * x)
{
   x->vendorId = BE16(x->vendorId);
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_query_request_swap(struct moca_pqos_query_request * x)
{
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_uc_fwd_swap(struct moca_uc_fwd * x)
{
   x->moca_dest_node_id = BE16(x->moca_dest_node_id);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mc_fwd_swap(struct moca_mc_fwd * x)
{
   x->dest_node_id = BE32(x->dest_node_id);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_src_addr_swap(struct moca_src_addr * x)
{
   x->moca_node_id = BE16(x->moca_node_id);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mac_aging_defaults(struct moca_mac_aging * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

   in->uc_fwd_age = MOCA_MAC_AGING_UC_FWD_AGE_DEF;
   in->mc_fwd_age = MOCA_MAC_AGING_MC_FWD_AGE_DEF;
   in->src_addr_age = MOCA_MAC_AGING_SRC_ADDR_AGE_DEF;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mac_aging_int_swap(struct moca_mac_aging_int * x)
{
   x->uc_fwd_age = BE16(x->uc_fwd_age);
   x->mc_fwd_age = BE16(x->mc_fwd_age);
   x->src_addr_age = BE16(x->src_addr_age);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mac_aging_swap(struct moca_mac_aging * x)
{
   x->uc_fwd_age = BE16(x->uc_fwd_age);
   x->mc_fwd_age = BE16(x->mc_fwd_age);
   x->src_addr_age = BE16(x->src_addr_age);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_loopback_en_defaults(uint32_t *loopback_en, uint32_t flags)
{
   *loopback_en = MOCA_LOOPBACK_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mcfilter_enable_defaults(uint32_t *mcfilter_enable, uint32_t flags)
{
   *mcfilter_enable = MOCA_MCFILTER_ENABLE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_pause_fc_en_defaults(uint32_t *pause_fc_en, uint32_t flags)
{
   if ((flags & MOCA_STANDALONE_FLAG)) {
      *pause_fc_en = MOCA_PAUSE_FC_EN_STANDALONE_DEF;
   }
   else {
      *pause_fc_en = MOCA_PAUSE_FC_EN_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_stag_priority_defaults(struct moca_stag_priority * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

   in->enable = MOCA_STAG_PRIORITY_ENABLE_DEF;
   in->tag_mask = MOCA_STAG_PRIORITY_TAG_MASK_DEF;
   in->moca_priority_0 = MOCA_STAG_PRIORITY_MOCA_PRIORITY_0_DEF;
   in->tag_priority_0 = MOCA_STAG_PRIORITY_TAG_PRIORITY_0_DEF;
   in->moca_priority_1 = MOCA_STAG_PRIORITY_MOCA_PRIORITY_1_DEF;
   in->tag_priority_1 = MOCA_STAG_PRIORITY_TAG_PRIORITY_1_DEF;
   in->moca_priority_2 = MOCA_STAG_PRIORITY_MOCA_PRIORITY_2_DEF;
   in->tag_priority_2 = MOCA_STAG_PRIORITY_TAG_PRIORITY_2_DEF;
   in->moca_priority_3 = MOCA_STAG_PRIORITY_MOCA_PRIORITY_3_DEF;
   in->tag_priority_3 = MOCA_STAG_PRIORITY_TAG_PRIORITY_3_DEF;
   in->moca_priority_4 = MOCA_STAG_PRIORITY_MOCA_PRIORITY_4_DEF;
   in->tag_priority_4 = MOCA_STAG_PRIORITY_TAG_PRIORITY_4_DEF;
   in->moca_priority_5 = MOCA_STAG_PRIORITY_MOCA_PRIORITY_5_DEF;
   in->tag_priority_5 = MOCA_STAG_PRIORITY_TAG_PRIORITY_5_DEF;
   in->moca_priority_6 = MOCA_STAG_PRIORITY_MOCA_PRIORITY_6_DEF;
   in->tag_priority_6 = MOCA_STAG_PRIORITY_TAG_PRIORITY_6_DEF;
   in->moca_priority_7 = MOCA_STAG_PRIORITY_MOCA_PRIORITY_7_DEF;
   in->tag_priority_7 = MOCA_STAG_PRIORITY_TAG_PRIORITY_7_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_stag_removal_defaults(struct moca_stag_removal * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

   in->enable = MOCA_STAG_REMOVAL_ENABLE_DEF;
   in->valid_0 = MOCA_STAG_REMOVAL_VALID_0_DEF;
   in->value_0 = MOCA_STAG_REMOVAL_VALUE_0_DEF;
   in->mask_0 = MOCA_STAG_REMOVAL_MASK_0_DEF;
   in->valid_1 = MOCA_STAG_REMOVAL_VALID_1_DEF;
   in->value_1 = MOCA_STAG_REMOVAL_VALUE_1_DEF;
   in->mask_1 = MOCA_STAG_REMOVAL_MASK_1_DEF;
   in->valid_2 = MOCA_STAG_REMOVAL_VALID_2_DEF;
   in->value_2 = MOCA_STAG_REMOVAL_VALUE_2_DEF;
   in->mask_2 = MOCA_STAG_REMOVAL_MASK_2_DEF;
   in->valid_3 = MOCA_STAG_REMOVAL_VALID_3_DEF;
   in->value_3 = MOCA_STAG_REMOVAL_VALUE_3_DEF;
   in->mask_3 = MOCA_STAG_REMOVAL_MASK_3_DEF;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_response_swap(struct moca_pqos_create_response * x)
{
   x->responsecode = BE32(x->responsecode);
   x->decision = BE32(x->decision);
   x->bw_limit_info = BE32(x->bw_limit_info);
   x->maxburstsize = BE32(x->maxburstsize);
   x->max_short_term_avg_ratio = BE32(x->max_short_term_avg_ratio);
   x->maxpeakdatarate = BE32(x->maxpeakdatarate);
   x->flowtag = BE32(x->flowtag);
   x->ingressnodeid = BE32(x->ingressnodeid);
   x->egressnodebitmask = BE32(x->egressnodebitmask);
   x->tpacketsize = BE32(x->tpacketsize);
   x->tleasetime = BE32(x->tleasetime);
   x->totalstps = BE32(x->totalstps);
   x->totaltxps = BE32(x->totaltxps);
   x->flowstps = BE32(x->flowstps);
   x->flowtxps = BE32(x->flowtxps);
   x->dest_flow_id = BE32(x->dest_flow_id);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_complete_swap(struct moca_pqos_create_complete * x)
{
   x->responsecode = BE32(x->responsecode);
   x->decision = BE32(x->decision);
   x->bw_limit_info = BE32(x->bw_limit_info);
   x->maxburstsize = BE32(x->maxburstsize);
   x->max_short_term_avg_ratio = BE32(x->max_short_term_avg_ratio);
   x->maxpeakdatarate = BE32(x->maxpeakdatarate);
   x->flowtag = BE32(x->flowtag);
   x->ingressnodeid = BE32(x->ingressnodeid);
   x->egressnodebitmask = BE32(x->egressnodebitmask);
   x->tpacketsize = BE32(x->tpacketsize);
   x->tleasetime = BE32(x->tleasetime);
   x->totalstps = BE32(x->totalstps);
   x->totaltxps = BE32(x->totaltxps);
   x->flowstps = BE32(x->flowstps);
   x->flowtxps = BE32(x->flowtxps);
   x->dest_flow_id = BE32(x->dest_flow_id);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_response_swap(struct moca_pqos_update_response * x)
{
   x->responsecode = BE32(x->responsecode);
   x->decision = BE32(x->decision);
   x->bw_limit_info = BE32(x->bw_limit_info);
   x->maxburstsize = BE32(x->maxburstsize);
   x->max_short_term_avg_ratio = BE32(x->max_short_term_avg_ratio);
   x->maxpeakdatarate = BE32(x->maxpeakdatarate);
   x->flowtag = BE32(x->flowtag);
   x->tpacketsize = BE32(x->tpacketsize);
   x->tleasetime = BE32(x->tleasetime);
   x->totalstps = BE32(x->totalstps);
   x->totaltxps = BE32(x->totaltxps);
   x->flowstps = BE32(x->flowstps);
   x->flowtxps = BE32(x->flowtxps);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_complete_swap(struct moca_pqos_update_complete * x)
{
   x->responsecode = BE32(x->responsecode);
   x->decision = BE32(x->decision);
   x->bw_limit_info = BE32(x->bw_limit_info);
   x->maxburstsize = BE32(x->maxburstsize);
   x->max_short_term_avg_ratio = BE32(x->max_short_term_avg_ratio);
   x->maxpeakdatarate = BE32(x->maxpeakdatarate);
   x->flowtag = BE32(x->flowtag);
   x->tpacketsize = BE32(x->tpacketsize);
   x->tleasetime = BE32(x->tleasetime);
   x->totalstps = BE32(x->totalstps);
   x->totaltxps = BE32(x->totaltxps);
   x->flowstps = BE32(x->flowstps);
   x->flowtxps = BE32(x->flowtxps);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_response_int_swap(struct moca_pqos_delete_response_int * x)
{
   x->responsecode = BE32(x->responsecode);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_response_swap(struct moca_pqos_delete_response * x)
{
   x->responsecode = BE32(x->responsecode);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_complete_int_swap(struct moca_pqos_delete_complete_int * x)
{
   x->responsecode = BE32(x->responsecode);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_complete_swap(struct moca_pqos_delete_complete * x)
{
   x->responsecode = BE32(x->responsecode);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_list_response_swap(struct moca_pqos_list_response * x)
{
   x->responsecode = BE32(x->responsecode);
   x->ingressnodeid = BE32(x->ingressnodeid);
   x->flow_update_count = BE32(x->flow_update_count);
   x->total_flow_id_count = BE32(x->total_flow_id_count);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_query_response_swap(struct moca_pqos_query_response * x)
{
   x->responsecode = BE32(x->responsecode);
   x->leasetimeleft = BE32(x->leasetimeleft);
   x->tpacketsize = BE16(x->tpacketsize);
   x->flowtag = BE32(x->flowtag);
   x->icr_vlan_dscp_iod_prot = BE16(x->icr_vlan_dscp_iod_prot);
   x->tpeakdatarate = BE32(x->tpeakdatarate);
   x->tleasetime = BE32(x->tleasetime);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_flow_out_swap(struct moca_pqos_create_flow_out * x)
{
   x->response_code = BE32(x->response_code);
   x->decision = BE32(x->decision);
   x->flow_tag = BE32(x->flow_tag);
   x->peak_data_rate = BE32(x->peak_data_rate);
   x->packet_size = BE32(x->packet_size);
   x->burst_size = BE32(x->burst_size);
   x->lease_time = BE32(x->lease_time);
   x->total_stps = BE32(x->total_stps);
   x->total_txps = BE32(x->total_txps);
   x->flow_stps = BE32(x->flow_stps);
   x->flow_txps = BE32(x->flow_txps);
   x->dest_flow_id = BE32(x->dest_flow_id);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
   x->max_short_term_avg_ratio = BE32(x->max_short_term_avg_ratio);
   x->bw_limit_info = BE32(x->bw_limit_info);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_flow_in_swap(struct moca_pqos_create_flow_in * x)
{
   x->packet_size = BE32(x->packet_size);
   x->flow_tag = BE32(x->flow_tag);
   x->peak_data_rate = BE32(x->peak_data_rate);
   x->lease_time = BE32(x->lease_time);
   x->burst_size = BE32(x->burst_size);
   x->vlan_id = BE32(x->vlan_id);
   x->max_latency = BE32(x->max_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_retry = BE32(x->max_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_flow_out_swap(struct moca_pqos_update_flow_out * x)
{
   x->response_code = BE32(x->response_code);
   x->decision = BE32(x->decision);
   x->flow_tag = BE32(x->flow_tag);
   x->peak_data_rate = BE32(x->peak_data_rate);
   x->packet_size = BE32(x->packet_size);
   x->burst_size = BE32(x->burst_size);
   x->lease_time = BE32(x->lease_time);
   x->total_stps = BE32(x->total_stps);
   x->total_txps = BE32(x->total_txps);
   x->flow_stps = BE32(x->flow_stps);
   x->flow_txps = BE32(x->flow_txps);
   x->maximum_latency = BE32(x->maximum_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_number_retry = BE32(x->max_number_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->traffic_protocol = BE32(x->traffic_protocol);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
   x->max_short_term_avg_ratio = BE32(x->max_short_term_avg_ratio);
   x->bw_limit_info = BE32(x->bw_limit_info);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_flow_in_swap(struct moca_pqos_update_flow_in * x)
{
   x->reserved = BE16(x->reserved);
   x->packet_size = BE32(x->packet_size);
   x->flow_tag = BE32(x->flow_tag);
   x->peak_data_rate = BE32(x->peak_data_rate);
   x->lease_time = BE32(x->lease_time);
   x->burst_size = BE32(x->burst_size);
   x->max_latency = BE32(x->max_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->max_retry = BE32(x->max_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->traffic_protocol = BE32(x->traffic_protocol);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_flow_out_int_swap(struct moca_pqos_delete_flow_out_int * x)
{
   x->response_code = BE32(x->response_code);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_flow_out_swap(struct moca_pqos_delete_flow_out * x)
{
   x->response_code = BE32(x->response_code);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_list_out_swap(struct moca_pqos_list_out * x)
{
   x->response_code = BE32(x->response_code);
   x->flow_update_count = BE32(x->flow_update_count);
   x->total_flow_id_count = BE32(x->total_flow_id_count);
   x->num_ret_flow_ids = BE32(x->num_ret_flow_ids);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_list_in_swap(struct moca_pqos_list_in * x)
{
   x->ingr_node_id = BE32(x->ingr_node_id);
   x->flow_max_return = BE16(x->flow_max_return);
   x->flow_start_index = BE32(x->flow_start_index);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_query_out_swap(struct moca_pqos_query_out * x)
{
   x->response_code = BE32(x->response_code);
   x->packet_size = BE32(x->packet_size);
   x->flow_tag = BE32(x->flow_tag);
   x->peak_data_rate = BE32(x->peak_data_rate);
   x->burst_size = BE32(x->burst_size);
   x->lease_time = BE32(x->lease_time);
   x->lease_time_left = BE32(x->lease_time_left);
   x->max_latency = BE32(x->max_latency);
   x->short_term_avg_ratio = BE32(x->short_term_avg_ratio);
   x->ingr_class_rule = BE32(x->ingr_class_rule);
   x->vlan_tag = BE32(x->vlan_tag);
   x->dscp_moca = BE32(x->dscp_moca);
   x->dest_flow_id = BE32(x->dest_flow_id);
   x->max_retry = BE32(x->max_retry);
   x->flow_per = BE32(x->flow_per);
   x->in_order_delivery = BE32(x->in_order_delivery);
   x->traffic_protocol = BE32(x->traffic_protocol);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_host_qos_defaults(uint32_t *host_qos, uint32_t flags)
{
   if ((flags & MOCA_FC_CAPABLE_CHIP_FLAG)) {
      *host_qos = MOCA_HOST_QOS_FC_CAPABLE_CHIP_DEF;
   }
   else {
      *host_qos = MOCA_HOST_QOS_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ooo_lmo_threshold_defaults(uint32_t *ooo_lmo_threshold, uint32_t flags)
{
   *ooo_lmo_threshold = MOCA_OOO_LMO_THRESHOLD_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_taboo_channels_defaults(struct moca_taboo_channels * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

   in->taboo_fixed_mask_start = MOCA_TABOO_CHANNELS_TABOO_FIXED_MASK_START_DEF;
   in->taboo_fixed_channel_mask = MOCA_TABOO_CHANNELS_TABOO_FIXED_CHANNEL_MASK_DEF;
   in->taboo_left_mask = MOCA_TABOO_CHANNELS_TABOO_LEFT_MASK_DEF;
   in->taboo_right_mask = MOCA_TABOO_CHANNELS_TABOO_RIGHT_MASK_DEF;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_fmr_request_int_swap(struct moca_fmr_request_int * x)
{
   x->vendorId = BE16(x->vendorId);
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_fmr_request_swap(struct moca_fmr_request * x)
{
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mr_request_int_swap(struct moca_mr_request_int * x)
{
   x->vendorId = BE16(x->vendorId);
   x->wave0Nodemask = BE32(x->wave0Nodemask);
   x->nonDefSeqNum = BE16(x->nonDefSeqNum);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mr_request_swap(struct moca_mr_request * x)
{
   x->wave0Nodemask = BE32(x->wave0Nodemask);
   x->nonDefSeqNum = BE16(x->nonDefSeqNum);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_gen_node_status_swap(struct moca_gen_node_status * x)
{
   x->freq_offset = BE32(x->freq_offset);
   x->node_tx_backoff = BE32(x->node_tx_backoff);
   x->protocol_support = BE32(x->protocol_support);
   x->active_moca_version = BE32(x->active_moca_version);
   x->max_ingress_pqos = BE32(x->max_ingress_pqos);
   x->max_egress_pqos = BE32(x->max_egress_pqos);
   x->ae_number = BE32(x->ae_number);
   x->max_aggr_pdus = BE32(x->max_aggr_pdus);
   x->max_aggr_kb = BE32(x->max_aggr_kb);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_node_stats_ext_swap(struct moca_node_stats_ext * x)
{
   x->rx_uc_crc_error = BE16(x->rx_uc_crc_error);
   x->rx_uc_crc_error_sec_ch = BE16(x->rx_uc_crc_error_sec_ch);
   x->rx_uc_timeout_error = BE16(x->rx_uc_timeout_error);
   x->rx_uc_timeout_error_sec_ch = BE16(x->rx_uc_timeout_error_sec_ch);
   x->rx_bc_crc_error = BE16(x->rx_bc_crc_error);
   x->rx_bc_timeout_error = BE16(x->rx_bc_timeout_error);
   x->rx_map_crc_error = BE16(x->rx_map_crc_error);
   x->rx_map_timeout_error = BE16(x->rx_map_timeout_error);
   x->rx_beacon_crc_error = BE16(x->rx_beacon_crc_error);
   x->rx_beacon_timeout_error = BE16(x->rx_beacon_timeout_error);
   x->rx_rr_crc_error = BE16(x->rx_rr_crc_error);
   x->rx_ofdma_rr_crc_error = BE16(x->rx_ofdma_rr_crc_error);
   x->rx_rr_timeout_error = BE16(x->rx_rr_timeout_error);
   x->rx_lc_uc_crc_error = BE16(x->rx_lc_uc_crc_error);
   x->rx_lc_bc_crc_error = BE16(x->rx_lc_bc_crc_error);
   x->rx_lc_uc_timeout_error = BE16(x->rx_lc_uc_timeout_error);
   x->rx_lc_bc_timeout_error = BE16(x->rx_lc_bc_timeout_error);
   x->rx_probe1_error = BE16(x->rx_probe1_error);
   x->rx_probe1_error_sec_ch = BE16(x->rx_probe1_error_sec_ch);
   x->rx_probe2_error = BE16(x->rx_probe2_error);
   x->rx_probe3_error = BE16(x->rx_probe3_error);
   x->rx_probe1_gcd_error = BE16(x->rx_probe1_gcd_error);
   x->rx_plp_crc_error = BE16(x->rx_plp_crc_error);
   x->rx_plp_timeout_error = BE16(x->rx_plp_timeout_error);
   x->rx_broken_packet_error = BE16(x->rx_broken_packet_error);
   x->rx_broken_packet_error_sec_ch = BE16(x->rx_broken_packet_error_sec_ch);
   x->rx_acf_crc_error = BE16(x->rx_acf_crc_error);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_node_stats_ext_in_swap(struct moca_node_stats_ext_in * x)
{
   x->index = BE32(x->index);
   x->reset_stats = BE32(x->reset_stats);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_ooo_lmo_defaults(uint32_t *ooo_lmo, uint32_t flags)
{
}

MOCALIB_GEN_SET_FUNCTION void moca_set_start_ulmo_defaults(struct moca_start_ulmo * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

}

MOCALIB_GEN_SWAP_FUNCTION void moca_dd_request_int_swap(struct moca_dd_request_int * x)
{
   x->vendorId = BE16(x->vendorId);
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_dd_request_swap(struct moca_dd_request * x)
{
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_fmr20_request_int_swap(struct moca_fmr20_request_int * x)
{
   x->vendorId = BE16(x->vendorId);
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_fmr20_request_swap(struct moca_fmr20_request * x)
{
   x->wave0Nodemask = BE32(x->wave0Nodemask);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_start_aca_swap(struct moca_start_aca * x)
{
   x->src_node = BE32(x->src_node);
   x->dest_nodemask = BE16(x->dest_nodemask);
   x->num_probes = BE16(x->num_probes);
   x->type = BE32(x->type);
   x->channel = BE32(x->channel);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_adm_stats_swap(struct moca_adm_stats * x)
{
   x->started = BE16(x->started);
   x->succeeded = BE16(x->succeeded);
   x->admission_failed = BE16(x->admission_failed);
   x->no_response = BE16(x->no_response);
   x->channel_unusable = BE16(x->channel_unusable);
   x->t2_timeout = BE16(x->t2_timeout);
   x->priv_full_blacklist = BE16(x->priv_full_blacklist);
   x->admission_failed_nc = BE16(x->admission_failed_nc);
   x->resync_loss = BE16(x->resync_loss);
   x->reserved = BE16(x->reserved);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_fmr_response_swap(struct moca_fmr_response * x)
{
   int i;
   x->responsecode = BE32(x->responsecode);
   x->responded_node_0 = BE32(x->responded_node_0);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_0[i] = BE16(x->fmrinfo_node_0[i]);
   }
   x->responded_node_1 = BE32(x->responded_node_1);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_1[i] = BE16(x->fmrinfo_node_1[i]);
   }
   x->responded_node_2 = BE32(x->responded_node_2);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_2[i] = BE16(x->fmrinfo_node_2[i]);
   }
   x->responded_node_3 = BE32(x->responded_node_3);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_3[i] = BE16(x->fmrinfo_node_3[i]);
   }
   x->responded_node_4 = BE32(x->responded_node_4);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_4[i] = BE16(x->fmrinfo_node_4[i]);
   }
   x->responded_node_5 = BE32(x->responded_node_5);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_5[i] = BE16(x->fmrinfo_node_5[i]);
   }
   x->responded_node_6 = BE32(x->responded_node_6);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_6[i] = BE16(x->fmrinfo_node_6[i]);
   }
   x->responded_node_7 = BE32(x->responded_node_7);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_7[i] = BE16(x->fmrinfo_node_7[i]);
   }
   x->responded_node_8 = BE32(x->responded_node_8);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_8[i] = BE16(x->fmrinfo_node_8[i]);
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mr_response_swap(struct moca_mr_response * x)
{
   x->ResponseCode = BE32(x->ResponseCode);
   x->ResetStatus = BE32(x->ResetStatus);
   x->NonDefSeqNum = BE32(x->NonDefSeqNum);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mr_complete_swap(struct moca_mr_complete * x)
{
   x->ResponseCode = BE32(x->ResponseCode);
   x->ResetStatus = BE32(x->ResetStatus);
   x->NonDefSeqNum = BE32(x->NonDefSeqNum);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_dd_response_swap(struct moca_dd_response * x)
{
   int i;
   x->responsecode = BE32(x->responsecode);
   for (i = 0; i < 81; i++) {
      x->data[i] = BE32(x->data[i]);
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_fmr20_response_swap(struct moca_fmr20_response * x)
{
   x->responsecode = BE32(x->responsecode);
   x->responded_node_0 = BE32(x->responded_node_0);
   x->responded_node_1 = BE32(x->responded_node_1);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_aca_result_1_swap(struct moca_aca_result_1 * x)
{
   x->aca_status = BE32(x->aca_status);
   x->aca_type = BE32(x->aca_type);
   x->tx_status = BE32(x->tx_status);
   x->rx_status = BE32(x->rx_status);
   x->total_power = BE32(x->total_power);
   x->relative_power = BE32(x->relative_power);
   x->num_elements = BE32(x->num_elements);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_aca_out_swap(struct moca_aca_out * x)
{
   x->aca_status = BE32(x->aca_status);
   x->aca_type = BE32(x->aca_type);
   x->tx_status = BE32(x->tx_status);
   x->rx_status = BE32(x->rx_status);
   x->total_power = BE32(x->total_power);
   x->relative_power = BE32(x->relative_power);
   x->num_elements = BE32(x->num_elements);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_aca_in_swap(struct moca_aca_in * x)
{
   x->src_node = BE32(x->src_node);
   x->dest_nodemask = BE32(x->dest_nodemask);
   x->type = BE32(x->type);
   x->channel = BE32(x->channel);
   x->num_probes = BE32(x->num_probes);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_fmr_init_out_swap(struct moca_fmr_init_out * x)
{
   int i;
   x->responsecode = BE32(x->responsecode);
   x->responded_node_0 = BE32(x->responded_node_0);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_0[i] = BE16(x->fmrinfo_node_0[i]);
   }
   x->responded_node_1 = BE32(x->responded_node_1);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_1[i] = BE16(x->fmrinfo_node_1[i]);
   }
   x->responded_node_2 = BE32(x->responded_node_2);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_2[i] = BE16(x->fmrinfo_node_2[i]);
   }
   x->responded_node_3 = BE32(x->responded_node_3);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_3[i] = BE16(x->fmrinfo_node_3[i]);
   }
   x->responded_node_4 = BE32(x->responded_node_4);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_4[i] = BE16(x->fmrinfo_node_4[i]);
   }
   x->responded_node_5 = BE32(x->responded_node_5);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_5[i] = BE16(x->fmrinfo_node_5[i]);
   }
   x->responded_node_6 = BE32(x->responded_node_6);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_6[i] = BE16(x->fmrinfo_node_6[i]);
   }
   x->responded_node_7 = BE32(x->responded_node_7);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_7[i] = BE16(x->fmrinfo_node_7[i]);
   }
   x->responded_node_8 = BE32(x->responded_node_8);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_8[i] = BE16(x->fmrinfo_node_8[i]);
   }
   x->responded_node_9 = BE32(x->responded_node_9);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_9[i] = BE16(x->fmrinfo_node_9[i]);
   }
   x->responded_node_10 = BE32(x->responded_node_10);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_10[i] = BE16(x->fmrinfo_node_10[i]);
   }
   x->responded_node_11 = BE32(x->responded_node_11);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_11[i] = BE16(x->fmrinfo_node_11[i]);
   }
   x->responded_node_12 = BE32(x->responded_node_12);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_12[i] = BE16(x->fmrinfo_node_12[i]);
   }
   x->responded_node_13 = BE32(x->responded_node_13);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_13[i] = BE16(x->fmrinfo_node_13[i]);
   }
   x->responded_node_14 = BE32(x->responded_node_14);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_14[i] = BE16(x->fmrinfo_node_14[i]);
   }
   x->responded_node_15 = BE32(x->responded_node_15);
   for (i = 0; i < 16; i++) {
      x->fmrinfo_node_15[i] = BE16(x->fmrinfo_node_15[i]);
   }
}

MOCALIB_GEN_SWAP_FUNCTION void moca_moca_reset_out_swap(struct moca_moca_reset_out * x)
{
   x->response_code = BE32(x->response_code);
   x->reset_status = BE32(x->reset_status);
   x->non_def_seq_num = BE32(x->non_def_seq_num);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_moca_reset_in_swap(struct moca_moca_reset_in * x)
{
   x->node_mask = BE32(x->node_mask);
   x->reset_timer = BE32(x->reset_timer);
   x->non_def_seq_num = BE32(x->non_def_seq_num);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_fmr_20_out_swap(struct moca_fmr_20_out * x)
{
   int i;
   x->responsecode = BE32(x->responsecode);
   for (i = 0; i < 16; i++) {
      x->node0_ofdmb_nper[i] = BE16(x->node0_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node0_ofdmb_vlper[i] = BE16(x->node0_ofdmb_vlper[i]);
   }
   x->node0_ofdmb_gcd = BE16(x->node0_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node0_ofdma_tab_node_bitmask[i] = BE32(x->node0_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node0_ofdma_tab_bps[i] = BE16(x->node0_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node1_ofdmb_nper[i] = BE16(x->node1_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node1_ofdmb_vlper[i] = BE16(x->node1_ofdmb_vlper[i]);
   }
   x->node1_ofdmb_gcd = BE16(x->node1_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node1_ofdma_tab_node_bitmask[i] = BE32(x->node1_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node1_ofdma_tab_bps[i] = BE16(x->node1_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node2_ofdmb_nper[i] = BE16(x->node2_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node2_ofdmb_vlper[i] = BE16(x->node2_ofdmb_vlper[i]);
   }
   x->node2_ofdmb_gcd = BE16(x->node2_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node2_ofdma_tab_node_bitmask[i] = BE32(x->node2_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node2_ofdma_tab_bps[i] = BE16(x->node2_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node3_ofdmb_nper[i] = BE16(x->node3_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node3_ofdmb_vlper[i] = BE16(x->node3_ofdmb_vlper[i]);
   }
   x->node3_ofdmb_gcd = BE16(x->node3_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node3_ofdma_tab_node_bitmask[i] = BE32(x->node3_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node3_ofdma_tab_bps[i] = BE16(x->node3_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node4_ofdmb_nper[i] = BE16(x->node4_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node4_ofdmb_vlper[i] = BE16(x->node4_ofdmb_vlper[i]);
   }
   x->node4_ofdmb_gcd = BE16(x->node4_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node4_ofdma_tab_node_bitmask[i] = BE32(x->node4_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node4_ofdma_tab_bps[i] = BE16(x->node4_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node5_ofdmb_nper[i] = BE16(x->node5_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node5_ofdmb_vlper[i] = BE16(x->node5_ofdmb_vlper[i]);
   }
   x->node5_ofdmb_gcd = BE16(x->node5_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node5_ofdma_tab_node_bitmask[i] = BE32(x->node5_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node5_ofdma_tab_bps[i] = BE16(x->node5_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node6_ofdmb_nper[i] = BE16(x->node6_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node6_ofdmb_vlper[i] = BE16(x->node6_ofdmb_vlper[i]);
   }
   x->node6_ofdmb_gcd = BE16(x->node6_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node6_ofdma_tab_node_bitmask[i] = BE32(x->node6_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node6_ofdma_tab_bps[i] = BE16(x->node6_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node7_ofdmb_nper[i] = BE16(x->node7_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node7_ofdmb_vlper[i] = BE16(x->node7_ofdmb_vlper[i]);
   }
   x->node7_ofdmb_gcd = BE16(x->node7_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node7_ofdma_tab_node_bitmask[i] = BE32(x->node7_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node7_ofdma_tab_bps[i] = BE16(x->node7_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node8_ofdmb_nper[i] = BE16(x->node8_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node8_ofdmb_vlper[i] = BE16(x->node8_ofdmb_vlper[i]);
   }
   x->node8_ofdmb_gcd = BE16(x->node8_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node8_ofdma_tab_node_bitmask[i] = BE32(x->node8_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node8_ofdma_tab_bps[i] = BE16(x->node8_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node9_ofdmb_nper[i] = BE16(x->node9_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node9_ofdmb_vlper[i] = BE16(x->node9_ofdmb_vlper[i]);
   }
   x->node9_ofdmb_gcd = BE16(x->node9_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node9_ofdma_tab_node_bitmask[i] = BE32(x->node9_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node9_ofdma_tab_bps[i] = BE16(x->node9_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node10_ofdmb_nper[i] = BE16(x->node10_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node10_ofdmb_vlper[i] = BE16(x->node10_ofdmb_vlper[i]);
   }
   x->node10_ofdmb_gcd = BE16(x->node10_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node10_ofdma_tab_node_bitmask[i] = BE32(x->node10_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node10_ofdma_tab_bps[i] = BE16(x->node10_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node11_ofdmb_nper[i] = BE16(x->node11_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node11_ofdmb_vlper[i] = BE16(x->node11_ofdmb_vlper[i]);
   }
   x->node11_ofdmb_gcd = BE16(x->node11_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node11_ofdma_tab_node_bitmask[i] = BE32(x->node11_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node11_ofdma_tab_bps[i] = BE16(x->node11_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node12_ofdmb_nper[i] = BE16(x->node12_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node12_ofdmb_vlper[i] = BE16(x->node12_ofdmb_vlper[i]);
   }
   x->node12_ofdmb_gcd = BE16(x->node12_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node12_ofdma_tab_node_bitmask[i] = BE32(x->node12_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node12_ofdma_tab_bps[i] = BE16(x->node12_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node13_ofdmb_nper[i] = BE16(x->node13_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node13_ofdmb_vlper[i] = BE16(x->node13_ofdmb_vlper[i]);
   }
   x->node13_ofdmb_gcd = BE16(x->node13_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node13_ofdma_tab_node_bitmask[i] = BE32(x->node13_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node13_ofdma_tab_bps[i] = BE16(x->node13_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node14_ofdmb_nper[i] = BE16(x->node14_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node14_ofdmb_vlper[i] = BE16(x->node14_ofdmb_vlper[i]);
   }
   x->node14_ofdmb_gcd = BE16(x->node14_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node14_ofdma_tab_node_bitmask[i] = BE32(x->node14_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node14_ofdma_tab_bps[i] = BE16(x->node14_ofdma_tab_bps[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node15_ofdmb_nper[i] = BE16(x->node15_ofdmb_nper[i]);
   }
   for (i = 0; i < 16; i++) {
      x->node15_ofdmb_vlper[i] = BE16(x->node15_ofdmb_vlper[i]);
   }
   x->node15_ofdmb_gcd = BE16(x->node15_ofdmb_gcd);
   for (i = 0; i < 4; i++) {
      x->node15_ofdma_tab_node_bitmask[i] = BE32(x->node15_ofdma_tab_node_bitmask[i]);
   }
   for (i = 0; i < 4; i++) {
      x->node15_ofdma_tab_bps[i] = BE16(x->node15_ofdma_tab_bps[i]);
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rf_band_defaults(uint32_t *rf_band, uint32_t flags)
{
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rf_switch_defaults(uint32_t *rf_switch, uint32_t flags)
{
   *rf_switch = MOCA_RF_SWITCH_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_if_access_en_defaults(uint32_t *if_access_en, uint32_t flags)
{
   *if_access_en = MOCA_IF_ACCESS_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_led_mode_defaults(uint32_t *led_mode, uint32_t flags)
{
   *led_mode = MOCA_LED_MODE_DEF;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_gen_stats_swap(struct moca_gen_stats * x)
{
   int i;
   x->ecl_tx_total_pkts = BE32(x->ecl_tx_total_pkts);
   x->ecl_tx_ucast_pkts = BE32(x->ecl_tx_ucast_pkts);
   x->ecl_tx_bcast_pkts = BE32(x->ecl_tx_bcast_pkts);
   x->ecl_tx_mcast_pkts = BE32(x->ecl_tx_mcast_pkts);
   x->ecl_tx_ucast_unknown = BE32(x->ecl_tx_ucast_unknown);
   x->ecl_tx_mcast_unknown = BE32(x->ecl_tx_mcast_unknown);
   x->ecl_tx_ucast_drops = BE32(x->ecl_tx_ucast_drops);
   x->ecl_tx_mcast_drops = BE32(x->ecl_tx_mcast_drops);
   x->ecl_tx_total_bytes = BE64(x->ecl_tx_total_bytes);
   x->ecl_tx_buff_drop_pkts = BE32(x->ecl_tx_buff_drop_pkts);
   x->ecl_tx_error_drop_pkts = BE32(x->ecl_tx_error_drop_pkts);
   x->ecl_rx_total_pkts = BE32(x->ecl_rx_total_pkts);
   x->ecl_rx_ucast_pkts = BE32(x->ecl_rx_ucast_pkts);
   x->ecl_rx_bcast_pkts = BE32(x->ecl_rx_bcast_pkts);
   x->ecl_rx_mcast_pkts = BE32(x->ecl_rx_mcast_pkts);
   x->ecl_rx_ucast_drops = BE32(x->ecl_rx_ucast_drops);
   x->ecl_rx_mcast_filter_pkts = BE32(x->ecl_rx_mcast_filter_pkts);
   x->ecl_rx_total_bytes = BE64(x->ecl_rx_total_bytes);
   x->ecl_fc_bg = BE32(x->ecl_fc_bg);
   x->ecl_fc_low = BE32(x->ecl_fc_low);
   x->ecl_fc_medium = BE32(x->ecl_fc_medium);
   x->ecl_fc_high = BE32(x->ecl_fc_high);
   x->ecl_fc_pqos = BE32(x->ecl_fc_pqos);
   x->ecl_fc_bp_all = BE32(x->ecl_fc_bp_all);
   x->mac_tx_low_drop_pkts = BE32(x->mac_tx_low_drop_pkts);
   x->mac_rx_buff_drop_pkts = BE32(x->mac_rx_buff_drop_pkts);
   x->mac_channel_usable_drop = BE32(x->mac_channel_usable_drop);
   x->mac_remove_node_drop = BE32(x->mac_remove_node_drop);
   x->mac_loopback_pkts = BE32(x->mac_loopback_pkts);
   x->mac_loopback_drop_pkts = BE32(x->mac_loopback_drop_pkts);
   x->aggr_pkt_stats_rx_max = BE32(x->aggr_pkt_stats_rx_max);
   x->aggr_pkt_stats_rx_count = BE32(x->aggr_pkt_stats_rx_count);
   for (i = 0; i < 30; i++) {
      x->aggr_pkt_stats_tx[i] = BE32(x->aggr_pkt_stats_tx[i]);
   }
   x->link_down_count = BE32(x->link_down_count);
   x->link_up_count = BE32(x->link_up_count);
   x->nc_handoff_counter = BE32(x->nc_handoff_counter);
   x->nc_backup_counter = BE32(x->nc_backup_counter);
   x->resync_attempts_to_network = BE32(x->resync_attempts_to_network);
   x->tx_beacons = BE32(x->tx_beacons);
   x->tx_map_packets = BE32(x->tx_map_packets);
   x->tx_rr_packets = BE32(x->tx_rr_packets);
   x->tx_ofdma_rr_packets = BE32(x->tx_ofdma_rr_packets);
   x->tx_control_uc_packets = BE32(x->tx_control_uc_packets);
   x->tx_control_bc_packets = BE32(x->tx_control_bc_packets);
   x->tx_protocol_ie = BE32(x->tx_protocol_ie);
   x->rx_beacons = BE32(x->rx_beacons);
   x->rx_map_packets = BE32(x->rx_map_packets);
   x->rx_rr_packets = BE32(x->rx_rr_packets);
   x->rx_ofdma_rr_packets = BE32(x->rx_ofdma_rr_packets);
   x->rx_control_uc_packets = BE32(x->rx_control_uc_packets);
   x->rx_control_bc_packets = BE32(x->rx_control_bc_packets);
   x->rx_protocol_ie = BE32(x->rx_protocol_ie);
   x->mac_frag_mpdu_tx = BE32(x->mac_frag_mpdu_tx);
   x->mac_frag_mpdu_rx = BE32(x->mac_frag_mpdu_rx);
   x->mac_pqos_policing_tx = BE32(x->mac_pqos_policing_tx);
   x->mac_pqos_policing_drop = BE32(x->mac_pqos_policing_drop);
   x->nc_became_nc_counter = BE32(x->nc_became_nc_counter);
   x->nc_became_backup_nc_counter = BE32(x->nc_became_backup_nc_counter);
   x->rx_buffer_full_counter = BE32(x->rx_buffer_full_counter);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_if_access_table_defaults(struct moca_if_access_table * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

}

MOCALIB_GEN_SET_FUNCTION void moca_set_m1_tx_power_variation_defaults(uint32_t *m1_tx_power_variation, uint32_t flags)
{
   *m1_tx_power_variation = MOCA_M1_TX_POWER_VARIATION_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_nc_listening_interval_defaults(uint32_t *nc_listening_interval, uint32_t flags)
{
   *nc_listening_interval = MOCA_NC_LISTENING_INTERVAL_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_nc_heartbeat_interval_defaults(uint32_t *nc_heartbeat_interval, uint32_t flags)
{
   *nc_heartbeat_interval = MOCA_NC_HEARTBEAT_INTERVAL_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_wom_mode_internal_defaults(uint32_t *wom_mode_internal, uint32_t flags)
{
   *wom_mode_internal = MOCA_WOM_MODE_INTERNAL_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_wom_magic_enable_defaults(uint32_t *wom_magic_enable, uint32_t flags)
{
   *wom_magic_enable = MOCA_WOM_MAGIC_ENABLE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_pm_restore_on_link_down_defaults(uint32_t *pm_restore_on_link_down, uint32_t flags)
{
   *pm_restore_on_link_down = MOCA_PM_RESTORE_ON_LINK_DOWN_DEF;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_wom_pattern_set_swap(struct moca_wom_pattern_set * x)
{
   x->index = BE32(x->index);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_standby_power_state_defaults(uint32_t *standby_power_state, uint32_t flags)
{
   *standby_power_state = MOCA_STANDBY_POWER_STATE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_wom_mode_defaults(uint32_t *wom_mode, uint32_t flags)
{
   if ((flags & MOCA_SWITCH_FLAG)) {
      *wom_mode = MOCA_WOM_MODE_SWITCH_DEF;
   }
   else {
      *wom_mode = MOCA_WOM_MODE_DEF;
   }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_privacy_en_defaults(uint32_t *privacy_en, uint32_t flags)
{
   *privacy_en = MOCA_PRIVACY_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_pmk_exchange_interval_defaults(uint32_t *pmk_exchange_interval, uint32_t flags)
{
   *pmk_exchange_interval = MOCA_PMK_EXCHANGE_INTERVAL_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_tek_exchange_interval_defaults(uint32_t *tek_exchange_interval, uint32_t flags)
{
   *tek_exchange_interval = MOCA_TEK_EXCHANGE_INTERVAL_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_aes_exchange_interval_defaults(uint32_t *aes_exchange_interval, uint32_t flags)
{
   *aes_exchange_interval = MOCA_AES_EXCHANGE_INTERVAL_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_aes_pm_key_defaults(struct moca_aes_pm_key * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

}

MOCALIB_GEN_SET_FUNCTION void moca_set_network_password_defaults(struct moca_network_password * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 20; i++) {
      in->password[i] = MOCA_NETWORK_PASSWORD_PASSWORD_DEF;
   }
      for (i = 0; i <= 8; i++) {
         in->password[i] =  0x39 ;
      }
      for (i = 9; i <= 16; i++) {
         in->password[i] =  0x38 ;
      }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_password_defaults(struct moca_password * in, uint32_t flags)
{
   int i;

   memset(in, 0, sizeof(*in));

   for (i = 0; i < 32; i++) {
      in->password[i] = MOCA_PASSWORD_PASSWORD_DEF;
   }
      for (i = 0; i <= 8; i++) {
         in->password[i] =  0x39 ;
      }
      for (i = 9; i <= 16; i++) {
         in->password[i] =  0x38 ;
      }
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mtm_en_defaults(uint32_t *mtm_en, uint32_t flags)
{
   *mtm_en = MOCA_MTM_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_const_rx_submode_defaults(uint32_t *const_rx_submode, uint32_t flags)
{
   *const_rx_submode = MOCA_CONST_RX_SUBMODE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_cir_prints_defaults(uint32_t *cir_prints, uint32_t flags)
{
   *cir_prints = MOCA_CIR_PRINTS_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_snr_prints_defaults(uint32_t *snr_prints, uint32_t flags)
{
   *snr_prints = MOCA_SNR_PRINTS_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_sigma2_prints_defaults(uint32_t *sigma2_prints, uint32_t flags)
{
   *sigma2_prints = MOCA_SIGMA2_PRINTS_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_bad_probe_prints_defaults(uint32_t *bad_probe_prints, uint32_t flags)
{
   *bad_probe_prints = MOCA_BAD_PROBE_PRINTS_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_const_tx_params_defaults(struct moca_const_tx_params * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

   in->const_tx_submode = MOCA_CONST_TX_PARAMS_CONST_TX_SUBMODE_DEF;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_gmii_trap_header_int_swap(struct moca_gmii_trap_header_int * x)
{
   x->eth_type = BE16(x->eth_type);
   x->ip_length = BE16(x->ip_length);
   x->id = BE16(x->id);
   x->flags_fragoffs = BE16(x->flags_fragoffs);
   x->ip_checksum = BE16(x->ip_checksum);
   x->src_port = BE16(x->src_port);
   x->dst_port = BE16(x->dst_port);
   x->udp_length = BE16(x->udp_length);
   x->udp_checksum = BE16(x->udp_checksum);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_gmii_trap_header_swap(struct moca_gmii_trap_header * x)
{
   x->id = BE16(x->id);
   x->ip_checksum = BE16(x->ip_checksum);
   x->src_port = BE16(x->src_port);
   x->dst_port = BE16(x->dst_port);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_moca_core_trace_enable_defaults(uint32_t *moca_core_trace_enable, uint32_t flags)
{
   *moca_core_trace_enable = MOCA_MOCA_CORE_TRACE_ENABLE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_dont_start_moca_defaults(uint32_t *dont_start_moca, uint32_t flags)
{
   *dont_start_moca = MOCA_DONT_START_MOCA_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_lab_mode_defaults(uint32_t *lab_mode, uint32_t flags)
{
   *lab_mode = MOCA_LAB_MODE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_nc_mode_defaults(uint32_t *nc_mode, uint32_t flags)
{
   *nc_mode = MOCA_NC_MODE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_rx_tx_packets_per_qm_defaults(uint32_t *rx_tx_packets_per_qm, uint32_t flags)
{
   *rx_tx_packets_per_qm = MOCA_RX_TX_PACKETS_PER_QM_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_extra_rx_packets_per_qm_defaults(uint32_t *extra_rx_packets_per_qm, uint32_t flags)
{
   *extra_rx_packets_per_qm = MOCA_EXTRA_RX_PACKETS_PER_QM_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_target_phy_rate_20_defaults(uint32_t *target_phy_rate_20, uint32_t flags)
{
   *target_phy_rate_20 = MOCA_TARGET_PHY_RATE_20_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_target_phy_rate_20_turbo_defaults(uint32_t *target_phy_rate_20_turbo, uint32_t flags)
{
   *target_phy_rate_20_turbo = MOCA_TARGET_PHY_RATE_20_TURBO_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_turbo_en_defaults(uint32_t *turbo_en, uint32_t flags)
{
   *turbo_en = MOCA_TURBO_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res1_defaults(uint32_t *res1, uint32_t flags)
{
   *res1 = MOCA_RES1_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res2_defaults(uint32_t *res2, uint32_t flags)
{
   *res2 = MOCA_RES2_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res3_defaults(uint32_t *res3, uint32_t flags)
{
   *res3 = MOCA_RES3_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res4_defaults(uint32_t *res4, uint32_t flags)
{
   *res4 = MOCA_RES4_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res5_defaults(uint32_t *res5, uint32_t flags)
{
   *res5 = MOCA_RES5_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res6_defaults(uint32_t *res6, uint32_t flags)
{
   *res6 = MOCA_RES6_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res7_defaults(uint32_t *res7, uint32_t flags)
{
   *res7 = MOCA_RES7_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res8_defaults(uint32_t *res8, uint32_t flags)
{
   *res8 = MOCA_RES8_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res9_defaults(uint32_t *res9, uint32_t flags)
{
   *res9 = MOCA_RES9_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_init1_defaults(uint32_t *init1, uint32_t flags)
{
   *init1 = MOCA_INIT1_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_init2_defaults(uint32_t *init2, uint32_t flags)
{
   *init2 = MOCA_INIT2_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_init3_defaults(uint32_t *init3, uint32_t flags)
{
   *init3 = MOCA_INIT3_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_init4_defaults(uint32_t *init4, uint32_t flags)
{
   *init4 = MOCA_INIT4_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_init5_defaults(uint32_t *init5, uint32_t flags)
{
   *init5 = MOCA_INIT5_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_init6_defaults(uint32_t *init6, uint32_t flags)
{
   *init6 = MOCA_INIT6_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_init7_defaults(uint32_t *init7, uint32_t flags)
{
   *init7 = MOCA_INIT7_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_init8_defaults(uint32_t *init8, uint32_t flags)
{
   *init8 = MOCA_INIT8_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_init9_defaults(uint32_t *init9, uint32_t flags)
{
   *init9 = MOCA_INIT9_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_lab_snr_graph_set_defaults(uint32_t *lab_snr_graph_set, uint32_t flags)
{
   *lab_snr_graph_set = MOCA_LAB_SNR_GRAPH_SET_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_block_nack_rate_defaults(uint32_t *block_nack_rate, uint32_t flags)
{
   *block_nack_rate = MOCA_BLOCK_NACK_RATE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res10_defaults(uint32_t *res10, uint32_t flags)
{
   *res10 = MOCA_RES10_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res11_defaults(uint32_t *res11, uint32_t flags)
{
   *res11 = MOCA_RES11_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res12_defaults(uint32_t *res12, uint32_t flags)
{
   *res12 = MOCA_RES12_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res13_defaults(uint32_t *res13, uint32_t flags)
{
   *res13 = MOCA_RES13_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res14_defaults(uint32_t *res14, uint32_t flags)
{
   *res14 = MOCA_RES14_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res15_defaults(uint32_t *res15, uint32_t flags)
{
   *res15 = MOCA_RES15_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res16_defaults(uint32_t *res16, uint32_t flags)
{
   *res16 = MOCA_RES16_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res17_defaults(uint32_t *res17, uint32_t flags)
{
   *res17 = MOCA_RES17_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res18_defaults(uint32_t *res18, uint32_t flags)
{
   *res18 = MOCA_RES18_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res19_defaults(uint32_t *res19, uint32_t flags)
{
   *res19 = MOCA_RES19_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_res20_defaults(uint32_t *res20, uint32_t flags)
{
   *res20 = MOCA_RES20_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_target_phy_rate_20_turbo_vlper_defaults(uint32_t *target_phy_rate_20_turbo_vlper, uint32_t flags)
{
   *target_phy_rate_20_turbo_vlper = MOCA_TARGET_PHY_RATE_20_TURBO_VLPER_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_target_phy_rate_20_sec_ch_defaults(uint32_t *target_phy_rate_20_sec_ch, uint32_t flags)
{
   *target_phy_rate_20_sec_ch = MOCA_TARGET_PHY_RATE_20_SEC_CH_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_target_phy_rate_20_turbo_sec_ch_defaults(uint32_t *target_phy_rate_20_turbo_sec_ch, uint32_t flags)
{
   *target_phy_rate_20_turbo_sec_ch = MOCA_TARGET_PHY_RATE_20_TURBO_SEC_CH_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_target_phy_rate_20_turbo_vlper_sec_ch_defaults(uint32_t *target_phy_rate_20_turbo_vlper_sec_ch, uint32_t flags)
{
   *target_phy_rate_20_turbo_vlper_sec_ch = MOCA_TARGET_PHY_RATE_20_TURBO_VLPER_SEC_CH_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_cap_phy_rate_en_defaults(uint32_t *cap_phy_rate_en, uint32_t flags)
{
   *cap_phy_rate_en = MOCA_CAP_PHY_RATE_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_cap_target_phy_rate_defaults(uint32_t *cap_target_phy_rate, uint32_t flags)
{
   *cap_target_phy_rate = MOCA_CAP_TARGET_PHY_RATE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_cap_snr_base_margin_defaults(uint32_t *cap_snr_base_margin, uint32_t flags)
{
   *cap_snr_base_margin = MOCA_CAP_SNR_BASE_MARGIN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_map_capture_defaults(uint32_t *map_capture, uint32_t flags)
{
   *map_capture = MOCA_MAP_CAPTURE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_lab_iq_diagram_set_defaults(struct moca_lab_iq_diagram_set * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

}

MOCALIB_GEN_SET_FUNCTION void moca_set_lab_tpcap_defaults(struct moca_lab_tpcap * in, uint32_t flags)
{
   memset(in, 0, sizeof(*in));

}

MOCALIB_GEN_SET_FUNCTION void moca_set_assert_restart_defaults(uint32_t *assert_restart, uint32_t flags)
{
   *assert_restart = MOCA_ASSERT_RESTART_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_moca_cpu_freq_defaults(uint32_t *moca_cpu_freq, uint32_t flags)
{
   *moca_cpu_freq = MOCA_MOCA_CPU_FREQ_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mps_en_defaults(uint32_t *mps_en, uint32_t flags)
{
   *mps_en = MOCA_MPS_EN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mps_privacy_receive_defaults(uint32_t *mps_privacy_receive, uint32_t flags)
{
   *mps_privacy_receive = MOCA_MPS_PRIVACY_RECEIVE_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mps_privacy_down_defaults(uint32_t *mps_privacy_down, uint32_t flags)
{
   *mps_privacy_down = MOCA_MPS_PRIVACY_DOWN_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mps_walk_time_defaults(uint32_t *mps_walk_time, uint32_t flags)
{
   *mps_walk_time = MOCA_MPS_WALK_TIME_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mps_unpaired_time_defaults(uint32_t *mps_unpaired_time, uint32_t flags)
{
   *mps_unpaired_time = MOCA_MPS_UNPAIRED_TIME_DEF;
}

MOCALIB_GEN_SET_FUNCTION void moca_set_mps_state_defaults(uint32_t *mps_state, uint32_t flags)
{
   *mps_state = MOCA_MPS_STATE_DEF;
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mps_init_scan_payload_swap(struct moca_mps_init_scan_payload * x)
{
   x->channel = BE32(x->channel);
   x->nc_moca_version = BE32(x->nc_moca_version);
   x->mps_code = BE32(x->mps_code);
   x->mps_parameters = BE32(x->mps_parameters);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mps_data_swap(struct moca_mps_data * x)
{
   x->valid = BE32(x->valid);
}

MOCALIB_GEN_SWAP_FUNCTION void moca_mps_request_mpskey_swap(struct moca_mps_request_mpskey * x)
{
   x->is_nn = BE32(x->is_nn);
}

MOCALIB_GEN_SET_FUNCTION void moca_set_privacy_defaults_defaults(uint32_t *privacy_defaults, uint32_t flags)
{
   *privacy_defaults = MOCA_PRIVACY_DEFAULTS_DEF;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_continuous_rx_mode_attn_check(int32_t val, uint32_t flags)
{
      if ((int32_t) BE32(val) < MOCA_CONTINUOUS_RX_MODE_ATTN_MIN)
         return MOCA_CONTINUOUS_RX_MODE_ATTN_ERR;
      if ((int32_t) BE32(val) > MOCA_CONTINUOUS_RX_MODE_ATTN_MAX)
         return MOCA_CONTINUOUS_RX_MODE_ATTN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_prim_ch_offs_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_PRIM_CH_OFFS_MAX)
         return MOCA_PRIM_CH_OFFS_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_sec_ch_offs_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_SEC_CH_OFFS_MAX)
         return MOCA_SEC_CH_OFFS_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_non_def_seq_num_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_NON_DEF_SEQ_NUM_MAX)
         return MOCA_NON_DEF_SEQ_NUM_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_bonding_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_BAND_D_LOW_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_BONDING_BAND_D_LOW_MAX)
            return MOCA_BONDING_ERR;
      }
      else if (flags & MOCA_BONDING_SUPPORTED_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_BONDING_BONDING_SUPPORTED_MAX)
            return MOCA_BONDING_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_BONDING_MAX)
         return MOCA_BONDING_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_listening_duration_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) < MOCA_LISTENING_DURATION_MIN)
         return MOCA_LISTENING_DURATION_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_limit_traffic_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_LIMIT_TRAFFIC_MAX)
         return MOCA_LIMIT_TRAFFIC_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_remote_man_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_REMOTE_MAN_MAX)
         return MOCA_REMOTE_MAN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_c4_moca20_en_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_C4_MOCA20_EN_MAX)
         return MOCA_C4_MOCA20_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_power_save_mechanism_dis_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_POWER_SAVE_MECHANISM_DIS_MAX)
         return MOCA_POWER_SAVE_MECHANISM_DIS_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_psm_config_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_7425_FLAG) {
         if ((uint32_t) BE32(val) < MOCA_PSM_CONFIG_7425_MIN)
            return MOCA_PSM_CONFIG_ERR;
      }
      else if (flags & MOCA_28NM_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_PSM_CONFIG_28NM_MAX)
            return MOCA_PSM_CONFIG_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_PSM_CONFIG_MAX)
         return MOCA_PSM_CONFIG_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_use_ext_data_mem_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_BONDING_SUPPORTED_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_USE_EXT_DATA_MEM_BONDING_SUPPORTED_MAX)
            return MOCA_USE_EXT_DATA_MEM_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_USE_EXT_DATA_MEM_MAX)
         return MOCA_USE_EXT_DATA_MEM_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_aif_mode_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_AIF_MODE_MAX)
         return MOCA_AIF_MODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_prof_pad_ctrl_deg_6802c0_bonding_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_BONDING_SUPPORTED_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_BONDING_SUPPORTED_MAX)
            return MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_MAX)
         return MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_prop_bonding_compatibility_mode_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_BONDING_SUPPORTED_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_PROP_BONDING_COMPATIBILITY_MODE_BONDING_SUPPORTED_MAX)
            return MOCA_PROP_BONDING_COMPATIBILITY_MODE_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_PROP_BONDING_COMPATIBILITY_MODE_MAX)
         return MOCA_PROP_BONDING_COMPATIBILITY_MODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_rdeg_3450_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_RDEG_3450_MAX)
         return MOCA_RDEG_3450_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_phy_clock_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_3390B0_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_PHY_CLOCK_3390B0_MAX)
            return MOCA_PHY_CLOCK_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_PHY_CLOCK_MAX)
         return MOCA_PHY_CLOCK_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_max_tx_power_tune_check(struct moca_max_tx_power_tune * in, uint32_t flags)
{
   int i;

   for (i = 0; i < 86; i++)
   {
      if ((int8_t) in->offset[i] < MOCA_MAX_TX_POWER_TUNE_OFFSET_MIN)
         return MOCA_MAX_TX_POWER_TUNE_OFFSET_ERR;
      if ((int8_t) in->offset[i] > MOCA_MAX_TX_POWER_TUNE_OFFSET_MAX)
         return MOCA_MAX_TX_POWER_TUNE_OFFSET_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_max_tx_power_tune_sec_ch_check(struct moca_max_tx_power_tune_sec_ch * in, uint32_t flags)
{
   int i;

   for (i = 0; i < 86; i++)
   {
      if ((int8_t) in->offset[i] < MOCA_MAX_TX_POWER_TUNE_SEC_CH_OFFSET_MIN)
         return MOCA_MAX_TX_POWER_TUNE_SEC_CH_OFFSET_ERR;
      if ((int8_t) in->offset[i] > MOCA_MAX_TX_POWER_TUNE_SEC_CH_OFFSET_MAX)
         return MOCA_MAX_TX_POWER_TUNE_SEC_CH_OFFSET_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_rx_power_tune_check(struct moca_rx_power_tune * in, uint32_t flags)
{
   int i;

   for (i = 0; i < 86; i++)
   {
      if ((int8_t) in->offset[i] < MOCA_RX_POWER_TUNE_OFFSET_MIN)
         return MOCA_RX_POWER_TUNE_OFFSET_ERR;
      if ((int8_t) in->offset[i] > MOCA_RX_POWER_TUNE_OFFSET_MAX)
         return MOCA_RX_POWER_TUNE_OFFSET_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_impedance_mode_bonding_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_BONDING_SUPPORTED_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_IMPEDANCE_MODE_BONDING_BONDING_SUPPORTED_MAX)
            return MOCA_IMPEDANCE_MODE_BONDING_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_IMPEDANCE_MODE_BONDING_MAX)
         return MOCA_IMPEDANCE_MODE_BONDING_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_rework_6802_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_BONDING_SUPPORTED_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_REWORK_6802_BONDING_SUPPORTED_MAX)
            return MOCA_REWORK_6802_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_REWORK_6802_MAX)
         return MOCA_REWORK_6802_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_prof_pad_ctrl_deg_6802c0_single_check(struct moca_prof_pad_ctrl_deg_6802c0_single * in, uint32_t flags)
{
   int i;

   for (i = 0; i < 86; i++)
   {
      if ((int8_t) in->offset[i] < MOCA_PROF_PAD_CTRL_DEG_6802C0_SINGLE_OFFSET_MIN)
         return MOCA_PROF_PAD_CTRL_DEG_6802C0_SINGLE_OFFSET_ERR;
      if ((int8_t) in->offset[i] > MOCA_PROF_PAD_CTRL_DEG_6802C0_SINGLE_OFFSET_MAX)
         return MOCA_PROF_PAD_CTRL_DEG_6802C0_SINGLE_OFFSET_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_lof_update_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_LOF_UPDATE_MAX)
         return MOCA_LOF_UPDATE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_primary_ch_offset_check(int32_t val, uint32_t flags)
{
      do {
         switch ((int32_t) BE32(val))
         {
            case -25:
            case 0:
            case 1:
            case 25:
               continue;
         }
         return MOCA_PRIMARY_CH_OFFSET_ERR;
      } while (0);

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_mr_seq_num_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_MR_SEQ_NUM_MAX)
         return MOCA_MR_SEQ_NUM_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_secondary_ch_offset_check(int32_t val, uint32_t flags)
{
      do {
         switch ((int32_t) BE32(val))
         {
            case -125:
            case 0:
            case 1:
            case 125:
               continue;
         }
         return MOCA_SECONDARY_CH_OFFSET_ERR;
      } while (0);

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_max_tx_power_check(int32_t val, uint32_t flags)
{
      if ((int32_t) BE32(val) < MOCA_MAX_TX_POWER_MIN)
         return MOCA_MAX_TX_POWER_ERR;
      if ((int32_t) BE32(val) > MOCA_MAX_TX_POWER_MAX)
         return MOCA_MAX_TX_POWER_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_beacon_pwr_reduction_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_BAND_F_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_BEACON_PWR_REDUCTION_BAND_F_MAX)
            return MOCA_BEACON_PWR_REDUCTION_ERR;
      }
      else if (flags & MOCA_BAND_E_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_BEACON_PWR_REDUCTION_BAND_E_MAX)
            return MOCA_BEACON_PWR_REDUCTION_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_BEACON_PWR_REDUCTION_MAX)
         return MOCA_BEACON_PWR_REDUCTION_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_beacon_pwr_reduction_en_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_BAND_F_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_BEACON_PWR_REDUCTION_EN_BAND_F_MAX)
            return MOCA_BEACON_PWR_REDUCTION_EN_ERR;
      }
      else if (flags & MOCA_BAND_E_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_BEACON_PWR_REDUCTION_EN_BAND_E_MAX)
            return MOCA_BEACON_PWR_REDUCTION_EN_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_BEACON_PWR_REDUCTION_EN_MAX)
         return MOCA_BEACON_PWR_REDUCTION_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_star_topology_en_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_STAR_TOPOLOGY_EN_MAX)
         return MOCA_STAR_TOPOLOGY_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_ofdma_en_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_OFDMA_EN_MAX)
         return MOCA_OFDMA_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_min_bw_alarm_threshold_check(uint32_t mbps, uint32_t flags)
{
      if ((uint32_t) BE32(mbps) < MOCA_MIN_BW_ALARM_THRESHOLD_MIN)
         return MOCA_MIN_BW_ALARM_THRESHOLD_ERR;
      if ((uint32_t) BE32(mbps) > MOCA_MIN_BW_ALARM_THRESHOLD_MAX)
         return MOCA_MIN_BW_ALARM_THRESHOLD_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_target_phy_rate_qam128_check(uint32_t mbps, uint32_t flags)
{
      if ((uint32_t) BE32(mbps) > MOCA_TARGET_PHY_RATE_QAM128_MAX)
         return MOCA_TARGET_PHY_RATE_QAM128_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_target_phy_rate_qam256_check(uint32_t mbps, uint32_t flags)
{
      if ((uint32_t) BE32(mbps) > MOCA_TARGET_PHY_RATE_QAM256_MAX)
         return MOCA_TARGET_PHY_RATE_QAM256_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_arpl_th_50_check(int32_t arpl, uint32_t flags)
{
      if ((int32_t) BE32(arpl) < MOCA_ARPL_TH_50_MIN)
         return MOCA_ARPL_TH_50_ERR;
      if ((int32_t) BE32(arpl) > MOCA_ARPL_TH_50_MAX)
         return MOCA_ARPL_TH_50_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_bandwidth_check(uint32_t bandwidth, uint32_t flags)
{
      if ((uint32_t) BE32(bandwidth) > MOCA_BANDWIDTH_MAX)
         return MOCA_BANDWIDTH_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_arpl_th_100_check(int32_t arpl, uint32_t flags)
{
      if ((int32_t) BE32(arpl) < MOCA_ARPL_TH_100_MIN)
         return MOCA_ARPL_TH_100_ERR;
      if ((int32_t) BE32(arpl) > MOCA_ARPL_TH_100_MAX)
         return MOCA_ARPL_TH_100_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_cp_const_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_CP_CONST_MAX)
         return MOCA_CP_CONST_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_preamble_uc_const_check(uint32_t val, uint32_t flags)
{
      do {
         switch ((uint32_t) BE32(val))
         {
            case 0:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 5:
               continue;
         }
         return MOCA_PREAMBLE_UC_CONST_ERR;
      } while (0);

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_agc_const_en_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_AGC_CONST_EN_MAX)
         return MOCA_AGC_CONST_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_max_constellation_all_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) < MOCA_MAX_CONSTELLATION_ALL_MIN)
         return MOCA_MAX_CONSTELLATION_ALL_ERR;
      if ((uint32_t) BE32(val) > MOCA_MAX_CONSTELLATION_ALL_MAX)
         return MOCA_MAX_CONSTELLATION_ALL_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_max_constellation_check(struct moca_max_constellation * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->node_id) > MOCA_MAX_CONSTELLATION_NODE_ID_MAX)
         return MOCA_MAX_CONSTELLATION_NODE_ID_ERR;
      if ((uint32_t) BE32(in->p2p_limit_50) < MOCA_MAX_CONSTELLATION_P2P_LIMIT_50_MIN)
         return MOCA_MAX_CONSTELLATION_P2P_LIMIT_50_ERR;
      if ((uint32_t) BE32(in->p2p_limit_50) > MOCA_MAX_CONSTELLATION_P2P_LIMIT_50_MAX)
         return MOCA_MAX_CONSTELLATION_P2P_LIMIT_50_ERR;
      if ((uint32_t) BE32(in->gcd_limit_50) < MOCA_MAX_CONSTELLATION_GCD_LIMIT_50_MIN)
         return MOCA_MAX_CONSTELLATION_GCD_LIMIT_50_ERR;
      if ((uint32_t) BE32(in->gcd_limit_50) > MOCA_MAX_CONSTELLATION_GCD_LIMIT_50_MAX)
         return MOCA_MAX_CONSTELLATION_GCD_LIMIT_50_ERR;
      if ((uint32_t) BE32(in->p2p_limit_100) < MOCA_MAX_CONSTELLATION_P2P_LIMIT_100_MIN)
         return MOCA_MAX_CONSTELLATION_P2P_LIMIT_100_ERR;
      if ((uint32_t) BE32(in->p2p_limit_100) > MOCA_MAX_CONSTELLATION_P2P_LIMIT_100_MAX)
         return MOCA_MAX_CONSTELLATION_P2P_LIMIT_100_ERR;
      if ((uint32_t) BE32(in->gcd_limit_100) < MOCA_MAX_CONSTELLATION_GCD_LIMIT_100_MIN)
         return MOCA_MAX_CONSTELLATION_GCD_LIMIT_100_ERR;
      if ((uint32_t) BE32(in->gcd_limit_100) > MOCA_MAX_CONSTELLATION_GCD_LIMIT_100_MAX)
         return MOCA_MAX_CONSTELLATION_GCD_LIMIT_100_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_rs_check(uint16_t * mgntable, uint32_t flags)
{
   int i;

   for (i = 0; i < 22; i++)
   {
      if ((uint16_t) BE16(mgntable[i]) > MOCA_SNR_MARGIN_TABLE_RS_MGNTABLE_MAX)
         return MOCA_SNR_MARGIN_TABLE_RS_MGNTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_sapm_table_fw_50_check(struct moca_sapm_table_fw_50 * in, uint32_t flags)
{
   int i;

   for (i = 0; i < 112; i++)
   {
      if ((uint8_t) in->sapmtablelo[i] > MOCA_SAPM_TABLE_FW_50_SAPMTABLELO_MAX)
         return MOCA_SAPM_TABLE_FW_50_SAPMTABLELO_ERR;
   }
   for (i = 0; i < 112; i++)
   {
      if ((uint8_t) in->sapmtablehi[i] > MOCA_SAPM_TABLE_FW_50_SAPMTABLEHI_MAX)
         return MOCA_SAPM_TABLE_FW_50_SAPMTABLEHI_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_rlapm_table_50_check(struct moca_rlapm_table_50 * in, uint32_t flags)
{
   int i;

   for (i = 0; i < 66; i++)
   {
      if ((uint8_t) in->rlapmtable[i] > MOCA_RLAPM_TABLE_50_RLAPMTABLE_MAX)
         return MOCA_RLAPM_TABLE_50_RLAPMTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ldpc_check(uint16_t * mgntable, uint32_t flags)
{
   int i;

   for (i = 0; i < 22; i++)
   {
      if ((uint16_t) BE16(mgntable[i]) > MOCA_SNR_MARGIN_TABLE_LDPC_MGNTABLE_MAX)
         return MOCA_SNR_MARGIN_TABLE_LDPC_MGNTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ldpc_sec_ch_check(uint16_t * mgntable, uint32_t flags)
{
   int i;

   for (i = 0; i < 22; i++)
   {
      if ((uint16_t) BE16(mgntable[i]) > MOCA_SNR_MARGIN_TABLE_LDPC_SEC_CH_MGNTABLE_MAX)
         return MOCA_SNR_MARGIN_TABLE_LDPC_SEC_CH_MGNTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ldpc_pre5_check(uint16_t * mgntable, uint32_t flags)
{
   int i;

   for (i = 0; i < 22; i++)
   {
      if ((uint16_t) BE16(mgntable[i]) > MOCA_SNR_MARGIN_TABLE_LDPC_PRE5_MGNTABLE_MAX)
         return MOCA_SNR_MARGIN_TABLE_LDPC_PRE5_MGNTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ofdma_check(uint16_t * mgntable, uint32_t flags)
{
   int i;

   for (i = 0; i < 22; i++)
   {
      if ((uint16_t) BE16(mgntable[i]) > MOCA_SNR_MARGIN_TABLE_OFDMA_MGNTABLE_MAX)
         return MOCA_SNR_MARGIN_TABLE_OFDMA_MGNTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_sapm_table_fw_100_check(struct moca_sapm_table_fw_100 * in, uint32_t flags)
{
   int i;

   for (i = 0; i < 240; i++)
   {
      if ((uint8_t) in->sapmtablelo[i] > MOCA_SAPM_TABLE_FW_100_SAPMTABLELO_MAX)
         return MOCA_SAPM_TABLE_FW_100_SAPMTABLELO_ERR;
   }
   for (i = 0; i < 240; i++)
   {
      if ((uint8_t) in->sapmtablehi[i] > MOCA_SAPM_TABLE_FW_100_SAPMTABLEHI_MAX)
         return MOCA_SAPM_TABLE_FW_100_SAPMTABLEHI_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_rlapm_table_100_check(struct moca_rlapm_table_100 * in, uint32_t flags)
{
   int i;

   for (i = 0; i < 66; i++)
   {
      if ((uint8_t) in->rlapmtable[i] > MOCA_RLAPM_TABLE_100_RLAPMTABLE_MAX)
         return MOCA_RLAPM_TABLE_100_RLAPMTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_cplength_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_CPLENGTH_MAX)
         return MOCA_CPLENGTH_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_sapm_table_fw_sec_check(struct moca_sapm_table_fw_sec * in, uint32_t flags)
{
   int i;

   for (i = 0; i < 240; i++)
   {
      if ((uint8_t) in->sapmtablelo[i] > MOCA_SAPM_TABLE_FW_SEC_SAPMTABLELO_MAX)
         return MOCA_SAPM_TABLE_FW_SEC_SAPMTABLELO_ERR;
   }
   for (i = 0; i < 240; i++)
   {
      if ((uint8_t) in->sapmtablehi[i] > MOCA_SAPM_TABLE_FW_SEC_SAPMTABLEHI_MAX)
         return MOCA_SAPM_TABLE_FW_SEC_SAPMTABLEHI_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ldpc_pri_ch_check(uint16_t * mgntable, uint32_t flags)
{
   int i;

   for (i = 0; i < 22; i++)
   {
      if ((uint16_t) BE16(mgntable[i]) > MOCA_SNR_MARGIN_TABLE_LDPC_PRI_CH_MGNTABLE_MAX)
         return MOCA_SNR_MARGIN_TABLE_LDPC_PRI_CH_MGNTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_pre5_pri_ch_check(uint16_t * mgntable, uint32_t flags)
{
   int i;

   for (i = 0; i < 22; i++)
   {
      if ((uint16_t) BE16(mgntable[i]) > MOCA_SNR_MARGIN_TABLE_PRE5_PRI_CH_MGNTABLE_MAX)
         return MOCA_SNR_MARGIN_TABLE_PRE5_PRI_CH_MGNTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_pre5_sec_ch_check(uint16_t * mgntable, uint32_t flags)
{
   int i;

   for (i = 0; i < 22; i++)
   {
      if ((uint16_t) BE16(mgntable[i]) > MOCA_SNR_MARGIN_TABLE_PRE5_SEC_CH_MGNTABLE_MAX)
         return MOCA_SNR_MARGIN_TABLE_PRE5_SEC_CH_MGNTABLE_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_nv_cal_enable_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_NV_CAL_ENABLE_MAX)
         return MOCA_NV_CAL_ENABLE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_snr_margin_rs_check(struct moca_snr_margin_rs * in, uint32_t flags)
{
   int i;

      if ((int32_t) BE32(in->base_margin) < MOCA_SNR_MARGIN_RS_BASE_MARGIN_MIN)
         return MOCA_SNR_MARGIN_RS_BASE_MARGIN_ERR;
      if ((int32_t) BE32(in->base_margin) > MOCA_SNR_MARGIN_RS_BASE_MARGIN_MAX)
         return MOCA_SNR_MARGIN_RS_BASE_MARGIN_ERR;
   for (i = 0; i < 10; i++)
   {
      if ((int16_t) BE16(in->offsets[i]) < MOCA_SNR_MARGIN_RS_OFFSETS_MIN)
         return MOCA_SNR_MARGIN_RS_OFFSETS_ERR;
      if ((int16_t) BE16(in->offsets[i]) > MOCA_SNR_MARGIN_RS_OFFSETS_MAX)
         return MOCA_SNR_MARGIN_RS_OFFSETS_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_snr_margin_ldpc_check(struct moca_snr_margin_ldpc * in, uint32_t flags)
{
   int i;

      if ((int32_t) BE32(in->base_margin) < MOCA_SNR_MARGIN_LDPC_BASE_MARGIN_MIN)
         return MOCA_SNR_MARGIN_LDPC_BASE_MARGIN_ERR;
      if ((int32_t) BE32(in->base_margin) > MOCA_SNR_MARGIN_LDPC_BASE_MARGIN_MAX)
         return MOCA_SNR_MARGIN_LDPC_BASE_MARGIN_ERR;
   for (i = 0; i < 10; i++)
   {
      if ((int16_t) BE16(in->offsets[i]) < MOCA_SNR_MARGIN_LDPC_OFFSETS_MIN)
         return MOCA_SNR_MARGIN_LDPC_OFFSETS_ERR;
      if ((int16_t) BE16(in->offsets[i]) > MOCA_SNR_MARGIN_LDPC_OFFSETS_MAX)
         return MOCA_SNR_MARGIN_LDPC_OFFSETS_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_snr_margin_ldpc_sec_ch_check(struct moca_snr_margin_ldpc_sec_ch * in, uint32_t flags)
{
   int i;

      if ((int32_t) BE32(in->base_margin) < MOCA_SNR_MARGIN_LDPC_SEC_CH_BASE_MARGIN_MIN)
         return MOCA_SNR_MARGIN_LDPC_SEC_CH_BASE_MARGIN_ERR;
      if ((int32_t) BE32(in->base_margin) > MOCA_SNR_MARGIN_LDPC_SEC_CH_BASE_MARGIN_MAX)
         return MOCA_SNR_MARGIN_LDPC_SEC_CH_BASE_MARGIN_ERR;
   for (i = 0; i < 10; i++)
   {
      if ((int16_t) BE16(in->offsets[i]) < MOCA_SNR_MARGIN_LDPC_SEC_CH_OFFSETS_MIN)
         return MOCA_SNR_MARGIN_LDPC_SEC_CH_OFFSETS_ERR;
      if ((int16_t) BE16(in->offsets[i]) > MOCA_SNR_MARGIN_LDPC_SEC_CH_OFFSETS_MAX)
         return MOCA_SNR_MARGIN_LDPC_SEC_CH_OFFSETS_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_snr_margin_ldpc_pre5_check(struct moca_snr_margin_ldpc_pre5 * in, uint32_t flags)
{
   int i;

      if ((int32_t) BE32(in->base_margin) < MOCA_SNR_MARGIN_LDPC_PRE5_BASE_MARGIN_MIN)
         return MOCA_SNR_MARGIN_LDPC_PRE5_BASE_MARGIN_ERR;
      if ((int32_t) BE32(in->base_margin) > MOCA_SNR_MARGIN_LDPC_PRE5_BASE_MARGIN_MAX)
         return MOCA_SNR_MARGIN_LDPC_PRE5_BASE_MARGIN_ERR;
   for (i = 0; i < 10; i++)
   {
      if ((int16_t) BE16(in->offsets[i]) < MOCA_SNR_MARGIN_LDPC_PRE5_OFFSETS_MIN)
         return MOCA_SNR_MARGIN_LDPC_PRE5_OFFSETS_ERR;
      if ((int16_t) BE16(in->offsets[i]) > MOCA_SNR_MARGIN_LDPC_PRE5_OFFSETS_MAX)
         return MOCA_SNR_MARGIN_LDPC_PRE5_OFFSETS_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_snr_margin_ofdma_check(struct moca_snr_margin_ofdma * in, uint32_t flags)
{
   int i;

      if ((int32_t) BE32(in->base_margin) < MOCA_SNR_MARGIN_OFDMA_BASE_MARGIN_MIN)
         return MOCA_SNR_MARGIN_OFDMA_BASE_MARGIN_ERR;
      if ((int32_t) BE32(in->base_margin) > MOCA_SNR_MARGIN_OFDMA_BASE_MARGIN_MAX)
         return MOCA_SNR_MARGIN_OFDMA_BASE_MARGIN_ERR;
   for (i = 0; i < 10; i++)
   {
      if ((int16_t) BE16(in->offsets[i]) < MOCA_SNR_MARGIN_OFDMA_OFFSETS_MIN)
         return MOCA_SNR_MARGIN_OFDMA_OFFSETS_ERR;
      if ((int16_t) BE16(in->offsets[i]) > MOCA_SNR_MARGIN_OFDMA_OFFSETS_MAX)
         return MOCA_SNR_MARGIN_OFDMA_OFFSETS_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_sapm_table_50_check(uint8_t * val, uint32_t flags)
{
   int i;

   for (i = 0; i < 256; i++)
   {
      if ((uint8_t) val[i] > MOCA_SAPM_TABLE_50_VAL_MAX)
         return MOCA_SAPM_TABLE_50_VAL_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_sapm_table_100_check(uint8_t * val, uint32_t flags)
{
   int i;

   for (i = 0; i < 512; i++)
   {
      if ((uint8_t) val[i] > MOCA_SAPM_TABLE_100_VAL_MAX)
         return MOCA_SAPM_TABLE_100_VAL_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_sapm_table_sec_check(uint8_t * val, uint32_t flags)
{
   int i;

   for (i = 0; i < 512; i++)
   {
      if ((uint8_t) val[i] > MOCA_SAPM_TABLE_SEC_VAL_MAX)
         return MOCA_SAPM_TABLE_SEC_VAL_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_snr_margin_ldpc_pri_ch_check(struct moca_snr_margin_ldpc_pri_ch * in, uint32_t flags)
{
   int i;

      if ((int32_t) BE32(in->base_margin) < MOCA_SNR_MARGIN_LDPC_PRI_CH_BASE_MARGIN_MIN)
         return MOCA_SNR_MARGIN_LDPC_PRI_CH_BASE_MARGIN_ERR;
      if ((int32_t) BE32(in->base_margin) > MOCA_SNR_MARGIN_LDPC_PRI_CH_BASE_MARGIN_MAX)
         return MOCA_SNR_MARGIN_LDPC_PRI_CH_BASE_MARGIN_ERR;
   for (i = 0; i < 10; i++)
   {
      if ((int16_t) BE16(in->offsets[i]) < MOCA_SNR_MARGIN_LDPC_PRI_CH_OFFSETS_MIN)
         return MOCA_SNR_MARGIN_LDPC_PRI_CH_OFFSETS_ERR;
      if ((int16_t) BE16(in->offsets[i]) > MOCA_SNR_MARGIN_LDPC_PRI_CH_OFFSETS_MAX)
         return MOCA_SNR_MARGIN_LDPC_PRI_CH_OFFSETS_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_snr_margin_pre5_pri_ch_check(struct moca_snr_margin_pre5_pri_ch * in, uint32_t flags)
{
   int i;

      if ((int32_t) BE32(in->base_margin) < MOCA_SNR_MARGIN_PRE5_PRI_CH_BASE_MARGIN_MIN)
         return MOCA_SNR_MARGIN_PRE5_PRI_CH_BASE_MARGIN_ERR;
      if ((int32_t) BE32(in->base_margin) > MOCA_SNR_MARGIN_PRE5_PRI_CH_BASE_MARGIN_MAX)
         return MOCA_SNR_MARGIN_PRE5_PRI_CH_BASE_MARGIN_ERR;
   for (i = 0; i < 10; i++)
   {
      if ((int16_t) BE16(in->offsets[i]) < MOCA_SNR_MARGIN_PRE5_PRI_CH_OFFSETS_MIN)
         return MOCA_SNR_MARGIN_PRE5_PRI_CH_OFFSETS_ERR;
      if ((int16_t) BE16(in->offsets[i]) > MOCA_SNR_MARGIN_PRE5_PRI_CH_OFFSETS_MAX)
         return MOCA_SNR_MARGIN_PRE5_PRI_CH_OFFSETS_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_snr_margin_pre5_sec_ch_check(struct moca_snr_margin_pre5_sec_ch * in, uint32_t flags)
{
   int i;

      if ((int32_t) BE32(in->base_margin) < MOCA_SNR_MARGIN_PRE5_SEC_CH_BASE_MARGIN_MIN)
         return MOCA_SNR_MARGIN_PRE5_SEC_CH_BASE_MARGIN_ERR;
      if ((int32_t) BE32(in->base_margin) > MOCA_SNR_MARGIN_PRE5_SEC_CH_BASE_MARGIN_MAX)
         return MOCA_SNR_MARGIN_PRE5_SEC_CH_BASE_MARGIN_ERR;
   for (i = 0; i < 10; i++)
   {
      if ((int16_t) BE16(in->offsets[i]) < MOCA_SNR_MARGIN_PRE5_SEC_CH_OFFSETS_MIN)
         return MOCA_SNR_MARGIN_PRE5_SEC_CH_OFFSETS_ERR;
      if ((int16_t) BE16(in->offsets[i]) > MOCA_SNR_MARGIN_PRE5_SEC_CH_OFFSETS_MAX)
         return MOCA_SNR_MARGIN_PRE5_SEC_CH_OFFSETS_ERR;
   }

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_max_frame_size_check(uint32_t bytes, uint32_t flags)
{
      if ((uint32_t) BE32(bytes) < MOCA_MAX_FRAME_SIZE_MIN)
         return MOCA_MAX_FRAME_SIZE_ERR;
      if ((uint32_t) BE32(bytes) > MOCA_MAX_FRAME_SIZE_MAX)
         return MOCA_MAX_FRAME_SIZE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_fragmentation_check(uint32_t bool_val, uint32_t flags)
{
      if ((uint32_t) BE32(bool_val) > MOCA_FRAGMENTATION_MAX)
         return MOCA_FRAGMENTATION_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_max_transmit_time_check(uint32_t usec, uint32_t flags)
{
      if ((uint32_t) BE32(usec) < MOCA_MAX_TRANSMIT_TIME_MIN)
         return MOCA_MAX_TRANSMIT_TIME_ERR;
      if ((uint32_t) BE32(usec) > MOCA_MAX_TRANSMIT_TIME_MAX)
         return MOCA_MAX_TRANSMIT_TIME_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_max_pkt_aggr_check(uint32_t pkts, uint32_t flags)
{
      if ((uint32_t) BE32(pkts) < MOCA_MAX_PKT_AGGR_MIN)
         return MOCA_MAX_PKT_AGGR_ERR;
      if ((uint32_t) BE32(pkts) > MOCA_MAX_PKT_AGGR_MAX)
         return MOCA_MAX_PKT_AGGR_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_probe_request_check(struct moca_probe_request * in, uint32_t flags)
{
      if ((uint16_t) BE16(in->probe_type) > MOCA_PROBE_REQUEST_PROBE_TYPE_MAX)
         return MOCA_PROBE_REQUEST_PROBE_TYPE_ERR;
      if ((uint16_t) BE16(in->timeslots) > MOCA_PROBE_REQUEST_TIMESLOTS_MAX)
         return MOCA_PROBE_REQUEST_TIMESLOTS_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_rtr_config_check(struct moca_rtr_config * in, uint32_t flags)
{
      if ((uint8_t) in->low > MOCA_RTR_CONFIG_LOW_MAX)
         return MOCA_RTR_CONFIG_LOW_ERR;
      if ((uint8_t) in->med > MOCA_RTR_CONFIG_MED_MAX)
         return MOCA_RTR_CONFIG_MED_ERR;
      if ((uint8_t) in->high > MOCA_RTR_CONFIG_HIGH_MAX)
         return MOCA_RTR_CONFIG_HIGH_ERR;
      if ((uint8_t) in->bg > MOCA_RTR_CONFIG_BG_MAX)
         return MOCA_RTR_CONFIG_BG_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_tlp_mode_check(uint32_t mode, uint32_t flags)
{
      if ((uint32_t) BE32(mode) < MOCA_TLP_MODE_MIN)
         return MOCA_TLP_MODE_ERR;
      if ((uint32_t) BE32(mode) > MOCA_TLP_MODE_MAX)
         return MOCA_TLP_MODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_max_pkt_aggr_bonding_check(uint32_t pkts, uint32_t flags)
{
      if ((uint32_t) BE32(pkts) < MOCA_MAX_PKT_AGGR_BONDING_MIN)
         return MOCA_MAX_PKT_AGGR_BONDING_ERR;
      if ((uint32_t) BE32(pkts) > MOCA_MAX_PKT_AGGR_BONDING_MAX)
         return MOCA_MAX_PKT_AGGR_BONDING_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_fc_mode_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_FC_MODE_MAX)
         return MOCA_FC_MODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_pqos_max_packet_size_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) < MOCA_PQOS_MAX_PACKET_SIZE_MIN)
         return MOCA_PQOS_MAX_PACKET_SIZE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_per_mode_check(uint32_t mode, uint32_t flags)
{
      if ((uint32_t) BE32(mode) > MOCA_PER_MODE_MAX)
         return MOCA_PER_MODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_policing_en_check(uint32_t enable, uint32_t flags)
{
      if ((uint32_t) BE32(enable) > MOCA_POLICING_EN_MAX)
         return MOCA_POLICING_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_orr_en_check(uint32_t enable, uint32_t flags)
{
      if ((uint32_t) BE32(enable) > MOCA_ORR_EN_MAX)
         return MOCA_ORR_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_brcmtag_enable_check(uint32_t enable, uint32_t flags)
{
      if ((uint32_t) BE32(enable) > MOCA_BRCMTAG_ENABLE_MAX)
         return MOCA_BRCMTAG_ENABLE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_priority_allocations_check(struct moca_priority_allocations * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->reservation_pqos) > MOCA_PRIORITY_ALLOCATIONS_RESERVATION_PQOS_MAX)
         return MOCA_PRIORITY_ALLOCATIONS_RESERVATION_PQOS_ERR;
      if ((uint32_t) BE32(in->reservation_high) > MOCA_PRIORITY_ALLOCATIONS_RESERVATION_HIGH_MAX)
         return MOCA_PRIORITY_ALLOCATIONS_RESERVATION_HIGH_ERR;
      if ((uint32_t) BE32(in->reservation_med) > MOCA_PRIORITY_ALLOCATIONS_RESERVATION_MED_MAX)
         return MOCA_PRIORITY_ALLOCATIONS_RESERVATION_MED_ERR;
      if ((uint32_t) BE32(in->reservation_low) > MOCA_PRIORITY_ALLOCATIONS_RESERVATION_LOW_MAX)
         return MOCA_PRIORITY_ALLOCATIONS_RESERVATION_LOW_ERR;
      if ((uint32_t) BE32(in->limitation_pqos) > MOCA_PRIORITY_ALLOCATIONS_LIMITATION_PQOS_MAX)
         return MOCA_PRIORITY_ALLOCATIONS_LIMITATION_PQOS_ERR;
      if ((uint32_t) BE32(in->limitation_high) > MOCA_PRIORITY_ALLOCATIONS_LIMITATION_HIGH_MAX)
         return MOCA_PRIORITY_ALLOCATIONS_LIMITATION_HIGH_ERR;
      if ((uint32_t) BE32(in->limitation_med) > MOCA_PRIORITY_ALLOCATIONS_LIMITATION_MED_MAX)
         return MOCA_PRIORITY_ALLOCATIONS_LIMITATION_MED_ERR;
      if ((uint32_t) BE32(in->limitation_low) > MOCA_PRIORITY_ALLOCATIONS_LIMITATION_LOW_MAX)
         return MOCA_PRIORITY_ALLOCATIONS_LIMITATION_LOW_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_egr_mc_addr_filter_check(struct moca_egr_mc_addr_filter_set * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->entryid) > MOCA_EGR_MC_ADDR_FILTER_ENTRYID_MAX)
         return MOCA_EGR_MC_ADDR_FILTER_ENTRYID_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_pause_fc_en_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_PAUSE_FC_EN_MAX)
         return MOCA_PAUSE_FC_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_stag_priority_check(struct moca_stag_priority * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->enable) > MOCA_STAG_PRIORITY_ENABLE_MAX)
         return MOCA_STAG_PRIORITY_ENABLE_ERR;
      if ((uint32_t) BE32(in->tag_mask) > MOCA_STAG_PRIORITY_TAG_MASK_MAX)
         return MOCA_STAG_PRIORITY_TAG_MASK_ERR;
      if ((uint32_t) BE32(in->moca_priority_0) > MOCA_STAG_PRIORITY_MOCA_PRIORITY_0_MAX)
         return MOCA_STAG_PRIORITY_MOCA_PRIORITY_0_ERR;
      if ((uint32_t) BE32(in->tag_priority_0) > MOCA_STAG_PRIORITY_TAG_PRIORITY_0_MAX)
         return MOCA_STAG_PRIORITY_TAG_PRIORITY_0_ERR;
      if ((uint32_t) BE32(in->moca_priority_1) > MOCA_STAG_PRIORITY_MOCA_PRIORITY_1_MAX)
         return MOCA_STAG_PRIORITY_MOCA_PRIORITY_1_ERR;
      if ((uint32_t) BE32(in->tag_priority_1) > MOCA_STAG_PRIORITY_TAG_PRIORITY_1_MAX)
         return MOCA_STAG_PRIORITY_TAG_PRIORITY_1_ERR;
      if ((uint32_t) BE32(in->moca_priority_2) > MOCA_STAG_PRIORITY_MOCA_PRIORITY_2_MAX)
         return MOCA_STAG_PRIORITY_MOCA_PRIORITY_2_ERR;
      if ((uint32_t) BE32(in->tag_priority_2) > MOCA_STAG_PRIORITY_TAG_PRIORITY_2_MAX)
         return MOCA_STAG_PRIORITY_TAG_PRIORITY_2_ERR;
      if ((uint32_t) BE32(in->moca_priority_3) > MOCA_STAG_PRIORITY_MOCA_PRIORITY_3_MAX)
         return MOCA_STAG_PRIORITY_MOCA_PRIORITY_3_ERR;
      if ((uint32_t) BE32(in->tag_priority_3) > MOCA_STAG_PRIORITY_TAG_PRIORITY_3_MAX)
         return MOCA_STAG_PRIORITY_TAG_PRIORITY_3_ERR;
      if ((uint32_t) BE32(in->moca_priority_4) > MOCA_STAG_PRIORITY_MOCA_PRIORITY_4_MAX)
         return MOCA_STAG_PRIORITY_MOCA_PRIORITY_4_ERR;
      if ((uint32_t) BE32(in->tag_priority_4) > MOCA_STAG_PRIORITY_TAG_PRIORITY_4_MAX)
         return MOCA_STAG_PRIORITY_TAG_PRIORITY_4_ERR;
      if ((uint32_t) BE32(in->tag_priority_5) > MOCA_STAG_PRIORITY_TAG_PRIORITY_5_MAX)
         return MOCA_STAG_PRIORITY_TAG_PRIORITY_5_ERR;
      if ((uint32_t) BE32(in->moca_priority_6) > MOCA_STAG_PRIORITY_MOCA_PRIORITY_6_MAX)
         return MOCA_STAG_PRIORITY_MOCA_PRIORITY_6_ERR;
      if ((uint32_t) BE32(in->tag_priority_6) > MOCA_STAG_PRIORITY_TAG_PRIORITY_6_MAX)
         return MOCA_STAG_PRIORITY_TAG_PRIORITY_6_ERR;
      if ((uint32_t) BE32(in->moca_priority_7) > MOCA_STAG_PRIORITY_MOCA_PRIORITY_7_MAX)
         return MOCA_STAG_PRIORITY_MOCA_PRIORITY_7_ERR;
      if ((uint32_t) BE32(in->tag_priority_7) > MOCA_STAG_PRIORITY_TAG_PRIORITY_7_MAX)
         return MOCA_STAG_PRIORITY_TAG_PRIORITY_7_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_stag_removal_check(struct moca_stag_removal * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->enable) > MOCA_STAG_REMOVAL_ENABLE_MAX)
         return MOCA_STAG_REMOVAL_ENABLE_ERR;
      if ((uint32_t) BE32(in->valid_0) > MOCA_STAG_REMOVAL_VALID_0_MAX)
         return MOCA_STAG_REMOVAL_VALID_0_ERR;
      if ((uint32_t) BE32(in->valid_1) > MOCA_STAG_REMOVAL_VALID_1_MAX)
         return MOCA_STAG_REMOVAL_VALID_1_ERR;
      if ((uint32_t) BE32(in->valid_2) > MOCA_STAG_REMOVAL_VALID_2_MAX)
         return MOCA_STAG_REMOVAL_VALID_2_ERR;
      if ((uint32_t) BE32(in->valid_3) > MOCA_STAG_REMOVAL_VALID_3_MAX)
         return MOCA_STAG_REMOVAL_VALID_3_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_pqos_create_flow_check(struct moca_pqos_create_flow_in * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->packet_size) < MOCA_PQOS_CREATE_FLOW_PACKET_SIZE_MIN)
         return MOCA_PQOS_CREATE_FLOW_PACKET_SIZE_ERR;
      if ((uint32_t) BE32(in->peak_data_rate) < MOCA_PQOS_CREATE_FLOW_PEAK_DATA_RATE_MIN)
         return MOCA_PQOS_CREATE_FLOW_PEAK_DATA_RATE_ERR;
      if ((uint32_t) BE32(in->peak_data_rate) > MOCA_PQOS_CREATE_FLOW_PEAK_DATA_RATE_MAX)
         return MOCA_PQOS_CREATE_FLOW_PEAK_DATA_RATE_ERR;
      if ((uint32_t) BE32(in->burst_size) > MOCA_PQOS_CREATE_FLOW_BURST_SIZE_MAX)
         return MOCA_PQOS_CREATE_FLOW_BURST_SIZE_ERR;
      if ((uint32_t) BE32(in->max_latency) > MOCA_PQOS_CREATE_FLOW_MAX_LATENCY_MAX)
         return MOCA_PQOS_CREATE_FLOW_MAX_LATENCY_ERR;
      if ((uint32_t) BE32(in->short_term_avg_ratio) > MOCA_PQOS_CREATE_FLOW_SHORT_TERM_AVG_RATIO_MAX)
         return MOCA_PQOS_CREATE_FLOW_SHORT_TERM_AVG_RATIO_ERR;
      if ((uint32_t) BE32(in->max_retry) > MOCA_PQOS_CREATE_FLOW_MAX_RETRY_MAX)
         return MOCA_PQOS_CREATE_FLOW_MAX_RETRY_ERR;
      if ((uint32_t) BE32(in->flow_per) > MOCA_PQOS_CREATE_FLOW_FLOW_PER_MAX)
         return MOCA_PQOS_CREATE_FLOW_FLOW_PER_ERR;
      if ((uint32_t) BE32(in->in_order_delivery) > MOCA_PQOS_CREATE_FLOW_IN_ORDER_DELIVERY_MAX)
         return MOCA_PQOS_CREATE_FLOW_IN_ORDER_DELIVERY_ERR;
      if ((uint32_t) BE32(in->traffic_protocol) > MOCA_PQOS_CREATE_FLOW_TRAFFIC_PROTOCOL_MAX)
         return MOCA_PQOS_CREATE_FLOW_TRAFFIC_PROTOCOL_ERR;
      do {
         switch ((uint32_t) BE32(in->ingr_class_rule))
         {
            case 0:
            case 4:
            case 5:
            case 6:
            case 7:
               continue;
         }
         return MOCA_PQOS_CREATE_FLOW_INGR_CLASS_RULE_ERR;
      } while (0);
      if ((uint32_t) BE32(in->vlan_tag) > MOCA_PQOS_CREATE_FLOW_VLAN_TAG_MAX)
         return MOCA_PQOS_CREATE_FLOW_VLAN_TAG_ERR;
      if ((uint32_t) BE32(in->dscp_moca) > MOCA_PQOS_CREATE_FLOW_DSCP_MOCA_MAX)
         return MOCA_PQOS_CREATE_FLOW_DSCP_MOCA_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_pqos_update_flow_check(struct moca_pqos_update_flow_in * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->packet_size) < MOCA_PQOS_UPDATE_FLOW_PACKET_SIZE_MIN)
         return MOCA_PQOS_UPDATE_FLOW_PACKET_SIZE_ERR;
      if ((uint32_t) BE32(in->peak_data_rate) < MOCA_PQOS_UPDATE_FLOW_PEAK_DATA_RATE_MIN)
         return MOCA_PQOS_UPDATE_FLOW_PEAK_DATA_RATE_ERR;
      if ((uint32_t) BE32(in->peak_data_rate) > MOCA_PQOS_UPDATE_FLOW_PEAK_DATA_RATE_MAX)
         return MOCA_PQOS_UPDATE_FLOW_PEAK_DATA_RATE_ERR;
      if ((uint32_t) BE32(in->burst_size) < MOCA_PQOS_UPDATE_FLOW_BURST_SIZE_MIN)
         return MOCA_PQOS_UPDATE_FLOW_BURST_SIZE_ERR;
      if ((uint32_t) BE32(in->burst_size) > MOCA_PQOS_UPDATE_FLOW_BURST_SIZE_MAX)
         return MOCA_PQOS_UPDATE_FLOW_BURST_SIZE_ERR;
      if ((uint32_t) BE32(in->max_latency) > MOCA_PQOS_UPDATE_FLOW_MAX_LATENCY_MAX)
         return MOCA_PQOS_UPDATE_FLOW_MAX_LATENCY_ERR;
      if ((uint32_t) BE32(in->short_term_avg_ratio) > MOCA_PQOS_UPDATE_FLOW_SHORT_TERM_AVG_RATIO_MAX)
         return MOCA_PQOS_UPDATE_FLOW_SHORT_TERM_AVG_RATIO_ERR;
      if ((uint32_t) BE32(in->max_retry) > MOCA_PQOS_UPDATE_FLOW_MAX_RETRY_MAX)
         return MOCA_PQOS_UPDATE_FLOW_MAX_RETRY_ERR;
      if ((uint32_t) BE32(in->flow_per) > MOCA_PQOS_UPDATE_FLOW_FLOW_PER_MAX)
         return MOCA_PQOS_UPDATE_FLOW_FLOW_PER_ERR;
      if ((uint32_t) BE32(in->in_order_delivery) > MOCA_PQOS_UPDATE_FLOW_IN_ORDER_DELIVERY_MAX)
         return MOCA_PQOS_UPDATE_FLOW_IN_ORDER_DELIVERY_ERR;
      if ((uint32_t) BE32(in->traffic_protocol) > MOCA_PQOS_UPDATE_FLOW_TRAFFIC_PROTOCOL_MAX)
         return MOCA_PQOS_UPDATE_FLOW_TRAFFIC_PROTOCOL_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_pqos_list_check(struct moca_pqos_list_in * in, uint32_t flags)
{
      if ((uint16_t) BE16(in->flow_max_return) > MOCA_PQOS_LIST_FLOW_MAX_RETURN_MAX)
         return MOCA_PQOS_LIST_FLOW_MAX_RETURN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_ooo_lmo_check(uint32_t node_id, uint32_t flags)
{
      if ((uint32_t) BE32(node_id) > MOCA_OOO_LMO_MAX)
         return MOCA_OOO_LMO_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_start_ulmo_check(struct moca_start_ulmo * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->report_type) > MOCA_START_ULMO_REPORT_TYPE_MAX)
         return MOCA_START_ULMO_REPORT_TYPE_ERR;
      if ((uint32_t) BE32(in->node_id) > MOCA_START_ULMO_NODE_ID_MAX)
         return MOCA_START_ULMO_NODE_ID_ERR;
      if ((uint32_t) BE32(in->ofdma_node_mask) > MOCA_START_ULMO_OFDMA_NODE_MASK_MAX)
         return MOCA_START_ULMO_OFDMA_NODE_MASK_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_rxd_lmo_request_check(struct moca_rxd_lmo_request * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->node_id) > MOCA_RXD_LMO_REQUEST_NODE_ID_MAX)
         return MOCA_RXD_LMO_REQUEST_NODE_ID_ERR;
      if ((uint32_t) BE32(in->probe_id) < MOCA_RXD_LMO_REQUEST_PROBE_ID_MIN)
         return MOCA_RXD_LMO_REQUEST_PROBE_ID_ERR;
      if ((uint32_t) BE32(in->probe_id) > MOCA_RXD_LMO_REQUEST_PROBE_ID_MAX)
         return MOCA_RXD_LMO_REQUEST_PROBE_ID_ERR;
      if ((uint32_t) BE32(in->channel_id) > MOCA_RXD_LMO_REQUEST_CHANNEL_ID_MAX)
         return MOCA_RXD_LMO_REQUEST_CHANNEL_ID_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_start_aca_check(struct moca_start_aca * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->src_node) > MOCA_START_ACA_SRC_NODE_MAX)
         return MOCA_START_ACA_SRC_NODE_ERR;
      if ((uint16_t) BE16(in->dest_nodemask) < MOCA_START_ACA_DEST_NODEMASK_MIN)
         return MOCA_START_ACA_DEST_NODEMASK_ERR;
      if ((uint16_t) BE16(in->num_probes) > MOCA_START_ACA_NUM_PROBES_MAX)
         return MOCA_START_ACA_NUM_PROBES_ERR;
      if ((uint32_t) BE32(in->type) < MOCA_START_ACA_TYPE_MIN)
         return MOCA_START_ACA_TYPE_ERR;
      if ((uint32_t) BE32(in->type) > MOCA_START_ACA_TYPE_MAX)
         return MOCA_START_ACA_TYPE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_aca_check(struct moca_aca_in * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->src_node) > MOCA_ACA_SRC_NODE_MAX)
         return MOCA_ACA_SRC_NODE_ERR;
      if ((uint32_t) BE32(in->dest_nodemask) < MOCA_ACA_DEST_NODEMASK_MIN)
         return MOCA_ACA_DEST_NODEMASK_ERR;
      if ((uint32_t) BE32(in->dest_nodemask) > MOCA_ACA_DEST_NODEMASK_MAX)
         return MOCA_ACA_DEST_NODEMASK_ERR;
      if ((uint32_t) BE32(in->type) < MOCA_ACA_TYPE_MIN)
         return MOCA_ACA_TYPE_ERR;
      if ((uint32_t) BE32(in->type) > MOCA_ACA_TYPE_MAX)
         return MOCA_ACA_TYPE_ERR;
      if ((uint32_t) BE32(in->num_probes) > MOCA_ACA_NUM_PROBES_MAX)
         return MOCA_ACA_NUM_PROBES_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_moca_reset_check(struct moca_moca_reset_in * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->reset_timer) > MOCA_MOCA_RESET_RESET_TIMER_MAX)
         return MOCA_MOCA_RESET_RESET_TIMER_ERR;
      if ((uint32_t) BE32(in->non_def_seq_num) > MOCA_MOCA_RESET_NON_DEF_SEQ_NUM_MAX)
         return MOCA_MOCA_RESET_NON_DEF_SEQ_NUM_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_wakeup_node_check(uint32_t node, uint32_t flags)
{
      if ((uint32_t) BE32(node) > MOCA_WAKEUP_NODE_MAX)
         return MOCA_WAKEUP_NODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_rf_switch_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_RF_SWITCH_MAX)
         return MOCA_RF_SWITCH_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_if_access_en_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_IF_ACCESS_EN_MAX)
         return MOCA_IF_ACCESS_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_led_mode_check(uint32_t val, uint32_t flags)
{
      if (flags & MOCA_STANDALONE_FLAG) {
         if ((uint32_t) BE32(val) > MOCA_LED_MODE_STANDALONE_MAX)
            return MOCA_LED_MODE_ERR;
      }
      else if ((uint32_t) BE32(val) > MOCA_LED_MODE_MAX)
         return MOCA_LED_MODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_m1_tx_power_variation_check(uint32_t state, uint32_t flags)
{
      if ((uint32_t) BE32(state) > MOCA_M1_TX_POWER_VARIATION_MAX)
         return MOCA_M1_TX_POWER_VARIATION_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_nc_listening_interval_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) < MOCA_NC_LISTENING_INTERVAL_MIN)
         return MOCA_NC_LISTENING_INTERVAL_ERR;
      if ((uint32_t) BE32(val) > MOCA_NC_LISTENING_INTERVAL_MAX)
         return MOCA_NC_LISTENING_INTERVAL_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_nc_heartbeat_interval_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) < MOCA_NC_HEARTBEAT_INTERVAL_MIN)
         return MOCA_NC_HEARTBEAT_INTERVAL_ERR;
      if ((uint32_t) BE32(val) > MOCA_NC_HEARTBEAT_INTERVAL_MAX)
         return MOCA_NC_HEARTBEAT_INTERVAL_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_wom_mode_internal_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_WOM_MODE_INTERNAL_MAX)
         return MOCA_WOM_MODE_INTERNAL_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_wom_magic_enable_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_WOM_MAGIC_ENABLE_MAX)
         return MOCA_WOM_MAGIC_ENABLE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_pm_restore_on_link_down_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_PM_RESTORE_ON_LINK_DOWN_MAX)
         return MOCA_PM_RESTORE_ON_LINK_DOWN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_power_state_check(uint32_t state, uint32_t flags)
{
      if ((uint32_t) BE32(state) > MOCA_POWER_STATE_MAX)
         return MOCA_POWER_STATE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_wom_pattern_check(struct moca_wom_pattern_set * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->index) > MOCA_WOM_PATTERN_INDEX_MAX)
         return MOCA_WOM_PATTERN_INDEX_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_wom_ip_check(struct moca_wom_ip * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->index) > MOCA_WOM_IP_INDEX_MAX)
         return MOCA_WOM_IP_INDEX_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_standby_power_state_check(uint32_t state, uint32_t flags)
{
      if ((uint32_t) BE32(state) > MOCA_STANDBY_POWER_STATE_MAX)
         return MOCA_STANDBY_POWER_STATE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_wom_mode_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_WOM_MODE_MAX)
         return MOCA_WOM_MODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_ps_cmd_check(uint32_t new_state, uint32_t flags)
{
      if ((uint32_t) BE32(new_state) > MOCA_PS_CMD_MAX)
         return MOCA_PS_CMD_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_pmk_exchange_interval_check(uint32_t msec, uint32_t flags)
{
      if ((uint32_t) BE32(msec) < MOCA_PMK_EXCHANGE_INTERVAL_MIN)
         return MOCA_PMK_EXCHANGE_INTERVAL_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_tek_exchange_interval_check(uint32_t msec, uint32_t flags)
{
      if ((uint32_t) BE32(msec) < MOCA_TEK_EXCHANGE_INTERVAL_MIN)
         return MOCA_TEK_EXCHANGE_INTERVAL_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_aes_exchange_interval_check(uint32_t msec, uint32_t flags)
{
      if ((uint32_t) BE32(msec) < MOCA_AES_EXCHANGE_INTERVAL_MIN)
         return MOCA_AES_EXCHANGE_INTERVAL_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_const_rx_submode_check(uint32_t submode, uint32_t flags)
{
      if ((uint32_t) BE32(submode) > MOCA_CONST_RX_SUBMODE_MAX)
         return MOCA_CONST_RX_SUBMODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_const_tx_params_check(struct moca_const_tx_params * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->const_tx_submode) > MOCA_CONST_TX_PARAMS_CONST_TX_SUBMODE_MAX)
         return MOCA_CONST_TX_PARAMS_CONST_TX_SUBMODE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int __moca_map_capture_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_MAP_CAPTURE_MAX)
         return MOCA_MAP_CAPTURE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_lab_iq_diagram_set_check(struct moca_lab_iq_diagram_set * in, uint32_t flags)
{
      if ((uint32_t) BE32(in->nodeid) > MOCA_LAB_IQ_DIAGRAM_SET_NODEID_MAX)
         return MOCA_LAB_IQ_DIAGRAM_SET_NODEID_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_mps_en_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_MPS_EN_MAX)
         return MOCA_MPS_EN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_mps_privacy_receive_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_MPS_PRIVACY_RECEIVE_MAX)
         return MOCA_MPS_PRIVACY_RECEIVE_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_mps_privacy_down_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_MPS_PRIVACY_DOWN_MAX)
         return MOCA_MPS_PRIVACY_DOWN_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_mps_walk_time_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) < MOCA_MPS_WALK_TIME_MIN)
         return MOCA_MPS_WALK_TIME_ERR;
      if ((uint32_t) BE32(val) > MOCA_MPS_WALK_TIME_MAX)
         return MOCA_MPS_WALK_TIME_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_mps_unpaired_time_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) < MOCA_MPS_UNPAIRED_TIME_MIN)
         return MOCA_MPS_UNPAIRED_TIME_ERR;
      if ((uint32_t) BE32(val) > MOCA_MPS_UNPAIRED_TIME_MAX)
         return MOCA_MPS_UNPAIRED_TIME_ERR;

   return 0;
}

MOCALIB_GEN_RANGE_FUNCTION int moca_privacy_defaults_check(uint32_t val, uint32_t flags)
{
      if ((uint32_t) BE32(val) > MOCA_PRIVACY_DEFAULTS_MAX)
         return MOCA_PRIVACY_DEFAULTS_ERR;

   return 0;
}

// Group node
MOCALIB_GEN_GET_FUNCTION int moca_get_preferred_nc(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PREFERRED_NC, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_preferred_nc(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PREFERRED_NC, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_preferred_nc (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--preferred_nc ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_single_channel_operation(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_SINGLE_CHANNEL_OPERATION, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_single_channel_operation(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_SINGLE_CHANNEL_OPERATION, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_single_channel_operation (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--single_channel_operation ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_continuous_power_tx_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_CONTINUOUS_POWER_TX_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_continuous_power_tx_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_CONTINUOUS_POWER_TX_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_continuous_rx_mode_attn(void *vctx, int32_t *val)
{
   return(moca_get(vctx, IE_CONTINUOUS_RX_MODE_ATTN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_continuous_rx_mode_attn(void *vctx, int32_t val)
{
   return(moca_set(vctx, IE_CONTINUOUS_RX_MODE_ATTN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_continuous_rx_mode_attn (char *cli, int32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--continuous_rx_mode_attn ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%d  ", (int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_lof(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_LOF, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_lof(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_LOF, &val, sizeof(val)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_max_nbas_primary(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MAX_NBAS_PRIMARY, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_max_nbas_primary(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MAX_NBAS_PRIMARY, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_nbas_primary (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_nbas_primary ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_tx1(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PS_SWCH_TX1, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_tx1(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PS_SWCH_TX1, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ps_swch_tx1 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ps_swch_tx1 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_tx2(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PS_SWCH_TX2, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_tx2(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PS_SWCH_TX2, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ps_swch_tx2 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ps_swch_tx2 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_rx1(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PS_SWCH_RX1, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_rx1(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PS_SWCH_RX1, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ps_swch_rx1 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ps_swch_rx1 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_rx2(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PS_SWCH_RX2, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_rx2(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PS_SWCH_RX2, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ps_swch_rx2 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ps_swch_rx2 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_rx3(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PS_SWCH_RX3, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_rx3(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PS_SWCH_RX3, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ps_swch_rx3 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ps_swch_rx3 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_prim_ch_offs(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PRIM_CH_OFFS, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_prim_ch_offs(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PRIM_CH_OFFS, &val, sizeof(val)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_sec_ch_offs(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_SEC_CH_OFFS, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_sec_ch_offs(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_SEC_CH_OFFS, &val, sizeof(val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_non_def_seq_num(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_NON_DEF_SEQ_NUM, &val, sizeof(val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_bonding(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_BONDING, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_bonding(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_BONDING, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_bonding (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--bonding ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_listening_freq_mask(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_LISTENING_FREQ_MASK, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_listening_freq_mask(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_LISTENING_FREQ_MASK, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_listening_freq_mask (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--listening_freq_mask ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_listening_duration(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_LISTENING_DURATION, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_listening_duration(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_LISTENING_DURATION, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_listening_duration (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--listening_duration ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_limit_traffic(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_LIMIT_TRAFFIC, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_limit_traffic(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_LIMIT_TRAFFIC, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_limit_traffic (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--limit_traffic ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_remote_man(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_REMOTE_MAN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_remote_man(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_REMOTE_MAN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_remote_man (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--remote_man ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_c4_moca20_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_C4_MOCA20_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_c4_moca20_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_C4_MOCA20_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_c4_moca20_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--c4_moca20_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_power_save_mechanism_dis(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_POWER_SAVE_MECHANISM_DIS, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_power_save_mechanism_dis(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_POWER_SAVE_MECHANISM_DIS, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_power_save_mechanism_dis (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--power_save_mechanism_dis ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_psm_config(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PSM_CONFIG, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_psm_config(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PSM_CONFIG, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_psm_config (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--psm_config ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_use_ext_data_mem(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_USE_EXT_DATA_MEM, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_use_ext_data_mem(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_USE_EXT_DATA_MEM, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_use_ext_data_mem (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--use_ext_data_mem ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_aif_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_AIF_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_aif_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--aif_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_aif_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_AIF_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_prof_pad_ctrl_deg_6802c0_bonding(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PROF_PAD_CTRL_DEG_6802C0_BONDING, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_prof_pad_ctrl_deg_6802c0_bonding(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PROF_PAD_CTRL_DEG_6802C0_BONDING, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_prof_pad_ctrl_deg_6802c0_bonding (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--prof_pad_ctrl_deg_6802c0_bonding ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_prop_bonding_compatibility_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PROP_BONDING_COMPATIBILITY_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_prop_bonding_compatibility_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PROP_BONDING_COMPATIBILITY_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_prop_bonding_compatibility_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--prop_bonding_compatibility_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rdeg_3450(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RDEG_3450, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rdeg_3450 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--rdeg_3450 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rdeg_3450(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RDEG_3450, val, sizeof(*val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_phy_clock(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PHY_CLOCK, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_phy_clock(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PHY_CLOCK, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_phy_clock (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--phy_clock ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_max_nbas_secondary(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MAX_NBAS_SECONDARY, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_max_nbas_secondary(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MAX_NBAS_SECONDARY, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_nbas_secondary (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_nbas_secondary ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mac_addr(void *vctx, struct moca_mac_addr *out)
{
   int ret;
   struct moca_mac_addr_int mac_addr_int;
   ret = moca_get_noswap(vctx, IE_MAC_ADDR, &mac_addr_int, sizeof(mac_addr_int));
   out->val = mac_addr_int.val;

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mac_addr(void *vctx, struct moca_mac_addr *in)
{
   struct moca_mac_addr_int tmp;
   tmp.val = in->val;
   tmp.reserved_0 = 0;
   return(moca_set_noswap(vctx, IE_MAC_ADDR, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_node_status(void *vctx, struct moca_node_status *out)
{
   int ret;
   ret = moca_get(vctx, IE_NODE_STATUS, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_start_moca_core(void *vctx, uint32_t phy_freq)
{
   return(moca_set(vctx, IE_START_MOCA_CORE, &phy_freq, sizeof(phy_freq)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_download_to_packet_ram_done(void *vctx)
{
   return(moca_set(vctx, IE_DOWNLOAD_TO_PACKET_RAM_DONE, NULL, 0));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_beacon_channel_set(void *vctx, uint32_t channel)
{
   return(moca_set(vctx, IE_BEACON_CHANNEL_SET, &channel, sizeof(channel)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_beacon_channel_set (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--beacon_channel_set ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_fw_version(void *vctx, struct moca_fw_version *out)
{
   int ret;
   ret = moca_get(vctx, IE_FW_VERSION, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_shutdown(void *vctx)
{
   return(moca_set(vctx, IE_SHUTDOWN, NULL, 0));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_tx_power_tune(void *vctx, struct moca_max_tx_power_tune *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_MAX_TX_POWER_TUNE, out, sizeof(*out));
   moca_max_tx_power_tune_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_tx_power_tune(void *vctx, struct moca_max_tx_power_tune *in)
{
   struct moca_max_tx_power_tune tmp = *in;
   moca_max_tx_power_tune_swap(&tmp);
   return(moca_set_noswap(vctx, IE_MAX_TX_POWER_TUNE, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_tx_power_tune (char *cli, struct moca_max_tx_power_tune * in, uint32_t * max_len)
{
   int8_t last_value = (int8_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--max_tx_power_tune ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 86 + 1; i++) {
      if ( ((i != 0) && (in->offset[i] != last_value)) || (i == (86))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offset ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index * 25);
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) * 25);
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offset[i];
   }
   bytes_written = snprintf (c + strlen(c), *max_len, "padding %u  ", (unsigned int) in->padding);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_tx_power_tune_sec_ch(void *vctx, struct moca_max_tx_power_tune_sec_ch *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_MAX_TX_POWER_TUNE_SEC_CH, out, sizeof(*out));
   moca_max_tx_power_tune_sec_ch_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_tx_power_tune_sec_ch(void *vctx, struct moca_max_tx_power_tune_sec_ch *in)
{
   struct moca_max_tx_power_tune_sec_ch tmp = *in;
   moca_max_tx_power_tune_sec_ch_swap(&tmp);
   return(moca_set_noswap(vctx, IE_MAX_TX_POWER_TUNE_SEC_CH, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_tx_power_tune_sec_ch (char *cli, struct moca_max_tx_power_tune_sec_ch * in, uint32_t * max_len)
{
   int8_t last_value = (int8_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--max_tx_power_tune_sec_ch ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 86 + 1; i++) {
      if ( ((i != 0) && (in->offset[i] != last_value)) || (i == (86))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offset ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index * 25);
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) * 25);
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offset[i];
   }
   bytes_written = snprintf (c + strlen(c), *max_len, "padding %u  ", (unsigned int) in->padding);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rx_power_tune(void *vctx, struct moca_rx_power_tune *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_RX_POWER_TUNE, out, sizeof(*out));
   moca_rx_power_tune_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rx_power_tune(void *vctx, struct moca_rx_power_tune *in)
{
   struct moca_rx_power_tune tmp = *in;
   moca_rx_power_tune_swap(&tmp);
   return(moca_set_noswap(vctx, IE_RX_POWER_TUNE, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rx_power_tune (char *cli, struct moca_rx_power_tune * in, uint32_t * max_len)
{
   int8_t last_value = (int8_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--rx_power_tune ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 86 + 1; i++) {
      if ( ((i != 0) && (in->offset[i] != last_value)) || (i == (86))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offset ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index * 25);
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) * 25);
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offset[i];
   }
   bytes_written = snprintf (c + strlen(c), *max_len, "padding %u  ", (unsigned int) in->padding);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mocad_forwarding_rx_mac(void *vctx, macaddr_t * mac_addr)
{
   return(moca_set(vctx, IE_MOCAD_FORWARDING_RX_MAC, mac_addr, sizeof(*mac_addr)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mocad_forwarding_rx_ack(void *vctx, const struct moca_mocad_forwarding_rx_ack *in)
{
   return(moca_set(vctx, IE_MOCAD_FORWARDING_RX_ACK, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mocad_forwarding_tx_alloc(void *vctx, struct moca_mocad_forwarding_tx_alloc *out)
{
   int ret;
   ret = moca_get(vctx, IE_MOCAD_FORWARDING_TX_ALLOC, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mocad_forwarding_tx_send(void *vctx, const struct moca_mocad_forwarding_tx_send *in)
{
   return(moca_set(vctx, IE_MOCAD_FORWARDING_TX_SEND, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_impedance_mode_bonding(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_IMPEDANCE_MODE_BONDING, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_impedance_mode_bonding(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_IMPEDANCE_MODE_BONDING, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_impedance_mode_bonding (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--impedance_mode_bonding ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rework_6802(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_REWORK_6802, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rework_6802(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_REWORK_6802, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rework_6802 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--rework_6802 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_prof_pad_ctrl_deg_6802c0_single(void *vctx, struct moca_prof_pad_ctrl_deg_6802c0_single *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_PROF_PAD_CTRL_DEG_6802C0_SINGLE, out, sizeof(*out));
   moca_prof_pad_ctrl_deg_6802c0_single_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_prof_pad_ctrl_deg_6802c0_single(void *vctx, struct moca_prof_pad_ctrl_deg_6802c0_single *in)
{
   struct moca_prof_pad_ctrl_deg_6802c0_single tmp = *in;
   moca_prof_pad_ctrl_deg_6802c0_single_swap(&tmp);
   return(moca_set_noswap(vctx, IE_PROF_PAD_CTRL_DEG_6802C0_SINGLE, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_prof_pad_ctrl_deg_6802c0_single (char *cli, struct moca_prof_pad_ctrl_deg_6802c0_single * in, uint32_t * max_len)
{
   int8_t last_value = (int8_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--prof_pad_ctrl_deg_6802c0_single ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 86 + 1; i++) {
      if ( ((i != 0) && (in->offset[i] != last_value)) || (i == (86))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offset ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index * 25);
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) * 25);
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offset[i];
   }
   bytes_written = snprintf (c + strlen(c), *max_len, "padding %u  ", (unsigned int) in->padding);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

static inline int mocalib_handle_core_ready(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_core_ready *in = (struct moca_core_ready *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_core_ready_swap(in);
   if(ctx->cb.core_ready_cb == NULL)
      return(-5);
   ctx->cb.core_ready_cb(ctx->cb.core_ready_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_core_ready_cb(void *vctx, void (*callback)(void *userarg, struct moca_core_ready *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.core_ready_cb = callback;
   ctx->cb.core_ready_userarg = userarg;
}

static inline int mocalib_handle_power_up_status(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.power_up_status_cb == NULL)
      return(-3);
   ctx->cb.power_up_status_cb(ctx->cb.power_up_status_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_power_up_status_cb(void *vctx, void (*callback)(void *userarg, uint32_t status), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.power_up_status_cb = callback;
   ctx->cb.power_up_status_userarg = userarg;
}

static inline int mocalib_handle_new_lof(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.new_lof_cb == NULL)
      return(-3);
   ctx->cb.new_lof_cb(ctx->cb.new_lof_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_new_lof_cb(void *vctx, void (*callback)(void *userarg, uint32_t lof), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.new_lof_cb = callback;
   ctx->cb.new_lof_userarg = userarg;
}

static inline int mocalib_handle_admission_completed(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.admission_completed_cb == NULL)
      return(-3);
   ctx->cb.admission_completed_cb(ctx->cb.admission_completed_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_admission_completed_cb(void *vctx, void (*callback)(void *userarg, uint32_t lof), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.admission_completed_cb = callback;
   ctx->cb.admission_completed_userarg = userarg;
}

static inline int mocalib_handle_tpcap_done(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   if(ctx->cb.tpcap_done_cb == NULL)
      return(-3);
   ctx->cb.tpcap_done_cb(ctx->cb.tpcap_done_userarg);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_tpcap_done_cb(void *vctx, void (*callback)(void *userarg), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.tpcap_done_cb = callback;
   ctx->cb.tpcap_done_userarg = userarg;
}

static inline int mocalib_handle_mocad_forwarding_rx_packet(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_mocad_forwarding_rx_packet *in = (struct moca_mocad_forwarding_rx_packet *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.mocad_forwarding_rx_packet_cb == NULL)
      return(-4);
   ctx->cb.mocad_forwarding_rx_packet_cb(ctx->cb.mocad_forwarding_rx_packet_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_mocad_forwarding_rx_packet_cb(void *vctx, void (*callback)(void *userarg, struct moca_mocad_forwarding_rx_packet *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.mocad_forwarding_rx_packet_cb = callback;
   ctx->cb.mocad_forwarding_rx_packet_userarg = userarg;
}

static inline int mocalib_handle_mocad_forwarding_tx_ack(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.mocad_forwarding_tx_ack_cb == NULL)
      return(-3);
   ctx->cb.mocad_forwarding_tx_ack_cb(ctx->cb.mocad_forwarding_tx_ack_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_mocad_forwarding_tx_ack_cb(void *vctx, void (*callback)(void *userarg, uint32_t offset), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.mocad_forwarding_tx_ack_cb = callback;
   ctx->cb.mocad_forwarding_tx_ack_userarg = userarg;
}

static inline int mocalib_handle_pr_degradation(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   if(ctx->cb.pr_degradation_cb == NULL)
      return(-3);
   ctx->cb.pr_degradation_cb(ctx->cb.pr_degradation_userarg);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_pr_degradation_cb(void *vctx, void (*callback)(void *userarg), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.pr_degradation_cb = callback;
   ctx->cb.pr_degradation_userarg = userarg;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_start(void *vctx)
{
   return(moca_set(vctx, IE_START, NULL, 0));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_stop(void *vctx)
{
   return(moca_set(vctx, IE_STOP, NULL, 0));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_drv_info(void *vctx, uint32_t reset_stats, struct moca_drv_info *out)
{
   int ret;
   ret = moca_get_inout_noswap(vctx, IE_DRV_INFO, &reset_stats, sizeof(reset_stats), out, sizeof(*out));
   moca_drv_info_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_miscval(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MISCVAL, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_miscval (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--miscval ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_miscval(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MISCVAL, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_en_capable(void *vctx, uint32_t enable)
{
   return(moca_set(vctx, IE_EN_CAPABLE, &enable, sizeof(enable)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_en_capable (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--en_capable ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_en_capable(void *vctx, uint32_t *enable)
{
   return(moca_get(vctx, IE_EN_CAPABLE, enable, sizeof(*enable)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_restore_defaults(void *vctx)
{
   return(moca_set(vctx, IE_RESTORE_DEFAULTS, NULL, 0));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mocad_version(void *vctx, struct moca_mocad_version *out)
{
   int ret;
   ret = moca_get(vctx, IE_MOCAD_VERSION, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_restart(void *vctx)
{
   return(moca_set(vctx, IE_RESTART, NULL, 0));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_lof_update(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_LOF_UPDATE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_lof_update(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_LOF_UPDATE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_lof_update (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--lof_update ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_primary_ch_offset(void *vctx, int32_t *val)
{
   return(moca_get(vctx, IE_PRIMARY_CH_OFFSET, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_primary_ch_offset(void *vctx, int32_t val)
{
   return(moca_set(vctx, IE_PRIMARY_CH_OFFSET, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_primary_ch_offset (char *cli, int32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--primary_ch_offset ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%d  ", (int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_assertText(void *vctx, uint32_t *assertText)
{
   return(moca_get(vctx, IE_ASSERTTEXT, assertText, sizeof(*assertText)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_assertText(void *vctx, uint32_t assertText)
{
   return(moca_set(vctx, IE_ASSERTTEXT, &assertText, sizeof(assertText)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_assertText (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--assertText ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_wdog_enable(void *vctx, uint32_t *enable)
{
   return(moca_get(vctx, IE_WDOG_ENABLE, enable, sizeof(*enable)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_wdog_enable(void *vctx, uint32_t enable)
{
   return(moca_set(vctx, IE_WDOG_ENABLE, &enable, sizeof(enable)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_wdog_enable (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--wdog_enable ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_miscval2(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MISCVAL2, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_miscval2 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--miscval2 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_miscval2(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MISCVAL2, val, sizeof(*val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mr_seq_num(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MR_SEQ_NUM, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mr_seq_num(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MR_SEQ_NUM, &val, sizeof(val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_secondary_ch_offset(void *vctx, int32_t *val)
{
   return(moca_get(vctx, IE_SECONDARY_CH_OFFSET, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_secondary_ch_offset(void *vctx, int32_t val)
{
   return(moca_set(vctx, IE_SECONDARY_CH_OFFSET, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_secondary_ch_offset (char *cli, int32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--secondary_ch_offset ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%d  ", (int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_cof(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_COF, &val, sizeof(val)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_config_flags(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_CONFIG_FLAGS, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_amp_type(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_AMP_TYPE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_amp_type (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--amp_type ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_amp_type(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_AMP_TYPE, val, sizeof(*val)));
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_preferred_nc( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_preferred_nc;
   uint32_t preferred_nc;
   memset(&default_preferred_nc, 0, sizeof(default_preferred_nc));
   memset(&preferred_nc, 0, sizeof(preferred_nc));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_preferred_nc(handle, &preferred_nc);
      moca_set_preferred_nc_defaults(&default_preferred_nc, config_flags);

      if ((ret == 0) && (memcmp(&default_preferred_nc, &preferred_nc, sizeof(preferred_nc))!=0))
         moca_nv_print_preferred_nc(cli, &preferred_nc, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_single_channel_operation( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_single_channel_operation;
   uint32_t single_channel_operation;
   memset(&default_single_channel_operation, 0, sizeof(default_single_channel_operation));
   memset(&single_channel_operation, 0, sizeof(single_channel_operation));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_single_channel_operation(handle, &single_channel_operation);
      moca_set_single_channel_operation_defaults(&default_single_channel_operation, config_flags);

      if ((ret == 0) && (memcmp(&default_single_channel_operation, &single_channel_operation, sizeof(single_channel_operation))!=0))
         moca_nv_print_single_channel_operation(cli, &single_channel_operation, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_continuous_rx_mode_attn( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   int32_t default_continuous_rx_mode_attn;
   int32_t continuous_rx_mode_attn;
   memset(&default_continuous_rx_mode_attn, 0, sizeof(default_continuous_rx_mode_attn));
   memset(&continuous_rx_mode_attn, 0, sizeof(continuous_rx_mode_attn));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_continuous_rx_mode_attn(handle, &continuous_rx_mode_attn);
      moca_set_continuous_rx_mode_attn_defaults(&default_continuous_rx_mode_attn, config_flags);

      if ((ret == 0) && (memcmp(&default_continuous_rx_mode_attn, &continuous_rx_mode_attn, sizeof(continuous_rx_mode_attn))!=0))
         moca_nv_print_continuous_rx_mode_attn(cli, &continuous_rx_mode_attn, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_nbas_primary( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_nbas_primary;
   uint32_t max_nbas_primary;
   memset(&default_max_nbas_primary, 0, sizeof(default_max_nbas_primary));
   memset(&max_nbas_primary, 0, sizeof(max_nbas_primary));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_max_nbas_primary(handle, &max_nbas_primary);
      moca_set_max_nbas_primary_defaults(&default_max_nbas_primary, config_flags);

      if ((ret == 0) && (memcmp(&default_max_nbas_primary, &max_nbas_primary, sizeof(max_nbas_primary))!=0))
         moca_nv_print_max_nbas_primary(cli, &max_nbas_primary, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_ps_swch_tx1( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_ps_swch_tx1;
   uint32_t ps_swch_tx1;
   memset(&default_ps_swch_tx1, 0, sizeof(default_ps_swch_tx1));
   memset(&ps_swch_tx1, 0, sizeof(ps_swch_tx1));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_ps_swch_tx1(handle, &ps_swch_tx1);
      moca_set_ps_swch_tx1_defaults(&default_ps_swch_tx1, config_flags);

      if ((ret == 0) && (memcmp(&default_ps_swch_tx1, &ps_swch_tx1, sizeof(ps_swch_tx1))!=0))
         moca_nv_print_ps_swch_tx1(cli, &ps_swch_tx1, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_ps_swch_tx2( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_ps_swch_tx2;
   uint32_t ps_swch_tx2;
   memset(&default_ps_swch_tx2, 0, sizeof(default_ps_swch_tx2));
   memset(&ps_swch_tx2, 0, sizeof(ps_swch_tx2));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_ps_swch_tx2(handle, &ps_swch_tx2);
      moca_set_ps_swch_tx2_defaults(&default_ps_swch_tx2, config_flags);

      if ((ret == 0) && (memcmp(&default_ps_swch_tx2, &ps_swch_tx2, sizeof(ps_swch_tx2))!=0))
         moca_nv_print_ps_swch_tx2(cli, &ps_swch_tx2, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_ps_swch_rx1( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_ps_swch_rx1;
   uint32_t ps_swch_rx1;
   memset(&default_ps_swch_rx1, 0, sizeof(default_ps_swch_rx1));
   memset(&ps_swch_rx1, 0, sizeof(ps_swch_rx1));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_ps_swch_rx1(handle, &ps_swch_rx1);
      moca_set_ps_swch_rx1_defaults(&default_ps_swch_rx1, config_flags);

      if ((ret == 0) && (memcmp(&default_ps_swch_rx1, &ps_swch_rx1, sizeof(ps_swch_rx1))!=0))
         moca_nv_print_ps_swch_rx1(cli, &ps_swch_rx1, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_ps_swch_rx2( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_ps_swch_rx2;
   uint32_t ps_swch_rx2;
   memset(&default_ps_swch_rx2, 0, sizeof(default_ps_swch_rx2));
   memset(&ps_swch_rx2, 0, sizeof(ps_swch_rx2));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_ps_swch_rx2(handle, &ps_swch_rx2);
      moca_set_ps_swch_rx2_defaults(&default_ps_swch_rx2, config_flags);

      if ((ret == 0) && (memcmp(&default_ps_swch_rx2, &ps_swch_rx2, sizeof(ps_swch_rx2))!=0))
         moca_nv_print_ps_swch_rx2(cli, &ps_swch_rx2, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_ps_swch_rx3( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_ps_swch_rx3;
   uint32_t ps_swch_rx3;
   memset(&default_ps_swch_rx3, 0, sizeof(default_ps_swch_rx3));
   memset(&ps_swch_rx3, 0, sizeof(ps_swch_rx3));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_ps_swch_rx3(handle, &ps_swch_rx3);
      moca_set_ps_swch_rx3_defaults(&default_ps_swch_rx3, config_flags);

      if ((ret == 0) && (memcmp(&default_ps_swch_rx3, &ps_swch_rx3, sizeof(ps_swch_rx3))!=0))
         moca_nv_print_ps_swch_rx3(cli, &ps_swch_rx3, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_bonding( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_bonding;
   uint32_t bonding;
   memset(&default_bonding, 0, sizeof(default_bonding));
   memset(&bonding, 0, sizeof(bonding));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_bonding(handle, &bonding);
      moca_set_bonding_defaults(&default_bonding, config_flags);

      if ((ret == 0) && (memcmp(&default_bonding, &bonding, sizeof(bonding))!=0))
         moca_nv_print_bonding(cli, &bonding, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_listening_freq_mask( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_listening_freq_mask;
   uint32_t listening_freq_mask;
   memset(&default_listening_freq_mask, 0, sizeof(default_listening_freq_mask));
   memset(&listening_freq_mask, 0, sizeof(listening_freq_mask));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_listening_freq_mask(handle, &listening_freq_mask);
      moca_set_listening_freq_mask_defaults(&default_listening_freq_mask, config_flags);

      if ((ret == 0) && (memcmp(&default_listening_freq_mask, &listening_freq_mask, sizeof(listening_freq_mask))!=0))
         moca_nv_print_listening_freq_mask(cli, &listening_freq_mask, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_listening_duration( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_listening_duration;
   uint32_t listening_duration;
   memset(&default_listening_duration, 0, sizeof(default_listening_duration));
   memset(&listening_duration, 0, sizeof(listening_duration));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_listening_duration(handle, &listening_duration);
      moca_set_listening_duration_defaults(&default_listening_duration, config_flags);

      if ((ret == 0) && (memcmp(&default_listening_duration, &listening_duration, sizeof(listening_duration))!=0))
         moca_nv_print_listening_duration(cli, &listening_duration, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_limit_traffic( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_limit_traffic;
   uint32_t limit_traffic;
   memset(&default_limit_traffic, 0, sizeof(default_limit_traffic));
   memset(&limit_traffic, 0, sizeof(limit_traffic));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_limit_traffic(handle, &limit_traffic);
      moca_set_limit_traffic_defaults(&default_limit_traffic, config_flags);

      if ((ret == 0) && (memcmp(&default_limit_traffic, &limit_traffic, sizeof(limit_traffic))!=0))
         moca_nv_print_limit_traffic(cli, &limit_traffic, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_remote_man( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_remote_man;
   uint32_t remote_man;
   memset(&default_remote_man, 0, sizeof(default_remote_man));
   memset(&remote_man, 0, sizeof(remote_man));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_remote_man(handle, &remote_man);
      moca_set_remote_man_defaults(&default_remote_man, config_flags);

      if ((ret == 0) && (memcmp(&default_remote_man, &remote_man, sizeof(remote_man))!=0))
         moca_nv_print_remote_man(cli, &remote_man, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_c4_moca20_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_c4_moca20_en;
   uint32_t c4_moca20_en;
   memset(&default_c4_moca20_en, 0, sizeof(default_c4_moca20_en));
   memset(&c4_moca20_en, 0, sizeof(c4_moca20_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_c4_moca20_en(handle, &c4_moca20_en);
      moca_set_c4_moca20_en_defaults(&default_c4_moca20_en, config_flags);

      if ((ret == 0) && (memcmp(&default_c4_moca20_en, &c4_moca20_en, sizeof(c4_moca20_en))!=0))
         moca_nv_print_c4_moca20_en(cli, &c4_moca20_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_power_save_mechanism_dis( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_power_save_mechanism_dis;
   uint32_t power_save_mechanism_dis;
   memset(&default_power_save_mechanism_dis, 0, sizeof(default_power_save_mechanism_dis));
   memset(&power_save_mechanism_dis, 0, sizeof(power_save_mechanism_dis));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_power_save_mechanism_dis(handle, &power_save_mechanism_dis);
      moca_set_power_save_mechanism_dis_defaults(&default_power_save_mechanism_dis, config_flags);

      if ((ret == 0) && (memcmp(&default_power_save_mechanism_dis, &power_save_mechanism_dis, sizeof(power_save_mechanism_dis))!=0))
         moca_nv_print_power_save_mechanism_dis(cli, &power_save_mechanism_dis, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_psm_config( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_psm_config;
   uint32_t psm_config;
   memset(&default_psm_config, 0, sizeof(default_psm_config));
   memset(&psm_config, 0, sizeof(psm_config));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_psm_config(handle, &psm_config);
      moca_set_psm_config_defaults(&default_psm_config, config_flags);

      if ((ret == 0) && (memcmp(&default_psm_config, &psm_config, sizeof(psm_config))!=0))
         moca_nv_print_psm_config(cli, &psm_config, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_use_ext_data_mem( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_use_ext_data_mem;
   uint32_t use_ext_data_mem;
   memset(&default_use_ext_data_mem, 0, sizeof(default_use_ext_data_mem));
   memset(&use_ext_data_mem, 0, sizeof(use_ext_data_mem));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_use_ext_data_mem(handle, &use_ext_data_mem);
      moca_set_use_ext_data_mem_defaults(&default_use_ext_data_mem, config_flags);

      if ((ret == 0) && (memcmp(&default_use_ext_data_mem, &use_ext_data_mem, sizeof(use_ext_data_mem))!=0))
         moca_nv_print_use_ext_data_mem(cli, &use_ext_data_mem, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_aif_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_aif_mode;
   uint32_t aif_mode;
   memset(&default_aif_mode, 0, sizeof(default_aif_mode));
   memset(&aif_mode, 0, sizeof(aif_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_aif_mode(handle, &aif_mode);
      moca_set_aif_mode_defaults(&default_aif_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_aif_mode, &aif_mode, sizeof(aif_mode))!=0))
         moca_nv_print_aif_mode(cli, &aif_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_prof_pad_ctrl_deg_6802c0_bonding( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_prof_pad_ctrl_deg_6802c0_bonding;
   uint32_t prof_pad_ctrl_deg_6802c0_bonding;
   memset(&default_prof_pad_ctrl_deg_6802c0_bonding, 0, sizeof(default_prof_pad_ctrl_deg_6802c0_bonding));
   memset(&prof_pad_ctrl_deg_6802c0_bonding, 0, sizeof(prof_pad_ctrl_deg_6802c0_bonding));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_prof_pad_ctrl_deg_6802c0_bonding(handle, &prof_pad_ctrl_deg_6802c0_bonding);
      moca_set_prof_pad_ctrl_deg_6802c0_bonding_defaults(&default_prof_pad_ctrl_deg_6802c0_bonding, config_flags);

      if ((ret == 0) && (memcmp(&default_prof_pad_ctrl_deg_6802c0_bonding, &prof_pad_ctrl_deg_6802c0_bonding, sizeof(prof_pad_ctrl_deg_6802c0_bonding))!=0))
         moca_nv_print_prof_pad_ctrl_deg_6802c0_bonding(cli, &prof_pad_ctrl_deg_6802c0_bonding, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_prop_bonding_compatibility_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_prop_bonding_compatibility_mode;
   uint32_t prop_bonding_compatibility_mode;
   memset(&default_prop_bonding_compatibility_mode, 0, sizeof(default_prop_bonding_compatibility_mode));
   memset(&prop_bonding_compatibility_mode, 0, sizeof(prop_bonding_compatibility_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_prop_bonding_compatibility_mode(handle, &prop_bonding_compatibility_mode);
      moca_set_prop_bonding_compatibility_mode_defaults(&default_prop_bonding_compatibility_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_prop_bonding_compatibility_mode, &prop_bonding_compatibility_mode, sizeof(prop_bonding_compatibility_mode))!=0))
         moca_nv_print_prop_bonding_compatibility_mode(cli, &prop_bonding_compatibility_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rdeg_3450( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_rdeg_3450;
   uint32_t rdeg_3450;
   memset(&default_rdeg_3450, 0, sizeof(default_rdeg_3450));
   memset(&rdeg_3450, 0, sizeof(rdeg_3450));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rdeg_3450(handle, &rdeg_3450);
      moca_set_rdeg_3450_defaults(&default_rdeg_3450, config_flags);

      if ((ret == 0) && (memcmp(&default_rdeg_3450, &rdeg_3450, sizeof(rdeg_3450))!=0))
         moca_nv_print_rdeg_3450(cli, &rdeg_3450, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_phy_clock( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_phy_clock;
   uint32_t phy_clock;
   memset(&default_phy_clock, 0, sizeof(default_phy_clock));
   memset(&phy_clock, 0, sizeof(phy_clock));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_phy_clock(handle, &phy_clock);
      moca_set_phy_clock_defaults(&default_phy_clock, config_flags);

      if ((ret == 0) && (memcmp(&default_phy_clock, &phy_clock, sizeof(phy_clock))!=0))
         moca_nv_print_phy_clock(cli, &phy_clock, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_nbas_secondary( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_nbas_secondary;
   uint32_t max_nbas_secondary;
   memset(&default_max_nbas_secondary, 0, sizeof(default_max_nbas_secondary));
   memset(&max_nbas_secondary, 0, sizeof(max_nbas_secondary));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_max_nbas_secondary(handle, &max_nbas_secondary);
      moca_set_max_nbas_secondary_defaults(&default_max_nbas_secondary, config_flags);

      if ((ret == 0) && (memcmp(&default_max_nbas_secondary, &max_nbas_secondary, sizeof(max_nbas_secondary))!=0))
         moca_nv_print_max_nbas_secondary(cli, &max_nbas_secondary, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_tx_power_tune( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_max_tx_power_tune default_max_tx_power_tune;
   struct moca_max_tx_power_tune max_tx_power_tune;
   memset(&default_max_tx_power_tune, 0, sizeof(default_max_tx_power_tune));
   memset(&max_tx_power_tune, 0, sizeof(max_tx_power_tune));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_tx_power_tune(handle, &max_tx_power_tune);
      moca_set_max_tx_power_tune_defaults(&default_max_tx_power_tune, config_flags);

      if ((ret == 0) && (memcmp(&default_max_tx_power_tune, &max_tx_power_tune, sizeof(max_tx_power_tune))!=0))
         moca_nv_print_max_tx_power_tune(cli, &max_tx_power_tune, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_tx_power_tune_sec_ch( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_max_tx_power_tune_sec_ch default_max_tx_power_tune_sec_ch;
   struct moca_max_tx_power_tune_sec_ch max_tx_power_tune_sec_ch;
   memset(&default_max_tx_power_tune_sec_ch, 0, sizeof(default_max_tx_power_tune_sec_ch));
   memset(&max_tx_power_tune_sec_ch, 0, sizeof(max_tx_power_tune_sec_ch));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_tx_power_tune_sec_ch(handle, &max_tx_power_tune_sec_ch);
      moca_set_max_tx_power_tune_sec_ch_defaults(&default_max_tx_power_tune_sec_ch, config_flags);

      if ((ret == 0) && (memcmp(&default_max_tx_power_tune_sec_ch, &max_tx_power_tune_sec_ch, sizeof(max_tx_power_tune_sec_ch))!=0))
         moca_nv_print_max_tx_power_tune_sec_ch(cli, &max_tx_power_tune_sec_ch, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rx_power_tune( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_rx_power_tune default_rx_power_tune;
   struct moca_rx_power_tune rx_power_tune;
   memset(&default_rx_power_tune, 0, sizeof(default_rx_power_tune));
   memset(&rx_power_tune, 0, sizeof(rx_power_tune));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rx_power_tune(handle, &rx_power_tune);
      moca_set_rx_power_tune_defaults(&default_rx_power_tune, config_flags);

      if ((ret == 0) && (memcmp(&default_rx_power_tune, &rx_power_tune, sizeof(rx_power_tune))!=0))
         moca_nv_print_rx_power_tune(cli, &rx_power_tune, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_impedance_mode_bonding( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_impedance_mode_bonding;
   uint32_t impedance_mode_bonding;
   memset(&default_impedance_mode_bonding, 0, sizeof(default_impedance_mode_bonding));
   memset(&impedance_mode_bonding, 0, sizeof(impedance_mode_bonding));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_impedance_mode_bonding(handle, &impedance_mode_bonding);
      moca_set_impedance_mode_bonding_defaults(&default_impedance_mode_bonding, config_flags);

      if ((ret == 0) && (memcmp(&default_impedance_mode_bonding, &impedance_mode_bonding, sizeof(impedance_mode_bonding))!=0))
         moca_nv_print_impedance_mode_bonding(cli, &impedance_mode_bonding, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rework_6802( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_rework_6802;
   uint32_t rework_6802;
   memset(&default_rework_6802, 0, sizeof(default_rework_6802));
   memset(&rework_6802, 0, sizeof(rework_6802));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rework_6802(handle, &rework_6802);
      moca_set_rework_6802_defaults(&default_rework_6802, config_flags);

      if ((ret == 0) && (memcmp(&default_rework_6802, &rework_6802, sizeof(rework_6802))!=0))
         moca_nv_print_rework_6802(cli, &rework_6802, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_prof_pad_ctrl_deg_6802c0_single( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_prof_pad_ctrl_deg_6802c0_single default_prof_pad_ctrl_deg_6802c0_single;
   struct moca_prof_pad_ctrl_deg_6802c0_single prof_pad_ctrl_deg_6802c0_single;
   memset(&default_prof_pad_ctrl_deg_6802c0_single, 0, sizeof(default_prof_pad_ctrl_deg_6802c0_single));
   memset(&prof_pad_ctrl_deg_6802c0_single, 0, sizeof(prof_pad_ctrl_deg_6802c0_single));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_prof_pad_ctrl_deg_6802c0_single(handle, &prof_pad_ctrl_deg_6802c0_single);
      moca_set_prof_pad_ctrl_deg_6802c0_single_defaults(&default_prof_pad_ctrl_deg_6802c0_single, config_flags);

      if ((ret == 0) && (memcmp(&default_prof_pad_ctrl_deg_6802c0_single, &prof_pad_ctrl_deg_6802c0_single, sizeof(prof_pad_ctrl_deg_6802c0_single))!=0))
         moca_nv_print_prof_pad_ctrl_deg_6802c0_single(cli, &prof_pad_ctrl_deg_6802c0_single, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_en_capable( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_en_capable;
   uint32_t en_capable;
   memset(&default_en_capable, 0, sizeof(default_en_capable));
   memset(&en_capable, 0, sizeof(en_capable));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_en_capable(handle, &en_capable);
      moca_set_en_capable_defaults(&default_en_capable, config_flags);

      if ((ret == 0) && (memcmp(&default_en_capable, &en_capable, sizeof(en_capable))!=0))
         moca_nv_print_en_capable(cli, &en_capable, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_lof_update( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_lof_update;
   uint32_t lof_update;
   memset(&default_lof_update, 0, sizeof(default_lof_update));
   memset(&lof_update, 0, sizeof(lof_update));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_lof_update(handle, &lof_update);
      moca_set_lof_update_defaults(&default_lof_update, config_flags);

      if ((ret == 0) && (memcmp(&default_lof_update, &lof_update, sizeof(lof_update))!=0))
         moca_nv_print_lof_update(cli, &lof_update, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_primary_ch_offset( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   int32_t default_primary_ch_offset;
   int32_t primary_ch_offset;
   memset(&default_primary_ch_offset, 0, sizeof(default_primary_ch_offset));
   memset(&primary_ch_offset, 0, sizeof(primary_ch_offset));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_primary_ch_offset(handle, &primary_ch_offset);
      moca_set_primary_ch_offset_defaults(&default_primary_ch_offset, config_flags);

      if ((ret == 0) && (memcmp(&default_primary_ch_offset, &primary_ch_offset, sizeof(primary_ch_offset))!=0))
         moca_nv_print_primary_ch_offset(cli, &primary_ch_offset, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_assertText( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_assertText;
   uint32_t assertText;
   memset(&default_assertText, 0, sizeof(default_assertText));
   memset(&assertText, 0, sizeof(assertText));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_assertText(handle, &assertText);
      moca_set_assertText_defaults(&default_assertText, config_flags);

      if ((ret == 0) && (memcmp(&default_assertText, &assertText, sizeof(assertText))!=0))
         moca_nv_print_assertText(cli, &assertText, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_wdog_enable( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_wdog_enable;
   uint32_t wdog_enable;
   memset(&default_wdog_enable, 0, sizeof(default_wdog_enable));
   memset(&wdog_enable, 0, sizeof(wdog_enable));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_wdog_enable(handle, &wdog_enable);
      moca_set_wdog_enable_defaults(&default_wdog_enable, config_flags);

      if ((ret == 0) && (memcmp(&default_wdog_enable, &wdog_enable, sizeof(wdog_enable))!=0))
         moca_nv_print_wdog_enable(cli, &wdog_enable, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_secondary_ch_offset( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   int32_t default_secondary_ch_offset;
   int32_t secondary_ch_offset;
   memset(&default_secondary_ch_offset, 0, sizeof(default_secondary_ch_offset));
   memset(&secondary_ch_offset, 0, sizeof(secondary_ch_offset));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_secondary_ch_offset(handle, &secondary_ch_offset);
      moca_set_secondary_ch_offset_defaults(&default_secondary_ch_offset, config_flags);

      if ((ret == 0) && (memcmp(&default_secondary_ch_offset, &secondary_ch_offset, sizeof(secondary_ch_offset))!=0))
         moca_nv_print_secondary_ch_offset(cli, &secondary_ch_offset, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_amp_type( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_amp_type;
   uint32_t amp_type;
   memset(&default_amp_type, 0, sizeof(default_amp_type));
   memset(&amp_type, 0, sizeof(amp_type));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_amp_type(handle, &amp_type);
      moca_set_amp_type_defaults(&default_amp_type, config_flags);

      if ((ret == 0) && (memcmp(&default_amp_type, &amp_type, sizeof(amp_type))!=0))
         moca_nv_print_amp_type(cli, &amp_type, max_len);
}

// Group phy
MOCALIB_GEN_GET_FUNCTION int moca_get_tpc_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_TPC_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_tpc_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_TPC_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_tpc_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--tpc_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_tx_power(void *vctx, int32_t *val)
{
   return(moca_get(vctx, IE_MAX_TX_POWER, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_tx_power(void *vctx, int32_t val)
{
   return(moca_set(vctx, IE_MAX_TX_POWER, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_tx_power (char *cli, int32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_tx_power ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%d  ", (int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_beacon_pwr_reduction(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_BEACON_PWR_REDUCTION, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_beacon_pwr_reduction(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_BEACON_PWR_REDUCTION, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_beacon_pwr_reduction (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--beacon_pwr_reduction ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_beacon_pwr_reduction_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_BEACON_PWR_REDUCTION_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_beacon_pwr_reduction_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_BEACON_PWR_REDUCTION_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_beacon_pwr_reduction_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--beacon_pwr_reduction_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_bo_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_BO_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_bo_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_BO_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_bo_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--bo_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_qam256_capability(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_QAM256_CAPABILITY, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_qam256_capability(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_QAM256_CAPABILITY, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_qam256_capability (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--qam256_capability ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_otf_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_OTF_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_otf_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_OTF_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_otf_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--otf_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_star_topology_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_STAR_TOPOLOGY_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_star_topology_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_STAR_TOPOLOGY_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_star_topology_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--star_topology_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_ofdma_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_OFDMA_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_ofdma_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_OFDMA_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ofdma_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ofdma_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_min_bw_alarm_threshold(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_MIN_BW_ALARM_THRESHOLD, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_min_bw_alarm_threshold(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_MIN_BW_ALARM_THRESHOLD, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_min_bw_alarm_threshold (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--min_bw_alarm_threshold ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_en_max_rate_in_max_bo(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_EN_MAX_RATE_IN_MAX_BO, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_en_max_rate_in_max_bo(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_EN_MAX_RATE_IN_MAX_BO, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_en_max_rate_in_max_bo (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--en_max_rate_in_max_bo ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_target_phy_rate_qam128(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_TARGET_PHY_RATE_QAM128, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_target_phy_rate_qam128(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_TARGET_PHY_RATE_QAM128, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_target_phy_rate_qam128 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--target_phy_rate_qam128 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_target_phy_rate_qam256(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_TARGET_PHY_RATE_QAM256, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_target_phy_rate_qam256(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_TARGET_PHY_RATE_QAM256, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_target_phy_rate_qam256 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--target_phy_rate_qam256 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_sapm_en(void *vctx, uint32_t *bool_val)
{
   return(moca_get(vctx, IE_SAPM_EN, bool_val, sizeof(*bool_val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_sapm_en(void *vctx, uint32_t bool_val)
{
   return(moca_set(vctx, IE_SAPM_EN, &bool_val, sizeof(bool_val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_sapm_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--sapm_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_arpl_th_50(void *vctx, int32_t *arpl)
{
   return(moca_get(vctx, IE_ARPL_TH_50, arpl, sizeof(*arpl)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_arpl_th_50(void *vctx, int32_t arpl)
{
   return(moca_set(vctx, IE_ARPL_TH_50, &arpl, sizeof(arpl)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_arpl_th_50 (char *cli, int32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--arpl_th_50 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%d  ", (int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rlapm_en(void *vctx, uint32_t *bool_val)
{
   return(moca_get(vctx, IE_RLAPM_EN, bool_val, sizeof(*bool_val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rlapm_en(void *vctx, uint32_t bool_val)
{
   return(moca_set(vctx, IE_RLAPM_EN, &bool_val, sizeof(bool_val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rlapm_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--rlapm_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_freq_shift(void *vctx, uint32_t *direction)
{
   return(moca_get(vctx, IE_FREQ_SHIFT, direction, sizeof(*direction)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_freq_shift(void *vctx, uint32_t direction)
{
   return(moca_set(vctx, IE_FREQ_SHIFT, &direction, sizeof(direction)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_freq_shift (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--freq_shift ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_phy_rate(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_MAX_PHY_RATE, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_phy_rate(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_MAX_PHY_RATE, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_phy_rate (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_phy_rate ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_bandwidth(void *vctx, uint32_t *bandwidth)
{
   return(moca_get(vctx, IE_BANDWIDTH, bandwidth, sizeof(*bandwidth)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_bandwidth(void *vctx, uint32_t bandwidth)
{
   return(moca_set(vctx, IE_BANDWIDTH, &bandwidth, sizeof(bandwidth)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_bandwidth (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--bandwidth ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_arpl_th_100(void *vctx, int32_t *arpl)
{
   return(moca_get(vctx, IE_ARPL_TH_100, arpl, sizeof(*arpl)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_arpl_th_100(void *vctx, int32_t arpl)
{
   return(moca_set(vctx, IE_ARPL_TH_100, &arpl, sizeof(arpl)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_arpl_th_100 (char *cli, int32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--arpl_th_100 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%d  ", (int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_adc_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_ADC_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_adc_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_ADC_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_adc_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--adc_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_phy_rate_turbo(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_MAX_PHY_RATE_TURBO, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_phy_rate_turbo(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_MAX_PHY_RATE_TURBO, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_phy_rate_turbo (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_phy_rate_turbo ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_cp_const(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_CP_CONST, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_cp_const(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_CP_CONST, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_cp_const (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--cp_const ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_preamble_uc_const(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PREAMBLE_UC_CONST, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_preamble_uc_const(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PREAMBLE_UC_CONST, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_preamble_uc_const (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--preamble_uc_const ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_cp_margin_increase(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_CP_MARGIN_INCREASE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_cp_margin_increase(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_CP_MARGIN_INCREASE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_cp_margin_increase (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--cp_margin_increase ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_ac_cc_shift(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_AC_CC_SHIFT, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_ac_cc_shift(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_AC_CC_SHIFT, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ac_cc_shift (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ac_cc_shift ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_mfc_th_increase(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MFC_TH_INCREASE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_mfc_th_increase(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MFC_TH_INCREASE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_mfc_th_increase (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--mfc_th_increase ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_agc_const_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_AGC_CONST_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_agc_const_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_AGC_CONST_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_agc_const_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--agc_const_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_agc_const_address(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_AGC_CONST_ADDRESS, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_agc_const_address(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_AGC_CONST_ADDRESS, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_agc_const_address (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--agc_const_address ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_min_bo_insert_2_bfm_lock(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MIN_BO_INSERT_2_BFM_LOCK, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_min_bo_insert_2_bfm_lock(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MIN_BO_INSERT_2_BFM_LOCK, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_min_bo_insert_2_bfm_lock (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--min_bo_insert_2_bfm_lock ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_min_snr_avg_db_2_bfm_lock(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MIN_SNR_AVG_DB_2_BFM_LOCK, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_min_snr_avg_db_2_bfm_lock(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MIN_SNR_AVG_DB_2_BFM_LOCK, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_min_snr_avg_db_2_bfm_lock (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--min_snr_avg_db_2_bfm_lock ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_phy_rate_50M(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_MAX_PHY_RATE_50M, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_phy_rate_50M(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_MAX_PHY_RATE_50M, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_phy_rate_50M (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_phy_rate_50M ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_constellation_all(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MAX_CONSTELLATION_ALL, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_constellation_all(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MAX_CONSTELLATION_ALL, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_constellation_all (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_constellation_all ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_constellation(void *vctx, uint32_t node_id, struct moca_max_constellation *out)
{
   int ret;
   ret = moca_get_inout(vctx, IE_MAX_CONSTELLATION, &node_id, sizeof(node_id), out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_constellation(void *vctx, const struct moca_max_constellation *in)
{
   return(moca_set(vctx, IE_MAX_CONSTELLATION, in, sizeof(*in)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_constellation (char *cli, struct moca_max_constellation * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_constellation ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "node_id %u  ", (unsigned int) in->node_id);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "p2p_limit_50 %u  ", (unsigned int) in->p2p_limit_50);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "gcd_limit_50 %u  ", (unsigned int) in->gcd_limit_50);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "p2p_limit_100 %u  ", (unsigned int) in->p2p_limit_100);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "gcd_limit_100 %u  ", (unsigned int) in->gcd_limit_100);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_sig_y_done(void *vctx, const struct moca_sig_y_done *in)
{
   return(moca_set(vctx, IE_SIG_Y_DONE, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_rs(void *vctx, struct moca_snr_margin_table_rs *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_TABLE_RS, out, sizeof(*out));
   moca_snr_margin_table_rs_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_rs(void *vctx, struct moca_snr_margin_table_rs *in)
{
   struct moca_snr_margin_table_rs tmp = *in;
   moca_snr_margin_table_rs_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_TABLE_RS, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_sapm_table_fw_50(void *vctx, struct moca_sapm_table_fw_50 *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SAPM_TABLE_FW_50, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_sapm_table_fw_50(void *vctx, struct moca_sapm_table_fw_50 *in)
{
   struct moca_sapm_table_fw_50 tmp = *in;
   return(moca_set_noswap(vctx, IE_SAPM_TABLE_FW_50, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rlapm_table_50(void *vctx, struct moca_rlapm_table_50 *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_RLAPM_TABLE_50, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rlapm_table_50(void *vctx, struct moca_rlapm_table_50 *in)
{
   struct moca_rlapm_table_50 tmp = *in;
   return(moca_set_noswap(vctx, IE_RLAPM_TABLE_50, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rlapm_table_50 (char *cli, struct moca_rlapm_table_50 * in, uint32_t * max_len)
{
   uint8_t last_value = (uint8_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--rlapm_table_50 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 66 + 1; i++) {
      if ( ((i != 0) && (in->rlapmtable[i] != last_value)) || (i == (66))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "rlapmtable ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%u  %d ", (unsigned int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->rlapmtable[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_phy_status(void *vctx, uint32_t *tx_gcd_power_reduction)
{
   return(moca_get(vctx, IE_PHY_STATUS, tx_gcd_power_reduction, sizeof(*tx_gcd_power_reduction)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ldpc(void *vctx, struct moca_snr_margin_table_ldpc *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_TABLE_LDPC, out, sizeof(*out));
   moca_snr_margin_table_ldpc_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ldpc(void *vctx, struct moca_snr_margin_table_ldpc *in)
{
   struct moca_snr_margin_table_ldpc tmp = *in;
   moca_snr_margin_table_ldpc_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_TABLE_LDPC, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ldpc_sec_ch(void *vctx, struct moca_snr_margin_table_ldpc_sec_ch *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_TABLE_LDPC_SEC_CH, out, sizeof(*out));
   moca_snr_margin_table_ldpc_sec_ch_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ldpc_sec_ch(void *vctx, struct moca_snr_margin_table_ldpc_sec_ch *in)
{
   struct moca_snr_margin_table_ldpc_sec_ch tmp = *in;
   moca_snr_margin_table_ldpc_sec_ch_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_TABLE_LDPC_SEC_CH, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ldpc_pre5(void *vctx, struct moca_snr_margin_table_ldpc_pre5 *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_TABLE_LDPC_PRE5, out, sizeof(*out));
   moca_snr_margin_table_ldpc_pre5_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ldpc_pre5(void *vctx, struct moca_snr_margin_table_ldpc_pre5 *in)
{
   struct moca_snr_margin_table_ldpc_pre5 tmp = *in;
   moca_snr_margin_table_ldpc_pre5_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_TABLE_LDPC_PRE5, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ofdma(void *vctx, struct moca_snr_margin_table_ofdma *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_TABLE_OFDMA, out, sizeof(*out));
   moca_snr_margin_table_ofdma_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ofdma(void *vctx, struct moca_snr_margin_table_ofdma *in)
{
   struct moca_snr_margin_table_ofdma tmp = *in;
   moca_snr_margin_table_ofdma_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_TABLE_OFDMA, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_sapm_table_fw_100(void *vctx, struct moca_sapm_table_fw_100 *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SAPM_TABLE_FW_100, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_sapm_table_fw_100(void *vctx, struct moca_sapm_table_fw_100 *in)
{
   struct moca_sapm_table_fw_100 tmp = *in;
   return(moca_set_noswap(vctx, IE_SAPM_TABLE_FW_100, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rlapm_table_100(void *vctx, struct moca_rlapm_table_100 *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_RLAPM_TABLE_100, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rlapm_table_100(void *vctx, struct moca_rlapm_table_100 *in)
{
   struct moca_rlapm_table_100 tmp = *in;
   return(moca_set_noswap(vctx, IE_RLAPM_TABLE_100, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rlapm_table_100 (char *cli, struct moca_rlapm_table_100 * in, uint32_t * max_len)
{
   uint8_t last_value = (uint8_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--rlapm_table_100 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 66 + 1; i++) {
      if ( ((i != 0) && (in->rlapmtable[i] != last_value)) || (i == (66))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "rlapmtable ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%u  %d ", (unsigned int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->rlapmtable[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_cplength(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_CPLENGTH, &val, sizeof(val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_rf_calib_data(void *vctx, struct moca_rf_calib_data *in)
{
   struct moca_rf_calib_data tmp = *in;
   moca_rf_calib_data_swap(&tmp);
   return(moca_set_noswap(vctx, IE_RF_CALIB_DATA, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_probe_2_results(void *vctx, const struct moca_probe_2_results *in)
{
   return(moca_set(vctx, IE_PROBE_2_RESULTS, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_sapm_table_fw_sec(void *vctx, struct moca_sapm_table_fw_sec *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SAPM_TABLE_FW_SEC, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_sapm_table_fw_sec(void *vctx, struct moca_sapm_table_fw_sec *in)
{
   struct moca_sapm_table_fw_sec tmp = *in;
   return(moca_set_noswap(vctx, IE_SAPM_TABLE_FW_SEC, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rx_gain_params(void *vctx, uint32_t table_index, struct moca_rx_gain_params *out)
{
   int ret;
   ret = moca_get_inout(vctx, IE_RX_GAIN_PARAMS, &table_index, sizeof(table_index), out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_rx_gain_agc_table(void *vctx, uint32_t * in, uint32_t *out, int max_out_len)
{
   int ret;

   ret = moca_get_table(vctx, IE_RX_GAIN_AGC_TABLE, in, out, sizeof(*out), max_out_len, FL_SWAP_RD);

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_tx_power_params(void *vctx, struct moca_tx_power_params_in *in, struct moca_tx_power_params *out)
{
   int ret;
   ret = moca_get_inout_noswap(vctx, IE_TX_POWER_PARAMS, in, sizeof(*in), out, sizeof(*out));
   moca_tx_power_params_swap(out);

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ldpc_pri_ch(void *vctx, struct moca_snr_margin_table_ldpc_pri_ch *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_TABLE_LDPC_PRI_CH, out, sizeof(*out));
   moca_snr_margin_table_ldpc_pri_ch_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ldpc_pri_ch(void *vctx, struct moca_snr_margin_table_ldpc_pri_ch *in)
{
   struct moca_snr_margin_table_ldpc_pri_ch tmp = *in;
   moca_snr_margin_table_ldpc_pri_ch_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_TABLE_LDPC_PRI_CH, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_pre5_pri_ch(void *vctx, struct moca_snr_margin_table_pre5_pri_ch *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_TABLE_PRE5_PRI_CH, out, sizeof(*out));
   moca_snr_margin_table_pre5_pri_ch_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_pre5_pri_ch(void *vctx, struct moca_snr_margin_table_pre5_pri_ch *in)
{
   struct moca_snr_margin_table_pre5_pri_ch tmp = *in;
   moca_snr_margin_table_pre5_pri_ch_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_TABLE_PRE5_PRI_CH, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_pre5_sec_ch(void *vctx, struct moca_snr_margin_table_pre5_sec_ch *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_TABLE_PRE5_SEC_CH, out, sizeof(*out));
   moca_snr_margin_table_pre5_sec_ch_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_pre5_sec_ch(void *vctx, struct moca_snr_margin_table_pre5_sec_ch *in)
{
   struct moca_snr_margin_table_pre5_sec_ch tmp = *in;
   moca_snr_margin_table_pre5_sec_ch_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_TABLE_PRE5_SEC_CH, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_nv_cal_enable(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_NV_CAL_ENABLE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_nv_cal_enable (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--nv_cal_enable ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_nv_cal_enable(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_NV_CAL_ENABLE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rlapm_cap_50(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RLAPM_CAP_50, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rlapm_cap_50 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--rlapm_cap_50 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rlapm_cap_50(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RLAPM_CAP_50, val, sizeof(*val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_margin_rs(void *vctx, struct moca_snr_margin_rs *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_RS, out, sizeof(*out));
   moca_snr_margin_rs_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_snr_margin_rs(void *vctx, struct moca_snr_margin_rs *in)
{
   struct moca_snr_margin_rs tmp = *in;
   moca_snr_margin_rs_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_RS, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_snr_margin_rs (char *cli, struct moca_snr_margin_rs * in, uint32_t * max_len)
{
   int16_t last_value = (int16_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--snr_margin_rs ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "base_margin %d  ", (int) in->base_margin);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 10 + 1; i++) {
      if ( ((i != 0) && (in->offsets[i] != last_value)) || (i == (10))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offsets ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offsets[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_margin_ldpc(void *vctx, struct moca_snr_margin_ldpc *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_LDPC, out, sizeof(*out));
   moca_snr_margin_ldpc_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_snr_margin_ldpc(void *vctx, struct moca_snr_margin_ldpc *in)
{
   struct moca_snr_margin_ldpc tmp = *in;
   moca_snr_margin_ldpc_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_LDPC, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_snr_margin_ldpc (char *cli, struct moca_snr_margin_ldpc * in, uint32_t * max_len)
{
   int16_t last_value = (int16_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--snr_margin_ldpc ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "base_margin %d  ", (int) in->base_margin);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 10 + 1; i++) {
      if ( ((i != 0) && (in->offsets[i] != last_value)) || (i == (10))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offsets ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offsets[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_margin_ldpc_sec_ch(void *vctx, struct moca_snr_margin_ldpc_sec_ch *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_LDPC_SEC_CH, out, sizeof(*out));
   moca_snr_margin_ldpc_sec_ch_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_snr_margin_ldpc_sec_ch(void *vctx, struct moca_snr_margin_ldpc_sec_ch *in)
{
   struct moca_snr_margin_ldpc_sec_ch tmp = *in;
   moca_snr_margin_ldpc_sec_ch_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_LDPC_SEC_CH, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_snr_margin_ldpc_sec_ch (char *cli, struct moca_snr_margin_ldpc_sec_ch * in, uint32_t * max_len)
{
   int16_t last_value = (int16_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--snr_margin_ldpc_sec_ch ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "base_margin %d  ", (int) in->base_margin);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 10 + 1; i++) {
      if ( ((i != 0) && (in->offsets[i] != last_value)) || (i == (10))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offsets ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offsets[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_margin_ldpc_pre5(void *vctx, struct moca_snr_margin_ldpc_pre5 *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_LDPC_PRE5, out, sizeof(*out));
   moca_snr_margin_ldpc_pre5_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_snr_margin_ldpc_pre5(void *vctx, struct moca_snr_margin_ldpc_pre5 *in)
{
   struct moca_snr_margin_ldpc_pre5 tmp = *in;
   moca_snr_margin_ldpc_pre5_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_LDPC_PRE5, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_snr_margin_ldpc_pre5 (char *cli, struct moca_snr_margin_ldpc_pre5 * in, uint32_t * max_len)
{
   int16_t last_value = (int16_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--snr_margin_ldpc_pre5 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "base_margin %d  ", (int) in->base_margin);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 10 + 1; i++) {
      if ( ((i != 0) && (in->offsets[i] != last_value)) || (i == (10))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offsets ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offsets[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_margin_ofdma(void *vctx, struct moca_snr_margin_ofdma *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_OFDMA, out, sizeof(*out));
   moca_snr_margin_ofdma_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_snr_margin_ofdma(void *vctx, struct moca_snr_margin_ofdma *in)
{
   struct moca_snr_margin_ofdma tmp = *in;
   moca_snr_margin_ofdma_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_OFDMA, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_snr_margin_ofdma (char *cli, struct moca_snr_margin_ofdma * in, uint32_t * max_len)
{
   int16_t last_value = (int16_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--snr_margin_ofdma ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "base_margin %d  ", (int) in->base_margin);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 10 + 1; i++) {
      if ( ((i != 0) && (in->offsets[i] != last_value)) || (i == (10))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offsets ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offsets[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rlapm_cap_100(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RLAPM_CAP_100, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rlapm_cap_100 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--rlapm_cap_100 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rlapm_cap_100(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RLAPM_CAP_100, val, sizeof(*val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_sapm_table_50(void *vctx, struct moca_sapm_table_50 *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SAPM_TABLE_50, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_sapm_table_50(void *vctx, struct moca_sapm_table_50 *in)
{
   struct moca_sapm_table_50 tmp = *in;
   return(moca_set_noswap(vctx, IE_SAPM_TABLE_50, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_sapm_table_50 (char *cli, struct moca_sapm_table_50 * in, uint32_t * max_len)
{
   uint8_t last_value = (uint8_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--sapm_table_50 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 256 + 1; i++) {
      if ( ((i != 0) && (in->val[i] != last_value)) || (i == (256))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "val ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%u  %d ", (unsigned int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->val[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_sapm_table_100(void *vctx, struct moca_sapm_table_100 *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SAPM_TABLE_100, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_sapm_table_100(void *vctx, struct moca_sapm_table_100 *in)
{
   struct moca_sapm_table_100 tmp = *in;
   return(moca_set_noswap(vctx, IE_SAPM_TABLE_100, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_sapm_table_100 (char *cli, struct moca_sapm_table_100 * in, uint32_t * max_len)
{
   uint8_t last_value = (uint8_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--sapm_table_100 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 512 + 1; i++) {
      if ( ((i != 0) && (in->val[i] != last_value)) || (i == (512))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "val ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%u  %d ", (unsigned int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->val[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_nv_cal_clear(void *vctx)
{
   return(moca_set(vctx, IE_NV_CAL_CLEAR, NULL, 0));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_sapm_table_sec(void *vctx, struct moca_sapm_table_sec *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SAPM_TABLE_SEC, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_sapm_table_sec(void *vctx, struct moca_sapm_table_sec *in)
{
   struct moca_sapm_table_sec tmp = *in;
   return(moca_set_noswap(vctx, IE_SAPM_TABLE_SEC, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_sapm_table_sec (char *cli, struct moca_sapm_table_sec * in, uint32_t * max_len)
{
   uint8_t last_value = (uint8_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--sapm_table_sec ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 512 + 1; i++) {
      if ( ((i != 0) && (in->val[i] != last_value)) || (i == (512))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "val ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%u  %d ", (unsigned int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->val[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_amp_reg(void *vctx, uint32_t addr, struct moca_amp_reg *out)
{
   int ret;
   ret = moca_get_inout(vctx, IE_AMP_REG, &addr, sizeof(addr), out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_amp_reg(void *vctx, const struct moca_amp_reg *in)
{
   return(moca_set(vctx, IE_AMP_REG, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_margin_ldpc_pri_ch(void *vctx, struct moca_snr_margin_ldpc_pri_ch *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_LDPC_PRI_CH, out, sizeof(*out));
   moca_snr_margin_ldpc_pri_ch_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_snr_margin_ldpc_pri_ch(void *vctx, struct moca_snr_margin_ldpc_pri_ch *in)
{
   struct moca_snr_margin_ldpc_pri_ch tmp = *in;
   moca_snr_margin_ldpc_pri_ch_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_LDPC_PRI_CH, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_snr_margin_ldpc_pri_ch (char *cli, struct moca_snr_margin_ldpc_pri_ch * in, uint32_t * max_len)
{
   int16_t last_value = (int16_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--snr_margin_ldpc_pri_ch ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "base_margin %d  ", (int) in->base_margin);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 10 + 1; i++) {
      if ( ((i != 0) && (in->offsets[i] != last_value)) || (i == (10))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offsets ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offsets[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_margin_pre5_pri_ch(void *vctx, struct moca_snr_margin_pre5_pri_ch *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_PRE5_PRI_CH, out, sizeof(*out));
   moca_snr_margin_pre5_pri_ch_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_snr_margin_pre5_pri_ch(void *vctx, struct moca_snr_margin_pre5_pri_ch *in)
{
   struct moca_snr_margin_pre5_pri_ch tmp = *in;
   moca_snr_margin_pre5_pri_ch_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_PRE5_PRI_CH, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_snr_margin_pre5_pri_ch (char *cli, struct moca_snr_margin_pre5_pri_ch * in, uint32_t * max_len)
{
   int16_t last_value = (int16_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--snr_margin_pre5_pri_ch ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "base_margin %d  ", (int) in->base_margin);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 10 + 1; i++) {
      if ( ((i != 0) && (in->offsets[i] != last_value)) || (i == (10))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offsets ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offsets[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_margin_pre5_sec_ch(void *vctx, struct moca_snr_margin_pre5_sec_ch *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_SNR_MARGIN_PRE5_SEC_CH, out, sizeof(*out));
   moca_snr_margin_pre5_sec_ch_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_snr_margin_pre5_sec_ch(void *vctx, struct moca_snr_margin_pre5_sec_ch *in)
{
   struct moca_snr_margin_pre5_sec_ch tmp = *in;
   moca_snr_margin_pre5_sec_ch_swap(&tmp);
   return(moca_set_noswap(vctx, IE_SNR_MARGIN_PRE5_SEC_CH, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_snr_margin_pre5_sec_ch (char *cli, struct moca_snr_margin_pre5_sec_ch * in, uint32_t * max_len)
{
   int16_t last_value = (int16_t) 0;
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--snr_margin_pre5_sec_ch ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "base_margin %d  ", (int) in->base_margin);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 10 + 1; i++) {
      if ( ((i != 0) && (in->offsets[i] != last_value)) || (i == (10))) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "offsets ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%d  %d ", (int) last_value, start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      last_value = in->offsets[i];
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_tpc_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_tpc_en;
   uint32_t tpc_en;
   memset(&default_tpc_en, 0, sizeof(default_tpc_en));
   memset(&tpc_en, 0, sizeof(tpc_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_tpc_en(handle, &tpc_en);
      moca_set_tpc_en_defaults(&default_tpc_en, config_flags);

      if ((ret == 0) && (memcmp(&default_tpc_en, &tpc_en, sizeof(tpc_en))!=0))
         moca_nv_print_tpc_en(cli, &tpc_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_tx_power( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   int32_t default_max_tx_power;
   int32_t max_tx_power;
   memset(&default_max_tx_power, 0, sizeof(default_max_tx_power));
   memset(&max_tx_power, 0, sizeof(max_tx_power));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_tx_power(handle, &max_tx_power);
      moca_set_max_tx_power_defaults(&default_max_tx_power, config_flags);

      if ((ret == 0) && (memcmp(&default_max_tx_power, &max_tx_power, sizeof(max_tx_power))!=0))
         moca_nv_print_max_tx_power(cli, &max_tx_power, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_beacon_pwr_reduction( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_beacon_pwr_reduction;
   uint32_t beacon_pwr_reduction;
   memset(&default_beacon_pwr_reduction, 0, sizeof(default_beacon_pwr_reduction));
   memset(&beacon_pwr_reduction, 0, sizeof(beacon_pwr_reduction));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_beacon_pwr_reduction(handle, &beacon_pwr_reduction);
      moca_set_beacon_pwr_reduction_defaults(&default_beacon_pwr_reduction, config_flags);

      if ((ret == 0) && (memcmp(&default_beacon_pwr_reduction, &beacon_pwr_reduction, sizeof(beacon_pwr_reduction))!=0))
         moca_nv_print_beacon_pwr_reduction(cli, &beacon_pwr_reduction, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_beacon_pwr_reduction_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_beacon_pwr_reduction_en;
   uint32_t beacon_pwr_reduction_en;
   memset(&default_beacon_pwr_reduction_en, 0, sizeof(default_beacon_pwr_reduction_en));
   memset(&beacon_pwr_reduction_en, 0, sizeof(beacon_pwr_reduction_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_beacon_pwr_reduction_en(handle, &beacon_pwr_reduction_en);
      moca_set_beacon_pwr_reduction_en_defaults(&default_beacon_pwr_reduction_en, config_flags);

      if ((ret == 0) && (memcmp(&default_beacon_pwr_reduction_en, &beacon_pwr_reduction_en, sizeof(beacon_pwr_reduction_en))!=0))
         moca_nv_print_beacon_pwr_reduction_en(cli, &beacon_pwr_reduction_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_bo_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_bo_mode;
   uint32_t bo_mode;
   memset(&default_bo_mode, 0, sizeof(default_bo_mode));
   memset(&bo_mode, 0, sizeof(bo_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_bo_mode(handle, &bo_mode);
      moca_set_bo_mode_defaults(&default_bo_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_bo_mode, &bo_mode, sizeof(bo_mode))!=0))
         moca_nv_print_bo_mode(cli, &bo_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_qam256_capability( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_qam256_capability;
   uint32_t qam256_capability;
   memset(&default_qam256_capability, 0, sizeof(default_qam256_capability));
   memset(&qam256_capability, 0, sizeof(qam256_capability));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_qam256_capability(handle, &qam256_capability);
      moca_set_qam256_capability_defaults(&default_qam256_capability, config_flags);

      if ((ret == 0) && (memcmp(&default_qam256_capability, &qam256_capability, sizeof(qam256_capability))!=0))
         moca_nv_print_qam256_capability(cli, &qam256_capability, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_otf_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_otf_en;
   uint32_t otf_en;
   memset(&default_otf_en, 0, sizeof(default_otf_en));
   memset(&otf_en, 0, sizeof(otf_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_otf_en(handle, &otf_en);
      moca_set_otf_en_defaults(&default_otf_en, config_flags);

      if ((ret == 0) && (memcmp(&default_otf_en, &otf_en, sizeof(otf_en))!=0))
         moca_nv_print_otf_en(cli, &otf_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_star_topology_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_star_topology_en;
   uint32_t star_topology_en;
   memset(&default_star_topology_en, 0, sizeof(default_star_topology_en));
   memset(&star_topology_en, 0, sizeof(star_topology_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_star_topology_en(handle, &star_topology_en);
      moca_set_star_topology_en_defaults(&default_star_topology_en, config_flags);

      if ((ret == 0) && (memcmp(&default_star_topology_en, &star_topology_en, sizeof(star_topology_en))!=0))
         moca_nv_print_star_topology_en(cli, &star_topology_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_ofdma_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_ofdma_en;
   uint32_t ofdma_en;
   memset(&default_ofdma_en, 0, sizeof(default_ofdma_en));
   memset(&ofdma_en, 0, sizeof(ofdma_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_ofdma_en(handle, &ofdma_en);
      moca_set_ofdma_en_defaults(&default_ofdma_en, config_flags);

      if ((ret == 0) && (memcmp(&default_ofdma_en, &ofdma_en, sizeof(ofdma_en))!=0))
         moca_nv_print_ofdma_en(cli, &ofdma_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_min_bw_alarm_threshold( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_min_bw_alarm_threshold;
   uint32_t min_bw_alarm_threshold;
   memset(&default_min_bw_alarm_threshold, 0, sizeof(default_min_bw_alarm_threshold));
   memset(&min_bw_alarm_threshold, 0, sizeof(min_bw_alarm_threshold));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_min_bw_alarm_threshold(handle, &min_bw_alarm_threshold);
      moca_set_min_bw_alarm_threshold_defaults(&default_min_bw_alarm_threshold, config_flags);

      if ((ret == 0) && (memcmp(&default_min_bw_alarm_threshold, &min_bw_alarm_threshold, sizeof(min_bw_alarm_threshold))!=0))
         moca_nv_print_min_bw_alarm_threshold(cli, &min_bw_alarm_threshold, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_target_phy_rate_qam128( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_target_phy_rate_qam128;
   uint32_t target_phy_rate_qam128;
   memset(&default_target_phy_rate_qam128, 0, sizeof(default_target_phy_rate_qam128));
   memset(&target_phy_rate_qam128, 0, sizeof(target_phy_rate_qam128));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_target_phy_rate_qam128(handle, &target_phy_rate_qam128);
      moca_set_target_phy_rate_qam128_defaults(&default_target_phy_rate_qam128, config_flags);

      if ((ret == 0) && (memcmp(&default_target_phy_rate_qam128, &target_phy_rate_qam128, sizeof(target_phy_rate_qam128))!=0))
         moca_nv_print_target_phy_rate_qam128(cli, &target_phy_rate_qam128, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_target_phy_rate_qam256( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_target_phy_rate_qam256;
   uint32_t target_phy_rate_qam256;
   memset(&default_target_phy_rate_qam256, 0, sizeof(default_target_phy_rate_qam256));
   memset(&target_phy_rate_qam256, 0, sizeof(target_phy_rate_qam256));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_target_phy_rate_qam256(handle, &target_phy_rate_qam256);
      moca_set_target_phy_rate_qam256_defaults(&default_target_phy_rate_qam256, config_flags);

      if ((ret == 0) && (memcmp(&default_target_phy_rate_qam256, &target_phy_rate_qam256, sizeof(target_phy_rate_qam256))!=0))
         moca_nv_print_target_phy_rate_qam256(cli, &target_phy_rate_qam256, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_sapm_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_sapm_en;
   uint32_t sapm_en;
   memset(&default_sapm_en, 0, sizeof(default_sapm_en));
   memset(&sapm_en, 0, sizeof(sapm_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_sapm_en(handle, &sapm_en);
      moca_set_sapm_en_defaults(&default_sapm_en, config_flags);

      if ((ret == 0) && (memcmp(&default_sapm_en, &sapm_en, sizeof(sapm_en))!=0))
         moca_nv_print_sapm_en(cli, &sapm_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_arpl_th_50( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   int32_t default_arpl_th_50;
   int32_t arpl_th_50;
   memset(&default_arpl_th_50, 0, sizeof(default_arpl_th_50));
   memset(&arpl_th_50, 0, sizeof(arpl_th_50));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_arpl_th_50(handle, &arpl_th_50);
      moca_set_arpl_th_50_defaults(&default_arpl_th_50, config_flags);

      if ((ret == 0) && (memcmp(&default_arpl_th_50, &arpl_th_50, sizeof(arpl_th_50))!=0))
         moca_nv_print_arpl_th_50(cli, &arpl_th_50, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rlapm_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_rlapm_en;
   uint32_t rlapm_en;
   memset(&default_rlapm_en, 0, sizeof(default_rlapm_en));
   memset(&rlapm_en, 0, sizeof(rlapm_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rlapm_en(handle, &rlapm_en);
      moca_set_rlapm_en_defaults(&default_rlapm_en, config_flags);

      if ((ret == 0) && (memcmp(&default_rlapm_en, &rlapm_en, sizeof(rlapm_en))!=0))
         moca_nv_print_rlapm_en(cli, &rlapm_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_freq_shift( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_freq_shift;
   uint32_t freq_shift;
   memset(&default_freq_shift, 0, sizeof(default_freq_shift));
   memset(&freq_shift, 0, sizeof(freq_shift));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_freq_shift(handle, &freq_shift);
      moca_set_freq_shift_defaults(&default_freq_shift, config_flags);

      if ((ret == 0) && (memcmp(&default_freq_shift, &freq_shift, sizeof(freq_shift))!=0))
         moca_nv_print_freq_shift(cli, &freq_shift, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_phy_rate( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_phy_rate;
   uint32_t max_phy_rate;
   memset(&default_max_phy_rate, 0, sizeof(default_max_phy_rate));
   memset(&max_phy_rate, 0, sizeof(max_phy_rate));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_phy_rate(handle, &max_phy_rate);
      moca_set_max_phy_rate_defaults(&default_max_phy_rate, config_flags);

      if ((ret == 0) && (memcmp(&default_max_phy_rate, &max_phy_rate, sizeof(max_phy_rate))!=0))
         moca_nv_print_max_phy_rate(cli, &max_phy_rate, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_bandwidth( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_bandwidth;
   uint32_t bandwidth;
   memset(&default_bandwidth, 0, sizeof(default_bandwidth));
   memset(&bandwidth, 0, sizeof(bandwidth));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_bandwidth(handle, &bandwidth);
      moca_set_bandwidth_defaults(&default_bandwidth, config_flags);

      if ((ret == 0) && (memcmp(&default_bandwidth, &bandwidth, sizeof(bandwidth))!=0))
         moca_nv_print_bandwidth(cli, &bandwidth, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_arpl_th_100( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   int32_t default_arpl_th_100;
   int32_t arpl_th_100;
   memset(&default_arpl_th_100, 0, sizeof(default_arpl_th_100));
   memset(&arpl_th_100, 0, sizeof(arpl_th_100));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_arpl_th_100(handle, &arpl_th_100);
      moca_set_arpl_th_100_defaults(&default_arpl_th_100, config_flags);

      if ((ret == 0) && (memcmp(&default_arpl_th_100, &arpl_th_100, sizeof(arpl_th_100))!=0))
         moca_nv_print_arpl_th_100(cli, &arpl_th_100, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_adc_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_adc_mode;
   uint32_t adc_mode;
   memset(&default_adc_mode, 0, sizeof(default_adc_mode));
   memset(&adc_mode, 0, sizeof(adc_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_adc_mode(handle, &adc_mode);
      moca_set_adc_mode_defaults(&default_adc_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_adc_mode, &adc_mode, sizeof(adc_mode))!=0))
         moca_nv_print_adc_mode(cli, &adc_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_phy_rate_turbo( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_phy_rate_turbo;
   uint32_t max_phy_rate_turbo;
   memset(&default_max_phy_rate_turbo, 0, sizeof(default_max_phy_rate_turbo));
   memset(&max_phy_rate_turbo, 0, sizeof(max_phy_rate_turbo));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_phy_rate_turbo(handle, &max_phy_rate_turbo);
      moca_set_max_phy_rate_turbo_defaults(&default_max_phy_rate_turbo, config_flags);

      if ((ret == 0) && (memcmp(&default_max_phy_rate_turbo, &max_phy_rate_turbo, sizeof(max_phy_rate_turbo))!=0))
         moca_nv_print_max_phy_rate_turbo(cli, &max_phy_rate_turbo, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_cp_const( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_cp_const;
   uint32_t cp_const;
   memset(&default_cp_const, 0, sizeof(default_cp_const));
   memset(&cp_const, 0, sizeof(cp_const));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_cp_const(handle, &cp_const);
      moca_set_cp_const_defaults(&default_cp_const, config_flags);

      if ((ret == 0) && (memcmp(&default_cp_const, &cp_const, sizeof(cp_const))!=0))
         moca_nv_print_cp_const(cli, &cp_const, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_preamble_uc_const( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_preamble_uc_const;
   uint32_t preamble_uc_const;
   memset(&default_preamble_uc_const, 0, sizeof(default_preamble_uc_const));
   memset(&preamble_uc_const, 0, sizeof(preamble_uc_const));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_preamble_uc_const(handle, &preamble_uc_const);
      moca_set_preamble_uc_const_defaults(&default_preamble_uc_const, config_flags);

      if ((ret == 0) && (memcmp(&default_preamble_uc_const, &preamble_uc_const, sizeof(preamble_uc_const))!=0))
         moca_nv_print_preamble_uc_const(cli, &preamble_uc_const, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_cp_margin_increase( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_cp_margin_increase;
   uint32_t cp_margin_increase;
   memset(&default_cp_margin_increase, 0, sizeof(default_cp_margin_increase));
   memset(&cp_margin_increase, 0, sizeof(cp_margin_increase));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_cp_margin_increase(handle, &cp_margin_increase);
      moca_set_cp_margin_increase_defaults(&default_cp_margin_increase, config_flags);

      if ((ret == 0) && (memcmp(&default_cp_margin_increase, &cp_margin_increase, sizeof(cp_margin_increase))!=0))
         moca_nv_print_cp_margin_increase(cli, &cp_margin_increase, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_ac_cc_shift( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_ac_cc_shift;
   uint32_t ac_cc_shift;
   memset(&default_ac_cc_shift, 0, sizeof(default_ac_cc_shift));
   memset(&ac_cc_shift, 0, sizeof(ac_cc_shift));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_ac_cc_shift(handle, &ac_cc_shift);
      moca_set_ac_cc_shift_defaults(&default_ac_cc_shift, config_flags);

      if ((ret == 0) && (memcmp(&default_ac_cc_shift, &ac_cc_shift, sizeof(ac_cc_shift))!=0))
         moca_nv_print_ac_cc_shift(cli, &ac_cc_shift, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_mfc_th_increase( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_mfc_th_increase;
   uint32_t mfc_th_increase;
   memset(&default_mfc_th_increase, 0, sizeof(default_mfc_th_increase));
   memset(&mfc_th_increase, 0, sizeof(mfc_th_increase));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_mfc_th_increase(handle, &mfc_th_increase);
      moca_set_mfc_th_increase_defaults(&default_mfc_th_increase, config_flags);

      if ((ret == 0) && (memcmp(&default_mfc_th_increase, &mfc_th_increase, sizeof(mfc_th_increase))!=0))
         moca_nv_print_mfc_th_increase(cli, &mfc_th_increase, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_agc_const_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_agc_const_en;
   uint32_t agc_const_en;
   memset(&default_agc_const_en, 0, sizeof(default_agc_const_en));
   memset(&agc_const_en, 0, sizeof(agc_const_en));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_agc_const_en(handle, &agc_const_en);
      moca_set_agc_const_en_defaults(&default_agc_const_en, config_flags);

      if ((ret == 0) && (memcmp(&default_agc_const_en, &agc_const_en, sizeof(agc_const_en))!=0))
         moca_nv_print_agc_const_en(cli, &agc_const_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_agc_const_address( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_agc_const_address;
   uint32_t agc_const_address;
   memset(&default_agc_const_address, 0, sizeof(default_agc_const_address));
   memset(&agc_const_address, 0, sizeof(agc_const_address));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_agc_const_address(handle, &agc_const_address);
      moca_set_agc_const_address_defaults(&default_agc_const_address, config_flags);

      if ((ret == 0) && (memcmp(&default_agc_const_address, &agc_const_address, sizeof(agc_const_address))!=0))
         moca_nv_print_agc_const_address(cli, &agc_const_address, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_min_bo_insert_2_bfm_lock( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_min_bo_insert_2_bfm_lock;
   uint32_t min_bo_insert_2_bfm_lock;
   memset(&default_min_bo_insert_2_bfm_lock, 0, sizeof(default_min_bo_insert_2_bfm_lock));
   memset(&min_bo_insert_2_bfm_lock, 0, sizeof(min_bo_insert_2_bfm_lock));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_min_bo_insert_2_bfm_lock(handle, &min_bo_insert_2_bfm_lock);
      moca_set_min_bo_insert_2_bfm_lock_defaults(&default_min_bo_insert_2_bfm_lock, config_flags);

      if ((ret == 0) && (memcmp(&default_min_bo_insert_2_bfm_lock, &min_bo_insert_2_bfm_lock, sizeof(min_bo_insert_2_bfm_lock))!=0))
         moca_nv_print_min_bo_insert_2_bfm_lock(cli, &min_bo_insert_2_bfm_lock, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_min_snr_avg_db_2_bfm_lock( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_min_snr_avg_db_2_bfm_lock;
   uint32_t min_snr_avg_db_2_bfm_lock;
   memset(&default_min_snr_avg_db_2_bfm_lock, 0, sizeof(default_min_snr_avg_db_2_bfm_lock));
   memset(&min_snr_avg_db_2_bfm_lock, 0, sizeof(min_snr_avg_db_2_bfm_lock));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_min_snr_avg_db_2_bfm_lock(handle, &min_snr_avg_db_2_bfm_lock);
      moca_set_min_snr_avg_db_2_bfm_lock_defaults(&default_min_snr_avg_db_2_bfm_lock, config_flags);

      if ((ret == 0) && (memcmp(&default_min_snr_avg_db_2_bfm_lock, &min_snr_avg_db_2_bfm_lock, sizeof(min_snr_avg_db_2_bfm_lock))!=0))
         moca_nv_print_min_snr_avg_db_2_bfm_lock(cli, &min_snr_avg_db_2_bfm_lock, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_phy_rate_50M( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_phy_rate_50M;
   uint32_t max_phy_rate_50M;
   memset(&default_max_phy_rate_50M, 0, sizeof(default_max_phy_rate_50M));
   memset(&max_phy_rate_50M, 0, sizeof(max_phy_rate_50M));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_phy_rate_50M(handle, &max_phy_rate_50M);
      moca_set_max_phy_rate_50M_defaults(&default_max_phy_rate_50M, config_flags);

      if ((ret == 0) && (memcmp(&default_max_phy_rate_50M, &max_phy_rate_50M, sizeof(max_phy_rate_50M))!=0))
         moca_nv_print_max_phy_rate_50M(cli, &max_phy_rate_50M, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_constellation_all( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_constellation_all;
   uint32_t max_constellation_all;
   memset(&default_max_constellation_all, 0, sizeof(default_max_constellation_all));
   memset(&max_constellation_all, 0, sizeof(max_constellation_all));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_constellation_all(handle, &max_constellation_all);
      moca_set_max_constellation_all_defaults(&default_max_constellation_all, config_flags);

      if ((ret == 0) && (memcmp(&default_max_constellation_all, &max_constellation_all, sizeof(max_constellation_all))!=0))
         moca_nv_print_max_constellation_all(cli, &max_constellation_all, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_constellation( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t node_id;
   struct moca_max_constellation default_max_constellation;
   struct moca_max_constellation max_constellation;
   memset(&default_max_constellation, 0, sizeof(default_max_constellation));
   memset(&max_constellation, 0, sizeof(max_constellation));
   (void)ret; //eliminate unused variable warning

   for (node_id = MOCA_MAX_CONSTELLATION_NODE_ID_MIN; node_id <= MOCA_MAX_CONSTELLATION_NODE_ID_MAX; node_id++) {
      ret = moca_get_max_constellation(handle, node_id, &max_constellation);
      moca_set_max_constellation_defaults(&default_max_constellation, config_flags);

      default_max_constellation.node_id = node_id;
      max_constellation.node_id = node_id;
      if ((ret == 0) && (memcmp(&default_max_constellation, &max_constellation, sizeof(max_constellation))!=0))
         moca_nv_print_max_constellation(cli, &max_constellation, max_len);

      cli += strlen(cli);
   }
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rlapm_table_50( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_rlapm_table_50 default_rlapm_table_50;
   struct moca_rlapm_table_50 rlapm_table_50;
   memset(&default_rlapm_table_50, 0, sizeof(default_rlapm_table_50));
   memset(&rlapm_table_50, 0, sizeof(rlapm_table_50));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rlapm_table_50(handle, &rlapm_table_50);
      moca_set_rlapm_table_50_defaults(&default_rlapm_table_50, config_flags);

      if ((ret == 0) && (memcmp(&default_rlapm_table_50, &rlapm_table_50, sizeof(rlapm_table_50))!=0))
         moca_nv_print_rlapm_table_50(cli, &rlapm_table_50, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rlapm_table_100( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_rlapm_table_100 default_rlapm_table_100;
   struct moca_rlapm_table_100 rlapm_table_100;
   memset(&default_rlapm_table_100, 0, sizeof(default_rlapm_table_100));
   memset(&rlapm_table_100, 0, sizeof(rlapm_table_100));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rlapm_table_100(handle, &rlapm_table_100);
      moca_set_rlapm_table_100_defaults(&default_rlapm_table_100, config_flags);

      if ((ret == 0) && (memcmp(&default_rlapm_table_100, &rlapm_table_100, sizeof(rlapm_table_100))!=0))
         moca_nv_print_rlapm_table_100(cli, &rlapm_table_100, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_nv_cal_enable( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_nv_cal_enable;
   uint32_t nv_cal_enable;
   memset(&default_nv_cal_enable, 0, sizeof(default_nv_cal_enable));
   memset(&nv_cal_enable, 0, sizeof(nv_cal_enable));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_nv_cal_enable(handle, &nv_cal_enable);
      moca_set_nv_cal_enable_defaults(&default_nv_cal_enable, config_flags);

      if ((ret == 0) && (memcmp(&default_nv_cal_enable, &nv_cal_enable, sizeof(nv_cal_enable))!=0))
         moca_nv_print_nv_cal_enable(cli, &nv_cal_enable, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rlapm_cap_50( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_rlapm_cap_50;
   uint32_t rlapm_cap_50;
   memset(&default_rlapm_cap_50, 0, sizeof(default_rlapm_cap_50));
   memset(&rlapm_cap_50, 0, sizeof(rlapm_cap_50));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rlapm_cap_50(handle, &rlapm_cap_50);
      moca_set_rlapm_cap_50_defaults(&default_rlapm_cap_50, config_flags);

      if ((ret == 0) && (memcmp(&default_rlapm_cap_50, &rlapm_cap_50, sizeof(rlapm_cap_50))!=0))
         moca_nv_print_rlapm_cap_50(cli, &rlapm_cap_50, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_snr_margin_rs( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_snr_margin_rs default_snr_margin_rs;
   struct moca_snr_margin_rs snr_margin_rs;
   memset(&default_snr_margin_rs, 0, sizeof(default_snr_margin_rs));
   memset(&snr_margin_rs, 0, sizeof(snr_margin_rs));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_snr_margin_rs(handle, &snr_margin_rs);
      moca_set_snr_margin_rs_defaults(&default_snr_margin_rs, config_flags);

      if ((ret == 0) && (memcmp(&default_snr_margin_rs, &snr_margin_rs, sizeof(snr_margin_rs))!=0))
         moca_nv_print_snr_margin_rs(cli, &snr_margin_rs, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_snr_margin_ldpc( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_snr_margin_ldpc default_snr_margin_ldpc;
   struct moca_snr_margin_ldpc snr_margin_ldpc;
   memset(&default_snr_margin_ldpc, 0, sizeof(default_snr_margin_ldpc));
   memset(&snr_margin_ldpc, 0, sizeof(snr_margin_ldpc));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_snr_margin_ldpc(handle, &snr_margin_ldpc);
      moca_set_snr_margin_ldpc_defaults(&default_snr_margin_ldpc, config_flags);

      if ((ret == 0) && (memcmp(&default_snr_margin_ldpc, &snr_margin_ldpc, sizeof(snr_margin_ldpc))!=0))
         moca_nv_print_snr_margin_ldpc(cli, &snr_margin_ldpc, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_snr_margin_ldpc_sec_ch( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_snr_margin_ldpc_sec_ch default_snr_margin_ldpc_sec_ch;
   struct moca_snr_margin_ldpc_sec_ch snr_margin_ldpc_sec_ch;
   memset(&default_snr_margin_ldpc_sec_ch, 0, sizeof(default_snr_margin_ldpc_sec_ch));
   memset(&snr_margin_ldpc_sec_ch, 0, sizeof(snr_margin_ldpc_sec_ch));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_snr_margin_ldpc_sec_ch(handle, &snr_margin_ldpc_sec_ch);
      moca_set_snr_margin_ldpc_sec_ch_defaults(&default_snr_margin_ldpc_sec_ch, config_flags);

      if ((ret == 0) && (memcmp(&default_snr_margin_ldpc_sec_ch, &snr_margin_ldpc_sec_ch, sizeof(snr_margin_ldpc_sec_ch))!=0))
         moca_nv_print_snr_margin_ldpc_sec_ch(cli, &snr_margin_ldpc_sec_ch, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_snr_margin_ldpc_pre5( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_snr_margin_ldpc_pre5 default_snr_margin_ldpc_pre5;
   struct moca_snr_margin_ldpc_pre5 snr_margin_ldpc_pre5;
   memset(&default_snr_margin_ldpc_pre5, 0, sizeof(default_snr_margin_ldpc_pre5));
   memset(&snr_margin_ldpc_pre5, 0, sizeof(snr_margin_ldpc_pre5));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_snr_margin_ldpc_pre5(handle, &snr_margin_ldpc_pre5);
      moca_set_snr_margin_ldpc_pre5_defaults(&default_snr_margin_ldpc_pre5, config_flags);

      if ((ret == 0) && (memcmp(&default_snr_margin_ldpc_pre5, &snr_margin_ldpc_pre5, sizeof(snr_margin_ldpc_pre5))!=0))
         moca_nv_print_snr_margin_ldpc_pre5(cli, &snr_margin_ldpc_pre5, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_snr_margin_ofdma( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_snr_margin_ofdma default_snr_margin_ofdma;
   struct moca_snr_margin_ofdma snr_margin_ofdma;
   memset(&default_snr_margin_ofdma, 0, sizeof(default_snr_margin_ofdma));
   memset(&snr_margin_ofdma, 0, sizeof(snr_margin_ofdma));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_snr_margin_ofdma(handle, &snr_margin_ofdma);
      moca_set_snr_margin_ofdma_defaults(&default_snr_margin_ofdma, config_flags);

      if ((ret == 0) && (memcmp(&default_snr_margin_ofdma, &snr_margin_ofdma, sizeof(snr_margin_ofdma))!=0))
         moca_nv_print_snr_margin_ofdma(cli, &snr_margin_ofdma, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rlapm_cap_100( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_rlapm_cap_100;
   uint32_t rlapm_cap_100;
   memset(&default_rlapm_cap_100, 0, sizeof(default_rlapm_cap_100));
   memset(&rlapm_cap_100, 0, sizeof(rlapm_cap_100));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rlapm_cap_100(handle, &rlapm_cap_100);
      moca_set_rlapm_cap_100_defaults(&default_rlapm_cap_100, config_flags);

      if ((ret == 0) && (memcmp(&default_rlapm_cap_100, &rlapm_cap_100, sizeof(rlapm_cap_100))!=0))
         moca_nv_print_rlapm_cap_100(cli, &rlapm_cap_100, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_sapm_table_50( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_sapm_table_50 default_sapm_table_50;
   struct moca_sapm_table_50 sapm_table_50;
   memset(&default_sapm_table_50, 0, sizeof(default_sapm_table_50));
   memset(&sapm_table_50, 0, sizeof(sapm_table_50));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_sapm_table_50(handle, &sapm_table_50);
      moca_set_sapm_table_50_defaults(&default_sapm_table_50, config_flags);

      if ((ret == 0) && (memcmp(&default_sapm_table_50, &sapm_table_50, sizeof(sapm_table_50))!=0))
         moca_nv_print_sapm_table_50(cli, &sapm_table_50, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_sapm_table_100( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_sapm_table_100 default_sapm_table_100;
   struct moca_sapm_table_100 sapm_table_100;
   memset(&default_sapm_table_100, 0, sizeof(default_sapm_table_100));
   memset(&sapm_table_100, 0, sizeof(sapm_table_100));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_sapm_table_100(handle, &sapm_table_100);
      moca_set_sapm_table_100_defaults(&default_sapm_table_100, config_flags);

      if ((ret == 0) && (memcmp(&default_sapm_table_100, &sapm_table_100, sizeof(sapm_table_100))!=0))
         moca_nv_print_sapm_table_100(cli, &sapm_table_100, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_sapm_table_sec( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_sapm_table_sec default_sapm_table_sec;
   struct moca_sapm_table_sec sapm_table_sec;
   memset(&default_sapm_table_sec, 0, sizeof(default_sapm_table_sec));
   memset(&sapm_table_sec, 0, sizeof(sapm_table_sec));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_sapm_table_sec(handle, &sapm_table_sec);
      moca_set_sapm_table_sec_defaults(&default_sapm_table_sec, config_flags);

      if ((ret == 0) && (memcmp(&default_sapm_table_sec, &sapm_table_sec, sizeof(sapm_table_sec))!=0))
         moca_nv_print_sapm_table_sec(cli, &sapm_table_sec, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_snr_margin_ldpc_pri_ch( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_snr_margin_ldpc_pri_ch default_snr_margin_ldpc_pri_ch;
   struct moca_snr_margin_ldpc_pri_ch snr_margin_ldpc_pri_ch;
   memset(&default_snr_margin_ldpc_pri_ch, 0, sizeof(default_snr_margin_ldpc_pri_ch));
   memset(&snr_margin_ldpc_pri_ch, 0, sizeof(snr_margin_ldpc_pri_ch));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_snr_margin_ldpc_pri_ch(handle, &snr_margin_ldpc_pri_ch);
      moca_set_snr_margin_ldpc_pri_ch_defaults(&default_snr_margin_ldpc_pri_ch, config_flags);

      if ((ret == 0) && (memcmp(&default_snr_margin_ldpc_pri_ch, &snr_margin_ldpc_pri_ch, sizeof(snr_margin_ldpc_pri_ch))!=0))
         moca_nv_print_snr_margin_ldpc_pri_ch(cli, &snr_margin_ldpc_pri_ch, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_snr_margin_pre5_pri_ch( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_snr_margin_pre5_pri_ch default_snr_margin_pre5_pri_ch;
   struct moca_snr_margin_pre5_pri_ch snr_margin_pre5_pri_ch;
   memset(&default_snr_margin_pre5_pri_ch, 0, sizeof(default_snr_margin_pre5_pri_ch));
   memset(&snr_margin_pre5_pri_ch, 0, sizeof(snr_margin_pre5_pri_ch));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_snr_margin_pre5_pri_ch(handle, &snr_margin_pre5_pri_ch);
      moca_set_snr_margin_pre5_pri_ch_defaults(&default_snr_margin_pre5_pri_ch, config_flags);

      if ((ret == 0) && (memcmp(&default_snr_margin_pre5_pri_ch, &snr_margin_pre5_pri_ch, sizeof(snr_margin_pre5_pri_ch))!=0))
         moca_nv_print_snr_margin_pre5_pri_ch(cli, &snr_margin_pre5_pri_ch, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_snr_margin_pre5_sec_ch( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_snr_margin_pre5_sec_ch default_snr_margin_pre5_sec_ch;
   struct moca_snr_margin_pre5_sec_ch snr_margin_pre5_sec_ch;
   memset(&default_snr_margin_pre5_sec_ch, 0, sizeof(default_snr_margin_pre5_sec_ch));
   memset(&snr_margin_pre5_sec_ch, 0, sizeof(snr_margin_pre5_sec_ch));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_snr_margin_pre5_sec_ch(handle, &snr_margin_pre5_sec_ch);
      moca_set_snr_margin_pre5_sec_ch_defaults(&default_snr_margin_pre5_sec_ch, config_flags);

      if ((ret == 0) && (memcmp(&default_snr_margin_pre5_sec_ch, &snr_margin_pre5_sec_ch, sizeof(snr_margin_pre5_sec_ch))!=0))
         moca_nv_print_snr_margin_pre5_sec_ch(cli, &snr_margin_pre5_sec_ch, max_len);
}

// Group mac_layer
MOCALIB_GEN_GET_FUNCTION int moca_get_max_frame_size(void *vctx, uint32_t *bytes)
{
   return(moca_get(vctx, IE_MAX_FRAME_SIZE, bytes, sizeof(*bytes)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_frame_size(void *vctx, uint32_t bytes)
{
   return(moca_set(vctx, IE_MAX_FRAME_SIZE, &bytes, sizeof(bytes)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_frame_size (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_frame_size ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_min_aggr_waiting_time(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MIN_AGGR_WAITING_TIME, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_min_aggr_waiting_time(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MIN_AGGR_WAITING_TIME, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_min_aggr_waiting_time (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--min_aggr_waiting_time ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_selective_rr(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_SELECTIVE_RR, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_selective_rr(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_SELECTIVE_RR, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_selective_rr (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--selective_rr ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_fragmentation(void *vctx, uint32_t *bool_val)
{
   return(moca_get(vctx, IE_FRAGMENTATION, bool_val, sizeof(*bool_val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_fragmentation(void *vctx, uint32_t bool_val)
{
   return(moca_set(vctx, IE_FRAGMENTATION, &bool_val, sizeof(bool_val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_fragmentation (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--fragmentation ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_ifg_moca20(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_IFG_MOCA20, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_ifg_moca20(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_IFG_MOCA20, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ifg_moca20 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ifg_moca20 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_map_seed(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MAP_SEED, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_map_seed(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MAP_SEED, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_map_seed (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--map_seed ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_transmit_time(void *vctx, uint32_t *usec)
{
   return(moca_get(vctx, IE_MAX_TRANSMIT_TIME, usec, sizeof(*usec)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_transmit_time(void *vctx, uint32_t usec)
{
   return(moca_set(vctx, IE_MAX_TRANSMIT_TIME, &usec, sizeof(usec)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_transmit_time (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_transmit_time ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_pkt_aggr(void *vctx, uint32_t *pkts)
{
   return(moca_get(vctx, IE_MAX_PKT_AGGR, pkts, sizeof(*pkts)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_pkt_aggr(void *vctx, uint32_t pkts)
{
   return(moca_set(vctx, IE_MAX_PKT_AGGR, &pkts, sizeof(pkts)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_pkt_aggr (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_pkt_aggr ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_probe_request(void *vctx, struct moca_probe_request *in)
{
   struct moca_probe_request tmp = *in;
   moca_probe_request_swap(&tmp);
   return(moca_set_noswap(vctx, IE_PROBE_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_probe_request (char *cli, struct moca_probe_request * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--probe_request ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "probe_type %u  ", (unsigned int) in->probe_type);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "timeslots %u  ", (unsigned int) in->timeslots);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rtr_config(void *vctx, struct moca_rtr_config *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_RTR_CONFIG, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rtr_config(void *vctx, struct moca_rtr_config *in)
{
   struct moca_rtr_config tmp = *in;
   return(moca_set_noswap(vctx, IE_RTR_CONFIG, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rtr_config (char *cli, struct moca_rtr_config * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--rtr_config ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "low %u  ", (unsigned int) in->low);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "med %u  ", (unsigned int) in->med);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "high %u  ", (unsigned int) in->high);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "bg %u  ", (unsigned int) in->bg);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_tlp_mode(void *vctx, uint32_t *mode)
{
   return(moca_get(vctx, IE_TLP_MODE, mode, sizeof(*mode)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_tlp_mode(void *vctx, uint32_t mode)
{
   return(moca_set(vctx, IE_TLP_MODE, &mode, sizeof(mode)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_tlp_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--tlp_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_max_pkt_aggr_bonding(void *vctx, uint32_t *pkts)
{
   return(moca_get(vctx, IE_MAX_PKT_AGGR_BONDING, pkts, sizeof(*pkts)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_max_pkt_aggr_bonding(void *vctx, uint32_t pkts)
{
   return(moca_set(vctx, IE_MAX_PKT_AGGR_BONDING, &pkts, sizeof(pkts)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_max_pkt_aggr_bonding (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--max_pkt_aggr_bonding ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_frame_size( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_frame_size;
   uint32_t max_frame_size;
   memset(&default_max_frame_size, 0, sizeof(default_max_frame_size));
   memset(&max_frame_size, 0, sizeof(max_frame_size));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_frame_size(handle, &max_frame_size);
      moca_set_max_frame_size_defaults(&default_max_frame_size, config_flags);

      if ((ret == 0) && (memcmp(&default_max_frame_size, &max_frame_size, sizeof(max_frame_size))!=0))
         moca_nv_print_max_frame_size(cli, &max_frame_size, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_selective_rr( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_selective_rr;
   uint32_t selective_rr;
   memset(&default_selective_rr, 0, sizeof(default_selective_rr));
   memset(&selective_rr, 0, sizeof(selective_rr));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_selective_rr(handle, &selective_rr);
      moca_set_selective_rr_defaults(&default_selective_rr, config_flags);

      if ((ret == 0) && (memcmp(&default_selective_rr, &selective_rr, sizeof(selective_rr))!=0))
         moca_nv_print_selective_rr(cli, &selective_rr, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_fragmentation( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_fragmentation;
   uint32_t fragmentation;
   memset(&default_fragmentation, 0, sizeof(default_fragmentation));
   memset(&fragmentation, 0, sizeof(fragmentation));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_fragmentation(handle, &fragmentation);
      moca_set_fragmentation_defaults(&default_fragmentation, config_flags);

      if ((ret == 0) && (memcmp(&default_fragmentation, &fragmentation, sizeof(fragmentation))!=0))
         moca_nv_print_fragmentation(cli, &fragmentation, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_ifg_moca20( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_ifg_moca20;
   uint32_t ifg_moca20;
   memset(&default_ifg_moca20, 0, sizeof(default_ifg_moca20));
   memset(&ifg_moca20, 0, sizeof(ifg_moca20));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_ifg_moca20(handle, &ifg_moca20);
      moca_set_ifg_moca20_defaults(&default_ifg_moca20, config_flags);

      if ((ret == 0) && (memcmp(&default_ifg_moca20, &ifg_moca20, sizeof(ifg_moca20))!=0))
         moca_nv_print_ifg_moca20(cli, &ifg_moca20, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_map_seed( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_map_seed;
   uint32_t map_seed;
   memset(&default_map_seed, 0, sizeof(default_map_seed));
   memset(&map_seed, 0, sizeof(map_seed));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_map_seed(handle, &map_seed);
      moca_set_map_seed_defaults(&default_map_seed, config_flags);

      if ((ret == 0) && (memcmp(&default_map_seed, &map_seed, sizeof(map_seed))!=0))
         moca_nv_print_map_seed(cli, &map_seed, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_transmit_time( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_transmit_time;
   uint32_t max_transmit_time;
   memset(&default_max_transmit_time, 0, sizeof(default_max_transmit_time));
   memset(&max_transmit_time, 0, sizeof(max_transmit_time));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_transmit_time(handle, &max_transmit_time);
      moca_set_max_transmit_time_defaults(&default_max_transmit_time, config_flags);

      if ((ret == 0) && (memcmp(&default_max_transmit_time, &max_transmit_time, sizeof(max_transmit_time))!=0))
         moca_nv_print_max_transmit_time(cli, &max_transmit_time, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_pkt_aggr( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_pkt_aggr;
   uint32_t max_pkt_aggr;
   memset(&default_max_pkt_aggr, 0, sizeof(default_max_pkt_aggr));
   memset(&max_pkt_aggr, 0, sizeof(max_pkt_aggr));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_pkt_aggr(handle, &max_pkt_aggr);
      moca_set_max_pkt_aggr_defaults(&default_max_pkt_aggr, config_flags);

      if ((ret == 0) && (memcmp(&default_max_pkt_aggr, &max_pkt_aggr, sizeof(max_pkt_aggr))!=0))
         moca_nv_print_max_pkt_aggr(cli, &max_pkt_aggr, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rtr_config( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_rtr_config default_rtr_config;
   struct moca_rtr_config rtr_config;
   memset(&default_rtr_config, 0, sizeof(default_rtr_config));
   memset(&rtr_config, 0, sizeof(rtr_config));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rtr_config(handle, &rtr_config);
      moca_set_rtr_config_defaults(&default_rtr_config, config_flags);

      if ((ret == 0) && (memcmp(&default_rtr_config, &rtr_config, sizeof(rtr_config))!=0))
         moca_nv_print_rtr_config(cli, &rtr_config, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_tlp_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_tlp_mode;
   uint32_t tlp_mode;
   memset(&default_tlp_mode, 0, sizeof(default_tlp_mode));
   memset(&tlp_mode, 0, sizeof(tlp_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_tlp_mode(handle, &tlp_mode);
      moca_set_tlp_mode_defaults(&default_tlp_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_tlp_mode, &tlp_mode, sizeof(tlp_mode))!=0))
         moca_nv_print_tlp_mode(cli, &tlp_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_max_pkt_aggr_bonding( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_max_pkt_aggr_bonding;
   uint32_t max_pkt_aggr_bonding;
   memset(&default_max_pkt_aggr_bonding, 0, sizeof(default_max_pkt_aggr_bonding));
   memset(&max_pkt_aggr_bonding, 0, sizeof(max_pkt_aggr_bonding));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_max_pkt_aggr_bonding(handle, &max_pkt_aggr_bonding);
      moca_set_max_pkt_aggr_bonding_defaults(&default_max_pkt_aggr_bonding, config_flags);

      if ((ret == 0) && (memcmp(&default_max_pkt_aggr_bonding, &max_pkt_aggr_bonding, sizeof(max_pkt_aggr_bonding))!=0))
         moca_nv_print_max_pkt_aggr_bonding(cli, &max_pkt_aggr_bonding, max_len);
}

// Group forwarding
MOCALIB_GEN_GET_FUNCTION int moca_get_multicast_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MULTICAST_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_multicast_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MULTICAST_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_multicast_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--multicast_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_low_pri_q_num(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_LOW_PRI_Q_NUM, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_low_pri_q_num(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_LOW_PRI_Q_NUM, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_low_pri_q_num (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--low_pri_q_num ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_egr_mc_filter_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_EGR_MC_FILTER_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_egr_mc_filter_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_EGR_MC_FILTER_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_egr_mc_filter_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--egr_mc_filter_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_fc_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_FC_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_fc_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_FC_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_fc_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--fc_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_pqos_max_packet_size(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PQOS_MAX_PACKET_SIZE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_pqos_max_packet_size(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PQOS_MAX_PACKET_SIZE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_pqos_max_packet_size (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--pqos_max_packet_size ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_per_mode(void *vctx, uint32_t *mode)
{
   return(moca_get(vctx, IE_PER_MODE, mode, sizeof(*mode)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_per_mode(void *vctx, uint32_t mode)
{
   return(moca_set(vctx, IE_PER_MODE, &mode, sizeof(mode)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_per_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--per_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_policing_en(void *vctx, uint32_t *enable)
{
   return(moca_get(vctx, IE_POLICING_EN, enable, sizeof(*enable)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_policing_en(void *vctx, uint32_t enable)
{
   return(moca_set(vctx, IE_POLICING_EN, &enable, sizeof(enable)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_policing_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--policing_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_pqos_egress_numflows(void *vctx, uint32_t *pqos_egress_numflows)
{
   return(moca_get(vctx, IE_PQOS_EGRESS_NUMFLOWS, pqos_egress_numflows, sizeof(*pqos_egress_numflows)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_orr_en(void *vctx, uint32_t *enable)
{
   return(moca_get(vctx, IE_ORR_EN, enable, sizeof(*enable)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_orr_en(void *vctx, uint32_t enable)
{
   return(moca_set(vctx, IE_ORR_EN, &enable, sizeof(enable)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_orr_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--orr_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_brcmtag_enable(void *vctx, uint32_t *enable)
{
   return(moca_get(vctx, IE_BRCMTAG_ENABLE, enable, sizeof(*enable)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_brcmtag_enable(void *vctx, uint32_t enable)
{
   return(moca_set(vctx, IE_BRCMTAG_ENABLE, &enable, sizeof(enable)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_brcmtag_enable (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--brcmtag_enable ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_priority_allocations(void *vctx, struct moca_priority_allocations *out)
{
   int ret;
   ret = moca_get(vctx, IE_PRIORITY_ALLOCATIONS, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_priority_allocations(void *vctx, const struct moca_priority_allocations *in)
{
   return(moca_set(vctx, IE_PRIORITY_ALLOCATIONS, in, sizeof(*in)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_priority_allocations (char *cli, struct moca_priority_allocations * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--priority_allocations ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "reservation_pqos %u  ", (unsigned int) in->reservation_pqos);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "reservation_high %u  ", (unsigned int) in->reservation_high);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "reservation_med %u  ", (unsigned int) in->reservation_med);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "reservation_low %u  ", (unsigned int) in->reservation_low);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "limitation_pqos %u  ", (unsigned int) in->limitation_pqos);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "limitation_high %u  ", (unsigned int) in->limitation_high);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "limitation_med %u  ", (unsigned int) in->limitation_med);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "limitation_low %u  ", (unsigned int) in->limitation_low);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_egr_mc_addr_filter(void *vctx, uint32_t entryid, struct moca_egr_mc_addr_filter *out)
{
   int ret;
   ret = moca_get_inout_noswap(vctx, IE_EGR_MC_ADDR_FILTER, &entryid, sizeof(entryid), out, sizeof(*out));
   moca_egr_mc_addr_filter_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_egr_mc_addr_filter(void *vctx, struct moca_egr_mc_addr_filter_set *in)
{
   struct moca_egr_mc_addr_filter_set_int tmp;
   tmp.entryid = in->entryid;
   tmp.valid = in->valid;
   tmp.addr = in->addr;
   tmp.reserved_0 = 0;
   moca_egr_mc_addr_filter_set_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_EGR_MC_ADDR_FILTER, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_create_request(void *vctx, struct moca_pqos_create_request *in)
{
   struct moca_pqos_create_request_int tmp;
   tmp.hdrFmt = 0x8;
   tmp.reserved_0 = 0;
   tmp.vendorId = 0;
   tmp.transType = 0x1;
   tmp.transSubtype = 0x1;
   tmp.wave0Nodemask = in->wave0Nodemask;
   tmp.reserved_1 = 0;
   tmp.reserved_2 = 0;
   tmp.msgPriority = 0xf0;
   tmp.txnLastWaveNum = 2;
   tmp.reserved_3 = 0;
   tmp.flowid = in->flowid;
   tmp.reserved_4 = 0;
   tmp.tPacketSize = in->tPacketSize;
   tmp.reserved_5 = 0;
   tmp.ingressNodeId = in->ingressNodeId;
   tmp.max_latency = in->max_latency;
   tmp.short_term_avg_ratio = in->short_term_avg_ratio;
   tmp.reserved_6 = 0;
   tmp.egressNodeId = in->egressNodeId;
   tmp.flowTag = in->flowTag;
   tmp.packetda = in->packetda;
   tmp.reserved_7 = 0;
   tmp.tPeakDataRate = in->tPeakDataRate;
   tmp.tLeaseTime = in->tLeaseTime;
   tmp.tBurstSize = in->tBurstSize;
   tmp.retry_per_delivery = in->retry_per_delivery;
   tmp.proto_rule_vlan_dscp = in->proto_rule_vlan_dscp;
   moca_pqos_create_request_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_PQOS_CREATE_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_ingr_add_flow(void *vctx, struct moca_pqos_ingr_add_flow *in)
{
   struct moca_pqos_ingr_add_flow_int tmp;
   tmp.flowid = in->flowid;
   tmp.reserved_0 = 0;
   tmp.flowtag = in->flowtag;
   tmp.qtag = in->qtag;
   tmp.tpeakdatarate = in->tpeakdatarate;
   tmp.tpacketsize = in->tpacketsize;
   tmp.tburstsize = in->tburstsize;
   tmp.tleasetime = in->tleasetime;
   tmp.egressnodeid = in->egressnodeid;
   tmp.flowsa = in->flowsa;
   tmp.flowda = in->flowda;
   tmp.flowvlanid = in->flowvlanid;
   tmp.committedstps = in->committedstps;
   tmp.committedtxps = in->committedtxps;
   tmp.dest_flow_id = in->dest_flow_id;
   tmp.maximum_latency = in->maximum_latency;
   tmp.short_term_avg_ratio = in->short_term_avg_ratio;
   tmp.max_number_retry = in->max_number_retry;
   tmp.flow_per = in->flow_per;
   tmp.in_order_delivery = in->in_order_delivery;
   tmp.ingr_class_rule = in->ingr_class_rule;
   tmp.traffic_protocol = in->traffic_protocol;
   tmp.vlan_tag = in->vlan_tag;
   tmp.dscp_moca = in->dscp_moca;
   moca_pqos_ingr_add_flow_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_PQOS_INGR_ADD_FLOW, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_update_request(void *vctx, struct moca_pqos_update_request *in)
{
   struct moca_pqos_update_request_int tmp;
   tmp.hdrFmt = 0x8;
   tmp.reserved_0 = 0;
   tmp.vendorId = 0;
   tmp.transType = 0x1;
   tmp.transSubtype = 0x2;
   tmp.wave0Nodemask = in->wave0Nodemask;
   tmp.reserved_1 = 0;
   tmp.reserved_2 = 0;
   tmp.msgPriority = 0xf0;
   tmp.txnLastWaveNum = 2;
   tmp.reserved_3 = 0;
   tmp.flowid = in->flowid;
   tmp.reserved_4 = 0;
   tmp.tPacketSize = in->tPacketSize;
   tmp.reserved_5 = 0;
   tmp.ingressNodeId = in->ingressNodeId;
   tmp.max_latency = in->max_latency;
   tmp.short_term_avg_ratio = in->short_term_avg_ratio;
   tmp.reserved_6 = 0;
   tmp.egressNodeId = in->egressNodeId;
   tmp.flowTag = in->flowTag;
   tmp.packetda = in->packetda;
   tmp.reserved_7 = 0;
   tmp.tPeakDataRate = in->tPeakDataRate;
   tmp.tLeaseTime = in->tLeaseTime;
   tmp.tBurstSize = in->tBurstSize;
   tmp.retry_per_delivery = in->retry_per_delivery;
   tmp.proto_rule_vlan_dscp = in->proto_rule_vlan_dscp;
   moca_pqos_update_request_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_PQOS_UPDATE_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_ingr_update(void *vctx, struct moca_pqos_ingr_update *in)
{
   struct moca_pqos_ingr_update_int tmp;
   tmp.flowid = in->flowid;
   tmp.reserved_0 = 0;
   tmp.flowtag = in->flowtag;
   tmp.flowda = in->flowda;
   tmp.reserved_1 = 0;
   tmp.tpeakdatarate = in->tpeakdatarate;
   tmp.tpacketsize = in->tpacketsize;
   tmp.tburstsize = in->tburstsize;
   tmp.tleasetime = in->tleasetime;
   tmp.committedstps = in->committedstps;
   tmp.committedtxps = in->committedtxps;
   tmp.maximum_latency = in->maximum_latency;
   tmp.short_term_avg_ratio = in->short_term_avg_ratio;
   tmp.max_number_retry = in->max_number_retry;
   tmp.flow_per = in->flow_per;
   tmp.in_order_delivery = in->in_order_delivery;
   tmp.ingr_class_rule = in->ingr_class_rule;
   tmp.traffic_protocol = in->traffic_protocol;
   tmp.vlan_tag = in->vlan_tag;
   tmp.dscp_moca = in->dscp_moca;
   moca_pqos_ingr_update_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_PQOS_INGR_UPDATE, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_delete_request(void *vctx, struct moca_pqos_delete_request *in)
{
   struct moca_pqos_delete_request_int tmp;
   tmp.hdrFmt = 0x8;
   tmp.reserved_0 = 0;
   tmp.vendorId = 0;
   tmp.transType = 0x1;
   tmp.transSubtype = 0x3;
   tmp.wave0Nodemask = in->wave0Nodemask;
   tmp.reserved_1 = 0;
   tmp.reserved_2 = 0;
   tmp.msgPriority = 0xf0;
   tmp.txnLastWaveNum = 2;
   tmp.reserved_3 = 0;
   tmp.flowid = in->flowid;
   tmp.reserved_4 = 0;
   tmp.reserved_5 = 0;
   moca_pqos_delete_request_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_PQOS_DELETE_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_ingr_delete(void *vctx, struct moca_pqos_ingr_delete *in)
{
   struct moca_pqos_ingr_delete_int tmp;
   tmp.flowid = in->flowid;
   tmp.reserved_0 = 0;
   return(moca_set_noswap(vctx, IE_PQOS_INGR_DELETE, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_list_request(void *vctx, struct moca_pqos_list_request *in)
{
   struct moca_pqos_list_request_int tmp;
   int i;
   tmp.hdrFmt = 0x8;
   tmp.reserved_0 = 0;
   tmp.vendorId = 0;
   tmp.transType = 0x1;
   tmp.transSubtype = 0x4;
   tmp.wave0Nodemask = in->wave0Nodemask;
   tmp.reserved_1 = 0;
   tmp.reserved_2 = 0;
   tmp.msgPriority = 0x80;
   tmp.txnLastWaveNum = 1;
   tmp.reserved_3 = 0;
   tmp.flowStartIndex = in->flowStartIndex;
   tmp.flowMaxReturn = in->flowMaxReturn;
   for (i = 0; i < 3; i++)
      tmp.reserved_4[i] = 0;
   moca_pqos_list_request_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_PQOS_LIST_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_query_request(void *vctx, struct moca_pqos_query_request *in)
{
   struct moca_pqos_query_request_int tmp;
   tmp.hdrFmt = 0x8;
   tmp.reserved_0 = 0;
   tmp.vendorId = 0;
   tmp.transType = 0x1;
   tmp.transSubtype = 0x5;
   tmp.wave0Nodemask = in->wave0Nodemask;
   tmp.reserved_1 = 0;
   tmp.reserved_2 = 0;
   tmp.msgPriority = 0x80;
   tmp.txnLastWaveNum = 1;
   tmp.reserved_3 = 0;
   tmp.reserved_4 = 0;
   tmp.flowid = in->flowid;
   tmp.reserved_5 = 0;
   moca_pqos_query_request_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_PQOS_QUERY_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_pqos_maintenance_start(void *vctx)
{
   return(moca_set(vctx, IE_PQOS_MAINTENANCE_START, NULL, 0));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_uc_fwd(void *vctx, struct moca_uc_fwd *out, int max_out_len)
{
   int ret;
   int i;

   ret = moca_get_table(vctx, IE_UC_FWD, NULL, out, sizeof(*out), max_out_len, 0);

   for (i = 0; i < ret; i++)
   {
      moca_uc_fwd_swap(out);
      out++;
   }

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mc_fwd(void *vctx, struct moca_mc_fwd *out, int max_out_len)
{
   int ret;
   int i;

   ret = moca_get_table(vctx, IE_MC_FWD, NULL, out, sizeof(*out), max_out_len, 0);

   for (i = 0; i < ret; i++)
   {
      moca_mc_fwd_swap(out);
      out++;
   }

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mc_fwd(void *vctx, struct moca_mc_fwd_set *in)
{
   struct moca_mc_fwd_set_int tmp;
   tmp.multicast_mac_addr = in->multicast_mac_addr;
   tmp.dest_mac_addr1 = in->dest_mac_addr1;
   tmp.dest_mac_addr2 = in->dest_mac_addr2;
   tmp.dest_mac_addr3 = in->dest_mac_addr3;
   tmp.dest_mac_addr4 = in->dest_mac_addr4;
   tmp.reserved_0 = 0;
   return(moca_set_noswap(vctx, IE_MC_FWD, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_src_addr(void *vctx, struct moca_src_addr *out, int max_out_len)
{
   int ret;
   int i;

   ret = moca_get_table(vctx, IE_SRC_ADDR, NULL, out, sizeof(*out), max_out_len, 0);

   for (i = 0; i < ret; i++)
   {
      moca_src_addr_swap(out);
      out++;
   }

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mac_aging(void *vctx, struct moca_mac_aging *out)
{
   int ret;
   struct moca_mac_aging_int mac_aging_int;
   ret = moca_get_noswap(vctx, IE_MAC_AGING, &mac_aging_int, sizeof(mac_aging_int));
   out->uc_fwd_age = mac_aging_int.uc_fwd_age;
   out->mc_fwd_age = mac_aging_int.mc_fwd_age;
   out->src_addr_age = mac_aging_int.src_addr_age;
   moca_mac_aging_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mac_aging(void *vctx, struct moca_mac_aging *in)
{
   struct moca_mac_aging_int tmp;
   tmp.uc_fwd_age = in->uc_fwd_age;
   tmp.mc_fwd_age = in->mc_fwd_age;
   tmp.src_addr_age = in->src_addr_age;
   tmp.reserved_0 = 0;
   moca_mac_aging_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_MAC_AGING, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_mac_aging (char *cli, struct moca_mac_aging * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--mac_aging ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "uc_fwd_age %u  ", (unsigned int) in->uc_fwd_age);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "mc_fwd_age %u  ", (unsigned int) in->mc_fwd_age);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "src_addr_age %u  ", (unsigned int) in->src_addr_age);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_loopback_en(void *vctx, uint32_t *en)
{
   return(moca_get(vctx, IE_LOOPBACK_EN, en, sizeof(*en)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_loopback_en(void *vctx, uint32_t en)
{
   return(moca_set(vctx, IE_LOOPBACK_EN, &en, sizeof(en)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_loopback_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--loopback_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mcfilter_enable(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MCFILTER_ENABLE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mcfilter_enable(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MCFILTER_ENABLE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_mcfilter_enable (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--mcfilter_enable ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mcfilter_addentry(void *vctx, struct moca_mcfilter_addentry *in)
{
   struct moca_mcfilter_addentry_int tmp;
   tmp.addr = in->addr;
   tmp.reserved_0 = 0;
   return(moca_set_noswap(vctx, IE_MCFILTER_ADDENTRY, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mcfilter_delentry(void *vctx, struct moca_mcfilter_delentry *in)
{
   struct moca_mcfilter_delentry_int tmp;
   tmp.addr = in->addr;
   tmp.reserved_0 = 0;
   return(moca_set_noswap(vctx, IE_MCFILTER_DELENTRY, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_pause_fc_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PAUSE_FC_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_pause_fc_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PAUSE_FC_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_pause_fc_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--pause_fc_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_stag_priority(void *vctx, struct moca_stag_priority *out)
{
   int ret;
   ret = moca_get(vctx, IE_STAG_PRIORITY, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_stag_priority(void *vctx, const struct moca_stag_priority *in)
{
   return(moca_set(vctx, IE_STAG_PRIORITY, in, sizeof(*in)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_stag_priority (char *cli, struct moca_stag_priority * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--stag_priority ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "enable %u  ", (unsigned int) in->enable);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "tag_mask %u  ", (unsigned int) in->tag_mask);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "moca_priority_0 %u  ", (unsigned int) in->moca_priority_0);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "tag_priority_0 %u  ", (unsigned int) in->tag_priority_0);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "moca_priority_1 %u  ", (unsigned int) in->moca_priority_1);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "tag_priority_1 %u  ", (unsigned int) in->tag_priority_1);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "moca_priority_2 %u  ", (unsigned int) in->moca_priority_2);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "tag_priority_2 %u  ", (unsigned int) in->tag_priority_2);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "moca_priority_3 %u  ", (unsigned int) in->moca_priority_3);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "tag_priority_3 %u  ", (unsigned int) in->tag_priority_3);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "moca_priority_4 %u  ", (unsigned int) in->moca_priority_4);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "tag_priority_4 %u  ", (unsigned int) in->tag_priority_4);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "moca_priority_5 %u  ", (unsigned int) in->moca_priority_5);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "tag_priority_5 %u  ", (unsigned int) in->tag_priority_5);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "moca_priority_6 %u  ", (unsigned int) in->moca_priority_6);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "tag_priority_6 %u  ", (unsigned int) in->tag_priority_6);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "moca_priority_7 %u  ", (unsigned int) in->moca_priority_7);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "tag_priority_7 %u  ", (unsigned int) in->tag_priority_7);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_stag_removal(void *vctx, struct moca_stag_removal *out)
{
   int ret;
   ret = moca_get(vctx, IE_STAG_REMOVAL, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_stag_removal(void *vctx, const struct moca_stag_removal *in)
{
   return(moca_set(vctx, IE_STAG_REMOVAL, in, sizeof(*in)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_stag_removal (char *cli, struct moca_stag_removal * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--stag_removal ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "enable %u  ", (unsigned int) in->enable);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "valid_0 %u  ", (unsigned int) in->valid_0);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "value_0 %u  ", (unsigned int) in->value_0);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "mask_0 %u  ", (unsigned int) in->mask_0);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "valid_1 %u  ", (unsigned int) in->valid_1);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "value_1 %u  ", (unsigned int) in->value_1);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "mask_1 %u  ", (unsigned int) in->mask_1);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "valid_2 %u  ", (unsigned int) in->valid_2);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "value_2 %u  ", (unsigned int) in->value_2);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "mask_2 %u  ", (unsigned int) in->mask_2);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "valid_3 %u  ", (unsigned int) in->valid_3);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "value_3 %u  ", (unsigned int) in->value_3);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "mask_3 %u  ", (unsigned int) in->mask_3);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_eport_link_state(void *vctx, uint32_t state)
{
   return(moca_set(vctx, IE_EPORT_LINK_STATE, &state, sizeof(state)));
}

static inline int mocalib_handle_ucfwd_update(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   if(ctx->cb.ucfwd_update_cb == NULL)
      return(-3);
   ctx->cb.ucfwd_update_cb(ctx->cb.ucfwd_update_userarg);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_ucfwd_update_cb(void *vctx, void (*callback)(void *userarg), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.ucfwd_update_cb = callback;
   ctx->cb.ucfwd_update_userarg = userarg;
}

static inline int mocalib_handle_pqos_maintenance_complete(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_pqos_maintenance_complete *in = (struct moca_pqos_maintenance_complete *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.pqos_maintenance_complete_cb == NULL)
      return(-4);
   ctx->cb.pqos_maintenance_complete_cb(ctx->cb.pqos_maintenance_complete_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_pqos_maintenance_complete_cb(void *vctx, void (*callback)(void *userarg, struct moca_pqos_maintenance_complete *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.pqos_maintenance_complete_cb = callback;
   ctx->cb.pqos_maintenance_complete_userarg = userarg;
}

static inline int mocalib_handle_pqos_create_flow(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_pqos_create_flow_out *in = (struct moca_pqos_create_flow_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_pqos_create_flow_out_swap(in);
   if(ctx->cb.pqos_create_flow_cb == NULL)
      return(-5);
   ctx->cb.pqos_create_flow_cb(ctx->cb.pqos_create_flow_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_pqos_create_flow_cb(void *vctx, void (*callback)(void *userarg, struct moca_pqos_create_flow_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.pqos_create_flow_cb = callback;
   ctx->cb.pqos_create_flow_userarg = userarg;
}

static void moca_pqos_create_flow_cb(void * userarg, struct moca_pqos_create_flow_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_pqos_create_flow_out * data = (struct moca_pqos_create_flow_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_pqos_create_flow(void *vctx, struct moca_pqos_create_flow_in *in, struct moca_pqos_create_flow_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_pqos_create_flow_cb(vctx, moca_pqos_create_flow_cb, (void *)&userarg);
   moca_pqos_create_flow_in_swap(in);
   ret = moca_set_noswap(vctx, IE_PQOS_CREATE_FLOW, in, sizeof(struct moca_pqos_create_flow_in));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_pqos_update_flow(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_pqos_update_flow_out *in = (struct moca_pqos_update_flow_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_pqos_update_flow_out_swap(in);
   if(ctx->cb.pqos_update_flow_cb == NULL)
      return(-5);
   ctx->cb.pqos_update_flow_cb(ctx->cb.pqos_update_flow_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_pqos_update_flow_cb(void *vctx, void (*callback)(void *userarg, struct moca_pqos_update_flow_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.pqos_update_flow_cb = callback;
   ctx->cb.pqos_update_flow_userarg = userarg;
}

static void moca_pqos_update_flow_cb(void * userarg, struct moca_pqos_update_flow_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_pqos_update_flow_out * data = (struct moca_pqos_update_flow_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_pqos_update_flow(void *vctx, struct moca_pqos_update_flow_in *in, struct moca_pqos_update_flow_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_pqos_update_flow_cb(vctx, moca_pqos_update_flow_cb, (void *)&userarg);
   moca_pqos_update_flow_in_swap(in);
   ret = moca_set_noswap(vctx, IE_PQOS_UPDATE_FLOW, in, sizeof(struct moca_pqos_update_flow_in));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_pqos_delete_flow(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_pqos_delete_flow_out *in = (struct moca_pqos_delete_flow_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_pqos_delete_flow_out_swap(in);
   if(ctx->cb.pqos_delete_flow_cb == NULL)
      return(-5);
   ctx->cb.pqos_delete_flow_cb(ctx->cb.pqos_delete_flow_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_pqos_delete_flow_cb(void *vctx, void (*callback)(void *userarg, struct moca_pqos_delete_flow_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.pqos_delete_flow_cb = callback;
   ctx->cb.pqos_delete_flow_userarg = userarg;
}

static void moca_pqos_delete_flow_cb(void * userarg, struct moca_pqos_delete_flow_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_pqos_delete_flow_out * data = (struct moca_pqos_delete_flow_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_pqos_delete_flow(void *vctx, macaddr_t flow_id, struct moca_pqos_delete_flow_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_pqos_delete_flow_cb(vctx, moca_pqos_delete_flow_cb, (void *)&userarg);
   ret = moca_set_noswap(vctx, IE_PQOS_DELETE_FLOW, &flow_id, sizeof(macaddr_t));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_pqos_list(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_pqos_list_out *in = (struct moca_pqos_list_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_pqos_list_out_swap(in);
   if(ctx->cb.pqos_list_cb == NULL)
      return(-5);
   ctx->cb.pqos_list_cb(ctx->cb.pqos_list_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_pqos_list_cb(void *vctx, void (*callback)(void *userarg, struct moca_pqos_list_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.pqos_list_cb = callback;
   ctx->cb.pqos_list_userarg = userarg;
}

static void moca_pqos_list_cb(void * userarg, struct moca_pqos_list_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_pqos_list_out * data = (struct moca_pqos_list_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_pqos_list(void *vctx, struct moca_pqos_list_in *in, struct moca_pqos_list_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_pqos_list_cb(vctx, moca_pqos_list_cb, (void *)&userarg);
   moca_pqos_list_in_swap(in);
   ret = moca_set_noswap(vctx, IE_PQOS_LIST, in, sizeof(struct moca_pqos_list_in));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_pqos_query(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_pqos_query_out *in = (struct moca_pqos_query_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_pqos_query_out_swap(in);
   if(ctx->cb.pqos_query_cb == NULL)
      return(-5);
   ctx->cb.pqos_query_cb(ctx->cb.pqos_query_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_pqos_query_cb(void *vctx, void (*callback)(void *userarg, struct moca_pqos_query_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.pqos_query_cb = callback;
   ctx->cb.pqos_query_userarg = userarg;
}

static void moca_pqos_query_cb(void * userarg, struct moca_pqos_query_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_pqos_query_out * data = (struct moca_pqos_query_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_pqos_query(void *vctx, macaddr_t flow_id, struct moca_pqos_query_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_pqos_query_cb(vctx, moca_pqos_query_cb, (void *)&userarg);
   ret = moca_set_noswap(vctx, IE_PQOS_QUERY, &flow_id, sizeof(macaddr_t));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_pqos_status(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_pqos_status_out *in = (struct moca_pqos_status_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.pqos_status_cb == NULL)
      return(-4);
   ctx->cb.pqos_status_cb(ctx->cb.pqos_status_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_pqos_status_cb(void *vctx, void (*callback)(void *userarg, struct moca_pqos_status_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.pqos_status_cb = callback;
   ctx->cb.pqos_status_userarg = userarg;
}

static void moca_pqos_status_cb(void * userarg, struct moca_pqos_status_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_pqos_status_out * data = (struct moca_pqos_status_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_pqos_status(void *vctx, uint32_t unused, struct moca_pqos_status_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_pqos_status_cb(vctx, moca_pqos_status_cb, (void *)&userarg);
   ret = moca_set(vctx, IE_PQOS_STATUS, &unused, sizeof(uint32_t));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mcfilter_clear_table(void *vctx)
{
   return(moca_set(vctx, IE_MCFILTER_CLEAR_TABLE, NULL, 0));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mcfilter_table(void *vctx, struct moca_mcfilter_table *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_MCFILTER_TABLE, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_host_qos(void *vctx, uint32_t *enable)
{
   return(moca_get(vctx, IE_HOST_QOS, enable, sizeof(*enable)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_host_qos(void *vctx, uint32_t enable)
{
   return(moca_set(vctx, IE_HOST_QOS, &enable, sizeof(enable)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_host_qos (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--host_qos ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_multicast_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_multicast_mode;
   uint32_t multicast_mode;
   memset(&default_multicast_mode, 0, sizeof(default_multicast_mode));
   memset(&multicast_mode, 0, sizeof(multicast_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_multicast_mode(handle, &multicast_mode);
      moca_set_multicast_mode_defaults(&default_multicast_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_multicast_mode, &multicast_mode, sizeof(multicast_mode))!=0))
         moca_nv_print_multicast_mode(cli, &multicast_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_low_pri_q_num( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_low_pri_q_num;
   uint32_t low_pri_q_num;
   memset(&default_low_pri_q_num, 0, sizeof(default_low_pri_q_num));
   memset(&low_pri_q_num, 0, sizeof(low_pri_q_num));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_low_pri_q_num(handle, &low_pri_q_num);
      moca_set_low_pri_q_num_defaults(&default_low_pri_q_num, config_flags);

      if ((ret == 0) && (memcmp(&default_low_pri_q_num, &low_pri_q_num, sizeof(low_pri_q_num))!=0))
         moca_nv_print_low_pri_q_num(cli, &low_pri_q_num, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_egr_mc_filter_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_egr_mc_filter_en;
   uint32_t egr_mc_filter_en;
   memset(&default_egr_mc_filter_en, 0, sizeof(default_egr_mc_filter_en));
   memset(&egr_mc_filter_en, 0, sizeof(egr_mc_filter_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_egr_mc_filter_en(handle, &egr_mc_filter_en);
      moca_set_egr_mc_filter_en_defaults(&default_egr_mc_filter_en, config_flags);

      if ((ret == 0) && (memcmp(&default_egr_mc_filter_en, &egr_mc_filter_en, sizeof(egr_mc_filter_en))!=0))
         moca_nv_print_egr_mc_filter_en(cli, &egr_mc_filter_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_fc_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_fc_mode;
   uint32_t fc_mode;
   memset(&default_fc_mode, 0, sizeof(default_fc_mode));
   memset(&fc_mode, 0, sizeof(fc_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_fc_mode(handle, &fc_mode);
      moca_set_fc_mode_defaults(&default_fc_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_fc_mode, &fc_mode, sizeof(fc_mode))!=0))
         moca_nv_print_fc_mode(cli, &fc_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_pqos_max_packet_size( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_pqos_max_packet_size;
   uint32_t pqos_max_packet_size;
   memset(&default_pqos_max_packet_size, 0, sizeof(default_pqos_max_packet_size));
   memset(&pqos_max_packet_size, 0, sizeof(pqos_max_packet_size));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_pqos_max_packet_size(handle, &pqos_max_packet_size);
      moca_set_pqos_max_packet_size_defaults(&default_pqos_max_packet_size, config_flags);

      if ((ret == 0) && (memcmp(&default_pqos_max_packet_size, &pqos_max_packet_size, sizeof(pqos_max_packet_size))!=0))
         moca_nv_print_pqos_max_packet_size(cli, &pqos_max_packet_size, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_per_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_per_mode;
   uint32_t per_mode;
   memset(&default_per_mode, 0, sizeof(default_per_mode));
   memset(&per_mode, 0, sizeof(per_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_per_mode(handle, &per_mode);
      moca_set_per_mode_defaults(&default_per_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_per_mode, &per_mode, sizeof(per_mode))!=0))
         moca_nv_print_per_mode(cli, &per_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_policing_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_policing_en;
   uint32_t policing_en;
   memset(&default_policing_en, 0, sizeof(default_policing_en));
   memset(&policing_en, 0, sizeof(policing_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_policing_en(handle, &policing_en);
      moca_set_policing_en_defaults(&default_policing_en, config_flags);

      if ((ret == 0) && (memcmp(&default_policing_en, &policing_en, sizeof(policing_en))!=0))
         moca_nv_print_policing_en(cli, &policing_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_orr_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_orr_en;
   uint32_t orr_en;
   memset(&default_orr_en, 0, sizeof(default_orr_en));
   memset(&orr_en, 0, sizeof(orr_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_orr_en(handle, &orr_en);
      moca_set_orr_en_defaults(&default_orr_en, config_flags);

      if ((ret == 0) && (memcmp(&default_orr_en, &orr_en, sizeof(orr_en))!=0))
         moca_nv_print_orr_en(cli, &orr_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_brcmtag_enable( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_brcmtag_enable;
   uint32_t brcmtag_enable;
   memset(&default_brcmtag_enable, 0, sizeof(default_brcmtag_enable));
   memset(&brcmtag_enable, 0, sizeof(brcmtag_enable));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_brcmtag_enable(handle, &brcmtag_enable);
      moca_set_brcmtag_enable_defaults(&default_brcmtag_enable, config_flags);

      if ((ret == 0) && (memcmp(&default_brcmtag_enable, &brcmtag_enable, sizeof(brcmtag_enable))!=0))
         moca_nv_print_brcmtag_enable(cli, &brcmtag_enable, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_priority_allocations( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_priority_allocations default_priority_allocations;
   struct moca_priority_allocations priority_allocations;
   memset(&default_priority_allocations, 0, sizeof(default_priority_allocations));
   memset(&priority_allocations, 0, sizeof(priority_allocations));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_priority_allocations(handle, &priority_allocations);
      moca_set_priority_allocations_defaults(&default_priority_allocations, config_flags);

      if ((ret == 0) && (memcmp(&default_priority_allocations, &priority_allocations, sizeof(priority_allocations))!=0))
         moca_nv_print_priority_allocations(cli, &priority_allocations, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_mac_aging( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_mac_aging default_mac_aging;
   struct moca_mac_aging mac_aging;
   memset(&default_mac_aging, 0, sizeof(default_mac_aging));
   memset(&mac_aging, 0, sizeof(mac_aging));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_mac_aging(handle, &mac_aging);
      moca_set_mac_aging_defaults(&default_mac_aging, config_flags);

      if ((ret == 0) && (memcmp(&default_mac_aging, &mac_aging, sizeof(mac_aging))!=0))
         moca_nv_print_mac_aging(cli, &mac_aging, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_loopback_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_loopback_en;
   uint32_t loopback_en;
   memset(&default_loopback_en, 0, sizeof(default_loopback_en));
   memset(&loopback_en, 0, sizeof(loopback_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_loopback_en(handle, &loopback_en);
      moca_set_loopback_en_defaults(&default_loopback_en, config_flags);

      if ((ret == 0) && (memcmp(&default_loopback_en, &loopback_en, sizeof(loopback_en))!=0))
         moca_nv_print_loopback_en(cli, &loopback_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_mcfilter_enable( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_mcfilter_enable;
   uint32_t mcfilter_enable;
   memset(&default_mcfilter_enable, 0, sizeof(default_mcfilter_enable));
   memset(&mcfilter_enable, 0, sizeof(mcfilter_enable));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_mcfilter_enable(handle, &mcfilter_enable);
      moca_set_mcfilter_enable_defaults(&default_mcfilter_enable, config_flags);

      if ((ret == 0) && (memcmp(&default_mcfilter_enable, &mcfilter_enable, sizeof(mcfilter_enable))!=0))
         moca_nv_print_mcfilter_enable(cli, &mcfilter_enable, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_pause_fc_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_pause_fc_en;
   uint32_t pause_fc_en;
   memset(&default_pause_fc_en, 0, sizeof(default_pause_fc_en));
   memset(&pause_fc_en, 0, sizeof(pause_fc_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_pause_fc_en(handle, &pause_fc_en);
      moca_set_pause_fc_en_defaults(&default_pause_fc_en, config_flags);

      if ((ret == 0) && (memcmp(&default_pause_fc_en, &pause_fc_en, sizeof(pause_fc_en))!=0))
         moca_nv_print_pause_fc_en(cli, &pause_fc_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_stag_priority( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_stag_priority default_stag_priority;
   struct moca_stag_priority stag_priority;
   memset(&default_stag_priority, 0, sizeof(default_stag_priority));
   memset(&stag_priority, 0, sizeof(stag_priority));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_stag_priority(handle, &stag_priority);
      moca_set_stag_priority_defaults(&default_stag_priority, config_flags);

      if ((ret == 0) && (memcmp(&default_stag_priority, &stag_priority, sizeof(stag_priority))!=0))
         moca_nv_print_stag_priority(cli, &stag_priority, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_stag_removal( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_stag_removal default_stag_removal;
   struct moca_stag_removal stag_removal;
   memset(&default_stag_removal, 0, sizeof(default_stag_removal));
   memset(&stag_removal, 0, sizeof(stag_removal));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_stag_removal(handle, &stag_removal);
      moca_set_stag_removal_defaults(&default_stag_removal, config_flags);

      if ((ret == 0) && (memcmp(&default_stag_removal, &stag_removal, sizeof(stag_removal))!=0))
         moca_nv_print_stag_removal(cli, &stag_removal, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_host_qos( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_host_qos;
   uint32_t host_qos;
   memset(&default_host_qos, 0, sizeof(default_host_qos));
   memset(&host_qos, 0, sizeof(host_qos));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_host_qos(handle, &host_qos);
      moca_set_host_qos_defaults(&default_host_qos, config_flags);

      if ((ret == 0) && (memcmp(&default_host_qos, &host_qos, sizeof(host_qos))!=0))
         moca_nv_print_host_qos(cli, &host_qos, max_len);
}

// Group network
MOCALIB_GEN_GET_FUNCTION int __moca_get_ooo_lmo_threshold(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_OOO_LMO_THRESHOLD, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_ooo_lmo_threshold(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_OOO_LMO_THRESHOLD, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ooo_lmo_threshold (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ooo_lmo_threshold ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_taboo_channels(void *vctx, struct moca_taboo_channels *out)
{
   int ret;
   ret = moca_get(vctx, IE_TABOO_CHANNELS, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_taboo_channels(void *vctx, const struct moca_taboo_channels *in)
{
   return(moca_set(vctx, IE_TABOO_CHANNELS, in, sizeof(*in)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_taboo_channels (char *cli, struct moca_taboo_channels * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--taboo_channels ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "taboo_fixed_mask_start %u  ", (unsigned int) in->taboo_fixed_mask_start);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "taboo_fixed_channel_mask %u  ", (unsigned int) in->taboo_fixed_channel_mask);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "taboo_left_mask %u  ", (unsigned int) in->taboo_left_mask);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "taboo_right_mask %u  ", (unsigned int) in->taboo_right_mask);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_fmr_request(void *vctx, struct moca_fmr_request *in)
{
   struct moca_fmr_request_int tmp;
   tmp.hdrFmt = 0x8;
   tmp.reserved_0 = 0;
   tmp.vendorId = 0;
   tmp.transType = 0x2;
   tmp.transSubtype = 0x1;
   tmp.wave0Nodemask = in->wave0Nodemask;
   tmp.reserved_1 = 0;
   tmp.reserved_2 = 0;
   tmp.msgPriority = 0x80;
   tmp.txnLastWaveNum = 1;
   tmp.reserved_3 = 0;
   tmp.reserved_4 = 0;
   moca_fmr_request_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_FMR_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_mr_request(void *vctx, struct moca_mr_request *in)
{
   struct moca_mr_request_int tmp;
   tmp.hdrFmt = 0x8;
   tmp.reserved_0 = 0;
   tmp.vendorId = 0;
   tmp.transType = 0x3;
   tmp.transSubtype = 0x1;
   tmp.wave0Nodemask = in->wave0Nodemask;
   tmp.reserved_1 = 0;
   tmp.reserved_2 = 0;
   tmp.msgPriority = 0x80;
   tmp.txnLastWaveNum = 2;
   tmp.reserved_3 = 0;
   tmp.resetStatus = 0;
   tmp.resetTimer = in->resetTimer;
   tmp.nonDefSeqNum = in->nonDefSeqNum;
   moca_mr_request_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_MR_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_gen_node_status(void *vctx, uint32_t index, struct moca_gen_node_status *out)
{
   int ret;
   ret = moca_get_inout_noswap(vctx, IE_GEN_NODE_STATUS, &index, sizeof(index), out, sizeof(*out));
   moca_gen_node_status_swap(out);

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_gen_node_ext_status(void *vctx, struct moca_gen_node_ext_status_in *in, struct moca_gen_node_ext_status *out)
{
   int ret;
   ret = moca_get_inout(vctx, IE_GEN_NODE_EXT_STATUS, in, sizeof(*in), out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_node_stats(void *vctx, struct moca_node_stats_in *in, struct moca_node_stats *out)
{
   int ret;
   ret = moca_get_inout(vctx, IE_NODE_STATS, in, sizeof(*in), out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_node_stats_ext(void *vctx, struct moca_node_stats_ext_in *in, struct moca_node_stats_ext *out)
{
   int ret;
   ret = moca_get_inout_noswap(vctx, IE_NODE_STATS_EXT, in, sizeof(*in), out, sizeof(*out));
   moca_node_stats_ext_swap(out);

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_network_status(void *vctx, struct moca_network_status *out)
{
   int ret;
   ret = moca_get(vctx, IE_NETWORK_STATUS, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_ooo_lmo(void *vctx, uint32_t node_id)
{
   return(moca_set(vctx, IE_OOO_LMO, &node_id, sizeof(node_id)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_ooo_lmo (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--ooo_lmo ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_start_ulmo(void *vctx, struct moca_start_ulmo *out)
{
   int ret;
   ret = moca_get(vctx, IE_START_ULMO, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_start_ulmo(void *vctx, const struct moca_start_ulmo *in)
{
   return(moca_set(vctx, IE_START_ULMO, in, sizeof(*in)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_start_ulmo (char *cli, struct moca_start_ulmo * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   char p[2048];

   bytes_written = snprintf(c, *max_len, "--start_ulmo ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "report_type %u  ", (unsigned int) in->report_type);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "node_id %u  ", (unsigned int) in->node_id);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "ofdma_node_mask %u  ", (unsigned int) in->ofdma_node_mask);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   mocacli_to_bits(p, in->subcarrier, 16);
   bytes_written = snprintf (c + strlen(c), *max_len, "subcarrier ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%s ", p);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_dd_request(void *vctx, struct moca_dd_request *in)
{
   struct moca_dd_request_int tmp;
   tmp.hdrFmt = 0x8;
   tmp.reserved_0 = 0;
   tmp.vendorId = 0;
   tmp.transType = 0x4;
   tmp.transSubtype = 0x1;
   tmp.wave0Nodemask = in->wave0Nodemask;
   tmp.reserved_1 = 0;
   tmp.reserved_2 = 0;
   tmp.msgPriority = 0x80;
   tmp.txnLastWaveNum = 1;
   tmp.reserved_3 = 0;
   tmp.reserved_4 = 0;
   moca_dd_request_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_DD_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_fmr20_request(void *vctx, struct moca_fmr20_request *in)
{
   struct moca_fmr20_request_int tmp;
   tmp.hdrFmt = 0x8;
   tmp.reserved_0 = 0;
   tmp.vendorId = 0;
   tmp.transType = 0x2;
   tmp.transSubtype = 0x2;
   tmp.wave0Nodemask = in->wave0Nodemask;
   tmp.reserved_1 = 0;
   tmp.reserved_2 = 0;
   tmp.msgPriority = 0x80;
   tmp.txnLastWaveNum = 1;
   tmp.reserved_3 = 0;
   tmp.reserved_4 = 0;
   moca_fmr20_request_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_FMR20_REQUEST, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rxd_lmo_request(void *vctx, const struct moca_rxd_lmo_request *in)
{
   return(moca_set(vctx, IE_RXD_LMO_REQUEST, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_ofdma_definition_table(void *vctx, struct moca_ofdma_definition_table *out)
{
   int ret;
   ret = moca_get(vctx, IE_OFDMA_DEFINITION_TABLE, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_ofdma_assignment_table(void *vctx, struct moca_ofdma_assignment_table *out)
{
   int ret;
   ret = moca_get(vctx, IE_OFDMA_ASSIGNMENT_TABLE, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_start_aca(void *vctx, struct moca_start_aca *in)
{
   struct moca_start_aca tmp = *in;
   moca_start_aca_swap(&tmp);
   return(moca_set_noswap(vctx, IE_START_ACA, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_adm_stats(void *vctx, struct moca_adm_stats *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_ADM_STATS, out, sizeof(*out));
   moca_adm_stats_swap(out);

   return(ret);
}

static inline int mocalib_handle_admission_status(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.admission_status_cb == NULL)
      return(-3);
   ctx->cb.admission_status_cb(ctx->cb.admission_status_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_admission_status_cb(void *vctx, void (*callback)(void *userarg, uint32_t status), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.admission_status_cb = callback;
   ctx->cb.admission_status_userarg = userarg;
}

static inline int mocalib_handle_limited_bw(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.limited_bw_cb == NULL)
      return(-3);
   ctx->cb.limited_bw_cb(ctx->cb.limited_bw_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_limited_bw_cb(void *vctx, void (*callback)(void *userarg, uint32_t bw_status), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.limited_bw_cb = callback;
   ctx->cb.limited_bw_userarg = userarg;
}

static inline int mocalib_handle_lmo_info(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_lmo_info *in = (struct moca_lmo_info *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.lmo_info_cb == NULL)
      return(-4);
   ctx->cb.lmo_info_cb(ctx->cb.lmo_info_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_lmo_info_cb(void *vctx, void (*callback)(void *userarg, struct moca_lmo_info *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.lmo_info_cb = callback;
   ctx->cb.lmo_info_userarg = userarg;
}

static inline int mocalib_handle_topology_changed(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.topology_changed_cb == NULL)
      return(-3);
   ctx->cb.topology_changed_cb(ctx->cb.topology_changed_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_topology_changed_cb(void *vctx, void (*callback)(void *userarg, uint32_t nodemask), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.topology_changed_cb = callback;
   ctx->cb.topology_changed_userarg = userarg;
}

static inline int mocalib_handle_moca_version_changed(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.moca_version_changed_cb == NULL)
      return(-3);
   ctx->cb.moca_version_changed_cb(ctx->cb.moca_version_changed_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_moca_version_changed_cb(void *vctx, void (*callback)(void *userarg, uint32_t new_version), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.moca_version_changed_cb = callback;
   ctx->cb.moca_version_changed_userarg = userarg;
}

static inline int mocalib_handle_moca_reset_request(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_moca_reset_request *in = (struct moca_moca_reset_request *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.moca_reset_request_cb == NULL)
      return(-4);
   ctx->cb.moca_reset_request_cb(ctx->cb.moca_reset_request_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_moca_reset_request_cb(void *vctx, void (*callback)(void *userarg, struct moca_moca_reset_request *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.moca_reset_request_cb = callback;
   ctx->cb.moca_reset_request_userarg = userarg;
}

static inline int mocalib_handle_nc_id_changed(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.nc_id_changed_cb == NULL)
      return(-3);
   ctx->cb.nc_id_changed_cb(ctx->cb.nc_id_changed_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_nc_id_changed_cb(void *vctx, void (*callback)(void *userarg, uint32_t new_nc_id), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.nc_id_changed_cb = callback;
   ctx->cb.nc_id_changed_userarg = userarg;
}

static inline int mocalib_handle_mr_event(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.mr_event_cb == NULL)
      return(-3);
   ctx->cb.mr_event_cb(ctx->cb.mr_event_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_mr_event_cb(void *vctx, void (*callback)(void *userarg, uint32_t status), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.mr_event_cb = callback;
   ctx->cb.mr_event_userarg = userarg;
}

static inline int mocalib_handle_aca(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_aca_out *in = (struct moca_aca_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_aca_out_swap(in);
   if(ctx->cb.aca_cb == NULL)
      return(-5);
   ctx->cb.aca_cb(ctx->cb.aca_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_aca_cb(void *vctx, void (*callback)(void *userarg, struct moca_aca_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.aca_cb = callback;
   ctx->cb.aca_userarg = userarg;
}

static void moca_aca_cb(void * userarg, struct moca_aca_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_aca_out * data = (struct moca_aca_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_aca(void *vctx, struct moca_aca_in *in, struct moca_aca_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_aca_cb(vctx, moca_aca_cb, (void *)&userarg);
   moca_aca_in_swap(in);
   ret = moca_set_noswap(vctx, IE_ACA, in, sizeof(struct moca_aca_in));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 20, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_fmr_init(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_fmr_init_out *in = (struct moca_fmr_init_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_fmr_init_out_swap(in);
   if(ctx->cb.fmr_init_cb == NULL)
      return(-5);
   ctx->cb.fmr_init_cb(ctx->cb.fmr_init_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_fmr_init_cb(void *vctx, void (*callback)(void *userarg, struct moca_fmr_init_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.fmr_init_cb = callback;
   ctx->cb.fmr_init_userarg = userarg;
}

static void moca_fmr_init_cb(void * userarg, struct moca_fmr_init_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_fmr_init_out * data = (struct moca_fmr_init_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_fmr_init(void *vctx, uint32_t node_mask, struct moca_fmr_init_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_fmr_init_cb(vctx, moca_fmr_init_cb, (void *)&userarg);
   ret = moca_set(vctx, IE_FMR_INIT, &node_mask, sizeof(uint32_t));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_moca_reset(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_moca_reset_out *in = (struct moca_moca_reset_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_moca_reset_out_swap(in);
   if(ctx->cb.moca_reset_cb == NULL)
      return(-5);
   ctx->cb.moca_reset_cb(ctx->cb.moca_reset_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_moca_reset_cb(void *vctx, void (*callback)(void *userarg, struct moca_moca_reset_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.moca_reset_cb = callback;
   ctx->cb.moca_reset_userarg = userarg;
}

static void moca_moca_reset_cb(void * userarg, struct moca_moca_reset_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_moca_reset_out * data = (struct moca_moca_reset_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_moca_reset(void *vctx, struct moca_moca_reset_in *in, struct moca_moca_reset_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_moca_reset_cb(vctx, moca_moca_reset_cb, (void *)&userarg);
   moca_moca_reset_in_swap(in);
   ret = moca_set_noswap(vctx, IE_MOCA_RESET, in, sizeof(struct moca_moca_reset_in));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_dd_init(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_dd_init_out *in = (struct moca_dd_init_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.dd_init_cb == NULL)
      return(-4);
   ctx->cb.dd_init_cb(ctx->cb.dd_init_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_dd_init_cb(void *vctx, void (*callback)(void *userarg, struct moca_dd_init_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.dd_init_cb = callback;
   ctx->cb.dd_init_userarg = userarg;
}

static void moca_dd_init_cb(void * userarg, struct moca_dd_init_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_dd_init_out * data = (struct moca_dd_init_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_dd_init(void *vctx, uint32_t node_mask, struct moca_dd_init_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_dd_init_cb(vctx, moca_dd_init_cb, (void *)&userarg);
   ret = moca_set(vctx, IE_DD_INIT, &node_mask, sizeof(uint32_t));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_fmr_20(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_fmr_20_out *in = (struct moca_fmr_20_out *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_fmr_20_out_swap(in);
   if(ctx->cb.fmr_20_cb == NULL)
      return(-5);
   ctx->cb.fmr_20_cb(ctx->cb.fmr_20_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_fmr_20_cb(void *vctx, void (*callback)(void *userarg, struct moca_fmr_20_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.fmr_20_cb = callback;
   ctx->cb.fmr_20_userarg = userarg;
}

static void moca_fmr_20_cb(void * userarg, struct moca_fmr_20_out *out)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   struct moca_fmr_20_out * data = (struct moca_fmr_20_out *) arg->data;

   memcpy(data, out, sizeof(*out));
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_fmr_20(void *vctx, uint32_t node_mask, struct moca_fmr_20_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_fmr_20_cb(vctx, moca_fmr_20_cb, (void *)&userarg);
   ret = moca_set(vctx, IE_FMR_20, &node_mask, sizeof(uint32_t));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_error_stats(void *vctx, struct moca_error_stats *out)
{
   int ret;
   ret = moca_get(vctx, IE_ERROR_STATS, out, sizeof(*out));
   return(ret);
}

static inline int mocalib_handle_hostless_mode(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.hostless_mode_cb == NULL)
      return(-3);
   ctx->cb.hostless_mode_cb(ctx->cb.hostless_mode_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_hostless_mode_cb(void *vctx, void (*callback)(void *userarg, uint32_t status), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.hostless_mode_cb = callback;
   ctx->cb.hostless_mode_userarg = userarg;
}

static void moca_hostless_mode_cb(void * userarg, uint32_t status)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   uint32_t * data = (uint32_t *) arg->data;

   *data = status;
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_hostless_mode(void *vctx, uint32_t enable, uint32_t *status)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) status;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_hostless_mode_cb(vctx, moca_hostless_mode_cb, (void *)&userarg);
   ret = moca_set(vctx, IE_HOSTLESS_MODE, &enable, sizeof(uint32_t));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

static inline int mocalib_handle_wakeup_node(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.wakeup_node_cb == NULL)
      return(-3);
   ctx->cb.wakeup_node_cb(ctx->cb.wakeup_node_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_wakeup_node_cb(void *vctx, void (*callback)(void *userarg, uint32_t status), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.wakeup_node_cb = callback;
   ctx->cb.wakeup_node_userarg = userarg;
}

static void moca_wakeup_node_cb(void * userarg, uint32_t status)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   uint32_t * data = (uint32_t *) arg->data;

   *data = status;
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_wakeup_node(void *vctx, uint32_t node, uint32_t *status)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) status;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_wakeup_node_cb(vctx, moca_wakeup_node_cb, (void *)&userarg);
   ret = moca_set(vctx, IE_WAKEUP_NODE, &node, sizeof(uint32_t));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_last_mr_events(void *vctx, struct moca_last_mr_events *out)
{
   int ret;
   ret = moca_get(vctx, IE_LAST_MR_EVENTS, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_ooo_lmo_threshold( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_ooo_lmo_threshold;
   uint32_t ooo_lmo_threshold;
   memset(&default_ooo_lmo_threshold, 0, sizeof(default_ooo_lmo_threshold));
   memset(&ooo_lmo_threshold, 0, sizeof(ooo_lmo_threshold));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_ooo_lmo_threshold(handle, &ooo_lmo_threshold);
      moca_set_ooo_lmo_threshold_defaults(&default_ooo_lmo_threshold, config_flags);

      if ((ret == 0) && (memcmp(&default_ooo_lmo_threshold, &ooo_lmo_threshold, sizeof(ooo_lmo_threshold))!=0))
         moca_nv_print_ooo_lmo_threshold(cli, &ooo_lmo_threshold, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_taboo_channels( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_taboo_channels default_taboo_channels;
   struct moca_taboo_channels taboo_channels;
   memset(&default_taboo_channels, 0, sizeof(default_taboo_channels));
   memset(&taboo_channels, 0, sizeof(taboo_channels));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_taboo_channels(handle, &taboo_channels);
      moca_set_taboo_channels_defaults(&default_taboo_channels, config_flags);

      if ((ret == 0) && (memcmp(&default_taboo_channels, &taboo_channels, sizeof(taboo_channels))!=0))
         moca_nv_print_taboo_channels(cli, &taboo_channels, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_start_ulmo( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_start_ulmo default_start_ulmo;
   struct moca_start_ulmo start_ulmo;
   memset(&default_start_ulmo, 0, sizeof(default_start_ulmo));
   memset(&start_ulmo, 0, sizeof(start_ulmo));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_start_ulmo(handle, &start_ulmo);
      moca_set_start_ulmo_defaults(&default_start_ulmo, config_flags);

      if ((ret == 0) && (memcmp(&default_start_ulmo, &start_ulmo, sizeof(start_ulmo))!=0))
         moca_nv_print_start_ulmo(cli, &start_ulmo, max_len);
}

// Group intfc
MOCALIB_GEN_GET_FUNCTION int moca_get_rf_band(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RF_BAND, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rf_band(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RF_BAND, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rf_band (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--rf_band ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_rf_switch(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RF_SWITCH, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_rf_switch(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RF_SWITCH, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rf_switch (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--rf_switch ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_if_access_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_IF_ACCESS_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_if_access_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_IF_ACCESS_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_if_access_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--if_access_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_led_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_LED_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_led_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_LED_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_led_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--led_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_gen_stats(void *vctx, uint32_t reset_stats, struct moca_gen_stats *out)
{
   int ret;
   ret = moca_get_inout_noswap(vctx, IE_GEN_STATS, &reset_stats, sizeof(reset_stats), out, sizeof(*out));
   moca_gen_stats_swap(out);

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_interface_status(void *vctx, struct moca_interface_status *out)
{
   int ret;
   ret = moca_get(vctx, IE_INTERFACE_STATUS, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_reset_core_stats(void *vctx)
{
   return(moca_set(vctx, IE_RESET_CORE_STATS, NULL, 0));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_if_access_table(void *vctx, struct moca_if_access_table *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_IF_ACCESS_TABLE, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_if_access_table(void *vctx, struct moca_if_access_table *in)
{
   struct moca_if_access_table tmp = *in;
   return(moca_set_noswap(vctx, IE_IF_ACCESS_TABLE, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_if_access_table (char *cli, struct moca_if_access_table * in, uint32_t * max_len)
{
   macaddr_t last_value = {{0}};
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   int i;
   int start_index = 0;

   bytes_written = snprintf(c, *max_len, "--if_access_table ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   for (i = 0; i < 16 + 1; i++) {
      if ( ((i != 0) && !MOCA_MACADDR_COMPARE(&in->mac_addr[i], &last_value)) || (i == 16)) 
      {
         bytes_written = snprintf (c + strlen(c), *max_len, "mac_addr ");
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         bytes_written = snprintf (c + strlen(c), *max_len, "%02x:%02x:%02x:%02x:%02x:%02x  %d ", MOCA_DISPLAY_MAC(last_value), start_index );
         if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
         else *max_len -= bytes_written;

         if (start_index != (i-1) )
         {
            bytes_written = snprintf (c + strlen(c), *max_len, "%d ",   (i-1) );
            if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
            else *max_len -= bytes_written;

         }
         start_index = i;
      }
      MOCA_MACADDR_COPY(&last_value, &in->mac_addr[i]);
   }
   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

static inline int mocalib_handle_link_up_state(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.link_up_state_cb == NULL)
      return(-3);
   ctx->cb.link_up_state_cb(ctx->cb.link_up_state_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_link_up_state_cb(void *vctx, void (*callback)(void *userarg, uint32_t status), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.link_up_state_cb = callback;
   ctx->cb.link_up_state_userarg = userarg;
}

static inline int mocalib_handle_new_rf_band(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.new_rf_band_cb == NULL)
      return(-3);
   ctx->cb.new_rf_band_cb(ctx->cb.new_rf_band_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_new_rf_band_cb(void *vctx, void (*callback)(void *userarg, uint32_t rf_band), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.new_rf_band_cb = callback;
   ctx->cb.new_rf_band_userarg = userarg;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_ext_octet_count(void *vctx, struct moca_ext_octet_count *out)
{
   int ret;
   ret = moca_get(vctx, IE_EXT_OCTET_COUNT, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_reset_stats(void *vctx)
{
   return(moca_set(vctx, IE_RESET_STATS, NULL, 0));
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rf_band( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t rf_band;
   memset(&rf_band, 0, sizeof(rf_band));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rf_band(handle, &rf_band);
      if (ret == 0)
         moca_nv_print_rf_band(cli, &rf_band, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rf_switch( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_rf_switch;
   uint32_t rf_switch;
   memset(&default_rf_switch, 0, sizeof(default_rf_switch));
   memset(&rf_switch, 0, sizeof(rf_switch));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_rf_switch(handle, &rf_switch);
      moca_set_rf_switch_defaults(&default_rf_switch, config_flags);

      if ((ret == 0) && (memcmp(&default_rf_switch, &rf_switch, sizeof(rf_switch))!=0))
         moca_nv_print_rf_switch(cli, &rf_switch, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_if_access_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_if_access_en;
   uint32_t if_access_en;
   memset(&default_if_access_en, 0, sizeof(default_if_access_en));
   memset(&if_access_en, 0, sizeof(if_access_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_if_access_en(handle, &if_access_en);
      moca_set_if_access_en_defaults(&default_if_access_en, config_flags);

      if ((ret == 0) && (memcmp(&default_if_access_en, &if_access_en, sizeof(if_access_en))!=0))
         moca_nv_print_if_access_en(cli, &if_access_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_led_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_led_mode;
   uint32_t led_mode;
   memset(&default_led_mode, 0, sizeof(default_led_mode));
   memset(&led_mode, 0, sizeof(led_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_led_mode(handle, &led_mode);
      moca_set_led_mode_defaults(&default_led_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_led_mode, &led_mode, sizeof(led_mode))!=0))
         moca_nv_print_led_mode(cli, &led_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_if_access_table( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_if_access_table default_if_access_table;
   struct moca_if_access_table if_access_table;
   memset(&default_if_access_table, 0, sizeof(default_if_access_table));
   memset(&if_access_table, 0, sizeof(if_access_table));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_if_access_table(handle, &if_access_table);
      moca_set_if_access_table_defaults(&default_if_access_table, config_flags);

      if ((ret == 0) && (memcmp(&default_if_access_table, &if_access_table, sizeof(if_access_table))!=0))
         moca_nv_print_if_access_table(cli, &if_access_table, max_len);
}

// Group power_mgmt
MOCALIB_GEN_GET_FUNCTION int moca_get_m1_tx_power_variation(void *vctx, uint32_t *state)
{
   return(moca_get(vctx, IE_M1_TX_POWER_VARIATION, state, sizeof(*state)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_m1_tx_power_variation(void *vctx, uint32_t state)
{
   return(moca_set(vctx, IE_M1_TX_POWER_VARIATION, &state, sizeof(state)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_m1_tx_power_variation (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--m1_tx_power_variation ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_nc_listening_interval(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_NC_LISTENING_INTERVAL, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_nc_listening_interval(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_NC_LISTENING_INTERVAL, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_nc_listening_interval (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--nc_listening_interval ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_nc_heartbeat_interval(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_NC_HEARTBEAT_INTERVAL, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_nc_heartbeat_interval(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_NC_HEARTBEAT_INTERVAL, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_nc_heartbeat_interval (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--nc_heartbeat_interval ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_wom_mode_internal(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_WOM_MODE_INTERNAL, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_wom_mode_internal (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--wom_mode_internal ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_wom_mode_internal(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_WOM_MODE_INTERNAL, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_wom_magic_enable(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_WOM_MAGIC_ENABLE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_wom_magic_enable (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--wom_magic_enable ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_wom_magic_enable(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_WOM_MAGIC_ENABLE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_pm_restore_on_link_down(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PM_RESTORE_ON_LINK_DOWN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_pm_restore_on_link_down (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--pm_restore_on_link_down ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_pm_restore_on_link_down(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PM_RESTORE_ON_LINK_DOWN, val, sizeof(*val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_power_state(void *vctx, uint32_t *state)
{
   return(moca_get(vctx, IE_POWER_STATE, state, sizeof(*state)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_power_state(void *vctx, uint32_t state)
{
   return(moca_set(vctx, IE_POWER_STATE, &state, sizeof(state)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_hostless_mode_request(void *vctx, uint32_t *enable)
{
   return(moca_get(vctx, IE_HOSTLESS_MODE_REQUEST, enable, sizeof(*enable)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_hostless_mode_request(void *vctx, uint32_t enable)
{
   return(moca_set(vctx, IE_HOSTLESS_MODE_REQUEST, &enable, sizeof(enable)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_wakeup_node_request(void *vctx, uint32_t node)
{
   return(moca_set(vctx, IE_WAKEUP_NODE_REQUEST, &node, sizeof(node)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_node_power_state(void *vctx, uint32_t node, struct moca_node_power_state *out)
{
   int ret;
   ret = moca_get_inout(vctx, IE_NODE_POWER_STATE, &node, sizeof(node), out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_filter_m2_data_wakeUp(void *vctx, uint32_t *mode)
{
   return(moca_get(vctx, IE_FILTER_M2_DATA_WAKEUP, mode, sizeof(*mode)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_filter_m2_data_wakeUp(void *vctx, uint32_t mode)
{
   return(moca_set(vctx, IE_FILTER_M2_DATA_WAKEUP, &mode, sizeof(mode)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_wom_pattern(void *vctx, struct moca_wom_pattern *out, int max_out_len)
{
   int ret;

   ret = moca_get_table(vctx, IE_WOM_PATTERN, NULL, out, sizeof(*out), max_out_len, 0);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_wom_pattern(void *vctx, struct moca_wom_pattern_set *in)
{
   struct moca_wom_pattern_set tmp = *in;
   moca_wom_pattern_set_swap(&tmp);
   return(moca_set_noswap(vctx, IE_WOM_PATTERN, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_wom_ip(void *vctx, uint32_t *out, int max_out_len)
{
   int ret;

   ret = moca_get_table(vctx, IE_WOM_IP, NULL, out, sizeof(*out), max_out_len, FL_SWAP_RD);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_wom_ip(void *vctx, const struct moca_wom_ip *in)
{
   return(moca_set(vctx, IE_WOM_IP, in, sizeof(*in)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_wom_magic_mac(void *vctx, struct moca_wom_magic_mac *in)
{
   struct moca_wom_magic_mac_int tmp;
   tmp.val = in->val;
   tmp.reserved_0 = 0;
   return(moca_set_noswap(vctx, IE_WOM_MAGIC_MAC, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_wom_magic_mac(void *vctx, struct moca_wom_magic_mac *out)
{
   int ret;
   struct moca_wom_magic_mac_int wom_magic_mac_int;
   ret = moca_get_noswap(vctx, IE_WOM_MAGIC_MAC, &wom_magic_mac_int, sizeof(wom_magic_mac_int));
   out->val = wom_magic_mac_int.val;

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_standby_power_state(void *vctx, uint32_t *state)
{
   return(moca_get(vctx, IE_STANDBY_POWER_STATE, state, sizeof(*state)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_standby_power_state(void *vctx, uint32_t state)
{
   return(moca_set(vctx, IE_STANDBY_POWER_STATE, &state, sizeof(state)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_standby_power_state (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--standby_power_state ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_wom_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_WOM_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_wom_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--wom_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_wom_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_WOM_MODE, val, sizeof(*val)));
}

static inline int mocalib_handle_power_state_rsp(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.power_state_rsp_cb == NULL)
      return(-3);
   ctx->cb.power_state_rsp_cb(ctx->cb.power_state_rsp_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_power_state_rsp_cb(void *vctx, void (*callback)(void *userarg, uint32_t rsp_code), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.power_state_rsp_cb = callback;
   ctx->cb.power_state_rsp_userarg = userarg;
}

static inline int mocalib_handle_power_state_event(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.power_state_event_cb == NULL)
      return(-3);
   ctx->cb.power_state_event_cb(ctx->cb.power_state_event_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_power_state_event_cb(void *vctx, void (*callback)(void *userarg, uint32_t event_code), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.power_state_event_cb = callback;
   ctx->cb.power_state_event_userarg = userarg;
}

static inline int mocalib_handle_power_state_cap(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.power_state_cap_cb == NULL)
      return(-3);
   ctx->cb.power_state_cap_cb(ctx->cb.power_state_cap_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_power_state_cap_cb(void *vctx, void (*callback)(void *userarg, uint32_t power_modes), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.power_state_cap_cb = callback;
   ctx->cb.power_state_cap_userarg = userarg;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_wol(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_WOL, &val, sizeof(val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_wol(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_WOL, val, sizeof(*val)));
}

static inline int mocalib_handle_ps_cmd(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   uint32_t *in = (uint32_t *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   if(ctx->cb.ps_cmd_cb == NULL)
      return(-3);
   ctx->cb.ps_cmd_cb(ctx->cb.ps_cmd_userarg, BE32(*in));
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_ps_cmd_cb(void *vctx, void (*callback)(void *userarg, uint32_t rsp_code), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.ps_cmd_cb = callback;
   ctx->cb.ps_cmd_userarg = userarg;
}

static void moca_ps_cmd_cb(void * userarg, uint32_t rsp_code)
{
   struct moca_callback_arg * arg = (struct moca_callback_arg *) userarg;
   uint32_t * data = (uint32_t *) arg->data;

   *data = rsp_code;
   arg->test = 1;

   return;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_ps_cmd(void *vctx, uint32_t new_state, uint32_t *rsp_code)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) rsp_code;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_ps_cmd_cb(vctx, moca_ps_cmd_cb, (void *)&userarg);
   ret = moca_set(vctx, IE_PS_CMD, &new_state, sizeof(uint32_t));

   if (ret == 0)
      ret = moca_wait_for_event(vctx, 5, &userarg.test);

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_power_state_capabilities(void *vctx, uint32_t *power_modes)
{
   return(moca_get(vctx, IE_POWER_STATE_CAPABILITIES, power_modes, sizeof(*power_modes)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_last_ps_event_code(void *vctx, int32_t *val)
{
   return(moca_get(vctx, IE_LAST_PS_EVENT_CODE, val, sizeof(*val)));
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_m1_tx_power_variation( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_m1_tx_power_variation;
   uint32_t m1_tx_power_variation;
   memset(&default_m1_tx_power_variation, 0, sizeof(default_m1_tx_power_variation));
   memset(&m1_tx_power_variation, 0, sizeof(m1_tx_power_variation));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_m1_tx_power_variation(handle, &m1_tx_power_variation);
      moca_set_m1_tx_power_variation_defaults(&default_m1_tx_power_variation, config_flags);

      if ((ret == 0) && (memcmp(&default_m1_tx_power_variation, &m1_tx_power_variation, sizeof(m1_tx_power_variation))!=0))
         moca_nv_print_m1_tx_power_variation(cli, &m1_tx_power_variation, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_nc_listening_interval( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_nc_listening_interval;
   uint32_t nc_listening_interval;
   memset(&default_nc_listening_interval, 0, sizeof(default_nc_listening_interval));
   memset(&nc_listening_interval, 0, sizeof(nc_listening_interval));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_nc_listening_interval(handle, &nc_listening_interval);
      moca_set_nc_listening_interval_defaults(&default_nc_listening_interval, config_flags);

      if ((ret == 0) && (memcmp(&default_nc_listening_interval, &nc_listening_interval, sizeof(nc_listening_interval))!=0))
         moca_nv_print_nc_listening_interval(cli, &nc_listening_interval, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_nc_heartbeat_interval( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_nc_heartbeat_interval;
   uint32_t nc_heartbeat_interval;
   memset(&default_nc_heartbeat_interval, 0, sizeof(default_nc_heartbeat_interval));
   memset(&nc_heartbeat_interval, 0, sizeof(nc_heartbeat_interval));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_nc_heartbeat_interval(handle, &nc_heartbeat_interval);
      moca_set_nc_heartbeat_interval_defaults(&default_nc_heartbeat_interval, config_flags);

      if ((ret == 0) && (memcmp(&default_nc_heartbeat_interval, &nc_heartbeat_interval, sizeof(nc_heartbeat_interval))!=0))
         moca_nv_print_nc_heartbeat_interval(cli, &nc_heartbeat_interval, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_wom_magic_enable( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_wom_magic_enable;
   uint32_t wom_magic_enable;
   memset(&default_wom_magic_enable, 0, sizeof(default_wom_magic_enable));
   memset(&wom_magic_enable, 0, sizeof(wom_magic_enable));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_wom_magic_enable(handle, &wom_magic_enable);
      moca_set_wom_magic_enable_defaults(&default_wom_magic_enable, config_flags);

      if ((ret == 0) && (memcmp(&default_wom_magic_enable, &wom_magic_enable, sizeof(wom_magic_enable))!=0))
         moca_nv_print_wom_magic_enable(cli, &wom_magic_enable, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_pm_restore_on_link_down( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_pm_restore_on_link_down;
   uint32_t pm_restore_on_link_down;
   memset(&default_pm_restore_on_link_down, 0, sizeof(default_pm_restore_on_link_down));
   memset(&pm_restore_on_link_down, 0, sizeof(pm_restore_on_link_down));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_pm_restore_on_link_down(handle, &pm_restore_on_link_down);
      moca_set_pm_restore_on_link_down_defaults(&default_pm_restore_on_link_down, config_flags);

      if ((ret == 0) && (memcmp(&default_pm_restore_on_link_down, &pm_restore_on_link_down, sizeof(pm_restore_on_link_down))!=0))
         moca_nv_print_pm_restore_on_link_down(cli, &pm_restore_on_link_down, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_standby_power_state( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_standby_power_state;
   uint32_t standby_power_state;
   memset(&default_standby_power_state, 0, sizeof(default_standby_power_state));
   memset(&standby_power_state, 0, sizeof(standby_power_state));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_standby_power_state(handle, &standby_power_state);
      moca_set_standby_power_state_defaults(&default_standby_power_state, config_flags);

      if ((ret == 0) && (memcmp(&default_standby_power_state, &standby_power_state, sizeof(standby_power_state))!=0))
         moca_nv_print_standby_power_state(cli, &standby_power_state, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_wom_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_wom_mode;
   uint32_t wom_mode;
   memset(&default_wom_mode, 0, sizeof(default_wom_mode));
   memset(&wom_mode, 0, sizeof(wom_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_wom_mode(handle, &wom_mode);
      moca_set_wom_mode_defaults(&default_wom_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_wom_mode, &wom_mode, sizeof(wom_mode))!=0))
         moca_nv_print_wom_mode(cli, &wom_mode, max_len);
}

// Group security
MOCALIB_GEN_GET_FUNCTION int moca_get_privacy_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PRIVACY_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_privacy_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PRIVACY_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_privacy_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--privacy_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_pmk_exchange_interval(void *vctx, uint32_t *msec)
{
   return(moca_get(vctx, IE_PMK_EXCHANGE_INTERVAL, msec, sizeof(*msec)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_pmk_exchange_interval(void *vctx, uint32_t msec)
{
   return(moca_set(vctx, IE_PMK_EXCHANGE_INTERVAL, &msec, sizeof(msec)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_pmk_exchange_interval (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--pmk_exchange_interval ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_tek_exchange_interval(void *vctx, uint32_t *msec)
{
   return(moca_get(vctx, IE_TEK_EXCHANGE_INTERVAL, msec, sizeof(*msec)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_tek_exchange_interval(void *vctx, uint32_t msec)
{
   return(moca_set(vctx, IE_TEK_EXCHANGE_INTERVAL, &msec, sizeof(msec)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_tek_exchange_interval (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--tek_exchange_interval ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_aes_exchange_interval(void *vctx, uint32_t *msec)
{
   return(moca_get(vctx, IE_AES_EXCHANGE_INTERVAL, msec, sizeof(*msec)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_aes_exchange_interval(void *vctx, uint32_t msec)
{
   return(moca_set(vctx, IE_AES_EXCHANGE_INTERVAL, &msec, sizeof(msec)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_aes_exchange_interval (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--aes_exchange_interval ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mmk_key(void *vctx, struct moca_mmk_key *out)
{
   int ret;
   ret = moca_get(vctx, IE_MMK_KEY, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_mmk_key(void *vctx, const struct moca_mmk_key_set *in)
{
   return(moca_set(vctx, IE_MMK_KEY, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_pmk_initial_key(void *vctx, struct moca_pmk_initial_key *out)
{
   int ret;
   ret = moca_get(vctx, IE_PMK_INITIAL_KEY, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_pmk_initial_key(void *vctx, const struct moca_pmk_initial_key_set *in)
{
   return(moca_set(vctx, IE_PMK_INITIAL_KEY, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_aes_mm_key(void *vctx, struct moca_aes_mm_key *out)
{
   int ret;
   ret = moca_get(vctx, IE_AES_MM_KEY, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_aes_mm_key(void *vctx, const struct moca_aes_mm_key *in)
{
   return(moca_set(vctx, IE_AES_MM_KEY, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_aes_pm_key(void *vctx, struct moca_aes_pm_key *out)
{
   int ret;
   ret = moca_get(vctx, IE_AES_PM_KEY, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_aes_pm_key(void *vctx, const struct moca_aes_pm_key *in)
{
   return(moca_set(vctx, IE_AES_PM_KEY, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_current_keys(void *vctx, struct moca_current_keys *out)
{
   int ret;
   ret = moca_get(vctx, IE_CURRENT_KEYS, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_network_password(void *vctx, struct moca_network_password *in)
{
   struct moca_network_password tmp = *in;
   return(moca_set_noswap(vctx, IE_NETWORK_PASSWORD, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_network_password (char *cli, struct moca_network_password * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--network_password ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%s ", in->password);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_network_password(void *vctx, struct moca_network_password *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_NETWORK_PASSWORD, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_permanent_salt(void *vctx, struct moca_permanent_salt *out)
{
   int ret;
   ret = moca_get(vctx, IE_PERMANENT_SALT, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_aes_pmk_initial_key(void *vctx, struct moca_aes_pmk_initial_key *out)
{
   int ret;
   ret = moca_get(vctx, IE_AES_PMK_INITIAL_KEY, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_aes_pmk_initial_key(void *vctx, const struct moca_aes_pmk_initial_key *in)
{
   return(moca_set(vctx, IE_AES_PMK_INITIAL_KEY, in, sizeof(*in)));
}

static inline int mocalib_handle_key_changed(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_key_changed *in = (struct moca_key_changed *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.key_changed_cb == NULL)
      return(-4);
   ctx->cb.key_changed_cb(ctx->cb.key_changed_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_key_changed_cb(void *vctx, void (*callback)(void *userarg, struct moca_key_changed *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.key_changed_cb = callback;
   ctx->cb.key_changed_userarg = userarg;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_key_times(void *vctx, struct moca_key_times *out)
{
   int ret;
   ret = moca_get(vctx, IE_KEY_TIMES, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_password(void *vctx, struct moca_password *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_PASSWORD, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_password(void *vctx, struct moca_password *in)
{
   struct moca_password tmp = *in;
   return(moca_set_noswap(vctx, IE_PASSWORD, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_password (char *cli, struct moca_password * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--password ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%s ", in->password);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_privacy_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_privacy_en;
   uint32_t privacy_en;
   memset(&default_privacy_en, 0, sizeof(default_privacy_en));
   memset(&privacy_en, 0, sizeof(privacy_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_privacy_en(handle, &privacy_en);
      moca_set_privacy_en_defaults(&default_privacy_en, config_flags);

      if ((ret == 0) && (memcmp(&default_privacy_en, &privacy_en, sizeof(privacy_en))!=0))
         moca_nv_print_privacy_en(cli, &privacy_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_pmk_exchange_interval( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_pmk_exchange_interval;
   uint32_t pmk_exchange_interval;
   memset(&default_pmk_exchange_interval, 0, sizeof(default_pmk_exchange_interval));
   memset(&pmk_exchange_interval, 0, sizeof(pmk_exchange_interval));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_pmk_exchange_interval(handle, &pmk_exchange_interval);
      moca_set_pmk_exchange_interval_defaults(&default_pmk_exchange_interval, config_flags);

      if ((ret == 0) && (memcmp(&default_pmk_exchange_interval, &pmk_exchange_interval, sizeof(pmk_exchange_interval))!=0))
         moca_nv_print_pmk_exchange_interval(cli, &pmk_exchange_interval, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_tek_exchange_interval( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_tek_exchange_interval;
   uint32_t tek_exchange_interval;
   memset(&default_tek_exchange_interval, 0, sizeof(default_tek_exchange_interval));
   memset(&tek_exchange_interval, 0, sizeof(tek_exchange_interval));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_tek_exchange_interval(handle, &tek_exchange_interval);
      moca_set_tek_exchange_interval_defaults(&default_tek_exchange_interval, config_flags);

      if ((ret == 0) && (memcmp(&default_tek_exchange_interval, &tek_exchange_interval, sizeof(tek_exchange_interval))!=0))
         moca_nv_print_tek_exchange_interval(cli, &tek_exchange_interval, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_aes_exchange_interval( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_aes_exchange_interval;
   uint32_t aes_exchange_interval;
   memset(&default_aes_exchange_interval, 0, sizeof(default_aes_exchange_interval));
   memset(&aes_exchange_interval, 0, sizeof(aes_exchange_interval));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_aes_exchange_interval(handle, &aes_exchange_interval);
      moca_set_aes_exchange_interval_defaults(&default_aes_exchange_interval, config_flags);

      if ((ret == 0) && (memcmp(&default_aes_exchange_interval, &aes_exchange_interval, sizeof(aes_exchange_interval))!=0))
         moca_nv_print_aes_exchange_interval(cli, &aes_exchange_interval, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_password( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_password default_password;
   struct moca_password password;
   memset(&default_password, 0, sizeof(default_password));
   memset(&password, 0, sizeof(password));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_password(handle, &password);
      moca_set_password_defaults(&default_password, config_flags);

      if ((ret == 0) && (memcmp(&default_password, &password, sizeof(password))!=0))
         moca_nv_print_password(cli, &password, max_len);
}

// Group debug
MOCALIB_GEN_GET_FUNCTION int moca_get_mtm_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MTM_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mtm_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MTM_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_mtm_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--mtm_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_const_rx_submode(void *vctx, uint32_t *submode)
{
   return(moca_get(vctx, IE_CONST_RX_SUBMODE, submode, sizeof(*submode)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_const_rx_submode(void *vctx, uint32_t submode)
{
   return(moca_set(vctx, IE_CONST_RX_SUBMODE, &submode, sizeof(submode)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_const_rx_submode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--const_rx_submode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_cir_prints(void *vctx, uint32_t *bool_val)
{
   return(moca_get(vctx, IE_CIR_PRINTS, bool_val, sizeof(*bool_val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_cir_prints(void *vctx, uint32_t bool_val)
{
   return(moca_set(vctx, IE_CIR_PRINTS, &bool_val, sizeof(bool_val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_cir_prints (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--cir_prints ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_prints(void *vctx, uint32_t *bool_val)
{
   return(moca_get(vctx, IE_SNR_PRINTS, bool_val, sizeof(*bool_val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_snr_prints(void *vctx, uint32_t bool_val)
{
   return(moca_set(vctx, IE_SNR_PRINTS, &bool_val, sizeof(bool_val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_snr_prints (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--snr_prints ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_mmp_version(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MMP_VERSION, val, sizeof(*val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_sigma2_prints(void *vctx, uint32_t *bool_val)
{
   return(moca_get(vctx, IE_SIGMA2_PRINTS, bool_val, sizeof(*bool_val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_sigma2_prints(void *vctx, uint32_t bool_val)
{
   return(moca_set(vctx, IE_SIGMA2_PRINTS, &bool_val, sizeof(bool_val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_sigma2_prints (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--sigma2_prints ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_bad_probe_prints(void *vctx, uint32_t *bool_val)
{
   return(moca_get(vctx, IE_BAD_PROBE_PRINTS, bool_val, sizeof(*bool_val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_bad_probe_prints(void *vctx, uint32_t bool_val)
{
   return(moca_set(vctx, IE_BAD_PROBE_PRINTS, &bool_val, sizeof(bool_val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_bad_probe_prints (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--bad_probe_prints ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_const_tx_params(void *vctx, struct moca_const_tx_params *out)
{
   int ret;
   ret = moca_get(vctx, IE_CONST_TX_PARAMS, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_const_tx_params(void *vctx, const struct moca_const_tx_params *in)
{
   return(moca_set(vctx, IE_CONST_TX_PARAMS, in, sizeof(*in)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_const_tx_params (char *cli, struct moca_const_tx_params * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;
   char p[2048];

   bytes_written = snprintf(c, *max_len, "--const_tx_params ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "const_tx_submode %u  ", (unsigned int) in->const_tx_submode);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "const_tx_sc1 %u  ", (unsigned int) in->const_tx_sc1);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "const_tx_sc2 %u  ", (unsigned int) in->const_tx_sc2);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   mocacli_to_bits(p, in->const_tx_band, 16);
   bytes_written = snprintf (c + strlen(c), *max_len, "const_tx_band ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%s ", p);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_gmii_trap_header(void *vctx, struct moca_gmii_trap_header *in)
{
   struct moca_gmii_trap_header_int tmp;
   int i;
   for (i = 0; i < 6; i++)
      tmp.dest_mac[i] = in->dest_mac[i];
   for (i = 0; i < 6; i++)
      tmp.source_mac[i] = in->source_mac[i];
   tmp.eth_type = 0x800;
   tmp.ver_len = 0x45;
   tmp.dscp_ecn = in->dscp_ecn;
   tmp.ip_length = 0;
   tmp.id = in->id;
   tmp.flags_fragoffs = 0;
   tmp.ttl = in->ttl;
   tmp.prot = in->prot;
   tmp.ip_checksum = in->ip_checksum;
   for (i = 0; i < 4; i++)
      tmp.src_ip_addr[i] = in->src_ip_addr[i];
   for (i = 0; i < 4; i++)
      tmp.dst_ip_addr[i] = in->dst_ip_addr[i];
   tmp.src_port = in->src_port;
   tmp.dst_port = in->dst_port;
   tmp.udp_length = 0;
   tmp.udp_checksum = 0;
   moca_gmii_trap_header_int_swap(&tmp);
   return(moca_set_noswap(vctx, IE_GMII_TRAP_HEADER, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_led_status(void *vctx, uint32_t *led_status)
{
   return(moca_get(vctx, IE_LED_STATUS, led_status, sizeof(*led_status)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_moca_core_trace_enable(void *vctx, uint32_t *bool_val)
{
   return(moca_get(vctx, IE_MOCA_CORE_TRACE_ENABLE, bool_val, sizeof(*bool_val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_moca_core_trace_enable(void *vctx, uint32_t bool_val)
{
   return(moca_set(vctx, IE_MOCA_CORE_TRACE_ENABLE, &bool_val, sizeof(bool_val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_moca_core_trace_enable (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--moca_core_trace_enable ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

static inline int mocalib_handle_error(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_error *in = (struct moca_error *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.error_cb == NULL)
      return(-4);
   ctx->cb.error_cb(ctx->cb.error_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_error_cb(void *vctx, void (*callback)(void *userarg, struct moca_error *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.error_cb = callback;
   ctx->cb.error_userarg = userarg;
}

static inline int mocalib_handle_error_lookup(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_error_lookup *in = (struct moca_error_lookup *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.error_lookup_cb == NULL)
      return(-4);
   ctx->cb.error_lookup_cb(ctx->cb.error_lookup_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_error_lookup_cb(void *vctx, void (*callback)(void *userarg, struct moca_error_lookup *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.error_lookup_cb = callback;
   ctx->cb.error_lookup_userarg = userarg;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_error_to_mask(void *vctx, const struct moca_error_to_mask *in)
{
   return(moca_set(vctx, IE_ERROR_TO_MASK, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_error_to_mask(void *vctx, struct moca_error_to_mask *out)
{
   int ret;
   ret = moca_get(vctx, IE_ERROR_TO_MASK, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_fw_file(void *vctx, struct moca_fw_file *in)
{
   struct moca_fw_file tmp = *in;
   return(moca_set_noswap(vctx, IE_FW_FILE, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_verbose(void *vctx, uint32_t *level)
{
   return(moca_get(vctx, IE_VERBOSE, level, sizeof(*level)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_verbose(void *vctx, uint32_t level)
{
   return(moca_set(vctx, IE_VERBOSE, &level, sizeof(level)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_dont_start_moca(void *vctx, uint32_t dont_start_moca)
{
   return(moca_set(vctx, IE_DONT_START_MOCA, &dont_start_moca, sizeof(dont_start_moca)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_dont_start_moca (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--dont_start_moca ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_dont_start_moca(void *vctx, uint32_t *dont_start_moca)
{
   return(moca_get(vctx, IE_DONT_START_MOCA, dont_start_moca, sizeof(*dont_start_moca)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_no_rtt(void *vctx)
{
   return(moca_set(vctx, IE_NO_RTT, NULL, 0));
}

static inline int mocalib_handle_mocad_printf(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_mocad_printf_out *in = (struct moca_mocad_printf_out *)vin;
   if(in_len > sizeof(*in))
      return(-2);
   if(ctx->cb.mocad_printf_cb == NULL)
      return(-5);
   ctx->cb.mocad_printf_cb(ctx->cb.mocad_printf_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_mocad_printf_cb(void *vctx, void (*callback)(void *userarg, struct moca_mocad_printf_out *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.mocad_printf_cb = callback;
   ctx->cb.mocad_printf_userarg = userarg;
}

MOCALIB_GEN_DO_FUNCTION int moca_do_mocad_printf(void *vctx, struct moca_mocad_printf_out *out)
{
   struct moca_callback_arg userarg;
   int ret;

   userarg.data = (void *) out;
   userarg.test = 0;

   ret = moca_init_evt(vctx);
   if (ret != 0)
      return(ret);

   moca_register_mocad_printf_cb(vctx, moca_mocad_printf_cb, (void *)&userarg);
   if (ret == 0)
      ret = moca_wait_for_event(vctx, 200000000, &userarg.test);

   return(ret);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_mtm_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_mtm_en;
   uint32_t mtm_en;
   memset(&default_mtm_en, 0, sizeof(default_mtm_en));
   memset(&mtm_en, 0, sizeof(mtm_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_mtm_en(handle, &mtm_en);
      moca_set_mtm_en_defaults(&default_mtm_en, config_flags);

      if ((ret == 0) && (memcmp(&default_mtm_en, &mtm_en, sizeof(mtm_en))!=0))
         moca_nv_print_mtm_en(cli, &mtm_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_const_rx_submode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_const_rx_submode;
   uint32_t const_rx_submode;
   memset(&default_const_rx_submode, 0, sizeof(default_const_rx_submode));
   memset(&const_rx_submode, 0, sizeof(const_rx_submode));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_const_rx_submode(handle, &const_rx_submode);
      moca_set_const_rx_submode_defaults(&default_const_rx_submode, config_flags);

      if ((ret == 0) && (memcmp(&default_const_rx_submode, &const_rx_submode, sizeof(const_rx_submode))!=0))
         moca_nv_print_const_rx_submode(cli, &const_rx_submode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_cir_prints( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_cir_prints;
   uint32_t cir_prints;
   memset(&default_cir_prints, 0, sizeof(default_cir_prints));
   memset(&cir_prints, 0, sizeof(cir_prints));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_cir_prints(handle, &cir_prints);
      moca_set_cir_prints_defaults(&default_cir_prints, config_flags);

      if ((ret == 0) && (memcmp(&default_cir_prints, &cir_prints, sizeof(cir_prints))!=0))
         moca_nv_print_cir_prints(cli, &cir_prints, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_snr_prints( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_snr_prints;
   uint32_t snr_prints;
   memset(&default_snr_prints, 0, sizeof(default_snr_prints));
   memset(&snr_prints, 0, sizeof(snr_prints));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_snr_prints(handle, &snr_prints);
      moca_set_snr_prints_defaults(&default_snr_prints, config_flags);

      if ((ret == 0) && (memcmp(&default_snr_prints, &snr_prints, sizeof(snr_prints))!=0))
         moca_nv_print_snr_prints(cli, &snr_prints, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_sigma2_prints( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_sigma2_prints;
   uint32_t sigma2_prints;
   memset(&default_sigma2_prints, 0, sizeof(default_sigma2_prints));
   memset(&sigma2_prints, 0, sizeof(sigma2_prints));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_sigma2_prints(handle, &sigma2_prints);
      moca_set_sigma2_prints_defaults(&default_sigma2_prints, config_flags);

      if ((ret == 0) && (memcmp(&default_sigma2_prints, &sigma2_prints, sizeof(sigma2_prints))!=0))
         moca_nv_print_sigma2_prints(cli, &sigma2_prints, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_bad_probe_prints( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_bad_probe_prints;
   uint32_t bad_probe_prints;
   memset(&default_bad_probe_prints, 0, sizeof(default_bad_probe_prints));
   memset(&bad_probe_prints, 0, sizeof(bad_probe_prints));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_bad_probe_prints(handle, &bad_probe_prints);
      moca_set_bad_probe_prints_defaults(&default_bad_probe_prints, config_flags);

      if ((ret == 0) && (memcmp(&default_bad_probe_prints, &bad_probe_prints, sizeof(bad_probe_prints))!=0))
         moca_nv_print_bad_probe_prints(cli, &bad_probe_prints, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_const_tx_params( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   struct moca_const_tx_params default_const_tx_params;
   struct moca_const_tx_params const_tx_params;
   memset(&default_const_tx_params, 0, sizeof(default_const_tx_params));
   memset(&const_tx_params, 0, sizeof(const_tx_params));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_const_tx_params(handle, &const_tx_params);
      moca_set_const_tx_params_defaults(&default_const_tx_params, config_flags);

      if ((ret == 0) && (memcmp(&default_const_tx_params, &const_tx_params, sizeof(const_tx_params))!=0))
         moca_nv_print_const_tx_params(cli, &const_tx_params, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_moca_core_trace_enable( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_moca_core_trace_enable;
   uint32_t moca_core_trace_enable;
   memset(&default_moca_core_trace_enable, 0, sizeof(default_moca_core_trace_enable));
   memset(&moca_core_trace_enable, 0, sizeof(moca_core_trace_enable));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_moca_core_trace_enable(handle, &moca_core_trace_enable);
      moca_set_moca_core_trace_enable_defaults(&default_moca_core_trace_enable, config_flags);

      if ((ret == 0) && (memcmp(&default_moca_core_trace_enable, &moca_core_trace_enable, sizeof(moca_core_trace_enable))!=0))
         moca_nv_print_moca_core_trace_enable(cli, &moca_core_trace_enable, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_dont_start_moca( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_dont_start_moca;
   uint32_t dont_start_moca;
   memset(&default_dont_start_moca, 0, sizeof(default_dont_start_moca));
   memset(&dont_start_moca, 0, sizeof(dont_start_moca));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_dont_start_moca(handle, &dont_start_moca);
      moca_set_dont_start_moca_defaults(&default_dont_start_moca, config_flags);

      if ((ret == 0) && (memcmp(&default_dont_start_moca, &dont_start_moca, sizeof(dont_start_moca))!=0))
         moca_nv_print_dont_start_moca(cli, &dont_start_moca, max_len);
}

// Group lab
MOCALIB_GEN_GET_FUNCTION int moca_get_lab_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_LAB_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_lab_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_LAB_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_lab_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--lab_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_nc_mode(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_NC_MODE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_nc_mode(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_NC_MODE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_nc_mode (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--nc_mode ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_rx_tx_packets_per_qm(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RX_TX_PACKETS_PER_QM, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_rx_tx_packets_per_qm(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RX_TX_PACKETS_PER_QM, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_rx_tx_packets_per_qm (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--rx_tx_packets_per_qm ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_extra_rx_packets_per_qm(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_EXTRA_RX_PACKETS_PER_QM, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_extra_rx_packets_per_qm(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_EXTRA_RX_PACKETS_PER_QM, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_extra_rx_packets_per_qm (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--extra_rx_packets_per_qm ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_target_phy_rate_20(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_TARGET_PHY_RATE_20, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_target_phy_rate_20(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_TARGET_PHY_RATE_20, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_target_phy_rate_20 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--target_phy_rate_20 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_target_phy_rate_20_turbo(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_TARGET_PHY_RATE_20_TURBO, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_target_phy_rate_20_turbo(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_TARGET_PHY_RATE_20_TURBO, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_target_phy_rate_20_turbo (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--target_phy_rate_20_turbo ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_turbo_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_TURBO_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_turbo_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_TURBO_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_turbo_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--turbo_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res1(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES1, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res1(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES1, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res1 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res1 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res2(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES2, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res2(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES2, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res2 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res2 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res3(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES3, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res3(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES3, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res3 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res3 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res4(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES4, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res4(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES4, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res4 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res4 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res5(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES5, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res5(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES5, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res5 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res5 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res6(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES6, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res6(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES6, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res6 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res6 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res7(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES7, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res7(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES7, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res7 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res7 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res8(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES8, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res8(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES8, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res8 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res8 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res9(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES9, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res9(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES9, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res9 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res9 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_init1(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_INIT1, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_init1(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_INIT1, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_init1 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--init1 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_init2(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_INIT2, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_init2(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_INIT2, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_init2 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--init2 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_init3(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_INIT3, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_init3(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_INIT3, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_init3 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--init3 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_init4(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_INIT4, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_init4(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_INIT4, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_init4 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--init4 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_init5(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_INIT5, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_init5(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_INIT5, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_init5 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--init5 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_init6(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_INIT6, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_init6(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_INIT6, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_init6 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--init6 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_init7(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_INIT7, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_init7(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_INIT7, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_init7 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--init7 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_init8(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_INIT8, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_init8(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_INIT8, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_init8 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--init8 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_init9(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_INIT9, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_init9(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_INIT9, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_init9 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--init9 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_lab_snr_graph_set(void *vctx, uint32_t *node_id)
{
   return(moca_get(vctx, IE_LAB_SNR_GRAPH_SET, node_id, sizeof(*node_id)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_lab_snr_graph_set(void *vctx, uint32_t node_id)
{
   return(moca_set(vctx, IE_LAB_SNR_GRAPH_SET, &node_id, sizeof(node_id)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_lab_snr_graph_set (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--lab_snr_graph_set ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_block_nack_rate(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_BLOCK_NACK_RATE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_block_nack_rate(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_BLOCK_NACK_RATE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_block_nack_rate (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--block_nack_rate ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res10(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES10, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res10(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES10, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res10 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res10 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res11(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES11, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res11(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES11, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res11 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res11 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res12(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES12, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res12(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES12, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res12 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res12 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res13(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES13, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res13(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES13, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res13 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res13 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res14(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES14, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res14(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES14, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res14 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res14 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res15(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES15, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res15(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES15, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res15 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res15 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res16(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES16, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res16(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES16, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res16 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res16 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res17(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES17, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res17(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES17, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res17 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res17 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res18(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES18, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res18(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES18, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res18 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res18 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res19(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES19, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res19(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES19, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res19 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res19 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_res20(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_RES20, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_res20(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_RES20, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_res20 (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--res20 ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_target_phy_rate_20_turbo_vlper(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_TARGET_PHY_RATE_20_TURBO_VLPER, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_target_phy_rate_20_turbo_vlper(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_TARGET_PHY_RATE_20_TURBO_VLPER, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_target_phy_rate_20_turbo_vlper (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--target_phy_rate_20_turbo_vlper ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_target_phy_rate_20_sec_ch(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_TARGET_PHY_RATE_20_SEC_CH, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_target_phy_rate_20_sec_ch(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_TARGET_PHY_RATE_20_SEC_CH, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_target_phy_rate_20_sec_ch (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--target_phy_rate_20_sec_ch ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_target_phy_rate_20_turbo_sec_ch(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_TARGET_PHY_RATE_20_TURBO_SEC_CH, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_target_phy_rate_20_turbo_sec_ch(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_TARGET_PHY_RATE_20_TURBO_SEC_CH, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_target_phy_rate_20_turbo_sec_ch (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--target_phy_rate_20_turbo_sec_ch ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_target_phy_rate_20_turbo_vlper_sec_ch(void *vctx, uint32_t *mbps)
{
   return(moca_get(vctx, IE_TARGET_PHY_RATE_20_TURBO_VLPER_SEC_CH, mbps, sizeof(*mbps)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_target_phy_rate_20_turbo_vlper_sec_ch(void *vctx, uint32_t mbps)
{
   return(moca_set(vctx, IE_TARGET_PHY_RATE_20_TURBO_VLPER_SEC_CH, &mbps, sizeof(mbps)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_target_phy_rate_20_turbo_vlper_sec_ch (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--target_phy_rate_20_turbo_vlper_sec_ch ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_cap_phy_rate_en(void *vctx, uint32_t *bool_val)
{
   return(moca_get(vctx, IE_CAP_PHY_RATE_EN, bool_val, sizeof(*bool_val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_cap_phy_rate_en(void *vctx, uint32_t bool_val)
{
   return(moca_set(vctx, IE_CAP_PHY_RATE_EN, &bool_val, sizeof(bool_val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_cap_phy_rate_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--cap_phy_rate_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_cap_target_phy_rate(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_CAP_TARGET_PHY_RATE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_cap_target_phy_rate(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_CAP_TARGET_PHY_RATE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_cap_target_phy_rate (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--cap_target_phy_rate ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_cap_snr_base_margin(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_CAP_SNR_BASE_MARGIN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_cap_snr_base_margin(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_CAP_SNR_BASE_MARGIN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_cap_snr_base_margin (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--cap_snr_base_margin ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_map_capture(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MAP_CAPTURE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_map_capture(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MAP_CAPTURE, &val, sizeof(val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_lab_pilots(void *vctx, struct moca_lab_pilots *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_LAB_PILOTS, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_lab_iq_diagram_set(void *vctx, const struct moca_lab_iq_diagram_set *in)
{
   return(moca_set(vctx, IE_LAB_IQ_DIAGRAM_SET, in, sizeof(*in)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_lab_iq_diagram_set (char *cli, struct moca_lab_iq_diagram_set * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--lab_iq_diagram_set ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "nodeid %u  ", (unsigned int) in->nodeid);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "bursttype %u  ", (unsigned int) in->bursttype);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "acmtsymnum %u  ", (unsigned int) in->acmtsymnum);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_lab_register(void *vctx, uint32_t address, uint32_t *data)
{
   return(moca_get_inout(vctx, IE_LAB_REGISTER, &address, sizeof(address), data, sizeof(*data)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_lab_register(void *vctx, const struct moca_lab_register *in)
{
   return(moca_set(vctx, IE_LAB_REGISTER, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_lab_cmd(void *vctx, uint32_t index, uint32_t *retval)
{
   return(moca_get_inout(vctx, IE_LAB_CMD, &index, sizeof(index), retval, sizeof(*retval)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_lab_cmd(void *vctx, const struct moca_lab_cmd *in)
{
   return(moca_set(vctx, IE_LAB_CMD, in, sizeof(*in)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_lab_tpcap(void *vctx, const struct moca_lab_tpcap *in)
{
   return(moca_set(vctx, IE_LAB_TPCAP, in, sizeof(*in)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_lab_tpcap (char *cli, struct moca_lab_tpcap * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--lab_tpcap ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "enable %u  ", (unsigned int) in->enable);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "type %u  ", (unsigned int) in->type);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "type_2 %u  ", (unsigned int) in->type_2);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "stopBurstType %u  ", (unsigned int) in->stopBurstType);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_host_pool(void *vctx, struct moca_host_pool *out)
{
   int ret;
   ret = moca_get(vctx, IE_HOST_POOL, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int moca_set_force_handoff(void *vctx, const struct moca_force_handoff *in)
{
   return(moca_set(vctx, IE_FORCE_HANDOFF, in, sizeof(*in)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_tpcap_capture_time(void *vctx, uint32_t *last_time_tpcap_cap)
{
   return(moca_get(vctx, IE_TPCAP_CAPTURE_TIME, last_time_tpcap_cap, sizeof(*last_time_tpcap_cap)));
}

static inline int mocalib_handle_assert(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_assert *in = (struct moca_assert *)vin;
   if(in_len > sizeof(*in))
      return(-2);
   __moca_copy_be32(in, in, sizeof(*in));
   if(ctx->cb.assert_cb == NULL)
      return(-4);
   ctx->cb.assert_cb(ctx->cb.assert_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_assert_cb(void *vctx, void (*callback)(void *userarg, struct moca_assert *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.assert_cb = callback;
   ctx->cb.assert_userarg = userarg;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_message(void *vctx, uint32_t id)
{
   return(moca_set(vctx, IE_MESSAGE, &id, sizeof(id)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_snr_data(void *vctx, struct moca_snr_data *out)
{
   int ret;
   ret = moca_get(vctx, IE_SNR_DATA, out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int moca_get_iq_data(void *vctx, struct moca_iq_data *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_IQ_DATA, out, sizeof(*out));

   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_cir_data(void *vctx, uint32_t node, struct moca_cir_data *out)
{
   int ret;
   ret = moca_get_inout(vctx, IE_CIR_DATA, &node, sizeof(node), out, sizeof(*out));
   return(ret);
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_assert_restart(void *vctx, uint32_t *enable)
{
   return(moca_get(vctx, IE_ASSERT_RESTART, enable, sizeof(*enable)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_assert_restart(void *vctx, uint32_t enable)
{
   return(moca_set(vctx, IE_ASSERT_RESTART, &enable, sizeof(enable)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_assert_restart (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--assert_restart ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int __moca_get_moca_cpu_freq(void *vctx, uint32_t *hz)
{
   return(moca_get(vctx, IE_MOCA_CPU_FREQ, hz, sizeof(*hz)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_moca_cpu_freq(void *vctx, uint32_t hz)
{
   return(moca_set(vctx, IE_MOCA_CPU_FREQ, &hz, sizeof(hz)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_moca_cpu_freq (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--moca_cpu_freq ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_lab_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_lab_mode;
   uint32_t lab_mode;
   memset(&default_lab_mode, 0, sizeof(default_lab_mode));
   memset(&lab_mode, 0, sizeof(lab_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_lab_mode(handle, &lab_mode);
      moca_set_lab_mode_defaults(&default_lab_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_lab_mode, &lab_mode, sizeof(lab_mode))!=0))
         moca_nv_print_lab_mode(cli, &lab_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_nc_mode( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_nc_mode;
   uint32_t nc_mode;
   memset(&default_nc_mode, 0, sizeof(default_nc_mode));
   memset(&nc_mode, 0, sizeof(nc_mode));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_nc_mode(handle, &nc_mode);
      moca_set_nc_mode_defaults(&default_nc_mode, config_flags);

      if ((ret == 0) && (memcmp(&default_nc_mode, &nc_mode, sizeof(nc_mode))!=0))
         moca_nv_print_nc_mode(cli, &nc_mode, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_rx_tx_packets_per_qm( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_rx_tx_packets_per_qm;
   uint32_t rx_tx_packets_per_qm;
   memset(&default_rx_tx_packets_per_qm, 0, sizeof(default_rx_tx_packets_per_qm));
   memset(&rx_tx_packets_per_qm, 0, sizeof(rx_tx_packets_per_qm));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_rx_tx_packets_per_qm(handle, &rx_tx_packets_per_qm);
      moca_set_rx_tx_packets_per_qm_defaults(&default_rx_tx_packets_per_qm, config_flags);

      if ((ret == 0) && (memcmp(&default_rx_tx_packets_per_qm, &rx_tx_packets_per_qm, sizeof(rx_tx_packets_per_qm))!=0))
         moca_nv_print_rx_tx_packets_per_qm(cli, &rx_tx_packets_per_qm, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_extra_rx_packets_per_qm( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_extra_rx_packets_per_qm;
   uint32_t extra_rx_packets_per_qm;
   memset(&default_extra_rx_packets_per_qm, 0, sizeof(default_extra_rx_packets_per_qm));
   memset(&extra_rx_packets_per_qm, 0, sizeof(extra_rx_packets_per_qm));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_extra_rx_packets_per_qm(handle, &extra_rx_packets_per_qm);
      moca_set_extra_rx_packets_per_qm_defaults(&default_extra_rx_packets_per_qm, config_flags);

      if ((ret == 0) && (memcmp(&default_extra_rx_packets_per_qm, &extra_rx_packets_per_qm, sizeof(extra_rx_packets_per_qm))!=0))
         moca_nv_print_extra_rx_packets_per_qm(cli, &extra_rx_packets_per_qm, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_target_phy_rate_20( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_target_phy_rate_20;
   uint32_t target_phy_rate_20;
   memset(&default_target_phy_rate_20, 0, sizeof(default_target_phy_rate_20));
   memset(&target_phy_rate_20, 0, sizeof(target_phy_rate_20));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_target_phy_rate_20(handle, &target_phy_rate_20);
      moca_set_target_phy_rate_20_defaults(&default_target_phy_rate_20, config_flags);

      if ((ret == 0) && (memcmp(&default_target_phy_rate_20, &target_phy_rate_20, sizeof(target_phy_rate_20))!=0))
         moca_nv_print_target_phy_rate_20(cli, &target_phy_rate_20, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_target_phy_rate_20_turbo( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_target_phy_rate_20_turbo;
   uint32_t target_phy_rate_20_turbo;
   memset(&default_target_phy_rate_20_turbo, 0, sizeof(default_target_phy_rate_20_turbo));
   memset(&target_phy_rate_20_turbo, 0, sizeof(target_phy_rate_20_turbo));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_target_phy_rate_20_turbo(handle, &target_phy_rate_20_turbo);
      moca_set_target_phy_rate_20_turbo_defaults(&default_target_phy_rate_20_turbo, config_flags);

      if ((ret == 0) && (memcmp(&default_target_phy_rate_20_turbo, &target_phy_rate_20_turbo, sizeof(target_phy_rate_20_turbo))!=0))
         moca_nv_print_target_phy_rate_20_turbo(cli, &target_phy_rate_20_turbo, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_turbo_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_turbo_en;
   uint32_t turbo_en;
   memset(&default_turbo_en, 0, sizeof(default_turbo_en));
   memset(&turbo_en, 0, sizeof(turbo_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_turbo_en(handle, &turbo_en);
      moca_set_turbo_en_defaults(&default_turbo_en, config_flags);

      if ((ret == 0) && (memcmp(&default_turbo_en, &turbo_en, sizeof(turbo_en))!=0))
         moca_nv_print_turbo_en(cli, &turbo_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res1( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res1;
   uint32_t res1;
   memset(&default_res1, 0, sizeof(default_res1));
   memset(&res1, 0, sizeof(res1));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res1(handle, &res1);
      moca_set_res1_defaults(&default_res1, config_flags);

      if ((ret == 0) && (memcmp(&default_res1, &res1, sizeof(res1))!=0))
         moca_nv_print_res1(cli, &res1, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res2( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res2;
   uint32_t res2;
   memset(&default_res2, 0, sizeof(default_res2));
   memset(&res2, 0, sizeof(res2));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res2(handle, &res2);
      moca_set_res2_defaults(&default_res2, config_flags);

      if ((ret == 0) && (memcmp(&default_res2, &res2, sizeof(res2))!=0))
         moca_nv_print_res2(cli, &res2, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res3( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res3;
   uint32_t res3;
   memset(&default_res3, 0, sizeof(default_res3));
   memset(&res3, 0, sizeof(res3));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res3(handle, &res3);
      moca_set_res3_defaults(&default_res3, config_flags);

      if ((ret == 0) && (memcmp(&default_res3, &res3, sizeof(res3))!=0))
         moca_nv_print_res3(cli, &res3, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res4( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res4;
   uint32_t res4;
   memset(&default_res4, 0, sizeof(default_res4));
   memset(&res4, 0, sizeof(res4));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res4(handle, &res4);
      moca_set_res4_defaults(&default_res4, config_flags);

      if ((ret == 0) && (memcmp(&default_res4, &res4, sizeof(res4))!=0))
         moca_nv_print_res4(cli, &res4, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res5( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res5;
   uint32_t res5;
   memset(&default_res5, 0, sizeof(default_res5));
   memset(&res5, 0, sizeof(res5));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res5(handle, &res5);
      moca_set_res5_defaults(&default_res5, config_flags);

      if ((ret == 0) && (memcmp(&default_res5, &res5, sizeof(res5))!=0))
         moca_nv_print_res5(cli, &res5, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res6( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res6;
   uint32_t res6;
   memset(&default_res6, 0, sizeof(default_res6));
   memset(&res6, 0, sizeof(res6));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res6(handle, &res6);
      moca_set_res6_defaults(&default_res6, config_flags);

      if ((ret == 0) && (memcmp(&default_res6, &res6, sizeof(res6))!=0))
         moca_nv_print_res6(cli, &res6, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res7( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res7;
   uint32_t res7;
   memset(&default_res7, 0, sizeof(default_res7));
   memset(&res7, 0, sizeof(res7));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res7(handle, &res7);
      moca_set_res7_defaults(&default_res7, config_flags);

      if ((ret == 0) && (memcmp(&default_res7, &res7, sizeof(res7))!=0))
         moca_nv_print_res7(cli, &res7, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res8( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res8;
   uint32_t res8;
   memset(&default_res8, 0, sizeof(default_res8));
   memset(&res8, 0, sizeof(res8));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res8(handle, &res8);
      moca_set_res8_defaults(&default_res8, config_flags);

      if ((ret == 0) && (memcmp(&default_res8, &res8, sizeof(res8))!=0))
         moca_nv_print_res8(cli, &res8, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res9( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res9;
   uint32_t res9;
   memset(&default_res9, 0, sizeof(default_res9));
   memset(&res9, 0, sizeof(res9));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res9(handle, &res9);
      moca_set_res9_defaults(&default_res9, config_flags);

      if ((ret == 0) && (memcmp(&default_res9, &res9, sizeof(res9))!=0))
         moca_nv_print_res9(cli, &res9, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_init1( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_init1;
   uint32_t init1;
   memset(&default_init1, 0, sizeof(default_init1));
   memset(&init1, 0, sizeof(init1));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_init1(handle, &init1);
      moca_set_init1_defaults(&default_init1, config_flags);

      if ((ret == 0) && (memcmp(&default_init1, &init1, sizeof(init1))!=0))
         moca_nv_print_init1(cli, &init1, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_init2( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_init2;
   uint32_t init2;
   memset(&default_init2, 0, sizeof(default_init2));
   memset(&init2, 0, sizeof(init2));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_init2(handle, &init2);
      moca_set_init2_defaults(&default_init2, config_flags);

      if ((ret == 0) && (memcmp(&default_init2, &init2, sizeof(init2))!=0))
         moca_nv_print_init2(cli, &init2, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_init3( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_init3;
   uint32_t init3;
   memset(&default_init3, 0, sizeof(default_init3));
   memset(&init3, 0, sizeof(init3));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_init3(handle, &init3);
      moca_set_init3_defaults(&default_init3, config_flags);

      if ((ret == 0) && (memcmp(&default_init3, &init3, sizeof(init3))!=0))
         moca_nv_print_init3(cli, &init3, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_init4( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_init4;
   uint32_t init4;
   memset(&default_init4, 0, sizeof(default_init4));
   memset(&init4, 0, sizeof(init4));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_init4(handle, &init4);
      moca_set_init4_defaults(&default_init4, config_flags);

      if ((ret == 0) && (memcmp(&default_init4, &init4, sizeof(init4))!=0))
         moca_nv_print_init4(cli, &init4, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_init5( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_init5;
   uint32_t init5;
   memset(&default_init5, 0, sizeof(default_init5));
   memset(&init5, 0, sizeof(init5));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_init5(handle, &init5);
      moca_set_init5_defaults(&default_init5, config_flags);

      if ((ret == 0) && (memcmp(&default_init5, &init5, sizeof(init5))!=0))
         moca_nv_print_init5(cli, &init5, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_init6( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_init6;
   uint32_t init6;
   memset(&default_init6, 0, sizeof(default_init6));
   memset(&init6, 0, sizeof(init6));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_init6(handle, &init6);
      moca_set_init6_defaults(&default_init6, config_flags);

      if ((ret == 0) && (memcmp(&default_init6, &init6, sizeof(init6))!=0))
         moca_nv_print_init6(cli, &init6, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_init7( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_init7;
   uint32_t init7;
   memset(&default_init7, 0, sizeof(default_init7));
   memset(&init7, 0, sizeof(init7));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_init7(handle, &init7);
      moca_set_init7_defaults(&default_init7, config_flags);

      if ((ret == 0) && (memcmp(&default_init7, &init7, sizeof(init7))!=0))
         moca_nv_print_init7(cli, &init7, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_init8( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_init8;
   uint32_t init8;
   memset(&default_init8, 0, sizeof(default_init8));
   memset(&init8, 0, sizeof(init8));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_init8(handle, &init8);
      moca_set_init8_defaults(&default_init8, config_flags);

      if ((ret == 0) && (memcmp(&default_init8, &init8, sizeof(init8))!=0))
         moca_nv_print_init8(cli, &init8, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_init9( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_init9;
   uint32_t init9;
   memset(&default_init9, 0, sizeof(default_init9));
   memset(&init9, 0, sizeof(init9));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_init9(handle, &init9);
      moca_set_init9_defaults(&default_init9, config_flags);

      if ((ret == 0) && (memcmp(&default_init9, &init9, sizeof(init9))!=0))
         moca_nv_print_init9(cli, &init9, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_lab_snr_graph_set( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_lab_snr_graph_set;
   uint32_t lab_snr_graph_set;
   memset(&default_lab_snr_graph_set, 0, sizeof(default_lab_snr_graph_set));
   memset(&lab_snr_graph_set, 0, sizeof(lab_snr_graph_set));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_lab_snr_graph_set(handle, &lab_snr_graph_set);
      moca_set_lab_snr_graph_set_defaults(&default_lab_snr_graph_set, config_flags);

      if ((ret == 0) && (memcmp(&default_lab_snr_graph_set, &lab_snr_graph_set, sizeof(lab_snr_graph_set))!=0))
         moca_nv_print_lab_snr_graph_set(cli, &lab_snr_graph_set, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_block_nack_rate( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_block_nack_rate;
   uint32_t block_nack_rate;
   memset(&default_block_nack_rate, 0, sizeof(default_block_nack_rate));
   memset(&block_nack_rate, 0, sizeof(block_nack_rate));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_block_nack_rate(handle, &block_nack_rate);
      moca_set_block_nack_rate_defaults(&default_block_nack_rate, config_flags);

      if ((ret == 0) && (memcmp(&default_block_nack_rate, &block_nack_rate, sizeof(block_nack_rate))!=0))
         moca_nv_print_block_nack_rate(cli, &block_nack_rate, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res10( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res10;
   uint32_t res10;
   memset(&default_res10, 0, sizeof(default_res10));
   memset(&res10, 0, sizeof(res10));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res10(handle, &res10);
      moca_set_res10_defaults(&default_res10, config_flags);

      if ((ret == 0) && (memcmp(&default_res10, &res10, sizeof(res10))!=0))
         moca_nv_print_res10(cli, &res10, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res11( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res11;
   uint32_t res11;
   memset(&default_res11, 0, sizeof(default_res11));
   memset(&res11, 0, sizeof(res11));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res11(handle, &res11);
      moca_set_res11_defaults(&default_res11, config_flags);

      if ((ret == 0) && (memcmp(&default_res11, &res11, sizeof(res11))!=0))
         moca_nv_print_res11(cli, &res11, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res12( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res12;
   uint32_t res12;
   memset(&default_res12, 0, sizeof(default_res12));
   memset(&res12, 0, sizeof(res12));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res12(handle, &res12);
      moca_set_res12_defaults(&default_res12, config_flags);

      if ((ret == 0) && (memcmp(&default_res12, &res12, sizeof(res12))!=0))
         moca_nv_print_res12(cli, &res12, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res13( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res13;
   uint32_t res13;
   memset(&default_res13, 0, sizeof(default_res13));
   memset(&res13, 0, sizeof(res13));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res13(handle, &res13);
      moca_set_res13_defaults(&default_res13, config_flags);

      if ((ret == 0) && (memcmp(&default_res13, &res13, sizeof(res13))!=0))
         moca_nv_print_res13(cli, &res13, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res14( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res14;
   uint32_t res14;
   memset(&default_res14, 0, sizeof(default_res14));
   memset(&res14, 0, sizeof(res14));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res14(handle, &res14);
      moca_set_res14_defaults(&default_res14, config_flags);

      if ((ret == 0) && (memcmp(&default_res14, &res14, sizeof(res14))!=0))
         moca_nv_print_res14(cli, &res14, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res15( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res15;
   uint32_t res15;
   memset(&default_res15, 0, sizeof(default_res15));
   memset(&res15, 0, sizeof(res15));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res15(handle, &res15);
      moca_set_res15_defaults(&default_res15, config_flags);

      if ((ret == 0) && (memcmp(&default_res15, &res15, sizeof(res15))!=0))
         moca_nv_print_res15(cli, &res15, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res16( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res16;
   uint32_t res16;
   memset(&default_res16, 0, sizeof(default_res16));
   memset(&res16, 0, sizeof(res16));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res16(handle, &res16);
      moca_set_res16_defaults(&default_res16, config_flags);

      if ((ret == 0) && (memcmp(&default_res16, &res16, sizeof(res16))!=0))
         moca_nv_print_res16(cli, &res16, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res17( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res17;
   uint32_t res17;
   memset(&default_res17, 0, sizeof(default_res17));
   memset(&res17, 0, sizeof(res17));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res17(handle, &res17);
      moca_set_res17_defaults(&default_res17, config_flags);

      if ((ret == 0) && (memcmp(&default_res17, &res17, sizeof(res17))!=0))
         moca_nv_print_res17(cli, &res17, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res18( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res18;
   uint32_t res18;
   memset(&default_res18, 0, sizeof(default_res18));
   memset(&res18, 0, sizeof(res18));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res18(handle, &res18);
      moca_set_res18_defaults(&default_res18, config_flags);

      if ((ret == 0) && (memcmp(&default_res18, &res18, sizeof(res18))!=0))
         moca_nv_print_res18(cli, &res18, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res19( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res19;
   uint32_t res19;
   memset(&default_res19, 0, sizeof(default_res19));
   memset(&res19, 0, sizeof(res19));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res19(handle, &res19);
      moca_set_res19_defaults(&default_res19, config_flags);

      if ((ret == 0) && (memcmp(&default_res19, &res19, sizeof(res19))!=0))
         moca_nv_print_res19(cli, &res19, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_res20( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_res20;
   uint32_t res20;
   memset(&default_res20, 0, sizeof(default_res20));
   memset(&res20, 0, sizeof(res20));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_res20(handle, &res20);
      moca_set_res20_defaults(&default_res20, config_flags);

      if ((ret == 0) && (memcmp(&default_res20, &res20, sizeof(res20))!=0))
         moca_nv_print_res20(cli, &res20, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_target_phy_rate_20_turbo_vlper( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_target_phy_rate_20_turbo_vlper;
   uint32_t target_phy_rate_20_turbo_vlper;
   memset(&default_target_phy_rate_20_turbo_vlper, 0, sizeof(default_target_phy_rate_20_turbo_vlper));
   memset(&target_phy_rate_20_turbo_vlper, 0, sizeof(target_phy_rate_20_turbo_vlper));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_target_phy_rate_20_turbo_vlper(handle, &target_phy_rate_20_turbo_vlper);
      moca_set_target_phy_rate_20_turbo_vlper_defaults(&default_target_phy_rate_20_turbo_vlper, config_flags);

      if ((ret == 0) && (memcmp(&default_target_phy_rate_20_turbo_vlper, &target_phy_rate_20_turbo_vlper, sizeof(target_phy_rate_20_turbo_vlper))!=0))
         moca_nv_print_target_phy_rate_20_turbo_vlper(cli, &target_phy_rate_20_turbo_vlper, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_target_phy_rate_20_sec_ch( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_target_phy_rate_20_sec_ch;
   uint32_t target_phy_rate_20_sec_ch;
   memset(&default_target_phy_rate_20_sec_ch, 0, sizeof(default_target_phy_rate_20_sec_ch));
   memset(&target_phy_rate_20_sec_ch, 0, sizeof(target_phy_rate_20_sec_ch));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_target_phy_rate_20_sec_ch(handle, &target_phy_rate_20_sec_ch);
      moca_set_target_phy_rate_20_sec_ch_defaults(&default_target_phy_rate_20_sec_ch, config_flags);

      if ((ret == 0) && (memcmp(&default_target_phy_rate_20_sec_ch, &target_phy_rate_20_sec_ch, sizeof(target_phy_rate_20_sec_ch))!=0))
         moca_nv_print_target_phy_rate_20_sec_ch(cli, &target_phy_rate_20_sec_ch, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_target_phy_rate_20_turbo_sec_ch( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_target_phy_rate_20_turbo_sec_ch;
   uint32_t target_phy_rate_20_turbo_sec_ch;
   memset(&default_target_phy_rate_20_turbo_sec_ch, 0, sizeof(default_target_phy_rate_20_turbo_sec_ch));
   memset(&target_phy_rate_20_turbo_sec_ch, 0, sizeof(target_phy_rate_20_turbo_sec_ch));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_target_phy_rate_20_turbo_sec_ch(handle, &target_phy_rate_20_turbo_sec_ch);
      moca_set_target_phy_rate_20_turbo_sec_ch_defaults(&default_target_phy_rate_20_turbo_sec_ch, config_flags);

      if ((ret == 0) && (memcmp(&default_target_phy_rate_20_turbo_sec_ch, &target_phy_rate_20_turbo_sec_ch, sizeof(target_phy_rate_20_turbo_sec_ch))!=0))
         moca_nv_print_target_phy_rate_20_turbo_sec_ch(cli, &target_phy_rate_20_turbo_sec_ch, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_target_phy_rate_20_turbo_vlper_sec_ch( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_target_phy_rate_20_turbo_vlper_sec_ch;
   uint32_t target_phy_rate_20_turbo_vlper_sec_ch;
   memset(&default_target_phy_rate_20_turbo_vlper_sec_ch, 0, sizeof(default_target_phy_rate_20_turbo_vlper_sec_ch));
   memset(&target_phy_rate_20_turbo_vlper_sec_ch, 0, sizeof(target_phy_rate_20_turbo_vlper_sec_ch));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_target_phy_rate_20_turbo_vlper_sec_ch(handle, &target_phy_rate_20_turbo_vlper_sec_ch);
      moca_set_target_phy_rate_20_turbo_vlper_sec_ch_defaults(&default_target_phy_rate_20_turbo_vlper_sec_ch, config_flags);

      if ((ret == 0) && (memcmp(&default_target_phy_rate_20_turbo_vlper_sec_ch, &target_phy_rate_20_turbo_vlper_sec_ch, sizeof(target_phy_rate_20_turbo_vlper_sec_ch))!=0))
         moca_nv_print_target_phy_rate_20_turbo_vlper_sec_ch(cli, &target_phy_rate_20_turbo_vlper_sec_ch, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_cap_phy_rate_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_cap_phy_rate_en;
   uint32_t cap_phy_rate_en;
   memset(&default_cap_phy_rate_en, 0, sizeof(default_cap_phy_rate_en));
   memset(&cap_phy_rate_en, 0, sizeof(cap_phy_rate_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_cap_phy_rate_en(handle, &cap_phy_rate_en);
      moca_set_cap_phy_rate_en_defaults(&default_cap_phy_rate_en, config_flags);

      if ((ret == 0) && (memcmp(&default_cap_phy_rate_en, &cap_phy_rate_en, sizeof(cap_phy_rate_en))!=0))
         moca_nv_print_cap_phy_rate_en(cli, &cap_phy_rate_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_cap_target_phy_rate( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_cap_target_phy_rate;
   uint32_t cap_target_phy_rate;
   memset(&default_cap_target_phy_rate, 0, sizeof(default_cap_target_phy_rate));
   memset(&cap_target_phy_rate, 0, sizeof(cap_target_phy_rate));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_cap_target_phy_rate(handle, &cap_target_phy_rate);
      moca_set_cap_target_phy_rate_defaults(&default_cap_target_phy_rate, config_flags);

      if ((ret == 0) && (memcmp(&default_cap_target_phy_rate, &cap_target_phy_rate, sizeof(cap_target_phy_rate))!=0))
         moca_nv_print_cap_target_phy_rate(cli, &cap_target_phy_rate, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_cap_snr_base_margin( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_cap_snr_base_margin;
   uint32_t cap_snr_base_margin;
   memset(&default_cap_snr_base_margin, 0, sizeof(default_cap_snr_base_margin));
   memset(&cap_snr_base_margin, 0, sizeof(cap_snr_base_margin));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_cap_snr_base_margin(handle, &cap_snr_base_margin);
      moca_set_cap_snr_base_margin_defaults(&default_cap_snr_base_margin, config_flags);

      if ((ret == 0) && (memcmp(&default_cap_snr_base_margin, &cap_snr_base_margin, sizeof(cap_snr_base_margin))!=0))
         moca_nv_print_cap_snr_base_margin(cli, &cap_snr_base_margin, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_assert_restart( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_assert_restart;
   uint32_t assert_restart;
   memset(&default_assert_restart, 0, sizeof(default_assert_restart));
   memset(&assert_restart, 0, sizeof(assert_restart));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_assert_restart(handle, &assert_restart);
      moca_set_assert_restart_defaults(&default_assert_restart, config_flags);

      if ((ret == 0) && (memcmp(&default_assert_restart, &assert_restart, sizeof(assert_restart))!=0))
         moca_nv_print_assert_restart(cli, &assert_restart, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_moca_cpu_freq( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_moca_cpu_freq;
   uint32_t moca_cpu_freq;
   memset(&default_moca_cpu_freq, 0, sizeof(default_moca_cpu_freq));
   memset(&moca_cpu_freq, 0, sizeof(moca_cpu_freq));
   (void)ret; //eliminate unused variable warning

      ret = __moca_get_moca_cpu_freq(handle, &moca_cpu_freq);
      moca_set_moca_cpu_freq_defaults(&default_moca_cpu_freq, config_flags);

      if ((ret == 0) && (memcmp(&default_moca_cpu_freq, &moca_cpu_freq, sizeof(moca_cpu_freq))!=0))
         moca_nv_print_moca_cpu_freq(cli, &moca_cpu_freq, max_len);
}

// Group mps
MOCALIB_GEN_GET_FUNCTION int moca_get_mps_en(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MPS_EN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mps_en(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MPS_EN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_mps_en (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--mps_en ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mps_privacy_receive(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MPS_PRIVACY_RECEIVE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mps_privacy_receive(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MPS_PRIVACY_RECEIVE, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_mps_privacy_receive (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--mps_privacy_receive ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mps_privacy_down(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MPS_PRIVACY_DOWN, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mps_privacy_down(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MPS_PRIVACY_DOWN, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_mps_privacy_down (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--mps_privacy_down ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mps_walk_time(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MPS_WALK_TIME, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mps_walk_time(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MPS_WALK_TIME, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_mps_walk_time (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--mps_walk_time ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mps_unpaired_time(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MPS_UNPAIRED_TIME, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mps_unpaired_time(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MPS_UNPAIRED_TIME, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_mps_unpaired_time (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--mps_unpaired_time ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mps_state(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_MPS_STATE, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_state(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MPS_STATE, &val, sizeof(val)));
}

MOCALIB_GEN_GET_FUNCTION int moca_get_mps_init_scan_payload(void *vctx, struct moca_mps_init_scan_payload *out)
{
   int ret;
   ret = moca_get_noswap(vctx, IE_MPS_INIT_SCAN_PAYLOAD, out, sizeof(*out));
   moca_mps_init_scan_payload_swap(out);

   return(ret);
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_trig(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_MPS_TRIG, &val, sizeof(val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_data(void *vctx, struct moca_mps_data *in)
{
   struct moca_mps_data tmp = *in;
   moca_mps_data_swap(&tmp);
   return(moca_set_noswap(vctx, IE_MPS_DATA, &tmp, sizeof(tmp)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_key(void *vctx, struct moca_mps_key *in)
{
   struct moca_mps_key tmp = *in;
   return(moca_set_noswap(vctx, IE_MPS_KEY, &tmp, sizeof(tmp)));
}

static inline int mocalib_handle_mps_privacy_changed(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   if(ctx->cb.mps_privacy_changed_cb == NULL)
      return(-3);
   ctx->cb.mps_privacy_changed_cb(ctx->cb.mps_privacy_changed_userarg);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_mps_privacy_changed_cb(void *vctx, void (*callback)(void *userarg), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.mps_privacy_changed_cb = callback;
   ctx->cb.mps_privacy_changed_userarg = userarg;
}

static inline int mocalib_handle_mps_trigger(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   if(ctx->cb.mps_trigger_cb == NULL)
      return(-3);
   ctx->cb.mps_trigger_cb(ctx->cb.mps_trigger_userarg);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_mps_trigger_cb(void *vctx, void (*callback)(void *userarg), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.mps_trigger_cb = callback;
   ctx->cb.mps_trigger_userarg = userarg;
}

static inline int mocalib_handle_mps_pair_fail(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   if(ctx->cb.mps_pair_fail_cb == NULL)
      return(-3);
   ctx->cb.mps_pair_fail_cb(ctx->cb.mps_pair_fail_userarg);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_mps_pair_fail_cb(void *vctx, void (*callback)(void *userarg), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.mps_pair_fail_cb = callback;
   ctx->cb.mps_pair_fail_userarg = userarg;
}

static inline int mocalib_handle_init_scan_rec(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   if(ctx->cb.init_scan_rec_cb == NULL)
      return(-3);
   ctx->cb.init_scan_rec_cb(ctx->cb.init_scan_rec_userarg);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_init_scan_rec_cb(void *vctx, void (*callback)(void *userarg), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.init_scan_rec_cb = callback;
   ctx->cb.init_scan_rec_userarg = userarg;
}

static inline int mocalib_handle_mps_request_mpskey(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   struct moca_mps_request_mpskey *in = (struct moca_mps_request_mpskey *)vin;
   if(in_len != sizeof(*in))
      return(-1);
   moca_mps_request_mpskey_swap(in);
   if(ctx->cb.mps_request_mpskey_cb == NULL)
      return(-5);
   ctx->cb.mps_request_mpskey_cb(ctx->cb.mps_request_mpskey_userarg, in);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_mps_request_mpskey_cb(void *vctx, void (*callback)(void *userarg, struct moca_mps_request_mpskey *out), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.mps_request_mpskey_cb = callback;
   ctx->cb.mps_request_mpskey_userarg = userarg;
}

static inline int mocalib_handle_mps_admission_nochange(void *vctx, void *vin, unsigned int in_len)
{
   struct moca_ctx *ctx = (struct moca_ctx*) vctx;
   if(ctx->cb.mps_admission_nochange_cb == NULL)
      return(-3);
   ctx->cb.mps_admission_nochange_cb(ctx->cb.mps_admission_nochange_userarg);
   return(0);
}

MOCALIB_GEN_REGISTER_FUNCTION void moca_register_mps_admission_nochange_cb(void *vctx, void (*callback)(void *userarg), void *userarg)
{
   struct moca_ctx *ctx = (struct moca_ctx *)vctx;
   ctx->cb.mps_admission_nochange_cb = callback;
   ctx->cb.mps_admission_nochange_userarg = userarg;
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mps_button_press(void *vctx)
{
   return(moca_set(vctx, IE_MPS_BUTTON_PRESS, NULL, 0));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_mps_reset(void *vctx)
{
   return(moca_set(vctx, IE_MPS_RESET, NULL, 0));
}

MOCALIB_GEN_SET_FUNCTION int moca_set_privacy_defaults(void *vctx, uint32_t val)
{
   return(moca_set(vctx, IE_PRIVACY_DEFAULTS, &val, sizeof(val)));
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_nv_print_privacy_defaults (char *cli, uint32_t * in, uint32_t * max_len)
{
   char *c = cli;
   uint32_t orig_max_len = *max_len;
   uint32_t bytes_written;

   bytes_written = snprintf(c, *max_len, "--privacy_defaults ");
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   bytes_written = snprintf (c + strlen(c), *max_len, "%u  ", (unsigned int) *in);
   if ((bytes_written >= *max_len) || (bytes_written == 0)) goto overrun;
   else *max_len -= bytes_written;

   return;

overrun:
   printf("%s: ran out of NV memory\n", __FUNCTION__);
   *max_len = orig_max_len;
   cli[0] = '\0';
   return;
}

MOCALIB_GEN_GET_FUNCTION int moca_get_privacy_defaults(void *vctx, uint32_t *val)
{
   return(moca_get(vctx, IE_PRIVACY_DEFAULTS, val, sizeof(*val)));
}

MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_data_ready(void *vctx)
{
   return(moca_set(vctx, IE_MPS_DATA_READY, NULL, 0));
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_mps_en( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_mps_en;
   uint32_t mps_en;
   memset(&default_mps_en, 0, sizeof(default_mps_en));
   memset(&mps_en, 0, sizeof(mps_en));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_mps_en(handle, &mps_en);
      moca_set_mps_en_defaults(&default_mps_en, config_flags);

      if ((ret == 0) && (memcmp(&default_mps_en, &mps_en, sizeof(mps_en))!=0))
         moca_nv_print_mps_en(cli, &mps_en, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_mps_privacy_receive( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_mps_privacy_receive;
   uint32_t mps_privacy_receive;
   memset(&default_mps_privacy_receive, 0, sizeof(default_mps_privacy_receive));
   memset(&mps_privacy_receive, 0, sizeof(mps_privacy_receive));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_mps_privacy_receive(handle, &mps_privacy_receive);
      moca_set_mps_privacy_receive_defaults(&default_mps_privacy_receive, config_flags);

      if ((ret == 0) && (memcmp(&default_mps_privacy_receive, &mps_privacy_receive, sizeof(mps_privacy_receive))!=0))
         moca_nv_print_mps_privacy_receive(cli, &mps_privacy_receive, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_mps_privacy_down( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_mps_privacy_down;
   uint32_t mps_privacy_down;
   memset(&default_mps_privacy_down, 0, sizeof(default_mps_privacy_down));
   memset(&mps_privacy_down, 0, sizeof(mps_privacy_down));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_mps_privacy_down(handle, &mps_privacy_down);
      moca_set_mps_privacy_down_defaults(&default_mps_privacy_down, config_flags);

      if ((ret == 0) && (memcmp(&default_mps_privacy_down, &mps_privacy_down, sizeof(mps_privacy_down))!=0))
         moca_nv_print_mps_privacy_down(cli, &mps_privacy_down, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_mps_walk_time( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_mps_walk_time;
   uint32_t mps_walk_time;
   memset(&default_mps_walk_time, 0, sizeof(default_mps_walk_time));
   memset(&mps_walk_time, 0, sizeof(mps_walk_time));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_mps_walk_time(handle, &mps_walk_time);
      moca_set_mps_walk_time_defaults(&default_mps_walk_time, config_flags);

      if ((ret == 0) && (memcmp(&default_mps_walk_time, &mps_walk_time, sizeof(mps_walk_time))!=0))
         moca_nv_print_mps_walk_time(cli, &mps_walk_time, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_mps_unpaired_time( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t default_mps_unpaired_time;
   uint32_t mps_unpaired_time;
   memset(&default_mps_unpaired_time, 0, sizeof(default_mps_unpaired_time));
   memset(&mps_unpaired_time, 0, sizeof(mps_unpaired_time));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_mps_unpaired_time(handle, &mps_unpaired_time);
      moca_set_mps_unpaired_time_defaults(&default_mps_unpaired_time, config_flags);

      if ((ret == 0) && (memcmp(&default_mps_unpaired_time, &mps_unpaired_time, sizeof(mps_unpaired_time))!=0))
         moca_nv_print_mps_unpaired_time(cli, &mps_unpaired_time, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION static void moca_nv_write_privacy_defaults( void * handle, uint32_t config_flags, char *cli, uint32_t * max_len)
{
   int ret=0;
   uint32_t privacy_defaults;
   memset(&privacy_defaults, 0, sizeof(privacy_defaults));
   (void)ret; //eliminate unused variable warning

      ret = moca_get_privacy_defaults(handle, &privacy_defaults);
      if (ret == 0)
         moca_nv_print_privacy_defaults(cli, &privacy_defaults, max_len);
}

MOCALIB_GEN_NVRAM_FUNCTION void moca_write_nvram( void * handle, char *c, uint32_t max_len)
{
   uint32_t config_flags;

   *c = '\0';
   __moca_get_config_flags(handle, &config_flags);

   moca_nv_write_ac_cc_shift(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_adc_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_aes_exchange_interval(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_agc_const_address(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_agc_const_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_aif_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_amp_type(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_arpl_th_100(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_arpl_th_50(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_assertText(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_assert_restart(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_bad_probe_prints(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_bandwidth(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_beacon_pwr_reduction(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_beacon_pwr_reduction_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_block_nack_rate(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_bo_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_bonding(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_brcmtag_enable(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_c4_moca20_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_cap_phy_rate_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_cap_snr_base_margin(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_cap_target_phy_rate(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_cir_prints(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_const_rx_submode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_const_tx_params(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_continuous_rx_mode_attn(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_cp_const(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_cp_margin_increase(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_dont_start_moca(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_egr_mc_filter_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_en_capable(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_extra_rx_packets_per_qm(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_fc_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_fragmentation(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_freq_shift(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_host_qos(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_if_access_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_if_access_table(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_ifg_moca20(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_impedance_mode_bonding(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_init1(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_init2(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_init3(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_init4(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_init5(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_init6(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_init7(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_init8(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_init9(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_lab_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_lab_snr_graph_set(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_led_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_limit_traffic(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_listening_duration(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_listening_freq_mask(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_lof_update(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_loopback_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_low_pri_q_num(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_m1_tx_power_variation(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_mac_aging(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_map_seed(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_constellation(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_constellation_all(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_frame_size(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_nbas_primary(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_nbas_secondary(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_phy_rate(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_phy_rate_50M(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_phy_rate_turbo(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_pkt_aggr(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_pkt_aggr_bonding(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_transmit_time(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_tx_power(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_tx_power_tune(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_max_tx_power_tune_sec_ch(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_mcfilter_enable(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_mfc_th_increase(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_min_bo_insert_2_bfm_lock(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_min_bw_alarm_threshold(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_min_snr_avg_db_2_bfm_lock(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_moca_core_trace_enable(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_moca_cpu_freq(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_mps_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_mps_privacy_down(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_mps_privacy_receive(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_mps_unpaired_time(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_mps_walk_time(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_mtm_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_multicast_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_nc_heartbeat_interval(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_nc_listening_interval(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_nc_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_nv_cal_enable(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_ofdma_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_ooo_lmo_threshold(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_orr_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_otf_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_password(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_pause_fc_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_per_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_phy_clock(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_pm_restore_on_link_down(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_pmk_exchange_interval(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_policing_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_power_save_mechanism_dis(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_pqos_max_packet_size(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_preamble_uc_const(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_preferred_nc(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_primary_ch_offset(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_priority_allocations(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_privacy_defaults(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_privacy_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_prof_pad_ctrl_deg_6802c0_bonding(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_prof_pad_ctrl_deg_6802c0_single(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_prop_bonding_compatibility_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_ps_swch_rx1(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_ps_swch_rx2(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_ps_swch_rx3(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_ps_swch_tx1(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_ps_swch_tx2(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_psm_config(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_qam256_capability(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rdeg_3450(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_remote_man(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res1(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res10(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res11(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res12(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res13(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res14(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res15(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res16(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res17(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res18(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res19(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res2(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res20(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res3(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res4(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res5(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res6(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res7(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res8(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_res9(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rework_6802(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rf_band(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rf_switch(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rlapm_cap_100(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rlapm_cap_50(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rlapm_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rlapm_table_100(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rlapm_table_50(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rtr_config(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rx_power_tune(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_rx_tx_packets_per_qm(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_sapm_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_sapm_table_100(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_sapm_table_50(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_sapm_table_sec(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_secondary_ch_offset(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_selective_rr(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_sigma2_prints(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_single_channel_operation(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_snr_margin_ldpc(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_snr_margin_ldpc_pre5(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_snr_margin_ldpc_pri_ch(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_snr_margin_ldpc_sec_ch(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_snr_margin_ofdma(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_snr_margin_pre5_pri_ch(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_snr_margin_pre5_sec_ch(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_snr_margin_rs(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_snr_prints(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_stag_priority(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_stag_removal(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_standby_power_state(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_star_topology_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_start_ulmo(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_taboo_channels(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_target_phy_rate_20(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_target_phy_rate_20_sec_ch(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_target_phy_rate_20_turbo(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_target_phy_rate_20_turbo_sec_ch(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_target_phy_rate_20_turbo_vlper(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_target_phy_rate_20_turbo_vlper_sec_ch(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_target_phy_rate_qam128(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_target_phy_rate_qam256(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_tek_exchange_interval(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_tlp_mode(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_tpc_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_turbo_en(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_use_ext_data_mem(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_wdog_enable(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_wom_magic_enable(handle, config_flags, c + strlen(c), &max_len);
   moca_nv_write_wom_mode(handle, config_flags, c + strlen(c), &max_len);


} /* moca_write_nvram */

static inline int mocalib_dispatch_event(void *vctx, uint32_t ie_type, void *vin, unsigned int in_len)
{
   switch(ie_type) {
      case IE_CORE_READY:
         return(mocalib_handle_core_ready(vctx, vin, in_len));
      case IE_POWER_UP_STATUS:
         return(mocalib_handle_power_up_status(vctx, vin, in_len));
      case IE_NEW_LOF:
         return(mocalib_handle_new_lof(vctx, vin, in_len));
      case IE_ADMISSION_COMPLETED:
         return(mocalib_handle_admission_completed(vctx, vin, in_len));
      case IE_TPCAP_DONE:
         return(mocalib_handle_tpcap_done(vctx, vin, in_len));
      case IE_MOCAD_FORWARDING_RX_PACKET:
         return(mocalib_handle_mocad_forwarding_rx_packet(vctx, vin, in_len));
      case IE_MOCAD_FORWARDING_TX_ACK:
         return(mocalib_handle_mocad_forwarding_tx_ack(vctx, vin, in_len));
      case IE_PR_DEGRADATION:
         return(mocalib_handle_pr_degradation(vctx, vin, in_len));
      case IE_UCFWD_UPDATE:
         return(mocalib_handle_ucfwd_update(vctx, vin, in_len));
      case IE_PQOS_MAINTENANCE_COMPLETE:
         return(mocalib_handle_pqos_maintenance_complete(vctx, vin, in_len));
      case IE_PQOS_CREATE_FLOW:
         return(mocalib_handle_pqos_create_flow(vctx, vin, in_len));
      case IE_PQOS_UPDATE_FLOW:
         return(mocalib_handle_pqos_update_flow(vctx, vin, in_len));
      case IE_PQOS_DELETE_FLOW:
         return(mocalib_handle_pqos_delete_flow(vctx, vin, in_len));
      case IE_PQOS_LIST:
         return(mocalib_handle_pqos_list(vctx, vin, in_len));
      case IE_PQOS_QUERY:
         return(mocalib_handle_pqos_query(vctx, vin, in_len));
      case IE_PQOS_STATUS:
         return(mocalib_handle_pqos_status(vctx, vin, in_len));
      case IE_ADMISSION_STATUS:
         return(mocalib_handle_admission_status(vctx, vin, in_len));
      case IE_LIMITED_BW:
         return(mocalib_handle_limited_bw(vctx, vin, in_len));
      case IE_LMO_INFO:
         return(mocalib_handle_lmo_info(vctx, vin, in_len));
      case IE_TOPOLOGY_CHANGED:
         return(mocalib_handle_topology_changed(vctx, vin, in_len));
      case IE_MOCA_VERSION_CHANGED:
         return(mocalib_handle_moca_version_changed(vctx, vin, in_len));
      case IE_MOCA_RESET_REQUEST:
         return(mocalib_handle_moca_reset_request(vctx, vin, in_len));
      case IE_NC_ID_CHANGED:
         return(mocalib_handle_nc_id_changed(vctx, vin, in_len));
      case IE_MR_EVENT:
         return(mocalib_handle_mr_event(vctx, vin, in_len));
      case IE_ACA:
         return(mocalib_handle_aca(vctx, vin, in_len));
      case IE_FMR_INIT:
         return(mocalib_handle_fmr_init(vctx, vin, in_len));
      case IE_MOCA_RESET:
         return(mocalib_handle_moca_reset(vctx, vin, in_len));
      case IE_DD_INIT:
         return(mocalib_handle_dd_init(vctx, vin, in_len));
      case IE_FMR_20:
         return(mocalib_handle_fmr_20(vctx, vin, in_len));
      case IE_HOSTLESS_MODE:
         return(mocalib_handle_hostless_mode(vctx, vin, in_len));
      case IE_WAKEUP_NODE:
         return(mocalib_handle_wakeup_node(vctx, vin, in_len));
      case IE_LINK_UP_STATE:
         return(mocalib_handle_link_up_state(vctx, vin, in_len));
      case IE_NEW_RF_BAND:
         return(mocalib_handle_new_rf_band(vctx, vin, in_len));
      case IE_POWER_STATE_RSP:
         return(mocalib_handle_power_state_rsp(vctx, vin, in_len));
      case IE_POWER_STATE_EVENT:
         return(mocalib_handle_power_state_event(vctx, vin, in_len));
      case IE_POWER_STATE_CAP:
         return(mocalib_handle_power_state_cap(vctx, vin, in_len));
      case IE_PS_CMD:
         return(mocalib_handle_ps_cmd(vctx, vin, in_len));
      case IE_KEY_CHANGED:
         return(mocalib_handle_key_changed(vctx, vin, in_len));
      case IE_ERROR:
         return(mocalib_handle_error(vctx, vin, in_len));
      case IE_ERROR_LOOKUP:
         return(mocalib_handle_error_lookup(vctx, vin, in_len));
      case IE_MOCAD_PRINTF:
         return(mocalib_handle_mocad_printf(vctx, vin, in_len));
      case IE_ASSERT:
         return(mocalib_handle_assert(vctx, vin, in_len));
      case IE_MPS_PRIVACY_CHANGED:
         return(mocalib_handle_mps_privacy_changed(vctx, vin, in_len));
      case IE_MPS_TRIGGER:
         return(mocalib_handle_mps_trigger(vctx, vin, in_len));
      case IE_MPS_PAIR_FAIL:
         return(mocalib_handle_mps_pair_fail(vctx, vin, in_len));
      case IE_INIT_SCAN_REC:
         return(mocalib_handle_init_scan_rec(vctx, vin, in_len));
      case IE_MPS_REQUEST_MPSKEY:
         return(mocalib_handle_mps_request_mpskey(vctx, vin, in_len));
      case IE_MPS_ADMISSION_NOCHANGE:
         return(mocalib_handle_mps_admission_nochange(vctx, vin, in_len));
   }
   return(-6);
}

