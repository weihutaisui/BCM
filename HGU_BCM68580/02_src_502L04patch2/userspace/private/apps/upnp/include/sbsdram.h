/*
 * BCM47XX Sonics SiliconBackplane SDRAM controller core hardware definitions.
 *
 * Copyright (c) 2003-2012  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 * $Id: sbsdram.h,v 13.12.26.1 2003/10/16 23:10:58 mthawani Exp $
 */

#ifndef	_SBSDRAM_H
#define	_SBSDRAM_H

#ifndef _LANGUAGE_ASSEMBLY

/* Sonics side: SDRAM core registers */
typedef volatile struct sbsdramregs {
	uint32	initcontrol;	/* Generates external SDRAM initialization sequence */
	uint32	config;		/* Initializes external SDRAM mode register */
	uint32	refresh;	/* Controls external SDRAM refresh rate */
	uint32	pad1;
	uint32	pad2;
} sbsdramregs_t;

#endif

/* SDRAM initialization control (initcontrol) register bits */
#define SDRAM_CBR	0x0001	/* Writing 1 generates refresh cycle and toggles bit */
#define SDRAM_PRE	0x0002	/* Writing 1 generates precharge cycle and toggles bit */
#define SDRAM_MRS	0x0004	/* Writing 1 generates mode register select cycle and toggles bit */
#define SDRAM_EN	0x0008	/* When set, enables access to SDRAM */
#define SDRAM_16Mb	0x0000	/* Use 16 Megabit SDRAM */
#define SDRAM_64Mb	0x0010	/* Use 64 Megabit SDRAM */
#define SDRAM_128Mb	0x0020	/* Use 128 Megabit SDRAM */
#define SDRAM_RSVMb	0x0030	/* Use special SDRAM */
#define SDRAM_RST	0x0080	/* Writing 1 causes soft reset of controller */
#define SDRAM_SELFREF	0x0100	/* Writing 1 enables self refresh mode */
#define SDRAM_PWRDOWN	0x0200	/* Writing 1 causes controller to power down */
#define SDRAM_32BIT	0x0400	/* When set, indicates 32 bit SDRAM interface */
#define SDRAM_9BITCOL	0x0800	/* When set, indicates 9 bit column */

/* SDRAM configuration (config) register bits */
#define SDRAM_BURSTFULL	0x0000	/* Use full page bursts */
#define SDRAM_BURST8	0x0001	/* Use burst of 8 */
#define SDRAM_BURST4	0x0002	/* Use burst of 4 */
#define SDRAM_BURST2	0x0003	/* Use burst of 2 */
#define SDRAM_CAS3	0x0000	/* Use CAS latency of 3 */
#define SDRAM_CAS2	0x0004	/* Use CAS latency of 2 */

/* SDRAM refresh control (refresh) register bits */
#define SDRAM_REF(p)	(((p)&0xff) | SDRAM_REF_EN)	/* Refresh period */
#define SDRAM_REF_EN	0x8000		/* Writing 1 enables periodic refresh */

/* SDRAM Core default Init values (OCP ID 0x803) */
#define SDRAM_INIT	MEM4MX16X2
#define SDRAM_CONFIG    SDRAM_BURSTFULL
#define SDRAM_REFRESH   SDRAM_REF(0x40)

#define MEM1MX16	0x009	/* 2 MB */
#define MEM1MX16X2	0x409	/* 4 MB */
#define MEM2MX8X2	0x809	/* 4 MB */
#define MEM2MX8X4	0xc09	/* 8 MB */
#define MEM2MX32	0x439	/* 8 MB */
#define MEM4MX16	0x019	/* 8 MB */
#define MEM4MX16X2	0x419	/* 16 MB */
#define MEM8MX8X2	0x819	/* 16 MB */
#define MEM8MX16	0x829	/* 16 MB */
#define MEM4MX32	0x429	/* 16 MB */
#define MEM8MX8X4	0xc19	/* 32 MB */
#define MEM8MX16X2	0xc29	/* 32 MB */

#endif	/* _SBSDRAM_H */
