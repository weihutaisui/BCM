/*
 * wl rmc command module
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
 * $Id: wluc_relmcast.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_mcast_ackmac, wl_mcast_ackreq, wl_mcast_status;
static cmd_func_t wl_mcast_actf_time, wl_mcast_rssi_thresh, wl_mcast_stats;
static cmd_func_t wl_mcast_rssi_delta, wl_mcast_vsie, wl_mcast_ar_timeout;

static cmd_t wl_rmc_cmds[] = {
	{ "rmc_ackmac", wl_mcast_ackmac, WLC_GET_VAR, WLC_SET_VAR,
	"Set/Get ACK required multicast mac address\n"
	"\tusage: wl rmc_ackmac -i [index] -t [multicast mac address]"
	},
	{ "rmc_ackreq", wl_mcast_ackreq, WLC_GET_VAR, WLC_SET_VAR,
	"Set/Get ACK rmc_mode 0 disable, 1 enable transmitter, 2 enable initiator \n"
	"\tusage: wl rmc_ackreq [mode]"
	},
	{ "rmc_txrate", wl_phy_rate, WLC_GET_VAR, WLC_SET_VAR,
	"Set/Get a fixed transmit rate for the reliable multicast:\n"
	"\tvalid values for 802.11ac are (6, 9, 12, 18, 24, 36, 48, 54)\n"
	"\t-1 (default) means automatically determine the best rate"
	},
	{ "rmc_status", wl_mcast_status, WLC_GET_VAR, -1,
	"Display reliable multicast client status"
	},
	{ "rmc_actf_time", wl_mcast_actf_time, WLC_GET_VAR, WLC_SET_VAR,
	"Set/Get mcast action frame tx time period in ms\n"
	"\tusage: wl rmc_actf_time [value]"
	},
	{ "rmc_ar_timeout", wl_mcast_ar_timeout, WLC_GET_VAR, WLC_SET_VAR,
	"Set/Get rmc active receiver timeout in ms\n"
	"\tusage: wl rmc_ar_timeout [duration in ms]"
	},
	{ "rmc_rssi_thresh", wl_mcast_rssi_thresh, WLC_GET_VAR, WLC_SET_VAR,
	"Set/Get minimum rssi needed for a station to be an active receiver\n"
	"\tusage: wl rmc_rssi_thresh [value]"
	},
	{ "rmc_stats", wl_mcast_stats, WLC_GET_VAR, WLC_SET_VAR,
	"Display/Clear reliable multicast client statistical counters\n"
	"\tusage: wl rmc_stats [arg]"
	},
	{ "rmc_rssi_delta", wl_mcast_rssi_delta, WLC_GET_VAR, WLC_SET_VAR,
	"Display/Set RSSI delta to switch receive leader\n"
	"\tusage: wl rmc_rssi_delta [arg]"
	},
	{ "rmc_vsie", wl_mcast_vsie, WLC_GET_VAR, WLC_SET_VAR,
	"Display/Set vendor specific IE contents\n"
	"\tusage: wl rmc_vsie [OUI] [Data]"
	},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_rmc_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register rmc commands */
	wl_module_cmds_register(wl_rmc_cmds);
}

static int
wl_mcast_ackmac(void *wl, cmd_t *cmd, char **argv)
{
	int ret = 0;
	int i, err;
	uint8 num_tr;
	wl_rmc_entry_t ackmac_params;
	wl_rmc_entry_table_t tablelist;
	void *ptr = NULL;
	wl_rmc_trans_in_network_t *reply = NULL;
	miniopt_t to;
	struct ether_addr* ea = NULL;
	bool index_set = FALSE; bool mac_set = FALSE;

	/* muticast mac address - ipv4 & ipv6 resp. */
	uint8 mc_mac[6]		 = {0x1, 0x0, 0x5e, 0x7f, 0xff, 0xff};
	uint8 mc_mac_ipv6[6] = {0x33, 0x33, 0x0, 0x0, 0x0, 0x0};

	/* index will tell us what to do */

	if (!*++argv) {
		/* Get and display all entries in the table */
		if ((ret = wlu_var_getbuf(wl, cmd->name,
		             NULL,
		             0,
		             &ptr)) < 0) {
			return ret;
		}
		reply = (wl_rmc_trans_in_network_t*)ptr;
		num_tr = reply->num_tr;

		printf("\n Num of transmitters %02x ", num_tr);
		printf("\n Transmitter Mac \t AR  Mac");
		printf("\t rmc_ar_timeout\t amt_idx   flag ");

		for (i = 0; (i < WL_RMC_MAX_NUM_TRS) && (i < num_tr); i++) {
			printf("\n %s \t", wl_ether_etoa(&reply->trs[i].tr_mac));
			printf(" %s \t", wl_ether_etoa(&reply->trs[i].ar_mac));
			printf(" %d \t %d      %04x",
				reply->trs[i].artmo, reply->trs[i].amt_idx, reply->trs[i].flag);
		}

		printf("\n");
		return 0;
	}

	miniopt_init(&to, cmd->name, NULL, FALSE);

	while ((err = miniopt(&to, argv)) != -1) {
		if (err == 1) {
			return BCME_USAGE_ERROR;
		}
		argv += to.consumed;
		/* Index for multicast address in RMC table */
		if (to.opt == 'i') {
		/* wl rmc_ackmac -i 8 -t <control multi-cast address> */
		/* this is used to change the AF multi-cast address   */
			if (!to.good_int || ((to.val != 8) && to.val)) {
				fprintf(stderr, "%s: Invalid mode value\n", cmd->name);
				err = -1;
				goto exit;
			}
			tablelist.index = to.val;
			index_set = TRUE;
		}

		/* Add multicast address with index "i" to RMC table */
		if (to.opt == 't') {
			if (!wl_ether_atoe(to.valstr, &ackmac_params.addr)) {
				fprintf(stderr,
					"%s: could not parse \"%s\" as a MAC address\n",
					cmd->name, to.valstr);
				err = -1;
				goto exit;
			}
			mac_set = TRUE;
		}
	}

	if (index_set) {
		if (!mac_set) {
			printf("\n Invalid without mac address");
			ret =  BCME_USAGE_ERROR;
		} else if (tablelist.index == 8) {
			/* Index 8 is for changing the destination  address of the AF */
			/* This multi-cast address is the da of the action frame that */
			/* is sent out periodically                                   */
			memcpy(&tablelist.entry[0].addr, &ackmac_params.addr,
				sizeof(struct ether_addr));

			/* insert to list */
			ea = &tablelist.entry[0].addr;
			tablelist.entry[0].flag = RELMCAST_ENTRY_OP_ENABLE;

			/* for ipv4, initial three bytes in mc address are standard &
			    2 bytes for ipv6
			 */
			if ((!memcmp(ea, mc_mac, 3) && !(ea->octet[3] & 0x80)) ||
			     !memcmp(ea, mc_mac_ipv6, 2)) {

				fprintf(stderr,
					    "\nAdding multi-cast mac %s\n", wl_ether_etoa(ea));

				return wlu_var_setbuf(wl, cmd->name,
				         &tablelist,
				         sizeof(wl_rmc_entry_table_t));

			} else {

				fprintf(stderr, "multicast mac started with"
					"01:00:5e:0... or 33:33:...\n");
				ret = BCME_BADARG;

			}
		} else {
			printf("\n Invalid index ");
			ret = BCME_USAGE_ERROR;
		}
	}
exit:
	return ret;
}

static int
wl_mcast_ackreq(void *wl, cmd_t *cmd, char **argv)
{
	const char* fn_name = "wl_mcast_ackreq";
	int err, i = 0;
	uint argc;
	char *endptr = NULL;
	void *ptr = NULL;
	uint8 *reply_mode = NULL;
	uint8 params_mode, old_mode;
	wl_relmcast_status_t status;

	memset(&params_mode, 0, sizeof(uint8));
	/* toss the command name */
	argv++;

	if ((err = wlu_var_getbuf_sm(wl, cmd->name, &params_mode,
		sizeof(uint8), &ptr))  != BCME_OK) {
		fprintf(stderr, "Error getting variable %s\n", argv[0]);
		return err;
	}

	reply_mode = (uint8 *)ptr;
	old_mode = *reply_mode;

	if (!*argv) {
		fprintf(stderr, "%s mode %d \n", fn_name, *reply_mode);
		return err;
	}

	/* arg count */
	for (argc = 0; argv[argc]; argc++)
		;

	/* required arg: mode disable, enable  or initiator enabled */
	if (argc < 1)
		return -1;

	/* get the new ackreq mode */
	params_mode = strtol(argv[0], &endptr, 0);

	if ((*endptr != '\0') || (params_mode > WL_RMC_MODE_INITIATOR))
		return -1;

	if (argc > 1) {
		fprintf(stderr,
			"%s: could not parse extra argument %s:\n",
			fn_name, argv[1]);

		err = -1;
		goto exit;
	}

	if ((err = wlu_var_setbuf(wl, cmd->name, &params_mode, sizeof(uint8))) != BCME_OK) {
		goto out_of_here;
	}

	if (params_mode == WL_RMC_MODE_INITIATOR ||
	   ((params_mode == WL_RMC_MODE_RECEIVER) &&
	   (old_mode == WL_RMC_MODE_INITIATOR))) {

		for (i = 0; i <= 10; i++) {
			/* check status of the RCV bit to make sure ack are receive */
			if ((err = wlu_iovar_get(wl, "rmc_status",
			                    (void *) &status,
			                    (sizeof(wl_relmcast_status_t)))) < 0) {
				return (err);
			}
#ifdef DSLCPE_ENDIAN
			status.err = dtoh16(status.err);
#endif
			if (status.err != (uint16)BCME_NOTREADY) {
				if (status.err == (uint16)BCME_RXFAIL) {
					fprintf(stderr, "%s: error in setting mode: no ack receive"
					        "%d tx code %d \n",
					         fn_name,  params_mode, status.err);

					err = -1;
				} else {
					err = 0;
				}
				goto out_of_here;
			}

			/* Allow ample time (by staying in loop) to get ACK
			   for previous TX frame
			*/
			{
				volatile uint16 busycnt = -1;
				/* There is no system call for sleep that will work for */
				/*  all os flavors. When there is a common sleep call we */
				/*  can replace the following busy wait loop */
				while (--busycnt)
					;
				busycnt = -1;
			}
		} /* for loop */
	}
out_of_here:
	if ((err < 0) || (i > 10))
		fprintf(stderr, "%s: Error setting %d err %d \n", fn_name, params_mode, err);
	else
		fprintf(stderr, "%s: setting %d err %d \n", fn_name, params_mode, err);
exit:
	return 0;
}

static int
wl_mcast_status(void *wl, cmd_t *cmd, char **argv)
{
	int err, i;
	wl_relmcast_status_t status;

	if (!*++argv) {
		/* Get */
		if ((err = wlu_iovar_get(wl, cmd->name, (void *) &status,
			(sizeof(wl_relmcast_status_t)))) < 0)
			return (err);

		if (status.ver != WL_RMC_VER) {
			printf("Wrong Version %d %d\n", WL_RMC_VER, status.ver);
		} else if (status.num == 0) {
			printf("No clients associated\n");
		} else {
				for (i = 0; i < status.num; i++) {
					printf("%s\t%d\t%c%c%c\n",
						wl_ether_etoa(&status.clients[i].addr),
#ifdef DSLCPE_ENDIAN
					        dtoh16(status.clients[i].rssi),
#else
						status.clients[i].rssi,
#endif
						((status.clients[i].flag &
					        WL_RMC_FLAG_ACTIVEACKER) ? 'A' : ' '),
						((status.clients[i].flag &
					        WL_RMC_FLAG_INBLACKLIST) ? 'B' : ' '),
						((status.clients[i].flag &
					        WL_RMC_FLAG_RELMCAST) ? 'R' : ' '));
				}
				printf("Notification Frame TimePeriod: %d ms\n", status.actf_time);
		}
	} else {
		printf("Cannot set reliable multicast status\n");
	}

	return 0;
}

static int
wl_mcast_actf_time(void *wl, cmd_t *cmd, char **argv)
{
	int val, error = -1;
	const char *name = cmd->name;

	/* toss the command name from the args */
	argv++;

	if (!*argv) {
		error = wlu_iovar_getint(wl, name, &val);
		if (error < 0)
			return (error);
		printf("Action Frame tx time period: %dms\n", val);

	} else {

		val = (uint16)strtol(*argv, NULL, 10); /* 10 is for base 10 (decimal) */

		if (val >= WL_RMC_ACTF_TIME_MIN &&
			val <= WL_RMC_ACTF_TIME_MAX) {

			error = wlu_iovar_setint(wl, name, val);

		} else {

			printf("\"Out of range\": valid range %dms - %dms\n",
			                               WL_RMC_ACTF_TIME_MIN,
			                               WL_RMC_ACTF_TIME_MAX);
		}
	}
	return error;
}

static int
wl_mcast_ar_timeout(void *wl, cmd_t *cmd, char **argv)
{
	int val, error = -1;
	const char *name = cmd->name;

	/* toss the command name from the args */
	argv++;

	if (!*argv) {
		error = wlu_iovar_getint(wl, name, &val);
		if (error < 0)
			return (error);
		printf("Active Receiver time out: %dms\n", val);

	} else {
		val = (uint16)strtol(*argv, NULL, 10);
		if (val >= WL_RMC_ARTMO_MIN &&
			val <= WL_RMC_ARTMO_MAX)
			error = wlu_iovar_setint(wl, name, val);
		else
			printf("\"Out of range\": valid range %dms - %dms\n",
			WL_RMC_ARTMO_MIN,
			WL_RMC_ARTMO_MAX);
	}
	return error;
}

static int
wl_mcast_rssi_thresh(void *wl, cmd_t *cmd, char **argv)
{
	int val, error = -1;
	const char *name = cmd->name;

	/* toss the command name from the args */
	argv++;

	if (!*argv) {
		error = wlu_iovar_getint(wl, name, &val);
		if (error < 0)
			return (error);
		printf("rmc rssi: %d\n", val);

	} else {
		val = (int8)strtol(*argv, NULL, 10);
		error = wlu_iovar_setint(wl, name, val);
	}

	return error;
}

static int
wl_mcast_stats(void *wl, cmd_t *cmd, char **argv)
{
#define	PRCNT(name)	pbuf += sprintf(pbuf, "%s %u ", #name, dtoh16(cnts_v.name))
#define	PRCNT32(name)	pbuf += sprintf(pbuf, "%s %u ", #name, dtoh32(cnts_v.name))
	wl_rmc_cnts_t *cnts = NULL;
	wl_rmc_cnts_t cnts_v;
	int err = BCME_OK;
	uint8 argval, argc;
	char *pbuf = buf;
	char *endptr = NULL;
	void *ptr = NULL;

	if (!*++argv) {
		/* no arg - get and display all values */

		if ((err = wlu_var_getbuf (wl, cmd->name, &cnts_v, sizeof(wl_rmc_cnts_t),  &ptr)))
			return (err);

		cnts = (wl_rmc_cnts_t*)ptr;

		memcpy((wl_rmc_cnts_t*)&cnts_v, cnts, sizeof(wl_rmc_cnts_t));
		cnts_v.version = dtoh16(cnts->version);
		cnts_v.length = dtoh16(cnts->length);

		if (cnts_v.version != WL_RMC_CNT_VERSION) {
			printf("\tIncorrect version of counters struct: expected %d; got %d\n",
			        WL_RMC_CNT_VERSION, cnts->version);

			return -1;
		}

		PRCNT(dupcnt); PRCNT(ackreq_err); PRCNT(af_tx_err); PRNL();
		PRCNT(null_tx_err); PRCNT(af_unicast_tx_err); PRCNT(mc_no_amt_slot); PRNL();
		PRCNT(mc_not_mirrored); PRCNT(mc_existing_tr); PRCNT(mc_exist_in_amt); PRNL();
		PRCNT(mc_utilized); PRCNT(mc_taken_other_tr); PRCNT32(rmc_rx_frames_mac); PRNL();
		PRCNT32(rmc_tx_frames_mac); PRCNT32(mc_ar_role_selected);
		PRCNT32(mc_ar_role_deleted); PRNL();
		PRCNT32(mc_noacktimer_expired); PRCNT32(mc_null_ar_cnt);
		PRCNT(mc_no_wl_clk); PRNL();
		PRCNT(mc_tr_cnt_exceeded); PRNL();
		fputs(buf, stdout);

	} else {

			/* arg count */
			for (argc = 0; argv[argc]; argc++)
				;
			argval = strtol(argv[0], &endptr, 0);
			if (argval == 255) {
			/* arg is -1, clear all the values */
				fprintf(stderr, "clearing rmc counters\n");
				err = wlu_var_setbuf(wl, cmd->name, &cnts_v, sizeof(wl_rmc_cnts_t));

			} else {
				fprintf(stderr, "Invalid arg, only -1"
					"is allowed to clear counters\n");
				err = BCME_BADARG;
			}
	}
	return err;
}

static int
wl_mcast_rssi_delta(void *wl, cmd_t *cmd, char **argv)
{
	int val, error = -1;
	const char *name = cmd->name;

	/* toss the command name from the args */
	argv++;

	if (!*argv) {
		error = wlu_iovar_getint(wl, name, &val);
		if (error < 0)
			return (error);
		printf("rmc rssi delta: %d\n", val);

	} else {
		val = (uint16)strtol(*argv, NULL, 10);
		if (val >= 0) /* rssi delta value should be a whole number */
			error = wlu_iovar_setint(wl, name, val);
		else
			printf("\"Out of range\": rmc rssi delta should be >=0\n");
	}
	return error;
}

static int
wl_mcast_vsie(void *wl, cmd_t *cmd, char **argv)
{
	int ret = 0;
	void *ptr = NULL;
	wl_rmc_vsie_t *reply = NULL;
	wl_rmc_vsie_t vsie;
	char *parse = NULL;
	char tmp[4];
	int idx, cnt;

	if (!*++argv) {

		/* Get and display all entries in the table */
		if ((ret = wlu_var_getbuf(wl, cmd->name,
		             NULL,
		             0,
		             &ptr)) < 0) {
			return ret;
		}

		reply = (wl_rmc_vsie_t*)ptr;

		printf("0x%x%x%x 0x%x", reply->oui[0], reply->oui[1],
#ifdef DSLCPE_ENDIAN
		        reply->oui[2], dtoh16(reply->payload));
#else
			reply->oui[2], reply->payload);
#endif

	} else {

		parse = *argv++;

		/* remove "0x" from the input string which is in hex */
		if (strlen(parse)/2 > DOT11_OUI_LEN) {
			if (!strncmp("0x", parse, strlen("0x")) ||
			    !strncmp("0X", parse, strlen("0X"))) {
				parse += strlen("0x");
			}
		}

		/* if OUI string is not 6 characters, simply reject */
		if (strlen(parse) != DOT11_OUI_LEN * 2)
			return BCME_ERROR;

		/* parse oui string */
		for (idx = 0; idx < DOT11_OUI_LEN; idx++) {
			for (cnt = 0; cnt < 2; cnt++) {
				tmp[cnt] = *parse++;
			}
			tmp[cnt] = '\0';
			vsie.oui[idx] = (uint8)(strtoul(tmp, NULL, 16));
		}

		/* second argument is missing!! */
		if (!*argv) {
			return BCME_ERROR;
		}
#ifdef DSLCPE_ENDIAN
		vsie.payload = htod16((uint16)(strtoul(*argv, NULL, 16)));
#else
		vsie.payload = (uint16)(strtoul(*argv, NULL, 16));
#endif
		ret = wlu_var_setbuf(wl, cmd->name, &vsie, sizeof(vsie));
	}

	return ret;
}
