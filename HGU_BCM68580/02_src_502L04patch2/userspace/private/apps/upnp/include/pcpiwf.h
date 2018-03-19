/*
 * Copyright (C) 2010  Internet Systems Consortium, Inc. ("ISC")
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: pcpiwf.h 1100 2011-01-09 17:14:51Z fdupont $ */

/* pseudo-firewall stuff for PCP */
#include <sys/queue.h>
#include "pcp.h"
#include "upnp.h"

#define PCP_NOT_READY	(100)
#define PCP_RENEW_STATE	(101)
#define PCP_DEFAULT_LEASETIME   4294967295 //Lifetime of infinite UPnP LeaseDuration
#define TTYPE_PCP_RENEW   4

/*
 * draft-ietf-pcp-upnp-igd-interworking:
 * UPnP IGD-PCP: Error mapping
 *
 * Only NOT_AUTHORIZED and CANNOT_PROVIDE_EXTERNAL are ConflictInMappingEntry
 * Others are ActionFailed
 */
#define IGD_PCP_ERROR_11 718 //CANNOT_PROVIDE_EXTERNAL<->ConflictInMappingEntry
#define IGD_PCP_ERROR_02 718 //NOT_AUTHORIZED <-> ConflictInMappingEntry
#define IGD_PCP_ERROR_OTHER 501 //Others <-> ActionFailed

#if 0
/* timeouts */
struct timeout {
	LIST_ENTRY(timeout) chain;
	struct timeval when;
	void (*action)(void *);
	void *arg;
};
extern LIST_HEAD(timeout_list, timeout) pcp_timeouts;
#endif

/* transactions */
struct transaction {
	/* header */
	LIST_ENTRY(transaction) chain;		/* chain */
	LIST_ENTRY(transaction) renew_chain;		/* chain */
	void (*handler)(struct transaction *);	/* response receive handler */
	int type;				/* type */
    timer_t tr;
    UFILE *up;
    PService psvc;
    PAction ac;
    pvar_entry_t args;
    int nargs;
	/* common part */
	pcp_t pcp;				/* PCP library handler */
	pcp_response_t response;		/* PCP library response */
	int status;				/* PCP library result */
//	struct timeout retrans;			/* retransmit timeout */
};

extern LIST_HEAD(transaction_list, transaction) pcp_transactions, pcp_renew_transactions;

void
pcp_send_request(struct transaction *t);

/* was AsyncGetPublicAddress() because it has this side-effect too */
void
pcp_boot(void);

void pcp_invoke( UFILE *up, PService psvc, PAction ac, pvar_entry_t args, int nargs, char * iaddr, int isAdd);

int
OpenPCPServerSocket(struct transaction *t, bool isRenew);

#if 0
int
OpenPCPSockets(int * sockets);

void
ProcessPCPRequest(int s, const char *pcp_server, const char *pcp_local);
#endif

void
ProcessPCPResponse(struct transaction *t);

/* Temporay to pass the 3.14 toolchain. Need to be removed when the toolchain is fixe */
#define LIST_FOREACH_SAFE(var, head, field, tvar)                                           \
               for ((var) = LIST_FIRST((head));                                                  \
                   (var) && ((tvar) = LIST_NEXT((var), field), 1);                      \
                   (var) = (tvar))

