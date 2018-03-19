/*
 * wl wnm command module
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
 * $Id: wluc_wnm.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_wnm_bsstrans_roamthrottle;
static cmd_func_t wl_wnm_bsstrans_rssi_rate_map;
static cmd_func_t wl_wnm;
static cmd_func_t wl_wnm_bsstq;
static cmd_func_t wl_tclas_add;
static cmd_func_t wl_tclas_del;
static cmd_func_t wl_tclas_list;
static cmd_func_t wl_wnm_tfsreq_add;
static cmd_func_t wl_wnm_dms_set;
static cmd_func_t wl_wnm_dms_status;
static cmd_func_t wl_wnm_dms_term;
static cmd_func_t wl_wnm_service_term;
static cmd_func_t wl_wnm_timbc_offset;
static cmd_func_t wl_wnm_timbc_set;
static cmd_func_t wl_wnm_timbc_status;
static cmd_func_t wl_wnm_maxidle;
static cmd_func_t wl_wnm_bsstrans_req;
static cmd_func_t wl_wnm_keepalives_max_idle;
static cmd_func_t wl_wnm_url;

static cmd_t wl_wnm_cmds[] = {
	{ "wnm", wl_wnm, WLC_GET_VAR, WLC_SET_VAR,
	"set driver wnm feature mask\n"
	"\ttype \'wl msglevel ?\' for values" },
	{ "wnm_bsstq", wl_wnm_bsstq, -1, WLC_SET_VAR,
	"send 11v BSS transition management query\n"
	"\tUsage: wl wnm_bsstq [ssid]"},
	{ "tclas_add", wl_tclas_add, -1, WLC_SET_VAR,
	"add tclas frame classifier type entry\n"
	"\tUsage: wl tclas_add <user priority> <type> <mask> <...>\n"
	"\ttype 0 eth2:     <src mac> <dst mac> <ether type>\n"
	"\ttype 1/4 ipv4:   <ver> <src> <dst> <s_port> <d_port> <dscp> <prot>\n"
	"\ttype 2 802.1Q:   <vlan tag>\n"
	"\ttype 3 filter:   <offset> <value> <mask>\n"
	"\ttype 4 ipv6:     <ver> <src> <dst> <s_port> <d_port> <dscp> <nxt_hdr> <flw_lbl>\n"
	"\ttype 5 802.1D/Q: <802.1Q PCP> <802.1Q CFI> <802.1Q VID>"
	},
	{ "tclas_del", wl_tclas_del, -1, WLC_SET_VAR,
	"delete tclas frame classifier type entry\n"
	"\tUsage: wl tclas_del [<idx> [<len>]]"
	},
	{ "tclas_list", wl_tclas_list, WLC_GET_VAR, -1,
	"list the added tclas frame classifier type entry\n"
	"\tUsage: wl tclas_list"
	},
	{ "wnm_tfsreq_add", wl_wnm_tfsreq_add, -1, WLC_SET_VAR,
	"add one tfs request element and send tfs request frame\n"
	"\tUsage: wl wnm_tfsreq_add <tfs_id> <tfs_action_code> <tfs_subelem_id> <send>\n"
	"\ttfs_id: a non-zero value (1 ~ 255)\n"
	"\ttfs_action_code bitfield: 1: delete after match, 2: notify\n"
	"\ttfs_subelem_id: TFS subelement (0 for none or 1 for previous tclas_add)\n"
	"\tsend: 0: store element, 1: send all stored elements"
	},
	{ "wnm_dms_set", wl_wnm_dms_set, -1, WLC_SET_VAR,
	"Optionally add pending DMS desc (after tclas_add) and optionally register all desc\n"
	"on AP side to enable the service (with send=1)"
	"\tUsage: wl wnm_dms_set <send> [<user_id> [<tc_pro>]]\n"
	"\t\tsend: 0: store descriptor, 1: send all stored descs/enable DMS on AP\n"
	"\t\tuser_id: new ID to assign to the created desc (if TCLAS added)\n"
	"\t\t         or existing ID to enable on AP (if no TCLAS added), 0 for all desc\n"
	"\t\ttc_pro: TCLAS processing element (if several TCLAS added)"
	},
	{ "wnm_dms_status", wl_wnm_dms_status, WLC_GET_VAR, -1,
	"list all DMS descriptors and provide their internal and AP status\n"
	"\tUsage: wl wl_wnm_dms_status"
	},
	{ "wnm_dms_term", wl_wnm_dms_term, -1, WLC_SET_VAR,
	"Disable registered DMS des on AP side and optionally discard them\n"
	"\tUsage: wl wnm_dms_term <del> [<user_id>]\n"
	"\t\tdel: Discard desc after disabling the service on AP side\n"
	"\t\tuser_id: desc to disable/delete, 0 for all desc"
	},
	{ "wnm_service_term", wl_wnm_service_term, -1, WLC_SET_VAR,
	"Disable service. Check specific wnm_XXX_term for more info\n"
	"\tUsage: wl wnm_service_term <srv> <service realted params>\n"
	"\t\tsrv: 1 for DMS, 2 for FMS, 3 for TFS"
	},
	{ "wnm_timbc_offset", wl_wnm_timbc_offset, WLC_GET_VAR, WLC_SET_VAR,
	"get/set TIM broadcast offset by -32768 period > offset(us) > 32768\n"
	"CAUTION!! Due to resource limitation, one radio can have only one set of TIMBC offset\n"
	"setting.  MBSS need to share the same setting\n"
	"\tUsage: wl wnm_timbc_offset <offset> [<tsf_present> [<fix_interval> [<rate_ovreride>]]]\n"
	"\t\toffset: in unit of us.  Transmit TIM frame in specific TBTT transmit time time\n"
	"\t\ttsf_present: can be omitted.  If set to 1, timestamp field will present in TIM frame."
	"If omitted, default setup to 1\n"
	"\t\tfix_interval: can be omitted.  If set with non-zero value, override STA request "
	"interval in TIM Broadcast request.  If omitted, default setup to 0\n"
	"\t\trate_override: can be omitted.  In unit of 500k, max setup to 108.  If set, override"
	"override high rate used to transmit TIM broadcast high rate frame"
	},
	{ "wnm_timbc_set", wl_wnm_timbc_set, -1, WLC_SET_VAR,
	"Enable/disable TIM Broadcast. Station will send appropriate request if AP suport TIMBC\n"
	"\tUsage: wl wnm_timbc_set <interval> [<flags> [<min_rate> [<max_rate>]]]\n"
	"\t\tinterval: Beacon interval requested for TIM frames, 0 to disable TIM BC\n"
	"\t\tflags: Bitfield with minimal requirements to keep the service enabled (check doc)\n"
	"\t\tmin_rate: Minimal rate requirement, in Mbps, for TIM high or low rate frames\n"
	"\t\tmax_rate: Maximal rate requirement"
	},
	{ "wnm_timbc_status", wl_wnm_timbc_status, WLC_GET_VAR, -1,
	"Retrieve TIM Broadcast configuration set with current AP"
	},
	{ "wnm_maxidle", wl_wnm_maxidle, WLC_GET_VAR, WLC_SET_VAR,
	"setup WNM BSS Max Idle Period interval and option\n"
	"\tUsage: wl wnm_maxidle <Idle Period> <Option>\n"
	"\tIdle Period: in unit of 1000TU(1.024s)\n"
	"\tOption: protected keep alive required(0 ~ 1)"
	},
	{ "wnm_bsstrans_query", wl_wnm_bsstq, -1, WLC_SET_VAR,
	"send 11v BSS transition management query\n"
	"\tUsage: wl wnm_bsstrans_query [ssid]"},
	{ "wnm_bsstrans_req", wl_wnm_bsstrans_req, -1, WLC_SET_VAR,
	"send BSS transition management request frame with BSS termination included bit set\n"
	"\tUsage: wl wnm_bsstrans_req <reqmode> <tbtt> <dur> [unicast]\n"
	"\treqmode: request mode of BSS transition request\n"
	"\ttbtt: time of BSS to end of life, in unit of TBTT, max to 65535\n"
	"\tdur: time of BSS to keep off, in unit of minute, max to 65535\n"
	"\tunicast: [1|0] unicast or broadcast to notify STA in BSS.  Default in unicast."
	},
	{ "wnm_keepalives_max_idle", wl_wnm_keepalives_max_idle, -1, -1,
	"\tUsage: wl wnm_keepalives_max_idle <keepalives_per_bss_max_idle> <mkeepalive_index>"
	" [<max_interval>]\n"
	"set/get the number of keepalives, mkeep-alive index and max_interval configured"
	" per BSS-Idle period."},
	{ "wnm_url", wl_wnm_url, WLC_GET_VAR, WLC_SET_VAR,
	"set/get wnm session information url\n"
	"Usage for set: wl wnm_url length urlstring\n"
	"Usage for get: wl wnm_url" },
	{ "wnm_bsstrans_roamthrottle", wl_wnm_bsstrans_roamthrottle, WLC_GET_VAR, WLC_SET_VAR,
	"Get/Set number of roam scans allowed in throttle period\n"
	"\tUsage: wl wnm_bsstrans_roamthrottle [throttle_period] [scans_allowed]"
	},
	{ "wnm_bsstrans_rssi_rate_map", wl_wnm_bsstrans_rssi_rate_map, WLC_GET_VAR, WLC_SET_VAR,
	"Get/Set rssi to rate map\n"
	"\tUsage: wl wnm_bsstrans_rssi_rate_map mode data\n"
	"\tData is interpeted based on mode\n"
	"\tFor mode=0: data = rssi0, rssi1, rssi2.... as per wl_bsstrans_rssi_rate_map_t\n"
	"\tFor mode=1: data = phy-type band streams rssi0, rssi1...\n"
	"\t\twhere, phy-type = cck, ofdm, 11n, 11ac\n"
	"\t\tband = 2g or 5g for ofdm, 11n and 11ac. Only 2g for cck\n"
	"\t\tstreams = Only 1 for cck and ofdm. 1 to 4 for 11n and 11ac\n"
	"\t\trssiX = monotonically non-decreasing rssi values for the combination of phy-type,\n"
	"\t\tband and streams. Max rssi values for 11ac: 10, 11n: 8, ofdm: 8, cck: 4"
	},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_wnm_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register wnm commands */
	wl_module_cmds_register(wl_wnm_cmds);
}

static int
wl_wnm_bsstrans_roamthrottle(void *wl, cmd_t *cmd, char **argv)
{
	int err, buflen;
	wl_bsstrans_roamthrottle_t *throttle;

	buflen = sprintf(buf, "%s", *argv) + 1;

	if (*++(argv) == NULL) {
		buf[buflen] = '\0';
		err = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN);
		if (err < 0)
			return err;

		throttle = (wl_bsstrans_roamthrottle_t *)buf;
#ifdef DSLCPE_ENDIAN
		printf("period %u\n", dtoh16(throttle->period));
		printf("scans allowed per period %u\n", dtoh16(throttle->scans_allowed));
#else
		printf("period %u\n", throttle->period);
		printf("scans allowed per period %u\n", throttle->scans_allowed);
#endif
		return 0;

	} else {
		throttle = (wl_bsstrans_roamthrottle_t *) (buf + buflen);
		buflen += sizeof(*throttle);
#ifdef DSLCPE_ENDIAN
		throttle->ver = htod16(WL_BSSTRANS_ROAMTHROTTLE_VERSION);
		throttle->period = htod16((uint32) strtoul(*argv, NULL, 0));
#else
		throttle->ver = WL_BSSTRANS_ROAMTHROTTLE_VERSION;
		throttle->period = (uint32) strtoul(*argv, NULL, 0);
#endif

		if (*++(argv) == NULL) {
			printf("Incorrect number of arguments\n");
			return BCME_ERROR;
		}
#ifdef DSLCPE_ENDIAN
		throttle->scans_allowed = htod16((uint16) strtoul(*argv, NULL, 0));
#else
		throttle->scans_allowed = (uint16) strtoul(*argv, NULL, 0);
#endif
		if (*++(argv)) {
			printf("extra arguments\n");
			return BCME_ERROR;
		}
		err = wlu_set(wl, WLC_SET_VAR, buf, buflen);

		return err;
	}
	return 0;
}
#define RSSI_RATE_MAP_PHY_TYPE_CCK 0
#define RSSI_RATE_MAP_PHY_TYPE_OFDM 1
#define RSSI_RATE_MAP_PHY_TYPE_N 2
#define RSSI_RATE_MAP_PHY_TYPE_AC 3

static int
wl_wnm_bsstrans_rssi_rate_map(void *wl, cmd_t *cmd, char **argv)
{
	int err, buflen;
	wl_bsstrans_rssi_rate_map_t *map;
	int i;
	uint32 mode;

	buflen = sprintf(buf, "%s", *argv) + 1;

	if (*++(argv) == NULL) {
		printf("Mode is required \n");
		return BCME_ERROR;
	}
	mode = (uint32)atoi(*argv);
	if (mode > 1) {
		printf("Mode can only be 0 or 1\n");
		return BCME_ERROR;
	}

	if (mode == 0) {
		/* Get */
		if (*++(argv) == NULL) {
			buf[buflen] = '\0';
			err = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN);
			if (err < 0)
				return err;

			map = (wl_bsstrans_rssi_rate_map_t *)buf;

			printf("cck rssi-2g:\n");
			for (i = 0; i < WL_NUM_RATES_CCK; i++) {
				printf("%d, ", map->cck[i].rssi_2g);
			}
			printf("\n");

			printf("cck rssi-5g:\n");
			for (i = 0; i < WL_NUM_RATES_CCK; i++) {
				printf("%d, ", map->cck[i].rssi_5g);
			}
			printf("\n");

			printf("ofdm rssi-2g:\n");
			for (i = 0; i < WL_NUM_RATES_OFDM; i++) {
				printf("%d, ", map->ofdm[i].rssi_2g);
			}
			printf("\n");

			printf("ofdm rssi-5g:\n");
			for (i = 0; i < WL_NUM_RATES_OFDM; i++) {
				printf("%d, ", map->ofdm[i].rssi_5g);
			}
			printf("\n");

			printf("phy_n 1x1 rssi-2g:\n");
			for (i = 0; i < WL_NUM_RATES_MCS_1STREAM; i++) {
				printf("%d, ", map->phy_n[0][i].rssi_2g);
			}
			printf("\n");

			printf("phy_n 1x1 rssi-5g:\n");
			for (i = 0; i < WL_NUM_RATES_MCS_1STREAM; i++) {
				printf("%d, ", map->phy_n[0][i].rssi_5g);
			}
			printf("\n");

			printf("phy_n 2x2 rssi-2g:\n");
			for (i = 0; i < WL_NUM_RATES_MCS_1STREAM; i++) {
				printf("%d, ", map->phy_n[1][i].rssi_2g);
			}
			printf("\n");

			printf("phy_n 2x2 rssi-5g:\n");
			for (i = 0; i < WL_NUM_RATES_MCS_1STREAM; i++) {
				printf("%d, ", map->phy_n[1][i].rssi_5g);
			}
			printf("\n");

			printf("phy_ac 1x1 rssi-2g:\n");
			for (i = 0; i < WL_NUM_RATES_VHT; i++) {
				printf("%d, ", map->phy_ac[0][i].rssi_2g);
			}
			printf("\n");

			printf("phy_ac 1x1 rssi-5g:\n");
			for (i = 0; i < WL_NUM_RATES_VHT; i++) {
				printf("%d, ", map->phy_ac[0][i].rssi_5g);
			}
			printf("\n");

			printf("phy_ac 2x2 rssi-2g:\n");
			for (i = 0; i < WL_NUM_RATES_VHT; i++) {
				printf("%d, ", map->phy_ac[1][i].rssi_2g);
			}
			printf("\n");

			printf("phy_ac 2x2 rssi-5g:\n");
			for (i = 0; i < WL_NUM_RATES_VHT; i++) {
				printf("%d, ", map->phy_ac[1][i].rssi_5g);
			}
			printf("\n");

			return 0;
		} else { /* Set */
			int8 *rssi;
			uint32 len = 0;
			map = (wl_bsstrans_rssi_rate_map_t *) (buf + buflen);
			rssi = (int8 *)map->cck;
			buflen += sizeof(*map);
#ifdef DSLCPE_ENDIAN
			map->ver = htod16(WL_BSSTRANS_RSSI_RATE_MAP_VERSION);
#else
			map->ver = WL_BSSTRANS_RSSI_RATE_MAP_VERSION;
#endif
			while (*argv) {
				*rssi++ = (int8) strtol(*argv, NULL, 0);
				len++;
				argv++;
			}
			map->len = sizeof(map->ver) + sizeof(map->len) + len;
#ifdef DSLCPE_ENDIAN
		map->len = htod16(map->len);
#endif
			err = wlu_set(wl, WLC_SET_VAR, buf, buflen);
			return err;
		}
	} else { /* Mode 1 */
		uint32 phy_type, band, streams;

		if (*++(argv) == NULL) {
			goto insufficient_params;
		}
		if (!strcmp(*argv, "cck")) {
			phy_type = RSSI_RATE_MAP_PHY_TYPE_CCK;
		}
		else if (!strcmp(*argv, "ofdm")) {
			phy_type = RSSI_RATE_MAP_PHY_TYPE_OFDM;
		}
		else if (!strcmp(*argv, "11n")) {
			phy_type = RSSI_RATE_MAP_PHY_TYPE_N;
		}
		else if (!strcmp(*argv, "11ac")) {
			phy_type = RSSI_RATE_MAP_PHY_TYPE_AC;
		}
		else {
			printf("Wrong phy-type: only cck/ofdm/11n/11ac supported\n");
			return BCME_ERROR;
		}

		if (*++(argv) == NULL) {
			goto insufficient_params;
		}

		if (!strcmp(*argv, "2g")) {
			band = 0;
		}
		else if (!strcmp(*argv, "5g")) {
			if (phy_type == RSSI_RATE_MAP_PHY_TYPE_CCK) {
				printf("cck not allowed on 5g\n");
				return BCME_ERROR;
			}
			band = 1;
		}
		else {
			printf("Wrong band-type: only '2g' and '5g' are supported\n");
			return BCME_ERROR;
		}

		if (*++(argv) == NULL) {
			goto insufficient_params;
		}

		streams = atoi(*argv);
		if (streams < 1 || streams > 4) {
			printf("Wrong number of streams: Only 1 to 4 supported\n");
			return BCME_ERROR;
		}
		if ((phy_type == RSSI_RATE_MAP_PHY_TYPE_CCK ||
			phy_type == RSSI_RATE_MAP_PHY_TYPE_OFDM) && streams > 1) {
			printf("Only 1 stream allowed for CCK/OFDM\n");
			return BCME_ERROR;
		}

		buf[buflen] = '\0';
		err = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN);
		if (err < 0) {
			return err;
		}
		map = (wl_bsstrans_rssi_rate_map_t *)buf;

		/* GET */
		if (*++(argv) == NULL) {
			switch (phy_type) {
			case RSSI_RATE_MAP_PHY_TYPE_CCK:
				printf("cck rssi-2g:\n");
				for (i = 0; i < WL_NUM_RATES_CCK; i++) {
					printf("%d, ", map->cck[i].rssi_2g);
				}
				printf("\n");
				break;
			case RSSI_RATE_MAP_PHY_TYPE_OFDM:
				printf("ofdm rssi-%sg:\n", band == 0?"2":"5");
				for (i = 0; i < WL_NUM_RATES_OFDM; i++) {
					if (band == 0)
						printf("%d, ", map->ofdm[i].rssi_2g);
					else
						printf("%d, ", map->ofdm[i].rssi_5g);
				}
				printf("\n");
				break;
			case RSSI_RATE_MAP_PHY_TYPE_N:
				printf("11N streams:%dx%d rssi-%sg:\n", streams,
					streams, band == 0 ? "2" : "5");
				for (i = 0; i < WL_NUM_RATES_MCS_1STREAM; i++) {
					if (band == 0) {
						printf("%d, ", map->phy_n[streams-1][i].rssi_2g);
					} else {
						printf("%d, ", map->phy_n[streams-1][i].rssi_5g);
					}
				}
				printf("\n");
				break;
			case RSSI_RATE_MAP_PHY_TYPE_AC:
				printf("11AC streams:%dx%d rssi-%sg:\n", streams,
					streams, band == 0 ? "2" : "5");
				for (i = 0; i < WL_NUM_RATES_VHT; i++) {
					if (band == 0) {
						printf("%d, ", map->phy_ac[streams-1][i].rssi_2g);
					} else {
						printf("%d, ", map->phy_ac[streams-1][i].rssi_5g);
					}
				}
				printf("\n");
				break;
			}
		} else { /* Set */
			int cmdlen = strlen(cmd->name);

			/* Make space for writing cmd name */
			memmove(buf+cmdlen+1, buf, sizeof(*map));

			buflen = sprintf(buf, "%s", cmd->name) + 1;
			buf[buflen] = '\0';

			/* SET */
			map = (wl_bsstrans_rssi_rate_map_t *) (buf + buflen);

			switch (phy_type) {
			case RSSI_RATE_MAP_PHY_TYPE_CCK:
				for (i = 0; i < WL_NUM_RATES_CCK; i++) {
					if (*argv == NULL) {
						break;
					}
					map->cck[i].rssi_2g = (int8) strtol(*argv, NULL, 0);
					argv++;
				}
				break;
			case RSSI_RATE_MAP_PHY_TYPE_OFDM:
				for (i = 0; i < WL_NUM_RATES_OFDM; i++) {
					if (*argv == NULL) {
						break;
					}
					if (band == 0) {
						map->ofdm[i].rssi_2g = (int8)strtol(*argv, NULL, 0);
					} else {
						map->ofdm[i].rssi_5g = (int8)strtol(*argv, NULL, 0);
					}
					argv++;
				}
				break;
			case RSSI_RATE_MAP_PHY_TYPE_N:
				for (i = 0; i < WL_NUM_RATES_MCS_1STREAM; i++) {
					if (*argv == NULL) {
						break;
					}
					if (band == 0) {
						map->phy_n[streams-1][i].rssi_2g =
							(int8)strtol(*argv, NULL, 0);
					} else {
						map->phy_n[streams-1][i].rssi_5g =
							(int8)strtol(*argv, NULL, 0);
					}
					argv++;
				}
				break;
			case RSSI_RATE_MAP_PHY_TYPE_AC:
				for (i = 0; i < WL_NUM_RATES_VHT; i++) {
					if (*argv == NULL) {
						break;
					}
					if (band == 0) {
						map->phy_ac[streams-1][i].rssi_2g =
							(int8)strtol(*argv, NULL, 0);
					} else {
						map->phy_ac[streams-1][i].rssi_5g =
							(int8)strtol(*argv, NULL, 0);
					}
					argv++;
				}
				break;
			}

			buflen += sizeof(*map);
			map->ver = WL_BSSTRANS_RSSI_RATE_MAP_VERSION;
			map->len = sizeof(*map);
			err = wlu_set(wl, WLC_SET_VAR, buf, buflen);
			return err;
		}
		return 0;
insufficient_params:
		printf("Insufficient params for mode 1\n");
		return BCME_ERROR;
	} /* Mode 1 */
	return 0;
}

static dbg_msg_t wl_wnm_msgs[] = {
	{WL_WNM_BSSTRANS,	"BSS-Transition"},
	{WL_WNM_PROXYARP,	"Proxy-ARP"},
	{WL_WNM_MAXIDLE,	"BSS-Max-Idle-Period"},
	{WL_WNM_TIMBC,		"TIM-Broadcast"},
	{WL_WNM_TFS,		"Traffic-Filtering"},
	{WL_WNM_SLEEP,		"WNM-Sleep-Mode"},
	{WL_WNM_DMS,		"Directed-Multicast"},
	{WL_WNM_FMS,		"Flexible-Multicast"},
	{WL_WNM_NOTIF,		"Notification"},
	{0, ""}
};

static int
wl_wnm(void *wl, cmd_t *cmd, char **argv)
{
	int ret, i;
	uint val;
	int wnmmask;
	char *s;

	ret = wlu_iovar_getint(wl, cmd->name, &wnmmask);
	if (ret < 0)
		return ret;

	if (!*++argv) {
		printf("0x%x:", wnmmask);
		for (i = 0; wl_wnm_msgs[i].value; i++)
			if (wnmmask & wl_wnm_msgs[i].value)
				printf("  %s", wl_wnm_msgs[i].string);
		printf("\n");
		return 0;
	}

	s = *argv;
	if (*s == '+' || *s == '-')
		s++;

	if (!*s)
		goto usage;

	val = strtoul(s, &s, 0);
	if (*s)
		goto usage;

	if (**argv == '+')
		wnmmask |= val;
	else if (**argv == '-')
		wnmmask &= ~val;
	else
		wnmmask = val;

	return wlu_iovar_setint(wl, cmd->name, wnmmask);

usage:
	fprintf(stderr, "WNM mask is a bitfield from the following set. "
		"Use + or - prefix to combine with current value:\n");

	for (i = 0; wl_wnm_msgs[i].value; i++)
		fprintf(stderr, " 0x%04x %s\n", wl_wnm_msgs[i].value, wl_wnm_msgs[i].string);

	return 0;
}

static int
wl_wnm_bsstq(void *wl, cmd_t *cmd, char **argv)
{
	int err, buflen;
	wlc_ssid_t ssid;

	UNUSED_PARAMETER(cmd);

	strcpy(buf, "wnm_bsstrans_query");
	buflen = strlen("wnm_bsstrans_query") + 1;

	if (*++argv) {
		uint32 len;

		len = strlen(*argv);
		if (len > DOT11_MAX_SSID_LEN) {
			printf("ssid too long\n");
			return (-1);
		}
		memset(&ssid, 0, sizeof(wlc_ssid_t));
		memcpy(ssid.SSID, *argv, len);
#ifdef DSLCPE
		ssid.SSID_len = htod32(len);
#else
		ssid.SSID_len = len;
#endif
		memcpy(&buf[buflen], &ssid, sizeof(wlc_ssid_t));
		buflen += sizeof(wlc_ssid_t);
	}

	err = wlu_set(wl, WLC_SET_VAR, buf, buflen);

	return err;
}

#define TCLAS_ARG_CHECK(argv, str) \
	do { \
		if (*++(argv) == NULL) { \
			printf("TCLAS Frame Classifier: %s not provided\n", (str)); \
			return BCME_ERROR; \
		} \
	} while (0)

static int
wl_tclas_add(void *wl, cmd_t *cmd, char **argv)
{
	int err = -1, buflen;
	dot11_tclas_fc_t *fc;

	UNUSED_PARAMETER(cmd);

	buflen = sprintf(buf, "%s", *argv) + 1;

	/* Check the user priority value */
	TCLAS_ARG_CHECK(argv, "user priority");
	buf[buflen++] = (uint8)strtoul(*argv, NULL, 0);

	/* unaligned! */
	fc = (dot11_tclas_fc_t *) (buf + buflen);
	memset((void *)fc, 0, sizeof(dot11_tclas_fc_t));

	/* Parse frame classifier type and mask */
	TCLAS_ARG_CHECK(argv, "type");
	fc->hdr.type = (uint8)strtoul(*argv, NULL, 0);

	TCLAS_ARG_CHECK(argv, "mask");
	fc->hdr.mask = (uint8)strtoul(*argv, NULL, 0);
	if (fc->hdr.type ==  DOT11_TCLAS_FC_0_ETH) {
		dot11_tclas_fc_0_eth_t *fc_eth = (dot11_tclas_fc_0_eth_t *)fc;

		TCLAS_ARG_CHECK(argv, "src mac");
		if (strlen(*argv) > 1) {
			if (!wl_ether_atoe(*argv, (struct ether_addr *)fc_eth->sa)) {
				printf(" ERROR: no valid src ether addr provided\n");
				return BCME_ERROR;
			}
		}
		else
			memset(fc_eth->sa, 0, ETHER_ADDR_LEN);

		TCLAS_ARG_CHECK(argv, "dst mac");
		if (strlen(*argv) > 1) {
			if (!wl_ether_atoe(*argv, (struct ether_addr *)fc_eth->da)) {
				printf(" ERROR: no valid dst ether addr provided\n");
				return BCME_ERROR;
			}
		}
		else
			memset(fc_eth->da, 0, ETHER_ADDR_LEN);

		TCLAS_ARG_CHECK(argv, "ether type");
		fc_eth->eth_type = hton16((uint16)strtoul(*argv, NULL, 0));

		buflen += DOT11_TCLAS_FC_0_ETH_LEN;

		err = wlu_set(wl, WLC_SET_VAR, buf, buflen);
	}
	else if (fc->hdr.type == DOT11_TCLAS_FC_1_IP ||
		fc->hdr.type == DOT11_TCLAS_FC_4_IP_HIGHER) {
		uint8 version;

		TCLAS_ARG_CHECK(argv, "ip version");
		version = (uint8)strtoul(*argv, NULL, 0);
		if (version == IP_VER_6) {
			dot11_tclas_fc_4_ipv6_t *fc_ipv6 = (dot11_tclas_fc_4_ipv6_t *)fc;
			uint32 fl;

			fc_ipv6->version = version;

			TCLAS_ARG_CHECK(argv, "ipv6 source ip");
			if (!wl_atoipv6(*argv, (struct ipv6_addr *)&fc_ipv6->saddr)) {
				printf("incorrect ipv6 source ip format\n");
				return BCME_ERROR;
			}

			TCLAS_ARG_CHECK(argv, "ipv6 dest ip");
			if (!wl_atoipv6(*argv, (struct ipv6_addr *)&fc_ipv6->daddr)) {
				printf("incorrect ipv6 dest ip format\n");
				return BCME_ERROR;
			}

			TCLAS_ARG_CHECK(argv, "ipv6 source port");
			fc_ipv6->src_port = hton16((uint16)strtoul(*argv, NULL, 0));

			TCLAS_ARG_CHECK(argv, "ipv6 dest port");
			fc_ipv6->dst_port = hton16((uint16)strtoul(*argv, NULL, 0));

			TCLAS_ARG_CHECK(argv, "ipv6 dscp");
			fc_ipv6->dscp = (uint8)strtoul(*argv, NULL, 0);

			TCLAS_ARG_CHECK(argv, "ipv6 next header");
			fc_ipv6->nexthdr = (uint8)strtoul(*argv, NULL, 0);

			TCLAS_ARG_CHECK(argv, "ipv6 flow label");
			fl = (uint32) strtoul(*argv, NULL, 0);
			fc_ipv6->flow_lbl[2] = (fl >> 16) & 0xFF;
			fc_ipv6->flow_lbl[1] = (fl >> 8) & 0xFF;
			fc_ipv6->flow_lbl[0] = (fl >> 0) & 0xFF;

			buflen += DOT11_TCLAS_FC_4_IPV6_LEN;

			err = wlu_set(wl, WLC_SET_VAR, buf, buflen);

		} else
		if (version == IP_VER_4) {
			dot11_tclas_fc_1_ipv4_t *fc_ipv4 = (dot11_tclas_fc_1_ipv4_t *)fc;

			fc_ipv4->version = version;

			TCLAS_ARG_CHECK(argv, "ipv4 source ip");
			if (!wl_atoip(*argv, (struct ipv4_addr *)&fc_ipv4->src_ip)) {
				printf("incorrect source ip format\n");
				return BCME_ERROR;
			}

			TCLAS_ARG_CHECK(argv, "ipv4 dest ip");
			if (!wl_atoip(*argv, (struct ipv4_addr *)&fc_ipv4->dst_ip)) {
				printf("incorrect dest ip format\n");
				return BCME_ERROR;
			}

			TCLAS_ARG_CHECK(argv, "ipv4 source port");
			fc_ipv4->src_port = (uint16)strtoul(*argv, NULL, 0);
			fc_ipv4->src_port = hton16(fc_ipv4->src_port);

			TCLAS_ARG_CHECK(argv, "ipv4 dest port");
			fc_ipv4->dst_port = (uint16)strtoul(*argv, NULL, 0);
			fc_ipv4->dst_port = hton16(fc_ipv4->dst_port);

			TCLAS_ARG_CHECK(argv, "ipv4 dscp");
			fc_ipv4->dscp = (uint8)strtoul(*argv, NULL, 0);

			TCLAS_ARG_CHECK(argv, "ipv4 protocol");
			fc_ipv4->protocol = (uint8)strtoul(*argv, NULL, 0);

			buflen += DOT11_TCLAS_FC_1_IPV4_LEN;

			err = wlu_set(wl, WLC_SET_VAR, buf, buflen);
		} else
			return BCME_ERROR;
	} else {
		printf("Unsupported frame classifier type 0x%2x\n", fc->hdr.type);
		return BCME_ERROR;
	}

	return err;
}

static int
wl_tclas_del(void *wl, cmd_t *cmd, char **argv)
{
	int err, buflen, totlen;

	UNUSED_PARAMETER(cmd);

	buflen = sprintf(buf, "%s", *argv) + 1;
	totlen = buflen + 2;

	/* Set initial length */
	buf[buflen] = 0;	/* first value present for init index */
	buf[buflen + 1] = 0;	/* second value present for delete count */

	/* Check idx and add if present */
	if (*++argv == NULL)
		goto set;
	buf[buflen] = 1;
	buf[buflen + 1] = (uint8)strtoul(*argv, NULL, 0);

	/* Check len and add if present */
	if (*++argv == NULL)
		goto set;
	buf[buflen] = 2;
	buf[buflen + 2] = (uint8)strtoul(*argv, NULL, 0);

	if (*++argv) {
		printf("Too much args provided\n");
		return BCME_ERROR;
	}
set:
	err = wlu_set(wl, WLC_SET_VAR, buf, totlen);

	return err;
}

static void
wl_tclas_dump(wl_tclas_t *tclas)
{
	dot11_tclas_fc_hdr_t *fc = &tclas->fc.hdr;
	dot11_tclas_fc_0_eth_t *eth = &tclas->fc.t0_eth;
	dot11_tclas_fc_1_ipv4_t *ipv4 = &tclas->fc.t1_ipv4;
	dot11_tclas_fc_4_ipv6_t *ipv6 = &tclas->fc.t4_ipv6;

#define TCDMP(x) ((fc->mask & (x))?'+':'-')

	printf("up %d  type %d  mask 0x%x  ", tclas->user_priority, fc->type, fc->mask);

	/* Check frame classifier parameter type */
	if (fc->type == DOT11_TCLAS_FC_0_ETH) {
		printf("(Eth: ");
		printf("%cSA %s  ", TCDMP(1), wl_ether_etoa((struct ether_addr*)eth->sa));
		printf("%cDA %s  ", TCDMP(2), wl_ether_etoa((struct ether_addr*)eth->da));
		printf("%ctype 0x%04x", TCDMP(4), ntoh16(eth->eth_type));
		printf(")\n");
	} else if (fc->type == DOT11_TCLAS_FC_1_IP ||
		(fc->type == DOT11_TCLAS_FC_4_IP_HIGHER && ipv4->version == IP_VER_4)) {
		/* Check parameter type 1/4 IPv4 version */
		printf("(IPv4: ");
		printf("%cver %d  ", TCDMP(1), ipv4->version);
		printf("%csip %s  ", TCDMP(2), wl_iptoa((struct ipv4_addr *)&ipv4->src_ip));
		printf("%cdip %s  ", TCDMP(4), wl_iptoa((struct ipv4_addr *)&ipv4->dst_ip));
		printf("%csp %d  ", TCDMP(8), ntoh16(ipv4->src_port));
		printf("%cdp %d  ", TCDMP(16), ntoh16(ipv4->dst_port));
		printf("%cdscp 0x%x  ", TCDMP(32), ipv4->dscp);
		printf("%cprot %d", TCDMP(64), ipv4->protocol);
		printf(")\n");
	} else if (fc->type == DOT11_TCLAS_FC_4_IP_HIGHER && ipv6->version == IP_VER_6) {
		/* Check parameter type 4 IPv6 version */
		printf("(IPv6: ");
		printf("%cver %d  ", TCDMP(1), ipv6->version);
		printf("%csip %s  ", TCDMP(2), wl_ipv6toa(ipv6->saddr));
		printf("%cdip %s  ", TCDMP(4), wl_ipv6toa(ipv6->daddr));
		printf("%csp %d  ", TCDMP(8), ntoh16(ipv6->src_port));
		printf("%cdp %d  ", TCDMP(16), ntoh16(ipv6->dst_port));
		printf("%cdscp 0x%x  ", TCDMP(32), ipv6->dscp);
		printf("%cprot %d", TCDMP(64), ipv6->nexthdr);
		printf("%cfl %d", TCDMP(64),
			ipv6->flow_lbl[2]<<16 | ipv6->flow_lbl[1]<<8 | ipv6->flow_lbl[0]);
		printf(")\n");
	} else {
		printf("(type unsupported)\n");
	}
}

static int
wl_tclas_list_parse(wl_tclas_list_t *list)
{
	uint32 i;
	uint8 *ptr = (uint8 *)&list->tclas[0];
	int retlen = 0;

	for (i = 0; i < list->num; i++) {
		wl_tclas_t *tclas = (wl_tclas_t *)ptr;

		printf("tclas idx %d: ", i);

		wl_tclas_dump(tclas);

		retlen += WL_TCLAS_FIXED_SIZE + tclas->fc_len;

		/* Move to the next tclas frame classifier parameter */
		ptr += WL_TCLAS_FIXED_SIZE + tclas->fc_len;
	}

	return retlen;
}

static int
wl_tclas_list(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	wl_tclas_list_t *tclas_list = NULL;
	void *ptr = NULL;

	if (*++argv) {
		printf("Too much args provided\n");
		return BCME_ERROR;
	}

	err = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr);
	if (err == BCME_OK) {
		tclas_list = (wl_tclas_list_t *)ptr;
#ifdef DSLCPE_ENDIAN
		tclas_list->num = dtoh32(tclas_list->num);
#endif
		if (tclas_list->num == 0)
			printf("No tclas frame classifier parameter entry\n");
		else
			wl_tclas_list_parse(tclas_list);
	}

	return err;
}

static int
wl_wnm_tfsreq_add(void *wl, cmd_t *cmd, char **argv)
{
	int argc;
	int err = -1, buflen;
	wl_tfs_req_t tfs_req;

	UNUSED_PARAMETER(cmd);

	/* arg count */
	for (argc = 0; argv[argc]; argc++)
		;

	strcpy(buf, "wnm_tfsreq_add");
	buflen = strlen("wnm_tfsreq_add") + 1;

	if (argc != 5 || *++(argv) == NULL) {
		printf("Incorrect args provided\n");
		return BCME_ERROR;
	}

	tfs_req.tfs_id = (uint8)strtoul(*argv++, NULL, 0);

	if (*argv != NULL)
		tfs_req.tfs_actcode = (uint8)strtoul(*argv++, NULL, 0);
	else {
		printf("Incorrect args provided\n");
		return BCME_ERROR;
	}

	if (*argv != NULL)
		tfs_req.tfs_subelem_id = (uint8)strtoul(*argv++, NULL, 0);
	else {
		printf("Incorrect args provided\n");
		return BCME_ERROR;
	}

	if (*argv != NULL)
		tfs_req.send = strtoul(*argv, NULL, 0) == 0 ? FALSE : TRUE;
	else {
		printf("Incorrect args provided\n");
		return BCME_ERROR;
	}

	if (tfs_req.tfs_id == 0 ||
		tfs_req.tfs_actcode > 3 ||
		tfs_req.tfs_subelem_id > 1) {
		printf("Input args not in range\n");
		return BCME_ERROR;
	}

	buf[buflen++] = tfs_req.tfs_id;
	buf[buflen++] = tfs_req.tfs_actcode;
	buf[buflen++] = tfs_req.tfs_subelem_id;
	buf[buflen++] = tfs_req.send;

	err = wlu_set(wl, WLC_SET_VAR, buf, buflen);

	return err;
}

static int
wl_wnm_dms_set(void *wl, cmd_t *cmd, char **argv)
{
	int err, buflen;
	wl_dms_set_t *dms_set;

	UNUSED_PARAMETER(cmd);

	buflen = sprintf(buf, "%s", *argv) + 1;
	dms_set = (wl_dms_set_t *) (buf + buflen);
	dms_set->user_id = 0;
	dms_set->tclas_proc = 0;
	buflen += sizeof(wl_dms_set_t);

	if (*++(argv) == NULL) {
		printf("Missing <send> argument\n");
		return BCME_ERROR;
	}
	dms_set->send = (uint8) strtoul(*argv, NULL, 0);

	if (*++(argv) == NULL)
		goto set;
	dms_set->user_id = (uint8) strtoul(*argv, NULL, 0);

	if (*++(argv) == NULL)
		goto set;
	dms_set->tclas_proc = (uint8) strtoul(*argv, NULL, 0);

	if (*++(argv)) {
		printf("extra argument\n");
		return BCME_ERROR;
	}
set:
	err = wlu_set(wl, WLC_SET_VAR, buf, buflen);

	return err;
}

static int
wl_wnm_dms_status(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	wl_dms_status_t *dms_list = (wl_dms_status_t *)buf;
	uint8 *p = (uint8 *)dms_list->desc;

	strcpy(buf, argv[0]);

	ret = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN);
	if (ret < 0)
		return ret;

	dms_list->cnt = dtoh32(dms_list->cnt);

	while (dms_list->cnt--) {

		wl_dms_desc_t *desc = (wl_dms_desc_t *)p;

		printf("DMS desc UserID %d:\n", desc->user_id);
		printf("\tstatus:%d   token:%d   DMS ID:%d   TCLAS proc:%d\n",
			desc->status, desc->token, desc->dms_id, desc->tclas_proc);

		p += WL_DMS_DESC_FIXED_SIZE;

		if (desc->mac_len) {
			printf("\tRegistered STA:\n");

			while (desc->mac_len) {
				printf("\t\t- %s\n", wl_ether_etoa((struct ether_addr*)p));
				p += ETHER_ADDR_LEN;
				desc->mac_len -= ETHER_ADDR_LEN;
			}
		}

		printf("\tTCLAS:\n");
		while (desc->tclas_len) {
			wl_tclas_t *tclas = (wl_tclas_t *)p;

			printf("\t\t- ");
			wl_tclas_dump(tclas);

			p += WL_TCLAS_FIXED_SIZE + tclas->fc_len;
			desc->tclas_len -= WL_TCLAS_FIXED_SIZE + tclas->fc_len;
		}
	}

	return 0;
}

static int
wl_wnm_dms_term(void *wl, cmd_t *cmd, char **argv)
{
	int err, buflen;
	wl_dms_term_t *dms_term;

	UNUSED_PARAMETER(cmd);

	buflen = sprintf(buf, "%s", *argv) + 1;

	dms_term = (wl_dms_term_t *) (buf + buflen);
	dms_term->user_id = 0;
	buflen += sizeof(wl_dms_term_t);

	if (*++(argv) == NULL) {
		printf("Missing <del> argument\n");
		return BCME_ERROR;
	}
	dms_term->del = (uint8) strtoul(*argv, NULL, 0);

	if (*++(argv) == NULL)
		goto set;
	dms_term->user_id = (uint8) strtoul(*argv, NULL, 0);

	if (*++(argv)) {
		printf("extra argument\n");
		return BCME_ERROR;
	}
set:
	err = wlu_set(wl, WLC_SET_VAR, buf, buflen);

	return err;
}

static int
wl_wnm_service_term(void *wl, cmd_t *cmd, char **argv)
{
	int err, buflen;
	wl_service_term_t *term;

	UNUSED_PARAMETER(cmd);

	buflen = sprintf(buf, "%s", *argv) + 1;

	term = (wl_service_term_t *) (buf + buflen);
	term->u.dms.user_id = 0;
	buflen += sizeof(wl_service_term_t);

	if (*++(argv) == NULL) {
		printf("Missing <service> argument\n");
		return BCME_ERROR;
	}
	term->service = (uint8) strtoul(*argv, NULL, 0);

	/* Need per service processing from here? */
	if (*++(argv) == NULL) {
		printf("Missing <del> argument\n");
		return BCME_ERROR;
	}
	term->u.dms.del = (uint8) strtoul(*argv, NULL, 0);

	if (*++(argv) == NULL)
		goto set;
	term->u.dms.user_id = (uint8) strtoul(*argv, NULL, 0);

	if (*++(argv)) {
		printf("extra argument\n");
		return BCME_ERROR;
	}
set:
	err = wlu_set(wl, WLC_SET_VAR, buf, buflen);

	return err;
}

static int
wl_wnm_timbc_offset(void *wl, cmd_t *cmd, char **argv)
{
	wl_timbc_offset_t *timbc_offset;
	char *p = buf;
	int size;
	UNUSED_PARAMETER(cmd);

	size = sprintf(p, "%s", *argv++) + 1;

	if (*argv == NULL) {
		/* retrieve bss idle period if argument count incorrect */
		int err = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN);
		if (err < 0)
			return err;

		timbc_offset = (wl_timbc_offset_t *)buf;
		printf("TIMBC offset: %d, tsf_present: %d, fix_interval: %d, rate_override: %d\n",
#ifdef DSLCPE_ENDIAN
		       dtoh16(timbc_offset->offset), timbc_offset->tsf_present, dtoh16(timbc_offset->fix_intv),
		       dtoh16(timbc_offset->rate_override));
#else
			timbc_offset->offset, timbc_offset->tsf_present, timbc_offset->fix_intv,
			timbc_offset->rate_override);
#endif

		return BCME_OK;

	} else {
		if (!isdigit((int)*argv[0]))
			return BCME_ERROR;

		timbc_offset = (wl_timbc_offset_t *)(p + size);

		timbc_offset->offset = (int16)strtoul(*argv++, NULL, 0);
		timbc_offset->tsf_present = TRUE;
		timbc_offset->fix_intv = 0;
		timbc_offset->rate_override = 0;

		if (*argv != NULL) {
			timbc_offset->tsf_present = (bool)strtoul(*argv++, NULL, 0);
			if (*argv != NULL) {
#ifdef DSLCPE_ENDIAN
			        timbc_offset->fix_intv = htod16((uint16)strtoul(*argv++, NULL, 0));
#else
				timbc_offset->fix_intv = (uint16)strtoul(*argv++, NULL, 0);
#endif
				if (*argv != NULL) {
					timbc_offset->rate_override =
#ifdef DSLCPE_ENDIAN
					        htod16((uint16)strtoul(*argv++, NULL, 0));
#else
						(uint16)strtoul(*argv++, NULL, 0);
#endif
				}
			}
		}

		return wlu_set(wl, WLC_SET_VAR, buf, size + sizeof(wl_timbc_offset_t));
	}
}

static int
wl_wnm_timbc_set(void *wl, cmd_t *cmd, char **argv)
{
	int err, buflen;
	wl_timbc_set_t *req;

	UNUSED_PARAMETER(cmd);

	buflen = sprintf(buf, "%s", *argv) + 1;

	req = (wl_timbc_set_t *) (buf + buflen);
	req->flags = 0;
	req->rate_min = 0;
	req->rate_max = 0;
	buflen += sizeof(wl_timbc_set_t);

	if (*++(argv) == NULL) {
		printf("Missing <interval> argument\n");
		return BCME_ERROR;
	}
	req->interval = (uint8) strtoul(*argv, NULL, 0);

	if (*++(argv) == NULL)
		goto set;
	req->flags = (uint8) strtoul(*argv, NULL, 0);

	if (*++(argv) == NULL)
		goto set;
	req->rate_min = htod16(rate_string2int(*argv));

	if (*++(argv) == NULL)
		goto set;
	req->rate_max = htod16(rate_string2int(*argv));

	if (*++(argv)) {
		printf("Too many arguments\n");
		return BCME_ERROR;
	}
set:
	err = wlu_set(wl, WLC_SET_VAR, buf, buflen);

	return err;
}

static int
wl_wnm_timbc_status(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	wl_timbc_status_t *status = (wl_timbc_status_t *)buf;
	char hrate[16], lrate[16];

	strcpy(buf, argv[0]);

	ret = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN);
	if (ret < 0)
		return ret;

	printf("TIM BC current status: %d  status_ap: %d\n"
		"  interval: %d   offset: %d\n"
		"  high rate: %s   low rate: %s\n",
		status->status_sta,
		status->status_ap,
#ifdef DSLCPE_ENDIAN
	        status->interval, dtoh32(status->offset),
	        rate_int2string(hrate, dtoh16(status->rate_high)),
	        rate_int2string(lrate, dtoh16(status->rate_low)));
#else
		status->interval, status->offset,
		rate_int2string(hrate, status->rate_high),
		rate_int2string(lrate, status->rate_low));
#endif

	return BCME_OK;
}

static int
wl_wnm_maxidle(void *wl, cmd_t *cmd, char **argv)
{
	struct {
		int period;
		int protect;
	} params = { 0, 0 };
	char *endptr;

	if (*++argv == NULL) {
		/* retrieve bss idle period if argument count incorrect */
		int err = wlu_iovar_getint(wl, cmd->name, &params.period);
		if (err < 0)
			return err;
#ifdef DSLCPE_ENDIAN
		printf("BSS Max Idle Period: %d\n", (params.period));
#else
		printf("BSS Max Idle Period: %d\n", dtoh32(params.period));
#endif
		return BCME_OK;

	} else {
		/* parse the idle period */
		params.period = strtoul(*argv++, &endptr, 0);
		params.period = htod32(params.period);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		/* parse the optional keep-alive protect flag */
		if (*argv) {
			params.protect = strtoul(*argv, &endptr, 0);
			params.protect = htod32(params.protect);
			if (*endptr != '\0')
				return BCME_USAGE_ERROR;
		}

		return wlu_var_setbuf(wl, cmd->name, &params, sizeof(params));
	}
}

static int
wl_wnm_bsstrans_req(void *wl, cmd_t *cmd, char **argv)
{
	int err = -1;
	wl_bsstrans_req_t bsstrans_req;
	int buflen = sprintf(buf, "%s", *argv) + 1;
	int tmp;

	UNUSED_PARAMETER(cmd);

	/* req mode parsing */
	if (*++argv == NULL) {
		fprintf(stderr, "%s: error: reqmode missing\n", __FUNCTION__);
		return BCME_ERROR;
	}
	bsstrans_req.reqmode = strtoul(*argv, NULL, 0);

	/* tbtt parsing */
	if (*++argv == NULL) {
		fprintf(stderr, "%s: error: tbtt missing\n", __FUNCTION__);
		return BCME_ERROR;
	}
	tmp = strtoul(*argv, NULL, 0);
	if (tmp > 65535 || tmp < 0) {
		fprintf(stderr, "%s: error: tbtt out of range(%d)\n", __FUNCTION__, tmp);
		return BCME_ERROR;
	}
	else
#ifdef DSLCPE_ENDIAN
	        bsstrans_req.tbtt = htod16(tmp);
#else
		bsstrans_req.tbtt = tmp;
#endif

	/* dur parsing */
	if (*++argv == NULL) {
		fprintf(stderr, "%s: error: dur missing\n", __FUNCTION__);
		return BCME_ERROR;
	}
	tmp = strtoul(*argv, NULL, 0);
	if (tmp > 65535 || tmp < 0) {
		fprintf(stderr, "%s: error: dur out of range(%d)\n", __FUNCTION__, tmp);
		return BCME_ERROR;
	}
	else
#ifdef DSLCPE_ENDIAN
	        bsstrans_req.dur = htod16(tmp);
#else
		bsstrans_req.dur = tmp;
#endif

	/* unicast/broadcast parsing */
	if (*++argv != NULL)
		bsstrans_req.unicast = strtoul(*argv, NULL, 0);
	else
		bsstrans_req.unicast = 1;

	memcpy(&buf[buflen], &bsstrans_req, sizeof(bsstrans_req));
	buflen += sizeof(bsstrans_req);

	err = wlu_set(wl, WLC_SET_VAR, buf, buflen);

	return err;
}

/*
 * Get or Set wnm keepalive configuration
 */
static int
wl_wnm_keepalives_max_idle(void *wl, cmd_t *cmd, char **argv)
{
	int err, argc;
	keepalives_max_idle_t keepalive;

	UNUSED_PARAMETER(cmd);

	argv++;

	if (*argv == NULL) {
		/* get current params */
		if ((err = wlu_iovar_get(wl, cmd->name, (void *) &keepalive,
			(sizeof(keepalive)))) < 0)
			return (err);

		printf("Keepalives_max_idle parameters -\n");
		printf("num_of_keepalives_per_bss_max_idle\t\t= %d\nmkeep_alive_index\t= %d"
#ifdef DSLCPE_ENDIAN
		        "\nmax_interval\t= %d\n", dtoh16(keepalive.keepalive_count),
		        keepalive.mkeepalive_index, dtoh16(keepalive.max_interval));
#else
			"\nmax_interval\t= %d\n", keepalive.keepalive_count,
			keepalive.mkeepalive_index, keepalive.max_interval);
#endif
	} else {
		char *endptr;
		/* Validate num of entries */
		for (argc = 0; argv[argc]; argc++);
		if (argc < 2 || argc > 3)
			return BCME_ERROR;
#ifdef DSLCPE_ENDIAN
		keepalive.keepalive_count = htod16(strtol(argv[0], &endptr, 0));
#else
		keepalive.keepalive_count = strtol(argv[0], &endptr, 0);
#endif
		keepalive.mkeepalive_index = strtol(argv[1], &endptr, 0);
		if (argc == 3)
#ifdef DSLCPE_ENDIAN
		        keepalive.max_interval = htod16(strtol(argv[2], &endptr, 0));
#else
			keepalive.max_interval = strtol(argv[2], &endptr, 0);
#endif
		else
			keepalive.max_interval = 0;

		/* Set params */
		err = wlu_iovar_set(wl, cmd->name, (void *) &keepalive,
			(sizeof(keepalives_max_idle_t)));
	}

	return err;
}

static int
wl_wnm_url(void *wl, cmd_t *cmd, char **argv)
{
	void *ptr;
	int err;
	uchar *data;
	uchar datalen, count;
	wnm_url_t *url;

	if (!*++argv) {
		err = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr);
		if (err == 0) {
			data = (uchar *)ptr;
			datalen = data[0];
			data ++;
			*(data + datalen) = '\0';
			printf("%s URL len %d\n", cmd->name, datalen);
			printf("%s URL: %s\n", cmd->name, data);
		}
		else {
			fprintf(stderr, "Error %d getting IOVar\n", err);
		}
		return err;
	}

	/* get URL length */
	datalen = (uchar)atoi(argv[0]);

	if (datalen > 0) {
		if (!argv[1]) {
			fprintf(stderr, "URL string should be specified for URL length %d\n",
				datalen);
			return BCME_BADARG;
		}
		else {
			if ((int)strlen (argv[1]) != datalen) {
				fprintf(stderr, "length is not matching to string\n");
				return BCME_BADARG;
			}
		}
	}

	if ((datalen == 0) && (argv[1] != NULL))
		fprintf(stderr, "Ignoring data bytes for length %d\n", datalen);

	count = sizeof(wnm_url_t) + datalen - 1;
	data = malloc(count);
	if (data == NULL) {
		fprintf(stderr, "memory alloc failure\n");
		return BCME_NOMEM;
	}

	url = (wnm_url_t *) data;
	url->len = datalen;
	if (datalen > 0) {
		memcpy(url->data, argv[1], datalen);
	}

	err = wlu_var_setbuf(wl, cmd->name, data, count);

	free(data);
	return (err);
}
