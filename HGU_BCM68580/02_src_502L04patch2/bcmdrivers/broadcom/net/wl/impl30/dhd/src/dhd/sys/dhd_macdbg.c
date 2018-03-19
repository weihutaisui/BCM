/* D11 macdbg functions for Broadcom 802.11abgn
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
 * $Id: dhd_macdbg.c 687763 2017-03-02 12:59:29Z $
 */

#include <typedefs.h>
#include <osl.h>

#include <bcmutils.h>
#include <dhd_dbg.h>
#include <dhd_macdbg.h>
#include "d11reglist_proto.h"
#include "dhdioctl.h"
#include <sdiovar.h>

#ifdef BCMDBUS
#include <dbus.h>
#define BUS_IOVAR_OP(a, b, c, d, e, f, g) dbus_iovar_op(a->dbus, b, c, d, e, f, g)
#else
#include <dhd_bus.h>
#define BUS_IOVAR_OP dhd_bus_iovar_op
#endif // endif

typedef struct _macdbg_info_t {
	dhd_pub_t *dhdp;
	d11regs_list_t *pd11regs;
	uint16 d11regs_sz;
	d11regs_list_t *pd11regs_x;
	uint16 d11regsx_sz;
	svmp_list_t *psvmpmems;
	uint16 svmpmems_sz;
} macdbg_info_t;

#define SVMPLIST_HARDCODE

int
dhd_macdbg_attach(dhd_pub_t *dhdp)
{
	macdbg_info_t *macdbg_info = MALLOCZ(dhdp->osh, sizeof(*macdbg_info));
#ifdef SVMPLIST_HARDCODE
	svmp_list_t svmpmems[] = {
		{0x20000, 256},
		{0x21e10, 16},
		{0x20300, 16},
		{0x20700, 16},
		{0x20b00, 16},
		{0x20be0, 16},
		{0x20bff, 16},
		{0xc000, 32},
		{0xe000, 32},
		{0x10000, 0x8000},
		{0x18000, 0x8000}
	};
#endif /* SVMPLIST_HARDCODE */

	if (macdbg_info == NULL) {
		return BCME_NOMEM;
	}
	dhdp->macdbg_info = macdbg_info;
	macdbg_info->dhdp = dhdp;

#ifdef SVMPLIST_HARDCODE
	macdbg_info->psvmpmems = MALLOCZ(dhdp->osh, sizeof(svmpmems));
	if (macdbg_info->psvmpmems == NULL) {
		return BCME_NOMEM;
	}

	macdbg_info->svmpmems_sz = ARRAYSIZE(svmpmems);
	memcpy(macdbg_info->psvmpmems, svmpmems, sizeof(svmpmems));

	DHD_ERROR(("%s: psvmpmems %p svmpmems_sz %d\n",
		__FUNCTION__, macdbg_info->psvmpmems, macdbg_info->svmpmems_sz));
#endif // endif
	return BCME_OK;
}

void
dhd_macdbg_detach(dhd_pub_t *dhdp)
{
	macdbg_info_t *macdbg_info = dhdp->macdbg_info;
	ASSERT(macdbg_info);

	if (macdbg_info->pd11regs) {
		ASSERT(macdbg_info->d11regs_sz > 0);
		MFREE(dhdp->osh, macdbg_info->pd11regs,
		(macdbg_info->d11regs_sz * sizeof(macdbg_info->pd11regs[0])));
		macdbg_info->d11regs_sz = 0;
	}
	if (macdbg_info->pd11regs_x) {
		ASSERT(macdbg_info->d11regsx_sz > 0);
		MFREE(dhdp->osh, macdbg_info->pd11regs_x,
		(macdbg_info->d11regsx_sz * sizeof(macdbg_info->pd11regs_x[0])));
		macdbg_info->d11regsx_sz = 0;
	}
	if (macdbg_info->psvmpmems) {
		ASSERT(macdbg_info->svmpmems_sz > 0);
		MFREE(dhdp->osh, macdbg_info->psvmpmems,
		(macdbg_info->svmpmems_sz * sizeof(macdbg_info->psvmpmems[0])));
		macdbg_info->svmpmems_sz = 0;
	}
	MFREE(dhdp->osh, macdbg_info, sizeof(*macdbg_info));
}

void
dhd_macdbg_event_handler(dhd_pub_t *dhdp, uint32 reason,
		uint8 *event_data, uint32 datalen)
{
	d11regs_list_t *pd11regs;
	macdbg_info_t *macdbg_info = dhdp->macdbg_info;
	uint d11regs_sz;

	DHD_TRACE(("%s: reason %d datalen %d\n", __FUNCTION__, reason, datalen));
	switch (reason) {
		case WLC_E_MACDBG_LIST_PSMX:
			/* Fall through */
		case WLC_E_MACDBG_LIST_PSM:
			pd11regs = MALLOCZ(dhdp->osh, datalen);
			if (pd11regs == NULL) {
				DHD_ERROR(("%s: NOMEM for len %d\n", __FUNCTION__, datalen));
				return;
			}
			memcpy(pd11regs, event_data, datalen);
			d11regs_sz = datalen / sizeof(pd11regs[0]);
			DHD_ERROR(("%s: d11regs %p d11regs_sz %d\n",
				__FUNCTION__, pd11regs, d11regs_sz));
			if (reason == WLC_E_MACDBG_LIST_PSM) {
				macdbg_info->pd11regs = pd11regs;
				macdbg_info->d11regs_sz = d11regs_sz;
			} else {
				macdbg_info->pd11regs_x = pd11regs;
				macdbg_info->d11regsx_sz = d11regs_sz;
			}
			break;
		case WLC_E_MACDBG_REGALL:
			/* Schedule to work queue as this context could be ISR */
			dhd_schedule_macdbg_dump(dhdp);
			break;
		default:
			DHD_ERROR(("%s: Unknown reason %d\n",
				__FUNCTION__, reason));
	}
	return;
}

static uint16
_dhd_get_ihr16(macdbg_info_t *macdbg_info, uint16 addr, struct bcmstrbuf *b, bool verbose)
{
	sdreg_t sdreg;
	uint16 val;

	sdreg.func = 2;
	sdreg.offset = (0x1000 | addr);
	BUS_IOVAR_OP(macdbg_info->dhdp, "sbreg",
		&sdreg, sizeof(sdreg), &val, sizeof(val), IOV_GET);
#ifdef BCMDBG
	if (verbose) {
		if (b) {
			bcm_bprintf(b, "DEBUG: IHR16: read 0x%08x, size 2, value 0x%04x\n",
				(addr + 0x18001000), val);
		} else {
			printf("DEBUG: IHR16: read 0x%08x, size 2, value 0x%04x\n",
				(addr + 0x18001000), val);
		}
	}
#else
	BCM_REFERENCE(verbose);
#endif /* BCMDBG */
	return val;
}

static uint32
_dhd_get_ihr32(macdbg_info_t *macdbg_info, uint16 addr, struct bcmstrbuf *b, bool verbose)
{
	sdreg_t sdreg;
	uint32 val;

	sdreg.func = 4;
	sdreg.offset = (0x1000 | addr);
	BUS_IOVAR_OP(macdbg_info->dhdp, "sbreg",
		&sdreg, sizeof(sdreg), &val, sizeof(val), IOV_GET);
#ifdef BCMDBG
	if (verbose) {
		if (b) {
			bcm_bprintf(b, "DEBUG: IHR32: read 0x%08x, size 4, value 0x%08x\n",
				(addr + 0x18001000), val);
		} else {
			printf("DEBUG: IHR32: read 0x%08x, size 4, value 0x%08x\n",
				(addr + 0x18001000), val);
		}
	}
#else
	BCM_REFERENCE(verbose);
#endif /* BCMDBG */
	return val;
}

static void
_dhd_set_ihr16(macdbg_info_t *macdbg_info, uint16 addr, uint16 val,
	struct bcmstrbuf *b, bool verbose)
{
	sdreg_t sdreg;

	sdreg.func = 2;
	sdreg.offset = (0x1000 | addr);
	sdreg.value = val;
#ifdef BCMDBG
	if (verbose) {
		if (b) {
			bcm_bprintf(b, "DEBUG: IHR16: write 0x%08x, size 2, value 0x%04x\n",
				(addr + 0x18001000), val);
		} else {
			printf("DEBUG: IHR16: write 0x%08x, size 2, value 0x%04x\n",
				(addr + 0x18001000), val);
		}
	}
#else
	BCM_REFERENCE(verbose);
#endif /* BCMDBG */
	BUS_IOVAR_OP(macdbg_info->dhdp, "sbreg",
		NULL, 0, &sdreg, sizeof(sdreg), IOV_SET);
}

static void
_dhd_set_ihr32(macdbg_info_t *macdbg_info, uint16 addr, uint32 val,
	struct bcmstrbuf *b, bool verbose)
{
	sdreg_t sdreg;

	sdreg.func = 4;
	sdreg.offset = (0x1000 | addr);
	sdreg.value = val;
#ifdef BCMDBG
	if (verbose) {
		if (b) {
			bcm_bprintf(b, "DEBUG: IHR32: write 0x%08x, size 4, value 0x%08x\n",
				(addr + 0x18001000), val);
		} else {
			printf("DEBUG: IHR32: write 0x%08x, size 4, value 0x%08x\n",
				(addr + 0x18001000), val);
		}
	}
#else
	BCM_REFERENCE(verbose);
#endif /* BCMDBG */
	BUS_IOVAR_OP(macdbg_info->dhdp, "sbreg",
		NULL, 0, &sdreg, sizeof(sdreg), IOV_SET);
}

static uint32
_dhd_get_d11obj32(macdbg_info_t *macdbg_info, uint16 objaddr, uint32 sel,
	struct bcmstrbuf *b, bool verbose)
{
	uint32 val;
	sdreg_t sdreg;
	sdreg.func = 4; // 4bytes by default.
	sdreg.offset = 0x1160;

	if (objaddr == 0xffff) {
#ifdef BCMDBG
		if (verbose) {
			goto objaddr_read;
		} else {
			goto objdata_read;
		}
#else
		BCM_REFERENCE(verbose);
		goto objdata_read;
#endif /* BCMDBG */
	}

	if (objaddr & 0x3) {
		printf("%s: ERROR! Invalid addr 0x%x\n", __FUNCTION__, objaddr);
	}

	sdreg.value = (sel | (objaddr >> 2));
#ifdef BCMDBG
	if (verbose) {
		if (b) {
			bcm_bprintf(b, "DEBUG: %s: Indirect: write 0x%08x, size %d, value 0x%08x\n",
				(sel & 0x00020000) ? "SCR":"SHM",
				(sdreg.offset + 0x18000000), sdreg.func, sdreg.value);
		} else {
			printf("DEBUG: %s: Indirect: write 0x%08x, size %d, value 0x%08x\n",
				(sel & 0x00020000) ? "SCR":"SHM",
				(sdreg.offset + 0x18000000), sdreg.func, sdreg.value);
		}
	}
#endif /* BCMDBG */
	BUS_IOVAR_OP(macdbg_info->dhdp, "sbreg",
		NULL, 0, &sdreg, sizeof(sdreg), IOV_SET);

#ifdef BCMDBG
objaddr_read:
#endif /* BCMDBG */
	/* Give some time to obj addr register */
	BUS_IOVAR_OP(macdbg_info->dhdp, "sbreg",
		&sdreg, sizeof(sdreg), &val, sizeof(val), IOV_GET);
#ifdef BCMDBG
	if (verbose) {
		if (b) {
			bcm_bprintf(b, "DEBUG: %s: Indirect: Read 0x%08x, size %d, value 0x%08x\n",
				(sel & 0x00020000) ? "SCR":"SHM",
				(sdreg.offset + 0x18000000), sdreg.func, val);
		} else {
			printf("DEBUG: %s: Indirect: Read 0x%08x, size %d, value 0x%08x\n",
				(sel & 0x00020000) ? "SCR":"SHM",
				(sdreg.offset + 0x18000000), sdreg.func, val);
		}
	}
#endif /* BCMDBG */

objdata_read:
	sdreg.offset = 0x1164;
	BUS_IOVAR_OP(macdbg_info->dhdp, "sbreg",
		&sdreg, sizeof(sdreg), &val, sizeof(val), IOV_GET);
#ifdef BCMDBG
	if (verbose) {
		if (b) {
			bcm_bprintf(b, "DEBUG: %s: Indirect: Read 0x%08x, size %d, value 0x%04x\n",
				(sel & 0x00020000) ? "SCR":"SHM",
				(sdreg.offset + 0x18000000), sdreg.func, val);
		} else {
			printf("DEBUG: %s: Indirect: Read 0x%08x, size %d, value 0x%04x\n",
				(sel & 0x00020000) ? "SCR":"SHM",
				(sdreg.offset + 0x18000000), sdreg.func, val);
		}
	}
#endif /* BCMDBG */
	return val;
}

static uint16
_dhd_get_d11obj16(macdbg_info_t *macdbg_info, uint16 objaddr,
	uint32 sel, d11obj_cache_t *obj_cache, struct bcmstrbuf *b, bool verbose)
{
	uint32 val;
	if (obj_cache && obj_cache->cache_valid && ((obj_cache->sel ^ sel) & (0xffffff)) == 0) {
		if (obj_cache->addr32 == (objaddr & ~0x3)) {
#ifdef BCMDBG
			if (verbose) {
				if (b) {
					bcm_bprintf(b, "DEBUG: %s: Read cache value: "
						"addr32 0x%04x, sel 0x%08x, value 0x%08x\n",
						(sel & 0x00020000) ? "SCR":"SHM",
						obj_cache->addr32, obj_cache->sel, obj_cache->val);
				} else {
					printf("DEBUG: %s: Read cache value: "
						"addr32 0x%04x, sel 0x%08x, value 0x%08x\n",
						(sel & 0x00020000) ? "SCR":"SHM",
						obj_cache->addr32, obj_cache->sel, obj_cache->val);
				}
			}
#else
			BCM_REFERENCE(verbose);
#endif /* BCMDBG */
			val = obj_cache->val;
			goto exit;
		} else if ((obj_cache->sel & 0x02000000) &&
			(obj_cache->addr32 + 4 == (objaddr & ~0x3))) {
#ifdef BCMDBG
			if (verbose) {
				if (b) {
					bcm_bprintf(b, "DEBUG: %s: Read objdata only: "
						"addr32 0x%04x, sel 0x%08x, value 0x%08x\n",
						(sel & 0x00020000) ? "SCR":"SHM",
						obj_cache->addr32, obj_cache->sel, obj_cache->val);
				} else {
					printf("DEBUG: %s: Read objdata only: "
						"addr32 0x%04x, sel 0x%08x, value 0x%08x\n",
						(sel & 0x00020000) ? "SCR":"SHM",
						obj_cache->addr32, obj_cache->sel, obj_cache->val);
				}
			}
#endif /* BCMDBG */
			val = _dhd_get_d11obj32(macdbg_info, 0xffff, sel, b, verbose);
			goto exit;
		}
	}
	val = _dhd_get_d11obj32(macdbg_info, (objaddr & ~0x2), sel, b, verbose);
exit:
	if (obj_cache) {
		obj_cache->addr32 = (objaddr & ~0x3);
		obj_cache->sel = sel;
		obj_cache->val = val;
		obj_cache->cache_valid = TRUE;
	}
	return (uint16)((objaddr & 0x2) ? (val >> 16) : val);
}

static int
_dhd_print_d11reg(macdbg_info_t *macdbg_info, int idx, int type, uint16 addr, struct bcmstrbuf *b,
	d11obj_cache_t *obj_cache, bool verbose)
{
	const char *regname[D11REG_TYPE_MAX] = D11REGTYPENAME;
	uint32 val;

	if (type == D11REG_TYPE_IHR32) {
		if ((addr & 0x3)) {
			printf("%s: ERROR! Invalid addr 0x%x\n", __FUNCTION__, addr);
			addr &= ~0x3;
		}
		val = _dhd_get_ihr32(macdbg_info, addr, b, verbose);
		if (b) {
			bcm_bprintf(b, "%-3d %s 0x%-4x = 0x%-8x\n",
				idx, regname[type], addr, val);
		} else {
			printf("%-3d %s 0x%-4x = 0x%-8x\n",
				idx, regname[type], addr, val);
		}
	} else {
		switch (type) {
		case D11REG_TYPE_IHR16: {
			if ((addr & 0x1)) {
				printf("%s: ERROR! Invalid addr 0x%x\n", __FUNCTION__, addr);
				addr &= ~0x1;
			}
			val = _dhd_get_ihr16(macdbg_info, addr, b, verbose);
			break;
		}
		case D11REG_TYPE_IHRX16:
			val = _dhd_get_d11obj16(macdbg_info, (addr - 0x400) << 1, 0x020b0000,
				obj_cache, b, verbose);
			break;
		case D11REG_TYPE_SCR:
			val = _dhd_get_d11obj16(macdbg_info, addr << 2, 0x02020000,
				obj_cache, b, verbose);
			break;
		case D11REG_TYPE_SCRX:
			val = _dhd_get_d11obj16(macdbg_info, addr << 2, 0x020a0000,
				obj_cache, b, verbose);
			break;
		case D11REG_TYPE_SHM:
			val = _dhd_get_d11obj16(macdbg_info, addr, 0x02010000,
				obj_cache, b, verbose);
			break;
		case D11REG_TYPE_SHMX:
			val = _dhd_get_d11obj16(macdbg_info, addr, 0x02090000,
				obj_cache, b, verbose);
			break;
		default:
			printf("Unrecognized type %d!\n", type);
			return 0;
		}
		if (b) {
			bcm_bprintf(b, "%-3d %s 0x%-4x = 0x%-4x\n",
				idx, regname[type], addr, val);
		} else {
			printf("%-3d %s 0x%-4x = 0x%-4x\n",
				idx, regname[type], addr, val);
		}
	}
	return 1;
}

static int
_dhd_print_d11regs(macdbg_info_t *macdbg_info, d11regs_list_t *pregs,
	int start_idx, struct bcmstrbuf *b, bool verbose)
{
	uint16 addr;
	int idx = 0;
	d11obj_cache_t obj_cache = {0, 0, 0, FALSE};

	addr = pregs->addr;
	if (pregs->type >= D11REG_TYPE_MAX) {
		printf("%s: wrong type %d\n", __FUNCTION__, pregs->type);
		return 0;
	}
	if (pregs->bitmap) {
		while (pregs->bitmap) {
			if (pregs->bitmap && (pregs->bitmap & 0x1)) {
				_dhd_print_d11reg(macdbg_info, (idx + start_idx), pregs->type,
					addr, b, &obj_cache, verbose);
				idx++;
			}
			pregs->bitmap = pregs->bitmap >> 1;
			addr += pregs->step;
		}
	} else {
		for (; idx < pregs->cnt; idx++) {
			_dhd_print_d11reg(macdbg_info, (idx + start_idx), pregs->type,
				addr, b, &obj_cache, verbose);
			addr += pregs->step;
		}
	}
	return idx;
}

static int
_dhd_pd11regs_bylist(macdbg_info_t *macdbg_info, d11regs_list_t *reglist,
	uint16 reglist_sz, struct bcmstrbuf *b)
{
	uint i, idx = 0;

	if (reglist != NULL && reglist_sz > 0) {
		for (i = 0; i < reglist_sz; i++) {
			DHD_TRACE(("%s %d %p %d\n", __FUNCTION__, __LINE__,
				&reglist[i], reglist_sz));
			idx += _dhd_print_d11regs(macdbg_info, &reglist[i], idx, b, FALSE);
		}
	}
	return idx;
}

int
dhd_macdbg_dumpmac(dhd_pub_t *dhdp, char *buf, int buflen,
	int *outbuflen, bool dump_x)
{
	macdbg_info_t *macdbg_info = dhdp->macdbg_info;
	struct bcmstrbuf *b = NULL;
	struct bcmstrbuf bcmstrbuf;
	uint cnt = 0;

	DHD_TRACE(("%s %d %p %d %p %d %p %d\n",	__FUNCTION__, __LINE__,
		buf, buflen, macdbg_info->pd11regs, macdbg_info->d11regs_sz,
		macdbg_info->pd11regs_x, macdbg_info->d11regsx_sz));

	if (buf && buflen > 0) {
		bcm_binit(&bcmstrbuf, buf, buflen);
		b = &bcmstrbuf;
	}
	if (!dump_x) {
		/* Dump PSMr */
		cnt += _dhd_pd11regs_bylist(macdbg_info, macdbg_info->pd11regs,
			macdbg_info->d11regs_sz, b);
	} else {
		/* Dump PSMx */
		cnt += _dhd_pd11regs_bylist(macdbg_info, macdbg_info->pd11regs_x,
			macdbg_info->d11regsx_sz, b);
	}

	if (b && outbuflen) {
		if (buflen > BCMSTRBUF_LEN(b)) {
			*outbuflen = buflen - BCMSTRBUF_LEN(b);
		} else {
			DHD_ERROR(("%s: buflen insufficient!\n", __FUNCTION__));
			*outbuflen = buflen;
			/* Do not return buftooshort to allow printing macregs we have got */
		}
	}

	return ((cnt > 0) ? BCME_OK : BCME_UNSUPPORTED);
}

int
dhd_macdbg_pd11regs(dhd_pub_t *dhdp, char *params, int plen, char *buf, int buflen)
{
	macdbg_info_t *macdbg_info = dhdp->macdbg_info;
	dhd_pd11regs_param *pd11regs = (void *)params;
	dhd_pd11regs_buf *pd11regs_buf = (void *)buf;
	uint16 start_idx;
	bool verbose;
	d11regs_list_t reglist;
	struct bcmstrbuf *b = NULL;
	struct bcmstrbuf bcmstrbuf;

	start_idx = pd11regs->start_idx;
	verbose = pd11regs->verbose;
	memcpy(&reglist, pd11regs->plist, sizeof(reglist));
	memset(buf, '\0', buflen);
	bcm_binit(&bcmstrbuf, pd11regs_buf->pbuf,
		(buflen - OFFSETOF(dhd_pd11regs_buf, pbuf)));
	b = &bcmstrbuf;
	pd11regs_buf->idx = _dhd_print_d11regs(macdbg_info, &reglist, start_idx, b, verbose);

	return ((pd11regs_buf->idx > 0) ? BCME_OK : BCME_ERROR);
}

int
dhd_macdbg_reglist(dhd_pub_t *dhdp, char *buf, int buflen)
{
	int err, desc_idx = 0;
	dhd_maclist_t *maclist = (dhd_maclist_t *)buf;
	macdbg_info_t *macdbg_info = dhdp->macdbg_info;
	void *xtlvbuf_p = maclist->plist;
	uint16 xtlvbuflen = (uint16)buflen;
	xtlv_desc_t xtlv_desc[] = {
		{0, 0, NULL},
		{0, 0, NULL},
		{0, 0, NULL},
		{0, 0, NULL}
	};

	if (!macdbg_info->pd11regs) {
		err = BCME_NOTFOUND;
		goto exit;
	}
	ASSERT(macdbg_info->d11regs_sz > 0);
	xtlv_desc[desc_idx].type = DHD_MACLIST_XTLV_R;
	xtlv_desc[desc_idx].len =
		macdbg_info->d11regs_sz * (uint16)sizeof(*(macdbg_info->pd11regs));
	xtlv_desc[desc_idx].ptr = macdbg_info->pd11regs;
	desc_idx++;

	if (macdbg_info->pd11regs_x) {
		ASSERT(macdbg_info->d11regsx_sz);
		xtlv_desc[desc_idx].type = DHD_MACLIST_XTLV_X;
		xtlv_desc[desc_idx].len = macdbg_info->d11regsx_sz *
			(uint16)sizeof(*(macdbg_info->pd11regs_x));
		xtlv_desc[desc_idx].ptr = macdbg_info->pd11regs_x;
		desc_idx++;
	}

	if (macdbg_info->psvmpmems) {
		ASSERT(macdbg_info->svmpmems_sz);
		xtlv_desc[desc_idx].type = DHD_SVMPLIST_XTLV;
		xtlv_desc[desc_idx].len = macdbg_info->svmpmems_sz *
			(uint16)sizeof(*(macdbg_info->psvmpmems));
		xtlv_desc[desc_idx].ptr = macdbg_info->psvmpmems;
		desc_idx++;
	}

	err = bcm_pack_xtlv_buf_from_mem(&xtlvbuf_p, &xtlvbuflen,
		xtlv_desc, BCM_XTLV_OPTION_ALIGN32);

	maclist->version = 0;		/* No version control for now anyway */
	maclist->bytes_len = (buflen - xtlvbuflen);

exit:
	return err;
}

static int
_dhd_print_svmps(macdbg_info_t *macdbg_info, svmp_list_t *psvmp,
	int start_idx, struct bcmstrbuf *b, bool verbose)
{
	int idx;
	uint32 addr, mem_id, offset, prev_mem_id, prev_offset;
	uint16 cnt, val;

	BCM_REFERENCE(start_idx);

	/* Set tbl ID and tbl offset. */
	_dhd_set_ihr32(macdbg_info, 0x3fc, 0x30000d, b, verbose);
	_dhd_set_ihr32(macdbg_info, 0x3fc, 0x8000000e, b, verbose);

	addr = psvmp->addr;
	cnt = psvmp->cnt;

	/* In validate previous mem_id and offset */
	prev_mem_id = (uint32)(-1);
	prev_offset = (uint32)(-1);

	for (idx = 0; idx < cnt; idx++, addr++) {
		mem_id = (addr >> 15);
		offset = (addr & 0x7fff) >> 1;

		if (mem_id != prev_mem_id) {
			/* Set mem_id */
			_dhd_set_ihr32(macdbg_info, 0x3fc, ((mem_id & 0xffff0000) | 0x10),
				b, verbose);
			_dhd_set_ihr32(macdbg_info, 0x3fc, ((mem_id << 16) | 0xf),
				b, verbose);
		}

		if (offset != prev_offset) {
			/* svmp offset */
			_dhd_set_ihr32(macdbg_info, 0x3fc, ((offset << 16) | 0xe),
				b, verbose);
		}
		/* Read hi or lo */
		_dhd_set_ihr16(macdbg_info, 0x3fc, ((addr & 0x1) ? 0x10 : 0xf), b, verbose);
		val = _dhd_get_ihr16(macdbg_info, 0x3fe, b, verbose);
		if (b) {
			bcm_bprintf(b, "0x%-4x 0x%-4x\n",
				addr, val);

		} else {
			printf("0x%-4x 0x%-4x\n",
				addr, val);
		}
		prev_mem_id = mem_id;
		prev_offset = offset;
	}
	return idx;
}

static int
_dhd_psvmps_bylist(macdbg_info_t *macdbg_info, svmp_list_t *svmplist,
	uint16 svmplist_sz, struct bcmstrbuf *b)
{
	uint i, idx = 0;

	if (svmplist != NULL && svmplist_sz > 0) {
		for (i = 0; i < svmplist_sz; i++) {
			DHD_TRACE(("%s %d %p %d\n", __FUNCTION__, __LINE__,
				&svmplist[i], svmplist_sz));
			idx += _dhd_print_svmps(macdbg_info, &svmplist[i], idx, b, FALSE);
		}
	}
	return idx;
}

int
dhd_macdbg_dumpsvmp(dhd_pub_t *dhdp, char *buf, int buflen,
	int *outbuflen)
{
	macdbg_info_t *macdbg_info = dhdp->macdbg_info;
	struct bcmstrbuf *b = NULL;
	struct bcmstrbuf bcmstrbuf;
	uint cnt = 0;

	DHD_TRACE(("%s %d %p %d %p %d\n",	__FUNCTION__, __LINE__,
		buf, buflen, macdbg_info->psvmpmems, macdbg_info->svmpmems_sz));

	if (buf && buflen > 0) {
		bcm_binit(&bcmstrbuf, buf, buflen);
		b = &bcmstrbuf;
	}
	cnt = _dhd_psvmps_bylist(macdbg_info, macdbg_info->psvmpmems,
			macdbg_info->svmpmems_sz, b);

	if (b && outbuflen) {
		if (buflen > BCMSTRBUF_LEN(b)) {
			*outbuflen = buflen - BCMSTRBUF_LEN(b);
		} else {
			DHD_ERROR(("%s: buflen insufficient!\n", __FUNCTION__));
			*outbuflen = buflen;
			/* Do not return buftooshort to allow printing macregs we have got */
		}
	}

	return ((cnt > 0) ? BCME_OK : BCME_UNSUPPORTED);
}

int
dhd_macdbg_psvmpmems(dhd_pub_t *dhdp, char *params, int plen, char *buf, int buflen)
{
	macdbg_info_t *macdbg_info = dhdp->macdbg_info;
	dhd_pd11regs_param *pd11regs = (void *)params;
	dhd_pd11regs_buf *pd11regs_buf = (void *)buf;
	uint16 start_idx;
	bool verbose;
	svmp_list_t reglist;
	struct bcmstrbuf *b = NULL;
	struct bcmstrbuf bcmstrbuf;

	start_idx = pd11regs->start_idx;
	verbose = pd11regs->verbose;
	memcpy(&reglist, pd11regs->plist, sizeof(reglist));
	memset(buf, '\0', buflen);
	bcm_binit(&bcmstrbuf, pd11regs_buf->pbuf,
		(buflen - OFFSETOF(dhd_pd11regs_buf, pbuf)));
	b = &bcmstrbuf;
	pd11regs_buf->idx = _dhd_print_svmps(macdbg_info, &reglist, start_idx, b, verbose);

	return ((pd11regs_buf->idx > 0) ? BCME_OK : BCME_ERROR);
}

#ifdef BCMPCIE
/**
 * For dump/debug purpose: definitions of certain d11 core registers
 */
#include <typedefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <sbchipc.h>
#include <hndpmu.h>
#include <hndsoc.h>
#include <bcmendian.h>
#include <dngl_stats.h>
#include <bcmdevs.h>
#include <dhd.h>

#define CC_WATCHDOGCOUNTER  (0x80 / sizeof(uint32))

#define D11_MACCONTROL      (0x120 / sizeof(uint32))
#define D11_MACINTMASK      (0x12c / sizeof(uint32))
#define D11_ALTMACINTMASK   (0x16c / sizeof(uint32))
#define D11_MACCONTROLX     (0x1b4 / sizeof(uint32)) /* for PSMx */
#define D11_OBJ_ADDR_OFFSET (0x160 / sizeof(uint32))
#define D11_OBJ_ADDR_DATA   (0x164 / sizeof(uint32))
#define D11_MACHWCAP1       (0x1A4 / sizeof(uint32))
#define D11_PHYREG_ADDR     (0x3FC / sizeof(uint16))
#define D11_PHYREG_DATA     (0x3FE / sizeof(uint16))
#define GET_SHM_SIZE(v)     ((v >> 1) & 0x7) /* in [uint32] units */

#define MCTL_PSM_RUN        (1 << 1)  /**< bitfield in MACCONTROL register */
#define MCTL_IHR_EN         (1 << 10) /**< bitfield in MACCONTROL register */

#define OBJ_ADDR_SL_SHIFT 16 /** bitfield in d11 object access register */
#define OBJ_ADDR_SL_UCODE (0 << OBJ_ADDR_SL_SHIFT)
#define OBJ_ADDR_SL_SHM   (1 << OBJ_ADDR_SL_SHIFT)
#define OBJ_ADDR_SL_SCR   (2 << OBJ_ADDR_SL_SHIFT)
#define OBJ_ADDR_SL_IHR   (3 << OBJ_ADDR_SL_SHIFT)
#define OBJ_ADDR_SL_AMT   (4 << OBJ_ADDR_SL_SHIFT)
#define OBJ_ADDR_SL_SHMx  (9 << OBJ_ADDR_SL_SHIFT)
#define OBJ_ADDR_SL_SCRx  (10 << OBJ_ADDR_SL_SHIFT)
#define OBJ_ADDR_SL_IHRx  (11 << OBJ_ADDR_SL_SHIFT)
#endif /* BCMPCIE */

#ifdef BCMPCIE

/**
 * WL utility specified a dump type, that dump type requires a certain selector value in the ObjAddr
 * register.
 */
static int
obj_addr_sl_get(uint32 dmp_type, int idx)
{
	switch (dmp_type) {
	case DUMP_DONGLE_D11MEM_SHM:
		return idx == 0 ? OBJ_ADDR_SL_SHM : OBJ_ADDR_SL_SHMx;
	case DUMP_DONGLE_D11MEM_SCR:
		return idx == 0 ? OBJ_ADDR_SL_SCR : OBJ_ADDR_SL_SCRx;
	case DUMP_DONGLE_D11IHR:
		return idx == 0 ? OBJ_ADDR_SL_IHR : OBJ_ADDR_SL_IHRx;
	case DUMP_DONGLE_D11MEM_AMT: return OBJ_ADDR_SL_AMT;
	}

	return 0;
}

/**
 * When dumping registers, the registers that have not been HW implemented need to be skipped.
 */
static bool
is_reg_in_list(uint16 reg_offset, CONST d11regs_list_t *regs_list, int n_reglist_entries)
{
	int i;
	uint32 addr;  /**< for short hand */

	for (i = 0; i < n_reglist_entries; i++) {
		addr = regs_list[i].addr;
		if (reg_offset < addr) {
			continue;
		}
		if (regs_list[i].bitmap == 0) {
			if (reg_offset < addr + regs_list[i].cnt * regs_list[i].step) {
				return TRUE;
			}
		} else if (reg_offset < addr + sizeof(uint32) * 8 * regs_list[i].step) {
			addr = (reg_offset - addr) / regs_list[i].step;
			if ((regs_list[i].bitmap >> addr) & 1) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

CONST d11regs_list_t cc_regs_rev48[] = { /* in e.g. 43602a3 */
	/*  type   addr      bitmap     step  cnt */
	{     0,   0x0, 0xFFF0FF0F,       4,    0   }, /*   0x0 -  0x7C */
	{     0,  0x80, 0xDF700E0F,       4,    0   }, /*  0x80 - 0x100 */
	{     0, 0x190, 0x00500007,       4,    0   }, /* 0x190 - 0x20C */
};

CONST d11regs_list_t cc_regs_rev58[] = { /* in e.g. 4366c0 */
	/*  type   addr      bitmap     step  cnt */
	{     0,   0x0, 0xFFF0FF0F,       4,    0   }, /*   0x0 -  0x7C */
	{     0,  0x80, 0xDF700E0F,       4,    0   }, /*  0x80 - 0x100 */
	{     0, 0x190, 0x00500007,       4,    0   }, /* 0x190 - 0x20C */
};

CONST d11regs_list_t d11_regs_rev49[] = { /* in e.g. 43602a3 */
	/*  type   addr      bitmap     step  cnt */
	{     0,  0x18, 0x00000000,       4,   14   }, /*  0x18 -  0x4C */
	{     0, 0x100, 0xFDF73F03,       4,    0   }, /* 0x100 - 0x17C */
	{     0, 0x180, 0x0700077F,       4,    0   }, /* 0x180 - 0x1FC */
	{     0, 0x200, 0x003F3F3F,       4,    0   }, /* 0x200 - 0x27C */
	{     0, 0x280, 0x0000003F,       4,    0   }, /* 0x280 - 0x28C */
	{     0, 0x300, 0x003F003F,       4,    6   }, /* 0x300 - 0x354 */
};

CONST d11regs_list_t d11_regs_rev65[] = { /* in e.g. 4366c0 */
	/*  type   addr      bitmap     step  cnt */
	{     0,  0x18, 0x00000000,       4,   14   }, /*  0x18 -  0x4C */
	{     0, 0x100, 0xFFF73F03,       4,    0   }, /* 0x100 - 0x17C */
	{     0, 0x180, 0x0700077F,       4,    0   }, /* 0x180 - 0x1FC */
	{     0, 0x200, 0x003F3F3F,       4,    0   }, /* 0x200 - 0x27C */
	{     0, 0x280, 0x0000003F,       4,    0   }, /* 0x280 - 0x28C */
	{     0, 0x300, 0x003F003F,       4,    6   }, /* 0x300 - 0x354 */
};

CONST d11regs_list_t pcieg2_regs_rev9[] = { /* in e.g. 43602a3 */
	/*  type   addr      bitmap     step  cnt */
	{     0,   0x0, 0x00FF3F3F,       4,    0   }, /*   0x0 -  0x7C */
	{     0, 0x100, 0x000F1F07,       4,    0   }, /* 0x100 - 0x17C */
	{     0, 0x180, 0x07000FFD,       4,    0   }, /* 0x180 - 0x19C */
	{     0, 0x200, 0x3F3F3F3F,       4,    0   }, /* 0x200 - 0x27C */
};

CONST d11regs_list_t pcieg2_regs_rev22[] = { /* in e.g. 4366c0 */
	/*  type   addr      bitmap     step  cnt */
	{     0,   0x0, 0x07FF3F7F,       4,    0   }, /*   0x0 -  0x7C */
	{     0, 0x100, 0x000F1F1F,       4,    0   }, /* 0x100 - 0x17C */
	{     0, 0x180, 0x070E0FFD,       4,    0   }, /* 0x180 - 0x19C */
	{     0, 0x200, 0x3F3F3F3F,       4,    0   }, /* 0x200 - 0x27C */
};

CONST d11regs_list_t pmu_regs_rev24[] = { /* in e.g. 43602a3 */
	/*  type   addr      bitmap     step  cnt */
	{    0,  0x600, 0xFFFEFFFF,       4,    0   }, /* 0x600 -  0x67C */
	{    0,  0x680, 0x0FFC800C,       4,    0   }, /* 0x680 -  0x6FC */
	{    0,  0x700, 0x33FF0303,       4,    0   }, /* 0x700 -  0x7FC */
};

CONST d11regs_list_t pmu_regs_rev31[] = { /* in e.g. 4366c0 */
	/*  type   addr      bitmap     step  cnt */
	{    0,  0x600, 0xFFFEFFFF,       4,    0   }, /* 0x600 -  0x67C */
	{    0,  0x680, 0x0FFC800C,       4,    0   }, /* 0x680 -  0x6FC */
	{    0,  0x700, 0x33FF0303,       4,    0   }, /* 0x700 -  0x7FC */
};

CONST d11regs_list_t ca7_regs_rev2[] = { /* in e.g. 4366c0 */
	/*  type   addr      bitmap     step  cnt */
	{     0,   0x0, 0x0000001F,       4,    0   }, /*   0x0 -  0x7C */
	{     0, 0x100, 0x00000001,       4,    0   }, /* 0x100 - 0x17C */
};

CONST d11regs_list_t cr4_regs_rev7[] = { /* in e.g. 43602a3 */
	/*  type   addr      bitmap     step  cnt */
	{     0,   0x0, 0x7C0F3FF7,       4,    0   }, /*   0x0 -  0x7C */
	{     0, 0x1e0, 0x00000005,       4,    0   }, /* 0x1e0 - 0x1eC */
};

CONST d11regs_list_t sysmem_regs_rev3[] = { /* in e.g. 4366c0 */
	/*  type   addr      bitmap     step  cnt */
	{     0,   0x0, 0x000F3FF5,       4,    0   }, /*   0x0 -  0x7C */
	{     0, 0x1e0, 0x00000000,       4,    3   }, /* 0x1e0 - 0x1e8 */
};

CONST d11regs_list_t gci_regs_rev7[] = { /* in e.g. 4366c0 */
	/*  type   addr      bitmap     step  cnt */
	{     0,   0x0, 0xFFFFFFFF,       4,    0   }, /*   0x0 -  0x7C */
	{     0, 0x100, 0xFFFFFFFF,       4,    0   }, /* 0x100 - 0x17C */
	{     0, 0x200, 0xFFFFFFFF,       4,    0   }, /* 0x200 - 0x27C */
};

CONST struct core_regs_s {
	uint core_id;               /**< id of core to dump */
	uint core_rev;              /**< rev of core to dump */
	uint n_regs_list_entries;   /**< #entries in struct member 'regs' */
	CONST d11regs_list_t *regs; /**< contains registers to dump */
} core_regs[] = {
	{CC_CORE_ID,    48, ARRAYSIZE(cc_regs_rev48), cc_regs_rev48},
	{CC_CORE_ID,    58, ARRAYSIZE(cc_regs_rev58), cc_regs_rev58},
	{PMU_CORE_ID,   24, ARRAYSIZE(pmu_regs_rev24), pmu_regs_rev24},
	{PMU_CORE_ID,   31, ARRAYSIZE(pmu_regs_rev31), pmu_regs_rev31},
	{PCIE2_CORE_ID,  9, ARRAYSIZE(pcieg2_regs_rev9), pcieg2_regs_rev9},
	{PCIE2_CORE_ID, 22, ARRAYSIZE(pcieg2_regs_rev22), pcieg2_regs_rev22},
	{ARMCA7_CORE_ID, 2, ARRAYSIZE(ca7_regs_rev2), ca7_regs_rev2},
	{ARMCR4_CORE_ID, 7, ARRAYSIZE(cr4_regs_rev7), cr4_regs_rev7},
	{SYSMEM_CORE_ID, 3, ARRAYSIZE(sysmem_regs_rev3), sysmem_regs_rev3},
	{D11_CORE_ID,   49, ARRAYSIZE(d11_regs_rev49), d11_regs_rev49},
	{D11_CORE_ID,   65, ARRAYSIZE(d11_regs_rev65), d11_regs_rev65},
	{GCI_CORE_ID,    7, ARRAYSIZE(gci_regs_rev7), gci_regs_rev7}
};

/**
 * Invoked by './dhd upload' command line. This function obtains core register dumps and d11 dumps
 * from the dongle and forwards those to user space. It does not require 'debug' enhanced firmware,
 * it can be used with 'production' firmware, and thus consumes no precious dongle memory (important
 * for e.g. the 43602). It is however only suited for PCIe and SDIO buses.
 *
 * @param outlen Length of output buffer pointed at by 'ddo' in [bytes]
 * @param ddi    Utility -> DHD direction
 * @param ddo    DHD -> Utility direction
 */
extern int
dhd_macdbg_dump_dongle(dhd_pub_t *dhdp, si_t *sih, int outlen, dump_dongle_in_t ddi,
	dump_dongle_out_t *ddo)
{
	uint16 *p16 = ddo->u.ui16;
	uint32 *p32 = ddo->u.ui32;
	uint64 *p64 = ddo->u.ui64;
	uint16 ui16;
	uint32 ui32;
	uint64 ui64;
	uint max_read_offset = 0; /**< in [bytes] */
	volatile uint16 *pv16 = NULL;
	volatile uint32 *pv = NULL;
	const int shm_sizes[] = {1024, 1536, 2048}; /**< reg bitfield MACHwCap1::ShmSize */
	uint obj_addr_sl = 0;  /**< reg bitfield ObjAddr::Select */
	uint obj_addr_idx = 0; /**< reg bitfield ObjAddr::Index */
	int bcmerror = BCME_OK;
	uint32 max_res_mask;
	CONST struct core_regs_s *crl = NULL;  /**< points at a core register list */

	ddo->element_width = sizeof(uint16);
	ddo->address = 0; // local memory address at which this dump starts
	ddo->n_bytes = 0;
	ddo->id = 0; /**< 0: core is not available */

	max_res_mask = pmu_corereg(sih, SI_CC_IDX, max_res_mask, 0, 0);
	si_pmu_set_min_res_mask(sih, NULL, max_res_mask); // 'up' cores before dump

	switch (ddi.type) {
	case DUMP_DONGLE_COREREG:
		max_read_offset = 4096 - 1; /* one core contains max 4096/4 registers */
		ddo->element_width = sizeof(uint32);
		if (si_setcoreidx(sih, ddi.index) == NULL) {
			goto exit; // beyond last core: core enumeration ended
		}
		for (ui32 = 0; ui32 < ARRAYSIZE(core_regs); ui32++) {
			crl = &core_regs[ui32];
			if (crl->core_id == si_coreid(sih) && crl->core_rev == si_corerev(sih)) {
				break;
			}
		}
		if (ui32 == ARRAYSIZE(core_regs)) {
			crl = NULL; /* register list for given core was not found */
		}

		// backplane address at which this dump starts
		ddo->address = si_addrspace(sih, 0);
		ddo->id = si_coreid(sih);
		ddo->rev = si_corerev(sih);
		if (!si_iscoreup(sih) && ddo->id != PMU_CORE_ID) {
			printf("dump: core id 0x%x is down\n", ddo->id);
			goto exit;
		}
		break;
	case DUMP_DONGLE_D11MEM_SHM:
	case DUMP_DONGLE_D11MEM_SCR:
	case DUMP_DONGLE_D11MEM_AMT:
	case DUMP_DONGLE_D11IHR:
	case DUMP_DONGLE_PHYREGS:
		pv = si_setcore(sih, D11_CORE_ID, 0);
		if (pv == NULL || !si_iscoreup(sih)) {
			printf("dump: d11 core is down\n");
			bcmerror = BCME_NOTUP;
			goto exit;
		}

		if (si_corerev(sih) < 64 && ddi.index > 0) { // d11rev < 64 has only one PSM
			goto exit;
		}

		obj_addr_sl = obj_addr_sl_get(ddi.type, ddi.index);
		switch (ddi.type) {
		case DUMP_DONGLE_D11MEM_SHM:
			ui32 = GET_SHM_SIZE(*(pv + D11_MACHWCAP1));
			max_read_offset = shm_sizes[ui32 <= 2 ? ui32 : 0];
			max_read_offset *= sizeof(uint32);
			break;
		case DUMP_DONGLE_D11MEM_SCR:
			max_read_offset = 0x1000;
			break;
		case DUMP_DONGLE_D11MEM_AMT:
			max_read_offset = 0x100;
			ddo->element_width = sizeof(uint64);
			break;
		case DUMP_DONGLE_D11IHR:
			max_read_offset = 0x1000; /* IHR reg space in [bytes] */
			break;
		case DUMP_DONGLE_PHYREGS:
			max_read_offset = 0x2000 * sizeof(uint16);
			/* To allow driver instead of PSM phy register access: */
			*(pv + D11_MACCONTROL) |= MCTL_IHR_EN;
			pv16 = (volatile uint16*) pv; // PHY access registers are 16 bits wide
			break;
		}
		obj_addr_idx = ddi.byte_offset / ddo->element_width;
		break;
	default:
		printf("dump failed: unknown type %d\n", ddi.type);
		bcmerror = BCME_BADARG;
		goto exit;
	}

	ddo->address += ddi.byte_offset; // start address of this dump

	while (ddi.byte_offset < max_read_offset &&
		sizeof(dump_dongle_out_t) + ddo->n_bytes + ddo->element_width < (uint)outlen) {
		switch (ddi.type) {
		case DUMP_DONGLE_COREREG:    /* core register entries are 32 bits wide */
			if (crl == NULL ||
			    is_reg_in_list(ddi.byte_offset, crl->regs, crl->n_regs_list_entries)) {
				*p32++ = si_corereg(sih, ddi.index, ddi.byte_offset, 0, 0);
			} else {
				*p32++ = 0xDEADBEEF;
			}
			break;
		case DUMP_DONGLE_D11MEM_SHM: /* SHM entries are 16 bits wide */
			*(pv + D11_OBJ_ADDR_OFFSET) = (obj_addr_idx | OBJ_ADDR_SL_SHM);
			ui32 = *(pv + D11_OBJ_ADDR_DATA);
			*p16++ = BCMSWAP16(ui32 & 0xFFFF); /* first entry */
			*p16++ = BCMSWAP16(ui32 >> 16);     /* second entry */
			ddi.byte_offset += sizeof(uint16);
			ddo->n_bytes += sizeof(uint16);
			break;
		case DUMP_DONGLE_D11MEM_AMT: /* AMT entries are 64 bits wide */
			// ObjData accesses the 32 low bits of AMT[index/2] for even values of index
			*(pv + D11_OBJ_ADDR_OFFSET) = (obj_addr_idx | OBJ_ADDR_SL_AMT);
			ui64 = *(pv + D11_OBJ_ADDR_DATA);
			// ... and the 32 high bits of AMT[(Index-1)/2] for odd values of index
			*(pv + D11_OBJ_ADDR_OFFSET) = (++obj_addr_idx | OBJ_ADDR_SL_AMT);
			ui32 = *(pv + D11_OBJ_ADDR_DATA);
			ui64 |= ((uint64)ui32 << 32);
			*p64++ = ui64;
			break;
		case DUMP_DONGLE_D11IHR:     /* SCR, IHR entries are 16 bits wide */
		case DUMP_DONGLE_D11MEM_SCR:
			*(pv + D11_OBJ_ADDR_OFFSET) = (obj_addr_idx | obj_addr_sl);
			ui32 = *(pv + D11_OBJ_ADDR_DATA);
			*p16++ = BCMSWAP16(ui32 & 0xFFFF); // upper two bytes are unused
			break;
		case DUMP_DONGLE_PHYREGS:
			*(pv16 + D11_PHYREG_ADDR) = obj_addr_idx;
			ui16 = *(pv16 + D11_PHYREG_DATA);
			*p16++ = BCMSWAP16(ui16);
			break;
		}
		ddi.byte_offset += ddo->element_width;
		ddo->n_bytes += ddo->element_width;
		++obj_addr_idx;
	}

exit:
	return bcmerror;
} /* dhd_macdbg_dump_dongle */

/**
 * PSM(s) in dongle have to be halted for consistent d11 memory (e.g. shm) dumps.
 *
 * param halt     '1' when PSMs should be halted
 * param bitmask  written when halt==1, contains previous psm_halt state of PSMs as a bitvector
 *
 * @return  BCME_OK on success
 */
extern int
dhd_macdbg_halt_psms(si_t *sih, uint halt, uint32 *bitmask)
{
	int bcmerror;
	volatile uint32 *pv = NULL;        /**< pointer to read/write core registers */

	pv = si_setcore(sih, CC_CORE_ID, 0);
	*(pv + CC_WATCHDOGCOUNTER) = 0; // disable CC watchdog

	pv = si_setcore(sih, D11_CORE_ID, 0);
	if (pv == NULL || !si_iscoreup(sih)) {
		printf("dump failed: d11 core is down\n");
		bcmerror = BCME_NOTUP;
		goto exit;
	}

	*(pv + D11_MACINTMASK) = 0;        // disable d11 irqs to avoid watchdog reset
	*(pv + D11_ALTMACINTMASK) = 0;

	if (halt) {
		*bitmask = (*(pv + D11_MACCONTROL) & MCTL_PSM_RUN) ? NBITVAL(0): 0;
		*bitmask |= (*(pv + D11_MACCONTROLX) & MCTL_PSM_RUN) ? NBITVAL(1): 0; // PSMx
		*(pv + D11_MACCONTROL) &= ~MCTL_PSM_RUN;
		*(pv + D11_MACCONTROLX) &= ~MCTL_PSM_RUN;
	} else {
		if (*bitmask & NBITVAL(0)) {
			*(pv + D11_MACCONTROL) |= MCTL_PSM_RUN;
		}
		if (*bitmask & NBITVAL(1)) {
			*(pv + D11_MACCONTROLX) |= MCTL_PSM_RUN; // PSMx
		}
	}

	bcmerror = BCME_OK;

exit:
	return bcmerror;
}

#endif /* BCMPCIE */
