/*
 * wl scan command module
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
 * $Id: wluc_scan.c 458728 2014-02-27 18:15:25Z $
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

#ifdef EXTENDED_SCAN
static cmd_func_t wl_extdscan;
static int wl_parse_extdchannel_list(char* list_str,
	chan_scandata_t* channel_list, int channel_num);
#endif // endif
static cmd_func_t wl_scanmac;

static cmd_t wl_scan_cmds[] = {
	{ "scancache_clear", wl_var_void, -1, WLC_SET_VAR,
	"clear the scan cache"},
#ifdef EXTENDED_SCAN
	{ "extdscan", wl_extdscan, -1, WLC_SET_VAR,
	"Initiate an extended scan.\n"
	"\tDefault to an active scan across all channels for any SSID.\n"
	"\tOptional args: list of SSIDs to scan.\n"
	"\tOptions:\n"
	"\t-s S1 S2 S3, --ssid=S1 S2 S3\t\tSSIDs to scan, comma or space separated\n"
	"\t-x x, --split_scan=ST\t[split_scan] scan type\n"
	"\t-t ST, --scan_type=ST\t[background:0/forcedbackground:1/foreground:2] scan type\n"
	"\t-n N, --nprobes=N\tnumber of probes per scanned channel, per SSID\n"
	"\t-c L, --channels=L\tcomma or space separated list of channels to scan"},
#endif // endif
	{ "passive", wl_int, WLC_GET_PASSIVE_SCAN, WLC_SET_PASSIVE_SCAN,
	"Puts scan engine into passive mode" },
	{ "scansuppress", wl_int, WLC_GET_SCANSUPPRESS, WLC_SET_SCANSUPPRESS,
	"Suppress all scans for testing.\n"
	"\t0 - allow scans\n"
	"\t1 - suppress scans" },
	{ "scan_channel_time", wl_int, WLC_GET_SCAN_CHANNEL_TIME, WLC_SET_SCAN_CHANNEL_TIME,
	"Get/Set scan channel time"},
	{ "scan_unassoc_time", wl_int, WLC_GET_SCAN_UNASSOC_TIME, WLC_SET_SCAN_UNASSOC_TIME,
	"Get/Set unassociated scan channel dwell time"},
	{ "scan_home_time", wl_int, WLC_GET_SCAN_HOME_TIME, WLC_SET_SCAN_HOME_TIME,
	"Get/Set scan home channel dwell time"},
	{ "scan_passive_time", wl_int, WLC_GET_SCAN_PASSIVE_TIME, WLC_SET_SCAN_PASSIVE_TIME,
	"Get/Set passive scan channel dwell time"},
	{ "scan_nprobes", wl_int, WLC_GET_SCAN_NPROBES, WLC_SET_SCAN_NPROBES,
	"Get/Set scan parameter for number of probes to use per channel scanned"},
	{ "scan_ps", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"Get/Set scan power optimization enable/disable"},
	{ "scanmac", wl_scanmac, WLC_GET_VAR, WLC_SET_VAR,
	"Configure scan MAC using subcommands:\n"
	"\tscanmac enable <0|1>\n"
	"\tscanmac bsscfg\n"
	"\tscanmac config <mac> <random_mask> <scan_bitmap>\n"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_scan_module_init(void)
{
	(void)g_swap;

	/* get the global buf */
	buf = wl_get_buf();

	/* register scan commands */
	wl_module_cmds_register(wl_scan_cmds);
}

#ifdef EXTENDED_SCAN
/* wl extdscan
 * -s --ssid=ssid1 ssid2 ssid3
 * -b --split_scan=0 : [split_scan]
 * -t --scan_type=0 : [background/forcedbackground/foreground]
 * -n --nprobes=
 * -c --channels=
 */
static int
wl_extdscan(void *wl, cmd_t *cmd, char **argv)
{
	wl_extdscan_params_t *params;
	int params_size = WL_EXTDSCAN_PARAMS_FIXED_SIZE +
		(WL_NUMCHANNELS * sizeof(chan_scandata_t));
	int val = 0;
	char *p, *eq, *valstr, *endptr;
	char opt;
	bool positional_param;
	bool good_int;
	bool opt_end;
	int err = 0;
	int keylen;
	char key[64];
	int i;
	int nssid = 0;

	fprintf(stderr, "params alloc size is %d\n", params_size);
	params = (wl_extdscan_params_t *)malloc(params_size);
	if (params == NULL) {
		fprintf(stderr, "Error allocating %d bytes for scan params\n", params_size);
		return BCME_NOMEM;
	}
	memset(params, 0, params_size);

	params->scan_type = EXTDSCAN_FORCEDBACKGROUND_SCAN;
	params->nprobes = 3;
	params->band = WLC_BAND_2G;
	params->split_scan = 0;

	/* skip the command name */
	argv++;

	if (*argv == NULL) {
		fprintf(stderr, "no arguments to wl_extdscan\n");
		err = BCME_USAGE_ERROR;
		goto exit;
	}
	opt_end = FALSE;
	while ((p = *argv) != NULL) {
		argv++;
		positional_param = FALSE;
		memset(key, 0, sizeof(key));
		opt = '\0';
		valstr = NULL;
		good_int = FALSE;

		if (opt_end) {
			positional_param = TRUE;
			valstr = p;
		}
		else if (!strcmp(p, "--")) {
			opt_end = TRUE;
			continue;
		}
		else if (!strncmp(p, "--", 2)) {
			eq = strchr(p, '=');
			if (eq == NULL) {
				fprintf(stderr,
				"wl_extdscan: missing \" = \" in long param \"%s\"\n", p);
				err = BCME_USAGE_ERROR;
				goto exit;
			}
			keylen = eq - (p + 2);
			if (keylen > 63)
				keylen = 63;
			memcpy(key, p + 2, keylen);

			valstr = eq + 1;
			if (*valstr == '\0') {
				fprintf(stderr,
				"extdscan: missing value after \" = \" in long param \"%s\"\n", p);
				err = BCME_USAGE_ERROR;
				goto exit;
			}
		}
		else if (!strncmp(p, "-", 1)) {
			opt = p[1];
			if (strlen(p) > 2) {
				fprintf(stderr,
				"extdscan: only single char options, error on param \"%s\"\n", p);
				err = BCME_BADARG;
				goto exit;
			}
			if (*argv == NULL) {
				fprintf(stderr,
				"extdscan: missing value parameter after \"%s\"\n", p);
				err = BCME_USAGE_ERROR;
				goto exit;
			}
			valstr = *argv;
			argv++;
		} else {
			positional_param = TRUE;
			valstr = p;
		}

		/* parse valstr as int just in case */
		if (valstr) {
			val = (int)strtol(valstr, &endptr, 0);
			if (*endptr == '\0') {
				/* not all the value string was parsed by strtol */
				good_int = TRUE;
			}
		}

		if (opt == 's' || !strcmp(key, "ssid") || positional_param) {
			nssid = wl_parse_ssid_list(valstr, params->ssid,
			                           nssid, WLC_EXTDSCAN_MAX_SSID);
			if (nssid < 0) {
				err = BCME_BADARG;
				goto exit;
			}
		}
		if (opt == 'b' || !strcmp(key, "band")) {
			if (!strcmp(valstr, "5G")) {
				params->band = WLC_BAND_5G;
			}
			else if (!strcmp(valstr, "2.4G")) {
				params->band = WLC_BAND_2G;
			}
			else if (!strcmp(valstr, "all")) {
				params->band = WLC_BAND_ALL;
			} else {
				fprintf(stderr,
				"scan_type value should be \"5G\" "
				"or \"2.4G\" " "or \"all\" but got \"%s\"\n", valstr);
				err = BCME_USAGE_ERROR;
				goto exit;
			}
		}
		if (opt == 't' || !strcmp(key, "scan_type")) {
			if (!strcmp(valstr, "background")) {
				params->scan_type = EXTDSCAN_BACKGROUND_SCAN;
			} else if (!strcmp(valstr, "fbackground")) {
				params->scan_type = EXTDSCAN_FORCEDBACKGROUND_SCAN;
			} else if (!strcmp(valstr, "foreground")) {
				params->scan_type = EXTDSCAN_FOREGROUND_SCAN;
			} else {
				fprintf(stderr,
				"scan_type value should be \"background\" "
				"or \"fbackground\" " "or \"foreground\" but got \"%s\"\n", valstr);
				err = BCME_USAGE_ERROR;
				goto exit;
			}
		}
		if (opt == 'n' || !strcmp(key, "nprobes")) {
			if (!good_int) {
				fprintf(stderr,
				"could not parse \"%s\" as an int for value nprobes\n", valstr);
				err = BCME_BADARG;
				goto exit;
			}
			params->nprobes = val;
		}
		if (opt == 'x' || !strcmp(key, "split_scan")) {
			if (val != 0)
				params->split_scan = 1;
		}
		if (opt == 'c' || !strcmp(key, "channels")) {
			params->channel_num = wl_parse_extdchannel_list(valstr,
				params->channel_list, WL_NUMCHANNELS);
			if (params->channel_num == -1) {
				fprintf(stderr, "error parsing channel list arg\n");
				err = BCME_BADARG;
				goto exit;
			}
		}
	}

	if (nssid > WLC_EXTDSCAN_MAX_SSID) {
		fprintf(stderr, "ssid count %d exceeds max of %d\n",
		        nssid, WLC_EXTDSCAN_MAX_SSID);
		err = BCME_BADARG;
		goto exit;
	}

	params_size = WL_EXTDSCAN_PARAMS_FIXED_SIZE +
	    (params->channel_num * sizeof(chan_scandata_t));

	fprintf(stderr, "ssid list is %s(%d) %s(%d) %s(%d) %s(%d) %s(%d)\n",
		(char *)&params->ssid[0].SSID, params->ssid[0].SSID_len,
		(char *)&params->ssid[1].SSID, params->ssid[1].SSID_len,
		(char *)&params->ssid[2].SSID, params->ssid[2].SSID_len,
		(char *)&params->ssid[3].SSID, params->ssid[3].SSID_len,
		(char *)&params->ssid[4].SSID, params->ssid[4].SSID_len);
	if (params->split_scan)
	    fprintf(stderr, "split scan is enabled\n");
	else
	   fprintf(stderr, "split scan is not enabled\n");

	fprintf(stderr, "scan type is %d, nprobes are %d, band is %d, channels are %d\n",
		params->scan_type, params->nprobes, params->band, params->channel_num);

	fprintf(stderr, "params size is %d\n", params_size);
	params->scan_type = htodenum(params->scan_type);
	for (i = 0; i < WLC_EXTDSCAN_MAX_SSID; i++) {
		params->ssid[i].SSID_len = htod32(params->ssid[i].SSID_len);
	}
	for (i = 0; i < params->channel_num; i++) {
		chanspec_t chanspec = params->channel_list[i].channel;
		chanspec = wl_chspec_to_driver(chanspec);
		if (chanspec == INVCHANSPEC) {
			err = BCME_USAGE_ERROR;
			goto exit;
		}
		params->channel_list[i].channel = chanspec;
		params->channel_list[i].channel_mintime =
		        htod32(params->channel_list[i].channel_mintime);
		params->channel_list[i].channel_maxtime =
		        htod32(params->channel_list[i].channel_maxtime);
	}
	params->channel_num = htod32(params->channel_num);
	err =  wlu_var_setbuf(wl, cmd->name, params, params_size);

exit:
	free(params);
	return err;
}

static int
wl_parse_extdchannel_list(char* list_str, chan_scandata_t* channel_list, int channel_num)
{
	int num;
	int val;
	char* str;
	char* endptr;

	if (list_str == NULL)
		return -1;

	str = list_str;
	num = 0;
	while (*str != '\0') {
		val = (int)strtol(str, &endptr, 0);
		if (endptr == str) {
			fprintf(stderr,
				"could not parse channel number starting at"
				" substring \"%s\" in list:\n%s\n",
				str, list_str);
			return -1;
		}
		str = endptr + strspn(endptr, " ,");

		if (num == channel_num) {
			fprintf(stderr, "too many channels (more than %d) in channel list:\n%s\n",
				channel_num, list_str);
			return -1;
		}
		channel_list->channel = (uint16)val;
		channel_list++;
		num++;
	}

	return num;
}
#endif /* EXTENDED_SCAN */

static int
wl_scanmac(void *wl, cmd_t *cmd, char **argv)
{
	int err = -1;
	char *subcmd;
	int subcmd_len;

	/* skip iovar */
	argv++;

	/* must have subcommand */
	subcmd = *argv++;
	if (!subcmd) {
		return BCME_USAGE_ERROR;
	}
	subcmd_len = strlen(subcmd);

	if (!*argv) {
		/* get */
		uint8 buffer[OFFSETOF(wl_scanmac_t, data)];
		wl_scanmac_t *sm = (wl_scanmac_t *)buffer;
		int len = OFFSETOF(wl_scanmac_t, data);

		memset(sm, 0, len);
		if (!strncmp(subcmd, "enable", subcmd_len)) {
			sm->subcmd_id = WL_SCANMAC_SUBCMD_ENABLE;
		} else if (!strncmp(subcmd, "bsscfg", subcmd_len)) {
			sm->subcmd_id = WL_SCANMAC_SUBCMD_BSSCFG;
		} else if (!strncmp(subcmd, "config", subcmd_len)) {
			sm->subcmd_id = WL_SCANMAC_SUBCMD_CONFIG;
		} else {
			return BCME_USAGE_ERROR;
		}

		/* invoke GET iovar */
		sm->subcmd_id = dtoh16(sm->subcmd_id);
		sm->len = dtoh16(sm->len);
		if ((err = wlu_iovar_getbuf(wl, cmd->name, sm, len, buf, WLC_IOCTL_SMLEN)) < 0) {
			return err;
		}

		/* process and print GET results */
		sm = (wl_scanmac_t *)buf;
		sm->subcmd_id = dtoh16(sm->subcmd_id);
		sm->len = dtoh16(sm->len);

		switch (sm->subcmd_id) {
		case WL_SCANMAC_SUBCMD_ENABLE:
		{
			wl_scanmac_enable_t *sm_enable = (wl_scanmac_enable_t *)sm->data;
			if (sm->len >= sizeof(*sm_enable)) {
				printf("%d\n", sm_enable->enable);
			} else {
				err = BCME_BADLEN;
			}
			break;
		}
		case WL_SCANMAC_SUBCMD_BSSCFG:
		{
			wl_scanmac_bsscfg_t *sm_bsscfg = (wl_scanmac_bsscfg_t *)sm->data;
			if (sm->len >= sizeof(*sm_bsscfg)) {
				sm_bsscfg->bsscfg = dtoh32(sm_bsscfg->bsscfg);
				printf("%d\n", sm_bsscfg->bsscfg);
			} else {
				err = BCME_BADLEN;
			}
			break;
		}
		case WL_SCANMAC_SUBCMD_CONFIG:
		{
			wl_scanmac_config_t *sm_config = (wl_scanmac_config_t *)sm->data;
			if (sm->len >= sizeof(*sm_config)) {
				sm_config->scan_bitmap = dtoh16(sm_config->scan_bitmap);
				printf("mac:         %s\n", wl_ether_etoa(&sm_config->mac));
				printf("random mask: %s\n", wl_ether_etoa(&sm_config->random_mask));
				printf("scan bitmap: 0x%02X\n", sm_config->scan_bitmap);
				if (sm_config->scan_bitmap & WL_SCANMAC_SCAN_UNASSOC) {
					printf("             unassoc\n");
				}
				if (sm_config->scan_bitmap & WL_SCANMAC_SCAN_ASSOC_ROAM) {
					printf("             assoc roam\n");
				}
				if (sm_config->scan_bitmap & WL_SCANMAC_SCAN_ASSOC_PNO) {
					printf("             assoc PNO\n");
				}
				if (sm_config->scan_bitmap & WL_SCANMAC_SCAN_ASSOC_HOST) {
					printf("             assoc host\n");
				}
			} else {
				err = BCME_BADLEN;
			}
			break;
		}
		default:
			break;
		}
	}
	else {
		/* set */
		uint8 buffer[OFFSETOF(wl_scanmac_t, data) +
			MAX(sizeof(wl_scanmac_enable_t), sizeof(wl_scanmac_config_t))];
		wl_scanmac_t *sm = (wl_scanmac_t *)buffer;
		int len = OFFSETOF(wl_scanmac_t, data);

		if (!strncmp(subcmd, "enable", subcmd_len) &&
			(*argv[0] == '0' || *argv[0] == '1')) {
			wl_scanmac_enable_t *sm_enable = (wl_scanmac_enable_t *)sm->data;
			sm->subcmd_id = WL_SCANMAC_SUBCMD_ENABLE;
			sm->len = sizeof(*sm_enable);
			sm_enable->enable = atoi(argv[0]);
		} else if (!strncmp(subcmd, "config", subcmd_len) &&
			argv[0] && argv[1] && argv[2]) {
			wl_scanmac_config_t *sm_config = (wl_scanmac_config_t *)sm->data;
			char *mac = argv[0];
			char *mask = argv[1];
			char *bitmap = argv[2];
			sm->subcmd_id = WL_SCANMAC_SUBCMD_CONFIG;
			sm->len = sizeof(*sm_config);
			if (!wl_ether_atoe(mac, &sm_config->mac) ||
				!wl_ether_atoe(mask, &sm_config->random_mask)) {
				return BCME_USAGE_ERROR;
			}
			sm_config->scan_bitmap = (uint16)strtoul(bitmap, NULL, 0);
			sm_config->scan_bitmap = htod16(sm_config->scan_bitmap);
		} else {
			return BCME_USAGE_ERROR;
		}

		/* invoke SET iovar */
		len = OFFSETOF(wl_scanmac_t, data) + sm->len;
		sm->subcmd_id = htod16(sm->subcmd_id);
		sm->len = htod16(sm->len);
		err = wlu_iovar_set(wl, cmd->name, sm, len);
	}

	return err;
}
