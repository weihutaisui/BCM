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

/* Common code, shared by more than one application */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

#include "usl.h"
#include "l2tp_private.h"
#include "l2tp_rpc.h"


int l2tp_parse_debug_mask(uint32_t *flags, char *debug_arg, int noisy)
{
	unsigned long mask = 0;
	char *end = NULL;
	int index;
	char *str;
	char *strp;
	int len;
	int result = 0;
	int valid;

	static const struct {
		int mask;
		const char *name;
		int min_name_len;
	} debug_flag_names[] = {
		{ L2TP_PROTOCOL, "protocol", 4 },
		{ L2TP_FSM, "fsm", 3 },
		{ L2TP_API, "api", 3 },
		{ L2TP_AVPHIDE, "avp_hide", 5 },
		{ L2TP_AVPDATA, "avp_data", 5 },
		{ L2TP_AVP, "avp_info", 5 },
		{ L2TP_FUNC, "func", 3 },
		{ L2TP_XPRT, "transport", 3 },
		{ L2TP_DATA, "data", 3 },
		{ L2TP_SYSTEM, "system", 3 },
		{ L2TP_PPP, "ppp_control", 3 },
	};

	/* Check for simple case - an integer mask */
	mask = strtoul(debug_arg, &end, 0);
	if ((end != NULL) && (*end == '\0')) {
		goto out;
	}

	/* Check for "all" */
	if (strcasecmp(debug_arg, "all") == 0) {
		mask = -1;
		goto out;
	}

	/* Look for colon-separated args, i.e. fsm,protocol */
	str = debug_arg;
	for (;;) {
		strp = strchr(str, ',');
		if (strp == NULL) {
			len = strlen(str);
		} else {
			len = strp - str;
		}
		if (len == 0) {
			break;
		}
		valid = 0;
		for (index = 0; index < (sizeof(debug_flag_names) / sizeof(debug_flag_names[0])); index++) {
			if (len < debug_flag_names[index].min_name_len) {
				continue;
			}
			if (strncasecmp(str, debug_flag_names[index].name, debug_flag_names[index].min_name_len) == 0) {
				mask |= debug_flag_names[index].mask;
				valid = 1;
				break;
			}
		}
		if (!valid) {
			result = -EINVAL;
			if (noisy) {
				fprintf(stderr, "Unknown trace flag: %-*.*s\n", len, len, str);
			}
			goto err;
		}
		if (strp == NULL) {
			break;
		}
		str = strp + 1;
	}

out:
	*flags = mask;
err:
	return result;
}

const char *l2tp_strerror(int error)
{
	static char unknown_err[30];
	char *str = &unknown_err[0];

	if (error < L2TP_ERR_BASE) {
		return strerror(error);
	}

	switch (error) {
	case L2TP_ERR_PARAM_NOT_MODIFIABLE:
		str = "Parameter not modifiable";
		break;
	case L2TP_ERR_PEER_ADDRESS_MISSING:
		str = "Peer address missing";
		break;
	case L2TP_ERR_PEER_NOT_FOUND:
		str = "Peer not found";
		break;
	case L2TP_ERR_PEER_PROFILE_NOT_FOUND:
		str = "Peer profile not found";
		break;
	case L2TP_ERR_PPP_PROFILE_NOT_FOUND:
		str = "PPP profile not found";
		break;
	case L2TP_ERR_PROFILE_ALREADY_EXISTS:
		str = "Profile already exists";
		break;
	case L2TP_ERR_PROFILE_NAME_ILLEGAL:
		str = "Profile name illegal";
		break;
	case L2TP_ERR_PROFILE_NAME_MISSING:
		str = "Profile name missing";
		break;
	case L2TP_ERR_SESSION_ALREADY_EXISTS:
		str = "Session already exists";
		break;
	case L2TP_ERR_SESSION_ID_ALLOC_FAILURE:
		str = "Session id allocation failure";
		break;
	case L2TP_ERR_SESSION_LIMIT_EXCEEDED:
		str = "Session limit exceeded";
		break;
	case L2TP_ERR_SESSION_NOT_FOUND:
		str = "Session not found";
		break;
	case L2TP_ERR_SESSION_PROFILE_NOT_FOUND:
		str = "Session profile not found";
		break;
	case L2TP_ERR_SESSION_SPEC_MISSING:
		str = "Session id or session name missing";
		break;
	case L2TP_ERR_SESSION_TYPE_BAD:
		str = "Session type invalid";
		break;
	case L2TP_ERR_SESSION_TYPE_ILLEGAL_FOR_TUNNEL:
		str = "Session type illegal for tunnel";
		break;
	case L2TP_ERR_TUNNEL_ADD_ADMIN_DISABLED:
		str = "Tunnel is administratively disabled";
		break;
	case L2TP_ERR_TUNNEL_CREATE_ADMIN_DISABLED:
		str = "Tunnel creation is administratively disabled";
		break;
	case L2TP_ERR_TUNNEL_ALREADY_EXISTS:
		str = "Tunnel already exists";
		break;
	case L2TP_ERR_TUNNEL_ID_ALLOC_FAILURE:
		str = "Tunnel id allocation failure";
		break;
	case L2TP_ERR_TUNNEL_NOT_FOUND:
		str = "Tunnel not found";
		break;
	case L2TP_ERR_TUNNEL_PROFILE_NOT_FOUND:
		str = "Tunnel profile not found";
		break;
	case L2TP_ERR_TUNNEL_SPEC_MISSING:
		str = "Tunnel id or tunnel name missing";
		break;
	case L2TP_ERR_TUNNEL_TOO_MANY_SESSIONS:
		str = "Tunnel too many sessions";
		break;
	case L2TP_ERR_TUNNEL_TOO_MANY_SAME_IP:
		str = "Too many tunnels between same IP addresses";
		break;
	case L2TP_ERR_TUNNEL_LIMIT_EXCEEDED:
		str = "Tunnel limit exceeded";
		break;
	case L2TP_ERR_SESSION_TYPE_NOT_SUPPORTED:
		str = "Session type not supported";
		break;
	case L2TP_ERR_TUNNEL_HIDE_AVPS_NEEDS_SECRET:
		str = "HideAVPs cannot be set without a secret";
		break;
	default:
		sprintf(&unknown_err[0], "Unknown error (%d)", error);
		break;
	}
	
	return str;
}

/*****************************************************************************
 * Context display functions.
 * These are used by the l2tpconfig CLI application and by openl2tpd if 
 * its status-file option is enabled.
 *****************************************************************************/

#ifdef L2TP_FEATURE_LOCAL_STAT_FILE

static int print_trace_flags(FILE *file, int trace_flags, const char *pfx)
{
	return fprintf(file, "%s  trace flags:%s%s%s%s%s%s%s%s%s%s%s%s\n",
		       pfx ? pfx : "",
		       (trace_flags & L2TP_DEBUG_PROTOCOL) ? " PROTOCOL" : "",
		       (trace_flags & L2TP_DEBUG_FSM) ? " FSM" : "",
		       (trace_flags & L2TP_DEBUG_API) ? " API" : "",
		       (trace_flags & L2TP_DEBUG_AVP) ? " AVP" : "",
		       (trace_flags & L2TP_DEBUG_AVP_HIDE) ? " AVPHIDE" : "",
		       (trace_flags & L2TP_DEBUG_AVP_DATA) ? " AVPDATA" : "",
		       (trace_flags & L2TP_DEBUG_FUNC) ? " FUNC" : "",
		       (trace_flags & L2TP_DEBUG_XPRT) ? " XPRT" : "",
		       (trace_flags & L2TP_DEBUG_DATA) ? " DATA" : "",
		       (trace_flags & L2TP_DEBUG_PPP) ? " PPP" : "",
		       (trace_flags & L2TP_DEBUG_SYSTEM) ? " SYSTEM" : "",
		       ((trace_flags & (L2TP_DEBUG_PROTOCOL | L2TP_DEBUG_FSM |
					L2TP_DEBUG_API | L2TP_DEBUG_AVP |
					L2TP_DEBUG_AVP_HIDE | L2TP_DEBUG_AVP_DATA |
					L2TP_DEBUG_FUNC | L2TP_DEBUG_XPRT |
					L2TP_DEBUG_DATA | L2TP_DEBUG_PPP | 
					L2TP_DEBUG_SYSTEM)) == 0) ? " NONE" : "");
}

static void ip_to_string(char *result, uint32_t addr)
{
	struct in_addr ip;
	char *str;

	ip.s_addr = addr;
	str = inet_ntoa(ip);
	if ((str != NULL) && (strcmp(str, "0.0.0.0") != 0)) {
		strcpy(result, str);
	} else {
		strcpy(result, "NOT SET");
	}
}

int l2tp_show_app_version(FILE *file, struct l2tp_api_app_msg_data *app)
{
	int len = 0;
	char lac_features[16];
	char lns_features[16];

	len += fprintf(file, "OpenL2TP V%d.%d, built %s [%s],\n\t%s %s\n",
		       app->major, app->minor, app->build_date, app->build_time,
		       L2TP_APP_COPYRIGHT_INFO, L2TP_APP_VENDOR_INFO);

	lac_features[0] = '\0';
	if ((app->features & (L2TP_API_APP_FEATURE_LAIC_SUPPORT | L2TP_API_APP_FEATURE_LAOC_SUPPORT)) !=
	    (L2TP_API_APP_FEATURE_LAIC_SUPPORT | L2TP_API_APP_FEATURE_LAOC_SUPPORT)) {
		sprintf(&lac_features[0], "(%s%s) ",
			app->features & L2TP_API_APP_FEATURE_LAIC_SUPPORT ? "LAIC/" : "-/",
			app->features & L2TP_API_APP_FEATURE_LAOC_SUPPORT ? "LAOC" : "-");
	}
	lns_features[0] = '\0';
	if ((app->features & (L2TP_API_APP_FEATURE_LNIC_SUPPORT | L2TP_API_APP_FEATURE_LNOC_SUPPORT)) !=
	    (L2TP_API_APP_FEATURE_LNIC_SUPPORT | L2TP_API_APP_FEATURE_LNOC_SUPPORT)) {
		sprintf(&lns_features[0], "(%s%s) ",
			app->features & L2TP_API_APP_FEATURE_LNIC_SUPPORT ? "LNIC/" : "-/",
			app->features & L2TP_API_APP_FEATURE_LNOC_SUPPORT ? "LNOC" : "-");
	}

	len += fprintf(file, "  Features: %s%s%s%s%s%s\n",
		       app->features & L2TP_API_APP_FEATURE_LAC_SUPPORT ? "LAC " : "",
		       app->features & (L2TP_API_APP_FEATURE_LAIC_SUPPORT | L2TP_API_APP_FEATURE_LAOC_SUPPORT) ? lac_features : "",
		       app->features & L2TP_API_APP_FEATURE_LNS_SUPPORT ? "LNS " : "",
		       app->features & (L2TP_API_APP_FEATURE_LNIC_SUPPORT | L2TP_API_APP_FEATURE_LNOC_SUPPORT) ? lns_features : "",
		       app->features & L2TP_API_APP_FEATURE_LOCAL_CONF_FILE ? "CONF " : "",
		       app->features & L2TP_API_APP_FEATURE_LOCAL_STAT_FILE ? "STAT " : "");

	if (app->patches.patches_len > 0) {
		int patch;
		len += fprintf(file, "  Patches: ");
		for (patch = 0; patch < app->patches.patches_len; patch++) {
			len += fprintf(file, "%d ", app->patches.patches_val[patch]);
		}
		len += fprintf(file, "\n");
	}
	if (app->cookie != L2TP_APP_COOKIE) {
		len += fprintf(file, "*** WARNING: CONTROL APPLICATION AND DAEMON ARE OUT OF SYNC. ***\n");
		len += fprintf(file, "*** UNDEFINED BEHAVIOR MAY RESULT. REINSTALL TO FIX.         ***\n\n");
	}

	return len;
}

int l2tp_show_system_config(FILE *file, struct l2tp_api_system_msg_data *sys)
{
	int len = 0;

	len += fprintf(file, "L2TP configuration:\n");
	len += fprintf(file, "  UDP port: %hu\n", sys->config.udp_port);
	len += fprintf(file, "  max tunnels: %u%s, max sessions: %u%s\n", 
	       sys->config.max_tunnels, sys->config.max_tunnels == 0 ? " (unlimited)" : "",
	       sys->config.max_sessions, sys->config.max_sessions == 0 ? " (unlimited)" : "");
	len += fprintf(file, "  drain tunnels: %s\n", sys->config.drain_tunnels ? "YES" : "NO");
	len += fprintf(file, "  tunnel establish timeout: %hu seconds%s\n", 
	       sys->config.tunnel_establish_timeout, sys->config.tunnel_establish_timeout == 0 ? " (unlimited)" : "");
	len += fprintf(file, "  session establish timeout: %hu seconds%s\n", 
	       sys->config.session_establish_timeout, sys->config.session_establish_timeout == 0 ? " (unlimited)" : "");
	len += fprintf(file, "  tunnel persist pend timeout: %hu seconds\n", sys->config.tunnel_persist_pend_timeout);
	len += fprintf(file, "  session persist pend timeout: %hu seconds\n", sys->config.session_persist_pend_timeout);
	len += fprintf(file, "  deny local tunnel creation: %s, deny remote tunnel creation: %s\n",
	       sys->config.deny_local_tunnel_creates ? "YES" : "NO", sys->config.deny_remote_tunnel_creates ? "YES" : "NO");
	len += print_trace_flags(file, sys->config.trace_flags, NULL);

	return len;
}

int l2tp_show_system_status(FILE *file, struct l2tp_api_system_msg_data *sys)
{
	int len = 0;

	len += fprintf(file, "L2TP service status:-\n");
	len += fprintf(file, "  tunnels: %u, sessions: %u\n",
		       sys->status.num_tunnels, sys->status.num_sessions);

	return len;
}

int l2tp_show_system_stats(FILE *file, struct l2tp_api_system_msg_data *sys)
{
	int len = 0;
	int type;
	static const char *msg_names[] = {
		"ILLEGAL", "SCCRQ", "SCCRP", "SCCCN", "STOPCCN", "RESERVED1", "HELLO",
		"OCRQ", "OCRP", "OCCN", "ICRQ", "ICRP", "ICCN", "RESERVED2",
		"CDN", "WEN", "SLI" 
	};

	len += fprintf(file, "L2TP counters:-\n");
	len += fprintf(file, "  Total messages sent: %u, received: %u, retransmitted: %u\n",
		       sys->status.stats.total_sent_control_frames, sys->status.stats.total_rcvd_control_frames,
		       sys->status.stats.total_retransmitted_control_frames);
	len += fprintf(file, "    illegal: %u, unsupported: %u, ignored AVPs: %u, vendor AVPs: %u\n",
		       sys->status.stats.illegal_messages, sys->status.stats.unsupported_messages,
		       sys->status.stats.ignored_avps, sys->status.stats.vendor_avps);
	len += fprintf(file, "  Setup failures: tunnels: %u, sessions: %u\n", 
		       sys->status.stats.tunnel_setup_failures, sys->status.stats.session_setup_failures);
	len += fprintf(file, "  Resource failures: control frames: %u, peers: %u\n"
		       "    tunnels: %u, sessions: %u, ppp: %u\n",
		       sys->status.stats.no_control_frame_resources,
		       sys->status.stats.no_peer_resources,
		       sys->status.stats.no_tunnel_resources,
		       sys->status.stats.no_session_resources,
		       sys->status.stats.no_ppp_resources);
	len += fprintf(file, "  Limit exceeded errors: tunnels: %u, sessions: %u\n", 
		       sys->status.stats.too_many_tunnels, sys->status.stats.too_many_sessions);
	len += fprintf(file, "  Frame errors: short frames: %u, wrong version frames: %u\n"
		       "     unexpected data frames: %u, bad frames: %u\n",
		       sys->status.stats.short_frames, sys->status.stats.wrong_version_frames,
		       sys->status.stats.unexpected_data_frames, sys->status.stats.bad_rcvd_frames);
	len += fprintf(file, "  Internal: authentication failures: %u, message encode failures: %u\n"
		       "     no matching tunnel discards: %u, mismatched tunnel ids: %u\n"
		       "     no matching session_discards: %u, mismatched session ids: %u\n"
		       "     total control frame send failures: %u, event queue fulls: %u\n\n",
		       sys->status.stats.auth_fails, sys->status.stats.encode_message_fails,
		       sys->status.stats.no_matching_tunnel_id_discards, sys->status.stats.mismatched_tunnel_ids,
		       sys->status.stats.no_matching_session_id_discards, sys->status.stats.mismatched_session_ids,
		       sys->status.stats.total_control_frame_send_fails, sys->status.stats.event_queue_full_errors);

	len += fprintf(file, "  Message counters:-\n");
	len += fprintf(file, "%16s %16s %16s %16s\n", "Message", "RX Good", "RX Bad", "TX");
	for (type = 0; type < sys->status.stats.messages.messages_len; type++) {
		if (type == L2TP_API_MSG_TYPE_COUNT) {
			break;
		}
		len += fprintf(file, "%16s %16u %16u %16u\n", msg_names[type], sys->status.stats.messages.messages_val[type].rx,
			       sys->status.stats.messages.messages_val[type].rx_bad, sys->status.stats.messages.messages_val[type].tx);
	}

	return len;
}

int l2tp_show_tunnel(FILE *file, struct l2tp_api_tunnel_msg_data *tun, int config_only, int transport_only)
{
	int len = 0;
	char our_ip[16];
	char peer_ip[16];
	char idstr[32];
	char *ip;
	struct in_addr ip_addr;

	ip_addr.s_addr = tun->peer_addr.s_addr;
	ip = inet_ntoa(ip_addr);
	strcpy(peer_ip, ip);
	ip_addr.s_addr = tun->our_addr.s_addr;
	ip = inet_ntoa(ip_addr);
	strcpy(our_ip, ip);
	idstr[0] = '\0';
	if (tun->config_id != 1) {
		sprintf(&idstr[0], ", config_id %d", tun->config_id);
	}
	len += fprintf(file, "Tunnel %hu, from %s to %s%s:-\n",
		       tun->tunnel_id, our_ip, peer_ip, idstr);
	len += fprintf(file, "  state: %s\n", OPTSTRING_PTR(tun->state));
	if (!transport_only) {
		if (OPTSTRING_PTR(tun->create_time) != NULL) {
			len += fprintf(file, "  created at: %s", OPTSTRING(tun->create_time));
		}
		if (OPTSTRING_PTR(tun->tunnel_name) != NULL) {
			len += fprintf(file, "  administrative name: '%s'\n", OPTSTRING(tun->tunnel_name));
		}
		if (OPTSTRING_PTR(tun->interface_name) != NULL) {
			len += fprintf(file, "  interface name: %s\n", OPTSTRING(tun->interface_name));
		}
		len += fprintf(file, "  created by admin: %s, tunnel mode: %s%s\n", 
			       (tun->created_by_admin) ? "YES" : "NO",
			       (tun->mode == L2TP_API_TUNNEL_MODE_LAC) ? "LAC" : "LNS",
			       (tun->created_by_admin && tun->persist) ? ", persist: YES" : "");
		if (OPTSTRING_PTR(tun->host_name) != NULL) {
			len += fprintf(file, "  local host name: %s\n", OPTSTRING(tun->host_name));
		}
		len += fprintf(file, "  peer tunnel id: %d, host name: %s\n", tun->peer_tunnel_id,
			       (OPTSTRING_PTR(tun->peer.host_name) == NULL) ? "NOT SET" : OPTSTRING(tun->peer.host_name));
		len += fprintf(file, "  UDP ports: local %hu, peer %hu\n",
			       tun->our_udp_port, tun->peer_udp_port);
		len += fprintf(file, "  authorization mode: %s%s, hide AVPs: %s, allow PPP proxy: %s\n",
			       (tun->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_NONE) ? "NONE" :
			       (tun->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_SIMPLE) ? "SIMPLE" :
			       (tun->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE) ? "CHALLENGE" : "??",
			       (tun->stats.using_ipsec) ? "/IPSec" : "",
			       (tun->hide_avps) ? "ON" : "OFF",
			       (tun->allow_ppp_proxy) ? "ON" : "OFF");
		if (OPTSTRING_PTR(tun->secret) != NULL) {
			len += fprintf(file, "  tunnel secret: '%s'\n", OPTSTRING(tun->secret));
		}
		len += fprintf(file, "  session limit: %d, session count: %d\n",
			       tun->max_sessions, tun->num_sessions);
		len += fprintf(file, "  tunnel profile: %s, peer profile: %s\n", 
			       OPTSTRING_PTR(tun->tunnel_profile_name) == NULL ? "NOT SET" : OPTSTRING(tun->tunnel_profile_name),
			       OPTSTRING_PTR(tun->peer_profile_name) == NULL ? "NOT SET" : OPTSTRING(tun->peer_profile_name));
		len += fprintf(file, "  session profile: %s, ppp profile: %s\n",
			       OPTSTRING_PTR(tun->session_profile_name) == NULL ? "NOT SET" : OPTSTRING(tun->session_profile_name),
			       OPTSTRING_PTR(tun->ppp_profile_name) == NULL ? "NOT SET" : OPTSTRING(tun->ppp_profile_name));
		len += fprintf(file, "  hello timeout: %d, retry timeout: %d, idle timeout: %d\n",
			       tun->hello_timeout, tun->retry_timeout, tun->idle_timeout);
		len += fprintf(file, "  rx window size: %d, tx window size: %d, max retries: %d\n",
			       tun->rx_window_size, tun->tx_window_size, tun->max_retries);
		len += fprintf(file, "  use udp checksums: %s\n", (tun->use_udp_checksums) ? "ON" : "OFF");
		len += fprintf(file, "  do pmtu discovery: %s, mtu: %d\n", (tun->do_pmtu_discovery) ? "ON" : "OFF", tun->mtu);
		len += fprintf(file, "  framing capability:%s%s%s, bearer capability:%s%s%s\n",
			       ((tun->framing_cap_sync == 0) && (tun->framing_cap_async == 0)) ? " NONE" : "",
			       (tun->framing_cap_sync) ? " SYNC" : "",
			       (tun->framing_cap_async) ? " ASYNC" : "",
			       ((tun->bearer_cap_digital == 0) && (tun->bearer_cap_analog == 0)) ? " NONE" : "",
			       (tun->bearer_cap_digital) ? " DIGITAL" : "",
			       (tun->bearer_cap_analog) ? " ANALOG" : "");
		len += fprintf(file, "  use tiebreaker: %s\n", (tun->use_tiebreaker) ? "ON" : "OFF");
		if (tun->tiebreaker.tiebreaker_len == 8) {
			len += fprintf(file, "  tiebreaker: %02x %02x %02x %02x %02x %02x %02x %02x\n",
				       tun->tiebreaker.tiebreaker_val[0], tun->tiebreaker.tiebreaker_val[1], 
				       tun->tiebreaker.tiebreaker_val[2], tun->tiebreaker.tiebreaker_val[3], 
				       tun->tiebreaker.tiebreaker_val[4], tun->tiebreaker.tiebreaker_val[5], 
				       tun->tiebreaker.tiebreaker_val[6], tun->tiebreaker.tiebreaker_val[7]);
		}
		if ((tun->result_code_result != 0) || (tun->result_code_error != 0)) {
			len += fprintf(file, "  local error information:-\n");
			len += fprintf(file, "    result code: %hu, error code: %hu\n", 
				       tun->result_code_result, tun->result_code_error);
			if (OPTSTRING_PTR(tun->result_code_message) != NULL) {
				len += fprintf(file, "    error message: %s\n", OPTSTRING(tun->result_code_message));
			}
		}
		if ((tun->peer.result_code_result != 0) || (tun->peer.result_code_error != 0)) {
			len += fprintf(file, "  last error information from peer:-\n");
			len += fprintf(file, "    result code: %hu, error code: %hu\n", 
				       tun->peer.result_code_result, tun->peer.result_code_error);
			if (OPTSTRING_PTR(tun->peer.result_code_message) != NULL) {
				len += fprintf(file, "    last error message: %s\n", OPTSTRING(tun->peer.result_code_message));
			}
		}
		print_trace_flags(file, tun->trace_flags, NULL);
		if (OPTSTRING_PTR(tun->peer.vendor_name) != NULL) {
			len += fprintf(file, "  peer vendor name: %s\n", OPTSTRING(tun->peer.vendor_name));
		}
		len += fprintf(file, "  peer protocol version: %d.%d, firmware %u\n",
			       tun->peer.protocol_version_ver, tun->peer.protocol_version_rev,
			       tun->peer.firmware_revision);
		len += fprintf(file, "  peer framing capability:%s%s%s\n",
			       ((tun->peer.framing_cap_sync == 0) && (tun->peer.framing_cap_async == 0)) ? " NONE" : "",
			       (tun->peer.framing_cap_sync) ? " SYNC" : "",
			       (tun->peer.framing_cap_async) ? " ASYNC" : "");
		len += fprintf(file, "  peer bearer capability:%s%s%s\n",
			       ((tun->peer.bearer_cap_digital == 0) && (tun->peer.bearer_cap_analog == 0)) ? " NONE" : "",
			       (tun->peer.bearer_cap_digital) ? " DIGITAL" : "",
			       (tun->peer.bearer_cap_analog) ? " ANALOG" : "");
		len += fprintf(file, "  peer rx window size: %hu\n", tun->peer.rx_window_size);
		if ((tun->actual_tx_window_size > 0) && (tun->tx_window_size != tun->actual_tx_window_size)) {
			len += fprintf(file, "  negotiated tx window size: %hu\n", tun->actual_tx_window_size);
		}
		if (tun->peer.tiebreaker.tiebreaker_len == 8) {
			len += fprintf(file, "  peer tiebreaker: %02x %02x %02x %02x %02x %02x %02x %02x\n",
				       tun->peer.tiebreaker.tiebreaker_val[0], tun->peer.tiebreaker.tiebreaker_val[1], 
				       tun->peer.tiebreaker.tiebreaker_val[2], tun->peer.tiebreaker.tiebreaker_val[3], 
				       tun->peer.tiebreaker.tiebreaker_val[4], tun->peer.tiebreaker.tiebreaker_val[5], 
				       tun->peer.tiebreaker.tiebreaker_val[6], tun->peer.tiebreaker.tiebreaker_val[7]);
		}
	}
	if (!config_only) {
		len += fprintf(file, "  Transport status:-\n");
		len += fprintf(file, "    ns/nr: %hu/%hu, peer %hu/%hu\n"
			       "    cwnd: %hu, ssthresh: %hu, congpkt_acc: %hu\n",
			       tun->stats.ns, tun->stats.nr, tun->stats.peer_nr, tun->stats.peer_ns, 
			       tun->stats.cwnd, tun->stats.ssthresh, tun->stats.congpkt_acc);
		len += fprintf(file, "  Transport statistics:-\n");
		len += fprintf(file, "    out-of-sequence control/data discards: %llu/%llu\n", 
			       (unsigned long long) tun->stats.control_rx_oos_discards, 
			       (unsigned long long) tun->stats.data_rx_oos_discards);
		len += fprintf(file, "    zlbs tx/txfail/rx: %u/%u/%u\n", tun->stats.tx_zlbs,
			       tun->stats.tx_zlb_fails, tun->stats.rx_zlbs);
		len += fprintf(file, "    retransmits: %u, duplicate pkt discards: %u, data pkt discards: %u\n", 
			       tun->stats.retransmits, tun->stats.duplicate_pkt_discards, 
			       tun->stats.data_pkt_discards);
		len += fprintf(file, "    hellos tx/txfail/rx: %u/%u/%u\n", tun->stats.tx_hellos,
			       tun->stats.tx_hello_fails, tun->stats.rx_hellos);
		len += fprintf(file, "    control rx packets: %llu, rx bytes: %llu\n", 
			       (unsigned long long) tun->stats.control_rx_packets, 
			       (unsigned long long) tun->stats.control_rx_bytes);
		len += fprintf(file, "    control tx packets: %llu, tx bytes: %llu\n", 
			       (unsigned long long) tun->stats.control_tx_packets, 
			       (unsigned long long) tun->stats.control_tx_bytes);
		if (tun->stats.control_rx_oos_packets > 0) {
			len += fprintf(file, "    control rx out-of-sequence packets: %llu\n", 
				       (unsigned long long) tun->stats.control_rx_oos_packets);
		}
		len += fprintf(file, "    data rx packets: %llu, rx bytes: %llu, rx errors: %llu\n", 
			       (unsigned long long) tun->stats.data_rx_packets, 
			       (unsigned long long) tun->stats.data_rx_bytes, 
			       (unsigned long long) tun->stats.data_rx_errors);
		len += fprintf(file, "    data tx packets: %llu, tx bytes: %llu, tx errors: %llu\n", 
			       (unsigned long long) tun->stats.data_tx_packets, 
			       (unsigned long long) tun->stats.data_tx_bytes, 
			       (unsigned long long) tun->stats.data_tx_errors);
		if (tun->stats.data_rx_oos_packets > 0) {
			len += fprintf(file, "    data rx out-of-sequence packets: %llu\n", 
				       (unsigned long long) tun->stats.data_rx_oos_packets);
		}
		if (tun->created_by_admin) {
			len += fprintf(file, "    establish retries: %d\n", tun->num_establish_retries);
		}
	}

	return len;
}

int l2tp_show_tunnel_profile(FILE *file, struct l2tp_api_tunnel_profile_msg_data *tp)
{
	char str[16];
	int len = 0;

	len += fprintf(file, "Tunnel profile %s\n", tp->profile_name);
	if (OPTSTRING_PTR(tp->host_name) != NULL) {
		len += fprintf(file, "  l2tp host name: %s\n", OPTSTRING(tp->host_name));
	}
	if (tp->our_addr.s_addr != 0) {
		struct in_addr ip;
		ip.s_addr = tp->our_addr.s_addr;
		strcpy(&str[0], inet_ntoa(ip));
		len += fprintf(file, "  local IP address: %s\n", str);
	}
	if (tp->our_udp_port != 0) {
		len += fprintf(file, "  local UDP port: %hu\n", tp->our_udp_port);
	}
	if (tp->peer_addr.s_addr != 0) {
		struct in_addr ip;
		ip.s_addr = tp->peer_addr.s_addr;
		strcpy(&str[0], inet_ntoa(ip));
		len += fprintf(file, "  peer IP address: %s\n", str);
	}
	if (tp->peer_udp_port != 0) {
		len += fprintf(file, "  peer UDP port: %hu\n", tp->peer_udp_port);
	}
	len += fprintf(file, "  authorization mode %s, hide AVPs %s, allow PPP proxy %s\n",
		       (tp->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_NONE) ? "NONE" :
		       (tp->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_SIMPLE) ? "SIMPLE" :
		       (tp->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE) ? "CHALLENGE" : "??",
		       (tp->hide_avps) ? "ON" : "OFF",
		       (tp->allow_ppp_proxy) ? "ON" : "OFF");
	if (OPTSTRING(tp->secret) != NULL) {
		len += fprintf(file, "  tunnel secret: '%s'\n", OPTSTRING(tp->secret));
	}
	len += fprintf(file, "  hello timeout %d, retry timeout %d, idle timeout %d\n",
		       tp->hello_timeout, tp->retry_timeout, tp->idle_timeout);
	len += fprintf(file, "  rx window size %d, tx window size %d, max retries %d\n",
		       tp->rx_window_size, tp->tx_window_size, tp->max_retries);
	len += fprintf(file, "  use UDP checksums: %s\n", (tp->use_udp_checksums) ? "ON" : "OFF");
	len += fprintf(file, "  do pmtu discovery: %s, mtu: %d\n", (tp->do_pmtu_discovery) ? "ON" : "OFF", tp->mtu);
	len += fprintf(file, "  framing capability: %s%s%s\n",
		       ((tp->framing_cap_sync == 0) && (tp->framing_cap_async == 0)) ? "NONE " : "",
		       (tp->framing_cap_sync) ? "SYNC " : "",
		       (tp->framing_cap_async) ? "ASYNC " : "");
	len += fprintf(file, "  bearer capability: %s%s%s\n",
		       ((tp->bearer_cap_digital == 0) && (tp->bearer_cap_analog == 0)) ? "NONE " : "",
		       (tp->bearer_cap_digital) ? "DIGITAL " : "",
		       (tp->bearer_cap_analog) ? "ANALOG " : "");
	len += fprintf(file, "  use tiebreaker: %s\n", (tp->use_tiebreaker) ? "ON" : "OFF");
	if (tp->max_sessions != 0) {
		len += fprintf(file, "  max sessions: %d\n", tp->max_sessions);
	}
	len += fprintf(file, "  peer profile: %s\n", OPTSTRING_PTR(tp->peer_profile_name) == NULL ? "NOT SET" : OPTSTRING(tp->peer_profile_name));
	len += fprintf(file, "  session profile: %s\n", OPTSTRING_PTR(tp->session_profile_name) == NULL ? "NOT SET" : OPTSTRING(tp->session_profile_name));
	len += fprintf(file, "  ppp profile: %s\n", OPTSTRING_PTR(tp->ppp_profile_name) == NULL ? "NOT SET" : OPTSTRING(tp->ppp_profile_name));
	print_trace_flags(file, tp->trace_flags, NULL);
	len += fprintf(file, "\n");

	return len;
}

int l2tp_show_session(FILE *file, struct l2tp_api_session_msg_data *sess)
{
	int len = 0;

	len += fprintf(file, "Session %hu on tunnel %hu:-\n", sess->session_id, sess->tunnel_id);
	len += fprintf(file, "  type: %s, state: %s\n", 
		       (sess->session_type == L2TP_API_SESSION_TYPE_LAIC ? "LAC Incoming Call" :
			sess->session_type == L2TP_API_SESSION_TYPE_LAOC ? "LAC Outgoing Call" :
			sess->session_type == L2TP_API_SESSION_TYPE_LNIC ? "LNS Incoming Call" :
			sess->session_type == L2TP_API_SESSION_TYPE_LNOC ? "LNS Outgoing Call" : "??"),
		       OPTSTRING_PTR(sess->state));
	if (OPTSTRING_PTR(sess->create_time) != NULL) {
		len += fprintf(file, "  created at: %s", OPTSTRING(sess->create_time));
	}
	if (OPTSTRING_PTR(sess->session_name) != NULL) {
		len += fprintf(file, "  administrative name: %s\n", OPTSTRING(sess->session_name));
	}
	len += fprintf(file, "  created by admin: %s", sess->created_by_admin ? "YES" : "NO");
	if (sess->peer_session_id != 0) {
		len += fprintf(file, ", peer session id: %hu\n", sess->peer_session_id);
	} else {
		len += fprintf(file, "\n");
	}
	if ((sess->flags & L2TP_API_SESSION_FLAG_PROFILE_NAME) && (OPTSTRING_PTR(sess->profile_name) != NULL)) {
		len += fprintf(file, "  session profile name: %s\n", OPTSTRING(sess->profile_name));
	}
	if (OPTSTRING_PTR(sess->priv_group_id) != NULL) {
		len += fprintf(file, "  private group id: %s\n", OPTSTRING(sess->priv_group_id));
	}
	if (!sess->no_ppp) {
		if (OPTSTRING_PTR(sess->user_name) != NULL) {
			len += fprintf(file, "  ppp user name: %s\n", OPTSTRING(sess->user_name));
		}
		if (OPTSTRING_PTR(sess->user_password) != NULL) {
			len += fprintf(file, "  ppp user password: %s\n", OPTSTRING(sess->user_password));
		}
		if (OPTSTRING_PTR(sess->interface_name) != NULL) {
			len += fprintf(file, "  ppp interface name: %s\n", OPTSTRING(sess->interface_name));
		}
		if ((sess->flags & L2TP_API_SESSION_FLAG_PPP_PROFILE_NAME) && (OPTSTRING_PTR(sess->ppp_profile_name) != NULL)) {
			len += fprintf(file, "  ppp profile name: %s\n", OPTSTRING(sess->ppp_profile_name));
		}
	}
	len += fprintf(file, "  data sequencing required: %s\n", 
		       sess->sequencing_required ? "ON" : "OFF");
	len += fprintf(file, "  use data sequence numbers: %s\n", 
		       sess->use_sequence_numbers ? "ON" : "OFF");
	if (sess->reorder_timeout != 0) {
		len += fprintf(file, "  reorder timeout: %u\n", sess->reorder_timeout);
	}
	print_trace_flags(file, sess->trace_flags, NULL);
	len += fprintf(file, "  framing types:%s%s%s\n", 
		       ((sess->framing_type_sync == 0) && (sess->framing_type_async == 0)) ? " NONE" : "",
		       sess->framing_type_sync ? " SYNC" : "",
		       sess->framing_type_async ? " ASYNC" : "");
	len += fprintf(file, "  bearer types:%s%s%s\n", 
		       ((sess->bearer_type_digital == 0) && (sess->bearer_type_analog == 0)) ? " NONE" : "",
		       sess->bearer_type_digital ? " DIGITAL" : "",
		       sess->bearer_type_analog ? " ANALOG" : "");
	if (sess->call_serial_number != 0) {
		len += fprintf(file, "  call serial number: %d\n", sess->call_serial_number);
	}
	if (sess->physical_channel_id != 0) {
		len += fprintf(file, "  physical channel id: %d\n", sess->physical_channel_id);
	}
	if ((sess->minimum_bps != 0) || (sess->maximum_bps != 0)) {
		len += fprintf(file, "  min bps: %d, max bps: %d\n", sess->minimum_bps, sess->maximum_bps);
	}
	if (sess->tx_connect_speed != 0) {
		if (sess->rx_connect_speed == 0) {
			len += fprintf(file, "  connect speed: %d\n", sess->tx_connect_speed);
		} else {
			len += fprintf(file, "  tx connect speed: %d, rx connect speed: %d\n", 
				       sess->tx_connect_speed, sess->rx_connect_speed);
		}
	}
	if (OPTSTRING_PTR(sess->calling_number) != NULL) {
		len += fprintf(file, "  calling number: '%s'\n", OPTSTRING(sess->calling_number));
	}
	if (OPTSTRING_PTR(sess->called_number) != NULL) {
		len += fprintf(file, "  called number: '%s'\n", OPTSTRING(sess->called_number));
	}
	if (OPTSTRING_PTR(sess->sub_address) != NULL) {
		len += fprintf(file, "  sub address: '%s'\n", OPTSTRING(sess->sub_address));
	}
	if (!sess->no_ppp) {
		len += fprintf(file, "  use ppp proxy: %s\n", sess->use_ppp_proxy ? "YES" : "NO");
		if (sess->proxy_auth_type != L2TP_API_SESSION_PROXY_AUTH_TYPE_RESERVED) {
			len += fprintf(file, "  proxy auth type: %s\n", 
				       (sess->proxy_auth_type == L2TP_API_SESSION_PROXY_AUTH_TYPE_PLAIN_TEXT) ? "TEXT" :
				       (sess->proxy_auth_type == L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_CHAP) ? "CHAP" :
				       (sess->proxy_auth_type == L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_PAP) ? "PAP" :
				       (sess->proxy_auth_type == L2TP_API_SESSION_PROXY_AUTH_TYPE_NO_AUTH) ? "NONE" :
				       (sess->proxy_auth_type == L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_MSCHAPV1) ? "MSCHAP" : "??");
		}
		if (OPTSTRING_PTR(sess->proxy_auth_name) != NULL) {
			len += fprintf(file, "  proxy auth name: '%s'\n", OPTSTRING(sess->proxy_auth_name));
		}
		if (sess->proxy_auth_challenge.proxy_auth_challenge_len > 0) {
			int index;
			len += fprintf(file, "  proxy auth challenge: ");
			for (index = 0; index < sess->proxy_auth_challenge.proxy_auth_challenge_len; index++) {
				len += fprintf(file, "%02x", sess->proxy_auth_challenge.proxy_auth_challenge_val[index]);
			}
			len += fprintf(file, "\n");
		}
		if (sess->proxy_auth_response.proxy_auth_response_len > 0) {
			int index;
			len += fprintf(file, "  proxy auth response: ");
			for (index = 0; index < sess->proxy_auth_response.proxy_auth_response_len; index++) {
				len += fprintf(file, "%02x", sess->proxy_auth_response.proxy_auth_response_val[index]);
			}
			len += fprintf(file, "\n");
		}
		if (sess->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_len > 0) {
			int index;
			len += fprintf(file, "  initial received LCP CONFREQ: ");
			for (index = 0; index < sess->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_len; index++) {
				len += fprintf(file, "%02x", sess->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val[index]);
			}
			len += fprintf(file, "\n");
		}
		if (sess->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_len > 0) {
			int index;
			len += fprintf(file, "  last received LCP CONFREQ: ");
			for (index = 0; index < sess->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_len; index++) {
				len += fprintf(file, "%02x", sess->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val[index]);
			}
			len += fprintf(file, "\n");
		}
		if (sess->last_sent_lcp_confreq.last_sent_lcp_confreq_len > 0) {
			int index;
			len += fprintf(file, "  last sent LCP CONFREQ: ");
			for (index = 0; index < sess->last_sent_lcp_confreq.last_sent_lcp_confreq_len; index++) {
				len += fprintf(file, "%02x", sess->last_sent_lcp_confreq.last_sent_lcp_confreq_val[index]);
			}
			len += fprintf(file, "\n");
		}
	}
	if (sess->peer.result_code != 0) {
		len += fprintf(file, "  last peer response information:\n");
		len += fprintf(file, "    result code: %hu, error code: %hu\n",
			       sess->peer.result_code, sess->peer.error_code);
		if (OPTSTRING_PTR(sess->peer.error_message) != NULL) {
			len += fprintf(file, "    message: '%s'\n", OPTSTRING(sess->peer.error_message));
		}
	}
	if (sess->peer.q931_cause_code != 0) {
		len += fprintf(file, "  last peer Q931 information:\n");
		len += fprintf(file, "    q931 cause code: %hu, cause msg: %hu\n",
			       sess->peer.q931_cause_code, sess->peer.q931_cause_msg);
		if (OPTSTRING_PTR(sess->peer.q931_advisory_msg) != NULL) {
			len += fprintf(file, "    advisory: '%s'\n", OPTSTRING(sess->peer.q931_advisory_msg));
		}
	}

	len += fprintf(file, "\n  Peer configuration data:-\n");
	if (OPTSTRING_PTR(sess->peer.private_group_id) != NULL) {
		len += fprintf(file, "    private group id: %s\n", OPTSTRING(sess->peer.private_group_id));
	}
	len += fprintf(file, "    data sequencing required: %s\n", 
		       sess->peer.sequencing_required ? "ON" : "OFF");
	len += fprintf(file, "    framing types:%s%s%s\n", 
		       ((sess->framing_type_sync == 0) && (sess->framing_type_async == 0)) ? " NONE" : "",
		       sess->peer.framing_type_sync ? " SYNC" : "",
		       sess->peer.framing_type_async ? " ASYNC" : "");
	len += fprintf(file, "    bearer types:%s%s%s\n", 
		       ((sess->bearer_type_digital == 0) && (sess->bearer_type_analog == 0)) ? " NONE" : "",
		       sess->peer.bearer_type_digital ? " DIGITAL" : "",
		       sess->peer.bearer_type_analog ? " ANALOG" : "");
	if (sess->peer.call_serial_number != 0) {
		len += fprintf(file, "    call serial number: %d\n", sess->peer.call_serial_number);
	}
	if (sess->peer.physical_channel_id != 0) {
		len += fprintf(file, "    physical channel id: %d\n", sess->peer.physical_channel_id);
	}
	if ((sess->peer.minimum_bps != 0) || (sess->peer.maximum_bps != 0)) {
		len += fprintf(file, "    min bps: %u, max bps: %u\n", sess->peer.minimum_bps, sess->peer.maximum_bps);
	}
	if (sess->peer.connect_speed != 0) {
		if (sess->peer.rx_connect_speed == 0) {
			len += fprintf(file, "    connect speed: %u\n", sess->peer.connect_speed);
		} else {
			len += fprintf(file, "    tx connect speed: %u, rx connect speed: %u\n", 
				       sess->peer.connect_speed, sess->peer.rx_connect_speed);
		}
	}
	if (OPTSTRING_PTR(sess->peer.calling_number) != NULL) {
		len += fprintf(file, "    calling number: '%s'\n", OPTSTRING(sess->peer.calling_number));
	}
	if (OPTSTRING_PTR(sess->peer.called_number) != NULL) {
		len += fprintf(file, "    called number: '%s'\n", OPTSTRING(sess->peer.called_number));
	}
	if (OPTSTRING_PTR(sess->peer.sub_address) != NULL) {
		len += fprintf(file, "    calling number: '%s'\n", OPTSTRING(sess->peer.sub_address));
	}
	if ((sess->stats.data_rx_oos_discards > 0) || (sess->stats.data_rx_oos_packets > 0)) {
		len += fprintf(file, "  data rx out-of-sequence packets: %llu, discards: %llu\n",
			       (unsigned long long) sess->stats.data_rx_oos_packets, 
			       (unsigned long long) sess->stats.data_rx_oos_discards);
	}
	len += fprintf(file, "  data rx packets: %llu, rx bytes: %llu, rx errors: %llu\n", 
		       (unsigned long long) sess->stats.data_rx_packets, 
		       (unsigned long long) sess->stats.data_rx_bytes, 
		       (unsigned long long) sess->stats.data_rx_errors);
	len += fprintf(file, "  data tx packets: %llu, tx bytes: %llu, tx errors: %llu\n", 
		       (unsigned long long) sess->stats.data_tx_packets, 
		       (unsigned long long) sess->stats.data_tx_bytes, 
		       (unsigned long long) sess->stats.data_tx_errors);

	return len;
}

int l2tp_show_session_profile(FILE *file, struct l2tp_api_session_profile_msg_data *sp)
{
	int len = 0;

	len += fprintf(file, "Session profile %s\n", sp->profile_name);
	len += fprintf(file, "  ppp profile: %s\n", OPTSTRING(sp->ppp_profile_name));
	print_trace_flags(file, sp->trace_flags, NULL);
	len += fprintf(file, "  session type: %s\n",
		       (sp->session_type == L2TP_API_SESSION_TYPE_UNSPECIFIED) ? "unspecified" :
		       (sp->session_type == L2TP_API_SESSION_TYPE_LAIC) ? "LAC Incoming Call" :
		       (sp->session_type == L2TP_API_SESSION_TYPE_LAOC) ? "LAC Outgoing Call" :
		       (sp->session_type == L2TP_API_SESSION_TYPE_LNIC) ? "LNS Incoming Call" :
		       (sp->session_type == L2TP_API_SESSION_TYPE_LNOC) ? "LNS Outgoing Call" : "??");
	if (OPTSTRING_PTR(sp->priv_group_id) != NULL) {
		len += fprintf(file, "  private group id: %s\n", OPTSTRING(sp->priv_group_id));
	}
	len += fprintf(file, "  data sequencing required: %s\n", 
		       sp->sequencing_required ? "ON" : "OFF");
	len += fprintf(file, "  use data sequence numbers: %s\n", 
		       sp->use_sequence_numbers ? "ON" : "OFF");
	if (sp->reorder_timeout != 0) {
		len += fprintf(file, "  reorder timeout: %u ms\n", sp->reorder_timeout);
	}
	if (sp->no_ppp) {
		len += fprintf(file, "  ppp disabled: YES\n");
	}
	len += fprintf(file, "  framing types:%s%s%s\n", 
		       ((sp->framing_type_sync == 0) && (sp->framing_type_async == 0)) ? " NONE" : "",
		       sp->framing_type_sync ? " SYNC" : "",
		       sp->framing_type_async ? " ASYNC" : "");
	len += fprintf(file, "  bearer types:%s%s%s\n", 
		       ((sp->bearer_type_digital == 0) && (sp->bearer_type_analog == 0)) ? " NONE" : "",
		       sp->bearer_type_digital ? " DIGITAL" : "",
		       sp->bearer_type_analog ? " ANALOG" : "");
	if ((sp->minimum_bps != 0) || (sp->maximum_bps != 0)) {
		len += fprintf(file, "  min bps: %d, max bps: %d\n", sp->minimum_bps, sp->maximum_bps);
	}
	if (sp->tx_connect_speed != 0) {
		if (sp->rx_connect_speed == 0) {
			len += fprintf(file, "  connect speed: %d\n", sp->tx_connect_speed);
		} else {
			len += fprintf(file, "  tx connect speed: %d, rx connect speed: %d\n", 
				       sp->tx_connect_speed, sp->rx_connect_speed);
		}
	}
	len += fprintf(file, "\n");

	return len;
}

int l2tp_show_ppp_profile(FILE *file, struct l2tp_api_ppp_profile_msg_data *pp)
{
	char local_ip[16];
	char peer_ip[16];
	char dns1[16];
	char dns2[16];
	char wins1[16];
	char wins2[16];
	int len = 0;

	len += fprintf(file, "Ppp profile %s\n", pp->profile_name);
	print_trace_flags(file, pp->trace_flags, NULL);
	len += fprintf(file, "  mru: %hu, mtu: %hu, mode: %s\n",
		       pp->mru, pp->mtu, 
		       (pp->sync_mode == L2TP_API_PPP_SYNCMODE_SYNC_ASYNC) ? "SYNC|ASYNC" :
		       (pp->sync_mode == L2TP_API_PPP_SYNCMODE_SYNC) ? "SYNC" :
		       (pp->sync_mode == L2TP_API_PPP_SYNCMODE_ASYNC) ? "ASYNC" : "??");
	len += fprintf(file, "  allowed authentications: %s%s%s%s%s%s\n", 
		       !pp->auth_refuse_pap ? "PAP " : "",
		       !pp->auth_refuse_chap ? "CHAP " : "",
		       !pp->auth_refuse_mschap ? "MSCHAP " : "",
		       !pp->auth_refuse_mschapv2 ? "MSCHAPv2 " : "",
		       !pp->auth_refuse_eap ? "EAP " : "",
		       (pp->auth_none && (!pp->auth_peer)) ? "NOAUTH " : "");
	if (pp->auth_peer) {
		len += fprintf(file, "  authenticate peer: YES\n");
	}
	if (OPTSTRING_PTR(pp->local_name) != NULL) {
		len += fprintf(file, "  local name for authentication: %s\n", OPTSTRING(pp->local_name));
	}
	if (OPTSTRING_PTR(pp->remote_name) != NULL) {
		len += fprintf(file, "  remote name for authentication: %s\n", OPTSTRING(pp->remote_name));
	}
	len += fprintf(file, "  max connect time: %d, max failure count: %d, idle timeout: %d\n",
		       pp->max_connect_time, pp->max_failure_count, pp->idle_timeout);
	if (pp->asyncmap != 0) {
		len += fprintf(file, "  asyncmap: %#x\n", pp->asyncmap);
	}
	len += fprintf(file, "  multilink: %s, proxy arp: %s\n", 
		       pp->multilink ? "YES" : "NO", pp->proxy_arp ? "YES" : "NO");
	len += fprintf(file, "  IP parameters:-\n");
	ip_to_string(&local_ip[0], pp->local_ip_addr.s_addr);
	ip_to_string(&peer_ip[0], pp->peer_ip_addr.s_addr);
	ip_to_string(&dns1[0], pp->dns_addr_1.s_addr);
	ip_to_string(&dns2[0], pp->dns_addr_2.s_addr);
	ip_to_string(&wins1[0], pp->wins_addr_1.s_addr);
	ip_to_string(&wins2[0], pp->wins_addr_2.s_addr);
	len += fprintf(file, "    local address: %s, peer address: %s%s\n"
		       "    dns addresses: %s / %s\n"
		       "    wins addresses: %s / %s\n",
		       local_ip, peer_ip, 
		       pp->use_as_default_route ? " [default route]" : "",
		       dns1, dns2, wins1, wins2);
	if (OPTSTRING_PTR(pp->ip_pool_name) != NULL) {
		len += fprintf(file, "    ip pool name: %s\n", OPTSTRING(pp->ip_pool_name));
	}
	len += fprintf(file, "    use radius: %s\n", pp->use_radius ? "YES" : "NO");
	if (pp->use_radius && OPTSTRING_PTR(pp->radius_hint) != NULL) {
		len += fprintf(file, "    radius hint: %s\n", OPTSTRING(pp->radius_hint));
	}
	len += fprintf(file, "  PAP parameters:-\n");
	len += fprintf(file, "    max auth requests: %d, restart interval: %d, timeout: %d\n",
		       pp->pap_max_auth_requests, pp->pap_restart_interval, pp->pap_timeout);
	len += fprintf(file, "  CHAP parameters:-\n");
	len += fprintf(file, "    interval: %d, max challenge: %d, restart: %d\n",
		       pp->chap_interval, pp->chap_max_challenge, pp->chap_restart);
	len += fprintf(file, "  LCP parameters:-\n");
	len += fprintf(file, "    echo failure count: %d, echo interval: %d\n"
		       "    max config requests: %d, max config naks: %d\n"
		       "    max terminate requests: %d, retransmit interval: %d\n",
		       pp->lcp_echo_failure_count, pp->lcp_echo_interval,
		       pp->lcp_max_config_requests, pp->lcp_max_config_naks,
		       pp->lcp_max_terminate_requests, pp->lcp_retransmit_interval);
	len += fprintf(file, "  IPCP parameters:-\n");
	len += fprintf(file, "    max config requests: %d, max config naks: %d\n"
		       "    max terminate requests: %d, retransmit interval: %d\n",
		       pp->ipcp_max_config_requests, pp->ipcp_max_config_naks,
		       pp->ipcp_max_terminate_requests, pp->ipcp_retransmit_interval);
	len += fprintf(file, "\n");

	return len;
}

int l2tp_show_peer_profile(FILE *file, struct l2tp_api_peer_profile_msg_data *pp)
{
	char port[16];
	struct in_addr ip_addr;
	int len = 0;

	ip_addr.s_addr = pp->peer_addr.s_addr;
	sprintf(&port[0], "%d", pp->peer_port);
	len += fprintf(file, "Peer profile %s:-\n"
		       "  address: %s, port %s\n",
		       pp->profile_name, 
		       ip_addr.s_addr == INADDR_ANY ? "ANY" : inet_ntoa(ip_addr), 
		       pp->peer_port == 0 ? "default" : port);
	if (pp->netmask.s_addr != INADDR_BROADCAST) {
		ip_addr.s_addr = pp->netmask.s_addr;
		len += fprintf(file, "  netmask: %s\n", inet_ntoa(ip_addr));
	}
	len += fprintf(file, "  mode %s/%s\n",
		       pp->we_can_be_lac ? "LAC" : "-",
		       pp->we_can_be_lns ? "LNS" : "-");
	len += fprintf(file, "  default tunnel profile: %s\n"
		       "  default session profile: %s\n"
		       "  default ppp profile: %s\n",
		       OPTSTRING_PTR(pp->default_tunnel_profile_name) ? OPTSTRING(pp->default_tunnel_profile_name) : L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME,
		       OPTSTRING_PTR(pp->default_session_profile_name) ? OPTSTRING(pp->default_session_profile_name) : L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME,
		       OPTSTRING_PTR(pp->default_ppp_profile_name) ? OPTSTRING(pp->default_ppp_profile_name) : L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME);
	len += fprintf(file, "  use count: %d\n", pp->use_count);
	len += fprintf(file, "\n");

	return len;
}

int l2tp_show_peer(FILE *file, struct l2tp_api_peer_msg_data *peer)
{
	int len = 0;
	struct in_addr ip_addr;

	ip_addr.s_addr = peer->peer_addr.s_addr;
	len += fprintf(file, "Peer %s, ", inet_ntoa(ip_addr));
	ip_addr.s_addr = peer->local_addr.s_addr;
	len += fprintf(file, "local %s:-\n", ip_addr.s_addr == INADDR_ANY ? "ANY" : inet_ntoa(ip_addr));
	len += fprintf(file, "  number active tunnels: %d\n", peer->num_tunnels);

	return len;
}

#endif /* L2TP_FEATURE_LOCAL_STAT_FILE */
