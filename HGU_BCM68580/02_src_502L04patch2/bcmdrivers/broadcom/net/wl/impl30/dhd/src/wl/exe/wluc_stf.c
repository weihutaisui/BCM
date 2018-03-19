/*
 * wl stf command module
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
 * $Id: wluc_stf.c 458728 2014-02-27 18:15:25Z $
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

#include "wlu_rates_matrix.h"

static void wl_txppr_print(ppr_t *ppr, int cck, uint flags);
static void wl_txppr_print_bw(ppr_t *ppr, int cck, uint flags, wl_tx_bw_t bw);
static cmd_func_t wl_get_current_txppr;
static cmd_func_t wl_txcore;
static cmd_func_t wl_txcore_pwr_offset;
static int wl_mimo_stf(void *wl, cmd_t *cmd, char **argv);
static cmd_func_t wl_spatial_policy, wl_ratetbl_ppr;

static cmd_t wl_stf_cmds[] = {
	{ "curppr", wl_get_current_txppr, WLC_GET_VAR, -1,
	"Return current tx power per rate offset."},
	{ "txcore", wl_txcore, WLC_GET_VAR, WLC_SET_VAR,
	"Usage: wl txcore -k <CCK core mask> -o <OFDM core mask> -s <1..4> -c <core bitmap>\n"
	"\t-k CCK core mask\n"
	"\t-o OFDM core mask\n"
	"\t-s # of space-time-streams\n"
	"\t-c active core (bitmask) to be used when transmitting frames"
	},
	{ "txcore_override", wl_txcore, WLC_GET_VAR, -1,
	"Usage: wl txcore_override\n"
	"\tget the user override of txcore"
	},
	{ "txchain_pwr_offset", wl_txcore_pwr_offset, WLC_GET_VAR, WLC_SET_VAR,
	"Usage: wl txchain_pwr_offset [qdBm offsets]\n"
	"\tGet/Set the current offsets for each core in qdBm (quarter dBm)"
	},
	{ "mimo_ss_stf", wl_mimo_stf, WLC_GET_VAR, WLC_SET_VAR,
	"get/set SS STF mode.\n"
	"\tUsage: wl mimo_ss_stf <value> <-b a | b>\n"
	"\tvalue: 0 - SISO; 1 - CDD\n"
	"\t-b(band): a - 5G; b - 2.4G"},
	{ "spatial_policy", wl_spatial_policy, WLC_GET_VAR, WLC_SET_VAR,
	"set/get spatial_policy\n"
	"\tUsage: wl spatial_policy <-1: auto / 0: turn off / 1: turn on>\n"
	"\t       to control individual band/sub-band use\n"
	"\t       wl spatial_policy a b c d e\n"
	"\t       where a is 2.4G band setting\n"
	"\t       where b is 5G lower band setting\n"
	"\t       where c is 5G middle band setting\n"
	"\t       where d is 5G high band setting\n"
	"\t       where e is 5G upper band setting"},
	{ "ratetbl_ppr", wl_ratetbl_ppr, WLC_GET_VAR, WLC_SET_VAR,
	"Usage: For get: wl ratetbl_ppr\n"
	"\t     For set: wl ratetbl_ppr <rate> <ppr>" },
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_stf_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register stf commands */
	wl_module_cmds_register(wl_stf_cmds);
}

static void
wl_txppr_print(ppr_t *ppr, int cck, uint flags)
{

	switch (ppr_get_ch_bw(ppr)) {
	case WL_TX_BW_20:
		printf("\n20MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_20);
		break;
	case WL_TX_BW_40:
		printf("\n20 in 40MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_20IN40);
		printf("\n40MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_40);
		break;
	case WL_TX_BW_80:
		printf("\n20 in 80MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_20IN80);
		printf("\n40 in 80MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_40IN80);
		printf("\n80MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_80);
		break;
	case WL_TX_BW_160:
		printf("\n20 in 160MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_20IN160);
		printf("\n40 in 160MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_40IN160);
		printf("\n80 in 160MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_80IN160);
		printf("\n160MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_160);
		break;
	case WL_TX_BW_8080:
		printf("\n20 in 80+80MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_20IN8080);
		printf("\n40 in 80+80MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_40IN8080);
		printf("\n80 in 80+80MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_80IN8080);
		printf("\nchan1 80+80MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_8080);
		printf("\nchan2 80+80MHz:\n");
		wl_txppr_print_bw(ppr, cck, flags, WL_TX_BW_8080CHAN2);
		break;
	default:
		break;
	}
	/* MCS32 value is obsoleted */
	/* printf("MCS32        %2d\n", ppr->mcs32); */
	printf("\n");
}

#define PRINT_PPR_RATE_LOOP(idx, len, rates)		  \
			for (idx = 0; idx < len; idx++) { \
				if (rates[idx] == WL_RATE_DISABLED) \
					printf("  -"); \
				else \
					printf(" %2d", rates[idx]); \
			}

/* print power offset for for a given bandwidth */
static void
wl_txppr_print_bw(ppr_t *ppr, int cck, uint flags, wl_tx_bw_t bw)
{
	uint i, j, rlen;
	uint n = WL_NUM_2x2_ELEMENTS;
	uint offset = 0;
	int8 *ptr, *vhtptr;
	const char *str = "";
	bool siso = ((flags & WL_TX_POWER_F_MIMO) == 0);
	bool vht = ((flags & WL_TX_POWER_F_VHT) != 0);
	ppr_ofdm_rateset_t ofdm_rate;
	ppr_vht_mcs_rateset_t vhtrate;

	if (!siso) {
		offset = WL_NUM_3x3_ELEMENTS;
		n = WL_NUM_4x4_ELEMENTS;
	}

	if (cck) {
		ppr_dsss_rateset_t rate;
		ppr_get_dsss(ppr, bw, WL_TX_CHAINS_1, &rate);
		printf("CCK         ");
		PRINT_PPR_RATE_LOOP(j, WL_RATESET_SZ_DSSS, rate.pwr);
		if (!siso) {
			for (i = WL_TX_CHAINS_2; i <= WL_TX_CHAINS_4; i++) {
				ppr_get_dsss(ppr, bw, i, &rate);
				printf("\nCCK CDD 1x%d ", i);
				PRINT_PPR_RATE_LOOP(j, WL_RATESET_SZ_DSSS, rate.pwr);
			}
		}
	}
	ppr_get_ofdm(ppr, bw, WL_TX_MODE_NONE, WL_TX_CHAINS_1, &ofdm_rate);
	printf("\nOFDM        ");
	PRINT_PPR_RATE_LOOP(j, WL_RATESET_SZ_OFDM, ofdm_rate.pwr);
	for (i = WL_TX_CHAINS_2; i <= WL_TX_CHAINS_4; i++) {
		ppr_get_ofdm(ppr, bw, WL_TX_MODE_CDD, i, &ofdm_rate);
		printf("\nOFDM-CDD 1x%d", i);
		PRINT_PPR_RATE_LOOP(j, WL_RATESET_SZ_OFDM, ofdm_rate.pwr);
	}
	printf("\n");
	for (i = 0; i < n; i++) {
		wl_tx_nss_t nss;
		wl_tx_mode_t mode;
		wl_tx_chains_t chains;
		switch (i + offset) {
			case 0:
				str = "MCS-SISO   ";
				nss = WL_TX_NSS_1;
				mode = WL_TX_MODE_NONE;
				chains = WL_TX_CHAINS_1;
				ptr = vhtrate.pwr;
				vhtptr = NULL;
				break;
			case 1:
				str = "MCS-CDD    ";
				nss = WL_TX_NSS_1;
				mode = WL_TX_MODE_CDD;
				chains = WL_TX_CHAINS_2;
				ptr = vhtrate.pwr;
				vhtptr = NULL;
				break;
			case 2:
				str = "MCS STBC   ";
				nss = WL_TX_NSS_1;
				mode = WL_TX_MODE_STBC;
				chains = WL_TX_CHAINS_2;
				ptr = vhtrate.pwr;
				vhtptr = NULL;
				break;
			case 3:
				str = "MCS 8~15   ";
				nss = WL_TX_NSS_2;
				mode = WL_TX_MODE_NONE;
				chains = WL_TX_CHAINS_2;
				ptr = vhtrate.pwr;
				vhtptr = NULL;
				break;
			case 4:
			case 5:
				ptr = NULL;
				vhtptr = NULL;
				break;
			case 6:
				str = "1 Nsts 1 Tx";
				nss = WL_TX_NSS_1;
				mode = WL_TX_MODE_NONE;
				chains = WL_TX_CHAINS_1;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;
			case 7:
				str = "1 Nsts 2 Tx";
				nss = WL_TX_NSS_1;
				mode = WL_TX_MODE_CDD;
				chains = WL_TX_CHAINS_2;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;
			case 8:
				str = "1 Nsts 3 Tx";
				nss = WL_TX_NSS_1;
				mode = WL_TX_MODE_CDD;
				chains = WL_TX_CHAINS_3;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;
			case 9:
				str = "1 Nsts 4 Tx";
				nss = WL_TX_NSS_1;
				mode = WL_TX_MODE_CDD;
				chains = WL_TX_CHAINS_4;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;

			case 10:
				str = "2 Nsts 2 Tx";
				nss = WL_TX_NSS_2;
				mode = WL_TX_MODE_NONE;
				chains = WL_TX_CHAINS_2;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;
			case 11:
				str = "2 Nsts 3 Tx";
				nss = WL_TX_NSS_2;
				mode = WL_TX_MODE_NONE;
				chains = WL_TX_CHAINS_3;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;
			case 12:
				str = "2 Nsts 4 Tx";
				nss = WL_TX_NSS_2;
				mode = WL_TX_MODE_NONE;
				chains = WL_TX_CHAINS_4;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;
			case 13:
				str = "3 Nsts 3 Tx";
				nss = WL_TX_NSS_3;
				mode = WL_TX_MODE_NONE;
				chains = WL_TX_CHAINS_3;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;
			case 14:
				str = "3 Nsts 4 Tx";
				nss = WL_TX_NSS_3;
				mode = WL_TX_MODE_NONE;
				chains = WL_TX_CHAINS_4;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;
			case 15:
				str = "4 Nsts 4 Tx";
				nss = WL_TX_NSS_4;
				mode = WL_TX_MODE_NONE;
				chains = WL_TX_CHAINS_4;
				ptr = vhtrate.pwr;
				vhtptr = &vhtrate.pwr[8];
				break;
			default:
				ptr = NULL;
				vhtptr = NULL;
				break;
		}
		if (ptr == NULL)
			continue;
		ppr_get_vht_mcs(ppr, bw, nss, mode, chains, &vhtrate);
		printf("%s ", str);
		if (vht && vhtptr)
			rlen = WL_RATESET_SZ_VHT_MCS_P;
		else
			rlen = WL_RATESET_SZ_HT_MCS;
		PRINT_PPR_RATE_LOOP(j, rlen, ptr);
		printf("\n");
	}
}

static int
wl_get_current_txppr(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	uint flags;
	chanspec_t chanspec;
	char chanspec_str[CHANSPEC_STR_LEN];
	uint pprsize = ppr_ser_size_by_bw(ppr_get_max_bw());
	wl_txppr_t *wl_txppr;
	ppr_t *pprptr = NULL;

	wl_txppr = (wl_txppr_t *)malloc(sizeof(*wl_txppr) + pprsize);

	if (wl_txppr == NULL) {
		fprintf(stderr, "Error allocating memory failed for curppr");
		return BCME_NOMEM;
	}

	memset(wl_txppr, 0, sizeof(*wl_txppr));
	wl_txppr->buflen = pprsize;
	if ((err = ppr_init_ser_mem_by_bw(wl_txppr->pprbuf, ppr_get_max_bw(), pprsize))
		!= BCME_OK) {
		free(wl_txppr);
		return err;
	}

	if (WLC_IOCTL_MAXLEN < sizeof(wl_txppr_t) + pprsize) {
		free(wl_txppr);
		return BCME_ERROR;
	}

	argv++;
	if (*argv)
		fprintf(stderr, "Ignoring arguments for %s\n", cmd->name);

	wl_txppr->ver = WL_TXPPR_VERSION;
	wl_txppr->len = WL_TXPPR_LENGTH;
	if ((err = wlu_iovar_getbuf(wl, "curppr", wl_txppr, sizeof(*wl_txppr) + pprsize,
		buf, WLC_IOCTL_MAXLEN)) < 0) {
		free(wl_txppr);
		return err;
	}

	/* the input buffer is no longer needed, output results are in buf */
	free(wl_txppr);
	wl_txppr = (wl_txppr_t *)buf;

	/* parse */
	wl_txppr->flags = dtoh32(wl_txppr->flags);
	wl_txppr->chanspec = wl_chspec_from_driver(wl_txppr->chanspec);
	wl_txppr->local_chanspec = wl_chspec_from_driver(wl_txppr->local_chanspec);

	chanspec = wl_txppr->chanspec;
	flags = (wl_txppr->flags & WL_TX_POWER_F_VHT) |
	       (wl_txppr->flags & WL_TX_POWER_F_HT) |
	       (wl_txppr->flags & WL_TX_POWER_F_MIMO) |
	       (wl_txppr->flags & WL_TX_POWER_F_SISO);

	/* dump */
	printf("Current channel:\t %s\n",
	       wf_chspec_ntoa(wl_txppr->chanspec, chanspec_str));
	printf("BSS channel:\t\t %s\n",
	       wf_chspec_ntoa(wl_txppr->local_chanspec, chanspec_str));
	printf("Power/Rate Dump (in 1/4dB): Channel %d\n", CHSPEC_CHANNEL(chanspec));
	if ((err = ppr_deserialize_create(NULL, wl_txppr->pprbuf, pprsize, &pprptr)) == BCME_OK) {
		wl_txppr_print(pprptr, CHSPEC_IS2G(chanspec), flags);
		ppr_delete(NULL, pprptr);
	}
	return err;
}

static int
wl_txcore_pwr_offset(void *wl, cmd_t *cmd, char **argv)
{
	wl_txchain_pwr_offsets_t offsets;
	char *endptr;
	int i;
	long val;
	int err;

	/* toss the command name */
	argv++;

	if (!*argv) {
		err = wlu_iovar_get(wl, cmd->name, &offsets, sizeof(wl_txchain_pwr_offsets_t));

		if (err < 0)
			return err;

		printf("txcore offsets qdBm: %d %d %d %d\n",
		       offsets.offset[0], offsets.offset[1],
		       offsets.offset[2], offsets.offset[3]);

		return 0;
	}

	memset(&offsets, 0, sizeof(wl_txchain_pwr_offsets_t));

	for (i = 0; i < WL_NUM_TXCHAIN_MAX; i++, argv++) {
		if (!*argv)
			break;

		val = strtol(*argv, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		if (val > 0)
			return BCME_BADARG;

		offsets.offset[i] = (int8)val;
	}

	err = wlu_iovar_set(wl, cmd->name, &offsets, sizeof(wl_txchain_pwr_offsets_t));

	return err;
}

static int
wl_txcore(void *wl, cmd_t *cmd, char **argv)
{
	miniopt_t to;
	const char* fn_name = "wl_txcore";
	int err = 0, opt_err, val;
	uint8 streams = 0;
	bool streams_set = FALSE;
	uint8 core = 0;
	bool core_set = FALSE;
	uint8 cck_mask = 0;
	bool cck_set = FALSE;
	uint8 ofdm_mask = 0;
	bool ofdm_set = FALSE;
	uint8 mcs_mask[4] = {0, 0, 0, 0}; /* pre-initialize # of streams {core:4 | stream:4} */
	bool mcs_set = FALSE;
	uint8 idx;
	uint32 coremask[2] = {0, 0};

	/* toss the command name */
	argv++;

	if (!*argv) {
		if (cmd->get < 0)
			return -1;
		if ((err = wlu_iovar_get(wl, cmd->name, &coremask, sizeof(uint32)*2)) < 0)
			return err;

#ifdef DSLCPE_ENDIAN
		coremask[0] = dtoh32(coremask[0]);
		coremask[1] = dtoh32(coremask[1]);
#endif
		printf("txcore enabled bitmap (Nsts {4..1}) 0x%02x 0x%02x 0x%02x 0x%02x\n",
			(coremask[0] >> 24) & 0xff, (coremask[0] >> 16) & 0xff,
			(coremask[0] >> 8) & 0xff, coremask[0] & 0xff);
		printf("txcore mask OFDM 0x%02x  CCK 0x%02x\n",
			(coremask[1] >> 8) & 0xff, coremask[1] & 0xff);
		return 0;
	}

	val = atoi(*argv);
	if (val == -1)
		goto next;

	miniopt_init(&to, fn_name, "w", FALSE);
	while ((opt_err = miniopt(&to, argv)) != -1) {
		if (opt_err == 1) {
			err = BCME_USAGE_ERROR;
			goto exit;
		}
		argv += to.consumed;

		if (to.opt == 's') {
			if (!to.good_int) {
				fprintf(stderr,
					"%s: could not parse \"%s\" as an int for streams\n",
					fn_name, to.valstr);
				err = BCME_BADARG;
				goto exit;
			}
			streams_set = TRUE;
			streams = (to.val & 0x0f);
			if (streams > 4)
				fprintf(stderr, "%s: Nsts > %d\n", fn_name, to.val);
		}
		if (to.opt == 'c') {
			if (!to.good_int) {
				fprintf(stderr,
					"%s: could not parse \"%s\" as an int for stf core\n",
					fn_name, to.valstr);
				err = BCME_BADARG;
				goto exit;
			}
			core_set = TRUE;
			core = (to.val & 0x0f) << 4;
			if (core == 0) {
				fprintf(stderr, "%s: %1d-stream core cannot be zero\n",
					fn_name, streams);
				err = BCME_BADARG;
				goto exit;
			}
		}
		if (to.opt == 'o') {
			if (!to.good_int) {
				fprintf(stderr,
					"%s: could not parse \"%s\" as an int for streams\n",
					fn_name, to.valstr);
				err = BCME_BADARG;
				goto exit;
			}
			ofdm_set = TRUE;
			ofdm_mask = (to.val & 0x0f);
			if (ofdm_mask == 0) {
				fprintf(stderr, "%s: OFDM core cannot be zero\n", fn_name);
				err = BCME_BADARG;
				goto exit;
			}
		}
		if (to.opt == 'k') {
			if (!to.good_int) {
				fprintf(stderr,
					"%s: could not parse \"%s\" as an int for streams\n",
					fn_name, to.valstr);
				err = BCME_BADARG;
				goto exit;
			}
			cck_set = TRUE;
			cck_mask = (to.val & 0x0f);
			if (cck_mask == 0) {
				fprintf(stderr, "%s: CCK core cannot be zero\n", fn_name);
				err = BCME_BADARG;
				goto exit;
			}
		}

		if (streams_set && core_set) {
			streams_set = core_set = FALSE;
			mcs_set = TRUE;
			idx = streams - 1;
			mcs_mask[idx] = (uint8)(core|streams);
		}
	}

	if (streams_set != core_set) {
		fprintf(stderr, "%s: require to set both -s x -c y\n", fn_name);
		err = BCME_BADARG;
		goto exit;
	}

	if (mcs_set) {
		coremask[0] |= mcs_mask[0] << 0;
		coremask[0] |= mcs_mask[1] << 8;
		coremask[0] |= mcs_mask[2] << 16;
		coremask[0] |= mcs_mask[3] << 24;
	}
	if (cck_set)
		coremask[1] |= cck_mask;
	if (ofdm_set)
		coremask[1] |= ofdm_mask << 8;
next:
#ifdef DSLCPE_ENDIAN
	coremask[0] = htod32(coremask[0]);
	coremask[1] = htod32(coremask[1]);
#endif
	err = wlu_var_setbuf(wl, cmd->name, coremask, sizeof(uint32)*2);
exit:
	return err;
}

static int
wl_mimo_stf(void *wl, cmd_t *cmd, char **argv)
{
	char var[256];
	int32 int_val;
	bool get = TRUE;
	uint32 len = 0;
	void *ptr = NULL;
	char *endptr;
	int i = 0, j = 0;
	int ret = 0;

	while (argv[i])
		i++;

	if (i > 4)
		return BCME_USAGE_ERROR;

	/* toss the command name */
	argv++;
	j = 1;

	if (i == 1) {
		int_val = -1;
		memcpy(&var[len], (char *)&int_val, sizeof(int_val));
		len += sizeof(int_val);
	}
	else {
		if (isdigit((unsigned char)(**argv))) {
			get = FALSE;
			int_val = htod32(strtoul(*argv, &endptr, 0));
			if ((int_val != 0) && (int_val != 1)) {
				fprintf(stderr, "wl mimo_ss_stf: bad stf mode.\n");
				return BCME_BADARG;
			}
			memcpy(var, (char *)&int_val, sizeof(int_val));
			len += sizeof(int_val);
			argv++;
			j++;
		}

		 if (j == i) {
			int_val = -1;
			memcpy(&var[len], (char *)&int_val, sizeof(int_val));
			len += sizeof(int_val);
		}
		else if (!strncmp(*argv, "-b", 2)) {
			argv++;
			j++;
			if (j == i)
				return BCME_BADARG;

			if (!strcmp(*argv, "a"))
#ifdef DSLCPE_ENDIAN
			        int_val = htod32(1);
#else
				int_val = 1;
#endif
			else if (!strcmp(*argv, "b"))
				int_val = 0;
			else {
				fprintf(stderr,
					"wl mimo_ss_stf: wrong -b option, \"-b a\" or \"-b b\"\n");
				return BCME_USAGE_ERROR;
			}
			j++;
			if (j < i)
				return BCME_BADARG;
			memcpy(&var[len], (char *)&int_val, sizeof(int_val));
			len += sizeof(int_val);
		}
	}

	if (get) {
		if ((ret = wlu_var_getbuf(wl, cmd->name, var, sizeof(var), &ptr)) < 0)
			return ret;

		printf("0x%x\n", dtoh32(*(int *)ptr));
	}
	else
		ret = wlu_var_setbuf(wl, cmd->name, &var, sizeof(var));
	return ret;
}

static int
wl_spatial_policy(void *wl, cmd_t *cmd, char **argv)
{
	void *ptr = NULL;
	int err, i, *reply;
	int mode[SPATIAL_MODE_MAX_IDX] = {-1, -1, -1, -1, -1};

	/* Order is 2G, 5G-LOW, 5G-MID, 5G-HIGH, 5G-UPPER
	 * if only one argument given, than all band or sub-band take the
	 * same value
	 */
	if (!*++argv) {
		bool all_same = TRUE;
		if ((err = wlu_var_getbuf(wl, cmd->name, &mode, sizeof(mode), &ptr)) < 0)
			return err;
		reply = (int *)ptr;
		for (i = 1; i < SPATIAL_MODE_MAX_IDX; i++) {
			 /* check if return values for each band/sub-band is same or not */
			 if (reply[i-1] != reply[i])
				 all_same = FALSE;
		}
		if (all_same)
#ifdef DSLCPE_ENDIAN
		        printf("%2d\n", dtoh32(reply[0]));
#else
			printf("%2d\n", reply[0]);
#endif
		else {
#ifdef DSLCPE_ENDIAN
		        printf("2.4GHz       : %2d\n", dtoh32(reply[SPATIAL_MODE_2G_IDX]));
			printf("5GHz (lower) : %2d\n", dtoh32(reply[SPATIAL_MODE_5G_LOW_IDX]));
			printf("5GHz (middle): %2d\n", dtoh32(reply[SPATIAL_MODE_5G_MID_IDX]));
			printf("5GHz (high)  : %2d\n", dtoh32(reply[SPATIAL_MODE_5G_HIGH_IDX]));
			printf("5GHz (upper) : %2d\n", dtoh32(reply[SPATIAL_MODE_5G_UPPER_IDX]));
#else
			printf("2.4GHz       : %2d\n", reply[SPATIAL_MODE_2G_IDX]);
			printf("5GHz (lower) : %2d\n", reply[SPATIAL_MODE_5G_LOW_IDX]);
			printf("5GHz (middle): %2d\n", reply[SPATIAL_MODE_5G_MID_IDX]);
			printf("5GHz (high)  : %2d\n", reply[SPATIAL_MODE_5G_HIGH_IDX]);
			printf("5GHz (upper) : %2d\n", reply[SPATIAL_MODE_5G_UPPER_IDX]);
#endif
		}
		return 0;
	}
#ifdef DSLCPE_ENDIAN
	mode[0] = htod32(atoi(*argv));
#else
	mode[0] = atoi(*argv);
#endif
	if (!*++argv) {
		for (i = 1; i < SPATIAL_MODE_MAX_IDX; i++)
			mode[i] = mode[0];
	} else {
		for (i = 1; i < SPATIAL_MODE_MAX_IDX; i++) {
#ifdef DSLCPE_ENDIAN
		        mode[i] = htod32(atoi(*argv));
#else
			mode[i] = atoi(*argv);
#endif
			if (!*++argv && i < (SPATIAL_MODE_MAX_IDX - 1)) {
				printf("error: missing arguments\n");
				return BCME_USAGE_ERROR;
			}
		}
	}
	err = wlu_var_setbuf(wl, cmd->name, &mode, sizeof(mode));
	return err;
}

static int
wl_ratetbl_ppr(void *wl, cmd_t *cmd, char **argv)
{
	void *ptr = NULL;
	int err, i, *reply;
	int val[12];

	/* Order is 2G, 5G-LOW, 5G-MID, 5G-HIGH, 5G-UPPER
	 * if only one argument given, than all band or sub-band take the
	 * same value
	 */
	memset(&val, 0, sizeof(val));
	if (!*++argv) {
		if ((err = wlu_var_getbuf(wl, cmd->name, &val, sizeof(val), &ptr)) < 0)
			return err;
		reply = (int *)ptr;
		for (i = 0; i < 12; i++)
#ifdef DSLCPE_ENDIAN
		        printf("%s: %2d\n", (dtoh32(reply[i]) & 0x80) ? "OFDM" : "CCK ", (reply[i] & 0x7f));
#else
			printf("%s: %2d\n", (reply[i] & 0x80) ? "OFDM" : "CCK ", (reply[i] & 0x7f));
#endif
		return 0;
	}
#ifdef DSLCPE_ENDIAN
	val[0] = htod32(atoi(*argv++));
	val[1] = htod32(atoi(*argv++));
#else
	val[0] = atoi(*argv++);
	val[1] = atoi(*argv++);
#endif
	err = wlu_var_setbuf(wl, cmd->name, &val, sizeof(val));
	return err;
}
