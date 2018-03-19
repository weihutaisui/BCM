/*
 * wl pkt_filter command module
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
 * $Id: wluc_pkt_filter.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_pkt_filter_add;
static cmd_func_t wl_pkt_filter_enable;
static cmd_func_t wl_pkt_filter_list;
static cmd_func_t wl_pkt_filter_stats;
static cmd_func_t wl_pkt_filter_ports;

static cmd_t wl_pkt_filter_cmds[] = {
	{ "pkt_filter_add", wl_pkt_filter_add, -1, -1,
	"Install a packet filter.\n"
	"\tUsage: wl pkt_filter_add <id> <polarity> <type> <offset> <bitmask> <pattern>\n"
	"\tid:       Integer. User specified id.\n"
	"\ttype:     0 (Pattern matching filter)\n"
	"\t          1 (Magic pattern match (variable offset)\n"
	"\t          2 (Extended pattern list)\n"
	"\toffset:   (type 0): Integer offset in received packet to start matching.\n"
	"\t          (type 1): Integer offset, match here are anywhere later.\n"
	"\t          (type 2): [<base>:]<offset>. Symbolic packet loc plus relative\n"
	"\t                    offset, use wl_pkt_filter_add -l for a <base> list.\n"
	"\tpolarity: Set to 1 to negate match result. 0 is default.\n"
	"\tbitmask:  Hex bitmask that indicates which bits of 'pattern' to match.\n"
	"\t          Must be same size as 'pattern'. Bit 0 of bitmask corresponds\n"
	"\t          to bit 0 of pattern, etc.  If bit N of bitmask is 0, then do\n"
	"\t          *not* match bit N of the pattern with the received payload. If\n"
	"\t          bit N of bitmask is 1, then perform match.\n"
	"\tpattern:  Hex pattern to match.  Must be same size as <bitmask>.\n"
	"\t          Syntax: same as bitmask, but for type 2 (pattern list), a '!'\n"
	"\t          may be used to negate that pattern match (e.g. !0xff03).\n"
	"\tFor type 2: [<base>:]<offset> <bitmask> [!]<pattern> triple may be\n"
	"\trepeated; all sub-patterns must match for the filter to match."},
	{ "pkt_filter_clear_stats", wl_varint, -1, WLC_SET_VAR,
	"Clear packet filter statistic counter values.\n"
	"\tUsage: wl pkt_filter_clear_stats <id>" },
	{ "pkt_filter_enable", wl_pkt_filter_enable, -1, -1,
	"Enable/disable a packet filter.\n"
	"\tUsage: wl pkt_filter_enable <id> <0|1>"},
	{ "pkt_filter_list", wl_pkt_filter_list, -1, -1,
	"List installed packet filters.\n"
	"\tUsage: wl pkt_filter_list [val]\n"
	"\tval: 0 (disabled filters) 1 (enabled filters)"},
	{ "pkt_filter_mode", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"Set packet filter match action.\n"
	"\tUsage: wl pkt_filter_mode <value>\n"
	"\tvalue: 1 - Forward packet on match, discard on non-match (default).\n"
	"\t       0 - Discard packet on match, forward on non-match." },
	{ "pkt_filter_delete", wl_varint, -1, WLC_SET_VAR,
	"Uninstall a packet filter.\n"
	"\tUsage: wl pkt_filter_delete <id>"},
	{ "pkt_filter_stats", wl_pkt_filter_stats, -1, -1,
	"Retrieve packet filter statistic counter values.\n"
	"\tUsage: wl pkt_filter_stats <id>"},
	{ "pkt_filter_ports", wl_pkt_filter_ports, WLC_GET_VAR, WLC_SET_VAR,
	"Set up additional port filters for TCP and UDP packets.\n"
	"\tUsage: wl pkt_filter_ports [<port-number>] ...\n"
	"\t       wl pkt_filter_ports none (to clear/disable)"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_pkt_filter_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register pkt_filter commands */
	wl_module_cmds_register(wl_pkt_filter_cmds);
}

/* Packet filter section: extended filters have named offsets, add table here */
typedef struct {
	char *name;
	uint16 base;
} wl_pfbase_t;

static wl_pfbase_t basenames[] = { WL_PKT_FILTER_BASE_NAMES };

static void
wl_pkt_filter_base_list(void)
{
	uint i;

	printf("Names accepted for base offsets:\n");
	for (i = 0; i < ARRAYSIZE(basenames); i++)
	{
		printf("\t%s\n", basenames[i].name);
	}
}

static int
wl_pkt_filter_base_parse(char *name)
{
	uint i;
	char *bname, *uname;

	for (i = 0; i < ARRAYSIZE(basenames); i++) {
		bname = basenames[i].name;
		for (uname = name; *uname; bname++, uname++) {
			if (*bname != toupper(*uname))
				break;
		}
		if (!*uname && !*bname)
			break;
	}

	if (i < ARRAYSIZE(basenames)) {
		return basenames[i].base;
	} else {
		return -1;
	}
}

static char *
wl_pkt_filter_base_show(uint16 base)
{
	uint i;
	static char numeric[6];

	for (i = 0; i < ARRAYSIZE(basenames); i++) {
		if (basenames[i].base == base)
			return basenames[i].name;
	}

	sprintf(numeric, "%d", base);
	return numeric;
}

/* Enable/disable installed packet filter. */
static int
wl_pkt_filter_enable(void *wl, cmd_t *cmd, char **argv)
{
	wl_pkt_filter_enable_t	enable_parm;
	int							rc;

	if (*++argv == NULL) {
		printf("No args provided\n");
		return BCME_USAGE_ERROR;
	}

	/* Parse packet filter id. */
	enable_parm.id = htod32(strtoul(*argv, NULL, 0));

	if (*++argv == NULL) {
		printf("Enable/disable value not provided\n");
		return BCME_USAGE_ERROR;
	}

	/* Parse enable/disable value. */
	enable_parm.enable = htod32(strtoul(*argv, NULL, 0));

	/* Enable/disable the specified filter. */
	rc = wlu_var_setbuf(wl,
	                   cmd->name,
	                   &enable_parm,
	                   sizeof(wl_pkt_filter_enable_t));

	return (rc);
}

/* Install a new packet filter. */
static int
wl_pkt_filter_add(void *wl, cmd_t *cmd, char **argv)
{
	const char          *str;
	wl_pkt_filter_t     pkt_filter;
	wl_pkt_filter_t     *pkt_filterp;
	int                 buf_len;
	int                 str_len;
	int                 rc;
	uint32              mask_size;
	uint32              pattern_size;
	uint                ftype;
	char                *endptr;

	UNUSED_PARAMETER(cmd);

	str = "pkt_filter_add";
	str_len = strlen(str);
	strncpy(buf, str, str_len);
	buf[ str_len ] = '\0';
	buf_len = str_len + 1;

	pkt_filterp = (wl_pkt_filter_t *) (buf + str_len + 1);

	if (argv[1] && strcmp(argv[1], "-l") == 0) {
		wl_pkt_filter_base_list();
		return BCME_OK;
	}

	/* Parse packet filter id. */
	if (*++argv == NULL) {
		printf("No args provided\n");
		return BCME_USAGE_ERROR;
	}
	pkt_filter.id = htod32(strtoul(*argv, &endptr, 0));
	if (*endptr) {
		printf("Invalid number for id: %s\n", *argv);
		return BCME_USAGE_ERROR;
	}

	/* Parse filter polarity. */
	if (*++argv == NULL) {
		printf("Polarity not provided\n");
		return BCME_USAGE_ERROR;
	}
	pkt_filter.negate_match = htod32(strtoul(*argv, &endptr, 0));
	if (*endptr) {
		printf("Invalid number for polarity: %s\n", *argv);
		return BCME_USAGE_ERROR;
	}

	/* Parse filter type. */
	if (*++argv == NULL) {
		printf("Filter type not provided\n");
		return BCME_USAGE_ERROR;
	}
	pkt_filter.type = htod32(strtoul(*argv, &endptr, 0));
	ftype = htod32(strtoul(*argv, &endptr, 0));
	if (*endptr) {
		printf("Invalid number for filter type: %s\n", *argv);
		return BCME_USAGE_ERROR;
	}
	if ((ftype != 0) && (ftype != 2)) {
		printf("Invalid filter type %d\n", ftype);
		return BCME_USAGE_ERROR;
	}
	pkt_filter.type = htod32(ftype);

	/* Handle basic (or magic) pattern filter */
	if ((ftype == 0) || (ftype == 1)) {
		wl_pkt_filter_pattern_t *pfilter = &pkt_filterp->u.pattern;

		/* Parse pattern filter offset. */
		if (*++argv == NULL) {
			printf("Offset not provided\n");
			return BCME_USAGE_ERROR;
		}
		pkt_filter.u.pattern.offset = htod32(strtoul(*argv, &endptr, 0));
		if (*endptr) {
			printf("Invalid number for offset: %s\n", *argv);
			return BCME_USAGE_ERROR;
		}

		/* Parse pattern filter mask. */
		if (*++argv == NULL) {
			printf("Bitmask not provided\n");
			return BCME_USAGE_ERROR;
		}
		rc =   wl_pattern_atoh(*argv, (char *)pfilter->mask_and_pattern);
		if (rc == -1) {
			printf("Rejecting: %s\n", *argv);
			return BCME_USAGE_ERROR;
		}
		mask_size = htod32(rc);

		/* Parse pattern filter pattern. */
		if (*++argv == NULL) {
			printf("Pattern not provided\n");
			return BCME_USAGE_ERROR;
		}
		rc = wl_pattern_atoh(*argv, (char *)&pfilter->mask_and_pattern[rc]);
		if (rc == -1) {
			printf("Rejecting: %s\n", *argv);
			return BCME_USAGE_ERROR;
		}
		pattern_size = htod32(rc);

		if (mask_size != pattern_size) {
			printf("Mask and pattern not the same size\n");
			return BCME_USAGE_ERROR;
		}

		pkt_filter.u.pattern.size_bytes = mask_size;
		buf_len += WL_PKT_FILTER_FIXED_LEN;
		buf_len += (WL_PKT_FILTER_PATTERN_FIXED_LEN + 2 * rc);

		/* The fields that were put in a local for alignment purposes now
		 * get copied to the right place in the ioctl buffer.
		 */
		memcpy((char *)pkt_filterp, &pkt_filter,
		       WL_PKT_FILTER_FIXED_LEN + WL_PKT_FILTER_PATTERN_FIXED_LEN);
	}

	/* Handle pattern list */
	if (ftype == 2) {
		int list_cnt = 0;
		wl_pkt_filter_pattern_listel_t *pf_el = &pkt_filterp->u.patlist.patterns[0];

		while (*++argv != NULL) {
			/* Parse pattern filter base and offset. */
			if (isdigit(**argv)) {
				/* Numeric base */
				rc = strtoul(*argv, &endptr, 0);
			} else {
				endptr = strchr(*argv, ':');
				if (endptr) {
					*endptr = '\0';
					rc = wl_pkt_filter_base_parse(*argv);
					if (rc == -1) {
						printf("Invalid base %s\n", *argv);
						wl_pkt_filter_base_list();
						return BCME_USAGE_ERROR;
					}
					*endptr = ':';
				} else {
					printf("Invalid [base:]offset format: %s\n", *argv);
					return BCME_USAGE_ERROR;
				}
			}

			if (*endptr == ':') {
				pkt_filter.u.patlist.patterns[0].base_offs = htod16(rc);
				rc = strtoul(endptr + 1, &endptr, 0);
			} else {
				/* Must have had a numeric offset only */
				pkt_filter.u.patlist.patterns[0].base_offs = htod16(0);
			}

		        if (*endptr) {
				printf("Invalid [base:]offset format: %s\n", *argv);
				return BCME_USAGE_ERROR;
			}
			if (rc > 0x0000FFFF) {
				printf("Offset too large\n");
				return BCME_USAGE_ERROR;
			}
			pkt_filter.u.patlist.patterns[0].rel_offs = htod16(rc);

			/* Clear match_flag (may be set in parsing which follows) */
			pkt_filter.u.patlist.patterns[0].match_flags = htod16(0);

			/* Parse pattern filter mask and pattern directly into ioctl buffer */
			if (*++argv == NULL) {
				printf("Bitmask not provided\n");
				return BCME_USAGE_ERROR;
			}
			rc = wl_pattern_atoh(*argv, (char*)pf_el->mask_and_data);
			if (rc == -1) {
				printf("Rejecting: %s\n", *argv);
				return BCME_USAGE_ERROR;
			}
			mask_size = htod16(rc);

			if (*++argv == NULL) {
				printf("Pattern not provided\n");
				return BCME_USAGE_ERROR;
			}

			if (**argv == '!') {
				pkt_filter.u.patlist.patterns[0].match_flags =
				        htod16(WL_PKT_FILTER_MFLAG_NEG);
				(*argv)++;
			}
			if (*argv == '\0') {
				printf("Pattern not provided\n");
				return BCME_USAGE_ERROR;
			}
			rc = wl_pattern_atoh(*argv, (char*)&pf_el->mask_and_data[rc]);
			if (rc == -1) {
				printf("Rejecting: %s\n", *argv);
				return BCME_USAGE_ERROR;
			}
			pattern_size = htod16(rc);

			if (mask_size != pattern_size) {
				printf("Mask and pattern not the same size\n");
				return BCME_USAGE_ERROR;
			}

			pkt_filter.u.patlist.patterns[0].size_bytes = mask_size;

			/* Account for the size of this pattern element */
			buf_len += WL_PKT_FILTER_PATTERN_LISTEL_FIXED_LEN + 2 * rc;

			/* And the pattern element fields that were put in a local for
			 * alignment purposes now get copied to the ioctl buffer.
			 */
			memcpy((char*)pf_el, &pkt_filter.u.patlist.patterns[0],
			       WL_PKT_FILTER_PATTERN_FIXED_LEN);

			/* Move to next element location in ioctl buffer */
			pf_el = (wl_pkt_filter_pattern_listel_t*)
			        ((uint8*)pf_el + WL_PKT_FILTER_PATTERN_LISTEL_FIXED_LEN + 2 * rc);

			/* Count list element */
			list_cnt++;
		}

		/* Account for initial fixed size, and copy initial fixed fields */
		buf_len += WL_PKT_FILTER_FIXED_LEN + WL_PKT_FILTER_PATTERN_LIST_FIXED_LEN;

		/* Update list count and total size */
#ifdef DSLCPE_ENDIAN
		pkt_filter.u.patlist.list_cnt = htod32(list_cnt);
#else
		pkt_filter.u.patlist.list_cnt = list_cnt;
#endif
		pkt_filter.u.patlist.PAD1[0] = 0;
		pkt_filter.u.patlist.totsize = buf + buf_len - (char*)pkt_filterp;
		pkt_filter.u.patlist.totsize -= WL_PKT_FILTER_FIXED_LEN;

#ifdef DSLCPE_ENDIAN
		pkt_filter.u.patlist.totsize = htod32(pkt_filter.u.patlist.totsize);
#endif
		memcpy((char *)pkt_filterp, &pkt_filter,
		       WL_PKT_FILTER_FIXED_LEN + WL_PKT_FILTER_PATTERN_LIST_FIXED_LEN);
	}

	rc = wlu_set(wl, WLC_SET_VAR, buf, buf_len);

	return (rc);
}

/* List installed packet filters. */
static void
wl_pkt_filter_list_mask_pat(uint8 *bytes, uint size, char *indent)
{
	uint j;

	printf("%sMask        :0x", indent);

	for (j = 0; j < size; j++)
		printf("%02x", bytes[j]);

	printf("\n%sPattern     :0x", indent);

	for (; j < 2 * size; j++)
		printf("%02x", bytes[j]);

	printf("\n\n");
}

/* List installed packet filters. */
static int
wl_pkt_filter_list(void *wl, cmd_t *cmd, char **argv)
{
	wl_pkt_filter_list_t	*list;
	wl_pkt_filter_t		*filterp;
	void						*ptr = NULL;
	unsigned int			i;
	unsigned int			j;
	int 						rc;
	unsigned int			filter_len;
	uint32					enable;

	if (*++argv == NULL) {
		printf("No args provided\n");
		return (BCME_USAGE_ERROR);
	}

	/* Parse filter list to retrieve (enabled/disabled). */
	enable = htod32(strtoul(*argv, NULL, 0));
	/*
	** Get list of installed packet filters.
	*/
	if ((rc = wlu_var_getbuf(wl, cmd->name, &enable, sizeof(enable), &ptr)) < 0)
		return rc;

	list = (wl_pkt_filter_list_t *) ptr;

#ifdef DSLCPE_ENDIAN
	printf("Num filters: %d\n\n", dtoh32(list->num));
#else
	printf("Num filters: %d\n\n", list->num);
#endif
	filterp = list->filter;
#ifdef DSLCPE_ENDIAN
	for (i = 0; i < dtoh32(list->num); i++)
#else
	for (i = 0; i < list->num; i++)
#endif
	{
		uint type = dtoh32(filterp->type);

		if (type != 2) {
			printf("Id          :%d\n"
			       "Negate      :%d\n"
			       "Type        :%d\n"
			       "Offset      :%d\n"
			       "Pattern len :%d\n",
			       dtoh32(filterp->id),
			       dtoh32(filterp->negate_match),
			       dtoh32(filterp->type),
			       dtoh32(filterp->u.pattern.offset),
			       dtoh32(filterp->u.pattern.size_bytes));

			wl_pkt_filter_list_mask_pat(filterp->u.pattern.mask_and_pattern,
			                            dtoh32(filterp->u.pattern.size_bytes), "");
			filter_len = WL_PKT_FILTER_PATTERN_FIXED_LEN +
			        2 * dtoh32(filterp->u.pattern.size_bytes);
		} else {
			char *indent = "    ";
			uint cnt = filterp->u.patlist.list_cnt;
			wl_pkt_filter_pattern_listel_t *listel = filterp->u.patlist.patterns;

			printf("Id          :%d\n"
			       "Negate      :%d\n"
			       "Type        :%d\n"
			       "List count  :%d\n",
			       dtoh32(filterp->id),
			       dtoh32(filterp->negate_match),
			       dtoh32(filterp->type),
			       cnt);

			for (j = 1; j <= cnt; j++) {
				printf("%sPattern %d\n", indent, j);
				printf("%sOffset      :%s:%d\n"
				       "%sMatch flags :%04x\n"
				       "%sPattern len :%d\n",
				       indent, wl_pkt_filter_base_show(dtoh16(listel->base_offs)),
				       dtoh16(listel->rel_offs),
				       indent, dtoh16(listel->match_flags),
				       indent, dtoh16(listel->size_bytes));
				wl_pkt_filter_list_mask_pat(listel->mask_and_data,
				                            dtoh16(listel->size_bytes), indent);
				listel = (wl_pkt_filter_pattern_listel_t*)
				        ((uintptr)listel +
				         WL_PKT_FILTER_PATTERN_LISTEL_FIXED_LEN +
				         2 * dtoh16(listel->size_bytes));
			}

			filter_len = (uint8*)listel - (uint8*)&filterp->u.patlist;
		}

		filter_len += WL_PKT_FILTER_FIXED_LEN;
		filterp = (wl_pkt_filter_t *) ((uint8 *)filterp + filter_len);
		filterp = ALIGN_ADDR(filterp, sizeof(uint32));

	}

	return (rc);
}

/* Get packet filter debug statistics. */
static int
wl_pkt_filter_stats(void *wl, cmd_t *cmd, char **argv)
{
	wl_pkt_filter_stats_t	*stats;
	uint32						id;
	int 							rc;
	void							*ptr = NULL;

	if (*++argv == NULL) {
		printf("No args provided\n");
		return BCME_USAGE_ERROR;
	}

	/* Parse filter id to retrieve. */
	id = htod32(strtoul(*argv, NULL, 0));

	/* Get debug stats. */
	if ((rc = wlu_var_getbuf(wl, cmd->name, &id, sizeof(id), &ptr)) < 0)
		return rc;

	stats = (wl_pkt_filter_stats_t *) ptr;

	printf("Packets matched for filter '%d': %d\n"
			 "Total packets discarded        : %d\n"
			 "Total packet forwarded         : %d\n",
#ifdef DSLCPE_ENDIAN
	                 dtoh32(id),
#else
			 id,
#endif
			 dtoh32(stats->num_pkts_matched),
			 dtoh32(stats->num_pkts_discarded),
			 dtoh32(stats->num_pkts_forwarded));

	return (rc);
}

/* Get/set packet filter port list */
static int
wl_pkt_filter_ports(void *wl, cmd_t *cmd, char **argv)
{
	int rc, i;
	void *ptr;
	uint16 count;
	uint16 *ports;
	wl_pkt_filter_ports_t *portlist;

	uint len;
	char *endptr = NULL;
	unsigned long portnum;

	if ((strlen("pkt_filter_ports") + 1 +
	     WL_PKT_FILTER_PORTS_FIXED_LEN +
	     sizeof(uint16) * WL_PKT_FILTER_PORTS_MAX) > WLC_IOCTL_MEDLEN) {
		fprintf(stderr, "Ioctl sizing error.\n");
		return -1;
	}

	if (*++argv == NULL) {
		/* Get iovar */
		if ((rc = wlu_var_getbuf_med(wl, cmd->name, NULL, 0, &ptr)))
			return rc;

		portlist = (wl_pkt_filter_ports_t*)ptr;
		count = dtoh16(portlist->count);
		ports = &portlist->ports[0];

		/* Bail if anything in the headeer is unexpected */
		if (portlist->version != WL_PKT_FILTER_PORTS_VERSION) {
			printf("Unsupported version %d, only support %d\n",
			       portlist->version, WL_PKT_FILTER_PORTS_VERSION);
			return BCME_USAGE_ERROR;
		}
		if (portlist->reserved != 0) {
			printf("Format error: nonzero reserved element 0x%02x\n",
			       portlist->reserved);
			return BCME_USAGE_ERROR;
		}
		if (count > WL_PKT_FILTER_PORTS_MAX) {
			printf("Invalid count %d\n", count);
			return BCME_USAGE_ERROR;
		}

		printf("Port count %d:\n", count);
		for (i = 0; i < count; i++) {
			printf("%d\n", dtoh16(ports[i]));
		}

		return 0;

	} else {
		/* Set iovar - build the structure in the global buffer */
		portlist = (wl_pkt_filter_ports_t*)buf;
		portlist->version = WL_PKT_FILTER_PORTS_VERSION;
		portlist->reserved = 0;

		ports = &portlist->ports[0];

		for (count = 0; *argv && (count < WL_PKT_FILTER_PORTS_MAX); count++, argv++) {
			portnum = strtoul(*argv, &endptr, 0);
			if ((*endptr != '\0') || (portnum > 0xffff)) {
				if (!strcmp(*argv, "none")) {
					argv += 1;
					break;
				} else {
					printf("Bad port number %s\n", *argv);
					return BCME_USAGE_ERROR;
				}
			}
			ports[count] = htod16((uint16)portnum);
		}

		if (*argv) {
			printf("Too many port numbers!\n");
			return BCME_USAGE_ERROR;
		}

		portlist->count = htod16(count);

		len = WL_PKT_FILTER_PORTS_FIXED_LEN + (count * sizeof(uint16));
		memmove((buf + strlen("pkt_filter_ports") + 1), buf, len);
		strcpy(buf, "pkt_filter_ports");
		len += strlen("pkt_filter_ports") + 1;

		return wlu_set(wl, WLC_SET_VAR, buf, len);
	}
}
