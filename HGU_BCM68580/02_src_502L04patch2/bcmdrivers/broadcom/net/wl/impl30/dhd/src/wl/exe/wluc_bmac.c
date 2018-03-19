/*
 * wl bmac command module
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
 * $Id: wluc_bmac.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_gpioout;
static cmd_func_t wl_nvsource;
static cmd_func_t wl_var_getinthex;
static cmd_func_t wl_otpw, wl_otpraw;
static cmd_func_t wl_devpath;
static cmd_func_t wl_diag;
static cmd_func_t wl_var_setintandprintstr;

static cmd_t wl_bmac_cmds[] = {
	{ "srcrc", wlu_srwrite, WLC_GET_SROM, -1,
	"Get the CRC for input binary file" },
	{ "cis_source", wl_varint, WLC_GET_VAR, -1,
	"Display which source is used for the SDIO CIS"},
	{ "nvram_source", wl_nvsource, WLC_GET_VAR, -1,
	"Display which source is used for nvram"},
	{ "customvar1", wl_var_getinthex, -1, -1,
	"print the value of customvar1 in hex format" },
	{ "gpioout", wl_gpioout, -1, -1,
	"Set any GPIO pins to any value. Use with caution as GPIOs would be "
	"assigned to chipcommon\n"
	"\tUsage: gpiomask gpioval"},
	{ "devpath", wl_devpath, WLC_GET_VAR, -1,
	"print device path" },
	{ "otpraw", wl_otpraw, WLC_GET_VAR, WLC_SET_VAR,
	"Read/Write raw data to on-chip otp\n"
	"Usage: wl otpraw <offset> <bits> [<data>]"},
	{ "otpw", wl_otpw, -1, WLC_OTPW,
	"Write an srom image to on-chip otp\n"
	"Usage: wl otpw file"},
	{ "nvotpw", wl_otpw, -1, WLC_NVOTPW,
	"Write nvram to on-chip otp\n"
	"Usage: wl nvotpw file"},
	{ "diag", wl_diag, WLC_GET_VAR, -1,
	"diag testindex(1-interrupt, 2-loopback, 3-memory, 4-led);"
	" precede by 'wl down' and follow by 'wl up'" },
	{ "otpdump", wl_var_setintandprintstr, WLC_GET_VAR, -1,
	"Dump raw otp"},
	{ "otpstat", wl_var_setintandprintstr, WLC_GET_VAR, -1,
	"Dump OTP status"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_bmac_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register bmac commands */
	wl_module_cmds_register(wl_bmac_cmds);
}

static int
wl_gpioout(void *wl, cmd_t *cmd, char **argv)
{
	uint32 mask;
	uint32 val;
	char *endptr = NULL;
	uint argc;
	uint32 *int_ptr;

	UNUSED_PARAMETER(cmd);

	val = 0;

	/* eat command name */
	argv++;

	/* arg count */
	for (argc = 0; argv[argc]; argc++)
		;

	/* Get and print the values */
	if (argc == 0) {
		uint32 gpio_cntrl;
		uint32 gpio_out;
		uint32 gpio_outen;
		int ret;

		if ((ret = wlu_iovar_get(wl, "gpioout", buf, sizeof(uint32) *3)) < 0)
			return ret;
		gpio_cntrl = dtoh32(((uint32 *)buf)[0]);
		gpio_out = dtoh32(((uint32 *)buf)[1]);
		gpio_outen = dtoh32(((uint32 *)buf)[2]);

		printf("gpiocontrol 0x%x gpioout 0x%x gpioouten 0x%x\n", gpio_cntrl,
		       gpio_out, gpio_outen);

		return 0;
	}

	/* required arg: mask value */
	if (argc < 2)
		return BCME_USAGE_ERROR;

	mask = strtoul(argv[0], &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;

	val = strtoul(argv[1], &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;

	if ((~mask & val) != 0)
		return BCME_BADARG;

	int_ptr = (uint32 *)buf;
	mask = htod32(mask);
	memcpy(int_ptr, (const void *)&mask, sizeof(mask));
	int_ptr++;
	val = htod32(val);
	memcpy(int_ptr, (const void *)&val, sizeof(val));

	return wlu_iovar_set(wl, "gpioout", buf, sizeof(uint32) *2);
}

static int
wl_nvsource(void *wl, cmd_t *cmd, char **argv)
{
	int32 val, err;

	if ((err = wl_var_get(wl, cmd, argv)))
		return (err);

	val = dtoh32(*(int32*)buf);

	switch (val) {
	case 0:
		printf("SROM\n");
		break;
	case 1:
		printf("OTP\n");
		break;
	case 2:
		printf("NVRAM\n");
		break;
	default:
		printf("Unrecognized source %d\n", val);
		break;
	}

	return 0;
}

#include <bcmnvram.h>

static int
wl_otpraw(void *wl, cmd_t *cmd, char **argv)
{
	char var[392];
	uint32 offset;
	uint32 bits;
	uint32 len;
	bool get = TRUE;
	void *ptr = NULL;
	char *endptr;
	uint32 i;

	if (argv[1]) {
		offset = htod32(strtoul(argv[1], &endptr, 0));
		memcpy(var, (char *)&offset, sizeof(offset));
		len = sizeof(offset);
	}
	else
		return BCME_USAGE_ERROR;

	if (argv[2]) {
		bits = htod32(strtoul(argv[2], &endptr, 0));
		if (bits > 3072)
		{
			printf("bit size (%d) too long or negative!!\n", bits);
			return BCME_BADARG;
		}
	}
	else
		bits = 1;

	memcpy(&var[len], (char *)&bits, sizeof(bits));
	len += sizeof(bits);

	if (argv[3]) {
		unsigned char data[768];
		uint32  patlen;
		char *inptr = argv[3];

		get = FALSE;

		if (*inptr == '0' && toupper((int)(*(inptr + 1))) == 'X')
			inptr += 2;

		patlen = strlen(inptr);
		if (patlen > 768 || (patlen * 4) < bits)
		{
			printf("data length (%d) too long or small!!\n", patlen);
			return BCME_USAGE_ERROR;
		}

		for (i = 1; i <= patlen; i++)
		{
			int n = (int)((unsigned char)*inptr++);
			if (!isxdigit(n)) {
				fprintf(stderr, "invalid hex digit %c\n", n);
				return BCME_USAGE_ERROR;
			}
			data[patlen - i] = (unsigned char)(isdigit(n) ? (n - '0')
				        : ((islower(n) ? (toupper(n)) : n) - 'A' + 10));
		}

		for (i = 0; i < patlen; i += 2)
		{
			unsigned char v;
			v = data[i];
			if (i + 1 < patlen)
				v += (data[i+1] * 16);
			memcpy(&var[len], (char *)&v, sizeof(v));
			len += sizeof(v);
		}

		printf("OTP RAM Write:");
		for (i = 0; i < bits; i += 8)
		{
			unsigned char v;
			v = var[2*sizeof(uint32) + (i/8)];

			if ((i % 64) == 0)
				printf("\nbit %4d:", offset + i);
			printf(" 0x%x", v);
		}
		printf("\n");

	}

	if (get) {
		int ret;
		unsigned char v, *cptr;

		if ((ret = wlu_var_getbuf(wl, cmd->name, var, sizeof(var), &ptr)) < 0) {
			printf("Error reading from OTP data\n");
			return ret;
		}

		cptr = (unsigned char *)ptr;

		printf("OTP RAM Read:");
		for (i = 0; i < bits; i += 8)
		{
			v = *cptr++;

			if ((i % 64) == 0)
				printf("\nbit %4d:", offset + i);
			printf(" 0x%02x", v);
		}
		printf("\n");
		return 0;
	}

	return wlu_var_setbuf(wl, cmd->name, &var, sizeof(var));
}

static int
wl_otpw(void *wl, cmd_t *cmd, char **argv)
{
#if !defined(BWL_FILESYSTEM_SUPPORT)
	UNUSED_PARAMETER(wl); UNUSED_PARAMETER(cmd); UNUSED_PARAMETER(argv);
	return (-1);
#elif	defined(_CFE_)
	UNUSED_PARAMETER(wl); UNUSED_PARAMETER(cmd); UNUSED_PARAMETER(argv);
	return CFE_ERR_UNSUPPORTED;
#elif	defined(DONGLEBUILD)
	UNUSED_PARAMETER(wl); UNUSED_PARAMETER(cmd); UNUSED_PARAMETER(argv);
	return 0;
#else
	FILE *fp;
	int ret = 0;
	struct nvram_header *nvr;
	char *p, otpw_buf[1024 - 128];
	const char *msg;
	int len;

	if (!*++argv)
		return BCME_USAGE_ERROR;

	if (!(fp = fopen(*argv, "rb"))) {
		fprintf(stderr, "%s: No such file or directory\n", *argv);
		return BCME_BADARG;
	}

	len = fread(otpw_buf, 1, sizeof(otpw_buf) - 1, fp);
	if ((ret = ferror(fp))) {
		printf("\nerror %d reading %s\n", ret, *argv);
		ret = BCME_ERROR;
		goto out;
	}
	if (!feof(fp)) {
		printf("\nFile %s too large\n", *argv);
		ret = BCME_ERROR;
		goto out;
	}

	/* Got the bits, do they look like the output of nvserial? */
	nvr = (struct nvram_header *)otpw_buf;
	if (nvr->magic == NVRAM_MAGIC) {
		if (cmd->set == WLC_OTPW) {
			printf("File %s looks like an nvserial file, use nvotpw\n", *argv);
			fflush(stdout);
			ret = BCME_ERROR;
			goto out;
		}
		len  = nvr->len - sizeof(struct nvram_header);
		if (len <= 0) {
			printf("Invalid length (%d)\n", len);
			ret = BCME_ERROR;
			goto out;
		}
		if (len & 1) {
			otpw_buf[len++] = '\0';
		}
		p = (char *)(nvr + 1);
		msg = "nvserial";
	} else {
		if (cmd->set == WLC_NVOTPW) {
			printf("File %s is not an nvserial file\n", *argv);
			ret = BCME_ERROR;
			goto out;
		}
		if (len & 1) {
			printf("File %s has an odd length (%d)\n", *argv, len);
			ret = BCME_ERROR;
			goto out;
		}
		p = otpw_buf;
		msg = "raw";
	}

	printf("Writing %d bytes from %s file %s to otp ...\n", len, msg, *argv);
	fflush(stdout);

#ifdef DSLCPE_ENDIAN
	nvr->magic = htod32(nvr->magic);
	nvr->len = htod32(nvr->len);
	nvr->crc_ver_init = htod32(nvr->crc_ver_init);
	nvr->config_refresh = htod32(nvr->config_refresh);
	nvr->config_ncdl = htod32(nvr->config_ncdl);
#endif
	if ((ret = wlu_set(wl, cmd->set, p, len)) < 0) {
		printf("\nError %d writing %s to otp\n", ret, *argv);
	}

out:
	fclose(fp);
	return ret;
#endif /* BWL_FILESYSTEM_SUPPORT */
}

/*
 * wlu_reg3args is a generic function that is used for setting/getting
 * WL_IOVAR variables that require address + offset for read, and
 * address + offset + data for write.
 */
int
wlu_reg3args(void *wl, cmd_t *cmd, char **argv)
{
	char var[256];
	uint32 int_val;
	bool get = TRUE;
	uint32 len, i;
	void *ptr = NULL;
	char *endptr;
	uint numargs;
	int ret = 0;

	len = 0;

	if (!argv[1] || !argv[2]) {
		printf("Wrong syntax => dev offset [val]\n");
		return BCME_USAGE_ERROR;
	}

	if (argv[3]) {
		numargs = 3;
		get = FALSE;
	} else
		numargs = 2;

	for (i = 1; i <= numargs; i++) {
		int_val = htod32(strtoul(argv[i], &endptr, 0));
		memcpy(&var[len], (char *)&int_val, sizeof(int_val));
		len += sizeof(int_val);
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
wl_var_getinthex(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	int32 val;

	if ((err = wl_var_get(wl, cmd, argv)))
		return (err);

	val = dtoh32(*(int32*)buf);

	printf("0x%08x\n", val);

	return 0;
}

/* Variation: Like getandprint, but allow an int arg to be passed */
static int
wl_var_setintandprintstr(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	int32 val;
	char *varname;
	char *endptr = NULL;

	UNUSED_PARAMETER(cmd);

	if (!*argv) {
		printf("set: missing arguments\n");
		return BCME_USAGE_ERROR;
	}

	varname = *argv++;

	if (!*argv) {
		val = 0;
	} else {
		val = strtol(*argv, &endptr, 0);
		if (*endptr != '\0') {
			/* not all the value string was parsed by strtol */
			printf("set: error parsing value \"%s\" as an integer for set of \"%s\"\n",
			       *argv, varname);
			return BCME_USAGE_ERROR;
		}
	}

	val = htod32(val);
	err = wlu_iovar_getbuf(wl, varname, &val, sizeof(int), buf, WLC_IOCTL_MAXLEN);

	if (err)
		return (err);

	printf("%s\n", buf);
	return (0);
}

void
wl_printlasterror(void *wl)
{
	char error_str[128];

	if (wlu_iovar_get(wl, "bcmerrorstr", error_str, sizeof(error_str)) != 0) {
		fprintf(stderr, "%s: \nError getting the last error\n", wlu_av0);
	} else {
		fprintf(stderr, "%s: %s\n", wlu_av0, error_str);
	}
}

static int
wl_devpath(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	void *ptr;
	char *pbuf = buf;

	UNUSED_PARAMETER(argv);

	if ((err = wlu_var_getbuf_sm (wl, cmd->name, NULL, 0, &ptr)))
		return (err);

	pbuf += strlen(buf);
	sprintf(pbuf, "\n");
	fputs(buf, stdout);
	return (0);
}

static int
wl_diag(void *wl, cmd_t *cmd, char **argv)
{
	uint testindex;
	int buflen, err;
	char *param;
	uint32 testresult;

	if (!*++argv) {
		printf(" Usage: %s testindex[1-4]\n", cmd->name);
		return BCME_USAGE_ERROR;
	}

	testindex = atoi(*argv);

	strcpy(buf, "diag");
	buflen = strlen(buf) + 1;
	param = (char *)(buf + buflen);
	testindex = htod32(testindex);
	memcpy(param, (char*)&testindex, sizeof(testindex));

	if ((err = wlu_get(wl, cmd->get, buf, WLC_IOCTL_MAXLEN)) < 0)
		return err;

	testresult = *(uint32 *)buf;
	testindex = dtoh32(testindex);
	testresult = dtoh32(testresult);
	if (testresult != 0) {
		printf("\ndiag test %d failed(error code %d)\n", testindex, testresult);
	} else
		printf("\ndiag test %d passed\n", testindex);

	return (0);
}
