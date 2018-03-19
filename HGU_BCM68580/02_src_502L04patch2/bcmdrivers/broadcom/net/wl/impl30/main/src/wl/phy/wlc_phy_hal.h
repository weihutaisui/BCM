/*
 * Required functions exported by the PHY module (phy-dependent)
 * to common (os-independent) driver code.
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
 * $Id: wlc_phy_hal.h 649337 2016-07-15 17:46:35Z $
 */

#ifndef _wlc_phy_h_
#define _wlc_phy_h_

#include <typedefs.h>
#include <wlioctl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <d11.h>
#include <wlc_phy_shim.h>
#include <wlc_ppr.h>

#define M_LCNXN_BLK_PTR         (71 * 2)

/* Noise Cal enable */
#define NOISE_CAL_LCNXNPHY
#define ENABLE_LISTEN_GAIN_CHANGE

#define ENABLE_FDS

/* SMTH MACROS */
#define	SMTH_DISABLE			0x0
#define	SMTH_ENABLE			0x1
#define	SMTH_ENABLE_NO_NW		0x2
#define	SMTH_ENABLE_NO_NW_GD		0x3
#define	SMTH_ENABLE_NO_NW_GD_MTE	0x4
#define	DISABLE_SIGB_AND_SMTH		0x5
#define	SMTH_FOR_TXBF			0x6

/* SMTH DUMP MODE */
#define	SMTH_NODUMP			0
#define	SMTH_FREQDUMP			2
#define	SMTH_FREQDUMP_AFTER_NW		3
#define	SMTH_FREQDUMP_AFTER_GD		4
#define	SMTH_FREQDUMP_AFTER_MTE		5
#define	SMTH_TIMEDUMP_AFTER_IFFT	6
#define	SMTH_TIMEDUMP_AFTER_WIN		7
#define	SMTH_FREQDUMP_AFTER_FFT		8

/* Radio macros */

/* Radio ID */
#define	IDCODE_VER_MASK		0x0000000f
#define	IDCODE_VER_SHIFT	0
#define	IDCODE_MFG_MASK		0x00000fff
#define	IDCODE_MFG_SHIFT	0
#define	IDCODE_ID_MASK		0x0ffff000
#define	IDCODE_ID_SHIFT		12
#define	IDCODE_REV_MASK		0xf0000000
#define	IDCODE_REV_SHIFT	28

#define IDCODE_ACPHY_ID_MASK   0xffff
#define IDCODE_ACPHY_ID_SHIFT  0
#define IDCODE_ACPHY_REV_MASK  0xffff0000
#define IDCODE_ACPHY_REV_SHIFT 16
#define IDCODE_ACPHY_MAJORREV_MASK  0xfff00000
#define IDCODE_ACPHY_MAJORREV_SHIFT 20
#define IDCODE_ACPHY_MINORREV_MASK  0x000f0000
#define IDCODE_ACPHY_MINORREV_SHIFT 16

#define	NORADIO_ID		0xe4f5
#define	NORADIO_IDCODE		0x4e4f5246

#define	BCM2050_ID		0x2050
#define	BCM2050_IDCODE		0x02050000
#define	BCM2050A0_IDCODE	0x1205017f
#define	BCM2050A1_IDCODE	0x2205017f
#define	BCM2050R8_IDCODE	0x8205017f

#define BCM2055_ID		0x2055
#define BCM2055_IDCODE		0x02055000
#define BCM2055A0_IDCODE	0x1205517f

#define BCM2056_ID		0x2056
#define BCM2056_IDCODE		0x02056000
#define BCM2056A0_IDCODE	0x1205617f

#define BCM2057_ID		0x2057
#define BCM2057_IDCODE		0x02057000
#define BCM2057A0_IDCODE	0x1205717f

#define BCM2059_ID		0x2059
#define BCM2059A0_IDCODE	0x0205917f

#define	BCM2060_ID		0x2060
#define	BCM2060_IDCODE		0x02060000
#define	BCM2060WW_IDCODE	0x1206017f

#define BCM2062_ID		0x2062
#define BCM2062_IDCODE		0x02062000
#define BCM2062A0_IDCODE	0x0206217f

#define BCM2063_ID		0x2063
#define BCM2063_IDCODE		0x02063000
#define BCM2063A0_IDCODE	0x0206317f

#define BCM2064_ID		0x2064
#define BCM2064_IDCODE		0x02064000
#define BCM2064A0_IDCODE	0x0206417f

#define BCM2065_ID		0x2065
#define BCM2065_IDCODE		0x02065000
#define BCM2065A0_IDCODE	0x0206517f

#define BCM2067_ID		0x2067
#define BCM2067_IDCODE		0x02067000
#define BCM2067A0_IDCODE	0x0206717f

#define BCM2066_ID		0x2066
#define BCM2066_IDCODE		0x02066000
#define BCM2066A0_IDCODE	0x0206617f

#define BCM20671_ID		0x022e
#define BCM20671_IDCODE		0x0022e000
#define BCM20671A0_IDCODE	0x0022e17f
#define BCM20671A1_IDCODE	0x1022e17f
#define BCM20671B0_IDCODE	0x11022e17f

#define BCM2069_ID		0x2069
#define BCM2069A0_IDCODE	0x02069000

#define BCM20691_ID		0x30b
#define BCM20693_ID		0x3eb

/* PHY macros */
#define PHY_MAX_CORES		4	/* max number of cores supported by PHY HAL */

#define PHY_TPC_HW_OFF		FALSE
#define PHY_TPC_HW_ON		TRUE

#define PHY_PERICAL_DRIVERUP	1	/* periodic cal for driver up */
#define PHY_PERICAL_WATCHDOG	2	/* periodic cal for watchdog */
#define PHY_PERICAL_PHYINIT	3	/* periodic cal for phy_init */
#define PHY_PERICAL_JOIN_BSS	4	/* periodic cal for join BSS */
#define PHY_PERICAL_START_IBSS	5	/* periodic cal for join IBSS */
#define PHY_PERICAL_UP_BSS	6	/* periodic cal for up BSS */
#define PHY_PERICAL_CHAN	7 	/* periodic cal for channel change */
#define PHY_PERICAL_DCS	    11 	/* periodic cal for DCS */
#define PHY_FULLCAL	8		/* full calibration routine */
#define PHY_PAPDCAL	10		/* papd calibration routine */
#ifdef WLOTA_EN
#define PHY_FULLCAL_SPHASE	PHY_PERICAL_JOIN_BSS /* full cal ina single phase */
#endif /* WLOTA_EN */
#ifdef DSLCPE_C601911
#define PHY_PERICAL_RXCHAIN	12
#endif
#define PHY_PERICAL_DISABLE	0	/* periodic cal disabled */
#define PHY_PERICAL_SPHASE	1	/* periodic cal enabled, single phase only */
#define PHY_PERICAL_MPHASE	2	/* periodic cal enabled, can do multiphase */
#define PHY_PERICAL_MANUAL	3	/* disable periodic cal, only run it from iovar */

#define PHY_PERICAL_MAXINTRVL (15*60) /* Maximum time interval in sec between PHY calibrations */

#define PHY_PERICAL_DELAY_DEFAULT	5	/* delay (in ms) between each cal mphase */
#define PHY_PERICAL_DELAY_MIN		1	/* min delay between each cal mphase */
#define PHY_PERICAL_DELAY_MAX		2000	/* max delay between each cal mphase */
#define PHY_PERICAL_DELAY_EU            60      /* delay between each cal for mphase- REGION_EU */

#define PHY_MPH_CAL_DELAY		150	/* multi phase phy cal schedule delay */

#define PHY_HOLD_FOR_ASSOC	1	/* hold PHY activities(like cal) during association */
#define PHY_HOLD_FOR_SCAN	2	/* hold PHY activities(like cal) during scan */
#define PHY_HOLD_FOR_RM		4	/* hold PHY activities(like cal) during radio measure */
#define PHY_HOLD_FOR_PLT	8	/* hold PHY activities(like cal) during plt */
#define PHY_HOLD_FOR_MUTE		0x10
#define PHY_HOLD_FOR_NOT_ASSOC	0x20
#define PHY_HOLD_FOR_ACI_SCAN	0x40 /* hold PHY activities(like cal) during ACI scan */
#define PHY_HOLD_FOR_PKT_ENG	0x80 /* hold PHY activities(like cal) during PKTENG mode */
#define PHY_HOLD_FOR_DCS	    0x100 /* hold PHY activities(like cal) during DCS */
#define PHY_HOLD_FOR_MPC_SCAN	0x200 /* hold PHY activities(like cal) during scan in mpc 1 mode */
#ifdef WL_MULTIQUEUE
#define PHY_HOLD_FOR_EXCURSION	0x400 /* hold PHY activities while in excursion */
#endif // endif
#define PHY_HOLD_FOR_TOF	0x800 /* hold PHY activities while doing wifi ranging */
#define PHY_HOLD_FOR_VSDB	0x1000 /* hold PHY cals during  VSDB */

#define PHY_MUTE_FOR_PREISM	1
#define PHY_MUTE_ALL		0xffffffff

/* Fixed Noise for APHY/BPHY/GPHY */
#define PHY_NOISE_FIXED_VAL 		(-95)	/* reported fixed noise */
#define PHY_NOISE_FIXED_VAL_NPHY       	(-92)	/* reported fixed noise */
#define PHY_NOISE_FIXED_VAL_NPHY_4324x_EPA  (-105)	/* reported fixed noise */
#define PHY_NOISE_FIXED_VAL_LCNPHY     	(-92)	/* reported fixed noise */
#define PHY_NOISE_FIXED_VAL_SSLPNPHY    (-92)   /* reported fixed noise */

/* phy_mode bit defs for high level phy mode state information */
#define PHY_MODE_ACI		0x0001	/* set if phy is in ACI mitigation mode */
#define PHY_MODE_CAL		0x0002	/* set if phy is in Calibration mode */
#define PHY_MODE_NOISEM		0x0004	/* set if phy is in Noise Measurement mode */

#define WLC_TXPWR_DB_FACTOR	4	/* most txpower parameters are in 1/4 dB unit */

/* phy capabilities */
#define PHY_CAP_40MHZ		0x00000001	/* 40MHz channels supported */
#define PHY_CAP_80MHZ		0x00000008	/* 80MHz channels supported */
#define PHY_CAP_8080MHZ		0x00000040	/* 80+80 MHz channels supported */
#define PHY_CAP_160MHZ		0x00000080	/* 160 MHz channels supported */
#define PHY_CAP_STBC		0x00000002
#define PHY_CAP_SGI			0x00000004
#define PHY_CAP_LDPC		0x00000010
#define PHY_CAP_VHT_PROP_RATES	0x00000020	/* Supports Proprietary VHT rates */

/* AFE Override */
#define PHY_AFE_OVERRIDE_USR	1
#define PHY_AFE_OVERRIDE_DRV	2

#ifdef BCMPHYCORENUM
#define PHYCORENUM(cn)	(BCMPHYCORENUM)
#define PHY_CORE_MAX	(BCMPHYCORENUM)
#else
#define PHYCORENUM(cn)	(cn)
#define PHY_CORE_MAX	4
#endif // endif

#ifdef BCMPHYCOREMASK
#define PHYCOREMASK(cm)	(BCMPHYCOREMASK)
#else
#define PHYCOREMASK(cm)	(cm)
#endif // endif

/* add the feature to take out the BW RESET duriny the BW change  0: disable 1: enable */
#define BW_RESET 1
/* add the feature to disable DSSF  0: disable 1: enable */
#define DSSF_ENABLE 1
#define DSSFB_ENABLE 1

/* add wlc_phydump_phycal to WLTEST for mfgtest */
#define PHYCAL_DMP_FOR_MFGTEST

/*
 * These region constants are set by the
 * driver on region change if and when region-specific
 * tuning is required. For example, this is used to
 * turn on EU-specific energy detection.
 */
#define REGION_OTHER 0
#define REGION_EU 1

/* channel bitvec */
typedef struct {
	uint8   vec[MAXCHANNEL/NBBY];   /* bitvec of channels */
} chanvec_t;

/* iovar table */
enum {
	/* OLD PHYTYPE specific ones, to phase out, use unified ones at the end
	 * For each unified, mark the original one as "depreciated".
	 * User scripts should stop using them for new testcases
	 */
	IOV_LCNPHY_LDOVOLT,
	IOV_LPPHY_ACI_CHAN_SCAN_CNT,
	IOV_LPPHY_ACI_CHAN_SCAN_CNT_THRESH,
	IOV_LPPHY_ACI_CHAN_SCAN_PWR_THRESH,
	IOV_LPPHY_ACI_CHAN_SCAN_TIMEOUT,
	IOV_LPPHY_ACI_GLITCH_TIMEOUT,
	IOV_LPPHY_ACI_OFF_THRESH,
	IOV_LPPHY_ACI_OFF_TIMEOUT,
	IOV_LPPHY_ACI_ON_THRESH,
	IOV_LPPHY_ACI_ON_TIMEOUT,
	IOV_LPPHY_CAL_DELTA_TEMP,
	IOV_LPPHY_CCK_ANALOG_FILT_BW_OVERRIDE,
	IOV_LPPHY_CCK_DIG_FILT_TYPE,
	IOV_LPPHY_CCK_RCCAL_OVERRIDE,
	IOV_LPPHY_CRS,
	IOV_LPPHY_FULLCAL,		/* depreciated */
	IOV_LPPHY_IDLE_TSSI_UPDATE_DELTA_TEMP,
	IOV_LPPHY_NOISE_SAMPLES,
	IOV_LPPHY_OFDM_ANALOG_FILT_BW_OVERRIDE,
	IOV_LPPHY_OFDM_DIG_FILT_TYPE,
	IOV_LPPHY_OFDM_RCCAL_OVERRIDE,
	IOV_LPPHY_PAPDCAL,
	IOV_LPPHY_PAPDCALTYPE,
	IOV_LPPHY_PAPDEPSTBL,
	IOV_LPPHY_PAPD_RECAL_COUNTER,
	IOV_LPPHY_PAPD_RECAL_ENABLE,
	IOV_LPPHY_PAPD_RECAL_GAIN_DELTA,
	IOV_LPPHY_PAPD_RECAL_MAX_INTERVAL,
	IOV_LPPHY_PAPD_RECAL_MIN_INTERVAL,
	IOV_LPPHY_PAPD_SLOW_CAL,
	IOV_LPPHY_RXIQCAL,
	IOV_LPPHY_RX_GAIN_TEMP_ADJ_METRIC,
	IOV_LPPHY_RX_GAIN_TEMP_ADJ_TEMPSENSE,
	IOV_LPPHY_RX_GAIN_TEMP_ADJ_THRESH,
	IOV_LPPHY_TEMPSENSE,		/* depreciated */
	IOV_LPPHY_TXIQCC,			/* depreciated */
	IOV_LPPHY_TXIQLOCAL,
	IOV_LPPHY_TXLOCC,			/* depreciated */
	IOV_LPPHY_TXPWRCTRL,		/* depreciated */
	IOV_LPPHY_TXPWRINDEX,		/* depreciated */
	IOV_PHY_OCLSCDENABLE,
	IOV_PHY_RXANTSEL,
	IOV_LPPHY_TXRF_SP_9_OVR,
	IOV_LPPHY_TX_TONE,
	IOV_LPPHY_VBATSENSE,
	IOV_NPHY_5G_PWRGAIN,
	IOV_NPHY_ACI_SCAN,
	IOV_NPHY_BPHY_EVM,
	IOV_NPHY_BPHY_RFCS,
	IOV_NPHY_CALTXGAIN,
	IOV_NPHY_CAL_RESET,
	IOV_NPHY_CAL_SANITY,
	IOV_NPHY_ELNA_GAIN_CONFIG,
	IOV_NPHY_ENABLERXCORE,
	IOV_NPHY_EST_TONEPWR,
	IOV_NPHY_FORCECAL,		/* depreciated */
	IOV_NPHY_GAIN_BOOST,
	IOV_NPHY_GPIOSEL,
	IOV_NPHY_HPVGA1GAIN,
	IOV_NPHY_INITGAIN,
	IOV_NPHY_PAPDCAL,
	IOV_NPHY_PAPDCALINDEX,
	IOV_NPHY_PAPDCALTYPE,
	IOV_NPHY_PERICAL,		/* depreciated */
	IOV_NPHY_PHYREG_SKIPCNT,
	IOV_NPHY_PHYREG_SKIPDUMP,
	IOV_NPHY_RFSEQ,
	IOV_NPHY_RFSEQ_TXGAIN,
	IOV_NPHY_RSSICAL,
	IOV_NPHY_RSSISEL,
	IOV_NPHY_RXCALPARAMS,
	IOV_NPHY_RXIQCAL,
	IOV_NPHY_SCRAMINIT,
	IOV_NPHY_SKIPPAPD,
	IOV_NPHY_TBLDUMP_MAXIDX,
	IOV_NPHY_TBLDUMP_MINIDX,
	IOV_NPHY_TEMPOFFSET,
	IOV_NPHY_TEMPSENSE,		/* depreciated */
	IOV_NPHY_TEMPTHRESH,		/* depreciated */
	IOV_NPHY_TEST_TSSI,
	IOV_NPHY_TEST_TSSI_OFFS,
	IOV_NPHY_TXIQLOCAL,
	IOV_NPHY_TXPWRCTRL,		/* depreciated */
	IOV_NPHY_TXPWRINDEX,		/* depreciated */
	IOV_NPHY_TX_TEMP_TONE,
	IOV_NPHY_TX_TONE,
	IOV_NPHY_VCOCAL,
	IOV_NPHY_CCK_PWR_OFFSET,
	IOV_SSLPNPHY_CARRIER_SUPPRESS,
	IOV_SSLPNPHY_CRS,
	IOV_SSLPNPHY_FULLCAL,
	IOV_SSLPNPHY_NOISE_SAMPLES,
	IOV_SSLPNPHY_PAPARAMS,
	IOV_SSLPNPHY_PAPDCAL,
	IOV_SSLPNPHY_PAPD_DEBUG,
	IOV_SSLPNPHY_RXIQCAL,
	IOV_SSLPNPHY_TXIQLOCAL,
	IOV_SSLPNPHY_TXPWRCTRL,		/* depreciated */
	IOV_SSLPNPHY_TXPWRINDEX,	/* depreciated */
	IOV_SSLPNPHY_TX_TONE,
	IOV_SSLPNPHY_UNMOD_RSSI,
	IOV_SSLPNPHY_SPBRUN,
	IOV_SSLPNPHY_SPBDUMP,
	IOV_SSLPNPHY_SPBRANGE,
	IOV_SSLPNPHY_PKTENG_STATS,
	IOV_SSLPNPHY_PKTENG,
	IOV_SSLPNPHY_CGA,
	IOV_SSLPNPHY_TX_IQCC,
	IOV_SSLPNPHY_TX_LOCC,
	IOV_SSLPNPHY_PER_CAL,
	IOV_SSLPNPHY_VCO_CAL,
	IOV_SSLPNPHY_TXPWRINIT,

	/* ==========================================
	 * unified phy iovar, independent of PHYTYPE
	 * ==========================================
	 */
	IOV_PHYHAL_MSG = 300,
	IOV_PHY_WATCHDOG,
#ifndef PHYMOD3_TRUNK_MERGE
	IOV_RADAR_ARGS,
	IOV_RADAR_ARGS_40MHZ,
	IOV_RADAR_THRS,
	IOV_PHY_DFS_LP_BUFFER,
	IOV_RADAR_STATUS,
	IOV_CLEAR_RADAR_STATUS,
#endif /* PHYMOD3_TRUNK_MERGE */
	IOV_FAST_TIMER,
	IOV_SLOW_TIMER,
	IOV_GLACIAL_TIMER,
	IOV_TXINSTPWR,
	IOV_PHY_FIXED_NOISE,
	IOV_PHYNOISE_POLL,
	IOV_PHY_SPURAVOID,
	IOV_CARRIER_SUPPRESS,
	IOV_UNMOD_RSSI,
	IOV_PKTENG_STATS,
	IOV_ACI_EXIT_CHECK_PERIOD,
	IOV_PHY_RXIQ_EST,
	IOV_PHYTABLE,
	IOV_NUM_STREAM,
	IOV_BAND_RANGE,
	IOV_PHYWREG_LIMIT,
	IOV_MIN_TXPOWER,
	IOV_TSSIVISI_THRESH,
	IOV_PHY_MUTED,
	IOV_IQ_IMBALANCE_METRIC_DATA,
	IOV_IQ_IMBALANCE_METRIC,
	IOV_IQ_IMBALANCE_METRIC_PASS,
	IOV_PHY_SAMPLE_COLLECT,
	IOV_PHY_SAMPLE_COLLECT_GAIN_ADJUST,
	IOV_PHY_SAMPLE_COLLECT_GAIN_INDEX,
	IOV_PHY_SAMPLE_DATA,
	IOV_PHY_TXPWRCTRL,
	IOV_PHY_RESETCCA,
	IOV_PHY_GLITCHK,
	IOV_PHY_NOISE_UP,
	IOV_PHY_NOISE_DWN,
	IOV_PHY_TXPWRINDEX,
	IOV_PHY_AVGTSSI_REG,
	IOV_PHY_IDLETSSI_REG,
	IOV_PHY_IDLETSSI,
	IOV_PHY_ADJUSTED_TSSI,
	IOV_PHY_CAL_DISABLE,
	IOV_PHY_PERICAL,	/* Periodic calibration cofig */
	IOV_PHY_PERICAL_DELAY,
#ifdef DSLCPE_PHYCAL_OVERRIDE_ON_TRAFFIC
	IOV_PHY_WATCHDOG_OVERRIDE_ON_TRAFFIC,
	IOV_PHY_WATCHDOG_TRAFFIC_LIMIT,
#endif /* DSLCPE_PHYCAL_OVERRIDE_ON_TRAFFIC */
	IOV_PHY_FORCECAL,
	IOV_PHY_SKIPPAPD,
	IOV_PHY_TXRX_CHAIN,
	IOV_PHY_BPHY_EVM,
	IOV_PHY_BPHY_RFCS,
	IOV_PHY_ENABLERXCORE,
	IOV_PHY_EST_TONEPWR,
	IOV_PHY_GPIOSEL,
	IOV_PHY_5G_PWRGAIN,
	IOV_PHY_RFSEQ,
	IOV_PHY_SCRAMINIT,
	IOV_PHY_TEMPOFFSET,
	IOV_PHY_TEMPTHRESH,
	IOV_PHY_TEMPSENSE,
	IOV_PHY_VBATSENSE,
	IOV_PHY_TEST_TSSI,
	IOV_PHY_TEST_TSSI_OFFS,
	IOV_PHY_TEST_IDLETSSI,
	IOV_PHY_TX_TONE,
	IOV_PHY_TX_TONE_HZ,
	IOV_PHY_TX_TONE_STOP,
	IOV_PHY_FEM2G,
	IOV_PHY_FEM5G,
	IOV_PHY_MAXP,
	IOV_PAVARS,
	IOV_PATRIM,
	IOV_POVARS,
	IOV_RPCALVARS,
	IOV_PHYCAL_STATE,
	IOV_LCNPHY_PAPDEPSTBL,
	IOV_PHY_TXIQCC,
	IOV_PHY_TXLOCC,
	IOV_PHYCAL_TEMPDELTA,
	IOV_PHY_PAPD_DEBUG,
	IOV_PHY_PACALSTATUS,
	IOV_PHY_ACTIVECAL,
	IOV_NOISE_MEASURE,
	IOV_NOISE_MEASURE_DISABLE,
	IOV_PHY_PACALIDX0,
	IOV_PHY_PACALIDX1,
	IOV_PHY_IQLOCALIDX,
	IOV_PHY_PACALIDX,
	IOV_PHY_LOWPOWER_BEACON_MODE,
	IOV_PHY_SROM_TEMPSENSE,
	IOV_PHY_RXGAIN_RSSI,
	IOV_PHY_RSSI_CAL_REV,
	IOV_PHY_RUD_AGC_ENABLE,
	IOV_PHY_GAIN_CAL_TEMP,
	IOV_PHY_TEMP_COMP_TRLOSS,
	IOV_PHY_SETRPTBL,
	IOV_PHY_FORCEIMPBF,
	IOV_PHY_FORCESTEER,
	IOV_PHY_TXFILTER_SM_OVERRIDE,
	IOV_TSSICAL_START_IDX,
	IOV_TSSICAL_START,
	IOV_TSSICAL_POWER,
	IOV_TSSICAL_PARAMS,
	IOV_PHY_DEAF,
	IOV_PHY_TSSITXDELAY,
	IOV_PHY_RXDESENS,
	IOV_NTD_GDS_LOWTXPWR,
	IOV_PAPDCAL_INDEXDELTA,
	IOV_LCNPHY_CWTXPWRCTRL,
	IOV_PHYNOISE_SROM,
	IOV_LCNPHY_SAMP_CAP,
	IOV_PHY_SUBBAND5GVER,
	IOV_PHY_FCBSINIT,
	IOV_PHY_FCBS,
	IOV_PHY_FCBSARM,
	IOV_PHY_FCBSEXIT,
	IOV_PHY_CRS_WAR,
	IOV_LCNPHY_RXIQGAIN,
	IOV_LCNPHY_RXIQGSPOWER,
	IOV_LCNPHY_RXIQPOWER,
	IOV_LCNPHY_RXIQSTATUS,
	IOV_LCNPHY_RXIQSTEPS,
	IOV_PHY_TEMPSENSE_OVERRIDE,
	IOV_PHY_TEMP_HYSTERESIS,
	IOV_LCNPHY_TXPWRCLAMP_DIS,
	IOV_LCNPHY_TXPWRCLAMP_OFDM,
	IOV_LCNPHY_TXPWRCLAMP_CCK,
	IOV_LCNPHY_TSSI_MAXPWR,
	IOV_LCNPHY_TSSI_MINPWR,
	IOV_PHY_CGA_5G,
	IOV_PHY_CGA_2G,
	IOV_PHY_DEBUG_CMD,

	IOV_PHY_FORCE_FDIQI,
	IOV_PHY_FORCE_GAINLEVEL,
	IOV_PHY_FORCE_SPURMODE,
	IOV_PHY_BBMULT,
	IOV_LNLDO2,
	IOV_PHY_FORCE_CRSMIN,
	IOV_PHY_BTCOEX_DESENSE,
#ifdef BCMLTECOEX
	IOV_PHY_LTECX_MODE,
#endif // endif
	IOV_PAVARS2,
	IOV_PHY_BTC_RESTAGE_RXGAIN,
	IOV_PHY_DSSF,
	IOV_ED_THRESH,
	IOV_PHY_SAR_LIMIT,
	IOV_PHY_TXPWR_CORE,
	IOV_EDCRS,
	IOV_LP_MODE,
	IOV_RADIO_PD,
	IOV_LP_VCO_2G,
	IOV_SMTH,
	IOV_PHY_FORCECAL_OBT,
	IOV_PHY_DYN_ML,
	IOV_PHY_ACI_NAMS,
	IOV_PHY_RSSI_CAL_FREQ_GRP_2G,
	IOV_PHY_RSSI_GAIN_DELTA_2GB0,
	IOV_PHY_RSSI_GAIN_DELTA_2GB1,
	IOV_PHY_RSSI_GAIN_DELTA_2GB2,
	IOV_PHY_RSSI_GAIN_DELTA_2GB3,
	IOV_PHY_RSSI_GAIN_DELTA_2GB4,
	IOV_PHY_RSSI_GAIN_DELTA_2G,
	IOV_PHY_RSSI_GAIN_DELTA_2GH,
	IOV_PHY_RSSI_GAIN_DELTA_2GHH,
	IOV_PHY_RSSI_GAIN_DELTA_5GL,
	IOV_PHY_RSSI_GAIN_DELTA_5GML,
	IOV_PHY_RSSI_GAIN_DELTA_5GMU,
	IOV_PHY_RSSI_GAIN_DELTA_5GH,
	IOV_PHY_RXGAINERR_2G,
	IOV_PHY_RXGAINERR_5GL,
	IOV_PHY_RXGAINERR_5GM,
	IOV_PHY_RXGAINERR_5GH,
	IOV_PHY_RXGAINERR_5GU,
	IOV_PHY_RSSI_GAIN_CAL_TEMP,
	IOV_PHY_MAC_TRIGGERED_SAMPLE_COLLECT,
	IOV_PHY_MAC_TRIGGERED_SAMPLE_DATA,
	IOV_PHY_VCOCAL,
	IOV_PHY_FORCECAL_NOISE,
	IOV_PKTENG_GAININDEX,
	IOV_PHY_AUXPGA,
	IOV_HIRSSI_PERIOD,
	IOV_HIRSSI_EN,
	IOV_HIRSSI_BYP_RSSI,
	IOV_HIRSSI_RES_RSSI,
	IOV_HIRSSI_BYP_CNT,
	IOV_HIRSSI_RES_CNT,
	IOV_HIRSSI_STATUS,
	IOV_PHY_TXSWCTRLMAP,
	IOV_PHY_MAC_TRIGGERD_SAMPLE_COLLECT,
	IOV_PHY_MAC_TRIGGERD_SAMPLE_DATA,
	IOV_PHY_WFD_LL_ENABLE,

#if defined(WLTEST) || defined(BCMDBG)

	IOV_PHY_ENABLE_EPA_DPD_2G,
	IOV_PHY_ENABLE_EPA_DPD_5G,
	IOV_PHY_EPACAL2GMASK,

#endif // endif
	IOV_ANT_DIV_SW_CORE0,
	IOV_ANT_DIV_SW_CORE1,
	IOV_PHY_AFE_OVERRIDE,
	IOV_PHY_RF_SWCTRL_TOGGLE,
	IOV_PA_BW_VER,
	IOV_PHY_OCL_FORCE_CORE0,
	IOV_CAL_PERIOD,
	IOV_PHY_TXCAL_GAINSWEEP,
	IOV_PHY_TXCAL_GAINSWEEP_MEAS,
	IOV_PHY_TXCAL_PWR_TSSI_TBL,
	IOV_PHY_TXCAL_APPLY_PWR_TSSI_TBL,
	IOV_PHY_READ_ESTPWR_LUT,
	IOV_PHY_LAST	/* insert before this one */
};

typedef enum  phy_radar_detect_mode {
	RADAR_DETECT_MODE_FCC,
	RADAR_DETECT_MODE_EU,
	RADAR_DETECT_MODE_MAX
} phy_radar_detect_mode_t;

#ifdef WLC_SW_DIVERSITY
typedef enum _wlc_swdiv_swctrl_t {
	SWDIV_SWCTRL_0,   /* Diversity switch controlled via GPIO */
	SWDIV_SWCTRL_1,   /* Diversity switch controlled via LCN40PHY_swctrlOvr_val */
	SWDIV_SWCTRL_2    /* Diversity switch controlled via LCN40PHY_RFOverrideVal0 */
} wlc_swdiv_swctrl_t;
#endif /* WLC_SW_DIVERSITY */

/* forward declaration */
struct rpc_info;
typedef struct shared_phy shared_phy_t;

/* Public phy info structure */
struct phy_pub;

#ifdef WLC_HIGH_ONLY
typedef struct wlc_rpc_phy wlc_phy_t;
#else
typedef struct phy_pub wlc_phy_t;
#endif // endif

typedef struct shared_phy_params {
	void 	*osh;		/* pointer to OS handle */
	si_t	*sih;		/* si handle (cookie for siutils calls) */
	void	*physhim;	/* phy <-> wl shim layer for wlapi */
	uint	unit;		/* device instance number */
	uint	corerev;	/* core revision */
	uint	bustype;	/* SI_BUS, PCI_BUS  */
	uint	buscorerev; 	/* buscore rev */
	char	*vars;		/* phy attach time only copy of vars */
	uint16	vid;		/* vendorid */
	uint16	did;		/* deviceid */
	uint	chip;		/* chip number */
	uint	chiprev;	/* chip revision */
	uint	chippkg;	/* chip package option */
	uint	sromrev;	/* srom revision */
	uint	boardtype;	/* board type */
	uint	boardrev;	/* board revision */
	uint	boardvendor;	/* board vendor */
	uint32	boardflags;	/* board specific flags from srom */
	uint32	boardflags2;	/* more board flags if sromrev >= 4 */
} shared_phy_params_t;

/* parameter structure for wlc_phy_txpower_core_offset_get/set functions */
struct phy_txcore_pwr_offsets {
	int8 offset[PHY_MAX_CORES];	/* quarter dBm signed offset for each chain */
};

/* phy_tx_power_t.flags bits */
#define WL_TX_POWER_F_ENABLED	1
#define WL_TX_POWER_F_HW		2
#define WL_TX_POWER_F_MIMO		4
#define WL_TX_POWER_F_SISO		8
#define WL_TX_POWER_F_HT		0x10

typedef struct {
	uint32 flags;
	chanspec_t chanspec;		/* txpwr report for this channel */
	chanspec_t local_chanspec;	/* channel on which we are associated */
	uint8 rf_cores;				/* count of RF Cores being reported */
	uint8 display_core;			/* the displayed core in curpower */
	uint8 est_Pout[4];			/* Latest tx power out estimate per RF chain */
	uint8 est_Pout_act[4]; /* Latest tx power out estimate per RF chain w/o adjustment */
	uint8 est_Pout_cck;			/* Latest CCK tx power out estimate */
	uint8 tx_power_max[4];		/* Maximum target power among all rates */
	uint tx_power_max_rate_ind[4];		/* Index of the rate with the max target power */
	ppr_t *ppr_board_limits;
	ppr_t *ppr_target_powers;
#ifdef WL_SARLIMIT
	int8 SARLIMIT[PHY_MAX_CORES];
#endif // endif
} phy_tx_power_t;

#ifdef TXPWR_TIMING
extern uint32 hndrte_time_us(void);

extern uint wlc_phy_txpower_limit_set_time;
extern uint wlc_channel_reg_limits_time;
extern uint wlc_channel_update_txchain_offsets_time;
extern uint wlc_stf_ss_algo_channel_get_time;
extern uint temp1_time;
extern uint temp2_time;
#endif // endif

#ifdef WLTXPWR_CACHE
#ifdef WLC_LOW
extern void wlc_phy_clear_tx_power_offset(wlc_phy_t *pi);
#endif // endif
#endif /* WLTXPWR_CACHE */

/* attach/detach/init/deinit */
extern void wlc_beacon_loss_war_lcnxn(wlc_phy_t *ppi, uint8 enable);

extern void wlc_sslpnphy_noise_measure(wlc_phy_t *ppi);

extern shared_phy_t *wlc_phy_shared_attach(shared_phy_params_t *shp);
extern void  wlc_phy_shared_detach(shared_phy_t *phy_sh);
extern wlc_phy_t *wlc_phy_attach(shared_phy_t *sh, void *regs, int bandtype, char *vars);
extern void  wlc_phy_detach(wlc_phy_t *ppi);

extern bool wlc_phy_get_phyversion(wlc_phy_t *pih, uint16 *phytype, uint16 *phyrev,
	uint16 *radioid, uint16 *radiover);
extern bool wlc_phy_get_encore(wlc_phy_t *pih);
extern uint32 wlc_phy_get_coreflags(wlc_phy_t *pih);

extern void  wlc_phy_hw_clk_state_upd(wlc_phy_t *ppi, bool newstate);
extern void  wlc_phy_hw_state_upd(wlc_phy_t *ppi, bool newstate);
extern void  wlc_phy_init(wlc_phy_t *ppi, chanspec_t chanspec);
extern int32  wlc_phy_watchdog(wlc_phy_t *ppi);
extern int   wlc_phy_down(wlc_phy_t *ppi);
extern void wlc_phy_cal_init(wlc_phy_t *ppi);
extern void wlc_phy_antsel_init(wlc_phy_t *ppi, bool lut_init);
#ifdef WFD_PHY_LL
extern void wlc_phy_wfdll_chan_active(wlc_phy_t * ppi, bool chan_active);
#endif // endif

/* bandwidth/chanspec */
extern void wlc_phy_chanspec_set(wlc_phy_t *ppi, chanspec_t chanspec);
extern chanspec_t wlc_phy_chanspec_get(wlc_phy_t *ppi);
extern int32 wlc_phy_min_txpwr_limit_get(wlc_phy_t *ppi);
extern void wlc_phy_chanspec_radio_set(wlc_phy_t *ppi, chanspec_t newch);
extern uint16 wlc_phy_bw_state_get(wlc_phy_t *ppi);
extern void wlc_phy_bw_state_set(wlc_phy_t *ppi, uint16 bw);
extern bool wlc_phy_get_filt_war(wlc_phy_t *ppi);
extern void wlc_phy_set_filt_war(wlc_phy_t *ppi, bool filt_war);

/* states sync */
extern void wlc_phy_rssi_compute(wlc_phy_t *pih, void *ctx);
extern void wlc_phy_por_inform(wlc_phy_t *ppi);
extern void wlc_phy_noise_sample_intr(wlc_phy_t *ppi);
extern bool wlc_phy_bist_check_phy(wlc_phy_t *ppi);

extern void wlc_phy_set_deaf(wlc_phy_t *ppi, bool user_flag);
extern void wlc_phy_clear_deaf(wlc_phy_t *ppi, bool user_flag);
#if defined(BCMDBG_PHYDUMP)
#ifdef WLC_LOW_ONLY
extern void wlc_phy_dump_phy_regs(wlc_phy_t *ppi, struct bcmstrbuf *b, int *offset);
extern void wlc_phy_dump_radio_regs(wlc_phy_t *ppi, struct bcmstrbuf *b, bool nameon, int *offset);
#else
extern void wlc_phy_dump_phy_regs(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phy_dump_radio_regs(wlc_phy_t *ppi, struct bcmstrbuf *b, bool nameon);
#endif // endif
#endif /* WLC_LOW_ONLY */
#if defined(BCMDBG) || defined(BCMDBG_PHYDUMP) || defined(WLTEST)
extern void wlc_phydump_phycal(wlc_phy_t *ppi, struct bcmstrbuf *b);
#endif // endif
#if defined(BCMDBG) || defined(BCMDBG_PHYDUMP)
#if defined(DBG_PHY_IOV) || defined(BCMDBG_PHYDUMP)
#ifdef WLC_LOW_ONLY
extern int wlc_phydump_reg(wlc_phy_t *ppi, struct bcmstrbuf *b, int *offset);
extern int wlc_phydump_radioreg(wlc_phy_t *ppi, struct bcmstrbuf *b, int *offset);
#else
extern int wlc_phydump_reg(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern int wlc_phydump_radioreg(wlc_phy_t *ppi, struct bcmstrbuf *b);
#endif // endif
extern int wlc_phydump_tbl1(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern int wlc_phydump_tbl2(wlc_phy_t *ppi, struct bcmstrbuf *b);
#endif // endif
extern void wlc_phydump_aci(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phydump_papd(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phydump_state(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phydump_noise(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phydump_measlo(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phydump_lnagain(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phydump_initgain(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phydump_hpf1tbl(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phydump_lpphytbl0(wlc_phy_t *ppi, struct bcmstrbuf *b);
extern void wlc_phydump_chanest(wlc_phy_t *ppi, struct bcmstrbuf *b);
#ifdef ENABLE_FCBS
extern void wlc_phydump_fcbs(wlc_phy_t *ppi, struct bcmstrbuf *b);
#endif /* ENABLE_FCBS */
extern void wlc_phydump_txv0(wlc_phy_t *ppi, struct bcmstrbuf *b);
#endif // endif
#ifdef WLTEST
extern void wlc_phydump_ch4rpcal(wlc_phy_t *ppi, struct bcmstrbuf *b);
#endif /* WLTEST */
#if defined(DBG_BCN_LOSS)
extern void wlc_phydump_phycal_rx_min(wlc_phy_t *ppi, struct bcmstrbuf *b);
#endif // endif

extern void wlc_phy_switch_radio(wlc_phy_t *ppi, bool on);
extern void wlc_phy_anacore(wlc_phy_t *ppi, bool on);

/* flow */
extern void wlc_phy_BSSinit(wlc_phy_t *ppi, bool bonlyap, int noise);

#ifndef PHYMOD3_TRUNK_MERGE
extern int  wlc_phy_ioctl(wlc_phy_t *ppi, int cmd, int len, void *arg, bool *ta_ok);
extern int  wlc_phy_iovar_dispatch(wlc_phy_t *ppi, uint32 id, uint16, void *, uint, void *,
	int, int);
#endif /* PHYMOD3_TRUNK_MERGE */

#ifdef ENABLE_FCBS
/* Fast channel/band switch  (FCBS) function prototypes */
extern bool wlc_phy_fcbs_init(wlc_phy_t *ppi, int chanidx);
extern bool wlc_phy_fcbs_uninit(wlc_phy_t *ppi, chanspec_t chanspec);
extern int wlc_phy_fcbs(wlc_phy_t *ppi, int chanidx, bool set);
extern void wlc_phy_fcbs_exit(wlc_phy_t *ppi);
extern bool wlc_phy_fcbs_arm(wlc_phy_t *ppi, chanspec_t chanspec, int chanidx);
#endif /* ENABLE_FCBS */

/* chanspec */
extern void wlc_phy_chanspec_ch14_widefilter_set(wlc_phy_t *ppi, bool wide_filter);
extern void wlc_phy_chanspec_band_validch(wlc_phy_t *ppi, uint band, chanvec_t *channels);
extern chanspec_t wlc_phy_chanspec_band_firstch(wlc_phy_t *ppi, uint band);

extern void wlc_phy_txpower_sromlimit(wlc_phy_t *ppi, chanspec_t chanspec,
    uint8 *min_pwr, ppr_t *max_pwr, uint8 core);
extern void wlc_phy_txpower_sromlimit_max_get(wlc_phy_t *ppi, uint chan,
	uint8 *_max_, uint8 *_min_);
extern void wlc_phy_txpower_boardlimit_band(wlc_phy_t *ppi, uint band, int32 *, int32 *, uint32 *);
extern void wlc_phy_txpower_limit_set(wlc_phy_t *ppi, ppr_t* txpwr, chanspec_t chanspec);
extern int  wlc_phy_txpower_get(wlc_phy_t *ppi, uint *qdbm, bool *override);

extern int  wlc_phy_txpower_set(wlc_phy_t *ppi, uint qdbm, bool override, ppr_t *reg_pwr);
extern int  wlc_phy_neg_txpower_set(wlc_phy_t *ppi, uint qdbm);
extern bool wlc_phy_txpower_hw_ctrl_get(wlc_phy_t *ppi);
extern void wlc_phy_txpower_hw_ctrl_set(wlc_phy_t *ppi, bool hwpwrctrl);
extern uint8 wlc_phy_txpower_get_target_min(wlc_phy_t *ppi);
extern uint8 wlc_phy_txpower_get_target_max(wlc_phy_t *ppi);
extern bool wlc_phy_txpower_ipa_ison(wlc_phy_t *pih);
extern int  wlc_phy_txpower_core_offset_set(wlc_phy_t *ppi, struct phy_txcore_pwr_offsets *offsets);
extern int  wlc_phy_txpower_core_offset_get(wlc_phy_t *ppi, struct phy_txcore_pwr_offsets *offsets);

#ifdef WL_SARLIMIT
extern void wlc_phy_sar_limit_set(wlc_phy_t *pih, uint32 int_val);
#endif /* WL_SARLIMIT */

extern void wlc_phy_stf_chain_init(wlc_phy_t *pih, uint8 txchain, uint8 rxchain);
extern void wlc_phy_stf_chain_set(wlc_phy_t *pih, uint8 txchain, uint8 rxchain);
extern void wlc_phy_stf_chain_get(wlc_phy_t *pih, uint8 *txchain, uint8 *rxchain);
extern uint8 wlc_phy_rssi_ant_compare(wlc_phy_t *pih);
extern uint8 wlc_phy_stf_chain_active_get(wlc_phy_t *pih);
extern int8 wlc_phy_stf_ssmode_get(wlc_phy_t *pih, chanspec_t chanspec);
extern void wlc_phy_ldpc_override_set(wlc_phy_t *ppi, bool val);

extern void wlc_phy_mphase_cal_schedule(wlc_phy_t *pih, uint delay_val);
extern void wlc_phy_cal_perical(wlc_phy_t *ppi, uint8 reason);
extern int8 wlc_phy_noise_avg(wlc_phy_t *wpi);
extern int8 wlc_phy_noise_lte_avg(wlc_phy_t *wpi);
extern int8 wlc_phy_noise_avg_per_antenna(wlc_phy_t *pih, int coreidx);

extern void wlc_phy_noise_sample_request_external(wlc_phy_t *ppi);
extern void wlc_phy_noise_sample_request_crsmincal(wlc_phy_t *ppi);
extern void wlc_phy_interference_set(wlc_phy_t *ppi, bool init);
extern void wlc_phy_acimode_noisemode_reset(wlc_phy_t *ppi, uint channel,
	bool clear_aci_state, bool clear_noise_state, bool disassoc);
extern void wlc_phy_edcrs_lock(wlc_phy_t *pih, bool lock);
extern void wlc_phy_cal_papd_recal(wlc_phy_t *ppi);

/* misc */
extern int8 wlc_phy_preamble_override_get(wlc_phy_t *ppi);
extern void wlc_phy_preamble_override_set(wlc_phy_t *ppi, int8 override);
extern bool wlc_phy_swdiv_ant_set(wlc_phy_t *ppi, uint8 new_ant);
extern bool wlc_phy_swdiv_ant_get(wlc_phy_t *ppi, uint8 *cur_ant);
extern void wlc_phy_ant_rxdiv_set(wlc_phy_t *ppi, uint8 val);
extern bool wlc_phy_ant_rxdiv_get(wlc_phy_t *ppi, uint8 *pval);
extern void wlc_phy_clear_tssi(wlc_phy_t *ppi);
extern void wlc_phy_hold_upd(wlc_phy_t *ppi, mbool id, bool val);
extern bool wlc_phy_ismuted(wlc_phy_t *pih);
extern void wlc_phy_mute_upd(wlc_phy_t *ppi, bool val, mbool flags);
extern bool wlc_phy_get_tempsense_degree(wlc_phy_t *ppi, int8 *pval);

extern void wlc_phy_first_radar_indicator(wlc_phy_t *ppi);
extern void wlc_phy_radar_detect_enable(wlc_phy_t *ppi, bool on);
extern int  wlc_phy_radar_detect_run(wlc_phy_t *ppi);
extern int  wlc_phy_radar_status(wlc_phy_t *ppi);
extern void wlc_phy_radar_detect_mode_set(wlc_phy_t *pih, phy_radar_detect_mode_t mode);

extern void wlc_phy_antsel_type_set(wlc_phy_t *ppi, uint8 antsel_type);
extern bool wlc_phy_check_antdiv_enable_acphy(wlc_phy_t *ppi);
#if defined(PHYCAL_CACHING) || defined(WLMCHAN) || defined(WL_MODESW)
extern int  wlc_phy_cal_cache_init(wlc_phy_t *ppi);
extern void wlc_phy_cal_cache_deinit(wlc_phy_t *ppi);
extern int wlc_phy_create_chanctx(wlc_phy_t *ppi, chanspec_t chanspec);
extern void wlc_phy_destroy_chanctx(wlc_phy_t *ppi, chanspec_t chanspec);
extern void wlc_phy_display_chanctx(wlc_phy_t *ppi);
extern int wlc_phy_invalidate_chanctx(wlc_phy_t *ppi, chanspec_t chanspec);
extern int wlc_phy_reuse_chanctx(wlc_phy_t *ppi, chanspec_t chanspec);
extern void wlc_phy_get_cachedchans(wlc_phy_t *ppi, chanspec_t *chanlist);
extern void wlc_phy_get_all_cached_ctx(wlc_phy_t *ppi, chanspec_t *chanlist);
extern int32 wlc_phy_get_est_chanset_time(wlc_phy_t *ppi, chanspec_t chanspec);
extern bool wlc_phy_chan_iscached(wlc_phy_t *ppi, chanspec_t chanspec);
extern void wlc_phy_set_est_chanset_time(wlc_phy_t *ppi, chanspec_t chanspec,
	bool wascached,	bool inband, int32 rectime);
extern int8 wlc_phy_get_max_cachedchans(wlc_phy_t *ppi);
extern void wlc_phy_update_chctx_glacial_time(wlc_phy_t *pi, chanspec_t chanspec);
extern int wlc_phy_cal_cache_return(wlc_phy_t *ppi);
extern void wlc_phy_cal_cache(wlc_phy_t *ppi);
#ifdef WLOLPC
extern void wlc_phy_update_olpc_cal(wlc_phy_t *ppi, bool set, bool dbg);
#endif /* WLOLPC */
extern uint32 wlc_phy_get_current_cachedchans(wlc_phy_t *ppi);
#endif /* PHYCAL_CACHING || MCHAN */

#ifndef WLC_DISABLE_ACI
#ifdef WLPHYACICACHE
extern int wlc_phy_create_aci_chanctx(wlc_phy_t *ppi, chanspec_t chanspec);
extern int wlc_phy_destroy_aci_chanctx(wlc_phy_t *ppi, chanspec_t chanspec);
extern int wlc_phy_invalidate_aci_chanctx(wlc_phy_t *ppi, chanspec_t chanspec);
extern int wlc_phy_reuse_aci_chanctx(wlc_phy_t *ppi, chanspec_t chanspec);
extern void wlc_phy_do_aci_update_cache(wlc_phy_t *ppi, chanspec_t chanspec, bool entry);
extern void wlc_phy_display_aci_chanctx(wlc_phy_t *ppi);
#endif /* WLPHYACICACHE */
#endif /* WLC_DISABLE_ACI */

#if defined(PHYCAL_CACHING) || defined(PHYCAL_CACHE_SMALL)
extern void wlc_phy_cal_cache_set(wlc_phy_t *ppi, bool state);
extern bool wlc_phy_cal_cache_get(wlc_phy_t *ppi);
#endif // endif

extern void wlc_phy_cal_mode(wlc_phy_t *ppi, uint mode);
extern void wlc_phy_set_glacial_timer(wlc_phy_t *ppi, uint period);

#ifdef WLTEST
extern void wlc_phy_boardflag_upd(wlc_phy_t *phi);
#endif // endif
#if defined(WLTEST)
extern void wlc_phy_resetcntrl_regwrite(wlc_phy_t *phi);
#endif // endif

extern void wlc_phy_txpower_get_current(wlc_phy_t *ppi, ppr_t *reg_pwr, phy_tx_power_t *power);
void wlc_phy_get_est_pout(wlc_phy_t *ppi, uint8* est_Pout, uint8* est_Pout_adj,
	uint8* est_Pout_cck);
void wlc_phy_get_tssi_sens_min(wlc_phy_t *ppi, int8 *tssiSensMinPwr);

#ifdef BCMDBG
extern void wlc_phy_txpower_limits_dump(ppr_t* txpwr, bool isht);
#endif /* BCM_DBG */

extern void wlc_phy_initcal_enable(wlc_phy_t *pih, bool initcal);
extern bool wlc_phy_test_ison(wlc_phy_t *ppi);
extern void wlc_phy_txpwr_percent_set(wlc_phy_t *ppi, uint8 txpwr_percent);
#ifdef DSLCPE
extern void wlc_phy_txpwr_percent_mw_set(wlc_phy_t *ppi, uint8 txpwr_percent);
#endif
extern void wlc_phy_ofdm_rateset_war(wlc_phy_t *pih, bool war);
extern void wlc_phy_bf_preempt_enable(wlc_phy_t *pih, bool bf_preempt);
extern void wlc_phy_machwcap_set(wlc_phy_t *ppi, uint32 machwcap);

extern void wlc_phy_runbist_config(wlc_phy_t *ppi, bool start_end);

/* old GPHY stuff */
extern void wlc_phy_freqtrack_start(wlc_phy_t *ppi);
extern void wlc_phy_freqtrack_end(wlc_phy_t *ppi);

extern const uint8 * wlc_phy_get_ofdm_rate_lookup(void);

extern void wlc_phy_tkip_rifs_war(wlc_phy_t *ppi, uint8 rifs);

extern int8 wlc_phy_get_tx_power_offset_by_mcs(wlc_phy_t *ppi, uint8 mcs_offset);
extern int8 wlc_phy_get_tx_power_offset(wlc_phy_t *ppi, uint8 tbl_offset);
extern void wlc_phy_btclock_war(wlc_phy_t *ppi, bool enable);
extern uint32 wlc_phy_cap_get(wlc_phy_t *pi);
extern void wlc_phy_tx_pwr_limit_check(wlc_phy_t *pih);
#if defined(WLC_LOWPOWER_BEACON_MODE)
extern void wlc_phy_lowpower_beacon_mode(wlc_phy_t *pih, int lowpower_beacon_mode);
#endif /* WLC_LOWPOWER_BEACON_MODE */
#if defined(WLMEDIA_N2DEV) || defined(WLMEDIA_N2DBG) || defined(RXDESENS_EN)
extern int wlc_nphy_get_rxdesens(wlc_phy_t *ppi, int32 *ret_int_ptr);
extern int wlc_nphy_set_rxdesens(wlc_phy_t *ppi, int32 int_val);
#endif // endif
#if defined(WLMEDIA_N2DEV) || defined(WLMEDIA_N2DBG)
extern int wlc_nphy_get_lowtxpwr(wlc_phy_t *ppi, int32 *ret_int_ptr);
extern int wlc_nphy_set_lowtxpwr(wlc_phy_t *ppi, int32 int_val);
#endif // endif

#ifdef WLTEST
extern void wlc_lcnphy_iovar_samp_cap(wlc_phy_t *ppi, int32 gain, int32 *ret);
extern void wlc_phy_pkteng_boostackpwr(wlc_phy_t *ppi);
#endif // endif

#if defined(WLSRVSDB)

#define CHANNEL_UNDEFINED       0xEF
#define SR_MEMORY_BANK	2

#endif /* end WLSRVSDB */

extern void wlc_phy_reset_srvsdb_engine(wlc_phy_t *pi);
extern void wlc_phy_sr_vsdb_reset(wlc_phy_t *pi);
#ifdef WFD_PHY_LL
extern int wlc_phy_sr_vsdb_invalidate_active(wlc_phy_t * ppi);
#endif // endif
extern void wlc_phy_force_vsdb_chans(wlc_phy_t *pi, uint16 * chans, uint8 set);
extern void wlc_phy_detach_srvsdb_module(wlc_phy_t *pi);
extern uint8 wlc_phy_attach_srvsdb_module(wlc_phy_t *ppi, chanspec_t chan0, chanspec_t chan1);
extern uint8 wlc_set_chanspec_sr_vsdb(wlc_phy_t *pi, chanspec_t chanspec, uint8 * last_chan_saved);

#ifdef BCMDBG
extern void wlc_acphy_txerr_dump(uint16 PhyErr);
extern void wlc_htphy_txerr_dump(uint16 PhyErr);
#endif /* BCMDBG */

#ifdef WL_LPC
/* Remove the following #defs once branches have been updated */
#define wlc_phy_pwrsel_getminidx	wlc_phy_lpc_getminidx
#define wlc_phy_pwrsel_getoffset	wlc_phy_lpc_getoffset
#define wlc_phy_pwrsel_get_txcpwrval	wlc_phy_lpc_get_txcpwrval
#define wlc_phy_pwrsel_set_txcpwrval	wlc_phy_lpc_set_txcpwrval
#define wlc_phy_powersel_algo_set	wlc_phy_lpc_algo_set

uint8 wlc_phy_lpc_getminidx(wlc_phy_t *ppi);
uint8 wlc_phy_lpc_getoffset(wlc_phy_t *ppi, uint8 index);
uint8 wlc_phy_lpc_get_txcpwrval(wlc_phy_t *ppi, uint16 phytxctrlword);
void wlc_phy_lpc_set_txcpwrval(wlc_phy_t *ppi, uint16 *phytxctrlword, uint8 txcpwrval);
void wlc_phy_lpc_algo_set(wlc_phy_t *ppi, bool enable);
#ifdef WL_LPC_DEBUG
uint8 * wlc_phy_lpc_get_pwrlevelptr(wlc_phy_t *ppi);
#endif // endif
#endif /* WL_LPC */
#ifdef DSLCPE_C601911
extern void wlc_phy_need_reinit(wlc_phy_t *pih);
#endif
extern void wlc_phy_block_bbpll_change(wlc_phy_t *pih, bool block, bool going_down);
extern void wlc_acphy_set_scramb_dyn_bw_en(wlc_phy_t *pi, bool enable);

extern void wlc_phy_interf_rssi_update(wlc_phy_t *pi, chanspec_t chanNum, int8 leastRSSI);
extern void wlc_phy_interf_chan_stats_update(wlc_phy_t *pi, chanspec_t chanspec, uint32 crsglitch,
        uint32 bphy_crsglitch, uint32 badplcp, uint32 bphy_badplcp, uint32 mbsstime);

extern void wlc_acphy_set_scramb_dyn_bw_en(wlc_phy_t *pi, bool enable);
extern void wlc_phy_set_femctrl_bt_wlan_ovrd(wlc_phy_t *pih, int8 state);
extern int8 wlc_phy_get_femctrl_bt_wlan_ovrd(wlc_phy_t *pih);
extern void wlc_phy_set_locale(wlc_phy_t *ppi, uint8 region_group);
extern uint8 wlc_phy_get_locale(wlc_phy_t *ppi);

#ifdef	WL_DYNAMIC_TEMPSENSE
extern int wlc_phy_current_temperature(wlc_phy_t *pih);
extern int wlc_phy_temperature_threshold(wlc_phy_t *pih);
#if defined(BCMDBG) || defined(WLTEST)
extern int wlc_phy_temperature_override(wlc_phy_t *pih);
#endif // endif
#endif /* WL_DYNAMIC_TEMPSENSE */

#ifdef BCM_OL_DEV
extern void wlc_phy_sarlimit_write(wlc_phy_t *ppi, uint32 sar);
extern void wlc_phy_curpwr_write(wlc_phy_t *ppi, uint8 curpwr);
#endif /* BCM_OL_DEV */
extern int wlc_phy_tssivisible_thresh(wlc_phy_t *pih);

extern void wlc_phy_clear_match_tx_offset(wlc_phy_t *ppi, ppr_t *pprptr);
#ifdef RXDESENS_EN
extern void wlc_phy_rxdesens(wlc_phy_t *ppi, bool desens_en);
#ifdef WLSRVSDB
extern bool wlc_phy_rxdesens_defer(wlc_phy_t *ppi);
#endif // endif
#endif /* RXDESENS_EN */
#ifdef WLMCHAN
extern bool wlc_phy_acimode_noisemode_reset_allowed(wlc_phy_t *ppi, uint16 HomeChannel);
extern bool wlc_phy_srvsdb_switch_status(wlc_phy_t *ppi);
#endif // endif
#ifdef ATE_BUILD
void wlc_phy_gpaio(wlc_phy_t *ppi, wl_gpaio_option_t option);
void wlc_ate_gpiosel_disable(wlc_phy_t *ppi);
#endif // endif
#ifdef WLC_TXDIVERSITY
uint8 wlc_phy_get_oclscdenable_status(wlc_phy_t *pih);
#endif /* WLC_TXDIVERSITY */
extern bool wlc_phy_is_txbfcal(wlc_phy_t *ppi);
extern void wlc_phy_update_link_rssi(wlc_phy_t *pih, int8 rssi);
#ifdef WL_PROXDETECT
extern int wlc_phy_tof(wlc_phy_t *ppi, bool enter, bool hw_adj);
extern int wlc_phy_chan_freq_response(wlc_phy_t *ppi,
	int len, int nbits, int32* Hr, int32* Hi, uint32* Hraw);
extern int wlc_phy_chan_mag_sqr_impulse_response(wlc_phy_t *ppi, int frame_type,
	int len, int offset, int nbits, int32* h, int* pgd, uint32* hraw, uint16 tof_shm_ptr);
extern int wlc_phy_tof_info(wlc_phy_t *ppi, int* p_frame_type, int* p_frame_bw, int8* p_rssi);
extern void wlc_phy_tof_cmd(wlc_phy_t *ppi, bool seq);
extern int wlc_phy_tof_kvalue(wlc_phy_t *ppi, chanspec_t chanspec, uint32 *kip, uint32 *ktp);
#endif // endif
#if (defined(WLTEST) || defined(WLPKTENG))
extern bool wlc_phy_isperratedpden(wlc_phy_t *ppi);
extern void wlc_phy_perratedpdset(wlc_phy_t *ppi, bool enable);
#endif // endif
#if defined(WLTEST)
extern void
wlc_phy_pkteng_rxstats_update(wlc_phy_t *ppi, uint8 statidx);
#endif // endif

#ifdef WLTXPWR_CACHE
tx_pwr_cache_entry_t* wlc_phy_get_txpwr_cache(wlc_phy_t *ppi);
#if !defined(WLC_LOW_ONLY) && !defined(WLTXPWR_CACHE_PHY_ONLY)
void wlc_phy_set_txpwr_cache(wlc_phy_t *ppi, tx_pwr_cache_entry_t* cacheptr);
#endif // endif
#endif	/* WLTXPWR_CACHE */

extern uint8 wlc_phy_get_txpwr_backoff(wlc_phy_t *ppi);
extern void wlc_phy_get_txpwr_min(wlc_phy_t *ppi, uint8 *min_pwr);
extern void wlc_phy_reset_iqest_blk(wlc_phy_t *pih);
#endif	/* _wlc_phy_h_ */
extern void wlc_phy_radar_detect_on_off_cfg(wlc_phy_t *ppi, bool radar_on);
