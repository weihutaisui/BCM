/*
 * Monitor Mode routines.
 * This header file housing the define and function use by DHD
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
 * $Id: bcmwifi_monitor.h 512698 2016-02-11 13:12:15Z $
 */
#ifndef _BCMWIFI_MONITOR_H_
#define _BCMWIFI_MONITOR_H_

#include <proto/monitor.h>
#include <bcmwifi_radiotap.h>

#define MAX_RADIOTAP_SIZE	sizeof(wl_radiotap_vht_t)
#define MAX_MON_PKT_SIZE	(4096 + MAX_RADIOTAP_SIZE)

#define	MON_PKT_NON_AMSDU		1
#define	MON_PKT_AMSDU_FIRST		2
#define	MON_PKT_AMSDU_N_ONE		3
#define	MON_PKT_AMSDU_INTERMEDIATE	4
#define	MON_PKT_AMSDU_LAST		5
#define	MON_PKT_AMSDU_ERROR		6

typedef struct monitor_info monitor_info_t;

typedef struct rx_monitor_chain {
	uint            pkt_count;
	void            *pkthead;
	void            *pkttail;
} rx_monitor_chain_t;

struct monitor_info {
	ratespec_t		ampdu_rspec;	/* spec value for AMPDU sniffing */
	uint16			ampdu_counter;
	uint16			amsdu_len;
	uint8*			amsdu_pkt;
	int8			headroom;
	rx_monitor_chain_t	rxchain;
};

typedef struct monitor_pkt_ts {
	union {
		uint32	ts_low; /* time stamp low 32 bits */
		uint32	reserved; /* If timestamp not used */
	};
	union {
		uint32  ts_high; /* time stamp high 28 bits */
		union {
			uint32  ts_high_ext :28; /* time stamp high 28 bits */
			uint32  clk_id_ext :3; /* clock ID source  */
			uint32  phase :1; /* Phase bit */
			uint32	marker_ext;
		};
	};
} monitor_pkt_ts_t;

typedef struct monitor_pkt_info {
	uint32	marker;
	/* timestamp */
	monitor_pkt_ts_t ts;
} monitor_pkt_info_t;

extern uint16 bcmwifi_monitor_create(monitor_info_t**);
extern void bcmwifi_monitor_delete(monitor_info_t* info);
extern uint16 bcmwifi_monitor(monitor_info_t* info,
		monitor_pkt_info_t* pkt_info, void *pdata, uint16 len,
		void* pout, int16* offset, uint16 *pkt_type, uint8 dma_flags);
extern uint16 wl_rxsts_to_rtap(struct wl_rxsts* rxsts, void *pdata,
		uint16 len, void* pout, int16 *offset);
#endif /* _BCMWIFI_MONITOR_H_ */
