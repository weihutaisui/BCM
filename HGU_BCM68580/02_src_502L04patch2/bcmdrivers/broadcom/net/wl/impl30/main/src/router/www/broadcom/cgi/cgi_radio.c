 /*
 * Broadcom Home Gateway Reference Design
 * Web Page Configuration Support Routines
 *
 * Copyright (C) 2017, Broadcom. All Rights Reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 * $Id$
 */

#ifdef WEBS
#include <webs.h>
#include <uemf.h>
#include <ej.h>
#else /* !WEBS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <httpd.h>
#endif /* WEBS */
#ifdef DSLCPE
#include <wlcsm_linux.h>
#include <wlcsm_lib_api.h>
#endif

#include <typedefs.h>
#include <proto/ethernet.h>
#include <bcmparams.h>
#include <bcmconfig.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <bcmdevs.h>
#include <shutils.h>
#include <wlif_utils.h>
#include <netconf.h>
#include <nvparse.h>
#include <wlutils.h>
#include <bcmcvar.h>
#include <ezc.h>
#include <opencrypto.h>
#include <time.h>
#include <epivers.h>
#include "router_version.h"
#include <proto/802.11.h>
#include <proto/802.1d.h>
#include <security_ipc.h>

#include <cgi_common.h>

/* From wlc_rate.[ch] */
#define MCS_TABLE_SIZE 33

struct mcs_table_info {
	uint phy_rate_20;
	uint phy_rate_40;
};
/* rates are in units of Kbps */
static const struct mcs_table_info mcs_rate_table[MCS_TABLE_SIZE] = {
	{6500,   13500},	/* MCS  0 */
	{13000,  27000},	/* MCS  1 */
	{19500,  40500},	/* MCS  2 */
	{26000,  54000},	/* MCS  3 */
	{39000,  81000},	/* MCS  4 */
	{52000,  108000},	/* MCS  5 */
	{58500,  121500},	/* MCS  6 */
	{65000,  135000},	/* MCS  7 */
	{13000,  27000},	/* MCS  8 */
	{26000,  54000},	/* MCS  9 */
	{39000,  81000},	/* MCS 10 */
	{52000,  108000},	/* MCS 11 */
	{78000,  162000},	/* MCS 12 */
	{104000, 216000},	/* MCS 13 */
	{117000, 243000},	/* MCS 14 */
	{130000, 270000},	/* MCS 15 */
	{19500,  40500},	/* MCS 16 */
	{39000,  81000},	/* MCS 17 */
	{58500,  121500},	/* MCS 18 */
	{78000,  162000},	/* MCS 19 */
	{117000, 243000},	/* MCS 20 */
	{156000, 324000},	/* MCS 21 */
	{175500, 364500},	/* MCS 22 */
	{195000, 405000},	/* MCS 23 */
	{26000,  54000},	/* MCS 24 */
	{52000,  108000},	/* MCS 25 */
	{78000,  162000},	/* MCS 26 */
	{104000, 216000},	/* MCS 27 */
	{156000, 324000},	/* MCS 28 */
	{208000, 432000},	/* MCS 29 */
	{234000, 486000},	/* MCS 30 */
	{260000, 540000},	/* MCS 31 */
	{0,      6000},		/* MCS 32 */
};

#define MCS_TABLE_RATE(mcs, _is40) ((_is40)? mcs_rate_table[(mcs)].phy_rate_40: \
	mcs_rate_table[(mcs)].phy_rate_20)

static unsigned int bits_count(unsigned int n)
{
	unsigned int count = 0;

	while (n > 0) {
		if (n & 1)
			count++;
		n >>= 1;
	}

	return count;
}

static int
wl_print_country_rev_list(webs_t wp, char *name, char *abbrev)
{
	char *buf = (char *)malloc(WLC_IOCTL_MAXLEN);
	int buflen;
	wl_uint32_list_t *list;
	int ret = 0, i = 0, c = 0;

	if (buf == NULL)
		return -1;

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif 
	
	strncpy_n(buf, "country_rev", WLC_IOCTL_MAXLEN);
	buflen = strlen(buf) + 1;

	strncpy(buf + buflen, abbrev, WLC_CNTRY_BUF_SZ);
	buflen += WLC_CNTRY_BUF_SZ;

	/* Add list */
	list = (wl_uint32_list_t *)(buf + buflen);
#ifdef DSLCPE_ENDIAN
	list->count = htoe32(WLC_IOCTL_SMLEN);
#else
	list->count = WLC_IOCTL_SMLEN;
#endif
	buflen += sizeof(uint32)*(WLC_IOCTL_SMLEN + 1);

	ret = wl_ioctl(name, WLC_GET_VAR, buf, buflen);
	if (ret < 0)
		goto exit;

	list = (wl_uint32_list_t *)buf;

#ifdef DSLCPE_ENDIAN
	list->count = etoh32(list->count);
	for (i = 0; i < list->count; ++i) {
		list->element[i]=etoh32(list->element[i]);
	}
#endif
	if (list->count == 0) {
		ret = 0;
		goto exit;
	}

	websWrite(wp, "\t\tif (country == \"%s\")\n\t\t\tcountry_revs = new Array(",
	          abbrev);
	for (i = 0; i < list->count; i++) {
		c = list->element[i];
		websWrite(wp, "\"%d\"", c);
		if (i != (list->count - 1))
			websWrite(wp, ", ");
	}
	websWrite(wp,");\n");

exit:
	free((void *)buf);
	return ret;
}

/*
 * Country names and abbreviations from ISO 3166
 */
typedef struct {
	char *name;     /* Long name */
	char *abbrev;   /* Abbreviation */
} country_name_t;
country_name_t country_names[];     /* At end of this file */

/* Return whether the current phytype is NPHY */
static int
ej_wl_nphy_set(int eid, webs_t wp, int argc, char_t **argv)
{
	int phytype, status;

	/* Get configured phy type */
	if ((status = wl_phytype_get(wp, &phytype)) != 0)
		return status;

	return websWrite(wp, "%d", ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
	                            (phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT) ||
	                            (phytype == WLC_PHY_TYPE_AC)));
}
REG_EJ_HANDLER(wl_nphy_set);

/* Return a radio ID given a phy type */
static int
ej_wl_radioid(int eid, webs_t wp, int argc, char_t **argv)
{
	char *phytype=NULL, var[NVRAM_BUFSIZE], *next;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	int which;

	if (ejArgs(argc, argv, "%s", &phytype) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	assert(phytype);

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL))
		return websWrite(wp, "None");

	which = strcspn(nvram_safe_get(strcat_r(prefix, "phytypes", tmp)), phytype);
	foreach(var, nvram_safe_get(strcat_r(prefix, "radioids", tmp)), next) {
		if (which == 0)
			return websWrite(wp, var);
		which--;
	}

	return websWrite(wp, "None");
}
REG_EJ_HANDLER(wl_radioid);

static int
ej_wl_country_list(int eid, webs_t wp, int argc, char_t **argv)
{
	int i =0, status = 0;
	char *name =NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char *phytype = NULL;
	wl_country_list_t *cl = (wl_country_list_t *)malloc(WLC_IOCTL_MAXLEN);
	country_name_t *cntry=NULL;
	char *abbrev=NULL;
	int band = WLC_BAND_ALL, cur_phytype;

	if (!cl) {
		status = -1;
		goto exit;
	}

	if (ejArgs(argc, argv, "%s %d", &phytype, &band) < 1) {
		websError(wp, 400, "Insufficient args\n");
		status = -1;
		goto exit;
	}

	assert(phytype);
	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		status = -1;
		goto exit;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif 
	/* Get configured phy type */
	wl_ioctl(name, WLC_GET_PHYTYPE, &cur_phytype, sizeof(cur_phytype));
#ifdef DSLCPE_ENDIAN
	cur_phytype=etoh32(cur_phytype);
#endif 
	cl->buflen = WLC_IOCTL_MAXLEN;
	cl->band_set = TRUE;

	if (!strcmp(phytype, "a") &&
	    (cur_phytype != WLC_PHY_TYPE_N) && (cur_phytype != WLC_PHY_TYPE_SSN) &&
	    (cur_phytype != WLC_PHY_TYPE_LCN) && (cur_phytype != WLC_PHY_TYPE_HT) &&
	    (cur_phytype != WLC_PHY_TYPE_AC)) {
		cl->band = WLC_BAND_5G;
	} else if (((!strcmp(phytype, "b")) || (!strcmp(phytype, "g"))) &&
	           (cur_phytype != WLC_PHY_TYPE_N) && (cur_phytype != WLC_PHY_TYPE_SSN) &&
	           (cur_phytype != WLC_PHY_TYPE_LCN) && (cur_phytype != WLC_PHY_TYPE_HT) &&
	           (cur_phytype != WLC_PHY_TYPE_AC)) {
		cl->band = WLC_BAND_2G;
	} else if ((!strcmp(phytype, "n") || !strcmp(phytype, "s") || !strcmp(phytype, "h") || !strcmp(phytype, "v")) &&
	           ((cur_phytype == WLC_PHY_TYPE_N) || (cur_phytype == WLC_PHY_TYPE_SSN) ||
	            (cur_phytype == WLC_PHY_TYPE_LCN) || (cur_phytype == WLC_PHY_TYPE_HT) ||
	            (cur_phytype == WLC_PHY_TYPE_AC))) {
		/* Need to have additional argument of the band */
		if (argc < 2 || (band != WLC_BAND_2G && band != WLC_BAND_5G)) {
			status = -1;
			goto exit;
		}
		cl->band = band;
	} else if ((!strcmp(phytype, "l")) && (cur_phytype == WLC_PHY_TYPE_LP)) {
		wl_ioctl(name, WLC_GET_BAND, &band, sizeof(band));
#ifdef DSLCPE_ENDIAN
		band=etoh32(band);
#endif 
		cl->band = band;
	} else {
		status = -1;
		goto exit;
	}

#ifdef DSLCPE_ENDIAN
	cl->buflen= htoe32(cl->buflen);
	cl->band_set= htoe32(cl->band_set);
	cl->band= htoe32(cl->band);
	cl->count= htoe32(cl->count);
	if (wl_ioctl(name, WLC_GET_COUNTRY_LIST, cl, WLC_IOCTL_MAXLEN ) == 0) {
#else
	if (wl_ioctl(name, WLC_GET_COUNTRY_LIST, cl, cl->buflen) == 0) {
#endif
		websWrite(wp, "\t\tvar countries = new Array(");
		
#ifdef DSLCPE_ENDIAN
		cl->buflen= etoh32(cl->buflen);
		cl->band_set= etoh32(cl->band_set);
		cl->band= etoh32(cl->band);
		cl->count= etoh32(cl->count);
#endif
		for(i = 0; i < cl->count; i++) {
			abbrev = &cl->country_abbrev[i*WLC_CNTRY_BUF_SZ];
			websWrite(wp, "\"%s\"", abbrev);
			if (i != (cl->count - 1))
				websWrite(wp, ", ");
		}
		websWrite(wp, ");\n");
		for(i = 0; i < cl->count; i++) {
			abbrev = &cl->country_abbrev[i*WLC_CNTRY_BUF_SZ];
			for(cntry = country_names;
				cntry->name && strcmp(abbrev, cntry->abbrev);
				cntry++);
			websWrite(wp, "\t\tdocument.forms[0].wl_country_code[%d] = new Option(\"%s\", \"%s\");\n",
				i, cntry->name ? cntry->name : abbrev, abbrev);
		}
	}

exit:
	if (cl)
		free((void *)cl);
	return status;
}
REG_EJ_HANDLER(wl_country_list);

static int
wl_print_chanspec_list(webs_t wp, char *name, char *abbrev, int band, int bw)
{
	chanspec_t c = 0, *chanspec;
	char *buf = (char *)malloc(WLC_IOCTL_MAXLEN);
	int buflen;
	wl_uint32_list_t *list;
	int ret = 0, i = 0;
	uint16 bandwidth = 0;

	if (buf == NULL) {
		ret = -1;
		goto exit;
	}

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
	memset(buf,0,WLC_IOCTL_MAXLEN);
#endif

	strncpy_n(buf, "chanspecs", WLC_IOCTL_MAXLEN);
	buflen = strlen(buf) + 1;

	if (band == WLC_BAND_5G)
		c |= WL_CHANSPEC_BAND_5G;
	else
		c |= WL_CHANSPEC_BAND_2G;

	if (bw == 160) {
		c |= WL_CHANSPEC_BW_160;
		bandwidth = WL_CHANSPEC_BW_160;
	} else if (bw == 80) {
		c |= WL_CHANSPEC_BW_80;
		bandwidth = WL_CHANSPEC_BW_80;
	} else if (bw == 40) {
		c |= WL_CHANSPEC_BW_40;
		bandwidth = WL_CHANSPEC_BW_40;
	} else {
		c |= WL_CHANSPEC_BW_20;
		bandwidth = WL_CHANSPEC_BW_20;
	}

	chanspec = (chanspec_t *)(buf + buflen);
#ifdef DSLCPE_ENDIAN
	*chanspec = htoe(c);
#else
	*chanspec = c;
#endif
	buflen += (sizeof(chanspec_t));
	strncpy(buf + buflen, abbrev, WLC_CNTRY_BUF_SZ);
	buflen += WLC_CNTRY_BUF_SZ;

	/* Add list */
	list = (wl_uint32_list_t *)(buf + buflen);
#ifdef DSLCPE_ENDIAN
	list->count = htoe32(WL_NUMCHANSPECS);
#else
	list->count = WL_NUMCHANSPECS;
#endif
	buflen += sizeof(uint32)*(WL_NUMCHANSPECS + 1);

	ret = wl_ioctl(name, WLC_GET_VAR, buf, buflen);
	if (ret < 0) {
		goto exit;
	}

	list = (wl_uint32_list_t *)buf;
#ifdef DSLCPE_ENDIAN
	list->count=etoh32(list->count);
	
	for (i = 0; i < list->count; ++i) {
		list->element[i]=etoh32(list->element[i]);
	}
#endif
	if (list->count > 0) {
		websWrite(wp, "\t\tif (country == \"%s\")\n\t\t\tchanspecs = new Array(\"0\"", abbrev);
		for (i = 0; i < list->count; i++) {
			char chanbuf[CHANSPEC_STR_LEN];

			c = (chanspec_t) list->element[i];

			if (bandwidth && CHSPEC_BW(c) != bandwidth)
				continue;

			websWrite(wp, ", \"%s\"", wf_chspec_ntoa(c, chanbuf));
		}
		websWrite(wp, ");\n");
	} else {
		websWrite(wp, "\t\tif (country == \"%s\")\n\t\t\tchanspecs = new Array(\"N/A\");\n", abbrev);
	}

exit:
	if (buf)
		free(buf);

	return ret;
}
static int
ej_wl_chanspec_list(int eid, webs_t wp, int argc, char_t **argv)
{
	int  i, status = 0;
	char *name=NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char *phytype = NULL;
	int cur_phytype;
	wl_country_list_t *cl = (wl_country_list_t *)malloc(WLC_IOCTL_MAXLEN);
	char *abbrev=NULL;
	int band = WLC_BAND_ALL;
	int bw = 0;

	if (!cl) {
		status = -1;
		goto exit;
	}

	if (ejArgs(argc, argv, "%s %d %d", &phytype, &band, &bw) < 1) {
		websError(wp, 400, "Insufficient args\n");
		status = -1;
		goto exit;
	}

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		status = -1;
		goto exit;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif 
	/* Get configured phy type */
	wl_ioctl(name, WLC_GET_PHYTYPE, &cur_phytype, sizeof(cur_phytype));
#ifdef DSLCPE_ENDIAN
	cur_phytype=etoh32(cur_phytype);
#endif 
	cl->buflen = WLC_IOCTL_MAXLEN;
	cl->band_set = TRUE;

	if (!strcmp(phytype, "a") &&
	    (cur_phytype != WLC_PHY_TYPE_N) && (cur_phytype != WLC_PHY_TYPE_SSN) &&
	    (cur_phytype != WLC_PHY_TYPE_LCN) && (cur_phytype != WLC_PHY_TYPE_HT) &&
	    (cur_phytype != WLC_PHY_TYPE_AC)) {
		cl->band = WLC_BAND_5G;
	} else if (((!strcmp(phytype, "b")) || (!strcmp(phytype, "g"))) &&
	           (cur_phytype != WLC_PHY_TYPE_N) && (cur_phytype != WLC_PHY_TYPE_SSN) &&
	           (cur_phytype != WLC_PHY_TYPE_LCN) && (cur_phytype != WLC_PHY_TYPE_HT) &&
	           (cur_phytype != WLC_PHY_TYPE_AC)) {
		cl->band = WLC_BAND_2G;
	} else if ((!strcmp(phytype, "l")) && (cur_phytype == WLC_PHY_TYPE_LP)) {
		wl_ioctl(name, WLC_GET_BAND, &band, sizeof(band));
#ifdef DSLCPE_ENDIAN
		band=etoh32(band);
#endif 
		cl->band = band;
	} else if ((!strcmp(phytype, "n") || !strcmp(phytype, "s") || !strcmp(phytype, "h") || !strcmp(phytype, "v")) &&
	           ((cur_phytype == WLC_PHY_TYPE_N) || (cur_phytype == WLC_PHY_TYPE_SSN) ||
	            (cur_phytype == WLC_PHY_TYPE_LCN) || (cur_phytype == WLC_PHY_TYPE_HT) ||
	            (cur_phytype == WLC_PHY_TYPE_AC))) {
		/* Need to have additional argument of the band */
		if (argc < 2 || (band != WLC_BAND_2G && band != WLC_BAND_5G)) {
			status = -1;
			goto exit;
		}
		cl->band = band;
	} else {
		status = -1;
		goto exit;
	}

	band = cl->band;

#ifdef DSLCPE_ENDIAN
	cl->buflen= htoe32(cl->buflen);
	cl->band_set= htoe32(cl->band_set);
	cl->band= htoe32(cl->band);
	cl->count= htoe32(cl->count);
	if (wl_ioctl(name, WLC_GET_COUNTRY_LIST, cl, WLC_IOCTL_MAXLEN ) == 0) {
		cl->buflen= etoh32(cl->buflen);
		cl->band_set= etoh32(cl->band_set);
		cl->band= etoh32(cl->band);
		cl->count= etoh32(cl->count);
#else
	if (wl_ioctl(name, WLC_GET_COUNTRY_LIST, cl, cl->buflen) == 0) {
#endif
		for (i = 0; i < cl->count; i++) {
			abbrev = &cl->country_abbrev[i*WLC_CNTRY_BUF_SZ];
			wl_print_chanspec_list(wp, name, abbrev, band, bw);
		}
	}

exit:
	if (cl)
		free(cl);

	return status;
}
REG_EJ_HANDLER(wl_chanspec_list);

/* Return a list of the supported bands on the currently selected wireless interface */
static int
ej_wl_nphyrates(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	int phytype;
	int ret = 0, i;
	/* -2 is for Legacy rate
	 * -1 is placeholder for 'Auto'
	 */
	int mcsidxs[]= { -1, -2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 32};
	int selected_idx, selected_bw, nbw, rate;

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	/* Get configured phy type */
	if ((ret = wl_phytype_get(wp, &phytype)) != 0)
		return ret;

	if ((phytype != WLC_PHY_TYPE_N) && (phytype != WLC_PHY_TYPE_SSN) &&
	    (phytype != WLC_PHY_TYPE_LCN) && (phytype != WLC_PHY_TYPE_HT) &&
	    (phytype != WLC_PHY_TYPE_AC))
		return ret;

	if (ejArgs(argc, argv, "%d", &nbw) < 1) {
		websError(wp, 400, "BW does not exist \n");
		return -1;
	}
	/* nbw: 0 - Good Neighbor, 20 - 20MHz, 40 - 40MHz */

	selected_idx = atoi(nvram_safe_get(strcat_r(prefix, "nmcsidx", tmp)));
	selected_bw = atoi(nvram_safe_get(strcat_r(prefix, "nbw", tmp)));
	rate = atoi(nvram_safe_get(strcat_r(prefix, "rate", tmp)));

	/* Zero out the length of options */
	ret += websWrite(wp, "\t\tdocument.forms[0].wl_nmcsidx.length = 0; \n");

	ret += websWrite(wp, "\t\tdocument.forms[0].wl_nmcsidx[0] = "
	                 "new Option(\"Auto\", \"-1\");\n");

	if (selected_idx == -1 || rate == 0)
		ret += websWrite(wp, "\t\tdocument.forms[0].wl_nmcsidx.selectedIndex = 0;\n");

	for (i = 1; i < ARRAYSIZE(mcsidxs); i++) {
		/* Limit MCS rates based on number of user selected TxChains
		 * This block of code adds an "if" statement into the NPHY Rates List
		 * which surrounds the MCS indexes greater than 7
		 */
		if (mcsidxs[i] == 8) {
			ret += websWrite(wp, "\t\tif(document.forms[0].wl_txchain.selectedIndex > 0) {\n");
		}

		/* MCS IDX 32 is valid only for 40 Mhz */
		if (mcsidxs[i] == 32 && (nbw == 20 || nbw == 0))
			continue;
		ret += websWrite(wp, "\t\tdocument.forms[0].wl_nmcsidx[%d] = new Option(\"", i);
		if (mcsidxs[i] == -2) {
			ret += websWrite(wp, "Use Legacy Rate\", \"-2\");\n");
			if (selected_idx == -2 && nbw == selected_bw)
				ret += websWrite(wp,
				                 "\t\tdocument.forms[0].wl_nmcsidx.selectedIndex ="
				                 "%d;\n", i);
		} else {
			uint mcs_rate = MCS_TABLE_RATE(mcsidxs[i], (nbw == 40));
			ret += websWrite(wp, "%2d: %d", mcsidxs[i], mcs_rate/1000);
			/* Handle floating point generation */
			if (mcs_rate % 1000)
				ret += websWrite(wp, ".%d", (mcs_rate % 1000)/100);
			ret += websWrite(wp, " Mbps");
			if (nbw == 0) {
				mcs_rate = MCS_TABLE_RATE(mcsidxs[i], TRUE);
				ret += websWrite(wp, " or %d", mcs_rate/1000);
				/* Handle floating point generation */
				if (mcs_rate % 1000)
					ret += websWrite(wp, ".%d", (mcs_rate % 1000)/100);
				ret += websWrite(wp, " Mbps");
			}
			ret += websWrite(wp, "\", \"%d\");\n", mcsidxs[i]);

			if (selected_idx == mcsidxs[i] && selected_bw == nbw)
				ret += websWrite(wp,
				                 "\t\tdocument.forms[0].wl_nmcsidx.selectedIndex ="
				                 "%d;\n", i);
		}
	}

	/* terminate the "if" condition for txchains */
	ret += websWrite(wp, "\t\t}\n");

	return ret;
}
REG_EJ_HANDLER(wl_nphyrates);

/* Return a list of the available number of txchains */
static int
ej_wl_txchains_list(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	int count;
	int txchain_cnt = 0; /* Default */
	int txchains = txchain_cnt;
	char *str;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	/* # of TX chains supported by device */
	str = nvram_get(strcat_r(prefix, "hw_txchain", tmp));
	if (str) {
		txchain_cnt = atoi(str);
		if (txchain_cnt == 2)
		  txchain_cnt = 1;
		else if (txchain_cnt == 3)
		  txchain_cnt = 2;
		else if (txchain_cnt == 7)
		  txchain_cnt = 3;
		else if (txchain_cnt == 15)
		  txchain_cnt = 4;
	}

	/* User configured # of TX streams */
	str = nvram_get("wl_txchain");
	if (str) {
		txchains = atoi(str);
		if (txchains == 2)
		  txchains = 1;
		else if (txchains == 3)
		  txchains = 2;
		else if (txchains == 7)
		  txchains = 3;
		else if (txchains == 15)
		  txchains = 4;
	}
	for (count=1; count <= txchain_cnt; count++) {
		ret += websWrite(wp, "\t<option value=%d %s>%d</option>\n",
			(count == 1) ? 1 : (count == 2) ? 3 : (count == 3) ? 7 : 15,
			(count == txchains) ? "selected" : "", count);
	}

	return ret;
}
REG_EJ_HANDLER(wl_txchains_list);

/* Return a list of the available number of rxchains */
static int
ej_wl_rxchains_list(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	int count;
	int rxchain_cnt = 0; /* Default */
	int rxchains = rxchain_cnt;
	char *str;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	/* # of RX chains supported by device */
	str = nvram_get(strcat_r(prefix, "hw_rxchain", tmp));
	if (str) {
		rxchain_cnt = atoi(str);
		if (rxchain_cnt == 2)
		  rxchain_cnt = 1;
		else if (rxchain_cnt == 3)
		  rxchain_cnt = 2;
		else if (rxchain_cnt == 7)
		  rxchain_cnt = 3;
		else if (rxchain_cnt == 15)
		  rxchain_cnt = 4;
	}

	/* User configured # of RX streams */
	str = nvram_get("wl_rxchain");
	if (str) {
		rxchains = atoi(str);
		if (rxchains == 2)
		  rxchains = 1;
		else if (rxchains == 3)
		  rxchains = 2;
		else if (rxchains == 7)
		  rxchains = 3;
		else if (rxchains == 15)
		  rxchains = 4;
	}
	for (count=1; count <= rxchain_cnt; count++) {
		ret += websWrite(wp, "\t<option value=%d %s>%d</option>\n",
			(count == 1) ? 1 : (count == 2) ? 3 : (count == 3) ? 7 : 15,
			(count == rxchains) ? "selected" : "", count);
	}
	return ret;
}
REG_EJ_HANDLER(wl_rxchains_list);

static int
ej_wl_inlist(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char *name=NULL, *next=NULL;
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];
	char *var=NULL, *item=NULL;

	if (ejArgs(argc, argv, "%s %s", &var, &item) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)){
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	if (wl_iovar_get(name, var, (void *)caps, sizeof(caps)))
		return -1;

	foreach(cap, caps, next) {
		if (!strcmp(cap, item))
			return websWrite(wp, "1");
	}

	return websWrite(wp, "0");
}
REG_EJ_HANDLER(wl_inlist);

static int
ej_wl_wds_status(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char *macs=NULL, *next=NULL, *name=NULL;
	char mac[100];
	int i=0, len=0;
	sta_info_t *sta=NULL;
	char buf[sizeof(sta_info_t)];

	if (ejArgs(argc, argv, "%d", &i) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)){
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));
	macs = nvram_safe_get(strcat_r(prefix, "wds", tmp));
#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif
	foreach(mac, macs, next) {
		if (i-- == 0) {
			len = snprintf(buf, sizeof(buf), "sta_info");
			ether_atoe(mac, (unsigned char *)&buf[len + 1]);
			if (atoi(nvram_safe_get(strcat_r(prefix, "wds_timeout", tmp))) &&
			    !wl_ioctl(name, WLC_GET_VAR, buf, sizeof(buf))) {
				sta = (sta_info_t *)buf;
#ifdef DSLCPE_ENDIAN
				sta->flags=etoh32(sta->flags);
#endif
				return websWrite(wp, "%s", (sta->flags & WL_STA_WDS_LINKUP) ? "up" : "down");
			}
			else
				return websWrite(wp, "%s", "unknown");
		}
	}

	return 0;
}
REG_EJ_HANDLER(wl_wds_status);

/* If current phytype is NOT nphy, the print start of comment <!--
 * in the HTML for the concerned fields to prevent them from appearing on the page
 */
static int
ej_wl_nphy_comment_beg(int eid, webs_t wp, int argc, char_t **argv)
{
	int phytype, status;

	/* Get configured phy type */
	if ((status = wl_phytype_get(wp, &phytype)) != 0)
		return status;
	return websWrite(wp, "%s", ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
	                            (phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT) ||
	                            (phytype == WLC_PHY_TYPE_AC))? "":"<!--");
}
REG_EJ_HANDLER(wl_nphy_comment_beg);

static int
ej_wl_nphy_comment_end(int eid, webs_t wp, int argc, char_t **argv)
{
	int phytype, status;

	/* Get configured phy type */
	if ((status = wl_phytype_get(wp, &phytype)) != 0)
		return status;
	return websWrite(wp, "%s", ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
	                            (phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT) ||
	                            (phytype == WLC_PHY_TYPE_AC))? "":"-->");
}
REG_EJ_HANDLER(wl_nphy_comment_end);

/* Return the variable for the Band field.
 * For 11n, its wl_nband, while for a/b/g, it's wl_phytype
 */
static int
ej_wl_phytype_name(int eid, webs_t wp, int argc, char_t **argv)
{
	int phytype, status;

	/* Get configured phy type */
	if ((status = wl_phytype_get(wp, &phytype)) != 0)
		return status;

	return websWrite(wp, "%s", ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
	                            (phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT) ||
	                            (phytype == WLC_PHY_TYPE_AC))? "\"wl_nband\"":"\"wl_phytype\"");
}
REG_EJ_HANDLER(wl_phytype_name);

/* Writes "1" if Tx beamforming is supported. Otherwise, "0" */
static int
ej_wl_txbf_capable(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name = NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	int txbf_capable = 0;
	wlc_rev_info_t revinfo;

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif
	/* Get revision info */
	wl_ioctl(name, WLC_GET_REVINFO, &revinfo, sizeof(revinfo));

#ifdef DSLCPE_ENDIAN
	revinfo.corerev=etoh32(revinfo.corerev);
#endif
	/*
	 * Beamforming is available on core revs >= 40. Currently, 1-2
	 * streams have beamforming.
	 */
	if (revinfo.corerev >= 40) {
		int txchain;

		if (wl_iovar_getint(name, "txchain", &txchain))
			return -1;

		if (bits_count((unsigned int)txchain) > 1) {
			txbf_capable = 1;
		}
	}

	return websWrite(wp, "%d", txbf_capable);
}
REG_EJ_HANDLER(wl_txbf_capable);

static int
ej_wl_txbf_bfr_list(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name = NULL, *next = NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];

	if (!make_wl_prefix(prefix, sizeof(prefix), 0, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps)))
		return -1;

	websWrite(wp, "<option value=\"0\" %s>Disabled</option>",
		nvram_match("wl_txbf_bfr_cap", "0") ? "selected" : "");

	foreach(cap, caps, next) {
		if (!strcmp(cap, "multi-user-beamformer")) {
			websWrite(wp, "<option value=\"1\" %s>SU BFR</option>",
				nvram_match("wl_txbf_bfr_cap", "1") ? "selected" : "");
			websWrite(wp, "<option value=\"2\" %s>MU BFR</option>",
				nvram_match("wl_txbf_bfr_cap", "2") ? "selected" : "");
			return 0;
		}
	}

	/* If MU BFR is not supported, display SU BFR on GUI when wl_txbf_bfr_cap != 0 */
	websWrite(wp, "<option value=\"1\" %s>SU BFR</option>",
		!nvram_match("wl_txbf_bfr_cap", "0") ? "selected" : "");

	return 0;
}
REG_EJ_HANDLER(wl_txbf_bfr_list);

static int
ej_wl_txbf_bfe_list(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name = NULL, *next = NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];

	if (!make_wl_prefix(prefix, sizeof(prefix), 0, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps)))
		return -1;

	websWrite(wp, "<option value=\"0\" %s>Disabled</option>",
		nvram_match("wl_txbf_bfe_cap", "0") ? "selected" : "");

	foreach(cap, caps, next) {
		if (!strcmp(cap, "multi-user-beamformee")) {
			websWrite(wp, "<option value=\"1\" %s>SU BFE</option>",
				nvram_match("wl_txbf_bfe_cap", "1") ? "selected" : "");
			websWrite(wp, "<option value=\"2\" %s>MU BFE</option>",
				nvram_match("wl_txbf_bfe_cap", "2") ? "selected" : "");
			return 0;
		}
	}

	/* If MU BFE is not supported, display SU BFE on GUI when wl_txbf_bfe_cap != 0 */
	websWrite(wp, "<option value=\"1\" %s>SU BFE</option>",
	          !nvram_match("wl_txbf_bfe_cap", "0") ? "selected" : "");

	return 0;
}
REG_EJ_HANDLER(wl_txbf_bfe_list);

static int
ej_mutx_feature_list(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name = NULL, *next = NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];
	char *mu_features;
	uint32 mu_features_val, mutx_feature;

#define MU_FEATURES_MUTX	(1 << 0)
#define MU_FEATURES_AUTO	(1 << 15)

	if (!make_wl_prefix(prefix, sizeof(prefix), 0, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps)))
		return -1;

	mu_features = nvram_safe_get(strcat_r(prefix, "mu_features", tmp));
	mu_features_val = strtoul(mu_features, NULL, 0);
	mutx_feature = mu_features_val & (MU_FEATURES_MUTX | MU_FEATURES_AUTO);
	websWrite(wp, "<option value=\"0\" %s>Disabled</option>",
		(mutx_feature == 0) ? "selected" : "");

	foreach(cap, caps, next) {
		if (!strcmp(cap, "multi-user-beamformer")) {
			websWrite(wp, "<option value=\"1\" %s>Enabled</option>",
				(mutx_feature == 1) ? "selected" : "");
			websWrite(wp, "<option value=\"0x8000\" %s>Auto</option>",
				(mutx_feature & 0x8000) ? "selected" : "");
			return 0;
		}
	}

	return 0;
}
REG_EJ_HANDLER(mutx_feature_list);

/* Return current country */
static int
ej_wl_cur_country(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name=NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	wl_country_t cspec = {{0}, 0, {0}};

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)){
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	if (wl_iovar_get(name, "country", &cspec, sizeof(cspec)))
		return -1;
	return websWrite(wp, "%s", cspec.ccode);
}
REG_EJ_HANDLER(wl_cur_country);

static int
ej_wl_cur_regrev(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name=NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	wl_country_t cspec = {{0}, 0, {0}};

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)){
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif
	/* country_rev is not in the nvram get it from the driver */
	if (wl_iovar_get(name, "country", &cspec, sizeof(cspec)))
		return -1;
#ifdef DSLCPE_ENDIAN
	return websWrite(wp, "%d", etoh32(cspec.rev));
#else
	return websWrite(wp, "%d", cspec.rev);
#endif
}
REG_EJ_HANDLER(wl_cur_regrev);

/* Return current phytype */
static int
ej_wl_cur_phytype(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	int phytype;

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)){
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif
	/* Get configured phy type */
	wl_ioctl(name, WLC_GET_PHYTYPE, &phytype, sizeof(phytype));
#ifdef DSLCPE_ENDIAN
	phytype=etoh32(phytype);
#endif
	if ((phytype != WLC_PHY_TYPE_N) && (phytype != WLC_PHY_TYPE_SSN) &&
	    (phytype != WLC_PHY_TYPE_LCN) && (phytype != WLC_PHY_TYPE_HT) &&
	    (phytype != WLC_PHY_TYPE_AC))
		if (phytype == WLC_PHY_TYPE_LP) {
			wl_ioctl(name, WLC_GET_BAND, &phytype, sizeof(phytype));
			return websWrite(wp, "Current: %s", phytype == WLC_BAND_5G ? "5 GHz" :
		                 phytype == WLC_BAND_2G ? "2.4 GHz" : "Auto");
		} else
			return websWrite(wp, "Current: 802.11%s", phytype == WLC_PHY_TYPE_A ? "a" :
		                 phytype == WLC_PHY_TYPE_B ? "b" : "g");
	else {
		wl_ioctl(name, WLC_GET_BAND, &phytype, sizeof(phytype));
#ifdef DSLCPE_ENDIAN
		phytype=etoh32(phytype);
#endif
		return websWrite(wp, "Current: %s", phytype == WLC_BAND_5G ? "5 GHz" :
		                 phytype == WLC_BAND_2G ? "2.4 GHz" : "Auto");
	}
	return 0;
}
REG_EJ_HANDLER(wl_cur_phytype);

/* Return a list of the supported bands on the currently selected wireless interface */
static int
ej_wl_phytypes(int eid, webs_t wp, int argc, char_t **argv)
{
	int  ret = 0;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char *phytype=NULL, *name;
	char *phylist=NULL;
	int i=0;
	int bandtype, error;
	int list[WLC_BAND_ALL];

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL))
		return websWrite(wp, "None");

	/* Get configured phy type */
	phytype = nvram_safe_get("wl_phytype");

	if (phytype[i] == 'n' || phytype[i] == 'l' || phytype[i] == 's' || phytype[i] == 'c' || phytype[i] == 'h' || phytype[i] == 'v') {
		bandtype = atoi(nvram_safe_get(strcat_r(prefix, "nband", tmp)));

		name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

#ifdef DSLCPE_ENDIAN
		wl_endian_probe(name);
#endif
		/* Get band list. Assume both the bands in case of error */
		if ((error = wl_ioctl(name, WLC_GET_BANDLIST, list, sizeof(list))) < 0) {
			for (i = 1; i < 3; i++)
				ret += websWrite(wp, "<option value=\"%d\" %s> %s GHz</option>\n",
				                 i,
				                 (bandtype == i)? "selected": "",
				                 (i == WLC_BAND_5G)? "5":"2.4");
			return ret;
		}

#ifdef DSLCPE_ENDIAN
		for (i = 0; i < WLC_BAND_ALL; ++i)
			list[i]=etoh32(list[i]);
#endif
		if (list[0] > 2)
			list[0] = 2;

		for (i = 1; i <= list[0]; i++) {
			if ((phytype[0] == 'n') || (phytype[0] == 's')|| (phytype[0] == 'c') || (phytype[0] == 'h') || phytype[0] == 'v')
				ret += websWrite(wp, "<option value=\"%d\" %s> %s GHz</option>",
					list[i], (bandtype == list[i])? "selected": "",
					(list[i] == WLC_BAND_5G)? "5":"2.4");
			else /* if lpphy */ {
				if (list[i] == WLC_BAND_5G)
					snprintf( tmp, sizeof(tmp), "802.11%c (%s GHz)",'a',"5");
				else
					snprintf( tmp, sizeof(tmp), "802.11%c (%s GHz)",'g',"2.4");
				ret += websWrite(wp, "<option value=\"%c\" %s>%s</option>",
					((list[i] == WLC_BAND_5G)? 'a':'g'),
					bandtype == list[i] ? "selected" : "", tmp);
			}
		}
		return ret;
	}
	/* Get available phy types of the currently selected wireless interface */
	phylist = nvram_safe_get(strcat_r(prefix, "phytypes", tmp));

	for (i = 0; i < strlen(phylist); i++) {
		snprintf( tmp, sizeof(tmp), "802.11%c (%s GHz)", phylist[i],
		        phylist[i] == 'a' ? "5" : "2.4");
		ret += websWrite(wp, "<option value=\"%c\" %s>%s</option>",
		                 phylist[i], phylist[i] == *phytype ? "selected" : "", tmp);
	}

	return ret;
}
REG_EJ_HANDLER(wl_phytypes);

/* Return current wireless channel specification */
static int
ej_wl_cur_chanspec(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name=NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	channel_info_t ci;
	int phytype;
	uint32 chanspec;
	int channel;
	int chan_adj = 0;
	int status;
	uint32 chanim_enab = 0;
	uint32 interference = 0;
	const char CHANIM_S[] =  "***Interference Level: Severe";
	const char CHANIM_A[] =  "***Interference Level: Acceptable";
	static union {
		char bufdata[WLC_IOCTL_SMLEN];
		uint32 alignme;
	} bufstruct;
	char *retbuf = (char*) &bufstruct.bufdata;
	char chanbuf[CHANSPEC_STR_LEN];
#define CHANIMSTR(a, b, c, d) ((a) ? ((b) ? c : d) : "")

	(void) chan_adj;
	(void) channel;
	(void) ci;

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)){
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif
	/* Get configured phy type */
	if ((status = wl_phytype_get(wp, &phytype)) != 0)
		return status;

	if (wl_iovar_getint(name, "chanim_enab", (int*)(void*)&chanim_enab))
		chanim_enab = 0;

	if (wl_iovar_getint(name, "chanspec", (int*)(void *)&chanspec))
		return -1;

	if (chanim_enab) {
#ifdef DSLCPE_ENDIAN
	    chanspec=etoh32(chanspec);
#endif
		if (wl_iovar_getbuf(name, "chanim_state", &chanspec, sizeof(chanspec),
			  retbuf, WLC_IOCTL_SMLEN))
			return -1;

#ifdef DSLCPE_ENDIAN
		interference = etoh32(*(int*)retbuf);
#else
		interference = *(int*)retbuf;
#endif
	}

	return websWrite(wp, "Current: %s %s",
#ifdef DSLCPE_ENDIAN
	                 wf_chspec_ntoa(etoh32(chanspec), chanbuf),
#else
	                 wf_chspec_ntoa(chanspec, chanbuf),
#endif
			 CHANIMSTR(chanim_enab, interference, CHANIM_S, CHANIM_A));

#undef CHANIMSTR
}
REG_EJ_HANDLER(wl_cur_chanspec);

static int
ej_wl_bw_cap_list(int eid, webs_t wp, int argc, char_t **argv)
{
	int status = 0;
	char *name = NULL, *next = NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	int cur_phytype;
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		status = -1;
		goto exit;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif
	/* Get configured phy type */
	wl_ioctl(name, WLC_GET_PHYTYPE, &cur_phytype, sizeof(cur_phytype));

#ifdef DSLCPE_ENDIAN
	cur_phytype=etoh32(cur_phytype);
#endif
	websWrite(wp, "<option value=\"1\" %s>20 MHz</option>",
	          nvram_match("wl_bw_cap", "1") ? "selected" : "");
	websWrite(wp, "<option value=\"3\" %s>40 MHz</option>",
	          nvram_match("wl_bw_cap", "3") ? "selected" : "");

	if (cur_phytype == WLC_PHY_TYPE_AC) {
		/* List 80 MHz bandwidth as well */
		websWrite(wp, "<option value=\"7\" %s>80 MHz</option>",
		          nvram_match("wl_bw_cap", "7") ? "selected" : "");

		if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps)))
			return -1;

		/* List 160 MHz bandwidth if phy support */
		foreach(cap, caps, next) {
			if (!strcmp(cap, "160")) {
				websWrite(wp, "<option value=\"15\" %s>160 MHz</option>",
				nvram_match("wl_bw_cap", "15") ? "selected" : "");
			}
		}
	}

exit:
	return status;
}
REG_EJ_HANDLER(wl_bw_cap_list);

/* Return current 802.11n channel bandwidth */
static int
ej_wl_cur_bw(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name=NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
#ifdef DSLCPE_ENDIAN
	int chanspec;
#else
	chanspec_t chanspec;
#endif

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)){
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	if (wl_iovar_getint(name, "chanspec", (int*)(void *)&chanspec))
		return -1;

	return websWrite(wp, "Current: %s",
	                 (CHSPEC_BW(chanspec) == WL_CHANSPEC_BW_160) ? "160MHz" :
	                 (CHSPEC_BW(chanspec) == WL_CHANSPEC_BW_80) ? "80MHz" :
	                 (CHSPEC_BW(chanspec) == WL_CHANSPEC_BW_40) ? "40MHz" : "20MHz");
}
REG_EJ_HANDLER(wl_cur_bw);

/* Generate the whole protection rows */
static int
ej_wl_protection(int eid, webs_t wp, int argc, char_t **argv)
{
	int phytype, status;
	int ret =0;
	char *prot_str;

	/* Get configured phy type */
	if ((status = wl_phytype_get(wp, &phytype)) != 0)
		return status;

	prot_str = ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
			(phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT) ||
			(phytype == WLC_PHY_TYPE_AC))?
		"wl_nmode_protection":"wl_gmode_protection";

	ret += websWrite(wp, "<th width=\"310\"\n\tonMouseOver=\"return overlib('In <b>Auto</b>"
	                 "mode the AP will use RTS/CTS to improve");
	ret += websWrite(wp, " %s performance in mixed %s networks. Turn protection <b>Off</b> to "
	                 "maximize %s througput under most conditions.', LEFT);\"\n",
	                 ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
			  (phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT) ||
			  (phytype == WLC_PHY_TYPE_AC))?
				"802.11n":"802.11g",
	                 ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
			  (phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT) ||
			  (phytype == WLC_PHY_TYPE_AC))?
				"802.11n/a/b/g":"802.11g/b",
	                 ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
			  (phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT) ||
			  (phytype == WLC_PHY_TYPE_AC))?
				"802.11n":"802.11g");
	ret += websWrite(wp, "\tonMouseOut=\"return nd();\">\n\t%s Protection:&nbsp;&nbsp;"
	                 "\n\t</th>\n\t<td>&nbsp;&nbsp;</td>\n\t<td>\n",
	                 ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
			  (phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT) ||
			  (phytype == WLC_PHY_TYPE_AC))?
				"802.11n":"54g");
	ret += websWrite(wp, "\t\t<select name=\"%s\">\n",
	                 prot_str);
	ret += websWrite(wp, "\t\t\t<option value=\"auto\" %s>Auto</option>\n",
	                 nvram_match(prot_str, "auto")?"selected":"");
	ret += websWrite(wp, "\t\t\t<option value=\"off\" %s>Off</option>\n",
	                 nvram_match(prot_str, "off")?"selected":"");
	ret += websWrite(wp, "\t\t</select>\n\t</td>");

	return ret;
}
REG_EJ_HANDLER(wl_protection);

/* Generate the whole mimo preamble rows */
static int
ej_wl_mimo_preamble(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret =0, mode = 0;
	char *prot_str;
	char prefix[] = "wlXXXXXXXXXX_";
	char *name = NULL;
	char wlif[64];
	wlc_rev_info_t rev;

	if (!make_wl_prefix(prefix, sizeof(prefix), mode, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname",wlif));

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif
	wl_ioctl(name, WLC_GET_REVINFO, &rev, sizeof(rev));
#ifdef DSLCPE_ENDIAN
	rev.chipnum=etoh32(rev.chipnum);
#endif

	if (!((rev.chipnum == BCM4716_CHIP_ID) ||
		(rev.chipnum == BCM47162_CHIP_ID) ||
		(rev.chipnum == BCM4748_CHIP_ID) ||
		(rev.chipnum == BCM4331_CHIP_ID) ||
		(rev.chipnum == BCM43431_CHIP_ID)))
		return -1;

	prot_str =  "wl_mimo_preamble";

	ret += websWrite(wp, "<th width=\"310\"\n\tonMouseOver=\"return overlib('Force to use Green-Field or Mixed-Mode preamble. in<b>Auto</b>"
				"mode the AP will use GF or MM based on required protection ', LEFT);\"\n");
	ret += websWrite(wp, "\tonMouseOut=\"return nd();\">\n\t%s Mimo PrEamble:&nbsp;&nbsp;"
				"\n\t</th>\n\t<td>&nbsp;&nbsp;</td>\n\t<td>\n","802.11n");
	ret += websWrite(wp, "\t\t<select name=\"%s\">\n", prot_str);
	ret += websWrite(wp, "\t\t\t<option value=\"gfbcm\" %s>GF-BRCM</option>\n",
				nvram_match(prot_str, "gfbcm")?"selected":"");
	ret += websWrite(wp, "\t\t\t<option value=\"auto\" %s>Auto</option>\n",
				nvram_match(prot_str, "auto")?"selected":"");
	ret += websWrite(wp, "\t\t\t<option value=\"gf\" %s>Green Field</option>\n",
				nvram_match(prot_str, "gf")?"selected":"");
	ret += websWrite(wp, "\t\t\t<option value=\"mm\" %s>Mixed Mode</option>\n",
				nvram_match(prot_str, "mm")?"selected":"");
	ret += websWrite(wp, "\t\t</select>\n\t</td>");

	return ret;
}
REG_EJ_HANDLER(wl_mimo_preamble);

/* If current phytype is nphy, the print string 'legacy' for Rate drop down */
static int
ej_wl_legacy_string(int eid, webs_t wp, int argc, char_t **argv)
{
	int phytype, status;

	/* Get configured phy type */
	if ((status = wl_phytype_get(wp, &phytype)) != 0)
		return status;

	return websWrite(wp, "%s", ((phytype == WLC_PHY_TYPE_N) || (phytype == WLC_PHY_TYPE_SSN) ||
			(phytype == WLC_PHY_TYPE_LCN) || (phytype == WLC_PHY_TYPE_HT))? "Legacy":"");
}
REG_EJ_HANDLER(wl_legacy_string);

static int
ej_wl_country_rev_list(int eid, webs_t wp, int argc, char_t **argv)
{
	/* country_rev from 0 to 99 */
	int i =0, status = 0;
	char *name =NULL;
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char *phytype = NULL;
	wl_country_list_t *cl = (wl_country_list_t *)malloc(WLC_IOCTL_MAXLEN);
	char *abbrev=NULL;
	int band = WLC_BAND_ALL, cur_phytype;

	if (!cl) {
		status = -1;
		goto exit;
	}

	if (ejArgs(argc, argv, "%s %d", &phytype, &band) < 1) {
		websError(wp, 400, "Insufficient args\n");
		status = -1;
		goto exit;
	}

	assert(phytype);
	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		status = -1;
		goto exit;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif 
	/* Get configured phy type */
	wl_ioctl(name, WLC_GET_PHYTYPE, &cur_phytype, sizeof(cur_phytype));
#ifdef DSLCPE_ENDIAN
	cur_phytype=etoh32(cur_phytype);
#endif 
	cl->buflen = WLC_IOCTL_MAXLEN;
	cl->band_set = TRUE;

	if (!strcmp(phytype, "a") &&
	    (cur_phytype != WLC_PHY_TYPE_N) && (cur_phytype != WLC_PHY_TYPE_SSN) &&
	    (cur_phytype != WLC_PHY_TYPE_LCN) && (cur_phytype != WLC_PHY_TYPE_HT) &&
	    (cur_phytype != WLC_PHY_TYPE_AC)) {
		cl->band = WLC_BAND_5G;
	} else if (((!strcmp(phytype, "b")) || (!strcmp(phytype, "g"))) &&
	           (cur_phytype != WLC_PHY_TYPE_N) && (cur_phytype != WLC_PHY_TYPE_SSN) &&
	           (cur_phytype != WLC_PHY_TYPE_LCN) && (cur_phytype != WLC_PHY_TYPE_HT) &&
	           (cur_phytype != WLC_PHY_TYPE_AC)) {
		cl->band = WLC_BAND_2G;
	} else if ((!strcmp(phytype, "n") || !strcmp(phytype, "s") || !strcmp(phytype, "h") ||
	            !strcmp(phytype, "v")) &&
	           ((cur_phytype == WLC_PHY_TYPE_N) || (cur_phytype == WLC_PHY_TYPE_SSN) ||
	            (cur_phytype == WLC_PHY_TYPE_LCN) || (cur_phytype == WLC_PHY_TYPE_HT) ||
	            (cur_phytype == WLC_PHY_TYPE_AC))) {
		/* Need to have additional argument of the band */
		if (argc < 2 || (band != WLC_BAND_2G && band != WLC_BAND_5G)) {
			status = -1;
			goto exit;
		}
		cl->band = band;
	} else if ((!strcmp(phytype, "l")) && (cur_phytype == WLC_PHY_TYPE_LP)) {
		wl_ioctl(name, WLC_GET_BAND, &band, sizeof(band));
#ifdef DSLCPE_ENDIAN
		band=etoh32(band);
#endif
		cl->band = band;
	} else {
		status = -1;
		goto exit;
	}

#ifdef DSLCPE_ENDIAN
	cl->buflen= htoe32(cl->buflen);
	cl->band_set= htoe32(cl->band_set);
	cl->band= htoe32(cl->band);
	cl->count= htoe32(cl->count);
	if (wl_ioctl(name, WLC_GET_COUNTRY_LIST, cl, WLC_IOCTL_MAXLEN ) == 0) {
#else
	if (wl_ioctl(name, WLC_GET_COUNTRY_LIST, cl, cl->buflen) == 0) {
#endif
#ifdef DSLCPE_ENDIAN
		cl->buflen= etoh32(cl->buflen);
		cl->band_set= etoh32(cl->band_set);
		cl->band= etoh32(cl->band);
		cl->count= etoh32(cl->count);
#endif
		for(i = 0; i < cl->count; i++) {
			abbrev = &cl->country_abbrev[i*WLC_CNTRY_BUF_SZ];
			wl_print_country_rev_list(wp, name, abbrev);
		}
	}

exit:
	if (cl)
		free((void *)cl);
	return status;

}
REG_EJ_HANDLER(wl_country_rev_list);

/*
 * Country names and abbreviations from ISO 3166
 */
country_name_t country_names[] = {

{"COUNTRY Z1",		 "Z1"},
{"COUNTRY Z2",		 "Z2"},
{"AFGHANISTAN",		 "AF"},
{"ALBANIA",		 "AL"},
{"ALGERIA",		 "DZ"},
{"AMERICAN SAMOA",	 "AS"},
{"ANDORRA",		 "AD"},
{"ANGOLA",		 "AO"},
{"ANGUILLA",		 "AI"},
{"ANTARCTICA",		 "AQ"},
{"ANTIGUA AND BARBUDA",	 "AG"},
{"ARGENTINA",		 "AR"},
{"ARMENIA",		 "AM"},
{"ARUBA",		 "AW"},
{"ASCENSION ISLAND",		 "AC"},
{"ASHMORE AND BARBUDA",		 "AG"},
{"AUSTRALIA",		 "AU"},
{"AUSTRIA",		 "AT"},
{"AZERBAIJAN",		 "AZ"},
{"BAHAMAS",		 "BS"},
{"BAHRAIN",		 "BH"},
{"BAKER ISLAND",		 "Z2"},
{"BANGLADESH",		 "BD"},
{"BARBADOS",		 "BB"},
{"BELARUS",		 "BY"},
{"BELGIUM",		 "BE"},
{"BELIZE",		 "BZ"},
{"BENIN",		 "BJ"},
{"BERMUDA",		 "BM"},
{"BHUTAN",		 "BT"},
{"BOLIVIA",		 "BO"},
{"BOSNIA AND HERZEGOVINA","BA"},
{"BOTSWANA",		 "BW"},
{"BOUVET ISLAND",	 "BV"},
{"BRAZIL",		 "BR"},
{"BRITISH INDIAN OCEAN TERRITORY", "IO"},
{"BRUNEI DARUSSALAM",	 "BN"},
{"BULGARIA",		 "BG"},
{"BURKINA FASO",	 "BF"},
{"BURUNDI",		 "BI"},
{"CAMBODIA",		 "KH"},
{"CAMEROON",		 "CM"},
{"CANADA",		 "CA"},
{"CAPE VERDE",		 "CV"},
{"CAYMAN ISLANDS",	 "KY"},
{"CENTRAL AFRICAN REPUBLIC","CF"},
{"CHAD",		 "TD"},
{"CHANNEL ISLANDS",		 "Z1"},
{"CHILE",		 "CL"},
{"CHINA",		 "CN"},
{"CHRISTMAS ISLAND",	 "CX"},
{"CLIPPERTON ISLAND",	 "CP"},
{"COCOS (KEELING) ISLANDS","CC"},
{"COLOMBIA",		 "CO"},
{"COMOROS",		 "KM"},
{"CONGO",		 "CG"},
{"CONGO, THE DEMOCRATIC REPUBLIC OF THE", "CD"},
{"COOK ISLANDS",	 "CK"},
{"COSTA RICA",		 "CR"},
{"COTE D'IVOIRE",	 "CI"},
{"CROATIA",		 "HR"},
{"CUBA",		 "CU"},
{"CYPRUS",		 "CY"},
{"CZECH REPUBLIC",	 "CZ"},
{"DENMARK",		 "DK"},
{"DIEGO GARCIA",	 "Z1"},
{"DJIBOUTI",		 "DJ"},
{"DOMINICA",		 "DM"},
{"DOMINICAN REPUBLIC",	 "DO"},
{"ECUADOR",		 "EC"},
{"EGYPT",		 "EG"},
{"EL SALVADOR",		 "SV"},
{"EQUATORIAL GUINEA",	 "GQ"},
{"ERITREA",		 "ER"},
{"ESTONIA",		 "EE"},
{"ETHIOPIA",		 "ET"},
{"FALKLAND ISLANDS (MALVINAS)",	"FK"},
{"FAROE ISLANDS",	 "FO"},
{"FIJI",		 "FJ"},
{"FINLAND",		 "FI"},
{"FRANCE",		 "FR"},
{"FRENCH GUIANA",	 "GF"},
{"FRENCH POLYNESIA",	 "PF"},
{"FRENCH SOUTHERN TERRITORIES",	 "TF"},
{"GABON",		 "GA"},
{"GAMBIA",		 "GM"},
{"GEORGIA",		 "GE"},
{"GERMANY",		 "DE"},
{"GHANA",		 "GH"},
{"GIBRALTAR",		 "GI"},
{"GREECE",		 "GR"},
{"GREENLAND",		 "GL"},
{"GRENADA",		 "GD"},
{"GUADELOUPE",		 "GP"},
{"GUAM",		 "GU"},
{"GUANTANAMO BAY",		 "Z1"},
{"GUATEMALA",		 "GT"},
{"GUERNSEY",		 "GG"},
{"GUINEA",		 "GN"},
{"GUINEA-BISSAU",	 "GW"},
{"GUYANA",		 "GY"},
{"HAITI",		 "HT"},
{"HEARD ISLAND AND MCDONALD ISLANDS",	"HM"},
{"HOLY SEE (VATICAN CITY STATE)",	"VA"},
{"HONDURAS",		 "HN"},
{"HONG KONG",		 "HK"},
{"HOWLAND ISLAND",		 "Z2"},
{"HUNGARY",		 "HU"},
{"ICELAND",		 "IS"},
{"INDIA",		 "IN"},
{"INDONESIA",		 "ID"},
{"IRAN, ISLAMIC REPUBLIC OF",		"IR"},
{"IRAQ",		 "IQ"},
{"IRELAND",		 "IE"},
{"ISRAEL",		 "IL"},
{"ITALY",		 "IT"},
{"JAMAICA",		 "JM"},
{"JAN MAYEN AMAICA",		 "Z1"},
{"JAPAN",		 "JP"},
{"JAPAN_1",		 "J1"},
{"JAPAN_2",		 "J2"},
{"JAPAN_3",		 "J3"},
{"JAPAN_4",		 "J4"},
{"JAPAN_5",		 "J5"},
{"JAPAN_6",		 "J6"},
{"JAPAN_7",		 "J7"},
{"JAPAN_8",		 "J8"},
{"JARVIS ISLAND",		 "Z2"},
{"JERSEY",		 "JE"},
{"JOHNSTON ATOLL",		 "Z2"},
{"JORDON",		 "JO"},
{"KAZAKHSTAN",		 "KZ"},
{"KENYA",		 "KE"},
{"KINGMAN REEF",		 "Z2"},
{"KIRIBATI",		 "KI"},
{"KOREA, DEMOCRATIC PEOPLE'S REPUBLIC OF", "KP"},
{"KOREA, REPUBLIC OF",	 "KR"},
{"KUWAIT",		 "KW"},
{"KYRGYZSTAN",		 "KG"},
{"LAO PEOPLE'S DEMOCRATIC REPUBLIC",	"LA"},
{"LATVIA",		 "LV"},
{"LEBANON",		 "LB"},
{"LESOTHO",		 "LS"},
{"LIBERIA",		 "LR"},
{"LIBYAN ARAB JAMAHIRIYA","LY"},
{"LIECHTENSTEIN",	 "LI"},
{"LITHUANIA",		 "LT"},
{"LUXEMBOURG",		 "LU"},
{"MACAO",		 "MO"},
{"MACEDONIA, THE FORMER YUGOSLAV REPUBLIC OF",	 "MK"},
{"MADAGASCAR",		 "MG"},
{"MALAWI",		 "MW"},
{"MALAYSIA",		 "MY"},
{"MALDIVES",		 "MV"},
{"MALI",		 "ML"},
{"MALTA",		 "MT"},
{"MAN, ISLE OF",		 "IM"},
{"MARSHALL ISLANDS",	 "MH"},
{"MARTINIQUE",		 "MQ"},
{"MAURITANIA",		 "MR"},
{"MAURITIUS",		 "MU"},
{"MAYOTTE",		 "YT"},
{"MEXICO",		 "MX"},
{"MICRONESIA, FEDERATED STATES OF",	"FM"},
{"MIDWAY ISLANDS",	"Z2"},
{"MOLDOVA, REPUBLIC OF", "MD"},
{"MONACO",		 "MC"},
{"MONGOLIA",		 "MN"},
{"MONTSERRAT",		 "MS"},
{"MOROCCO",		 "MA"},
{"MOZAMBIQUE",		 "MZ"},
{"MYANMAR",		 "MM"},
{"NAMIBIA",		 "NA"},
{"NAURU",		 "NR"},
{"NEPAL",		 "NP"},
{"NETHERLANDS",		 "NL"},
{"NETHERLANDS ANTILLES", "AN"},
{"NEW CALEDONIA",	 "NC"},
{"NEW ZEALAND",		 "NZ"},
{"NICARAGUA",		 "NI"},
{"NIGER",		 "NE"},
{"NIGERIA",		 "NG"},
{"NIUE",		 "NU"},
{"NORFOLK ISLAND",	 "NF"},
{"NORTHERN MARIANA ISLANDS","MP"},
{"NORWAY",		 "NO"},
{"OMAN",		 "OM"},
{"PAKISTAN",		 "PK"},
{"PALAU",		 "PW"},
{"PALESTINIAN TERRITORY, OCCUPIED",	"PS"},
{"PALMYRA ATOLL",	"Z2"},
{"PANAMA",		 "PA"},
{"PAPUA NEW GUINEA",	 "PG"},
{"PARAGUAY",		 "PY"},
{"PERU",		 "PE"},
{"PHILIPPINES",		 "PH"},
{"PITCAIRN",		 "PN"},
{"POLAND",		 "PL"},
{"PORTUGAL",		 "PT"},
{"PUERTO RICO",		 "PR"},
{"QATAR",		 "QA"},
{"Q1",		 "Q1"},
{"REUNION",		 "RE"},
{"ROMANIA",		 "RO"},
{"ROTA ISLAND",		 "Z1"},
{"RUSSIAN FEDERATION",	 "RU"},
{"RWANDA",		 "RW"},
{"SAINT HELENA",	 "SH"},
{"SAINT KITTS AND NEVIS","KN"},
{"SAINT LUCIA",		 "LC"},
{"SAINT PIERRE AND MIQUELON",		"PM"},
{"SAINT VINCENT AND THE GRENADINES",	"VC"},
{"SAIPAN",	"Z1"},
{"SAMOA",		 "WS"},
{"SAN MARINO",		 "SM"},
{"SAO TOME AND PRINCIPE","ST"},
{"SAUDI ARABIA",	 "SA"},
{"SENEGAL",		 "SN"},
{"SEYCHELLES",		 "SC"},
{"SIERRA LEONE",	 "SL"},
{"SINGAPORE",		 "SG"},
{"SLOVAKIA",		 "SK"},
{"SLOVENIA",		 "SI"},
{"SOLOMON ISLANDS",	 "SB"},
{"SOMALIA",		 "SO"},
{"SOUTH AFRICA",	 "ZA"},
{"SOUTH GEORGIA AND THE SOUTH SANDWICH ISLANDS", "GS"},
{"SPAIN",		 "ES"},
{"SRI LANKA",		 "LK"},
{"SUDAN",		 "SD"},
{"SURINAME",		 "SR"},
{"SVALBARD AND JAN MAYEN","SJ"},
{"SWAZILAND",		 "SZ"},
{"SWEDEN",		 "SE"},
{"SWITZERLAND",		 "CH"},
{"SYRIAN ARAB REPUBLIC", "SY"},
{"TAIWAN, PROVINCE OF CHINA",		"TW"},
{"TAJIKISTAN",		 "TJ"},
{"TANZANIA, UNITED REPUBLIC OF",	"TZ"},
{"THAILAND",		 "TH"},
{"TIMOR-LESTE",		 "TL"},
{"TINIAN ISLAND",	 "Z1"},
{"TOGO",		 "TG"},
{"TOKELAU",		 "TK"},
{"TONGA",		 "TO"},
{"TRINIDAD AND TOBAGO",	 "TT"},
{"TRISTAN DA CUNHA",	 "TA"},
{"TUNISIA",		 "TN"},
{"TURKEY",		 "TR"},
{"TURKMENISTAN",	 "TM"},
{"TURKS AND CAICOS ISLANDS",		"TC"},
{"TUVALU",		 "TV"},
{"UGANDA",		 "UG"},
{"UKRAINE",		 "UA"},
{"UNITED ARAB EMIRATES", "AE"},
{"UNITED KINGDOM",	 "GB"},
{"UNITED STATES",	 "US"},
{"UNITED STATES MINOR OUTLYING ISLANDS","UM"},
{"URUGUAY",		 "UY"},
{"UZBEKISTAN",		 "UZ"},
{"VANUATU",		 "VU"},
{"VENEZUELA",		 "VE"},
{"VIET NAM",		 "VN"},
{"VIRGIN ISLANDS, BRITISH", "VG"},
{"VIRGIN ISLANDS, U.S.", "VI"},
{"WAKE ISLAND",		 "Z1"},
{"WALLIS AND FUTUNA",	 "WF"},
{"WESTERN SAHARA",	 "EH"},
{"YEMEN",		 "YE"},
{"YUGOSLAVIA",		 "YU"},
{"ZAMBIA",		 "ZM"},
{"ZIMBABWE",		 "ZW"},
{"ALL",			 "ALL"},
{"RADAR CHANNELS",	 "RDR"},
{"XA (EUROPE / APAC 2005)",      "XA"},
{"XB (NORTH AND SOUTH AMERICA AND TAIWAN)",      "XB"},
{"X0 (FCC WORLDWIDE)",   "X0"},
{"X1 (WORLDWIDE APAC)",  "X1"},
{"X2 (WORLDWIDE ROW 2)", "X2"},
{"X3 (ETSI)",            "X3"},
{"EU (EUROPEAN UNION)",  "EU"},
{"XW (WORLDWIDE LOCALE FOR LINUX DRIVER)",       "XW"},
{"XX (WORLDWIDE LOCALE (PASSIVE Ch12-14))",      "XX"},
{"XY (FAKE COUNTRY CODE)",       "XY"},
{"XZ (WORLDWIDE LOCALE (PASSIVE Ch12-14))",      "XZ"},
{"XU (EUROPEAN LOCALE 0dBi ANTENNA IN 2.4GHz)",  "XU"},
{"XV (WORLDWIDE SAFE MODE LOCALE (PASSIVE Ch12-14))",    "XV"},
{"XT (SINGLE SKU fOR PC-OEMs)",  "XT"},
{NULL,			 NULL}
};
