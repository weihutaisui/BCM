/*
 * wl ota test command module
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
 * $Id: wluc_ota_test.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_ota_loadtest, wl_otatest_status, wl_load_cmd_stream;
static cmd_func_t wl_ota_teststop;

static cmd_t wl_ota_cmds[] = {
	{ "ota_teststop", wl_ota_teststop, -1, WLC_SET_VAR,
	"\tUsage: ota_teststop"
	},
	{ "ota_loadtest", wl_ota_loadtest, -1, WLC_SET_VAR,
	"\tUsage: ota_loadtest [filename] \n"
	"\t\tpicks up ota_test.txt if file is not given"
	},
	{ "ota_stream", wl_load_cmd_stream, -1, WLC_SET_VAR,
	"\tUsage: wl ota_stream start   : to start the test\n"
	"\twl ota_stream ota_sync \n"
	"\twl ota_stream test_setup synchtimeoout(seconds) synchbreak/loop synchmac txmac rxmac \n"
	"\twl ota_stream ota_tx chan bandwidth contrlchan rates stf txant rxant tx_ifs tx_len"
	"num_pkt pwrctrl start:delta:end \n"
	"\twl ota_stream ota_rx chan bandwidth contrlchan -1 stf txant rxant tx_ifs"
	"tx_len num_pkt \n"
	"\twl ota_stream stop   : to stop the test"
	},
	{ "ota_teststatus", wl_otatest_status, WLC_GET_VAR, -1,
	"\tUsage: otatest_status"
	"\t\tDisplays current running test details"
	"\totatest_status n	"
	"\t\tdisplays test arguments for nth line"
	},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_ota_module_init(void)
{
	(void)g_swap;

	/* get the global buf */
	buf = wl_get_buf();

	/* register ota test commands */
	wl_module_cmds_register(wl_ota_cmds);
}

#define WL_OTA_STRING_MAX_LEN		100
#define WL_OTA_CMDSTREAM_MAX_LEN	200

/* test_setup cmd argument ordering */
enum {
	WL_OTA_SYNC_TIMEOUT = 1,	/* Timeout in seconds */
	WL_OTA_SYNCFAIL_ACTION,		/* Fail actio -1/0/1 */
	WL_OTA_SYNC_MAC,		/* Mac address for sync */
	WL_OTA_TX_MAC,			/* Mac address for tx test */
	WL_OTA_RX_MAC,			/* Mac address for rx test */
	WL_OTA_LOOP_TEST		/* Put test into loop mode */
};

/* ota_tx / ota_rx format ordering */
enum {
	WL_OTA_CUR_TEST,		/* ota_tx or ota_rx */
	WL_OTA_CHAN,			/* cur channel */
	WL_OTA_BW,			/* cur bandwidth */
	WL_OTA_CONTROL_BAND,		/* cur control band */
	WL_OTA_RATE,			/* cur rate */
	WL_OTA_STF_MODE,		/* cur stf mode */
	WL_OTA_TXANT,			/* tx ant to be used */
	WL_OTA_RXANT,			/* rx ant to be used */
	WL_OTA_TX_IFS,			/* ifs */
	WL_OTA_TX_PKT_LEN,		/* pkt length */
	WL_OTA_TX_NUM_PKT,		/* num of packets */
	WL_OTA_PWR_CTRL_ON,		/* power control on/off */
	WL_OTA_PWR_SWEEP		/* start:delta:stop */
};
/* Various error chcking options */
enum {
	WL_OTA_SYNCFAILACTION,
	WL_OTA_CTRLBANDVALID,
	WL_OTA_TXANTVALID,
	WL_OTA_RXANTVALID,
	WL_OTA_PWRCTRLVALID
};
/* Display init test seq */
static void
wl_ota_display_test_init_info(wl_ota_test_status_t *init_info)
{
	printf("Test Init Summary\n");
	printf("----------------------------------------------------------\n");
	printf("Toatl Number of test req %d\n\n", init_info->test_cnt);
	printf("Sync timeout %d synch fail action: %d \n", init_info->sync_timeout,
		init_info->sync_fail_action);
	printf("Sync Mac address : \t");
	printf("%s\n", wl_ether_etoa(&(init_info->sync_mac)));
	printf("Tx Mac address : \t");
	printf("%s\n", wl_ether_etoa(&(init_info->tx_mac)));
	printf("Rx Mac address : \t");
	printf("%s\n", wl_ether_etoa(&(init_info->rx_mac)));
	printf("Test in Loop mode : %d \n", init_info->loop_test);
	printf("\n\n\n");
}
static void
wl_ota_display_rt_info(uint8 rate)
{
#ifndef D11AC_IOTYPES
	if (rate & NRATE_MCS_INUSE) {
		printf("m");
		printf("%d ", rate & NRATE_RATE_MASK);
	} else if (rate == 11) {
		printf("5.5 ");
	} else {
		printf("%d ", (rate & NRATE_RATE_MASK) / 2);
	}
#else
	printf("format changed : fix it %x \n", rate);
#endif // endif
}
/* display nth tesr arg details */
static void
wl_ota_display_test_option(wl_ota_test_args_t *test_arg, int16 cnt)
{
	uint8 i;
	printf("Test cnt %d  \n", cnt);
	printf("-----------------------------------------------------------\n");
	printf("Curr Test : %s\n", ((test_arg->cur_test == 0) ? "TX" : "RX"));
	printf("Wait for sync enabled %d \n", test_arg->wait_for_sync);
	printf("Channel : %d", test_arg->chan);
	printf("\t Bandwidth : %s ", ((test_arg->bw == WL_OTA_TEST_BW_20MHZ) ? "20" :
		((test_arg->bw == WL_OTA_TEST_BW_40MHZ) ? "40" : "20 in 40")));
	printf("\t Control Band : %c \n",  test_arg->control_band);
	printf("Rates : ");
	for (i = 0; i < test_arg->rt_info.rate_cnt; i++)
#ifdef DSLCPE_ENDIAN
	        wl_ota_display_rt_info(dtoh16(test_arg->rt_info.rate_val_mbps[i]));
#else
		wl_ota_display_rt_info(test_arg->rt_info.rate_val_mbps[i]);
#endif
	printf("\nStf mode :  %d \n", test_arg->stf_mode);
	printf("Txant: %d   rxant: %d \n", test_arg->txant, test_arg->rxant);
#ifdef DSLCPE_ENDIAN
	printf("Pkt eng Options :  ifs %d  len: %d num: %d \n", dtoh16(test_arg->pkteng.delay),
	        dtoh16(test_arg->pkteng.length), dtoh16(test_arg->pkteng.nframes));
#else
	printf("Pkt eng Options :  ifs %d  len: %d num: %d \n", test_arg->pkteng.delay,
		test_arg->pkteng.length, test_arg->pkteng.nframes);
#endif
	printf("Tx power sweep options :\nPower control %d \nstart pwr: %d  "
		"delta: %d end pwr : %d \n", test_arg->pwr_info.pwr_ctrl_on,
		test_arg->pwr_info.start_pwr, test_arg->pwr_info.delta_pwr,
		test_arg->pwr_info.end_pwr);
}
/* do minimum string validations possible */
/* Make stricter conditions in future */
static int
wl_ota_validate_string(uint8 arg, void* value)
{
	int ret = 0;
	switch (arg) {
		case WL_OTA_TXANTVALID:
		case WL_OTA_RXANTVALID:
			if (*(uint8*)value > 3)
				ret = -1;
			break;
		case WL_OTA_CTRLBANDVALID:
			if ((strncmp((char *)value, "l", 1)) && (strncmp((char *)value, "u", 1)))
				ret = -1;
			break;
		case WL_OTA_PWRCTRLVALID:
		case WL_OTA_SYNCFAILACTION:
			if ((*(int8 *)value < -1) || (*(int8 *)value > 1))
				ret = -1;
			break;
		default:
			break;
	}
	return ret;
}

/* convert power info string to integer */
/* start:delta:end */
static int
wl_ota_pwrinfo_parse(const char *tok_bkp, wl_ota_test_args_t *test_arg)
{
	char *endptr = NULL;
	int ret = 0;

	/* convert string to int */
	/* Read start pwr */
	test_arg->pwr_info.start_pwr = (int8)strtol(tok_bkp, &endptr, 10);
	if (*endptr == ':') {
		endptr++;
		tok_bkp = endptr;
	} else {
		return -1;
	}
	/* read delta pwr */
	test_arg->pwr_info.delta_pwr = (int8)strtol(tok_bkp, &endptr, 10);
	if (*endptr == ':') {
		endptr++;
		tok_bkp = endptr;
	} else {
		return -1;
	}
	/* read end pwr */
	test_arg->pwr_info.end_pwr = (int8)strtol(tok_bkp, &endptr, 10);

	if ((*endptr != '\0') && (*endptr != '\n') && (*endptr != ' '))
		ret = -1;

	return ret;
}

/* parsing the test init seq line */
static int
wl_ota_parse_test_init(wl_ota_test_vector_t * init_info, char * tok, uint16 cnt)
{
	int ret = 0;
	char * endptr = NULL;

	switch (cnt) {
		case WL_OTA_SYNC_TIMEOUT:
			init_info->sync_timeout = (uint8)strtol(tok, &endptr, 10);
			if (*endptr != '\0')
				ret = -1;
			break;
		case WL_OTA_SYNCFAIL_ACTION:
			init_info->sync_fail_action = (int8)strtol(tok, &endptr, 10);
			if (*endptr != '\0') {
				ret = -1;
				break;
			} else {
				ret = wl_ota_validate_string(WL_OTA_SYNCFAILACTION,
					&(init_info->sync_fail_action));
			}
			break;
		case WL_OTA_SYNC_MAC:
			if (!wl_ether_atoe(tok, &(init_info->sync_mac)))
				ret = -1;
			break;
		case WL_OTA_TX_MAC:
			if (!wl_ether_atoe(tok, &(init_info->tx_mac)))
				ret = -1;
			break;
		case WL_OTA_RX_MAC:
			if (!wl_ether_atoe(tok, &(init_info->rx_mac)))
				ret = -1;
			break;
		case WL_OTA_LOOP_TEST:
			init_info->loop_test = (int8)strtol(tok, &endptr, 10);

			if ((*endptr != '\0') && (*endptr != '\n') && (*endptr != ' '))
				ret = -1;
			break;
		default:
			break;
	}
	return ret;
}
/* parse test arguments */
static int
wl_ota_test_parse_test_option(wl_ota_test_args_t *test_arg, char * tok, uint16 cnt,
	char rt_string[])
{
	char * endptr = NULL;
	uint16 tok_len = 0;
	int ret = 0;

	if (test_arg->cur_test == WL_OTA_TEST_RX) {
		switch (cnt) {
			case WL_OTA_PWR_CTRL_ON:
			case WL_OTA_PWR_SWEEP:
				return 0;
				break;
			default:
				break;
		}
	}
	switch (cnt) {
		case WL_OTA_CUR_TEST:
			if (strncmp(tok, "ota_tx", 6) == 0)
				test_arg->cur_test = WL_OTA_TEST_TX;
			else if (strncmp(tok, "ota_rx", 6) == 0)
				test_arg->cur_test = WL_OTA_TEST_RX;
			else
				ret = -1;
			break;
		case WL_OTA_CHAN:
			test_arg->chan = (uint8)strtol(tok, &endptr, 10);
			if (*endptr != '\0')
				ret = -1;
			break;
		case WL_OTA_BW:
			if (strncmp(tok, "20/40", 5) == 0) {
				test_arg->bw = WL_OTA_TEST_BW_20_IN_40MHZ;
			} else if (strncmp(tok, "20", 2) == 0) {
				test_arg->bw = WL_OTA_TEST_BW_20MHZ;
			} else if (strncmp(tok, "40", 2) == 0) {
				test_arg->bw = WL_OTA_TEST_BW_40MHZ;
			} else {
				ret = -1;
			}
			break;
		case WL_OTA_CONTROL_BAND:
			test_arg->control_band = *tok;
			ret = wl_ota_validate_string(WL_OTA_CTRLBANDVALID, tok);
			break;
		case WL_OTA_RATE:
			tok_len = strlen(tok);
			if (tok_len > WL_OTA_STRING_MAX_LEN) {
				ret = -1;
				goto fail;
			}
			strncpy(rt_string, tok, tok_len);
			break;
		case WL_OTA_STF_MODE:
#ifndef D11AC_IOTYPES
			if (strncmp(tok, "siso", 4) == 0)
				test_arg->stf_mode = NRATE_STF_SISO;
			else if (strncmp(tok, "cdd", 3) == 0)
				test_arg->stf_mode = NRATE_STF_CDD;
			else if (strncmp(tok, "stbc", 4) == 0)
				test_arg->stf_mode = NRATE_STF_STBC;
			else if (strncmp(tok, "sdm", 3) == 0)
				test_arg->stf_mode = NRATE_STF_SDM;
			else
				ret = -1;
#endif // endif
			break;
		case WL_OTA_TXANT:
			test_arg->txant =  (uint8)strtol(tok, &endptr, 10);
			if (*endptr != '\0') {
				ret = -1;
				goto fail;
			}
			ret = wl_ota_validate_string(WL_OTA_TXANTVALID, &test_arg->txant);
			break;
		case WL_OTA_RXANT:
			test_arg->rxant = (uint8)strtol(tok, &endptr, 10);
			if (*endptr != '\0') {
				ret = -1;
				goto fail;
			}
			ret = wl_ota_validate_string(WL_OTA_RXANTVALID, &test_arg->rxant);
			break;
		case WL_OTA_TX_IFS:
#ifdef DSLCPE_ENDIAN
		        test_arg->pkteng.delay =  htod16((uint16)strtol(tok, &endptr, 10));
#else
			test_arg->pkteng.delay =  (uint16)strtol(tok, &endptr, 10);
#endif
			if (*endptr != '\0')
				ret = -1;
			break;
		case WL_OTA_TX_PKT_LEN:
#ifdef DSLCPE_ENDIAN
		        test_arg->pkteng.length = htod16((uint16)strtol(tok, &endptr, 10));
#else
			test_arg->pkteng.length = (uint16)strtol(tok, &endptr, 10);
#endif
			if (*endptr != '\0')
				ret = -1;
			break;
		case WL_OTA_TX_NUM_PKT:
#ifdef DSLCPE_ENDIAN
		        test_arg->pkteng.nframes = htod16((uint16)strtol(tok, &endptr, 10));
#else
			test_arg->pkteng.nframes = (uint16)strtol(tok, &endptr, 10);
#endif
			if ((*endptr != '\0') && (*endptr != '\n') && (*endptr != ' '))
				ret = -1;
			break;
		case WL_OTA_PWR_CTRL_ON:
			test_arg->pwr_info.pwr_ctrl_on = (int8)strtol(tok, &endptr, 10);
			if (*endptr != '\0') {
				ret = -1;
				goto fail;
			}
			ret = wl_ota_validate_string(WL_OTA_PWRCTRLVALID,
				&test_arg->pwr_info.pwr_ctrl_on);
			break;
		case WL_OTA_PWR_SWEEP:
			ret = wl_ota_pwrinfo_parse(tok, test_arg);
		default:
			break;
	}

fail:
	return ret;
}
static int
wl_ota_test_parse_rate_string(wl_ota_test_args_t *test_arg, char rt_string[100])
{

	uint8 cnt = 0;
	char * tok = NULL;
	char rate_st[5] = "\0";
	uint8 int_val = 0;
	uint16 tok_len = 0;
	int ret = 0;
	tok = strtok(rt_string, ",");

	/* convert rate strings to int array */
	while (tok != NULL) {
		strncpy(rate_st, " ", 4);
		/* skip rate parsing if its rx test case */
		if (test_arg->cur_test == WL_OTA_TEST_RX) {
			test_arg->rt_info.rate_val_mbps[cnt] = 0;
			cnt = 1;
			break;
		}

		/* Support a max of 30 rates */
		if (cnt >= WL_OTA_TEST_MAX_NUM_RATE) {
			ret = -1;
			break;
		}
		tok_len = strlen(tok);
		if (tok_len > 5) {
			ret = -1;
			break;
		}
		strncpy(rate_st, tok, tok_len);
		if (strncmp(rate_st, "5.5", 3) == 0) {
			int_val = 11;
		} else {
			if (rate_st[0] == 'm') {
				rate_st[0] = ' ';
				int_val = atoi(rate_st);
#ifndef D11AC_IOTYPES
				int_val |= NRATE_MCS_INUSE;
#endif // endif
			} else {
				int_val = 2 * atoi(rate_st);
			}
		}
#ifdef DSLCPE_ENDIAN
		test_arg->rt_info.rate_val_mbps[cnt] = htod16(int_val);
#else
		test_arg->rt_info.rate_val_mbps[cnt] = int_val;
#endif
		tok = strtok(NULL, ",");
		cnt++;
	}

	test_arg->rt_info.rate_cnt = cnt;
	return ret;
}
static int
wl_ota_test_parse_arg(char line[], wl_ota_test_vector_t *ota_test_vctr, uint16 *test_cnt,
	uint8 *ota_sync_found)
{
	char * tok = NULL;
	char rt_string[WL_OTA_STRING_MAX_LEN] = "\0";
	uint16 cnt = 0;
	int ret = -1;
	tok = strtok(line, " ");

	if (tok == NULL)
		goto fail;

	/* Initialize the power arguments */
	ota_test_vctr->test_arg[*test_cnt].pwr_info.pwr_ctrl_on = -1;
	ota_test_vctr->test_arg[*test_cnt].pwr_info.start_pwr = -1;
	ota_test_vctr->test_arg[*test_cnt].pwr_info.delta_pwr = -1;
	ota_test_vctr->test_arg[*test_cnt].pwr_info.end_pwr = -1;

	if (!strncmp(tok, "test_setup", 10))  {
		/* Parse test setup details */
		cnt = 0;
		while (tok != NULL) {
			if ((ret = wl_ota_parse_test_init(ota_test_vctr, tok, cnt)) != 0)
				return ret;
			tok = strtok(NULL, " ");
			cnt++;
		}
	} else if (!(strncmp(tok, "ota_tx", 6)) || (!strncmp(tok, "ota_rx", 6))) {
		/* parse tx /rx test argumenst */
		cnt = 0;
		while (tok != NULL) {
			if ((ret = wl_ota_test_parse_test_option
				(&(ota_test_vctr->test_arg[*test_cnt]),
				tok, cnt, rt_string)) != 0) {
				goto fail;
			}
			tok = strtok(NULL, " ");
			cnt++;
		}

		/* split rate string into integer array */
		if ((ret = wl_ota_test_parse_rate_string(&(ota_test_vctr->test_arg[*test_cnt]),
			rt_string)) != 0) {
			goto fail;
		}

		/* Add sync option if specified by user */
		ota_test_vctr->test_arg[*test_cnt].wait_for_sync  = (*ota_sync_found);

		/* Reset ota_sync_found for next test arg */
		*ota_sync_found = 0;

		/* Increment test cnt */
		*test_cnt = *test_cnt + 1;
	} else if (strncmp(tok, "ota_sync", 8) == 0) {
		/* detect if a sync packet is required */
		*ota_sync_found = 1;
		ret = 0;
	}
fail:
	return (ret);
}
static int
wl_load_cmd_stream(void *wl, cmd_t *cmd, char **argv)
{
	int ret = -1;
	char test_arg[WL_OTA_CMDSTREAM_MAX_LEN] = "\0";
	uint16 test_cnt = 0;
	uint8 * ptr1 = NULL;
	uint8 i, num_loop = 0;
	uint8 ota_sync_found = 0;
	unsigned int cmdlen = 0;

	wl_seq_cmd_pkt_t *next_cmd;
	wl_ota_test_vector_t *ota_test_vctr = NULL;

	argv++;

	if (*argv == NULL) {
		return ret;
	} else if (!strncmp(argv[0], "start", 5)) {
		ret = wl_seq_start(wl, cmd, argv);
	} else if (!strncmp(argv[0], "stop", 4)) {
		ret = 0;
		/* test info pointer */
		ota_test_vctr = (wl_ota_test_vector_t *)malloc(sizeof(wl_ota_test_vector_t));

		if (ota_test_vctr == NULL) {
			fprintf(stderr, "Failed to allocate  %d bytes of memory \n",
				(uint16)sizeof(wl_ota_test_vector_t));
			return BCME_NOMEM;
		}

		/* Assign a new pointer so that byte wise operation is possible */
		ptr1 = (uint8 *)ota_test_vctr;

		/* Passing test structure to dongle happens in steps */
		/* For OTA implementations its split up into chunks of 1200 bytes */
		num_loop = sizeof(wl_ota_test_vector_t) / WL_OTA_ARG_PARSE_BLK_SIZE;

		if (!cmd_batching_mode) {
			printf("calling ota_stream stop when it's already out of batching mode\n");
			ret = BCME_ERROR;
			goto fail;
		}
		cmd_batching_mode = FALSE;
		next_cmd = cmd_list.head;
		if (next_cmd == NULL) {
			printf("no command batched\n");
			ret = 0;
			goto fail;
		}
		test_cnt = 0;
		while (next_cmd != NULL) {
			/* Max number of test options is ARRAYSIZE(ota_test_vctr->test_arg) */
			if (test_cnt == ARRAYSIZE(ota_test_vctr->test_arg))
				break;

			if ((ret = wl_ota_test_parse_arg(next_cmd->data, ota_test_vctr, &test_cnt,
				&ota_sync_found)) != 0) {
				printf("Error Parsing the test command \n");
				ret = BCME_BADARG;
				goto fail;
			}
			next_cmd = next_cmd->next;
		}
		ota_test_vctr->test_cnt = test_cnt;
		/* Full size of wl_ota_test_vector_t can not be parse through wl */
		/* max size whihc can be passed from host to dongle is limited by eth size */
		for (i = 0; i <= num_loop; i++) {
			/* pass on the test info to wl->test_info structure */
			if ((ret = wlu_var_setbuf(wl, "ota_loadtest", ptr1 + i *
				WL_OTA_ARG_PARSE_BLK_SIZE, WL_OTA_ARG_PARSE_BLK_SIZE)) < 0) {
				fprintf(stderr, "host to dongle download failed to pass  %d"
					"bytes in stage %d \n",
					WL_OTA_ARG_PARSE_BLK_SIZE, i);
			}
		}
fail:
		clean_up_cmd_list();
		free(ota_test_vctr);
	} else {
		cmdlen = sizeof(test_arg) - 1;
		while (*argv) {
			strncat(test_arg, *argv, cmdlen);
			cmdlen -= strlen(*argv);
			argv++;
			if (*argv) {
				strncat(test_arg, " ", 1);
				cmdlen--;
			}
			if (*argv && cmdlen < (strlen(*argv)))
			{
				fprintf(stderr, "\n Insufficient length for cmd buffer\n");
				return -1;
			}
		}
		return add_one_batched_cmd(WLC_SET_VAR, test_arg, strlen(test_arg));

	}
	return ret;
}

static int
wl_ota_loadtest(void *wl, cmd_t *cmd, char **argv)
{
	int ret = -1;
	FILE *fp;
	const char *fname = "ota_test.txt";
	char line[WL_OTA_CMDSTREAM_MAX_LEN] = "\0";
	char line_bkp[WL_OTA_CMDSTREAM_MAX_LEN] = "\0";
	uint16 test_cnt = 0;
	uint8 * ptr1 = NULL;
	uint8 i, num_loop = 0;
	uint8 ota_sync_found = 0;
	wl_ota_test_vector_t *ota_test_vctr = NULL;

	UNUSED_PARAMETER(wl); UNUSED_PARAMETER(cmd);

	/* Read the file name */
	if (argv[1]) {
		fname = argv[1];
	}

	/* test info pointer */
	ota_test_vctr = (wl_ota_test_vector_t *)malloc(sizeof(wl_ota_test_vector_t));

	if (ota_test_vctr == NULL) {
		fprintf(stderr, "Failed to allocate  %d bytes of memory \n",
			(uint16)sizeof(wl_ota_test_vector_t));
		return BCME_NOMEM;
	}

	/* Assign a new pointer so that byte wide operation is possible */
	ptr1 = (uint8 *)ota_test_vctr;

	/* find number of iterations required to parse full block form host to dongle */
	num_loop = sizeof(wl_ota_test_vector_t) / WL_OTA_ARG_PARSE_BLK_SIZE;

	/* open the flow file */
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "Problem opening file %s\n", fname);
		free(ota_test_vctr);
		return BCME_BADARG;
	}

	test_cnt = 0;
	while (1) {
		fgets(line, WL_OTA_CMDSTREAM_MAX_LEN - 1, fp);

		if (feof(fp)) {
			break;
		}

		/* Max number of test options is ARRAYSIZE(ota_test_vctr->test_arg) */
		if (test_cnt == ARRAYSIZE(ota_test_vctr->test_arg))
			break;

		strncpy(line_bkp, line, WL_OTA_CMDSTREAM_MAX_LEN - 1);

		if ((ret = wl_ota_test_parse_arg(line_bkp, ota_test_vctr,
			&test_cnt, &ota_sync_found)) != 0) {
			printf("Flow File Error: \nError Parsing string : %s \n", line);
			ret = BCME_BADARG;
			goto fail;
		}
	}
	if (ota_sync_found) {
		ret = -1;
		printf("Flow File Error : \nFile can not end with ota_sync\n");
		goto fail;
	}
#ifdef DSLCPE_ENDIAN
	ota_test_vctr->test_cnt = htod16(test_cnt);
#else
	ota_test_vctr->test_cnt = test_cnt;
#endif
	/* Full size of wl_ota_test_vector_t can not be parse through wl */
	/* max size whihc can be passed from host to dongle is limited by eth size */
	for (i = 0; i <= num_loop; i++) {
		/* pass on the test info to wl->test_info structure */
		if ((ret = wlu_var_setbuf(wl, cmd->name, ptr1 + i * WL_OTA_ARG_PARSE_BLK_SIZE,
			WL_OTA_ARG_PARSE_BLK_SIZE)) < 0) {
			fprintf(stderr, "host to dongle download failed to pass  %d"
				"bytes in stage %d \n",
				WL_OTA_ARG_PARSE_BLK_SIZE, i);
			break;
		}
	}
fail:
	/* close the fp */
	if (fp)
		fclose(fp);

	free(ota_test_vctr);
	return ret;
}
static void
wl_otatest_display_skip_test_reason(int8 skip_test_reason)
{
	switch (skip_test_reason) {
		case 0 :
			printf("Test successfully finished\n");
			break;
		case WL_OTA_SKIP_TEST_CAL_FAIL:
			printf("Phy cal Failure \n");
			break;
		case WL_OTA_SKIP_TEST_SYNCH_FAIL:
			printf("Sync Packet failure \n");
			break;
		case WL_OTA_SKIP_TEST_FILE_DWNLD_FAIL:
			printf("File download Failure \n");
			break;
		case WL_OTA_SKIP_TEST_NO_TEST_FOUND:
			printf("No test found in the flow file \n");
			break;
		case WL_OTA_SKIP_TEST_WL_NOT_UP:
			printf("WL Not UP \n");
			break;
		case WL_OTA_SKIP_TEST_UNKNOWN_CALL:
			printf("Erroneous scheduling of test. Not intended \n");
			break;
		default:
			printf("Unknown test state \n");
			break;
	}
}
static int
wl_otatest_status(void *wl, cmd_t *cmd, char **argv)
{
	int ret = 0;
	int16 cnt = 0;
	wl_ota_test_status_t *test_status = NULL;
	wl_ota_test_vector_t *ota_test_vctr = NULL;

	UNUSED_PARAMETER(wl); UNUSED_PARAMETER(cmd);

	test_status = (wl_ota_test_status_t *)buf;
	if (argv[1]) {

		cnt = atoi(argv[1]);
		if ((cnt < 1) || ((uint16)cnt > ARRAYSIZE(ota_test_vctr->test_arg))) {
			printf("Error, Out of range \n");
			return BCME_RANGE;
		}
		/* read nth test arg details */
#ifdef DSLCPE_ENDIAN
		cnt = htod16(cnt);
#endif
		if ((ret = wlu_iovar_getbuf(wl, cmd->name, &cnt, sizeof(uint16),
			buf, WLC_IOCTL_MAXLEN)) < 0)
			return ret;
#ifdef DSLCPE_ENDIAN
		cnt = dtoh16(cnt);
		test_status->test_cnt = dtoh16(test_status->test_cnt);
#endif
		if (cnt > (test_status->test_cnt)) {
			printf("Error : Number of test seq downloaded %d  \n",
				test_status->test_cnt);
			return BCME_RANGE;
		}

		/* Display Test init info */
		wl_ota_display_test_init_info(test_status);

		/* Dsiplay test arg info */
		wl_ota_display_test_option(&(test_status->test_arg), cnt);
	} else {
		/* read back current state */
		if ((ret = wlu_iovar_getbuf(wl, cmd->name, NULL, 0,
			buf, WLC_IOCTL_MAXLEN)) < 0)
			return ret;
#ifdef DSLCPE_ENDIAN
		cnt = dtoh16(test_status->cur_test_cnt);
#else
		cnt = test_status->cur_test_cnt;
#endif
		switch (test_status->test_stage) {
			case WL_OTA_TEST_IDLE:		/* Idle state */
				printf("Init state \n");
				break;
			case WL_OTA_TEST_ACTIVE:	/* Active test state */
				/* Read back details for current test arg */
				cnt++;
				ret = wlu_iovar_getbuf(wl, cmd->name, &cnt, sizeof(uint16),
					buf, WLC_IOCTL_MAXLEN);
				if (test_status->sync_status == WL_OTA_SYNC_ACTIVE)
					printf("Waiting for sync \n");
				else
					wl_ota_display_test_option(&(test_status->test_arg), cnt);
				break;
			case WL_OTA_TEST_SUCCESS:	/* Test Finished */
				printf("Test completed \n");
				break;
			case WL_OTA_TEST_FAIL:		/* Test Failed to complete */
				wl_otatest_display_skip_test_reason(test_status->skip_test_reason);
				break;
			default:
				printf("Invalid test Phase \n");
				break;
		}
	}
	return ret;
}
/* To stop the ota test suite */
static int
wl_ota_teststop(void *wl, cmd_t *cmd, char **argv)
{
	UNUSED_PARAMETER(argv);
	return (wlu_iovar_setint(wl, cmd->name, 1));
}

int
ota_loadtest(void *wl, char *command, char **argv)
{
	int ret = -1;
	FILE *fp;
	const char *fname = "ota_test.txt";
	char line[WL_OTA_CMDSTREAM_MAX_LEN] = "\0";
	char line_bkp[WL_OTA_CMDSTREAM_MAX_LEN] = "\0";
	uint16 test_cnt = 0;
	uint8 * ptr1 = NULL;
	uint8 i, num_loop = 0;
	uint8 ota_sync_found = 0;
	wl_ota_test_vector_t *ota_test_vctr = NULL;

	UNUSED_PARAMETER(wl);

	/* Read the file name */
	if (argv[1]) {
		fname = argv[1];
	} else {
		fprintf(stderr, "Missing ota test flow file name\n");
		return BCME_BADARG;
	}

	/* test info pointer */
	ota_test_vctr = (wl_ota_test_vector_t *)malloc(sizeof(wl_ota_test_vector_t));

	if (ota_test_vctr == NULL) {
		fprintf(stderr, "Failed to allocate  %d bytes of memory \n",
			(uint16)sizeof(wl_ota_test_vector_t));
		return BCME_NOMEM;
	}

	/* Assign a new pointer so that byte wide operation is possible */
	ptr1 = (uint8 *)ota_test_vctr;

	/* find number of iterations required to parse full block form host to dongle */
	num_loop = sizeof(wl_ota_test_vector_t) / WL_OTA_ARG_PARSE_BLK_SIZE;

	/* open the flow file */
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "Problem opening file %s\n", fname);
		free(ota_test_vctr);
		return BCME_BADARG;
	}

	test_cnt = 0;
	while (1) {
		fgets(line, WL_OTA_CMDSTREAM_MAX_LEN - 1, fp);

		if (feof(fp)) {
			break;
		}

		/* Max number of test options is ARRAYSIZE(ota_test_vctr->test_arg) */
		if (test_cnt == ARRAYSIZE(ota_test_vctr->test_arg))
			break;

		strncpy(line_bkp, line, WL_OTA_CMDSTREAM_MAX_LEN - 1);

		if ((ret = wl_ota_test_parse_arg(line_bkp, ota_test_vctr,
			&test_cnt, &ota_sync_found)) != 0) {
			printf("Flow File Error: \nError Parsing string : %s \n", line);
			ret = BCME_BADARG;
			goto fail;
		}
	}
	if (ota_sync_found) {
		ret = -1;
		printf("Flow File Error : \nFile can not end with ota_sync\n");
		goto fail;
	}
	ota_test_vctr->test_cnt = test_cnt;

	/* Full size of wl_ota_test_vector_t can not be parse through wl */
	/* max size whihc can be passed from host to dongle is limited by eth size */
	for (i = 0; i <= num_loop; i++) {
		/* pass on the test info to wl->test_info structure */
		if ((ret = wlu_var_setbuf(wl, command, ptr1 + i * WL_OTA_ARG_PARSE_BLK_SIZE,
			WL_OTA_ARG_PARSE_BLK_SIZE)) < 0) {
			fprintf(stderr, "host to dongle download failed to pass  %d"
				"bytes in stage %d \n",
				WL_OTA_ARG_PARSE_BLK_SIZE, i);
			break;
		}
	}
fail:
	/* close the fp */
	if (fp)
		fclose(fp);

	free(ota_test_vctr);
	return ret;
}
