/*
 * Extensible Authentication Protocol (EAP) definitions
 *
 * See
 * RFC 2284: PPP Extensible Authentication Protocol (EAP)
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
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: eap.h 518342 2014-12-01 23:21:41Z $
 */

#ifndef _eap_h_
#define _eap_h_

/* This marks the start of a packed structure section. */
#include <packed_section_start.h>

/* EAP packet format */
typedef BWL_PRE_PACKED_STRUCT struct {
	unsigned char code;	/* EAP code */
	unsigned char id;	/* Current request ID */
	unsigned short length;	/* Length including header */
	unsigned char type;	/* EAP type (optional) */
	unsigned char data[1];	/* Type data (optional) */
} BWL_POST_PACKED_STRUCT eap_header_t;

#define EAP_HEADER_LEN 4

/* EAP codes */
#define EAP_REQUEST	1
#define EAP_RESPONSE	2
#define EAP_SUCCESS	3
#define EAP_FAILURE	4

/* EAP types */
#define EAP_IDENTITY		1
#define EAP_NOTIFICATION	2
#define EAP_NAK			3
#define EAP_MD5			4
#define EAP_OTP			5
#define EAP_GTC			6
#define EAP_TLS			13
#define EAP_EXPANDED		254
#define BCM_EAP_SES		10
#define BCM_EAP_EXP_LEN		12  /* EAP_LEN 5 + 3 bytes for SMI ID + 4 bytes for ven type */
#define BCM_SMI_ID		0x113d
#define WFA_VENDOR_SMI	0x009F68

/* This marks the end of a packed structure section. */
#include <packed_section_end.h>

#endif /* _eap_h_ */
