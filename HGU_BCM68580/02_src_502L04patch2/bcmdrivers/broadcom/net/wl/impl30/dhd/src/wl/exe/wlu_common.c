/*
 * Common code for wl routines
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
 * $Id: wlu_common.c 514727 2014-11-12 03:02:48Z $
 */

#ifdef WIN32
#include <windows.h>
#endif // endif

#include "wlu_common.h"
#include "wlu.h"
#include <bcmendian.h>

extern int wl_get(void *wl, int cmd, void *buf, int len);
extern int wl_set(void *wl, int cmd, void *buf, int len);

wl_cmd_list_t cmd_list;
int cmd_pkt_list_num;
bool cmd_batching_mode;

const char *wlu_av0;

/* global wlc index indentifier for RSDB -W/--wlc option */
int g_wlc_idx = -1;

/* IOCTL swapping mode for Big Endian host with Little Endian dongle.  Default to off */
bool g_swap = FALSE;

#ifdef SERDOWNLOAD
extern int debug;
#endif // endif

/*
 * format an iovar buffer
 * iovar name is converted to lower case
 */
static uint
wl_iovar_mkbuf(const char *name, char *data, uint datalen, char *iovar_buf, uint buflen, int *perr)
{
	uint iovar_len;
	char *p;

	iovar_len = strlen(name) + 1;

	/* check for overflow */
	if ((iovar_len + datalen) > buflen) {
		*perr = BCME_BUFTOOSHORT;
		return 0;
	}

	/* copy data to the buffer past the end of the iovar name string */
	if (datalen > 0)
		memmove(&iovar_buf[iovar_len], data, datalen);

	/* copy the name to the beginning of the buffer */
	strcpy(iovar_buf, name);

	/* wl command line automatically converts iovar names to lower case for
	 * ease of use
	 */
	p = iovar_buf;
	while (*p != '\0') {
		*p = tolower((int)*p);
		p++;
	}

	*perr = 0;
	return (iovar_len + datalen);
}

void
init_cmd_batchingmode(void)
{
	cmd_pkt_list_num = 0;
	cmd_batching_mode = FALSE;
}

void
clean_up_cmd_list(void)
{
	wl_seq_cmd_pkt_t *this_cmd, *next_cmd;

	this_cmd = cmd_list.head;
	while (this_cmd != NULL) {
		next_cmd = this_cmd->next;
		if (this_cmd->data != NULL) {
			free(this_cmd->data);
		}
		free(this_cmd);
		this_cmd = next_cmd;
	}
	cmd_list.head = NULL;
	cmd_list.tail = NULL;
	cmd_pkt_list_num = 0;
}

int
add_one_batched_cmd(int cmd, void *cmdbuf, int len)
{
	wl_seq_cmd_pkt_t *new_cmd;

	new_cmd = malloc(sizeof(wl_seq_cmd_pkt_t));

	if (new_cmd == NULL) {
		printf("malloc(%d) failed, free %d batched commands and exit batching mode\n",
			(int)sizeof(wl_seq_cmd_pkt_t), cmd_pkt_list_num);
		goto free_and_exit;
	} else {
#ifdef SERDOWNLOAD
		if (debug)
#endif /* SERDOWNLOAD */
			printf("batching %dth command %d\n", cmd_pkt_list_num+1, cmd);

	}

	new_cmd->cmd_header.cmd = cmd;
	new_cmd->cmd_header.len = len;
	new_cmd->next  = NULL;

	new_cmd->data = malloc(len);

	if (new_cmd->data == NULL) {
		printf("malloc(%d) failed, free %d batched commands and exit batching mode\n",
			len, cmd_pkt_list_num);
		free(new_cmd);
		goto free_and_exit;
	}

	memcpy(new_cmd->data, cmdbuf, len);

	if (cmd_list.tail != NULL)
		cmd_list.tail->next = new_cmd;
	else
		cmd_list.head = new_cmd;

	cmd_list.tail = new_cmd;

	cmd_pkt_list_num ++;
	return 0;

free_and_exit:

	clean_up_cmd_list();

	if (cmd_batching_mode) {
		cmd_batching_mode = FALSE;
	}
	else {
		printf("calling add_one_batched_cmd() at non-command-batching mode, weird\n");
	}

	return -1;
}

int
wlu_get_req_buflen(int cmd, void *cmdbuf, int len)
{
	int modified_len = len;
	char *cmdstr = (char *)cmdbuf;

	if (len == WLC_IOCTL_MAXLEN) {
		if ((strcmp(cmdstr, "dump") == 0) ||
			(cmd == WLC_SCAN_RESULTS))
			modified_len = WLC_IOCTL_MAXLEN;
		else
			modified_len = WLC_IOCTL_MEDLEN;
	}
	return modified_len;
}

/* Wrapper function that converts -W option in to "wlc:" prefix
 * (1) It converts an existing iovar to the following format
 * wlc:<iovar_name>\0<wlc_idx><param>
 * (2) It converts an existing ioctl to the following format
 * wlc:ioc\0<wlc_idx><ioct_cmd_id><param>
 * NOTE: (2) requires new iovar named "ioc" in driver
*/
static int
wlu_wlc_wrapper(void *wl, bool get, int* cmd, void *cmdbuf, int len, void **outbuf, int *outlen)
{
	void *param = cmdbuf;
	int paramlen = len;
	int wlc_idx = g_wlc_idx;
	char *name = NULL;
	BCM_REFERENCE(wl);
	/* Wrap only if we find a valid WLC index and iovar name */
	if (wlc_idx >= 0) {
		int cmdlen = 0;
		int prefix_len = 0;
		char *lbuf = NULL;
		char *buf = NULL;
		bool ioctl_wrap = FALSE;
		if ((*cmd == WLC_GET_VAR) || (*cmd == WLC_SET_VAR)) {
			/* incoming command is an iovar */
			/* pull out name\0param */
			name = cmdbuf;
			cmdlen = strlen(name);
			param = ((char*)cmdbuf) + cmdlen + 1;
			paramlen = len - cmdlen - 1;
		} else {
			/* we are an ioctl, invoke the common "ioc" iovar and wrap the cmd */
			name = "ioc";
			cmdlen = strlen(name);
			/* additional 4 bytes for storing IOCTL_CMD_ID */
			prefix_len = sizeof(int);
			ioctl_wrap = TRUE;
		}
		prefix_len += strlen("wlc:") + 1 +  cmdlen + sizeof(int);
		/* now create wlc:<name>\0<wlc_idx><param> */
		buf = lbuf = malloc(prefix_len + paramlen);
		if (buf == NULL) {
			printf("%s:malloc(%d) failed\n", __FUNCTION__, prefix_len + paramlen);
			return BCME_NOMEM;
		}
		memcpy(buf, "wlc:", 4); buf += 4;
		strcpy(buf, name); buf += (cmdlen+1);
		wlc_idx = htod32(wlc_idx);
		memcpy(buf, &wlc_idx, sizeof(int32)); buf += sizeof(int32);
		if (ioctl_wrap) {
			/* For IOCTL wlc:ioc\0<wlc_idx><ioctl_id><param> */
			int32 ioctl_cmd = htod32(*cmd);
			memcpy(buf, &ioctl_cmd, sizeof(int32)); buf += sizeof(int32);
		}
		memcpy(buf, param, paramlen);
		*cmd = (get) ? WLC_GET_VAR : WLC_SET_VAR;
		param = lbuf;
		paramlen += prefix_len;
	}
	*outlen = paramlen;
	*outbuf = param;
	return BCME_OK;
}
/* now IOCTL GET commands shall call wlu_get() instead of wl_get() so that the commands
 * can be batched when needed
 */
int
wlu_get(void *wl, int cmd, void *cmdbuf, int len)
{
	void *outbuf = NULL;
	int outlen;
	int err = 0;
	if (cmd_batching_mode) {
		if (!WL_SEQ_CMDS_GET_IOCTL_FILTER(cmd)) {
			printf("IOCTL GET command %d is not supported in batching mode\n", cmd);
			return BCME_UNSUPPORTED;
		}
	}

	err = wlu_wlc_wrapper(wl, TRUE, &cmd, cmdbuf, len, &outbuf, &outlen);
	if (err != BCME_OK) return err;
	err = wl_get(wl, cmd, outbuf, outlen);

	if (outbuf != cmdbuf) {
		memcpy(cmdbuf, outbuf, len);
		free(outbuf);
	}
	return err;
}

/* now IOCTL SET commands shall call wlu_set() instead of wl_set() so that the commands
 * can be batched when needed
 */
int
wlu_set(void *wl, int cmd, void *cmdbuf, int len)
{
	int err = 0;
	void *outbuf = NULL;
	int outlen;

	err = wlu_wlc_wrapper(wl, FALSE, &cmd, cmdbuf, len, &outbuf, &outlen);
	if (err != BCME_OK) return err;

	if (cmd_batching_mode) {
		err = add_one_batched_cmd(cmd, outbuf, outlen);
	}
	else {
		err = wl_set(wl, cmd, outbuf, outlen);
	}
	if (outbuf != cmdbuf) {
		memcpy(cmdbuf, outbuf, len);
		free(outbuf);
	}
	return err;

}

/*
 * get named iovar providing both parameter and i/o buffers
 * iovar name is converted to lower case
 */
int
wlu_iovar_getbuf(void* wl, const char *iovar,
	void *param, int paramlen, void *bufptr, int buflen)
{
	int err;

	wl_iovar_mkbuf(iovar, param, paramlen, bufptr, buflen, &err);
	if (err)
		return err;

	return wlu_get(wl, WLC_GET_VAR, bufptr, buflen);
}

/*
 * set named iovar providing both parameter and i/o buffers
 * iovar name is converted to lower case
 */
int
wlu_iovar_setbuf(void* wl, const char *iovar,
	void *param, int paramlen, void *bufptr, int buflen)
{
	int err;
	int iolen;

	iolen = wl_iovar_mkbuf(iovar, param, paramlen, bufptr, buflen, &err);
	if (err)
		return err;

	return wlu_set(wl, WLC_SET_VAR, bufptr, iolen);
}

/*
 * get named iovar without parameters into a given buffer
 * iovar name is converted to lower case
 */
int
wlu_iovar_get(void *wl, const char *iovar, void *outbuf, int len)
{
	char smbuf[WLC_IOCTL_SMLEN];
	int err;

	/* use the return buffer if it is bigger than what we have on the stack */
	if (len > (int)sizeof(smbuf)) {
		err = wlu_iovar_getbuf(wl, iovar, NULL, 0, outbuf, len);
	} else {
		memset(smbuf, 0, sizeof(smbuf));
		err = wlu_iovar_getbuf(wl, iovar, NULL, 0, smbuf, sizeof(smbuf));
		if (err == 0)
			memcpy(outbuf, smbuf, len);
	}

	return err;
}

/*
 * set named iovar given the parameter buffer
 * iovar name is converted to lower case
 */
int
wlu_iovar_set(void *wl, const char *iovar, void *param, int paramlen)
{
	char smbuf[WLC_IOCTL_SMLEN*2];

	memset(smbuf, 0, sizeof(smbuf));

	return wlu_iovar_setbuf(wl, iovar, param, paramlen, smbuf, sizeof(smbuf));
}

/*
 * get named iovar as an integer value
 * iovar name is converted to lower case
 */
int
wlu_iovar_getint(void *wl, const char *iovar, int *pval)
{
	int ret;

	ret = wlu_iovar_get(wl, iovar, pval, sizeof(int));
	if (ret >= 0)
	{
		*pval = dtoh32(*pval);
	}
	return ret;
}

/*
 * set named iovar given an integer parameter
 * iovar name is converted to lower case
 */
int
wlu_iovar_setint(void *wl, const char *iovar, int val)
{
	val = htod32(val);
	return wlu_iovar_set(wl, iovar, &val, sizeof(int));
}
