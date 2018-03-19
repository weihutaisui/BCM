/*
 * This module does provides various mappings to or from the CLM rate indexes.
 *
 * Copyright 2017 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 */
/*FILE-CSTYLED*/

#include <stdio.h>
#include <typedefs.h>
#include <assert.h>
#include "wlu_rates_matrix.h"

#define CLM_NO_RATE_STRING "NO_RATE"
#define EXP_MODE_UNKNOWN   "Unknown expansion mode"
#define AUTO_RATESPEC 0x0
#define MHZ_TO_HALF_MHZ 2

typedef enum exp_mode {
	EXP_MODE_ID_DEF = 0,
	EXP_MODE_ID_STBC,
	EXP_MODE_ID_TXBF,
	EXP_MODE_ID_MAX
} exp_mode_t;

static reg_rate_index_t get_legacy_reg_rate_index(int rate_hmhz, int tx_expansion, exp_mode_t mode);
static reg_rate_index_t get_ht_reg_rate_index(int mcs, int tx_expansion, exp_mode_t mode);
static reg_rate_index_t get_vht_reg_rate_index(int mcs, int nss, int tx_expansion, exp_mode_t mode);
static int get_legacy_rate_identifier(int rate_hmhz);
static int get_legacy_mode_identifier(int tx_expansion, exp_mode_t mode);
static int get_vht_rate_identifier(int vht_mcs_index);
static int get_vht_ss1_mode_identifier(int tx_expansion, exp_mode_t mode);
static int get_vht_ss2_mode_identifier(int tx_expansion, exp_mode_t mode);
static int get_vht_ss3_mode_identifier(int tx_expansion, exp_mode_t mode);
static int get_vht_ss4_mode_identifier(int tx_expansion, exp_mode_t mode);
static const char *get_mode_name(exp_mode_t mode);

enum {
	LEGACY_RATE_ID_NO_RATE = -1,
	LEGACY_RATE_ID_1MHZ = 0,
	LEGACY_RATE_ID_2MHZ,
	LEGACY_RATE_ID_5_5MHZ,
	LEGACY_RATE_ID_11MHZ,
	LEGACY_RATE_ID_6MHZ,
	LEGACY_RATE_ID_9MHZ,
	LEGACY_RATE_ID_12MHZ,
	LEGACY_RATE_ID_18MHZ,
	LEGACY_RATE_ID_24MHZ,
	LEGACY_RATE_ID_36MHZ,
	LEGACY_RATE_ID_48MHZ,
	LEGACY_RATE_ID_54MHZ,
	LEGACY_RATE_ID_MAX
};

enum {
	LEGACY_MODE_ID_NONE = 0,
	LEGACY_MODE_ID_TXEXP1,
	LEGACY_MODE_ID_TXEXP2,
	LEGACY_MODE_ID_TXEXP3,
	LEGACY_MODE_ID_TXBF1,
	LEGACY_MODE_ID_TXBF2,
	LEGACY_MODE_ID_TXBF3,
	LEGACY_MODE_ID_MAX
};

enum {
	VHT_RATE_INDEX_0 = 0,
	VHT_RATE_INDEX_1,
	VHT_RATE_INDEX_2,
	VHT_RATE_INDEX_3,
	VHT_RATE_INDEX_4,
	VHT_RATE_INDEX_5,
	VHT_RATE_INDEX_6,
	VHT_RATE_INDEX_7,
	VHT_RATE_INDEX_8,
	VHT_RATE_INDEX_9,
	VHT_RATE_INDEX_10,
	VHT_RATE_INDEX_11,
	VHT_RATE_INDEX_MAX
};

enum {
	VHT_SS1_MODE_ID_NONE = 0,
	VHT_SS1_MODE_ID_CDD1,
	VHT_SS1_MODE_ID_STBC,
	VHT_SS1_MODE_ID_CDD2,
	VHT_SS1_MODE_ID_STBC_SPEXP1,
	VHT_SS1_MODE_ID_CDD3,
	VHT_SS1_MODE_ID_STBC_SPEXP2,
	VHT_SS1_MODE_ID_TXBF1,
	VHT_SS1_MODE_ID_TXBF2,
	VHT_SS1_MODE_ID_TXBF3,
	VHT_SS1_MODE_ID_MAX
};

enum {
	VHT_SS2_MODE_ID_NONE = 0,
	VHT_SS2_MODE_ID_SPEXP1,
	VHT_SS2_MODE_ID_SPEXP2,
	VHT_SS2_MODE_ID_TXBF0,
	VHT_SS2_MODE_ID_TXBF1,
	VHT_SS2_MODE_ID_TXBF2,
	VHT_SS2_MODE_ID_MAX
};

enum {
	VHT_SS3_MODE_ID_NONE = 0,
	VHT_SS3_MODE_ID_SPEXP1,
	VHT_SS3_MODE_ID_TXBF0,
	VHT_SS3_MODE_ID_TXBF1,
	VHT_SS3_MODE_ID_MAX
};

enum {
	VHT_SS4_MODE_ID_NONE = 0,
	VHT_SS4_MODE_ID_TXBF0,
	VHT_SS4_MODE_ID_MAX
};

const char *clm_rate_group_labels[] = {
	"DSSS",
	"OFDM",
	"MCS0_7",
	"VHT8_9SS1",
	"VHT10_11SS1",
	"DSSS_MULTI1",
	"OFDM_CDD1",
	"MCS0_7_CDD1",
	"VHT8_9SS1_CDD1",
	"VHT10_11SS1_CDD1",
	"MCS0_7_STBC",
	"VHT8_9SS1_STBC",
	"VHTi10_11SS1_STBC",
	"MCS8_15",
	"VHT8_9SS2",
	"VHT10_11SS2",
	"DSSS_MULTI2",
	"OFDM_CDD2",
	"MCS0_7_CDD2",
	"VHT8_9SS1_CDD2",
	"VHT10_11SS1_CDD2",
	"MCS0_7_STBC_SPEXP1",
	"VHT8_9SS1_STBC_SPEXP1",
	"VHT10_11SS1_STBC_SPEXP1",
	"MCS8_15_SPEXP1",
	"VHT8_9SS2_SPEXP1",
	"VHT10_11SS2_SPEXP1",
	"MCS16_23",
	"VHT8_9SS3",
	"VHT10_11SS3",
	"DSSS_MULTI3",
	"OFDM_CDD3",
	"MCS0_7_CDD3",
	"VHT8_9SS1_CDD3",
	"VHT10_11SS1_CDD3",
	"MCS0_7_STBC_SPEXP2",
	"VHT8_9SS1_STBC_SPEXP2",
	"VHT10_11SS1_STBC_SPEXP2",
	"MCS8_15_SPEXP2",
	"VHT8_9SS2_SPEXP2",
	"VHT10_11SS2_SPEXP2",
	"MCS16_23_SPEXP1",
	"VHT8_9SS3_SPEXP1",
	"VHT10_11SS3_SPEXP1",
	"MCS24_31",
	"VHT8_9SS4",
	"VHT10_11SS4",
	"OFDM_TXBF1",
	"MCS0_7_TXBF1",
	"VHT8_9SS1_TXBF1",
	"VHT10_11SS1_TXBF1",
	"MCS8_15_TXBF0",
	"",
	"",
	"OFDM_TXBF2",
	"MCS0_7_TXBF2",
	"VHT8_9SS1_TXBF2",
	"VHT10_11SS1_TXBF2",
	"MCS8_15_TXBF1",
	"VHT8_9SS2_TXBF1",
	"VHT10_11SS2_TXBF1",
	"MCS16_23_TXBF0",
	"",
	"",
	"OFDM_TXBF3",
	"MCS0_7_TXBF3",
	"VHT8_9SS1_TXBF3",
	"VHT10_11SS1_TXBF3",
	"MCS8_15_TXBF2",
	"VHT8_9SS2_TXBF2",
	"VHT10_11SS2_TXBF2",
	"MCS16_23_TXBF1",
	"VHT8_9SS3_TXBF1",
	"VHT10_11SS3_TXBF1",
	"MCS24_31_TXBF0",
	"",
	"",
};

const char *clm_rate_labels[] = {
	"DSSS1",
	"DSSS2",
	"DSSS5",
	"DSSS11",
	"OFDM6",
	"OFDM9",
	"OFDM12",
	"OFDM18",
	"OFDM24",
	"OFDM36",
	"OFDM48",
	"OFDM54",
	"MCS0",
	"MCS1",
	"MCS2",
	"MCS3",
	"MCS4",
	"MCS5",
	"MCS6",
	"MCS7",
	"VHT8SS1",
	"VHT9SS1",
	"VHT10SS1",
	"VHT11SS1",
	"DSSS1_MULTI1",
	"DSSS2_MULTI1",
	"DSSS5_MULTI1",
	"DSSS11_MULTI1",
	"OFDM6_CDD1",
	"OFDM9_CDD1",
	"OFDM12_CDD1",
	"OFDM18_CDD1",
	"OFDM24_CDD1",
	"OFDM36_CDD1",
	"OFDM48_CDD1",
	"OFDM54_CDD1",
	"MCS0_CDD1",
	"MCS1_CDD1",
	"MCS2_CDD1",
	"MCS3_CDD1",
	"MCS4_CDD1",
	"MCS5_CDD1",
	"MCS6_CDD1",
	"MCS7_CDD1",
	"VHT8SS1_CDD1",
	"VHT9SS1_CDD1",
	"VHT10SS1_CDD1",
	"VHT11SS1_CDD1",
	"MCS0_STBC",
	"MCS1_STBC",
	"MCS2_STBC",
	"MCS3_STBC",
	"MCS4_STBC",
	"MCS5_STBC",
	"MCS6_STBC",
	"MCS7_STBC",
	"VHT8SS1_STBC",
	"VHT9SS1_STBC",
	"VHT10SS1_STBC",
	"VHT11SS1_STBC",
	"MCS8",
	"MCS9",
	"MCS10",
	"MCS11",
	"MCS12",
	"MCS13",
	"MCS14",
	"MCS15",
	"VHT8SS2",
	"VHT9SS2",
	"VHT10SS2",
	"VHT11SS2",
	"DSSS1_MULTI2",
	"DSSS2_MULTI2",
	"DSSS5_MULTI2",
	"DSSS11_MULTI2",
	"OFDM6_CDD2",
	"OFDM9_CDD2",
	"OFDM12_CDD2",
	"OFDM18_CDD2",
	"OFDM24_CDD2",
	"OFDM36_CDD2",
	"OFDM48_CDD2",
	"OFDM54_CDD2",
	"MCS0_CDD2",
	"MCS1_CDD2",
	"MCS2_CDD2",
	"MCS3_CDD2",
	"MCS4_CDD2",
	"MCS5_CDD2",
	"MCS6_CDD2",
	"MCS7_CDD2",
	"VHT8SS1_CDD2",
	"VHT9SS1_CDD2",
	"VHT10SS1_CDD2",
	"VHT11SS1_CDD2",
	"MCS0_STBC_SPEXP1",
	"MCS1_STBC_SPEXP1",
	"MCS2_STBC_SPEXP1",
	"MCS3_STBC_SPEXP1",
	"MCS4_STBC_SPEXP1",
	"MCS5_STBC_SPEXP1",
	"MCS6_STBC_SPEXP1",
	"MCS7_STBC_SPEXP1",
	"VHT8SS1_STBC_SPEXP1",
	"VHT9SS1_STBC_SPEXP1",
	"VHT10SS1_STBC_SPEXP1",
	"VHT11SS1_STBC_SPEXP1",
	"MCS8_SPEXP1",
	"MCS9_SPEXP1",
	"MCS10_SPEXP1",
	"MCS11_SPEXP1",
	"MCS12_SPEXP1",
	"MCS13_SPEXP1",
	"MCS14_SPEXP1",
	"MCS15_SPEXP1",
	"VHT8SS2_SPEXP1",
	"VHT9SS2_SPEXP1",
	"VHT10SS2_SPEXP1",
	"VHT11SS2_SPEXP1",
	"MCS16",
	"MCS17",
	"MCS18",
	"MCS19",
	"MCS20",
	"MCS21",
	"MCS22",
	"MCS23",
	"VHT8SS3",
	"VHT9SS3",
	"VHT10SS3",
	"VHT11SS3",
	"DSSS1_MULTI3",
	"DSSS2_MULTI3",
	"DSSS5_MULTI3",
	"DSSS11_MULTI3",
	"OFDM6_CDD3",
	"OFDM9_CDD3",
	"OFDM12_CDD3",
	"OFDM18_CDD3",
	"OFDM24_CDD3",
	"OFDM36_CDD3",
	"OFDM48_CDD3",
	"OFDM54_CDD3",
	"MCS0_CDD3",
	"MCS1_CDD3",
	"MCS2_CDD3",
	"MCS3_CDD3",
	"MCS4_CDD3",
	"MCS5_CDD3",
	"MCS6_CDD3",
	"MCS7_CDD3",
	"VHT8SS1_CDD3",
	"VHT9SS1_CDD3",
	"VHT10SS1_CDD3",
	"VHT11SS1_CDD3",
	"MCS0_STBC_SPEXP2",
	"MCS1_STBC_SPEXP2",
	"MCS2_STBC_SPEXP2",
	"MCS3_STBC_SPEXP2",
	"MCS4_STBC_SPEXP2",
	"MCS5_STBC_SPEXP2",
	"MCS6_STBC_SPEXP2",
	"MCS7_STBC_SPEXP2",
	"VHT8SS1_STBC_SPEXP2",
	"VHT9SS1_STBC_SPEXP2",
	"VHT10SS1_STBC_SPEXP2",
	"VHT11SS1_STBC_SPEXP2",
	"MCS8_SPEXP2",
	"MCS9_SPEXP2",
	"MCS10_SPEXP2",
	"MCS11_SPEXP2",
	"MCS12_SPEXP2",
	"MCS13_SPEXP2",
	"MCS14_SPEXP2",
	"MCS15_SPEXP2",
	"VHT8SS2_SPEXP2",
	"VHT9SS2_SPEXP2",
	"VHT10SS2_SPEXP2",
	"VHT11SS2_SPEXP2",
	"MCS16_SPEXP1",
	"MCS17_SPEXP1",
	"MCS18_SPEXP1",
	"MCS19_SPEXP1",
	"MCS20_SPEXP1",
	"MCS21_SPEXP1",
	"MCS22_SPEXP1",
	"MCS23_SPEXP1",
	"VHT8SS3_SPEXP1",
	"VHT9SS3_SPEXP1",
	"VHT10SS3_SPEXP1",
	"VHT11SS3_SPEXP1",
	"MCS24",
	"MCS25",
	"MCS26",
	"MCS27",
	"MCS28",
	"MCS29",
	"MCS30",
	"MCS31",
	"VHT8SS4",
	"VHT9SS4",
	"VHT10SS4",
	"VHT11SS4",
	"OFDM6_TXBF1",
	"OFDM9_TXBF1",
	"OFDM12_TXBF1",
	"OFDM18_TXBF1",
	"OFDM24_TXBF1",
	"OFDM36_TXBF1",
	"OFDM48_TXBF1",
	"OFDM54_TXBF1",
	"MCS0_TXBF1",
	"MCS1_TXBF1",
	"MCS2_TXBF1",
	"MCS3_TXBF1",
	"MCS4_TXBF1",
	"MCS5_TXBF1",
	"MCS6_TXBF1",
	"MCS7_TXBF1",
	"VHT8SS1_TXBF1",
	"VHT9SS1_TXBF1",
	"VHT10SS1_TXBF1",
	"VHT11SS1_TXBF1",
	"MCS8_TXBF0",
	"MCS9_TXBF0",
	"MCS10_TXBF0",
	"MCS11_TXBF0",
	"MCS12_TXBF0",
	"MCS13_TXBF0",
	"MCS14_TXBF0",
	"MCS15_TXBF0",
	"VHT8SS2_TXBF0",
	"VHT9SS2_TXBF0",
	"VHT10SS2_TXBF0",
	"VHT11SS2_TXBF0",
	"OFDM6_TXBF2",
	"OFDM9_TXBF2",
	"OFDM12_TXBF2",
	"OFDM18_TXBF2",
	"OFDM24_TXBF2",
	"OFDM36_TXBF2",
	"OFDM48_TXBF2",
	"OFDM54_TXBF2",
	"MCS0_TXBF2",
	"MCS1_TXBF2",
	"MCS2_TXBF2",
	"MCS3_TXBF2",
	"MCS4_TXBF2",
	"MCS5_TXBF2",
	"MCS6_TXBF2",
	"MCS7_TXBF2",
	"VHT8SS1_TXBF2",
	"VHT9SS1_TXBF2",
	"VHT10SS1_TXBF2",
	"VHT11SS1_TXBF2",
	"MCS8_TXBF1",
	"MCS9_TXBF1",
	"MCS10_TXBF1",
	"MCS11_TXBF1",
	"MCS12_TXBF1",
	"MCS13_TXBF1",
	"MCS14_TXBF1",
	"MCS15_TXBF1",
	"VHT8SS2_TXBF1",
	"VHT9SS2_TXBF1",
	"VHT10SS2_TXBF1",
	"VHT11SS2_TXBF1",
	"MCS16_TXBF0",
	"MCS17_TXBF0",
	"MCS18_TXBF0",
	"MCS19_TXBF0",
	"MCS20_TXBF0",
	"MCS21_TXBF0",
	"MCS22_TXBF0",
	"MCS23_TXBF0",
	"VHT8SS3_TXBF0",
	"VHT9SS3_TXBF0",
	"VHT10SS3_TXBF0",
	"VHT11SS3_TXBF0",
	"OFDM6_TXBF3",
	"OFDM9_TXBF3",
	"OFDM12_TXBF3",
	"OFDM18_TXBF3",
	"OFDM24_TXBF3",
	"OFDM36_TXBF3",
	"OFDM48_TXBF3",
	"OFDM54_TXBF3",
	"MCS0_TXBF3",
	"MCS1_TXBF3",
	"MCS2_TXBF3",
	"MCS3_TXBF3",
	"MCS4_TXBF3",
	"MCS5_TXBF3",
	"MCS6_TXBF3",
	"MCS7_TXBF3",
	"VHT8SS1_TXBF3",
	"VHT9SS1_TXBF3",
	"VHT10SS1_TXBF3",
	"VHT11SS1_TXBF3",
	"MCS8_TXBF2",
	"MCS9_TXBF2",
	"MCS10_TXBF2",
	"MCS11_TXBF2",
	"MCS12_TXBF2",
	"MCS13_TXBF2",
	"MCS14_TXBF2",
	"MCS15_TXBF2",
	"VHT8SS2_TXBF2",
	"VHT9SS2_TXBF2",
	"VHT10SS2_TXBF2",
	"VHT11SS2_TXBF2",
	"MCS16_TXBF1",
	"MCS17_TXBF1",
	"MCS18_TXBF1",
	"MCS19_TXBF1",
	"MCS20_TXBF1",
	"MCS21_TXBF1",
	"MCS22_TXBF1",
	"MCS23_TXBF1",
	"VHT8SS3_TXBF1",
	"VHT9SS3_TXBF1",
	"VHT10SS3_TXBF1",
	"VHT11SS3_TXBF1",
	"MCS24_TXBF0",
	"MCS25_TXBF0",
	"MCS26_TXBF0",
	"MCS27_TXBF0",
	"MCS28_TXBF0",
	"MCS29_TXBF0",
	"MCS30_TXBF0",
	"MCS31_TXBF0",
	"VHT8SS4_TXBF0",
	"VHT9SS4_TXBF0",
	"VHT10SS4_TXBF0",
	"VHT11SS4_TXBF0",
};

int legacy_reg_rate_map[LEGACY_RATE_ID_MAX][LEGACY_MODE_ID_MAX] = {
	{DSSS1,  DSSS1_MULTI1,  DSSS1_MULTI2, DSSS1_MULTI3,  NO_RATE,       NO_RATE,      NO_RATE},
	{DSSS2,  DSSS2_MULTI1,  DSSS2_MULTI2, DSSS2_MULTI3,  NO_RATE,       NO_RATE,      NO_RATE},
	{DSSS5,  DSSS5_MULTI1,  DSSS5_MULTI2, DSSS5_MULTI3,  NO_RATE,       NO_RATE,      NO_RATE},
	{DSSS11, DSSS11_MULTI1, DSSS11_MULTI2,DSSS11_MULTI3, NO_RATE,       NO_RATE,      NO_RATE},
	{OFDM6,  OFDM6_CDD1,    OFDM6_CDD2,   OFDM6_CDD3,    OFDM6_TXBF1,   OFDM6_TXBF2,  OFDM6_TXBF3},
	{OFDM9,  OFDM9_CDD1,    OFDM9_CDD2,   OFDM9_CDD3,    OFDM9_TXBF1,   OFDM9_TXBF2,  OFDM9_TXBF3},
	{OFDM12, OFDM12_CDD1,   OFDM12_CDD2,  OFDM12_CDD3,   OFDM12_TXBF1,  OFDM12_TXBF2, OFDM12_TXBF3},
	{OFDM18, OFDM18_CDD1,   OFDM18_CDD2,  OFDM18_CDD3,   OFDM18_TXBF1,  OFDM18_TXBF2, OFDM18_TXBF3},
	{OFDM24, OFDM24_CDD1,   OFDM24_CDD2,  OFDM24_CDD3,   OFDM24_TXBF1,  OFDM24_TXBF2, OFDM24_TXBF3},
	{OFDM36, OFDM36_CDD1,   OFDM36_CDD2,  OFDM36_CDD3,   OFDM36_TXBF1,  OFDM36_TXBF2, OFDM36_TXBF3},
	{OFDM48, OFDM48_CDD1,   OFDM48_CDD2,  OFDM48_CDD3,   OFDM48_TXBF1,  OFDM48_TXBF2, OFDM48_TXBF3},
	{OFDM54, OFDM54_CDD1,   OFDM54_CDD2,  OFDM54_CDD3,   OFDM54_TXBF1,  OFDM54_TXBF2, OFDM54_TXBF3},
};

int vht_ss1_reg_rate_map[VHT_RATE_INDEX_MAX][VHT_SS1_MODE_ID_MAX] = {
	{MCS0,    MCS0_CDD1,    MCS0_STBC,    MCS0_CDD2,    MCS0_STBC_SPEXP1,  MCS0_CDD3,  MCS0_STBC_SPEXP2,  MCS0_TXBF1,  MCS0_TXBF2, MCS0_TXBF3},
	{MCS1,    MCS1_CDD1,    MCS1_STBC,    MCS1_CDD2,    MCS1_STBC_SPEXP1,  MCS1_CDD3,  MCS1_STBC_SPEXP2,  MCS1_TXBF1,  MCS1_TXBF2, MCS1_TXBF3},
	{MCS2,    MCS2_CDD1,    MCS2_STBC,    MCS2_CDD2,    MCS2_STBC_SPEXP1,  MCS2_CDD3,  MCS2_STBC_SPEXP2,  MCS2_TXBF1,  MCS2_TXBF2, MCS2_TXBF3},
	{MCS3,    MCS3_CDD1,    MCS3_STBC,    MCS3_CDD2,    MCS3_STBC_SPEXP1,  MCS3_CDD3,  MCS3_STBC_SPEXP2,  MCS3_TXBF1,  MCS3_TXBF2, MCS3_TXBF3},
	{MCS4,    MCS4_CDD1,    MCS4_STBC,    MCS4_CDD2,    MCS4_STBC_SPEXP1,  MCS4_CDD3,  MCS4_STBC_SPEXP2,  MCS4_TXBF1,  MCS4_TXBF2, MCS4_TXBF3},
	{MCS5,    MCS5_CDD1,    MCS5_STBC,    MCS5_CDD2,    MCS5_STBC_SPEXP1,  MCS5_CDD3,  MCS5_STBC_SPEXP2,  MCS5_TXBF1,  MCS5_TXBF2, MCS5_TXBF3},
	{MCS6,    MCS6_CDD1,    MCS6_STBC,    MCS6_CDD2,    MCS6_STBC_SPEXP1,  MCS6_CDD3,  MCS6_STBC_SPEXP2,  MCS6_TXBF1,  MCS6_TXBF2, MCS6_TXBF3},
	{MCS7,    MCS7_CDD1,    MCS7_STBC,    MCS7_CDD2,    MCS7_STBC_SPEXP1,  MCS7_CDD3,  MCS7_STBC_SPEXP2,  MCS7_TXBF1,  MCS7_TXBF2, MCS7_TXBF3},
	{VHT8SS1, VHT8SS1_CDD1, VHT8SS1_STBC, VHT8SS1_CDD2, VHT8SS1_STBC_SPEXP1, VHT8SS1_CDD3, VHT8SS1_STBC_SPEXP2, VHT8SS1_TXBF1, VHT8SS1_TXBF2, VHT8SS1_TXBF3},
	{VHT9SS1, VHT9SS1_CDD1, VHT9SS1_STBC, VHT9SS1_CDD2, VHT9SS1_STBC_SPEXP1, VHT9SS1_CDD3, VHT9SS1_STBC_SPEXP2, VHT9SS1_TXBF1, VHT9SS1_TXBF2, VHT9SS1_TXBF3},
	{VHT10SS1, VHT10SS1_CDD1, VHT10SS1_STBC, VHT10SS1_CDD2, VHT10SS1_STBC_SPEXP1, VHT10SS1_CDD3, VHT10SS1_STBC_SPEXP2, VHT10SS1_TXBF1, VHT10SS1_TXBF2, VHT10SS1_TXBF3},
	{VHT11SS1, VHT11SS1_CDD1, VHT11SS1_STBC, VHT11SS1_CDD2, VHT11SS1_STBC_SPEXP1, VHT11SS1_CDD3, VHT11SS1_STBC_SPEXP2, VHT11SS1_TXBF1, VHT11SS1_TXBF2, VHT11SS1_TXBF3},
};

int vht_ss2_reg_rate_map[VHT_RATE_INDEX_MAX][VHT_SS2_MODE_ID_MAX] = {
	{MCS8,    MCS8_SPEXP1,   MCS8_SPEXP2,  MCS8_TXBF0,   MCS8_TXBF1, MCS8_TXBF2},
	{MCS9,    MCS9_SPEXP1,   MCS9_SPEXP2,  MCS9_TXBF0,   MCS9_TXBF1, MCS9_TXBF2},
	{MCS10,   MCS10_SPEXP1,  MCS10_SPEXP2, MCS10_TXBF0, MCS10_TXBF1, MCS10_TXBF2},
	{MCS11,   MCS11_SPEXP1,  MCS11_SPEXP2, MCS11_TXBF0, MCS11_TXBF1, MCS11_TXBF2},
	{MCS12,   MCS12_SPEXP1,  MCS12_SPEXP2, MCS12_TXBF0, MCS12_TXBF1, MCS12_TXBF2},
	{MCS13,   MCS13_SPEXP1,  MCS13_SPEXP2, MCS13_TXBF0, MCS13_TXBF1, MCS13_TXBF2},
	{MCS14,   MCS14_SPEXP1,  MCS14_SPEXP2, MCS14_TXBF0, MCS14_TXBF1, MCS14_TXBF2},
	{MCS15,   MCS15_SPEXP1,  MCS15_SPEXP2, MCS15_TXBF0, MCS15_TXBF1, MCS15_TXBF2},
	{VHT8SS2, VHT8SS2_SPEXP1,VHT8SS2_SPEXP2,VHT8SS2_TXBF0, VHT8SS2_TXBF1, VHT8SS2_TXBF2},
	{VHT9SS2, VHT9SS2_SPEXP1,VHT9SS2_SPEXP2,VHT9SS2_TXBF0, VHT9SS2_TXBF1, VHT9SS2_TXBF2},
	{VHT10SS2, VHT10SS2_SPEXP1,VHT10SS2_SPEXP2,VHT10SS2_TXBF0, VHT10SS2_TXBF1, VHT10SS2_TXBF2},
	{VHT11SS2, VHT11SS2_SPEXP1,VHT11SS2_SPEXP2,VHT11SS2_TXBF0, VHT11SS2_TXBF1, VHT11SS2_TXBF2},
};

int vht_ss3_reg_rate_map[VHT_RATE_INDEX_MAX][VHT_SS3_MODE_ID_MAX] = {
	{MCS16,      MCS16_SPEXP1, MCS16_TXBF0, MCS16_TXBF1},
	{MCS17,      MCS17_SPEXP1, MCS17_TXBF0, MCS17_TXBF1},
	{MCS18,      MCS18_SPEXP1, MCS18_TXBF0, MCS18_TXBF1},
	{MCS19,      MCS19_SPEXP1, MCS19_TXBF0, MCS19_TXBF1},
	{MCS20,      MCS20_SPEXP1, MCS20_TXBF0, MCS20_TXBF1},
	{MCS21,      MCS21_SPEXP1, MCS21_TXBF0, MCS21_TXBF1},
	{MCS22,      MCS22_SPEXP1, MCS22_TXBF0, MCS22_TXBF1},
	{MCS23,      MCS23_SPEXP1, MCS23_TXBF0, MCS23_TXBF1},
	{VHT8SS3,    VHT8SS3_SPEXP1, VHT8SS3_TXBF0, VHT8SS3_TXBF1},
	{VHT9SS3,    VHT9SS3_SPEXP1, VHT9SS3_TXBF0, VHT9SS3_TXBF1},
	{VHT10SS3,   VHT10SS3_SPEXP1, VHT10SS3_TXBF0, VHT10SS3_TXBF1},
	{VHT11SS3,   VHT11SS3_SPEXP1, VHT11SS3_TXBF0, VHT11SS3_TXBF1},
};

int vht_ss4_reg_rate_map[VHT_RATE_INDEX_MAX][VHT_SS4_MODE_ID_MAX] = {
	{MCS24,      MCS24_TXBF0},
	{MCS25,      MCS25_TXBF0},
	{MCS26,      MCS26_TXBF0},
	{MCS27,      MCS27_TXBF0},
	{MCS28,      MCS28_TXBF0},
	{MCS29,      MCS29_TXBF0},
	{MCS30,      MCS30_TXBF0},
	{MCS31,      MCS31_TXBF0},
	{VHT8SS4,    VHT8SS4_TXBF0},
	{VHT9SS4,    VHT9SS4_TXBF0},
	{VHT10SS4,   VHT10SS4_TXBF0},
	{VHT11SS4,   VHT11SS4_TXBF0},
};

const char *exp_mode_name[EXP_MODE_ID_MAX] = {
	"CDD",
	"STBC",
	"TXBF"
};

const char *
get_clm_rate_group_label(int rategroup)
{
	return clm_rate_group_labels[rategroup];
}

const char *
get_reg_rate_string_from_ratespec(int ratespec)
{
	reg_rate_index_t index = get_reg_rate_index_from_ratespec(ratespec);

	if (index >= 0)
	{
		return clm_rate_labels[index];
	}
	return CLM_NO_RATE_STRING;
}

reg_rate_index_t
get_reg_rate_index_from_ratespec(int ratespec)
{
	uint encode, rate, txexp;
	bool stbc,txbf;
	int rate_index = NO_RATE;
	exp_mode_t expmode = EXP_MODE_ID_DEF;

	/* If auto is set, we don't get a ratespec that can be decoded */
	if (ratespec == AUTO_RATESPEC)
		return rate_index;

	encode = (ratespec & WL_RSPEC_ENCODING_MASK);
	rate   = (ratespec & WL_RSPEC_RATE_MASK);
	txexp  = (ratespec & WL_RSPEC_TXEXP_MASK) >> WL_RSPEC_TXEXP_SHIFT;
	stbc   = (ratespec & WL_RSPEC_STBC) != 0;
	txbf   = (ratespec & WL_RSPEC_TXBF) != 0;

	if (stbc && txbf) {
		return rate_index;
	} else if (txbf) {
		expmode = EXP_MODE_ID_TXBF;
	} else if (stbc) {
		expmode = EXP_MODE_ID_STBC;
	}

	if (encode == WL_RSPEC_ENCODE_RATE) {
		rate_index = get_legacy_reg_rate_index(rate, txexp, expmode);
	} else if (encode == WL_RSPEC_ENCODE_HT) {
		rate_index = get_ht_reg_rate_index(rate, txexp, expmode);
	} else if (encode == WL_RSPEC_ENCODE_VHT) {
		uint mcs = (ratespec & WL_RSPEC_VHT_MCS_MASK);
		uint nss = (ratespec & WL_RSPEC_VHT_NSS_MASK) >> WL_RSPEC_VHT_NSS_SHIFT;
		rate_index = get_vht_reg_rate_index(mcs, nss, txexp, expmode);
	}

	return rate_index;
}

reg_rate_index_t
get_legacy_reg_rate_index(int rate_hmhz, int tx_expansion, exp_mode_t expmode)
{
	reg_rate_index_t index = NO_RATE;
	int rate_id;
	rate_id = get_legacy_rate_identifier(rate_hmhz);
	if (rate_id == LEGACY_RATE_ID_NO_RATE || (expmode == EXP_MODE_ID_TXBF && rate_id < LEGACY_RATE_ID_6MHZ))
	{
		fprintf(stderr, "ERROR: Bad legacy rate spec: %d\n", rate_hmhz);
	}
	else
	{
		index = legacy_reg_rate_map[rate_id][get_legacy_mode_identifier(tx_expansion, expmode)];
	}
	return index;
}

static int get_legacy_rate_identifier(int rate_hmhz)
{
	int rate_lut[LEGACY_RATE_ID_MAX];
	int rate_index = LEGACY_RATE_ID_NO_RATE;
	int i;

	rate_lut[LEGACY_RATE_ID_1MHZ]   = 1 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_2MHZ]   = 2 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_5_5MHZ] = 11; /* 5.5 * MHZ_TO_HALF_MHZ */
	rate_lut[LEGACY_RATE_ID_11MHZ]  = 11 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_6MHZ]   = 6 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_9MHZ]   = 9 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_12MHZ]  = 12 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_18MHZ]  = 18 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_24MHZ]  = 24 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_36MHZ]  = 36 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_48MHZ]  = 48 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_54MHZ]  = 54 * MHZ_TO_HALF_MHZ;

	for (i = 0; i < LEGACY_RATE_ID_MAX; i++)
	{
		if (rate_lut[i] == rate_hmhz)
		{
			rate_index = i;
			break;
		}
	}

	return rate_index;
}

static int get_legacy_mode_identifier(int tx_expansion, exp_mode_t expmode)
{
	int mode_identifier = 0;

	if (tx_expansion == 0)
	{
		mode_identifier = LEGACY_MODE_ID_NONE;
	}
	else if (tx_expansion == 1)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = LEGACY_MODE_ID_TXEXP1;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = LEGACY_MODE_ID_TXBF1;
			break;
		default:
			fprintf(stderr, "ERROR: Bad legacy tx_expansion spec: %d expansion mode %s\n",
				 tx_expansion, get_mode_name(expmode));
			break;
		}
	}
	else if (tx_expansion == 2)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = LEGACY_MODE_ID_TXEXP2;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = LEGACY_MODE_ID_TXBF2;
			break;
		default:
			fprintf(stderr, "ERROR: Bad legacy tx_expansion spec: %d expansion mode %s\n",
				 tx_expansion, get_mode_name(expmode));
			break;
		}
	}
	else if (tx_expansion == 3)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = LEGACY_MODE_ID_TXEXP3;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = LEGACY_MODE_ID_TXBF3;
			break;
		default:
			fprintf(stderr, "ERROR: Bad legacy tx_expansion spec: %d expansion mode %s\n",
				 tx_expansion, get_mode_name(expmode));
			break;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Bad legacy tx_expansion spec: %d expansion mode %s\n", tx_expansion,
				get_mode_name(expmode));
	}

	return mode_identifier;
}

reg_rate_index_t
get_ht_reg_rate_index(int mcs, int tx_expansion, exp_mode_t expmode)
{
	reg_rate_index_t rate_index = NO_RATE;
	int vht_mcs, nss;

	if (mcs > WLC_MAXMCS)
	{
		fprintf(stderr, "ERROR: Bad ht mcs spec: %d\n", mcs);
	}
	else
	{
		if (IS_PROPRIETARY_11N_MCS(mcs)) {
			switch (mcs) {
			case 87:
				nss = 1;
				vht_mcs = VHT_RATE_INDEX_8;
				break;
			case 99:
				nss = 2;
				vht_mcs = VHT_RATE_INDEX_8;
				break;
			case 101:
				nss = 3;
				vht_mcs = VHT_RATE_INDEX_8;
				break;
			case 88:
				nss = 1;
				vht_mcs = VHT_RATE_INDEX_9;
				break;
			case 100:
				nss = 2;
				vht_mcs = VHT_RATE_INDEX_9;
				break;
			case 102:
				nss = 3;
				vht_mcs = VHT_RATE_INDEX_9;
				break;
			default:
				return rate_index;
			}
		} else {
			vht_mcs = mcs % 8;
			nss = (mcs / 8) + 1;
		}
		rate_index = get_vht_reg_rate_index(vht_mcs, nss, tx_expansion, expmode);
	}

	return rate_index;
}

reg_rate_index_t
get_vht_reg_rate_index(int mcs, int nss, int tx_expansion, exp_mode_t expmode)
{
	reg_rate_index_t rate_index = NO_RATE;
	int rate_id = get_vht_rate_identifier(mcs);

	if (nss == 1)
	{
		rate_index = vht_ss1_reg_rate_map[rate_id][get_vht_ss1_mode_identifier(tx_expansion, expmode)];
	}
	else if (nss == 2)
	{
		rate_index = vht_ss2_reg_rate_map[rate_id][get_vht_ss2_mode_identifier(tx_expansion, expmode)];
	}
	else if (nss == 3)
	{
		rate_index = vht_ss3_reg_rate_map[rate_id][get_vht_ss3_mode_identifier(tx_expansion, expmode)];
	}
	else if (nss == 4)
	{
		rate_index = vht_ss4_reg_rate_map[rate_id][get_vht_ss4_mode_identifier(tx_expansion, expmode)];
	}

	return rate_index;
}

static int get_vht_rate_identifier(int vht_mcs_index)
{
	int rate_index = 0;

	if (vht_mcs_index >= VHT_RATE_INDEX_0 && vht_mcs_index < VHT_RATE_INDEX_MAX)
	{
		rate_index = vht_mcs_index;
	}
	else
	{
		fprintf(stderr, "ERROR: Bad vht mcs spec: %d\n", vht_mcs_index);
	}
	return rate_index;
}

static int get_vht_ss1_mode_identifier(int tx_expansion, exp_mode_t expmode)
{
	int mode_identifier = 0;

	if (tx_expansion == 0)
	{
		switch (expmode)
		{
			case EXP_MODE_ID_DEF:
				mode_identifier = VHT_SS1_MODE_ID_NONE;
				break;
			case EXP_MODE_ID_STBC:
				mode_identifier = VHT_SS1_MODE_ID_STBC;
				break;
			default:
				fprintf(stderr, "ERROR: Bad vht ss1 mode: %d, expansion mode %s\n", tx_expansion,
						get_mode_name(expmode));
				break;
		}
	}
	else if (tx_expansion == 1)
	{
		switch (expmode)
		{
			case EXP_MODE_ID_DEF:
				mode_identifier = VHT_SS1_MODE_ID_CDD1;
				break;
			case EXP_MODE_ID_STBC:
				mode_identifier = VHT_SS1_MODE_ID_STBC_SPEXP1;
				break;
			case EXP_MODE_ID_TXBF:
				mode_identifier = VHT_SS1_MODE_ID_TXBF1;
				break;
			default:
				fprintf(stderr, "ERROR: Bad vht ss1 mode: %d, expansion mode %s\n", tx_expansion,
						get_mode_name(expmode));
				break;
		}
	}
	else if (tx_expansion == 2)
	{
		switch (expmode)
		{
			case EXP_MODE_ID_DEF:
				mode_identifier = VHT_SS1_MODE_ID_CDD2;
				break;
			case EXP_MODE_ID_STBC:
				mode_identifier = VHT_SS1_MODE_ID_STBC_SPEXP2;
				break;
			case EXP_MODE_ID_TXBF:
				mode_identifier = VHT_SS1_MODE_ID_TXBF2;
				break;
			default:
				fprintf(stderr, "ERROR: Bad vht ss1 mode: %d, expansion mode %s\n", tx_expansion,
						get_mode_name(expmode));
				break;
		}
	}
	else if (tx_expansion == 3)
	{
		switch (expmode)
		{
			case EXP_MODE_ID_DEF:
				mode_identifier = VHT_SS1_MODE_ID_CDD3;
				break;
			case EXP_MODE_ID_TXBF:
				mode_identifier = VHT_SS1_MODE_ID_TXBF3;
				break;
			default:
				fprintf(stderr, "ERROR: Bad vht ss1 mode: %d, expansion mode %s\n", tx_expansion,
						get_mode_name(expmode));
				break;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Bad vht ss1 mode: %d, expansion mode: %s\n", tx_expansion,
				 get_mode_name(expmode));
	}

	return mode_identifier;
}

static int get_vht_ss2_mode_identifier(int tx_expansion, exp_mode_t expmode)
{
	int mode_identifier = 0;

	if (tx_expansion == 0)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = VHT_SS2_MODE_ID_NONE;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = VHT_SS2_MODE_ID_TXBF0;
			break;
		default:
			fprintf(stderr, "ERROR: Bad vht ss2 mode: %d, expansion mode %s\n", tx_expansion,
				get_mode_name(expmode));
			break;
		}
	}
	else if (tx_expansion == 1)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = VHT_SS2_MODE_ID_SPEXP1;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = VHT_SS2_MODE_ID_TXBF1;
			break;
		default:
			fprintf(stderr, "ERROR: Bad vht ss2 mode: %d, expansion mode %s\n", tx_expansion,
				  get_mode_name(expmode));
			break;
		}
	}
	else if (tx_expansion == 2)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = VHT_SS2_MODE_ID_SPEXP2;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = VHT_SS2_MODE_ID_TXBF2;
			break;
		default:
			fprintf(stderr, "ERROR: Bad vht ss2 mode: %d, expansion mode %s\n", tx_expansion,
				  get_mode_name(expmode));
			break;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Bad vht ss2 mode: %d expansion mode: %s\n", tx_expansion,
			   get_mode_name(expmode));
	}

	return mode_identifier;
}

static int get_vht_ss3_mode_identifier(int tx_expansion, exp_mode_t expmode)
{
	int mode_identifier = 0;

	if (tx_expansion == 0)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = VHT_SS3_MODE_ID_NONE;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = VHT_SS3_MODE_ID_TXBF0;
			break;
		default:
			fprintf(stderr, "ERROR: Bad vht ss3 mode: %d, expansion mode: %s\n", tx_expansion
					, get_mode_name(expmode));
			break;
		}
	}
	else if (tx_expansion == 1)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = VHT_SS3_MODE_ID_SPEXP1;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = VHT_SS3_MODE_ID_TXBF1;
			break;
		default:
			fprintf(stderr, "ERROR: Bad vht ss3 mode: %d, expansion mode: %s\n", tx_expansion
					, get_mode_name(expmode));
			break;
		}

	}
	else
	{
		fprintf(stderr, "ERROR: Bad vht ss3 mode: %d, expansion: %s\n", tx_expansion,
			   get_mode_name(expmode));
	}

	return mode_identifier;
}

static int get_vht_ss4_mode_identifier(int tx_expansion, exp_mode_t expmode)
{
	int mode_identifier = 0;

	if (tx_expansion == 0)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = VHT_SS4_MODE_ID_NONE;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = VHT_SS4_MODE_ID_TXBF0;
			break;
		default:
			fprintf(stderr, "ERROR: Bad vht ss4 mode: %d, expansion mode: %s\n", tx_expansion
					, get_mode_name(expmode));
			break;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Bad vht ss4 mode: %d, expansion: %s\n", tx_expansion,
			   get_mode_name(expmode));
	}

	return mode_identifier;
}

static const char *get_mode_name(exp_mode_t mode)
{
	if (mode >= EXP_MODE_ID_MAX)
		return EXP_MODE_UNKNOWN;

	return exp_mode_name[mode];
}
