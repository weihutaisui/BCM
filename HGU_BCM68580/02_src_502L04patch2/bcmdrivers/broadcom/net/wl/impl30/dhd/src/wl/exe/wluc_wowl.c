/*
 * wl wowl command module
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
 * $Id: wluc_wowl.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_nshostip;
static cmd_func_t wl_wowl_pattern, wl_wowl_wakeind, wl_wowl_pkt, wl_wowl_status;
static cmd_func_t wl_wowl_wake_reason, wl_wowl_extended_magic;

static cmd_t wl_wowl_cmds[] = {
	{ "ns_hostip", wl_nshostip, WLC_GET_VAR, WLC_SET_VAR,
	"Add a ns-ip address or display then"},
	{ "ns_hostip_clear", wl_var_void, -1, WLC_SET_VAR,
	"Clear all ns-ip addresses"},
	{ "wowl", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"Enable/disable WOWL events\n"
	"  0   - Clear all events\n"
	"Bit 0 - Wakeup on Magic Packet\n"
	"Bit 1 - Wakeup on NetPattern (use 'wl wowl_pattern' to configure pattern)\n"
	"Bit 2 - Wakeup on loss-of-link due to Disassociation/Deauth\n"
	"Bit 3 - Wakeup on retrograde tsf\n"
	"Bit 4 - Wakeup on loss of beacon (use 'wl wowl_bcn_loss' to configure time)"},
	{ "wowl_bcn_loss", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"Set #of seconds of beacon loss for wakeup event"},
	{ "wowl_pattern", wl_wowl_pattern, -1, -1,
	"usage: wowl_pattern [ [clr | [[ add | del ] offset mask value ]]]\n"
	"No options -- lists existing pattern list\n"
	"add -- Adds the pattern to the list\n"
	"del -- Removes a pattern from the list\n"
	"clr -- Clear current list\n"
	"offset -- Starting offset for the pattern\n"
	"mask -- Mask to be used for pattern. Bit i of mask => byte i of the pattern\n"
	"value -- Value of the pattern"
	},
	{ "wowl_wakeind", wl_wowl_wakeind, WLC_GET_VAR, WLC_SET_VAR,
	"usage: wowl_wakeind [clear]\n"
	"Shows last system wakeup event indications from PCI and D11 cores\n"
	"clear - Clear the indications"
	},
	{ "wowl_status", wl_wowl_status, WLC_GET_VAR, -1,
	"usage: wowl_status [clear]\n"
	"Shows last system wakeup setting"
	},
	{"wowl_pkt", wl_wowl_pkt, -1, -1,
	"Send a wakeup frame to wakup a sleeping STA in WAKE mode\n"
	"Usage: wl wowl_pkt <len> <dst ea | bcast | ucast <STA ea>>"
	"[ magic [<STA ea>] | net <offset> <pattern> <reason code> ]\n"
	"e.g. To send bcast magic frame -- "
	"wl wowl_pkt 102 bcast magic 00:90:4c:AA:BB:CC\n"
	"     To send ucast magic frame -- "
	"wl wowl_pkt 102 ucast 00:90:4c:aa:bb:cc magic\n"
	"     To send a frame with L2 unicast - "
	"wl wowl_pkt 102 00:90:4c:aa:bb:cc net 0 0x00904caabbcc 0x03\n"
	" NOTE: offset for netpattern frame starts from \"Dest EA\" of ethernet frame."
	"So dest ea will be used only when offset is >= 6\n"
	"     To send a eapol identity frame with L2 unicast - "
	"wl wowl_pkt 102 00:90:4c:aa:bb:cc eapid id-string"},
	{"wowl_ext_magic", wl_wowl_extended_magic, WLC_GET_VAR, WLC_SET_VAR,
	"Set 6-byte extended magic pattern\n"
	"Usage: wl wowl_ext_magic 0x112233445566"},
	{ "wowl_wakeup_reason", wl_wowl_wake_reason, WLC_GET_VAR, -1 /* Set not reqd */,
	"Returns pattern id and associated wakeup reason"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_wowl_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register wowl commands */
	wl_module_cmds_register(wl_wowl_cmds);
}

/*
 * If a host IP address is given, add it to the host-cache,
 * e.g. "wl nd_hostip fe00:0:0:0:0:290:1fc0:18c0 ".
 * If no address is given, dump all the addresses.
 */
static int
wl_nshostip(void *wl, cmd_t *cmd, char **argv)
{

	int ret = 0, i;
	struct ipv6_addr ipa_set, *ipa_get, null_ipa;
	uint16 *ip_addr;
	if (!*++argv) {
		/* Get */
		void *ptr = NULL;

		if ((ret = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr)) < 0)
			return ret;
		ip_addr = (uint16*)ptr;
		memset(null_ipa.addr, 0, IPV6_ADDR_LEN);
		for (ipa_get = (struct ipv6_addr *)ptr;
			memcmp(null_ipa.addr, ipa_get->addr, IPV6_ADDR_LEN) != 0;
			ipa_get++) {
				/* Print ipv6 Addr */
				for (i = 0; i < 8; i++)
				{
					printf("%x", ntoh16(ip_addr[i]));
					if (i < 7)
						printf(":");
				}
			}
			printf("\r\n");
			ip_addr += 8;
		}
	else {
		/* Add */
		if (!wl_atoipv6(*argv, &ipa_set))
			return -1;
		/* we add one ip-addr at a time */
		return wlu_var_setbuf(wl, cmd->name,
		&ipa_set, IPV6_ADDR_LEN);
		}
	return ret;
}

static int
wl_wowl_status(void *wl, cmd_t *cmd, char **argv)
{
	int flags_prev = 0;
	int err;

	UNUSED_PARAMETER(cmd);

	argv++;

	if ((err = wlu_iovar_getint(wl, "wowl_status", &flags_prev)))
		return err;

	printf("Status of last wakeup:\n");
	printf("\tflags:0x%x\n", flags_prev);

	if (flags_prev & WL_WOWL_BCN)
		printf("\t\tWake-on-Loss-of-Beacons enabled\n");

	if (flags_prev & WL_WOWL_MAGIC)
		printf("\t\tWake-on-Magic frame enabled\n");
	if (flags_prev & WL_WOWL_NET)
		printf("\t\tWake-on-Net pattern enabled\n");
	if (flags_prev & WL_WOWL_DIS)
		printf("\t\tWake-on-Deauth enabled\n");

	if (flags_prev & WL_WOWL_RETR)
		printf("\t\tRetrograde TSF enabled\n");
	if (flags_prev & WL_WOWL_TST)
		printf("\t\tTest-mode enabled\n");

	printf("\n");

	return 0;
}

static int
wl_wowl_wakeind(void *wl, cmd_t *cmd, char **argv)
{
	wl_wowl_wakeind_t wake = {0, 0};
	int err;

	UNUSED_PARAMETER(cmd);

	argv++;

	if (*argv) {
		if (strcmp(*argv, "clear"))
			return BCME_USAGE_ERROR;
		memset(&wake, 0, sizeof(wake));
		memcpy(&wake, *argv, strlen("clear"));
		err = wlu_iovar_set(wl, "wowl_wakeind", &wake, sizeof(wl_wowl_wakeind_t));
		return err;
	}

	if ((err = wlu_iovar_get(wl, "wowl_wakeind", &wake, sizeof(wl_wowl_wakeind_t))) < 0)
		return err;

	if (wake.pci_wakeind)
		printf("PCI Indication set\n");
#ifdef DSLCPE_ENDIAN
	wake.ucode_wakeind = dtoh32(wake.ucode_wakeind);
#endif
	if (wake.ucode_wakeind != 0) {
		printf("MAC Indication set\n");

		if ((wake.ucode_wakeind & WL_WOWL_MAGIC) == WL_WOWL_MAGIC)
			printf("\tMAGIC packet received\n");
		if ((wake.ucode_wakeind & WL_WOWL_NET) == WL_WOWL_NET)
			printf("\tPacket received with Netpattern\n");
		if ((wake.ucode_wakeind & WL_WOWL_DIS) == WL_WOWL_DIS)
			printf("\tDisassociation/Deauth received\n");
		if ((wake.ucode_wakeind & WL_WOWL_RETR) == WL_WOWL_RETR)
			printf("\tRetrograde TSF detected\n");
		if ((wake.ucode_wakeind & WL_WOWL_BCN) == WL_WOWL_BCN)
			printf("\tBeacons Lost\n");
		if ((wake.ucode_wakeind & WL_WOWL_TST) == WL_WOWL_TST)
			printf("\tTest Mode\n");
		if ((wake.ucode_wakeind & WL_WOWL_M1) == WL_WOWL_M1)
			printf("\tPTK Refresh received.\n");
		if ((wake.ucode_wakeind & WL_WOWL_EAPID) == WL_WOWL_EAPID)
			printf("\tEAP-Identity request received\n");
		if ((wake.ucode_wakeind & WL_WOWL_GTK_FAILURE) == WL_WOWL_GTK_FAILURE)
			printf("\tWake on GTK failure.\n");
		if ((wake.ucode_wakeind & WL_WOWL_EXTMAGPAT) == WL_WOWL_EXTMAGPAT)
			printf("\tExtended Magic Packet received.\n");
		if ((wake.ucode_wakeind & WL_WOWL_KEYROT) == WL_WOWL_KEYROT)
			printf("\tKey Rotation Packet received.\n");
		if ((wake.ucode_wakeind & (WL_WOWL_NET | WL_WOWL_MAGIC | WL_WOWL_EXTMAGPAT))) {
			if ((wake.ucode_wakeind & WL_WOWL_BCAST) == WL_WOWL_BCAST)
				printf("\t\tBroadcast/Mcast frame received\n");
			else
				printf("\t\tUnicast frame received\n");
		}
	}

	if (!wake.pci_wakeind && wake.ucode_wakeind == 0)
		printf("No wakeup indication set\n");

	return 0;
}
/* Used by NINTENDO2 */
static int
wl_wowl_wake_reason(void *wl, cmd_t *cmd, char **argv)
{
	int err = -1;
	wl_wr_t wr;

	UNUSED_PARAMETER(cmd);

	if (!*++argv) {
		err = wlu_iovar_get(wl, "wakeup_reason", &wr, sizeof(wl_wr_t));
		if (err)
			return err;

		if (wr.reason && wr.reason < REASON_LAST) {
			printf("ID: %d\t", wr.id);

			if (wr.reason == LCD_ON)
			printf("Reason: LCD_ON\n");
			else if (wr.reason == LCD_OFF)
			printf("Reason: LCD_OFF\n");
			else if (wr.reason == DRC1_WAKE)
			printf("Reason: DRC1_WAKE\n");
			else if (wr.reason == DRC2_WAKE)
			printf("Reason: DRC2_WAKE\n");
		}
		else
			printf("Unknown wakeup Reason\n");
	}
		return err;
}

/* Send a wakeup frame to sta in WAKE mode */
static int
wl_wowl_pkt(void *wl, cmd_t *cmd, char **argv)
{
	char *arg = buf;
	const char *str;
	char *dst;
	uint tot = 0;
	uint16 type, pkt_len;
	int dst_ea = 0; /* 0 == manual, 1 == bcast, 2 == ucast */
	char *ea[ETHER_ADDR_LEN];
	if (!*++argv)
		return BCME_USAGE_ERROR;

	UNUSED_PARAMETER(cmd);

	str = "wowl_pkt";
	strncpy(arg, str, strlen(str));
	arg[strlen(str)] = '\0';
	dst = arg + strlen(str) + 1;
	tot += strlen(str) + 1;

	pkt_len = (uint16)htod32(strtoul(*argv, NULL, 0));

	*((uint16*)dst) = pkt_len;

	dst += sizeof(pkt_len);
	tot += sizeof(pkt_len);

	if (!*++argv) {
		printf("Dest of the packet needs to be provided\n");
		return BCME_USAGE_ERROR;
	}

	/* Dest of the frame */
	if (!strcmp(*argv, "bcast")) {
		dst_ea = 1;
		if (!wl_ether_atoe("ff:ff:ff:ff:ff:ff", (struct ether_addr *)dst))
			return BCME_USAGE_ERROR;
	} else if (!strcmp(*argv, "ucast")) {
		dst_ea = 2;
		if (!*++argv) {
			printf("EA of ucast dest of the packet needs to be provided\n");
			return BCME_USAGE_ERROR;
		}
		if (!wl_ether_atoe(*argv, (struct ether_addr *)dst))
			return BCME_USAGE_ERROR;
		/* Store it */
		memcpy(ea, dst, ETHER_ADDR_LEN);
	} else if (!wl_ether_atoe(*argv, (struct ether_addr *)dst))
		return BCME_USAGE_ERROR;

	dst += ETHER_ADDR_LEN;
	tot += ETHER_ADDR_LEN;

	if (!*++argv) {
		printf("type - magic/net needs to be provided\n");
		return BCME_USAGE_ERROR;
	}

	if (strncmp(*argv, "magic", strlen("magic")) == 0)
		type = WL_WOWL_MAGIC;
	else if (strncmp(*argv, "net", strlen("net")) == 0)
		type = WL_WOWL_NET;
	else if (strncmp(*argv, "eapid", strlen("eapid")) == 0)
		type = WL_WOWL_EAPID;
	else
		return BCME_USAGE_ERROR;

	*((uint16*)dst) = type;
	dst += sizeof(type);
	tot += sizeof(type);

	if (type == WL_WOWL_MAGIC) {
		if (pkt_len < MAGIC_PKT_MINLEN)
			return BCME_BADARG;

		if (dst_ea == 2)
			memcpy(dst, ea, ETHER_ADDR_LEN);
		else {
			if (!*++argv)
				return BCME_USAGE_ERROR;

			if (!wl_ether_atoe(*argv, (struct ether_addr *)dst))
				return BCME_USAGE_ERROR;
		}
		tot += ETHER_ADDR_LEN;
	} else if (type == WL_WOWL_NET) {
		wl_wowl_pattern_t *wl_pattern;
		wl_pattern = (wl_wowl_pattern_t *)dst;

		if (!*++argv) {
			printf("Starting offset not provided\n");
			return BCME_USAGE_ERROR;
		}

		wl_pattern->offset = (uint)htod32(strtoul(*argv, NULL, 0));

		wl_pattern->masksize = 0;

		wl_pattern->patternoffset = (uint)htod32(sizeof(wl_wowl_pattern_t));

		dst += sizeof(wl_wowl_pattern_t);

		if (!*++argv) {
			printf("pattern not provided\n");
			return BCME_USAGE_ERROR;
		}

		wl_pattern->patternsize =
		        (uint)htod32(wl_pattern_atoh((char *)(uintptr)*argv, dst));
		dst += wl_pattern->patternsize;
		tot += sizeof(wl_wowl_pattern_t) + wl_pattern->patternsize;

		wl_pattern->reasonsize = 0;
		if (*++argv) {
			wl_pattern->reasonsize =
				(uint)htod32(wl_pattern_atoh((char *)(uintptr)*argv, dst));
			tot += wl_pattern->reasonsize;
		}
	} else {	/* eapid */
		if (!*++argv) {
			printf("EAPOL identity string not provided\n");
			return BCME_USAGE_ERROR;
		}

		*dst++ = strlen(*argv);
		strncpy(dst, *argv, strlen(*argv));
		tot += 1 + strlen(*argv);
	}
	return (wlu_set(wl, WLC_SET_VAR, arg, tot));
}

static int
wl_wowl_pattern(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	uint i, j;
	uint8 *ptr;
	wl_wowl_pattern_t *wl_pattern;

	UNUSED_PARAMETER(cmd);

	if (*++argv) {
		char *arg = buf;
		const char *str;
		char *dst;
		uint tot = 0;

		if (strcmp(*argv, "add") != 0 && strcmp(*argv, "del") != 0 &&
		    strcmp(*argv, "clr") != 0) {
			return BCME_USAGE_ERROR;
		}

		str = "wowl_pattern";
		strncpy(arg, str, strlen(str));
		arg[strlen(str)] = '\0';
		dst = arg + strlen(str) + 1;
		tot += strlen(str) + 1;

		str = *argv;
		strncpy(dst, str, strlen(str));
		tot += strlen(str) + 1;

		if (strcmp(str, "clr") != 0) {
			wl_pattern = (wl_wowl_pattern_t *)(dst + strlen(str) + 1);
			dst = (char*)wl_pattern + sizeof(wl_wowl_pattern_t);
			if (!*++argv) {
				printf("Starting offset not provided\n");
				return BCME_USAGE_ERROR;
			}
			wl_pattern->offset = htod32(strtoul(*argv, NULL, 0));
			if (!*++argv) {
				printf("Mask not provided\n");
				return BCME_USAGE_ERROR;
			}

			/* Parse the mask */
			str = *argv;
			wl_pattern->masksize = htod32(wl_pattern_atoh((char *)(uintptr)str, dst));
			if (wl_pattern->masksize == (uint)-1)
				return BCME_USAGE_ERROR;

			dst += wl_pattern->masksize;
			wl_pattern->patternoffset = htod32((sizeof(wl_wowl_pattern_t) +
			                                    wl_pattern->masksize));

			if (!*++argv) {
				printf("Pattern value not provided\n");
				return BCME_USAGE_ERROR;
			}

			/* Parse the value */
			str = *argv;
			wl_pattern->patternsize =
			        htod32(wl_pattern_atoh((char *)(uintptr)str, dst));
			if (wl_pattern->patternsize == (uint)-1)
				return BCME_USAGE_ERROR;
			tot += sizeof(wl_wowl_pattern_t) + wl_pattern->patternsize +
			        wl_pattern->masksize;
		}

		return (wlu_set(wl, WLC_SET_VAR, arg, tot));
	} else {
		wl_wowl_pattern_list_t *list;
		if ((err = wlu_iovar_get(wl, "wowl_pattern", buf, WLC_IOCTL_MAXLEN)) < 0)
			return err;
		list = (wl_wowl_pattern_list_t *)buf;
		printf("#of patterns :%d\n", list->count);
		ptr = (uint8 *)list->pattern;
		for (i = 0; i < list->count; i++) {
			uint8 *pattern;

			wl_pattern = (wl_wowl_pattern_t *)ptr;
#ifdef DSLCPE_ENDIAN
			wl_pattern->id = dtoh32(wl_pattern->id);
			wl_pattern->offset = dtoh32(wl_pattern->offset);
			wl_pattern->masksize = dtoh32(wl_pattern->masksize);
			wl_pattern->patternoffset = dtoh32(wl_pattern->patternoffset);
			wl_pattern->patternsize = dtoh32(wl_pattern->patternsize);
#endif
			printf("Pattern %d:\n", i+1);
			printf("ID         :0x%x\n"
				"Offset     :%d\n"
				"Masksize   :%d\n"
				"Mask       :0x",
				(uint32)wl_pattern->id, wl_pattern->offset, wl_pattern->masksize);
			pattern = ((uint8 *)wl_pattern + sizeof(wl_wowl_pattern_t));
			for (j = 0; j < wl_pattern->masksize; j++)
				printf("%02x", pattern[j]);
			printf("\n"
			       "PatternSize:%d\n"
			       "Pattern    :0x", wl_pattern->patternsize);
			/* Go to end to find pattern */
			pattern = ((uint8*)wl_pattern + wl_pattern->patternoffset);
			for (j = 0; j < wl_pattern->patternsize; j++)
				printf("%02x", pattern[j]);
			printf("\n\n");
			ptr += (wl_pattern->masksize + wl_pattern->patternsize +
			        sizeof(wl_wowl_pattern_t));
		}
	}

	return err;
}

static int
wl_wowl_extended_magic(void *wl, cmd_t *cmd, char **argv)
{
	char *arg = buf;
	const char *str;
	char *dst;
	uint tot;
	int ret;

	str = "wowl_ext_magic";
	strncpy(arg, str, strlen(str));
	arg[strlen(str)] = '\0';

	if (*++argv) {
		dst = arg + strlen(str) + 1;
		tot = strlen(str) + 1;
		ret = wl_pattern_atoh(*argv, dst);
		if (ret == -1)
			return BCME_USAGE_ERROR;
		if (ret != 6) {
			printf("Extended magic pattern must be 6-byte length\n");
			return BCME_USAGE_ERROR;
		}
		tot += 6;

		ret = wlu_set(wl, cmd->set, arg, tot);
		return ret;
	}

	if ((ret = wlu_get(wl, cmd->get, arg, WLC_IOCTL_MAXLEN)) < 0)
		return ret;

	printf("0x");
	for (ret = 0; ret < 6; ret++)
		printf("%02x", (uint8)arg[ret]);
	printf("\n");

	return 0;

}
