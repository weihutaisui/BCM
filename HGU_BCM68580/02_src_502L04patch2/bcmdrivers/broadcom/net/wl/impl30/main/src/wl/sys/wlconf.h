/*
 * Broadcom 802.11abg Networking Device Driver Configuration file
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *                                     
 * $Id$
 *
 * wl driver tunables
 */
#ifndef BCMDBUS
/*From wltunable_lx_router.h */

#define NRXBUFPOST	56	/* # rx buffers posted */
#define RXBND		24	/* max # rx frames to process */
#define PKTCBND		36	/* max # rx frames to chain */
#ifdef __ARM_ARCH_7A__
#define CTFPOOLSZ       512	/* max buffers in ctfpool */
#else
#define CTFPOOLSZ       192	/* max buffers in ctfpool */
#endif

#define WME_PER_AC_TX_PARAMS 1
#define WME_PER_AC_TUNING 1

#define NTXD_AC3X3		512	/* TX descriptor ring */
#define NRXD_AC3X3		512	/* RX descriptor ring */
#define NTXD_LARGE_AC3X3	2048	/* TX descriptor ring */
#define NRXD_LARGE_AC3X3	2048	/* RX descriptor ring */
#define NRXBUFPOST_AC3X3	500	/* # rx buffers posted */
#define RXBND_AC3X3		36	/* max # rx frames to process */
#ifdef __ARM_ARCH_7A__
#define CTFPOOLSZ_AC3X3		1024	/* max buffers in ctfpool */
#else
#define CTFPOOLSZ_AC3X3		512	/* max buffers in ctfpool */
#endif

#ifdef DSLCPE
#define TXBND_AC3X3		128
#define TXBND_AC3X3             128
#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148)
#define PKTCBND_AC3X3           48      /* max # rx frames to chain */
#else
#define PKTCBND_AC3X3		128	/* max # rx frames to chain */
#endif
#else
#define PKTCBND_AC3X3		48	/* max # rx frames to chain */
#endif

#define TXMR			2	/* number of outstanding reads */
#define TXPREFTHRESH		8	/* prefetch threshold */
#define TXPREFCTL		16	/* max descr allowed in prefetch request */
#define TXBURSTLEN		256	/* burst length for dma reads */

#define RXPREFTHRESH		1	/* prefetch threshold */
#define RXPREFCTL		8	/* max descr allowed in prefetch request */
#define RXBURSTLEN		256	/* burst length for dma writes */

#ifdef DSLCPE
#define MRRS			AUTO	/* Max read request size */
#else
#define MRRS			512	/* Max read request size */
#endif

#define AMPDU_PKTQ_FAVORED_LEN  4096

#define WLRXEXTHDROOM -1        /* to reserve extra headroom in DMA Rx buffer */

#else /* BCMDBUS */

#define NTXD		64	/* THIS HAS TO MATCH with HIGH driver tunable */
#define NRXD		32
#define NRXBUFPOST	16
#define WLC_DATAHIWAT	10
#define RXBND		16
#define NRPCTXBUFPOST	48
#endif
