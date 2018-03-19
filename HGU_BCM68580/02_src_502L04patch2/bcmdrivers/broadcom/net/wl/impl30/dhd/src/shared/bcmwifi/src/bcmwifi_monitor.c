/*
 * Monitor Mode routines.
 * This header file housing the Monitor Mode routines implementation.
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
 * $Id: bcmwifi_monitor.c 512698 2016-02-11 13:12:15Z $
 */

#ifndef WL11N
#define WL11N
#endif // endif
#ifndef WL11AC
#define WL11AC
#endif // endif

#include <bcmutils.h>
#include <bcmendian.h>
#include <bcmwifi_channels.h>
#include <proto/monitor.h>
#include <bcmwifi_radiotap.h>
#include <bcmwifi_monitor.h>
#include <bcmwifi_rates.h>
#include <d11.h>

/* channel bandwidth */
#define WLC_10_MHZ	10	/**< 10Mhz channel bandwidth */
#define WLC_20_MHZ	20	/**< 20Mhz channel bandwidth */
#define WLC_40_MHZ	40	/**< 40Mhz channel bandwidth */
#define WLC_80_MHZ	80	/**< 80Mhz channel bandwidth */
#define WLC_160_MHZ	160	/**< 160Mhz channel bandwidth */

int i = 0;
/** Calculate the rate of a received frame and return it as a ratespec (monitor mode) */
static ratespec_t BCMFASTPATH
wlc_recv_mon_compute_rspec(wlc_d11rxhdr_t *wrxh, uint8 *plcp)
{
	d11rxhdr_t *rxh = &wrxh->rxhdr;
	ratespec_t rspec;
	uint16 phy_ft;

	phy_ft = rxh->PhyRxStatus_0 & PRXS0_FT_MASK;

	switch (phy_ft) {
		case PRXS0_CCK:
			rspec = CCK_RSPEC(CCK_PHY2MAC_RATE(((cck_phy_hdr_t *)plcp)->signal));
			rspec |= WL_RSPEC_BW_20MHZ;
			break;
		case PRXS0_OFDM:
			rspec = OFDM_RSPEC(OFDM_PHY2MAC_RATE(((ofdm_phy_hdr_t *)plcp)->rlpt[0]));
			rspec |= WL_RSPEC_BW_20MHZ;
			break;
		case PRXS0_PREN: {
			uint ht_sig1, ht_sig2;
			uint8 stbc;

			ht_sig1 = plcp[0];	/* only interested in low 8 bits */
			ht_sig2 = plcp[3] | (plcp[4] << 8); /* only interested in low 10 bits */

			rspec = HT_RSPEC((ht_sig1 & HT_SIG1_MCS_MASK));
			if (ht_sig1 & HT_SIG1_CBW) {
				/* indicate rspec is for 40 MHz mode */
				rspec |= WL_RSPEC_BW_40MHZ;
			} else {
				/* indicate rspec is for 20 MHz mode */
				rspec |= WL_RSPEC_BW_20MHZ;
			}
			if (ht_sig2 & HT_SIG2_SHORT_GI)
				rspec |= WL_RSPEC_SGI;
			if (ht_sig2 & HT_SIG2_FEC_CODING)
				rspec |= WL_RSPEC_LDPC;
			stbc = ((ht_sig2 & HT_SIG2_STBC_MASK) >> HT_SIG2_STBC_SHIFT);
			if (stbc != 0) {
				rspec |= WL_RSPEC_STBC;
			}
			break;
		}
		case PRXS0_STDN:
			rspec = wf_vht_plcp_to_rspec(plcp);
			break;
		default:
			/* return a valid rspec if not a debug/assert build */
			rspec = OFDM_RSPEC(6) | WL_RSPEC_BW_20MHZ;
			break;
	}

	return rspec;
} /* wlc_recv_compute_rspec */

/* recover 32bit TSF value from the 16bit TSF value */
/* assumption is time in rxh is within 65ms of the current tsf */
/* local TSF inserted in the rxh is at RxStart which is before 802.11 header */
static uint32
wlc_recover_tsf32(uint16 rxh_tsf, uint32 ts_tsf)
{
	uint16 rfdly;

	/* adjust rx dly added in RxTSFTime */

	/* TODO: add PHY type specific value here... */
	rfdly = M_BPHY_PLCPRX_DLY;

	rxh_tsf -= rfdly;

	return (((ts_tsf - rxh_tsf) & 0xFFFF0000) | rxh_tsf);
}

static uint8
wlc_vht_get_gid(uint8 *plcp)
{
	uint32 plcp0 = plcp[0] | (plcp[1] << 8);
	return (plcp0 & VHT_SIGA1_GID_MASK) >> VHT_SIGA1_GID_SHIFT;
}

static uint16
wlc_vht_get_aid(uint8 *plcp)
{
	uint32 plcp0 = plcp[0] | (plcp[1] << 8) | (plcp[2] << 16);
	return (plcp0 & VHT_SIGA1_PARTIAL_AID_MASK) >> VHT_SIGA1_PARTIAL_AID_SHIFT;
}

static bool
wlc_vht_get_txop_ps_not_allowed(uint8 *plcp)
{
	return !!(plcp[2] & (VHT_SIGA1_TXOP_PS_NOT_ALLOWED >> 16));
}

static bool
wlc_vht_get_sgi_nsym_da(uint8 *plcp)
{
	return !!(plcp[3] & VHT_SIGA2_GI_W_MOD10);
}

static bool
wlc_vht_get_ldpc_extra_symbol(uint8 *plcp)
{
	return !!(plcp[3] & VHT_SIGA2_LDPC_EXTRA_OFDM_SYM);
}

static bool
wlc_vht_get_beamformed(uint8 *plcp)
{
	return !!(plcp[4] & (VHT_SIGA2_BEAMFORM_ENABLE >> 8));
}
/* Convert htflags and mcs values to
* rate in units of 500kbps
*/
static uint16
wlc_ht_phy_get_rate(uint8 htflags, uint8 mcs)
{

	ratespec_t rspec = HT_RSPEC(mcs);

	if (htflags & WL_RXS_HTF_40)
		rspec |= WL_RSPEC_BW_40MHZ;

	if (htflags & WL_RXS_HTF_SGI)
		rspec |= WL_RSPEC_SGI;

	return RSPEC2KBPS(rspec)/500;
}

uint16
wl_rxsts_to_rtap(struct wl_rxsts* rxsts, void *payload, uint16 len, void *pout, int16 *offset)
{
	uint16 rtap_len = 0;
	struct dot11_header* mac_header;
	uint8* p = payload;
	void *radio_tap;
	ASSERT(p && rxsts);

	if (rxsts->phytype == WL_RXS_PHY_N) {
		if (rxsts->encoding == WL_RXS_ENCODING_HT) {
			rtap_len = sizeof(wl_radiotap_ht_t);
		}
		else if (rxsts->encoding == WL_RXS_ENCODING_VHT) {
			rtap_len = sizeof(wl_radiotap_vht_t);
		}
		else {
			rtap_len = sizeof(wl_radiotap_legacy_t);
		}
	} else {
		rtap_len = sizeof(wl_radiotap_legacy_t);
	}
	radio_tap = pout - rtap_len;
	mac_header = (struct dot11_header *)(p);
	len += rtap_len;

	if ((rxsts->phytype != WL_RXS_PHY_N) ||
		((rxsts->encoding != WL_RXS_ENCODING_HT) &&
		(rxsts->encoding != WL_RXS_ENCODING_VHT))) {
		wl_radiotap_rx_legacy(mac_header, rxsts,
			(wl_radiotap_legacy_t *)radio_tap);
	}
	else if (rxsts->encoding == WL_RXS_ENCODING_VHT) {
		wl_radiotap_rx_vht(mac_header, rxsts,
			(wl_radiotap_vht_t *)radio_tap);
	}
	else if (rxsts->encoding == WL_RXS_ENCODING_HT) {
		wl_radiotap_rx_ht(mac_header, rxsts,
			(wl_radiotap_ht_t *)radio_tap);
	}
	*offset = rtap_len;
	return len;
}

/* Convert RX hardware status to standard format and send to wl_monitor
 * assume p points to plcp header
 */
static uint16
wl_d11rx_to_rxsts(monitor_info_t* info, monitor_pkt_info_t* pkt_info,
	wlc_d11rxhdr_t *wrxh, void *pkt, uint16 len, void *pout, int16 *offset)
{
	struct wl_rxsts sts;
	uint32 rx_tsf_l;
	ratespec_t rspec;
	uint16 chan_num;
	uint8 *plcp;
	uint8 *p = (uint8*)pkt;
	uint8 hwrxoff = 0;
	uint32 ts_tsf = 0;
	int8 rssi = 0;
	struct dot11_header *h;
	uint16 subtype, pad_present = 0;

	ASSERT(p);
	ASSERT(info);
	BCM_REFERENCE(chan_num);

	rssi = (pkt_info->marker >> 8) & 0xff;
	hwrxoff = (pkt_info->marker >> 16) & 0xff;

	plcp = (uint8*)p + hwrxoff;

	if (wrxh->rxhdr.RxStatus1 & RXS_PBPRES) {
		plcp += 2;
		pad_present = 1;
	}

	ts_tsf = pkt_info->ts.ts_low;
	rx_tsf_l = wlc_recover_tsf32(wrxh->rxhdr.RxTSFTime, ts_tsf);

	bzero((void *)&sts, sizeof(wl_rxsts_t));

	sts.mactime = rx_tsf_l;
	sts.antenna = (wrxh->rxhdr.PhyRxStatus_0 & PRXS0_RXANT_UPSUBBAND) ? 1 : 0;
	sts.signal = rssi;
	sts.noise = (int8)pkt_info->marker;
	sts.chanspec = wrxh->rxhdr.RxChan;

	if (wf_chspec_malformed(sts.chanspec))
	{
		return 0;
	}

	chan_num = CHSPEC_CHANNEL(sts.chanspec);
	rspec = wlc_recv_mon_compute_rspec(wrxh, plcp);

	h = (struct dot11_header *)(plcp + D11_PHY_HDR_LEN);
	subtype = (ltoh16(h->fc) & FC_SUBTYPE_MASK) >> FC_SUBTYPE_SHIFT;

	if ((subtype == FC_SUBTYPE_QOS_DATA) || (subtype == FC_SUBTYPE_QOS_NULL)) {
		/* A-MPDU parsing */
		if ((wrxh->rxhdr.PhyRxStatus_0 & PRXS0_FT_MASK) == PRXS0_PREN) {
			if (WLC_IS_MIMO_PLCP_AMPDU(plcp)) {
				sts.nfrmtype |= WL_RXS_NFRM_AMPDU_FIRST;
				/* Save the rspec for later */
				info->ampdu_rspec = rspec;
			} else if (!(plcp[0] | plcp[1] | plcp[2])) {
				sts.nfrmtype |= WL_RXS_NFRM_AMPDU_SUB;
				/* Use the saved rspec */
				rspec = info->ampdu_rspec;
			}
		}
		else if ((wrxh->rxhdr.PhyRxStatus_0 & PRXS0_FT_MASK) == FT_VHT) {
			if ((plcp[0] | plcp[1] | plcp[2]) &&
				!(wrxh->rxhdr.RxStatus2 & RXS_PHYRXST_VALID)) {
				/* First MPDU:
				 * PLCP header is valid, Phy RxStatus is not valid
				 */
				sts.nfrmtype |= WL_RXS_NFRM_AMPDU_FIRST;
				/* Save the rspec for later */
				info->ampdu_rspec = rspec;
				info->ampdu_counter++;
			} else if (!(plcp[0] | plcp[1] | plcp[2]) &&
				!(wrxh->rxhdr.RxStatus2 & RXS_PHYRXST_VALID)) {
				/* Sub MPDU:
				 * PLCP header is not valid, Phy RxStatus is not valid
				 */
				sts.nfrmtype |= WL_RXS_NFRM_AMPDU_SUB;
				/* Use the saved rspec */
				rspec = info->ampdu_rspec;
			} else if ((plcp[0] | plcp[1] | plcp[2]) &&
				(wrxh->rxhdr.RxStatus2 & RXS_PHYRXST_VALID)) {
				/* MPDU is not a part of A-MPDU:
				 * PLCP header is valid and Phy RxStatus is valid
				 */
				info->ampdu_counter++;
			} else {
				/* Last MPDU */
				rspec = info->ampdu_rspec;
			}

			sts.ampdu_counter = info->ampdu_counter;
		}
	}
	/* A-MSDU parsing */
	if (wrxh->rxhdr.RxStatus2 & RXS_AMSDU_MASK) {
		/* it's chained buffer, break it if necessary */
		sts.nfrmtype |= WL_RXS_NFRM_AMSDU_FIRST | WL_RXS_NFRM_AMSDU_SUB;
	}

	if (PRXS5_ACPHY_DYNBWINNONHT(&wrxh->rxhdr))
		sts.vhtflags |= WL_RXS_VHTF_DYN_BW_NONHT;
	else
		sts.vhtflags &= ~WL_RXS_VHTF_DYN_BW_NONHT;

	switch (PRXS5_ACPHY_CHBWINNONHT(&wrxh->rxhdr)) {
	default: case PRXS5_ACPHY_CHBWINNONHT_20MHZ:
		sts.bw_nonht = WLC_20_MHZ;
		break;
	case PRXS5_ACPHY_CHBWINNONHT_40MHZ:
		sts.bw_nonht = WLC_40_MHZ;
		break;
	case PRXS5_ACPHY_CHBWINNONHT_80MHZ:
		sts.bw_nonht = WLC_80_MHZ;
		break;
	case PRXS5_ACPHY_CHBWINNONHT_160MHZ:
		sts.bw_nonht = WLC_160_MHZ;
		break;
	}

	/* prepare rate/modulation info */
	if (RSPEC_ISVHT(rspec)) {
		uint32 bw = RSPEC_BW(rspec);
		/* prepare VHT rate/modulation info */
		sts.nss = (rspec & WL_RSPEC_VHT_NSS_MASK) >> WL_RSPEC_VHT_NSS_SHIFT;
		sts.mcs = (rspec & WL_RSPEC_VHT_MCS_MASK);

		if (CHSPEC_IS80(sts.chanspec)) {
			if (bw == WL_RSPEC_BW_20MHZ) {
				switch (CHSPEC_CTL_SB(sts.chanspec)) {
					default:
					case WL_CHANSPEC_CTL_SB_LL:
						sts.bw = WL_RXS_VHT_BW_20LL;
						break;
					case WL_CHANSPEC_CTL_SB_LU:
						sts.bw = WL_RXS_VHT_BW_20LU;
						break;
					case WL_CHANSPEC_CTL_SB_UL:
						sts.bw = WL_RXS_VHT_BW_20UL;
						break;
					case WL_CHANSPEC_CTL_SB_UU:
						sts.bw = WL_RXS_VHT_BW_20UU;
						break;
				}
			} else if (bw == WL_RSPEC_BW_40MHZ) {
				switch (CHSPEC_CTL_SB(sts.chanspec)) {
					default:
					case WL_CHANSPEC_CTL_SB_L:
						sts.bw = WL_RXS_VHT_BW_40L;
						break;
					case WL_CHANSPEC_CTL_SB_U:
						sts.bw = WL_RXS_VHT_BW_40U;
						break;
				}
			} else {
				sts.bw = WL_RXS_VHT_BW_80;
			}
		} else if (CHSPEC_IS40(sts.chanspec)) {
			if (bw == WL_RSPEC_BW_20MHZ) {
				switch (CHSPEC_CTL_SB(sts.chanspec)) {
					default:
					case WL_CHANSPEC_CTL_SB_L:
						sts.bw = WL_RXS_VHT_BW_20L;
						break;
					case WL_CHANSPEC_CTL_SB_U:
						sts.bw = WL_RXS_VHT_BW_20U;
						break;
				}
			} else if (bw == WL_RSPEC_BW_40MHZ) {
				sts.bw = WL_RXS_VHT_BW_40;
			}
		} else {
			sts.bw = WL_RXS_VHT_BW_20;
		}

		if (RSPEC_ISSTBC(rspec))
			sts.vhtflags |= WL_RXS_VHTF_STBC;
		if (wlc_vht_get_txop_ps_not_allowed(plcp))
			sts.vhtflags |= WL_RXS_VHTF_TXOP_PS;
		if (RSPEC_ISSGI(rspec)) {
			sts.vhtflags |= WL_RXS_VHTF_SGI;
			if (wlc_vht_get_sgi_nsym_da(plcp))
				sts.vhtflags |= WL_RXS_VHTF_SGI_NSYM_DA;
		}
		if (RSPEC_ISLDPC(rspec)) {
			sts.coding = WL_RXS_VHTF_CODING_LDCP;
			if (wlc_vht_get_ldpc_extra_symbol(plcp))
				sts.vhtflags |= WL_RXS_VHTF_LDPC_EXTRA;
		}
		if (wlc_vht_get_beamformed(plcp))
			sts.vhtflags |= WL_RXS_VHTF_BF;

		sts.gid = wlc_vht_get_gid(plcp);
		sts.aid = wlc_vht_get_aid(plcp);
		sts.datarate = RSPEC2KBPS(rspec)/500;
	} else if (RSPEC_ISHT(rspec)) {
		/* prepare HT rate/modulation info */
		sts.mcs = (rspec & WL_RSPEC_HT_MCS_MASK);

		if (CHSPEC_IS40(sts.chanspec) || CHSPEC_IS80(sts.chanspec)) {
			uint32 bw = RSPEC_BW(rspec);

			if (bw == WL_RSPEC_BW_20MHZ) {
				if (CHSPEC_CTL_SB(sts.chanspec) == WL_CHANSPEC_CTL_SB_L) {
					sts.htflags = WL_RXS_HTF_20L;
				} else {
					sts.htflags = WL_RXS_HTF_20U;
				}
			} else if (bw == WL_RSPEC_BW_40MHZ) {
				sts.htflags = WL_RXS_HTF_40;
			}
		}

		if (RSPEC_ISSGI(rspec))
			sts.htflags |= WL_RXS_HTF_SGI;
		if (RSPEC_ISLDPC(rspec))
			sts.htflags |= WL_RXS_HTF_LDPC;
		if (RSPEC_ISSTBC(rspec))
			sts.htflags |= (1 << WL_RXS_HTF_STBC_SHIFT);

		sts.datarate = wlc_ht_phy_get_rate(sts.htflags, sts.mcs);
	} else {
		/* round non-HT data rate to nearest 500bkps unit */
		sts.datarate = RSPEC2KBPS(rspec)/500;
	}

	sts.pktlength = wrxh->rxhdr.RxFrameSize - D11_PHY_HDR_LEN;
	sts.sq = ((wrxh->rxhdr.PhyRxStatus_1 & PRXS1_SQ_MASK) >> PRXS1_SQ_SHIFT);

	sts.phytype = WL_RXS_PHY_N;

	if (RSPEC_ISCCK(rspec)) {
		sts.encoding = WL_RXS_ENCODING_DSSS_CCK;
		sts.preamble = (wrxh->rxhdr.PhyRxStatus_0 & PRXS0_SHORTH) ?
			WL_RXS_PREAMBLE_SHORT : WL_RXS_PREAMBLE_LONG;
	} else if (RSPEC_ISOFDM(rspec)) {
		sts.encoding = WL_RXS_ENCODING_OFDM;
		sts.preamble = WL_RXS_PREAMBLE_SHORT;
	} else {	/* MCS rate */
		sts.encoding = WL_RXS_ENCODING_HT;
		if (RSPEC_ISVHT(rspec)) {
			sts.encoding = WL_RXS_ENCODING_VHT;
		}
		sts.preamble = (uint32)((D11HT_MMPLCPLen(&wrxh->rxhdr) != 0) ?
			WL_RXS_PREAMBLE_HT_MM : WL_RXS_PREAMBLE_HT_GF);
	}
	/* translate error code */
	if (wrxh->rxhdr.RxStatus1 & RXS_DECERR)
		sts.pkterror |= WL_RXS_DECRYPT_ERR;
	if (wrxh->rxhdr.RxStatus1 & RXS_FCSERR)
		sts.pkterror |= WL_RXS_CRC_ERROR;

	if (wrxh->rxhdr.RxStatus1 & RXS_PBPRES) {
		p += 2; len -= 2;
	}

	p += (hwrxoff + D11_PHY_HDR_LEN);
	len -= (hwrxoff + D11_PHY_HDR_LEN);
	wl_rxsts_to_rtap(&sts, p, len, pout, offset);
	*offset -= info->headroom;

	return len;
}

static uint16
wl_monitor_amsdu(monitor_info_t* info, monitor_pkt_info_t* pkt_info, wlc_d11rxhdr_t *wrxh,
	void *pkt, uint16 len, void *pout, int16* offset, uint16 *pkt_type, uint8 dma_flags)
{
	uint8 *p = pkt;
	uint8 hwrxoff = (pkt_info->marker >> 16) & 0xff;
	uint16  aggtype, pad_present;
	d11rxhdrshort_t *srxh = NULL;   /* short receive header. first and intermediate frags */
	if (dma_flags & RXS_SHORT_MASK) {
		srxh = (d11rxhdrshort_t*) &wrxh->rxhdr;
		aggtype = (srxh->mrxs & RXSS_AGGTYPE_MASK) >> RXSS_AGGTYPE_SHIFT;
	}
	else {
		aggtype = (wrxh->rxhdr.RxStatus2 & RXS_AGGTYPE_MASK) >> RXS_AGGTYPE_SHIFT;
	}

	switch (aggtype) {
	case RXS_AMSDU_FIRST:
	case RXS_AMSDU_N_ONE:
		/* Flush any previously collected */
		if (info->amsdu_len) {
			info->amsdu_len = 0;
			info->amsdu_pkt = NULL;
			*pkt_type = MON_PKT_AMSDU_ERROR;
			return 0;
		}

		/* Save the new starting AMSDU subframe */
		info->amsdu_len = len;
		info->amsdu_pkt = pkt;
		info->headroom =  D11_PHY_HDR_LEN + hwrxoff;

		if (dma_flags & RXS_SHORT_MASK) {
			srxh = (d11rxhdrshort_t*) &wrxh->rxhdr;
			pad_present = ((srxh->mrxs & RXSS_PBPRES) != 0);
		}
		else {
			pad_present = ((wrxh->rxhdr.RxStatus1 & RXS_PBPRES) != 0);
		}
		if (pad_present) {
			 info->headroom += 2;
		}
		*pkt_type = MON_PKT_AMSDU_FIRST;
		if (aggtype == RXS_AMSDU_N_ONE) {
			/* all-in-one AMSDU subframe */
			wl_d11rx_to_rxsts(info, pkt_info, wrxh, p,
					len, pout + info->headroom, offset);

			*pkt_type = MON_PKT_AMSDU_N_ONE;
			info->amsdu_len = 0;
		}
		break;

	case RXS_AMSDU_INTERMEDIATE:
	case RXS_AMSDU_LAST:
	default:
		/* Check for previously collected */
		if (info->amsdu_len) {
			/* Append next AMSDU subframe */
			p += hwrxoff;
			len -= hwrxoff;
			if (wrxh->rxhdr.RxStatus1 & RXS_PBPRES) {
				p += 2;
				len -= 2;
			}
			memcpy(info->amsdu_pkt + info->amsdu_len, p, len);
			info->amsdu_len += len;

			*pkt_type = MON_PKT_AMSDU_INTERMEDIATE;
			/* complete AMSDU frame */
			if (aggtype == RXS_AMSDU_LAST) {
				*pkt_type = MON_PKT_AMSDU_LAST;
				wl_d11rx_to_rxsts(info, pkt_info, wrxh, info->amsdu_pkt,
					info->amsdu_len, info->amsdu_pkt + info->headroom, offset);
				info->amsdu_len = 0;
				info->headroom = 0;
			}
		}
		else
		{
			info->amsdu_len = 0;
			info->amsdu_pkt = NULL;
			info->headroom = 0;
			*pkt_type = MON_PKT_AMSDU_ERROR;
			return 0;
		}
		break;
	}

	return len;
}

uint16 bcmwifi_monitor_create(monitor_info_t** info)
{
	*info = MALLOC(NULL, sizeof(struct monitor_info));
	if (!(*info))
		return FALSE;
	(*info)->amsdu_len = 0;
	return TRUE;
}

void
bcmwifi_monitor_delete(monitor_info_t* info)
{
	if (info)
		MFREE(NULL, info, sizeof(struct monitor_info));
}

uint16
bcmwifi_monitor(monitor_info_t* info, monitor_pkt_info_t* pkt_info,
	void *pkt, uint16 len, void *pout, int16* offset, uint16 *pkt_type, uint8 dma_flags)
{
	wlc_d11rxhdr_t *wrxh = (wlc_d11rxhdr_t*)pkt;
	uint16 hwrxoff, pad_present;
	if ((dma_flags & RXS_SHORT_MASK) || (wrxh->rxhdr.RxStatus2 & RXS_AMSDU_MASK)) {
		return wl_monitor_amsdu(info, pkt_info, wrxh, pkt,
				len, pout, offset, pkt_type, dma_flags);
	} else {
		if (info->amsdu_len) {
			info->amsdu_len = 0;
			info->amsdu_pkt = NULL;
			*pkt_type = MON_PKT_AMSDU_ERROR;
			return 0;
		}
		pad_present = ((wrxh->rxhdr.RxStatus1 & RXS_PBPRES) != 0);
		hwrxoff = (pkt_info->marker >> 16) & 0xff;
		info->amsdu_len = 0; /* reset amsdu */
		*pkt_type = MON_PKT_NON_AMSDU;
		pout += hwrxoff + D11_PHY_HDR_LEN;
		info->headroom = D11_PHY_HDR_LEN + hwrxoff;
		if (pad_present) {
			pout += 2;
			info->headroom += 2;
		}
		wl_d11rx_to_rxsts(info, pkt_info, wrxh, pkt, len, pout, offset);
		info->headroom = 0;
		return len;
	}
}
