/* D11 macdbg function prototypes for Broadcom 802.11abgn
 * Networking Adapter Device Drivers.
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
 * $Id: dhd_macdbg.h 678038 2017-01-06 05:29:19Z $
 */

#ifndef _dhd_macdbg_h_
#define _dhd_macdbg_h_

struct si_pub;

#include <dngl_stats.h>
#include <dhd.h>

extern int dhd_macdbg_attach(dhd_pub_t *dhdp);
extern void dhd_macdbg_detach(dhd_pub_t *dhdp);
extern void dhd_macdbg_event_handler(dhd_pub_t *dhdp, uint32 reason,
	uint8 *event_data, uint32 datalen);
extern int dhd_macdbg_dumpmac(dhd_pub_t *dhdp, char *buf, int buflen, int *outbuflen, bool dump_x);
extern int dhd_macdbg_pd11regs(dhd_pub_t *dhdp, char *params, int plen, char *buf, int buflen);
extern int dhd_macdbg_reglist(dhd_pub_t *dhdp, char *buf, int buflen);
extern int dhd_macdbg_dumpsvmp(dhd_pub_t *dhdp, char *buf, int buflen, int *outbuflen);
extern int dhd_macdbg_psvmpmems(dhd_pub_t *dhdp, char *params, int plen, char *buf, int buflen);

#ifdef BCMPCIE
extern int dhd_macdbg_halt_psms(const struct si_pub *sih, uint halt, uint32 *bitmask);
extern int dhd_macdbg_dump_dongle(dhd_pub_t *dhdp, const struct si_pub *sih, int len,
	dump_dongle_in_t ddi, dump_dongle_out_t *ddo);
#endif /* BCMPCIE */

#endif /* _dhd_macdbg_h_ */
