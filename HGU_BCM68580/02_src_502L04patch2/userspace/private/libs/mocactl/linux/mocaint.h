/******************************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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
 *
 ************************************************************************/
/***************************************************************************
 *
 *     Copyright (c) 2008-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *  Description: Internal declarations for libmoca and mocad
 *
 ***************************************************************************/

#ifndef _MOCAINT_H_
#define _MOCAINT_H_

#include "moca_os.h"

#include <stdint.h>
#include "bcmendian.h"



#ifdef __cplusplus
extern "C" {
#endif

#if defined(DSL_MOCA)
#define MOCA_CMD_SOCK_FMT	"/var/moca-cmd.%s"
#define MOCA_EVT_SOCK_FMT	"/var/moca-evt.%s"
#else
#define MOCA_CMD_SOCK_FMT	"/tmp/moca-cmd.%s"
#define MOCA_EVT_SOCK_FMT	"/tmp/moca-evt.%s"
#endif
#define MOCA_FILENAME_LEN	64
#define MOCA_MAX_CORE_RSP  512
#define MOCA_BUF_LEN		528
#define MOCA_GMII_BUF_LEN	1520 // make multiple of 4 bytes
#define MOCA_BIG_BUF_LEN	4112  // increase for SNR data: (SNR_DATA_SIZE+16)
#define MOCA_DEFAULT_IFNAME	"default"
#define MOCA_DEFAULT_DEV	"/dev/bmoca0"

#define FL_SWAP_RD		0x00000001
#define FL_SWAP_WR		0x00000002

#define MOCACORE_PATH_0		"/etc/moca/mocacore.bin"
#define MOCACORE_PATH_1		"mocacore.bin"
#define MOCACORE_PATH_GEN_1 "/etc/moca/mocacore-gen1.bin"
#define MOCACORE_PATH_GEN_2 "/etc/moca/mocacore-gen2.bin"
#define MOCA20CORE_PATH_GEN_1  "/etc/moca/moca20core-gen21.bin"
#define MOCA20CORE_PATH_GEN_1B "/etc/moca/moca20core-gen21B.bin"
#define MOCA20CORE_PATH_GEN_2  "/etc/moca/moca20core-gen22.bin"
#define MOCA20CORE_PATH_GEN_2B "/etc/moca/moca20core-gen22B.bin"
#define MOCA20CORE_PATH_GEN_3  "/etc/moca/moca20core-gen23.bin"
#define MOCA20CORE_PATH_GEN_3B "/etc/moca/moca20core-gen23B.bin"
#define MOCA20CORE_PATH_GEN_4  "/etc/moca/moca20core-gen24.bin"
#define MOCA20CORE_PATH_GEN_4B "/etc/moca/moca20core-gen24B.bin"
#define MOCA20CORE_PATH_GEN_5  "/etc/moca/moca20core-gen25.bin"
#define MOCA20CORE_PATH_GEN_5B "/etc/moca/moca20core-gen25B.bin"



#define LOF_PATH_FMT             "lof.%s"
#define NONDEFSEQNUM_PATH_FMT    "nondefseqnum.%s"
#define E2M_PATH_FMT             "e2m.%s"
#define RF_CALIB_DATA_PATH_FMT   "rfcalibdata.%s"
#define PROBE_2_RESULTS_PATH_FMT "probe2res.%s"
#define MPS_DATA_PATH_FMT        "mpsdata.%s"
#define PRECFG_SEED_BND_PATH_FMT "pcseedb.%s"

#define PIDFILE_FMT		"mocad-%s.pid"

#define IFNAME_GLOB		"/sys/class/net/*"
#define DEVNAME_FMT		"/sys/class/bmoca/%s/dev"

#define SNR_DATA_SIZE		(8 * 1024)
#define IQ_DATA_SIZE			(1 * 1024)
#define CIR_DATA_SIZE		(1 * 1024)

#define MUTEX_INIT()		MoCAOS_MutexInit()
#define MUTEX_LOCK(x)		MoCAOS_MutexLock((MoCAOS_MutexHandle)x)
#define MUTEX_UNLOCK(x)		MoCAOS_MutexUnlock((MoCAOS_MutexHandle)x)
#define MUTEX_FREE(x)		MoCAOS_MutexClose((MoCAOS_MutexHandle)x)
#define MUTEX_T			MoCAOS_MutexHandle

#define MOCA_SAPM_TABLE_FW_50_LOW_START   4
#define MOCA_SAPM_TABLE_FW_50_LOW_END     115
#define MOCA_SAPM_TABLE_FW_50_HI_START    141
#define MOCA_SAPM_TABLE_FW_50_HI_END      252
#define MOCA_SAPM_TABLE_FW_100_LOW_START  4
#define MOCA_SAPM_TABLE_FW_100_LOW_END    243
#define MOCA_SAPM_TABLE_FW_100_HI_START   269
#define MOCA_SAPM_TABLE_FW_100_HI_END     508
#define MOCA_SAPM_TABLE_FW_SEC_LOW_START  4
#define MOCA_SAPM_TABLE_FW_SEC_LOW_END    243
#define MOCA_SAPM_TABLE_FW_SEC_HI_START   269
#define MOCA_SAPM_TABLE_FW_SEC_HI_END     508

#define MOCA_CONST_TX_SUBMODE_SINGLE_TONE       0
#define MOCA_CONST_TX_SUBMODE_PROBE_I           1
#define MOCA_CONST_TX_SUBMODE_CW                2
#define MOCA_CONST_TX_SUBMODE_BAND              3


void __moca_copy_be32(void *out, const void *in, int size);

extern int (*__mocad_cmd_hook)(void *vctx, uint32_t msg_type, uint32_t ie_type,
	const void *wr, int wr_len, void *rd, int rd_len, int flags);

extern int (*__mocad_table_cmd_hook)(void * vctx, uint32_t ie_type, uint32_t * in,
	void * out, int struct_len, int max_out_len, int flags);

#define MOCA_MACADDR_COMPARE(mac1, mac2) (((mac1)->addr[0] == (mac2)->addr[0]) && \
                                          ((mac1)->addr[1] == (mac2)->addr[1]) && \
                                          ((mac1)->addr[2] == (mac2)->addr[2]) && \
                                          ((mac1)->addr[3] == (mac2)->addr[3]) && \
                                          ((mac1)->addr[4] == (mac2)->addr[4]) && \
                                          ((mac1)->addr[5] == (mac2)->addr[5]))
#define MOCA_MACADDR_IS_BCAST(mac)       (((mac)->addr[0] == 0xFF) && \
                                          ((mac)->addr[1] == 0xFF) && \
                                          ((mac)->addr[2] == 0xFF) && \
                                          ((mac)->addr[3] == 0xFF) && \
                                          ((mac)->addr[4] == 0xFF) && \
                                          ((mac)->addr[5] == 0xFF))
#define MOCA_MACADDR_IS_NULL(mac)        (((mac)->addr[0] == 0x0) && \
                                          ((mac)->addr[1] == 0x0) && \
                                          ((mac)->addr[2] == 0x0) && \
                                          ((mac)->addr[3] == 0x0) && \
                                          ((mac)->addr[4] == 0x0) && \
                                          ((mac)->addr[5] == 0x0))
#define MOCA_MACADDR_COPY(dst, src)       memcpy(dst, src, sizeof(macaddr_t))

//#define MOCA_MACADDR_COMPARE_BE32(mac1, mac2) ((BE32((mac1)->hi) == (mac2)->hi) && (BE32((mac1)->lo) == (mac2)->lo))
//#define MOCA_MACADDR_IS_BCAST_BE32(mac) ((BE32((mac)->hi) == 0xFFFFFFFF) && (BE32((mac)->lo) == 0xFFFF0000))
//#define MOCA_MACADDR_COPY_BE32(dst, src) (dst)->hi = BE32((src)->hi); (dst)->lo = BE32((src)->lo);

struct mmp_msg_hdr
{
	uint32_t   type;
	uint32_t   length;
	uint32_t   ie_type;
} __attribute__((packed));

struct moca_callback_arg
{
	void *     data;
	uint32_t   test;
};

#define MOCA_FMR20_MAX_OFDMA_SUBCHANNELS 4
struct moca_fmr20_tx_node_info 
{
   uint8_t     gap_nper;
   uint8_t     gap_vlper;
   uint16_t    ofdmb_nper;
   uint16_t    ofdmb_vlper;
} __attribute__((packed));

struct moca_fmr20_ofdma_info
{
   uint32_t    node_bitmask;
   uint8_t     ofdma_subchan;
   uint8_t     ofdma_gap;
   uint16_t    ofdmab;
} __attribute__((packed));

struct moca_fmr20_node_data
{
   struct moca_fmr20_tx_node_info   tx_node_info[MOCA_MAX_NODES];
   uint8_t                          gap_gcd;
   uint16_t                         ofdmb_gcd;
   uint8_t                          ofdma_def_tab_num;
   struct moca_fmr20_ofdma_info     ofdma_tab_info[MOCA_FMR20_MAX_OFDMA_SUBCHANNELS];
} __attribute__((packed,aligned(4)));


/* The extra header length is the length of the fields
   in the header that follow the length field */
#define MOCA_EXTRA_MMP_HDR_LEN (sizeof(uint32_t))
#define MOCA_MMP_MSG_ACK_BIT   (0x80000000)
#define MOCA_MMP_FW_FUNC_FLAG  (1 << 24)

#define MOCA_MSG_SET              0x01
#define MOCA_MSG_GET              0x02
#define MOCA_MSG_TRAP             0x03
#define MOCA_MSG_CONCAT_TRAP      0x04
#define MOCA_MSG_SET_CPU0         0x11
#define MOCA_MSG_GET_CPU0         0x12
#define MOCA_MSG_TRAP_CPU0        0x13
#define MOCA_MSG_CONCAT_TRAP_CPU0 0x14


/* GENERATED DECLARATIONS BELOW THIS LINE - DO NOT EDIT */

#define MOCA_GROUP_NODE                 1
#define MOCA_GROUP_PHY                  2
#define MOCA_GROUP_MAC_LAYER            3
#define MOCA_GROUP_FORWARDING           4
#define MOCA_GROUP_NETWORK              5
#define MOCA_GROUP_INTFC                6
#define MOCA_GROUP_POWER_MGMT           7
#define MOCA_GROUP_SECURITY             8
#define MOCA_GROUP_DEBUG                9
#define MOCA_GROUP_LAB                  10
#define MOCA_GROUP_MPS                  11

// Group node
#define IE_PREFERRED_NC                 ((MOCA_GROUP_NODE << 16) | 1)
#define IE_SINGLE_CHANNEL_OPERATION     ((MOCA_GROUP_NODE << 16) | 2)
#define IE_CONTINUOUS_POWER_TX_MODE     ((MOCA_GROUP_NODE << 16) | 3)
#define IE_CONTINUOUS_RX_MODE_ATTN      ((MOCA_GROUP_NODE << 16) | 4)
#define IE_LOF                          ((MOCA_GROUP_NODE << 16) | 5)
#define IE_MAX_NBAS_PRIMARY             ((MOCA_GROUP_NODE << 16) | 6)
#define IE_PS_SWCH_TX1                  ((MOCA_GROUP_NODE << 16) | 7)
#define IE_PS_SWCH_TX2                  ((MOCA_GROUP_NODE << 16) | 8)
#define IE_PS_SWCH_RX1                  ((MOCA_GROUP_NODE << 16) | 9)
#define IE_PS_SWCH_RX2                  ((MOCA_GROUP_NODE << 16) | 10)
#define IE_PS_SWCH_RX3                  ((MOCA_GROUP_NODE << 16) | 11)
#define IE_PRIM_CH_OFFS                 ((MOCA_GROUP_NODE << 16) | 12)
#define IE_SEC_CH_OFFS                  ((MOCA_GROUP_NODE << 16) | 13)
#define IE_NON_DEF_SEQ_NUM              ((MOCA_GROUP_NODE << 16) | 15)
#define IE_BONDING                      ((MOCA_GROUP_NODE << 16) | 16)
#define IE_LISTENING_FREQ_MASK          ((MOCA_GROUP_NODE << 16) | 17)
#define IE_LISTENING_DURATION           ((MOCA_GROUP_NODE << 16) | 18)
#define IE_LIMIT_TRAFFIC                ((MOCA_GROUP_NODE << 16) | 19)
#define IE_REMOTE_MAN                   ((MOCA_GROUP_NODE << 16) | 20)
#define IE_C4_MOCA20_EN                 ((MOCA_GROUP_NODE << 16) | 21)
#define IE_POWER_SAVE_MECHANISM_DIS     ((MOCA_GROUP_NODE << 16) | 22)
#define IE_PSM_CONFIG                   ((MOCA_GROUP_NODE << 16) | 23)
#define IE_USE_EXT_DATA_MEM             ((MOCA_GROUP_NODE << 16) | 24)
#define IE_AIF_MODE                     ((MOCA_GROUP_NODE << 16) | 25)
#define IE_PROF_PAD_CTRL_DEG_6802C0_BONDING ((MOCA_GROUP_NODE << 16) | 26)
#define IE_PROP_BONDING_COMPATIBILITY_MODE ((MOCA_GROUP_NODE << 16) | 27)
#define IE_RDEG_3450                    ((MOCA_GROUP_NODE << 16) | 28)
#define IE_PHY_CLOCK                    ((MOCA_GROUP_NODE << 16) | 29)
#define IE_MAX_NBAS_SECONDARY           ((MOCA_GROUP_NODE << 16) | 30)
#define IE_MAC_ADDR                     ((MOCA_GROUP_NODE << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_NODE_STATUS                  ((MOCA_GROUP_NODE << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_START_MOCA_CORE              ((MOCA_GROUP_NODE << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_DOWNLOAD_TO_PACKET_RAM_DONE  ((MOCA_GROUP_NODE << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_BEACON_CHANNEL_SET           ((MOCA_GROUP_NODE << 16) | 1028 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_FW_VERSION                   ((MOCA_GROUP_NODE << 16) | 1029 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SHUTDOWN                     ((MOCA_GROUP_NODE << 16) | 1030 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MAX_TX_POWER_TUNE            ((MOCA_GROUP_NODE << 16) | 1031 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MAX_TX_POWER_TUNE_SEC_CH     ((MOCA_GROUP_NODE << 16) | 1032 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_RX_POWER_TUNE                ((MOCA_GROUP_NODE << 16) | 1033 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MOCAD_FORWARDING_RX_MAC      ((MOCA_GROUP_NODE << 16) | 1034 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MOCAD_FORWARDING_RX_ACK      ((MOCA_GROUP_NODE << 16) | 1035 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MOCAD_FORWARDING_TX_ALLOC    ((MOCA_GROUP_NODE << 16) | 1036 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MOCAD_FORWARDING_TX_SEND     ((MOCA_GROUP_NODE << 16) | 1037 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_IMPEDANCE_MODE_BONDING       ((MOCA_GROUP_NODE << 16) | 1038 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_REWORK_6802                  ((MOCA_GROUP_NODE << 16) | 1039 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PROF_PAD_CTRL_DEG_6802C0_SINGLE ((MOCA_GROUP_NODE << 16) | 1040 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_CORE_READY                   ((MOCA_GROUP_NODE << 16) | 2048)
#define IE_POWER_UP_STATUS              ((MOCA_GROUP_NODE << 16) | 2049)
#define IE_NEW_LOF                      ((MOCA_GROUP_NODE << 16) | 2050)
#define IE_ADMISSION_COMPLETED          ((MOCA_GROUP_NODE << 16) | 2051)
#define IE_TPCAP_DONE                   ((MOCA_GROUP_NODE << 16) | 2052)
#define IE_MOCAD_FORWARDING_RX_PACKET   ((MOCA_GROUP_NODE << 16) | 2053)
#define IE_MOCAD_FORWARDING_TX_ACK      ((MOCA_GROUP_NODE << 16) | 2054)
#define IE_PR_DEGRADATION               ((MOCA_GROUP_NODE << 16) | 2055)
#define IE_START                        ((MOCA_GROUP_NODE << 16) | 4096)
#define IE_STOP                         ((MOCA_GROUP_NODE << 16) | 4097)
#define IE_DRV_INFO                     ((MOCA_GROUP_NODE << 16) | 4098)
#define IE_WDT                          ((MOCA_GROUP_NODE << 16) | 4099)
#define IE_ABORT                        ((MOCA_GROUP_NODE << 16) | 4100)
#define IE_MISCVAL                      ((MOCA_GROUP_NODE << 16) | 4101)
#define IE_EN_CAPABLE                   ((MOCA_GROUP_NODE << 16) | 4102)
#define IE_RESTORE_DEFAULTS             ((MOCA_GROUP_NODE << 16) | 4103)
#define IE_MOCAD_VERSION                ((MOCA_GROUP_NODE << 16) | 4104)
#define IE_RESTART                      ((MOCA_GROUP_NODE << 16) | 4105)
#define IE_LOF_UPDATE                   ((MOCA_GROUP_NODE << 16) | 4106)
#define IE_PRIMARY_CH_OFFSET            ((MOCA_GROUP_NODE << 16) | 4107)
#define IE_ASSERTTEXT                   ((MOCA_GROUP_NODE << 16) | 4108)
#define IE_WDOG_ENABLE                  ((MOCA_GROUP_NODE << 16) | 4109)
#define IE_MISCVAL2                     ((MOCA_GROUP_NODE << 16) | 4110)
#define IE_MR_SEQ_NUM                   ((MOCA_GROUP_NODE << 16) | 4111)
#define IE_SECONDARY_CH_OFFSET          ((MOCA_GROUP_NODE << 16) | 4112)
#define IE_COF                          ((MOCA_GROUP_NODE << 16) | 4113)
#define IE_CONFIG_FLAGS                 ((MOCA_GROUP_NODE << 16) | 4114)
#define IE_AMP_TYPE                     ((MOCA_GROUP_NODE << 16) | 4115)
#define IE_PM_NOTIFICATION              ((MOCA_GROUP_NODE << 16) | 4116)

// Group phy
#define IE_TPC_EN                       ((MOCA_GROUP_PHY << 16) | 1)
#define IE_MAX_TX_POWER                 ((MOCA_GROUP_PHY << 16) | 2)
#define IE_BEACON_PWR_REDUCTION         ((MOCA_GROUP_PHY << 16) | 3)
#define IE_BEACON_PWR_REDUCTION_EN      ((MOCA_GROUP_PHY << 16) | 4)
#define IE_BO_MODE                      ((MOCA_GROUP_PHY << 16) | 5)
#define IE_QAM256_CAPABILITY            ((MOCA_GROUP_PHY << 16) | 6)
#define IE_OTF_EN                       ((MOCA_GROUP_PHY << 16) | 7)
#define IE_STAR_TOPOLOGY_EN             ((MOCA_GROUP_PHY << 16) | 8)
#define IE_OFDMA_EN                     ((MOCA_GROUP_PHY << 16) | 9)
#define IE_MIN_BW_ALARM_THRESHOLD       ((MOCA_GROUP_PHY << 16) | 10)
#define IE_EN_MAX_RATE_IN_MAX_BO        ((MOCA_GROUP_PHY << 16) | 11)
#define IE_TARGET_PHY_RATE_QAM128       ((MOCA_GROUP_PHY << 16) | 12)
#define IE_TARGET_PHY_RATE_QAM256       ((MOCA_GROUP_PHY << 16) | 13)
#define IE_SAPM_EN                      ((MOCA_GROUP_PHY << 16) | 14)
#define IE_ARPL_TH_50                   ((MOCA_GROUP_PHY << 16) | 15)
#define IE_RLAPM_EN                     ((MOCA_GROUP_PHY << 16) | 16)
#define IE_FREQ_SHIFT                   ((MOCA_GROUP_PHY << 16) | 17)
#define IE_MAX_PHY_RATE                 ((MOCA_GROUP_PHY << 16) | 19)
#define IE_BANDWIDTH                    ((MOCA_GROUP_PHY << 16) | 20)
#define IE_ARPL_TH_100                  ((MOCA_GROUP_PHY << 16) | 21)
#define IE_ADC_MODE                     ((MOCA_GROUP_PHY << 16) | 22)
#define IE_MAX_PHY_RATE_TURBO           ((MOCA_GROUP_PHY << 16) | 23)
#define IE_CP_CONST                     ((MOCA_GROUP_PHY << 16) | 24)
#define IE_PREAMBLE_UC_CONST            ((MOCA_GROUP_PHY << 16) | 25)
#define IE_CP_MARGIN_INCREASE           ((MOCA_GROUP_PHY << 16) | 26)
#define IE_AC_CC_SHIFT                  ((MOCA_GROUP_PHY << 16) | 27)
#define IE_MFC_TH_INCREASE              ((MOCA_GROUP_PHY << 16) | 28)
#define IE_AGC_CONST_EN                 ((MOCA_GROUP_PHY << 16) | 29)
#define IE_AGC_CONST_ADDRESS            ((MOCA_GROUP_PHY << 16) | 30)
#define IE_MIN_BO_INSERT_2_BFM_LOCK     ((MOCA_GROUP_PHY << 16) | 31)
#define IE_MIN_SNR_AVG_DB_2_BFM_LOCK    ((MOCA_GROUP_PHY << 16) | 32)
#define IE_MAX_PHY_RATE_50M             ((MOCA_GROUP_PHY << 16) | 33)
#define IE_MAX_CONSTELLATION_ALL        ((MOCA_GROUP_PHY << 16) | 34)
#define IE_MAX_CONSTELLATION            ((MOCA_GROUP_PHY << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SIG_Y_DONE                   ((MOCA_GROUP_PHY << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SNR_MARGIN_TABLE_RS          ((MOCA_GROUP_PHY << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SAPM_TABLE_FW_50             ((MOCA_GROUP_PHY << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_RLAPM_TABLE_50               ((MOCA_GROUP_PHY << 16) | 1028 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PHY_STATUS                   ((MOCA_GROUP_PHY << 16) | 1029 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SNR_MARGIN_TABLE_LDPC        ((MOCA_GROUP_PHY << 16) | 1030 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SNR_MARGIN_TABLE_LDPC_SEC_CH ((MOCA_GROUP_PHY << 16) | 1031 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SNR_MARGIN_TABLE_LDPC_PRE5   ((MOCA_GROUP_PHY << 16) | 1032 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SNR_MARGIN_TABLE_OFDMA       ((MOCA_GROUP_PHY << 16) | 1033 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SAPM_TABLE_FW_100            ((MOCA_GROUP_PHY << 16) | 1034 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_RLAPM_TABLE_100              ((MOCA_GROUP_PHY << 16) | 1035 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_CPLENGTH                     ((MOCA_GROUP_PHY << 16) | 1036 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_RF_CALIB_DATA                ((MOCA_GROUP_PHY << 16) | 1037 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PROBE_2_RESULTS              ((MOCA_GROUP_PHY << 16) | 1038 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SAPM_TABLE_FW_SEC            ((MOCA_GROUP_PHY << 16) | 1039 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_RX_GAIN_PARAMS               ((MOCA_GROUP_PHY << 16) | 1040 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_RX_GAIN_AGC_TABLE            ((MOCA_GROUP_PHY << 16) | 1041 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_TX_POWER_PARAMS              ((MOCA_GROUP_PHY << 16) | 1042 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SNR_MARGIN_TABLE_LDPC_PRI_CH ((MOCA_GROUP_PHY << 16) | 1043 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SNR_MARGIN_TABLE_PRE5_PRI_CH ((MOCA_GROUP_PHY << 16) | 1044 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SNR_MARGIN_TABLE_PRE5_SEC_CH ((MOCA_GROUP_PHY << 16) | 1045 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SIG_Y_READY                  ((MOCA_GROUP_PHY << 16) | 2048)
#define IE_NEW_RF_CALIB_DATA            ((MOCA_GROUP_PHY << 16) | 2049)
#define IE_NEW_PROBE_2_RESULTS          ((MOCA_GROUP_PHY << 16) | 2050)
#define IE_NV_CAL_ENABLE                ((MOCA_GROUP_PHY << 16) | 4096)
#define IE_RLAPM_CAP_50                 ((MOCA_GROUP_PHY << 16) | 4097)
#define IE_SNR_MARGIN_RS                ((MOCA_GROUP_PHY << 16) | 4098)
#define IE_SNR_MARGIN_LDPC              ((MOCA_GROUP_PHY << 16) | 4099)
#define IE_SNR_MARGIN_LDPC_SEC_CH       ((MOCA_GROUP_PHY << 16) | 4100)
#define IE_SNR_MARGIN_LDPC_PRE5         ((MOCA_GROUP_PHY << 16) | 4101)
#define IE_SNR_MARGIN_OFDMA             ((MOCA_GROUP_PHY << 16) | 4102)
#define IE_RLAPM_CAP_100                ((MOCA_GROUP_PHY << 16) | 4103)
#define IE_SAPM_TABLE_50                ((MOCA_GROUP_PHY << 16) | 4104)
#define IE_SAPM_TABLE_100               ((MOCA_GROUP_PHY << 16) | 4105)
#define IE_NV_CAL_CLEAR                 ((MOCA_GROUP_PHY << 16) | 4106)
#define IE_SAPM_TABLE_SEC               ((MOCA_GROUP_PHY << 16) | 4107)
#define IE_AMP_REG                      ((MOCA_GROUP_PHY << 16) | 4108)
#define IE_SNR_MARGIN_LDPC_PRI_CH       ((MOCA_GROUP_PHY << 16) | 4109)
#define IE_SNR_MARGIN_PRE5_PRI_CH       ((MOCA_GROUP_PHY << 16) | 4110)
#define IE_SNR_MARGIN_PRE5_SEC_CH       ((MOCA_GROUP_PHY << 16) | 4111)

// Group mac_layer
#define IE_MAX_FRAME_SIZE               ((MOCA_GROUP_MAC_LAYER << 16) | 1)
#define IE_MIN_AGGR_WAITING_TIME        ((MOCA_GROUP_MAC_LAYER << 16) | 2)
#define IE_SELECTIVE_RR                 ((MOCA_GROUP_MAC_LAYER << 16) | 3)
#define IE_FRAGMENTATION                ((MOCA_GROUP_MAC_LAYER << 16) | 4)
#define IE_IFG_MOCA20                   ((MOCA_GROUP_MAC_LAYER << 16) | 5)
#define IE_MAP_SEED                     ((MOCA_GROUP_MAC_LAYER << 16) | 6)
#define IE_MAX_TRANSMIT_TIME            ((MOCA_GROUP_MAC_LAYER << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MAX_PKT_AGGR                 ((MOCA_GROUP_MAC_LAYER << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PROBE_REQUEST                ((MOCA_GROUP_MAC_LAYER << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_RTR_CONFIG                   ((MOCA_GROUP_MAC_LAYER << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_TLP_MODE                     ((MOCA_GROUP_MAC_LAYER << 16) | 1028 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MAX_PKT_AGGR_BONDING         ((MOCA_GROUP_MAC_LAYER << 16) | 1029 | MOCA_MMP_FW_FUNC_FLAG)

// Group forwarding
#define IE_MULTICAST_MODE               ((MOCA_GROUP_FORWARDING << 16) | 1)
#define IE_LOW_PRI_Q_NUM                ((MOCA_GROUP_FORWARDING << 16) | 2)
#define IE_EGR_MC_FILTER_EN             ((MOCA_GROUP_FORWARDING << 16) | 3)
#define IE_FC_MODE                      ((MOCA_GROUP_FORWARDING << 16) | 4)
#define IE_PQOS_MAX_PACKET_SIZE         ((MOCA_GROUP_FORWARDING << 16) | 5)
#define IE_PER_MODE                     ((MOCA_GROUP_FORWARDING << 16) | 6)
#define IE_POLICING_EN                  ((MOCA_GROUP_FORWARDING << 16) | 7)
#define IE_PQOS_EGRESS_NUMFLOWS         ((MOCA_GROUP_FORWARDING << 16) | 8)
#define IE_ORR_EN                       ((MOCA_GROUP_FORWARDING << 16) | 9)
#define IE_BRCMTAG_ENABLE               ((MOCA_GROUP_FORWARDING << 16) | 10)
#define IE_PRIORITY_ALLOCATIONS         ((MOCA_GROUP_FORWARDING << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_EGR_MC_ADDR_FILTER           ((MOCA_GROUP_FORWARDING << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PQOS_CREATE_REQUEST          ((MOCA_GROUP_FORWARDING << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PQOS_INGR_ADD_FLOW           ((MOCA_GROUP_FORWARDING << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PQOS_UPDATE_REQUEST          ((MOCA_GROUP_FORWARDING << 16) | 1028 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PQOS_INGR_UPDATE             ((MOCA_GROUP_FORWARDING << 16) | 1029 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PQOS_DELETE_REQUEST          ((MOCA_GROUP_FORWARDING << 16) | 1030 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PQOS_INGR_DELETE             ((MOCA_GROUP_FORWARDING << 16) | 1031 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PQOS_LIST_REQUEST            ((MOCA_GROUP_FORWARDING << 16) | 1032 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PQOS_QUERY_REQUEST           ((MOCA_GROUP_FORWARDING << 16) | 1033 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PQOS_MAINTENANCE_START       ((MOCA_GROUP_FORWARDING << 16) | 1034 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_UC_FWD                       ((MOCA_GROUP_FORWARDING << 16) | 1035 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MC_FWD                       ((MOCA_GROUP_FORWARDING << 16) | 1036 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_SRC_ADDR                     ((MOCA_GROUP_FORWARDING << 16) | 1037 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MAC_AGING                    ((MOCA_GROUP_FORWARDING << 16) | 1038 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_LOOPBACK_EN                  ((MOCA_GROUP_FORWARDING << 16) | 1039 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MCFILTER_ENABLE              ((MOCA_GROUP_FORWARDING << 16) | 1040 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MCFILTER_ADDENTRY            ((MOCA_GROUP_FORWARDING << 16) | 1041 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MCFILTER_DELENTRY            ((MOCA_GROUP_FORWARDING << 16) | 1042 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PAUSE_FC_EN                  ((MOCA_GROUP_FORWARDING << 16) | 1043 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_STAG_PRIORITY                ((MOCA_GROUP_FORWARDING << 16) | 1044 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_STAG_REMOVAL                 ((MOCA_GROUP_FORWARDING << 16) | 1045 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_EPORT_LINK_STATE             ((MOCA_GROUP_FORWARDING << 16) | 1046 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_UCFWD_UPDATE                 ((MOCA_GROUP_FORWARDING << 16) | 2048)
#define IE_PQOS_CREATE_RESPONSE         ((MOCA_GROUP_FORWARDING << 16) | 2049)
#define IE_PQOS_CREATE_COMPLETE         ((MOCA_GROUP_FORWARDING << 16) | 2050)
#define IE_PQOS_UPDATE_RESPONSE         ((MOCA_GROUP_FORWARDING << 16) | 2051)
#define IE_PQOS_UPDATE_COMPLETE         ((MOCA_GROUP_FORWARDING << 16) | 2052)
#define IE_PQOS_DELETE_RESPONSE         ((MOCA_GROUP_FORWARDING << 16) | 2053)
#define IE_PQOS_DELETE_COMPLETE         ((MOCA_GROUP_FORWARDING << 16) | 2054)
#define IE_PQOS_LIST_RESPONSE           ((MOCA_GROUP_FORWARDING << 16) | 2055)
#define IE_PQOS_QUERY_RESPONSE          ((MOCA_GROUP_FORWARDING << 16) | 2056)
#define IE_PQOS_MAINTENANCE_COMPLETE    ((MOCA_GROUP_FORWARDING << 16) | 2057)
#define IE_PQOS_CREATE_FLOW             ((MOCA_GROUP_FORWARDING << 16) | 4097)
#define IE_PQOS_UPDATE_FLOW             ((MOCA_GROUP_FORWARDING << 16) | 4098)
#define IE_PQOS_DELETE_FLOW             ((MOCA_GROUP_FORWARDING << 16) | 4099)
#define IE_PQOS_LIST                    ((MOCA_GROUP_FORWARDING << 16) | 4100)
#define IE_PQOS_QUERY                   ((MOCA_GROUP_FORWARDING << 16) | 4101)
#define IE_PQOS_STATUS                  ((MOCA_GROUP_FORWARDING << 16) | 4102)
#define IE_MCFILTER_CLEAR_TABLE         ((MOCA_GROUP_FORWARDING << 16) | 4105)
#define IE_MCFILTER_TABLE               ((MOCA_GROUP_FORWARDING << 16) | 4106)
#define IE_HOST_QOS                     ((MOCA_GROUP_FORWARDING << 16) | 4107)

// Group network
#define IE_OOO_LMO_THRESHOLD            ((MOCA_GROUP_NETWORK << 16) | 2)
#define IE_TABOO_CHANNELS               ((MOCA_GROUP_NETWORK << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_FMR_REQUEST                  ((MOCA_GROUP_NETWORK << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MR_REQUEST                   ((MOCA_GROUP_NETWORK << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_GEN_NODE_STATUS              ((MOCA_GROUP_NETWORK << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_GEN_NODE_EXT_STATUS          ((MOCA_GROUP_NETWORK << 16) | 1028 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_NODE_STATS                   ((MOCA_GROUP_NETWORK << 16) | 1029 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_NODE_STATS_EXT               ((MOCA_GROUP_NETWORK << 16) | 1030 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_NETWORK_STATUS               ((MOCA_GROUP_NETWORK << 16) | 1031 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_OOO_LMO                      ((MOCA_GROUP_NETWORK << 16) | 1032 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_START_ULMO                   ((MOCA_GROUP_NETWORK << 16) | 1033 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_DD_REQUEST                   ((MOCA_GROUP_NETWORK << 16) | 1034 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_FMR20_REQUEST                ((MOCA_GROUP_NETWORK << 16) | 1035 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_RXD_LMO_REQUEST              ((MOCA_GROUP_NETWORK << 16) | 1036 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_OFDMA_DEFINITION_TABLE       ((MOCA_GROUP_NETWORK << 16) | 1037 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_OFDMA_ASSIGNMENT_TABLE       ((MOCA_GROUP_NETWORK << 16) | 1038 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_START_ACA                    ((MOCA_GROUP_NETWORK << 16) | 1039 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_ADM_STATS                    ((MOCA_GROUP_NETWORK << 16) | 1040 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_ADMISSION_STATUS             ((MOCA_GROUP_NETWORK << 16) | 2048)
#define IE_LIMITED_BW                   ((MOCA_GROUP_NETWORK << 16) | 2049)
#define IE_LMO_INFO                     ((MOCA_GROUP_NETWORK << 16) | 2050)
#define IE_TOPOLOGY_CHANGED             ((MOCA_GROUP_NETWORK << 16) | 2051)
#define IE_MOCA_VERSION_CHANGED         ((MOCA_GROUP_NETWORK << 16) | 2052)
#define IE_MOCA_RESET_REQUEST           ((MOCA_GROUP_NETWORK << 16) | 2053)
#define IE_NC_ID_CHANGED                ((MOCA_GROUP_NETWORK << 16) | 2054)
#define IE_FMR_RESPONSE                 ((MOCA_GROUP_NETWORK << 16) | 2055)
#define IE_MR_RESPONSE                  ((MOCA_GROUP_NETWORK << 16) | 2056)
#define IE_MR_COMPLETE                  ((MOCA_GROUP_NETWORK << 16) | 2057)
#define IE_CPU_CHECK                    ((MOCA_GROUP_NETWORK << 16) | 2058)
#define IE_DD_RESPONSE                  ((MOCA_GROUP_NETWORK << 16) | 2059)
#define IE_FMR20_RESPONSE               ((MOCA_GROUP_NETWORK << 16) | 2060)
#define IE_PKTRAM_FILL                  ((MOCA_GROUP_NETWORK << 16) | 2061)
#define IE_MR_EVENT                     ((MOCA_GROUP_NETWORK << 16) | 2062)
#define IE_HOSTLESS_MODE_RESPONSE       ((MOCA_GROUP_NETWORK << 16) | 2063)
#define IE_WAKEUP_NODE_RESPONSE         ((MOCA_GROUP_NETWORK << 16) | 2064)
#define IE_ACA_RESULT_1                 ((MOCA_GROUP_NETWORK << 16) | 2065)
#define IE_ACA_RESULT_2                 ((MOCA_GROUP_NETWORK << 16) | 2066)
#define IE_ACA                          ((MOCA_GROUP_NETWORK << 16) | 4096)
#define IE_FMR_INIT                     ((MOCA_GROUP_NETWORK << 16) | 4097)
#define IE_MOCA_RESET                   ((MOCA_GROUP_NETWORK << 16) | 4098)
#define IE_DD_INIT                      ((MOCA_GROUP_NETWORK << 16) | 4099)
#define IE_FMR_20                       ((MOCA_GROUP_NETWORK << 16) | 4100)
#define IE_ERROR_STATS                  ((MOCA_GROUP_NETWORK << 16) | 4101)
#define IE_HOSTLESS_MODE                ((MOCA_GROUP_NETWORK << 16) | 4102)
#define IE_WAKEUP_NODE                  ((MOCA_GROUP_NETWORK << 16) | 4103)
#define IE_LAST_MR_EVENTS               ((MOCA_GROUP_NETWORK << 16) | 4104)

// Group intfc
#define IE_RF_BAND                      ((MOCA_GROUP_INTFC << 16) | 1)
#define IE_RF_SWITCH                    ((MOCA_GROUP_INTFC << 16) | 2)
#define IE_IF_ACCESS_EN                 ((MOCA_GROUP_INTFC << 16) | 3)
#define IE_LED_MODE                     ((MOCA_GROUP_INTFC << 16) | 4)
#define IE_GEN_STATS                    ((MOCA_GROUP_INTFC << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_INTERFACE_STATUS             ((MOCA_GROUP_INTFC << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_RESET_CORE_STATS             ((MOCA_GROUP_INTFC << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_IF_ACCESS_TABLE              ((MOCA_GROUP_INTFC << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_LINK_UP_STATE                ((MOCA_GROUP_INTFC << 16) | 2048)
#define IE_NEW_RF_BAND                  ((MOCA_GROUP_INTFC << 16) | 2049)
#define IE_EXT_OCTET_COUNT              ((MOCA_GROUP_INTFC << 16) | 4096)
#define IE_RESET_STATS                  ((MOCA_GROUP_INTFC << 16) | 4097)

// Group power_mgmt
#define IE_M1_TX_POWER_VARIATION        ((MOCA_GROUP_POWER_MGMT << 16) | 1)
#define IE_NC_LISTENING_INTERVAL        ((MOCA_GROUP_POWER_MGMT << 16) | 2)
#define IE_NC_HEARTBEAT_INTERVAL        ((MOCA_GROUP_POWER_MGMT << 16) | 3)
#define IE_WOM_MODE_INTERNAL            ((MOCA_GROUP_POWER_MGMT << 16) | 4)
#define IE_WOM_MAGIC_ENABLE             ((MOCA_GROUP_POWER_MGMT << 16) | 5)
#define IE_PM_RESTORE_ON_LINK_DOWN      ((MOCA_GROUP_POWER_MGMT << 16) | 6)
#define IE_POWER_STATE                  ((MOCA_GROUP_POWER_MGMT << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_HOSTLESS_MODE_REQUEST        ((MOCA_GROUP_POWER_MGMT << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_WAKEUP_NODE_REQUEST          ((MOCA_GROUP_POWER_MGMT << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_NODE_POWER_STATE             ((MOCA_GROUP_POWER_MGMT << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_FILTER_M2_DATA_WAKEUP        ((MOCA_GROUP_POWER_MGMT << 16) | 1028 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_WOM_PATTERN                  ((MOCA_GROUP_POWER_MGMT << 16) | 1029 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_WOM_IP                       ((MOCA_GROUP_POWER_MGMT << 16) | 1030 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_WOM_MAGIC_MAC                ((MOCA_GROUP_POWER_MGMT << 16) | 1031 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_STANDBY_POWER_STATE          ((MOCA_GROUP_POWER_MGMT << 16) | 1032)
#define IE_WOM_MODE                     ((MOCA_GROUP_POWER_MGMT << 16) | 1033)
#define IE_POWER_STATE_RSP              ((MOCA_GROUP_POWER_MGMT << 16) | 2048)
#define IE_POWER_STATE_EVENT            ((MOCA_GROUP_POWER_MGMT << 16) | 2049)
#define IE_POWER_STATE_CAP              ((MOCA_GROUP_POWER_MGMT << 16) | 2050)
#define IE_WOL                          ((MOCA_GROUP_POWER_MGMT << 16) | 4096)
#define IE_PS_CMD                       ((MOCA_GROUP_POWER_MGMT << 16) | 4097)
#define IE_POWER_STATE_CAPABILITIES     ((MOCA_GROUP_POWER_MGMT << 16) | 4098)
#define IE_LAST_PS_EVENT_CODE           ((MOCA_GROUP_POWER_MGMT << 16) | 4099)

// Group security
#define IE_PRIVACY_EN                   ((MOCA_GROUP_SECURITY << 16) | 1)
#define IE_PMK_EXCHANGE_INTERVAL        ((MOCA_GROUP_SECURITY << 16) | 2)
#define IE_TEK_EXCHANGE_INTERVAL        ((MOCA_GROUP_SECURITY << 16) | 3)
#define IE_AES_EXCHANGE_INTERVAL        ((MOCA_GROUP_SECURITY << 16) | 4)
#define IE_MMK_KEY                      ((MOCA_GROUP_SECURITY << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PMK_INITIAL_KEY              ((MOCA_GROUP_SECURITY << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_AES_MM_KEY                   ((MOCA_GROUP_SECURITY << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_AES_PM_KEY                   ((MOCA_GROUP_SECURITY << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_CURRENT_KEYS                 ((MOCA_GROUP_SECURITY << 16) | 1028 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_NETWORK_PASSWORD             ((MOCA_GROUP_SECURITY << 16) | 1029 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_PERMANENT_SALT               ((MOCA_GROUP_SECURITY << 16) | 1030 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_AES_PMK_INITIAL_KEY          ((MOCA_GROUP_SECURITY << 16) | 1031 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_KEY_CHANGED                  ((MOCA_GROUP_SECURITY << 16) | 2048)
#define IE_KEY_TIMES                    ((MOCA_GROUP_SECURITY << 16) | 4096)
#define IE_PASSWORD                     ((MOCA_GROUP_SECURITY << 16) | 4097)

// Group debug
#define IE_MTM_EN                       ((MOCA_GROUP_DEBUG << 16) | 1)
#define IE_CONST_RX_SUBMODE             ((MOCA_GROUP_DEBUG << 16) | 2)
#define IE_CIR_PRINTS                   ((MOCA_GROUP_DEBUG << 16) | 3)
#define IE_SNR_PRINTS                   ((MOCA_GROUP_DEBUG << 16) | 4)
#define IE_MMP_VERSION                  ((MOCA_GROUP_DEBUG << 16) | 5)
#define IE_SIGMA2_PRINTS                ((MOCA_GROUP_DEBUG << 16) | 6)
#define IE_BAD_PROBE_PRINTS             ((MOCA_GROUP_DEBUG << 16) | 7)
#define IE_CONST_TX_PARAMS              ((MOCA_GROUP_DEBUG << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_GMII_TRAP_HEADER             ((MOCA_GROUP_DEBUG << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_LED_STATUS                   ((MOCA_GROUP_DEBUG << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MOCA_CORE_TRACE_ENABLE       ((MOCA_GROUP_DEBUG << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_ERROR                        ((MOCA_GROUP_DEBUG << 16) | 2048)
#define IE_FW_PRINTF                    ((MOCA_GROUP_DEBUG << 16) | 2049)
#define IE_WARNING                      ((MOCA_GROUP_DEBUG << 16) | 2050)
#define IE_ERROR_LOOKUP                 ((MOCA_GROUP_DEBUG << 16) | 2051)
#define IE_WARNING_LOOKUP               ((MOCA_GROUP_DEBUG << 16) | 2052)
#define IE_ERROR_TO_MASK                ((MOCA_GROUP_DEBUG << 16) | 4096)
#define IE_FW_FILE                      ((MOCA_GROUP_DEBUG << 16) | 4097)
#define IE_VERBOSE                      ((MOCA_GROUP_DEBUG << 16) | 4098)
#define IE_DONT_START_MOCA              ((MOCA_GROUP_DEBUG << 16) | 4099)
#define IE_NO_RTT                       ((MOCA_GROUP_DEBUG << 16) | 4100)
#define IE_MOCAD_PRINTF                 ((MOCA_GROUP_DEBUG << 16) | 4101)

// Group lab
#define IE_LAB_MODE                     ((MOCA_GROUP_LAB << 16) | 1)
#define IE_NC_MODE                      ((MOCA_GROUP_LAB << 16) | 2)
#define IE_RX_TX_PACKETS_PER_QM         ((MOCA_GROUP_LAB << 16) | 3)
#define IE_EXTRA_RX_PACKETS_PER_QM      ((MOCA_GROUP_LAB << 16) | 4)
#define IE_TARGET_PHY_RATE_20           ((MOCA_GROUP_LAB << 16) | 5)
#define IE_TARGET_PHY_RATE_20_TURBO     ((MOCA_GROUP_LAB << 16) | 6)
#define IE_TURBO_EN                     ((MOCA_GROUP_LAB << 16) | 7)
#define IE_RES1                         ((MOCA_GROUP_LAB << 16) | 8)
#define IE_RES2                         ((MOCA_GROUP_LAB << 16) | 9)
#define IE_RES3                         ((MOCA_GROUP_LAB << 16) | 10)
#define IE_RES4                         ((MOCA_GROUP_LAB << 16) | 11)
#define IE_RES5                         ((MOCA_GROUP_LAB << 16) | 12)
#define IE_RES6                         ((MOCA_GROUP_LAB << 16) | 13)
#define IE_RES7                         ((MOCA_GROUP_LAB << 16) | 14)
#define IE_RES8                         ((MOCA_GROUP_LAB << 16) | 15)
#define IE_RES9                         ((MOCA_GROUP_LAB << 16) | 16)
#define IE_INIT1                        ((MOCA_GROUP_LAB << 16) | 17)
#define IE_INIT2                        ((MOCA_GROUP_LAB << 16) | 18)
#define IE_INIT3                        ((MOCA_GROUP_LAB << 16) | 19)
#define IE_INIT4                        ((MOCA_GROUP_LAB << 16) | 20)
#define IE_INIT5                        ((MOCA_GROUP_LAB << 16) | 21)
#define IE_INIT6                        ((MOCA_GROUP_LAB << 16) | 22)
#define IE_INIT7                        ((MOCA_GROUP_LAB << 16) | 23)
#define IE_INIT8                        ((MOCA_GROUP_LAB << 16) | 24)
#define IE_INIT9                        ((MOCA_GROUP_LAB << 16) | 25)
#define IE_LAB_SNR_GRAPH_SET            ((MOCA_GROUP_LAB << 16) | 26)
#define IE_BLOCK_NACK_RATE              ((MOCA_GROUP_LAB << 16) | 27)
#define IE_RES10                        ((MOCA_GROUP_LAB << 16) | 28)
#define IE_RES11                        ((MOCA_GROUP_LAB << 16) | 29)
#define IE_RES12                        ((MOCA_GROUP_LAB << 16) | 30)
#define IE_RES13                        ((MOCA_GROUP_LAB << 16) | 31)
#define IE_RES14                        ((MOCA_GROUP_LAB << 16) | 32)
#define IE_RES15                        ((MOCA_GROUP_LAB << 16) | 33)
#define IE_RES16                        ((MOCA_GROUP_LAB << 16) | 34)
#define IE_RES17                        ((MOCA_GROUP_LAB << 16) | 35)
#define IE_RES18                        ((MOCA_GROUP_LAB << 16) | 36)
#define IE_RES19                        ((MOCA_GROUP_LAB << 16) | 37)
#define IE_RES20                        ((MOCA_GROUP_LAB << 16) | 38)
#define IE_TARGET_PHY_RATE_20_TURBO_VLPER ((MOCA_GROUP_LAB << 16) | 39)
#define IE_TARGET_PHY_RATE_20_SEC_CH    ((MOCA_GROUP_LAB << 16) | 40)
#define IE_TARGET_PHY_RATE_20_TURBO_SEC_CH ((MOCA_GROUP_LAB << 16) | 41)
#define IE_TARGET_PHY_RATE_20_TURBO_VLPER_SEC_CH ((MOCA_GROUP_LAB << 16) | 42)
#define IE_CAP_PHY_RATE_EN              ((MOCA_GROUP_LAB << 16) | 43)
#define IE_CAP_TARGET_PHY_RATE          ((MOCA_GROUP_LAB << 16) | 44)
#define IE_CAP_SNR_BASE_MARGIN          ((MOCA_GROUP_LAB << 16) | 45)
#define IE_MAP_CAPTURE                  ((MOCA_GROUP_LAB << 16) | 46)
#define IE_LAB_PILOTS                   ((MOCA_GROUP_LAB << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_LAB_IQ_DIAGRAM_SET           ((MOCA_GROUP_LAB << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_LAB_REGISTER                 ((MOCA_GROUP_LAB << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_LAB_CMD                      ((MOCA_GROUP_LAB << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_LAB_TPCAP                    ((MOCA_GROUP_LAB << 16) | 1028 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_HOST_POOL                    ((MOCA_GROUP_LAB << 16) | 1029 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_FORCE_HANDOFF                ((MOCA_GROUP_LAB << 16) | 1030 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_TPCAP_CAPTURE_TIME           ((MOCA_GROUP_LAB << 16) | 1031 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_LAB_PRINTF                   ((MOCA_GROUP_LAB << 16) | 2048)
#define IE_LAB_PRINTF_CODES             ((MOCA_GROUP_LAB << 16) | 2049)
#define IE_ASSERT                       ((MOCA_GROUP_LAB << 16) | 2050)
#define IE_MIPS_EXCEPTION               ((MOCA_GROUP_LAB << 16) | 2051)
#define IE_DRV_PRINTF                   ((MOCA_GROUP_LAB << 16) | 2052)
#define IE_LAB_PRINTF_CODES_LOOKUP      ((MOCA_GROUP_LAB << 16) | 2053)
#define IE_MESSAGE                      ((MOCA_GROUP_LAB << 16) | 4096)
#define IE_SNR_DATA                     ((MOCA_GROUP_LAB << 16) | 4097)
#define IE_IQ_DATA                      ((MOCA_GROUP_LAB << 16) | 4098)
#define IE_CIR_DATA                     ((MOCA_GROUP_LAB << 16) | 4099)
#define IE_ASSERT_RESTART               ((MOCA_GROUP_LAB << 16) | 4100)
#define IE_MOCA_CPU_FREQ                ((MOCA_GROUP_LAB << 16) | 4101)

// Group mps
#define IE_MPS_EN                       ((MOCA_GROUP_MPS << 16) | 1)
#define IE_MPS_PRIVACY_RECEIVE          ((MOCA_GROUP_MPS << 16) | 2)
#define IE_MPS_PRIVACY_DOWN             ((MOCA_GROUP_MPS << 16) | 3)
#define IE_MPS_WALK_TIME                ((MOCA_GROUP_MPS << 16) | 4)
#define IE_MPS_UNPAIRED_TIME            ((MOCA_GROUP_MPS << 16) | 5)
#define IE_MPS_STATE                    ((MOCA_GROUP_MPS << 16) | 6)
#define IE_MPS_INIT_SCAN_PAYLOAD        ((MOCA_GROUP_MPS << 16) | 1024 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MPS_TRIG                     ((MOCA_GROUP_MPS << 16) | 1025 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MPS_DATA                     ((MOCA_GROUP_MPS << 16) | 1026 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MPS_KEY                      ((MOCA_GROUP_MPS << 16) | 1027 | MOCA_MMP_FW_FUNC_FLAG)
#define IE_MPS_PRIVACY_CHANGED          ((MOCA_GROUP_MPS << 16) | 2048)
#define IE_MPS_TRIGGER                  ((MOCA_GROUP_MPS << 16) | 2049)
#define IE_MPS_PAIR_FAIL                ((MOCA_GROUP_MPS << 16) | 2050)
#define IE_INIT_SCAN_REC                ((MOCA_GROUP_MPS << 16) | 2051)
#define IE_MPS_REQUEST_MPSKEY           ((MOCA_GROUP_MPS << 16) | 2052)
#define IE_MPS_ADMISSION_NOCHANGE       ((MOCA_GROUP_MPS << 16) | 2053)
#define IE_MPS_BUTTON_PRESS             ((MOCA_GROUP_MPS << 16) | 4096)
#define IE_MPS_RESET                    ((MOCA_GROUP_MPS << 16) | 4097)
#define IE_PRIVACY_DEFAULTS             ((MOCA_GROUP_MPS << 16) | 4098)
#define IE_MPS_DATA_READY               ((MOCA_GROUP_MPS << 16) | 4099)

#define MOCA_MAX_NBAS_PRIMARY_DEF                  0

#define MOCA_PS_SWCH_TX1_DEF                       0

#define MOCA_PS_SWCH_TX2_DEF                       0

#define MOCA_PS_SWCH_RX1_DEF                       0

#define MOCA_PS_SWCH_RX2_DEF                       0

#define MOCA_PS_SWCH_RX3_DEF                       0

#define MOCA_PRIM_CH_OFFS_DEF                      0
#define MOCA_PRIM_CH_OFFS_MIN                      0
#define MOCA_PRIM_CH_OFFS_MAX                      2

#define MOCA_SEC_CH_OFFS_DEF                       1
#define MOCA_SEC_CH_OFFS_MIN                       0
#define MOCA_SEC_CH_OFFS_MAX                       2

#define MOCA_NON_DEF_SEQ_NUM_MIN                   0
#define MOCA_NON_DEF_SEQ_NUM_MAX                   0xFFFF

#define MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_DEF  0
#define MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_BONDING_SUPPORTED_DEF  0xB
#define MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_MIN  0
#define MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_MAX  0
#define MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_BONDING_SUPPORTED_MAX  0xF

#define MOCA_MAX_NBAS_SECONDARY_DEF                0

#define MOCA_PROF_PAD_CTRL_DEG_6802C0_SINGLE_OFFSET_DEF  0xB
#define MOCA_PROF_PAD_CTRL_DEG_6802C0_SINGLE_OFFSET_MIN  0
#define MOCA_PROF_PAD_CTRL_DEG_6802C0_SINGLE_OFFSET_MAX  0xF

#define MOCA_CP_CONST_DEF                          0
#define MOCA_CP_CONST_MIN                          0
#define MOCA_CP_CONST_MAX                          64

#define MOCA_PREAMBLE_UC_CONST_DEF                 0

#define MOCA_CP_MARGIN_INCREASE_DEF                0

#define MOCA_AC_CC_SHIFT_DEF                       0

#define MOCA_MFC_TH_INCREASE_DEF                   0

#define MOCA_AGC_CONST_EN_DEF                      0
#define MOCA_AGC_CONST_EN_MIN                      0
#define MOCA_AGC_CONST_EN_MAX                      1

#define MOCA_AGC_CONST_ADDRESS_DEF                 0

#define MOCA_MIN_BO_INSERT_2_BFM_LOCK_DEF          0

#define MOCA_MIN_SNR_AVG_DB_2_BFM_LOCK_DEF         0

#define MOCA_SNR_MARGIN_TABLE_RS_MGNTABLE_DEF      0
#define MOCA_SNR_MARGIN_TABLE_RS_MGNTABLE_MIN      0
#define MOCA_SNR_MARGIN_TABLE_RS_MGNTABLE_MAX      12800

#define MOCA_SAPM_TABLE_FW_50_SAPMTABLELO_DEF      0
#define MOCA_SAPM_TABLE_FW_50_SAPMTABLELO_MIN      0
#define MOCA_SAPM_TABLE_FW_50_SAPMTABLELO_MAX      120

#define MOCA_SAPM_TABLE_FW_50_SAPMTABLEHI_DEF      0
#define MOCA_SAPM_TABLE_FW_50_SAPMTABLEHI_MIN      0
#define MOCA_SAPM_TABLE_FW_50_SAPMTABLEHI_MAX      120

#define MOCA_SNR_MARGIN_TABLE_LDPC_MGNTABLE_DEF    0
#define MOCA_SNR_MARGIN_TABLE_LDPC_MGNTABLE_MIN    0
#define MOCA_SNR_MARGIN_TABLE_LDPC_MGNTABLE_MAX    12800

#define MOCA_SNR_MARGIN_TABLE_LDPC_SEC_CH_MGNTABLE_DEF  0
#define MOCA_SNR_MARGIN_TABLE_LDPC_SEC_CH_MGNTABLE_MIN  0
#define MOCA_SNR_MARGIN_TABLE_LDPC_SEC_CH_MGNTABLE_MAX  12800

#define MOCA_SNR_MARGIN_TABLE_LDPC_PRE5_MGNTABLE_DEF  0
#define MOCA_SNR_MARGIN_TABLE_LDPC_PRE5_MGNTABLE_MIN  0
#define MOCA_SNR_MARGIN_TABLE_LDPC_PRE5_MGNTABLE_MAX  12800

#define MOCA_SNR_MARGIN_TABLE_OFDMA_MGNTABLE_DEF   0
#define MOCA_SNR_MARGIN_TABLE_OFDMA_MGNTABLE_MIN   0
#define MOCA_SNR_MARGIN_TABLE_OFDMA_MGNTABLE_MAX   12800

#define MOCA_SAPM_TABLE_FW_100_SAPMTABLELO_DEF     0
#define MOCA_SAPM_TABLE_FW_100_SAPMTABLELO_MIN     0
#define MOCA_SAPM_TABLE_FW_100_SAPMTABLELO_MAX     120

#define MOCA_SAPM_TABLE_FW_100_SAPMTABLEHI_DEF     0
#define MOCA_SAPM_TABLE_FW_100_SAPMTABLEHI_MIN     0
#define MOCA_SAPM_TABLE_FW_100_SAPMTABLEHI_MAX     120

#define MOCA_CPLENGTH_MAX                          255

#define MOCA_RF_CALIB_DATA_VALID_DEF               0

#define MOCA_RF_CALIB_DATA_DATA_DEF                0

#define MOCA_PROBE_2_RESULTS_THETA_DEF             0

#define MOCA_PROBE_2_RESULTS_RHO_DEF               0

#define MOCA_PROBE_2_RESULTS_SCALE_DEF             0

#define MOCA_SAPM_TABLE_FW_SEC_SAPMTABLELO_DEF     0
#define MOCA_SAPM_TABLE_FW_SEC_SAPMTABLELO_MIN     0
#define MOCA_SAPM_TABLE_FW_SEC_SAPMTABLELO_MAX     120

#define MOCA_SAPM_TABLE_FW_SEC_SAPMTABLEHI_DEF     0
#define MOCA_SAPM_TABLE_FW_SEC_SAPMTABLEHI_MIN     0
#define MOCA_SAPM_TABLE_FW_SEC_SAPMTABLEHI_MAX     120

#define MOCA_SNR_MARGIN_TABLE_LDPC_PRI_CH_MGNTABLE_DEF  0
#define MOCA_SNR_MARGIN_TABLE_LDPC_PRI_CH_MGNTABLE_MIN  0
#define MOCA_SNR_MARGIN_TABLE_LDPC_PRI_CH_MGNTABLE_MAX  12800

#define MOCA_SNR_MARGIN_TABLE_PRE5_PRI_CH_MGNTABLE_DEF  0
#define MOCA_SNR_MARGIN_TABLE_PRE5_PRI_CH_MGNTABLE_MIN  0
#define MOCA_SNR_MARGIN_TABLE_PRE5_PRI_CH_MGNTABLE_MAX  12800

#define MOCA_SNR_MARGIN_TABLE_PRE5_SEC_CH_MGNTABLE_DEF  0
#define MOCA_SNR_MARGIN_TABLE_PRE5_SEC_CH_MGNTABLE_MIN  0
#define MOCA_SNR_MARGIN_TABLE_PRE5_SEC_CH_MGNTABLE_MAX  12800

#define MOCA_FRAGMENTATION_DEF                     1
#define MOCA_FRAGMENTATION_MIN                     0
#define MOCA_FRAGMENTATION_MAX                     1

#define MOCA_IFG_MOCA20_DEF                        0

#define MOCA_MAP_SEED_DEF                          0

#define MOCA_PROBE_REQUEST_PROBE_TYPE_DEF          0
#define MOCA_PROBE_REQUEST_PROBE_TYPE_MIN          0
#define MOCA_PROBE_REQUEST_PROBE_TYPE_MAX          1

#define MOCA_PROBE_REQUEST_TIMESLOTS_MIN           0
#define MOCA_PROBE_REQUEST_TIMESLOTS_MAX           20000

#define MOCA_LOW_PRI_Q_NUM_6816_DEF                2
#define MOCA_LOW_PRI_Q_NUM_DEF                     0

#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_PQOS_DEF  9
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_PQOS_7408_DEF  1
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_PQOS_MIN  0
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_PQOS_MAX  300

#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_HIGH_DEF  9
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_HIGH_7408_DEF  1
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_HIGH_MIN  0
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_HIGH_MAX  300

#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_MED_DEF  64
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_MED_7408_DEF  1
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_MED_MIN  0
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_MED_MAX  300

#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_LOW_DEF  64
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_LOW_7408_DEF  10
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_LOW_MIN  0
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_LOW_MAX  300

#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_PQOS_DEF  300
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_PQOS_MIN  0
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_PQOS_MAX  300

#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_HIGH_DEF  300
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_HIGH_MIN  0
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_HIGH_MAX  300

#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_MED_DEF  300
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_MED_MIN  0
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_MED_MAX  300

#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_LOW_DEF  300
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_LOW_MIN  0
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_LOW_MAX  300

#define MOCA_PQOS_CREATE_REQUEST_EGRESSNODEID_DEF  0x3F

#define MOCA_PQOS_LIST_REQUEST_FLOWSTARTINDEX_DEF  0

#define MOCA_PQOS_LIST_REQUEST_FLOWMAXRETURN_DEF   32

#define MOCA_OOO_LMO_THRESHOLD_DEF                 5

#define MOCA_START_ACA_SRC_NODE_MIN                0
#define MOCA_START_ACA_SRC_NODE_MAX                15

#define MOCA_START_ACA_DEST_NODEMASK_MIN           1

#define MOCA_START_ACA_NUM_PROBES_MAX              8

#define MOCA_START_ACA_TYPE_DEF                    1
#define MOCA_START_ACA_TYPE_MIN                    1
#define MOCA_START_ACA_TYPE_MAX                    2

#define MOCA_RF_SWITCH_DEF                         0
#define MOCA_RF_SWITCH_MIN                         0
#define MOCA_RF_SWITCH_MAX                         2

#define MOCA_WOM_MODE_INTERNAL_DEF                 0
#define MOCA_WOM_MODE_INTERNAL_MIN                 0
#define MOCA_WOM_MODE_INTERNAL_MAX                 1

#define MOCA_POWER_STATE_MIN                       0
#define MOCA_POWER_STATE_MAX                       3

#define MOCA_NETWORK_PASSWORD_PASSWORD_DEF         0

#define MOCA_CONST_RX_SUBMODE_DEF                  0
#define MOCA_CONST_RX_SUBMODE_MIN                  0
#define MOCA_CONST_RX_SUBMODE_MAX                  1

#define MOCA_RES1_DEF                              0

#define MOCA_RES2_DEF                              0

#define MOCA_RES3_DEF                              0

#define MOCA_RES4_DEF                              0

#define MOCA_RES5_DEF                              0

#define MOCA_RES6_DEF                              0

#define MOCA_RES7_DEF                              0

#define MOCA_RES8_DEF                              0

#define MOCA_RES9_DEF                              0

#define MOCA_INIT1_DEF                             0

#define MOCA_INIT2_DEF                             0

#define MOCA_INIT3_DEF                             0

#define MOCA_INIT4_DEF                             0

#define MOCA_INIT5_DEF                             0

#define MOCA_INIT6_DEF                             0

#define MOCA_INIT7_DEF                             0

#define MOCA_INIT8_DEF                             0

#define MOCA_INIT9_DEF                             0

#define MOCA_LAB_SNR_GRAPH_SET_DEF                 0xff

#define MOCA_BLOCK_NACK_RATE_DEF                   0

#define MOCA_RES10_DEF                             0

#define MOCA_RES11_DEF                             0

#define MOCA_RES12_DEF                             0

#define MOCA_RES13_DEF                             0

#define MOCA_RES14_DEF                             0

#define MOCA_RES15_DEF                             0

#define MOCA_RES16_DEF                             0

#define MOCA_RES17_DEF                             0

#define MOCA_RES18_DEF                             0

#define MOCA_RES19_DEF                             0

#define MOCA_RES20_DEF                             0

#define MOCA_MAP_CAPTURE_DEF                       0
#define MOCA_MAP_CAPTURE_MIN                       0
#define MOCA_MAP_CAPTURE_MAX                       1

#define MOCA_ASSERT_RESTART_DEF                    1

#define MOCA_MOCA_CPU_FREQ_DEF                     0

#define MOCA_MPS_STATE_DEF                         0

#define MOCA_MPS_TRIG_DEF                          0

#define MOCA_PRIM_CH_OFFS_ERR                                          -1001
#define MOCA_SEC_CH_OFFS_ERR                                           -1002
#define MOCA_NON_DEF_SEQ_NUM_ERR                                       -1003
#define MOCA_PROF_PAD_CTRL_DEG_6802C0_BONDING_ERR                      -1013
#define MOCA_PROF_PAD_CTRL_DEG_6802C0_SINGLE_OFFSET_ERR                -1022
#define MOCA_CP_CONST_ERR                                              -1038
#define MOCA_PREAMBLE_UC_CONST_ERR                                     -1039
#define MOCA_AGC_CONST_EN_ERR                                          -1040
#define MOCA_SNR_MARGIN_TABLE_RS_MGNTABLE_ERR                          -1047
#define MOCA_SAPM_TABLE_FW_50_SAPMTABLELO_ERR                          -1048
#define MOCA_SAPM_TABLE_FW_50_SAPMTABLEHI_ERR                          -1049
#define MOCA_SNR_MARGIN_TABLE_LDPC_MGNTABLE_ERR                        -1051
#define MOCA_SNR_MARGIN_TABLE_LDPC_SEC_CH_MGNTABLE_ERR                 -1052
#define MOCA_SNR_MARGIN_TABLE_LDPC_PRE5_MGNTABLE_ERR                   -1053
#define MOCA_SNR_MARGIN_TABLE_OFDMA_MGNTABLE_ERR                       -1054
#define MOCA_SAPM_TABLE_FW_100_SAPMTABLELO_ERR                         -1055
#define MOCA_SAPM_TABLE_FW_100_SAPMTABLEHI_ERR                         -1056
#define MOCA_CPLENGTH_ERR                                              -1058
#define MOCA_SAPM_TABLE_FW_SEC_SAPMTABLELO_ERR                         -1059
#define MOCA_SAPM_TABLE_FW_SEC_SAPMTABLEHI_ERR                         -1060
#define MOCA_SNR_MARGIN_TABLE_LDPC_PRI_CH_MGNTABLE_ERR                 -1061
#define MOCA_SNR_MARGIN_TABLE_PRE5_PRI_CH_MGNTABLE_ERR                 -1062
#define MOCA_SNR_MARGIN_TABLE_PRE5_SEC_CH_MGNTABLE_ERR                 -1063
#define MOCA_FRAGMENTATION_ERR                                         -1085
#define MOCA_PROBE_REQUEST_PROBE_TYPE_ERR                              -1088
#define MOCA_PROBE_REQUEST_TIMESLOTS_ERR                               -1089
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_PQOS_ERR                 -1102
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_HIGH_ERR                 -1103
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_MED_ERR                  -1104
#define MOCA_PRIORITY_ALLOCATIONS_RESERVATION_LOW_ERR                  -1105
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_PQOS_ERR                  -1106
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_HIGH_ERR                  -1107
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_MED_ERR                   -1108
#define MOCA_PRIORITY_ALLOCATIONS_LIMITATION_LOW_ERR                   -1109
#define MOCA_START_ACA_SRC_NODE_ERR                                    -1163
#define MOCA_START_ACA_DEST_NODEMASK_ERR                               -1164
#define MOCA_START_ACA_NUM_PROBES_ERR                                  -1165
#define MOCA_START_ACA_TYPE_ERR                                        -1166
#define MOCA_RF_SWITCH_ERR                                             -1174
#define MOCA_WOM_MODE_INTERNAL_ERR                                     -1180
#define MOCA_POWER_STATE_ERR                                           -1183
#define MOCA_CONST_RX_SUBMODE_ERR                                      -1192
#define MOCA_MAP_CAPTURE_ERR                                           -1194


struct moca_mac_addr_int {
   macaddr_t               val;
   uint16_t                reserved_0;
} __attribute__((packed,aligned(4)));

struct moca_prof_pad_ctrl_deg_6802c0_single {
   int8_t                  offset[86];
   uint16_t                padding;
} __attribute__((packed,aligned(4)));

struct moca_sig_y_done {
   uint32_t                lmo_counter;
   uint32_t                cp;
   uint32_t                cc_offset;
   uint32_t                ac_offset;
   uint32_t                cp_min_ac_delay_offset;
   uint32_t                enableHT;
   uint32_t                cp_overflow;
   uint32_t                reg_pp_cc_maxf_th_scl;
} __attribute__((packed,aligned(4)));

struct moca_snr_margin_table_rs {
   uint16_t                mgntable[22];
} __attribute__((packed,aligned(4)));

struct moca_sapm_table_fw_50 {
   uint8_t                 sapmtablelo[112];
   uint8_t                 sapmtablehi[112];
} __attribute__((packed,aligned(4)));

struct moca_snr_margin_table_ldpc {
   uint16_t                mgntable[22];
} __attribute__((packed,aligned(4)));

struct moca_snr_margin_table_ldpc_sec_ch {
   uint16_t                mgntable[22];
} __attribute__((packed,aligned(4)));

struct moca_snr_margin_table_ldpc_pre5 {
   uint16_t                mgntable[22];
} __attribute__((packed,aligned(4)));

struct moca_snr_margin_table_ofdma {
   uint16_t                mgntable[22];
} __attribute__((packed,aligned(4)));

struct moca_sapm_table_fw_100 {
   uint8_t                 sapmtablelo[240];
   uint8_t                 sapmtablehi[240];
} __attribute__((packed,aligned(4)));

struct moca_rf_calib_data {
   uint32_t                valid;
   uint8_t                 data[480];
} __attribute__((packed,aligned(4)));

struct moca_probe_2_results {
   uint32_t                theta;
   uint32_t                rho;
   uint32_t                scale;
} __attribute__((packed,aligned(4)));

struct moca_sapm_table_fw_sec {
   uint8_t                 sapmtablelo[240];
   uint8_t                 sapmtablehi[240];
} __attribute__((packed,aligned(4)));

struct moca_snr_margin_table_ldpc_pri_ch {
   uint16_t                mgntable[22];
} __attribute__((packed,aligned(4)));

struct moca_snr_margin_table_pre5_pri_ch {
   uint16_t                mgntable[22];
} __attribute__((packed,aligned(4)));

struct moca_snr_margin_table_pre5_sec_ch {
   uint16_t                mgntable[22];
} __attribute__((packed,aligned(4)));

struct moca_sig_y_ready {
   uint32_t                lmo_counter;
   uint32_t                descriptor_addr;
   uint32_t                numberOfSymbales;
   uint32_t                min_cp_threshold;
   uint32_t                firstTime2_calc_cp;
   uint32_t                reg_pp_cc_maxf_th_scl;
   uint32_t                g_pp_cc_maxf_th_scl;
   uint32_t                g_cp_min;
   uint32_t                g_cp_max;
   uint32_t                reg_pp_cc_avg_length;
} __attribute__((packed,aligned(4)));

struct moca_new_rf_calib_data {
   uint8_t                 data[480];
} __attribute__((packed,aligned(4)));

struct moca_new_probe_2_results {
   uint32_t                theta;
   uint32_t                rho;
   uint32_t                scale;
} __attribute__((packed,aligned(4)));

struct moca_probe_request {
   uint16_t                probe_type;
   uint16_t                timeslots;
} __attribute__((packed,aligned(4)));

struct moca_priority_allocations {
   uint32_t                reservation_pqos;
   uint32_t                reservation_high;
   uint32_t                reservation_med;
   uint32_t                reservation_low;
   uint32_t                limitation_pqos;
   uint32_t                limitation_high;
   uint32_t                limitation_med;
   uint32_t                limitation_low;
} __attribute__((packed,aligned(4)));

struct moca_egr_mc_addr_filter_set_int {
   uint32_t                entryid;
   uint32_t                valid;
   macaddr_t               addr;
   uint16_t                reserved_0;
} __attribute__((packed,aligned(4)));

struct moca_pqos_create_request {
   uint32_t                wave0Nodemask;
   macaddr_t               flowid;
   uint16_t                tPacketSize;
   uint8_t                 ingressNodeId;
   uint8_t                 max_latency;
   uint8_t                 short_term_avg_ratio;
   uint8_t                 egressNodeId;
   uint32_t                flowTag;
   macaddr_t               packetda;
   uint32_t                tPeakDataRate;
   uint32_t                tLeaseTime;
   uint8_t                 tBurstSize;
   uint8_t                 retry_per_delivery;
   uint16_t                proto_rule_vlan_dscp;
} __attribute__((packed,aligned(4)));

struct moca_pqos_create_request_int {
   uint8_t                 hdrFmt;
   uint8_t                 entryNodeId;
   uint8_t                 entryIndex;
   uint8_t                 reserved_0;
   uint16_t                vendorId;
   uint8_t                 transType;
   uint8_t                 transSubtype;
   uint32_t                wave0Nodemask;
   uint32_t                reserved_1;
   uint8_t                 reserved_2;
   uint8_t                 msgPriority;
   uint8_t                 txnLastWaveNum;
   uint8_t                 reserved_3;
   macaddr_t               flowid;
   uint16_t                reserved_4;
   uint16_t                tPacketSize;
   uint8_t                 reserved_5;
   uint8_t                 ingressNodeId;
   uint8_t                 max_latency;
   uint8_t                 short_term_avg_ratio;
   uint8_t                 reserved_6;
   uint8_t                 egressNodeId;
   uint32_t                flowTag;
   macaddr_t               packetda;
   uint16_t                reserved_7;
   uint32_t                tPeakDataRate;
   uint32_t                tLeaseTime;
   uint8_t                 tBurstSize;
   uint8_t                 retry_per_delivery;
   uint16_t                proto_rule_vlan_dscp;
} __attribute__((packed,aligned(4)));

struct moca_pqos_ingr_add_flow {
   macaddr_t               flowid;
   uint32_t                flowtag;
   uint32_t                qtag;
   uint32_t                tpeakdatarate;
   uint32_t                tpacketsize;
   uint32_t                tburstsize;
   uint32_t                tleasetime;
   uint32_t                egressnodeid;
   macaddr_t               flowsa;
   macaddr_t               flowda;
   uint32_t                flowvlanid;
   uint32_t                committedstps;
   uint32_t                committedtxps;
   uint32_t                dest_flow_id;
   uint32_t                maximum_latency;
   uint32_t                short_term_avg_ratio;
   uint32_t                max_number_retry;
   uint32_t                flow_per;
   uint32_t                in_order_delivery;
   uint32_t                ingr_class_rule;
   uint32_t                traffic_protocol;
   uint32_t                vlan_tag;
   uint32_t                dscp_moca;
} __attribute__((packed,aligned(4)));

struct moca_pqos_ingr_add_flow_int {
   macaddr_t               flowid;
   uint16_t                reserved_0;
   uint32_t                flowtag;
   uint32_t                qtag;
   uint32_t                tpeakdatarate;
   uint32_t                tpacketsize;
   uint32_t                tburstsize;
   uint32_t                tleasetime;
   uint32_t                egressnodeid;
   macaddr_t               flowsa;
   macaddr_t               flowda;
   uint32_t                flowvlanid;
   uint32_t                committedstps;
   uint32_t                committedtxps;
   uint32_t                dest_flow_id;
   uint32_t                maximum_latency;
   uint32_t                short_term_avg_ratio;
   uint32_t                max_number_retry;
   uint32_t                flow_per;
   uint32_t                in_order_delivery;
   uint32_t                ingr_class_rule;
   uint32_t                traffic_protocol;
   uint32_t                vlan_tag;
   uint32_t                dscp_moca;
} __attribute__((packed,aligned(4)));

struct moca_pqos_update_request {
   uint32_t                wave0Nodemask;
   macaddr_t               flowid;
   uint16_t                tPacketSize;
   uint8_t                 ingressNodeId;
   uint8_t                 max_latency;
   uint8_t                 short_term_avg_ratio;
   uint8_t                 egressNodeId;
   uint32_t                flowTag;
   macaddr_t               packetda;
   uint32_t                tPeakDataRate;
   uint32_t                tLeaseTime;
   uint8_t                 tBurstSize;
   uint8_t                 retry_per_delivery;
   uint16_t                proto_rule_vlan_dscp;
} __attribute__((packed,aligned(4)));

struct moca_pqos_update_request_int {
   uint8_t                 hdrFmt;
   uint8_t                 entryNodeId;
   uint8_t                 entryIndex;
   uint8_t                 reserved_0;
   uint16_t                vendorId;
   uint8_t                 transType;
   uint8_t                 transSubtype;
   uint32_t                wave0Nodemask;
   uint32_t                reserved_1;
   uint8_t                 reserved_2;
   uint8_t                 msgPriority;
   uint8_t                 txnLastWaveNum;
   uint8_t                 reserved_3;
   macaddr_t               flowid;
   uint16_t                reserved_4;
   uint16_t                tPacketSize;
   uint8_t                 reserved_5;
   uint8_t                 ingressNodeId;
   uint8_t                 max_latency;
   uint8_t                 short_term_avg_ratio;
   uint8_t                 reserved_6;
   uint8_t                 egressNodeId;
   uint32_t                flowTag;
   macaddr_t               packetda;
   uint16_t                reserved_7;
   uint32_t                tPeakDataRate;
   uint32_t                tLeaseTime;
   uint8_t                 tBurstSize;
   uint8_t                 retry_per_delivery;
   uint16_t                proto_rule_vlan_dscp;
} __attribute__((packed,aligned(4)));

struct moca_pqos_ingr_update {
   macaddr_t               flowid;
   uint32_t                flowtag;
   macaddr_t               flowda;
   uint32_t                tpeakdatarate;
   uint32_t                tpacketsize;
   uint32_t                tburstsize;
   uint32_t                tleasetime;
   uint32_t                committedstps;
   uint32_t                committedtxps;
   uint32_t                maximum_latency;
   uint32_t                short_term_avg_ratio;
   uint32_t                max_number_retry;
   uint32_t                flow_per;
   uint32_t                in_order_delivery;
   uint32_t                ingr_class_rule;
   uint32_t                traffic_protocol;
   uint32_t                vlan_tag;
   uint32_t                dscp_moca;
} __attribute__((packed,aligned(4)));

struct moca_pqos_ingr_update_int {
   macaddr_t               flowid;
   uint16_t                reserved_0;
   uint32_t                flowtag;
   macaddr_t               flowda;
   uint16_t                reserved_1;
   uint32_t                tpeakdatarate;
   uint32_t                tpacketsize;
   uint32_t                tburstsize;
   uint32_t                tleasetime;
   uint32_t                committedstps;
   uint32_t                committedtxps;
   uint32_t                maximum_latency;
   uint32_t                short_term_avg_ratio;
   uint32_t                max_number_retry;
   uint32_t                flow_per;
   uint32_t                in_order_delivery;
   uint32_t                ingr_class_rule;
   uint32_t                traffic_protocol;
   uint32_t                vlan_tag;
   uint32_t                dscp_moca;
} __attribute__((packed,aligned(4)));

struct moca_pqos_delete_request {
   uint32_t                wave0Nodemask;
   macaddr_t               flowid;
} __attribute__((packed,aligned(4)));

struct moca_pqos_delete_request_int {
   uint8_t                 hdrFmt;
   uint8_t                 entryNodeId;
   uint8_t                 entryIndex;
   uint8_t                 reserved_0;
   uint16_t                vendorId;
   uint8_t                 transType;
   uint8_t                 transSubtype;
   uint32_t                wave0Nodemask;
   uint32_t                reserved_1;
   uint8_t                 reserved_2;
   uint8_t                 msgPriority;
   uint8_t                 txnLastWaveNum;
   uint8_t                 reserved_3;
   macaddr_t               flowid;
   uint16_t                reserved_4;
   uint32_t                reserved_5;
} __attribute__((packed,aligned(4)));

struct moca_pqos_ingr_delete {
   macaddr_t               flowid;
} __attribute__((packed,aligned(4)));

struct moca_pqos_ingr_delete_int {
   macaddr_t               flowid;
   uint16_t                reserved_0;
} __attribute__((packed,aligned(4)));

struct moca_pqos_list_request {
   uint32_t                wave0Nodemask;
   uint32_t                flowStartIndex;
   uint8_t                 flowMaxReturn;
} __attribute__((packed,aligned(4)));

struct moca_pqos_list_request_int {
   uint8_t                 hdrFmt;
   uint8_t                 entryNodeId;
   uint8_t                 entryIndex;
   uint8_t                 reserved_0;
   uint16_t                vendorId;
   uint8_t                 transType;
   uint8_t                 transSubtype;
   uint32_t                wave0Nodemask;
   uint32_t                reserved_1;
   uint8_t                 reserved_2;
   uint8_t                 msgPriority;
   uint8_t                 txnLastWaveNum;
   uint8_t                 reserved_3;
   uint32_t                flowStartIndex;
   uint8_t                 flowMaxReturn;
   uint8_t                 reserved_4[3];
} __attribute__((packed,aligned(4)));

struct moca_pqos_query_request {
   uint32_t                wave0Nodemask;
   macaddr_t               flowid;
} __attribute__((packed,aligned(4)));

struct moca_pqos_query_request_int {
   uint8_t                 hdrFmt;
   uint8_t                 entryNodeId;
   uint8_t                 entryIndex;
   uint8_t                 reserved_0;
   uint16_t                vendorId;
   uint8_t                 transType;
   uint8_t                 transSubtype;
   uint32_t                wave0Nodemask;
   uint32_t                reserved_1;
   uint8_t                 reserved_2;
   uint8_t                 msgPriority;
   uint8_t                 txnLastWaveNum;
   uint8_t                 reserved_3;
   uint32_t                reserved_4;
   macaddr_t               flowid;
   uint16_t                reserved_5;
} __attribute__((packed,aligned(4)));

struct moca_mc_fwd_set_int {
   macaddr_t               multicast_mac_addr;
   macaddr_t               dest_mac_addr1;
   macaddr_t               dest_mac_addr2;
   macaddr_t               dest_mac_addr3;
   macaddr_t               dest_mac_addr4;
   uint16_t                reserved_0;
} __attribute__((packed,aligned(4)));

struct moca_mac_aging_int {
   uint16_t                uc_fwd_age;
   uint16_t                mc_fwd_age;
   uint16_t                src_addr_age;
   uint16_t                reserved_0;
} __attribute__((packed,aligned(4)));

struct moca_mcfilter_addentry_int {
   macaddr_t               addr;
   uint16_t                reserved_0;
} __attribute__((packed,aligned(4)));

struct moca_mcfilter_delentry_int {
   macaddr_t               addr;
   uint16_t                reserved_0;
} __attribute__((packed,aligned(4)));

struct moca_pqos_create_response {
   macaddr_t               flowid;
   macaddr_t               flowda;
   uint32_t                responsecode;
   uint32_t                decision;
   uint32_t                bw_limit_info;
   uint32_t                maxburstsize;
   uint32_t                max_short_term_avg_ratio;
   uint32_t                maxpeakdatarate;
   uint32_t                flowtag;
   uint32_t                ingressnodeid;
   uint32_t                egressnodebitmask;
   uint32_t                tpacketsize;
   uint32_t                tleasetime;
   uint32_t                totalstps;
   uint32_t                totaltxps;
   uint32_t                flowstps;
   uint32_t                flowtxps;
   uint32_t                dest_flow_id;
   uint32_t                maximum_latency;
   uint32_t                short_term_avg_ratio;
   uint32_t                max_number_retry;
   uint32_t                flow_per;
   uint32_t                in_order_delivery;
   uint32_t                ingr_class_rule;
   uint32_t                traffic_protocol;
   uint32_t                vlan_tag;
   uint32_t                dscp_moca;
} __attribute__((packed,aligned(4)));

struct moca_pqos_create_complete {
   macaddr_t               flowid;
   macaddr_t               flowda;
   uint32_t                responsecode;
   uint32_t                decision;
   uint32_t                bw_limit_info;
   uint32_t                maxburstsize;
   uint32_t                max_short_term_avg_ratio;
   uint32_t                maxpeakdatarate;
   uint32_t                flowtag;
   uint32_t                ingressnodeid;
   uint32_t                egressnodebitmask;
   uint32_t                tpacketsize;
   uint32_t                tleasetime;
   uint32_t                totalstps;
   uint32_t                totaltxps;
   uint32_t                flowstps;
   uint32_t                flowtxps;
   uint32_t                dest_flow_id;
   uint32_t                maximum_latency;
   uint32_t                short_term_avg_ratio;
   uint32_t                max_number_retry;
   uint32_t                flow_per;
   uint32_t                in_order_delivery;
   uint32_t                ingr_class_rule;
   uint32_t                traffic_protocol;
   uint32_t                vlan_tag;
   uint32_t                dscp_moca;
} __attribute__((packed,aligned(4)));

struct moca_pqos_update_response {
   macaddr_t               flowid;
   macaddr_t               flowda;
   uint32_t                responsecode;
   uint32_t                decision;
   uint32_t                bw_limit_info;
   uint32_t                maxburstsize;
   uint32_t                max_short_term_avg_ratio;
   uint32_t                maxpeakdatarate;
   uint32_t                flowtag;
   uint32_t                tpacketsize;
   uint32_t                tleasetime;
   uint32_t                totalstps;
   uint32_t                totaltxps;
   uint32_t                flowstps;
   uint32_t                flowtxps;
   uint32_t                maximum_latency;
   uint32_t                short_term_avg_ratio;
   uint32_t                max_number_retry;
   uint32_t                flow_per;
   uint32_t                in_order_delivery;
   uint32_t                ingr_class_rule;
   uint32_t                traffic_protocol;
   uint32_t                vlan_tag;
   uint32_t                dscp_moca;
} __attribute__((packed,aligned(4)));

struct moca_pqos_update_complete {
   macaddr_t               flowid;
   macaddr_t               flowda;
   uint32_t                responsecode;
   uint32_t                decision;
   uint32_t                bw_limit_info;
   uint32_t                maxburstsize;
   uint32_t                max_short_term_avg_ratio;
   uint32_t                maxpeakdatarate;
   uint32_t                flowtag;
   uint32_t                tpacketsize;
   uint32_t                tleasetime;
   uint32_t                totalstps;
   uint32_t                totaltxps;
   uint32_t                flowstps;
   uint32_t                flowtxps;
   uint32_t                maximum_latency;
   uint32_t                short_term_avg_ratio;
   uint32_t                max_number_retry;
   uint32_t                flow_per;
   uint32_t                in_order_delivery;
   uint32_t                ingr_class_rule;
   uint32_t                traffic_protocol;
   uint32_t                vlan_tag;
   uint32_t                dscp_moca;
} __attribute__((packed,aligned(4)));

struct moca_pqos_delete_response {
   macaddr_t               flowid;
   uint32_t                responsecode;
} __attribute__((packed,aligned(4)));

struct moca_pqos_delete_response_int {
   macaddr_t               flowid;
   uint16_t                reserved_0;
   uint32_t                responsecode;
} __attribute__((packed,aligned(4)));

struct moca_pqos_delete_complete {
   macaddr_t               flowid;
   uint32_t                responsecode;
} __attribute__((packed,aligned(4)));

struct moca_pqos_delete_complete_int {
   macaddr_t               flowid;
   uint16_t                reserved_0;
   uint32_t                responsecode;
} __attribute__((packed,aligned(4)));

struct moca_pqos_list_response {
   uint32_t                responsecode;
   uint32_t                ingressnodeid;
   uint32_t                flow_update_count;
   uint32_t                total_flow_id_count;
   macaddr_t               flowid[32];
} __attribute__((packed,aligned(4)));

struct moca_pqos_query_response {
   uint32_t                responsecode;
   uint32_t                leasetimeleft;
   uint8_t                 flowid[6];
   uint8_t                 reserved_0[2];
   uint16_t                tpacketsize;
   uint8_t                 reserved_1;
   uint8_t                 ingressnodeid;
   uint8_t                 max_latency;
   uint8_t                 short_term_avg_ratio;
   uint8_t                 reserved_2;
   uint8_t                 egressnodeid;
   uint32_t                flowtag;
   uint16_t                icr_vlan_dscp_iod_prot;
   uint8_t                 packetda[6];
   uint32_t                tpeakdatarate;
   uint32_t                tleasetime;
   uint8_t                 tburstsize;
   uint8_t                 dest_flow_id;
   uint8_t                 max_retry_flow_per;
   uint8_t                 reserved_3;
} __attribute__((packed,aligned(4)));

struct moca_pqos_delete_flow_out_int {
   macaddr_t               flowid;
   uint16_t                reserved_0;
   uint32_t                response_code;
} __attribute__((packed,aligned(4)));

struct moca_fmr_request {
   uint32_t                wave0Nodemask;
} __attribute__((packed,aligned(4)));

struct moca_fmr_request_int {
   uint8_t                 hdrFmt;
   uint8_t                 entryNodeId;
   uint8_t                 entryIndex;
   uint8_t                 reserved_0;
   uint16_t                vendorId;
   uint8_t                 transType;
   uint8_t                 transSubtype;
   uint32_t                wave0Nodemask;
   uint32_t                reserved_1;
   uint8_t                 reserved_2;
   uint8_t                 msgPriority;
   uint8_t                 txnLastWaveNum;
   uint8_t                 reserved_3;
   uint32_t                reserved_4;
} __attribute__((packed,aligned(4)));

struct moca_mr_request {
   uint32_t                wave0Nodemask;
   uint8_t                 resetTimer;
   uint16_t                nonDefSeqNum;
} __attribute__((packed,aligned(4)));

struct moca_mr_request_int {
   uint8_t                 hdrFmt;
   uint8_t                 entryNodeId;
   uint8_t                 entryIndex;
   uint8_t                 reserved_0;
   uint16_t                vendorId;
   uint8_t                 transType;
   uint8_t                 transSubtype;
   uint32_t                wave0Nodemask;
   uint32_t                reserved_1;
   uint8_t                 reserved_2;
   uint8_t                 msgPriority;
   uint8_t                 txnLastWaveNum;
   uint8_t                 reserved_3;
   uint8_t                 resetStatus;
   uint8_t                 resetTimer;
   uint16_t                nonDefSeqNum;
} __attribute__((packed,aligned(4)));

struct moca_dd_request {
   uint32_t                wave0Nodemask;
} __attribute__((packed,aligned(4)));

struct moca_dd_request_int {
   uint8_t                 hdrFmt;
   uint8_t                 entryNodeId;
   uint8_t                 entryIndex;
   uint8_t                 reserved_0;
   uint16_t                vendorId;
   uint8_t                 transType;
   uint8_t                 transSubtype;
   uint32_t                wave0Nodemask;
   uint32_t                reserved_1;
   uint8_t                 reserved_2;
   uint8_t                 msgPriority;
   uint8_t                 txnLastWaveNum;
   uint8_t                 reserved_3;
   uint32_t                reserved_4;
} __attribute__((packed,aligned(4)));

struct moca_fmr20_request {
   uint32_t                wave0Nodemask;
} __attribute__((packed,aligned(4)));

struct moca_fmr20_request_int {
   uint8_t                 hdrFmt;
   uint8_t                 entryNodeId;
   uint8_t                 entryIndex;
   uint8_t                 reserved_0;
   uint16_t                vendorId;
   uint8_t                 transType;
   uint8_t                 transSubtype;
   uint32_t                wave0Nodemask;
   uint32_t                reserved_1;
   uint8_t                 reserved_2;
   uint8_t                 msgPriority;
   uint8_t                 txnLastWaveNum;
   uint8_t                 reserved_3;
   uint32_t                reserved_4;
} __attribute__((packed,aligned(4)));

struct moca_start_aca {
   uint32_t                src_node;
   uint16_t                dest_nodemask;
   uint16_t                num_probes;
   uint32_t                type;
   uint32_t                channel;
} __attribute__((packed,aligned(4)));

struct moca_fmr_response {
   uint32_t                responsecode;
   uint32_t                responded_node_0;
   uint16_t                fmrinfo_node_0[16];
   uint32_t                responded_node_1;
   uint16_t                fmrinfo_node_1[16];
   uint32_t                responded_node_2;
   uint16_t                fmrinfo_node_2[16];
   uint32_t                responded_node_3;
   uint16_t                fmrinfo_node_3[16];
   uint32_t                responded_node_4;
   uint16_t                fmrinfo_node_4[16];
   uint32_t                responded_node_5;
   uint16_t                fmrinfo_node_5[16];
   uint32_t                responded_node_6;
   uint16_t                fmrinfo_node_6[16];
   uint32_t                responded_node_7;
   uint16_t                fmrinfo_node_7[16];
   uint32_t                responded_node_8;
   uint16_t                fmrinfo_node_8[16];
} __attribute__((packed,aligned(4)));

struct moca_mr_response {
   uint32_t                ResponseCode;
   uint32_t                ResetStatus;
   uint32_t                NonDefSeqNum;
   uint8_t                 n00ResetStatus;
   uint8_t                 n00RspCode;
   uint8_t                 n01ResetStatus;
   uint8_t                 n01RspCode;
   uint8_t                 n02ResetStatus;
   uint8_t                 n02RspCode;
   uint8_t                 n03ResetStatus;
   uint8_t                 n03RspCode;
   uint8_t                 n04ResetStatus;
   uint8_t                 n04RspCode;
   uint8_t                 n05ResetStatus;
   uint8_t                 n05RspCode;
   uint8_t                 n06ResetStatus;
   uint8_t                 n06RspCode;
   uint8_t                 n07ResetStatus;
   uint8_t                 n07RspCode;
   uint8_t                 n08ResetStatus;
   uint8_t                 n08RspCode;
   uint8_t                 n09ResetStatus;
   uint8_t                 n09RspCode;
   uint8_t                 n10ResetStatus;
   uint8_t                 n10RspCode;
   uint8_t                 n11ResetStatus;
   uint8_t                 n11RspCode;
   uint8_t                 n12ResetStatus;
   uint8_t                 n12RspCode;
   uint8_t                 n13ResetStatus;
   uint8_t                 n13RspCode;
   uint8_t                 n14ResetStatus;
   uint8_t                 n14RspCode;
   uint8_t                 n15ResetStatus;
   uint8_t                 n15RspCode;
} __attribute__((packed,aligned(4)));

struct moca_mr_complete {
   uint32_t                ResponseCode;
   uint32_t                ResetStatus;
   uint32_t                NonDefSeqNum;
   uint8_t                 n00ResetStatus;
   uint8_t                 n00RspCode;
   uint8_t                 n01ResetStatus;
   uint8_t                 n01RspCode;
   uint8_t                 n02ResetStatus;
   uint8_t                 n02RspCode;
   uint8_t                 n03ResetStatus;
   uint8_t                 n03RspCode;
   uint8_t                 n04ResetStatus;
   uint8_t                 n04RspCode;
   uint8_t                 n05ResetStatus;
   uint8_t                 n05RspCode;
   uint8_t                 n06ResetStatus;
   uint8_t                 n06RspCode;
   uint8_t                 n07ResetStatus;
   uint8_t                 n07RspCode;
   uint8_t                 n08ResetStatus;
   uint8_t                 n08RspCode;
   uint8_t                 n09ResetStatus;
   uint8_t                 n09RspCode;
   uint8_t                 n10ResetStatus;
   uint8_t                 n10RspCode;
   uint8_t                 n11ResetStatus;
   uint8_t                 n11RspCode;
   uint8_t                 n12ResetStatus;
   uint8_t                 n12RspCode;
   uint8_t                 n13ResetStatus;
   uint8_t                 n13RspCode;
   uint8_t                 n14ResetStatus;
   uint8_t                 n14RspCode;
   uint8_t                 n15ResetStatus;
   uint8_t                 n15RspCode;
} __attribute__((packed,aligned(4)));

struct moca_dd_response {
   uint32_t                responsecode;
   uint8_t                 num_nodes;
   uint8_t                 reserved_0[3];
   uint32_t                data[81];
} __attribute__((packed,aligned(4)));

struct moca_fmr20_response {
   uint32_t                responsecode;
   uint32_t                responded_node_0;
   uint8_t                 node0_data[132];
   uint32_t                responded_node_1;
   uint8_t                 node1_data[132];
} __attribute__((packed,aligned(4)));

struct moca_aca_result_1 {
   uint32_t                aca_status;
   uint32_t                aca_type;
   uint32_t                tx_status;
   uint32_t                rx_status;
   int32_t                 total_power;
   int32_t                 relative_power;
   uint32_t                num_elements;
   uint8_t                 power_profile[256];
} __attribute__((packed,aligned(4)));

struct moca_aca_result_2 {
   uint8_t                 power_profile[256];
} __attribute__((packed,aligned(4)));

struct moca_wom_magic_mac_int {
   macaddr_t               val;
   uint16_t                reserved_0;
} __attribute__((packed,aligned(4)));

struct moca_mmk_key_set {
   uint32_t                mmk_key_hi;
   uint32_t                mmk_key_lo;
} __attribute__((packed,aligned(4)));

struct moca_pmk_initial_key_set {
   uint32_t                pmk_initial_key_hi;
   uint32_t                pmk_initial_key_lo;
} __attribute__((packed,aligned(4)));

struct moca_network_password {
   char                    password[20];
} __attribute__((packed,aligned(4)));

struct moca_gmii_trap_header_int {
   uint8_t                 dest_mac[6];
   uint8_t                 source_mac[6];
   uint16_t                eth_type;
   uint8_t                 ver_len;
   uint8_t                 dscp_ecn;
   uint16_t                ip_length;
   uint16_t                id;
   uint16_t                flags_fragoffs;
   uint8_t                 ttl;
   uint8_t                 prot;
   uint16_t                ip_checksum;
   uint8_t                 src_ip_addr[4];
   uint8_t                 dst_ip_addr[4];
   uint16_t                src_port;
   uint16_t                dst_port;
   uint16_t                udp_length;
   uint16_t                udp_checksum;
} __attribute__((packed,aligned(4)));

struct moca_fw_printf {
   uint32_t                str_ptr;
   uint32_t                str_len;
} __attribute__((packed,aligned(4)));

struct moca_warning {
   uint32_t                string_id;
   uint32_t                num_params;
   uint32_t                msg_type;
} __attribute__((packed,aligned(4)));

struct moca_warning_lookup {
   uint32_t                string_id;
   uint32_t                num_params;
   uint32_t                msg_type;
} __attribute__((packed,aligned(4)));

struct moca_lab_cmd {
   uint32_t                index;
   uint32_t                params[32];
} __attribute__((packed,aligned(4)));

struct moca_lab_printf {
   uint8_t                 string[500];
} __attribute__((packed,aligned(4)));

struct moca_lab_printf_codes {
   uint32_t                string_id;
   uint32_t                num_params;
   uint32_t                msg_type;
} __attribute__((packed,aligned(4)));

struct moca_mips_exception {
   uint32_t                err_code;
   uint32_t                zero;
   uint32_t                cp0_epc;
   uint32_t                cp0_cause;
   uint32_t                cp0_status;
   uint32_t                cp0_errorepc;
} __attribute__((packed,aligned(4)));

struct moca_drv_printf {
   int8_t                  msg[104];
} __attribute__((packed,aligned(4)));

struct moca_lab_printf_codes_lookup {
   uint32_t                string_id;
   uint32_t                num_params;
   uint32_t                msg_type;
} __attribute__((packed,aligned(4)));

struct moca_cir_data {
   uint32_t                data[256];
} __attribute__((packed,aligned(4)));

struct moca_mps_data {
   uint32_t                valid;
   uint8_t                 public_key[32];
   uint8_t                 public_key_hash[16];
} __attribute__((packed,aligned(4)));

struct moca_mps_key {
   uint8_t                 key[16];
} __attribute__((packed,aligned(4)));

MOCALIB_GEN_SWAP_FUNCTION void moca_prof_pad_ctrl_deg_6802c0_single_swap(struct moca_prof_pad_ctrl_deg_6802c0_single * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_rs_swap(struct moca_snr_margin_table_rs * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ldpc_swap(struct moca_snr_margin_table_ldpc * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ldpc_sec_ch_swap(struct moca_snr_margin_table_ldpc_sec_ch * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ldpc_pre5_swap(struct moca_snr_margin_table_ldpc_pre5 * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ofdma_swap(struct moca_snr_margin_table_ofdma * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_rf_calib_data_swap(struct moca_rf_calib_data * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_ldpc_pri_ch_swap(struct moca_snr_margin_table_ldpc_pri_ch * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_pre5_pri_ch_swap(struct moca_snr_margin_table_pre5_pri_ch * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_snr_margin_table_pre5_sec_ch_swap(struct moca_snr_margin_table_pre5_sec_ch * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_probe_request_swap(struct moca_probe_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_egr_mc_addr_filter_set_int_swap(struct moca_egr_mc_addr_filter_set_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_request_int_swap(struct moca_pqos_create_request_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_request_swap(struct moca_pqos_create_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_ingr_add_flow_int_swap(struct moca_pqos_ingr_add_flow_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_ingr_add_flow_swap(struct moca_pqos_ingr_add_flow * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_request_int_swap(struct moca_pqos_update_request_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_request_swap(struct moca_pqos_update_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_ingr_update_int_swap(struct moca_pqos_ingr_update_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_ingr_update_swap(struct moca_pqos_ingr_update * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_request_int_swap(struct moca_pqos_delete_request_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_request_swap(struct moca_pqos_delete_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_list_request_int_swap(struct moca_pqos_list_request_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_list_request_swap(struct moca_pqos_list_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_query_request_int_swap(struct moca_pqos_query_request_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_query_request_swap(struct moca_pqos_query_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_mac_aging_int_swap(struct moca_mac_aging_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_response_swap(struct moca_pqos_create_response * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_create_complete_swap(struct moca_pqos_create_complete * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_response_swap(struct moca_pqos_update_response * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_update_complete_swap(struct moca_pqos_update_complete * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_response_int_swap(struct moca_pqos_delete_response_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_response_swap(struct moca_pqos_delete_response * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_complete_int_swap(struct moca_pqos_delete_complete_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_complete_swap(struct moca_pqos_delete_complete * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_list_response_swap(struct moca_pqos_list_response * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_query_response_swap(struct moca_pqos_query_response * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_pqos_delete_flow_out_int_swap(struct moca_pqos_delete_flow_out_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_fmr_request_int_swap(struct moca_fmr_request_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_fmr_request_swap(struct moca_fmr_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_mr_request_int_swap(struct moca_mr_request_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_mr_request_swap(struct moca_mr_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_dd_request_int_swap(struct moca_dd_request_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_dd_request_swap(struct moca_dd_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_fmr20_request_int_swap(struct moca_fmr20_request_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_fmr20_request_swap(struct moca_fmr20_request * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_start_aca_swap(struct moca_start_aca * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_fmr_response_swap(struct moca_fmr_response * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_mr_response_swap(struct moca_mr_response * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_mr_complete_swap(struct moca_mr_complete * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_dd_response_swap(struct moca_dd_response * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_fmr20_response_swap(struct moca_fmr20_response * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_aca_result_1_swap(struct moca_aca_result_1 * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_gmii_trap_header_int_swap(struct moca_gmii_trap_header_int * x);
MOCALIB_GEN_SWAP_FUNCTION void moca_mps_data_swap(struct moca_mps_data * x);

MOCALIB_GEN_SET_FUNCTION void moca_set_max_nbas_primary_defaults(uint32_t *max_nbas_primary, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_tx1_defaults(uint32_t *ps_swch_tx1, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_tx2_defaults(uint32_t *ps_swch_tx2, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_rx1_defaults(uint32_t *ps_swch_rx1, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_rx2_defaults(uint32_t *ps_swch_rx2, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_ps_swch_rx3_defaults(uint32_t *ps_swch_rx3, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_prof_pad_ctrl_deg_6802c0_bonding_defaults(uint32_t *prof_pad_ctrl_deg_6802c0_bonding, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_max_nbas_secondary_defaults(uint32_t *max_nbas_secondary, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_prof_pad_ctrl_deg_6802c0_single_defaults(struct moca_prof_pad_ctrl_deg_6802c0_single * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_cp_const_defaults(uint32_t *cp_const, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_preamble_uc_const_defaults(uint32_t *preamble_uc_const, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_cp_margin_increase_defaults(uint32_t *cp_margin_increase, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_ac_cc_shift_defaults(uint32_t *ac_cc_shift, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_mfc_th_increase_defaults(uint32_t *mfc_th_increase, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_agc_const_en_defaults(uint32_t *agc_const_en, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_agc_const_address_defaults(uint32_t *agc_const_address, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_min_bo_insert_2_bfm_lock_defaults(uint32_t *min_bo_insert_2_bfm_lock, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_min_snr_avg_db_2_bfm_lock_defaults(uint32_t *min_snr_avg_db_2_bfm_lock, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_rs_defaults(struct moca_snr_margin_table_rs * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ldpc_defaults(struct moca_snr_margin_table_ldpc * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ldpc_sec_ch_defaults(struct moca_snr_margin_table_ldpc_sec_ch * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ldpc_pre5_defaults(struct moca_snr_margin_table_ldpc_pre5 * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ofdma_defaults(struct moca_snr_margin_table_ofdma * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_ldpc_pri_ch_defaults(struct moca_snr_margin_table_ldpc_pri_ch * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_pre5_pri_ch_defaults(struct moca_snr_margin_table_pre5_pri_ch * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_snr_margin_table_pre5_sec_ch_defaults(struct moca_snr_margin_table_pre5_sec_ch * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_fragmentation_defaults(uint32_t *fragmentation, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_ifg_moca20_defaults(uint32_t *ifg_moca20, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_map_seed_defaults(uint32_t *map_seed, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_probe_request_defaults(struct moca_probe_request * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_low_pri_q_num_defaults(uint32_t *low_pri_q_num, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_priority_allocations_defaults(struct moca_priority_allocations * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_ooo_lmo_threshold_defaults(uint32_t *ooo_lmo_threshold, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_rf_switch_defaults(uint32_t *rf_switch, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_wom_mode_internal_defaults(uint32_t *wom_mode_internal, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_network_password_defaults(struct moca_network_password * in, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_const_rx_submode_defaults(uint32_t *const_rx_submode, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res1_defaults(uint32_t *res1, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res2_defaults(uint32_t *res2, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res3_defaults(uint32_t *res3, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res4_defaults(uint32_t *res4, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res5_defaults(uint32_t *res5, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res6_defaults(uint32_t *res6, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res7_defaults(uint32_t *res7, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res8_defaults(uint32_t *res8, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res9_defaults(uint32_t *res9, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_init1_defaults(uint32_t *init1, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_init2_defaults(uint32_t *init2, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_init3_defaults(uint32_t *init3, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_init4_defaults(uint32_t *init4, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_init5_defaults(uint32_t *init5, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_init6_defaults(uint32_t *init6, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_init7_defaults(uint32_t *init7, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_init8_defaults(uint32_t *init8, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_init9_defaults(uint32_t *init9, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_lab_snr_graph_set_defaults(uint32_t *lab_snr_graph_set, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_block_nack_rate_defaults(uint32_t *block_nack_rate, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res10_defaults(uint32_t *res10, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res11_defaults(uint32_t *res11, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res12_defaults(uint32_t *res12, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res13_defaults(uint32_t *res13, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res14_defaults(uint32_t *res14, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res15_defaults(uint32_t *res15, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res16_defaults(uint32_t *res16, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res17_defaults(uint32_t *res17, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res18_defaults(uint32_t *res18, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res19_defaults(uint32_t *res19, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_res20_defaults(uint32_t *res20, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_map_capture_defaults(uint32_t *map_capture, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_assert_restart_defaults(uint32_t *assert_restart, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_moca_cpu_freq_defaults(uint32_t *moca_cpu_freq, uint32_t flags);
MOCALIB_GEN_SET_FUNCTION void moca_set_mps_state_defaults(uint32_t *mps_state, uint32_t flags);

MOCALIB_GEN_RANGE_FUNCTION int __moca_prim_ch_offs_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_sec_ch_offs_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_non_def_seq_num_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_prof_pad_ctrl_deg_6802c0_bonding_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_prof_pad_ctrl_deg_6802c0_single_check(struct moca_prof_pad_ctrl_deg_6802c0_single * in, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_cp_const_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_preamble_uc_const_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_agc_const_en_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_rs_check(uint16_t * mgntable, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_sapm_table_fw_50_check(struct moca_sapm_table_fw_50 * in, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ldpc_check(uint16_t * mgntable, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ldpc_sec_ch_check(uint16_t * mgntable, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ldpc_pre5_check(uint16_t * mgntable, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ofdma_check(uint16_t * mgntable, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_sapm_table_fw_100_check(struct moca_sapm_table_fw_100 * in, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_cplength_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_sapm_table_fw_sec_check(struct moca_sapm_table_fw_sec * in, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_ldpc_pri_ch_check(uint16_t * mgntable, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_pre5_pri_ch_check(uint16_t * mgntable, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_snr_margin_table_pre5_sec_ch_check(uint16_t * mgntable, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_fragmentation_check(uint32_t bool_val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_probe_request_check(struct moca_probe_request * in, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_priority_allocations_check(struct moca_priority_allocations * in, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_start_aca_check(struct moca_start_aca * in, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_rf_switch_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_wom_mode_internal_check(uint32_t val, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_power_state_check(uint32_t state, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_const_rx_submode_check(uint32_t submode, uint32_t flags);
MOCALIB_GEN_RANGE_FUNCTION int __moca_map_capture_check(uint32_t val, uint32_t flags);

MOCALIB_GEN_GET_FUNCTION int __moca_get_max_nbas_primary(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_max_nbas_primary(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_tx1(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_tx1(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_tx2(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_tx2(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_rx1(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_rx1(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_rx2(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_rx2(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_ps_swch_rx3(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_ps_swch_rx3(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_prim_ch_offs(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_prim_ch_offs(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_sec_ch_offs(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_sec_ch_offs(void *vctx, uint32_t val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_non_def_seq_num(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_prof_pad_ctrl_deg_6802c0_bonding(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_prof_pad_ctrl_deg_6802c0_bonding(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_max_nbas_secondary(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_max_nbas_secondary(void *vctx, uint32_t val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_start_moca_core(void *vctx, uint32_t phy_freq);
MOCALIB_GEN_SET_FUNCTION int __moca_set_download_to_packet_ram_done(void *vctx);
MOCALIB_GEN_SET_FUNCTION int __moca_set_shutdown(void *vctx);
MOCALIB_GEN_GET_FUNCTION int __moca_get_prof_pad_ctrl_deg_6802c0_single(void *vctx, struct moca_prof_pad_ctrl_deg_6802c0_single *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_prof_pad_ctrl_deg_6802c0_single(void *vctx, struct moca_prof_pad_ctrl_deg_6802c0_single *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_config_flags(void *vctx, uint32_t *val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_cp_const(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_cp_const(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_preamble_uc_const(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_preamble_uc_const(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_cp_margin_increase(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_cp_margin_increase(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_ac_cc_shift(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_ac_cc_shift(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_mfc_th_increase(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_mfc_th_increase(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_agc_const_en(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_agc_const_en(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_agc_const_address(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_agc_const_address(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_min_bo_insert_2_bfm_lock(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_min_bo_insert_2_bfm_lock(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_min_snr_avg_db_2_bfm_lock(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_min_snr_avg_db_2_bfm_lock(void *vctx, uint32_t val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_sig_y_done(void *vctx, const struct moca_sig_y_done *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_rs(void *vctx, struct moca_snr_margin_table_rs *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_rs(void *vctx, struct moca_snr_margin_table_rs *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_sapm_table_fw_50(void *vctx, struct moca_sapm_table_fw_50 *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_sapm_table_fw_50(void *vctx, struct moca_sapm_table_fw_50 *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ldpc(void *vctx, struct moca_snr_margin_table_ldpc *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ldpc(void *vctx, struct moca_snr_margin_table_ldpc *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ldpc_sec_ch(void *vctx, struct moca_snr_margin_table_ldpc_sec_ch *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ldpc_sec_ch(void *vctx, struct moca_snr_margin_table_ldpc_sec_ch *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ldpc_pre5(void *vctx, struct moca_snr_margin_table_ldpc_pre5 *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ldpc_pre5(void *vctx, struct moca_snr_margin_table_ldpc_pre5 *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ofdma(void *vctx, struct moca_snr_margin_table_ofdma *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ofdma(void *vctx, struct moca_snr_margin_table_ofdma *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_sapm_table_fw_100(void *vctx, struct moca_sapm_table_fw_100 *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_sapm_table_fw_100(void *vctx, struct moca_sapm_table_fw_100 *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_cplength(void *vctx, uint32_t val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_rf_calib_data(void *vctx, struct moca_rf_calib_data *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_probe_2_results(void *vctx, const struct moca_probe_2_results *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_sapm_table_fw_sec(void *vctx, struct moca_sapm_table_fw_sec *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_sapm_table_fw_sec(void *vctx, struct moca_sapm_table_fw_sec *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_rx_gain_agc_table(void *vctx, uint32_t * in, uint32_t *out, int max_out_len);
MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_ldpc_pri_ch(void *vctx, struct moca_snr_margin_table_ldpc_pri_ch *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_ldpc_pri_ch(void *vctx, struct moca_snr_margin_table_ldpc_pri_ch *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_pre5_pri_ch(void *vctx, struct moca_snr_margin_table_pre5_pri_ch *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_pre5_pri_ch(void *vctx, struct moca_snr_margin_table_pre5_pri_ch *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_snr_margin_table_pre5_sec_ch(void *vctx, struct moca_snr_margin_table_pre5_sec_ch *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_snr_margin_table_pre5_sec_ch(void *vctx, struct moca_snr_margin_table_pre5_sec_ch *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_fragmentation(void *vctx, uint32_t *bool_val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_fragmentation(void *vctx, uint32_t bool_val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_ifg_moca20(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_ifg_moca20(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_map_seed(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_map_seed(void *vctx, uint32_t val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_probe_request(void *vctx, struct moca_probe_request *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_low_pri_q_num(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_low_pri_q_num(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_priority_allocations(void *vctx, struct moca_priority_allocations *out);
MOCALIB_GEN_SET_FUNCTION int __moca_set_priority_allocations(void *vctx, const struct moca_priority_allocations *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_create_request(void *vctx, struct moca_pqos_create_request *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_ingr_add_flow(void *vctx, struct moca_pqos_ingr_add_flow *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_update_request(void *vctx, struct moca_pqos_update_request *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_ingr_update(void *vctx, struct moca_pqos_ingr_update *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_delete_request(void *vctx, struct moca_pqos_delete_request *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_ingr_delete(void *vctx, struct moca_pqos_ingr_delete *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_list_request(void *vctx, struct moca_pqos_list_request *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_pqos_query_request(void *vctx, struct moca_pqos_query_request *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_eport_link_state(void *vctx, uint32_t state);
MOCALIB_GEN_GET_FUNCTION int __moca_get_ooo_lmo_threshold(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_ooo_lmo_threshold(void *vctx, uint32_t val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_fmr_request(void *vctx, struct moca_fmr_request *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_mr_request(void *vctx, struct moca_mr_request *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_dd_request(void *vctx, struct moca_dd_request *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_fmr20_request(void *vctx, struct moca_fmr20_request *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_start_aca(void *vctx, struct moca_start_aca *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_rf_switch(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_rf_switch(void *vctx, uint32_t val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_reset_core_stats(void *vctx);
MOCALIB_GEN_SET_FUNCTION int __moca_set_wom_mode_internal(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_wom_mode_internal(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_power_state(void *vctx, uint32_t state);
MOCALIB_GEN_SET_FUNCTION int __moca_set_mmk_key(void *vctx, const struct moca_mmk_key_set *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_pmk_initial_key(void *vctx, const struct moca_pmk_initial_key_set *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_network_password(void *vctx, struct moca_network_password *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_network_password(void *vctx, struct moca_network_password *out);
MOCALIB_GEN_GET_FUNCTION int __moca_get_const_rx_submode(void *vctx, uint32_t *submode);
MOCALIB_GEN_SET_FUNCTION int __moca_set_const_rx_submode(void *vctx, uint32_t submode);
MOCALIB_GEN_GET_FUNCTION int __moca_get_mmp_version(void *vctx, uint32_t *val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res1(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res1(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res2(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res2(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res3(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res3(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res4(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res4(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res5(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res5(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res6(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res6(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res7(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res7(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res8(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res8(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res9(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res9(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_init1(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_init1(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_init2(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_init2(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_init3(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_init3(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_init4(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_init4(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_init5(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_init5(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_init6(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_init6(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_init7(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_init7(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_init8(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_init8(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_init9(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_init9(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_lab_snr_graph_set(void *vctx, uint32_t *node_id);
MOCALIB_GEN_SET_FUNCTION int __moca_set_lab_snr_graph_set(void *vctx, uint32_t node_id);
MOCALIB_GEN_GET_FUNCTION int __moca_get_block_nack_rate(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_block_nack_rate(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res10(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res10(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res11(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res11(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res12(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res12(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res13(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res13(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res14(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res14(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res15(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res15(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res16(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res16(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res17(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res17(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res18(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res18(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res19(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res19(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_res20(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_res20(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_map_capture(void *vctx, uint32_t *val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_map_capture(void *vctx, uint32_t val);
MOCALIB_GEN_GET_FUNCTION int __moca_get_lab_cmd(void *vctx, uint32_t index, uint32_t *retval);
MOCALIB_GEN_SET_FUNCTION int __moca_set_lab_cmd(void *vctx, const struct moca_lab_cmd *in);
MOCALIB_GEN_GET_FUNCTION int __moca_get_cir_data(void *vctx, uint32_t node, struct moca_cir_data *out);
MOCALIB_GEN_GET_FUNCTION int __moca_get_assert_restart(void *vctx, uint32_t *enable);
MOCALIB_GEN_SET_FUNCTION int __moca_set_assert_restart(void *vctx, uint32_t enable);
MOCALIB_GEN_GET_FUNCTION int __moca_get_moca_cpu_freq(void *vctx, uint32_t *hz);
MOCALIB_GEN_SET_FUNCTION int __moca_set_moca_cpu_freq(void *vctx, uint32_t hz);
MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_state(void *vctx, uint32_t val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_trig(void *vctx, uint32_t val);
MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_data(void *vctx, struct moca_mps_data *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_key(void *vctx, struct moca_mps_key *in);
MOCALIB_GEN_SET_FUNCTION int __moca_set_mps_data_ready(void *vctx);


typedef union NV_PARAM {
  uint32_t ac_cc_shift;
  uint32_t adc_mode;
  uint32_t aes_exchange_interval;
  uint32_t agc_const_address;
  uint32_t agc_const_en;
  uint32_t aif_mode;
  uint32_t amp_type;
  int32_t arpl_th_100;
  int32_t arpl_th_50;
  uint32_t assertText;
  uint32_t assert_restart;
  uint32_t bad_probe_prints;
  uint32_t bandwidth;
  uint32_t beacon_pwr_reduction;
  uint32_t beacon_pwr_reduction_en;
  uint32_t block_nack_rate;
  uint32_t bo_mode;
  uint32_t bonding;
  uint32_t brcmtag_enable;
  uint32_t c4_moca20_en;
  uint32_t cap_phy_rate_en;
  uint32_t cap_snr_base_margin;
  uint32_t cap_target_phy_rate;
  uint32_t cir_prints;
  uint32_t const_rx_submode;
  struct moca_const_tx_params const_tx_params;
  int32_t continuous_rx_mode_attn;
  uint32_t cp_const;
  uint32_t cp_margin_increase;
  uint32_t dont_start_moca;
  uint32_t egr_mc_filter_en;
  uint32_t en_capable;
  uint32_t extra_rx_packets_per_qm;
  uint32_t fc_mode;
  uint32_t fragmentation;
  uint32_t freq_shift;
  uint32_t host_qos;
  uint32_t if_access_en;
  struct moca_if_access_table if_access_table;
  uint32_t ifg_moca20;
  uint32_t impedance_mode_bonding;
  uint32_t init1;
  uint32_t init2;
  uint32_t init3;
  uint32_t init4;
  uint32_t init5;
  uint32_t init6;
  uint32_t init7;
  uint32_t init8;
  uint32_t init9;
  uint32_t lab_mode;
  uint32_t lab_snr_graph_set;
  uint32_t led_mode;
  uint32_t limit_traffic;
  uint32_t listening_duration;
  uint32_t listening_freq_mask;
  uint32_t lof_update;
  uint32_t loopback_en;
  uint32_t low_pri_q_num;
  uint32_t m1_tx_power_variation;
  struct moca_mac_aging mac_aging;
  uint32_t map_seed;
  struct moca_max_constellation max_constellation;
  uint32_t max_constellation_all;
  uint32_t max_frame_size;
  uint32_t max_nbas_primary;
  uint32_t max_nbas_secondary;
  uint32_t max_phy_rate;
  uint32_t max_phy_rate_50M;
  uint32_t max_phy_rate_turbo;
  uint32_t max_pkt_aggr;
  uint32_t max_pkt_aggr_bonding;
  uint32_t max_transmit_time;
  int32_t max_tx_power;
  struct moca_max_tx_power_tune max_tx_power_tune;
  struct moca_max_tx_power_tune_sec_ch max_tx_power_tune_sec_ch;
  uint32_t mcfilter_enable;
  uint32_t mfc_th_increase;
  uint32_t min_bo_insert_2_bfm_lock;
  uint32_t min_bw_alarm_threshold;
  uint32_t min_snr_avg_db_2_bfm_lock;
  uint32_t moca_core_trace_enable;
  uint32_t moca_cpu_freq;
  uint32_t mps_en;
  uint32_t mps_privacy_down;
  uint32_t mps_privacy_receive;
  uint32_t mps_unpaired_time;
  uint32_t mps_walk_time;
  uint32_t mtm_en;
  uint32_t multicast_mode;
  uint32_t nc_heartbeat_interval;
  uint32_t nc_listening_interval;
  uint32_t nc_mode;
  uint32_t nv_cal_enable;
  uint32_t ofdma_en;
  uint32_t ooo_lmo_threshold;
  uint32_t orr_en;
  uint32_t otf_en;
  struct moca_password password;
  uint32_t pause_fc_en;
  uint32_t per_mode;
  uint32_t phy_clock;
  uint32_t pm_restore_on_link_down;
  uint32_t pmk_exchange_interval;
  uint32_t policing_en;
  uint32_t power_save_mechanism_dis;
  uint32_t pqos_max_packet_size;
  uint32_t preamble_uc_const;
  uint32_t preferred_nc;
  int32_t primary_ch_offset;
  struct moca_priority_allocations priority_allocations;
  uint32_t privacy_defaults;
  uint32_t privacy_en;
  uint32_t prof_pad_ctrl_deg_6802c0_bonding;
  struct moca_prof_pad_ctrl_deg_6802c0_single prof_pad_ctrl_deg_6802c0_single;
  uint32_t prop_bonding_compatibility_mode;
  uint32_t ps_swch_rx1;
  uint32_t ps_swch_rx2;
  uint32_t ps_swch_rx3;
  uint32_t ps_swch_tx1;
  uint32_t ps_swch_tx2;
  uint32_t psm_config;
  uint32_t qam256_capability;
  uint32_t rdeg_3450;
  uint32_t remote_man;
  uint32_t res1;
  uint32_t res10;
  uint32_t res11;
  uint32_t res12;
  uint32_t res13;
  uint32_t res14;
  uint32_t res15;
  uint32_t res16;
  uint32_t res17;
  uint32_t res18;
  uint32_t res19;
  uint32_t res2;
  uint32_t res20;
  uint32_t res3;
  uint32_t res4;
  uint32_t res5;
  uint32_t res6;
  uint32_t res7;
  uint32_t res8;
  uint32_t res9;
  uint32_t rework_6802;
  uint32_t rf_band;
  uint32_t rf_switch;
  uint32_t rlapm_cap_100;
  uint32_t rlapm_cap_50;
  uint32_t rlapm_en;
  struct moca_rlapm_table_100 rlapm_table_100;
  struct moca_rlapm_table_50 rlapm_table_50;
  struct moca_rtr_config rtr_config;
  struct moca_rx_power_tune rx_power_tune;
  uint32_t rx_tx_packets_per_qm;
  uint32_t sapm_en;
  struct moca_sapm_table_100 sapm_table_100;
  struct moca_sapm_table_50 sapm_table_50;
  struct moca_sapm_table_sec sapm_table_sec;
  int32_t secondary_ch_offset;
  uint32_t selective_rr;
  uint32_t sigma2_prints;
  uint32_t single_channel_operation;
  struct moca_snr_margin_ldpc snr_margin_ldpc;
  struct moca_snr_margin_ldpc_pre5 snr_margin_ldpc_pre5;
  struct moca_snr_margin_ldpc_pri_ch snr_margin_ldpc_pri_ch;
  struct moca_snr_margin_ldpc_sec_ch snr_margin_ldpc_sec_ch;
  struct moca_snr_margin_ofdma snr_margin_ofdma;
  struct moca_snr_margin_pre5_pri_ch snr_margin_pre5_pri_ch;
  struct moca_snr_margin_pre5_sec_ch snr_margin_pre5_sec_ch;
  struct moca_snr_margin_rs snr_margin_rs;
  uint32_t snr_prints;
  struct moca_stag_priority stag_priority;
  struct moca_stag_removal stag_removal;
  uint32_t standby_power_state;
  uint32_t star_topology_en;
  struct moca_start_ulmo start_ulmo;
  struct moca_taboo_channels taboo_channels;
  uint32_t target_phy_rate_20;
  uint32_t target_phy_rate_20_sec_ch;
  uint32_t target_phy_rate_20_turbo;
  uint32_t target_phy_rate_20_turbo_sec_ch;
  uint32_t target_phy_rate_20_turbo_vlper;
  uint32_t target_phy_rate_20_turbo_vlper_sec_ch;
  uint32_t target_phy_rate_qam128;
  uint32_t target_phy_rate_qam256;
  uint32_t tek_exchange_interval;
  uint32_t tlp_mode;
  uint32_t tpc_en;
  uint32_t turbo_en;
  uint32_t use_ext_data_mem;
  uint32_t wdog_enable;
  uint32_t wom_magic_enable;
  uint32_t wom_mode;
} MOCA_NV_PARAM;


struct mocalib_cb_tbl {
   void         (*core_ready_cb)(void *, struct moca_core_ready *);
   void         *core_ready_userarg;

   void         (*power_up_status_cb)(void *, uint32_t status);
   void         *power_up_status_userarg;

   void         (*new_lof_cb)(void *, uint32_t lof);
   void         *new_lof_userarg;

   void         (*admission_completed_cb)(void *, uint32_t lof);
   void         *admission_completed_userarg;

   void         (*tpcap_done_cb)(void *);
   void         *tpcap_done_userarg;

   void         (*mocad_forwarding_rx_packet_cb)(void *, struct moca_mocad_forwarding_rx_packet *);
   void         *mocad_forwarding_rx_packet_userarg;

   void         (*mocad_forwarding_tx_ack_cb)(void *, uint32_t offset);
   void         *mocad_forwarding_tx_ack_userarg;

   void         (*pr_degradation_cb)(void *);
   void         *pr_degradation_userarg;

   void         (*ucfwd_update_cb)(void *);
   void         *ucfwd_update_userarg;

   void         (*pqos_maintenance_complete_cb)(void *, struct moca_pqos_maintenance_complete *);
   void         *pqos_maintenance_complete_userarg;

   void         (*pqos_create_flow_cb)(void *, struct moca_pqos_create_flow_out *);
   void         *pqos_create_flow_userarg;

   void         (*pqos_update_flow_cb)(void *, struct moca_pqos_update_flow_out *);
   void         *pqos_update_flow_userarg;

   void         (*pqos_delete_flow_cb)(void *, struct moca_pqos_delete_flow_out *);
   void         *pqos_delete_flow_userarg;

   void         (*pqos_list_cb)(void *, struct moca_pqos_list_out *);
   void         *pqos_list_userarg;

   void         (*pqos_query_cb)(void *, struct moca_pqos_query_out *);
   void         *pqos_query_userarg;

   void         (*pqos_status_cb)(void *, struct moca_pqos_status_out *);
   void         *pqos_status_userarg;

   void         (*admission_status_cb)(void *, uint32_t status);
   void         *admission_status_userarg;

   void         (*limited_bw_cb)(void *, uint32_t bw_status);
   void         *limited_bw_userarg;

   void         (*lmo_info_cb)(void *, struct moca_lmo_info *);
   void         *lmo_info_userarg;

   void         (*topology_changed_cb)(void *, uint32_t nodemask);
   void         *topology_changed_userarg;

   void         (*moca_version_changed_cb)(void *, uint32_t new_version);
   void         *moca_version_changed_userarg;

   void         (*moca_reset_request_cb)(void *, struct moca_moca_reset_request *);
   void         *moca_reset_request_userarg;

   void         (*nc_id_changed_cb)(void *, uint32_t new_nc_id);
   void         *nc_id_changed_userarg;

   void         (*mr_event_cb)(void *, uint32_t status);
   void         *mr_event_userarg;

   void         (*aca_cb)(void *, struct moca_aca_out *);
   void         *aca_userarg;

   void         (*fmr_init_cb)(void *, struct moca_fmr_init_out *);
   void         *fmr_init_userarg;

   void         (*moca_reset_cb)(void *, struct moca_moca_reset_out *);
   void         *moca_reset_userarg;

   void         (*dd_init_cb)(void *, struct moca_dd_init_out *);
   void         *dd_init_userarg;

   void         (*fmr_20_cb)(void *, struct moca_fmr_20_out *);
   void         *fmr_20_userarg;

   void         (*hostless_mode_cb)(void *, uint32_t status);
   void         *hostless_mode_userarg;

   void         (*wakeup_node_cb)(void *, uint32_t status);
   void         *wakeup_node_userarg;

   void         (*link_up_state_cb)(void *, uint32_t status);
   void         *link_up_state_userarg;

   void         (*new_rf_band_cb)(void *, uint32_t rf_band);
   void         *new_rf_band_userarg;

   void         (*power_state_rsp_cb)(void *, uint32_t rsp_code);
   void         *power_state_rsp_userarg;

   void         (*power_state_event_cb)(void *, uint32_t event_code);
   void         *power_state_event_userarg;

   void         (*power_state_cap_cb)(void *, uint32_t power_modes);
   void         *power_state_cap_userarg;

   void         (*ps_cmd_cb)(void *, uint32_t rsp_code);
   void         *ps_cmd_userarg;

   void         (*key_changed_cb)(void *, struct moca_key_changed *);
   void         *key_changed_userarg;

   void         (*error_cb)(void *, struct moca_error *);
   void         *error_userarg;

   void         (*error_lookup_cb)(void *, struct moca_error_lookup *);
   void         *error_lookup_userarg;

   void         (*mocad_printf_cb)(void *, struct moca_mocad_printf_out *);
   void         *mocad_printf_userarg;

   void         (*assert_cb)(void *, struct moca_assert *);
   void         *assert_userarg;

   void         (*mps_privacy_changed_cb)(void *);
   void         *mps_privacy_changed_userarg;

   void         (*mps_trigger_cb)(void *);
   void         *mps_trigger_userarg;

   void         (*mps_pair_fail_cb)(void *);
   void         *mps_pair_fail_userarg;

   void         (*init_scan_rec_cb)(void *);
   void         *init_scan_rec_userarg;

   void         (*mps_request_mpskey_cb)(void *, struct moca_mps_request_mpskey *);
   void         *mps_request_mpskey_userarg;

   void         (*mps_admission_nochange_cb)(void *);
   void         *mps_admission_nochange_userarg;

};

/* GENERATED DECLARATIONS ABOVE THIS LINE - DO NOT EDIT */

struct moca_ctx
{
        uint32_t                useMocadHook; // This must be the first field of the structure
        uint32_t                valid;
	MUTEX_T			lock;

	MoCAOS_ClientHandle	connect_fd;
        MoCAOS_ClientHandle	mocad_rx_fd;

	uint8_t			buf[MOCA_BIG_BUF_LEN];
	uint8_t			evt_buf[MOCA_BIG_BUF_LEN];

	struct mocalib_cb_tbl	cb;
	char			ifname[MoCAOS_IFNAMSIZE];
};

char *moca_get_buf(void *vctx, int *size);
char *moca_get_evtbuf(void *vctx, int *size);
int moca_wait_for_raw_event(void *vctx, uint32_t timeout_s);
void moca_close_evt(void *vctx);
int moca_req(struct moca_ctx *ctx, int wr_len);

#ifdef __cplusplus
}
#endif

#endif /* ! _MOCAINT_H_ */
