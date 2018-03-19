/*
    Copyright (c) 2017 Broadcom
    All Rights Reserved

    <:label-BRCM:2017:DUAL/GPL:standard

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.

    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.

    :>
*/

#ifndef _nbuff_pkt_h_
#define _nbuff_pkt_h_

#if defined(BCM_NBUFF_PKT)

#include <typedefs.h>

extern void *nbuff_pktdup(osl_t *osh, void *pkt);
extern void *nbuff_pktdup_cpy(osl_t *osh, void *pkt);
extern void nbuff_pktfree(osl_t *osh, void *pkt, bool send);
extern uchar *nbuff_pktdata(osl_t *osh, void *pkt);
extern uint nbuff_pktlen(osl_t *osh, void *pkt);
extern uint nbuff_pktheadroom(osl_t *osh, void *pkt);
extern uint nbuff_pkttailroom(osl_t *osh, void *pkt);
extern void nbuff_pktsetnext(osl_t *osh, void *pkt, void *x);
extern void *nbuff_pktnext(osl_t *osh, void *pkt);
extern uchar *nbuff_pktpush(osl_t *osh, void *pkt, int bytes);
extern uchar *nbuff_pktpull(osl_t *osh, void *pkt, int bytes);
extern void nbuff_pktsetlen(osl_t *osh, void *pkt, uint len);
extern bool nbuff_pktshared(void *pkt);
extern void *nbuff_pkt_frmnative(osl_t *osh, void *pkt);
extern struct sk_buff *nbuff_pkt_tonative(osl_t *osh, void *pkt);
extern void *nbuff_pktlink(void *pkt);
extern void nbuff_pktsetlink(void *pkt, void *x);
extern uint nbuff_pktprio(void *pkt);
extern void nbuff_pktsetprio(void *pkt, uint x);
extern void *nbuff_pkt_get_tag(void *pkt);
extern int nbuff_pkttag_attach(void *osh, void *pkt);
extern uint nbuff_pktflowid(void *pkt);
extern void nbuff_pktsetflowid(void *pkt, uint x);

/* packet primitives */
#define	PKTATTACHTAG(osh, pkt)		nbuff_pkttag_attach((osh), (pkt))
#define PKTFLOWID(pkt)			nbuff_pktflowid((pkt))
#define PKTSETFLOWID(pkt, x)		nbuff_pktsetflowid((pkt), (x))

#define	PKTGET(osh, len, send)		linux_pktget((osh), (len))
#define PKTDUP(osh, pkt)		nbuff_pktdup((osh), (pkt))
#define PKTDUP_CPY(osh, pkt) 		nbuff_pktdup_cpy((osh), (pkt))

#define PKTLIST_DUMP(osh, buf)          BCM_REFERENCE(osh)

#define	PKTFREE(osh, pkt, send)		nbuff_pktfree((osh), (pkt), (send))

#define PKTDATA(osh, pkt)		nbuff_pktdata((osh), (pkt))
#define	PKTLEN(osh, pkt)		nbuff_pktlen((osh), (pkt))
#define PKTHEADROOM(osh, pkt)		nbuff_pktheadroom((osh), (pkt))
#define PKTTAILROOM(osh, pkt)           nbuff_pkttailroom((osh), (pkt))

#define	PKTNEXT(osh, pkt)		nbuff_pktnext((osh), (pkt))
#define	PKTSETNEXT(osh, pkt, x)		nbuff_pktsetnext((osh), (pkt), (x))	

#define	PKTPUSH(osh, pkt, bytes)	nbuff_pktpush((osh), (pkt), (bytes))
#define PKTPULL(osh, pkt, bytes)	nbuff_pktpull((osh), (pkt), (bytes))
#define	PKTSETLEN(osh, pkt, len)	nbuff_pktsetlen((osh), (pkt), (len))
#define PKTTAG(pkt)			nbuff_pkt_get_tag((pkt))

#define PKTSETPOOL(osh, pkt, x, y)	BCM_REFERENCE(osh)
#define	PKTPOOL(osh, pkt)		({BCM_REFERENCE(osh); BCM_REFERENCE(pkt); FALSE;})
#define PKTFREELIST(pkt)        	PKTLINK(pkt)
#define PKTSETFREELIST(pkt, x)  	PKTSETLINK((pkt), (x))

#ifdef BCMDBG_CTRACE
#define	DEL_CTRACE(zosh, zskb) ({BCM_REFERENCE(zosh); BCM_REFERENCE(zskb);})
#define	UPDATE_CTRACE(zskb, zfile, zline) ({BCM_REFERENCE(zskb); BCM_REFERENCE(zfile); BCM_REFERENCE(zline);})
#define	ADD_CTRACE(zosh, zskb, zfile, zline) ({BCM_REFERENCE(zosh); BCM_REFERENCE(zskb); BCM_REFERENCE(zfile); BCM_REFERENCE(zline);})
#define PKTCALLER(zskb)	({BCM_REFERENCE(zskb);})
#endif /* BCMDBG_CTRACE */

#define PKTSETFWDERBUF(osh, pkt)  ({ BCM_REFERENCE(osh); BCM_REFERENCE(pkt); })
#define PKTCLRFWDERBUF(osh, pkt)  ({ BCM_REFERENCE(osh); BCM_REFERENCE(pkt); })
#define PKTISFWDERBUF(osh, pkt)   ({ BCM_REFERENCE(osh); BCM_REFERENCE(pkt); FALSE;})
#define PKTORPHAN(pkt)            ({BCM_REFERENCE(pkt); 0;})

#define PKTFRMNATIVE(osh, pkt)		nbuff_pkt_frmnative((osh), (pkt))
#define PKTTONATIVE(osh, pkt)		nbuff_pkt_tonative((osl_t *)(osh), (pkt))
#define	PKTLINK(pkt)			nbuff_pktlink((pkt))
#define	PKTSETLINK(pkt, x)		nbuff_pktsetlink((pkt), (x))
#define	PKTPRIO(pkt)			nbuff_pktprio((pkt))
#define	PKTSETPRIO(pkt, x)		nbuff_pktsetprio((pkt), (x))

#define PKTSHARED(pkt)                  nbuff_pktshared((pkt))

#ifdef PKTC
#define CHAINED (1 << 3)
#define PKTSETCHAINED(osh, skb) \
	(IS_SKBUFF_PTR(skb) ? (((struct sk_buff*)(skb))->pktc_flags |= CHAINED) : BCM_REFERENCE(skb))
#define PKTCLRCHAINED(osh, skb) \
	(IS_SKBUFF_PTR(skb) ? (((struct sk_buff*)(skb))->pktc_flags &= (~CHAINED)) : BCM_REFERENCE(skb))
#define PKTISCHAINED(skb) \
	(IS_SKBUFF_PTR(skb) ? (((struct sk_buff*)(skb))->pktc_flags & CHAINED) : FALSE)

/* Use 8 bytes of skb tstamp field to store below info */
struct chain_node {
        struct sk_buff  *link;
        unsigned int    flags:3, pkts:9, bytes:20;
};

#define CHAIN_NODE(skb)         ((struct chain_node*)(((struct sk_buff*)skb)->pktc_cb))
#define PKTCSETATTR(skb, f, p, b) (IS_SKBUFF_PTR(skb) ? ({CHAIN_NODE(skb)->flags = (f); CHAIN_NODE(skb)->pkts = (p); \
                                 CHAIN_NODE(skb)->bytes = (b);}) : BCM_REFERENCE(skb))
#define PKTCCLRATTR(skb)        (IS_SKBUFF_PTR(skb) ? ({CHAIN_NODE(skb)->flags = CHAIN_NODE(skb)->pkts = \
                                 CHAIN_NODE(skb)->bytes = 0;}) : BCM_REFERENCE(skb))
#define PKTCGETATTR(skb)        (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags << 29 | CHAIN_NODE(skb)->pkts << 20 | \
                                 CHAIN_NODE(skb)->bytes) : BCM_REFERENCE(skb))
#define PKTCCNT(skb)            (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->pkts) : 1)
#define PKTCLEN(skb)            (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->bytes) : PKTLEN(skb))
#define PKTCGETFLAGS(skb)       (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags) : BCM_REFERENCE(skb))
#define PKTCSETFLAGS(skb, f)    (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags = (f)) : BCM_REFERENCE(skb))
#define PKTCCLRFLAGS(skb)       (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags = 0) : BCM_REFERENCE(skb))
#define PKTCFLAGS(skb)          (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags) : BCM_REFERENCE(skb))
#define PKTCSETCNT(skb, c)      (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->pkts = (c)) : BCM_REFERENCE(skb))
#define PKTCINCRCNT(skb)        (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->pkts++) : BCM_REFERENCE(skb))
#define PKTCADDCNT(skb, c)      (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->pkts += (c)) : BCM_REFERENCE(skb))
#define PKTCSETLEN(skb, l)      (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->bytes = (l)) : BCM_REFERENCE(skb))
#define PKTCADDLEN(skb, l)      (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->bytes += (l)) : BCM_REFERENCE(skb))
#define PKTCSETFLAG(skb, fb)    (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags |= (fb)) : BCM_REFERENCE(skb))
#define PKTCCLRFLAG(skb, fb)    (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags &= ~(fb)) : BCM_REFERENCE(skb))
#define PKTCLINK(skb)           (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->link) : NULL)
#define PKTSETCLINK(skb, x)     (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->link = (struct sk_buff*)(x)) : BCM_REFERENCE(skb))
#define FOREACH_CHAINED_PKT(skb, nskb) \
	for (; (skb) != NULL; (skb) = (nskb)) \
		if ((nskb) = (PKTISCHAINED(skb) ? PKTCLINK(skb) : NULL), \
			PKTSETCLINK((skb), NULL), 1)
		
#define PKTCFREE(osh, skb, send) \
	(IS_SKBUFF_PTR(skb) ? \
		({do { \
			void *nskb; \
			ASSERT((skb) != NULL); \
			FOREACH_CHAINED_PKT((skb), nskb) { \
				PKTCLRCHAINED((osh), (skb)); \
				PKTCCLRFLAGS((skb)); \
				PKTFREE((osh), (skb), (send)); \
			} \
		} while (0); }) : \
		PKTFREE(osh, skb, send))

#define PKTCENQTAIL(h, t, p) \
	(IS_SKBUFF_PTR(p) ? \
		({do { \
			if ((t) == NULL) { \
				(h) = (t) = (p); \
			} else { \
				PKTSETCLINK((t), (p)); \
				(t) = (p); \
			} \
		} while (0); }) : BCM_REFERENCE(p))

#endif /* PKTC */

#ifdef PKTC_TBL
#define PKTCENQCHAINTAIL(h, t, h1, t1) \
	do { \
		if (((h1) == NULL) || ((t1) == NULL)) \
			break;  \
		if ((t) == NULL) { \
			(h) = (h1); \
			(t) = (t1); \
		} else { \
			PKTSETCLINK((t), (h1)); \
			(t) = (t1); \
		} \
	} while (0)
#endif /* PKTC_TBL */

#endif /* BCM_NBUFF_PKT */

#endif	/* _nbuff_pkt_h_ */
