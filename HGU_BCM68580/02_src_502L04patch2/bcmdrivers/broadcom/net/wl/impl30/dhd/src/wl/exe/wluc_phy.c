/*
 * wl phy command module
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
 *
 * $Id: wluc_phy.c 458728 2014-02-27 18:15:25Z $
 */

#ifdef WIN32
#include <windows.h>
#endif // endif

#include <wlioctl.h>

#if	defined(DONGLEBUILD)
#include <typedefs.h>
#include <osl.h>
#endif // endif

/* Because IL_BIGENDIAN was removed there are few warnings that need
 * to be fixed. Windows was not compiled earlier with IL_BIGENDIAN.
 * Hence these warnings were not seen earlier.
 * For now ignore the following warnings
 */
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4761)
#endif // endif

#include <bcmutils.h>
#include <bcmendian.h>
#include <bcmsrom_fmt.h>
#include <bcmsrom_tbl.h>
#include <bcmdevs.h>
#include "wlu_common.h"
#include "wlu.h"
#include <miniopt.h>

/* For backwards compatibility, the absense of the define 'NO_FILESYSTEM_SUPPORT'
 * implies that a filesystem is supported.
 */
#if !defined(BWL_NO_FILESYSTEM_SUPPORT)
#define BWL_FILESYSTEM_SUPPORT
#endif // endif

static cmd_func_t wl_pkteng, wl_pkteng_stats, wl_phy_txpwrindex;
static cmd_func_t wl_sample_collect;
static cmd_func_t wl_phy_force_crsmin;
static cmd_func_t wl_phy_rssi_ant;
static cmd_func_t wl_tssi, wl_atten, wl_evm;
static cmd_func_t wl_interfere, wl_interfere_override;
static cmd_func_t wl_get_instant_power;
static cmd_func_t wl_phymsglevel;
#if defined(BCMDBG)
static cmd_func_t wl_phy_debug_cmd;
#endif // endif
static cmd_func_t wl_rifs;
static cmd_func_t wl_rifs_advert;
static cmd_func_t wl_test_tssi, wl_test_tssi_offs, wl_phy_rssiant, wl_rxiq;
static cmd_func_t wl_test_idletssi;
static cmd_func_t wlu_afeoverride;
static cmd_func_t wl_phy_papdepstbl;
static cmd_func_t wl_phy_txiqcc, wl_phy_txlocc;
static cmd_func_t wl_rssi_cal_freq_grp_2g;
static cmd_func_t wl_phy_rssi_gain_delta_2g, wl_phy_rssi_gain_delta_5g;
static cmd_func_t wl_phy_rssi_gain_delta_2g_sub;
static cmd_func_t wl_phy_rxgainerr;
static cmd_func_t wl_phytable, wl_phy_pavars, wl_phy_povars;
static cmd_func_t wl_phy_fem, wl_phy_maxpower;
static cmd_func_t wl_phy_rpcalvars;
static cmd_func_t wl_phy_force_vsdb_chans;
static cmd_func_t wl_radar_args, wl_radar_thrs, wl_radar_thrs2;
static cmd_func_t wl_phy_dyn_switch_th;

#if defined(WLTEST)
static cmd_func_t wl_patrim;
#endif // endif

typedef struct {
	uint value;
	const char *string;
} phy_msg_t;

static cmd_t wl_phy_cmds[] = {
	{ "restart", wl_void, -1, WLC_RESTART,
	"Restart driver.  Driver must already be down."},
	{ "phymsglevel", wl_phymsglevel, WLC_GET_VAR, WLC_SET_VAR,
	"set phy debugging message bitvector\n"
	"\ttype \'wl phymsglevel ?\' for values" },
	{ "tssi", wl_tssi, WLC_GET_TSSI, -1,
	"Get the tssi value from radio" },
	{ "txpathpwr", wl_int, WLC_GET_TX_PATH_PWR, WLC_SET_TX_PATH_PWR,
	"Turn the tx path power on or off on 2050 radios" },
	{ "powerindex", wl_int, WLC_GET_PWRIDX, WLC_SET_PWRIDX,
	"Set the transmit power for A band(0-63).\n"
	"\t-1 - default value" },
	{ "atten", wl_atten, WLC_GET_ATTEN, WLC_SET_ATTEN,
	"Set the transmit attenuation for B band. Args: bb radio txctl1.\n"
	"\tauto to revert to automatic control\n"
	"\tmanual to supspend automatic control" },
	{ "phyreg", wl_reg, WLC_GET_PHYREG, WLC_SET_PHYREG,
	"Get/Set a phy register:\n"
	"\toffset [ value ] [ band ]" },
	{ "radioreg", wl_reg, WLC_GET_RADIOREG, WLC_SET_RADIOREG,
	"Get/Set a radio register:\n"
	"\toffset [ value ] [ band/core ]\n"
	"HTPHY:\n"
	"\tGet a radio register: wl radioreg [ offset ] [ cr0/cr1/cr2 ]\n"
	"\tSet a radio register: wl radioreg [ offset ] [ value ] [ cr0/cr1/cr2/all ]\n"
	"ACPHY:\n"
	"\tGet a radio register: wl radioreg [ offset ] [ cr0/cr1/cr2/pll ]\n"
	"\tSet a radio register: wl radioreg [ offset ] [ value ] [ cr0/cr1/cr2/pll/all ]"},
	{ "phy_afeoverride", wlu_afeoverride, WLC_GET_VAR, WLC_SET_VAR, "g/set AFE override"},
	{ "pcieserdesreg", wlu_reg3args, WLC_GET_VAR, WLC_SET_VAR,
	"g/set SERDES registers: dev offset [val]"},
	{ "txinstpwr", wl_get_instant_power, WLC_GET_VAR, -1,
	"Return tx power based on instant TSSI "},
	{ "evm", wl_evm, -1, WLC_EVM,
	"Start an EVM test on the given channel, or stop EVM test.\n"
	"\tArg 1 is channel number 1-14, or \"off\" or 0 to stop the test.\n"
	"\tArg 2 is optional rate (1, 2, 5.5 or 11)"},
	{ "noise", wl_int, WLC_GET_PHY_NOISE, -1,
	"Get noise (moving average) right after tx in dBm" },
	{ "fqacurcy", wl_int, -1, WLC_FREQ_ACCURACY,
	"Manufacturing test: set frequency accuracy mode.\n"
	"\tfreqacuracy syntax is: fqacurcy <channel>\n"
	"\tArg is channel number 1-14, or 0 to stop the test." },
	{ "crsuprs", wl_int, -1, WLC_CARRIER_SUPPRESS,
	"Manufacturing test: set carrier suppression mode.\n"
	"\tcarriersuprs syntax is: crsuprs <channel>\n"
	"\tArg is channel number 1-14, or 0 to stop the test." },
	{ "longtrain", wl_int, -1, WLC_LONGTRAIN,
	"Manufacturing test: set longtraining mode.\n"
	"\tlongtrain syntax is: longtrain <channel>\n"
	"\tArg is A band channel number or 0 to stop the test." },
	{ "interference", wl_interfere, WLC_GET_INTERFERENCE_MODE, WLC_SET_INTERFERENCE_MODE,
	"NON-ACPHY. Get/Set interference mitigation mode. Choices are:\n"
	"\t0 = none\n"
	"\t1 = non wlan\n"
	"\t2 = wlan manual\n"
	"\t3 = wlan automatic\n"
	"\t4 = wlan automatic with noise reduction"
	"\n\n\tACPHY. Get/Set interference mitigation mode. Bit-Mask:\n"
	"\t0 = desense based on glitches\n"
	"\t1 = limit pktgain based on hwaci (high pwr aci)\n"
	"\t2 = limit pktgain based on w2/nb (high pwr aci)\n"
	"\t3 = enable preemption\n"
	"\t4 = enable HWACI based mitigation\n"
	"\t5 = enable low power detect preemption (requires bit 3 - preemption - to be set too)\n"
	"\tSo a value of 63 would enable all six\n"},
	{ "interference_override", wl_interfere_override,
	WLC_GET_INTERFERENCE_OVERRIDE_MODE,
	WLC_SET_INTERFERENCE_OVERRIDE_MODE,
	"NON-ACPHY. Get/Set interference mitigation override. Choices are:\n"
	"\t0 = no interference mitigation\n"
	"\t1 = non wlan\n"
	"\t2 = wlan manual\n"
	"\t3 = wlan automatic\n"
	"\t4 = wlan automatic with noise reduction\n"
	"\t-1 = remove override, override disabled"
	"\n\n\tACPHY. Get/Set interference mitigation mode. Bit-Mask:\n"
	"\t-1 = remove override, override disabled\n"
	"\t0 = desense based on glitches\n"
	"\t1 = limit pktgain based on hwaci (high pwr aci)\n"
	"\t2 = limit pktgain based on w2/nb (high pwr aci)\n"
	"\t3 = enable preemption\n"
	"\t4 = enable HWACI based mitigation\n"
	"\t5 = enable low power detect preemption (requires bit 3 - preemption - to be set too)\n"
	"\tSo a value of 63 would enable all six\n"},
	{ "phy_txpwrindex", wl_phy_txpwrindex, WLC_GET_VAR, WLC_SET_VAR,
	"usage: (set) phy_txpwrindex core0_idx core1_idx core2_idx core3_idx"
	"       (get) phy_txpwrindex, return format: core0_idx core1_idx core2_idx core3_idx"
	"Set/Get txpwrindex"
	},
	{ "rssi_cal_freq_grp_2g", wl_rssi_cal_freq_grp_2g, WLC_GET_VAR, WLC_SET_VAR,
	"usage: wl_rssi_cal_freq_grp_2g [chan_1_2,chan_3_4,...,chan_13_14]\n"
	"Each of the variables like - chan_1_2 is a byte"
	"Upper nibble of this byte is for chan1 and lower for chan2"
	"MSB of the nibble tells if the channel is used for calibration"
	"3 LSB's tell which group the channel falls in"
	"Set/get rssi calibration frequency grouping"
	},
	{ "phy_rssi_gain_delta_2gb0", wl_phy_rssi_gain_delta_2g_sub, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_2gb0 [val0 val1 ....]\n"
	"Number of arguments can be - "
	"\t 8 for single core (4345 and 4350)"
	"\t 9 by specifying core_num followed by 8 arguments (4345 and 4350)"
	"\t 16 for both cores (4350)"
	"Set/get rssi gain delta values"
	},
	{ "phy_rssi_gain_delta_2gb1", wl_phy_rssi_gain_delta_2g_sub, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_2gb1 [val0 val1 ....]\n"
	"Number of arguments can be - "
	"\t 8 for single core (4345 and 4350)"
	"\t 9 by specifying core_num followed by 8 arguments (4345 and 4350)"
	"\t 16 for both cores (4350)"
	"Set/get rssi gain delta values"
	},
	{ "phy_rssi_gain_delta_2gb2", wl_phy_rssi_gain_delta_2g_sub, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_2gb2 [val0 val1 ....]\n"
	"Number of arguments can be - "
	"\t 8 for single core (4345 and 4350)"
	"\t 9 by specifying core_num followed by 8 arguments (4345 and 4350)"
	"\t 16 for both cores (4350)"
	"Set/get rssi gain delta values"
	},
	{ "phy_rssi_gain_delta_2gb3", wl_phy_rssi_gain_delta_2g_sub, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_2gb3 [val0 val1 ....]\n"
	"Number of arguments can be - "
	"\t 8 for single core (4345 and 4350)"
	"\t 9 by specifying core_num followed by 8 arguments (4345 and 4350)"
	"\t 16 for both cores (4350)"
	"Set/get rssi gain delta values"
	},
	{ "phy_rssi_gain_delta_2gb4", wl_phy_rssi_gain_delta_2g_sub, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_2gb4 [val0 val1 ....]\n"
	"Number of arguments can be - "
	"\t 8 for single core (4345 and 4350)"
	"\t 9 by specifying core_num followed by 8 arguments (4345 and 4350)"
	"\t 16 for both cores (4350)"
	"Set/get rssi gain delta values"
	},
	{ "phy_rssi_gain_delta_2g", wl_phy_rssi_gain_delta_2g, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_2g [val0 val1 ....]\n"
	"Set/get rssi gain delta values"
	},
	{ "phy_rssi_gain_delta_5gl", wl_phy_rssi_gain_delta_5g, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_5gl [val0 val1 ....]\n"
	"Set/get rssi gain delta values"
	},
	{ "phy_rssi_gain_delta_5gml", wl_phy_rssi_gain_delta_5g, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_5gml [val0 val1 ....]\n"
	"Set/get rssi gain delta values"
	},
	{ "phy_rssi_gain_delta_5gmu", wl_phy_rssi_gain_delta_5g, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_5gmu [val0 val1 ....]\n"
	"Set/get rssi gain delta values"
	},
	{ "phy_rssi_gain_delta_5gh", wl_phy_rssi_gain_delta_5g, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rssi_gain_delta_5gh [val0 val1 ....]\n"
	"Set/get rssi gain delta values"
	},
	{ "phy_rxgainerr_2g", wl_phy_rxgainerr, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rxgainerr_2g [val0 val1 ....]\n"
	"Set/get rx gain delta values"
	},
	{ "phy_rxgainerr_5gl", wl_phy_rxgainerr, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rxgainerr_5gl [val0 val1 ....]\n"
	"Set/get rx gain delta values"
	},
	{ "phy_rxgainerr_5gm", wl_phy_rxgainerr, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rxgainerr_5gml [val0 val1 ....]\n"
	"Set/get rx gain delta values"
	},
	{ "phy_rxgainerr_5gh", wl_phy_rxgainerr, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rxgainerr_5gmu [val0 val1 ....]\n"
	"Set/get rx gain delta values"
	},
	{ "phy_rxgainerr_5gu", wl_phy_rxgainerr, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_rxgainerr_5gh [val0 val1 ....]\n"
	"Set/get rx gain delta values"
	},
	{ "phy_test_tssi", wl_test_tssi, WLC_GET_VAR, -1,
	"wl phy_test_tssi val"},
	{ "phy_test_tssi_offs", wl_test_tssi_offs, WLC_GET_VAR, -1,
	"wl phy_test_tssi_offs val"},
	{ "phy_rssiant", wl_phy_rssiant, WLC_GET_VAR, -1,
	"wl phy_rssiant antindex(0-3)"},
	{ "phy_rssi_ant", wl_phy_rssi_ant, WLC_GET_VAR, WLC_SET_VAR,
	"Get RSSI per antenna (only gives RSSI of current antenna for SISO PHY)"},
	{ "phy_test_idletssi", wl_test_idletssi, WLC_GET_VAR, -1,
	"get idletssi for the given core; wl phy_test_idletssi corenum"},
	{ "phy_setrptbl", wl_var_void, -1, WLC_SET_VAR,
	"populate the reciprocity compensation table based on SROM cal content\n\n"
	"\tusage: wl phy_setrptbl"},
	{ "phy_forceimpbf", wl_var_void, -1, WLC_SET_VAR,
	"force the beamformer into implicit TXBF mode and ready to construct steering matrix\n\n"
	"\tusage: wl phy_forceimpbf"},
	{ "phy_forcesteer", wl_var_setint, -1, WLC_SET_VAR,
	"force the beamformer to apply steering matrix when TXBF is turned on\n\n"
	"\tusage: wl phy_forcesteer 1/0"},
#if defined(BCMDBG)
	{ "phy_force_gainlevel", wl_phy_debug_cmd, WLC_GET_VAR, WLC_SET_VAR,
	"Force rxgain level \n"
	"\t 0 : force to init gain\n"
	"\t 1 : force to clip hi gain\n"
	"\t 2 : force to clip md gain\n"
	"\t 3 : force to clip lo gain\n"
	"\t 4 : force to adc clip gain\n"
	"\t 5 : force to nb clip gain\n"
	"\t 6 : force to wb clip gain\n"
	"\t -1 : disable\n"
	"\t usage: wl phy_force_gainlevel <int32 var>"
	},
#endif // endif
#if defined(BCMDBG)
	{ "phy_force_fdiqi", wl_phy_debug_cmd, WLC_GET_VAR, WLC_SET_VAR,
	"Enable/disable FDIQI Cal/Comp \n"
	"\t 0 : disable\n"
	"\t 1 : enable\n"
	"\t usage: wl phy_force_fdiqi <int32 var>"
	},
#endif // endif
#if defined(BCMDBG)
	{ "phy_btcoex_desense", wl_phy_debug_cmd, WLC_GET_VAR, WLC_SET_VAR,
	"Enable/disable btcoex desense\n"
	"\t 0 : disable\n"
	"\t 1 : mode 1\n"
	"\t usage: wl phy_btcoex_desense <int32 var>"
	},
#endif // endif
	{ "lcnphy_papdepstbl", wl_phy_papdepstbl, -1, WLC_GET_VAR,
	"print papd eps table; Usage: wl lcnphy_papdepstbl"
	},
	{ "rifs", wl_rifs, WLC_GET_VAR, WLC_SET_VAR,
	"set/get the rifs status; usage: wl rifs <1/0> (On/Off)"
	},
	{ "rifs_advert", wl_rifs_advert, WLC_GET_VAR, WLC_SET_VAR,
	"set/get the rifs mode advertisement status; usage: wl rifs_advert <-1/0> (Auto/Off)"
	},
	{ "phy_rxiqest", wl_rxiq, WLC_GET_VAR, -1,
	"Get phy RX IQ noise in dBm:\n"
	"\t-s # of samples (2^n)\n"
	"\t-a antenna select, 0,1,2 or 3\n"
	  "\t-r resolution select, 0 (coarse) or 1 (fine)\n"
	  "\t-f lpf hpc override select, 0 (hpc unchanged) or 1 (overridden to ltrn mode)\n"
	  "\t-w dig lpf override select, 0 (lpf unchanged) or 1 (overridden to ltrn_lpf mode)"
	  "\t or 2 (bypass)\n"
	  "\t-g gain-correction select, 0 (disable), 1(enable full correction) \n"
	  "\t	2 (enable temperature correction) or 3(verify rssi_gain_delta)\n"
	  "\t-e extra INITgain in dB on top of default. Valid values = {0, 3, 6, .., 21, 24}\n"
	  "\t-i gain mode select, 0 (default gain), 1 (fixed high gain) or 4 (fixed low gain)."
	},
	{ "phy_txiqcc", wl_phy_txiqcc, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_txiqcc [a b]\n"
	"Set/get the iqcc a, b values"
	},
	{ "phy_txlocc", wl_phy_txlocc, WLC_GET_VAR, WLC_SET_VAR,
	"usage: phy_txlocc [di dq ei eq fi fq]\n"
	"Set/get locc di dq ei eq fi fq values"
	},
	{ "phytable", wl_phytable, WLC_GET_VAR, WLC_SET_VAR,
	"usage: wl phytable table_id offset width_of_table_element [table_element]\n"
	"Set/get table element of a table with the given ID at the given offset\n"
	"Note that table width supplied should be 8, 16, 32, 48 or 64\n"
	"table ID, table offset can not be negative"
	},
	{ "force_vsdb_chans", wl_phy_force_vsdb_chans, WLC_GET_VAR, WLC_SET_VAR,
	"Set/get  channels for forced vsdb mode\n"
	"usage: wl force_vsdb_chans chan1 chan2\n"
	"Note: Give chan in the same format as chanspec: eg force_vsdb_chans 1l 48u"
	},
	{ "pavars", wl_phy_pavars, WLC_GET_VAR, WLC_SET_VAR,
	"Set/get temp PA parameters\n"
	"usage: wl down\n"
	"       wl pavars pa2gw0a0=0x1 pa2gw1a0=0x2 pa2gw2a0=0x3 ... \n"
	"       wl pavars\n"
	"       wl up\n"
	"  override the PA parameters after driver attach(srom read), before diver up\n"
	"  These override values will be propogated to HW when driver goes up\n"
	"  PA parameters in one band range (2g, 5gl, 5g, 5gh) must all present if\n"
	"  one of them is specified in the command, otherwise it will be filled with 0"
	},
	{ "povars", wl_phy_povars, WLC_GET_VAR, WLC_SET_VAR,
	"Set/get temp power offset\n"
	"usage: wl down\n"
	"       wl povars cck2gpo=0x1 ofdm2gpo=0x2 mcs2gpo=0x3 ... \n"
	"       wl povars\n"
	"       wl up\n"
	"  override the power offset after driver attach(srom read), before diver up\n"
	"  These override values will be propogated to HW when driver goes up\n"
	"  power offsets in one band range (2g, 5gl, 5g, 5gh) must all present if\n"
	"  one of them is specified in the command, otherwise it will be filled with 0"
	"  cck(2g only), ofdm, and mcs(0-7) for NPHY are supported "
	},
	{ "rpcalvars", wl_phy_rpcalvars, WLC_GET_VAR, WLC_SET_VAR,
	"Set/get temp RPCAL parameters\n"
	"usage: wl down\n"
	"       wl rpcalvars rpcal2g=0x1 \n"
	"       wl rpcalvars\n"
	"       wl up\n"
	"  override the RPCAL parameters after driver attach(srom read), before diver up\n"
	"  These override values will be propogated to HW when driver goes up\n"
	"  Only the RPCAL parameter specified in the command is updated, the rest is untouched"
	},
	{ "fem", wl_phy_fem, WLC_GET_VAR, WLC_SET_VAR,
	"Set temp fem2g/5g value\n"
	"usage: wl fem (tssipos2g=0x1 extpagain2g=0x2 pdetrange2g=0x1 triso2g=0x1 antswctl2g=0)\n"
	"	(tssipos5g=0x1 extpagain5g=0x2 pdetrange5g=0x1 triso5g=0x1 antswctl5g=0)"
	},
	{ "maxpower", wl_phy_maxpower, WLC_GET_VAR, WLC_SET_VAR,
	"Set temp maxp2g(5g)a0(a1) value\n"
	"usage: wl maxpower maxp2ga0=0x1 maxp2ga1=0x2 maxp5ga0=0xff maxp5ga1=0xff\n"
	"       maxp5gla0=0x3 maxp5gla1=0x4 maxp5gha0=0x5 maxp5gha1=0x6"
	},
	{ "sample_collect", wl_sample_collect, WLC_PHY_SAMPLE_COLLECT, -1,
	"Optional parameters ACPHY/HTPHY/(NPHY with NREV >= 7) are:\n"
	"\t-f File name to dump the sample buffer (default \"sample_collect.dat\")\n"
	"\t-t Trigger condition (default now)\n"
	"\t\t now, good_fcs, bad_fcs, bad_plcp, crs, crs_glitch, crs_deassert\n"
	"\t-b PreTrigger duration in us (default 10)\n"
	"\t-a PostTrigger duration in us (default 10) \n"
	"\t-m Sample collect mode (default 1) \n"
	  "\t\tSC_MODE_0_sd_adc\t\t\t0\n"
	  "\t\tSC_MODE_1_sd_adc_5bits\t\t\t1\n"
	  "\t\tSC_MODE_2_cic0\t\t\t\t2\n"
	  "\t\tSC_MODE_3_cic1\t\t\t\t3\n"
	  "\t\tSC_MODE_4s_rx_farrow_1core\t\t4\n"
	  "\t\tSC_MODE_4m_rx_farrow\t\t\t5\n"
	  "\t\tSC_MODE_5_iq_comp\t\t\t6\n"
	  "\t\tSC_MODE_6_dc_filt\t\t\t7\n"
	  "\t\tSC_MODE_7_rx_filt\t\t\t8\n"
	  "\t\tSC_MODE_8_rssi\t\t\t\t9\n"
	  "\t\tSC_MODE_9_rssi_all\t\t\t10\n"
	  "\t\tSC_MODE_10_tx_farrow\t\t\t11\n"
	  "\t\tSC_MODE_11_gpio\t\t\t\t12\n"
	  "\t\tSC_MODE_12_gpio_trans\t\t\t13\n"
	  "\t\tSC_MODE_14_spect_ana\t\t\t14\n"
	  "\t\tSC_MODE_5s_iq_comp\t\t\t15\n"
	  "\t\tSC_MODE_6s_dc_filt\t\t\t16\n"
	  "\t\tSC_MODE_7s_rx_filt\t\t\t17\n"
	  "\t\t HTPHY: 0=adc, 1..3=adc+rssi, 4=gpio\n"
	"\t\t NPHY: 1=Dual-Core adc[9:2], 2=Core0 adc[9:0], 3=Core1 adc[9:0], gpio=gpio\n"
	"\t-g GPIO mux select (default 0)\n"
	"\t\t use only for gpio mode\n"
	"\t-d Downsample enable (default 0)\n"
	"\t\t use only for HTPHY\n"
	"\t-e BeDeaf enable (default 0)\n"
	"\t-i Timeout in units of 10us. (ACPHY is in 10ms unit) (default 1000)\n"
	"Optional parameters (NPHY with NREV < 7) are:\n"
	"\t-f File name to dump the sample buffer (binary format, default \"sample_collect.dat\")\n"
	"\t-u Sample collect duration in us (default 60)\n"
	"\t-c Cores to do sample collect, only if BW=40MHz (default both)\n"
	"Optional parameters LCN40PHY are:\n"
	"\t-f File name to dump the sample buffer (default \"sample_collect.dat\")\n"
	"\t-t Trigger condition (default now)\n"
	"\t\t now\n"
	"\t-s Trigger State (default 0)\n"
	"\t-x Module_Sel1 (default 2)\n"
	"\t-y Module_Sel2 (default 6)\n"
	"\t-n Number of samples (Max 2048, default 2048)\n"
	"For (NREV < 7), the NPHY buffer returned has the format:\n"
	"\tIn 20MHz [(uint16)num_bytes, <I(core0), Q(core0), I(core1), Q(core1)>]\n"
	"\tIn 40MHz [(uint16)num_bytes(core0), <I(core0), Q(core0)>,\n"
	"\t\t(uint16)num_bytes(core1), <I(core1), Q(core1)>]"},
	{ "pkteng_start", wl_pkteng, -1, WLC_SET_VAR,
	"start packet engine tx usage: wl pkteng_start <xx:xx:xx:xx:xx:xx>"
	" <tx|txwithack> [(async)|sync] [ipg] [len] [nframes] [src]\n"
	"\tstart packet engine rx usage: wl pkteng_start <xx:xx:xx:xx:xx:xx>"
	" <rx|rxwithack> [(async)|sync] [rxframes] [rxtimeout]\n"
	"\tsync: synchronous mode\n"
	"\tipg: inter packet gap in us\n"
	"\tlen: packet length\n"
	"\tnframes: number of frames; 0 indicates continuous tx test\n"
	"\tsrc: source mac address\n"
	"\trxframes: number of receive frames (sync mode only)\n"
	"\trxtimeout: maximum timout in msec (sync mode only)"},
	{ "pkteng_stop", wl_pkteng, -1, WLC_SET_VAR,
	"stop packet engine; usage: wl pkteng_stop <tx|rx>"},
	{ "pkteng_stats", wl_pkteng_stats, -1, WLC_GET_VAR,
	"packet engine stats; usage: wl pkteng_stats"},
	{"phy_force_crsmin", wl_phy_force_crsmin, -1, WLC_SET_VAR,
	"Auto crsmin: \n"
	"       phy_force_crsmin -1\n"
	"Default crsmin value\n\n"
	"       phy_force_crsmin 0\n"
	"Set the crsmin value\n"
	"       phy_force_crsmin core0_th core1_offset core2_offset\n"
	"\n"
	"Threshold values = 2.5 x NoisePwr_dBm + intercept\n"
	"       where\n"
	"              NoisePwr_dBm ~= -36/-33/-30dBm for 20/40/80MHz, respectively\n"
	"              Intercept = 132/125/119 for 20/40/80MHz, respectively"
	},
	{ "radarargs", wl_radar_args, WLC_GET_VAR, WLC_SET_VAR,
	"Get/Set Radar parameters in \n"
	"\torder as version, npulses, ncontig, min_pw, max_pw, thresh0, thresh1,\n"
	"\tblank, fmdemodcfg, npulses_lp, min_pw_lp, max_pw_lp,\n"
	"\tmin_fm_lp, max_span_lp, min_deltat, max_deltat,\n"
	"\tautocorr, st_level_time, t2_min, fra_pulse_err, npulses_fra,\n"
	"\tnpulses_stg2, npulses_stg3, percal_mask, quant, \n"
	"\tmin_burst_intv_lp, max_burst_intv_lp, nskip_rst_lp, max_pw_tol, feature_mask, \n"
	"\tthresh0_sc, thresh1_sc"},
	{ "radarargs40", wl_radar_args, WLC_GET_VAR, WLC_SET_VAR,
	"Get/Set Radar parameters for 40Mhz channel in \n"
	"\torder as version, npulses, ncontig, min_pw, max_pw, thresh0, thresh1,\n"
	"\tthresh0_sc, thresh1_sc, blank, fmdemodcfg, npulses_lp, min_pw_lp, max_pw_lp,\n"
	"\tmin_fm_lp, max_span_lp, min_deltat, max_deltat,\n"
	"\tautocorr, st_level_time, t2_min, fra_pulse_err, npulses_fra,\n"
	"\tnpulses_stg2, npulses_stg3, percal_mask, quant, \n"
	"\tmin_burst_intv_lp, max_burst_intv_lp, nskip_rst_lp, max_pw_tol, feature_mask,\n"
	"\tthresh0_sc, thresh1_sc"},
	{ "radarthrs", wl_radar_thrs, -1, WLC_SET_VAR,
	"Set Radar threshold for both 20 & 40MHz & 80MHz BW:\n"
	"\torder as thresh0_20_lo, thresh1_20_lo, thresh0_40_lo, thresh1_40_lo\n"
	"\tthresh0_80_lo, thresh1_80_lo, thresh0_20_hi, thresh1_20_hi\n"
	"\tthresh0_40_hi, thresh1_40_hi, thresh0_80_hi, thresh1_80_hi\n"
	"\tthresh0_160_lo, thresh1_160_lo, thresh0_160_hi, thresh1_160_hi"},
	{ "radarthrs2", wl_radar_thrs2, WLC_GET_VAR, WLC_SET_VAR,
	"Set Radar threshold for both 20 & 40MHz & 80MHz BW:\n"
	"\tthresh0_sc_20_lo, thresh1_sc_20_lo, thresh0_sc_40_lo, thresh1_sc_40_lo\n"
	"\tthresh0_sc_80_lo, thresh1_sc_80_lo, thresh0_sc_20_hi, thresh1_sc_20_hi\n"
	"\tthresh0_sc_40_hi, thresh1_sc_40_hi, thresh0_sc_80_hi, thresh1_sc_80_hi\n"
	"\tfc_varth_sb, fc_varth_bin5_sb, notradar_enb, max_notradar_lp, max_notradar,\n"
	"\tmax_notradar_lp_sc, max_notradar_sc, highpow_war_enb, highpow_sp_ratio"},
	{ "phy_dyn_switch_th", wl_phy_dyn_switch_th, WLC_GET_VAR, WLC_SET_VAR,
	"Set wighting number for dynamic switch:\n"
	"\trssi_gain_80_3, rssi_gain_80_2, rssi_gain_80_1, rssi_gain_80_0\n"
	"\trssi_gain_160_3, rssi_gain_160_2, rssi_gain_160_1, rssi_gain_160_0\n"
	"\trssi_th_2, rssi_th_1, rssi_th_0"},
#if defined(WLTEST)
	{ "patrim", wl_patrim, WLC_GET_VAR, -1,
	"Get PA trim option" },
#endif // endif
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_phy_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register phy commands */
	wl_module_cmds_register(wl_phy_cmds);
}

static int
wl_phy_rssi_ant(void *wl, cmd_t *cmd, char **argv)
{
	int ret = 0;
	uint i;
	wl_rssi_ant_t *rssi_ant_p;

	if (!*++argv) {
		/* Get */
		void *ptr = NULL;

		if ((ret = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr)) < 0)
			return ret;

		rssi_ant_p = (wl_rssi_ant_t *)ptr;
		rssi_ant_p->version = dtoh32(rssi_ant_p->version);
		rssi_ant_p->count = dtoh32(rssi_ant_p->count);

		if (rssi_ant_p->count == 0) {
			printf("not supported on this chip\n");
		} else {
			for (i = 0; i < rssi_ant_p->count; i++)
				printf("rssi[%d] %d  ", i, rssi_ant_p->rssi_ant[i]);
			printf("\n");
		}
	} else {
		ret = BCME_USAGE_ERROR;
	}
	return ret;
}

#include <devctrl_if/phyioctl_defs.h>

static phy_msg_t wl_phy_msgs[] = {
	{PHYHAL_ERROR,	"error"},
	{PHYHAL_ERROR, 	"err"},
	{PHYHAL_TRACE,	"trace"},
	{PHYHAL_INFORM,	"inform"},
	{PHYHAL_TMP,	"tmp"},
	{PHYHAL_TXPWR,	"txpwr"},
	{PHYHAL_CAL,	"cal"},
	{PHYHAL_RADAR,	"radar"},
	{PHYHAL_THERMAL, "thermal"},
	{PHYHAL_PAPD,	"papd"},
	{PHYHAL_RXIQ,	"rxiq"},
	{PHYHAL_FCBS,	"fcbs"},
	{PHYHAL_CHANLOG, "chanlog"},
	{0,		NULL}
	};

static int
wl_phymsglevel(void *wl, cmd_t *cmd, char **argv)
{
	int ret, i;
	uint val = 0, last_val = 0;
	uint phymsglevel = 0, phymsglevel_add = 0, phymsglevel_del = 0;
	char *endptr;
	phy_msg_t *phy_msg = wl_phy_msgs;
	const char *cmdname = "phymsglevel";

	UNUSED_PARAMETER(cmd);
	if ((ret = wlu_iovar_getint(wl, cmdname, (int *)&phymsglevel) < 0)) {
		return ret;
	}
#ifndef DSLCPE_ENDIAN
	phymsglevel = dtoh32(phymsglevel);
#endif
	if (!*++argv) {
		printf("0x%x ", phymsglevel);
		for (i = 0; (val = phy_msg[i].value); i++) {
			if ((phymsglevel & val) && (val != last_val))
				printf(" %s", phy_msg[i].string);
			last_val = val;
		}
		printf("\n");
		return (0);
	}
	while (*argv) {
		char *s = *argv;
		if (*s == '+' || *s == '-')
			s++;
		else
			phymsglevel_del = ~0; /* make the whole list absolute */
		val = strtoul(s, &endptr, 0);
		if (val == 0xFFFFFFFF) {
			fprintf(stderr,
				"Bits >32 are not supported on this driver version\n");
			val = 1;
		}
		/* not an integer if not all the string was parsed by strtoul */
		if (*endptr != '\0') {
			for (i = 0; (val = phy_msg[i].value); i++)
				if (stricmp(phy_msg[i].string, s) == 0)
					break;
				if (!val)
					goto usage;
		}
		if (**argv == '-')
			phymsglevel_del |= val;
		else
			phymsglevel_add |= val;
		++argv;
	}
	phymsglevel &= ~phymsglevel_del;
	phymsglevel |= phymsglevel_add;
#ifndef DSLCPE_ENDIAN
	phymsglevel = htod32(phymsglevel);
#endif
	return (wlu_iovar_setint(wl, cmdname, (int)phymsglevel));

usage:
	fprintf(stderr, "msg values may be a list of numbers or names from the following set.\n");
	fprintf(stderr, "Use a + or - prefix to make an incremental change.");
	for (i = 0; (val = phy_msg[i].value); i++) {
		if (val != last_val)
			fprintf(stderr, "\n0x%04x %s", val, phy_msg[i].string);
		else
			fprintf(stderr, ", %s", phy_msg[i].string);
		last_val = val;
	}
	return 0;
}
static int
wl_get_instant_power(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	tx_inst_power_t *power;
	uint band_list[3];

	UNUSED_PARAMETER(cmd);
	UNUSED_PARAMETER(argv);

	strcpy(buf, "txinstpwr");
	if ((ret = wlu_get(wl, WLC_GET_VAR, &buf[0], WLC_IOCTL_MAXLEN)) < 0) {
		return ret;
	}

	power = (tx_inst_power_t *)buf;
	/* Make the most of the info returned in band_list!
	 * b/g and a
	 * b/g-uni
	 * a-uni
	 * NOTE: NO a and b/g case ...
	 */
	if ((ret = wlu_get(wl, WLC_GET_BANDLIST, band_list, sizeof(band_list))) < 0)
		return (ret);
	band_list[0] = dtoh32(band_list[0]);
	band_list[1] = dtoh32(band_list[1]);
	band_list[2] = dtoh32(band_list[2]);

	/* If B/G is present it's always the lower index */
	if (band_list[1] == WLC_BAND_2G) {
			printf("Last B phy CCK est. power:\t%2d.%d dBm\n",
			       DIV_QUO(power->txpwr_est_Pout[0], 4),
			       DIV_REM(power->txpwr_est_Pout[0], 4));
			printf("Last B phy OFDM est. power:\t%2d.%d dBm\n",
			       DIV_QUO(power->txpwr_est_Pout_gofdm, 4),
			       DIV_REM(power->txpwr_est_Pout_gofdm, 4));

			printf("\n");
	}

	/* A band */
	if (band_list[1] == WLC_BAND_5G || (band_list[0] > 1 && band_list[2] == WLC_BAND_5G)) {
		printf("Last A phy est. power:\t\t%2d.%d dBm\n",
		       DIV_QUO(power->txpwr_est_Pout[1], 4),
		       DIV_REM(power->txpwr_est_Pout[1], 4));
	}

	return ret;
}

static int
wl_evm(void *wl, cmd_t *cmd, char **argv)
{
	int val[3];

	/* Get channel */
	if (!*++argv) {
		fprintf(stderr, "Need to specify at least one parameter\n");
		return BCME_USAGE_ERROR;
	}

	if (!stricmp(*argv, "off"))
		val[0] = 0;
	else
		val[0] = atoi(*argv);

	/* set optional parameters to default */
	val[1] = 4;	/* rate in 500Kb units */
	val[2] = 0;	/* This is ignored */

	/* Get optional rate and convert to 500Kb units */
	if (*++argv)
		val[1] = rate_string2int(*argv);

	val[0] = htod32(val[0]);
	val[1] = htod32(val[1]);
	val[2] = htod32(val[2]);
	return wlu_set(wl, cmd->set, val, sizeof(val));
}

static int
wl_tssi(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int val;

	UNUSED_PARAMETER(argv);

	if (cmd->get < 0)
		return -1;
	if ((ret = wlu_get(wl, cmd->get, &val, sizeof(int))) < 0)
		return ret;

	val = dtoh32(val);
	printf("CCK %d OFDM %d\n", (val & 0xff), (val >> 8) & 0xff);
	return 0;
}

static int
wl_atten(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	atten_t atten;
	char *endptr;

	memset(&atten, 0, sizeof(atten_t));

	if (!*++argv) {
		if (cmd->get < 0)
			return -1;

		if ((ret = wlu_get(wl, cmd->get, &atten, sizeof(atten_t))) < 0)
			return ret;

		printf("tx %s bb/radio/ctl1 %d/%d/%d\n",
		       (dtoh16(atten.auto_ctrl) ? "auto" : ""),
			dtoh16(atten.bb), dtoh16(atten.radio), dtoh16(atten.txctl1));

		return 0;
	} else {
		if (cmd->set < 0)
			return -1;

		if (!stricmp(*argv, "auto")) {
			atten.auto_ctrl = WL_ATTEN_PCL_ON;
			atten.auto_ctrl = htod16(atten.auto_ctrl);
		}
		else if (!stricmp(*argv, "manual")) {
			atten.auto_ctrl = WL_ATTEN_PCL_OFF;
			atten.auto_ctrl = htod16(atten.auto_ctrl);
		}
		else {
			atten.auto_ctrl = WL_ATTEN_APP_INPUT_PCL_OFF;
			atten.auto_ctrl = htod16(atten.auto_ctrl);

			atten.bb = (uint16)strtoul(*argv, &endptr, 0);
			atten.bb = htod16(atten.bb);
			if (*endptr != '\0') {
				/* not all the value string was parsed by strtol */
				return BCME_USAGE_ERROR;
			}

			if (!*++argv)
				return BCME_USAGE_ERROR;

			atten.radio = (uint16)strtoul(*argv, &endptr, 0);
			atten.radio = htod16(atten.radio);
			if (*endptr != '\0') {
				/* not all the value string was parsed by strtol */
				return BCME_USAGE_ERROR;
			}

			if (!*++argv)
				return BCME_USAGE_ERROR;

			atten.txctl1 = (uint16)strtoul(*argv, &endptr, 0);
			atten.txctl1 = htod16(atten.txctl1);
			if (*endptr != '\0') {
				/* not all the value string was parsed by strtol */
				return BCME_USAGE_ERROR;
			}

		}

		return wlu_set(wl, cmd->set, &atten, sizeof(atten_t));
	}
}

static int
wl_interfere(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int val;
	char *endptr = NULL;
	int mode;
	wlc_rev_info_t revinfo;
	uint32 phytype;

	memset(&revinfo, 0, sizeof(revinfo));
	ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo));
	if (ret) {
		return ret;
	}
	phytype = dtoh32(revinfo.phytype);

	if (!*++argv) {
		if (cmd->get < 0)
			return -1;
		if ((ret = wlu_get(wl, cmd->get, &mode, sizeof(mode))) < 0)
			return ret;
		mode = dtoh32(mode);
		if (phytype == WLC_PHY_TYPE_AC) {
			mode &= 0x7f;
			if (mode == INTERFERE_NONE) {
				printf("All interference mitigation is disabled. (mode 0)\n");
			} else {
				printf("\nMode = %d. Following ACI modes are enabled:\n", mode);
				if (mode & ACPHY_ACI_GLITCHBASED_DESENSE)
					printf("\tbit-mask %d:  Receiver Desense based on glitch "
					       "count\n",
					       ACPHY_ACI_GLITCHBASED_DESENSE);
				if (mode & ACPHY_ACI_HWACI_PKTGAINLMT)
					printf("\tbit-mask %d:  Limit pktgain based on hwaci "
					       "(high pwr aci)\n",
					       ACPHY_ACI_HWACI_PKTGAINLMT);
				if (mode & ACPHY_ACI_W2NB_PKTGAINLMT)
					printf("\tbit-mask %d:  Limit pktgain based on w2/nb "
					       "(high pwr aci)\n",
					       ACPHY_ACI_W2NB_PKTGAINLMT);
				if (mode & ACPHY_ACI_PREEMPTION)
					printf("\tbit-mask %d:  Preemption is enabled\n",
					       ACPHY_ACI_PREEMPTION);
				if (mode & ACPHY_HWACI_MITIGATION)
					printf("\tbit-mask %d: HW ACI Detection + Mitigation\n",
					       ACPHY_HWACI_MITIGATION);
				if (mode & ACPHY_LPD_PREEMPTION)
					printf("\tbit-mask %d: Low Power Detect Preemption\n",
					       ACPHY_LPD_PREEMPTION);
			}
			printf("\n");
		} else {
			switch (mode & 0x7f) {
			case INTERFERE_NONE:
				printf("All interference mitigation is disabled. (mode 0)\n");
				break;
			case NON_WLAN:
				printf("Non-wireless LAN Interference mitigation is enabled."
					" (mode 1)\n");
				break;
			case WLAN_MANUAL:
				printf("Wireless LAN Interference mitigation is enabled."
					" (mode 2)\n");
				break;
			case WLAN_AUTO:
				printf("Auto Wireless LAN Interference mitigation is enabled and ");
				if (mode & AUTO_ACTIVE)
					printf("active. (mode 3)\n");
				else
					printf("not active. (mode 3)\n");

				break;
			case WLAN_AUTO_W_NOISE:
				printf("Auto Wireless LAN Interference mitigation is enabled and ");
				if (mode & AUTO_ACTIVE)
					printf("active, ");
				else
					printf("not active, ");

				printf("and noise reduction is enabled. (mode 4)\n");
				break;
			}
		}
		return 0;
	} else {
		mode = INTERFERE_NONE;
		val = strtol(*argv, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;
		if (phytype == WLC_PHY_TYPE_AC) {
			if (val > ACPHY_ACI_MAX_MODE) {
				return -1;
			} else if ((val & ACPHY_LPD_PREEMPTION) && !(val & ACPHY_ACI_PREEMPTION)) {
				fprintf(stderr, "Low Power Detect Preemption (bit 5)"
						" requires Preemption (bit3) to be enabled\n");
				return BCME_USAGE_ERROR;
			} else {
				mode = val;
			}
		} else {
			switch (val) {
			case 0:
				mode = INTERFERE_NONE;
				break;
			case 1:
				mode = NON_WLAN;
				break;
			case 2:
				mode = WLAN_MANUAL;
				break;
			case 3:
				mode = WLAN_AUTO;
				break;
			case 4:
				mode = WLAN_AUTO_W_NOISE;
				break;
			default:
				return BCME_BADARG;
			}
		}

		mode = htod32(mode);
		return wlu_set(wl, cmd->set, &mode, sizeof(mode));
	}
}

static int
wl_interfere_override(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int val;
	char *endptr;
	int mode;
	wlc_rev_info_t revinfo;
	uint32 phytype;

	memset(&revinfo, 0, sizeof(revinfo));
	ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo));
	if (ret) {
		return ret;
	}
	phytype = dtoh32(revinfo.phytype);

	if (!*++argv) {
		if (cmd->get < 0)
			return -1;
		if ((ret = wlu_get(wl, cmd->get, &mode, sizeof(mode))) < 0) {
			return ret;
		}
		mode = dtoh32(mode);
		if (phytype == WLC_PHY_TYPE_AC) {
			if (mode == INTERFERE_OVRRIDE_OFF)
				printf("Interference override disabled.\n");
			else if (mode == INTERFERE_NONE)
				printf("Interference override NONE, "
				       "all mitigation disabled. (mode 0)\n");
			else {
				printf("\nInterference override mode = %d. Following ACI modes "
				       "are enabled:\n", mode);
				if (mode & ACPHY_ACI_GLITCHBASED_DESENSE)
					printf("\tbit-mask %d:  Receiver Desense based on glitch "
					       "count\n",
					       ACPHY_ACI_GLITCHBASED_DESENSE);
				if (mode & ACPHY_ACI_HWACI_PKTGAINLMT)
					printf("\tbit-mask %d:  Limit pktgain based on hwaci "
					       "(high pwr aci)\n",
					       ACPHY_ACI_HWACI_PKTGAINLMT);
				if (mode & ACPHY_ACI_W2NB_PKTGAINLMT)
					printf("\tbit-mask %d:  Limit pktgain based on w2/nb "
					       "(high pwr aci)\n",
					       ACPHY_ACI_W2NB_PKTGAINLMT);
				if (mode & ACPHY_ACI_PREEMPTION)
					printf("\tbit-mask %d:  Preemption is enabled\n",
					       ACPHY_ACI_PREEMPTION);
				if (mode & ACPHY_HWACI_MITIGATION)
					printf("\tbit-mask %d: HW ACI Detection + Mitigation\n",
					       ACPHY_HWACI_MITIGATION);
				if (mode & ACPHY_LPD_PREEMPTION)
					printf("\tbit-mask %d: Low Power Detect Preemption\n",
					       ACPHY_LPD_PREEMPTION);
			}
			printf("\n");
		} else {
			switch (mode & 0x7f) {
			case INTERFERE_NONE:
				printf("Interference override NONE, "
				"all mitigation disabled. (mode 0)\n");
				break;
			case NON_WLAN:
				printf("Interference override enabled. "
					" Non-wireless LAN Interference mitigation is enabled."
					" (mode 1)\n");
				break;
			case WLAN_MANUAL:
				printf("Interference override enabled.  "
					" Wireless LAN Interference mitigation is enabled."
					" (mode 2)\n");
				break;
			case WLAN_AUTO:
				printf("Interference override enabled. "
					" Interference mitigation is enabled and ");
				if (mode & AUTO_ACTIVE)
					printf("active. (mode 3)\n");
				else
					printf("not active. (mode 3)\n");
				break;
			case WLAN_AUTO_W_NOISE:
				printf("Interference override enabled. "
					" Interference mitigation is enabled and ");
				if (mode & AUTO_ACTIVE)
					printf("active, ");
				else
					printf("not active, ");
				printf("and noise reduction is enabled. (mode 4)\n");
				break;
			case INTERFERE_OVRRIDE_OFF:
				printf("Interference override disabled. \n");
				break;
			}
		}
		return 0;
	} else {
		mode = INTERFERE_NONE;
		val = strtol(*argv, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;
		if (phytype == WLC_PHY_TYPE_AC) {
			if (val > ACPHY_ACI_MAX_MODE) {
				return -1;
			} else if ((val & ACPHY_LPD_PREEMPTION) && !(val & ACPHY_ACI_PREEMPTION)) {
				fprintf(stderr, "Low Power Detect Preemption (bit 5)"
						" requires Preemption (bit3) to be enabled\n");
				return BCME_USAGE_ERROR;
			} else {
				mode = val;
			}
		} else {
			switch (val) {
			case 0:
				mode = INTERFERE_NONE;
				break;
			case 1:
				mode = NON_WLAN;
				break;
			case 2:
				mode = WLAN_MANUAL;
				break;
			case 3:
				mode = WLAN_AUTO;
				break;
			case 4:
				mode = WLAN_AUTO_W_NOISE;
				break;
			case INTERFERE_OVRRIDE_OFF:
				mode = INTERFERE_OVRRIDE_OFF;
				break;
			default:
				return BCME_BADARG;
			}
		}

		mode = htod32(mode);
		return wlu_set(wl, cmd->set, &mode, sizeof(mode));
	}
}

#define ACI_SPIN	"spin"
#define ACI_ENTER	"enter"
#define ACI_EXIT	"exit"
#define ACI_GLITCH	"glitch"

#define NPHY_ACI_ADCPWR_ENTER "adcpwr_enter"
#define NPHY_ACI_ADCPWR_EXIT "adcpwr_exit"
#define NPHY_ACI_REPEAT_CTR "repeat"
#define NPHY_ACI_NUM_SAMPLES "samples"
#define NPHY_ACI_UNDETECT "undetect_sz"
#define NPHY_ACI_LOPWR "loaci"
#define NPHY_ACI_MDPWR "mdaci"
#define NPHY_ACI_HIPWR "hiaci"
#define NPHY_ACI_NOISE_NOASSOC_GLITCH_TH_UP "nphy_noise_noassoc_glitch_th_up"
#define NPHY_ACI_NOISE_NOASSOC_GLITCH_TH_DN "nphy_noise_noassoc_glitch_th_dn"
#define NPHY_ACI_NOISE_ASSOC_GLITCH_TH_UP "nphy_noise_assoc_glitch_th_up"
#define NPHY_ACI_NOISE_ASSOC_GLITCH_TH_DN "nphy_noise_assoc_glitch_th_dn"
#define NPHY_ACI_NOISE_ASSOC_ACI_GLITCH_TH_UP "nphy_noise_assoc_aci_glitch_th_up"
#define NPHY_ACI_NOISE_ASSOC_ACI_GLITCH_TH_DN "nphy_noise_assoc_aci_glitch_th_dn"
#define NPHY_ACI_NOISE_NOASSOC_ENTER_TH "nphy_noise_noassoc_enter_th"
#define NPHY_ACI_NOISE_ASSOC_ENTER_TH "nphy_noise_assoc_enter_th"
#define NPHY_ACI_NOISE_ASSOC_RX_GLITCH_BADPLCP_ENTER_TH \
"nphy_noise_assoc_rx_glitch_badplcp_enter_th"
#define NPHY_ACI_NOISE_ASSOC_CRSIDX_INCR "nphy_noise_assoc_crsidx_incr"
#define NPHY_ACI_NOISE_NOASSOC_CRSIDX_INCR "nphy_noise_noassoc_crsidx_incr"
#define NPHY_ACI_NOISE_CRSIDX_DECR "nphy_noise_crsidx_decr"

#if defined(BWL_FILESYSTEM_SUPPORT)
#if !defined(_CFE_) && !defined(DONGLEBUILD)
static int
wl_do_samplecollect_lcn40(void *wl, wl_samplecollect_args_t *collect, uint8 *buff, FILE *fp)
{
	uint32 cnt;
	int ret = 0;
	uint32 *data;
	int16 IData, QData;
	uint16 wordlength = 14;
	uint16 mask = ((0x1 << wordlength) - 1);
	uint16 wrap = (0x1 << (wordlength - 1));
	uint16 maxd = (0x1 << wordlength);

	ret = wlu_iovar_getbuf(wl, "sample_collect", collect, sizeof(wl_samplecollect_args_t),
		buff, WLC_SAMPLECOLLECT_MAXLEN);

	if (ret)
		return ret;

	data = (uint32*)buff;
	for (cnt = 0; cnt < collect->nsamps; cnt++) {

		IData = data[cnt] & mask;
		QData = ((data[cnt] >> 16) & mask);

		if (IData >= wrap) {
			IData = IData - maxd;
		}
		if (QData >= wrap) {
			QData = QData - maxd;
		}
		fprintf(fp, "%d %d\n", IData, QData);
	}
	return cnt;
}
static int
wl_do_samplecollect_n(void *wl, wl_samplecollect_args_t *collect, uint8 *buff, FILE *fp)
{
	uint16 nbytes;
	int ret = 0;

	ret = wlu_iovar_getbuf(wl, "sample_collect", collect, sizeof(wl_samplecollect_args_t),
		buff, WLC_SAMPLECOLLECT_MAXLEN);

	if (ret)
		return ret;

	/* bytes 1:0 indicate capture length */
	while ((nbytes = ltoh16_ua(buff))) {
		nbytes += 2;
		ret = fwrite(buff, 1, nbytes, fp);
		if (ret != nbytes) {
			fprintf(stderr, "Error writing %d bytes to file, rc %d!\n",
				nbytes, ret);
			ret = -1;
			break;
		} else {
			fprintf(stderr, "Wrote %d bytes\n", nbytes);
		}
		buff += nbytes;
	}
	return (ret);
}
#endif // endif
#endif /* defined(BWL_FILESYSTEM_SUPPORT) */

#if defined(BWL_FILESYSTEM_SUPPORT)
#if (!defined(_CFE_) && !defined(DONGLEBUILD))
static int
wl_do_samplecollect(void *wl, wl_samplecollect_args_t *collect, int sampledata_version,
	uint32 *buff, FILE *fp)
{
	uint16 nbytes, tag;
	uint32 flag, *header, sync;
	uint8 *ptr;
	int err;
	wl_sampledata_t *sample_collect;
	wl_sampledata_t sample_data, *psample;

	err = wlu_iovar_getbuf(wl, "sample_collect", collect, sizeof(wl_samplecollect_args_t),
		buff, WLC_SAMPLECOLLECT_MAXLEN);

	if (err)
		return err;

	sample_collect = (wl_sampledata_t *)buff;
	header = (uint32 *)&sample_collect[1];
	tag = ltoh16_ua(&sample_collect->tag);
	if (tag != WL_SAMPLEDATA_HEADER_TYPE) {
		fprintf(stderr, "Expect SampleData Header type %d, receive type %d\n",
			WL_SAMPLEDATA_HEADER_TYPE, tag);
		return -1;
	}

	nbytes = ltoh16_ua(&sample_collect->length);
	flag = ltoh32_ua(&sample_collect->flag);
	sync = ltoh32_ua(&header[0]);
	if (sync != 0xACDC2009) {
		fprintf(stderr, "Header sync word mismatch (0x%08x)\n", sync);
		return -1;
	}

	err = fwrite((uint8 *)header, 1, nbytes, fp);
	if (err != (int)nbytes)
		  fprintf(stderr, "Failed write file-header to file %d\n", err);

	memset(&sample_data, 0, sizeof(wl_sampledata_t));
#ifdef DSLCPE_ENDIAN
	sample_data.version = htod32(sampledata_version);
#else
	sample_data.version = sampledata_version;
#endif
	sample_data.size = htol16(sizeof(wl_sampledata_t));
	flag = 0;
	/* new format, used in htphy */
	do {
		sample_data.tag = htol16(WL_SAMPLEDATA_TYPE);
		sample_data.length = htol16(WLC_SAMPLECOLLECT_MAXLEN);
		/* mask seq# */
	        sample_data.flag = htol32((flag & 0xff));

		err = wlu_iovar_getbuf(wl, "sample_data", &sample_data, sizeof(wl_sampledata_t),
			buff, WLC_SAMPLECOLLECT_MAXLEN);
		if (err) {
			fprintf(stderr, "Error reading back sample collected data\n");
			err = -1;
			break;
		}

		ptr = (uint8 *)buff + sizeof(wl_sampledata_t);
		psample = (wl_sampledata_t *)buff;
		tag = ltoh16_ua(&psample->tag);
		nbytes = ltoh16_ua(&psample->length);
		flag = ltoh32_ua(&psample->flag);
		if (tag != WL_SAMPLEDATA_TYPE) {
			fprintf(stderr, "Expect SampleData type %d, receive type %d\n",
				WL_SAMPLEDATA_TYPE, tag);
			err = -1;
			break;
		}
		if (nbytes == 0) {
			fprintf(stderr, "Done retrieving sample data\n");
			err = -1;
			break;
		}

		err = fwrite(ptr, 1, nbytes, fp);
		if (err != (int)nbytes) {
			fprintf(stderr, "Error writing %d bytes to file, rc %d!\n",
				(int)nbytes, err);
			err = -1;
			break;
		} else {
			printf("Wrote %d bytes\n", err);
			err = 0;
		}
	} while (flag & WL_SAMPLEDATA_MORE_DATA);
	return err;
}
#endif // endif
#endif /* defined(BWL_FILESYSTEM_SUPPORT) */

static int
wl_sample_collect(void *wl, cmd_t *cmd, char **argv)
{
#if !defined(BWL_FILESYSTEM_SUPPORT)
	UNUSED_PARAMETER(wl); UNUSED_PARAMETER(cmd); UNUSED_PARAMETER(argv);
	return (-1);
#elif defined(_CFE_)
	UNUSED_PARAMETER(wl); UNUSED_PARAMETER(cmd); UNUSED_PARAMETER(argv);
	return CFE_ERR_UNSUPPORTED;
#elif defined(DONGLEBUILD)
	UNUSED_PARAMETER(wl); UNUSED_PARAMETER(cmd); UNUSED_PARAMETER(argv);
	return 0;
#else
	int ret = -1;
	uint8 *buff = NULL;
	wl_samplecollect_args_t collect;
	wlc_rev_info_t revinfo;
	uint32 phytype;
	uint32 phyrev;
	const char *fname = "sample_collect.dat";
	FILE *fp = NULL;

	/* Default setting for sampledata_version */
	int sampledata_version = htol16(WL_SAMPLEDATA_T_VERSION);

	UNUSED_PARAMETER(cmd);

	memset(&revinfo, 0, sizeof(revinfo));
	if ((ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo))) < 0)
		return ret;

	phytype = dtoh32(revinfo.phytype);
	phyrev = dtoh32(revinfo.phyrev);

	/* Assign some default params first */
	/* 60us is roughly the max we can store (for NPHY with NREV < 7). */
	collect.coll_us = 60;
	collect.cores = -1;
	collect.bitStart = -1;
	/* extended settings */
	collect.trigger = TRIGGER_NOW;
#ifdef DSLCPE_ENDIAN
	collect.mode = htod16(1);
	collect.post_dur = htod32(10);
	collect.pre_dur = htod32(10);
#else
	collect.mode = 1;
	collect.post_dur = 10;
	collect.pre_dur = 10;
#endif
	collect.gpio_sel = 0;
	collect.downsamp = FALSE;
	collect.be_deaf = FALSE;
#ifdef DSLCPE_ENDIAN
	collect.timeout = htod16(1000);
#else
	collect.timeout = 1000;
#endif
	collect.agc = FALSE;
	collect.filter = FALSE;
	collect.trigger_state = 0;
	collect.module_sel1 = 2;
	collect.module_sel2 = 6;
#ifdef DSLCPE_ENDIAN
	collect.nsamps = htod16(2048);
	collect.version = htod16(WL_SAMPLECOLLECT_T_VERSION);
	collect.length = htod16(sizeof(wl_samplecollect_args_t));
#else
	collect.nsamps = 2048;
	collect.version = WL_SAMPLECOLLECT_T_VERSION;
	collect.length = sizeof(wl_samplecollect_args_t);
#endif
	/* Skip the command name */
	argv++;
	ret = -1;
	while (*argv) {
		char *s = *argv;

		if (argv[1] == NULL) {
			ret = BCME_USAGE_ERROR;
			goto exit;
		}
		if (!strcmp(s, "-f")) {
			fname = argv[1];
		} else if (!strcmp(s, "-u"))
			collect.coll_us = atoi(argv[1]);
		else if (!strcmp(s, "-c"))
#ifdef DSLCPE_ENDIAN
		        collect.cores = htod32(atoi(argv[1]));
#else
			collect.cores = atoi(argv[1]);
#endif
		/* extended settings */
		else if (!strcmp(s, "-t")) {
			/* event trigger */
			if (!strcmp(argv[1], "crs"))
				collect.trigger = TRIGGER_CRS;
			else if (!strcmp(argv[1], "crs_deassert"))
				collect.trigger = TRIGGER_CRSDEASSERT;
			else if (!strcmp(argv[1], "good_fcs"))
				collect.trigger = TRIGGER_GOODFCS;
			else if (!strcmp(argv[1], "bad_fcs"))
				collect.trigger = TRIGGER_BADFCS;
			else if (!strcmp(argv[1], "bad_plcp"))
				collect.trigger = TRIGGER_BADPLCP;
			else if (!strcmp(argv[1], "crs_glitch"))
				collect.trigger = TRIGGER_CRSGLITCH;
		}
		else if (!strcmp(s, "-m")) {
			if (!strcmp(argv[1], "gpio")) {
				if (phytype == WLC_PHY_TYPE_HT) {
					collect.mode = 4;
				} else {
					/* MIMOPHY */
					collect.mode = 0xff;
				}
			} else {
			collect.mode = atoi(argv[1]);
			}
#ifdef DSLCPE_ENDIAN
			collect.mode = htod16(collect.mode);
#endif
		}
		else if (!strcmp(s, "-k"))
#ifdef DSLCPE_ENDIAN
		        collect.gpioCapMask = htod32(atoi(argv[1]));
#else
			collect.gpioCapMask = atoi(argv[1]);
#endif
		else if (!strcmp(s, "-s"))
#ifdef DSLCPE_ENDIAN
		        collect.bitStart = htod32(atoi(argv[1]));
#else
			collect.bitStart = atoi(argv[1]);
#endif
		else if (!strcmp(s, "-b"))
#ifdef DSLCPE_ENDIAN
		        collect.pre_dur = htod32(atoi(argv[1]));
#else
			collect.pre_dur = atoi(argv[1]);
#endif
		else if (!strcmp(s, "-a"))
#ifdef DSLCPE_ENDIAN
		        collect.post_dur = htod32(atoi(argv[1]));
#else
			collect.post_dur = atoi(argv[1]);
#endif
		else if (!strcmp(s, "-g"))
			collect.gpio_sel = atoi(argv[1]);
		else if (!strcmp(s, "-d"))
			collect.downsamp = atoi(argv[1]);
		else if (!strcmp(s, "-e"))
			collect.be_deaf = atoi(argv[1]);
		else if (!strcmp(s, "-i"))
#ifdef DSLCPE_ENDIAN
		        collect.timeout = htod16(atoi(argv[1]));
#else
			collect.timeout = atoi(argv[1]);
#endif
		else if (!strcmp(s, "--agc")) {
			/* perform software agc for sample collect */
			collect.agc = atoi(argv[1]);
		}
		else if (!strcmp(s, "--filter")) {
			/* Set HPC for LPF to lowest possible value (0x1) */
			collect.filter = atoi(argv[1]);
		}
		else if (!strcmp(s, "-v"))
#ifdef DSLCPE_ENDIAN
		         sampledata_version = htod32(atoi(argv[1]));
#else
			 sampledata_version = atoi(argv[1]);
#endif
		else if (!strcmp(s, "-s"))
			collect.trigger_state = atoi(argv[1]);
		else if (!strcmp(s, "-x"))
			collect.module_sel1 = atoi(argv[1]);
		else if (!strcmp(s, "-y"))
			collect.module_sel2 = atoi(argv[1]);
		else if (!strcmp(s, "-n"))
#ifdef DSLCPE_ENDIAN
		        collect.nsamps = htod16(atoi(argv[1]));
#else
			collect.nsamps = atoi(argv[1]);
#endif
		else {
			ret = BCME_USAGE_ERROR;
			goto exit;
		}

		argv += 2;
	}

	buff = malloc(WLC_SAMPLECOLLECT_MAXLEN);
	if (buff == NULL) {
		fprintf(stderr, "Failed to allocate dump buffer of %d bytes\n",
			WLC_SAMPLECOLLECT_MAXLEN);
		return BCME_NOMEM;
	}
	memset(buff, 0, WLC_SAMPLECOLLECT_MAXLEN);

	if ((fp = fopen(fname, "wb")) == NULL) {
		fprintf(stderr, "Problem opening file %s\n", fname);
		ret = BCME_BADARG;
		goto exit;
	}

	if ((phytype == WLC_PHY_TYPE_HT) || (phytype == WLC_PHY_TYPE_AC)) {
		ret = wl_do_samplecollect(wl, &collect, sampledata_version, (uint32 *)buff, fp);
	}
	else if (phytype == WLC_PHY_TYPE_N) {
		if (phyrev < 7) {
		ret = wl_do_samplecollect_n(wl, &collect, buff, fp);
		} else {
			ret = wl_do_samplecollect(wl, &collect, sampledata_version,
				(uint32 *)buff, fp);
		}
	} else if (phytype == WLC_PHY_TYPE_LCN40) {
#ifdef DSLCPE_ENDIAN
	        if (dtoh16(collect.nsamps) > (WLC_SAMPLECOLLECT_MAXLEN >> 2)) {
#else
		if (collect.nsamps > (WLC_SAMPLECOLLECT_MAXLEN >> 2)) {
#endif
			fprintf(stderr, "Max number of samples supported = %d\n",
				WLC_SAMPLECOLLECT_MAXLEN >> 2);
			ret = -1;
			goto exit;
		}
		ret = wl_do_samplecollect_lcn40(wl, &collect, buff, fp);
	}
exit:
	if (buff) free(buff);
#ifndef ATE_BUILD
	if (fp) fclose(fp);
#endif // endif
	return ret;
#endif /* !BWL_FILESYSTEM_SUPPORT */
}

static int
wl_test_tssi(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int val;
	char* endptr = NULL;

	/* toss the command name */
	argv++;

	if (!*argv)
		return BCME_USAGE_ERROR;

	val = htod32(strtol(*argv, &endptr, 0));
	if (*endptr != '\0') {
		/* not all the value string was parsed by strtol */
		printf("set: error parsing value \"%s\" as an integer\n", *argv);
		return BCME_USAGE_ERROR;
	}

	ret = wlu_iovar_getbuf(wl, cmd->name, &val, sizeof(val),
	                      buf, WLC_IOCTL_MAXLEN);

	if (ret)
		return ret;

	val = dtoh32(*(int*)buf);

	wl_printint(val);

	return ret;
}

static int
wl_test_tssi_offs(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int val;
	char* endptr = NULL;

	/* toss the command name */
	argv++;

	if (!*argv)
		return BCME_USAGE_ERROR;

	val = htod32(strtol(*argv, &endptr, 0));
	if (*endptr != '\0') {
		/* not all the value string was parsed by strtol */
		printf("set: error parsing value \"%s\" as an integer\n", *argv);
		return BCME_USAGE_ERROR;
	}

	ret = wlu_iovar_getbuf(wl, cmd->name, &val, sizeof(val),
	                      buf, WLC_IOCTL_MAXLEN);

	if (ret)
		return ret;

	val = dtoh32(*(int*)buf);

	wl_printint(val);

	return ret;
}

static int
wl_test_idletssi(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int val;
	char* endptr = NULL;

	/* toss the command name */
	argv++;

	if (!*argv)
		return BCME_USAGE_ERROR;

	val = htod32(strtol(*argv, &endptr, 0));
	if (*endptr != '\0') {
		/* not all the value string was parsed by strtol */
		printf("set: error parsing value \"%s\" as an integer\n", *argv);
		return -1;
	}

	if ((ret = wlu_iovar_getbuf(wl, cmd->name, &val, sizeof(val),
		buf, WLC_IOCTL_MAXLEN)) >= 0) {
		val = dtoh32(*(int*)buf);
		wl_printint(val);
	}

	return ret;
}

static int
wl_phy_rssiant(void *wl, cmd_t *cmd, char **argv)
{
	uint32 antindex;
	int buflen, err;
	char *param;
	int16 antrssi;

	if (!*++argv) {
		printf(" Usage: %s antenna_index[0-3]\n", cmd->name);
		return BCME_USAGE_ERROR;
	}

	antindex = htod32(atoi(*argv));

	strcpy(buf, "nphy_rssiant");
	buflen = strlen(buf) + 1;
	param = (char *)(buf + buflen);
	memcpy(param, (char*)&antindex, sizeof(antindex));

	if ((err = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN)) < 0)
		return err;

	antindex = dtoh32(antindex);
	antrssi = dtoh16(*(int16 *)buf);
	printf("\nnphy_rssiant ant%d = %d\n", antindex, antrssi);

	return (0);
}

static int
wl_pkteng_stats(void *wl, cmd_t *cmd, char **argv)
{
	wl_pkteng_stats_t *stats;
	void *ptr = NULL;
	int err;
	uint16 *pktstats;
	int i, j;

	UNUSED_PARAMETER(argv);

	if ((err = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr)) < 0)
			return err;

	stats = ptr;
	printf("Lost frame count %d\n", dtoh32(stats->lostfrmcnt));
	printf("RSSI %d\n", dtoh32(stats->rssi));
	printf("Signal to noise ratio %d\n", dtoh32(stats->snr));
	printf("rx1mbps %d rx2mbps %d rx5mbps5 %d\n"
		"rx6mbps %d rx9mbps %d, rx11mbps %d\n"
		"rx12mbps %d rx18mbps %d rx24mbps %d\n"
		"rx36mbps %d rx48mbps %d rx54mbps %d\n",
#ifdef DSLCPE_ENDIAN
	       dtoh16(stats->rxpktcnt[3]), dtoh16(stats->rxpktcnt[1]), dtoh16(stats->rxpktcnt[2]),
	       dtoh16(stats->rxpktcnt[7]), dtoh16(stats->rxpktcnt[11]), dtoh16(stats->rxpktcnt[0]),
	       dtoh16(stats->rxpktcnt[6]), dtoh16(stats->rxpktcnt[10]), dtoh16(stats->rxpktcnt[5]),
	       dtoh16(stats->rxpktcnt[9]), dtoh16(stats->rxpktcnt[4]), dtoh16(stats->rxpktcnt[8]));
#else
		stats->rxpktcnt[3], stats->rxpktcnt[1], stats->rxpktcnt[2],
		stats->rxpktcnt[7], stats->rxpktcnt[11], stats->rxpktcnt[0],
		stats->rxpktcnt[6], stats->rxpktcnt[10], stats->rxpktcnt[5],
		stats->rxpktcnt[9], stats->rxpktcnt[4], stats->rxpktcnt[8]);
#endif
	pktstats = &stats->rxpktcnt[NUM_80211b_RATES+NUM_80211ag_RATES];
	for (i = 0; i < NUM_80211n_RATES/4; i++) {
		for (j = 0; j < 4; j++) {
#ifdef DSLCPE_ENDIAN
		        printf("rxmcs%d %d ", j+4*i, dtoh16(pktstats[j+4*i]));
#else
			printf("rxmcs%d %d ", j+4*i, pktstats[j+4*i]);
#endif
		}
		printf("\n");
	}
#ifdef DSLCPE_ENDIAN
        printf("rxmcsother %d\n", dtoh16(stats->rxpktcnt[NUM_80211_RATES]));
#else
	printf("rxmcsother %d\n", stats->rxpktcnt[NUM_80211_RATES]);
#endif
	return 0;
}

#define LPPHY_PAPD_EPS_TBL_SIZE 64
static int
wl_phy_papdepstbl(void *wl, cmd_t *cmd, char **argv)
{
	int32 eps_real, eps_imag;
	int i;
	uint32 eps_tbl[LPPHY_PAPD_EPS_TBL_SIZE];
	int err;

	UNUSED_PARAMETER(argv);

	if ((err = wlu_iovar_get(wl, cmd->name, &eps_tbl, sizeof(eps_tbl))) < 0)
		return err;

	for (i = 0; i < LPPHY_PAPD_EPS_TBL_SIZE; i++) {
#ifdef DSLCPE_ENDIAN
	        eps_tbl[i] = dtoh32(eps_tbl[i]);
#endif
		if ((eps_real = (int32)(eps_tbl[i] >> 12)) > 0x7ff)
				eps_real -= 0x1000; /* Sign extend */
		if ((eps_imag = (int32)(eps_tbl[i] & 0xfff)) > 0x7ff)
				eps_imag -= 0x1000; /* Sign extend */
		printf("%d %d\n", eps_real, eps_imag);
	}

	return 0;
}

static int
wl_phy_txiqcc(void *wl, cmd_t *cmd, char **argv)
{
	int i;
	int err;
	int32 iqccValues[4];
	int32 value;
	char *endptr;
	int32 a, b, a1, b1;
	wlc_rev_info_t revinfo;
	uint32 phytype;

	memset(&revinfo, 0, sizeof(revinfo));
	if ((err = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo))) < 0)
		return err;

	phytype = dtoh32(revinfo.phytype);

	if (phytype != WLC_PHY_TYPE_N) {
		if (!*++argv) {
			if ((err = wlu_iovar_get(wl, cmd->name, iqccValues, 2*sizeof(int32))) < 0)
				return err;
#ifdef DSLCPE_ENDIAN
			iqccValues[0] = dtoh32(iqccValues[0]);
			iqccValues[1] = dtoh32(iqccValues[1]);
#endif
			a = (int16)iqccValues[0];
			b = (int16)iqccValues[1];
			/* sign extend a, b from 10 bit signed value to 32 bit signed value */
			a = ((a << 22) >> 22);
			b = ((b << 22) >> 22);
			printf("%d  %d\n", a, b);
		}
		else
		{
			for (i = 0; i < 2; i++) {
				value = strtol(*argv++, &endptr, 0);
				if (value > 511 || value < -512) {
					return BCME_BADARG;
				}
#ifdef DSLCPE_ENDIAN
				iqccValues[i] = htod32(value);
#else
				iqccValues[i] = value;
#endif
			}

		if ((err = wlu_var_setbuf(wl, cmd->name, iqccValues, 2*sizeof(int32))) < 0)
			return err;
		}
	} else {
		if (!*++argv) {
			if ((err = wlu_iovar_get(wl, cmd->name, iqccValues, 4*sizeof(int32))) < 0)
				return err;
#ifdef DSLCPE_ENDIAN
			iqccValues[0] = dtoh32(iqccValues[0]);
			iqccValues[1] = dtoh32(iqccValues[1]);
			iqccValues[2] = dtoh32(iqccValues[2]);
			iqccValues[3] = dtoh32(iqccValues[3]);
#endif
			a = (int16)iqccValues[0];
			b = (int16)iqccValues[1];
			a1 = (int16)iqccValues[2];
			b1 = (int16)iqccValues[3];
			/* sign extend a, b from 10 bit signed value to 32 bit signed value */
			a = ((a << 22) >> 22);
			b = ((b << 22) >> 22);
			a1 = ((a1 << 22) >> 22);
			b1 = ((b1 << 22) >> 22);
			printf("%d  %d  %d  %d\n", a, b, a1, b1);
		}
		else
		{
			for (i = 0; i < 4; i++) {
				value = strtol(*argv++, &endptr, 0);
				if (value > 511 || value < -512) {
					return BCME_BADARG;
				}
#ifdef DSLCPE_ENDIAN
				iqccValues[i] = htod32(value);
#else
				iqccValues[i] = value;
#endif
			}

			if ((err = wlu_var_setbuf(wl, cmd->name, iqccValues, 4*sizeof(int32))) < 0)
				return err;
		}
	}

	return 0;
}

static int
wl_phy_txlocc(void *wl, cmd_t *cmd, char **argv)
{
	int i;
	int err;
	int8 loccValues[12];
	int32 value;
	char *endptr;
	wlc_rev_info_t revinfo;
	uint32 phytype;

	memset(&revinfo, 0, sizeof(revinfo));
	if ((err = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo))) < 0)
		return err;

	phytype = dtoh32(revinfo.phytype);

	if (phytype != WLC_PHY_TYPE_N) {
		if (!*++argv) {
			if ((err = wlu_iovar_get(wl, cmd->name, loccValues,
				sizeof(loccValues))) < 0)
				return err;

			/* sign extend the loccValues */
			loccValues[2] = (loccValues[2] << 3) >> 3;
			loccValues[3] = (loccValues[3] << 3) >> 3;
			loccValues[4] = (loccValues[4] << 3) >> 3;
			loccValues[5] = (loccValues[5] << 3) >> 3;

			printf("%d  %d  %d  %d  %d  %d\n", loccValues[0],
				loccValues[1], loccValues[2], loccValues[3],
				loccValues[4], loccValues[5]);
		}
		else
		{
			for (i = 0; i < 6; i++) {
				value = strtol(*argv++, &endptr, 0);
				if ((i >= 2) && (value > 15 || value < -15)) {
					return BCME_BADARG;
				}
				loccValues[i] = (int8)value;
			}

			if ((err = wlu_var_setbuf(wl, cmd->name, loccValues, 6*sizeof(int8))) < 0)
				return err;
		}
	} else {
		if (!*++argv) {
			if ((err = wlu_iovar_get(wl, cmd->name, loccValues,
				sizeof(loccValues))) < 0)
				return err;

			/* sign extend the loccValues */
			loccValues[2] = (loccValues[2] << 3) >> 3;
			loccValues[3] = (loccValues[3] << 3) >> 3;
			loccValues[4] = (loccValues[4] << 3) >> 3;
			loccValues[5] = (loccValues[5] << 3) >> 3;
			loccValues[8] = (loccValues[8] << 3) >> 3;
			loccValues[9] = (loccValues[9] << 3) >> 3;
			loccValues[10] = (loccValues[10] << 3) >> 3;
			loccValues[11] = (loccValues[11] << 3) >> 3;

			printf("%d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d\n", loccValues[0],
				loccValues[1], loccValues[2], loccValues[3], loccValues[4],
				loccValues[5], loccValues[6], loccValues[7], loccValues[8],
				loccValues[9], loccValues[10], loccValues[11]);
		}
		else
		{
			for (i = 0; i < 12; i++) {
				value = strtol(*argv++, &endptr, 0);
				if (((i < 2) && (value > 63 || value < -64)) ||
					((i >= 2) && (value > 15 || value < -15))) {
					return BCME_BADARG;
				}
				loccValues[i] = (int8)value;
			}

			if ((err = wlu_var_setbuf(wl, cmd->name, loccValues, 12*sizeof(int8))) < 0)
				return err;
		}
	}

	return 0;
}

static int
wl_rssi_cal_freq_grp_2g(void *wl, cmd_t *cmd, char **argv)
{
	int i;
	int err = -1;
	uint8 nvramValues[14];
	char *endptr;
	int ret = -1;
	wlc_rev_info_t revinfo;

	uint8 N = 0;

	memset(&revinfo, 0, sizeof(revinfo));
	ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo));
	if (ret) {
		return ret;
	}

	if (!*++argv) {
		/* Reading the NVRAM variable */
		if ((err = wlu_iovar_get(wl, cmd->name, nvramValues, sizeof(nvramValues))) < 0)
			return err;

		N = 14; /* 14 corresponds to number of channels in 2g */

		for (i = 0; i < N-2; i++) {
			printf("0x%x%x,", nvramValues[i], nvramValues[i+1]);
			i++;
		}
		printf("0x%x%x\n", nvramValues[i], nvramValues[i+1]);
	} else {
		/* Writing to NVRAM variable */

		char *splt;
		int8 tmp;
		splt = strtok(*argv, ",");

		/* N = 14 corresponds to number of channels in 2g */
		/* N = N /2 to package 2 channel's nibbles into 1 byte */
		N = 7;

		i = 0;
		while (splt != NULL) {
			/* Splitting the input based on charecter ','
			 * Further each byte is divided into 2 nibbles
			 * and saved into 2 elements of array.
			 */
			tmp = strtol(splt, &endptr, 0);
			nvramValues[i] = (tmp >> 4) & 0xf;
			i++;
			nvramValues[i] = tmp & 0xf;
			splt = strtok(NULL, ",");
			i++;
		}
		if (i != 14) {
			printf("Insufficient arguments \n");
			return BCME_BADARG;
		}
		if ((err = wlu_var_setbuf(wl, cmd->name, nvramValues, N*2*sizeof(int8))) < 0)
			return err;
	}

	return 0;
}

static int
wl_phy_rssi_gain_delta_2g_sub(void *wl, cmd_t *cmd, char **argv)
{
	int i;
	int err = -1;
	int8 deltaValues[28];
	int32 value;
	char *endptr;
	int ret = -1;
	wlc_rev_info_t revinfo;
	uint32 phytype;
	uint8 N = 0;

	memset(&revinfo, 0, sizeof(revinfo));
	ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo));
	if (ret) {
		return ret;
	}
	phytype = dtoh32(revinfo.phytype);

	if (phytype != WLC_PHY_TYPE_AC) {
		return err;
	}

	if (!*++argv) {
		if ((err = wlu_iovar_get(wl, cmd->name, deltaValues, sizeof(deltaValues))) < 0)
			return err;
		N = 18; /* 9 entries per core, 4350 - 18 MAX entries; 4345 9 MAX entries */
		for (i = 0; i < N; i++) {
			if (i%9 == 0 && i > 0) {
				printf("\n");
				if (deltaValues[i] == -1) break;
			}

				printf("%d ", deltaValues[i]);
		}
		if (i == N)
			printf("\n");
	} else {
		int argc = 0;
		int index = 0;
		while (argv[argc])
			argc++;

		/* ACPHY : 8/9 entries for a core; core 0 delta's can be
		 * given with or with out core_num as first element
		 */
		N = argc;

		for (i = 0; i < N; i++) {
			value = strtol(*argv++, &endptr, 0);
			if ((value > 63 || value < -64)) {
				return BCME_BADARG;
			}
			if (argc == 9) {
				/* If number of arguments is 9, then core
				 * number has been provided.
				 * And 8 elements related to 2
				 * (BWs - 20 n 40) and 4 gain settings
				 * (elna_on, elna_off, rout_1, rout_2) are
				 * provided. So, 2 * 4 = 8 + 1 core_num = 9
				 */
				deltaValues[i] = (int8)value;
			} else {
				/* If the number of elements is not eq to 9,
				 * then, core number was not provided.
				 * So, if only 8 elements are provided, only
				 * core 0's info is given. So, for i = 0,
				 * deltaValues element is 0 (core_num). If 16
				 * elements are provided, then core 0 and 1's info is
				 * provided. So, i =0 element has core_num = 0,
				 * then, next 8 elements are core 0's
				 * deltas. For i = 8, core 1's core_num = 1
				 * is inserted into deltaValues array.
				 * Similarly for third core data.
				 */
				if (i == 0) {
					deltaValues[index] = 0;
					index++;
				} else if (i == 8) {
					deltaValues[index] = 1;
					index++;
				} else if (i == 16) {
					deltaValues[index] = 2;
					index++;
				}
				deltaValues[index] = (int8)value;
				index++;
			}
		}
		/* If argc == 8, then only 1 core's info was given,
		 * so, setbuf() is called once.
		 * If argc == 16 then core 0 and 1's info was given.
		 * So, setbuf() is called twice.
		 * If argc == 24 then core 0, 1 and 2's info was given.
		 * So, setbuf() is called thrice.
		 */
		if ((err = wlu_var_setbuf(wl, cmd->name,
		     deltaValues, 9*sizeof(int8))) < 0)
			return err;
		if (argc >= 16) {
			if ((err = wlu_var_setbuf(wl, cmd->name,
			      deltaValues + 9, 9*sizeof(int8))) < 0)
				return err;
		}
		if (argc == 24) {
			if ((err = wlu_var_setbuf(wl, cmd->name,
			     deltaValues + 18, 9*sizeof(int8))) < 0)
				return err;
		}
	}

	return 0;
}

static int
wl_phy_rssi_gain_delta_2g(void *wl, cmd_t *cmd, char **argv)
{
	int i;
	int err = -1;
	int8 deltaValues[18];
	int32 value;
	char *endptr;
	int ret = -1;
	wlc_rev_info_t revinfo;
	uint32 phytype;
	uint8 N = 0;

	memset(&revinfo, 0, sizeof(revinfo));
	ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo));
	if (ret) {
		return ret;
	}
	phytype = dtoh32(revinfo.phytype);

	if (phytype != WLC_PHY_TYPE_AC) {
		return err;
	}

	if (!*++argv) {
		if ((err = wlu_iovar_get(wl, cmd->name, deltaValues, sizeof(deltaValues))) < 0)
			return err;
		if (phytype == WLC_PHY_TYPE_AC)
			N = 15; /* ACPHY: 3 cores max x 5 entries */
		for (i = 0; i < N; i++) {
			if ((phytype == WLC_PHY_TYPE_AC) && (i%5 == 0)) {
				if (i > 0) printf("\n");
				if (deltaValues[i] == -1) break;
			}
			printf("%d ", deltaValues[i]);
		}
		if (i == N)
			printf("\n");
	} else {
		int argc = 0;
		int index = 0;
		while (argv[argc])
			argc++;
		if (phytype == WLC_PHY_TYPE_AC) {
			/* N = 5;  ACPHY : 5 entries for a core */
			N = argc;
		}

		for (i = 0; i < N; i++) {
			value = strtol(*argv++, &endptr, 0);
			if ((value > 63 || value < -64)) {
				return BCME_BADARG;
			}
			if (argc == 5) {
				/* If number of arguments is 5, then core number has been provided.
				 * And 8 elements related to 2 (BWs - 20 n 40) and 2 gain settings
				 * (elna_on, elna_off) are provided. So, 2 * 2 = 4 + 1 core_num = 5
				 */
				deltaValues[i] = (int8)value;
			} else {
				/* If the number of elements is not eq to 5,
				 * then, core number was not provided.
				 * So, if only 4 elements are provided, only
				 * core 0's info is given. So, for i = 0,
				 * deltaValues element is 0 (core_num). If 8
				 * elements are provided, then core 0 and 1's info is
				 * provided. So, i =0 element has core_num = 0,
				 * then, next 4 elements are core 0's
				 * deltas. For i = 4, core 1's core_num = 1
				 * is inserted into deltaValues array.
				 * Similarly for third core data.
				 */
				if (i == 0) {
					deltaValues[index] = 0;
					index++;
				} else if (i == 4) {
					deltaValues[index] = 1;
					index++;
				} else if (i == 8) {
					deltaValues[index] = 2;
					index++;
				}
				deltaValues[index] = (int8)value;
				index++;
			}

		}
		/* If argc == 4, then only 1 core's info was given,
		 * so, setbuf() is called once.
		 * If argc == 8 then core 0 and 1's info was given.
		 * So, setbuf() is called twice.
		 * If argc == 12 then core 0, 1 and 2's info was given.
		 * So, setbuf() is called thrice.
		 */
		if ((err = wlu_var_setbuf(wl, cmd->name,
		     deltaValues, 5*sizeof(int8))) < 0)
			return err;
		if (argc >= 8) {
			if ((err = wlu_var_setbuf(wl, cmd->name,
			     deltaValues + 5, 5*sizeof(int8))) < 0)
				return err;
		}
		if (argc == 12) {
			if ((err = wlu_var_setbuf(wl, cmd->name,
			     deltaValues + 10, 5*sizeof(int8))) < 0)
				return err;
		}

	}

	return 0;
}

static int
wl_phy_rssi_gain_delta_5g(void *wl, cmd_t *cmd, char **argv)
{
	int i;
	int err = -1;
	int8 deltaValues[28];
	int32 value = 0;
	char *endptr;
	int ret = -1;
	wlc_rev_info_t revinfo;
	uint32 phytype;
	uint8 N = 0, n_per_core, n_per_core_p1;

	char *varname = "rssi_cal_rev";
	const char *iovar = "nvram_get";
	void *p;

	err = wlu_var_getbuf(wl, iovar, varname, strlen(varname) + 1, &p);
	if (err == 0) {
		/* This means, NVRAM variable found. */
		/* Calls new function for accommodating ROuts */
		value = strtol(buf, &endptr, 0);
	}

	if ((err < 0) || ((err == 0) && (value == 0))) {
		/* This means, NVRAM variable not found or Variable is 0 */
		/* Calls old function */
		n_per_core = 6;
	} else {
		n_per_core = 12;
	}

	n_per_core_p1 = n_per_core  + 1;
	memset(&revinfo, 0, sizeof(revinfo));
	ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo));
	if (ret) {
		return ret;
	}
	phytype = dtoh32(revinfo.phytype);

	if (phytype != WLC_PHY_TYPE_AC) {
		return err;
	}

	if (!*++argv) {
		if ((err = wlu_iovar_get(wl, cmd->name, deltaValues, sizeof(deltaValues))) < 0)
			return err;
		if (phytype == WLC_PHY_TYPE_AC)
			N = n_per_core_p1 * 3; /* ACPHY: 3 cores max x 7 entries */
		for (i = 0; i < N; i++) {
			if ((phytype == WLC_PHY_TYPE_AC) && (i%n_per_core_p1 == 0)) {
				if (i > 0) printf("\n");
				if (deltaValues[i] == -1) break;
			}
			printf("%d ", deltaValues[i]);
		}
		if (i == N)
			printf("\n");
	} else {
		int argc = 0;
		int index = 0;
		while (argv[argc])
			argc++;

		if (phytype == WLC_PHY_TYPE_AC) {
			/* N = 7; ACPHY : 7 entries for a core */
			N = argc;
		}

		for (i = 0; i < N; i++) {
			value = strtol(*argv++, &endptr, 0);
			if ((value > 63 || value < -64)) {
				return BCME_BADARG;
			}
			if (argc == n_per_core_p1) {
				/* For Old implementation, ie, no Routs, n_per_core_p1 == 5
				 * for New implementation, ie, no Routs, n_per_core_p1 == 9
				 * If number of arguments is "n_per_core_p1",
				 * then core number has been provided.
				 */
				deltaValues[i] = (int8)value;
			} else  {
				/* If the number of elements is not eq to
				 *"n_per_core", then, core number was not provided.
				 * So, if only "n_per_core" elements are provided,
				 * only core 0's info is given. So, for i = 0,
				 * deltaValues element is 0 (core_num). If "n_per_core * 2"
				 * elements are provided, then core 0 and 1's info is
				 * provided. So, i =0 element has core_num = 0, then,
				 * next "n_per_core" elements are core 0's
				 * deltas. For i = "n_per_core", core 1's
				 * core_num = 1 is inserted into deltaValues array.
				 * Similarly for third core data.
				 */

				if (i == (n_per_core * 0)) {
					deltaValues[index] = 0;
					index++;
				}
				if (i == (n_per_core * 1)) {
					deltaValues[index] = 1;
					index++;
				}
				if (i == (n_per_core * 2)) {
					deltaValues[index] = 2;
					index++;
				}

				deltaValues[index] = (int8)value;
				index++;
			}

		}
		/* If argc == "n_per_core", then only 1 core's infoxs
		 * was given, so, setbuf() is called once.
		 * If argc == "n_per_core * 2" then core 0 and 1's info
		 * was given. So, setbuf() is called twice.
		 * If argc == "n_per_core * 3" then core 0, 1 and 2's
		 * info was given. So, setbuf() is called thrice.
		 */
		if ((err = wlu_var_setbuf(wl, cmd->name, deltaValues,
		      n_per_core_p1*sizeof(int8))) < 0)
			return err;
		if (argc >= (n_per_core * 2)) {
			if ((err = wlu_var_setbuf(wl, cmd->name, deltaValues +
			     (n_per_core_p1 * 1), n_per_core_p1*sizeof(int8))) < 0)
				return err;
		}
		if (argc == (n_per_core * 3)) {
			if ((err = wlu_var_setbuf(wl, cmd->name, deltaValues +
			     (n_per_core_p1 * 2), n_per_core_p1*sizeof(int8))) < 0)
				return err;
		}

	}
	return 0;
}


static int
wl_phy_rxgainerr(void *wl, cmd_t *cmd, char **argv)
{
	int i;
	int err = -1;
	int8 deltaValues[MAX_STREAMS_SUPPORTED];
	int32 value;
	char *endptr;
	int ret = -1;
	wlc_rev_info_t revinfo;
	uint32 phytype;
	uint8 N = 0;
	uint8 hw_rxchain;

	memset(&revinfo, 0, sizeof(revinfo));
	ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo));
	if (ret) {
		return ret;
	}
	phytype = dtoh32(revinfo.phytype);

	if (phytype != WLC_PHY_TYPE_AC) {
		return err;
	}

	ret = wlu_iovar_get(wl, "hw_rxchain", &hw_rxchain, sizeof(hw_rxchain));
	if (ret)
		return ret;

	for (i = 0; i < MAX_STREAMS_SUPPORTED; i++) {
		if (hw_rxchain & 0x1)
			N++;
		else
			break;
		hw_rxchain = hw_rxchain >> 1;
	}

	if (!*++argv) {
		if ((err = wlu_iovar_get(wl, cmd->name, deltaValues, sizeof(deltaValues))) < 0)
			return err;
		for (i = 0; i < N; i++) {
			printf("%d ", deltaValues[i]);
		}
		if (i == N)
			printf("\n");
	} else {
		int argc = 0;
		while (argv[argc])
			argc++;
		if (argc != N) {
			printf("IOVAR works only for %d cores scenario.\n", N);
			return err;
		}
		for (i = 0; i < N; i++) {
			value = strtol(*argv++, &endptr, 0);
			if ((value > 63 || value < -64)) {
				return BCME_BADARG;
			}
			deltaValues[i] = (int8)value;
		}
		if ((err = wlu_var_setbuf(wl, cmd->name, deltaValues, N*sizeof(int8))) < 0)
			return err;
	}

	return 0;
}


static int
wl_phytable(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	uint32 tableInfo[5];
	char *endptr;
	void *ptr = NULL;
	int32 tableId, tableOffset, tableWidth;
	uint64 tableElement;

	if (*++argv != NULL)
		tableId = strtol(*argv, &endptr, 0);
	else
		return BCME_USAGE_ERROR;

	if (*++argv != NULL)
		tableOffset = strtol(*argv, &endptr, 0);
	else
		return BCME_USAGE_ERROR;

	if (*++argv != NULL)
		tableWidth = strtol(*argv, &endptr, 0);
	else
		return BCME_USAGE_ERROR;

	if ((tableId < 0) || (tableOffset < 0))
		return BCME_BADARG;

	if ((tableWidth != 8) && (tableWidth != 16) && (tableWidth != 32) &&
		(tableWidth != 48) && (tableWidth != 64))
		return BCME_BADARG;

	if (!*++argv) {  /* wl utility reads a PHY table element */
#ifdef DSLCPE_ENDIAN
	        tableInfo[0] = htod32(tableId);
		tableInfo[1] = htod32(tableOffset);
		tableInfo[2] = htod32(tableWidth);
#else
		tableInfo[0] = tableId;
		tableInfo[1] = tableOffset;
		tableInfo[2] = tableWidth;
#endif
		if ((err = wlu_var_getbuf(wl, cmd->name, tableInfo, 4*sizeof(int32), &ptr)) < 0)
			return err;

		tableElement = ((uint64*)ptr)[0]; /* ptr is guaranteed to be 64 bits aligned */

		/* Mask out the correct data */
		if (tableWidth == 8)
			tableElement &= 0xFF;
		else if (tableWidth == 16)
			tableElement &= 0xFFFF;
		else if (tableWidth == 32)
			tableElement &= 0xFFFFFFFF;
		else if (tableWidth == 48)
			tableElement &= 0xFFFFFFFFFFFFULL;
#ifdef DSLCPE_ENDIAN
		printf("0x%llx(%lld)\n", dtoh64(tableElement), dtoh64(tableElement));
#else
		printf("0x%llx(%lld)\n", tableElement, tableElement);
#endif
	} else { /* wl utility writes a PHY table element */
		tableElement = bcm_strtoull(*argv++, &endptr, 0);
#ifdef DSLCPE_ENDIAN
		tableInfo[0] = htod32(tableId);
		tableInfo[1] = htod32(tableOffset);
		tableInfo[2] = htod32(tableWidth);
#else
		tableInfo[0] = tableId;
		tableInfo[1] = tableOffset;
		tableInfo[2] = tableWidth;
#endif
		htol64_ua_store(tableElement, &tableInfo[3]);

		if ((err = wlu_var_setbuf(wl, cmd->name, tableInfo, 5 * sizeof(int32))) < 0)
			return err;
	}

	return 0;
}

static int
wl_phy_force_crsmin(void *wl, cmd_t *cmd, char **argv)
{
	int err = -1;
	int8 th[4] = { 0 };
	int32 value;
	uint argc = 0;
	char *endptr;
	uint8 i = 0;
	int ret = -1;
	wlc_rev_info_t revinfo;
	uint32 phytype;
	memset(&revinfo, 0, sizeof(revinfo));
	ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo));
	if (ret) {
		return ret;
	}
	phytype = dtoh32(revinfo.phytype);

	if (phytype != WLC_PHY_TYPE_AC) {
		return err;
	}

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	if (argc == 0) {
		/* No get for now */
		return err;
	} else {
		if (argc > 3) {
			printf("IOVAR works only for up to 3 cores. \n");
			return err;
		}
		for (i = 0; i < argc; i++) {
			value = strtol(argv[i + 1], &endptr, 0);
			if ((i == 0) && (value < -1)) {
				/* Offset values (2nd/3rd arguments) can be negative */
				return BCME_BADARG;
			}
			th[i] = (int8) value;
		}
		if ((err = wlu_var_setbuf(wl, cmd->name, th, 4*sizeof(int8))) < 0)
			return err;
	}

	return 0;
}

static int
wl_phy_txpwrindex(void *wl, cmd_t *cmd, char **argv)
{
	uint i;
	int ret;
	uint32 txpwridx[4] = { 0 };
	int8 idx[4] = { 0 };
	uint argc;
	char *endptr;

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	for (i = 0; i < 4; i++) {
		if (argc > i) {
			txpwridx[i] = strtol(argv[1 + i], &endptr, 0);
			if (*endptr != '\0') {
				printf("error\n");
				return BCME_USAGE_ERROR;
			}
		}
	}

	if (argc == 0) {
		if ((ret = wlu_iovar_getint(wl, cmd->name, (int*)&txpwridx[0])) < 0) {
			return (ret);
		}
#ifndef DSLCPE_ENDIAN
		txpwridx[0] = dtoh32(txpwridx[0]);
#endif
		idx[0] = (int8)(txpwridx[0] & 0xff);
		idx[1] = (int8)((txpwridx[0] >> 8) & 0xff);
		idx[2] = (int8)((txpwridx[0] >> 16) & 0xff);
		idx[3] = (int8)((txpwridx[0] >> 24) & 0xff);
		printf("txpwrindex for core{0...3}: %d %d %d %d\n", idx[0], idx[1],
		       idx[2], idx[3]);
	} else {

		wlc_rev_info_t revinfo;
		uint32 phytype;

		memset(&revinfo, 0, sizeof(revinfo));
		if ((ret = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo))) < 0)
			return ret;

		phytype = dtoh32(revinfo.phytype);

#ifdef DSLCPE_ENDIAN
		txpwridx[0] = htod32(txpwridx[0]);
		txpwridx[1] = htod32(txpwridx[1]);
		txpwridx[2] = htod32(txpwridx[2]);
		txpwridx[3] = htod32(txpwridx[3]);
#endif

		if (phytype == WLC_PHY_TYPE_HT) {
			if (argc != 3) {
				printf("HTPHY must specify 3 core txpwrindex\n");
				return BCME_USAGE_ERROR;
			}
		} else if (phytype == WLC_PHY_TYPE_N) {
			if (argc != 2) {
				printf("NPHY must specify 2 core txpwrindex\n");
				return BCME_USAGE_ERROR;
			}
		}

		ret = wlu_iovar_setbuf(wl, cmd->name, txpwridx, 4*sizeof(uint32),
			buf, WLC_IOCTL_MAXLEN);
	}

	return ret;
}

static int
wl_phy_force_vsdb_chans(void *wl, cmd_t *cmd, char **argv)
{
	uint16 *chans = NULL;
	int ret = 0;
	void	*ptr;

	UNUSED_PARAMETER(wl); UNUSED_PARAMETER(cmd);

	if (argv[1] == NULL) {
		if ((ret = wlu_var_getbuf(wl, "force_vsdb_chans", NULL, 0, &ptr) < 0)) {
				printf("wl_phy_maxpower: fail to get maxpower\n");
				return ret;
		}
		chans = (uint16*)ptr;
#ifdef DSLCPE_ENDIAN
		printf("Chans : %x %x \n", dtoh16(chans[0]), dtoh16(chans[1]));
#else
		printf("Chans : %x %x \n", chans[0], chans[1]);
#endif
	} else if (argv[1] && argv[2]) {
		/* Allocate memory */
		chans = (uint16*)malloc(2 * sizeof(uint16));
		if (chans == NULL) {
			printf("unable to allocate Memory \n");
			return BCME_NOMEM;
		}
		chans[0]  = wf_chspec_aton(argv[1]);
		chans[1]  = wf_chspec_aton(argv[2]);
		if (((chans[0] & 0xff) == 0) || ((chans[1] & 0xff) == 0)) {
			chans[0] = 0;
			chans[1] = 0;
		}
#ifdef DSLCPE_ENDIAN
		chans[0] = htod16(chans[0]);
		chans[1] = htod16(chans[1]);
#endif
		ret = wlu_iovar_setbuf(wl, cmd->name, chans, 2 * sizeof(uint16),
			buf, WLC_IOCTL_MAXLEN);
		if (chans)
			free(chans);
	} else {
		ret = BCME_USAGE_ERROR;
	}

	return ret;
}

static int
wl_phy_pavars(void *wl, cmd_t *cmd, char **argv)
{
#if	defined(_CFE_)
		return CFE_ERR_UNSUPPORTED;
#elif	defined(DONGLEBUILD)
		return 0;
#else
		const pavars_t *pav = pavars;
		uint16	inpa[WL_PHY_PAVARS_LEN];
		char	*cpar = NULL, *p = NULL;
		char	*par;
		char	delimit[2] = " \0";
		int err = 0;
		unsigned int val, val2[SROM_PAVAR];
		void	*ptr = NULL;
		int paparambwver = 0;
		int sromrev = 0;

		const char *iovar = "nvram_dump";
		void *p1 = NULL;

		if ((err = wlu_var_getbuf(wl, iovar, NULL, 0, &p1)) < 0) {
			if ((err = wlu_get(wl, WLC_NVRAM_DUMP, &buf[0], WLC_IOCTL_MAXLEN)) < 0)
				return err;
			p1 = (void *)buf;
		}
		if ((p1 = strstr(p1, "paparambwver"))) {
			char *q = NULL;
			p1 = (void*)((char*)p1 + 13);
			paparambwver = strtoul(p1, &q, 10);
		}

		err = wlu_iovar_getint(wl, "sromrev", &sromrev);
		if (paparambwver == 1)
			pav = pavars_bwver_1;
		else if (paparambwver == 2)
			pav = pavars_bwver_2;
		else if (paparambwver == 3)
			pav = pavars_bwver_3;
		else {
		  if (sromrev > 12)
			pav = pavars_SROM13;
		  if (sromrev == 12)
			pav = pavars_SROM12;
		  if (sromrev < 12)
			pav = pavars;
		}
		if (*++argv) {	/* set */
			while (pav->phy_type != PHY_TYPE_NULL) {
				bool found = FALSE;
				int i = 0;
#ifdef DSLCPE_ENDIAN	
				inpa[i++] = htod16(pav->phy_type);
				inpa[i++] = htod16(pav->bandrange);
				inpa[i++] = htod16(pav->chain);
#else
				inpa[i++] = pav->phy_type;
				inpa[i++] = pav->bandrange;
				inpa[i++] = pav->chain;
#endif
				par = malloc(strlen(pav->vars)+1);
				if (!par)
					return BCME_NOMEM;

				strcpy(par, pav->vars);

				cpar = strtok (par, delimit);	/* current param */

				if ((pav->phy_type == PHY_TYPE_AC) ||
						(pav->phy_type == PHY_TYPE_LCN20)) {
				  int pnum = 0, n;
				  if (sromrev >= 12) {
					if ((pav->bandrange == WL_CHAN_FREQ_RANGE_2G) ||
					   (pav->bandrange == WL_CHAN_FREQ_RANGE_2G_40))
					  pnum = 4;
					else if ((pav->bandrange ==
						WL_CHAN_FREQ_RANGE_5G_5BAND) ||
						(pav->bandrange ==
						WL_CHAN_FREQ_RANGE_5G_5BAND_40) ||
						(pav->bandrange ==
						WL_CHAN_FREQ_RANGE_5G_5BAND_80))
							pnum = 20;
				  }
				  if (sromrev < 12) {
					if (pav->bandrange == WL_CHAN_FREQ_RANGE_2G)
					  pnum = 3;
					else if (pav->bandrange == WL_CHAN_FREQ_RANGE_5G_4BAND)
					  pnum = 12;
				  }
				  if (cpar) {
					/* Find the parameter in the input argument list */
					if ((p = find_pattern2(argv, cpar, val2, pnum))) {
						found = TRUE;
						for (n = 0; n < pnum; n ++)
#ifdef DSLCPE_ENDIAN
							inpa[i + n] = htod16((uint16)val2[n]);
#else
							inpa[i + n] = (uint16)val2[n];
#endif
					}
				  }
				} else {
					do {
						val = 0;
						if (cpar == NULL) {
							inpa[i] = val;
							break;
						}
						/* Find the parameter in the input argument list */
						if ((p = find_pattern(argv, cpar, &val))) {
							found = TRUE;
#ifdef DSLCPE_ENDIAN
							inpa[i] = htod16((uint16)val);
#else
							inpa[i] = (uint16)val;
#endif
						} else
							inpa[i] = 0;
						i++;
					} while ((cpar = strtok (NULL, delimit)) != NULL);
				}
				free(par);

				if (found) {
					if ((err = wlu_var_setbuf(wl, cmd->name, inpa,
						WL_PHY_PAVARS_LEN * sizeof(uint16))) < 0) {
						printf("wl_phy_pavars: fail to set\n");
						return err;
					}
				}
				pav++;
			}
		} else {	/* get */
			while (pav->phy_type != PHY_TYPE_NULL) {
				int i = 0;
				uint16	*outpa;
#ifdef DSLCPE_ENDIAN
				inpa[i++] = htod16(pav->phy_type);
				inpa[i++] = htod16(pav->bandrange);
				inpa[i++] = htod16(pav->chain);
#else
				inpa[i++] = pav->phy_type;
				inpa[i++] = pav->bandrange;
				inpa[i++] = pav->chain;
#endif
				par = malloc(strlen(pav->vars)+1);
				if (!par)
					return BCME_NOMEM;
				strcpy(par, pav->vars);
				if ((err = wlu_var_getbuf_sm(wl, cmd->name, inpa,
					WL_PHY_PAVARS_LEN * sizeof(uint16), &ptr)) < 0) {
					printf("phy %x band %x chain %d err %d\n", pav->phy_type,
						pav->chain, pav->bandrange, err);
					free(par);
					break;
				}

				outpa = (uint16*)ptr;
#ifdef DSLCPE_ENDIAN
			if (dtoh16(outpa[0]) == PHY_TYPE_NULL) {
#else
				if (outpa[0] == PHY_TYPE_NULL) {
#endif
					pav++;
					free(par);
					continue;
				}

				cpar = strtok(par, delimit);	/* current param */

				if ((pav->phy_type == PHY_TYPE_AC) ||
						(pav->phy_type == PHY_TYPE_LCN20)) {
				  int pnum = 0, n;
				  if (sromrev >= 12) {
					if ((pav->bandrange == WL_CHAN_FREQ_RANGE_2G) ||
						(pav->bandrange == WL_CHAN_FREQ_RANGE_2G_40))
						pnum = 4;
					else if ((pav->bandrange ==
						WL_CHAN_FREQ_RANGE_5G_5BAND) ||
						(pav->bandrange ==
						WL_CHAN_FREQ_RANGE_5G_5BAND_40) ||
						(pav->bandrange == WL_CHAN_FREQ_RANGE_5G_5BAND_80))
							pnum = 20;
				  }
				  if (sromrev < 12) {
					if (pav->bandrange == WL_CHAN_FREQ_RANGE_2G)
					  pnum = 3;
					else if (pav->bandrange == WL_CHAN_FREQ_RANGE_5G_4BAND)
					  pnum = 12;
				  }
				  printf("%s=", cpar);
				  for (n = 0; n < pnum; n ++) {
					if (n != 0)
					  printf(",");
#ifdef DSLCPE_ENDIAN
					printf("0x%x", dtoh16(outpa[i + n]));
#else
					printf("0x%x", outpa[i + n]);
#endif
				  }
				  printf("\n");
				} else {
					do {
#ifdef DSLCPE_ENDIAN
						printf("%s=0x%x\n", cpar, dtoh16(outpa[i++]));
#else
						printf("%s=0x%x\n", cpar, outpa[i++]);
#endif
					} while ((cpar = strtok (NULL, delimit)) != NULL);
				}
				pav++;
				free(par);
			}
		}
		return err;
#endif /* _CFE_ */

}

static int
wl_phy_povars(void *wl, cmd_t *cmd, char **argv)
{
#if	defined(_CFE_)
	return CFE_ERR_UNSUPPORTED;
#elif	defined(DONGLEBUILD)
	return 0;
#else
	const povars_t *pov = povars;
	wl_po_t	inpo;
	char	*cpar = NULL, *p = NULL;
	char	*par;	/* holds longest povars->vars */
	char	delimit[2] = " \0";
	int	err = 0;
	uint val;
	void	*ptr = NULL;

	if (*++argv) {	/* set */
		while (pov->phy_type != PHY_TYPE_NULL) {
			bool found = FALSE;
			int i = 0;
#ifdef DSLCPE_ENDIAN
			inpo.phy_type = htod16(pov->phy_type);
			inpo.band = htod16(pov->bandrange);
#else
			inpo.phy_type = pov->phy_type;
			inpo.band = pov->bandrange;
#endif

			par = malloc(strlen(pov->vars)+1);
			if (!par)
				return BCME_NOMEM;

			strcpy(par, pov->vars);

			/* Take care of cck and ofdm before walking through povars->vars */
			if (pov->bandrange == WL_CHAN_FREQ_RANGE_2G) {
				p = find_pattern(argv, "cck2gpo", &val);
				if (p)	found = TRUE;
#ifdef DSLCPE_ENDIAN
				inpo.cckpo = p ? htod16((uint16)val) : 0;
#else
				inpo.cckpo = p ? (uint16)val : 0;
#endif
				p = find_pattern(argv, "ofdm2gpo", &val);
			} else if (pov->bandrange == WL_CHAN_FREQ_RANGE_5GL) {
				p = find_pattern(argv, "ofdm5glpo", &val);
			} else if (pov->bandrange == WL_CHAN_FREQ_RANGE_5GM) {
				p = find_pattern(argv, "ofdm5gpo", &val);
			} else if (pov->bandrange == WL_CHAN_FREQ_RANGE_5GH) {
				p = find_pattern(argv, "ofdm5ghpo", &val);
			}
#ifdef DSLCPE_ENDIAN
			inpo.ofdmpo = p ? htod32((uint32)val) : 0;
#else
			inpo.ofdmpo = p ? (uint32)val : 0;
#endif
			if (p)	found = TRUE;

			cpar = strtok (par, delimit);	/* current param */
			do {
				val = 0;
				if (cpar == NULL) {
					inpo.mcspo[i] = val;
					break;
				}

				/* Find the parameter in the input argument list */
				p = find_pattern(argv, cpar, &val);
				if (p)	found = TRUE;
#ifdef DSLCPE_ENDIAN
				inpo.mcspo[i] = p ? htod16((uint16)val) : 0;
#else
				inpo.mcspo[i] = p ? (uint16)val : 0;
#endif
				i++;
			} while ((cpar = strtok (NULL, delimit)) != NULL);

			if (found) {
				if ((err = wlu_var_setbuf(wl, cmd->name, &inpo,
					sizeof(wl_po_t))) < 0) {
					printf("wl_phy_povars: fail to set\n");
					free(par);
					return err;
				}
			}
			pov++;
			free(par);
		}
	} else {	/* get */
		while (pov->phy_type != PHY_TYPE_NULL) {
			int i = 0;
			wl_po_t	*outpo;
#ifdef DSLCPE_ENDIAN
			inpo.phy_type = htod16(pov->phy_type);
			inpo.band = htod16(pov->bandrange);
#else
			inpo.phy_type = pov->phy_type;
			inpo.band = pov->bandrange;
#endif
			par = malloc(strlen(pov->vars)+1);
			if (!par)
				return BCME_NOMEM;

			strcpy(par, pov->vars);

			if ((err = wlu_var_getbuf(wl, cmd->name, &inpo, sizeof(povars_t),
				&ptr)) < 0) {
				printf("phy %x band %x err %d\n", pov->phy_type,
					pov->bandrange, err);
				free(par);
				break;
			}

			outpo = (wl_po_t*)ptr;
#ifdef DSLCPE_ENDIAN
			if (dtoh16(outpo->phy_type) == PHY_TYPE_NULL) {
#else
			if (outpo->phy_type == PHY_TYPE_NULL) {
#endif
				pov++;
				free(par);
				continue;
			}

			/* Take care of cck and ofdm before walking through povars->vars */
#ifdef DSLCPE_ENDIAN
			if (dtoh16(outpo->band) == WL_CHAN_FREQ_RANGE_2G) {
			        printf("cck2gpo=0x%x\n", dtoh16(outpo->cckpo));
			        printf("ofdm2gpo=0x%x\n", dtoh16(outpo->ofdmpo));
#else
			if (outpo->band == WL_CHAN_FREQ_RANGE_2G) {
				printf("cck2gpo=0x%x\n", outpo->cckpo);
				printf("ofdm2gpo=0x%x\n", outpo->ofdmpo);
#endif
			} else if (pov->bandrange == WL_CHAN_FREQ_RANGE_5GL) {
#ifdef DSLCPE_ENDIAN
				printf("ofdm5glpo=0x%x\n", dtoh32(outpo->ofdmpo));
#else	
				printf("ofdm5glpo=0x%x\n", outpo->ofdmpo);
#endif
			} else if (pov->bandrange == WL_CHAN_FREQ_RANGE_5GM) {
#ifdef DSLCPE_ENDIAN
			        printf("ofdm5gpo=0x%x\n", dtoh32(outpo->ofdmpo));
#else	
				printf("ofdm5gpo=0x%x\n", outpo->ofdmpo);
#endif
			} else if (pov->bandrange == WL_CHAN_FREQ_RANGE_5GH) {
#ifdef DSLCPE_ENDIAN
			        printf("ofdm5ghpo=0x%x\n", dtoh32(outpo->ofdmpo));
#else		
				printf("ofdm5ghpo=0x%x\n", outpo->ofdmpo);
#endif
			}

			cpar = strtok(par, delimit);	/* current param */
			do {
#ifdef DSLCPE_ENDIAN
			        printf("%s=0x%x\n", cpar, dtoh16(outpo->mcspo[i++]));
#else
				printf("%s=0x%x\n", cpar, outpo->mcspo[i++]);
#endif
			} while ((cpar = strtok (NULL, delimit)));

			pov++;
			free(par);
		}
	}

	return err;
#endif /* _CFE_ */
}

static int
wl_phy_rpcalvars(void *wl, cmd_t *cmd, char **argv)
{
#if	defined(_CFE_)
	return CFE_ERR_UNSUPPORTED;
#elif	defined(DONGLEBUILD)
	return 0;
#else
	int    err = 0, k;
	unsigned int val;
	wl_rpcal_t rpcal[2*WL_NUM_RPCALVARS], *rpcal_out;
	void *ptr = NULL;

	if (*++argv) {	/* set */
		bool found = FALSE;

		/* initialization */
		memset(&(rpcal[0]), 0, sizeof(wl_rpcal_t)*2*WL_NUM_RPCALVARS);

		if (find_pattern(argv, "rpcal2g", &val)) {
			found = TRUE;
			rpcal[WL_CHAN_FREQ_RANGE_2G].value  = (uint16) val;
			rpcal[WL_CHAN_FREQ_RANGE_2G].update = 1;
		}

		if (find_pattern(argv, "rpcal5gb0", &val)) {
			found = TRUE;
			rpcal[WL_CHAN_FREQ_RANGE_5G_BAND0].value  = (uint16) val;
			rpcal[WL_CHAN_FREQ_RANGE_5G_BAND0].update = 1;
		}

		if (find_pattern(argv, "rpcal5gb1", &val)) {
			found = TRUE;
			rpcal[WL_CHAN_FREQ_RANGE_5G_BAND1].value  = (uint16) val;
			rpcal[WL_CHAN_FREQ_RANGE_5G_BAND1].update = 1;
		}

		if (find_pattern(argv, "rpcal5gb2", &val)) {
			found = TRUE;
			rpcal[WL_CHAN_FREQ_RANGE_5G_BAND2].value  = (uint16) val;
			rpcal[WL_CHAN_FREQ_RANGE_5G_BAND2].update = 1;
		}

		if (find_pattern(argv, "rpcal5gb3", &val)) {
			found = TRUE;
			rpcal[WL_CHAN_FREQ_RANGE_5G_BAND3].value  = (uint16) val;
			rpcal[WL_CHAN_FREQ_RANGE_5G_BAND3].update = 1;
		}

		if (find_pattern(argv, "rpcal2gcore3", &val)) {
			found = TRUE;
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_2G].value =
			(uint16) (val & 0xff);
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_2G].update = 1;
		}

		if (find_pattern(argv, "rpcal5gb0core3", &val)) {
			found = TRUE;
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND0].value =
			(uint16) (val & 0xff);
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND0].update = 1;
		}

		if (find_pattern(argv, "rpcal5gb1core3", &val)) {
			found = TRUE;
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND1].value =
			(uint16) (val & 0xff);
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND1].update = 1;
		}

		if (find_pattern(argv, "rpcal5gb2core3", &val)) {
			found = TRUE;
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND2].value =
			(uint16) (val & 0xff);
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND2].update = 1;
		}

		if (find_pattern(argv, "rpcal5gb3core3", &val)) {
			found = TRUE;
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND3].value =
			(uint16) (val & 0xff);
			rpcal[WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND3].update = 1;
		}

		if (found) {
#ifdef DSLCPE_ENDIAN
			for (k = 0; k < 2*WL_NUM_RPCALVARS; k++) {
				if(rpcal[k].update) {
					rpcal[k].value = dtoh16(rpcal[k].value);
					rpcal[k].update = dtoh16(rpcal[k].update);
				}
			}
#endif
			err = wlu_var_setbuf(wl, cmd->name, &(rpcal[0]),
			                     sizeof(wl_rpcal_t)*2*WL_NUM_RPCALVARS);
			if (err < 0) {
				printf("wl_phy_rpcalvars: fail to set\n");
				return err;
			}
		} else {
			printf("wl_phy_rpcalvars: fail to found matching rpcalvar name\n");
			return err;
		}

	} else {	/* get */

		err = wlu_var_getbuf(wl, cmd->name, &(rpcal[0]),
		                     sizeof(wl_rpcal_t)*2*WL_NUM_RPCALVARS, &ptr);

		if (err < 0) {
			printf("wl_phy_rpcalvars: fail to get\n");
			return err;
		} else {
			rpcal_out = (wl_rpcal_t*) ptr;
		}

		for (k = 0; k < 2*WL_NUM_RPCALVARS; k++) {
#ifdef DSLCPE_ENDIAN
		        rpcal_out[k].value = dtoh16(rpcal_out[k].value);
#endif	
			switch (k) {
			case WL_CHAN_FREQ_RANGE_2G:
				printf("rpcal2g=0x%x ", rpcal_out[k].value);
				break;
			case WL_CHAN_FREQ_RANGE_5G_BAND0:
				printf("rpcal5gb0=0x%x ", rpcal_out[k].value);
				break;
			case WL_CHAN_FREQ_RANGE_5G_BAND1:
				printf("rpcal5gb1=0x%x ", rpcal_out[k].value);
				break;
			case WL_CHAN_FREQ_RANGE_5G_BAND2:
				printf("rpcal5gb2=0x%x ", rpcal_out[k].value);
				break;
			case WL_CHAN_FREQ_RANGE_5G_BAND3:
				printf("rpcal5gb3=0x%x\n", rpcal_out[k].value);
				break;
			case WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_2G:
				printf("rpcal2gcore3=0x%x ", rpcal_out[k].value);
				break;
			case WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND0:
				printf("rpcal5gb0core3=0x%x ", rpcal_out[k].value);
				break;
			case WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND1:
				printf("rpcal5gb1core3=0x%x ", rpcal_out[k].value);
				break;
			case WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND2:
				printf("rpcal5gb2core3=0x%x ", rpcal_out[k].value);
				break;
			case WL_NUM_RPCALVARS+WL_CHAN_FREQ_RANGE_5G_BAND3:
				printf("rpcal5gb3core3=0x%x\n", rpcal_out[k].value);
				break;
			}
		}
	}

	return 0;
#endif /* _CFE_ */
}

static int
wl_phy_fem(void *wl, cmd_t *cmd, char **argv)
{
#if	defined(_CFE_)
	return CFE_ERR_UNSUPPORTED;
#elif	defined(DONGLEBUILD)
	return 0;
#else
	srom_fem_t	fem;
	srom_fem_t	*rfem;
	void		*ptr;
	bool	found = FALSE;
	int	err = 0;
	uint	val;

	UNUSED_PARAMETER(cmd);

	if (*++argv) {	/* write fem */

		/* fem2g */
		memset(&fem, 0, sizeof(srom_fem_t));

		if (find_pattern(argv, "tssipos2g", &val)) {
			found = TRUE;
			fem.tssipos = val;
		}

		if (find_pattern(argv, "extpagain2g", &val)) {
			found = TRUE;
			fem.extpagain = val;
		}

		if (find_pattern(argv, "pdetrange2g", &val)) {
			found = TRUE;
			fem.pdetrange = val;
		}

		if (find_pattern(argv, "triso2g", &val)) {
			found = TRUE;
			fem.triso = val;
		}

		if (find_pattern(argv, "antswctl2g", &val)) {
			found = TRUE;
			fem.antswctrllut = val;
		}

		if (found) {
			if ((err = wlu_var_setbuf(wl, "fem2g", &fem, sizeof(srom_fem_t)) < 0))
				printf("wl_phy_fem: fail to set fem2g\n");
			else
				printf("fem2g set\n");
		}

		found = FALSE;
		/* fem5g */
		memset(&fem, 0, sizeof(srom_fem_t));

		if (find_pattern(argv, "tssipos5g", &val)) {
			found = TRUE;
			fem.tssipos = val;
		}

		if (find_pattern(argv, "extpagain5g", &val)) {
			found = TRUE;
			fem.extpagain = val;
		}

		if (find_pattern(argv, "pdetrange5g", &val)) {
			found = TRUE;
			fem.pdetrange = val;
		}

		if (find_pattern(argv, "triso5g", &val)) {
			found = TRUE;
			fem.triso = val;
		}

		if (find_pattern(argv, "antswctl5g", &val)) {
			found = TRUE;
			fem.antswctrllut = val;
		}

		if (found) {
			if ((err = wlu_var_setbuf(wl, "fem5g", &fem, sizeof(srom_fem_t)) < 0))
				printf("wl_phy_fem: fail to set fem5g\n");
			else
				printf("fem5g set\n");
		}
	} else {
		if ((err = wlu_var_getbuf(wl, "fem2g", NULL, 0, (void**)&ptr) < 0)) {
			printf("wl_phy_fem: fail to get fem2g\n");
		} else {
			rfem = (srom_fem_t*)ptr; /* skip the "fem2g" */
			printf("tssipos2g=0x%x extpagain2g=0x%x pdetrange2g=0x%x"
			       " triso2g=0x%x antswctl2g=0x%x\n",
			       rfem->tssipos, rfem->extpagain, rfem->pdetrange,
			       rfem->triso, rfem->antswctrllut);
	       }

		if ((err = wlu_var_getbuf(wl, "fem5g", NULL, 0, (void**)&ptr) < 0)) {
			printf("wl_phy_fem: fail to get fem5g\n");
		} else {
			rfem = (srom_fem_t*)ptr; /* skip the "fem2g" */
			printf("tssipos5g=0x%x extpagain5g=0x%x pdetrange5g=0x%x"
			       " triso5g=0x%x antswctl5g=0x%x\n",
			       rfem->tssipos, rfem->extpagain, rfem->pdetrange,
			       rfem->triso, rfem->antswctrllut);
		}
	}

	return err;
#endif /* _CFE_ */
}

static int
wl_phy_maxpower(void *wl, cmd_t *cmd, char **argv)
{
#if	defined(_CFE_)
	return CFE_ERR_UNSUPPORTED;
#elif	defined(DONGLEBUILD)
	return 0;
#else
	int	err = 0;
	uint	val;
	uint8	maxp[8];
	void	*ptr;
	uint8	*rmaxp;

	UNUSED_PARAMETER(cmd);

	if (*++argv) {	/* write maxpower */

		if (find_pattern(argv, "maxp2ga0", &val))
			maxp[0] = val;
		else
			printf("Missing maxp2ga0\n");

		if (find_pattern(argv, "maxp2ga1", &val))
			maxp[1] = val;
		else
			printf("Missing maxp2ga1\n");

		if (find_pattern(argv, "maxp5ga0", &val))
			maxp[2] = val;
		else
			printf("Missing maxp5ga0\n");

		if (find_pattern(argv, "maxp5ga1", &val))
			maxp[3] = val;
		else
			printf("Missing maxp5ga1\n");

		if (find_pattern(argv, "maxp5gla0", &val))
			maxp[4] = val;
		else
			printf("Missing maxp5gla0\n");

		if (find_pattern(argv, "maxp5gla1", &val))
			maxp[5] = val;
		else
			printf("Missing maxp5gla1\n");

		if (find_pattern(argv, "maxp5gha0", &val))
			maxp[6] = val;
		else
			printf("Missing maxp5gha0\n");

		if (find_pattern(argv, "maxp5gha1", &val))
			maxp[7] = val;
		else
			printf("Missing maxp5gha1\n");

		if ((err = wlu_var_setbuf(wl, "maxpower", &maxp, 8 * sizeof(uint8)) < 0)) {
			printf("wl_phy_maxpower: fail to set\n");
		}
	} else {
		if ((err = wlu_var_getbuf(wl, "maxpower", NULL, 0, &ptr) < 0)) {
			printf("wl_phy_maxpower: fail to get maxpower\n");
			return err;
		}
		rmaxp = (uint8*)ptr;
		printf("maxp2ga0=%x\n", rmaxp[0]);
		printf("maxp2ga1=%x\n", rmaxp[1]);
		printf("maxp5ga0=%x\n", rmaxp[2]);
		printf("maxp5ga1=%x\n", rmaxp[3]);
		printf("maxp5gla0=%x\n", rmaxp[4]);
		printf("maxp5gla1=%x\n", rmaxp[5]);
		printf("maxp5gha0=%x\n", rmaxp[6]);
		printf("maxp5gha1=%x\n", rmaxp[7]);
	}

	return err;
#endif /* _CFE_ */
}

static int
wl_pkteng(void *wl, cmd_t *cmd, char **argv)
{
	wl_pkteng_t pkteng;

	memset(&pkteng, 0, sizeof(pkteng));
	if (strcmp(cmd->name, "pkteng_stop") == 0) {
		if (!*++argv)
			return BCME_USAGE_ERROR;
		if (strcmp(*argv, "tx") == 0)
			pkteng.flags = WL_PKTENG_PER_TX_STOP;
		else if (strcmp(*argv, "rx") == 0)
			pkteng.flags = WL_PKTENG_PER_RX_STOP;
		else
			return BCME_USAGE_ERROR;
	}
	else if (strcmp(cmd->name, "pkteng_start") == 0) {
		if (!*++argv)
			return BCME_USAGE_ERROR;
		if (!wl_ether_atoe(*argv, (struct ether_addr *)&pkteng.dest))
			return BCME_USAGE_ERROR;
		if (!*++argv)
			return BCME_USAGE_ERROR;
		if ((strcmp(*argv, "tx") == 0) || (strcmp(*argv, "txwithack") == 0))  {
			if (strcmp(*argv, "tx") == 0)
				pkteng.flags = WL_PKTENG_PER_TX_START;
			else
				pkteng.flags = WL_PKTENG_PER_TX_WITH_ACK_START;
			if (!*++argv)
				return BCME_USAGE_ERROR;
			if (strcmp(*argv, "async") == 0)
				pkteng.flags &= ~WL_PKTENG_SYNCHRONOUS;
			else if (strcmp(*argv, "sync") == 0)
				pkteng.flags |= WL_PKTENG_SYNCHRONOUS;
			else
				/* neither optional parameter [async|sync] */
				--argv;
			if (!*++argv)
				return BCME_USAGE_ERROR;
			pkteng.delay = strtoul(*argv, NULL, 0);
			if (!*++argv)
				return BCME_USAGE_ERROR;
			pkteng.length = strtoul(*argv, NULL, 0);
			if (!*++argv)
				return BCME_USAGE_ERROR;
			pkteng.nframes = strtoul(*argv, NULL, 0);
			if (*++argv)
				if (!wl_ether_atoe(*argv, (struct ether_addr *)&pkteng.src))
					return BCME_USAGE_ERROR;
		}
		else if ((strcmp(*argv, "rx") == 0) || (strcmp(*argv, "rxwithack") == 0)) {
			if ((strcmp(*argv, "rx") == 0))
				pkteng.flags = WL_PKTENG_PER_RX_START;
			else
				pkteng.flags = WL_PKTENG_PER_RX_WITH_ACK_START;

			if (*++argv) {
				if (strcmp(*argv, "async") == 0)
					pkteng.flags &= ~WL_PKTENG_SYNCHRONOUS;
				else if (strcmp(*argv, "sync") == 0) {
					pkteng.flags |= WL_PKTENG_SYNCHRONOUS;
					/* sync mode requires number of frames and timeout */
					if (!*++argv)
						return BCME_USAGE_ERROR;
					pkteng.nframes = strtoul(*argv, NULL, 0);
					if (!*++argv)
						return BCME_USAGE_ERROR;
					pkteng.delay = strtoul(*argv, NULL, 0);
				}
			}
		}
		else
			return BCME_USAGE_ERROR;
	}
	else {
		printf("Invalid command name %s\n", cmd->name);
		return 0;
	}

	pkteng.flags = htod32(pkteng.flags);
	pkteng.delay = htod32(pkteng.delay);
	pkteng.nframes = htod32(pkteng.nframes);
	pkteng.length = htod32(pkteng.length);

	return (wlu_var_setbuf(wl, "pkteng", &pkteng, sizeof(pkteng)));
}

static int
wl_rxiq(void *wl, cmd_t *cmd, char **argv)
{
	miniopt_t to;
	const char* fn_name = "wl_rxiqest";
	int err, argc, opt_err;
	uint32 rxiq;
	uint8 resolution = 0;
	uint8 lpf_hpc = 1;
	uint8 dig_lpf = 1;
	uint8 gain_correct = 0;
	uint8 extra_gain_3dBsteps = 0;
	uint8 force_gain_type = 0;
	uint8 antenna = 3;
	int16 iqest_core[WL_STA_ANT_MAX];
	wlc_rev_info_t revinfo;
	memset(&revinfo, 0, sizeof(revinfo));
	if ((err = wlu_get(wl, WLC_GET_REVINFO, &revinfo, sizeof(revinfo))) < 0)
		return err;

	/* arg count */
	for (argc = 0; argv[argc]; argc++);

	/* DEFAULT:
	 * gain_correct = 0 (disable gain correction),
	 * lpf_hpc = 1 (sets lpf hpc to lowest value),
	 * dig_lpf = 1; (sets to ltrn_lpf mode)
	 * resolution = 0 (coarse),
	 * samples = 1024 (2^10) and antenna = 3
	 * force_gain_type = 0 (init gain mode)
	 */
	rxiq = (extra_gain_3dBsteps << 28) | (gain_correct << 24) | (dig_lpf << 22)
	        | (lpf_hpc << 20) | (resolution << 16) | (10 << 8) | (force_gain_type << 4)
	        | antenna;

	if (argc != 0) {
		miniopt_init(&to, fn_name, NULL, FALSE);
		while ((opt_err = miniopt(&to, argv)) != -1) {
			if (opt_err == 1) {
				err = BCME_USAGE_ERROR;
				goto exit;
			}
			argv += to.consumed;

			if (to.opt == 'g') {
				if (!to.good_int) {
					fprintf(stderr,
						"%s: could not parse \"%s\" as an int"
						" for gain-correction (0, 1, 2, 3, 4, 7, 8)\n",
						fn_name, to.valstr);

					err = BCME_BADARG;
					goto exit;
				}
				if ((to.val < 0) || (to.val > 8)) {
					fprintf(stderr, "%s: invalid gain-correction select %d"
						" (0,1,2,3,4,7,8)\n", fn_name, to.val);
					err = BCME_BADARG;
					goto exit;
				}
				gain_correct = to.val & 0xf;
				rxiq = ((gain_correct << 24) | (rxiq & 0xf0ffffff));
			}
			if (to.opt == 'f') {
				if (!to.good_int) {
					fprintf(stderr,
						"%s: could not parse \"%s\" as an int"
						" for lpf-hpc override select (0, 1)\n",
						fn_name, to.valstr);
					err = BCME_BADARG;
					goto exit;
				}
				if ((to.val < 0) || (to.val > 1)) {
					fprintf(stderr, "%s: invalid lpf-hpc override select %d"
						" (0,1)\n", fn_name, to.val);
					err = BCME_BADARG;
					goto exit;
				}
				lpf_hpc = to.val & 0xf;
				rxiq = ((lpf_hpc << 20) | (rxiq & 0xff0fffff));
			}
			if (to.opt == 'w') {
				if (!to.good_int) {
					fprintf(stderr,
						"%s: could not parse \"%s\" as an int"
						" for dig-lpf override select (0, 1 or 2)\n",
						fn_name, to.valstr);
					err = BCME_BADARG;
					goto exit;
				}
				if ((to.val < 0) || (to.val > 2)) {
					fprintf(stderr, "%s: invalid dig-lpf override select %d"
						" (0,1,2)\n", fn_name, to.val);
					err = BCME_BADARG;
					goto exit;
				}
				dig_lpf = to.val & 0x3;
				rxiq = ((dig_lpf << 22) | (rxiq & 0xff3fffff));
			}
			if (to.opt == 'r') {
				if (!to.good_int) {
					fprintf(stderr,
						"%s: could not parse \"%s\" as an int"
						" for resolution (0, 1)\n", fn_name, to.valstr);
					err = BCME_BADARG;
					goto exit;
				}
				if ((to.val < 0) || (to.val > 1)) {
					fprintf(stderr, "%s: invalid resolution select %d"
						" (0,1)\n", fn_name, to.val);
					err = BCME_BADARG;
					goto exit;
				}
				resolution = to.val & 0xf;
				rxiq = ((resolution << 16) | (rxiq & 0xfff0ffff));
			}
			if (to.opt == 's') {
				if (!to.good_int) {
					fprintf(stderr,
						"%s: could not parse \"%s\" as an int for"
						" the sample count\n", fn_name, to.valstr);
					err = BCME_BADARG;
					goto exit;
				}
				if (to.val < 0 || to.val > 16) {
					fprintf(stderr, "%s: sample count too large %d"
						"(10 <= x <= 16)\n", fn_name, to.val);
					err = BCME_BADARG;
					goto exit;
				}
				rxiq = (((to.val & 0xff) << 8) | (rxiq & 0xffff00ff));
			}
			if (to.opt == 'a') {
				if (!to.good_int) {
					fprintf(stderr,
						"%s: could not parse \"%s\" as an int"
						" for antenna (0, 1, 3)\n", fn_name, to.valstr);
					err = BCME_BADARG;
					goto exit;
				}
				if ((to.val < 0) || (to.val > 3)) {
					fprintf(stderr, "%s: invalid antenna select %d\n",
						fn_name, to.val);
					err = BCME_BADARG;
					goto exit;
				}
				rxiq = ((rxiq & 0xffffff00) | (to.val & 0xff));
			}
			if (to.opt == 'e') {
				if (!to.good_int) {
					fprintf(stderr,
					        "%s: could not parse \"%s\" as an int"
					        " for extra INITgain\n", fn_name, to.valstr);
					err = BCME_BADARG;
					goto exit;
				}
				if ((to.val < 0) || (to.val > 24) || (to.val % 3 != 0)) {
					fprintf(stderr,
					        "%s: Valid extra INITgain = {0, 3, .., 21, 24}\n",
					        fn_name);
					err = BCME_BADARG;
					goto exit;
				}
				rxiq = ((((to.val/3) & 0xf) << 28) | (rxiq & 0x0fffffff));
			}
			if (to.opt == 'i') {
				if (!to.good_int) {
					fprintf(stderr,
					        "%s: could not parse \"%s\" as an int"
					        " for init or clipLO mode\n", fn_name, to.valstr);
					err = BCME_BADARG;
					goto exit;
				}
				if ((to.val != 0) && (to.val != 1) &&
				    (to.val != 2) && (to.val != 3) && (to.val != 4)) {
					fprintf(stderr,
					"%s: Valid options - 0(default gain), 1(fixed high gain)"
					"or 4(fixed low gain). \n",
						fn_name);
					err = BCME_BADARG;
					goto exit;
				}
				rxiq = ((rxiq & 0xffffff0f) | ((to.val << 4) & 0xf0));
			}
		}
	}

	if (BCM4365_CHIP(dtoh32(revinfo.chipnum))) {
	  iqest_core[0] = rxiq & 0xffff;
	  iqest_core[1] = (rxiq >> 16) & 0xffff;

	  if ((err = wlu_var_setbuf(wl, cmd->name, iqest_core, WL_STA_ANT_MAX*sizeof(int16))) < 0)
	    return err;
	  if ((err = wlu_iovar_get(wl, cmd->name, iqest_core, WL_STA_ANT_MAX*sizeof(int16))) < 0)
	    return err;

	  if (resolution == 1) {
	    /* fine resolution power reporting (0.25dB resolution) */
	    uint8 core;
	    int16 tmp;
	    /* Four chains: */
	    for (core = 0; core < WL_STA_ANT_MAX; core++) {
	      tmp = iqest_core[core];
	      if (tmp < 0) {
		tmp = -1*tmp;
		printf("-%d.%ddBm ", (tmp >> 2), (tmp & 0x3)*25);
	      } else if (tmp > 0) {
		printf("%d.%ddBm ", (tmp >> 2), (tmp & 0x3)*25);
	      }
	    }
	    printf("\n");
	  } else {
	    /* fine resolution power reporting (0.25dB resolution) */
	    uint8 core;
	    int16 tmp;
	    /* Four chains: */
	    for (core = 0; core < WL_STA_ANT_MAX; core++) {
	      tmp = (int8)(iqest_core[core]);
	      if (tmp != 0)
		printf("%ddBm ", tmp);
	    }
	    printf("\n");
	  }
	} else {
	  if ((err = wlu_iovar_setint(wl, cmd->name, (int)rxiq)) < 0)
	    return err;
	  if ((err = wlu_iovar_getint(wl, cmd->name, (int*)&rxiq)) < 0)
	    return err;

	  if (resolution == 1) {
	    /* fine resolution power reporting (0.25dB resolution) */
	    uint8 core;
	    int16 tmp;
	    if (rxiq >> 20) {
	      /* Three chains: */
	      for (core = 0; core < 3; core ++) {
		tmp = (rxiq >> (10*core)) & 0x3ff;
		tmp = ((int16)(tmp << 6)) >> 6; /* sign extension */
		if (tmp < 0) {
		  tmp = -1*tmp;
		  printf("-%d.%ddBm ", (tmp >> 2), (tmp & 0x3)*25);
		} else {
		  printf("%d.%ddBm ", (tmp >> 2), (tmp & 0x3)*25);
		}
	      }
	      printf("\n");
	    } else if (rxiq >> 10) {
	      /* 2 chains */
	      for (core = 0; core < 2; core ++) {
		tmp = (rxiq >> (10*core)) & 0x3ff;
		tmp = ((int16)(tmp << 6)) >> 6; /* sign extension */
		if (tmp < 0) {
		  tmp = -1*tmp;
		  printf("-%d.%ddBm ", (tmp >> 2), (tmp & 0x3)*25);
		} else {
		  printf("%d.%ddBm ", (tmp >> 2), (tmp & 0x3)*25);
		}
	      }
	      printf("\n");
	    } else {
	      /* 1 chain */
	      tmp = rxiq & 0x3ff;
	      tmp = ((int16)(tmp << 6)) >> 6; /* sign extension */
	      if (tmp < 0) {
		tmp = -1*tmp;
		printf("-%d.%ddBm ", (tmp >> 2), (tmp & 0x3)*25);
	      } else {
		printf("%d.%ddBm ", (tmp >> 2), (tmp & 0x3)*25);
	      }
	      printf("\n");
	    }
	  } else {
	    if (rxiq >> 24)
	      printf("%ddBm %ddBm %ddBm %ddBm \n", (int8)(rxiq & 0xff),
	      (int8)((rxiq >> 8) & 0xff), (int8)((rxiq >> 16) & 0xff),
	      (int8)((rxiq >> 24) & 0xff));
	    else if (rxiq >> 16)
	      printf("%ddBm %ddBm %ddBm\n", (int8)(rxiq & 0xff),
	      (int8)((rxiq >> 8) & 0xff), (int8)((rxiq >> 16) & 0xff));
	    else if (rxiq >> 8)
	      printf("%ddBm %ddBm\n", (int8)(rxiq & 0xff), (int8)((rxiq >> 8) & 0xff));
	    else
	      printf("%ddBm\n", (int8)(rxiq & 0xff));
	  }
	}
exit:
	return err;
}

#if defined(BCMDBG)
static int
wl_phy_debug_cmd(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	int val;
	char *val_name;

	UNUSED_PARAMETER(cmd);

	/* command name */
	val_name = *argv++;
	val = (*argv == NULL) ? 0 : atoi(*argv);

	if ((err = wlu_iovar_setint(wl, val_name, (int)val)) < 0)
		printf("PHY DEBUG COMMAND error %d\n", err);

	return err;

}
#endif // endif

static int
wl_rifs(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	int val, rifs;
	char *val_name;

	UNUSED_PARAMETER(cmd);

	/* command name */
	val_name = *argv++;

	if (!*argv) {
		if ((err = wlu_iovar_getint(wl, val_name, (int*)&rifs)) < 0)
			return err;

		printf("%s\n", ((rifs & 0xff) ? "On" : "Off"));
		return 0;
	}

	val = rifs = (atoi(*argv) ? 1 : 0);
#ifdef DSLCPE_ENDIAN
	val = htod32(val);
#endif
	if ((err = wlu_set(wl, WLC_SET_FAKEFRAG, &val, sizeof(int))) < 0) {
		printf("Set frameburst error %d\n", err);
		return err;
	}
	if ((err = wlu_iovar_setint(wl, val_name, (int)rifs)) < 0)
		printf("Set rifs error %d\n", err);

	return err;
}

static int
wl_rifs_advert(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	int rifs_advert;
	char *val_name;

	BCM_REFERENCE(cmd);

	/* command name */
	val_name = *argv++;

	if (!*argv) {
		if ((err = wlu_iovar_getint(wl, val_name, (int*)&rifs_advert)) < 0)
			return err;

		printf("%s\n", ((rifs_advert & 0xff) ? "On" : "Off"));
		return 0;
	}

	if (strcmp(*argv, "-1") && strcmp(*argv, "0"))
		return BCME_USAGE_ERROR;

	rifs_advert = atoi(*argv);

	if ((err = wlu_iovar_setint(wl, val_name, (int)rifs_advert)) < 0)
		printf("Set rifs mode advertisement error %d\n", err);

	return err;
}

static int
wlu_afeoverride(void *wl, cmd_t *cmd, char **argv)
{
	char var[256];
	uint32 int_val;
	bool get = TRUE;
	void *ptr = NULL;
	char *endptr;

	if (argv[1]) {
		uint32 get_val;
		get = FALSE;
		int_val = htod32(strtoul(argv[1], &endptr, 0));
		if (wlu_var_getbuf(wl, cmd->name, var, sizeof(var), &ptr) < 0)
			return -1;
		get_val = *(int *)ptr;
		get_val &= ~1;
		if (int_val)
			int_val = get_val | 1;
		else
			int_val = get_val;
		memcpy(var, (char *)&int_val, sizeof(int_val));
	}
	if (get) {
		if (wlu_var_getbuf(wl, cmd->name, var, sizeof(var), &ptr) < 0)
			return -1;
		printf("0x%x\n", dtoh32(*(int *)ptr));
	}
	else
		wlu_var_setbuf(wl, cmd->name, &var, sizeof(var));
	return 0;
}

/*
 *  RADAR detection parameter control
 */
static int
wl_radar_args(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	wl_radar_args_t ra;

	/* Skip the command name */
	argv++;

	if (*argv == NULL) {
		/* Get */

		if ((ret = wlu_iovar_get(wl, cmd->name, &ra, sizeof(ra))) < 0)
			return ret;

#ifdef DSLCPE_ENDIAN
		ra.version = dtoh32(ra.version);
		ra.npulses = dtoh32(ra.npulses);
		ra.ncontig = dtoh32(ra.ncontig);
		ra.min_pw = dtoh32(ra.min_pw);
		ra.max_pw = dtoh32(ra.max_pw);
		ra.thresh0 = dtoh16(ra.thresh0);
		ra.thresh1 = dtoh16(ra.thresh1);
		ra.blank = dtoh16(ra.blank);
		ra.fmdemodcfg = dtoh16(ra.fmdemodcfg);
		ra.npulses_lp = dtoh32(ra.npulses_lp);
		ra.min_pw_lp = dtoh32(ra.min_pw_lp);
		ra.max_pw_lp = dtoh32(ra.max_pw_lp);
		ra.min_fm_lp = dtoh32(ra.min_fm_lp);
		ra.max_span_lp = dtoh32(ra.max_span_lp);
		ra.min_deltat = dtoh32(ra.min_deltat);
		ra.max_deltat = dtoh32(ra.max_deltat);
		ra.autocorr = dtoh16(ra.autocorr);
		ra.st_level_time = dtoh16(ra.st_level_time);
		ra.t2_min = dtoh16(ra.t2_min);
		ra.fra_pulse_err = dtoh32(ra.fra_pulse_err);
		ra.npulses_fra = dtoh32(ra.npulses_fra);
		ra.npulses_stg2 = dtoh32(ra.npulses_stg2);
		ra.npulses_stg3 = dtoh32(ra.npulses_stg3);
		ra.percal_mask = dtoh16(ra.percal_mask);
		ra.quant = dtoh32(ra.quant);
		ra.min_burst_intv_lp = dtoh32(ra.min_burst_intv_lp);
		ra.max_burst_intv_lp = dtoh32(ra.max_burst_intv_lp);
		ra.nskip_rst_lp = dtoh32(ra.nskip_rst_lp);
		ra.max_pw_tol = dtoh32(ra.max_pw_tol);
		ra.feature_mask = dtoh16(ra.feature_mask);
		ra.thresh0_sc    = dtoh16(ra.thresh0_sc);
		ra.thresh1_sc    = dtoh16(ra.thresh1_sc);
#endif

		if (ra.version != WL_RADAR_ARGS_VERSION) {
			printf("\tIncorrect version of RADAR_ARGS struct: expected %d; got %d\n",
			       WL_RADAR_ARGS_VERSION, ra.version);
			return -1;
		}
		printf("version %d npulses %d ncontig %d min_pw %d max_pw %d thresh0 0x%x "
		       "thresh1 0x%x\n",
		       ra.version, ra.npulses, ra.ncontig, ra.min_pw,
		       ra.max_pw, ra.thresh0, ra.thresh1);
		printf("blank 0x%x fmdemodcfg 0x%x npulses_lp %d min_pw_lp %d "
		       "max_pw_lp %d\n",
		       ra.blank, ra.fmdemodcfg, ra.npulses_lp, ra.min_pw_lp,
		       ra.max_pw_lp);
		printf("min_fm_lp %d max_span_lp %d min_deltat %d max_deltat %d\n",
		       ra.min_fm_lp, ra.max_span_lp, ra.min_deltat, ra.max_deltat);

		printf("autocorr 0x%x st_level_time 0x%x  t2_min %d fra_pulse_err %d\n",
		       ra.autocorr, ra.st_level_time, ra.t2_min, ra.fra_pulse_err);
		printf("npulses_fra %d npulses_stg2 %d npulses_stg3 %d percal_mask 0x%x quant %d\n",
			ra.npulses_fra, ra.npulses_stg2, ra.npulses_stg3, ra.percal_mask,
			ra.quant);
		printf("min_burst_intv_lp %d max_burst_intv_lp %d nskip_rst_lp %d max_pw_tol %d "
				"feature_mask 0x%x\n",
				ra.min_burst_intv_lp, ra.max_burst_intv_lp, ra.nskip_rst_lp,
				ra.max_pw_tol, ra.feature_mask);
		printf("thresh0_sc 0x%x thresh1_sc 0x%x\n",
				ra.thresh0_sc, ra.thresh1_sc);

		/* this part prints only param values */
		printf("%d %d %d %d %d 0x%x "
		       "0x%x",
		       ra.version, ra.npulses, ra.ncontig, ra.min_pw,
		       ra.max_pw, ra.thresh0, ra.thresh1);
		printf(" 0x%x 0x%x %d %d "
		       "%d",
		       ra.blank, ra.fmdemodcfg, ra.npulses_lp, ra.min_pw_lp,
		       ra.max_pw_lp);
		printf(" %d %d %d %d",
		       ra.min_fm_lp, ra.max_span_lp, ra.min_deltat, ra.max_deltat);

		printf(" 0x%x 0x%x %d %d",
		       ra.autocorr, ra.st_level_time, ra.t2_min, ra.fra_pulse_err);
		printf(" %d %d %d 0x%x %d",
			ra.npulses_fra, ra.npulses_stg2, ra.npulses_stg3, ra.percal_mask,
			ra.quant);
		printf(" %d %d %d %d "
				"0x%x  0x%x 0x%x\n",
				ra.min_burst_intv_lp, ra.max_burst_intv_lp, ra.nskip_rst_lp,
				ra.max_pw_tol, ra.feature_mask, ra.thresh0_sc, ra.thresh1_sc);

	} else {
		/* Set */
		char *endptr = NULL;
		int val_count = 32;
		long vals[32];
		long *pval;
		int i;

		for (i = 0; i < val_count; i++, argv++) {
			/* verify that there is another arg */
			if (*argv == NULL)
				return BCME_USAGE_ERROR;

			vals[i] = strtol(*argv, &endptr, 0);

			/* make sure all the value string was parsed by strtol */
			if (*endptr != '\0')
				return BCME_USAGE_ERROR;
		}

		pval = vals;

#ifdef DSLCPE_ENDIAN
		ra.version       = htod32(*pval++);
		ra.npulses       = htod32(*pval++);
		ra.ncontig       = htod32(*pval++);
		ra.min_pw        = htod32(*pval++);
		ra.max_pw        = htod32(*pval++);
		ra.thresh0       = htod16((uint16)*pval++);
		ra.thresh1       = htod16((uint16)*pval++);
		ra.blank         = htod16((uint16)*pval++);
		ra.fmdemodcfg    = htod16((uint16)*pval++);
		ra.npulses_lp    = htod32(*pval++);
		ra.min_pw_lp     = htod32(*pval++);
		ra.max_pw_lp     = htod32(*pval++);
		ra.min_fm_lp     = htod32(*pval++);
		ra.max_span_lp   = htod32(*pval++);
		ra.min_deltat    = htod32(*pval++);
		ra.max_deltat    = htod32(*pval++);
		ra.autocorr      = htod16((uint16)*pval++);
		ra.st_level_time = htod16((uint16)*pval++);
		ra.t2_min        = htod16((uint16)*pval++);
		ra.fra_pulse_err = htod32((uint32)*pval++);
		ra.npulses_fra   = htod32((int)*pval++);
		ra.npulses_stg2  = htod32((int)*pval++);
		ra.npulses_stg3  = htod32((int)*pval++);
		ra.percal_mask   = htod16((uint16)*pval++);
		ra.quant         = htod32((int)*pval++);
		ra.min_burst_intv_lp = htod32((uint32)*pval++);
		ra.max_burst_intv_lp = htod32((uint32)*pval++);
		ra.nskip_rst_lp  = htod32((int)*pval++);
		ra.max_pw_tol    = htod32((int)*pval++);
		ra.feature_mask  = htod16((uint16)*pval++);
		ra.thresh0_sc    = htod16((uint16)*pval++);
		ra.thresh1_sc    = htod16((uint16)*pval++);
#else
		ra.version       = *pval++;
		ra.npulses       = *pval++;
		ra.ncontig       = *pval++;
		ra.min_pw        = *pval++;
		ra.max_pw        = *pval++;
		ra.thresh0       = (uint16)*pval++;
		ra.thresh1       = (uint16)*pval++;
		ra.blank         = (uint16)*pval++;
		ra.fmdemodcfg    = (uint16)*pval++;
		ra.npulses_lp    = *pval++;
		ra.min_pw_lp     = *pval++;
		ra.max_pw_lp     = *pval++;
		ra.min_fm_lp     = *pval++;
		ra.max_span_lp   = *pval++;
		ra.min_deltat    = *pval++;
		ra.max_deltat    = *pval++;
		ra.autocorr      = (uint16)*pval++;
		ra.st_level_time = (uint16)*pval++;
		ra.t2_min        = (uint16)*pval++;
		ra.fra_pulse_err = (uint32)*pval++;
		ra.npulses_fra   = (int)*pval++;
		ra.npulses_stg2  = (int)*pval++;
		ra.npulses_stg3  = (int)*pval++;
		ra.percal_mask   = (int)*pval++;
		ra.quant         = (int)*pval++;
		ra.min_burst_intv_lp = (uint32)*pval++;
		ra.max_burst_intv_lp = (uint32)*pval++;
		ra.nskip_rst_lp  = (int)*pval++;
		ra.max_pw_tol    = (int)*pval++;
		ra.feature_mask  = (uint16)*pval++;
		ra.thresh0_sc    = (uint16)*pval++;
		ra.thresh1_sc    = (uint16)*pval++;
#endif
		return wlu_var_setbuf(wl, cmd->name, &ra, sizeof(wl_radar_args_t));
	}
	return ret;
}

static int
wl_radar_thrs(void *wl, cmd_t *cmd, char **argv)
{
	int ret = -1;
	wl_radar_thr_t radar_thrs;

	if (*++argv) {
		/* Set */
		char *endptr;
		int val_count = 16;
		uint16 vals[16];
		uint16 *pval;
		int i;

		for (i = 0; i < val_count; i++, argv++) {
			/* verify that there is another arg */
			if (*argv == NULL)
				return BCME_USAGE_ERROR;

			vals[i] = (uint16)strtol(*argv, &endptr, 0);

			/* make sure all the value string was parsed by strtol */
			if (*endptr != '\0')
				return BCME_USAGE_ERROR;
			if (i >= 12) {
				//for BW160
				if (*argv == NULL) {
					if (i == 12) {
						//To be compatable with older commands
						vals[i] = 0;
						vals[i+1] = 0;
						vals[i+2] = 0;
						vals[i+3] = 0;
						break;
					} else {
						return BCME_USAGE_ERROR;
					}
				} else {
					vals[i] = (uint16)strtol(*argv, &endptr, 0);
					/* make sure all the value string was parsed by strtol */
					if (*endptr != '\0')
						return BCME_USAGE_ERROR;
				}
			}
		}
#ifdef DSLCPE_ENDIAN
		radar_thrs.version = htod32(WL_RADAR_THR_VERSION);

		/* Order thresh0_20_lo, thresh1_20_lo, thresh0_40_lo, thresh1_40_lo
		 * thresh0_20_hi, thresh1_20_hi, thresh0_40_hi, thresh1_40_hi
		 */
		pval = vals;
		radar_thrs.thresh0_20_lo = htod16((uint16)*pval++);
		radar_thrs.thresh1_20_lo = htod16((uint16)*pval++);
		radar_thrs.thresh0_40_lo = htod16((uint16)*pval++);
		radar_thrs.thresh1_40_lo = htod16((uint16)*pval++);
		radar_thrs.thresh0_80_lo = htod16((uint16)*pval++);
		radar_thrs.thresh1_80_lo = htod16((uint16)*pval++);
		radar_thrs.thresh0_20_hi = htod16((uint16)*pval++);
		radar_thrs.thresh1_20_hi = htod16((uint16)*pval++);
		radar_thrs.thresh0_40_hi = htod16((uint16)*pval++);
		radar_thrs.thresh1_40_hi = htod16((uint16)*pval++);
		radar_thrs.thresh0_80_hi = htod16((uint16)*pval++);
		radar_thrs.thresh1_80_hi = htod16((uint16)*pval++);
		radar_thrs.thresh0_160_lo = htod16((uint16)*pval++);
		radar_thrs.thresh1_160_lo = htod16((uint16)*pval++);
		radar_thrs.thresh0_160_hi = htod16((uint16)*pval++);
		radar_thrs.thresh1_160_hi = htod16((uint16)*pval++);
#else
		radar_thrs.version = WL_RADAR_THR_VERSION;

		/* Order thresh0_20_lo, thresh1_20_lo, thresh0_40_lo, thresh1_40_lo
		 * thresh0_20_hi, thresh1_20_hi, thresh0_40_hi, thresh1_40_hi
		 */
		pval = vals;
		radar_thrs.thresh0_20_lo = (uint16)*pval++;
		radar_thrs.thresh1_20_lo = (uint16)*pval++;
		radar_thrs.thresh0_40_lo = (uint16)*pval++;
		radar_thrs.thresh1_40_lo = (uint16)*pval++;
		radar_thrs.thresh0_80_lo = (uint16)*pval++;
		radar_thrs.thresh1_80_lo = (uint16)*pval++;
		radar_thrs.thresh0_20_hi = (uint16)*pval++;
		radar_thrs.thresh1_20_hi = (uint16)*pval++;
		radar_thrs.thresh0_40_hi = (uint16)*pval++;
		radar_thrs.thresh1_40_hi = (uint16)*pval++;
		radar_thrs.thresh0_80_hi = (uint16)*pval++;
		radar_thrs.thresh1_80_hi = (uint16)*pval++;

		radar_thrs.thresh0_160_lo = (uint16)*pval++;
		radar_thrs.thresh1_160_lo = (uint16)*pval++;
		radar_thrs.thresh0_160_hi = (uint16)*pval++;
		radar_thrs.thresh1_160_hi = (uint16)*pval++;
#endif
		return wlu_var_setbuf(wl, cmd->name, &radar_thrs, sizeof(wl_radar_thr_t));
	}
	return ret;
}

static int
wl_radar_thrs2(void *wl, cmd_t *cmd, char **argv)
{
	int ret = -1;
	wl_radar_thr2_t radar_thrs2;
	argv++;

	if (*argv == NULL) {
		if ((ret = wlu_iovar_get(wl, cmd->name, &radar_thrs2, sizeof(radar_thrs2))) < 0)
					return ret;

				if (radar_thrs2.version != WL_RADAR_ARGS_VERSION) {
					printf("\tIncorrect version"
					"\tof RADAR_ARGS struct:expected %d; got %d\n",
					WL_RADAR_ARGS_VERSION, radar_thrs2.version);
					return -1;
				}
				printf("version %d\n"
				"thresh0_sc_20_lo 0x%x thresh1_sc_20_lo 0x%x"
				"thresh0_sc_40_lo 0x%x thresh1_sc_40_lo 0x%x"
				"thresh0_sc_80_lo 0x%x thresh1_sc_80_lo 0x%x\n"
				"thresh0_sc_20_hi 0x%x thresh1_sc_20_hi 0x%x"
				"thresh0_sc_40_hi 0x%x thresh1_sc_40_hi 0x%x"
				"thresh0_sc_80_hi 0x%x thresh1_sc_80_hi 0x%x\n"
				"fc_varth_sb 0x%x fc_varth_bin5_sb 0x%x notradar_enb 0x%x\n"
				"max_notradar_lp 0x%x max_notradar 0x%x max_notradar_lp_sc 0x%x "
				"max_notradar_sc 0x%x highpow_war_enb 0x%x highpow_sp_ratio 0x%x\n",
				radar_thrs2.version, radar_thrs2.thresh0_sc_20_lo,
				radar_thrs2.thresh1_sc_20_lo,
				radar_thrs2.thresh0_sc_40_lo, radar_thrs2.thresh1_sc_40_lo,
				radar_thrs2.thresh0_sc_80_lo, radar_thrs2.thresh1_sc_80_lo,
				radar_thrs2.thresh0_sc_20_hi, radar_thrs2.thresh1_sc_20_hi,
				radar_thrs2.thresh0_sc_40_hi, radar_thrs2.thresh1_sc_40_hi,
				radar_thrs2.thresh0_sc_80_hi, radar_thrs2.thresh1_sc_80_hi,
				radar_thrs2.fc_varth_sb, radar_thrs2.fc_varth_bin5_sb,
				radar_thrs2.notradar_enb, radar_thrs2.max_notradar_lp,
				radar_thrs2.max_notradar,
				radar_thrs2.max_notradar_lp_sc, radar_thrs2.max_notradar_sc,
				radar_thrs2.highpow_war_enb, radar_thrs2.highpow_sp_ratio);

				/* this part prints only param values */
				printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x "
				"0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
				radar_thrs2.thresh0_sc_20_lo, radar_thrs2.thresh1_sc_20_lo,
				radar_thrs2.thresh0_sc_40_lo, radar_thrs2.thresh1_sc_40_lo,
				radar_thrs2.thresh0_sc_80_lo, radar_thrs2.thresh1_sc_80_lo,
				radar_thrs2.thresh0_sc_20_hi, radar_thrs2.thresh1_sc_20_hi,
				radar_thrs2.thresh0_sc_40_hi, radar_thrs2.thresh1_sc_40_hi,
				radar_thrs2.thresh0_sc_80_hi, radar_thrs2.thresh1_sc_80_hi,
				radar_thrs2.fc_varth_sb, radar_thrs2.fc_varth_bin5_sb,
				radar_thrs2.notradar_enb,
				radar_thrs2.max_notradar_lp, radar_thrs2.max_notradar,
				radar_thrs2.max_notradar_lp_sc, radar_thrs2.max_notradar_sc,
				radar_thrs2.highpow_war_enb, radar_thrs2.highpow_sp_ratio);

	} else {
		/* Set */
		char *endptr = NULL;
		int val_count = 21;
		long vals[21];
		long *pval;
		int i;

		for (i = 0; i < val_count; i++, argv++) {
			/* verify that there is another arg */
			if (*argv == NULL)
				return BCME_USAGE_ERROR;

			vals[i] = strtol(*argv, &endptr, 0);

			/* make sure all the value string was parsed by strtol */
			if (*endptr != '\0')
				return BCME_USAGE_ERROR;
		}

		radar_thrs2.version = WL_RADAR_THR_VERSION;

		pval = vals;
		radar_thrs2.thresh0_sc_20_lo = (uint16)*pval++;
		radar_thrs2.thresh1_sc_20_lo = (uint16)*pval++;
		radar_thrs2.thresh0_sc_40_lo = (uint16)*pval++;
		radar_thrs2.thresh1_sc_40_lo = (uint16)*pval++;
		radar_thrs2.thresh0_sc_80_lo = (uint16)*pval++;
		radar_thrs2.thresh1_sc_80_lo = (uint16)*pval++;
		radar_thrs2.thresh0_sc_20_hi = (uint16)*pval++;
		radar_thrs2.thresh1_sc_20_hi = (uint16)*pval++;
		radar_thrs2.thresh0_sc_40_hi = (uint16)*pval++;
		radar_thrs2.thresh1_sc_40_hi = (uint16)*pval++;
		radar_thrs2.thresh0_sc_80_hi = (uint16)*pval++;
		radar_thrs2.thresh1_sc_80_hi = (uint16)*pval++;
		radar_thrs2.fc_varth_sb = (uint16)*pval++;
		radar_thrs2.fc_varth_bin5_sb = (uint16)*pval++;
		radar_thrs2.notradar_enb = (uint16)*pval++;
		radar_thrs2.max_notradar_lp = (uint16)*pval++;
		radar_thrs2.max_notradar = (uint16)*pval++;
		radar_thrs2.max_notradar_lp_sc = (uint16)*pval++;
		radar_thrs2.max_notradar_sc = (uint16)*pval++;
		radar_thrs2.highpow_war_enb = (uint16)*pval++;
		radar_thrs2.highpow_sp_ratio = (uint16)*pval++;
		return wlu_var_setbuf(wl, cmd->name, &radar_thrs2, sizeof(wl_radar_thr2_t));
	}
	return ret;

}
static int
wl_phy_dyn_switch_th(void *wl, cmd_t *cmd, char **argv)
{
	int ret = -1;
	wl_dyn_switch_th_t dyn_switch_th;
	argv++;

	if (*argv == NULL) {
		if ((ret = wlu_iovar_get(wl, cmd->name, &dyn_switch_th, sizeof(dyn_switch_th))) < 0)
					return ret;

				if (dyn_switch_th.ver != WL_PHY_DYN_SWITCH_TH_VERSION) {
					printf("\tIncorrect version"
					"\tof phy_dyn_switch_th:expected %d; got %d\n",
					WL_PHY_DYN_SWITCH_TH_VERSION, dyn_switch_th.ver);
					return -1;
				}
				printf("version %d\n"
				"rssi_gain_80_3 %d rssi_gain_80_2 %d "
				"rssi_gain_80_1 %d rssi_gain_80_0 %d \n"
				"rssi_gain_160_3 %d rssi_gain_160_2 %d "
				"rssi_gain_160_1 %d rssi_gain_160_0 %d \n"
				"rssi_th_2 %d rssi_th_1 %d "
				"rssi_th_0 %d\n",
				dyn_switch_th.ver, dyn_switch_th.rssi_gain_80[3],
				dyn_switch_th.rssi_gain_80[2],
				dyn_switch_th.rssi_gain_80[1], dyn_switch_th.rssi_gain_80[0],
				dyn_switch_th.rssi_gain_160[3], dyn_switch_th.rssi_gain_160[2],
				dyn_switch_th.rssi_gain_160[1], dyn_switch_th.rssi_gain_160[0],
				dyn_switch_th.rssi_th[2], dyn_switch_th.rssi_th[1],
				dyn_switch_th.rssi_th[0]);

				/* this part prints only param values */
				printf("%d %d %d %d %d %d %d %d %d "
				"%d %d\n",
				dyn_switch_th.rssi_gain_80[3], dyn_switch_th.rssi_gain_80[2],
				dyn_switch_th.rssi_gain_80[1], dyn_switch_th.rssi_gain_80[0],
				dyn_switch_th.rssi_gain_160[3], dyn_switch_th.rssi_gain_160[2],
				dyn_switch_th.rssi_gain_160[1], dyn_switch_th.rssi_gain_160[0],
				dyn_switch_th.rssi_th[2], dyn_switch_th.rssi_th[1],
				dyn_switch_th.rssi_th[0]);

	} else {
		/* Set */
		char *endptr = NULL;
		uint val_count = 11;
		long vals[11];
		long *pval;
		uint i;

		for (i = 0; i < val_count; i++, argv++) {
			/* verify that there is another arg */
			if (*argv == NULL)
				return BCME_USAGE_ERROR;
			vals[i] = strtol(*argv, &endptr, 0);

			/* make sure all the value string was parsed by strtol */
			if (*endptr != '\0')
				return BCME_USAGE_ERROR;
		}

		dyn_switch_th.ver = WL_PHY_DYN_SWITCH_TH_VERSION;

		pval = vals;
		dyn_switch_th.rssi_gain_80[3] = (uint16)*pval++;
		dyn_switch_th.rssi_gain_80[2] = (uint16)*pval++;
		dyn_switch_th.rssi_gain_80[1] = (uint16)*pval++;
		dyn_switch_th.rssi_gain_80[0] = (uint16)*pval++;
		dyn_switch_th.rssi_gain_160[3] = (uint16)*pval++;
		dyn_switch_th.rssi_gain_160[2] = (uint16)*pval++;
		dyn_switch_th.rssi_gain_160[1] = (uint16)*pval++;
		dyn_switch_th.rssi_gain_160[0] = (uint16)*pval++;
		dyn_switch_th.rssi_th[2] = (int16)*pval++;
		dyn_switch_th.rssi_th[1] = (int16)*pval++;
		dyn_switch_th.rssi_th[0] = (int16)*pval++;
		return wlu_var_setbuf(wl, cmd->name, &dyn_switch_th, sizeof(wl_dyn_switch_th_t));
	}
	return ret;

}
#if defined(WLTEST)
typedef struct {
	uint16	val;
	const char *str;
} patrim_t;

static const patrim_t patrims[] = {
	{0x0, "NULL"},
	{0x1, "bw40"},
	{0x2, "bw80"},
	{0x3, "bw4080"},
	{0x4, "cck"},
	{0x11, "NULL, bw40"},
	{0x12, "NULL, bw80"},
	{0x13, "NULL, bw4080"},
	{0x14, "NULL, cck"},
	{0x15, "bw40, bw80"},
	{0x21, "NULL, bw40, bw80"},
	{0x22, "NULL, bw40, cck"},
};

static int
wl_patrim(void *wl, cmd_t *cmd, char **argv)
{
	int patrim, ret;
	uint32 i;

	UNUSED_PARAMETER(cmd);
	UNUSED_PARAMETER(argv);

	if ((ret = wlu_iovar_getint(wl, "patrim", &patrim)) < 0)
		return ret;

	for (i = 0; i < sizeof(patrims) / sizeof(patrims[0]); i ++) {
		if (patrim == patrims[i].val) {
			printf("%s\n", patrims[i].str);
			return 0;
		}
	}

	return BCME_ERROR;
}
#endif // endif
