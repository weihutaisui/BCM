/*
 * Common [OS-independent] rate management
 * 802.11 Networking Adapter Device Driver.
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
 *
 * $Id: bcmwifi_rates.c 678788 2017-01-11 03:00:35Z $
 */

#include <typedefs.h>
#include <osl.h>
#include <proto/802.11.h>

#include <bcmwifi_rspec.h>
#include <bcmwifi_rates.h>

/* TODO: Consolidate rate utility functions from wlc_rate.c and bcmwifi_monitor.c
 * into here if they're shared by non wl layer as well...
 */

/* ============================================ */
/* ============================================ */

struct ieee_80211_mcs_rate_info {
	uint8 constellation_bits;
	uint8 coding_q;
	uint8 coding_d;
	uint8 dcm_capable;	/* 1 if dcm capable */
};

static const struct ieee_80211_mcs_rate_info wlc_mcs_info[] = {
	{ 1, 1, 2, 1 }, /* MCS  0: MOD: BPSK,   CR 1/2, dcm capable */
	{ 2, 1, 2, 1 }, /* MCS  1: MOD: QPSK,   CR 1/2, dcm capable */
	{ 2, 3, 4, 0 }, /* MCS  2: MOD: QPSK,   CR 3/4, NOT dcm capable */
	{ 4, 1, 2, 1 }, /* MCS  3: MOD: 16QAM,  CR 1/2, dcm capable */
	{ 4, 3, 4, 1 }, /* MCS  4: MOD: 16QAM,  CR 3/4, dcm capable */
	{ 6, 2, 3, 0 }, /* MCS  5: MOD: 64QAM,  CR 2/3, NOT dcm capable */
	{ 6, 3, 4, 0 }, /* MCS  6: MOD: 64QAM,  CR 3/4, NOT dcm capable */
	{ 6, 5, 6, 0 }, /* MCS  7: MOD: 64QAM,  CR 5/6, NOT dcm capable */
	{ 8, 3, 4, 0 }, /* MCS  8: MOD: 256QAM, CR 3/4, NOT dcm capable */
	{ 8, 5, 6, 0 }, /* MCS  9: MOD: 256QAM, CR 5/6, NOT dcm capable */
	{ 10, 3, 4, 0 }, /* MCS 10: MOD: 1024QAM, CR 3/4, NOT dcm capable */
	{ 10, 5, 6, 0 }  /* MCS 11: MOD: 1024QAM, CR 5/6, NOT dcm capable */
};

/* Nsd values Draft0.4 Table 26.63 onwards */
static const uint wlc_he_nsd[] = {
	234,	/* BW20 */
	468,	/* BW40 */
	980,	/* BW80 */
	1960	/* BW160 */
};

/* sym_len = 12.8 us. For calculation purpose, *10 */
#define HE_SYM_LEN_FACTOR		(128)

/* GI values = 0.8 , 1.6 or 3.2 us. For calculation purpose, *10 */
#define HE_GI_800us_FACTOR		(8)
#define HE_GI_1600us_FACTOR		(16)
#define HE_GI_3200us_FACTOR		(32)

#define HE_BW_TO_NSD(bwi) \
	((bwi) > 0 && ((bwi) << WL_RSPEC_BW_SHIFT) <= WL_RSPEC_BW_160MHZ) ? \
	wlc_he_nsd[(bwi)-1] : 0;

#define HE_IS_GI_0_8us(gi)	(((gi) == WL_RSPEC_HE_1x_LTF_GI_0_8us) || \
				((gi) == WL_RSPEC_HE_2x_LTF_GI_0_8us))
#define HE_IS_GI_1_6us(gi)	((gi) == WL_RSPEC_HE_2x_LTF_GI_1_6us)
#define HE_IS_GI_3_2us(gi)	((gi) == WL_RSPEC_HE_4x_LTF_GI_3_2us)

#define ksps		250 /* kilo symbols per sec, 4 us sym */
#define Nsd_20MHz	52
#define Nsd_40MHz	108
#define Nsd_80MHz	234
#define Nsd_160MHz	468

uint
wf_he_mcs_to_rate(uint mcs, uint nss, uint bw, uint gi, bool dcm)
{
	uint rate;
	uint rate_deno;

	rate = HE_BW_TO_NSD(bw >> WL_RSPEC_BW_SHIFT);

	/* Nbpscs: multiply by bits per number from the constellation in use */
	rate = rate * wlc_mcs_info[mcs].constellation_bits;

	/* Nss: adjust for the number of spatial streams */
	rate = rate * nss;

	/* R: adjust for the coding rate given as a quotient and divisor */
	rate = (rate * wlc_mcs_info[mcs].coding_q) / wlc_mcs_info[mcs].coding_d;

	/* take care of dcm: dcm divides R by 2. If not dcm mcs, ignore */
	if (dcm) {
		if (wlc_mcs_info[mcs].dcm_capable) {
			rate >>= 1;
		}
	}

	/* add sym len factor */
	rate_deno = HE_SYM_LEN_FACTOR;

	/* get GI for denominator */
	if (HE_IS_GI_3_2us(gi)) {
		rate_deno += HE_GI_3200us_FACTOR;
	} else if (HE_IS_GI_1_6us(gi)) {
		rate_deno += HE_GI_1600us_FACTOR;
	} else {
		/* assuming HE_GI_0_8us */
		rate_deno += HE_GI_800us_FACTOR;
	}

	/* as per above formula */
	rate *= 1000;	/* factor of 10. *100 to accommodate 2 places */
	rate /= rate_deno;
	rate *= 10; /* *100 was already done above. Splitting is done to avoid overflow. */

	return rate;
}

uint
wf_mcs_to_Ndbps(uint mcs, uint nss, uint bw)
{
	uint Nsd;
	uint Ndbps;

	/* This calculation works for 11n HT and 11ac VHT if the HT mcs values
	 * are decomposed into a base MCS = MCS % 8, and Nss = 1 + MCS / 8.
	 * That is, HT MCS 23 is a base MCS = 7, Nss = 3
	 */

	/* find the number of complex numbers per symbol */
	if (bw == WL_RSPEC_BW_20MHZ) {
		Nsd = Nsd_20MHz;
	} else if (bw == WL_RSPEC_BW_40MHZ) {
		Nsd = Nsd_40MHz;
	} else if (bw == WL_RSPEC_BW_80MHZ) {
		Nsd = Nsd_80MHz;
	} else if (bw == WL_RSPEC_BW_160MHZ) {
		Nsd = Nsd_160MHz;
	} else {
		Nsd = 0;
	}

	/* multiply number of spatial streams,
	 * bits per number from the constellation,
	 * and coding quotient
	 */
	Ndbps = Nsd * nss *
		wlc_mcs_info[mcs].coding_q * wlc_mcs_info[mcs].constellation_bits;

	/* adjust for the coding rate divisor */
	Ndbps = Ndbps / wlc_mcs_info[mcs].coding_d;

	return Ndbps;
}

/**
 * Returns the rate in [Kbps] units for a caller supplied MCS/bandwidth/Nss/Sgi combination.
 *     'mcs' : a *single* spatial stream MCS (11n or 11ac)
 */
uint
wf_mcs_to_rate(uint mcs, uint nss, uint bw, int sgi)
{
	uint rate;

	if (mcs == 32) {
		/* just return fixed values for mcs32 instead of trying to parametrize */
		rate = (sgi == 0) ? 6000 : 6778;
	} else if ((mcs <= WLC_MAX_VHT_MCS) || IS_PROPRIETARY_11N_SS_MCS(mcs) ||
		(mcs <= WLC_MAX_HE_MCS)) {
		/* This calculation works for 11n HT, 11ac VHT and 11ax HE if the HT mcs values
		 * are decomposed into a base MCS = MCS % 8, and Nss = 1 + MCS / 8.
		 * That is, HT MCS 23 is a base MCS = 7, Nss = 3
		 */

#if defined(WLPROPRIETARY_11N_RATES)
			switch (mcs) {
				case 87:
					mcs = 8; /* MCS  8: MOD: 256QAM, CR 3/4 */
					break;
				case 88:
					mcs = 9; /* MCS  9: MOD: 256QAM, CR 5/6 */
					break;
			default:
				break;
		}
#endif /* WLPROPRIETARY_11N_RATES */

		/* find the number of complex numbers per symbol */
		if (RSPEC_IS20MHZ(bw)) {
			rate = Nsd_20MHz;
		} else if (RSPEC_IS40MHZ(bw)) {
			rate = Nsd_40MHz;
		} else if (bw == WL_RSPEC_BW_80MHZ) {
			rate = Nsd_80MHz;
		} else if (bw == WL_RSPEC_BW_160MHZ) {
			rate = Nsd_160MHz;
		} else {
			rate = 0;
		}

		/* multiply by bits per number from the constellation in use */
		rate = rate * wlc_mcs_info[mcs].constellation_bits;

		/* adjust for the number of spatial streams */
		rate = rate * nss;

		/* adjust for the coding rate given as a quotient and divisor */
		rate = (rate * wlc_mcs_info[mcs].coding_q) / wlc_mcs_info[mcs].coding_d;

		/* multiply by Kilo symbols per sec to get Kbps */
		rate = rate * ksps;

		/* adjust the symbols per sec for SGI
		 * symbol duration is 4 us without SGI, and 3.6 us with SGI,
		 * so ratio is 10 / 9
		 */
		if (sgi) {
			/* add 4 for rounding of division by 9 */
			rate = ((rate * 10) + 4) / 9;
		}
	} else {
		rate = 0;
	}

	return rate;
} /* wf_mcs_to_rate */

uint8
wf_vht_plcp_to_rate(uint8 *plcp)
{
	uint8 rate, gid;
	uint nss;
	uint32 plcp0 = plcp[0] + (plcp[1] << 8); /* don't need plcp[2] */

	gid = (plcp0 & VHT_SIGA1_GID_MASK) >> VHT_SIGA1_GID_SHIFT;
	if (gid > VHT_SIGA1_GID_TO_AP && gid < VHT_SIGA1_GID_NOT_TO_AP) {
		/* for MU packet we hacked Signal Tail field in VHT-SIG-A2 to save nss and mcs,
		 * copy from murate in d11 rx header.
		 * nss = bit 18:19 (for 11ac 2 bits to indicate maximum 4 nss)
		 * mcs = 20:23
		 */
		rate = (plcp[5] & 0xF0) >> 4;
		nss = ((plcp[5] & 0x0C) >> 2) + 1;
	} else {
		rate = (plcp[3] >> VHT_SIGA2_MCS_SHIFT);
		nss = ((plcp0 & VHT_SIGA1_NSTS_SHIFT_MASK_USER0) >>
			VHT_SIGA1_NSTS_SHIFT) + 1;
		if (plcp0 & VHT_SIGA1_STBC)
			nss = nss >> 1;
	}
	rate |= ((nss << WL_RSPEC_VHT_NSS_SHIFT) | 0x80);

	return rate;
}

/* ============================================ */
/* ============================================ */

/**
 * Some functions require a single stream MCS as an input parameter. Given an MCS, this function
 * returns the single spatial stream MCS equivalent.
 */
uint8
wf_get_single_stream_mcs(uint mcs)
{
	if (mcs < 32)
		return mcs % 8;
	switch (mcs) {
		case 32:
			return 32;
		case 87:
		case 99:
		case 101:
			return 87;	/* MCS 87: SS 1, MOD: 256QAM, CR 3/4 */
		default:
			return 88;	/* MCS 88: SS 1, MOD: 256QAM, CR 5/6 */
	}
}

/* ============================================ */
/* ============================================ */

const uint8 plcp_ofdm_rate_tbl[] = {
	DOT11_RATE_48M, /* 8: 48Mbps */
	DOT11_RATE_24M, /* 9: 24Mbps */
	DOT11_RATE_12M, /* A: 12Mbps */
	DOT11_RATE_6M,  /* B:  6Mbps */
	DOT11_RATE_54M, /* C: 54Mbps */
	DOT11_RATE_36M, /* D: 36Mbps */
	DOT11_RATE_18M, /* E: 18Mbps */
	DOT11_RATE_9M   /* F:  9Mbps */
};
