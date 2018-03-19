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

/* $Id: pcp.c 1253 2011-07-04 14:54:05Z fdupont $ */

/*
 * PCP (port-control-protocol) client library body
 *
 * Francis_Dupont@isc.org, December 2010
 *
 * a la miniUPnP libnatpmp
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "pcp.h"
#include "cms_util.h"

LIBSPEC int pcp_debug_offset;
LIBSPEC int pcp_debug_line;

/* Initialize a PCP handler */

int
pcp_init(struct sockaddr *server, struct sockaddr *source, pcp_t *pcp, 
         int isRenew)
{
	struct sockaddr_in *server4 = (struct sockaddr_in *) server;
	struct sockaddr_in6 *server6 = (struct sockaddr_in6 *) server;
	int flags;

	if ((pcp == NULL) || (server == NULL) ||
	    ((source != NULL) && (server->sa_family != source->sa_family)))
    {
		return PCP_ERR_INVAL;
    }

    if (!isRenew)
    {
	    memset(pcp, 0, sizeof(pcp_t));
    }
	pcp->s = socket(server->sa_family == AF_INET6 ? PF_INET6 : PF_INET,
			SOCK_DGRAM, IPPROTO_UDP);

	if (pcp->s < 0)
    {
		return PCP_ERR_SOCKET;
    }
	pcp->usable = 1;

	if (((flags = fcntl(pcp->s, F_GETFL, 0)) < 0) ||
	    (fcntl(pcp->s, F_SETFL, flags | O_NONBLOCK) < 0))
	{
		(void) close(pcp->s);
		pcp->usable = 0;
		return PCP_ERR_SYSCALL;
	}
	if (server->sa_family == AF_INET6) {
		if (server6->sin6_port == 0)
			server6->sin6_port = htons(PCP_PORT);
		if ((source != NULL) &&
		    (bind(pcp->s, source, sizeof(*server6)) < 0)) {
			(void) close(pcp->s);
			pcp->usable = 0;
            cmsLog_error("fail bind source socket6");
			return PCP_ERR_BIND;
		}
		if (connect(pcp->s, server, sizeof(*server6)) < 0) {
			(void) close(pcp->s);
			pcp->usable = 0;
            cmsLog_error("fail connect to server6");
			return PCP_ERR_CONNECT;
		}
	} else {
		if (server4->sin_port == 0)
			server4->sin_port = htons(PCP_PORT);
		if ((source != NULL) &&
		    (bind(pcp->s, source, sizeof(*server4)) < 0)) {
			(void) close(pcp->s);
			pcp->usable = 0;
            cmsLog_error("fail bind source socket4");
			return PCP_ERR_BIND;
		}
		if (connect(pcp->s, server, sizeof(*server4)) < 0) {
			(void) close(pcp->s);
			pcp->usable = 0;
            cmsLog_error("fail connect to server4");
			return PCP_ERR_CONNECT;
		}
	}

    cmsLog_debug("family<IPv%d>", (server->sa_family==AF_INET6)?6:4);
	return PCP_OK;
}

/* close a PCP handler */

int
pcp_close(pcp_t *pcp, int isRenew)
{
	if (pcp == NULL)
		return PCP_ERR_INVAL;
	if (pcp->request != NULL)
		pcp_free(pcp->request);
	pcp->reqlen = 0;
	pcp->request = NULL;
	if (pcp->response != NULL)
		pcp_free(pcp->response);
	pcp->resplen = 0;
	pcp->response = NULL;
	if (pcp->usable == 0)
    {
		return PCP_OK;
    }
	if (!isRenew && close(pcp->s) < 0)
    {
		return PCP_ERR_SYSCALL;
    }
	pcp->usable = 0;
	return PCP_OK;
}

/* Get the socket file descriptor */

int
pcp_getsocket(pcp_t *pcp, int *sock)
{
	if ((pcp == NULL) || (pcp->usable == 0) || (sock == NULL))
		return PCP_ERR_INVAL;
	*sock = pcp->s;
	return PCP_OK;
}

/* Get the number of tries */

int
pcp_gettries(pcp_t *pcp, int *tries)
{
	if ((pcp == NULL) || (tries == NULL))
		return PCP_ERR_INVAL;
	*tries = pcp->tries;
	return PCP_OK;
}

/* Get the timeout for the next retry */
int
pcp_gettimeout(pcp_t *pcp, struct timeval *timeout, int relative)
{
	struct timeval now;

	if ((pcp == NULL) || (timeout == NULL))
		return PCP_ERR_INVAL;
	if (pcp->request == NULL)
		return PCP_ERR_NOREQUEST;
	timeout->tv_sec = pcp->retry.tv_sec;
	timeout->tv_usec = pcp->retry.tv_usec;
	if (relative) {
		if (gettimeofday(&now, NULL) < 0)
			return PCP_ERR_SYSCALL;
		timeout->tv_sec -= now.tv_sec;
		timeout->tv_usec = now.tv_usec;
		if (timeout->tv_usec < 0) {
			timeout->tv_sec -= 1;
			timeout->tv_usec += 1000000;
		}
	}
	return PCP_OK;
}

/* Set the reset flag */
int
pcp_setreset(pcp_t *pcp, int reset, int *old)
{
	if (pcp == NULL)
		return PCP_ERR_INVAL;
	if (old != NULL)
		*old = (int) pcp->reset;
	if (reset)
		pcp->reset = 1;
	else
		pcp->reset = 0;
	return PCP_OK;
}

/* Stack THIRD_PARTY option */
int
pcp_third_party(pcp_option_t ***options, const uint8_t *addr)
{
	pcp_option_t *o, **v = NULL;
	size_t sz;
	unsigned int i;

    cmsLog_debug("pcp_third_party");
	if (options == NULL)
		return PCP_ERR_INVAL;
    /* 
     * TODO: optimize here. 
     * options is just an array of pointers to pcp_option_t structure.
     * There is no need for pcp_option_t *options!!
     */
	if (*options == NULL) {
		sz = sizeof(pcp_option_t *) * PCP_VECOPT_SIZE;
		v = (pcp_option_t **) malloc(sz);
		if (v == NULL)
			return PCP_ERR_NOMEM;
		memset(v, 0, sz);
		*options = v;
	}
	for (i = 0; i < PCP_MAX_OPTIONS; i++)
		if ((*options)[i] == NULL)
			break;
	if (i >= PCP_MAX_OPTIONS)
		return PCP_ERR_TOOMANYOPTS;
	sz = sizeof(pcp_option_t) + PCP_LEN_THIRDPARTY_OPTION;
	o = (pcp_option_t *) pcp_malloc(sz);
	if (o == NULL) {
		if (*options == v) {
			*options = NULL;
			free(v);
		}
		return PCP_ERR_NOMEM;
	}
	o->code = PCP_OPTION_THIRD_PARTY;
	o->length = PCP_LEN_THIRDPARTY_OPTION;
	o->data = (uint8_t *) (o + 1);
	memcpy(o->data, addr, PCP_LEN_THIRDPARTY_OPTION);
	(*options)[i] = o;

	return PCP_OK;
}

/* Stack PREFER_FAILURE option */
int
pcp_prefer_failure(pcp_option_t ***options)
{
	pcp_option_t *o, **v = NULL;
	size_t sz;
	unsigned int i;

    cmsLog_debug("pcp_prefer_failure");
	if (options == NULL)
		return PCP_ERR_INVAL;
	if (*options == NULL) {
		sz = sizeof(pcp_option_t *) * PCP_VECOPT_SIZE;
		v = (pcp_option_t **) pcp_malloc(sz);
		if (v == NULL)
			return PCP_ERR_NOMEM;
		memset(v, 0, sz);
		*options = v;
	}
	for (i = 0; i < PCP_MAX_OPTIONS; i++)
		if ((*options)[i] == NULL)
			break;
	if (i >= PCP_MAX_OPTIONS)
		return PCP_ERR_TOOMANYOPTS;
	sz = sizeof(pcp_option_t);
	o = (pcp_option_t *) pcp_malloc(sz);
	if (o == NULL) {
		if (*options == v) {
			*options = NULL;
			free(v);
		}
		return PCP_ERR_NOMEM;
	}
	o->code = PCP_OPTION_PREF_FAIL;
	o->length = 0;
	(*options)[i] = o;

	return PCP_OK;
}

/* Stack FILTER option */
int
pcp_filter(pcp_option_t ***options, uint8_t *addr, uint16_t port)
{
	pcp_option_t *o, **v = NULL;
	size_t sz;
	unsigned int i;
	uint16_t n16;

    cmsLog_debug("pcp_filter");
	if (options == NULL)
		return PCP_ERR_INVAL;
	if (*options == NULL) {
		sz = sizeof(pcp_option_t *) * PCP_VECOPT_SIZE;
		v = (pcp_option_t **) pcp_malloc(sz);
		if (v == NULL)
			return PCP_ERR_NOMEM;
		memset(v, 0, sz);
		*options = v;
	}
	for (i = 0; i < PCP_MAX_OPTIONS; i++)
		if ((*options)[i] == NULL)
			break;
	if (i >= PCP_MAX_OPTIONS)
		return PCP_ERR_TOOMANYOPTS;
	sz = sizeof(pcp_option_t) + PCP_LEN_FILTER_OPTION;
	o = (pcp_option_t *) pcp_malloc(sz);
	if (o == NULL) {
		if (*options == v) {
			*options = NULL;
			free(v);
		}
		return PCP_ERR_NOMEM;
	}
	o->code = PCP_OPTION_FILTER;
	o->length = PCP_LEN_FILTER_OPTION;
	o->data = (uint8_t *) (o + 1);
    /* TODO: prefix length configuration */
	n16 = htons(port);
	memcpy(o->data + 2, &n16, 2);
	memcpy(o->data + 4, addr, 16);
	(*options)[i] = o;

	return PCP_OK;
}

static int clearrequest(pcp_t *pcp);
static int createrequest(pcp_t *pcp,
			 pcp_request_t *request,
			 pcp_option_t **options);

/* clear/create a request */

int
pcp_makerequest(pcp_t *pcp, pcp_request_t *request, pcp_option_t **options)
{
	int result;

    cmsLog_debug("pcp_makerequest: pcp<0x%08x>", (intptr_t)pcp);

	if (pcp == NULL)
		return PCP_ERR_INVAL;
	result = clearrequest(pcp);
	if ((request == NULL) || (result != PCP_OK))
		return result;

	return createrequest(pcp, request, options);
}

/* Clear request */

static int
clearrequest(pcp_t *pcp)
{
	if (pcp->request != NULL)
		pcp_free(pcp->request);
	pcp->reqlen = 0;
	pcp->request = NULL;
	pcp->tries = 0;
	pcp->retry.tv_sec = 0;
	pcp->retry.tv_usec = 0;
	return PCP_OK;
}

/* Create request */

static int
createrequest(pcp_t *pcp, pcp_request_t *request, pcp_option_t **options)
{
	unsigned int len, off, i;
	uint16_t n16;
    pcp_request_t *ptr;

    cmsLog_debug("createrequest");
	switch (request->opcode) {
	case PCP_OPCODE_MAP:
		off = len = PCP_LEN_MAP;
		break;
	case PCP_OPCODE_PEER:
		off = len = PCP_LEN_PEER;
		break;
	default:
		return PCP_ERR_INVAL;
	}

	if (options != NULL)
		for (i = 0; options[i] != NULL; i++)
			len += (4 + ((options[i]->length + 3) & ~3));
	if (len > 1024)
		return PCP_ERR_INVAL;
	pcp->request = (uint8_t *) pcp_malloc(len);
	if (pcp->request == NULL)
		return PCP_ERR_NOMEM;
	memset(pcp->request, 0, len);
	pcp->reqlen = len;
    ptr = (pcp_request_t *)pcp->request;

	ptr->ver = 1;
	ptr->opcode = request->opcode;
	ptr->lifetime = htonl(request->lifetime);
	memcpy(ptr->iaddr, request->iaddr, 16);

	if (request->opcode == PCP_OPCODE_MAP) 
    {
		pcp->request[PCP_MAP_PROTOCOL] = request->protocol;
		n16 = htons(request->iport);
		memcpy(pcp->request + PCP_MAP_INTERNAL_PORT, &n16, 2);
		n16 = htons(request->eport);
		memcpy(pcp->request + PCP_MAP_EXTERNAL_PORT, &n16, 2);
		memcpy(pcp->request + PCP_MAP_EXTERNAL_ADDR, request->eaddr, 16);
    }
    else /* PCP_OPCODE_PEER */
    {
		pcp->request[PCP_PEER_PROTOCOL] = request->protocol;
		n16 = htons(request->iport);
		memcpy(pcp->request + PCP_PEER_INTERNAL_PORT, &n16, 2);
		n16 = htons(request->eport);
		memcpy(pcp->request + PCP_PEER_EXTERNAL_PORT, &n16, 2);
		memcpy(pcp->request + PCP_PEER_EXTERNAL_ADDR, request->eaddr, 16);
		n16 = htons(request->rport);
		memcpy(pcp->request + PCP_PEER_REMOTE_PORT, &n16, 2);
		memcpy(pcp->request + PCP_PEER_REMOTE_ADDR, request->raddr, 16);
	}

	if (options != NULL)
		for (i = 0; options[i] != NULL; i++) {
			pcp_option_t *opt = options[i];
			unsigned int olen = opt->length;
			
			pcp->request[off] = opt->code;
			n16 = htons(opt->length);
			off += 2;
			memcpy(pcp->request + off, &n16, 2);
			off += 2;
			memcpy(pcp->request + off, opt->data, olen);
			off += (olen + 3) & ~3;
		}

	return PCP_OK;
}

/* Send/resend the current request */

int
pcp_sendrequest(pcp_t *pcp)
{
    cmsLog_debug("enter");
	if ((pcp == NULL) || (pcp->usable == 0))
    {
		return PCP_ERR_INVAL;
    }
	if (pcp->request == NULL)
    {
		return PCP_ERR_NOREQUEST;
    }

	if (send(pcp->s, pcp->request, pcp->reqlen, 0) < 0)
    {
		return PCP_ERR_SEND;
    }
	if (gettimeofday(&pcp->retry, NULL) < 0)
    {
		return PCP_ERR_SYSCALL;
    }
	if (pcp->tries <= PCP_MAX_TRIES)
		pcp->retry.tv_sec += PCP_RETRY_TIMER << pcp->tries;
	else
		pcp->retry.tv_sec += PCP_RETRY_TIMER << PCP_MAX_TRIES;
	pcp->tries++;
    cmsLog_debug("success");
	return PCP_OK;
}

#define BAD(off)					\
	do {						\
		pcp_debug_offset = off;			\
		pcp_debug_line = __LINE__;		\
		return PCP_ERR_RECVBAD;			\
	} while (0)

/* Receive the response to the current request */

int
pcp_recvresponse(pcp_t *pcp, pcp_response_t *response)
{
	uint32_t n32;
	uint16_t n16;
	unsigned int off = 0, optidx = 0;
	int cc;
    pcp_request_t *req_p;
    pcp_response_t *resp_p;

    cmsLog_debug("pcp_recvresponse");

	if ((pcp == NULL) || (pcp->usable == 0) || (response == NULL))
    {
		return PCP_ERR_INVAL;
    }

	memset(response, 0, sizeof(pcp_response_t));
	if (pcp->response == NULL) {
		pcp->response = (uint8_t *) pcp_malloc(PCP_RESPLENGTH);

		if (pcp->response == NULL)
			return PCP_ERR_NOMEM;
	}
	memset(pcp->response, 0, PCP_RESPLENGTH);
	cc = recv(pcp->s, pcp->response, PCP_RESPLENGTH, 0);
	if (cc < 0) {
		if (errno == EWOULDBLOCK)
        {
			return PCP_NETWORK_FAIL;
        }
		return PCP_ERR_RECV;
	} else if (cc > 1024)
    {
		BAD(cc);
    }
	else if ((cc & 3) != 0)
    {
		BAD(cc);
    }

	pcp->resplen = cc;
    req_p = (pcp_request_t *)pcp->request;
    resp_p = (pcp_response_t *)pcp->response;
	pcp_debug_offset = 0;
	pcp_debug_line = __LINE__;

    cmsLog_debug("resplen = %d", cc);

	if (req_p->ver != resp_p->ver)
    {
		BAD(0);
    }
	if (resp_p->opcode != (req_p->opcode | PCP_OPCODE_RESPONSE))
    {
		BAD(1);
    }
    if ((req_p->opcode != PCP_OPCODE_MAP) && (req_p->opcode != PCP_OPCODE_PEER))
    {
		BAD(1);
    }

    /* 
     * Received response data is stored at pcp->response 
     * So we copy the data into internal structure (transaction->response).
     * Addresses and info of opcode specific will be stored in the follows:
     * transaction->response.assigned (TODO: optimize it!)
     */
	response->ver = resp_p->ver;
	response->opcode = resp_p->opcode;
	response->result = resp_p->result;
	memcpy(&n32, &resp_p->lifetime, 4);
	response->lifetime = ntohl(n32);
	memcpy(&n32, &resp_p->epochtime, 4);
	response->epochtime = ntohl(n32);

	if (req_p->opcode == PCP_OPCODE_MAP) 
    {
		if (pcp->response[PCP_MAP_PROTOCOL] != pcp->request[PCP_MAP_PROTOCOL])
        {
			BAD(PCP_MAP_PROTOCOL);
        }

		response->assigned.protocol = pcp->response[PCP_MAP_PROTOCOL];
		if (memcmp(pcp->response + PCP_MAP_INTERNAL_PORT,
		            pcp->request + PCP_MAP_INTERNAL_PORT, 2) != 0)
        {
			BAD(PCP_MAP_INTERNAL_PORT);
        }

		memcpy(&n16, pcp->response + PCP_MAP_INTERNAL_PORT, 2);
		response->assigned.iport = ntohs(n16);
		memcpy(&n16, pcp->response + PCP_MAP_EXTERNAL_PORT, 2);
		response->assigned.eport = ntohs(n16);
		memcpy(&response->assigned.eaddr, 
               pcp->response + PCP_MAP_EXTERNAL_ADDR, 16);
		off = PCP_MAP_EXTERNAL_ADDR + 16;
    }
    else /* PCP_OPCODE_PEER */
    {
		if (pcp->response[PCP_PEER_PROTOCOL] != pcp->request[PCP_PEER_PROTOCOL])
			BAD(PCP_PEER_PROTOCOL);
		response->assigned.protocol = pcp->response[PCP_PEER_PROTOCOL];
		if (memcmp(pcp->response + PCP_PEER_INTERNAL_PORT,
		    	   pcp->request + PCP_PEER_INTERNAL_PORT, 2) != 0)
			BAD(PCP_PEER_INTERNAL_PORT);
		memcpy(&n16, pcp->response + PCP_PEER_INTERNAL_PORT, 2);
		response->assigned.iport = ntohs(n16);
		memcpy(&n16, pcp->response + PCP_PEER_EXTERNAL_PORT, 2);
		response->assigned.eport = ntohs(n16);
		memcpy(&response->assigned.eaddr, 
               pcp->response + PCP_PEER_EXTERNAL_ADDR, 16);
		if (memcmp(pcp->response + PCP_PEER_REMOTE_PORT,
			       pcp->request + PCP_PEER_REMOTE_PORT, 2) != 0)
			BAD(PCP_PEER_REMOTE_PORT);
		memcpy(&n16, pcp->response + PCP_PEER_REMOTE_PORT, 2);
		response->assigned.rport = ntohs(n16);
		memcpy(&response->assigned.raddr,
		       pcp->response + PCP_PEER_REMOTE_ADDR, 16);
		off = PCP_PEER_REMOTE_ADDR + 16;
	}

	while (cc != (int) off) {
		if ((int) off > cc)
        {
			BAD(off);
        }
		if (optidx > PCP_MAX_OPTIONS)
        {
			return PCP_ERR_TOOMANYOPTS;
        }
		response->optoffs[optidx] = off;
		response->options[optidx].code = pcp->response[off];
		off += 2;
		memcpy(&n16, pcp->response + off, 2);
		response->options[optidx].length = ntohs(n16);
		off += 2;
		response->options[optidx].data = pcp->response + off;
		off += response->options[optidx].length;
		off = (off + 3) & ~3;
		optidx++;
	}
	response->optcnt = optidx;
	if (response->result == 0)
    {
        cmsLog_debug("success");
		return PCP_OK;
    }
//	if ((response->assigned.lifetime > PCP_RESET_THRESHOLD) || !pcp->reset)
//    {
//cmsLog_error("fail12");
//		return PCP_ERR_PROTOBASE - response->result;
//    }
	/* transient error: resetting */
	if (gettimeofday(&pcp->retry, NULL) < 0)
    {
		return PCP_ERR_SYSCALL;
    }
	pcp->tries = 0;
	pcp->retry.tv_sec += response->assigned.lifetime;

//TODO: PCP try again!!
	return response->result;
}

/* Translate an error to its human friendly description */
const char *
pcp_strerror(int error)
{
	switch (error) {
	case PCP_OK:
		return "ok";
	case PCP_NETWORK_FAIL:
		return "network failure";
	case PCP_NO_EXTERNAL:
		return "no external";
	case PCP_ERR_INVAL:
		return "invalid arguments";
	case PCP_ERR_NOMEM:
		return "malloc() failed";
	case PCP_ERR_SOCKET:
		return "socket() syscall failed";
	case PCP_ERR_BIND:
		return "bind() syscall failed";
	case PCP_ERR_CONNECT:
		return "connect() syscall failed";
	case PCP_ERR_SEND:
		return "send() syscall failed";
	case PCP_ERR_RECV:
		return "recv() syscall failed";
	case PCP_ERR_SYSCALL:
		return "syscall failed";
	case PCP_ERR_NOREQUEST:
		return "no current request";
	case PCP_ERR_RECVBAD:
		return "bad response";
	case PCP_ERR_TOOMANYOPTS:
		return "too many options";
	case PCP_ERR_FAILURE:
		return "internal failure";
	case PCP_ERR_APP0:
		return err_app0_msg;
	case PCP_ERR_APP1:
		return err_app1_msg;
	case PCP_ERR_APP2:
		return err_app2_msg;
	case PCP_ERR_PROTOBASE:
		return "base for protocol errors (shouldn't happen)";
	case PCP_ERR_UNSUPVERSION:
		return "unsupported version";
	case PCP_ERR_BADREQUEST:
		return "malformed request";
	case PCP_ERR_UNSUPOPCODE:
		return "unsupported opcode";
	case PCP_ERR_UNSUPOPTION:
		return "unsupported option";
	case PCP_ERR_BADOPTION:
		return "malformed option";
	case PCP_ERR_PROCERROR:
		return "processing error";
	case PCP_ERR_SRVOVERLOAD:
		return "overloaded server";
	case PCP_ERR_NETFAILURE:
		return "network failure";
	case PCP_ERR_NORESOURCES:
		return "out of resources";
	case PCP_ERR_UNSUPPROTO:
		return "unsupported protocol";
	case PCP_ERR_NOTAUTH:
		return "not authorized";
	case PCP_ERR_EXQUOTA:
		return "user exceeded quota";
	case PCP_ERR_CANTPROVIDE:
		return "cannot provide external port";
	case PCP_ERR_TOOMANYPEER:
		return "excessive number of remote peers";
	case PCP_ERR_UNAUTH3PTY:
		return "unauthorized third party";
	case PCP_ERR_IMPLICITMAP:
		return "collides with implicit mapping";
	default:
		if (error >= PCP_ERR_PROTOBASE)
			return "unknown protocol error";
		else
			return "unknown error";
	}
}

#if 0
/* Helper: fill the intaddr field from getsockname() */

int
pcp_fillintaddr(pcp_t *pcp, pcp_request_t *request)
{
	struct sockaddr_in *s4;
	struct sockaddr_in6 *s6;
	struct sockaddr_storage ss;
	int fd, ret;
	socklen_t len;
	
    cmsLog_error("pcp_fillintaddr");
	if ((pcp == NULL) || (request == NULL))
		return PCP_ERR_INVAL;
	ret = pcp_getsocket(pcp, &fd);
	if (ret < PCP_OK)
		return ret;
	memset(&ss, 0, sizeof(ss));
	len = sizeof(ss);

	if (getsockname(fd, (struct sockaddr *) &ss, &len) != 0)
		return PCP_ERR_SYSCALL;
	switch (ss.ss_family) {
	case AF_INET:
		s4 = (struct sockaddr_in *) &ss;
		memcpy(&request->intaddr4, &s4->sin_addr, 4);
		return PCP_OK;
	case AF_INET6:
		s6 = (struct sockaddr_in6 *) &ss;
		memcpy(request->intaddr6, &s6->sin6_addr, 16);
		return PCP_OK;
	default:
		return PCP_ERR_FAILURE;
	}
}

/* Synthetic getmapping() */

int
pcp_getmapping(pcp_t *pcp,
	       pcp_request_t *request,
	       pcp_option_t **options,
	       pcp_response_t *response)
{
	int ret, fd, tries;
	fd_set set;
	struct timeval tv;

    cmsLog_debug("pcp_getmapping");
	if ((pcp == NULL) || (response == NULL))
		return PCP_ERR_INVAL;
	ret = pcp_getsocket(pcp, &fd);
	if (ret < PCP_OK)
		return ret;
	if (request != NULL)
		ret = pcp_makerequest(pcp, request, options);
	if (ret < PCP_OK)
		return ret;
	ret = pcp_sendrequest(pcp);
	if (ret < PCP_OK)
		return ret;
	for (;;) {
		ret = pcp_gettries(pcp, &tries);
		if (ret < PCP_OK)
			return ret;
		if (tries > PCP_MAX_TRIES)
			return PCP_ERR_FAILURE;
		FD_ZERO(&set);
		FD_SET(fd, &set);
		ret = pcp_gettimeout(pcp, &tv, 1);
		if (ret < PCP_OK)
			return ret;
		ret = select(fd + 1, &set, NULL, NULL, &tv);
		if (ret < 0)
			return PCP_ERR_SYSCALL;
		if (FD_ISSET(fd, &set)) {
			memset(response, 0, sizeof(*response));
			ret = pcp_recvresponse(pcp, response);
			if (ret == PCP_NO_EXTERNAL)
				continue;
			else
				return ret;
		}
		ret = pcp_sendrequest(pcp);
		if (ret < PCP_OK)
			return ret;
	}
}

/* Synthetix pcp_renewmapping() */

int
pcp_renewmapping(pcp_t *pcp, pcp_response_t *response)
{
	pcp_option_t **options;
	int ret;
	pcp_response_t r;

	if ((pcp == NULL) || (response == NULL))
		return PCP_ERR_INVAL;
	options = NULL;
	ret = pcp_prefer_failure(&options);
	if (ret != PCP_OK)
		return ret;
	memcpy(&r, response, sizeof(r));
	r.result = 0;
	r.epochtime = 0;
	/* optimistic! */
	r.assigned.lifetime *= 2;
	ret = pcp_getmapping(pcp, &r.assigned, options, &r);
	pcp_freeoptions(options);
	if (ret == PCP_OK)
		memcpy(response, &r, sizeof(r));
	return ret;
}

/* Synthetic pcp_delmapping() */

int
pcp_delmapping(pcp_t *pcp, pcp_response_t *response)
{
	int ret;
	pcp_response_t r;

    cmsLog_debug("pcp_delmapping");
	if ((pcp == NULL) || (response == NULL))
		return PCP_ERR_INVAL;
	memcpy(&r, response, sizeof(r));
	r.result = 0;
	r.epochtime = 0;
	r.assigned.lifetime = 0;
	r.assigned.eport = 0;
	ret = pcp_getmapping(pcp, &r.assigned, NULL, &r);
	if (ret == PCP_OK)
		memcpy(response, &r, sizeof(r));
	return ret;
}

/* Internal set request */

int
_pcp_setrequest(pcp_t *pcp, uint8_t *request, uint16_t reqlen)
{
	int result;

	if ((pcp == NULL) || ((request == NULL) && (reqlen != 0)))
		return PCP_ERR_INVAL;
	result = clearrequest(pcp);
	if ((request == NULL) || (result != PCP_OK))
		return result;
	pcp->request = (uint8_t *) pcp_malloc((size_t) reqlen);
	if (pcp->request == NULL)
		return PCP_ERR_NOMEM;
	memcpy(pcp->request, request, (size_t) reqlen);
	pcp->reqlen = reqlen;
	return PCP_OK;
}

/* Internal get request */

int
_pcp_getrequest(pcp_t *pcp, uint8_t **request, uint16_t *reqlen)
{
	if ((pcp == NULL) || (request == NULL) || (reqlen == NULL))
		return PCP_ERR_INVAL;
	*request = pcp->request;
	*reqlen = pcp->reqlen;
	return PCP_OK;
}

/* Internal set response */

int
_pcp_setresponse(pcp_t *pcp, uint8_t *response, uint16_t resplen)
{
	if ((pcp == NULL) || (resplen > PCP_RESPLENGTH) ||
	    ((response == NULL) && (resplen != 0)))
		return PCP_ERR_INVAL;
	pcp->resplen = resplen;
	if (resplen == 0)
		return PCP_OK;
	if (pcp->response == NULL) {
		pcp->response = (uint8_t *) pcp_malloc(PCP_RESPLENGTH);
		if (pcp->response == NULL) {
			pcp->resplen = 0;
			return PCP_ERR_NOMEM;
		}
	}
	memset(pcp->response, 0, PCP_RESPLENGTH);
	memcpy(pcp->response, response, resplen);
	return PCP_OK;
}

/* Internal get response */

int
_pcp_getresponse(pcp_t *pcp, uint8_t **response, uint16_t *resplen)
{
	if ((pcp == NULL) || (response == NULL) || (resplen == NULL))
		return PCP_ERR_INVAL;
	*response = pcp->response;
	*resplen = pcp->resplen;
	return PCP_OK;
}
#endif

/* Weak definition of pcp_malloc()/pcp_free() */

#pragma weak pcp_malloc
#pragma weak pcp_free

void *pcp_malloc(size_t size) { return malloc(size); }
void pcp_free(void *ptr) { free(ptr); }


/* Weak definition of err_app[012]_msg */

#pragma weak err_app0_msg
#pragma weak err_app1_msg
#pragma weak err_app2_msg

const char *err_app0_msg = "user application error 0";
const char *err_app1_msg = "user application error 1";
const char *err_app2_msg = "user application error 2";
