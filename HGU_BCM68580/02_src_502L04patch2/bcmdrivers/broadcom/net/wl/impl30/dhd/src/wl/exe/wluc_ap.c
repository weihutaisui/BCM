/*
 * wl ap command module
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
 * $Id: wluc_ap.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_maclist_1;
static cmd_func_t wl_management_info;
static cmd_func_t wl_bsscfg_enable;
static cmd_func_t wl_radar;

static cmd_t wl_ap_cmds[] = {
	{ "ap", wl_int, WLC_GET_AP, WLC_SET_AP,
	"Set AP mode: 0 (STA) or 1 (AP)" },
	{ "tkip_countermeasures", wl_int, -1, WLC_TKIP_COUNTERMEASURES,
	"Enable or disable TKIP countermeasures (TKIP-enabled AP only)\n"
	"\t0 - disable\n"
	"\t1 - enable" },
	{ "shortslot_restrict", wl_int, WLC_GET_SHORTSLOT_RESTRICT, WLC_SET_SHORTSLOT_RESTRICT,
	"Get/Set AP Restriction on associations for 11g Short Slot Timing capable STAs.\n"
	"\t0 - Do not restrict association based on ShortSlot capability\n"
	"\t1 - Restrict association to STAs with ShortSlot capability" },
	{ "ignore_bcns", wl_int, WLC_GET_IGNORE_BCNS, WLC_SET_IGNORE_BCNS,
	"AP only (G mode): Check for beacons without NONERP element"
	"(0=Examine beacons, 1=Ignore beacons)" },
	{ "scb_timeout", wl_int, WLC_GET_SCB_TIMEOUT, WLC_SET_SCB_TIMEOUT,
	"AP only: inactivity timeout value for authenticated stas" },
	{ "assoclist", wl_maclist, WLC_GET_ASSOCLIST, -1,
	"AP only: Get the list of associated MAC addresses."},
	{ "radar", wl_radar, WLC_GET_RADAR, WLC_SET_RADAR,
	"Enable/Disable radar. One-shot Radar simulation with optional sub-band"},
	{ "authe_sta_list", wl_maclist_1, WLC_GET_VAR, -1,
	"Get authenticated sta mac address list"},
	{ "autho_sta_list", wl_maclist_1, WLC_GET_VAR, -1,
	"Get authorized sta mac address list"},
	{ "beacon_info", wl_management_info, WLC_GET_VAR, -1,
	"Returns the 802.11 management frame beacon information\n"
	"Usage: wl beacon_info [-f file] [-r]\n"
	"\t-f Write beacon data to file\n"
	"\t-r Raw hex dump of beacon data" },
	{ "probe_resp_info", wl_management_info, WLC_GET_VAR, -1,
	"Returns the 802.11 management frame probe response information\n"
	"Usage: wl probe_resp_info [-f file] [-r]\n"
	"\t-f Write probe response data to file\n"
	"\t-r Raw hex dump of probe response data" },
	{ "bss", wl_bsscfg_enable, WLC_GET_VAR, WLC_SET_VAR,
	"set/get BSS enabled status: up/down"},
	{ "closednet", wl_bsscfg_int, WLC_GET_VAR, WLC_SET_VAR,
	"set/get BSS closed network attribute"},
	{ "ap_isolate", wl_bsscfg_int, WLC_GET_VAR, WLC_SET_VAR,
	"set/get AP isolation"},
	{ "mode_reqd", wl_bsscfg_int, WLC_GET_VAR, WLC_SET_VAR,
	"Set/Get operational capabilities required for STA to associate to the BSS "
	"supported by the interface.\n"
	"\tUsage: wl [-i ifname] mode_reqd [value]\n"
	"\t       wl mode_reqd [-C bss_idx ] [value]\n"
	"\t\t     <ifname> is the name of the interface corresponding to the BSS.\n"
	"\t\t\t   If the <ifname> is not given, the primary BSS is assumed.\n"
	"\t\t     <bss_idx> is the the BSS configuration index.\n"
	"\t\t\t   If the <bss_idx> is not given, configuraion #0 is assumed\n"
	"\t\t     <value> is the numeric values in the range [0..3]\n"
	"\t\t     0 - no requirements on joining devices.\n"
	"\t\t     1 - devices must advertise ERP (11g) capabilities to be allowed to associate\n"
	"\t\t\t   to a 2.4 GHz BSS.\n"
	"\t\t     2 - devices must advertise HT (11n) capabilities to be allowed to associate\n"
	"\t\t\t   to a BSS.\n"
	"\t\t     3 - devices must advertise VHT (11ac) capabilities to be allowed to associate\n"
	"\t\t\t   to a BSS.\n"
	"\tThe command returns an error if the BSS interface is up.\n"
	"\tThis configuration can only be changed while the BSS interface is down.\n"
	"\tNote that support for HT implies support for ERP,\n"
	"\tand support for VHT implies support for HT."},
#ifdef CUSTOMER_HW_31_1
	{ "pfn_nsc_spmac", wl_maclist_1, WLC_GET_VAR, WLC_SET_VAR,
	"Set or get the list of streetpass MAC addresses.\n"
	"\twl pfn_nsc_spmac xx:xx:xx:xx:xx:xx [xx:xx:xx:xx:xx:xx ...]\n"
	"\tTo Clear the list: wl mac none" },
#endif /* CUSTOMER_HW_31_1 */
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_ap_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register ap commands */
	wl_module_cmds_register(wl_ap_cmds);
}

/*
 * Get Radar Enable/Disable status
 * Set one-shot radar simulation with optional subband
 */
int
wl_radar(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int val;
	char *endptr = NULL;
	UNUSED_PARAMETER(cmd);

	if (!*++argv) {
		if ((ret = wlu_get(wl, WLC_GET_RADAR, &val, sizeof(int))) < 0) {
			return ret;
		}

		val = dtoh32(val);
		wl_printint(val);
	} else {
		struct {
			int val;
			uint sub;
		} radar;
		radar.val = strtol(*argv, &endptr, 0);
		if (*endptr != '\0') {
			/* not all the value string was parsed by strtol */
			return BCME_USAGE_ERROR;
		}

		radar.val = htod32(radar.val);
		radar.sub = 0;
		if (!*++argv) {
			ret = wlu_set(wl, WLC_SET_RADAR, &radar.val, sizeof(radar.val));
		} else {
			radar.sub = strtol(*argv, &endptr, 0);
			if (*endptr != '\0') {
				/* not all the value string was parsed by strtol */
				return BCME_USAGE_ERROR;
			}
			radar.sub = htod32(radar.sub);
			ret = wlu_set(wl, WLC_SET_RADAR, &radar, sizeof(radar));
		}
	}

	return ret;
}

static int
wl_bsscfg_enable(void *wl, cmd_t *cmd, char **argv)
{
	char *endptr;
	const char *val_name = "bss";
	int bsscfg_idx = 0;
	int val;
	int consumed;
	int ret;

	UNUSED_PARAMETER(cmd);

	/* skip the command name */
	argv++;

	/* parse a bsscfg_idx option if present */
	if ((ret = wl_cfg_option(argv, val_name, &bsscfg_idx, &consumed)) != 0)
		return ret;

	argv += consumed;
	if (consumed == 0) { /* Use the -i parameter if that was present */
		bsscfg_idx = -1;
	}

	if (!*argv) {
		bsscfg_idx = htod32(bsscfg_idx);
		ret = wlu_iovar_getbuf(wl, val_name, &bsscfg_idx, sizeof(bsscfg_idx),
		                      buf, WLC_IOCTL_MAXLEN);
		if (ret < 0)
			return ret;
		val = *(int*)buf;
		val = dtoh32(val);
		if (val)
			printf("up\n");
		else
			printf("down\n");
		return 0;
	} else {
		struct {
			int cfg;
			int val;
		} bss_setbuf;
		if (!stricmp(*argv, "move"))
			val = 4;
		else if (!stricmp(*argv, "ap"))
			val = 3;
		else if (!stricmp(*argv, "sta"))
			val = 2;
		else if (!stricmp(*argv, "up"))
			val = 1;
		else if (!stricmp(*argv, "down"))
			val = 0;
		else if (!stricmp(*argv, "del"))
			val = -1;
		else {
			val = strtol(*argv, &endptr, 0);
			if (*endptr != '\0') {
				/* not all the value string was parsed by strtol */
				return BCME_USAGE_ERROR;
			}
		}
		bss_setbuf.cfg = htod32(bsscfg_idx);
		bss_setbuf.val = htod32(val);

		return wlu_iovar_set(wl, val_name, &bss_setbuf, sizeof(bss_setbuf));
	}
}

static void dump_management_fields(uint8 *data, int len)
{
	int i, tag_len;
	uint8 tag;
	char temp[64];
	uint8 *p;

	while (len > 0) {
		/* Get the tag */
		tag = *data;
		data++; len--;

		/* Get the tag length */
		tag_len = (int) *data;
		data++; len--;

		printf("Tag:%d Len:%d - ", tag, tag_len);

		switch (tag) {
		case DOT11_MNG_SSID_ID:
			for (i = 0; i < tag_len; i++) {
				temp[i] = data[i];
			}
			if (i < 64) {
				temp[i] = '\0';
			}
			printf("SSID: '%s'\n", temp);
			break;
		case DOT11_MNG_FH_PARMS_ID:
			printf("FH Parameter Set\n");
			break;
		case DOT11_MNG_DS_PARMS_ID:
			printf("DS Parameter Set\n");
			break;
		case DOT11_MNG_CF_PARMS_ID:
			printf("CF Parameter Set\n");
			break;
		case DOT11_MNG_RATES_ID:
			printf("Supported Rates\n");
			break;
		case DOT11_MNG_TIM_ID:
			printf("Traffic Indication Map (TIM)\n");
			break;
		case DOT11_MNG_IBSS_PARMS_ID:
			printf("IBSS Parameter Set\n");
			break;
		case DOT11_MNG_COUNTRY_ID:
			p = data;
			printf("Country '%c%c%c'\n",
			       data[0], data[1], data[2]);
			p += DOT11_MNG_COUNTRY_ID_LEN;
			while (((data+tag_len) - p) >= DOT11_MNG_COUNTRY_ID_LEN) {
				printf("Start Channel: %d, Channels: %d, "
				       "Max TX Power: %d dBm\n",
				       p[0], p[1], p[2]);
				p += DOT11_MNG_COUNTRY_ID_LEN;
			}
			break;
		case DOT11_MNG_HOPPING_PARMS_ID:
			printf("Hopping Pattern Parameters\n");
			break;
		case DOT11_MNG_HOPPING_TABLE_ID:
			printf("Hopping Pattern Table\n");
			break;
		case DOT11_MNG_REQUEST_ID:
			printf("Request\n");
			break;
		case DOT11_MNG_QBSS_LOAD_ID:
			printf("QBSS Load\n");
			break;
		case DOT11_MNG_EDCA_PARAM_ID:
			printf("EDCA Parameter\n");
			break;
		case DOT11_MNG_CHALLENGE_ID:
			printf("Challenge text\n");
			break;
		case DOT11_MNG_PWR_CONSTRAINT_ID:
			printf("Power Constraint\n");
			break;
		case DOT11_MNG_PWR_CAP_ID:
			printf("Power Capability\n");
			break;
		case DOT11_MNG_TPC_REQUEST_ID:
			printf("Transmit Power Control (TPC) Request\n");
			break;
		case DOT11_MNG_TPC_REPORT_ID:
			printf("Transmit Power Control (TPC) Report\n");
			break;
		case DOT11_MNG_SUPP_CHANNELS_ID:
			printf("Supported Channels\n");
			break;
		case DOT11_MNG_CHANNEL_SWITCH_ID:
			printf("Channel Switch Announcement\n");
			break;
		case DOT11_MNG_MEASURE_REQUEST_ID:
			printf("Measurement Request\n");
			break;
		case DOT11_MNG_MEASURE_REPORT_ID:
			printf("Measurement Report\n");
			break;
		case DOT11_MNG_QUIET_ID:
			printf("Quiet\n");
			break;
		case DOT11_MNG_IBSS_DFS_ID:
			printf("IBSS DFS\n");
			break;
		case DOT11_MNG_ERP_ID:
			printf("ERP Information\n");
			break;
		case DOT11_MNG_TS_DELAY_ID:
			printf("TS Delay\n");
			break;
		case DOT11_MNG_HT_CAP:
			printf("HT Capabilities\n");
			break;
		case DOT11_MNG_QOS_CAP_ID:
			printf("QoS Capability\n");
			break;
		case DOT11_MNG_NONERP_ID:
			printf("NON-ERP\n");
			break;
		case DOT11_MNG_RSN_ID:
			printf("RSN\n");
			break;
		case DOT11_MNG_EXT_RATES_ID:
			printf("Extended Supported Rates\n");
			break;
		case DOT11_MNG_AP_CHREP_ID:
			printf("AP Channel Report\n");
			break;
		case DOT11_MNG_NEIGHBOR_REP_ID:
			printf("Neighbor Report\n");
			break;
		case DOT11_MNG_MDIE_ID:
			printf("Mobility Domain\n");
			break;
		case DOT11_MNG_FTIE_ID:
			printf("Fast BSS Transition\n");
			break;
		case DOT11_MNG_FT_TI_ID:
			printf("802.11R Timeout Interval\n");
			break;
		case DOT11_MNG_REGCLASS_ID:
			printf("Regulatory Class\n");
			break;
		case DOT11_MNG_EXT_CSA_ID:
			printf("Extended CSA\n");
			break;
		case DOT11_MNG_HT_ADD:
			printf("HT Information\n");
			break;
		case DOT11_MNG_EXT_CHANNEL_OFFSET:
			printf("Ext Channel\n");
			break;
		case DOT11_MNG_RRM_CAP_ID:
			printf("Radio Measurement\n");
			break;
		case DOT11_MNG_HT_BSS_COEXINFO_ID:
			printf("OBSS Coexistence INFO\n");
			break;
		case DOT11_MNG_HT_BSS_CHANNEL_REPORT_ID:
			printf("OBSS Intolerant Channel List\n");
			break;
		case DOT11_MNG_HT_OBSS_ID:
			printf("OBSS HT Info\n");
			break;
#ifdef DOT11_MNG_CHANNEL_USAGE
		case DOT11_MNG_CHANNEL_USAGE:
			printf("Channel Usage\n");
			break;
#endif // endif
		case DOT11_MNG_LINK_IDENTIFIER_ID:
			printf("TDLS Link Identifier\n");
			break;
		case DOT11_MNG_WAKEUP_SCHEDULE_ID:
			printf("TDLS Wakeup Schedule\n");
			break;
		case DOT11_MNG_CHANNEL_SWITCH_TIMING_ID:
			printf("TDLS Channel Switch Timing\n");
			break;
		case DOT11_MNG_PTI_CONTROL_ID:
			printf("TDLS PTI Control\n");
			break;
		case DOT11_MNG_PU_BUFFER_STATUS_ID:
			printf("TDLS PU Buffer Status\n");
			break;
		case DOT11_MNG_EXT_CAP_ID:
			printf("Management Ext Capability\n");
			break;
		case DOT11_MNG_PROPR_ID:
			printf("Proprietary\n");
			break;
		default:
			if (tag_len <= len) {
				printf("Unsupported tag\n");
			} else {
				/* Just dump the remaining data */
				printf("Unsupported tag error/malformed\n");
				tag_len = len;
			}
			break;
		} /* switch */

		wl_hexdump(data, tag_len);

		data += tag_len;
		len -= tag_len;
	} /* while */
}

static void dump_management_info(uint8 *data, int len)
{
	struct dot11_management_header hdr;
	struct dot11_bcn_prb parms;

	if (len <= (int) (sizeof(hdr)+sizeof(parms))) {
		/* Management packet invalid */
		return;
	}

	memcpy(&hdr, data, sizeof(hdr));
	data += sizeof(hdr);
	len -= sizeof(hdr);

	memcpy(&parms, data, sizeof(parms));
	data += sizeof(parms);
	len -= sizeof(parms);

	/* 802.11 MAC header */
	printf("Frame Ctl: 0x%04x\n", ltoh16(hdr.fc));
	printf("Duration : 0x%04x\n", ltoh16(hdr.durid));
	printf("Dest addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
	       hdr.da.octet[0],
	       hdr.da.octet[1],
	       hdr.da.octet[2],
	       hdr.da.octet[3],
	       hdr.da.octet[4],
	       hdr.da.octet[5]);
	printf("Src addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
	       hdr.sa.octet[0],
	       hdr.sa.octet[1],
	       hdr.sa.octet[2],
	       hdr.sa.octet[3],
	       hdr.sa.octet[4],
	       hdr.sa.octet[5]);
	printf("BSSID    : %02x:%02x:%02x:%02x:%02x:%02x\n",
	       hdr.bssid.octet[0],
	       hdr.bssid.octet[1],
	       hdr.bssid.octet[2],
	       hdr.bssid.octet[3],
	       hdr.bssid.octet[4],
	       hdr.bssid.octet[5]);
	printf("Seq ctl  : 0x%04x\n", hdr.seq);

	/* 802.11 management frame */
	printf("Timestamp: 0x%08x%08x\n",
	       ltoh32(parms.timestamp[0]), ltoh32(parms.timestamp[1]));
	printf("Beacon Interval: 0x%04x\n", ltoh16(parms.beacon_interval));
	printf("Capabilities: 0x%04x\n", ltoh32(parms.capability));

	dump_management_fields(data, len);
}

static int
wl_management_info(void *wl, cmd_t *cmd, char**argv)
{
	int ret = 0;
	int len;
	uint8 *data;
	FILE *fp = NULL;
	char *fname = NULL;
	int raw = 0;

	/* Skip the command name */
	argv++;

	while (*argv) {
		char *s = *argv;

		if (!strcmp(s, "-f") && argv[1] != NULL) {
			/* Write packet to a file */
			fname = argv[1];
			argv += 2;
		} else if (!strcmp(s, "-r")) {
			/* Do a hex dump to console */
			raw = 1;
			argv++;
		} else
			return BCME_USAGE_ERROR;
	}

	/* Get the beacon information */
	strcpy(buf, cmd->name);
	if ((ret = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN)) < 0)
		return ret;

	/*
	 * Dump out the beacon data. The first word (4 bytes) is the
	 * length of the management packet followed by the data itself.
	 */
	len = dtoh32(*(int *)buf);

	if (len <= 0) {
		/* Nothing to do */
		return ret;
	}

	data = (uint8 *) (buf + sizeof(int));
	printf("Data: %p Len: %d bytes\n", data, len);

	if (fname != NULL) {
		/* Write the packet to a file */
		if ((fp = fopen(fname, "wb")) == NULL) {
			fprintf(stderr, "Failed to open file %s\n",
			        fname);
			ret = BCME_BADARG;
		} else {
			ret = fwrite(data, 1, len, fp);

			if (ret != len) {
				fprintf(stderr,
				        "Error write %d bytes to file %s, rc %d!\n",
				        len, fname, ret);
				ret = -1;
			}
		}
	} else if (raw) {
		/* Hex dump */
		wl_hexdump(data, len);
	} else {
		/* Print management (w/some decode) */
		dump_management_info(data, len);
	}

	if (fp)
		fclose(fp);

	return ret;
}

static int
wl_maclist_1(void *wl, cmd_t *cmd, char **argv)
{
	struct maclist *maclist;
	struct ether_addr *ea;
	uint i;
	int ret;

	strcpy(buf, cmd->name);

	if (!argv[1]) {
		if ((ret = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN)) < 0)
			return ret;

		maclist = (struct maclist *)buf;

		for (i = 0, ea = maclist->ea; i < dtoh32(maclist->count); i++, ea++)
			printf("%s\n", wl_ether_etoa(ea));
		return 0;
	} else {
#ifndef CUSTOMER_HW_31_1
		return -1;
#else
		uint len;
		int oldlen;
		uint32 tmp;
		int cmdlen;
		uint max = (WLC_IOCTL_MEDLEN - sizeof(int)) / ETHER_ADDR_LEN;

		cmdlen = strlen(cmd->name) + 1;
		maclist = (struct maclist *)((char *)buf + cmdlen);

		if (cmd->set < 0)
			return -1;
		/* Clear list */
		/* maclist->count = htod32(0); */
		memset(&maclist->count, 0, sizeof(maclist->count));
		if (!stricmp(argv[1], "none") || !stricmp(argv[1], "clear"))
			return wlu_set(wl, cmd->set, buf, sizeof(*maclist) + cmdlen);

		/* Get old list */
		/* maclist->count = htod32(max); */
		tmp = htod32(max);
		bcopy(&maclist->count, &tmp, sizeof(maclist->count));
		if ((ret = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MEDLEN)) < 0)
			return ret;

		/* Append to old list */
		maclist = (struct maclist *)buf;
		maclist->count = dtoh32(maclist->count);
		if (maclist->count) {
			oldlen = (maclist->count * sizeof(maclist->ea)) + sizeof(maclist->count);
			memmove(buf + cmdlen, buf, oldlen);
		}

		strcpy(buf, cmd->name);
		maclist = (struct maclist *)((char *)buf + cmdlen);

		ea = &maclist->ea[maclist->count];
		argv++;
		while (*argv && maclist->count < max) {
			if (!wl_ether_atoe(*argv, ea)) {
				printf("Problem parsing MAC address \"%s\".\n", *argv);
				return BCME_USAGE_ERROR;
			}
			maclist->count++;
			ea++;
			argv++;
		}
		/* Set new list */
		len = sizeof(maclist->count) +
			maclist->count * sizeof(maclist->ea) +
			cmdlen;
		maclist->count = htod32(maclist->count);
		return wlu_set(wl, cmd->set, buf, len);
#endif /* CUSTOMER_HW_31_1 */
	}
}
