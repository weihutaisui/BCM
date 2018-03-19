/*
 * Copyright (C) 2010-2011  Internet Systems Consortium, Inc. ("ISC")
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

/* $Id: pcpiwf.c 1253 2011-07-04 14:54:05Z fdupont $ */

/* pseudo-firewall stuff for PCP */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pcpiwf.h"
#include "cms_util.h"

//static uint32_t lasttm;
//static struct timeval last;
//struct timeval now = { 0, 0 };
//int should_send_public_address_change_notif = 0;
in_addr_t external_addr = 0;

struct transaction_list pcp_transactions = { NULL };
struct transaction_list pcp_renew_transactions = { NULL };
//struct timeout_list pcp_timeouts = { NULL };

#define THIRD_PARTY_OPTION
#define TTYPE_SYSTEM    83
#define TTYPE_PCP   80

const char *err_app0_msg = "inet_pton failure";

/*
 * OpenPCPServerSocket() PCP:
 * setup the socket with the PCP server. 
 */
int
OpenPCPServerSocket(struct transaction *t, bool isRenew)
{
    cmsLog_debug("OpenPCPServerSocket");

    /* 
     * Assuming: PCP server is IPv6 address for DS-Lite 
     *           PCP server is IPv4 address for NAT444
     */
    if (mode == PCP_MODE_DSLITE)
    {
	    struct sockaddr_in6 srv6;
        struct sockaddr_in6 loc6;

    	memset(&srv6, 0, sizeof(srv6));
    	srv6.sin6_family = AF_INET6;
    	srv6.sin6_port = htons(PCP_PORT);
    	if (inet_pton(AF_INET6, pcpsrv, &srv6.sin6_addr) <= 0) {
            cmsLog_error("fail convert server addr: %s", pcpsrv);
    		return PCP_ERR_APP0;
    	}
    	memset(&loc6, 0, sizeof(loc6));
    	loc6.sin6_family = AF_INET6;
    	if (inet_pton(AF_INET6, pcplocal, &loc6.sin6_addr) <= 0) {
            cmsLog_error("fail convert local addr: %s", pcplocal);
    		return PCP_ERR_APP0;		
    	}

	    return pcp_init((struct sockaddr *) &srv6,
		    	(struct sockaddr *) &loc6,
    			&t->pcp, isRenew);
    }
    else if (mode == PCP_MODE_NAT444)
    {
	    struct sockaddr_in srv4;
        struct sockaddr_in loc4;

    	memset(&srv4, 0, sizeof(srv4));
    	srv4.sin_family = AF_INET;
    	srv4.sin_port = htons(PCP_PORT);
    	if (inet_pton(AF_INET, pcpsrv, &srv4.sin_addr) <= 0) {
            cmsLog_error("fail convert server addr: %s", pcpsrv);
    		return PCP_ERR_APP0;
    	}
    	memset(&loc4, 0, sizeof(loc4));
    	loc4.sin_family = AF_INET;
    	if (inet_pton(AF_INET, pcplocal, &loc4.sin_addr) <= 0) {
            cmsLog_error("fail convert local addr: %s", pcplocal);
    		return PCP_ERR_APP0;		
    	}

	    return pcp_init((struct sockaddr *) &srv4,
		    	(struct sockaddr *) &loc4,
    			&t->pcp, isRenew);
    }
    else
    {
        cmsLog_error("unknown mode: %d", mode);
    	return PCP_ERR_APP0;		
    }
}

static void renew_handler(timer_t tr, void *arg);
/*
 * After receiving MAP response with PCP_OK, need to set renew timer:
 * 1. Clear UPnP related params and handler in struct transaction
 * 2. set timer
 * 3. process_receive_fds() close socket and delete transaction list
 */
static void pcp_set_renew_timer(struct transaction *t)
{
    struct itimerspec timespec;
    uint32_t req_lifetime;
    uint32_t assigned_lifetime;
    pcp_request_t *ptr;

    memset(&timespec, 0, sizeof(timespec));
    ptr = (pcp_request_t *)(t->pcp.request);
    req_lifetime = ptr->lifetime;
    assigned_lifetime = t->response.lifetime;
    timespec.it_value.tv_sec = (assigned_lifetime * 3) >> 2;

    // Clear some fields in t
    t->up = 0;
    t->psvc = 0;
    t->ac = 0;
    t->args = 0;
    t->nargs = 0;
    t->handler = NULL;
    t->status = PCP_RENEW_STATE;
    if ((t->type & TTYPE_PCP_RENEW) == 0)
    {
        LIST_INSERT_HEAD(&pcp_renew_transactions, t, renew_chain);
    }
    t->type |= TTYPE_PCP_RENEW;

    t->tr = enqueue_event(&timespec, (event_callback_t)renew_handler, (void *)t);
}

static void pcp_renew_handler(struct transaction *t)
{
    if (t->status == PCP_OK)
    {
        pcp_set_renew_timer(t);
    }
}

/*
 * Send MAP request again to renew:
 * Open socket, send request, add transaction list, delete timer
 */
static void renew_handler(timer_t tr, void *arg)
{
    struct transaction *t = (struct transaction *) arg;

    timer_delete(tr);
    if (OpenPCPServerSocket(t, TRUE) != PCP_OK)
    {
        cmsLog_error("cannot open pcp socket");
        free(t);
        return;
    }

    t->handler = pcp_renew_handler;

    LIST_INSERT_HEAD(&pcp_transactions, t, chain);
    pcp_send_request(t);

    return;
}

static void
pcp_boot_handler(struct transaction *t)
{
	if (t->status > PCP_OK)
    {
        cmsLog_debug("status not ok");
		return;
    }
	if (t->status == PCP_OK) {
		/* done */
        /* Fetch external IP address!! */
        memcpy(pcpextaddr, t->pcp.response + PCP_MAP_EXTERNAL_ADDR, 16);
        cmsLog_debug("pcp_boot success");

		return;
	}
    cmsLog_debug("pcp_boot_handler: %s", pcp_strerror(t->status));
#if 0
	/* reset retrans timeout */
	t->retrans.action = pcp_boot_resend;
	t->retrans.arg = t;
	t->retrans.when.tv_sec = now.tv_sec + 60;
	t->retrans.when.tv_usec = now.tv_usec;
	LIST_INSERT_HEAD(&pcp_timeouts, &t->retrans, chain);
#endif
}

static void
pcp_handler(struct transaction *t)
{
    if (t->status < PCP_OK)
    {
        if (t->up)
        {
           soap_error(t->up, SOAP_INVALID_ARGS);
        }

        cmsLog_debug("pcp_handler with error status");
        return;
    }

    if (t->up == 0)
    {
        pcp_boot_handler(t);
    }
    else
    {
        if (t->status == PCP_OK)
        {
            pcp_add_invoke(t->up, t->psvc, t->ac, t->args, t->nargs);

            // Setup PCP renew timer
            pcp_set_renew_timer(t);
        }
        else
        {
            switch (t->status)
            {
                case PCP_NO_EXTERNAL:
                    soap_error(t->up, IGD_PCP_ERROR_11);
                    cmsLog_debug("pcp_handler with PCP_NO_EXTERNAL");
                    return;
                case PCP_NOT_AUTHORIZED:
                    soap_error(t->up, IGD_PCP_ERROR_02);
                    cmsLog_debug("pcp_handler with PCP_NOT_AUTHORIZED");
                    return;
                default:
                    soap_error(t->up, IGD_PCP_ERROR_OTHER);
                    cmsLog_debug("pcp_handler with error %d", t->status);
                    return;
            }
        }
    }
    cmsLog_debug("pcp_handler success exit");
}

void
pcp_boot(void)
{
	struct transaction *t;
	pcp_request_t req;
	pcp_option_t **options = NULL;
    uint8_t lan_addr[16];

    cmsLog_debug("pcp_boot");
	/* must not fail */
	t = (struct transaction *) malloc(sizeof(*t));
	memset(t, 0, sizeof(*t));
	t->type = TTYPE_SYSTEM;
	if (OpenPCPServerSocket(t, FALSE) != PCP_OK) {
		free(t);
        cmsLog_error("fail open PCP server socket");
		return;
	}
    cmsLog_debug("pcp_boot: set pcp req param");
	(void) pcp_setreset(&t->pcp, 1, NULL);
	t->status = PCP_NOT_READY;

	memset(&req, 0, sizeof(req));
	req.opcode = PCP_OPCODE_MAP;

    /* TODO: Should we delete all MAP at bootup time? */
#if 0
	/* 
     * lifetime = 0 for reset previous rules
     * internal address: Address of the tunnel/NATed WAN address
     * ports = 0
     */
    if (mode == PCP_MODE_DSLITE)
    {
    	if (inet_pton(AF_INET6, pcplocal, &req.iaddr) <= 0) {
	    	syslog(LOG_WARNING, "pcp_boot(inet_pton0)");
		    return;
    	}
    }
    else /* PCP_MODE_NAT444 */
    {
        char addrStr[CMS_IPADDR_LENGTH];

        sprintf(addrStr, "::ffff:%s", pcplocal);
    	if (inet_pton(AF_INET6, addrStr, &req.iaddr) <= 0) {
	    	syslog(LOG_WARNING, "pcp_boot(inet_pton1)");
		    return;
    	}
    }

    /* 
     * protocol (all=0)
     * ports (0:0)
     * eaddr = ::(IPv6) / ::ffff:0:0 (IPv4: case of DSLITE and 444)
   	 */
	if (inet_pton(AF_INET6, "::ffff:0:0", &req.eaddr) <= 0) {
	    syslog(LOG_WARNING, "pcp_boot(inet_pton2)");
    	return;
	}

	/* third party TODO: use br0's address? */
	if (inet_pton(AF_INET6, "::ffff:192.168.1.1", &lan_addr) <= 0) 
    {
		syslog(LOG_WARNING, "pcp_boot(inet_pton3)");
    	return;
	}
    /* 
     * TODO: in order to use prefer_failure option, external IP address must
     *       be obtained. At bootup time, send MAP req with a short lifetime
     *       to get that information. Below is the info of MAP req
     *       proto = UDP (according to RFC, zero should be valid?)
     *       iport = 9999 (must be non-zero, randomly choose 9999)
     *       lan_addr = 192.168.1.1 (randomly choose br0's address)
     */
	req.lifetime = 1;
	req.protocol = 17;
	req.iport = 9999;
	req.eport = 0;
#endif

	/* 
     * internal address: Address of the tunnel/NATed WAN address
     */
    if (mode == PCP_MODE_DSLITE)
    {
    	if (inet_pton(AF_INET6, pcplocal, &req.iaddr) <= 0) {
            cmsLog_debug("pton fail for dslite");
		    return;
    	}
    }
    else /* PCP_MODE_NAT444 */
    {
        char addrStr[CMS_IPADDR_LENGTH];

        sprintf(addrStr, "::ffff:%s", pcplocal);
    	if (inet_pton(AF_INET6, addrStr, &req.iaddr) <= 0) {
            cmsLog_debug("pton fail for nat444");
		    return;
    	}
    }

    /* 
     * eaddr = ::(IPv6) / ::ffff:0:0 (IPv4: case of DSLITE and 444)
   	 */
	if (inet_pton(AF_INET6, "::ffff:0:0", &req.eaddr) <= 0) {
        cmsLog_debug("pton fail for eaddr");
    	return;
	}

    /* 
     * TODO: in order to use prefer_failure option, external IP address must
     *       be obtained. At bootup time, send MAP req with a short lifetime
     *       to get that information. Below is the info of MAP req
     *       lifetime = 1
     *       proto = UDP (according to RFC, zero should be valid?)
     *       iport = 9999 (must be non-zero, randomly choose 9999)
     *       lan_addr = 192.168.1.1 (randomly choose br0's address)
     */
	
    /* third party */
	if (inet_pton(AF_INET6, "::ffff:192.168.1.1", &lan_addr) <= 0) 
    {
        cmsLog_debug("pton fail for lan_addr 192.168.1.1");
    	return;
	}
	req.lifetime = 1;
	req.protocol = 17;
	req.iport = 9999;

#ifdef THIRD_PARTY_OPTION
	if (pcp_third_party(&options, lan_addr) != PCP_OK) {
        cmsLog_debug("3rdparty fail");
		return;
	}
#endif
	if (pcp_makerequest(&t->pcp, &req, options) != PCP_OK) {
        cmsLog_debug("makereq fail");
		(void) pcp_close(&t->pcp, FALSE);
		free(t);
		pcp_freeoptions(options);
		return;
	}
	pcp_freeoptions(options);

	LIST_INSERT_HEAD(&pcp_transactions, t, chain);
	t->handler = pcp_handler;
	pcp_send_request(t);
    cmsLog_debug("req sent");
}

void
ProcessPCPResponse(struct transaction *t)
{
    cmsLog_debug("enter");
	t->status = pcp_recvresponse(&t->pcp, &t->response);
	if (t->status < PCP_OK)
    {
        cmsLog_debug("%s", pcp_strerror(t->status));
    }

    //TODO!!
#if 0 
	/* check server reboot */
	if (((t->status == PCP_OK) || (t->status <= PCP_ERR_PROTOBASE)) &&
	    (_pcp_getresponse(&t->pcp, &buf, &len) == PCP_OK))
		get_external_address(buf, len);

	/* cancel retrans */
	if (t->retrans.action)
		LIST_REMOVE(&t->retrans, chain);
	memset(&t->retrans, 0, sizeof(t->retrans));
#endif

    cmsLog_debug("Get one response, call handler");

	if (t->handler != NULL)
		(t->handler)(t);
}

void
pcp_send_request(struct transaction *t)
{
	int ret;

    cmsLog_debug("enter");
	/* set status */
	t->status = PCP_NO_EXTERNAL;
	/* send */
	ret = pcp_sendrequest(&t->pcp);
	if (ret != PCP_OK)
    {
        cmsLog_debug("%s", pcp_strerror(t->status));
    }
#if 0
	/* set retrans timeout */
	t->retrans.action = pcp_resend;
	t->retrans.arg = t;
	/* can't fail */
	(void) pcp_gettimeout(&t->pcp, &t->retrans.when, 0);
	LIST_INSERT_HEAD(&pcp_timeouts, &t->retrans, chain);
#endif
}

extern struct Param *findActionParamByName(PAction ac, char *varName);

void pcp_invoke(UFILE *up, PService psvc, PAction ac, 
                pvar_entry_t args, int nargs, char *iaddr, int isAdd)
{
    struct transaction *t = NULL;
    pcp_request_t req;
    pcp_option_t **options = NULL;
    char *    remoteHost = NULL;
    UINT16    externalPort = 0;
    char *    Protocol = NULL;
    struct Param *pParams;

    cmsLog_debug("%s", isAdd?"Add":"Del");

    t = (struct transaction *)malloc(sizeof(struct transaction));
    if (t == NULL)
    {
        cmsLog_error("cannot malloc transaction");
        return;
    }
    memset(t, 0, sizeof(struct transaction));

    if (OpenPCPServerSocket(t, FALSE) != PCP_OK)
    {
        cmsLog_error("cannot open pcp socket");
        free(t);
        return;
    }

    t->status = PCP_NOT_READY;

    memset(&req, 0, sizeof(req));

    /* Construct Request Header */
    req.ver = PCP_VERSION_IMPL;
    req.opcode = PCP_OPCODE_MAP;
    /* 
     * internal address: WAN IPv6 address for DS-Lite 
     *                   WAN IPv4 address for NAT444
     */
    if (mode == PCP_MODE_DSLITE)
    {
        if (inet_pton(AF_INET6, pcplocal, &req.iaddr) <= 0) {
            cmsLog_error("inet_pton() failed");
            goto bad;
        }
    }
    else /* mode == PCP_MODE_NAT444 */
    {
        char addrStr[CMS_IPADDR_LENGTH];

        sprintf(addrStr, "::ffff:%s", pcplocal);
        if (inet_pton(AF_INET6, addrStr, &req.iaddr) <= 0) {
            cmsLog_error("inet_pton() failed for NAT444");
            goto bad;
        }
    }

    if (isAdd)
    {
        UBOOL8    portMappingEnabled = FALSE;
        UINT16    internalPort = 0;
        char *    internalClient = NULL;
        char *    Description = NULL;
        UINT32    LeaseDuration = PCP_DEFAULT_LEASETIME;
        bool badargs = FALSE;

        /* 
         * validate_invoke() not only validate the parameters
         * but also update parameters in ac
         */
        badargs = validate_invoke(psvc, ac, args, nargs);
        if (badargs) 
        {
            /* if we get here, we are missing some args that should have been there. */
            soap_error( up, SOAP_INVALID_ARGS );
            goto bad;
        }

        pParams = findActionParamByName(ac,"NewRemoteHost");
        if(pParams!=NULL){
            remoteHost = pParams->value;
        }

        pParams = findActionParamByName(ac,"NewExternalPort");
        if(pParams!=NULL){
            externalPort = (atoi(pParams->value));
        }

        pParams = findActionParamByName(ac,"NewProtocol");
        if(pParams!=NULL){
            Protocol = pParams->value;
        }

        pParams = findActionParamByName(ac,"NewInternalClient");
        if(pParams!=NULL){
            internalClient = pParams->value;
        }

        pParams = findActionParamByName(ac,"NewInternalPort");
        if(pParams!=NULL){
            internalPort = (atoi(pParams->value));
        }

        pParams = findActionParamByName(ac,"NewEnabled");
        if(pParams!=NULL){
            portMappingEnabled= atoi(pParams->value);
        }

        pParams = findActionParamByName(ac,"NewPortMappingDescription");
        if(pParams!=NULL){
            Description = pParams->value;
        }

        pParams = findActionParamByName(ac,"NewLeaseDuration");
        if(pParams!=NULL){
            LeaseDuration= (atoi(pParams->value));
            if(LeaseDuration == 0){
                LeaseDuration = PCP_DEFAULT_LEASETIME;
            }
        } 

        /* Fill in MAP specific fields into pcp_request_t structure */
        req.protocol = strcmp(Protocol, "UDP")?IPPROTO_TCP:IPPROTO_UDP;
        req.iport = internalPort;
        req.eport = externalPort;
        req.lifetime = LeaseDuration;

        {
            /* LAN client address has to be converted to IPv6 mapped addr */
            char addrStr[CMS_IPADDR_LENGTH];
            uint8_t lanAddr[16];

            sprintf(addrStr, "::ffff:%s", internalClient);
            if (inet_pton(AF_INET6, addrStr, &lanAddr) <= 0) {
                cmsLog_error("inet_pton() failed for 3rdParty");
                goto bad;
            }

#ifdef THIRD_PARTY_OPTION
            if (pcp_third_party(&options, lanAddr) != PCP_OK) {
                cmsLog_error("pcp_third_party failed");
                goto bad;
            }
#endif

            /* if remoteHost is set, use it. Otherwise, use pcpextaddr */
            if (!IS_EMPTY_STRING(remoteHost))
            {
                memset(addrStr, 0, CMS_IPADDR_LENGTH);
                sprintf(addrStr, "::ffff:%s", remoteHost);

                if (inet_pton(AF_INET6, addrStr, &req.eaddr) <= 0) {
                    cmsLog_error("inet_pton() failed for eaddr");
                    goto bad;
                }
            }
            else
            {
                memcpy(&req.eaddr, pcpextaddr, 16);
            }
        }

        //PCP TODO: always prefer failure
        if (pcp_prefer_failure(&options) != PCP_OK) {
            cmsLog_error("prefer failure failed");
            goto bad;
        }

        if (pcp_makerequest(&t->pcp, &req, options) != PCP_OK) {
            cmsLog_error("pcp_makerequest failed");
            pcp_freeoptions(options);
            goto bad;
        }
        pcp_freeoptions(options);

        t->up = up;
        t->psvc = psvc;
        t->ac = ac;
        t->args = args;
        t->nargs = nargs;
        t->handler = pcp_handler;
    }
    else
    {
        struct transaction *renew_list, *tmp;

        pParams = findActionParamByName(ac,"NewRemoteHost");
        if(pParams!=NULL){
            remoteHost = pParams->value;
        }

        pParams = findActionParamByName(ac,"NewExternalPort");
        if(pParams!=NULL){
            externalPort = (atoi(pParams->value));
        }

        pParams = findActionParamByName(ac,"NewProtocol");
        if(pParams!=NULL){
            Protocol = pParams->value;
        }

        /* Fill in MAP specific fields into pcp_request_t structure */
        req.protocol = strcmp(Protocol, "UDP")?IPPROTO_TCP:IPPROTO_UDP;
        req.iport = 0;
        req.eport = externalPort;
        req.lifetime = 0;

        {
            char addrStr[CMS_IPADDR_LENGTH];
            uint8_t lanAddr[16];

            sprintf(addrStr, "::ffff:%s", iaddr);
            if (inet_pton(AF_INET6, addrStr, &lanAddr) <= 0) {
                cmsLog_error("inet_pton() failed for 3rdParty");
                goto bad;
            }

#ifdef THIRD_PARTY_OPTION
            if (pcp_third_party(&options, lanAddr) != PCP_OK) {
                cmsLog_error("pcp_third_party failed");
                goto bad;
            }
#endif

            memcpy(&req.eaddr, pcpextaddr, 16);
        }

        if (pcp_makerequest(&t->pcp, &req, options) != PCP_OK) {
            cmsLog_error("pcp_makerequest failed");
            pcp_freeoptions(options);
            goto bad;
        }
        pcp_freeoptions(options);

        t->handler = NULL;

        LIST_FOREACH_SAFE(renew_list, &pcp_renew_transactions, renew_chain, tmp)
        {
            pcp_request_t *ptr;

            ptr = &renew_list->response.assigned;
            cmsLog_debug("RenewList: t<0x%08" PRIxPTR "> proto<%d> eport<%d> t->pcp<0x%08" PRIxPTR "> t->pcp.s<%d>", (uintptr_t)renew_list, ptr->protocol, ptr->eport, (uintptr_t)&renew_list->pcp, renew_list->pcp.s);
            if ((ptr->protocol == req.protocol) && (ptr->eport == req.eport))
            {
                cmsLog_debug("DeletePortMapping received, delete t in renew list accordingly");
                LIST_REMOVE(renew_list, renew_chain);
                timer_delete(renew_list->tr);
                (void) pcp_close(&renew_list->pcp, TRUE);
                pcp_free((void *)renew_list);
                break;
            }
        }
    }

    LIST_INSERT_HEAD(&pcp_transactions, t, chain);
    pcp_send_request(t);
    return;

bad:
    (void) pcp_close(&t->pcp, FALSE);
    free(t);
    return;
}


#if 0
void
get_external_address(uint8_t *buf, uint16_t len)
{
	uint32_t tm, delta;
	in_addr_t addr;
	struct in_addr in;

	if ((len < PCP_LEN_MAP) ||
	    (buf[PCP_VERSION] != 1) ||
	    (buf[PCP_OPCODE] != (PCP_OPCODE_MAP | PCP_OPCODE_RESPONSE))) {
		syslog(LOG_WARNING, "get_external_address: skip");
		return;
	}
	memcpy(&tm, buf + PCP_EPOCH, 4);
	tm = ntohl(tm);
	/* protect against not initialized and clock going backward */
	if ((lasttm != 0) &&
	    ((now.tv_sec > last.tv_sec) ||
	     ((now.tv_sec == last.tv_sec) && (now.tv_usec >= last.tv_usec)))) {
		delta = now.tv_sec - last.tv_sec;
		if ((now.tv_usec - last.tv_usec) >= 500000)
			delta++;
		else if ((now.tv_usec - last.tv_usec) < -500000)
			delta--;
		delta = ((delta << 3) - delta) >> 3;
		if (lasttm + delta > tm + 1) {
			syslog(LOG_ERR, "epoch: server has rebootted");
			exit(0);
		}
	}
	lasttm = tm;
	last = now;
	memcpy(&addr, buf + PCP_MAP_EXTERNAL_ADDR, 4);
	if (addr == 0U) {
		syslog(LOG_WARNING, "no external address: skip");
		return;
	}
	if (external_addr != 0) {
		if (external_addr == addr)
			return;
		syslog(LOG_ERR, "external address: server has rebootted");
		exit(0);
	}
	external_addr = addr;
	in.s_addr = addr;
	syslog(LOG_NOTICE, "new external address '%s'", inet_ntoa(in));
	should_send_public_address_change_notif = 1;
}

static void
pcp_resend(void *arg)
{
	struct transaction *t = (struct transaction *) arg;
	int ret, trycnt;

    cmsLog_debug("enter");
	ret = pcp_gettries(&t->pcp, &trycnt);
	if (ret < PCP_OK) {
		syslog(LOG_WARNING, "pcp_resend(pcp_gettries)");
		t->status = ret;
		return;
	} else if (trycnt > PCP_MAX_TRIES) {
		memset(&t->retrans, 0, sizeof(t->retrans));
		t->status = PCP_ERR_FAILURE;
		if (t->handler != NULL)
			(t->handler)(t);
		return;
	}
	t->status = PCP_NO_EXTERNAL;
	ret = pcp_sendrequest(&t->pcp);
	if (ret != PCP_OK)
		syslog(LOG_WARNING, "pcp_resend: %s", pcp_strerror(ret));
	/* can't fail */
	(void) pcp_gettimeout(&t->pcp, &t->retrans.when, 0);
	LIST_INSERT_HEAD(&pcp_timeouts, &t->retrans, chain);
}

static void
pcp_boot_resend(void *arg)
{
	struct transaction *t = (struct transaction *) arg;
	int ret;

	t->status = PCP_NO_EXTERNAL;
	ret = pcp_sendrequest(&t->pcp);
	if (ret != PCP_OK)
		syslog(LOG_WARNING, "pcp_boot_resend: %s", pcp_strerror(ret));
	t->retrans.when.tv_sec = now.tv_sec + 60;
	t->retrans.when.tv_usec = now.tv_usec;
	LIST_INSERT_HEAD(&pcp_timeouts, &t->retrans, chain);
}

#endif

#if 0 //PCP_PROXY
static int
OpenPCPClientSocket(in_addr_t addr)
{
	int fd;
	struct sockaddr_in sin4;

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0) {
		syslog(LOG_ERR, "socket(pcp): %m");
		goto bad;
	}
	memset(&sin4, 0, sizeof(sin4));
	sin4.sin_family = AF_INET;
	sin4.sin_addr.s_addr = addr;
	sin4.sin_port = htons(PCP_PORT);
	if (bind(fd, (struct sockaddr *) &sin4, sizeof(sin4)) < 0) {
		syslog(LOG_ERR, "bind(pcp): %m");
		goto bad;
	}
	return fd;
    bad:
	if (fd >= 0)
		(void)close(fd);
	return -1;
}

int
OpenPCPSockets(int *sockets)
{
	int i, j;

	for (i = 0; i < n_lan_addr; i++) {
		sockets[i] = OpenPCPClientSocket(lan_addr[i].addr.s_addr);
		if (sockets[i] < 0) {
			for (j = 0; j < i; j++) {
				(void)close(sockets[j]);
				sockets[j] = -1;
			}
			sockets[i] = -1;
			return -1;
		}
	}
	return 0;
}

struct client_request {
	struct transaction t;
	int s;
	struct in_addr client_addr;
	unsigned short client_port;
	struct timeout expire;
};

static void
pcp_request_expire(void *arg)
{
	struct client_request *c = (struct client_request *) arg;

	(void) pcp_close(&c->t.pcp);
	LIST_REMOVE(&c->t, chain);
	if (c->t.retrans.action != NULL)
		LIST_REMOVE(&c->t.retrans, chain);
	free(c);
}

static uint16_t
in_cksum(uint8_t *p, u_int l)
{
	u_int sum = 0;

	while (l > 1) {
		sum += *p++ << 8;
		sum += *p++;
		l -= 2;
	}
	if (l == 1)
		sum += *p << 8;
	sum = ((sum >> 16) & 0xffff) + (sum & 0xffff);
	sum += sum >> 16;
	return htons((uint16_t) ~sum);
}

static void
pcp_icmp(struct client_request *c)
{
	struct sockaddr_in addr;
	uint8_t buf[36];
	socklen_t l;
	uint16_t cksum;
	int r;

	r = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (r < 0) {
		syslog(LOG_ERR, "pcp_icmp: socket");
		return;
	}
	memset(&addr, 0, sizeof(addr));
	l = sizeof(addr);
	if (getsockname(c->s, (struct sockaddr *) &addr, &l) < 0) {
		syslog(LOG_ERR, "pcp_icmp: getsockname");
		return;
	}

	memset(buf, 0, sizeof(buf));
	/* type = UNREACH */
	buf[0] = 3;
	/* code = UNREACH_PORT */
	buf[1] = 3;
	/* checksum (use UDP one), pptr/gwaddr/void */
	/* ip[0] = version | hlen */
	buf[8] = 0x45;
	/* tos, len = 20+8+28 */
	buf[11] = 56;
	/* id, off, ttl */
	buf[16] = 64;
	/* protocol */
	buf[17] = IPPROTO_UDP;
	/* checksum, source */
	memcpy(buf + 20, &c->client_addr, 4);
	/* destination */
	memcpy(buf + 24, &addr.sin_addr, 4);
	/* UDP source port */
	memcpy(buf + 28, &c->client_port, 2);
	/* UDP destionation port */
	memcpy(buf + 30, &addr.sin_port, 2);
	/* UDP length = 8+28 */
	buf[33] = 36;
	/* IP checksum */
	cksum = in_cksum(buf + 8, 20);
	memcpy(buf + 18, &cksum, 2);
	/* UDP checksum (to adjust ICMP one) */
	cksum = in_cksum(buf, sizeof(buf));
	memcpy(buf + 34, &cksum, 2);

	addr.sin_port = 0;
	if (bind(r, (struct sockaddr *) &addr, l) < 0) {
		syslog(LOG_ERR, "pcp_icmp: bind");
		return;
	}
	addr.sin_addr = c->client_addr;
	if (sendto(r, buf, sizeof(buf), 0, (struct sockaddr *) &addr, l) < 0)
		syslog(LOG_ERR, "pcp_icmp: sendto");
	(void) close(r);
}

static void
pcp_request_handler(struct transaction *t)
{
	struct client_request *c = t->parent;
	uint8_t *resp = NULL;
	uint16_t resplen;
	unsigned int optidx;
	pcp_option_t *tp = NULL;
	struct sockaddr_in client;

	if (t->status > PCP_OK)
		return;
	/* special case for recv() syscall error */
	if (t->status == PCP_ERR_RECV) {
		pcp_icmp(c);
		return;
	}
	/* not protocol errors */
	if ((t->status < PCP_OK) && (t->status > PCP_ERR_PROTOBASE))
		return;
	if (_pcp_getresponse(&t->pcp, &resp, &resplen) != PCP_OK) {
		syslog(LOG_ERR, "pcp_request_handler(getresponse)");
		return;
	}
	/* get back client address */
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = c->client_addr.s_addr;
	client.sin_port = c->client_port;
	/* pop third party */
	for (optidx = 0; optidx < t->response.optcnt; optidx++) {
		if (t->response.options[optidx].code != PCP_OPTION_THIRD_PARTY)
			continue;
		if (t->response.options[optidx].length != 4)
			continue;
		tp = &t->response.options[optidx];
		break;
	}
	if ((tp != NULL) && (memcmp(tp->data, &client.sin_addr, 4) == 0)) {
		memcpy(resp + PCP_CLIENT_ADDR, tp->data, 4);
		if (t->response.optoffs[optidx] + 8 > resplen) {
			memmove(resp + t->response.optoffs[optidx],
				resp + t->response.optoffs[optidx] + 8,
				resplen - (t->response.optoffs[optidx] + 8));
		}
		resplen -= 8;
	}
	/* send response back to the client */
	if (sendto(c->s, resp, (size_t) resplen, 0,
		   (struct sockaddr *) &client, sizeof(client)) < 0)
		syslog(LOG_ERR, "sendto(pcp): %m");
}

void
ProcessPCPRequest(int s)
{
	struct transaction *t;
	struct client_request *c;
	unsigned char req[1040];
	struct sockaddr_in client;
	socklen_t clen = sizeof(client);
	char clientstr[INET_ADDRSTRLEN];
	int cc, ret;

	memset(req, 0, sizeof(req));
	cc = recvfrom(s, req, sizeof(req), 0,
		      (struct sockaddr *) &client, &clen);
	if (cc < 0) {
		syslog(LOG_ERR, "recvfrom(pcp): %m");
		return;
	}
	if (cc < 4) {
		syslog(LOG_WARNING, "ProcessPCPRequest(underrun): %d", cc);
		return;
	}
	/* discarding PCP responses silently */
	if ((req[1] & PCP_OPCODE_RESPONSE) != 0)
		return;
	if (!inet_ntop(AF_INET, &client.sin_addr,
		       clientstr, INET_ADDRSTRLEN)) {
		syslog(LOG_ERR, "ProcessPCPRequest(inet_ntop)");
		return;
	}
	syslog(LOG_INFO, "PCP request received from %s:%hu %d bytes",
	       clientstr, ntohs(client.sin_port), cc);
	if (cc > 1024) {
		syslog(LOG_WARNING, "ProcessPCPRequest(overrun): %d", cc);
		/* just let 4 extra octets go through */
		if (cc > 1028)
			cc = 1028;
	}

	/* known? */
	LIST_FOREACH(t, &pcp_transactions, chain) {
		uint8_t *p = NULL;
		uint16_t l;

		if (t->type != TTYPE_PCP)
			continue;
		c = t->parent;
		if ((c->s == s) &&
		    (c->client_addr.s_addr == client.sin_addr.s_addr) &&
		    (c->client_port == client.sin_port) &&
		    (_pcp_getrequest(&t->pcp, &p, &l) == PCP_OK) &&
		    ((int) l == cc) &&
		    (memcmp(p, req, (size_t) l) == 0)) {
			if (t->status <= 0)
				pcp_request_handler(t);
			return;
		}
	}

	/* new one */
	c = (struct client_request *)malloc(sizeof(*c));
	if (c == NULL)
		return;
	memset(c, 0, sizeof(*c));
	/* fill client request structure */
	ret = OpenPCPServerSocket(&c->t, FALSE);
	if (ret != PCP_OK) {
		syslog(LOG_WARNING,
		       "ProcessPCPRequest(init): %s",
		       pcp_strerror(ret));
		free(c);
		return;
	}
	c->t.parent = c;
	c->t.type = TTYPE_PCP;
	c->t.status = PCP_NOT_READY;
	c->s = s;
	c->client_addr.s_addr = client.sin_addr.s_addr;
	c->client_port = client.sin_port;
	if (memcmp(req + PCP_CLIENT_ADDR, &client.sin_addr, 4) == 0) {
		if (inet_pton(AF_INET, "192.0.0.2",
			      req + PCP_CLIENT_ADDR) <= 0) {
			(void) pcp_close(&c->t.pcp);
			free(c);
			return;
		}
		req[cc] = PCP_OPTION_THIRD_PARTY;
		req[cc + 3] = 4;
		memcpy(req + cc + 4, &client.sin_addr, 4);
		cc += 8;
	}
	if (_pcp_setrequest(&c->t.pcp, req, (uint16_t) cc) != PCP_OK) {
		(void) pcp_close(&c->t.pcp);
		free(c);
		return;
	}
	c->expire.when.tv_sec = now.tv_sec + 120;
	c->expire.when.tv_usec = now.tv_usec;
	c->expire.action = pcp_request_expire;
	c->expire.arg = c;
//	LIST_INSERT_HEAD(&pcp_timeouts, &c->expire, chain);

	/* booting */
	if (external_addr == 0) {
		c->t.status = PCP_ERR_RECV;
		pcp_icmp(c);
	}

	/* forward */
	LIST_INSERT_HEAD(&pcp_transactions, &c->t, chain);
	c->t.handler = pcp_request_handler;
	pcp_send_request(&c->t);
}
#endif

