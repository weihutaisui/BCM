/*****************************************************************************
 * Copyright (C) 2004,2005,2006,2007,2008 Katalix Systems Ltd
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 *
 *****************************************************************************/

/*
 * Plugin to exercise kernel socket API without spawning pppd processes.
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef aligned_u64
/* should be defined in sys/types.h */
#define aligned_u64 unsigned long long __attribute__((aligned(8)))
#endif
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <linux/if_pppox.h>
#include <linux/ppp_defs.h>
#include <linux/if_ppp.h>
#include <linux/if_pppol2tp.h>

#include "usl.h"
#include "l2tp_private.h"

#ifndef offsetof
#define offsetof(type, field) ((int) ((char *) &((type *) 0)->field))
#endif

/* should be in system's socket.h */
#ifndef SOL_PPPOL2TP
#define SOL_PPPOL2TP	273
#endif

/* One ppp context is used per session. We bump reference counts on
 * the tunnel/session while we hold references to those data
 * structures.
 */
struct ppp_context {
	struct usl_list_head				list;
	uint16_t					tunnel_id;
	uint16_t					session_id;
	uint16_t					peer_tunnel_id;
	uint16_t					peer_session_id;
	struct l2tp_api_ppp_profile_msg_data		*ppp_profile_data;
	uint16_t					mtu;
	uint16_t					mru;
	int						trace_flags;
	int						tunnel_fd;
	int						kernel_fd;
	struct l2tp_tunnel const			*tunnel;
	struct l2tp_session const			*session;
};

static USL_LIST_HEAD(ppp_test_list);

static int ppp_test_delete(struct ppp_context *ppp);
static int ppp_test_close(struct ppp_context *ppp, uint16_t tunnel_id, uint16_t session_id);

/* Used to record previous value of hook so that we can chain them */
static int (*ppp_test_old_session_created_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id);
static int (*ppp_test_old_session_deleted_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id);
static int (*ppp_test_old_session_modified_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id);
static int (*ppp_test_old_session_up_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id, uint16_t peer_tunnel_id, uint16_t peer_session_id);
static int (*ppp_test_old_session_down_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id);

/*****************************************************************************
 * L2TP socket API
 *****************************************************************************/

static int ppp_test_socket_connect(int session_fd, int tunnel_fd, struct sockaddr_in const *addr, 
				   uint16_t tunnel_id, uint16_t session_id, uint16_t peer_tunnel_id, uint16_t peer_session_id)
{
	struct sockaddr_pppol2tp sax;
	int fd;

	L2TP_DEBUG(L2TP_FUNC, "%s: session_fd=%d tunnel_fd=%d tid=%hu sid=%hu peer=%hu/%hu addr=%s port=%hu",
		   __func__, session_fd, tunnel_fd, tunnel_id, session_id,
		   peer_tunnel_id, peer_session_id, inet_ntoa(addr->sin_addr), htons(addr->sin_port));

	/* Note, the target socket must be bound already, else it will not be ready */  
	sax.sa_family = AF_PPPOX;
	sax.sa_protocol = PX_PROTO_OL2TP;
	sax.pppol2tp.pid = 0;
	sax.pppol2tp.fd = tunnel_fd;
	sax.pppol2tp.addr.sin_addr.s_addr = addr->sin_addr.s_addr;
	sax.pppol2tp.addr.sin_port = addr->sin_port;
	sax.pppol2tp.addr.sin_family = AF_INET;
	sax.pppol2tp.s_tunnel  = tunnel_id;
	sax.pppol2tp.s_session = session_id;
	sax.pppol2tp.d_tunnel  = peer_tunnel_id;
	sax.pppol2tp.d_session = peer_session_id;
  
	fd = connect(session_fd, (struct sockaddr *)&sax, sizeof(sax));
	if (fd < 0 )	{
		return -errno;
	}
	return 0;
}

/*****************************************************************************
 * Internal implementation 
 *****************************************************************************/

static struct ppp_context *ppp_test_find_by_id(uint16_t tunnel_id, uint16_t session_id)
{
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	struct ppp_context *ppp;

	L2TP_DEBUG(L2TP_FUNC, "%s: look for %hu/%hu", __func__, tunnel_id, session_id);

	usl_list_for_each(walk, tmp, &ppp_test_list) {
		ppp = usl_list_entry(walk, struct ppp_context, list);
		L2TP_DEBUG(L2TP_FUNC, "%s: at %hu/%hu", __func__, ppp->tunnel_id, ppp->session_id);
		if ((ppp->tunnel_id == tunnel_id) && (ppp->session_id == session_id)) {
			return ppp;
		}
	}

	return NULL;
}

static int ppp_test_open(struct ppp_context *ppp)
{
	int result = 0;
	struct l2tp_session_config const *scfg;
	int session_fd;

	scfg = l2tp_session_get_config(ppp->session);

	session_fd = socket(AF_PPPOX, SOCK_DGRAM, PX_PROTO_OL2TP);
	if (session_fd < 0) {
		l2tp_session_log(ppp->session, L2TP_DATA, LOG_ERR, "sess %hu/%hu: unable to open pppox socket: %m", 
				 ppp->tunnel_id, ppp->session_id);
		result = -errno;
		goto out;
	}
	result = ppp_test_socket_connect(session_fd, ppp->tunnel_fd, l2tp_tunnel_get_peer_addr(ppp->tunnel),
					 ppp->tunnel_id, ppp->session_id, 
					 ppp->peer_tunnel_id, ppp->peer_session_id);
	if (result < 0) {
		l2tp_session_log(ppp->session, L2TP_DATA, LOG_ERR, "sess %hu/%hu: unable to connect pppox socket: %m", 
				 ppp->tunnel_id, ppp->session_id);
		result = -errno;
		close(session_fd);
		goto out;
	}

out:
	L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	return result;
}

/* Called on SESSION_DOWN.
 */
static int ppp_test_close(struct ppp_context *ppp, uint16_t tunnel_id, uint16_t session_id)
{
	if (ppp == NULL) {
		ppp = ppp_test_find_by_id(tunnel_id, session_id);
		if (ppp == NULL) {
			return 0;
		}
	}

	l2tp_session_log(ppp->session, L2TP_DATA, LOG_INFO, "%s: closing", l2tp_session_get_name(ppp->session));
	l2tp_session_close_event(ppp->tunnel_id, ppp->session_id);
	l2tp_session_dec_use_count((void *) ppp->session);
	ppp->session = NULL;

	return 0;
}


/* Get PPP parameter data from the profile.
 */
static int ppp_test_param_defaults(struct ppp_context *ppp, char *ppp_profile_name)
{
	l2tp_api_ppp_profile_msg_data *profile;
	int result = 0;

	profile = calloc(1, sizeof(*profile));
	if (profile == NULL) {
		result = -ENOMEM;
		goto out;
	}

	if (ppp_profile_name == NULL) {
		ppp_profile_name = L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME;
	}

	result = l2tp_ppp_profile_get(ppp_profile_name, profile);
	if (result < 0) {
		L2TP_DEBUG(L2TP_API, "Ppp profile '%s' not found", ppp_profile_name);
		free(profile);
		return result;
	}

	ppp->ppp_profile_data = profile;

	ppp->trace_flags = profile->trace_flags;

out:
	return result;
}

/*****************************************************************************
 * Context management
 *****************************************************************************/

static int ppp_test_delete(struct ppp_context *ppp)
{
	if (ppp->ppp_profile_data != NULL) {
		l2tp_ppp_profile_msg_free(ppp->ppp_profile_data);
	}

	if (ppp->tunnel != NULL) {
		l2tp_tunnel_dec_use_count((void *) ppp->tunnel);
	}
	if (ppp->session != NULL) {
		l2tp_session_dec_use_count((void *) ppp->session);
	}

	usl_list_del(&ppp->list);

#ifdef DEBUG
	memset(ppp, 0xe9, sizeof(*ppp));
#endif
	free(ppp);

	return 0;
}

static int ppp_test_create(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	struct ppp_context *ppp = NULL;
	int result = 0;
	struct l2tp_tunnel const *tunnel;
	struct l2tp_session_config const *scfg;

	ppp = ppp_test_find_by_id(tunnel_id, session_id);
	if (ppp != NULL) {
		/* Nothing to do if already exists - it just means we got multiple create events */
		result = -EEXIST;
		goto out;
	}

	/* Allocate a new ppp context */
	ppp = calloc(1, sizeof(struct ppp_context));
	if (ppp == NULL) {
		result = -ENOMEM;
		l2tp_stats.no_ppp_resources++;
		goto out;
	}

	USL_LIST_HEAD_INIT(&ppp->list);

	tunnel = l2tp_session_get_tunnel(session);
	ppp->tunnel_id = tunnel_id;
	ppp->session_id = session_id;
	l2tp_tunnel_inc_use_count((void *) tunnel);
	ppp->tunnel = tunnel;
	ppp->tunnel_fd = l2tp_tunnel_get_fd(tunnel);
	ppp->kernel_fd = l2tp_xprt_get_kernel_fd(tunnel);
	l2tp_session_inc_use_count((void *) session);
	ppp->session = session;

	/* Fill with values from the specified PPP profile. Use default profile if none is specified. */
	scfg = l2tp_session_get_config(session);
	result = ppp_test_param_defaults(ppp, scfg->ppp_profile_name);
	if (result < 0) {
		goto err;
	}

	/* The PPP session's MTU is derived from the PPP profile and tunnel's MTU */
	ppp->mtu = ppp->mru = l2tp_tunnel_get_mtu(tunnel);
	if (ppp->ppp_profile_data->mtu < ppp->mtu) {
		ppp->mtu = ppp->ppp_profile_data->mtu;
	}
	if (ppp->ppp_profile_data->mru < ppp->mru) {
		ppp->mru = ppp->ppp_profile_data->mru;
	}

	/* Link it to our list of ppps */
	usl_list_add(&ppp->list, &ppp_test_list);

	return 0;

err:
	l2tp_session_dec_use_count((void *) session);
	l2tp_tunnel_dec_use_count((void *) tunnel);
	if (ppp != NULL) {
		ppp_test_delete(ppp);
	}

out:
	return result;
}

/*****************************************************************************
 * Hooks
 * These functions are called by OpenL2TP when certain events occur.
 * We use the events to start/stop pppd.
 * SESSION_CREATED	- create a new context to handle ppp on the session
 * SESSION_UP		- spawn a pppd
 * SESSION_DOWN		- stop (kill) pppd and tell session in case it doesn't
 *			  yet know that ppp has died.
 * SESSION_DELETED	- destroy our ppp context
 *****************************************************************************/

static int ppp_test_session_created(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	int result = 0;

	if (ppp_test_old_session_created_hook != NULL) {
		result = (*ppp_test_old_session_created_hook)(session, tunnel_id, session_id);
	}

	L2TP_DEBUG(L2TP_DATA, "%s: %s: tid=%hu sid=%hu", __func__, l2tp_session_get_name(session),
		   tunnel_id, session_id);

	if (result >= 0) {
		l2tp_session_log(session, L2TP_DATA, LOG_INFO, "%s: creating UNIX pppd context", l2tp_session_get_name(session));

		result = ppp_test_create(session, tunnel_id, session_id);
	}

	return result;
}

static int ppp_test_session_deleted(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	int result = 0;

	if (ppp_test_old_session_deleted_hook != NULL) {
		result = (*ppp_test_old_session_deleted_hook)(session, tunnel_id, session_id);
	}

	L2TP_DEBUG(L2TP_DATA, "%s: %s: tid=%hu sid=%hu", __func__, l2tp_session_get_name(session),
		   tunnel_id, session_id);

	if (result >= 0) {
		struct ppp_context *ppp = ppp_test_find_by_id(tunnel_id, session_id);
		if (ppp != NULL) {
			l2tp_session_log(session, L2TP_DATA, LOG_INFO, "%s: cleaning UNIX pppd context", l2tp_session_get_name(session));
			ppp_test_delete(ppp);
		}
	}

	return result;
}

static int ppp_test_session_modified(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	int result = 0;

	if (ppp_test_old_session_modified_hook != NULL) {
		result = (*ppp_test_old_session_modified_hook)(session, tunnel_id, session_id);
	}

	L2TP_DEBUG(L2TP_API, "%s: tid=%hu sid=%hu", __func__, tunnel_id, session_id);

	/* Parameters of a pppd that is already running cannot be
	 * modified - pppd has no interface to to do. Should we report
	 * an error here?
	 */

	return result;
}

static int ppp_test_session_up(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id, uint16_t peer_tunnel_id, uint16_t peer_session_id)
{
	struct ppp_context *ppp;
	int result = 0;

	if (ppp_test_old_session_up_hook != NULL) {
		result = (*ppp_test_old_session_up_hook)(session, tunnel_id, session_id, peer_tunnel_id, peer_session_id);
	}

	L2TP_DEBUG(L2TP_DATA, "%s: %s: tid=%hu sid=%hu ptid=%hu psid=%hu", __func__, l2tp_session_get_name(session),
		   tunnel_id, session_id, peer_tunnel_id, peer_session_id);

	if (result >= 0) {
		ppp = ppp_test_find_by_id(tunnel_id, session_id);
		if (ppp != NULL) {
			ppp->peer_tunnel_id = peer_tunnel_id;
			ppp->peer_session_id = peer_session_id;
			l2tp_session_log(session, L2TP_DATA, LOG_INFO, "%s: opening kernel ppp", l2tp_session_get_name(session));
			result = ppp_test_open(ppp);
		} else {
			l2tp_session_log(session, L2TP_DATA, LOG_ERR, "%s: ppp context not found", l2tp_session_get_name(session));
			result = -ENOENT;
		}
	}

	return result;
}

static int ppp_test_session_down(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	int result = 0;

	if (ppp_test_old_session_down_hook != NULL) {
		result = (*ppp_test_old_session_down_hook)(session, tunnel_id, session_id);
	}

	L2TP_DEBUG(L2TP_DATA, "%s: %s: tid=%hu sid=%hu", __func__, l2tp_session_get_name(session),
		   tunnel_id, session_id);

	if (result >= 0) {
		result = ppp_test_close(NULL, tunnel_id, session_id);
	}

	return result;
}

/*****************************************************************************
 * L2TP plugin interface
 *****************************************************************************/

const char openl2tp_plugin_version[] = "V0.16";

int openl2tp_plugin_init(void)
{	
	ppp_test_old_session_created_hook = l2tp_session_created_hook;
	ppp_test_old_session_deleted_hook = l2tp_session_deleted_hook;
	ppp_test_old_session_modified_hook = l2tp_session_modified_hook;
	ppp_test_old_session_up_hook = l2tp_session_up_hook;
	ppp_test_old_session_down_hook = l2tp_session_down_hook;

	l2tp_session_created_hook = ppp_test_session_created;
	l2tp_session_deleted_hook = ppp_test_session_deleted;
	l2tp_session_modified_hook = ppp_test_session_modified;
	l2tp_session_up_hook = ppp_test_session_up;
	l2tp_session_down_hook = ppp_test_session_down;

	/* Disable loading default plugin */
	l2tp_inhibit_default_plugin = 1;

	return 0;
}

void openl2tp_plugin_cleanup(void)
{
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	struct ppp_context *ppp;

	l2tp_session_created_hook = ppp_test_old_session_created_hook;
	l2tp_session_deleted_hook = ppp_test_old_session_deleted_hook;
	l2tp_session_modified_hook = ppp_test_old_session_modified_hook;
	l2tp_session_up_hook = ppp_test_old_session_up_hook;
	l2tp_session_down_hook = ppp_test_old_session_down_hook;

	usl_list_for_each(walk, tmp, &ppp_test_list) {
		ppp = usl_list_entry(walk, struct ppp_context, list);
		ppp_test_delete(ppp);
	}
}

