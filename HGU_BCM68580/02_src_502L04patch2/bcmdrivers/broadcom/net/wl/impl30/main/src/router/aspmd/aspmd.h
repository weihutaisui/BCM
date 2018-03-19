/*
 * ASPMD include file
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
 * $Id:$
 */

#ifndef _EAPD_H_
#define _EAPD_H_

/* Message levels */
#define ASPMD_ERROR_VAL		0x00000001
#define ASPMD_INFO_VAL		0x00000002

#define STA_MAX_COUNT		256
#define ASPMD_EAPD_READ_MAX_LEN	2048

#define ASPMD_WKSP_FLAG_SHUTDOWN	0x1

typedef struct aspm_policy {
	char *sys_conf;
	uint ep_conf;
} aspm_policy_t;

typedef struct plat_chip {
	uint chipid;            /* Router CHIP ID */
	uint chiprev;           /* This rev starts to support ASPM */
	uint chippkg;           /* Package option */
	uint api;               /* ASPM policy index */
} plat_chip_t;

#define PRIMARY_IF_MAX_COUNT		3
#define VIRTUAL_IF_MAX_COUNT		7

typedef struct pcie_ep {
	char name[IFNAMSIZ];    /* Primary interface name */
	char virtual_names[VIRTUAL_IF_MAX_COUNT][IFNAMSIZ];   /* Virtual interface names */
	uint virtual_count;     /* Virtual interface count */
	uint chipid;            /* PCIe DEV CHIP ID */
	uint chiprev;           /* CHIP revision */
	uint chippkg;           /* Package option */
	uint bustype;           /* 0: SI_BUS, 1: PCI_BUS */
	char iov_cmd[8];        /* "wl": for NIC,  "dhd": for FD */
	uint aspm_supported;	/* 0: not support aspm, 1: support aspm */
	uint aspm_forced;	/* 0: ASPM support in IDLE mode (No wifi STA connected)
				 * 1: ASPM support in ACTIVE mode (wifi STA connected)
				 */
	uint aspm_policy;	/* ASPM policy */
} pcie_ep_t;

typedef struct aspm_info {
	plat_chip_t plat;
	uint ep_nums;	/* Total enpoints. Max is 3 */
	pcie_ep_t ep[PRIMARY_IF_MAX_COUNT];
} aspm_info_t;

/* ASPM configuration via nvram control
 * 0: ASPMD isn't running and ASPM is disabled
 * 1: ASPMD is running and ASPM is enabled only in IDLE mode
 * 2: ASPMD isn't running but ASPM is always enabled in ACTIVE mode
 */
#define ASPM_CONFIG_NONE		(-1)
#define ASPM_CONFIG_DISABLE		0
#define ASPM_CONFIG_IDLE		1
#define ASPM_CONFIG_FORCE		2

extern uint aspmd_msg_level;

#define ASPMDBANNER(fmt, arg...)	do { \
		printf(" ASPMD>> %s(%d): "fmt, __FUNCTION__, __LINE__ , ##arg);} while (0)

#ifdef BCMDBG
#define ASPMD_ERROR(fmt, arg...)	do { \
		if (aspmd_msg_level & ASPMD_ERROR_VAL) ASPMDBANNER(fmt , ##arg);} while (0)
#define ASPMD_INFO(fmt, arg...)	do { \
		if (aspmd_msg_level & ASPMD_INFO_VAL) ASPMDBANNER(fmt , ##arg);} while (0)
#else	/* #if BCMDBG */
#define ASPMD_ERROR(fmt, arg...)
#define ASPMD_INFO(fmt, arg...)
#endif	/* #if BCMDBG */

typedef struct aspmd_wksp {
	fd_set fdset;
	int	eapd_socket;	/* socket communicated with eapd */
	char aspm_readbuf[ASPMD_EAPD_READ_MAX_LEN];
	int	flags;
} aspmd_wksp_t;
#endif /* _EAPD_H_ */
