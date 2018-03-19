/*
 * Minimal debug/trace/assert driver definitions for
 * Broadcom UPNP implementation
 *
 * Copyright (c) 2003-2012 Broadcom Corporation
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
 * $Id: upnp_dbg.h,v 1.8.32.2 2003/10/31 21:31:36 mthawani Exp $
 */

#ifndef _upnp_dbg_
#define _upnp_dbg_

//#define BCMDBG 1

#ifdef	BCMDBG
/*
 * upnp_msg_level is a bitvector:
 *	0	0x0001	errors
 *	1	0x0002	function-level tracing
 *	2	0x0004	trace variable eventing 
 *	3	0x0008	trace subscriptions
 *	4	0x0010	informational messages
 *	5	0x0020	detailed debugging crap
 *	6	0x0040	subscription messages
 *	7	0x0080	one-line action trace
 *	8	0x0100	complete action trace
 *	9	0x0200	dump complete rx data
 *	10 	0x0400	dump complete tx data
 *	11 	0x0800	one-line HTTP trace
 *	12 	0x1000	complete HTTP headers
 *	13 	0x2000	trace socket usage
 *	14 	0x4000	trace port mappings
 */
#define	UPNP_ERROR(args)	do { if (!(upnp_msg_level & 0x01)) ; else printf args; fflush(stdout); } while (0)
#define	UPNP_TRACE(args)	if (!(upnp_msg_level & 0x02)) ; else printf args
#define	UPNP_PRPKT(m, b, n) if (!(upnp_msg_level & 0x08)) ; else prhex(m, b, n)
#define	UPNP_INFORM(args)	if (!(upnp_msg_level & 0x10)) ; else printf args
#define	UPNP_TMP(args)	if (!(upnp_msg_level & 0x20)) ; else printf args
#define	UPNP_HTTP(args)	if (!(upnp_msg_level & 0x800)) ; else printf args

#define	UPNP_PREVENT(args) if (!(upnp_msg_level & 0x04)) ; else printf args
#define	UPNP_SUBSCRIBE(args) if (!(upnp_msg_level & 0x08)) ; else printf args
#define	UPNP_SOCKET(args) ((upnp_msg_level & 0x2000) ? printf args : 0)
#define	UPNP_MAPPING(args) if (!(upnp_msg_level & 0x4000)) ; else printf args

/* take care of both one-line action trace and complete action trace */
#define UPNP_ACTION(psvc, ac, args, nargs) if (!(upnp_msg_level & 0x180)) ; else upnp_dumpaction(psvc, ac, args, nargs)
#define UPNP_RESPONSE(ns, ac, args, nargs) if (!(upnp_msg_level & 0x180)) ; else upnp_dumpresponse(ns, ac, args, nargs)
#define UPNP_TRACE_ACTION_ON()	((upnp_msg_level & 0x80) != 0)
#define UPNP_DUMP_ACTION_ON()	((upnp_msg_level & 0x100) != 0)

#define UPNP_MAPPING_ON()	((upnp_msg_level & 0x4000) != 0)
#define UPNP_RESPONSE_ON()	((upnp_msg_level & 0x180) != 0)
#define UPNP_ERROR_ON()	((upnp_msg_level & 0x01) != 0)
#define UPNP_PRHDRS_ON()	((upnp_msg_level & 0x04) != 0)
#define UPNP_PRPKT_ON()	((upnp_msg_level & 0x08) != 0)
#define UPNP_INFORM_ON()	((upnp_msg_level & 0x10) != 0)
#define UPNP_PRINTRX_ON()	((upnp_msg_level & 0x200) != 0)
#define UPNP_PRINTTX_ON()	((upnp_msg_level & 0x400) != 0)
#define UPNP_HTTP_TRACE_ON()	((upnp_msg_level & 0x800) != 0)
#define UPNP_HTTP_HDRS_ON()	((upnp_msg_level & 0x1000) != 0)


#else	/* BCMDBG */
#define	UPNP_MAPPING(args)
#define	UPNP_ERROR(args)
#define	UPNP_TRACE(args)
#define	UPNP_PRHDRS(i, s, h, p, n, l)
#define	UPNP_PRPKT(m, b, n)
#define	UPNP_INFORM(args)
#define UPNP_TRACE_ACTION(svc, ac) 
#define	UPNP_HTTP(args)
#define	UPNP_SOCKET(args) 

#define UPNP_MAPPING_ON()	0
#define UPNP_RESPONSE_ON()	0
#define UPNP_ERROR_ON()		0
#define UPNP_PRHDRS_ON()	0
#define UPNP_PRPKT_ON()		0
#define UPNP_INFORM_ON()	0
#define UPNP_PRINTRX_ON()	0
#define UPNP_PRINTTX_ON()	0
#define UPNP_HTTP_TRACE_ON()	0
#define UPNP_HTTP_HDRS_ON()	0

#define	UPNP_PREVENT(args)
#define	UPNP_SUBSCRIBE(args)

#define UPNP_ACTION(psvc, ac, args, nargs)
#define UPNP_RESPONSE(ns, ac, args, nargs)
#define UPNP_TRACE_ACTION_ON()	0
#define UPNP_DUMP_ACTION_ON()	0

#endif	/* BCMDBG */

#if defined(BCMDBG)
extern int upnp_msg_level;
#endif


#endif /* _upnp_dbg_ */
