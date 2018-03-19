/*
 * wl ht command module
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
 * $Id: wluc_ht.c 458728 2014-02-27 18:15:25Z $
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
#include "wlu_common.h"
#include "wlu.h"

#include <miniopt.h>

static cmd_func_t wl_bw_cap;
static cmd_func_t wl_nrate;
static cmd_func_t wl_cur_mcsset;
static cmd_func_t wl_txmcsset;
static cmd_func_t wl_rxmcsset;

static cmd_t wl_ht_cmds[] = {
	{ "bw_cap", wl_bw_cap, WLC_GET_VAR, WLC_SET_VAR,
	"Get/set the per-band bandwidth.\n"
	"Usage: wl bw_cap <2g|5g> [<cap>]\n"
	"\t2g|5g - Band: 2.4GHz or 5GHz respectively\n"
	"cap:\n"
	"\t0x1 - 20MHz\n"
	"\t0x3 - 20/40MHz\n"
	"\t0x7 - 20/40/80MHz\n"
	"\t0xff - Unrestricted" },
	{ "cur_mcsset", wl_cur_mcsset, WLC_GET_VAR, -1,
	"Get the current mcs set"
	},
	{ "mimo_ps", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"get/set mimo power save mode, (0=Dont send MIMO, 1=proceed MIMO with RTS, 2=N/A, "
	"3=No restriction)"},
	{ "ofdm_txbw", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"get/set ofdm txbw (2=20Mhz(lower), 3=20Mhz upper, 4(not allowed), 5=40Mhz dup)"},
	{ "cck_txbw", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"get/set cck txbw (2=20Mhz(lower), 3=20Mhz upper)"},
	{ "frameburst", wl_int, WLC_GET_FAKEFRAG, WLC_SET_FAKEFRAG,
	"Disable/Enable frameburst mode" },
	{ "frameburst_txop", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"get/set maximum txop limit for frameburst in usec" },
	{ "nrate", wl_nrate, WLC_GET_VAR, WLC_SET_VAR,
	"\"auto\" to clear a rate override, or:\n"
	"-r legacy rate (CCK, OFDM)\n"
	"-m HT MCS index\n"
	"-s stf mode (0=SISO,1=CDD,2=STBC,3=SDM)\n"
	"-w Override MCS only to support STA's with/without STBC capability"},
	{ "mimo_txbw", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"get/set mimo txbw (2=20Mhz(lower), 3=20Mhz upper, 4=40Mhz, 4=40Mhz(DUP)\n"
	"\t6=80Mhz(20LL), 7=80Mhz(20LU), 8=80Mhz(20UL), 9=80Mhz(20UU)\n"
	"\t10=80Mhz(40L), 11=80Mhz(40U), 12=80Mhz)"},
	{ "txmcsset", wl_txmcsset, WLC_GET_VAR, -1, "get Transmit MCS rateset for 11N device"},
	{ "rxmcsset", wl_rxmcsset, WLC_GET_VAR, -1, "get Receive MCS rateset for 11N device"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* IOCtl version read from targeted driver */
static int ioctl_version;

/* module initialization */
void
wluc_ht_module_init(void)
{
	(void)g_swap;

	/* get global ioctl_version */
	ioctl_version = wl_get_ioctl_version();

	/* get the global buf */
	buf = wl_get_buf();

	/* register ht commands */
	wl_module_cmds_register(wl_ht_cmds);
}

/* Format a ratespec for "nrate" output
 * Will handle both current wl_ratespec and legacy (ioctl_version 1) nrate ratespec
 */
void
wl_nrate_print(uint32 rspec)
{
	const char * rspec_auto = "auto";
	uint encode, rate, txexp = 0, bw_val;
	const char* stbc = "";
	const char* ldpc = "";
	const char* sgi = "";
	const char* bw = "";
	int stf;

	if (rspec == 0) {
		encode = WL_RSPEC_ENCODE_RATE;
	} else if (ioctl_version == 1) {
		encode = (rspec & OLD_NRATE_MCS_INUSE) ? WL_RSPEC_ENCODE_HT : WL_RSPEC_ENCODE_RATE;
		stf = (int)((rspec & OLD_NRATE_STF_MASK) >> OLD_NRATE_STF_SHIFT);
		rate = (rspec & OLD_NRATE_RATE_MASK);

		if (rspec & OLD_NRATE_OVERRIDE) {
			if (rspec & OLD_NRATE_OVERRIDE_MCS_ONLY)
				rspec_auto = "fixed mcs only";
			else
				rspec_auto = "fixed";
		}
	} else {
		int siso;
		encode = (rspec & WL_RSPEC_ENCODING_MASK);
		rate = (rspec & WL_RSPEC_RATE_MASK);
		txexp = (rspec & WL_RSPEC_TXEXP_MASK) >> WL_RSPEC_TXEXP_SHIFT;
		stbc  = ((rspec & WL_RSPEC_STBC) != 0) ? " stbc" : "";
		ldpc  = ((rspec & WL_RSPEC_LDPC) != 0) ? " ldpc" : "";
		sgi   = ((rspec & WL_RSPEC_SGI)  != 0) ? " sgi"  : "";
		bw_val = (rspec & WL_RSPEC_BW_MASK);

		if (bw_val == WL_RSPEC_BW_20MHZ) {
			bw = "bw20";
		} else if (bw_val == WL_RSPEC_BW_40MHZ) {
			bw = "bw40";
		} else if (bw_val == WL_RSPEC_BW_80MHZ) {
			bw = "bw80";
		} else if (bw_val == WL_RSPEC_BW_160MHZ) {
			bw = "bw160";
		}
		else if (bw_val == WL_RSPEC_BW_10MHZ) {
			bw = "bw10";
		} else if (bw_val == WL_RSPEC_BW_5MHZ) {
			bw = "bw5";
		} else if (bw_val == WL_RSPEC_BW_2P5MHZ) {
			bw = "bw2.5";
		}

		/* initialize stf mode to an illegal value and
		 * fix to a backward compatable value if possible
		 */
		stf = -1;
		/* for stf calculation, determine if the rate is single stream.
		 * Legacy rates WL_RSPEC_ENCODE_RATE are single stream, and
		 * HT rates for mcs 0-7 are single stream
		 */
		siso = (encode == WL_RSPEC_ENCODE_RATE) ||
			((encode == WL_RSPEC_ENCODE_HT) && rate < 8);

		/* calc a value for nrate stf mode */
		if (txexp == 0) {
			if ((rspec & WL_RSPEC_STBC) && siso) {
				/* STF mode STBC */
				stf = OLD_NRATE_STF_STBC;
			} else {
				/* STF mode SISO or SDM */
				stf = (siso) ? OLD_NRATE_STF_SISO : OLD_NRATE_STF_SDM;
			}
		} else if (txexp == 1 && siso) {
			/* STF mode CDD */
			stf = OLD_NRATE_STF_CDD;
		}

		if (rspec & WL_RSPEC_OVERRIDE_RATE) {
			rspec_auto = "fixed";
		}
	}

	if (encode == WL_RSPEC_ENCODE_RATE) {
		if (rspec == 0) {
			printf("auto\n");
		} else {
			printf("legacy rate %d%s Mbps stf mode %d %s\n",
			       rate/2, (rate % 2)?".5":"", stf, rspec_auto);
		}
	} else if (encode == WL_RSPEC_ENCODE_HT) {
		printf("mcs index %d stf mode %d %s\n",
		       rate, stf, rspec_auto);
	} else {
		uint vht = (rspec & WL_RSPEC_VHT_MCS_MASK);
		uint Nss = (rspec & WL_RSPEC_VHT_NSS_MASK) >> WL_RSPEC_VHT_NSS_SHIFT;
		printf("vht mcs %d Nss %d Tx Exp %d %s%s%s%s %s\n",
		       vht, Nss, txexp, bw, stbc, ldpc, sgi, rspec_auto);
	}
}

static int
wl_nrate(void *wl, cmd_t *cmd, char **argv)
{
	miniopt_t to;
	const char* fn_name = "wl_nrate";
	bool mcs_set = FALSE, legacy_set = FALSE, stf_set = FALSE;
	bool mcs_only = FALSE;
	int err, opt_err;
	uint32 val = 0;
	uint32 rate = 0;
	uint32 nrate = 0;
	uint32 mcs = 0;
	uint stf = 0;	/* (0=SISO,1=CDD,2=STBC,3=SDM) */

	/* toss the command name */
	argv++;

	if (!*argv) {
		if (cmd->get < 0)
			return -1;
		if ((err = wlu_iovar_getint(wl, "nrate", (int*)&val)) < 0)
			return err;

		/* print a user readable line for the nrate rspec */
		wl_nrate_print(val);

		return 0;
	}

	/* check for a single argument of "auto" or -1 */
	if ((!strcmp(argv[0], "auto") || !strcmp(argv[0], "-1")) &&
	    argv[1] == NULL) {
		/* clear the nrate override */
		err = wlu_iovar_setint(wl, "nrate", 0);
		goto exit;
	}

	miniopt_init(&to, fn_name, "w", FALSE);
	while ((opt_err = miniopt(&to, argv)) != -1) {
		if (opt_err == 1) {
			err = BCME_USAGE_ERROR;
			goto exit;
		}
		argv += to.consumed;

		if (to.opt == 'r') {
			if (!to.good_int) {
				/* special case check for "-r 5.5" */
				if (!strcmp(to.valstr, "5.5")) {
					to.val = 11;
				} else {
					fprintf(stderr,
						"%s: could not parse \"%s\" as a rate value\n",
						fn_name, to.valstr);
					err = BCME_BADARG;
					goto exit;
				}
			} else
			      to.val = to.val*2;
			if (mcs_set) {
				fprintf(stderr, "%s: cannot use -r and -m\n", fn_name);
				err = BCME_USAGE_ERROR;
				goto exit;
			}

			legacy_set = TRUE;
			rate = to.val;
		}
		if (to.opt == 'm') {
			if (!to.good_int) {
				fprintf(stderr,
					"%s: could not parse \"%s\" as an int for mcs\n",
					fn_name, to.valstr);
				err = BCME_BADARG;
				goto exit;
			}
			if (legacy_set) {
				fprintf(stderr, "%s: cannot use -r and -m\n", fn_name);
				err = BCME_USAGE_ERROR;
				goto exit;
			}
			mcs_set = TRUE;
			mcs = to.val;
		}
		if (to.opt == 's') {
			if (!to.good_int) {
				fprintf(stderr,
					"%s: could not parse \"%s\" as an int for stf mode\n",
					fn_name, to.valstr);
				err = BCME_BADARG;
				goto exit;
			}
			stf = to.val;
			stf_set = TRUE;
		}
		if (to.opt == 'w') {
			mcs_only = TRUE;
		}
	}

	if ((mcs_only && !mcs_set) || (mcs_only && (stf_set || legacy_set))) {
		fprintf(stderr, "%s: can use -w only with -m\n", fn_name);
		err = BCME_USAGE_ERROR;
		goto exit;
	}

	if (!stf_set) {
		if (legacy_set)
			stf = OLD_NRATE_STF_SISO;	/* SISO */
		else if (mcs_set) {
			if (GET_11N_MCS_NSS(mcs & OLD_NRATE_RATE_MASK) == 1)
				stf = OLD_NRATE_STF_SISO;	/* SISO */
			else
				stf = OLD_NRATE_STF_SDM;	/* SDM */
		}
	}

	if (!legacy_set && !mcs_set) {
		fprintf(stderr, "%s: you need to set a legacy or mcs rate\n", fn_name);
		err = BCME_USAGE_ERROR;
		goto exit;
	}

	if (ioctl_version == 1) {
		if (legacy_set) {
			nrate = rate;
		} else {
			nrate = mcs;
			nrate |= OLD_NRATE_MCS_INUSE;
			if (mcs_only) {
				nrate |= OLD_NRATE_OVERRIDE_MCS_ONLY;
			}
		}

		nrate |= (stf << OLD_NRATE_STF_SHIFT) & OLD_NRATE_STF_MASK;
	} else {
		uint tx_exp = 0;

		/* set the ratespec encoding type and basic rate value */
		if (legacy_set) {
			nrate = WL_RSPEC_ENCODE_RATE;	/* 11abg */
			nrate |= rate;
		} else {
			nrate = WL_RSPEC_ENCODE_HT;	/* 11n HT */
			nrate |= mcs;
		}

		/* decode nrate stf value into tx expansion and STBC */
		if (stf == OLD_NRATE_STF_CDD) {
			tx_exp = 1;
		} else if (stf == OLD_NRATE_STF_STBC) {
			nrate |= WL_RSPEC_STBC;
		}

		nrate |= (tx_exp << WL_RSPEC_TXEXP_SHIFT);
	}

	err = wlu_iovar_setint(wl, "nrate", (int)nrate);

exit:
	return err;
} /* wl_nrate */

/* Set per-band bandwidth */
static int wl_bw_cap(void *wl, cmd_t *cmd, char **argv)
{
	int err = 0;
	struct {
		uint32 band;
		uint32 bw_cap;
	} param = { 0, 0 };
	char *s = NULL;
	void *ptr = NULL;

	/* Skip the command name */
	argv++;

	if (*argv) {
		if (!strcmp(*argv, "a") || !strcmp(*argv, "5") || !strcmp(*argv, "5g")) {
#ifdef DSLCPE_ENDIAN
			param.band =htod32(WLC_BAND_5G);
#else
			param.band = WLC_BAND_5G;
#endif
		} else if (!strcmp(*argv, "b") || !strcmp(*argv, "2") || !strcmp(*argv, "2g")) {
#ifdef DSLCPE_ENDIAN
			param.band = htod32(WLC_BAND_2G);
#else
			param.band = WLC_BAND_2G;
#endif
		} else {
			fprintf(stderr,
			        "%s: invalid band %s\n",
			        cmd->name, *argv);
			err = BCME_USAGE_ERROR;
			goto exit;
		}

		argv++;

		if (*argv) {
			/* Optional 2nd arg is used to set the bandwidth cap */
			s = NULL;
#ifdef DSLCPE_ENDIAN
			param.bw_cap = htod32((uint32) strtoul(*argv, &s, 0));
#else
			param.bw_cap = (uint32) strtoul(*argv, &s, 0);
#endif
			if (s && *s != '\0') {
				fprintf(stderr, "%s: invalid bandwidth '%s'\n",
				        cmd->name, *argv);
				err = BCME_USAGE_ERROR;
				goto exit;
			}
		}
	} else {
		fprintf(stderr, "%s: band unspecified\n", cmd->name);
		err = BCME_USAGE_ERROR;
		goto exit;
	}

	if (param.bw_cap == 0) {
		if ((err = wlu_var_getbuf(wl, cmd->name, &param, sizeof(param), &ptr)) < 0)
			return err;
#ifdef DSLCPE_ENDIAN
		printf("0x%x\n", dtoh32(*((uint32 *)ptr)));
#else
		printf("0x%x\n", *((uint32 *)ptr));
#endif
	} else {
		err = wlu_var_setbuf(wl, cmd->name, &param, sizeof(param));
	}

exit:
	return err;
}

static int
wl_cur_mcsset(void *wl, cmd_t *cmd, char **argv)
{
	int ret;

	UNUSED_PARAMETER(cmd);
	UNUSED_PARAMETER(argv);

	memset(buf, 0, WLC_IOCTL_SMLEN);
	ret = wlu_iovar_get(wl, "cur_mcsset", &buf[0], MCSSET_LEN);
	if (ret < 0)
		return ret;

	wl_print_mcsset(buf);

	return ret;
}

static int
wl_txmcsset(void *wl, cmd_t *cmd, char **argv)
{
	int err;

	if ((err = wl_var_get(wl, cmd, argv)) < 0)
		return err;
	wl_print_mcsset(buf);

	return err;
}

static int
wl_rxmcsset(void *wl, cmd_t *cmd, char **argv)
{
	int err;

	if ((err = wl_var_get(wl, cmd, argv)) < 0)
		return err;

	wl_print_mcsset(buf);

	return err;
}
