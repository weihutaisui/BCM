/*
 * Common OS-independent driver header for rate management.
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
 * $Id: bcmwifi_rspec.h 675387 2016-12-15 13:43:55Z $
 */

#ifndef _bcmwifi_rspec_h_
#define _bcmwifi_rspec_h_

#include <typedefs.h>

/**
 * ===================================================================================
 * rate spec : holds rate and mode specific information required to generate a tx frame.
 * Legacy CCK and OFDM information is held in the same manner as was done in the past.
 * (in the lower byte) the upper 3 bytes primarily hold MIMO specific information
 * ===================================================================================
 */
typedef uint32	ratespec_t;

/* Rate spec. definitions */
#define WL_RSPEC_RATE_MASK	0x000000FF	/**< Legacy rate or MCS or MCS + NSS */
#define WL_RSPEC_TXEXP_MASK	0x00000300	/**< Tx chain expansion beyond Nsts */
#define WL_RSPEC_TXEXP_SHIFT	8
#define WL_RSPEC_BW_MASK	0x00070000	/**< Band width */
#define WL_RSPEC_BW_SHIFT	16
#define WL_RSPEC_DCM		0x00080000	/**< Dual Carrier Modulation */
#define WL_RSPEC_STBC		0x00100000	/**< STBC expansion, Nsts = 2 * Nss */
#define WL_RSPEC_TXBF		0x00200000
#define WL_RSPEC_LDPC		0x00400000
#define WL_RSPEC_SGI		0x00800000
#define WL_RSPEC_SHORT_PREAMBLE	0x00800000	/**< DSSS short preable - Encoding 0 */
#define WL_RSPEC_ENCODING_MASK	0x03000000	/**< Encoding of RSPEC_RATE field */
#define WL_RSPEC_ENCODING_SHIFT	24

/**< HE GI extension: inerpretation in HE is
 * WL_RSPEC_HE_GI_EXTN	WL_RSPEC_SGI		LTF + GI value
 * 0			0			1x HE-LTF and 0.8 us GI
 * 0			1			2x HE-LTF and 0.8 us GI
 * 1			0			2x HE-LTF and 1.6 us GI
 * 1			1			4x HE-LTF and 3.2 us GI
 */
#define WL_RSPEC_HE_GI_EXTN	0x08000000
#define WL_RSPEC_OVERRIDE_RATE	0x40000000	/**< override rate only */
#define WL_RSPEC_OVERRIDE_MODE	0x80000000	/**< override both rate & mode */

/* ======== RSPEC_HE_GI|RSPEC_SGI fields for HE ======== */

/* GI for HE */
#define RSPEC_HE_LTF_GI(rspec)	((((rspec) & WL_RSPEC_SGI) ? 1 : 0) | \
				 (((rspec) & WL_RSPEC_HE_GI_EXTN) ? 2 : 0))

#define WL_RSPEC_HE_1x_LTF_GI_0_8us	(0x0)
#define WL_RSPEC_HE_2x_LTF_GI_0_8us	(0x1)
#define WL_RSPEC_HE_2x_LTF_GI_1_6us	(0x2)
#define WL_RSPEC_HE_4x_LTF_GI_3_2us	(0x3)

/* ======== RSPEC_RATE field ======== */

/* Encoding 0 - legacy rate */
/* DSSS, CCK, and OFDM rates in [500kbps] units */
#define WL_RSPEC_LEGACY_RATE_MASK	0x0000007F
#define WLC_RATE_1M	2
#define WLC_RATE_2M	4
#define WLC_RATE_5M5	11
#define WLC_RATE_11M	22
#define WLC_RATE_6M	12
#define WLC_RATE_9M	18
#define WLC_RATE_12M	24
#define WLC_RATE_18M	36
#define WLC_RATE_24M	48
#define WLC_RATE_36M	72
#define WLC_RATE_48M	96
#define WLC_RATE_54M	108

/* Encoding 1 - HT MCS */
#define WL_RSPEC_HT_MCS_MASK		0x0000007F	/**< HT MCS value mask in rspec */

/* Encoding 2 - VHT MCS + NSS */
#define WL_RSPEC_VHT_MCS_MASK		0x0000000F	/**< VHT MCS value mask in rspec */
#define WL_RSPEC_VHT_NSS_MASK		0x000000F0	/**< VHT Nss value mask in rspec */
#define WL_RSPEC_VHT_NSS_SHIFT		4		/**< VHT Nss value shift in rspec */

/* Encoding 3 - HE MCS + NSS */
#define WL_RSPEC_HE_MCS_MASK		0x0000000F	/**< HE MCS value mask in rspec */
#define WL_RSPEC_HE_NSS_MASK		0x000000F0	/**< HE Nss value mask in rspec */
#define WL_RSPEC_HE_NSS_SHIFT		4		/**< HE Nss value shift in rpsec */

/* ======== RSPEC_BW field ======== */

#define WL_RSPEC_BW_UNSPECIFIED	0
#define WL_RSPEC_BW_20MHZ	0x00010000
#define WL_RSPEC_BW_40MHZ	0x00020000
#define WL_RSPEC_BW_80MHZ	0x00030000
#define WL_RSPEC_BW_160MHZ	0x00040000

/* ======== RSPEC_ENCODING field ======== */

#define WL_RSPEC_ENCODE_RATE	0x00000000	/**< Legacy rate is stored in RSPEC_RATE */
#define WL_RSPEC_ENCODE_HT	0x01000000	/**< HT MCS is stored in RSPEC_RATE */
#define WL_RSPEC_ENCODE_VHT	0x02000000	/**< VHT MCS and NSS are stored in RSPEC_RATE */
#define WL_RSPEC_ENCODE_HE	0x03000000	/**< HE MCS and NSS are stored in RSPEC_RATE */

/**
 * ===============================
 * Handy macros to parse rate spec
 * ===============================
 */
#define RSPEC_BW(rspec)		((rspec) & WL_RSPEC_BW_MASK)
#define RSPEC_IS20MHZ(rspec)	(RSPEC_BW(rspec) == WL_RSPEC_BW_20MHZ)
#define RSPEC_IS40MHZ(rspec)	(RSPEC_BW(rspec) == WL_RSPEC_BW_40MHZ)
#define RSPEC_IS80MHZ(rspec)	(RSPEC_BW(rspec) == WL_RSPEC_BW_80MHZ)
#define RSPEC_IS160MHZ(rspec)	(RSPEC_BW(rspec) == WL_RSPEC_BW_160MHZ)

#define RSPEC_ISSGI(rspec)	(((rspec) & WL_RSPEC_SGI) != 0)
#define RSPEC_ISLDPC(rspec)	(((rspec) & WL_RSPEC_LDPC) != 0)
#define RSPEC_ISSTBC(rspec)	(((rspec) & WL_RSPEC_STBC) != 0)
#define RSPEC_ISTXBF(rspec)	(((rspec) & WL_RSPEC_TXBF) != 0)

#define RSPEC_TXEXP(rspec)	(((rspec) & WL_RSPEC_TXEXP_MASK) >> WL_RSPEC_TXEXP_SHIFT)

#define RSPEC_ENCODE(rspec)	(((rspec) & WL_RSPEC_ENCODING_MASK) >> WL_RSPEC_ENCODING_SHIFT)
#ifdef WL11N
#define RSPEC_ISLEGACY(rspec)	(((rspec) & WL_RSPEC_ENCODING_MASK) == WL_RSPEC_ENCODE_RATE)
#else /* WL11N */
#define RSPEC_ISLEGACY(rspec)	1
#endif /* WL11N */
#define	RSPEC_ISCCK(rspec)	(RSPEC_ISLEGACY(rspec) && \
				 (int8)rate_info[(rspec) & WL_RSPEC_LEGACY_RATE_MASK] > 0)
#define	RSPEC_ISOFDM(rspec)	(RSPEC_ISLEGACY(rspec) && \
				 (int8)rate_info[(rspec) & WL_RSPEC_LEGACY_RATE_MASK] < 0)
#ifdef WL11N
#define RSPEC_ISHT(rspec)	(((rspec) & WL_RSPEC_ENCODING_MASK) == WL_RSPEC_ENCODE_HT)
#else /* WL11N */
#define RSPEC_ISHT(rspec)	0
#endif /* WL11N */
#ifdef WL11AC
#define RSPEC_ISVHT(rspec)	(((rspec) & WL_RSPEC_ENCODING_MASK) == WL_RSPEC_ENCODE_VHT)
#else /* WL11AC */
#define RSPEC_ISVHT(rspec)	0
#endif /* WL11AC */
#ifdef WL11AX
#define RSPEC_ISHE(rspec)	(((rspec) & WL_RSPEC_ENCODING_MASK) == WL_RSPEC_ENCODE_HE)
#else /* WL11AX */
#define RSPEC_ISHE(rspec)	0
#endif /* WL11AX */

/**
 * ================================
 * Handy macros to create rate spec
 * ================================
 */
/* create ratespecs */
#define LEGACY_RSPEC(rate)	(WL_RSPEC_ENCODE_RATE | WL_RSPEC_BW_20MHZ | \
				 ((rate) & WL_RSPEC_LEGACY_RATE_MASK))
#define CCK_RSPEC(cck)		LEGACY_RSPEC(cck)
#define OFDM_RSPEC(ofdm)	LEGACY_RSPEC(ofdm)
#define HT_RSPEC(mcs)		(WL_RSPEC_ENCODE_HT | ((mcs) & WL_RSPEC_HT_MCS_MASK))
#define VHT_RSPEC(mcs, nss)	(WL_RSPEC_ENCODE_VHT | \
				 (((nss) << WL_RSPEC_VHT_NSS_SHIFT) & WL_RSPEC_VHT_NSS_MASK) | \
				 ((mcs) & WL_RSPEC_VHT_MCS_MASK))
#define HE_RSPEC(mcs, nss)	(WL_RSPEC_ENCODE_HE | \
				 (((nss) << WL_RSPEC_HE_NSS_SHIFT) & WL_RSPEC_HE_NSS_MASK) | \
				 ((mcs) & WL_RSPEC_HE_MCS_MASK))

/**
 * ==================
 * Other handy macros
 * ==================
 */
/* return rate in unit of Kbps */
#ifdef WL11N
#define RSPEC2KBPS(rspec)	wf_rspec_to_rate(rspec)
#else
#define RSPEC2KBPS(rspec)	(((rspec) & WL_RSPEC_LEGACY_RATE_MASK) * 500)
#endif // endif

/* return rate in unit of 500Kbps */
#ifdef BCMDBG
#define RSPEC2RATE(rspec)	wf_rspec_to_rate_legacy(rspec)
#else
#define RSPEC2RATE(rspec)	((rspec) & WL_RSPEC_LEGACY_RATE_MASK)
#endif // endif

/**
 * =================================
 * Macros to use the rate_info table
 * =================================
 */
/* phy_rate table index is in [500kbps] units */
#define WLC_MAXRATE	108	/**< in 500kbps units */
extern const uint8 rate_info[];
/* phy_rate table value is encoded */
#define	RATE_INFO_OFDM_MASK	0x80	/* ofdm mask */
#define	RATE_INFO_RATE_MASK	0x7f	/* rate signal index mask */
#define	RATE_INFO_M_RATE_MASK	0x0f	/* M_RATE_TABLE index mask */
#define	RATE_INFO_RATE_ISCCK(r)	((r) <= WLC_MAXRATE && (int8)rate_info[r] > 0)
#define	RATE_INFO_RATE_ISOFDM(r) ((r) <= WLC_MAXRATE && (int8)rate_info[r] < 0)

/**
 * ===================
 * function prototypes
 * ===================
 */
ratespec_t wf_vht_plcp_to_rspec(uint8 *plcp);
ratespec_t wf_he_plcp_to_rspec(uint8 *plcp);
#ifdef BCMDBG
uint wf_rspec_to_rate_legacy(ratespec_t rspec);
#endif // endif
uint wf_rspec_to_rate(ratespec_t rspec);

#endif /* _bcmwifi_rspec_h_ */
