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
 * Configuration and management interface for l2tpd.
 * Each module implements the required RPC xxx_1_svc() callbacks which
 * are called directly by the RPC library.
 */

#include <rpc/pmap_clnt.h>
#include <net/ethernet.h>

#include "usl.h"

#include "l2tp_private.h"
#include "l2tp_rpc.h"

static SVCXPRT	*l2tp_rpc_udp_xprt;
static SVCXPRT	*l2tp_rpc_tcp_xprt;

extern void l2tp_prog_1(struct svc_req *rqstp, register SVCXPRT *transp);

/*****************************************************************************
 * XDR result cleanup.
 * The RPC XDR mechanism provides one entry point xxx_prog_1_free_result()
 * for the application to free data that was allocated for RPC. Since we
 * malloc all strings and variable length data, we must free it here. For
 * clarity, we use a separate function per XDR type.
 *
 * NOTE: when new XDR types are added to l2tp_rpc.x, additional 
 * l2tp_api_xdr_free_xxx() routines must be added here.
 *****************************************************************************/

static void l2tp_api_xdr_free_app_msg_data(caddr_t addr)
{
	struct l2tp_api_app_msg_data *msg = (void *) addr;

	if (msg->build_date != NULL) free(msg->build_date);
	if (msg->build_time != NULL) free(msg->build_time);
	if ((msg->patches.patches_len > 0) && (msg->patches.patches_val != NULL)) free(msg->patches.patches_val);
}

static void l2tp_api_xdr_free_system_msg_data(caddr_t addr)
{
	struct l2tp_api_system_msg_data *msg = (void *) addr;

	if (msg->status.stats.messages.messages_val != NULL) free(msg->status.stats.messages.messages_val);
}

static void l2tp_api_xdr_free_peer_profile_msg_data(caddr_t addr)
{
	struct l2tp_api_peer_profile_msg_data *msg = (void *) addr;

	if (msg->profile_name != NULL) free(msg->profile_name);
	if (OPTSTRING_PTR(msg->default_tunnel_profile_name) != NULL) free(OPTSTRING(msg->default_tunnel_profile_name));
	if (OPTSTRING_PTR(msg->default_session_profile_name) != NULL) free(OPTSTRING(msg->default_session_profile_name));
	if (OPTSTRING_PTR(msg->default_ppp_profile_name) != NULL) free(OPTSTRING(msg->default_ppp_profile_name));
}

static void l2tp_api_xdr_free_peer_profile_list_msg_data(caddr_t addr)
{
	struct l2tp_api_peer_profile_list_msg_data *msg = (void *) addr;
	struct l2tp_api_peer_profile_list_entry *entry = msg->profiles;
	struct l2tp_api_peer_profile_list_entry *tmpe;

	while (entry != NULL) {
		if (entry->profile_name != NULL) free(entry->profile_name);
		tmpe = entry->next;
		free(entry);
		entry = tmpe;
	}
}	

static void l2tp_api_xdr_free_peer_profile_unset_msg_data(caddr_t addr)
{
	struct l2tp_api_peer_profile_unset_msg_data *msg = (void *) addr;

	if (msg->profile_name != NULL) free(msg->profile_name);
}

static void l2tp_api_xdr_free_tunnel_msg_data(caddr_t addr)
{
	struct l2tp_api_tunnel_msg_data *msg = (void *) addr;

	if (OPTSTRING_PTR(msg->tunnel_name) != NULL) free(OPTSTRING(msg->tunnel_name));
	if (OPTSTRING_PTR(msg->state) != NULL) free(OPTSTRING(msg->state));
	if (OPTSTRING_PTR(msg->secret) != NULL) free(OPTSTRING(msg->secret));
	if (OPTSTRING_PTR(msg->host_name) != NULL) free(OPTSTRING(msg->host_name));
	if (OPTSTRING_PTR(msg->tunnel_profile_name) != NULL) free(OPTSTRING(msg->tunnel_profile_name));
	if (OPTSTRING_PTR(msg->peer_profile_name) != NULL) free(OPTSTRING(msg->peer_profile_name));
	if (msg->tiebreaker.tiebreaker_val != NULL) free(msg->tiebreaker.tiebreaker_val);
	if (OPTSTRING_PTR(msg->session_profile_name) != NULL) free(OPTSTRING(msg->session_profile_name));
	if (OPTSTRING_PTR(msg->ppp_profile_name) != NULL) free(OPTSTRING(msg->ppp_profile_name));
	if (OPTSTRING_PTR(msg->interface_name) != NULL) free(OPTSTRING(msg->interface_name));
	if (OPTSTRING_PTR(msg->result_code_message) != NULL) free(OPTSTRING(msg->result_code_message));
	if (OPTSTRING_PTR(msg->peer.host_name) != NULL) free(OPTSTRING(msg->peer.host_name));
	if (OPTSTRING_PTR(msg->peer.vendor_name) != NULL) free(OPTSTRING(msg->peer.vendor_name));
	if (msg->peer.tiebreaker.tiebreaker_val != NULL) free(msg->peer.tiebreaker.tiebreaker_val);
	if (OPTSTRING_PTR(msg->peer.result_code_message) != NULL) free(OPTSTRING(msg->peer.result_code_message));
	if (OPTSTRING_PTR(msg->create_time) != NULL) free(OPTSTRING(msg->create_time));
}

static void l2tp_api_xdr_free_tunnel_list_msg_data(caddr_t addr)
{
	struct l2tp_api_tunnel_list_msg_data *msg = (void *) addr;

	if (msg->tunnel_ids.tunnel_ids_val != NULL) free(msg->tunnel_ids.tunnel_ids_val);
}

static void l2tp_api_xdr_free_tunnel_profile_msg_data(caddr_t addr)
{
	struct l2tp_api_tunnel_profile_msg_data *msg = (void *) addr;

	if (msg->profile_name != NULL) free(msg->profile_name);
	if (OPTSTRING_PTR(msg->peer_profile_name) != NULL) free(OPTSTRING(msg->peer_profile_name));
	if (OPTSTRING_PTR(msg->session_profile_name) != NULL) free(OPTSTRING(msg->session_profile_name));
	if (OPTSTRING_PTR(msg->ppp_profile_name) != NULL) free(OPTSTRING(msg->ppp_profile_name));
	if (OPTSTRING_PTR(msg->secret) != NULL) free(OPTSTRING(msg->secret));
	if (OPTSTRING_PTR(msg->host_name) != NULL) free(OPTSTRING(msg->host_name));
}

static void l2tp_api_xdr_free_tunnel_profile_list_msg_data(caddr_t addr)
{
	struct l2tp_api_tunnel_profile_list_msg_data *msg = (void *) addr;
	struct l2tp_api_tunnel_profile_list_entry *entry = msg->profiles;
	struct l2tp_api_tunnel_profile_list_entry *tmpe;

	while (entry != NULL) {
		if (entry->profile_name != NULL) free(entry->profile_name);
		tmpe = entry->next;
		free(entry);
		entry = tmpe;
	}
}

static void l2tp_api_xdr_free_tunnel_profile_unset_msg_data(caddr_t addr)
{
	struct l2tp_api_tunnel_profile_unset_msg_data *msg = (void *) addr;

	if (msg->profile_name != NULL) free(msg->profile_name);
}


static void l2tp_api_xdr_free_session_msg_data(caddr_t addr)
{
	struct l2tp_api_session_msg_data *msg = (void *) addr;

	if (OPTSTRING_PTR(msg->tunnel_name) != NULL) free(OPTSTRING(msg->tunnel_name));
	if (OPTSTRING_PTR(msg->state) != NULL) free(OPTSTRING(msg->state));
	if (OPTSTRING_PTR(msg->session_name) != NULL) free(OPTSTRING(msg->session_name));
	if (OPTSTRING_PTR(msg->interface_name) != NULL) free(OPTSTRING(msg->interface_name));
	if (OPTSTRING_PTR(msg->user_name) != NULL) free(OPTSTRING(msg->user_name));
	if (OPTSTRING_PTR(msg->user_password) != NULL) free(OPTSTRING(msg->user_password));
	if (OPTSTRING_PTR(msg->priv_group_id) != NULL) free(OPTSTRING(msg->priv_group_id));
	if (OPTSTRING_PTR(msg->profile_name) != NULL) free(OPTSTRING(msg->profile_name));
	if (msg->proxy_auth_challenge.proxy_auth_challenge_val != NULL) free(msg->proxy_auth_challenge.proxy_auth_challenge_val);
	if (msg->proxy_auth_response.proxy_auth_response_val != NULL) free(msg->proxy_auth_response.proxy_auth_response_val);
	if (OPTSTRING_PTR(msg->proxy_auth_name) != NULL) free(OPTSTRING(msg->proxy_auth_name));
	if (OPTSTRING_PTR(msg->calling_number) != NULL) free(OPTSTRING(msg->calling_number));
	if (OPTSTRING_PTR(msg->called_number) != NULL) free(OPTSTRING(msg->called_number));
	if (OPTSTRING_PTR(msg->sub_address) != NULL) free(OPTSTRING(msg->sub_address));
	if (msg->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val != NULL) free(msg->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val);
	if (msg->last_sent_lcp_confreq.last_sent_lcp_confreq_val != NULL) free(msg->last_sent_lcp_confreq.last_sent_lcp_confreq_val);
	if (msg->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val != NULL) free(msg->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val);
	if (OPTSTRING_PTR(msg->ppp_profile_name) != NULL) free(OPTSTRING(msg->ppp_profile_name));
	if (msg->peer.proxy_auth_challenge.proxy_auth_challenge_val != NULL) free(msg->peer.proxy_auth_challenge.proxy_auth_challenge_val);
	if (msg->peer.proxy_auth_response.proxy_auth_response_val != NULL) free(msg->peer.proxy_auth_response.proxy_auth_response_val);
	if (OPTSTRING_PTR(msg->peer.private_group_id) != NULL) free(OPTSTRING(msg->peer.private_group_id));
	if (msg->peer.initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val != NULL) free(msg->peer.initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val);
	if (msg->peer.last_sent_lcp_confreq.last_sent_lcp_confreq_val != NULL) free(msg->peer.last_sent_lcp_confreq.last_sent_lcp_confreq_val);
	if (msg->peer.last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val != NULL) free(msg->peer.last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val);
	if (OPTSTRING_PTR(msg->peer.calling_number) != NULL) free(OPTSTRING(msg->peer.calling_number));
	if (OPTSTRING_PTR(msg->peer.called_number) != NULL) free(OPTSTRING(msg->peer.called_number));
	if (OPTSTRING_PTR(msg->peer.sub_address) != NULL) free(OPTSTRING(msg->peer.sub_address));
	if (OPTSTRING_PTR(msg->create_time) != NULL) free(OPTSTRING(msg->create_time));
}

static void l2tp_api_xdr_free_session_list_msg_data(caddr_t addr)
{
	struct l2tp_api_session_list_msg_data *msg = (void *) addr;

	if (msg->session_ids.session_ids_val != NULL) free(msg->session_ids.session_ids_val);
}

static void l2tp_api_xdr_free_session_profile_msg_data(caddr_t addr)
{
	struct l2tp_api_session_profile_msg_data *msg = (void *) addr;

	if (msg->profile_name != NULL) free(msg->profile_name);
	if (OPTSTRING_PTR(msg->ppp_profile_name) != NULL) free(OPTSTRING(msg->ppp_profile_name));
	if (OPTSTRING_PTR(msg->priv_group_id) != NULL) free(OPTSTRING(msg->priv_group_id));
	
}

static void l2tp_api_xdr_free_session_profile_list_msg_data(caddr_t addr)
{
	struct l2tp_api_session_profile_list_msg_data *msg = (void *) addr;
	struct l2tp_api_session_profile_list_entry *entry = msg->profiles;
	struct l2tp_api_session_profile_list_entry *tmpe;

	while (entry != NULL) {
		if (entry->profile_name != NULL) free(entry->profile_name);
		tmpe = entry->next;
		free(entry);
		entry = tmpe;
	}
}

static void l2tp_api_xdr_free_session_profile_unset_msg_data(caddr_t addr)
{
	struct l2tp_api_session_profile_unset_msg_data *msg = (void *) addr;

	if (msg->profile_name != NULL) free(msg->profile_name);
}


static void l2tp_api_xdr_free_ppp_profile_msg_data(caddr_t addr)
{
	struct l2tp_api_ppp_profile_msg_data *msg = (void *) addr;

	if (msg->profile_name != NULL) free(msg->profile_name);
	if (OPTSTRING_PTR(msg->ip_pool_name) != NULL) free(OPTSTRING(msg->ip_pool_name));
	if (OPTSTRING_PTR(msg->radius_hint) != NULL) free(OPTSTRING(msg->radius_hint));
	if (OPTSTRING_PTR(msg->local_name) != NULL) free(OPTSTRING(msg->local_name));
	if (OPTSTRING_PTR(msg->remote_name) != NULL) free(OPTSTRING(msg->remote_name));
}

static void l2tp_api_xdr_free_ppp_profile_list_msg_data(caddr_t addr)
{
	struct l2tp_api_ppp_profile_list_msg_data *msg = (void *) addr;
	struct l2tp_api_ppp_profile_list_entry *entry = msg->profiles;
	struct l2tp_api_ppp_profile_list_entry *tmpe;

	while (entry != NULL) {
		if (entry->profile_name != NULL) free(entry->profile_name);
		tmpe = entry->next;
		free(entry);
		entry = tmpe;
	}
}

static void l2tp_api_xdr_free_ppp_profile_unset_msg_data(caddr_t addr)
{
	struct l2tp_api_ppp_profile_unset_msg_data *msg = (void *) addr;

	if (msg->profile_name != NULL) free(msg->profile_name);
}


static void l2tp_api_xdr_free_peer_msg_data(caddr_t addr)
{
	/* No allocated memory to free here */
}

static void l2tp_api_xdr_free_peer_list_msg_data(caddr_t addr)
{
	struct l2tp_api_peer_list_msg_data *msg = (void *) addr;
	struct l2tp_api_peer_list_entry *entry = msg->peers;
	struct l2tp_api_peer_list_entry *tmpe;

	while (entry != NULL) {
		tmpe = entry->next;
		free(entry);
		entry = tmpe;
	}
}

static void l2tp_api_xdr_free_user_list_msg_data(caddr_t addr)
{
	struct l2tp_api_user_list_msg_data *msg = (void *) addr;
	struct l2tp_api_user_list_entry *entry = msg->users;
	struct l2tp_api_user_list_entry *tmpe;

	while ((entry != NULL) && (entry->next != NULL)) {
		tmpe = entry->next;
		if (tmpe->user_name != NULL) {
			free(tmpe->user_name);
		}
		if (tmpe->create_time != NULL) {
			free(tmpe->create_time);
		}
		free(entry);
		entry = tmpe;
	}
}

static void l2tp_api_xdr_free_test_msg_data(caddr_t addr)
{
	/* No allocated memory to free here */
}

static void l2tp_api_xdr_free_null(caddr_t addr)
{
}

struct l2tp_api_xdr_free_entry {
	xdrproc_t xdr_proc;
	void (*free_fn)(caddr_t addr);
};

/* Lookup table for matching XDR proc function pointers to the above _free() functions.
 */
static const struct l2tp_api_xdr_free_entry l2tp_api_xdr_free_table[] = {
	{ (xdrproc_t) xdr_l2tp_api_app_msg_data, 			l2tp_api_xdr_free_app_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_system_msg_data, 			l2tp_api_xdr_free_system_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_peer_profile_msg_data, 		l2tp_api_xdr_free_peer_profile_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_peer_profile_list_msg_data, 		l2tp_api_xdr_free_peer_profile_list_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_peer_profile_unset_msg_data,		l2tp_api_xdr_free_peer_profile_unset_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_msg_data, 			l2tp_api_xdr_free_tunnel_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_list_msg_data, 		l2tp_api_xdr_free_tunnel_list_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_profile_msg_data, 		l2tp_api_xdr_free_tunnel_profile_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_profile_list_msg_data, 	l2tp_api_xdr_free_tunnel_profile_list_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_profile_unset_msg_data,	l2tp_api_xdr_free_tunnel_profile_unset_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_session_msg_data, 			l2tp_api_xdr_free_session_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_session_list_msg_data, 		l2tp_api_xdr_free_session_list_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_session_profile_msg_data, 		l2tp_api_xdr_free_session_profile_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_session_profile_list_msg_data, 	l2tp_api_xdr_free_session_profile_list_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_session_profile_unset_msg_data,	l2tp_api_xdr_free_session_profile_unset_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_ppp_profile_msg_data, 		l2tp_api_xdr_free_ppp_profile_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_ppp_profile_list_msg_data, 		l2tp_api_xdr_free_ppp_profile_list_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_ppp_profile_unset_msg_data,		l2tp_api_xdr_free_ppp_profile_unset_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_peer_msg_data, 			l2tp_api_xdr_free_peer_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_peer_list_msg_data, 			l2tp_api_xdr_free_peer_list_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_user_list_msg_data, 			l2tp_api_xdr_free_user_list_msg_data },
	{ (xdrproc_t) xdr_l2tp_api_test_msg_data, 			l2tp_api_xdr_free_test_msg_data },
	{ (xdrproc_t) xdr_optstring,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_auth_mode,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_mode,				l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_ip_addr,				l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_system_config,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_system_msg_stats,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_system_stats,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_system_status,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_peer_list_entry,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_peer_profile_list_entry,		l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_peer,				l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_stats,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_tunnel_profile_list_entry,		l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_session_proxy_auth_type,		l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_session_call_errors,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_session_stats,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_session_peer_info,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_session_type,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_session_profile_list_entry,		l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_ppp_sync_mode,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_ppp_profile_list_entry,		l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_api_test_trigger_type,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_session_delete_1_argument,		l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_session_get_1_argument,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_l2tp_peer_get_1_argument,			l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_void,						l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_short,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_u_short,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_int,						l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_u_int,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_long,						l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_u_long,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_hyper,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_u_hyper,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_longlong_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_u_longlong_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_int8_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_uint8_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_int16_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_uint16_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_int32_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_uint32_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_int64_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_uint64_t,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_bool,						l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_enum,						l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_array,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_bytes,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_opaque,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_string,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_union,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_char,						l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_u_char,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_vector,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_float,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_double,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_reference,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_pointer,					l2tp_api_xdr_free_null },
	{ (xdrproc_t) xdr_wrapstring,					l2tp_api_xdr_free_null },
	{ NULL, 							NULL }
};

/* This function is called by the RPC mechanism to free memory 
 * allocated when sending RPC messages. We are passed a pointer
 * to the XDR parse function so we use that to derive the type 
 * of the structure to be freed. We use a lookup table and separate
 * free() functions per structure type because a switch() statement
 * can't be used to case on pointers and an if-then-else block
 * was error-prone.
 */
int l2tp_prog_1_freeresult (SVCXPRT *xprt, xdrproc_t proc, caddr_t addr)
{
	const struct l2tp_api_xdr_free_entry *entry = &l2tp_api_xdr_free_table[0];

	while (entry->xdr_proc != NULL) {
		if (entry->xdr_proc == proc) {
			(*entry->free_fn)(addr);
			return TRUE;
		}
		entry++;
	}

	l2tp_log(LOG_ERR, "Unimplemented XDR free_result() proc: %p", proc);
	return FALSE;
}

/* Come here when an RPC message is received. We dispatch to the RPC
 * library which does all the hard work.
 */
static void l2tp_api_rpc_msg(int fd, void *arg)
{
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	svc_getreqset(&fds);
}

/* Server callback to check that the request comes from an allowed IP
 * address.  A call to here is inserted in the rpcgen-generated
 * server-side dispatch code by the build process.
 */
int l2tp_api_rpc_check_request(SVCXPRT *xprt)
{
	/* If remote RPC is not enabled and the request is from a 
	 * non-loopback interface, reject the request.
	 */
	if ((!l2tp_opt_remote_rpc) &&
	    ((xprt->xp_raddr.sin_addr.s_addr != htonl(INADDR_LOOPBACK)) &&
	     (xprt->xp_raddr.sin_addr.s_addr != htonl(INADDR_ANY)))) {
		if (l2tp_opt_trace_flags & L2TP_DEBUG_API) {
			l2tp_log(LOG_ERR, "Rejecting RPC request from %s", inet_ntoa(xprt->xp_raddr.sin_addr));
		}
		svcerr_auth(xprt, AUTH_TOOWEAK);
		return -EPERM;
	}

	return 0;
}

/* Come here every time an event is processed in the USL event loop to
 * check for any changes in RPC fds used by the RPC subsystem which
 * allocates and releases file descriptors automatically under the
 * hood as TCP management connnections come and go.
 */
static void l2tp_api_poll_hook(fd_set *fds)
{
	static fd_set my_fds = { { 0, }, };
	static fd_set added_fds = { { 0, }, };
	int fd;
	int result;

	if (memcmp(&svc_fdset, &my_fds, sizeof(my_fds)) != 0) {
		my_fds = svc_fdset;

		result = 0;
		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if ((FD_ISSET(fd, &svc_fdset)) && (!FD_ISSET(fd, fds))) {
				L2TP_DEBUG(L2TP_API, "add fd %d (TCP RPC)", fd);
				result = usl_fd_add_fd(fd, l2tp_api_rpc_msg, l2tp_rpc_tcp_xprt);
				if (result == 0) {
					FD_SET(fd, &added_fds);
				}
				break;
			} else if ((!FD_ISSET(fd, &svc_fdset)) && (FD_ISSET(fd, fds)) && (FD_ISSET(fd, &added_fds))) {
				L2TP_DEBUG(L2TP_API, "remove fd %d (TCP RPC)", fd);
				result = usl_fd_remove_fd(fd);
				if (result == 0) {
					FD_CLR(fd, &added_fds);
				}
				break;
			}
		}
		if (result < 0) {
			l2tp_log(LOG_WARNING, "failed to track fd %d", fd);
		}
	}
}

/*****************************************************************************
 * Init and cleanup
 *****************************************************************************/

void l2tp_api_init(void)
{
	int result;

	usl_fd_poll_hook = l2tp_api_poll_hook;

	/* Register RPC interface */
	l2tp_rpc_udp_xprt = svcudp_create(RPC_ANYSOCK);
	if (l2tp_rpc_udp_xprt == NULL) {
		l2tp_log(LOG_ERR, "unable to register UDP service with RPC");
		exit(1);
	}

	l2tp_rpc_tcp_xprt = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (l2tp_rpc_tcp_xprt == NULL) {
		l2tp_log(LOG_ERR, "unable to register TCP service with RPC");
		exit(1);
	}

	(void) pmap_unset(L2TP_PROG, L2TP_VERSION);
	result = svc_register(l2tp_rpc_udp_xprt, L2TP_PROG, L2TP_VERSION, l2tp_prog_1, IPPROTO_UDP);
	if (result == 0) {	/* UNIX is nice and consistent about error codes ;-) */
		l2tp_log(LOG_ERR, "unable to register UDP RPC program");
		exit(1);
	}
	result = usl_fd_add_fd(l2tp_rpc_udp_xprt->xp_sock, l2tp_api_rpc_msg, l2tp_rpc_udp_xprt);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to register UDP RPC handler");
		exit(1);
	}

	result = svc_register(l2tp_rpc_tcp_xprt, L2TP_PROG, L2TP_VERSION, l2tp_prog_1, IPPROTO_TCP);
	if (result == 0) {
		l2tp_log(LOG_ERR, "unable to register TCP RPC program");
		exit(1);
	}
	result = usl_fd_add_fd(l2tp_rpc_tcp_xprt->xp_sock, l2tp_api_rpc_msg, l2tp_rpc_tcp_xprt);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to register TCP RPC handler");
		exit(1);
	}
}

void l2tp_api_cleanup(void)
{
	if ((l2tp_rpc_udp_xprt != NULL) || (l2tp_rpc_tcp_xprt != NULL)) {
		svc_unregister(L2TP_PROG, L2TP_VERSION);
	}

	if (l2tp_rpc_udp_xprt != NULL) {
		xprt_unregister(l2tp_rpc_udp_xprt);
		svc_destroy(l2tp_rpc_udp_xprt);
	}

	if (l2tp_rpc_tcp_xprt != NULL) {
		xprt_unregister(l2tp_rpc_tcp_xprt);
		svc_destroy(l2tp_rpc_tcp_xprt);
	}


	usl_fd_poll_hook = NULL;
}
