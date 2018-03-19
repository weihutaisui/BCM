/*****************************************************************************
 * Copyright (C) 2004,2005,2006,2007 Katalix Systems Ltd
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
 * Command Line Interface for OpenL2TP.
 * This started out as a quick hack but just grew and grew. There's duplicate
 * code and memory leaks all over the place.
 * Command syntax is defined in a syntax table near the bottom of this file.
 * The guts of the CLI is implemented in a library.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include <signal.h>

#include "usl.h"
#include "cli_api.h"

#include "l2tp_private.h"
#include "l2tp_rpc.h"

static char *empty_string = "";

static char *opt_rpc_protocol = "udp";
static int opt_quiet;
static int interactive = 0;

static CLIENT *cl;
static char server[48];
static char *l2tp_histfile = NULL;
static int l2tp_histfile_maxsize = -1;

#define L2TP_ACT_DECLARATIONS(_max_args, _ids_type, _clnt_res_type)				\
	struct cli_node *args[_max_args];							\
	char *arg_values[_max_args];								\
	int num_args = _max_args;								\
	int arg;										\
	int result;										\
	_ids_type arg_id;									\
	_clnt_res_type clnt_res;

#define L2TP_ACT_BEGIN()									\
	result = cli_find_args(argc, argv, node, &args[0], &arg_values[0], &num_args);		\
	if (result == 0) {									\
		for (arg = 0; arg < num_args; arg++) {						\
			if (args[arg] && args[arg]->arg) {					\
				arg_id = args[arg]->arg->id;

#define L2TP_ACT_END()										\
			}									\
		}										\
	} else {										\
		/* tell caller which arg failed */						\
		*arg_num = num_args;								\
		result = -EINVAL;								\
		goto out;									\
	}


#define L2TP_ACT_PARSE_ARG(_arg_node, _arg_value, _field, _flag_var, _flag)			\
	result = _arg_node->arg->parser(_arg_node, _arg_value, &_field);			\
	if (result < 0) {									\
		goto out;									\
	}											\
	_flag_var |= _flag;

/* Include code shared with openl2tpd */
#define L2TP_FEATURE_LOCAL_STAT_FILE
#include "l2tp_common.c"

/*****************************************************************************
 * server ...
 *****************************************************************************/

#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_SERVER_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#define FLG(id, name, doc) \
	{ name, { L2TP_SERVER_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

typedef enum {
	L2TP_SERVER_ARGID_NAME,
} l2tp_server_arg_ids_t;

static struct cli_arg_entry l2tp_args_server_modify[] = {
	ARG(NAME, 		"name", 		0, 	string,	"IP address or hostname of L2TP daemon to attach to. Default=localhost."),
	{ NULL, },
};

static void l2tp_set_prompt(char *server_name)
{
	static char prompt[48];

	snprintf(prompt, sizeof(prompt), "l2tp-%s", server_name);
	cli_set_prompt(prompt);
}


static int l2tp_act_server_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *server_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_server_arg_ids_t, int);

	clnt_res = 0;

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_SERVER_ARGID_NAME:
			server_name = arg_values[arg];
			break;
		}
	} L2TP_ACT_END();

	if (server_name == NULL) {
		fprintf(stderr, "Required name argument is missing.\n");
		goto out;
	}
	if (strcmp(server_name, &server[0])) {
		strncpy(&server[0], server_name, sizeof(server));

		clnt_destroy(cl);
		cl = clnt_create(server, L2TP_PROG, L2TP_VERSION, opt_rpc_protocol);
		if (cl == NULL) {
			clnt_pcreateerror(server);
			exit(1);
		}

		l2tp_set_prompt(server_name);
	}

out:
	return 0;
}

static int l2tp_act_server_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	printf("Connected to server: %s\n", server);
	return 0;
}

/*****************************************************************************
 * system ...
 *****************************************************************************/

#undef ARG
#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_SYSTEM_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#undef FLG
#define FLG(id, name, doc) \
	{ name, { L2TP_SYSTEM_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

typedef enum {
	L2TP_SYSTEM_ARGID_TRACE_FLAGS,
	L2TP_SYSTEM_ARGID_MAX_TUNNELS,
	L2TP_SYSTEM_ARGID_MAX_SESSIONS,
	L2TP_SYSTEM_ARGID_DRAIN_TUNNELS,
	L2TP_SYSTEM_ARGID_TUNNEL_ESTTO,
	L2TP_SYSTEM_ARGID_TUNNEL_PERSIST_PENDTO,
	L2TP_SYSTEM_ARGID_SESSION_PERSIST_PENDTO,
	L2TP_SYSTEM_ARGID_SESSION_ESTTO,
	L2TP_SYSTEM_ARGID_DENY_LOCAL_TUNNEL_CREATES,
	L2TP_SYSTEM_ARGID_DENY_REMOTE_TUNNEL_CREATES,
	L2TP_SYSTEM_ARGID_RESET_STATISTICS,
} l2tp_system_arg_ids_t;

static struct cli_arg_entry l2tp_args_system_modify[] = {
	ARG(TRACE_FLAGS, 		"trace_flags", 		0, 	string,	"Default trace flags to use if not otherwise overridden."),
	ARG(MAX_TUNNELS, 		"max_tunnels", 		0, 	uint32,	"Maximum number of tunnels permitted. Default=0 (no limit)."),
	ARG(MAX_SESSIONS, 		"max_sessions", 	0, 	uint32,	"Maximum number of sessions permitted. Default=0 (no limit)."),
	ARG(DRAIN_TUNNELS,		"drain_tunnels",	0, 	bool, 	"Enable the draining of existing tunnels (prevent new tunnels "
	    									"from being created."),
	ARG(TUNNEL_ESTTO, 		"tunnel_establish_timeout", 0, 	uint32,	"Timeout for tunnel establishment. Default=120 seconds.."),
	ARG(SESSION_ESTTO, 		"session_establish_timeout", 0, uint32,	"Timeout for session establishment. Default=120 seconds.."),
	ARG(TUNNEL_PERSIST_PENDTO,	"tunnel_persist_pend_timeout", 0, uint32, "Timeout to hold persistent tunnels before retrying. Default=300 seconds.."),
	ARG(SESSION_PERSIST_PENDTO,	"session_persist_pend_timeout", 0, uint32, "Timeout to hold persistent sessions before retrying. Default=60 seconds.."),
	ARG(DENY_LOCAL_TUNNEL_CREATES,	"deny_local_tunnel_creates", 0,	bool,	"Deny the creation of new tunnels by local request."),
	ARG(DENY_REMOTE_TUNNEL_CREATES,	"deny_remote_tunnel_creates", 0, bool,	"Deny the creation of new tunnels by remote peers."),
	FLG(RESET_STATISTICS,		"reset_statistics", 			"Reset statistics."),
	{ NULL, },
};


static int l2tp_act_exit(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	exit(0);
}

static int l2tp_act_help(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	printf("The help command is deprecated. Please see the l2tpconfig man page for help.\n");
	return 0;
}

static int l2tp_act_system_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	cli_bool_t bool_arg;
	struct l2tp_api_system_msg_data msg = { { 0, } };
	L2TP_ACT_DECLARATIONS(10, l2tp_system_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_SYSTEM_ARGID_TRACE_FLAGS:
			result = l2tp_parse_debug_mask(&msg.config.trace_flags, arg_values[arg], 1);
			if (result < 0) {
				goto out;
			}
			msg.config.flags |= L2TP_API_CONFIG_FLAG_TRACE_FLAGS;
			break;
		case L2TP_SYSTEM_ARGID_DRAIN_TUNNELS:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, msg.config.flags, L2TP_API_CONFIG_FLAG_DRAIN_TUNNELS);
			msg.config.drain_tunnels = bool_arg;
			break;
		case L2TP_SYSTEM_ARGID_MAX_TUNNELS:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.config.max_tunnels, msg.config.flags, L2TP_API_CONFIG_FLAG_MAX_TUNNELS);
			break;
		case L2TP_SYSTEM_ARGID_MAX_SESSIONS:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.config.max_sessions, msg.config.flags, L2TP_API_CONFIG_FLAG_MAX_SESSIONS);
			break;
		case L2TP_SYSTEM_ARGID_TUNNEL_ESTTO:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.config.tunnel_establish_timeout, msg.config.flags, 
					   L2TP_API_CONFIG_FLAG_TUNNEL_ESTABLISH_TIMEOUT);
			break;
		case L2TP_SYSTEM_ARGID_SESSION_ESTTO:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.config.session_establish_timeout, msg.config.flags, 
					   L2TP_API_CONFIG_FLAG_SESSION_ESTABLISH_TIMEOUT);
			break;
		case L2TP_SYSTEM_ARGID_TUNNEL_PERSIST_PENDTO:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.config.tunnel_persist_pend_timeout, msg.config.flags, 
					   L2TP_API_CONFIG_FLAG_TUNNEL_PERSIST_PEND_TIMEOUT);
			break;
		case L2TP_SYSTEM_ARGID_SESSION_PERSIST_PENDTO:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.config.session_persist_pend_timeout, msg.config.flags, 
					   L2TP_API_CONFIG_FLAG_SESSION_PERSIST_PEND_TIMEOUT);
			break;
 		case L2TP_SYSTEM_ARGID_DENY_LOCAL_TUNNEL_CREATES:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.config.deny_local_tunnel_creates, msg.config.flags, 
					   L2TP_API_CONFIG_FLAG_DENY_LOCAL_TUNNEL_CREATES);
			break;
		case L2TP_SYSTEM_ARGID_DENY_REMOTE_TUNNEL_CREATES:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.config.deny_remote_tunnel_creates, msg.config.flags, 
					   L2TP_API_CONFIG_FLAG_DENY_REMOTE_TUNNEL_CREATES);
			break;
		case L2TP_SYSTEM_ARGID_RESET_STATISTICS:
			msg.config.flags |= L2TP_API_CONFIG_FLAG_RESET_STATISTICS;
			break;
		}
	} L2TP_ACT_END();

	result = l2tp_system_modify_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		exit(1);
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		return 0;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Modified system config\n");
	}

out:
	return result;
}

static int l2tp_act_system_show_version(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_app_msg_data app;
	int result;

	memset(&app, 0, sizeof(app));
	result = l2tp_app_info_get_1(&app, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	l2tp_show_app_version(stdout, &app);

out:
	return result;
}

static int l2tp_act_system_show_config(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_app_msg_data app;
	struct l2tp_api_system_msg_data sys;
	int result;

	memset(&app, 0, sizeof(app));
	result = l2tp_app_info_get_1(&app, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	memset(&sys, 0, sizeof(sys));
	result = l2tp_system_get_1(&sys, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	if (app.cookie != L2TP_APP_COOKIE) {
		printf("*** WARNING: CONTROL APPLICATION AND DAEMON ARE OUT OF SYNC. ***\n");
		printf("*** UNDEFINED BAHAVIOR MAY RESULT. REINSTALL TO FIX.         ***\n\n");
	}

	l2tp_show_system_config(stdout, &sys);
out:
	return result;
}

static int l2tp_act_system_show_status(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_system_msg_data sys;
	int result;

	memset(&sys, 0, sizeof(sys));
	result = l2tp_system_get_1(&sys, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	l2tp_show_system_status(stdout, &sys);
out:
	return result;
}

static int l2tp_act_system_show_stats(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_system_msg_data sys;
	int result;

	memset(&sys, 0, sizeof(sys));
	result = l2tp_system_get_1(&sys, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	l2tp_show_system_stats(stdout, &sys);
out:
	return result;
}

/*****************************************************************************
 * Tunnel...
 *****************************************************************************/

typedef enum {
	L2TP_TUNNEL_ARGID_TRACE_FLAGS,
	L2TP_TUNNEL_ARGID_AUTH_MODE,
	L2TP_TUNNEL_ARGID_HIDE_AVPS,
	L2TP_TUNNEL_ARGID_UDP_CSUMS,
	L2TP_TUNNEL_ARGID_DO_PMTU_DISCOVERY,
	L2TP_TUNNEL_ARGID_PERSIST,
	L2TP_TUNNEL_ARGID_MTU,
	L2TP_TUNNEL_ARGID_HELLO_TIMEOUT,
	L2TP_TUNNEL_ARGID_MAX_RETRIES,
	L2TP_TUNNEL_ARGID_RX_WINDOW_SIZE,
	L2TP_TUNNEL_ARGID_TX_WINDOW_SIZE,
	L2TP_TUNNEL_ARGID_RETRY_TIMEOUT,
	L2TP_TUNNEL_ARGID_IDLE_TIMEOUT,
	L2TP_TUNNEL_ARGID_DEST_IPADDR,
	L2TP_TUNNEL_ARGID_CONFIG_ID,
	L2TP_TUNNEL_ARGID_SRC_IPADDR,
	L2TP_TUNNEL_ARGID_OUR_UDP_PORT,
	L2TP_TUNNEL_ARGID_PEER_UDP_PORT,
	L2TP_TUNNEL_ARGID_PROFILE_NAME,
	L2TP_TUNNEL_ARGID_USE_TIEBREAKER,
	L2TP_TUNNEL_ARGID_ALLOW_PPP_PROXY,
	L2TP_TUNNEL_ARGID_FRAMING_CAP,
	L2TP_TUNNEL_ARGID_BEARER_CAP,
	L2TP_TUNNEL_ARGID_HOST_NAME,
	L2TP_TUNNEL_ARGID_SECRET,
	L2TP_TUNNEL_ARGID_TUNNEL_ID,
	L2TP_TUNNEL_ARGID_MAX_SESSIONS,
	L2TP_TUNNEL_ARGID_TUNNEL_NAME,
	L2TP_TUNNEL_ARGID_PEER_PROFILE_NAME,
	L2TP_TUNNEL_ARGID_SESSION_PROFILE_NAME,
	L2TP_TUNNEL_ARGID_PPP_PROFILE_NAME,
	L2TP_TUNNEL_ARGID_INTERFACE_NAME,
	L2TP_TUNNEL_ARGID_SHOW_CONFIG,
	L2TP_TUNNEL_ARGID_SHOW_TRANSPORT,
	L2TP_TUNNEL_ARGID_LIST_LOCAL_ONLY,
	L2TP_TUNNEL_ARGID_LIST_REMOTE_ONLY,
	L2TP_TUNNEL_ARGID_LIST_NAMES,
} l2tp_tunnel_arg_ids_t;
 
#undef ARG
#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_TUNNEL_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#undef FLG
#define FLG(id, name, doc) \
	{ name, { L2TP_TUNNEL_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

/* Paremeters for Create and Modify operations */
#define L2TP_TUNNEL_MODIFY_ARGS 															\
	ARG(TRACE_FLAGS, 	"trace_flags", 		0, 	string,	"Trace flags, for debugging network problems"),					\
	ARG(UDP_CSUMS,		"use_udp_checksums",	0,	bool,	"Use UDP checksums in data frames. Default: ON"),				\
	ARG(PERSIST,		"persist",		0,	bool,	"Persist (recreate automatically if tunnel fails). Default: OFF"),		\
	ARG(HELLO_TIMEOUT,	"hello_timeout",	0,	int32,	("Set timeout used for periodic L2TP Hello messages (in seconds). "		\
									 "Default: 0 (no hello messages are generated.")),				\
	ARG(MAX_RETRIES,	"max_retries",		0,	int32,	"Maximum transmit retries before assuming tunnel failure."),			\
	ARG(RETRY_TIMEOUT,	"retry_timeout",	0,	int32,	"Retry timeout - initial delay between retries."),				\
	ARG(IDLE_TIMEOUT,	"idle_timeout",		0,	int32,	"Idle timeout - automatically delete tunnel if no sessions."),			\
	ARG(MAX_SESSIONS,	"max_sessions",		0,	int32,	"Maximum number of sessions allowed on tunnel. Default=0 (limited only "	\
	    								"by max_sessions limit in system parameters)."),				\
	ARG(MTU,		"mtu",			0,	int32,	"MTU for all sessions in tunnel. Default: 1460."),				\
	ARG(PEER_PROFILE_NAME,	"peer_profile_name",	0,	string,	("Name of peer profile which will be used for default values of the " 		\
									 "tunnel's parameters.")),							\
	ARG(SESSION_PROFILE_NAME, "session_profile_name", 0,	string,	("Name of session profile which will be used for default values of the " 	\
									 "tunnel's session parameters.")),						\
	ARG(PPP_PROFILE_NAME,	"ppp_profile_name",	0,	string,	("Name of ppp profile which will be used for default values of the " 		\
									 "tunnel's session PPP parameters.")),						\
	ARG(INTERFACE_NAME,	"interface_name",	0,	string,	("Name of system interface for the tunnel. Default: l2tpN where N is tunnel_id.")) \

/* Paremeters for Create operations */
#define L2TP_TUNNEL_CREATE_ARGS 															\
	ARG(SRC_IPADDR,		"src_ipaddr",		0,	ipaddr,	"Source IP address"),								\
	ARG(PEER_UDP_PORT,	"peer_udp_port",	0,	uint16,	"UDP port number with which to contact peer L2TP server. Default: 1701"),	\
	ARG(OUR_UDP_PORT,	"our_udp_port",		0,	uint16,	"Local UDP port number with which to contact peer L2TP server. "		\
									"Default: autogenerated"),							\
	ARG(USE_TIEBREAKER,	"use_tiebreaker",	0,	bool,	"Enable use of a tiebreaker when setting up the tunnel. Default: ON"),		\
	ARG(ALLOW_PPP_PROXY,	"allow_ppp_proxy",	0,	bool,	"Allow PPP proxy"),								\
	ARG(FRAMING_CAP,	"framing_caps",		0,	string,	("Framing capabilities:-\n"							\
									 "none, sync, async, any")),								\
	ARG(BEARER_CAP,		"bearer_caps",		0,	string,	("Bearer capabilities:-\n"							\
									 "none, digital, analog, any")),								\
	ARG(HOST_NAME,		"host_name",		0,	string,	"Name to advertise to peer when setting up the tunnel."),			\
	ARG(SECRET,		"secret",		0,	string,	("Optional secret which is shared with tunnel peer. Must be specified when "	\
									 "hide_avps is enabled.")),							\
	ARG(AUTH_MODE,		"auth_mode",		0,	string, ("Tunnel authentication mode:-\n"						\
									 "none      - no authentication, unless secret is given\n"			\
									 "simple    - check peer hostname\n"						\
									 "challenge - require tunnel secret\n")),					\
	ARG(HIDE_AVPS,		"hide_avps",		0,	bool,	"Hide AVPs. Default OFF"),							\
	ARG(RX_WINDOW_SIZE,	"rx_window_size",	0,	uint16,	"Rx window size"),								\
	ARG(TX_WINDOW_SIZE,	"tx_window_size",	0,	uint16,	"Tx window size"),								\
	ARG(DO_PMTU_DISCOVERY,	"do_pmtu_discovery",	0,	bool,	"Do Path MTU Discovery. Default: OFF")						\


#define L2TP_TUNNEL_ID_ARGS																\
	ARG(TUNNEL_ID,		"tunnel_id",		0,	uint16,	"Tunnel ID of tunnel."),							\
	ARG(TUNNEL_NAME,	"tunnel_name",		0,	string,	"Administrative name of tunnel.")						\

static struct cli_arg_entry l2tp_args_tunnel_create[] = {
	ARG(DEST_IPADDR,	"dest_ipaddr",		0,	ipaddr,	"Destination IP address"),
	ARG(CONFIG_ID,		"config_id",		0,	uint32,	("Optional configuration id, used to uniquify a tunnel when there is more "
									 "the one tunnel between the same two IP addresses")),
	ARG(TUNNEL_NAME,	"tunnel_name",		0,	string,	"Administrative name of tunnel."),
#ifdef L2TP_TEST
	ARG(TUNNEL_ID,		"tunnel_id",		0,	uint16,	"Optional tunnel id of new tunnel. Usually auto-generated. For testing only."),
#endif
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of tunnel profile which will be used for default values of this "
									 "tunnel's parameters.")),
	L2TP_TUNNEL_CREATE_ARGS,
	L2TP_TUNNEL_MODIFY_ARGS,
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_tunnel_delete[] = {
	L2TP_TUNNEL_ID_ARGS,
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_tunnel_modify[] = {
	L2TP_TUNNEL_ID_ARGS,
	L2TP_TUNNEL_MODIFY_ARGS,															\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_tunnel_show[] = {
	L2TP_TUNNEL_ID_ARGS,
	FLG(SHOW_CONFIG,	"config",				"Display only tunnel configuration/status information."),
	FLG(SHOW_TRANSPORT,	"transport",				"Display only tunnel transport information."),
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_tunnel_list[] = {
	FLG(LIST_LOCAL_ONLY,	"local_only",				"List only locally created tunnels."),
	FLG(LIST_REMOTE_ONLY,	"remote_only",				"List only remotely created tunnels."),
	FLG(LIST_NAMES,		"names",				"List only named tunnels."),
	{ NULL, },
};

static int l2tp_parse_tunnel_arg(l2tp_tunnel_arg_ids_t arg_id, struct cli_node *arg, char *arg_value, struct l2tp_api_tunnel_msg_data *msg)
{
	int result = -EINVAL;

	if (arg_value == NULL) {
		arg_value = empty_string;
	}

	switch (arg_id) {
	case L2TP_TUNNEL_ARGID_PROFILE_NAME:
		OPTSTRING(msg->tunnel_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->tunnel_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->tunnel_profile_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_FLAG_PROFILE_NAME;
		break;
	case L2TP_TUNNEL_ARGID_TRACE_FLAGS:
		result = l2tp_parse_debug_mask(&msg->trace_flags, arg_value, 1);
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_TUNNEL_FLAG_TRACE_FLAGS;
		break;
	case L2TP_TUNNEL_ARGID_AUTH_MODE:
		if (strcasecmp(arg_value, "none") == 0) {
			msg->auth_mode = L2TP_API_TUNNEL_AUTH_MODE_NONE;
		} else if (strcasecmp(arg_value, "simple") == 0) {
			msg->auth_mode = L2TP_API_TUNNEL_AUTH_MODE_SIMPLE;
		} else if (strcasecmp(arg_value, "challenge") == 0) {
			msg->auth_mode = L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE;
		} else {
			fprintf(stderr, "Bad authmode %s: expecting none|simple|challenge\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_TUNNEL_FLAG_AUTH_MODE;
		break;
	case L2TP_TUNNEL_ARGID_MAX_SESSIONS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->max_sessions, msg->flags, L2TP_API_TUNNEL_FLAG_MAX_SESSIONS);
		break;
	case L2TP_TUNNEL_ARGID_HIDE_AVPS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->hide_avps, msg->flags, L2TP_API_TUNNEL_FLAG_HIDE_AVPS);
		break;
	case L2TP_TUNNEL_ARGID_UDP_CSUMS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_udp_checksums, msg->flags, L2TP_API_TUNNEL_FLAG_USE_UDP_CHECKSUMS);
		break;
	case L2TP_TUNNEL_ARGID_PERSIST:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->persist, msg->flags, L2TP_API_TUNNEL_FLAG_PERSIST);
		break;
	case L2TP_TUNNEL_ARGID_DO_PMTU_DISCOVERY:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->do_pmtu_discovery, msg->flags, L2TP_API_TUNNEL_FLAG_DO_PMTU_DISCOVERY);
		break;
	case L2TP_TUNNEL_ARGID_MTU:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->mtu, msg->flags, L2TP_API_TUNNEL_FLAG_MTU);
		break;
	case L2TP_TUNNEL_ARGID_HELLO_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->hello_timeout, msg->flags, L2TP_API_TUNNEL_FLAG_HELLO_TIMEOUT);
		break;
	case L2TP_TUNNEL_ARGID_MAX_RETRIES:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->max_retries, msg->flags, L2TP_API_TUNNEL_FLAG_MAX_RETRIES);
		break;
	case L2TP_TUNNEL_ARGID_RX_WINDOW_SIZE:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->rx_window_size, msg->flags, L2TP_API_TUNNEL_FLAG_RX_WINDOW_SIZE);
		break;
	case L2TP_TUNNEL_ARGID_TX_WINDOW_SIZE:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->tx_window_size, msg->flags, L2TP_API_TUNNEL_FLAG_TX_WINDOW_SIZE);
		break;
	case L2TP_TUNNEL_ARGID_RETRY_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->retry_timeout, msg->flags, L2TP_API_TUNNEL_FLAG_RETRY_TIMEOUT);
		break;
	case L2TP_TUNNEL_ARGID_IDLE_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->idle_timeout, msg->flags, L2TP_API_TUNNEL_FLAG_IDLE_TIMEOUT);
		break;
	case L2TP_TUNNEL_ARGID_DEST_IPADDR:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->peer_addr, msg->flags, L2TP_API_TUNNEL_FLAG_PEER_ADDR);
		break;
	case L2TP_TUNNEL_ARGID_CONFIG_ID:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->config_id, msg->flags, L2TP_API_TUNNEL_FLAG_CONFIG_ID);
		break;
	case L2TP_TUNNEL_ARGID_SRC_IPADDR:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->our_addr, msg->flags, L2TP_API_TUNNEL_FLAG_OUR_ADDR);
		break;
	case L2TP_TUNNEL_ARGID_OUR_UDP_PORT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->our_udp_port, msg->flags, L2TP_API_TUNNEL_FLAG_OUR_UDP_PORT);
		break;
	case L2TP_TUNNEL_ARGID_PEER_UDP_PORT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->peer_udp_port, msg->flags, L2TP_API_TUNNEL_FLAG_PEER_UDP_PORT);
		break;
	case L2TP_TUNNEL_ARGID_USE_TIEBREAKER:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_tiebreaker, msg->flags, L2TP_API_TUNNEL_FLAG_USE_TIEBREAKER);
		break;
	case L2TP_TUNNEL_ARGID_ALLOW_PPP_PROXY:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->allow_ppp_proxy, msg->flags, L2TP_API_TUNNEL_FLAG_ALLOW_PPP_PROXY);
		break;
	case L2TP_TUNNEL_ARGID_FRAMING_CAP:
		if (strcasecmp(arg_value, "sync") == 0) {
			msg->framing_cap_sync = TRUE;
			msg->framing_cap_async = FALSE;
		} else if (strcasecmp(arg_value, "async") == 0) {
			msg->framing_cap_sync = FALSE;
			msg->framing_cap_async = TRUE;
		} else if (strcasecmp(arg_value, "any") == 0) {
			msg->framing_cap_sync = TRUE;
			msg->framing_cap_async = TRUE;
		} else if (strcasecmp(arg_value, "none") == 0) {
			msg->framing_cap_sync = FALSE;
			msg->framing_cap_async = FALSE;
		} else {
			fprintf(stderr, "Bad framing capabilities %s: expecting none|sync|async|any\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_TUNNEL_FLAG_FRAMING_CAP;
		break;
	case L2TP_TUNNEL_ARGID_BEARER_CAP:
		if (strcasecmp(arg_value, "digital") == 0) {
			msg->bearer_cap_digital = TRUE;
			msg->bearer_cap_analog = FALSE;
		} else if (strcasecmp(arg_value, "analog") == 0) {
			msg->bearer_cap_digital = FALSE;
			msg->bearer_cap_analog = TRUE;
		} else if (strcasecmp(arg_value, "any") == 0) {
			msg->bearer_cap_digital = TRUE;
			msg->bearer_cap_analog = TRUE;
		} else if (strcasecmp(arg_value, "none") == 0) {
			msg->bearer_cap_digital = FALSE;
			msg->bearer_cap_analog = FALSE;
		} else {
			fprintf(stderr, "Bad bearer capabilities %s: expecting none|digital|analog|any\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_TUNNEL_FLAG_BEARER_CAP;
		break;
	case L2TP_TUNNEL_ARGID_HOST_NAME:
		OPTSTRING(msg->host_name) = strdup(arg_value);
		if (OPTSTRING(msg->host_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->host_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_FLAG_HOST_NAME;
		break;
	case L2TP_TUNNEL_ARGID_SECRET:
		OPTSTRING(msg->secret) = strdup(arg_value);
		if (OPTSTRING(msg->secret) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->secret.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_FLAG_SECRET;
		break;
	case L2TP_TUNNEL_ARGID_TUNNEL_ID:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->tunnel_id, msg->flags, L2TP_API_TUNNEL_FLAG_TUNNEL_ID);
		break;
	case L2TP_TUNNEL_ARGID_TUNNEL_NAME:
		OPTSTRING(msg->tunnel_name) = strdup(arg_value);
		if (OPTSTRING(msg->tunnel_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->tunnel_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_FLAG_TUNNEL_NAME;
		break;
	case L2TP_TUNNEL_ARGID_PEER_PROFILE_NAME:
		OPTSTRING(msg->peer_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->peer_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->peer_profile_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_FLAG_PEER_PROFILE_NAME;
		break;
	case L2TP_TUNNEL_ARGID_SESSION_PROFILE_NAME:
		OPTSTRING(msg->session_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->session_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->session_profile_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_FLAG_SESSION_PROFILE_NAME;
		break;
	case L2TP_TUNNEL_ARGID_PPP_PROFILE_NAME:
		OPTSTRING(msg->ppp_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->ppp_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->ppp_profile_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_FLAG_PPP_PROFILE_NAME;
		break;
	case L2TP_TUNNEL_ARGID_INTERFACE_NAME:
		OPTSTRING(msg->interface_name) = strdup(arg_value);
		if (OPTSTRING(msg->interface_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->interface_name.valid = 1;
		msg->flags2 |= L2TP_API_TUNNEL_FLAG_INTERFACE_NAME;
		break;
	case L2TP_TUNNEL_ARGID_SHOW_CONFIG:
	case L2TP_TUNNEL_ARGID_SHOW_TRANSPORT:
	case L2TP_TUNNEL_ARGID_LIST_LOCAL_ONLY:
	case L2TP_TUNNEL_ARGID_LIST_REMOTE_ONLY:
	case L2TP_TUNNEL_ARGID_LIST_NAMES:
		break;
	}

	result = 0;

out:
	return result;
}

static int l2tp_act_tunnel_create(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_tunnel_msg_data msg = {0,0, };
	L2TP_ACT_DECLARATIONS(60, l2tp_tunnel_arg_ids_t, int);

	msg.our_udp_port = 1701;
	msg.our_addr.s_addr = INADDR_ANY;
	
	L2TP_ACT_BEGIN() {
		result = l2tp_parse_tunnel_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	result = l2tp_tunnel_create_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Created tunnel %hu\n", clnt_res & 0xffff);
	}

out:
	return result;
}

static int l2tp_act_tunnel_delete(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	uint16_t tunnel_id = 0;
	optstring tunnel_name = { 0, };
	optstring reason = { 0, };
	int flags;
	L2TP_ACT_DECLARATIONS(4, l2tp_tunnel_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_TUNNEL_ARGID_TUNNEL_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], tunnel_id, flags, L2TP_API_TUNNEL_FLAG_TUNNEL_ID);
			break;
		case L2TP_TUNNEL_ARGID_TUNNEL_NAME:
			OPTSTRING(tunnel_name) = strdup(arg_values[arg]);
			if (OPTSTRING(tunnel_name) == NULL) {
				result = -ENOMEM;
				goto out;
			}
			tunnel_name.valid = 1;
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if ((tunnel_id == 0) && (tunnel_name.valid == 0)) {
		fprintf(stderr, "Required tunnel_id or tunnel_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_tunnel_delete_1(tunnel_id, tunnel_name, reason, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		if (tunnel_id != 0) {
			fprintf(stderr, "Deleted tunnel %hu\n", tunnel_id);
		} else {
			fprintf(stderr, "Deleted tunnel %s\n", OPTSTRING_PTR(tunnel_name));
		}
	}

out:
	return result;
}

static int l2tp_act_tunnel_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_tunnel_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(40, l2tp_tunnel_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_tunnel_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if ((msg.tunnel_id == 0) && (msg.tunnel_name.valid == 0)) {
		fprintf(stderr, "Required tunnel_id or tunnel_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_tunnel_modify_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		if (msg.tunnel_id != 0) {
			fprintf(stderr, "Modified tunnel %hu\n", msg.tunnel_id);
		} else {
			fprintf(stderr, "Modified tunnel %s\n", OPTSTRING_PTR(msg.tunnel_name));
		}
	}

out:
	return result;
}

static void l2tp_act_tunnel_cleanup(struct l2tp_api_tunnel_msg_data *msg)
{
	if (OPTSTRING(msg->state) != NULL) free(OPTSTRING(msg->state));
	if (OPTSTRING(msg->host_name) != NULL) free(OPTSTRING(msg->host_name));
	if (OPTSTRING(msg->secret) != NULL) free(OPTSTRING(msg->secret));
	if (OPTSTRING(msg->tunnel_name) != NULL) free(OPTSTRING(msg->tunnel_name));
	if (OPTSTRING(msg->tunnel_profile_name) != NULL) free(OPTSTRING(msg->tunnel_profile_name));
	if (msg->tiebreaker.tiebreaker_val != NULL) free(msg->tiebreaker.tiebreaker_val);
	if (OPTSTRING(msg->result_code_message) != NULL) free(OPTSTRING(msg->result_code_message));
	if (OPTSTRING(msg->peer.host_name) != NULL) free(OPTSTRING(msg->peer.host_name));
	if (OPTSTRING(msg->peer.vendor_name) != NULL) free(OPTSTRING(msg->peer.vendor_name));
	if (OPTSTRING(msg->peer.result_code_message) != NULL) free(OPTSTRING(msg->peer.result_code_message));
	if (msg->peer.tiebreaker.tiebreaker_val != NULL) free(msg->peer.tiebreaker.tiebreaker_val);
}

static int l2tp_act_tunnel_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	uint16_t tunnel_id = 0;
	optstring tunnel_name = { 0, };
	int flags;
	L2TP_ACT_DECLARATIONS(8, l2tp_tunnel_arg_ids_t, struct l2tp_api_tunnel_msg_data);
	int show_config_only = FALSE;
	int show_transport_only = FALSE;

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_TUNNEL_ARGID_TUNNEL_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], tunnel_id, flags, L2TP_API_TUNNEL_FLAG_TUNNEL_ID);
			break;
		case L2TP_TUNNEL_ARGID_TUNNEL_NAME:
			OPTSTRING(tunnel_name) = strdup(arg_values[arg]);
			if (OPTSTRING(tunnel_name) == NULL) {
				result = -ENOMEM;
				goto out;
			}
			tunnel_name.valid = 1;
			break;
		case L2TP_TUNNEL_ARGID_SHOW_CONFIG:
			show_config_only = TRUE;
			break;
		case L2TP_TUNNEL_ARGID_SHOW_TRANSPORT:
			show_transport_only = TRUE;
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if ((tunnel_id == 0) && (tunnel_name.valid == 0)) {
		fprintf(stderr, "Required tunnel_id or tunnel_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}

	memset(&clnt_res, 0, sizeof(clnt_res));
	result = l2tp_tunnel_get_1(tunnel_id, tunnel_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	result = 0;
	if (clnt_res.result_code == 0) {
		l2tp_show_tunnel(stdout, &clnt_res, show_config_only, show_transport_only);
	} else {
		fprintf(stderr, "get tunnel failed: %s\n", l2tp_strerror(-clnt_res.result_code));
	}

out:
	return result;
}

static int l2tp_id_compare(const void *id1, const void *id2)
{
	uint16_t my_id1 = *(uint16_t *) id1;
	uint16_t my_id2 = *(uint16_t *) id2;

	return ((my_id1 > my_id2) ? 1 :
		(my_id1 < my_id2) ? -1 : 0);
}

static int l2tp_name_compare(const void *name1, const void *name2)
{
	char *my_name1 = *((char **) name1);
	char *my_name2 = *((char **) name2);

	return strcmp(my_name1, my_name2);
}

static int l2tp_act_tunnel_list(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_tunnel_msg_data config;
	int id;
	int num_tunnels;
	int local_only = 0;
	int remote_only = 0;
	int names = 0;
	int brief;
	L2TP_ACT_DECLARATIONS(8, l2tp_tunnel_arg_ids_t, struct l2tp_api_tunnel_list_msg_data);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_TUNNEL_ARGID_LIST_LOCAL_ONLY:
			local_only = 1;
			break;
		case L2TP_TUNNEL_ARGID_LIST_REMOTE_ONLY:
			remote_only = 1;
			break;
		case L2TP_TUNNEL_ARGID_LIST_NAMES:
			names = 1;
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (local_only && remote_only) {
		fprintf(stderr, "Cannot use local_only and remote_only together\n");
		result = -EINVAL;
		goto out;
	}
	brief = local_only || remote_only || names;

  	memset(&clnt_res, 0, sizeof(clnt_res));

	result = l2tp_tunnel_list_1(&clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result));
		result = clnt_res.result;
		goto out;
	}

	num_tunnels = clnt_res.tunnel_ids.tunnel_ids_len;

	if (num_tunnels > 0) {
		/* Sort the list of tunnel_ids */
		qsort(&clnt_res.tunnel_ids.tunnel_ids_val[0], num_tunnels, 
		      sizeof(clnt_res.tunnel_ids.tunnel_ids_val[0]), l2tp_id_compare);
		if (!brief) {
			printf("%c %6s %16s %16s %8s %8s %16s\n", ' ', "TunId", "Peer", "Local", "PeerTId", "ConfigId", "State");
		}
	}
	for (id = 0; id < num_tunnels; id++) {
		char peer_ip[16];
		char our_ip[16];
		char *ip;
		struct in_addr ip_addr;
		optstring tunnel_name = { 0, };

		memset(&config, 0, sizeof(config));
		result = l2tp_tunnel_get_1(clnt_res.tunnel_ids.tunnel_ids_val[id], tunnel_name, &config, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}
		if (config.result_code < 0) {
			continue;
		}
		ip_addr.s_addr = config.peer_addr.s_addr;
		ip = inet_ntoa(ip_addr);
		strcpy(&peer_ip[0], ip);
		ip_addr.s_addr = config.our_addr.s_addr;
		ip = inet_ntoa(ip_addr);
		strcpy(&our_ip[0], ip);
		if (brief) {
			if (local_only && !config.created_by_admin) {
				continue;
			}
			if (remote_only && config.created_by_admin) {
				continue;
			}
			if (names && (OPTSTRING_PTR(config.tunnel_name) == NULL)) {
				continue;
			}
			if (names) {
				printf("\t%s\n", OPTSTRING(config.tunnel_name));
			} else {
				printf("\t%hu\n", config.tunnel_id);
			}
		} else {
			printf("%c %6d %16s %16s %8d %8d %16s\n", 
			       config.created_by_admin ? ' ' : '*',
			       config.tunnel_id, peer_ip, our_ip, config.peer_tunnel_id, config.config_id, OPTSTRING(config.state));
		}
		l2tp_act_tunnel_cleanup(&config);
	}	

out:
	return 0;
}

/*****************************************************************************
 * Tunnel profiles
 *****************************************************************************/

static struct cli_arg_entry l2tp_args_tunnel_profile_create[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of tunnel profile")),							\
	ARG(DEST_IPADDR,	"dest_ipaddr",		0,	ipaddr,	"Destination IP address"),							\
	L2TP_TUNNEL_CREATE_ARGS,															\
	L2TP_TUNNEL_MODIFY_ARGS,															\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_tunnel_profile_delete[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of tunnel profile")),							\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_tunnel_profile_modify[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of tunnel profile")),							\
	ARG(DEST_IPADDR,	"dest_ipaddr",		0,	ipaddr,	"Destination IP address"),							\
	L2TP_TUNNEL_CREATE_ARGS,															\
	L2TP_TUNNEL_MODIFY_ARGS,															\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_tunnel_profile_show[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of tunnel profile")),							\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_tunnel_profile_unset[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of tunnel profile")), 					\
	FLG(DEST_IPADDR,	"dest_ipaddr",		"Destination IP address"),							\
	FLG(SRC_IPADDR,		"src_ipaddr",		"Source IP address"),								\
	FLG(TRACE_FLAGS, 	"trace_flags", 		"Trace flags, for debugging network problems"),					\
	FLG(UDP_CSUMS,		"use_udp_checksums",	"Use UDP checksums in data frames. Default: ON"),				\
	FLG(HELLO_TIMEOUT,	"hello_timeout",	("Set timeout used for periodic L2TP Hello messages (in seconds). "		\
							 "Default: 0 (no hello messages are generated.")),				\
	FLG(MAX_RETRIES,	"max_retries",		"Maximum transmit retries before assuming tunnel failure."),			\
	FLG(RETRY_TIMEOUT,	"retry_timeout",	"Retry timeout - initial delay between retries."),				\
	FLG(IDLE_TIMEOUT,	"idle_timeout",		"Idle timeout - automatically delete tunnel if no sessions."),			\
	FLG(MAX_SESSIONS,	"max_sessions",		"Maximum number of sessions allowed on tunnel. Default=0 (limited only "	\
	    						"by max_sessions limit in system parameters)."),				\
	FLG(MTU,		"mtu",			"MTU for all sessions in tunnel. Default: 1460."),				\
	FLG(PEER_PROFILE_NAME,	"peer_profile_name",	("Name of peer profile which will be used for default values of the " 		\
							 "tunnel's parameters.")),							\
	FLG(SESSION_PROFILE_NAME, "session_profile_name", ("Name of session profile which will be used for default values of the " 	\
							   "tunnel's session parameters.")), \
	FLG(PPP_PROFILE_NAME,	"ppp_profile_name",	("Name of ppp profile which will be used for default values of the " 		\
							 "tunnel's session PPP parameters.")),						\
	FLG(INTERFACE_NAME,	"interface_name",	("Name of system interface for the tunnel. Default: l2tpN where N is tunnel_id.")), \
	FLG(PEER_UDP_PORT,	"peer_udp_port",	"UDP port number with which to contact peer L2TP server. Default: 1701"),	\
	FLG(OUR_UDP_PORT,	"our_udp_port",		"Local UDP port number with which to contact peer L2TP server. "		\
							"Default: autogenerated"),							\
	FLG(USE_TIEBREAKER,	"use_tiebreaker",	"Enable use of a tiebreaker when setting up the tunnel. Default: ON"),		\
	FLG(ALLOW_PPP_PROXY,	"allow_ppp_proxy",	"Allow PPP proxy"),								\
	FLG(FRAMING_CAP,	"framing_caps",		("Framing capabilities:-\n"							\
							 "none, sync, async, any")), 							\
	FLG(BEARER_CAP,		"bearer_caps",		("Bearer capabilities:-\n"							\
							 "none, digital, analog, any")),						\
	FLG(HOST_NAME,		"host_name",		"Name to advertise to peer when setting up the tunnel."),			\
	FLG(SECRET,		"secret",		("Optional secret which is shared with tunnel peer. Must be specified when "	\
							 "hide_avps is enabled.")),							\
	FLG(AUTH_MODE,		"auth_mode",		"Tunnel authentication mode. Default: none"),					\
	FLG(HIDE_AVPS,		"hide_avps",		"Hide AVPs. Default OFF"),							\
	FLG(RX_WINDOW_SIZE,	"rx_window_size",	"Rx window size"),								\
	FLG(TX_WINDOW_SIZE,	"tx_window_size",	"Tx window size"),								\
	FLG(DO_PMTU_DISCOVERY,	"do_pmtu_discovery",	"Do Path MTU Discovery. Default: OFF"),						\
	{ NULL, },
};

static int l2tp_parse_tunnel_profile_arg(l2tp_tunnel_arg_ids_t arg_id, struct cli_node *arg, char *arg_value, struct l2tp_api_tunnel_profile_msg_data *msg)
{
	int result = -EINVAL;

	if (arg_value == NULL) {
		arg_value = empty_string;
	}

	switch (arg_id) {
	case L2TP_TUNNEL_ARGID_PROFILE_NAME:
		msg->profile_name = strdup(arg_value);
		if (msg->profile_name == NULL) {
			result = -ENOMEM;
			goto out;
		}
		break;
	case L2TP_TUNNEL_ARGID_TRACE_FLAGS:
		result = l2tp_parse_debug_mask(&msg->trace_flags, arg_value, 1);
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS;
		break;
	case L2TP_TUNNEL_ARGID_AUTH_MODE:
		if (strcasecmp(arg_value, "none") == 0) {
			msg->auth_mode = L2TP_API_TUNNEL_AUTH_MODE_NONE;
		} else if (strcasecmp(arg_value, "simple") == 0) {
			msg->auth_mode = L2TP_API_TUNNEL_AUTH_MODE_SIMPLE;
		} else if (strcasecmp(arg_value, "challenge") == 0) {
			msg->auth_mode = L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE;
		} else {
			fprintf(stderr, "Bad authmode %s: expecting none|simple|challenge\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_TUNNEL_PROFILE_FLAG_AUTH_MODE;
		break;
	case L2TP_TUNNEL_ARGID_MAX_SESSIONS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->max_sessions, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_MAX_SESSIONS);
		break;
	case L2TP_TUNNEL_ARGID_HIDE_AVPS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->hide_avps, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS);
		break;
	case L2TP_TUNNEL_ARGID_UDP_CSUMS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_udp_checksums, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_USE_UDP_CHECKSUMS);
		break;
	case L2TP_TUNNEL_ARGID_DO_PMTU_DISCOVERY:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->do_pmtu_discovery, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_DO_PMTU_DISCOVERY);
		break;
	case L2TP_TUNNEL_ARGID_MTU:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->mtu, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_MTU);
		break;
	case L2TP_TUNNEL_ARGID_HELLO_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->hello_timeout, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_HELLO_TIMEOUT);
		break;
	case L2TP_TUNNEL_ARGID_MAX_RETRIES:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->max_retries, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_MAX_RETRIES);
		break;
	case L2TP_TUNNEL_ARGID_RX_WINDOW_SIZE:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->rx_window_size, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_RX_WINDOW_SIZE);
		break;
	case L2TP_TUNNEL_ARGID_TX_WINDOW_SIZE:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->tx_window_size, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_TX_WINDOW_SIZE);
		break;
	case L2TP_TUNNEL_ARGID_RETRY_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->retry_timeout, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_RETRY_TIMEOUT);
		break;
	case L2TP_TUNNEL_ARGID_IDLE_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->idle_timeout, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_IDLE_TIMEOUT);
		break;
	case L2TP_TUNNEL_ARGID_DEST_IPADDR:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->peer_addr, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_PEER_ADDR);
		break;
	case L2TP_TUNNEL_ARGID_SRC_IPADDR:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->our_addr, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_OUR_ADDR);
		break;
	case L2TP_TUNNEL_ARGID_OUR_UDP_PORT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->our_udp_port, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_OUR_UDP_PORT);
		break;
	case L2TP_TUNNEL_ARGID_PEER_UDP_PORT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->peer_udp_port, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_PEER_UDP_PORT);
		break;
	case L2TP_TUNNEL_ARGID_USE_TIEBREAKER:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_tiebreaker, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_USE_TIEBREAKER);
		break;
	case L2TP_TUNNEL_ARGID_ALLOW_PPP_PROXY:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->allow_ppp_proxy, msg->flags, L2TP_API_TUNNEL_PROFILE_FLAG_ALLOW_PPP_PROXY);
		break;
	case L2TP_TUNNEL_ARGID_FRAMING_CAP:
		if (strcasecmp(arg_value, "sync") == 0) {
			msg->framing_cap_sync = TRUE;
			msg->framing_cap_async = FALSE;
		} else if (strcasecmp(arg_value, "async") == 0) {
			msg->framing_cap_sync = FALSE;
			msg->framing_cap_async = TRUE;
		} else if (strcasecmp(arg_value, "any") == 0) {
			msg->framing_cap_sync = TRUE;
			msg->framing_cap_async = TRUE;
		} else if (strcasecmp(arg_value, "none") == 0) {
			msg->framing_cap_sync = FALSE;
			msg->framing_cap_async = FALSE;
		} else {
			fprintf(stderr, "Bad framing capabilities %s: expecting none|sync|async|any\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_TUNNEL_PROFILE_FLAG_FRAMING_CAP;
		break;
	case L2TP_TUNNEL_ARGID_BEARER_CAP:
		if (strcasecmp(arg_value, "digital") == 0) {
			msg->bearer_cap_digital = TRUE;
			msg->bearer_cap_analog = FALSE;
		} else if (strcasecmp(arg_value, "analog") == 0) {
			msg->bearer_cap_digital = FALSE;
			msg->bearer_cap_analog = TRUE;
		} else if (strcasecmp(arg_value, "any") == 0) {
			msg->bearer_cap_digital = TRUE;
			msg->bearer_cap_analog = TRUE;
		} else if (strcasecmp(arg_value, "none") == 0) {
			msg->bearer_cap_digital = FALSE;
			msg->bearer_cap_analog = FALSE;
		} else {
			fprintf(stderr, "Bad bearer capabilities %s: expecting none|digital|analog|any\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_TUNNEL_PROFILE_FLAG_BEARER_CAP;
		break;
	case L2TP_TUNNEL_ARGID_HOST_NAME:
		OPTSTRING(msg->host_name) = strdup(arg_value);
		if (OPTSTRING(msg->host_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->host_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HOST_NAME;
		break;
	case L2TP_TUNNEL_ARGID_SECRET:
		OPTSTRING(msg->secret) = strdup(arg_value);
		if (OPTSTRING(msg->secret) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->secret.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_PROFILE_FLAG_SECRET;
		break;
	case L2TP_TUNNEL_ARGID_PEER_PROFILE_NAME:
		OPTSTRING(msg->peer_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->peer_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->peer_profile_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_PROFILE_NAME;
		break;
	case L2TP_TUNNEL_ARGID_SESSION_PROFILE_NAME:
		OPTSTRING(msg->session_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->session_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->session_profile_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_PROFILE_FLAG_SESSION_PROFILE_NAME;
		break;
	case L2TP_TUNNEL_ARGID_PPP_PROFILE_NAME:
		OPTSTRING(msg->ppp_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->ppp_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->ppp_profile_name.valid = 1;
		msg->flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PPP_PROFILE_NAME;
		break;
	case L2TP_TUNNEL_ARGID_CONFIG_ID:
	case L2TP_TUNNEL_ARGID_TUNNEL_ID:
	case L2TP_TUNNEL_ARGID_TUNNEL_NAME:
	case L2TP_TUNNEL_ARGID_INTERFACE_NAME:
	case L2TP_TUNNEL_ARGID_SHOW_CONFIG:
	case L2TP_TUNNEL_ARGID_SHOW_TRANSPORT:
	case L2TP_TUNNEL_ARGID_PERSIST:
	case L2TP_TUNNEL_ARGID_LIST_LOCAL_ONLY:
	case L2TP_TUNNEL_ARGID_LIST_REMOTE_ONLY:
	case L2TP_TUNNEL_ARGID_LIST_NAMES:
		/* not valid for tunnel profiles */
		result = -EINVAL;
		goto out;
	}

	result = 0;

out:
	return result;
}

static int l2tp_act_tunnel_profile_create(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_tunnel_profile_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(40, l2tp_tunnel_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_tunnel_profile_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_tunnel_profile_create_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Created tunnel profile %s\n", msg.profile_name);
	}

out:
	return result;
}

static int l2tp_act_tunnel_profile_delete(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *profile_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_tunnel_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_TUNNEL_ARGID_PROFILE_NAME:
			profile_name = arg_values[arg];
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_tunnel_profile_delete_1(profile_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Deleted tunnel profile %s\n", profile_name);
	}

out:
	return result;
}

static int l2tp_act_tunnel_profile_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_tunnel_profile_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(40, l2tp_tunnel_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_tunnel_profile_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_tunnel_profile_modify_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Modified tunnel profile %s\n", msg.profile_name);
	}

out:
	return result;
}

static int l2tp_act_tunnel_profile_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *profile_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_tunnel_arg_ids_t, struct l2tp_api_tunnel_profile_msg_data);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_TUNNEL_ARGID_PROFILE_NAME:
			profile_name = arg_values[arg];
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	memset(&clnt_res, 0, sizeof(clnt_res));
	result = l2tp_tunnel_profile_get_1(profile_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	if (clnt_res.result_code < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result_code));
		result = clnt_res.result_code;
		goto out;
	}

	l2tp_show_tunnel_profile(stdout, &clnt_res);
out:
	return result;
}

static int l2tp_act_tunnel_profile_list(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_tunnel_profile_list_msg_data clnt_res;
	struct l2tp_api_tunnel_profile_list_entry *walk;
	int result;
	const char **profile_names;
	int index;

	memset(&clnt_res, 0, sizeof(clnt_res));

	result = l2tp_tunnel_profile_list_1(&clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result));
		result = clnt_res.result;
		goto out;
	}

	if (clnt_res.num_profiles > 0) {
		profile_names = calloc(clnt_res.num_profiles, sizeof(profile_names[0]));
		if (profile_names == NULL) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(ENOMEM));
			goto out;
		}
	
		walk = clnt_res.profiles;
		for (index = 0; index < clnt_res.num_profiles; index++) {
			if ((walk == NULL) || (walk->profile_name[0] == '\0')) {
				break;
			}
			profile_names[index] = walk->profile_name;
			walk = walk->next;
		}	

		/* Sort the profile names */
		qsort(&profile_names[0], index, sizeof(profile_names[0]), l2tp_name_compare);

		for (index = 0; index < clnt_res.num_profiles; index++) {
			printf("\t%s\n", profile_names[index]);
		}

		free(profile_names);
	}

	result = 0;

out:
	return result;
}

static int l2tp_act_tunnel_profile_unset(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_tunnel_profile_unset_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(40, l2tp_tunnel_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_TUNNEL_ARGID_PROFILE_NAME:
			msg.profile_name = strdup(arg_values[arg]);
			if (msg.profile_name == NULL) {
				result = -ENOMEM;
				goto out;
			}
			break;
		case L2TP_TUNNEL_ARGID_TRACE_FLAGS:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS;
			break;
		case L2TP_TUNNEL_ARGID_AUTH_MODE:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_AUTH_MODE;
			break;
		case L2TP_TUNNEL_ARGID_MAX_SESSIONS:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_MAX_SESSIONS;
			break;
		case L2TP_TUNNEL_ARGID_HIDE_AVPS:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS;
			break;
		case L2TP_TUNNEL_ARGID_UDP_CSUMS:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_USE_UDP_CHECKSUMS;
			break;
		case L2TP_TUNNEL_ARGID_DO_PMTU_DISCOVERY:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_DO_PMTU_DISCOVERY;
			break;
		case L2TP_TUNNEL_ARGID_MTU:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_MTU;
			break;
		case L2TP_TUNNEL_ARGID_HELLO_TIMEOUT:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HELLO_TIMEOUT;
			break;
		case L2TP_TUNNEL_ARGID_MAX_RETRIES:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_MAX_RETRIES;
			break;
		case L2TP_TUNNEL_ARGID_RX_WINDOW_SIZE:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_RX_WINDOW_SIZE;
			break;
		case L2TP_TUNNEL_ARGID_TX_WINDOW_SIZE:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_TX_WINDOW_SIZE;
			break;
		case L2TP_TUNNEL_ARGID_RETRY_TIMEOUT:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_RETRY_TIMEOUT;
			break;
		case L2TP_TUNNEL_ARGID_IDLE_TIMEOUT:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_IDLE_TIMEOUT;
			break;
		case L2TP_TUNNEL_ARGID_DEST_IPADDR:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_ADDR;
			break;
		case L2TP_TUNNEL_ARGID_SRC_IPADDR:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_OUR_ADDR;
			break;
		case L2TP_TUNNEL_ARGID_OUR_UDP_PORT:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_OUR_UDP_PORT;
			break;
		case L2TP_TUNNEL_ARGID_PEER_UDP_PORT:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_UDP_PORT;
			break;
		case L2TP_TUNNEL_ARGID_USE_TIEBREAKER:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_USE_TIEBREAKER;
			break;
		case L2TP_TUNNEL_ARGID_ALLOW_PPP_PROXY:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_ALLOW_PPP_PROXY;
			break;
		case L2TP_TUNNEL_ARGID_FRAMING_CAP:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_FRAMING_CAP;
			break;
		case L2TP_TUNNEL_ARGID_BEARER_CAP:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_BEARER_CAP;
			break;
		case L2TP_TUNNEL_ARGID_HOST_NAME:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HOST_NAME;
			break;
		case L2TP_TUNNEL_ARGID_SECRET:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_SECRET;
			break;
		case L2TP_TUNNEL_ARGID_PEER_PROFILE_NAME:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_PROFILE_NAME;
			break;
		case L2TP_TUNNEL_ARGID_SESSION_PROFILE_NAME:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_SESSION_PROFILE_NAME;
			break;
		case L2TP_TUNNEL_ARGID_PPP_PROFILE_NAME:
			msg.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PPP_PROFILE_NAME;
			break;
		case L2TP_TUNNEL_ARGID_CONFIG_ID:
		case L2TP_TUNNEL_ARGID_TUNNEL_ID:
		case L2TP_TUNNEL_ARGID_TUNNEL_NAME:
		case L2TP_TUNNEL_ARGID_INTERFACE_NAME:
		case L2TP_TUNNEL_ARGID_SHOW_CONFIG:
		case L2TP_TUNNEL_ARGID_SHOW_TRANSPORT:
		case L2TP_TUNNEL_ARGID_PERSIST:
		case L2TP_TUNNEL_ARGID_LIST_LOCAL_ONLY:
		case L2TP_TUNNEL_ARGID_LIST_REMOTE_ONLY:
		case L2TP_TUNNEL_ARGID_LIST_NAMES:
			/* not valid for tunnel profiles */
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	result = 0;

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_tunnel_profile_unset_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Unset parameters of tunnel profile %s\n", msg.profile_name);
	}

out:
	return result;
}

/*****************************************************************************
 * Sessions
 *****************************************************************************/

typedef enum {
	L2TP_SESSION_ARGID_TUNNEL_NAME,
	L2TP_SESSION_ARGID_TUNNEL_ID,
	L2TP_SESSION_ARGID_SESSION_ID,
	L2TP_SESSION_ARGID_TRACE_FLAGS,
	L2TP_SESSION_ARGID_PROFILE_NAME,
	L2TP_SESSION_ARGID_PPP_PROFILE_NAME,
	L2TP_SESSION_ARGID_SEQUENCING_REQUIRED,
	L2TP_SESSION_ARGID_USE_SEQUENCE_NUMBERS,
	L2TP_SESSION_ARGID_REORDER_TIMEOUT,
	L2TP_SESSION_ARGID_SESSION_TYPE,
	L2TP_SESSION_ARGID_PRIV_GROUP_ID,
	L2TP_SESSION_ARGID_SESSION_NAME,
	L2TP_SESSION_ARGID_INTERFACE_NAME,
	L2TP_SESSION_ARGID_USER_NAME,
	L2TP_SESSION_ARGID_USER_PASSWORD,
	L2TP_SESSION_ARGID_FRAMING_TYPE,
	L2TP_SESSION_ARGID_BEARER_TYPE,
	L2TP_SESSION_ARGID_MINIMUM_BPS,
	L2TP_SESSION_ARGID_MAXIMUM_BPS,
	L2TP_SESSION_ARGID_CONNECT_SPEED,
	L2TP_SESSION_ARGID_USE_PPP_PROXY,
	L2TP_SESSION_ARGID_PROXY_AUTH_TYPE,
	L2TP_SESSION_ARGID_PROXY_AUTH_NAME,
	L2TP_SESSION_ARGID_PROXY_AUTH_CHALLENGE,
	L2TP_SESSION_ARGID_PROXY_AUTH_RESPONSE,
	L2TP_SESSION_ARGID_CALLING_NUMBER,
	L2TP_SESSION_ARGID_CALLED_NUMBER,
	L2TP_SESSION_ARGID_SUB_ADDRESS,
	L2TP_SESSION_ARGID_INITIAL_RCVD_LCP_CONFREQ,
	L2TP_SESSION_ARGID_LAST_SENT_LCP_CONFREQ,
	L2TP_SESSION_ARGID_LAST_RCVD_LCP_CONFREQ,
	L2TP_SESSION_ARGID_NO_PPP,
	L2TP_SESSION_ARGID_LIST_LOCAL_ONLY,
	L2TP_SESSION_ARGID_LIST_REMOTE_ONLY,
	L2TP_SESSION_ARGID_LIST_NAMES,
} l2tp_session_arg_ids_t;
 
#undef ARG
#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_SESSION_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#undef FLG
#define FLG(id, name, doc) \
	{ name, { L2TP_SESSION_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

/* Paremeters for Create and Modify operations */
#define L2TP_SESSION_MODIFY_ARGS	\
	ARG(TRACE_FLAGS, 		"trace_flags", 		0, 	string,	"Trace flags, for debugging network problems"),				\
	ARG(SEQUENCING_REQUIRED,	"sequencing_required",	0, 	bool, 	"The use of sequence numbers in the data channel is mandatory."),	\
	ARG(USE_SEQUENCE_NUMBERS,	"use_sequence_numbers",	0, 	bool, 	"Enable sequence numbers in the data channel if peer supports them."),	\
	ARG(REORDER_TIMEOUT,		"reorder_timeout",	0, 	uint32, "Timeout to wait for out-of-sequence packets before discarding.")	\


#define L2TP_SESSION_ID_ARGS 																\
	ARG(TUNNEL_ID,			"tunnel_id",		0,	uint16,	"Tunnel ID in which session exists."),					\
	ARG(TUNNEL_NAME,		"tunnel_name",		0,	string,	"Administrative name of tunnel in which session exists."),		\
	ARG(SESSION_ID,			"session_id",		0,	uint16,	"Session ID of session."),						\
	ARG(SESSION_NAME,		"session_name",		0,	string,	"Administrative name of session")					\

static struct cli_arg_entry l2tp_args_session_create[] = {
	ARG(TUNNEL_ID,			"tunnel_id",		0,	uint16,	"Tunnel ID in which to create session."),
	ARG(TUNNEL_NAME,		"tunnel_name",		0,	string,	"Administrative name of tunnel in which session to create session."),	\
	ARG(SESSION_NAME,		"session_name",		0,	string,	"Administrative name of session"),
#ifdef L2TP_TEST
	ARG(SESSION_ID,			"session_id",		0,	uint16,	"Session ID of session, usually auto-generated. For testing only."),
#endif
	ARG(PROFILE_NAME,		"profile_name",		0,	string,	("Name of session profile")), 
	ARG(PPP_PROFILE_NAME,		"ppp_profile_name",	0,	string,	("Name of ppp profile to use for PPP parameters")),
	L2TP_SESSION_MODIFY_ARGS,
	ARG(SESSION_TYPE,		"session_type",		0,	string,	"Session type: LAC/LNS incoming/outgoing"),
	ARG(PRIV_GROUP_ID,		"priv_group_id",	0,	string,	"Private group ID, used to separate this session into a named administrative group"),
	ARG(INTERFACE_NAME,		"interface_name",	0,	string,	"PPP interface name.Default: pppN"),
	ARG(USER_NAME,			"user_name",		0,	string,	"PPP user name"),
	ARG(USER_PASSWORD,		"user_password",	0,	string,	"PPP user password"),
	ARG(FRAMING_TYPE,		"framing_type",		0,	string,	"Framing type: sync, async or any. Default: any (derive from tunnel)"),
	ARG(BEARER_TYPE,		"bearer_type",		0,	string,	"Bearer type: none, digital, analog, any. Default: any (derive from tunnel)"),
	ARG(MINIMUM_BPS,		"minimum_bps",		0,	uint32,	"Minimum bits/sec acceptable. Default: 0"),
	ARG(MAXIMUM_BPS,		"maximum_bps",		0,	uint32,	"Maximum bits/sec required. Default: no limit"),
	ARG(CONNECT_SPEED,		"connect_speed",	0,	string,	"Specified as speed[:txspeed], indicates connection speeds."),
	ARG(NO_PPP,			"no_ppp",		0, 	bool, 	"Don't start PPP on the L2TP session."),
#ifdef L2TP_TEST
	ARG(USE_PPP_PROXY,		"use_ppp_proxy",	0,	bool,	""),
	ARG(PROXY_AUTH_TYPE,		"proxy_auth_type",	0,	int32,	""),
	ARG(PROXY_AUTH_NAME,		"proxy_auth_name",	0,	string,	""),
	ARG(PROXY_AUTH_CHALLENGE,	"proxy_auth_challenge",	0,	hex,	""),
	ARG(PROXY_AUTH_RESPONSE,	"proxy_auth_response",	0,	hex,	""),
	ARG(CALLING_NUMBER,		"calling_number",	0,	string,	""),
	ARG(CALLED_NUMBER,		"called_number",	0,	string,	""),
	ARG(SUB_ADDRESS,		"sub_address",		0,	string,	""),
	ARG(INITIAL_RCVD_LCP_CONFREQ,	"initial_rcvd_lcp_confreq", 0, hex,	""),
	ARG(LAST_SENT_LCP_CONFREQ,	"last_sent_lcp_confreq", 0,	hex,	""),
	ARG(LAST_RCVD_LCP_CONFREQ,	"last_rcvd_lcp_confreq", 0,	hex,	""),
#endif /* L2TP_TEST */
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_session_delete[] = {
	L2TP_SESSION_ID_ARGS,
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_session_modify[] = {
	L2TP_SESSION_ID_ARGS,
	L2TP_SESSION_MODIFY_ARGS,															\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_session_show[] = {
	L2TP_SESSION_ID_ARGS,
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_session_list[] = {
	ARG(TUNNEL_ID,			"tunnel_id",		0,	uint16,	"Tunnel ID in which to list sessions."),				\
	ARG(TUNNEL_NAME,		"tunnel_name",		0,	string,	"Administrative name of tunnel to list."),				\
	FLG(LIST_LOCAL_ONLY,		"local_only",				 "Show only locally created sessions."),				\
	FLG(LIST_REMOTE_ONLY,		"remote_only",				 "Show only remotely created sessions."),				\
	FLG(LIST_NAMES,			"names",				 "Show only named sessions."),						\
	{ NULL, },
};

static int l2tp_parse_session_arg(l2tp_session_arg_ids_t arg_id, struct cli_node *arg, char *arg_value, struct l2tp_api_session_msg_data *msg)
{
	int result = -EINVAL;
	int ints[2];
	int num_matches;

	if (arg_value == NULL) {
		arg_value = empty_string;
	}

	switch (arg_id) {
	case L2TP_SESSION_ARGID_TUNNEL_NAME:
		OPTSTRING(msg->tunnel_name) = strdup(arg_value);
		if (OPTSTRING(msg->tunnel_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->tunnel_name.valid = 1;
		break;
	case L2TP_SESSION_ARGID_TUNNEL_ID:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->tunnel_id, msg->flags, 0);
		break;
	case L2TP_SESSION_ARGID_SESSION_ID:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->session_id, msg->flags, 0);
		break;
	case L2TP_SESSION_ARGID_TRACE_FLAGS:
		result = l2tp_parse_debug_mask(&msg->trace_flags, arg_value, 1);
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_SESSION_FLAG_TRACE_FLAGS;
		break;
	case L2TP_SESSION_ARGID_PROFILE_NAME:
		OPTSTRING(msg->profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->profile_name.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_PROFILE_NAME;
		break;
	case L2TP_SESSION_ARGID_PPP_PROFILE_NAME:
		OPTSTRING(msg->ppp_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->ppp_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->ppp_profile_name.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_PPP_PROFILE_NAME;
		break;
	case L2TP_SESSION_ARGID_SEQUENCING_REQUIRED:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->sequencing_required, msg->flags, L2TP_API_SESSION_FLAG_SEQUENCING_REQUIRED);
		break;
	case L2TP_SESSION_ARGID_USE_SEQUENCE_NUMBERS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_sequence_numbers, msg->flags, L2TP_API_SESSION_FLAG_USE_SEQUENCE_NUMBERS);
		break;
	case L2TP_SESSION_ARGID_NO_PPP:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->no_ppp, msg->flags, L2TP_API_SESSION_FLAG_NO_PPP);
		break;
	case L2TP_SESSION_ARGID_REORDER_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->reorder_timeout, msg->flags, L2TP_API_SESSION_FLAG_REORDER_TIMEOUT);
		break;
	case L2TP_SESSION_ARGID_SESSION_TYPE:
		if (strcasecmp(arg_value, "laic") == 0) {
			msg->session_type = L2TP_API_SESSION_TYPE_LAIC;
		} else if (strcasecmp(arg_value, "laoc") == 0) {
			msg->session_type = L2TP_API_SESSION_TYPE_LAOC;
		} else if (strcasecmp(arg_value, "lnic") == 0) {
			msg->session_type = L2TP_API_SESSION_TYPE_LNIC;
		} else if (strcasecmp(arg_value, "lnoc") == 0) {
			msg->session_type = L2TP_API_SESSION_TYPE_LNOC;
		} else {
			fprintf(stderr, "Bad session type %s: expecting laic|laoc|lnic|lnoc\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_SESSION_FLAG_SESSION_TYPE;
		break;
	case L2TP_SESSION_ARGID_PRIV_GROUP_ID:
		OPTSTRING(msg->priv_group_id) = strdup(arg_value);
		if (OPTSTRING(msg->priv_group_id) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->priv_group_id.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_PRIV_GROUP_ID;
		break;
	case L2TP_SESSION_ARGID_SESSION_NAME:
		OPTSTRING(msg->session_name) = strdup(arg_value);
		if (OPTSTRING(msg->session_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->session_name.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_SESSION_NAME;
		break;
	case L2TP_SESSION_ARGID_INTERFACE_NAME:
		OPTSTRING(msg->interface_name) = strdup(arg_value);
		if (OPTSTRING(msg->interface_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->interface_name.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_INTERFACE_NAME;
		break;
	case L2TP_SESSION_ARGID_USER_NAME:
		OPTSTRING(msg->user_name) = strdup(arg_value);
		if (OPTSTRING(msg->user_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->user_name.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_USER_NAME;
		break;
	case L2TP_SESSION_ARGID_USER_PASSWORD:
		OPTSTRING(msg->user_password) = strdup(arg_value);
		if (OPTSTRING(msg->user_password) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->user_password.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_USER_PASSWORD;
		break;
	case L2TP_SESSION_ARGID_FRAMING_TYPE:
		if (strcasecmp(arg_value, "sync") == 0) {
			msg->framing_type_sync = TRUE;
			msg->framing_type_async = FALSE;
		} else if (strcasecmp(arg_value, "async") == 0) {
			msg->framing_type_sync = FALSE;
			msg->framing_type_async = TRUE;
		} else if (strcasecmp(arg_value, "any") == 0) {
			msg->framing_type_sync = TRUE;
			msg->framing_type_async = TRUE;
		} else if (strcasecmp(arg_value, "none") == 0) {
			msg->framing_type_sync = FALSE;
			msg->framing_type_async = FALSE;
		} else {
			fprintf(stderr, "Bad framing type %s: expecting none|sync|async|any\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_SESSION_FLAG_FRAMING_TYPE;
		break;
	case L2TP_SESSION_ARGID_BEARER_TYPE:
		if (strcasecmp(arg_value, "digital") == 0) {
			msg->bearer_type_digital = TRUE;
			msg->bearer_type_analog = FALSE;
		} else if (strcasecmp(arg_value, "analog") == 0) {
			msg->bearer_type_digital = FALSE;
			msg->bearer_type_analog = TRUE;
		} else if (strcasecmp(arg_value, "any") == 0) {
			msg->bearer_type_digital = TRUE;
			msg->bearer_type_analog = TRUE;
		} else if (strcasecmp(arg_value, "none") == 0) {
			msg->bearer_type_digital = FALSE;
			msg->bearer_type_analog = FALSE;
		} else {
			fprintf(stderr, "Bad bearer type %s: expecting none|digital|analog|any\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_SESSION_FLAG_BEARER_TYPE;
		break;
	case L2TP_SESSION_ARGID_MINIMUM_BPS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->minimum_bps, msg->flags, L2TP_API_SESSION_FLAG_MINIMUM_BPS);
		break;
	case L2TP_SESSION_ARGID_MAXIMUM_BPS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->maximum_bps, msg->flags, L2TP_API_SESSION_FLAG_MAXIMUM_BPS);
		break;
	case L2TP_SESSION_ARGID_CONNECT_SPEED:
		num_matches = sscanf(arg_value, "%d:%d", &ints[0], &ints[1]);
		if (num_matches >= 1) {
			msg->rx_connect_speed = ints[0];
			msg->tx_connect_speed = ints[0];
			if (num_matches == 2) {
				msg->tx_connect_speed = ints[1];
			}
			msg->flags |= L2TP_API_SESSION_FLAG_CONNECT_SPEED;
		} else {
			fprintf(stderr, "Expecting connect_speed[:tx_connect_speed]\n");
			goto out;
		}
		break;
#ifdef L2TP_TEST
		/* It is useful to fake these parameters using the CLI for testing.
		 * These parameters would only be used by an automated PPP call
		 * application, such as a BRAS. 
		 */ 
	case L2TP_SESSION_ARGID_USE_PPP_PROXY:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_ppp_proxy, msg->flags, L2TP_API_SESSION_FLAG_USE_PPP_PROXY);
		break;
	case L2TP_SESSION_ARGID_PROXY_AUTH_TYPE:
		if (strcasecmp(arg_value, "text") == 0) {
			msg->proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PLAIN_TEXT;
		} else if (strcasecmp(arg_value, "chap") == 0) {
			msg->proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_CHAP;
		} else if (strcasecmp(arg_value, "pap") == 0) {
			msg->proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_PAP;
		} else if (strcasecmp(arg_value, "none") == 0) {
			msg->proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_NO_AUTH;
		} else if (strcasecmp(arg_value, "mschap") == 0) {
			msg->proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_MSCHAPV1;
		} else {
			fprintf(stderr, "Bad auth type %s: expecting none|text|chap|pap|mschap\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_TYPE;
		break;
	case L2TP_SESSION_ARGID_PROXY_AUTH_NAME:
		OPTSTRING(msg->proxy_auth_name) = strdup(arg_value);
		if (OPTSTRING(msg->proxy_auth_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->proxy_auth_name.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_NAME;
		break;
	case L2TP_SESSION_ARGID_PROXY_AUTH_CHALLENGE:
		msg->proxy_auth_challenge.proxy_auth_challenge_len = strlen(arg_value) / 2;
		msg->proxy_auth_challenge.proxy_auth_challenge_val = malloc(msg->proxy_auth_challenge.proxy_auth_challenge_len + 2 /* slack */);
		if (msg->proxy_auth_challenge.proxy_auth_challenge_val == NULL) {
			result = -ENOMEM;
			goto out;
		}
		result = cli_arg_parse_hex(arg, arg_value, msg->proxy_auth_challenge.proxy_auth_challenge_val);
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_CHALLENGE;
		break;
	case L2TP_SESSION_ARGID_PROXY_AUTH_RESPONSE:
		msg->proxy_auth_response.proxy_auth_response_len = strlen(arg_value) / 2;
		msg->proxy_auth_response.proxy_auth_response_val = malloc(msg->proxy_auth_response.proxy_auth_response_len + 2 /* slack */);
		if (msg->proxy_auth_response.proxy_auth_response_val == NULL) {
			result = -ENOMEM;
			goto out;
		}
		result = cli_arg_parse_hex(arg, arg_value, msg->proxy_auth_response.proxy_auth_response_val);
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_RESPONSE;
		break;
	case L2TP_SESSION_ARGID_CALLING_NUMBER:
		OPTSTRING(msg->calling_number) = strdup(arg_value);
		if (OPTSTRING(msg->calling_number) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->calling_number.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_CALLING_NUMBER;
		break;
	case L2TP_SESSION_ARGID_CALLED_NUMBER:
		OPTSTRING(msg->called_number) = strdup(arg_value);
		if (OPTSTRING(msg->called_number) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->called_number.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_CALLED_NUMBER;
		break;
	case L2TP_SESSION_ARGID_SUB_ADDRESS:
		OPTSTRING(msg->sub_address) = strdup(arg_value);
		if (OPTSTRING(msg->sub_address) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->sub_address.valid = 1;
		msg->flags |= L2TP_API_SESSION_FLAG_SUB_ADDRESS;
		break;
	case L2TP_SESSION_ARGID_INITIAL_RCVD_LCP_CONFREQ:
		msg->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_len = strlen(arg_value) / 2;
		msg->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val = malloc(msg->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_len + 2 /* slack */);
		if (msg->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val == NULL) {
			result = -ENOMEM;
			goto out;
		}
		result = cli_arg_parse_hex(arg, arg_value, msg->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val);	
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_SESSION_FLAG_INITIAL_RCVD_LCP_CONFREQ;
		break;
	case L2TP_SESSION_ARGID_LAST_SENT_LCP_CONFREQ:
		msg->last_sent_lcp_confreq.last_sent_lcp_confreq_len = strlen(arg_value) / 2;
		msg->last_sent_lcp_confreq.last_sent_lcp_confreq_val = malloc(msg->last_sent_lcp_confreq.last_sent_lcp_confreq_len + 2 /* slack */);
		if (msg->last_sent_lcp_confreq.last_sent_lcp_confreq_val == NULL) {
			result = -ENOMEM;
			goto out;
		}
		result = cli_arg_parse_hex(arg, arg_value, msg->last_sent_lcp_confreq.last_sent_lcp_confreq_val);
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_SESSION_FLAG_LAST_SENT_LCP_CONFREQ;
		break;
	case L2TP_SESSION_ARGID_LAST_RCVD_LCP_CONFREQ:
		msg->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_len = strlen(arg_value) / 2;
		msg->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val = malloc(msg->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_len + 2 /* slack */);
		if (msg->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val == NULL) {
			result = -ENOMEM;
			goto out;
		}
		result = cli_arg_parse_hex(arg, arg_value, msg->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val);
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_SESSION_FLAG_LAST_RCVD_LCP_CONFREQ;
		break;
#else
	case L2TP_SESSION_ARGID_USE_PPP_PROXY:
	case L2TP_SESSION_ARGID_PROXY_AUTH_TYPE:
	case L2TP_SESSION_ARGID_PROXY_AUTH_NAME:
	case L2TP_SESSION_ARGID_PROXY_AUTH_CHALLENGE:
	case L2TP_SESSION_ARGID_PROXY_AUTH_RESPONSE:
	case L2TP_SESSION_ARGID_CALLING_NUMBER:
	case L2TP_SESSION_ARGID_CALLED_NUMBER:
	case L2TP_SESSION_ARGID_SUB_ADDRESS:
	case L2TP_SESSION_ARGID_INITIAL_RCVD_LCP_CONFREQ:
	case L2TP_SESSION_ARGID_LAST_SENT_LCP_CONFREQ:
	case L2TP_SESSION_ARGID_LAST_RCVD_LCP_CONFREQ:
		result = -EOPNOTSUPP;
		goto out;
#endif /* L2TP_TEST */

	case L2TP_SESSION_ARGID_LIST_LOCAL_ONLY:
	case L2TP_SESSION_ARGID_LIST_REMOTE_ONLY:
	case L2TP_SESSION_ARGID_LIST_NAMES:
		result = -EINVAL;
		goto out;
	}

	result = 0;

out:
	return result;
}

static int l2tp_act_session_create(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_session_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(80, l2tp_session_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_session_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if ((msg.tunnel_id == 0) && (!msg.tunnel_name.valid)) {
		fprintf(stderr, "Required tunnel_id / tunnel_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_session_create_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		if (msg.tunnel_id != 0) {
			fprintf(stderr, "Created session %hu/%hu\n", msg.tunnel_id, clnt_res & 0xffff);
		} else {
			fprintf(stderr, "Created session %hu on tunnel %s\n", clnt_res & 0xffff, OPTSTRING_PTR(msg.tunnel_name));
		}
	}

out:
	return result;
}

static int l2tp_act_session_delete(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	u_short tunnel_id = 0;
	u_short session_id = 0;
	optstring tunnel_name = { 0, };
	optstring session_name = { 0, };
	optstring reason = { 0, };
	int flags;
	L2TP_ACT_DECLARATIONS(6, l2tp_session_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_SESSION_ARGID_TUNNEL_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], tunnel_id, flags, 0);
			break;
		case L2TP_SESSION_ARGID_TUNNEL_NAME:
			OPTSTRING(tunnel_name) = strdup(arg_values[arg]);
			if (OPTSTRING(tunnel_name) == NULL) {
				result = -ENOMEM;
				goto out;
			}
			tunnel_name.valid = 1;
			break;
		case L2TP_SESSION_ARGID_SESSION_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], session_id, flags, 0);
			break;
		case L2TP_SESSION_ARGID_SESSION_NAME:
			OPTSTRING(session_name) = strdup(arg_values[arg]);
			if (OPTSTRING(session_name) == NULL) {
				result = -ENOMEM;
				goto out;
			}
			session_name.valid = 1;
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if ((tunnel_id == 0) && (tunnel_name.valid == 0)) {
		fprintf(stderr, "Required tunnel_id or tunnel_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}
	if ((session_id == 0) && (session_name.valid == 0)) {
		fprintf(stderr, "Required session_id or session_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_session_delete_1(tunnel_id, tunnel_name, session_id, session_name, reason, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		char tid[6];
		char sid[6];
		sprintf(&tid[0], "%hu", tunnel_id);
		sprintf(&sid[0], "%hu", session_id);
		fprintf(stderr, "Deleted session %s/%s\n", 
			tunnel_id == 0 ? OPTSTRING_PTR(tunnel_name) : tid,
			session_id == 0 ? OPTSTRING_PTR(session_name) : sid);
	}
out:
	return result;
}

static int l2tp_act_session_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_session_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(80, l2tp_session_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_session_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if ((msg.tunnel_id == 0) && (msg.tunnel_name.valid == 0)) {
		fprintf(stderr, "Required tunnel_id or tunnel_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}
	if ((msg.session_id == 0) && (msg.session_name.valid == 0)) {
		fprintf(stderr, "Required session_id or session_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_session_modify_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		char tid[6];
		char sid[6];
		sprintf(&tid[0], "%hu", msg.tunnel_id);
		sprintf(&sid[0], "%hu", msg.session_id);
		fprintf(stderr, "Modified session %s/%s\n", 
			msg.tunnel_id == 0 ? OPTSTRING_PTR(msg.tunnel_name) : tid,
			msg.session_id == 0 ? OPTSTRING_PTR(msg.session_name) : sid);
	}

out:
	return result;
}

static void l2tp_act_session_cleanup(struct l2tp_api_session_msg_data *msg)
{
	if (OPTSTRING(msg->tunnel_name) != NULL) free(OPTSTRING(msg->tunnel_name));
	if (OPTSTRING(msg->state) != NULL) free(OPTSTRING(msg->state));
	if (OPTSTRING(msg->session_name) != NULL) free(OPTSTRING(msg->session_name));
	if (OPTSTRING(msg->interface_name) != NULL) free(OPTSTRING(msg->interface_name));
	if (OPTSTRING(msg->user_name) != NULL) free(OPTSTRING(msg->user_name));
	if (OPTSTRING(msg->user_password) != NULL) free(OPTSTRING(msg->user_password));
	if (OPTSTRING(msg->priv_group_id) != NULL) free(OPTSTRING(msg->priv_group_id));
	if (OPTSTRING(msg->profile_name) != NULL) free(OPTSTRING(msg->profile_name));
	if (OPTSTRING(msg->create_time) != NULL) free(OPTSTRING(msg->create_time));
	if (OPTSTRING(msg->peer.error_message) != NULL) free(OPTSTRING(msg->peer.error_message));
	if (OPTSTRING(msg->peer.proxy_auth_name) != NULL) free(OPTSTRING(msg->peer.proxy_auth_name));
	if (msg->peer.proxy_auth_challenge.proxy_auth_challenge_val != NULL)
		free(msg->peer.proxy_auth_challenge.proxy_auth_challenge_val);
	if (msg->peer.proxy_auth_response.proxy_auth_response_val != NULL)
		free(msg->peer.proxy_auth_response.proxy_auth_response_val);
	if (OPTSTRING(msg->peer.private_group_id) != NULL) free(OPTSTRING(msg->peer.private_group_id));
	if (msg->peer.initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val != NULL)
		free(msg->peer.initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val);
	if (msg->peer.last_sent_lcp_confreq.last_sent_lcp_confreq_val != NULL)
		free(msg->peer.last_sent_lcp_confreq.last_sent_lcp_confreq_val);
	if (msg->peer.last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val != NULL)
		free(msg->peer.last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val);
	if (OPTSTRING(msg->peer.calling_number) != NULL) free(OPTSTRING(msg->peer.calling_number));
	if (OPTSTRING(msg->peer.called_number) != NULL) free(OPTSTRING(msg->peer.called_number));
	if (OPTSTRING(msg->peer.sub_address) != NULL) free(OPTSTRING(msg->peer.sub_address));
	if (OPTSTRING(msg->peer.q931_advisory_msg) != NULL) free(OPTSTRING(msg->peer.q931_advisory_msg));
	if (OPTSTRING(msg->proxy_auth_name) != NULL) free(OPTSTRING(msg->proxy_auth_name));
	if (msg->proxy_auth_challenge.proxy_auth_challenge_val != NULL)
		free(msg->proxy_auth_challenge.proxy_auth_challenge_val);
	if (msg->proxy_auth_response.proxy_auth_response_val != NULL)
		free(msg->proxy_auth_response.proxy_auth_response_val);
	if (OPTSTRING(msg->calling_number) != NULL) free(OPTSTRING(msg->calling_number));
	if (OPTSTRING(msg->called_number) != NULL) free(OPTSTRING(msg->called_number));
	if (OPTSTRING(msg->sub_address) != NULL) free(OPTSTRING(msg->sub_address));
	if (OPTSTRING(msg->ppp_profile_name) != NULL) free(OPTSTRING(msg->ppp_profile_name));
	if (msg->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val != NULL)
		free(msg->initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val);
	if (msg->last_sent_lcp_confreq.last_sent_lcp_confreq_val != NULL)
		free(msg->last_sent_lcp_confreq.last_sent_lcp_confreq_val);
	if (msg->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val != NULL)
		free(msg->last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val);
}

static int l2tp_act_session_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	u_short tunnel_id = 0;
	optstring tunnel_name = { 0, };
	u_short session_id = 0;
	optstring session_name = { 0, };
	int flags;
	L2TP_ACT_DECLARATIONS(6, l2tp_session_arg_ids_t, struct l2tp_api_session_msg_data);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_SESSION_ARGID_TUNNEL_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], tunnel_id, flags, 0);
			break;
		case L2TP_SESSION_ARGID_TUNNEL_NAME:
			OPTSTRING(tunnel_name) = strdup(arg_values[arg]);
			if (OPTSTRING(tunnel_name) == NULL) {
				result = -ENOMEM;
				goto out;
			}
			tunnel_name.valid = 1;
			break;
		case L2TP_SESSION_ARGID_SESSION_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], session_id, flags, 0);
			break;
		case L2TP_SESSION_ARGID_SESSION_NAME:
			OPTSTRING(session_name) = strdup(arg_values[arg]);
			if (OPTSTRING(session_name) == NULL) {
				result = -ENOMEM;
				goto out;
			}
			session_name.valid = 1;
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if ((tunnel_id == 0) && (tunnel_name.valid == 0)) {
		fprintf(stderr, "Required tunnel_id or tunnel_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}
	if ((session_id == 0) && (session_name.valid == 0)) {
		fprintf(stderr, "Required session_id or session_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}

	memset(&clnt_res, 0, sizeof(clnt_res));
	result = l2tp_session_get_1(tunnel_id, tunnel_name, session_id, session_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res.result_code < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result_code));
		result = clnt_res.result_code;
		goto out;
	}

	l2tp_show_session(stdout, &clnt_res);
	l2tp_act_session_cleanup(&clnt_res);
out:
	return result;
}

static int l2tp_act_session_list(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	u_short tunnel_id = 0;
	optstring tunnel_name = { 0, };
	optstring session_name = { 0, };
	int local_only = 0;
	int remote_only = 0;
	int names = 0;
	int get_session;
	int loop;
	int flags;
	L2TP_ACT_DECLARATIONS(8, l2tp_session_arg_ids_t, struct l2tp_api_session_list_msg_data);
	struct l2tp_api_session_msg_data config;

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_SESSION_ARGID_TUNNEL_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], tunnel_id, flags, 0);
			break;
		case L2TP_SESSION_ARGID_TUNNEL_NAME:
			OPTSTRING(tunnel_name) = strdup(arg_values[arg]);
			if (OPTSTRING(tunnel_name) == NULL) {
				result = -ENOMEM;
				goto out;
			}
			tunnel_name.valid = 1;
			break;
		case L2TP_SESSION_ARGID_LIST_LOCAL_ONLY:
			local_only = 1;
			break;
		case L2TP_SESSION_ARGID_LIST_REMOTE_ONLY:
			remote_only = 1;
			break;
		case L2TP_SESSION_ARGID_LIST_NAMES:
			names = 1;
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if ((tunnel_id == 0) && (tunnel_name.valid == 0)) {
		fprintf(stderr, "Required tunnel_id or tunnel_name argument is missing\n");
		result = -EINVAL;
		goto out;
	}
	if (local_only && remote_only) {
		fprintf(stderr, "Cannot use local_only and remote_only together\n");
		result = -EINVAL;
		goto out;
	}
	get_session = local_only || remote_only || names;

	memset(&clnt_res, 0, sizeof(clnt_res));

	result = l2tp_session_list_1(tunnel_id, tunnel_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result));
		result = clnt_res.result;
		goto out;
	}

	if (clnt_res.session_ids.session_ids_len > 0) {
		if (!get_session) {
			char tunnel_id_str[10];
			sprintf(&tunnel_id_str[0], "%hu", tunnel_id);
			printf("%hd sessions on tunnel %s:-\n", clnt_res.session_ids.session_ids_len, 
			       (tunnel_id != 0) ? tunnel_id_str : OPTSTRING_PTR(tunnel_name));
		}

		/* Sort the list of session ids */
		qsort(&clnt_res.session_ids.session_ids_val[0], clnt_res.session_ids.session_ids_len,
		      sizeof(clnt_res.session_ids.session_ids_val[0]), l2tp_id_compare);

		for (loop = 0; loop < clnt_res.session_ids.session_ids_len; loop++) {
			if (get_session) {
				memset(&config, 0, sizeof(config));
				result = l2tp_session_get_1(tunnel_id, tunnel_name, clnt_res.session_ids.session_ids_val[loop], 
							    session_name, &config, cl);
				if (result != RPC_SUCCESS) {
					continue;
				}
				if (config.result_code < 0) {
					continue;
				}
				if (local_only && !config.created_by_admin) {
					continue;
				}
				if (remote_only && config.created_by_admin) {
					continue;
				}
				if (names && (OPTSTRING_PTR(config.session_name) == NULL)) {
					continue;
				}
				if (names) {
					printf("\t%s\n", OPTSTRING(config.session_name));
				} else {
					printf("\t%hu\n", clnt_res.session_ids.session_ids_val[loop]);
				}
				l2tp_act_session_cleanup(&config);
			} else {
				printf("\t%hu\n", clnt_res.session_ids.session_ids_val[loop]);
			}
		}
	}	

out:
	return result;
}

/*****************************************************************************
 * Session profiles
 *****************************************************************************/

static struct cli_arg_entry l2tp_args_session_profile_create[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	"Name of session profile"),
	ARG(PPP_PROFILE_NAME,	"ppp_profile_name",	0,	string,	"Name of ppp profile to use for PPP parameters"),
	L2TP_SESSION_MODIFY_ARGS,
	ARG(SESSION_TYPE,	"session_type",		0,	string,	"Session type: LAC/LNS incoming/outgoing"),
	ARG(PRIV_GROUP_ID,	"priv_group_id",	0,	string,	"Private group ID, used to separate this session into a named administrative group"),
	ARG(FRAMING_TYPE,	"framing_type",		0,	string,	"Framing type: sync, async or any. Default: any (derived from tunnel)"),
	ARG(BEARER_TYPE,	"bearer_type",		0,	string,	"Bearer type: none, digital, analog, any. Default: any (derived from tunnel)"),
	ARG(MINIMUM_BPS,	"minimum_bps",		0,	uint32,	"Minimum bits/sec acceptable. Default: 0"),
	ARG(MAXIMUM_BPS,	"maximum_bps",		0,	uint32,	"Maximum bits/sec required. Default: no limit"),
	ARG(CONNECT_SPEED,	"connect_speed",	0,	uint32,	"Specified as speed[:txspeed, indicates connection speeds."),
	ARG(NO_PPP,		"no_ppp",		0, 	bool, 	"Don't start PPP on the L2TP session."),
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_session_profile_delete[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	"Name of session profile"),
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_session_profile_modify[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	"Name of session profile"),
	ARG(PPP_PROFILE_NAME,	"ppp_profile_name",	0,	string,	"Name of ppp profile to use for PPP parameters"),
	L2TP_SESSION_MODIFY_ARGS,
	ARG(SESSION_TYPE,	"session_type",		0,	string,	"Session type: LAC/LNS incoming/outgoing"),
	ARG(PRIV_GROUP_ID,	"priv_group_id",	0,	string,	"Private group ID, used to separate this session into a named administrative group"),
	ARG(FRAMING_TYPE,	"framing_type",		0,	string,	"Framing type: sync, async or any. Default: any (derived from tunnel)"),
	ARG(BEARER_TYPE,	"bearer_type",		0,	string,	"Bearer type: none, digital, analog, any. Default: any (derived from tunnel)"),
	ARG(MINIMUM_BPS,	"minimum_bps",		0,	uint32,	"Minimum bits/sec acceptable. Default: 0"),
	ARG(MAXIMUM_BPS,	"maximum_bps",		0,	uint32,	"Maximum bits/sec required. Default: no limit"),
	ARG(CONNECT_SPEED,	"connect_speed",	0,	uint32,	"Specified as speed[:txspeed, indicates connection speeds."),
	ARG(NO_PPP,		"no_ppp",		0, 	bool, 	"Don't start PPP on the L2TP session."),
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_session_profile_show[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	"Name of session profile"),
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_session_profile_unset[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	"Name of session profile"),
	FLG(TRACE_FLAGS, 	"trace_flags", 		"Trace flags, for debugging network problems. Default: none"),
	FLG(SEQUENCING_REQUIRED,"sequencing_required",	"The use of sequence numbers in the data channel is mandatory. Default: off"),
	FLG(USE_SEQUENCE_NUMBERS,"use_sequence_numbers","Enable sequence numbers in the data channel if peer supports them. Default: off"),
	FLG(REORDER_TIMEOUT,	"reorder_timeout",	"Timeout to wait for out-of-sequence packets before discarding."),
	FLG(PPP_PROFILE_NAME,	"ppp_profile_name",	"Name of ppp profile to use for PPP parameters. Default: not set"),
	FLG(SESSION_TYPE,	"session_type",		"Session type: LAC/LNS incoming/outgoing. Default: derived"),
	FLG(PRIV_GROUP_ID,	"priv_group_id",	"Private group ID, used to separate this session into a named administrative group. Default: none"),
	FLG(FRAMING_TYPE,	"framing_type",		"Framing type: sync, async or any. Default: any (derived from tunnel)"),
	FLG(BEARER_TYPE,	"bearer_type",		"Bearer type: none, digital, analog, any. Default: any (derived from tunnel)"),
	FLG(MINIMUM_BPS,	"minimum_bps",		"Minimum bits/sec acceptable. Default: 0"),
	FLG(MAXIMUM_BPS,	"maximum_bps",		"Maximum bits/sec required. Default: no limit"),
	FLG(CONNECT_SPEED,	"connect_speed",	"Indicates connection speeds. Default: not set"),
	FLG(NO_PPP,		"no_ppp",		"Don't start PPP on the L2TP session. Default: no"),
	{ NULL, },
};


static int l2tp_parse_session_profile_arg(l2tp_session_arg_ids_t arg_id, struct cli_node *arg, char *arg_value, struct l2tp_api_session_profile_msg_data *msg)
{
	int result = -EINVAL;
	int num_matches;
	int ints[2];

	if (arg_value == NULL) {
		arg_value = empty_string;
	}

	switch (arg_id) {
	case L2TP_SESSION_ARGID_PROFILE_NAME:
		msg->profile_name = strdup(arg_value);
		if (msg->profile_name == NULL) {
			result = -ENOMEM;
			goto out;
		}
		break;
	case L2TP_SESSION_ARGID_TRACE_FLAGS:
		result = l2tp_parse_debug_mask(&msg->trace_flags, arg_value, 1);
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_SESSION_PROFILE_FLAG_TRACE_FLAGS;
		break;
	case L2TP_SESSION_ARGID_SEQUENCING_REQUIRED:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->sequencing_required, msg->flags, L2TP_API_SESSION_PROFILE_FLAG_SEQUENCING_REQUIRED);
		break;
	case L2TP_SESSION_ARGID_USE_SEQUENCE_NUMBERS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_sequence_numbers, msg->flags, L2TP_API_SESSION_PROFILE_FLAG_USE_SEQUENCE_NUMBERS);
		break;
	case L2TP_SESSION_ARGID_NO_PPP:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->no_ppp, msg->flags, L2TP_API_SESSION_PROFILE_FLAG_NO_PPP);
		break;
	case L2TP_SESSION_ARGID_REORDER_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->reorder_timeout, msg->flags, L2TP_API_SESSION_PROFILE_FLAG_REORDER_TIMEOUT);
		break;
	case L2TP_SESSION_ARGID_PPP_PROFILE_NAME:
		OPTSTRING(msg->ppp_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->ppp_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->ppp_profile_name.valid = 1;
		msg->flags |= L2TP_API_SESSION_PROFILE_FLAG_PPP_PROFILE_NAME;
		break;
	case L2TP_SESSION_ARGID_SESSION_TYPE:
		if (strcasecmp(arg_value, "laic") == 0) {
			msg->session_type = L2TP_API_SESSION_TYPE_LAIC;
		} else if (strcasecmp(arg_value, "laoc") == 0) {
			msg->session_type = L2TP_API_SESSION_TYPE_LAOC;
		} else if (strcasecmp(arg_value, "lnic") == 0) {
			msg->session_type = L2TP_API_SESSION_TYPE_LNIC;
		} else if (strcasecmp(arg_value, "lnoc") == 0) {
			msg->session_type = L2TP_API_SESSION_TYPE_LNOC;
		} else {
			fprintf(stderr, "Bad session type %s: expecting laic|laoc|lnic|lnoc\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_SESSION_PROFILE_FLAG_SESSION_TYPE;
		break;
	case L2TP_SESSION_ARGID_PRIV_GROUP_ID:
		OPTSTRING(msg->priv_group_id) = strdup(arg_value);
		if (OPTSTRING(msg->priv_group_id) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->priv_group_id.valid = 1;
		msg->flags |= L2TP_API_SESSION_PROFILE_FLAG_PRIV_GROUP_ID;
		break;
	case L2TP_SESSION_ARGID_FRAMING_TYPE:
		if (strcasecmp(arg_value, "sync") == 0) {
			msg->framing_type_sync = TRUE;
			msg->framing_type_async = FALSE;
		} else if (strcasecmp(arg_value, "async") == 0) {
			msg->framing_type_sync = FALSE;
			msg->framing_type_async = TRUE;
		} else if (strcasecmp(arg_value, "any") == 0) {
			msg->framing_type_sync = TRUE;
			msg->framing_type_async = TRUE;
		} else if (strcasecmp(arg_value, "none") == 0) {
			msg->framing_type_sync = FALSE;
			msg->framing_type_async = FALSE;
		} else {
			fprintf(stderr, "Bad framing type %s: expecting none|sync|async|any\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_SESSION_PROFILE_FLAG_FRAMING_TYPE;
		break;
	case L2TP_SESSION_ARGID_BEARER_TYPE:
		if (strcasecmp(arg_value, "digital") == 0) {
			msg->bearer_type_digital = TRUE;
			msg->bearer_type_analog = FALSE;
		} else if (strcasecmp(arg_value, "analog") == 0) {
			msg->bearer_type_digital = FALSE;
			msg->bearer_type_analog = TRUE;
		} else if (strcasecmp(arg_value, "any") == 0) {
			msg->bearer_type_digital = TRUE;
			msg->bearer_type_analog = TRUE;
		} else if (strcasecmp(arg_value, "none") == 0) {
			msg->bearer_type_digital = FALSE;
			msg->bearer_type_analog = FALSE;
		} else {
			fprintf(stderr, "Bad bearer type %s: expecting none|digital|analog|any\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_SESSION_PROFILE_FLAG_BEARER_TYPE;
		break;
	case L2TP_SESSION_ARGID_MINIMUM_BPS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->minimum_bps, msg->flags, L2TP_API_SESSION_PROFILE_FLAG_MINIMUM_BPS);
		break;
	case L2TP_SESSION_ARGID_MAXIMUM_BPS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->maximum_bps, msg->flags, L2TP_API_SESSION_PROFILE_FLAG_MAXIMUM_BPS);
		break;
	case L2TP_SESSION_ARGID_CONNECT_SPEED:
		num_matches = sscanf(arg_value, "%d:%d", &ints[0], &ints[1]);
		if (num_matches >= 1) {
			msg->rx_connect_speed = ints[0];
			msg->tx_connect_speed = ints[0];
			if (num_matches == 2) {
				msg->tx_connect_speed = ints[1];
			}
			msg->flags |= L2TP_API_SESSION_PROFILE_FLAG_CONNECT_SPEED;
		} else {
			fprintf(stderr, "Expecting connect_speed[:tx_connect_speed]\n");
			goto out;
		}
		break;
	case L2TP_SESSION_ARGID_USE_PPP_PROXY:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_ppp_proxy, msg->flags, L2TP_API_SESSION_PROFILE_FLAG_USE_PPP_PROXY);
		break;
	case L2TP_SESSION_ARGID_TUNNEL_ID:
	case L2TP_SESSION_ARGID_TUNNEL_NAME:
	case L2TP_SESSION_ARGID_SESSION_ID:
	case L2TP_SESSION_ARGID_SESSION_NAME:
	case L2TP_SESSION_ARGID_INTERFACE_NAME:
	case L2TP_SESSION_ARGID_USER_NAME:
	case L2TP_SESSION_ARGID_USER_PASSWORD:
	case L2TP_SESSION_ARGID_PROXY_AUTH_TYPE:
	case L2TP_SESSION_ARGID_PROXY_AUTH_NAME:
	case L2TP_SESSION_ARGID_PROXY_AUTH_CHALLENGE:
	case L2TP_SESSION_ARGID_PROXY_AUTH_RESPONSE:
	case L2TP_SESSION_ARGID_CALLING_NUMBER:
	case L2TP_SESSION_ARGID_CALLED_NUMBER:
	case L2TP_SESSION_ARGID_SUB_ADDRESS:
	case L2TP_SESSION_ARGID_INITIAL_RCVD_LCP_CONFREQ:
	case L2TP_SESSION_ARGID_LAST_SENT_LCP_CONFREQ:
	case L2TP_SESSION_ARGID_LAST_RCVD_LCP_CONFREQ:
	case L2TP_SESSION_ARGID_LIST_LOCAL_ONLY:
	case L2TP_SESSION_ARGID_LIST_REMOTE_ONLY:
	case L2TP_SESSION_ARGID_LIST_NAMES:
		/* these are invalid in a session profile */
		result = -EINVAL;
		break;
	}

	result = 0;

out:
	return result;
}

static int l2tp_act_session_profile_create(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_session_profile_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(40, l2tp_session_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_session_profile_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_session_profile_create_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Created session profile %s\n", msg.profile_name);
	}

out:
	return result;
}

static int l2tp_act_session_profile_delete(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *profile_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_session_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_SESSION_ARGID_PROFILE_NAME:
			profile_name = arg_values[arg];
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_session_profile_delete_1(profile_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Deleted session profile %s\n", profile_name);
	}

out:
	return result;
}

static int l2tp_act_session_profile_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_session_profile_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(50, l2tp_session_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_session_profile_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_session_profile_modify_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Modified session profile %s\n", msg.profile_name);
	}

out:
	return result;
}

static int l2tp_act_session_profile_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *profile_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_session_arg_ids_t, struct l2tp_api_session_profile_msg_data);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_SESSION_ARGID_PROFILE_NAME:
			profile_name = arg_values[arg];
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	memset(&clnt_res, 0, sizeof(clnt_res));
	result = l2tp_session_profile_get_1(profile_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	if (clnt_res.result_code < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result_code));
		result = clnt_res.result_code;
		goto out;
	}

	l2tp_show_session_profile(stdout, &clnt_res);
out:
	return result;
}

static int l2tp_act_session_profile_list(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_session_profile_list_msg_data clnt_res;
	struct l2tp_api_session_profile_list_entry *walk;
	int result;
	const char **profile_names;
	int index;

	memset(&clnt_res, 0, sizeof(clnt_res));

	result = l2tp_session_profile_list_1(&clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result));
		result = clnt_res.result;
		goto out;
	}

	if (clnt_res.num_profiles > 0) {
		profile_names = calloc(clnt_res.num_profiles, sizeof(profile_names[0]));
		if (profile_names == NULL) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(ENOMEM));
			goto out;
		}
	
		walk = clnt_res.profiles;
		for (index = 0; index < clnt_res.num_profiles; index++) {
			if ((walk == NULL) || (walk->profile_name[0] == '\0')) {
				break;
			}
			profile_names[index] = walk->profile_name;
			walk = walk->next;
		}	

		/* Sort the profile names */
		qsort(&profile_names[0], index, sizeof(profile_names[0]), l2tp_name_compare);

		for (index = 0; index < clnt_res.num_profiles; index++) {
			printf("\t%s\n", profile_names[index]);
		}

		free(profile_names);
	}

	result = 0;

out:
	return result;
}

static int l2tp_act_session_profile_unset(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_session_profile_unset_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(50, l2tp_session_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_SESSION_ARGID_PROFILE_NAME:
			msg.profile_name = strdup(arg_values[arg]);
			if (msg.profile_name == NULL) {
				result = -ENOMEM;
				goto out;
			}
			break;
		case L2TP_SESSION_ARGID_TRACE_FLAGS:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_TRACE_FLAGS;
			break;
		case L2TP_SESSION_ARGID_SEQUENCING_REQUIRED:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_SEQUENCING_REQUIRED;
			break;
		case L2TP_SESSION_ARGID_USE_SEQUENCE_NUMBERS:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_USE_SEQUENCE_NUMBERS;
			break;
		case L2TP_SESSION_ARGID_NO_PPP:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_NO_PPP;
			break;
		case L2TP_SESSION_ARGID_REORDER_TIMEOUT:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_REORDER_TIMEOUT;
			break;
		case L2TP_SESSION_ARGID_PPP_PROFILE_NAME:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_PPP_PROFILE_NAME;
			break;
		case L2TP_SESSION_ARGID_SESSION_TYPE:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_SESSION_TYPE;
			break;
		case L2TP_SESSION_ARGID_PRIV_GROUP_ID:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_PRIV_GROUP_ID;
			break;
		case L2TP_SESSION_ARGID_FRAMING_TYPE:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_FRAMING_TYPE;
			break;
		case L2TP_SESSION_ARGID_BEARER_TYPE:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_BEARER_TYPE;
			break;
		case L2TP_SESSION_ARGID_MINIMUM_BPS:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_MINIMUM_BPS;
			break;
		case L2TP_SESSION_ARGID_MAXIMUM_BPS:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_MAXIMUM_BPS;
			break;
		case L2TP_SESSION_ARGID_CONNECT_SPEED:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_CONNECT_SPEED;
			break;
		case L2TP_SESSION_ARGID_USE_PPP_PROXY:
			msg.flags |= L2TP_API_SESSION_PROFILE_FLAG_USE_PPP_PROXY;
			break;
		case L2TP_SESSION_ARGID_TUNNEL_ID:
		case L2TP_SESSION_ARGID_TUNNEL_NAME:
		case L2TP_SESSION_ARGID_SESSION_ID:
		case L2TP_SESSION_ARGID_SESSION_NAME:
		case L2TP_SESSION_ARGID_INTERFACE_NAME:
		case L2TP_SESSION_ARGID_USER_NAME:
		case L2TP_SESSION_ARGID_USER_PASSWORD:
		case L2TP_SESSION_ARGID_PROXY_AUTH_TYPE:
		case L2TP_SESSION_ARGID_PROXY_AUTH_NAME:
		case L2TP_SESSION_ARGID_PROXY_AUTH_CHALLENGE:
		case L2TP_SESSION_ARGID_PROXY_AUTH_RESPONSE:
		case L2TP_SESSION_ARGID_CALLING_NUMBER:
		case L2TP_SESSION_ARGID_CALLED_NUMBER:
		case L2TP_SESSION_ARGID_SUB_ADDRESS:
		case L2TP_SESSION_ARGID_INITIAL_RCVD_LCP_CONFREQ:
		case L2TP_SESSION_ARGID_LAST_SENT_LCP_CONFREQ:
		case L2TP_SESSION_ARGID_LAST_RCVD_LCP_CONFREQ:
		case L2TP_SESSION_ARGID_LIST_LOCAL_ONLY:
		case L2TP_SESSION_ARGID_LIST_REMOTE_ONLY:
		case L2TP_SESSION_ARGID_LIST_NAMES:
			/* these are invalid in a session profile */
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_session_profile_unset_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Unset parameters of session profile %s\n", msg.profile_name);
	}

out:
	return result;
}


/*****************************************************************************
 * Ppp profiles
 *****************************************************************************/

typedef enum {
	L2TP_PPP_ARGID_TRACE_FLAGS,
	L2TP_PPP_ARGID_PROFILE_NAME,
	L2TP_PPP_ARGID_ASYNCMAP,
	L2TP_PPP_ARGID_MTU,
	L2TP_PPP_ARGID_MRU,
	L2TP_PPP_ARGID_SYNC_MODE,
	L2TP_PPP_ARGID_AUTH_PAP,
	L2TP_PPP_ARGID_AUTH_CHAP,
	L2TP_PPP_ARGID_AUTH_MSCHAP,
	L2TP_PPP_ARGID_AUTH_MSCHAPV2,
	L2TP_PPP_ARGID_AUTH_EAP,
	L2TP_PPP_ARGID_AUTH_NONE,
	L2TP_PPP_ARGID_AUTH_PEER,
	L2TP_PPP_ARGID_CHAP_INTERVAL,
	L2TP_PPP_ARGID_CHAP_MAX_CHALLENGE,
	L2TP_PPP_ARGID_CHAP_RESTART,
	L2TP_PPP_ARGID_PAP_MAX_AUTH_REQS,
	L2TP_PPP_ARGID_PAP_RESTART_INTVL,
	L2TP_PPP_ARGID_PAP_TIMEOUT,
	L2TP_PPP_ARGID_IDLE_TIMEOUT,
	L2TP_PPP_ARGID_IPCP_MAX_CFG_REQS,
	L2TP_PPP_ARGID_IPCP_MAX_CFG_NAKS,
	L2TP_PPP_ARGID_IPCP_MAX_TERM_REQS,
	L2TP_PPP_ARGID_IPCP_RETX_INTVL,
	L2TP_PPP_ARGID_LCP_ECHO_FAIL_COUNT,
	L2TP_PPP_ARGID_LCP_ECHO_INTERVAL,
	L2TP_PPP_ARGID_LCP_MAX_CFG_REQS,
	L2TP_PPP_ARGID_LCP_MAX_CFG_NAKS,
	L2TP_PPP_ARGID_LCP_MAX_TERM_REQS,
	L2TP_PPP_ARGID_LCP_RETX_INTVL,
	L2TP_PPP_ARGID_MAX_CONNECT_TIME,
	L2TP_PPP_ARGID_MAX_FAILURE_COUNT,
	L2TP_PPP_ARGID_LOCAL_IPADDR,
	L2TP_PPP_ARGID_REMOTE_IPADDR,
	L2TP_PPP_ARGID_DNS_IPADDR_PRI,
	L2TP_PPP_ARGID_DNS_IPADDR_SEC,
	L2TP_PPP_ARGID_WINS_IPADDR_PRI,
	L2TP_PPP_ARGID_WINS_IPADDR_SEC,
	L2TP_PPP_ARGID_IP_POOL_NAME,
	L2TP_PPP_ARGID_USE_RADIUS,
	L2TP_PPP_ARGID_RADIUS_HINT,
	L2TP_PPP_ARGID_USE_AS_DEFAULT_ROUTE,
	L2TP_PPP_ARGID_MULTILINK,
	L2TP_PPP_ARGID_LOCAL_NAME,
	L2TP_PPP_ARGID_REMOTE_NAME,
	L2TP_PPP_ARGID_PROXY_ARP,
} l2tp_ppp_arg_ids_t;
 
#undef ARG
#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_PPP_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#undef FLG
#define FLG(id, name, doc) \
	{ name, { L2TP_PPP_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

/* Paremeters for Create and Modify operations */
#define L2TP_PPP_MODIFY_ARGS 																\
	ARG(TRACE_FLAGS, 	"trace_flags", 		0, 	string,	"Trace flags, for debugging network problems"),					\
	ARG(ASYNCMAP, 		"asyncmap", 		0, 	uint32,	"Async character map. Valid only if PPP is async mode."),			\
	ARG(MTU, 		"mtu",	 		0, 	uint16,	"Maximum Transmit Unit (MTU) or maximum packet size transmitted."),		\
	ARG(MRU, 		"mru",	 		0, 	uint16,	"Maximum Receive Unit (MRU) or maximum packet size passed when received."),	\
	ARG(SYNC_MODE, 		"sync_mode", 		0, 	string,	"Allow PPP sync/async operation."),						\
	ARG(AUTH_PAP, 		"auth_pap", 		0, 	bool,	"Allow PPP PAP authentication. Default: YES"),					\
	ARG(AUTH_CHAP, 		"auth_chap", 		0, 	bool,	"Allow PPP CHAP authentication. Default: YES"),					\
	ARG(AUTH_MSCHAP,	"auth_mschapv1",	0, 	bool,	"Allow PPP MSCHAP authentication. Default: YES"),				\
	ARG(AUTH_MSCHAPV2, 	"auth_mschapv2", 	0, 	bool,	"Allow PPP MSCHAPV2 authentication. Default: YES"),				\
	ARG(AUTH_EAP, 		"auth_eap", 		0, 	bool,	"Allow PPP EAP authentication. Default: YES"),					\
	ARG(AUTH_NONE, 		"auth_none", 		0, 	bool,	"Allow unauthenticated peers. Default: NO/YES (server/client)"),		\
	ARG(AUTH_PEER, 		"auth_peer", 		0, 	bool,	"Authenticate peer PPP user (server). Default: YES/NO (server/client)"),	\
	ARG(CHAP_INTERVAL,	"chap_interval",	0,	int32,	("Rechallenge the peer every chap_interval seconds. "				\
									 "Default=0 (don't rechallenge).")),						\
	ARG(CHAP_MAX_CHALLENGE,	"chap_max_challenge",	0,	int32,	("Maximum number of CHAP challenges to transmit without successful "		\
									 "acknowledgment before declaring a failure. Default=10.")),			\
	ARG(CHAP_RESTART,	"chap_restart",		0,	int32,	("Retransmission timeout for CHAP challenges. Default=3.")),			\
	ARG(PAP_MAX_AUTH_REQS,	"pap_max_auth_requests", 0,	int32,	("Maximum number of PAP authenticate-request transmissions. Default=10.")),	\
	ARG(PAP_RESTART_INTVL,	"pap_restart_interval",	0,	int32,	"Retransmission timeout for PAP requests. Default=3."),				\
	ARG(PAP_TIMEOUT,	"pap_timeout",		0,	int32,	"Maximum time to wait for peer to authenticate itself. Default=0 (no limit)."),	\
	ARG(IDLE_TIMEOUT,	"idle_timeout",		0,	int32,	"Disconnect session if idle for more than N seconds. Default=0 (no limit)."),	\
	ARG(IPCP_MAX_CFG_REQS,	"ipcp_max_config_requests", 0,	int32,	("Maximum number of IPCP config-requests to transmit without successful "	\
	    								 "acknowledgement before declaring a failure. Default=10.")),			\
	ARG(IPCP_MAX_CFG_NAKS,	"ipcp_max_config_naks",	0,	int32,	("Maximum number of IPCP config-naks to allow before starting to send "		\
									 "config-rejects instead. Default=10.")),					\
	ARG(IPCP_MAX_TERM_REQS,	"ipcp_max_terminate_requests", 0, int32, "Maximum number of IPCP term-requests to send. Default=3."),			\
	ARG(IPCP_RETX_INTVL,	"ipcp_retransmit_interval", 0,	int32,	"IPCP retransmission timeout. Default=3."),					\
	ARG(LCP_ECHO_FAIL_COUNT,"lcp_echo_failure_count",0,	int32,	("Number of LCP echo failures to accept before assuming peer is down. "		\
									 "Default=5.")),								\
	ARG(LCP_ECHO_INTERVAL,	"lcp_echo_interval",	0,	int32,	"Send LCP echo-request to peer every N seconds. Default=0 (don't send)."),	\
	ARG(LCP_MAX_CFG_REQS,	"lcp_max_config_requests", 0,	int32,	"Maximum number of LCP config-request transmissions. Default=10."),		\
	ARG(LCP_MAX_CFG_NAKS,	"lcp_max_config_naks",	0,	int32,	("Maximum number of LCP config-requests to transmit without successful "  	\
	    								 "acknowledgement before declaring a failure. Default=10.")),			\
	ARG(LCP_MAX_TERM_REQS,	"lcp_max_terminate_requests", 0, int32,	"Maximum number of LCP term-requests to send. Default=3."),			\
	ARG(LCP_RETX_INTVL,	"lcp_retransmit_interval", 0,	int32,	"LCP retransmission timeout. Default=3."),					\
	ARG(MAX_CONNECT_TIME,	"max_connect_time",	0,	int32,	("Maximum connect time (in seconds) that the PPP session may stay in use."	\
									 "Default=0 (no limit)")),							\
	ARG(MAX_FAILURE_COUNT,	"max_failure_count",	0,	int32,	"Terminate after N consecutive attempts. 0 is no limit. Default=10."),		\
	ARG(IP_POOL_NAME,	"ip_pool_name",		0,	string,	"IP pool name. If system supports IP address pools, this name will be "		\
									"passed to PPP for address assignment."),					\
	ARG(LOCAL_IPADDR,	"local_ipaddr",		0,	ipaddr,	"IP address of local PPP interface"),						\
	ARG(REMOTE_IPADDR,	"remote_ipaddr",	0,	ipaddr,	"IP address of remote PPP interface"),						\
	ARG(DNS_IPADDR_PRI,	"dns_ipaddr_pri",	0,	ipaddr,	"Primary DNS address"),								\
	ARG(DNS_IPADDR_SEC,	"dns_ipaddr_sec",	0,	ipaddr,	"Secondary DNS address"),							\
	ARG(WINS_IPADDR_PRI,	"wins_ipaddr_pri",	0,	ipaddr,	"Primary WINS address"),							\
	ARG(WINS_IPADDR_SEC,	"wins_ipaddr_sec",	0,	ipaddr,	"Secondary WINS address"),							\
	ARG(USE_RADIUS,		"use_radius",		0,	bool,	"Use RADIUS for PPP authentication and connection attributes"),			\
	ARG(RADIUS_HINT,	"radius_hint",		0,	string,	"String to pass to RADIUS client for use when doing RADIUS lookup"),		\
	ARG(LOCAL_NAME,		"local_name",		0,	string,	"Name to use for authentication with the peer, unless overridden by user_name"),\
	ARG(REMOTE_NAME,	"remote_name",		0,	string,	"Name to assume for the remote peer for authentication purposes"),		\
	ARG(USE_AS_DEFAULT_ROUTE, "default_route",	0,	bool,	"Use link as default route"),							\
	ARG(MULTILINK,		"multilink",		0, 	bool, 	"Enable PPP multilink connections."),						\
	ARG(PROXY_ARP,		"proxy_arp",		0, 	bool, 	"Use proxy arp.")


static struct cli_arg_entry l2tp_args_ppp_profile_create[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of ppp profile")),
	L2TP_PPP_MODIFY_ARGS,
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_ppp_profile_delete[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of ppp profile")),
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_ppp_profile_modify[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of ppp profile")),
	L2TP_PPP_MODIFY_ARGS,
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_ppp_profile_show[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of ppp profile")),
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_ppp_profile_unset[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of ppp profile")),
	FLG(TRACE_FLAGS, 	"trace_flags", 		"Trace flags, for debugging network problems"),					\
	FLG(ASYNCMAP, 		"asyncmap", 		"Async character map. Valid only if PPP is async mode."),			\
	FLG(MTU, 		"mtu",	 		"Maximum Transmit Unit (MTU) or maximum packet size transmitted."),		\
	FLG(MRU, 		"mru",	 		"Maximum Receive Unit (MRU) or maximum packet size passed when received."),	\
	FLG(SYNC_MODE, 		"sync_mode", 		"Allow PPP sync/async operation."),						\
	FLG(AUTH_PAP, 		"auth_pap", 		"Allow PPP PAP authentication. Default: YES"),					\
	FLG(AUTH_CHAP, 		"auth_chap", 		"Allow PPP CHAP authentication. Default: YES"),					\
	FLG(AUTH_MSCHAP,	"auth_mschapv1",	"Allow PPP MSCHAP authentication. Default: YES"),				\
	FLG(AUTH_MSCHAPV2, 	"auth_mschapv2", 	"Allow PPP MSCHAPV2 authentication. Default: YES"),				\
	FLG(AUTH_EAP, 		"auth_eap", 		"Allow PPP EAP authentication. Default: YES"),					\
	FLG(AUTH_NONE, 		"auth_none", 		"Allow unauthenticated peers. Default: NO/YES (server/client)"),		\
	FLG(AUTH_PEER, 		"auth_peer", 		"Authenticate peer PPP user (server). Default: YES/NO (server/client)"),	\
	FLG(CHAP_INTERVAL,	"chap_interval",	("Rechallenge the peer every chap_interval seconds. "				\
							 "Default=0 (don't rechallenge).")),						\
	FLG(CHAP_MAX_CHALLENGE,	"chap_max_challenge",	("Maximum number of CHAP challenges to transmit without successful "		\
							 "acknowledgment before declaring a failure. Default=10.")),			\
	FLG(CHAP_RESTART,	"chap_restart",		("Retransmission timeout for CHAP challenges. Default=3.")),			\
	FLG(PAP_MAX_AUTH_REQS,	"pap_max_auth_requests",("Maximum number of PAP authenticate-request transmissions. Default=10.")),	\
	FLG(PAP_RESTART_INTVL,	"pap_restart_interval",	"Retransmission timeout for PAP requests. Default=3."),				\
	FLG(PAP_TIMEOUT,	"pap_timeout",		"Maximum time to wait for peer to authenticate itself. Default=0 (no limit)."),	\
	FLG(IDLE_TIMEOUT,	"idle_timeout",		"Disconnect session if idle for more than N seconds. Default=0 (no limit)."),	\
	FLG(IPCP_MAX_CFG_REQS,	"ipcp_max_config_requests", ("Maximum number of IPCP config-requests to transmit without successful "	\
							     "acknowledgement before declaring a failure. Default=10.")), \
	FLG(IPCP_MAX_CFG_NAKS,	"ipcp_max_config_naks",	("Maximum number of IPCP config-naks to allow before starting to send "		\
							 "config-rejects instead. Default=10.")), \
	FLG(IPCP_MAX_TERM_REQS,	"ipcp_max_terminate_requests", "Maximum number of IPCP term-requests to send. Default=3."),			\
	FLG(IPCP_RETX_INTVL,	"ipcp_retransmit_interval", "IPCP retransmission timeout. Default=3."),					\
	FLG(LCP_ECHO_FAIL_COUNT,"lcp_echo_failure_count", ("Number of LCP echo failures to accept before assuming peer is down. "		\
							   "Default=5.")), \
	FLG(LCP_ECHO_INTERVAL,	"lcp_echo_interval",	"Send LCP echo-request to peer every N seconds. Default=0 (don't send)."),	\
	FLG(LCP_MAX_CFG_REQS,	"lcp_max_config_requests", "Maximum number of LCP config-request transmissions. Default=10."),		\
	FLG(LCP_MAX_CFG_NAKS,	"lcp_max_config_naks",	("Maximum number of LCP config-requests to transmit without successful "  	\
							 "acknowledgement before declaring a failure. Default=10.")), \
	FLG(LCP_MAX_TERM_REQS,	"lcp_max_terminate_requests", "Maximum number of LCP term-requests to send. Default=3."),			\
	FLG(LCP_RETX_INTVL,	"lcp_retransmit_interval", "LCP retransmission timeout. Default=3."),					\
	FLG(MAX_CONNECT_TIME,	"max_connect_time",	("Maximum connect time (in seconds) that the PPP session may stay in use."	\
							 "Default=0 (no limit)")),							\
	FLG(MAX_FAILURE_COUNT,	"max_failure_count",	"Terminate after N consecutive attempts. 0 is no limit. Default=10."),		\
	FLG(IP_POOL_NAME,	"ip_pool_name",		"IP pool name. If system supports IP address pools, this name will be "		\
							"passed to PPP for address assignment."),					\
	FLG(LOCAL_IPADDR,	"local_ipaddr",		"IP address of local PPP interface"),						\
	FLG(REMOTE_IPADDR,	"remote_ipaddr",	"IP address of remote PPP interface"),						\
	FLG(DNS_IPADDR_PRI,	"dns_ipaddr_pri",	"Primary DNS address"),								\
	FLG(DNS_IPADDR_SEC,	"dns_ipaddr_sec",	"Secondary DNS address"),							\
	FLG(WINS_IPADDR_PRI,	"wins_ipaddr_pri",	"Primary WINS address"),							\
	FLG(WINS_IPADDR_SEC,	"wins_ipaddr_sec",	"Secondary WINS address"),							\
	FLG(USE_RADIUS,		"use_radius",		"Use RADIUS for PPP authentication and connection attributes"),			\
	FLG(RADIUS_HINT,	"radius_hint",		"String to pass to RADIUS client for use when doing RADIUS lookup"),		\
	FLG(LOCAL_NAME,		"local_name",		"Name to use for authentication with the peer, unless overridden by user_name"),\
	FLG(REMOTE_NAME,	"remote_name",		"Name to assume for the remote peer for authentication purposes"),		\
	FLG(USE_AS_DEFAULT_ROUTE, "default_route",	"Use link as default route"),							\
	FLG(MULTILINK,		"multilink",		"Enable PPP multilink connections."),						\
	FLG(PROXY_ARP,		"proxy_arp",		"Use proxy arp."),
	{ NULL, },
};

static int l2tp_parse_ppp_profile_arg(l2tp_ppp_arg_ids_t arg_id, struct cli_node *arg, char *arg_value, struct l2tp_api_ppp_profile_msg_data *msg)
{
	int result = -EINVAL;
	int ints[2];

	if (arg_value == NULL) {
		arg_value = empty_string;
	}

	switch (arg_id) {
	case L2TP_PPP_ARGID_PROFILE_NAME:
		msg->profile_name = strdup(arg_value);
		if (msg->profile_name == NULL) {
			result = -ENOMEM;
			goto out;
		}
		break;
	case L2TP_PPP_ARGID_TRACE_FLAGS:
		result = l2tp_parse_debug_mask(&msg->trace_flags, arg_value, 1);
		if (result < 0) {
			goto out;
		}
		msg->flags |= L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS;
		break;
	case L2TP_PPP_ARGID_ASYNCMAP:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->asyncmap, msg->flags, L2TP_API_PPP_PROFILE_FLAG_ASYNCMAP);
		break;
	case L2TP_PPP_ARGID_MTU:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->mtu, msg->flags, L2TP_API_PPP_PROFILE_FLAG_MTU);
		break;
	case L2TP_PPP_ARGID_MRU:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->mru, msg->flags, L2TP_API_PPP_PROFILE_FLAG_MRU);
		break;
	case L2TP_PPP_ARGID_SYNC_MODE:
		if (strcasecmp(arg_value, "sync") == 0) {
			msg->sync_mode = L2TP_API_PPP_SYNCMODE_SYNC;
		} else if (strcasecmp(arg_value, "async") == 0) {
			msg->sync_mode = L2TP_API_PPP_SYNCMODE_ASYNC;
		} else if (strcasecmp(arg_value, "any") == 0) {
			msg->sync_mode = L2TP_API_PPP_SYNCMODE_SYNC_ASYNC;
		} else {
			fprintf(stderr, "Bad sync mode %s: expecting sync|async|any\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_PPP_PROFILE_FLAG_SYNC_MODE;
		break;
	case L2TP_PPP_ARGID_AUTH_PAP:
		L2TP_ACT_PARSE_ARG(arg, arg_value, ints[0], msg->flags2, L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_PAP);
		if (ints[0]) {
			msg->auth_refuse_pap = 0;
		} else {
			msg->auth_refuse_pap = -1;
		}
		break;
	case L2TP_PPP_ARGID_AUTH_CHAP:
		L2TP_ACT_PARSE_ARG(arg, arg_value, ints[0], msg->flags2, L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_CHAP);
		if (ints[0]) {
			msg->auth_refuse_chap = 0;
		} else {
			msg->auth_refuse_chap = -1;
		}
		break;
	case L2TP_PPP_ARGID_AUTH_MSCHAP:
		L2TP_ACT_PARSE_ARG(arg, arg_value, ints[0], msg->flags2, L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAP);
		if (ints[0]) {
			msg->auth_refuse_mschap = 0;
		} else {
			msg->auth_refuse_mschap = -1;
		}
		break;
	case L2TP_PPP_ARGID_AUTH_MSCHAPV2:
		L2TP_ACT_PARSE_ARG(arg, arg_value, ints[0], msg->flags2, L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAPV2);
		if (ints[0]) {
			msg->auth_refuse_mschapv2 = 0;
		} else {
			msg->auth_refuse_mschapv2 = -1;
		}
		break;
	case L2TP_PPP_ARGID_AUTH_EAP:
		L2TP_ACT_PARSE_ARG(arg, arg_value, ints[0], msg->flags2, L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_EAP);
		if (ints[0]) {
			msg->auth_refuse_eap = 0;
		} else {
			msg->auth_refuse_eap = -1;
		}
		break;
	case L2TP_PPP_ARGID_AUTH_NONE:
		L2TP_ACT_PARSE_ARG(arg, arg_value, ints[0], msg->flags2, L2TP_API_PPP_PROFILE_FLAG_AUTH_NONE);
		msg->auth_none = ints[0];
		break;
	case L2TP_PPP_ARGID_AUTH_PEER:
		L2TP_ACT_PARSE_ARG(arg, arg_value, ints[0], msg->flags2, L2TP_API_PPP_PROFILE_FLAG_AUTH_PEER);
		msg->auth_peer = ints[0];
		break;
	case L2TP_PPP_ARGID_CHAP_INTERVAL:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->chap_interval, msg->flags, L2TP_API_PPP_PROFILE_FLAG_CHAP_INTERVAL);
		break;
	case L2TP_PPP_ARGID_CHAP_MAX_CHALLENGE:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->chap_max_challenge, msg->flags, L2TP_API_PPP_PROFILE_FLAG_CHAP_MAX_CHALLENGE);
		break;
	case L2TP_PPP_ARGID_CHAP_RESTART:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->chap_restart, msg->flags, L2TP_API_PPP_PROFILE_FLAG_CHAP_RESTART);
		break;
	case L2TP_PPP_ARGID_PAP_MAX_AUTH_REQS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->pap_max_auth_requests, msg->flags, L2TP_API_PPP_PROFILE_FLAG_PAP_MAX_AUTH_REQUESTS);
		break;
	case L2TP_PPP_ARGID_PAP_RESTART_INTVL:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->pap_restart_interval, msg->flags, L2TP_API_PPP_PROFILE_FLAG_PAP_RESTART_INTERVAL);
		break;
	case L2TP_PPP_ARGID_PAP_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->pap_timeout, msg->flags, L2TP_API_PPP_PROFILE_FLAG_PAP_TIMEOUT);
		break;
	case L2TP_PPP_ARGID_IDLE_TIMEOUT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->idle_timeout, msg->flags, L2TP_API_PPP_PROFILE_FLAG_IDLE_TIMEOUT);
		break;
	case L2TP_PPP_ARGID_IPCP_MAX_CFG_REQS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->ipcp_max_config_requests, msg->flags, L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_REQUESTS);
		break;
	case L2TP_PPP_ARGID_IPCP_MAX_CFG_NAKS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->ipcp_max_config_naks, msg->flags, L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_NAKS);
		break;
	case L2TP_PPP_ARGID_IPCP_MAX_TERM_REQS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->ipcp_max_terminate_requests, msg->flags, L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_TERMINATE_REQUESTS);
		break;
	case L2TP_PPP_ARGID_IPCP_RETX_INTVL:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->ipcp_retransmit_interval, msg->flags, L2TP_API_PPP_PROFILE_FLAG_IPCP_RETRANSMIT_INTERVAL);
		break;
	case L2TP_PPP_ARGID_LCP_ECHO_FAIL_COUNT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->lcp_echo_failure_count, msg->flags, L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_FAILURE_COUNT);
		break;
	case L2TP_PPP_ARGID_LCP_ECHO_INTERVAL:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->lcp_echo_interval, msg->flags, L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_INTERVAL);
		break;
	case L2TP_PPP_ARGID_LCP_MAX_CFG_REQS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->lcp_max_config_requests, msg->flags, L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_REQUESTS);
		break;
	case L2TP_PPP_ARGID_LCP_MAX_CFG_NAKS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->lcp_max_config_naks, msg->flags, L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_NAKS);
		break;
	case L2TP_PPP_ARGID_LCP_MAX_TERM_REQS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->lcp_max_terminate_requests, msg->flags, L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_TERMINATE_REQUESTS);
		break;
	case L2TP_PPP_ARGID_LCP_RETX_INTVL:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->lcp_retransmit_interval, msg->flags, L2TP_API_PPP_PROFILE_FLAG_LCP_RETRANSMIT_INTERVAL);
		break;
	case L2TP_PPP_ARGID_MAX_CONNECT_TIME:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->max_connect_time, msg->flags, L2TP_API_PPP_PROFILE_FLAG_MAX_CONNECT_TIME);
		break;
	case L2TP_PPP_ARGID_MAX_FAILURE_COUNT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->max_failure_count, msg->flags, L2TP_API_PPP_PROFILE_FLAG_MAX_FAILURE_COUNT);
		break;
	case L2TP_PPP_ARGID_LOCAL_IPADDR:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->local_ip_addr, msg->flags2, L2TP_API_PPP_PROFILE_FLAG_LOCAL_IP_ADDR);
		break;
	case L2TP_PPP_ARGID_REMOTE_IPADDR:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->peer_ip_addr, msg->flags2, L2TP_API_PPP_PROFILE_FLAG_PEER_IP_ADDR);
		break;
	case L2TP_PPP_ARGID_DNS_IPADDR_PRI:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->dns_addr_1, msg->flags2, L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_1);
		break;
	case L2TP_PPP_ARGID_DNS_IPADDR_SEC:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->dns_addr_2, msg->flags2, L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_2);
		break;
	case L2TP_PPP_ARGID_WINS_IPADDR_PRI:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->wins_addr_1, msg->flags2, L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_1);
		break;
	case L2TP_PPP_ARGID_WINS_IPADDR_SEC:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->wins_addr_2, msg->flags2, L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_2);
		break;
	case L2TP_PPP_ARGID_IP_POOL_NAME:
		OPTSTRING(msg->ip_pool_name) = strdup(arg_value);
		if (OPTSTRING(msg->ip_pool_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->ip_pool_name.valid = 1;
		msg->flags2 |= L2TP_API_PPP_PROFILE_FLAG_IP_POOL_NAME;
		break;
	case L2TP_PPP_ARGID_USE_RADIUS:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_radius, msg->flags, L2TP_API_PPP_PROFILE_FLAG_USE_RADIUS);
		break;
	case L2TP_PPP_ARGID_RADIUS_HINT:
		OPTSTRING(msg->radius_hint) = strdup(arg_value);
		if (OPTSTRING(msg->radius_hint) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->radius_hint.valid = 1;
		msg->flags |= L2TP_API_PPP_PROFILE_FLAG_RADIUS_HINT;
		break;
	case L2TP_PPP_ARGID_USE_AS_DEFAULT_ROUTE:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->use_as_default_route, msg->flags2, L2TP_API_PPP_PROFILE_FLAG_USE_AS_DEFAULT_ROUTE);
		break;
	case L2TP_PPP_ARGID_MULTILINK:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->multilink, msg->flags2, L2TP_API_PPP_PROFILE_FLAG_MULTILINK);
		break;
	case L2TP_PPP_ARGID_PROXY_ARP:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->proxy_arp, msg->flags2, L2TP_API_PPP_PROFILE_FLAG_PROXY_ARP);
		break;
	case L2TP_PPP_ARGID_LOCAL_NAME:
		OPTSTRING(msg->local_name) = strdup(arg_value);
		if (OPTSTRING(msg->local_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->local_name.valid = 1;
		msg->flags2 |= L2TP_API_PPP_PROFILE_FLAG_LOCAL_NAME;
		break;
	case L2TP_PPP_ARGID_REMOTE_NAME:
		OPTSTRING(msg->remote_name) = strdup(arg_value);
		if (OPTSTRING(msg->remote_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->remote_name.valid = 1;
		msg->flags2 |= L2TP_API_PPP_PROFILE_FLAG_REMOTE_NAME;
		break;
	}

	result = 0;

out:
	return result;
}


static int l2tp_act_ppp_profile_create(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_ppp_profile_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(100, l2tp_ppp_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_ppp_profile_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_ppp_profile_create_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Created ppp profile %s\n", msg.profile_name);
	}

out:
	return result;
}

static int l2tp_act_ppp_profile_delete(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *profile_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_ppp_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_PPP_ARGID_PROFILE_NAME:
			profile_name = arg_values[arg];
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_ppp_profile_delete_1(profile_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Deleted ppp profile %s\n", profile_name);
	}

out:
	return result;
}

static int l2tp_act_ppp_profile_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_ppp_profile_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(100, l2tp_ppp_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_ppp_profile_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_ppp_profile_modify_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Modified ppp profile %s\n", msg.profile_name);
	}

out:
	return result;
}

static int l2tp_act_ppp_profile_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *profile_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_ppp_arg_ids_t, struct l2tp_api_ppp_profile_msg_data);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_PPP_ARGID_PROFILE_NAME:
			profile_name = arg_values[arg];
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	memset(&clnt_res, 0, sizeof(clnt_res));
	result = l2tp_ppp_profile_get_1(profile_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	if (clnt_res.result_code < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result_code));
		result = clnt_res.result_code;
		goto out;
	}

	l2tp_show_ppp_profile(stdout, &clnt_res);
out:
	return result;
}

static int l2tp_act_ppp_profile_list(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_ppp_profile_list_msg_data clnt_res;
	struct l2tp_api_ppp_profile_list_entry *walk;
	int result;
	const char **profile_names;
	int index;

	memset(&clnt_res, 0, sizeof(clnt_res));

	result = l2tp_ppp_profile_list_1(&clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result));
		result = clnt_res.result;
		goto out;
	}

	if (clnt_res.num_profiles > 0) {
		profile_names = calloc(clnt_res.num_profiles, sizeof(profile_names[0]));
		if (profile_names == NULL) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(ENOMEM));
			goto out;
		}
	
		walk = clnt_res.profiles;
		for (index = 0; index < clnt_res.num_profiles; index++) {
			if ((walk == NULL) || (walk->profile_name[0] == '\0')) {
				break;
			}
			profile_names[index] = walk->profile_name;
			walk = walk->next;
		}	

		/* Sort the profile names */
		qsort(&profile_names[0], index, sizeof(profile_names[0]), l2tp_name_compare);

		for (index = 0; index < clnt_res.num_profiles; index++) {
			printf("\t%s\n", profile_names[index]);
		}

		free(profile_names);
	}

	result = 0;

out:
	return result;
}

static int l2tp_act_ppp_profile_unset(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_ppp_profile_unset_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(100, l2tp_ppp_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_PPP_ARGID_PROFILE_NAME:
			msg.profile_name = strdup(arg_values[arg]);
			if (msg.profile_name == NULL) {
				result = -ENOMEM;
				goto out;
			}
			break;
		case L2TP_PPP_ARGID_TRACE_FLAGS:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS;
			break;
		case L2TP_PPP_ARGID_ASYNCMAP:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_ASYNCMAP;
			break;
		case L2TP_PPP_ARGID_MTU:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_MTU;
			break;
		case L2TP_PPP_ARGID_MRU:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_MRU;
			break;
		case L2TP_PPP_ARGID_SYNC_MODE:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_SYNC_MODE;
			break;
		case L2TP_PPP_ARGID_AUTH_PAP:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_PAP;
			break;
		case L2TP_PPP_ARGID_AUTH_CHAP:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_CHAP;
			break;
		case L2TP_PPP_ARGID_AUTH_MSCHAP:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAP;
			break;
		case L2TP_PPP_ARGID_AUTH_MSCHAPV2:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAPV2;
			break;
		case L2TP_PPP_ARGID_AUTH_EAP:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_EAP;
			break;
		case L2TP_PPP_ARGID_AUTH_NONE:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_NONE;
			break;
		case L2TP_PPP_ARGID_AUTH_PEER:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_PEER;
			break;
		case L2TP_PPP_ARGID_CHAP_INTERVAL:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_CHAP_INTERVAL;
			break;
		case L2TP_PPP_ARGID_CHAP_MAX_CHALLENGE:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_CHAP_MAX_CHALLENGE;
			break;
		case L2TP_PPP_ARGID_CHAP_RESTART:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_CHAP_RESTART;
			break;
		case L2TP_PPP_ARGID_PAP_MAX_AUTH_REQS:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_PAP_MAX_AUTH_REQUESTS;
			break;
		case L2TP_PPP_ARGID_PAP_RESTART_INTVL:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_PAP_RESTART_INTERVAL;
			break;
		case L2TP_PPP_ARGID_PAP_TIMEOUT:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_PAP_TIMEOUT;
			break;
		case L2TP_PPP_ARGID_IDLE_TIMEOUT:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_IDLE_TIMEOUT;
			break;
		case L2TP_PPP_ARGID_IPCP_MAX_CFG_REQS:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_REQUESTS;
			break;
		case L2TP_PPP_ARGID_IPCP_MAX_CFG_NAKS:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_NAKS;
			break;
		case L2TP_PPP_ARGID_IPCP_MAX_TERM_REQS:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_TERMINATE_REQUESTS;
			break;
		case L2TP_PPP_ARGID_IPCP_RETX_INTVL:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_RETRANSMIT_INTERVAL;
			break;
		case L2TP_PPP_ARGID_LCP_ECHO_FAIL_COUNT:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_FAILURE_COUNT;
			break;
		case L2TP_PPP_ARGID_LCP_ECHO_INTERVAL:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_INTERVAL;
			break;
		case L2TP_PPP_ARGID_LCP_MAX_CFG_REQS:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_REQUESTS;
			break;
		case L2TP_PPP_ARGID_LCP_MAX_CFG_NAKS:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_NAKS;
			break;
		case L2TP_PPP_ARGID_LCP_MAX_TERM_REQS:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_TERMINATE_REQUESTS;
			break;
		case L2TP_PPP_ARGID_LCP_RETX_INTVL:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_RETRANSMIT_INTERVAL;
			break;
		case L2TP_PPP_ARGID_MAX_CONNECT_TIME:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_MAX_CONNECT_TIME;
			break;
		case L2TP_PPP_ARGID_MAX_FAILURE_COUNT:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_MAX_FAILURE_COUNT;
			break;
		case L2TP_PPP_ARGID_LOCAL_IPADDR:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_LOCAL_IP_ADDR;
			break;
		case L2TP_PPP_ARGID_REMOTE_IPADDR:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_PEER_IP_ADDR;
			break;
		case L2TP_PPP_ARGID_DNS_IPADDR_PRI:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_1;
			break;
		case L2TP_PPP_ARGID_DNS_IPADDR_SEC:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_2;
			break;
		case L2TP_PPP_ARGID_WINS_IPADDR_PRI:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_1;
			break;
		case L2TP_PPP_ARGID_WINS_IPADDR_SEC:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_2;
			break;
		case L2TP_PPP_ARGID_IP_POOL_NAME:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_IP_POOL_NAME;
			break;
		case L2TP_PPP_ARGID_USE_RADIUS:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_USE_RADIUS;
			break;
		case L2TP_PPP_ARGID_RADIUS_HINT:
			msg.flags |= L2TP_API_PPP_PROFILE_FLAG_RADIUS_HINT;
			break;
		case L2TP_PPP_ARGID_USE_AS_DEFAULT_ROUTE:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_USE_AS_DEFAULT_ROUTE;
			break;
		case L2TP_PPP_ARGID_MULTILINK:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_MULTILINK;
			break;
		case L2TP_PPP_ARGID_PROXY_ARP:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_PROXY_ARP;
			break;
		case L2TP_PPP_ARGID_LOCAL_NAME:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_LOCAL_NAME;
			break;
		case L2TP_PPP_ARGID_REMOTE_NAME:
			msg.flags2 |= L2TP_API_PPP_PROFILE_FLAG_REMOTE_NAME;
			break;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_ppp_profile_unset_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Unset parameters of ppp profile %s\n", msg.profile_name);
	}

out:
	return result;
}


/*****************************************************************************
 * Peer profiles
 *****************************************************************************/

typedef enum {
	L2TP_PEER_PROFILE_ARGID_PROFILE_NAME,
	L2TP_PEER_PROFILE_ARGID_PEER_IPADDR,
	L2TP_PEER_PROFILE_ARGID_PEER_PORT,
	L2TP_PEER_PROFILE_ARGID_LACLNS,
	L2TP_PEER_PROFILE_ARGID_TUNNEL_PROFILE,
	L2TP_PEER_PROFILE_ARGID_SESSION_PROFILE,
	L2TP_PEER_PROFILE_ARGID_PPP_PROFILE,
	L2TP_PEER_PROFILE_ARGID_NETMASK,
} l2tp_peer_profile_arg_ids_t;
 
#undef ARG
#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_PEER_PROFILE_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#undef FLG
#define FLG(id, name, doc) \
	{ name, { L2TP_PEER_PROFILE_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

/* Paremeters for Create and Modify operations */
#define L2TP_PEER_MODIFY_ARGS 																\
	ARG(PEER_IPADDR,	"peer_ipaddr",		0,	ipaddr,	("IP address of peer")),							\
	ARG(PEER_PORT,		"peer_port",		0,	uint16,	("UDP port with which to connect to peer. Default=1701.")),			\
	ARG(NETMASK,		"netmask",		0,	ipaddr,	("IP netmask to be used when matching for peer_ipaddr. Default=255.255.255.255.")), \
	ARG(LACLNS,		"lac_lns",		0,	string,	("We can operate as a LAC or LNS or both.")),					\
	ARG(TUNNEL_PROFILE,	"tunnel_profile_name",	0,	string,	("Name of default Tunnel Profile. Default=\"default\"")),			\
	ARG(SESSION_PROFILE,	"session_profile_name",	0,	string,	("Name of default Session Profile. Default=\"default\"")),			\
	ARG(PPP_PROFILE,	"ppp_profile_name",	0,	string,	("Name of default Ppp Profile. Default=\"default\""))				\

static struct cli_arg_entry l2tp_args_peer_profile_create[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of peer profile")),							\
	L2TP_PEER_MODIFY_ARGS,															\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_peer_profile_delete[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of peer profile")),							\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_peer_profile_modify[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of peer profile")),							\
	L2TP_PEER_MODIFY_ARGS,															\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_peer_profile_show[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	("Name of peer profile")),							\
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_peer_profile_unset[] = {
	ARG(PROFILE_NAME,	"profile_name",		0,	string,	"Name of peer profile"),							\
	FLG(PEER_IPADDR,	"peer_ipaddr",		"IP address of peer"),										\
	FLG(PEER_PORT,		"peer_port",		"UDP port with which to connect to peer. Default=1701."),					\
	FLG(NETMASK,		"netmask",		"IP netmask to be used when matching for peer_ipaddr. Default=255.255.255.255."), 		\
	FLG(LACLNS,		"lac_lns",		"We can operate as a LAC or LNS or both. Default=both"),					\
	FLG(TUNNEL_PROFILE,	"tunnel_profile_name",	"Name of default Tunnel Profile. Default=\"default\""),						\
	FLG(SESSION_PROFILE,	"session_profile_name",	"Name of default Session Profile. Default=\"default\""),					\
	FLG(PPP_PROFILE,	"ppp_profile_name",	"Name of default Ppp Profile. Default=\"default\""),						\
	{ NULL, },
};


static int l2tp_parse_peer_profile_arg(l2tp_peer_profile_arg_ids_t arg_id, struct cli_node *arg, char *arg_value, struct l2tp_api_peer_profile_msg_data *msg)
{
	int result = -EINVAL;

	if (arg_value == NULL) {
		arg_value = empty_string;
	}

	switch (arg_id) {
	case L2TP_PEER_PROFILE_ARGID_PROFILE_NAME:
		msg->profile_name = strdup(arg_value);
		if (msg->profile_name == NULL) {
			result = -ENOMEM;
			goto out;
		}
		break;
	case L2TP_PEER_PROFILE_ARGID_TUNNEL_PROFILE:
		OPTSTRING(msg->default_tunnel_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->default_tunnel_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->default_tunnel_profile_name.valid = 1;
		msg->flags |= L2TP_API_PEER_PROFILE_FLAG_TUNNEL_PROFILE_NAME;
		break;
	case L2TP_PEER_PROFILE_ARGID_SESSION_PROFILE:
		OPTSTRING(msg->default_session_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->default_session_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->default_session_profile_name.valid = 1;
		msg->flags |= L2TP_API_PEER_PROFILE_FLAG_SESSION_PROFILE_NAME;
		break;
	case L2TP_PEER_PROFILE_ARGID_PPP_PROFILE:
		OPTSTRING(msg->default_ppp_profile_name) = strdup(arg_value);
		if (OPTSTRING(msg->default_ppp_profile_name) == NULL) {
			result = -ENOMEM;
			goto out;
		}
		msg->default_ppp_profile_name.valid = 1;
		msg->flags |= L2TP_API_PEER_PROFILE_FLAG_PPP_PROFILE_NAME;
		break;
	case L2TP_PEER_PROFILE_ARGID_PEER_IPADDR:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->peer_addr, msg->flags, L2TP_API_PEER_PROFILE_FLAG_PEER_IPADDR);
		break;
	case L2TP_PEER_PROFILE_ARGID_PEER_PORT:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->peer_port, msg->flags, L2TP_API_PEER_PROFILE_FLAG_PEER_PORT);
		break;
	case L2TP_PEER_PROFILE_ARGID_LACLNS:
		if (strcasecmp(arg_value, "laclns") == 0) {
			msg->we_can_be_lac = 1;
			msg->we_can_be_lns = 1;
		} else if (strcasecmp(arg_value, "lac") == 0) {
			msg->we_can_be_lac = 1;
			msg->we_can_be_lns = 0;
		} else if (strcasecmp(arg_value, "lns") == 0) {
			msg->we_can_be_lac = 0;
			msg->we_can_be_lns = 1;
		} else {
			fprintf(stderr, "Bad authmode %s: expecting laclns|lac|lns\n", arg_value);
			result = -EINVAL;
			goto out;
		} 
		msg->flags |= L2TP_API_PEER_PROFILE_FLAG_LACLNS;
		break;
	case L2TP_PEER_PROFILE_ARGID_NETMASK:
		L2TP_ACT_PARSE_ARG(arg, arg_value, msg->netmask, msg->flags, L2TP_API_PEER_PROFILE_FLAG_NETMASK);
		break;
	}

	result = 0;

out:
	return result;
}

static int l2tp_act_peer_profile_create(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_peer_profile_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(40, l2tp_peer_profile_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_peer_profile_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_peer_profile_create_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Created peer profile %s\n", msg.profile_name);
	}

out:
	return result;
}

static int l2tp_act_peer_profile_delete(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *profile_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_peer_profile_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_PEER_PROFILE_ARGID_PROFILE_NAME:
			profile_name = arg_values[arg];
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_peer_profile_delete_1(profile_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Deleted peer profile %s\n", profile_name);
	}

out:
	return result;
}

static int l2tp_act_peer_profile_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_peer_profile_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(40, l2tp_peer_profile_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		result = l2tp_parse_peer_profile_arg(arg_id, args[arg], arg_values[arg], &msg);
		if (result < 0) {
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_peer_profile_modify_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Modified peer profile %s\n", msg.profile_name);
	}

out:
	return result;
}

static int l2tp_act_peer_profile_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *profile_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_peer_profile_arg_ids_t, struct l2tp_api_peer_profile_msg_data);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_PEER_PROFILE_ARGID_PROFILE_NAME:
			profile_name = arg_values[arg];
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	memset(&clnt_res, 0, sizeof(clnt_res));
	result = l2tp_peer_profile_get_1(profile_name, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	if (clnt_res.result_code < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result_code));
		result = clnt_res.result_code;
		goto out;
	}

	l2tp_show_peer_profile(stdout, &clnt_res);
out:
	return result;
}

static int l2tp_act_peer_profile_list(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_peer_profile_list_msg_data clnt_res;
	struct l2tp_api_peer_profile_list_entry *walk;
	int result;
	const char **profile_names;
	int index;

	memset(&clnt_res, 0, sizeof(clnt_res));

	result = l2tp_peer_profile_list_1(&clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result));
		result = clnt_res.result;
		goto out;
	}

	if (clnt_res.num_profiles > 0) {
		profile_names = calloc(clnt_res.num_profiles, sizeof(profile_names[0]));
		if (profile_names == NULL) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(ENOMEM));
			goto out;
		}
	
		walk = clnt_res.profiles;
		for (index = 0; index < clnt_res.num_profiles; index++) {
			if ((walk == NULL) || (walk->profile_name[0] == '\0')) {
				break;
			}
			profile_names[index] = walk->profile_name;
			walk = walk->next;
		}	

		/* Sort the profile names */
		qsort(&profile_names[0], index, sizeof(profile_names[0]), l2tp_name_compare);

		for (index = 0; index < clnt_res.num_profiles; index++) {
			printf("\t%s\n", profile_names[index]);
		}

		free(profile_names);
	}

	result = 0;

out:
	return result;
}

static int l2tp_act_peer_profile_unset(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_peer_profile_unset_msg_data msg = { 0, };
	L2TP_ACT_DECLARATIONS(40, l2tp_peer_profile_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_PEER_PROFILE_ARGID_PROFILE_NAME:
			msg.profile_name = strdup(arg_values[arg]);
			if (msg.profile_name == NULL) {
				result = -ENOMEM;
				goto out;
			}
			break;
		case L2TP_PEER_PROFILE_ARGID_TUNNEL_PROFILE:
			msg.flags |= L2TP_API_PEER_PROFILE_FLAG_TUNNEL_PROFILE_NAME;
			break;
		case L2TP_PEER_PROFILE_ARGID_SESSION_PROFILE:
			msg.flags |= L2TP_API_PEER_PROFILE_FLAG_SESSION_PROFILE_NAME;
			break;
		case L2TP_PEER_PROFILE_ARGID_PPP_PROFILE:
			msg.flags |= L2TP_API_PEER_PROFILE_FLAG_PPP_PROFILE_NAME;
			break;
		case L2TP_PEER_PROFILE_ARGID_PEER_IPADDR:
			msg.flags |= L2TP_API_PEER_PROFILE_FLAG_PEER_IPADDR;
			break;
		case L2TP_PEER_PROFILE_ARGID_PEER_PORT:
			msg.flags |= L2TP_API_PEER_PROFILE_FLAG_PEER_PORT;
			break;
		case L2TP_PEER_PROFILE_ARGID_LACLNS:
			msg.flags |= L2TP_API_PEER_PROFILE_FLAG_LACLNS;
			break;
		case L2TP_PEER_PROFILE_ARGID_NETMASK:
			msg.flags |= L2TP_API_PEER_PROFILE_FLAG_NETMASK;
			break;
		}
	} L2TP_ACT_END();

	if (msg.profile_name == NULL) {
		fprintf(stderr, "Required profile_name argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_peer_profile_unset_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Unset parameters of peer profile %s\n", msg.profile_name);
	}

out:
	return result;
}


/*****************************************************************************
 * Peers
 *****************************************************************************/

typedef enum {
	L2TP_PEER_ARGID_PEER_IPADDR,
	L2TP_PEER_ARGID_LOCAL_IPADDR,
} l2tp_peer_arg_ids_t;
 
#undef ARG
#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_PEER_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#undef FLG
#define FLG(id, name, doc) \
	{ name, { L2TP_PEER_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

static struct cli_arg_entry l2tp_args_peer_show[] = {
	ARG(PEER_IPADDR,	"peer_ipaddr",		0,	ipaddr,	("IP address of peer")),							\
	ARG(LOCAL_IPADDR,	"local_ipaddr",		0,	ipaddr,	("IP address of local interface")),						\
	{ NULL, },
};

static int l2tp_act_peer_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_ip_addr peer_addr = { INADDR_ANY, };
	struct l2tp_api_ip_addr local_addr = { INADDR_ANY, };

	int flags = 0;
	L2TP_ACT_DECLARATIONS(4, l2tp_peer_arg_ids_t, struct l2tp_api_peer_msg_data);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_PEER_ARGID_PEER_IPADDR:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], peer_addr, flags, 0);
			break;
		case L2TP_PEER_ARGID_LOCAL_IPADDR:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], local_addr, flags, 0);
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (peer_addr.s_addr == INADDR_ANY) {
		fprintf(stderr, "Required peer_ipaddr argument missing\n");
		result = -EINVAL;
		goto out;
	}

	memset(&clnt_res, 0, sizeof(clnt_res));
	result = l2tp_peer_get_1(local_addr, peer_addr, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	if (clnt_res.result_code < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result_code));
		result = clnt_res.result_code;
		goto out;
	}

	l2tp_show_peer(stdout, &clnt_res);
out:
	return result;
}

static int l2tp_act_peer_list(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_peer_list_msg_data clnt_res;
	struct l2tp_api_peer_list_entry *walk;
	int result;

	memset(&clnt_res, 0, sizeof(clnt_res));

	result = l2tp_peer_list_1(&clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result));
		result = clnt_res.result;
		goto out;
	}

	walk = clnt_res.peers;
	if (walk->peer_addr.s_addr != INADDR_ANY) {
		printf("%-16s %-16s\n", "Local", "Remote"); 
	}
	while ((walk != NULL) && (walk->peer_addr.s_addr != INADDR_ANY)) {
		struct in_addr ip_addr;
		ip_addr.s_addr = walk->local_addr.s_addr;
		printf("%-16s ", 
		       ip_addr.s_addr == INADDR_ANY ? "ANY" : inet_ntoa(ip_addr));
		ip_addr.s_addr = walk->peer_addr.s_addr;
		printf("%-16s\n", 
		       ip_addr.s_addr == INADDR_ANY ? "ANY" : inet_ntoa(ip_addr));
		walk = walk->next;
	}	

	result = 0;

out:
	return result;
}

/*****************************************************************************
 * users
 *****************************************************************************/

static int l2tp_act_user_list(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_user_list_msg_data clnt_res;
	struct l2tp_api_user_list_entry *walk;
	int result;
	const char **user_names;
	int index;

	memset(&clnt_res, 0, sizeof(clnt_res));

	result = l2tp_user_list_1(&clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res.result));
		result = clnt_res.result;
		goto out;
	}

	if (clnt_res.num_users > 0) {
		user_names = calloc(clnt_res.num_users, sizeof(user_names[0]));
		if (user_names == NULL) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(ENOMEM));
			goto out;
		}
	
		walk = clnt_res.users;
		for (index = 0; index < clnt_res.num_users; index++) {
			if (walk == NULL) {
				break;
			}
			user_names[index] = walk->user_name;
			walk = walk->next;
		}	

		/* Sort the user names */
		qsort(&user_names[0], index, sizeof(user_names[0]), l2tp_name_compare);

		printf("%c %6s %6s  %-36s %24s\n", ' ', "TunId", "SesId", "User", "Create Time");

		for (index = 0; index < clnt_res.num_users; index++) {
			/* find the username in the list */
			walk = clnt_res.users;
			while (walk != NULL) {
				if ((user_names[index][0] != '\0') &&
				    (walk->tunnel_id != 0) &&
				    (strcmp(walk->user_name, user_names[index]) == 0)) {
					printf("%c %6hu %6hu  %-36.36s %24.24s\n",
					       walk->created_by_admin ? ' ' : '*',
					       walk->tunnel_id, walk->session_id,
					       walk->user_name, walk->create_time);

					/* prevent it being used again */
					walk->tunnel_id = 0;
					break;
				}
				
				walk = walk->next;
			}
		}

		free(user_names);
	}

	result = 0;

out:
	return result;
}

/*****************************************************************************
 * config save/restore
 *****************************************************************************/

#define Y_OR_N(_var) (_var) ? "yes" : "no"

#undef ARG
#undef FLG

#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_CONFIG_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#define FLG(id, name, doc) \
	{ name, { L2TP_CONFIG_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

typedef enum {
	L2TP_CONFIG_ARGID_FILENAME,
} l2tp_config_arg_ids_t;

static struct cli_arg_entry l2tp_args_config[] = {
	ARG(FILENAME, 		"file", 		0, 	string,	"Filename for save/restore operation."),
	{ NULL, },
};

static void l2tp_config_dump_system(FILE *file, struct l2tp_api_system_msg_data *cfg)
{
	if (cfg->config.flags & (L2TP_API_CONFIG_FLAG_TRACE_FLAGS |
				 L2TP_API_CONFIG_FLAG_MAX_TUNNELS |
				 L2TP_API_CONFIG_FLAG_DRAIN_TUNNELS |
				 L2TP_API_CONFIG_FLAG_MAX_SESSIONS |
				 L2TP_API_CONFIG_FLAG_TUNNEL_ESTABLISH_TIMEOUT |
				 L2TP_API_CONFIG_FLAG_SESSION_ESTABLISH_TIMEOUT |
				 L2TP_API_CONFIG_FLAG_DENY_LOCAL_TUNNEL_CREATES |
				 L2TP_API_CONFIG_FLAG_DENY_REMOTE_TUNNEL_CREATES |
				 L2TP_API_CONFIG_FLAG_TUNNEL_PERSIST_PEND_TIMEOUT |
				 L2TP_API_CONFIG_FLAG_SESSION_PERSIST_PEND_TIMEOUT)) {

		fprintf(file, "system modify \\\n");

		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_TRACE_FLAGS) {
			fprintf(file, "trace_flags=%u \\\n", cfg->config.trace_flags);
		}
		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_MAX_TUNNELS) {
			fprintf(file, "max_tunnels=%d \\\n", cfg->config.max_tunnels);
		}
		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_DRAIN_TUNNELS) {
			fprintf(file, "drain_tunnels=%s \\\n", Y_OR_N(cfg->config.drain_tunnels));
		}
		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_MAX_SESSIONS) {
			fprintf(file, "max_sessions=%d \\\n", cfg->config.max_sessions);
		}
		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_TUNNEL_ESTABLISH_TIMEOUT) {
			fprintf(file, "tunnel_establish_timeout=%d \\\n", cfg->config.tunnel_establish_timeout);
		}
		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_SESSION_ESTABLISH_TIMEOUT) {
			fprintf(file, "session_establish_timeout=%d \\\n", cfg->config.session_establish_timeout);
		}
		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_DENY_LOCAL_TUNNEL_CREATES) {
			fprintf(file, "deny_local_tunnel_creates=%s \\\n", Y_OR_N(cfg->config.deny_local_tunnel_creates));
		}
		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_DENY_REMOTE_TUNNEL_CREATES) {
			fprintf(file, "deny_remote_tunnel_creates=%s \\\n", Y_OR_N(cfg->config.deny_remote_tunnel_creates));
		}
		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_TUNNEL_PERSIST_PEND_TIMEOUT) {
			fprintf(file, "tunnel_persist_pend_timeout=%d \\\n", cfg->config.tunnel_persist_pend_timeout);
		}
		if (cfg->config.flags & L2TP_API_CONFIG_FLAG_SESSION_PERSIST_PEND_TIMEOUT) {
			fprintf(file, "session_persist_pend_timeout=%d \\\n", cfg->config.session_persist_pend_timeout);
		}
	}
}

static void l2tp_config_dump_peer_profile(FILE *file, struct l2tp_api_peer_profile_msg_data *cfg)
{
 	if (strcmp(cfg->profile_name, L2TP_API_PEER_PROFILE_DEFAULT_PROFILE_NAME) != 0) {
 		fprintf(file, "peer profile create profile_name=%s\n", cfg->profile_name);
 	}
 	if (cfg->flags & (L2TP_API_PEER_PROFILE_FLAG_LACLNS |
 			  L2TP_API_PEER_PROFILE_FLAG_TUNNEL_PROFILE_NAME |
 			  L2TP_API_PEER_PROFILE_FLAG_SESSION_PROFILE_NAME |
 			  L2TP_API_PEER_PROFILE_FLAG_PPP_PROFILE_NAME |
 			  L2TP_API_PEER_PROFILE_FLAG_PEER_IPADDR |
 			  L2TP_API_PEER_PROFILE_FLAG_PEER_PORT |
 			  L2TP_API_PEER_PROFILE_FLAG_NETMASK)) {
 
 		fprintf(file, "peer profile modify profile_name=%s \\\n", cfg->profile_name);
		if (cfg->flags & L2TP_API_PEER_PROFILE_FLAG_LACLNS) {
			fprintf(file, "\tlac_lns=%s \\\n",
				(cfg->we_can_be_lac && cfg->we_can_be_lns) ? "laclns" :
				(!cfg->we_can_be_lac && cfg->we_can_be_lns) ? "lns" :
				(cfg->we_can_be_lac && !cfg->we_can_be_lns) ? "lac" : "??");
		}
		if ((cfg->flags & L2TP_API_PEER_PROFILE_FLAG_TUNNEL_PROFILE_NAME) && (OPTSTRING_PTR(cfg->default_tunnel_profile_name) != NULL)) {
			fprintf(file, "\ttunnel_profile_name=%s \\\n", OPTSTRING_PTR(cfg->default_tunnel_profile_name));
		}
		if ((cfg->flags & L2TP_API_PEER_PROFILE_FLAG_SESSION_PROFILE_NAME) && (OPTSTRING_PTR(cfg->default_session_profile_name) != NULL)) {
			fprintf(file, "\tsession_profile_name=%s \\\n", OPTSTRING_PTR(cfg->default_session_profile_name));
		}
		if ((cfg->flags & L2TP_API_PEER_PROFILE_FLAG_PPP_PROFILE_NAME) && (OPTSTRING_PTR(cfg->default_ppp_profile_name) != NULL)) {
			fprintf(file, "\tppp_profile_name=%s \\\n", OPTSTRING_PTR(cfg->default_ppp_profile_name));
		}
		if (cfg->flags & L2TP_API_PEER_PROFILE_FLAG_PEER_IPADDR) {
			struct in_addr ip;
			ip.s_addr = cfg->peer_addr.s_addr;
			fprintf(file, "\tpeer_ipaddr=%s \\\n", inet_ntoa(ip));
		}
		if (cfg->flags & L2TP_API_PEER_PROFILE_FLAG_PEER_PORT) {
			fprintf(file, "\tpeer_port=%hu \\\n", cfg->peer_port);
		}
		if (cfg->flags & L2TP_API_PEER_PROFILE_FLAG_NETMASK) {
			struct in_addr addr;
			addr.s_addr = cfg->netmask.s_addr;
			fprintf(file, "\tnetmask=%s \\\n", inet_ntoa(addr));
		}
		fprintf(file, "\n");
	}
}

static void l2tp_config_dump_tunnel_profile(FILE *file, struct l2tp_api_tunnel_profile_msg_data *cfg)
{
 	if (strcmp(cfg->profile_name, L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME) != 0) {
 		fprintf(file, "tunnel profile create profile_name=%s\n", cfg->profile_name);
 	}
 	if (cfg->flags & (L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_AUTH_MODE |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_FRAMING_CAP |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_BEARER_CAP |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_USE_TIEBREAKER |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_HELLO_TIMEOUT |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_MAX_RETRIES |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_RX_WINDOW_SIZE |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_TX_WINDOW_SIZE |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_RETRY_TIMEOUT |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_IDLE_TIMEOUT |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_SECRET |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_ALLOW_PPP_PROXY |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_USE_UDP_CHECKSUMS |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_HOST_NAME |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_MAX_SESSIONS |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_OUR_ADDR |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_PEER_ADDR |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_OUR_UDP_PORT |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_PEER_UDP_PORT |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_PEER_PROFILE_NAME |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_SESSION_PROFILE_NAME |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_PPP_PROFILE_NAME |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_DO_PMTU_DISCOVERY |
 			  L2TP_API_TUNNEL_PROFILE_FLAG_MTU)) {
 
 		fprintf(file, "tunnel profile modify profile_name=%s \\\n", cfg->profile_name);
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS) {
			fprintf(file, "\thide_avps=%s \\\n", Y_OR_N(cfg->hide_avps));
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_AUTH_MODE) {
			fprintf(file, "\tauth_mode=%s \\\n", 
				(cfg->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_NONE) ? "none" :
				(cfg->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_SIMPLE) ? "simple" :
				(cfg->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE) ? "challenge" : "??");
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_FRAMING_CAP) {
			fprintf(file, "\tframing_cap=%s \\\n", 
				(!cfg->framing_cap_sync && !cfg->framing_cap_async) ? "none" :
				(cfg->framing_cap_sync && cfg->framing_cap_async) ? "any" :
				(cfg->framing_cap_sync && !cfg->framing_cap_async) ? "sync" :
				(!cfg->framing_cap_sync && cfg->framing_cap_async) ? "async" : "??");
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_BEARER_CAP) {
			fprintf(file, "\tbearer_cap=%s \\\n", 
				(!cfg->bearer_cap_digital && !cfg->bearer_cap_analog) ? "none" :
				(cfg->bearer_cap_digital && cfg->bearer_cap_analog) ? "any" :
				(cfg->bearer_cap_digital && !cfg->bearer_cap_analog) ? "digital" :
				(!cfg->bearer_cap_digital && cfg->bearer_cap_analog) ? "analog" : "??");
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_USE_TIEBREAKER) {
			fprintf(file, "\tuse_tiebreaker=%s \\\n", Y_OR_N(cfg->use_tiebreaker));
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_HELLO_TIMEOUT) {
			fprintf(file, "\thello_timeout=%d \\\n", cfg->hello_timeout);
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_MAX_RETRIES) {
			fprintf(file, "\tmax_retries=%d \\\n", cfg->max_retries);
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_RX_WINDOW_SIZE) {
			fprintf(file, "\trx_window_size=%d \\\n", cfg->rx_window_size);
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_TX_WINDOW_SIZE) {
			fprintf(file, "\ttx_window_size=%d \\\n", cfg->tx_window_size);
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_RETRY_TIMEOUT) {
			fprintf(file, "\tretry_timeout=%d \\\n", cfg->retry_timeout);
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_IDLE_TIMEOUT) {
			fprintf(file, "\tidle_timeout=%d \\\n", cfg->idle_timeout);
		}
		if ((cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_SECRET) && (OPTSTRING_PTR(cfg->secret) != NULL)) {
			fprintf(file, "\tsecret=%s \\\n", OPTSTRING_PTR(cfg->secret));
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_ALLOW_PPP_PROXY) {
			fprintf(file, "\tallow_ppp_proxy=%s \\\n", Y_OR_N(cfg->allow_ppp_proxy));
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS) {
			fprintf(file, "\ttrace_flags=%u \\\n", cfg->trace_flags);
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_USE_UDP_CHECKSUMS) {
			fprintf(file, "\tuse_udp_checksums=%s \\\n", Y_OR_N(cfg->use_udp_checksums));
		}
		if ((cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_HOST_NAME) && (OPTSTRING_PTR(cfg->host_name) != NULL)) {
			fprintf(file, "\thost_name=%s \\\n", OPTSTRING_PTR(cfg->host_name));
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_MAX_SESSIONS) {
			fprintf(file, "\tmax_sessions=%d \\\n", cfg->max_sessions);
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_OUR_ADDR) {
			struct in_addr ip;
			ip.s_addr = cfg->our_addr.s_addr;
			fprintf(file, "\tsrc_ipaddr=%s \\\n", inet_ntoa(ip));
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_PEER_ADDR) {
			struct in_addr ip;
			ip.s_addr = cfg->peer_addr.s_addr;
			fprintf(file, "\tdest_ipaddr=%s \\\n", inet_ntoa(ip));
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_OUR_UDP_PORT) {
			fprintf(file, "\tour_udp_port=%hu \\\n", cfg->our_udp_port);
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_PEER_UDP_PORT) {
			fprintf(file, "\tpeer_udp_port=%hu \\\n", cfg->peer_udp_port);
		}
		if ((cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_PEER_PROFILE_NAME) && (OPTSTRING_PTR(cfg->peer_profile_name) != NULL)) {
			fprintf(file, "\tpeer_profile_name=%s \\\n", OPTSTRING_PTR(cfg->peer_profile_name));
		}
		if ((cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_SESSION_PROFILE_NAME) && (OPTSTRING_PTR(cfg->session_profile_name) != NULL)) {
			fprintf(file, "\tsession_profile_name=%s \\\n", OPTSTRING_PTR(cfg->session_profile_name));
		}
		if ((cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_PPP_PROFILE_NAME) && (OPTSTRING_PTR(cfg->ppp_profile_name) != NULL)) {
			fprintf(file, "\tppp_profile_name=%s \\\n", OPTSTRING_PTR(cfg->ppp_profile_name));
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_DO_PMTU_DISCOVERY) {
			fprintf(file, "\tdo_pmtu_discovery=%s \\\n", Y_OR_N(cfg->do_pmtu_discovery));
		}
		if (cfg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_MTU) {
			fprintf(file, "\tmtu=%d \\\n", cfg->mtu);
		}
		fprintf(file, "\n");
	}
}

static void l2tp_config_dump_session_profile(FILE *file, struct l2tp_api_session_profile_msg_data *cfg)
{
 	if (strcmp(cfg->profile_name, L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME) != 0) {
 		fprintf(file, "session profile create profile_name=%s\n", cfg->profile_name);
 	}
 	
 	if (cfg->flags & (L2TP_API_SESSION_PROFILE_FLAG_TRACE_FLAGS |
 			  L2TP_API_SESSION_PROFILE_FLAG_SEQUENCING_REQUIRED |
 			  L2TP_API_SESSION_PROFILE_FLAG_PPP_PROFILE_NAME |
 			  L2TP_API_SESSION_PROFILE_FLAG_SESSION_TYPE |
 			  L2TP_API_SESSION_PROFILE_FLAG_PRIV_GROUP_ID |
 			  L2TP_API_SESSION_PROFILE_FLAG_FRAMING_TYPE |
 			  L2TP_API_SESSION_PROFILE_FLAG_BEARER_TYPE |
 			  L2TP_API_SESSION_PROFILE_FLAG_MINIMUM_BPS |
 			  L2TP_API_SESSION_PROFILE_FLAG_MAXIMUM_BPS |
 			  L2TP_API_SESSION_PROFILE_FLAG_CONNECT_SPEED |
 			  L2TP_API_SESSION_PROFILE_FLAG_USE_PPP_PROXY |
 			  L2TP_API_SESSION_PROFILE_FLAG_USE_SEQUENCE_NUMBERS |
 			  L2TP_API_SESSION_PROFILE_FLAG_NO_PPP |
 			  L2TP_API_SESSION_PROFILE_FLAG_REORDER_TIMEOUT)) {
 
 		fprintf(file, "session profile modify profile_name=%s \\\n", cfg->profile_name);
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_TRACE_FLAGS) {
			fprintf(file, "\ttrace_flags=%u \\\n", cfg->trace_flags);
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_SEQUENCING_REQUIRED) {
			fprintf(file, "\tsequencing_required=%s \\\n", Y_OR_N(cfg->sequencing_required));
		}
		if ((cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_PPP_PROFILE_NAME) && (OPTSTRING_PTR(cfg->ppp_profile_name) != NULL)) {
			fprintf(file, "\tppp_profile_name=%s \\\n", OPTSTRING_PTR(cfg->ppp_profile_name));
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_SESSION_TYPE) {
			fprintf(file, "\tsession_type=%s \\\n", 
				(cfg->session_type == L2TP_API_SESSION_TYPE_LAIC) ? "laic" : 
				(cfg->session_type == L2TP_API_SESSION_TYPE_LAOC) ? "laoc" : 
				(cfg->session_type == L2TP_API_SESSION_TYPE_LNIC) ? "lnic" : 
				(cfg->session_type == L2TP_API_SESSION_TYPE_LNOC) ? "lnoc" : "??");
		}
		if ((cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_PRIV_GROUP_ID) && (OPTSTRING_PTR(cfg->priv_group_id) != NULL)) {
			fprintf(file, "\tpriv_group_id=%s \\\n", OPTSTRING_PTR(cfg->priv_group_id));
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_FRAMING_TYPE) {
			fprintf(file, "\tframing_type=%s \\\n", 
				(!cfg->framing_type_sync && !cfg->framing_type_async) ? "none" :
				(cfg->framing_type_sync && cfg->framing_type_async) ? "any" :
				(cfg->framing_type_sync && !cfg->framing_type_async) ? "sync" :
				(!cfg->framing_type_sync && cfg->framing_type_async) ? "async" : "??");
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_BEARER_TYPE) {
			fprintf(file, "\tbearer_type=%s \\\n", 
				(!cfg->bearer_type_digital && !cfg->bearer_type_analog) ? "none" :
				(cfg->bearer_type_digital && cfg->bearer_type_analog) ? "any" :
				(cfg->bearer_type_digital && !cfg->bearer_type_analog) ? "digital" :
				(!cfg->bearer_type_digital && cfg->bearer_type_analog) ? "analog" : "??");
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_MINIMUM_BPS) {
			fprintf(file, "\tminimum_bps=%d \\\n", cfg->minimum_bps);
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_MAXIMUM_BPS) {
			fprintf(file, "\tmaximum_bps=%d \\\n", cfg->maximum_bps);
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_CONNECT_SPEED) {
			fprintf(file, "\tconnect_speed=%d:%d \\\n", cfg->rx_connect_speed, cfg->tx_connect_speed);
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_USE_PPP_PROXY) {
			fprintf(file, "\tuse_ppp_proxy=%s \\\n", Y_OR_N(cfg->use_ppp_proxy));
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_USE_SEQUENCE_NUMBERS) {
			fprintf(file, "\tuse_sequence_numbers=%s \\\n", Y_OR_N(cfg->use_sequence_numbers));
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_NO_PPP) {
			fprintf(file, "\tno_ppp=%s \\\n", Y_OR_N(cfg->no_ppp));
		}
		if (cfg->flags & L2TP_API_SESSION_PROFILE_FLAG_REORDER_TIMEOUT) {
			fprintf(file, "\treorder_timeout=%d \\\n", cfg->reorder_timeout);
		}
		fprintf(file, "\n");
	}
}

static void l2tp_config_dump_ppp_profile(FILE *file, struct l2tp_api_ppp_profile_msg_data *cfg)
{
 	if (strcmp(cfg->profile_name, L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME) != 0) {
 		fprintf(file, "ppp profile create profile_name=%s\n", cfg->profile_name);
 	}
 
 	/* Unfortunately we have 2 flags variables to check because there are so 
 	 * many arguments... 
 	 */
 	if ((cfg->flags & (L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS |
 			   L2TP_API_PPP_PROFILE_FLAG_ASYNCMAP |
 			   L2TP_API_PPP_PROFILE_FLAG_MRU |
 			   L2TP_API_PPP_PROFILE_FLAG_MTU |
 			   L2TP_API_PPP_PROFILE_FLAG_USE_RADIUS |
 			   L2TP_API_PPP_PROFILE_FLAG_RADIUS_HINT |
 			   L2TP_API_PPP_PROFILE_FLAG_SYNC_MODE |
 			   L2TP_API_PPP_PROFILE_FLAG_CHAP_INTERVAL |
 			   L2TP_API_PPP_PROFILE_FLAG_CHAP_MAX_CHALLENGE |
 			   L2TP_API_PPP_PROFILE_FLAG_CHAP_RESTART |
 			   L2TP_API_PPP_PROFILE_FLAG_PAP_MAX_AUTH_REQUESTS |
 			   L2TP_API_PPP_PROFILE_FLAG_PAP_RESTART_INTERVAL |
 			   L2TP_API_PPP_PROFILE_FLAG_PAP_TIMEOUT |
 			   L2TP_API_PPP_PROFILE_FLAG_IDLE_TIMEOUT |
 			   L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_REQUESTS |
 			   L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_NAKS |
 			   L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_TERMINATE_REQUESTS |
 			   L2TP_API_PPP_PROFILE_FLAG_IPCP_RETRANSMIT_INTERVAL |
 			   L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_FAILURE_COUNT |
 			   L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_INTERVAL |
 			   L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_REQUESTS |
 			   L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_NAKS |
 			   L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_TERMINATE_REQUESTS |
 			   L2TP_API_PPP_PROFILE_FLAG_LCP_RETRANSMIT_INTERVAL |
 			   L2TP_API_PPP_PROFILE_FLAG_MAX_CONNECT_TIME |
 			   L2TP_API_PPP_PROFILE_FLAG_MAX_FAILURE_COUNT)) ||
 	    (cfg->flags2 & (L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_1 |
 			    L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_2 |
 			    L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_1 |
 			    L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_2 |
 			    L2TP_API_PPP_PROFILE_FLAG_LOCAL_IP_ADDR |
 			    L2TP_API_PPP_PROFILE_FLAG_PEER_IP_ADDR |
 			    L2TP_API_PPP_PROFILE_FLAG_IP_POOL_NAME |
 			    L2TP_API_PPP_PROFILE_FLAG_LOCAL_NAME |
 			    L2TP_API_PPP_PROFILE_FLAG_REMOTE_NAME |
 			    L2TP_API_PPP_PROFILE_FLAG_MULTILINK |
			    L2TP_API_PPP_PROFILE_FLAG_AUTH_NONE |
			    L2TP_API_PPP_PROFILE_FLAG_AUTH_PEER |
			    L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_PAP |
			    L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_CHAP |
			    L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAP |
			    L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAPV2 |
			    L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_EAP |
 			    L2TP_API_PPP_PROFILE_FLAG_PROXY_ARP))) {
 
 		fprintf(file, "ppp profile modify profile_name=%s \\\n", cfg->profile_name);

		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS) {
			fprintf(file, "\ttrace_flags=%u \\\n", cfg->trace_flags);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_ASYNCMAP) {
			fprintf(file, "\tasyncmap=%u \\\n", cfg->asyncmap);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_MRU) {
			fprintf(file, "\tmru=%hu \\\n", cfg->mru);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_MTU) {
			fprintf(file, "\tmtu=%hu \\\n", cfg->mtu);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_USE_RADIUS) {
			fprintf(file, "\tuse_radius=%s \\\n", Y_OR_N(cfg->use_radius));
			if ((cfg->flags & L2TP_API_PPP_PROFILE_FLAG_RADIUS_HINT) && (OPTSTRING_PTR(cfg->radius_hint) != NULL)) {
				fprintf(file, "\tradius_hint=%s \\\n", OPTSTRING_PTR(cfg->radius_hint));
			}
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_PAP) {
			fprintf(file, "\tauth_pap=%s \\\n", Y_OR_N(!cfg->auth_refuse_pap));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_CHAP) {
			fprintf(file, "\tauth_chap=%s \\\n", Y_OR_N(!cfg->auth_refuse_chap));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAP) {
			fprintf(file, "\tauth_mschapv1=%s \\\n", Y_OR_N(!cfg->auth_refuse_mschap));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAPV2) {
			fprintf(file, "\tauth_mschapv2=%s \\\n", Y_OR_N(!cfg->auth_refuse_mschapv2));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_EAP) {
			fprintf(file, "\tauth_eap=%s \\\n", Y_OR_N(!cfg->auth_refuse_eap));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_NONE) {
			fprintf(file, "\tauth_none=%s \\\n", Y_OR_N(cfg->auth_none));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_PEER) {
			fprintf(file, "\tauth_peer=%s \\\n", Y_OR_N(cfg->auth_peer));
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_SYNC_MODE) {
			fprintf(file, "\tsync_mode=%s \\\n", 
				cfg->sync_mode == L2TP_API_PPP_SYNCMODE_SYNC ? "sync" :
				cfg->sync_mode == L2TP_API_PPP_SYNCMODE_ASYNC ? "async" :
				cfg->sync_mode == L2TP_API_PPP_SYNCMODE_SYNC_ASYNC ? "any" : "??");
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_CHAP_INTERVAL) {
			fprintf(file, "\tchap_interval=%d \\\n", cfg->chap_interval);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_CHAP_MAX_CHALLENGE) {
			fprintf(file, "\tchap_max_challenge=%d \\\n", cfg->chap_max_challenge);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_CHAP_RESTART) {
			fprintf(file, "\tchap_restart=%d \\\n", cfg->chap_restart);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_PAP_MAX_AUTH_REQUESTS) {
			fprintf(file, "\tpap_max_auth_requests=%d \\\n", cfg->pap_max_auth_requests);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_PAP_RESTART_INTERVAL) {
			fprintf(file, "\tpap_restart_interval=%d \\\n", cfg->pap_restart_interval);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_PAP_TIMEOUT) {
			fprintf(file, "\tpap_timeout=%d \\\n", cfg->pap_timeout);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_IDLE_TIMEOUT) {
			fprintf(file, "\tidle_timeout=%d \\\n", cfg->idle_timeout);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_REQUESTS) {
			fprintf(file, "\tipcp_max_config_requests=%d \\\n", cfg->ipcp_max_config_requests);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_NAKS) {
			fprintf(file, "\tipcp_max_config_naks=%d \\\n", cfg->ipcp_max_config_naks);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_TERMINATE_REQUESTS) {
			fprintf(file, "\tipcp_max_terminate_requests=%d \\\n", cfg->ipcp_max_terminate_requests);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_RETRANSMIT_INTERVAL) {
			fprintf(file, "\tipcp_retransmit_interval=%d \\\n", cfg->ipcp_retransmit_interval);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_FAILURE_COUNT) {
			fprintf(file, "\tlcp_echo_failure_count=%d \\\n", cfg->lcp_echo_failure_count);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_INTERVAL) {
			fprintf(file, "\tlcp_echo_interval=%d \\\n", cfg->lcp_echo_interval);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_REQUESTS) {
			fprintf(file, "\tlcp_max_config_requests=%d \\\n", cfg->lcp_max_config_requests);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_NAKS) {
			fprintf(file, "\tlcp_max_config_naks=%d \\\n", cfg->lcp_max_config_naks);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_TERMINATE_REQUESTS) {
			fprintf(file, "\tlcp_max_terminate_requests=%d \\\n", cfg->lcp_max_terminate_requests);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_RETRANSMIT_INTERVAL) {
			fprintf(file, "\tlcp_retransmit_interval=%d \\\n", cfg->lcp_retransmit_interval);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_MAX_CONNECT_TIME) {
			fprintf(file, "\tmax_connect_time=%d \\\n", cfg->max_connect_time);
		}
		if (cfg->flags & L2TP_API_PPP_PROFILE_FLAG_MAX_FAILURE_COUNT) {
			fprintf(file, "\tmax_failure_count=%d \\\n", cfg->max_failure_count);
		}

		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_1) {
			struct in_addr ip;
			ip.s_addr = cfg->dns_addr_1.s_addr;
			fprintf(file, "\tdns_ipaddr_pri=%s \\\n", inet_ntoa(ip));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_2) {
			struct in_addr ip;
			ip.s_addr = cfg->dns_addr_2.s_addr;
			fprintf(file, "\tdns_ipaddr_sec=%s \\\n", inet_ntoa(ip));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_1) {
			struct in_addr ip;
			ip.s_addr = cfg->wins_addr_1.s_addr;
			fprintf(file, "\twins_ipaddr_pri=%s \\\n", inet_ntoa(ip));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_2) {
			struct in_addr ip;
			ip.s_addr = cfg->wins_addr_2.s_addr;
			fprintf(file, "\twins_ipaddr_sec=%s \\\n", inet_ntoa(ip));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_LOCAL_IP_ADDR) {
			struct in_addr ip;
			ip.s_addr = cfg->local_ip_addr.s_addr;
			fprintf(file, "\tlocal_ipaddr=%s \\\n", inet_ntoa(ip));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_PEER_IP_ADDR) {
			struct in_addr ip;
			ip.s_addr = cfg->peer_ip_addr.s_addr;
			fprintf(file, "\tremote_ipaddr=%s \\\n", inet_ntoa(ip));
		}
		if ((cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_IP_POOL_NAME) && (OPTSTRING_PTR(cfg->ip_pool_name) != NULL)) {
			fprintf(file, "\tip_pool_name=%s \\\n", OPTSTRING_PTR(cfg->ip_pool_name));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_USE_AS_DEFAULT_ROUTE) {
			fprintf(file, "\tdefault_route=%s \\\n", Y_OR_N(cfg->use_as_default_route));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_MULTILINK) {
			fprintf(file, "\tmultilink=%s \\\n", Y_OR_N(cfg->multilink));
		}
		if ((cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_LOCAL_NAME) && (OPTSTRING_PTR(cfg->local_name) != NULL)) {
			fprintf(file, "\tlocal_name=%s \\\n", OPTSTRING_PTR(cfg->local_name));
		}
		if ((cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_REMOTE_NAME) && (OPTSTRING_PTR(cfg->remote_name) != NULL)) {
			fprintf(file, "\tremote_name=%s \\\n", OPTSTRING_PTR(cfg->remote_name));
		}
		if (cfg->flags2 & L2TP_API_PPP_PROFILE_FLAG_PROXY_ARP) {
			fprintf(file, "\tproxy_arp=%s \\\n", Y_OR_N(cfg->proxy_arp));
		}
		fprintf(file, "\n");
	}
}

struct l2tp_config_tunnel_map {
	char *tunnel_name;
	uint16_t tunnel_id;
	struct l2tp_config_tunnel_map *next;
};

static struct l2tp_config_tunnel_map *l2tp_config_tunnel_map = NULL;

static int l2tp_config_tunnel_map_add(char *tunnel_name, uint16_t tunnel_id)
{
	struct l2tp_config_tunnel_map *entry;
	static struct l2tp_config_tunnel_map *tail = NULL;

	entry = calloc(1, sizeof(*entry));
	if (entry == NULL) {
		return -ENOMEM;
	}

	if (tunnel_name != NULL) {
		entry->tunnel_name = strdup(tunnel_name);
		if (entry->tunnel_name == NULL) {
			free(entry);
			return -ENOMEM;
		}
	}
	entry->tunnel_id = tunnel_id;

	if (l2tp_config_tunnel_map == NULL) {
		l2tp_config_tunnel_map = entry;
		tail = entry;
	} else {
		tail->next = entry;
		tail = entry;
	}

	return 0;
}

static struct l2tp_config_tunnel_map *l2tp_config_tunnel_map_find(char *tunnel_name, uint16_t tunnel_id)
{
	struct l2tp_config_tunnel_map *entry;

	for (entry = l2tp_config_tunnel_map; entry != NULL; entry = entry->next) {
		if (tunnel_id != 0) {
			if (entry->tunnel_id == tunnel_id) {
				return entry;
			}
		} else if ((tunnel_name != NULL) && (entry->tunnel_name != NULL)) {
			if (strcmp(entry->tunnel_name, tunnel_name) == 0) {
				return entry;
			}
		}
	}

	return NULL;
}

static void l2tp_config_tunnel_map_cleanup(void)
{
	struct l2tp_config_tunnel_map *entry;
	struct l2tp_config_tunnel_map *tmp;

	for (entry = l2tp_config_tunnel_map; entry != NULL; ) {
		tmp = entry->next;
		free(entry->tunnel_name);
		free(entry);
		entry = tmp;
	}
	
}

static void l2tp_config_dump_tunnel(FILE *file, struct l2tp_api_tunnel_msg_data *cfg)
{
	struct in_addr ip;
	char tunnel_name[10]; /* l2tpNNNNN */

	/* If the tunnel wasn't given an tunnel_name, derive one from its tunnel_id */
	if ((cfg->flags & L2TP_API_TUNNEL_FLAG_TUNNEL_NAME) == 0) {
		sprintf(&tunnel_name[0], "l2tp%hu", cfg->tunnel_id);
		OPTSTRING(cfg->tunnel_name) = tunnel_name;
		cfg->tunnel_name.valid = 1;
	}

	/* Record the mapping of tunnel_id to tunnel_name so that we can easily
	 * derive the tunnel name from a tunnel_id when dumping sessions.
	 */
	l2tp_config_tunnel_map_add(OPTSTRING_PTR(cfg->tunnel_name), cfg->tunnel_id);

	ip.s_addr = cfg->peer_addr.s_addr;
	fprintf(file, "tunnel create tunnel_name=%s dest_ipaddr=%s \\\n", 
		OPTSTRING_PTR(cfg->tunnel_name), inet_ntoa(ip));
#ifdef L2TP_TEST
	fprintf(file, "\ttunnel_id=%hu \\\n", cfg->tunnel_id);
#endif
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_CONFIG_ID) {
		fprintf(file, "\tconfig_id=%d \\\n", cfg->config_id);
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_HIDE_AVPS) {
		fprintf(file, "\thide_avps=%s \\\n", Y_OR_N(cfg->hide_avps));
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_AUTH_MODE) {
		fprintf(file, "\tauth_mode=%s \\\n", 
			(cfg->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_NONE) ? "none" :
			(cfg->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_SIMPLE) ? "simple" :
			(cfg->auth_mode == L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE) ? "challenge" : "??");
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_FRAMING_CAP) {
		fprintf(file, "\tframing_cap=%s \\\n", 
			(!cfg->framing_cap_sync && !cfg->framing_cap_async) ? "none" :
			(cfg->framing_cap_sync && cfg->framing_cap_async) ? "any" :
			(cfg->framing_cap_sync && !cfg->framing_cap_async) ? "sync" :
			(!cfg->framing_cap_sync && cfg->framing_cap_async) ? "async" : "??");
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_BEARER_CAP) {
		fprintf(file, "\tbearer_cap=%s \\\n", 
			(!cfg->bearer_cap_digital && !cfg->bearer_cap_analog) ? "none" :
			(cfg->bearer_cap_digital && cfg->bearer_cap_analog) ? "any" :
			(cfg->bearer_cap_digital && !cfg->bearer_cap_analog) ? "digital" :
			(!cfg->bearer_cap_digital && cfg->bearer_cap_analog) ? "analog" : "??");
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_USE_TIEBREAKER) {
		fprintf(file, "\tuse_tiebreaker=%s \\\n", Y_OR_N(cfg->use_tiebreaker));
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_HELLO_TIMEOUT) {
		fprintf(file, "\thello_timeout=%d \\\n", cfg->hello_timeout);
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_MAX_RETRIES) {
		fprintf(file, "\tmax_retries=%d \\\n", cfg->max_retries);
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_RX_WINDOW_SIZE) {
		fprintf(file, "\trx_window_size=%d \\\n", cfg->rx_window_size);
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_TX_WINDOW_SIZE) {
		fprintf(file, "\ttx_window_size=%d \\\n", cfg->tx_window_size);
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_RETRY_TIMEOUT) {
		fprintf(file, "\tretry_timeout=%d \\\n", cfg->retry_timeout);
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_IDLE_TIMEOUT) {
		fprintf(file, "\tidle_timeout=%d \\\n", cfg->idle_timeout);
	}
	if ((cfg->flags & L2TP_API_TUNNEL_FLAG_SECRET) && (OPTSTRING_PTR(cfg->secret) != NULL)) {
		fprintf(file, "\tsecret=%s \\\n", OPTSTRING_PTR(cfg->secret));
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_ALLOW_PPP_PROXY) {
		fprintf(file, "\tallow_ppp_proxy=%s \\\n", Y_OR_N(cfg->allow_ppp_proxy));
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_TRACE_FLAGS) {
		fprintf(file, "\ttrace_flags=%u \\\n", cfg->trace_flags);
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_USE_UDP_CHECKSUMS) {
		fprintf(file, "\tuse_udp_checksums=%s \\\n", Y_OR_N(cfg->use_udp_checksums));
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_PERSIST) {
		fprintf(file, "\tpersist=%s \\\n", Y_OR_N(cfg->persist));
	}
	if ((cfg->flags & L2TP_API_TUNNEL_FLAG_HOST_NAME) && (OPTSTRING_PTR(cfg->host_name) != NULL)) {
		fprintf(file, "\thost_name=%s \\\n", OPTSTRING_PTR(cfg->host_name));
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_MAX_SESSIONS) {
		fprintf(file, "\tmax_sessions=%d \\\n", cfg->max_sessions);
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_OUR_ADDR) {
		struct in_addr ip;
		ip.s_addr = cfg->our_addr.s_addr;
		fprintf(file, "\tsrc_ipaddr=%s \\\n", inet_ntoa(ip));
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_OUR_UDP_PORT) {
		fprintf(file, "\tour_udp_port=%hu \\\n", cfg->our_udp_port);
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_PEER_UDP_PORT) {
		fprintf(file, "\tpeer_udp_port=%hu \\\n", cfg->peer_udp_port);
	}
	if ((cfg->flags & L2TP_API_TUNNEL_FLAG_PEER_PROFILE_NAME) && (OPTSTRING_PTR(cfg->peer_profile_name) != NULL)) {
		fprintf(file, "\tpeer_profile_name=%s \\\n", OPTSTRING_PTR(cfg->peer_profile_name));
	}
	if ((cfg->flags & L2TP_API_TUNNEL_FLAG_SESSION_PROFILE_NAME) && (OPTSTRING_PTR(cfg->session_profile_name) != NULL)) {
		fprintf(file, "\tsession_profile_name=%s \\\n", OPTSTRING_PTR(cfg->session_profile_name));
	}
	if ((cfg->flags & L2TP_API_TUNNEL_FLAG_PPP_PROFILE_NAME) && (OPTSTRING_PTR(cfg->ppp_profile_name) != NULL)) {
		fprintf(file, "\tppp_profile_name=%s \\\n", OPTSTRING_PTR(cfg->ppp_profile_name));
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_DO_PMTU_DISCOVERY) {
		fprintf(file, "\tdo_pmtu_discovery=%s \\\n", Y_OR_N(cfg->do_pmtu_discovery));
	}
	if (cfg->flags & L2TP_API_TUNNEL_FLAG_MTU) {
		fprintf(file, "\tmtu=%d \\\n", cfg->mtu);
	}
	fprintf(file, "\n");
}

static void l2tp_config_dump_session(FILE *file, struct l2tp_api_session_msg_data *cfg)
{
	struct l2tp_config_tunnel_map *entry;

	/* Derive the tunnel name from the tunnel_id if tunnel_name is not set */

	if ((cfg->flags & L2TP_API_SESSION_FLAG_TUNNEL_NAME) == 0) {
		entry = l2tp_config_tunnel_map_find(NULL, cfg->tunnel_id);
		if (entry == NULL) {
			return;
		}
		OPTSTRING(cfg->tunnel_name) = entry->tunnel_name;
		cfg->tunnel_name.valid = 1;
	}

	fprintf(file, "session create tunnel_name=%s \\\n", OPTSTRING_PTR(cfg->tunnel_name));
#ifdef L2TP_TEST
	fprintf(file, "\ttunnel_id=%hu \\\n", cfg->tunnel_id);
	fprintf(file, "\tsession_id=%hu \\\n", cfg->session_id);
#endif

	if (cfg->flags & L2TP_API_SESSION_FLAG_TRACE_FLAGS) {
		fprintf(file, "\ttrace_flags=%u \\\n", cfg->trace_flags);
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_SEQUENCING_REQUIRED) {
		fprintf(file, "\tsequencing_required=%s \\\n", Y_OR_N(cfg->sequencing_required));
	}
	if ((cfg->flags & L2TP_API_SESSION_FLAG_PROFILE_NAME) && (OPTSTRING_PTR(cfg->profile_name) != NULL)) {
		fprintf(file, "\tprofile_name=%s \\\n", OPTSTRING_PTR(cfg->profile_name));
	}
	if ((cfg->flags & L2TP_API_SESSION_FLAG_PPP_PROFILE_NAME) && (OPTSTRING_PTR(cfg->ppp_profile_name) != NULL)) {
		fprintf(file, "\tppp_profile_name=%s \\\n", OPTSTRING_PTR(cfg->ppp_profile_name));
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_SESSION_TYPE) {
		fprintf(file, "\tsession_type=%s \\\n", 
			(cfg->session_type == L2TP_API_SESSION_TYPE_LAIC) ? "laic" : 
			(cfg->session_type == L2TP_API_SESSION_TYPE_LAOC) ? "laoc" : 
			(cfg->session_type == L2TP_API_SESSION_TYPE_LNIC) ? "lnic" : 
			(cfg->session_type == L2TP_API_SESSION_TYPE_LNOC) ? "lnoc" : "??");
	}
	if ((cfg->flags & L2TP_API_SESSION_FLAG_USER_NAME) && (OPTSTRING(cfg->user_name) != NULL)) {
		fprintf(file, "\tuser_name=%s \\\n", OPTSTRING(cfg->user_name));
	}
	if ((cfg->flags & L2TP_API_SESSION_FLAG_USER_PASSWORD) && (OPTSTRING(cfg->user_password) != NULL)) {
		fprintf(file, "\tuser_password=%s \\\n", OPTSTRING(cfg->user_password));
	}
	if ((cfg->flags & L2TP_API_SESSION_FLAG_PRIV_GROUP_ID) && (OPTSTRING_PTR(cfg->priv_group_id) != NULL)) {
		fprintf(file, "\tpriv_group_id=%s \\\n", OPTSTRING_PTR(cfg->priv_group_id));
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_FRAMING_TYPE) {
		fprintf(file, "\tframing_type=%s \\\n", 
			(!cfg->framing_type_sync && !cfg->framing_type_async) ? "none" :
			(cfg->framing_type_sync && cfg->framing_type_async) ? "any" :
			(cfg->framing_type_sync && !cfg->framing_type_async) ? "sync" :
			(!cfg->framing_type_sync && cfg->framing_type_async) ? "async" : "??");
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_BEARER_TYPE) {
		fprintf(file, "\tbearer_type=%s \\\n", 
			(!cfg->bearer_type_digital && !cfg->bearer_type_analog) ? "none" :
			(cfg->bearer_type_digital && cfg->bearer_type_analog) ? "any" :
			(cfg->bearer_type_digital && !cfg->bearer_type_analog) ? "digital" :
			(!cfg->bearer_type_digital && cfg->bearer_type_analog) ? "analog" : "??");
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_MINIMUM_BPS) {
		fprintf(file, "\tminimum_bps=%d \\\n", cfg->minimum_bps);
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_MAXIMUM_BPS) {
		fprintf(file, "\tmaximum_bps=%d \\\n", cfg->maximum_bps);
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_CONNECT_SPEED) {
		fprintf(file, "\tconnect_speed=%d:%d \\\n", cfg->rx_connect_speed, cfg->tx_connect_speed);
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_USE_PPP_PROXY) {
		fprintf(file, "\tuse_ppp_proxy=%s \\\n", Y_OR_N(cfg->use_ppp_proxy));
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_USE_SEQUENCE_NUMBERS) {
		fprintf(file, "\tuse_sequence_numbers=%s \\\n", Y_OR_N(cfg->use_sequence_numbers));
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_NO_PPP) {
		fprintf(file, "\tno_ppp=%s \\\n", Y_OR_N(cfg->no_ppp));
	}
	if (cfg->flags & L2TP_API_SESSION_FLAG_REORDER_TIMEOUT) {
		fprintf(file, "\treorder_timeout=%d \\\n", cfg->reorder_timeout);
	}
	fprintf(file, "\n");
}

static int l2tp_config_save(const char *file_name)
{
	struct l2tp_api_system_msg_data sys;
	int tid, sid;
	struct l2tp_api_peer_profile_list_msg_data peer_profile_list;
	struct l2tp_api_peer_profile_msg_data peer_profile;
	struct l2tp_api_peer_profile_list_entry *pewalk;
	struct l2tp_api_tunnel_profile_list_msg_data tunnel_profile_list;
	struct l2tp_api_tunnel_profile_msg_data tunnel_profile;
	struct l2tp_api_tunnel_profile_list_entry *tpwalk;
	struct l2tp_api_session_profile_list_msg_data session_profile_list;
	struct l2tp_api_session_profile_msg_data session_profile;
	struct l2tp_api_session_profile_list_entry *spwalk;
	struct l2tp_api_ppp_profile_list_msg_data ppp_profile_list;
	struct l2tp_api_ppp_profile_msg_data ppp_profile;
	struct l2tp_api_ppp_profile_list_entry *ppwalk;
	struct l2tp_api_tunnel_list_msg_data tunnel_list;
	struct l2tp_api_tunnel_msg_data tunnel;
	struct l2tp_api_session_list_msg_data session_list;
	struct l2tp_api_session_msg_data session;
	int num_tunnels;
	int num_sessions;
	int result = 0;
	FILE *file;
	optstring session_name = { 0, };
	optstring tunnel_name = { 0, };

	/* Open the output stream */
	
	if (file_name != NULL) {
		file = fopen(file_name, "w");
		if (file == NULL) {
			fprintf(stderr, "Failed to open output file %s: %m\n", file_name);
			result = -errno;
			goto out;
		}
	} else {
		file = stdout;
	}

	/* system */

	fprintf(file, "\n# system\n");
	memset(&sys, 0, sizeof(sys));
	result = l2tp_system_get_1(&sys, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	l2tp_config_dump_system(file, &sys);

	/* peer profile */

	fprintf(file, "\n# peer profiles\n");
	memset(&peer_profile_list, 0, sizeof(peer_profile_list));
	result = l2tp_peer_profile_list_1(&peer_profile_list, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (peer_profile_list.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-peer_profile_list.result));
		result = peer_profile_list.result;
		goto out;
	}

	pewalk = peer_profile_list.profiles;
	while ((pewalk != NULL) && (pewalk->profile_name[0] != '\0')) {
		memset(&peer_profile, 0, sizeof(peer_profile));
		result = l2tp_peer_profile_get_1(pewalk->profile_name, &peer_profile, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}

		if (peer_profile.result_code < 0) {
			continue;
		}

		l2tp_config_dump_peer_profile(file, &peer_profile);

		pewalk = pewalk->next;
	}	

	/* tunnel profile */

	fprintf(file, "\n# tunnel profiles\n");
	memset(&tunnel_profile_list, 0, sizeof(tunnel_profile_list));
	result = l2tp_tunnel_profile_list_1(&tunnel_profile_list, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (tunnel_profile_list.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-tunnel_profile_list.result));
		result = tunnel_profile_list.result;
		goto out;
	}

	tpwalk = tunnel_profile_list.profiles;
	while ((tpwalk != NULL) && (tpwalk->profile_name[0] != '\0')) {
		memset(&tunnel_profile, 0, sizeof(tunnel_profile));
		result = l2tp_tunnel_profile_get_1(tpwalk->profile_name, &tunnel_profile, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}

		if (tunnel_profile.result_code < 0) {
			continue;
		}

		l2tp_config_dump_tunnel_profile(file, &tunnel_profile);

		tpwalk = tpwalk->next;
	}	

	/* session profile */

	fprintf(file, "\n# session profiles\n");
	memset(&session_profile_list, 0, sizeof(session_profile_list));
	result = l2tp_session_profile_list_1(&session_profile_list, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (session_profile_list.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-session_profile_list.result));
		result = session_profile_list.result;
		goto out;
	}

	spwalk = session_profile_list.profiles;
	while ((spwalk != NULL) && (spwalk->profile_name[0] != '\0')) {
		memset(&session_profile, 0, sizeof(session_profile));
		result = l2tp_session_profile_get_1(spwalk->profile_name, &session_profile, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}

		if (session_profile.result_code < 0) {
			continue;
		}

		l2tp_config_dump_session_profile(file, &session_profile);

		spwalk = spwalk->next;
	}	

	/* ppp profile */

	fprintf(file, "\n# ppp profiles\n");
	memset(&ppp_profile_list, 0, sizeof(ppp_profile_list));
	result = l2tp_ppp_profile_list_1(&ppp_profile_list, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (ppp_profile_list.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-ppp_profile_list.result));
		result = ppp_profile_list.result;
		goto out;
	}

	ppwalk = ppp_profile_list.profiles;
	while ((ppwalk != NULL) && (ppwalk->profile_name[0] != '\0')) {
		memset(&ppp_profile, 0, sizeof(ppp_profile));
		result = l2tp_ppp_profile_get_1(ppwalk->profile_name, &ppp_profile, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}

		if (ppp_profile.result_code < 0) {
			continue;
		}

		l2tp_config_dump_ppp_profile(file, &ppp_profile);

		ppwalk = ppwalk->next;
	}	

	/* tunnels and sessions */

	fprintf(file, "\n# locally created tunnels and sessions\n");
	memset(&tunnel_list, 0, sizeof(tunnel_list));
	result = l2tp_tunnel_list_1(&tunnel_list, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (tunnel_list.result != 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-tunnel_list.result));
		result = tunnel_list.result;
		goto out;
	}

	num_tunnels = tunnel_list.tunnel_ids.tunnel_ids_len;

	for (tid = 0; tid < num_tunnels; tid++) {
		memset(&tunnel, 0, sizeof(tunnel));
		result = l2tp_tunnel_get_1(tunnel_list.tunnel_ids.tunnel_ids_val[tid], tunnel_name, &tunnel, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}
		if (tunnel.result_code < 0) {
			continue;
		}
		if (!tunnel.created_by_admin) {
			continue;
		}

		l2tp_config_dump_tunnel(file, &tunnel);

		memset(&session_list, 0, sizeof(session_list));
		result = l2tp_session_list_1(tunnel_list.tunnel_ids.tunnel_ids_val[tid], tunnel_name, &session_list, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}
		if (session_list.result != 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-session_list.result));
			result = session_list.result;
			goto out;
		}

		num_sessions = session_list.session_ids.session_ids_len;

		for (sid = 0; sid < num_sessions; sid++) {
			memset(&session, 0, sizeof(session));
			result = l2tp_session_get_1(tunnel_list.tunnel_ids.tunnel_ids_val[tid], tunnel_name, session_list.session_ids.session_ids_val[sid], session_name, &session, cl);
			if (result != RPC_SUCCESS) {
				clnt_perror(cl, server);
				result = -EAGAIN;
				goto out;
			}
			if (session.result_code < 0) {
				continue;
			}
			if (!session.created_by_admin) {
				continue;
			}

			l2tp_config_dump_session(file, &session);
		}
	}

out:
	l2tp_config_tunnel_map_cleanup();

	if (file != NULL) {
		fflush(file);
		fclose(file);
	}

	return result;
}

static int l2tp_config_get_line(char *buffer, int buf_size, FILE *file)
{
	int count = 0;

	buffer = fgets(buffer, buf_size, file);
	if (buffer == NULL) {
		return -1;
	}
	count = strlen(buffer);
	if (count > 0) {
		/* strip cr at end of line */
		count--;
		buffer[count] = '\0';
	}
	
	return count;
}

static int l2tp_config_restore(const char *file_name)
{
	int result = 0;
	FILE *file = NULL;
	char *buffer = NULL;
	int count;
	char *cmd[] = { NULL, };

	if (file_name == NULL) {
		result = -EINVAL;
		goto out;
	}

	buffer = malloc(4000);
	if (buffer == NULL) {
		result = -ENOMEM;
		goto out;
	}
	cmd[0] = buffer;

	/* Open the input stream */
	
	file = fopen(file_name, "r");
	if (file == NULL) {
		fprintf(stderr, "Failed to open input file %s: %m\n", file_name);
		result = -errno;
		goto out;
	}

	/* Read line into our input buffer. If a newline is escaped with a '\\',
	 * continue reading next line into buffer.
	 * Ignore lines beginning with '#'.
	 * Ignore blank lines.
	 */
	count = 0;
	for (;;) {
		int chars_read;

		chars_read = l2tp_config_get_line(buffer + count, 4000 - count, file);
		if (chars_read == 0) {
			/* blank line */
			if (count > 0) goto got_command;
			continue;
		}
		if (chars_read < 0) {
			/* end of input */
			break;
		}
		if ((count == 0) && (buffer[0] == '#')) {
			/* comment line */
			if (count > 0) goto got_command;
			continue;
		}
		count += chars_read;
		if (buffer[count - 1] == '\\') {
			/* line is continued on next */
			count--;
			buffer[count] = '\0';
			continue;
		}

	got_command:
		/* replay the command  */
		result = cli_execute(1, cmd);
		if (result < 0) {
			fprintf(stderr, "Command replay at command:\n");
			fprintf(stderr, "%s\n", buffer);
			fprintf(stderr, "Aborting.\n");
			result = 0;
			goto out;
		}

		/* get ready for next line */
		count = 0;
	}

out:
	if (buffer != NULL) {
		free(buffer);
	}
	if (file != NULL) {
		fclose(file);
	}

	return result;
}

static int l2tp_act_config_save(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *file_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_config_arg_ids_t, int);
	int ret = 0;

	clnt_res = 0;

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_CONFIG_ARGID_FILENAME:
			file_name = arg_values[arg];
			break;
		}
	} L2TP_ACT_END();

	ret = l2tp_config_save(file_name);

out:
	return ret;
}

static int l2tp_act_config_restore(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *file_name = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_config_arg_ids_t, int);
	int ret = 0;

	clnt_res = 0;

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_CONFIG_ARGID_FILENAME:
			file_name = arg_values[arg];
			break;
		}
	} L2TP_ACT_END();

	if (file_name == NULL) {
		fprintf(stderr, "Required file_name argument is missing.\n");
		exit(1);
	}

	ret = l2tp_config_restore(file_name);

out:
	return ret;
}

/*****************************************************************************
 * debug ...
 *****************************************************************************/

#undef ARG
#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_DEBUG_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#undef FLG
#define FLG(id, name, doc) \
	{ name, { L2TP_DEBUG_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

typedef enum {
	L2TP_DEBUG_ARGID_DEBUG_PROTOCOL,
	L2TP_DEBUG_ARGID_DEBUG_FSM,
	L2TP_DEBUG_ARGID_DEBUG_API,
	L2TP_DEBUG_ARGID_DEBUG_AVP,
	L2TP_DEBUG_ARGID_DEBUG_AVP_HIDE,
	L2TP_DEBUG_ARGID_DEBUG_AVP_DATA,
	L2TP_DEBUG_ARGID_DEBUG_FUNC,
	L2TP_DEBUG_ARGID_DEBUG_XPRT,
	L2TP_DEBUG_ARGID_DEBUG_DATA,
	L2TP_DEBUG_ARGID_DEBUG_PPP,
	L2TP_DEBUG_ARGID_DEBUG_SYSTEM,
	L2TP_DEBUG_ARGID_DEBUG_ALL,
	L2TP_DEBUG_ARGID_TUNNEL_ID,
	L2TP_DEBUG_ARGID_TUNNEL_NAME,
	L2TP_DEBUG_ARGID_SESSION_ID,
	L2TP_DEBUG_ARGID_SESSION_NAME,
	L2TP_DEBUG_ARGID_TUNNEL_PROFILE_NAME,
	L2TP_DEBUG_ARGID_SESSION_PROFILE_NAME,
	L2TP_DEBUG_ARGID_PPP_PROFILE_NAME,
	L2TP_DEBUG_ARGID_APP,
} l2tp_debug_arg_ids_t;

static struct cli_arg_entry l2tp_args_debug_modify[] = {
	ARG(DEBUG_PROTOCOL,		"protocol",		0,	bool,	"L2TP protocol events"),
	ARG(DEBUG_FSM,			"fsm",			0,	bool,	"Finite State Machine events (e.g. state changes)"),
	ARG(DEBUG_API,			"api",			0,	bool,	"Management interface interactions"),
	ARG(DEBUG_XPRT,			"transport",		0,	bool,	"Log tunnel transport activity, e.g. packet sequence" \
										"numbers, packet receive and transmit, to debug tunnel " \
										"link establishment or failures"),
	ARG(DEBUG_DATA,			"data",			0,	bool,	"Log L2TP data channel activity. Only L2TP control " \
										"messages are logged, never user data packets."),
	ARG(DEBUG_PPP,			"ppp_control",		0,	bool,	"Enables trace of PPP packets from the PPP subsystem" ),
	ARG(DEBUG_AVP,			"avp_info",		0,	bool,	"High level AVP info (shows AVPs present, not their contents)"),
	ARG(DEBUG_AVP_DATA,		"avp_data",		0,	bool,	"L2TP Attribute Value Pairs (AVPs) data contents" \
	    									"For detailed message content trace"),
	ARG(DEBUG_AVP_HIDE,		"avp_hide",		0,	bool,	"Show AVP hiding details"),
	ARG(DEBUG_FUNC,			"func",			0,	bool,	"Internal functional behavior"),
	ARG(DEBUG_SYSTEM,		"system",		0,	bool,	"Low level system activity, e.g. timers, sockets etc" ),
	ARG(DEBUG_ALL,			"all",			0,	bool,	"All possible trace categories" ),
	ARG(TUNNEL_ID,			"tunnel_id",		0,	uint16,	"tunnel_id of entity being modified"),
	ARG(TUNNEL_NAME,		"tunnel_name",		0,	string,	"tunnel_name of entity being modified"),
	ARG(SESSION_ID,			"session_id",		0,	uint16,	"session_id of entity being modified"),
	ARG(SESSION_NAME,		"session_name",		0,	string,	"session_name of entity being modified"),
	ARG(TUNNEL_PROFILE_NAME,	"tunnel_profile_name",	0,	bool,	"Name of tunnel profile being modified"),
	ARG(SESSION_PROFILE_NAME,	"session_profile_name",	0,	bool,	"Name of session profile being modified"),
	ARG(PPP_PROFILE_NAME,		"ppp_profile_name",	0,	bool,	"Name of ppp profile being modified"),
	FLG(APP,			"app",					"Modify application global debug settings"),
	{ NULL, },
};

static struct cli_arg_entry l2tp_args_debug_show[] = {
	ARG(TUNNEL_ID,			"tunnel_id",		0,	uint16,	"tunnel_id of entity being shown"),
	ARG(TUNNEL_NAME,		"tunnel_name",		0,	string,	"tunnel_name of entity being shown"),
	ARG(SESSION_ID,			"session_id",		0,	uint16,	"session_id of entity being shown"),
	ARG(SESSION_NAME,		"session_name",		0,	string,	"session_name of entity being shown"),
	ARG(TUNNEL_PROFILE_NAME,	"tunnel_profile_name",	0,	bool,	"Name of tunnel profile being shown"),
	ARG(SESSION_PROFILE_NAME,	"session_profile_name",	0,	bool,	"Name of session profile being shown"),
	ARG(PPP_PROFILE_NAME,		"ppp_profile_name",	0,	bool,	"Name of ppp profile being shown"),
	FLG(APP,			"app",					"Application global debug settings"),
	{ NULL, },
};

static int l2tp_act_debug_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	cli_bool_t bool_arg;
	int flags;
	uint32_t trace_flags;
	uint32_t trace_flags_mask;
	struct l2tp_debug_info {
		uint16_t tunnel_id;
		char	*tunnel_name;
		uint16_t session_id;
		char	*session_name;
		char	*tunnel_profile_name;
		char	*session_profile_name;
		char	*ppp_profile_name;
		int	app;
	} msg = { 0, };
	L2TP_ACT_DECLARATIONS(60, l2tp_debug_arg_ids_t, int);

	trace_flags = 0;
	trace_flags_mask = 0;
	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_DEBUG_ARGID_DEBUG_PROTOCOL:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_PROTOCOL;
			}
			trace_flags_mask |= L2TP_DEBUG_PROTOCOL;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_FSM:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_FSM;
			}
			trace_flags_mask |= L2TP_DEBUG_FSM;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_API:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_API;
			}
			trace_flags_mask |= L2TP_DEBUG_API;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_XPRT:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_XPRT;
			}
			trace_flags_mask |= L2TP_DEBUG_XPRT;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_DATA:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_DATA;
			}
			trace_flags_mask |= L2TP_DEBUG_DATA;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_PPP:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_PPP;
			}
			trace_flags_mask |= L2TP_DEBUG_PPP;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_AVP_DATA:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_AVP_DATA;
			}
			trace_flags_mask |= L2TP_DEBUG_AVP_DATA;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_AVP_HIDE:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_AVP_HIDE;
			}
			trace_flags_mask |= L2TP_DEBUG_AVP_HIDE;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_AVP:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_AVP;
			}
			trace_flags_mask |= L2TP_DEBUG_AVP;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_FUNC:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_FUNC;
			}
			trace_flags_mask |= L2TP_DEBUG_FUNC;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_SYSTEM:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags |= L2TP_DEBUG_SYSTEM;
			}
			trace_flags_mask |= L2TP_DEBUG_SYSTEM;
			break;
		case L2TP_DEBUG_ARGID_DEBUG_ALL:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], bool_arg, flags, 0);
			if (bool_arg) {
				trace_flags = 0xffffffff;
			} else {
				trace_flags = 0;
			}
			trace_flags_mask = 0xffffffff;
			break;
		case L2TP_DEBUG_ARGID_TUNNEL_PROFILE_NAME:
			msg.tunnel_profile_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_SESSION_PROFILE_NAME:
			msg.session_profile_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_PPP_PROFILE_NAME:
			msg.ppp_profile_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_TUNNEL_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.tunnel_id, flags, 0);
			break;
		case L2TP_DEBUG_ARGID_TUNNEL_NAME:
			msg.tunnel_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_SESSION_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.session_id, flags, 0);
			break;
		case L2TP_DEBUG_ARGID_SESSION_NAME:
			msg.session_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_APP:
			msg.app = 1;
			break;
		}
	} L2TP_ACT_END();

	if (msg.tunnel_profile_name != NULL) {
		struct l2tp_api_tunnel_profile_msg_data tunnel_profile;
		memset(&tunnel_profile, 0, sizeof(tunnel_profile));
		tunnel_profile.profile_name = msg.tunnel_profile_name;
		tunnel_profile.trace_flags = trace_flags;
		tunnel_profile.trace_flags_mask = trace_flags_mask;
		tunnel_profile.flags = L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS;
		result = l2tp_tunnel_profile_modify_1(tunnel_profile, &clnt_res, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			exit(1);
		}
		if (clnt_res < 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
			goto out;
		}
	} else if (msg.session_profile_name != NULL) {
		struct l2tp_api_session_profile_msg_data session_profile;
		memset(&session_profile, 0, sizeof(session_profile));
		session_profile.profile_name = msg.session_profile_name;
		session_profile.trace_flags = trace_flags;
		session_profile.trace_flags_mask = trace_flags_mask;
		session_profile.flags = L2TP_API_SESSION_PROFILE_FLAG_TRACE_FLAGS;
		result = l2tp_session_profile_modify_1(session_profile, &clnt_res, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			exit(1);
		}
		if (clnt_res < 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
			goto out;
		}
	} else if (msg.ppp_profile_name != NULL) {
		struct l2tp_api_ppp_profile_msg_data ppp_profile;
		memset(&ppp_profile, 0, sizeof(ppp_profile));
		ppp_profile.profile_name = msg.ppp_profile_name;
		ppp_profile.trace_flags = trace_flags;
		ppp_profile.trace_flags_mask = trace_flags_mask;
		ppp_profile.flags = L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS;
		result = l2tp_ppp_profile_modify_1(ppp_profile, &clnt_res, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			exit(1);
		}
		if (clnt_res < 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
			goto out;
		}
	} else if (((msg.tunnel_id != 0) || (msg.tunnel_name != NULL))  && ((msg.session_id != 0) || (msg.session_name != NULL))) {
		struct l2tp_api_session_msg_data session;
		memset(&session, 0, sizeof(session));
		session.tunnel_id = msg.tunnel_id;
		if (msg.tunnel_name != NULL) {
			OPTSTRING(session.tunnel_name) = msg.tunnel_name;
			session.tunnel_name.valid = 1;
		}
		session.session_id = msg.session_id;
		if (msg.session_name != NULL) {
			OPTSTRING(session.session_name) = msg.session_name;
			session.session_name.valid = 1;
		}
		session.trace_flags = trace_flags;
		session.trace_flags_mask = trace_flags_mask;
		session.flags = L2TP_API_SESSION_FLAG_TRACE_FLAGS;
		result = l2tp_session_modify_1(session, &clnt_res, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			exit(1);
		}
		if (clnt_res < 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
			goto out;
		}
	} else if ((msg.tunnel_id != 0) || (msg.tunnel_name != NULL)) {
		struct l2tp_api_tunnel_msg_data tunnel;
		memset(&tunnel, 0, sizeof(tunnel));
		tunnel.tunnel_id = msg.tunnel_id;
		if (msg.tunnel_name != NULL) {
			OPTSTRING(tunnel.tunnel_name) = msg.tunnel_name;
			tunnel.tunnel_name.valid = 1;
		}
		tunnel.trace_flags = trace_flags;
		tunnel.trace_flags_mask = trace_flags_mask;
		tunnel.flags = L2TP_API_TUNNEL_FLAG_TRACE_FLAGS;
		result = l2tp_tunnel_modify_1(tunnel, &clnt_res, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			exit(1);
		}
		if (clnt_res < 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
			goto out;
		}
	} else if (msg.app) {
		struct l2tp_api_system_msg_data sys;
		memset(&sys, 0, sizeof(sys));
		sys.config.trace_flags = trace_flags;
		sys.config.trace_flags_mask = trace_flags_mask;
		sys.config.flags = L2TP_API_CONFIG_FLAG_TRACE_FLAGS;
		result = l2tp_system_modify_1(sys, &clnt_res, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			exit(1);
		}
		if (clnt_res < 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
			goto out;
		}
	} else {
		fprintf(stderr, "Missing argument\n");
		result = -EINVAL;
		goto out;
	}

out:
	return result;
}

static int l2tp_act_debug_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	int flags = 0;
	int trace_flags;
	struct { 
		int app;
		uint16_t tunnel_id;
		char *tunnel_name;
		uint16_t session_id;
		char *session_name;
		char *tunnel_profile_name;
		char *session_profile_name;
		char *ppp_profile_name;
	} msg = { 0, };

	L2TP_ACT_DECLARATIONS(20, l2tp_debug_arg_ids_t, int);
	clnt_res = 0;

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_DEBUG_ARGID_APP:
			msg.app = 1;
			break;
		case L2TP_DEBUG_ARGID_TUNNEL_PROFILE_NAME:
			msg.tunnel_profile_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_SESSION_PROFILE_NAME:
			msg.session_profile_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_PPP_PROFILE_NAME:
			msg.ppp_profile_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_TUNNEL_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.tunnel_id, flags, 0);
			break;
		case L2TP_DEBUG_ARGID_TUNNEL_NAME:
			msg.tunnel_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_SESSION_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.session_id, flags, 0);
			break;
		case L2TP_DEBUG_ARGID_SESSION_NAME:
			msg.session_name = arg_values[arg];
			break;
		case L2TP_DEBUG_ARGID_DEBUG_PROTOCOL:
		case L2TP_DEBUG_ARGID_DEBUG_FSM:
		case L2TP_DEBUG_ARGID_DEBUG_API:
		case L2TP_DEBUG_ARGID_DEBUG_AVP:
		case L2TP_DEBUG_ARGID_DEBUG_AVP_HIDE:
		case L2TP_DEBUG_ARGID_DEBUG_AVP_DATA:
		case L2TP_DEBUG_ARGID_DEBUG_FUNC:
		case L2TP_DEBUG_ARGID_DEBUG_XPRT:
		case L2TP_DEBUG_ARGID_DEBUG_DATA:
		case L2TP_DEBUG_ARGID_DEBUG_PPP:
		case L2TP_DEBUG_ARGID_DEBUG_SYSTEM:
		case L2TP_DEBUG_ARGID_DEBUG_ALL:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END();

	if (msg.app) {
		struct l2tp_api_system_msg_data sys;
		memset(&sys, 0, sizeof(sys));
		result = l2tp_system_get_1(&sys, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}
		trace_flags = sys.config.trace_flags;
	} else if (msg.tunnel_profile_name != NULL) {
		struct l2tp_api_tunnel_profile_msg_data tunnel_profile;
		memset(&tunnel_profile, 0, sizeof(tunnel_profile));
		result = l2tp_tunnel_profile_get_1(msg.tunnel_profile_name, &tunnel_profile, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}
		if (tunnel_profile.result_code < 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-tunnel_profile.result_code));
			result = tunnel_profile.result_code;
			goto out;
		}
		trace_flags = tunnel_profile.trace_flags;
	} else if (msg.session_profile_name != NULL) {
		struct l2tp_api_session_profile_msg_data session_profile;
		memset(&session_profile, 0, sizeof(session_profile));
		result = l2tp_session_profile_get_1(msg.session_profile_name, &session_profile, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}
		if (session_profile.result_code < 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-session_profile.result_code));
			result = session_profile.result_code;
			goto out;
		}
		trace_flags = session_profile.trace_flags;
	} else if (msg.ppp_profile_name != NULL) {
		struct l2tp_api_ppp_profile_msg_data ppp_profile;
		memset(&ppp_profile, 0, sizeof(ppp_profile));
		result = l2tp_ppp_profile_get_1(msg.ppp_profile_name, &ppp_profile, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}
		if (ppp_profile.result_code < 0) {
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-ppp_profile.result_code));
			result = ppp_profile.result_code;
			goto out;
		}
		trace_flags = ppp_profile.trace_flags;
	} else if (((msg.tunnel_id != 0) || (msg.tunnel_name != NULL))  && ((msg.session_id != 0) || (msg.session_name != NULL))) {
		struct l2tp_api_session_msg_data session;
		optstring tunnel_name = { 0, };
		optstring session_name = { 0, };
		if (msg.tunnel_name != NULL) {
			OPTSTRING(tunnel_name) = msg.tunnel_name;
			tunnel_name.valid = 1;
		}
		if (msg.session_name != NULL) {
			OPTSTRING(session_name) = msg.session_name;
			session_name.valid = 1;
		}
		memset(&session, 0, sizeof(session));
		result = l2tp_session_get_1(msg.tunnel_id, tunnel_name, msg.session_id, session_name, &session, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}
		result = 0;
		if (session.result_code < 0) {
			result = session.result_code;
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-result));
			goto out;
		}
		l2tp_act_session_cleanup(&session);
		trace_flags = session.trace_flags;
	} else if ((msg.tunnel_id != 0) || (msg.tunnel_name != NULL)) {
		struct l2tp_api_tunnel_msg_data tunnel;
		optstring tunnel_name = { 0, };
		if (msg.tunnel_name != NULL) {
			OPTSTRING(tunnel_name) = msg.tunnel_name;
			tunnel_name.valid = 1;
		}
		memset(&tunnel, 0, sizeof(tunnel));
		result = l2tp_tunnel_get_1(msg.tunnel_id, tunnel_name, &tunnel, cl);
		if (result != RPC_SUCCESS) {
			clnt_perror(cl, server);
			result = -EAGAIN;
			goto out;
		}
		result = 0;
		if (tunnel.result_code < 0) {
			result = tunnel.result_code;
			fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-result));
			goto out;
		}
		trace_flags = tunnel.trace_flags;
		l2tp_act_tunnel_cleanup(&tunnel);
	} else {
		fprintf(stderr, "Missing argument\n");
		result = -EINVAL;
		goto out;
	}

	print_trace_flags(stdout, trace_flags, NULL);

out:
	return result;
}

/*****************************************************************************
 * Test/debug functions
 *****************************************************************************/

#ifdef L2TP_TEST

#undef ARG
#define ARG(id, name, flag, type, doc) \
	{ name, { L2TP_TEST_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#undef FLG
#define FLG(id, name, doc) \
	{ name, { L2TP_TEST_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

typedef enum {
	L2TP_TEST_ARGID_FAKE_RX_DROP,
	L2TP_TEST_ARGID_FAKE_TX_DROP,
	L2TP_TEST_ARGID_FAKE_TRIGGER_TYPE,
	L2TP_TEST_ARGID_CLEAR_FAKE_TRIGGER,
	L2TP_TEST_ARGID_HOLD_TUNNELS,
	L2TP_TEST_ARGID_HOLD_SESSIONS,
	L2TP_TEST_ARGID_NO_RANDOM_IDS,
	L2TP_TEST_ARGID_SHOW_PROFILE_USAGE,
	L2TP_TEST_ARGID_RESET_IDS,
	L2TP_TEST_ARGID_DEFAULT_CONFIG,
	L2TP_TEST_ARGID_LOG_MESSAGE,
	L2TP_TEST_ARGID_DO_TRANSPORT_TEST,
	L2TP_TEST_ARGID_TUNNEL_ID,
	L2TP_TEST_ARGID_SESSION_ID,
} l2tp_test_arg_ids_t;

static struct cli_arg_entry l2tp_args_test_modify[] = {
	ARG(FAKE_RX_DROP,	"fake_rx_drop",		0, 	bool, 	"Fake the dropping of one or more received L2TP control frames."),
	ARG(FAKE_TX_DROP,	"fake_tx_drop",		0, 	bool, 	"Fake the dropping of one or more transmitted L2TP control frames."),
	ARG(FAKE_TRIGGER_TYPE,	"fake_trigger_type",	0, 	string,	"Specifies how faked errors are to occur. Possible values are:-\n"
                                                                        "off    - faked error conditions off\n"
                                                                        "on     - faked error conditions on\n"
                                                                        "once   - faked error condition is forced once only\n"
                                                                        "low    - faked error condition occurs at random (~1%)\n"
                                                                        "medium - faked error condition occurs at random (~5%)\n"
                                                                        "high   - faked error condition occurs at random (~20%)"),
	FLG(CLEAR_FAKE_TRIGGER,	"clear_trigger",			"Clear the fake trigger status"),
	ARG(HOLD_TUNNELS,	"hold_tunnels",		0,	bool,	"Hold tunnel contexts until operator explicitely deletes them."),
	ARG(HOLD_SESSIONS,	"hold_sessions",	0,	bool,	"Hold session contexts until operator explicitely deletes them."),
	ARG(NO_RANDOM_IDS,	"no_random_ids",	0,	bool,	"Disable random tunnel_id/session_id generator."),
	ARG(SHOW_PROFILE_USAGE,	"show_profile_usage",	0,	bool,	"Show derived profile names in show commands."),
	FLG(RESET_IDS,		"reset_ids",				"Reset tunnel_id/session_id generator back to 0."),
	FLG(DEFAULT_CONFIG,	"default_config",			"Restore system back to default configuration."),
	FLG(DO_TRANSPORT_TEST,	"do_transport_test",			"Do transport test on the specified tunnel_id."),
	ARG(TUNNEL_ID,		"tunnel_id",		0,	uint16,	"Tunnel ID for some tests"),
	ARG(SESSION_ID,		"session_id",		0,	uint16,	"Session ID for some tests"),
	{ NULL, },
};

static int l2tp_act_test_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_test_msg_data msg = {0. };
	L2TP_ACT_DECLARATIONS(10, l2tp_test_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_TEST_ARGID_FAKE_RX_DROP:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.fake_rx_drop, msg.flags, L2TP_API_TEST_FLAG_FAKE_RX_DROP);
			break;
		case L2TP_TEST_ARGID_FAKE_TX_DROP:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.fake_tx_drop, msg.flags, L2TP_API_TEST_FLAG_FAKE_TX_DROP);
			break;
		case L2TP_TEST_ARGID_FAKE_TRIGGER_TYPE:
			if (arg_values[arg] == NULL) {
				arg_values[arg] = empty_string;
			}
			if (strcasecmp(arg_values[arg], "once") == 0) {
				msg.fake_trigger_type = L2TP_API_TEST_FAKE_TRIGGER_ONCE;
			} else if (strcasecmp(arg_values[arg], "low") == 0) {
				msg.fake_trigger_type = L2TP_API_TEST_FAKE_TRIGGER_LOW;
			} else if (strcasecmp(arg_values[arg], "medium") == 0) {
				msg.fake_trigger_type = L2TP_API_TEST_FAKE_TRIGGER_MEDIUM;
			} else if (strcasecmp(arg_values[arg], "high") == 0) {
				msg.fake_trigger_type = L2TP_API_TEST_FAKE_TRIGGER_HIGH;
			} else if (strcasecmp(arg_values[arg], "off") == 0) {
				msg.fake_trigger_type = L2TP_API_TEST_FAKE_TRIGGER_OFF;
			} else if (strcasecmp(arg_values[arg], "on") == 0) {
				msg.fake_trigger_type = L2TP_API_TEST_FAKE_TRIGGER_ON;
			} else {
				fprintf(stderr, "Bad value: %s. Expecting once|low|medium|high|off|on\n", arg_values[arg]);
				result = -EINVAL;
				goto out;
			}
			msg.flags |= L2TP_API_TEST_FLAG_FAKE_TRIGGER_TYPE;
			break;
		case L2TP_TEST_ARGID_CLEAR_FAKE_TRIGGER:
			msg.flags |= L2TP_API_TEST_FLAG_CLEAR_FAKE_TRIGGER;
			break;
		case L2TP_TEST_ARGID_HOLD_TUNNELS:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.hold_tunnels, msg.flags, L2TP_API_TEST_FLAG_HOLD_TUNNELS);
			break;
		case L2TP_TEST_ARGID_HOLD_SESSIONS:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.hold_sessions, msg.flags, L2TP_API_TEST_FLAG_HOLD_SESSIONS);
			break;
		case L2TP_TEST_ARGID_NO_RANDOM_IDS:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.no_random_ids, msg.flags, L2TP_API_TEST_FLAG_NO_RANDOM_IDS);
			break;
		case L2TP_TEST_ARGID_SHOW_PROFILE_USAGE:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.show_profile_usage, msg.flags, L2TP_API_TEST_FLAG_SHOW_PROFILE_USAGE);
			break;
		case L2TP_TEST_ARGID_RESET_IDS:
			msg.flags |= L2TP_API_TEST_FLAG_RESET_IDS;
			break;
		case L2TP_TEST_ARGID_DEFAULT_CONFIG:
			msg.flags |= L2TP_API_TEST_FLAG_DEFAULT_CONFIG;
			break;
		case L2TP_TEST_ARGID_LOG_MESSAGE:
			result = -EINVAL;
			goto out;
		case L2TP_TEST_ARGID_DO_TRANSPORT_TEST:
			msg.flags |= L2TP_API_TEST_FLAG_DO_TRANSPORT_TEST;
			break;
		case L2TP_TEST_ARGID_TUNNEL_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.tunnel_id, msg.flags, L2TP_API_TEST_FLAG_TUNNEL_ID);
			break;
		case L2TP_TEST_ARGID_SESSION_ID:
			L2TP_ACT_PARSE_ARG(args[arg], arg_values[arg], msg.session_id, msg.flags, L2TP_API_TEST_FLAG_SESSION_ID);
			break;
		}
	} L2TP_ACT_END()

	result = l2tp_test_modify_1(msg, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = -clnt_res;
		goto out;
	}
	if (!opt_quiet) {
		fprintf(stderr, "Modified test config\n");
	}

out:
	return 0;
}

static struct cli_arg_entry l2tp_args_test_log[] = {
	ARG(LOG_MESSAGE,	"message",		0,	string,	"Send a message to the L2TP service log file."),
	{ NULL, },
};

static int l2tp_act_test_log(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	char *message = NULL;
	L2TP_ACT_DECLARATIONS(4, l2tp_test_arg_ids_t, int);

	L2TP_ACT_BEGIN() {
		switch (arg_id) {
		case L2TP_TEST_ARGID_LOG_MESSAGE:
			message = arg_values[arg];
			break;
		default:
			result = -EINVAL;
			goto out;
		}
	} L2TP_ACT_END()

	if (message == NULL) {
		fprintf(stderr, "Required message argument missing\n");
		result = -EINVAL;
		goto out;
	}

	result = l2tp_test_log_1(message, &clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}
	if (clnt_res < 0) {
		fprintf(stderr, "Operation failed: %s\n", l2tp_strerror(-clnt_res));
		result = -clnt_res;
		goto out;
	}

out:
	return 0;
}

static int l2tp_act_test_show(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	struct l2tp_api_test_msg_data clnt_res;
	int result = 0;

	memset(&clnt_res, 0, sizeof(clnt_res));

	result = l2tp_test_get_1(&clnt_res, cl);
	if (result != RPC_SUCCESS) {
		clnt_perror(cl, server);
		result = -EAGAIN;
		goto out;
	}

	printf("Test settings:-\n");
	printf("  fake rx drop: %s, fake tx drop: %s, fake trigger: %s\n",
	       clnt_res.fake_rx_drop ? "YES" : "NO", 
	       clnt_res.fake_tx_drop ? "YES" : "NO",
	       (clnt_res.fake_trigger_type == L2TP_API_TEST_FAKE_TRIGGER_ON) ? "ON" :
	       (clnt_res.fake_trigger_type == L2TP_API_TEST_FAKE_TRIGGER_OFF) ? "OFF" :
	       (clnt_res.fake_trigger_type == L2TP_API_TEST_FAKE_TRIGGER_LOW) ? "LOW" :
	       (clnt_res.fake_trigger_type == L2TP_API_TEST_FAKE_TRIGGER_MEDIUM) ? "MEDIUM" :
	       (clnt_res.fake_trigger_type == L2TP_API_TEST_FAKE_TRIGGER_HIGH) ? "HIGH" :
	       (clnt_res.fake_trigger_type == L2TP_API_TEST_FAKE_TRIGGER_ONCE) ? "ONCE" : "??");
	if ((clnt_res.tunnel_id != 0) || (clnt_res.session_id != 0)) {
		printf("  tunnel_id: %hu, session_id: %hu\n", 
		       clnt_res.tunnel_id, clnt_res.session_id);
	}
	printf("  trigger status: %s\n", clnt_res.fake_trigger_fired ? "TRIGGERED" : "NOT TRIGGERED");
	if (clnt_res.fake_trigger_fired) {
		printf("  rx drops: %u, tx drops: %u\n", 
		       clnt_res.num_rx_drops, clnt_res.num_tx_drops);
	}
	printf("  hold tunnels: %s, sessions: %s\n",
	       clnt_res.hold_tunnels ? "YES" : "NO", clnt_res.hold_sessions ? "YES" : "NO");
	printf("  hash list hits/misses:-\n");
	printf("    tunnel_id: %d/%d\n", 
	       clnt_res.num_tunnel_id_hash_hits, clnt_res.num_tunnel_id_hash_misses);
	printf("    tunnel_name: %d/%d\n", 
	       clnt_res.num_tunnel_name_hash_hits, clnt_res.num_tunnel_name_hash_misses);
	printf("    session_id: %d/%d\n", 
	       clnt_res.num_session_id_hash_hits, clnt_res.num_session_id_hash_misses);

out:
	return result;
}

#endif /* L2TP_TEST */

/*****************************************************************************
 * Syntax tree
 *****************************************************************************/

static struct cli_node_entry cmds[] = {
	{ 0, CLI_NODE_TYPE_COMMAND, "exit", "exit application", l2tp_act_exit },
	{ 0, CLI_NODE_TYPE_COMMAND, "quit", "exit application", l2tp_act_exit },
	{ 0, CLI_NODE_TYPE_COMMAND, "help", "display help information", l2tp_act_help },
	{ 0, CLI_NODE_TYPE_KEYWORD, "config", "configuration save/restore", },
	{ 1, CLI_NODE_TYPE_COMMAND, "save", "save configuration", l2tp_act_config_save, &l2tp_args_config[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "restore", "restore configurationfrom file", l2tp_act_config_restore, &l2tp_args_config[0], },
	{ 0, CLI_NODE_TYPE_KEYWORD, "server", "server configuration", },
	{ 1, CLI_NODE_TYPE_COMMAND, "modify", "modify server parameters", l2tp_act_server_modify, &l2tp_args_server_modify[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "show", "show server parameters", l2tp_act_server_show, },
	{ 0, CLI_NODE_TYPE_KEYWORD, "system", "system commands", },
	{ 1, CLI_NODE_TYPE_COMMAND, "modify", "modify system parameters", l2tp_act_system_modify, &l2tp_args_system_modify[0], },
	{ 1, CLI_NODE_TYPE_KEYWORD, "show", "show system configuration and statistics", },
	{ 2, CLI_NODE_TYPE_COMMAND, "configuration", "show system configuration", l2tp_act_system_show_config },
	{ 2, CLI_NODE_TYPE_COMMAND, "status", "show system status", l2tp_act_system_show_status },
	{ 2, CLI_NODE_TYPE_COMMAND, "statistics", "show system statistics", l2tp_act_system_show_stats },
	{ 2, CLI_NODE_TYPE_COMMAND, "version", "show system version", l2tp_act_system_show_version },
#ifdef L2TP_TEST
	{ 0, CLI_NODE_TYPE_KEYWORD, "test", "test commands" },
	{ 1, CLI_NODE_TYPE_COMMAND, "modify", "modify test parameters", l2tp_act_test_modify, &l2tp_args_test_modify[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "log", "test messages", l2tp_act_test_log, &l2tp_args_test_log[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "show", "show test parameters", l2tp_act_test_show, },
#endif /* L2TP_TEST */
	{ 0, CLI_NODE_TYPE_KEYWORD, "tunnel", "tunnel commands" },
	{ 1, CLI_NODE_TYPE_COMMAND, "create", "create a new L2TP tunnel", l2tp_act_tunnel_create, &l2tp_args_tunnel_create[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "delete", "delete an L2TP tunnel", l2tp_act_tunnel_delete, &l2tp_args_tunnel_delete[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "modify", "modify an L2TP tunnel", l2tp_act_tunnel_modify, &l2tp_args_tunnel_modify[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "show", "show an L2TP tunnel", l2tp_act_tunnel_show, &l2tp_args_tunnel_show[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "list", "list all L2TP tunnels", l2tp_act_tunnel_list, &l2tp_args_tunnel_list[0], },
	{ 1, CLI_NODE_TYPE_KEYWORD, "profile", "tunnel profile commands" },
	{ 2, CLI_NODE_TYPE_COMMAND, "create", "create a new L2TP tunnel profile", l2tp_act_tunnel_profile_create, &l2tp_args_tunnel_profile_create[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "delete", "delete an L2TP tunnel profile", l2tp_act_tunnel_profile_delete, &l2tp_args_tunnel_profile_delete[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "modify", "modify an L2TP tunnel profile", l2tp_act_tunnel_profile_modify, &l2tp_args_tunnel_profile_modify[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "show", "show an L2TP tunnel profile", l2tp_act_tunnel_profile_show, &l2tp_args_tunnel_profile_show[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "list", "list all L2TP tunnel profiles", l2tp_act_tunnel_profile_list },
	{ 2, CLI_NODE_TYPE_COMMAND, "unset", "unset parameters of an L2TP tunnel profile", l2tp_act_tunnel_profile_unset, &l2tp_args_tunnel_profile_unset[0], },
	{ 0, CLI_NODE_TYPE_KEYWORD, "session", "session commands" },
	{ 1, CLI_NODE_TYPE_COMMAND, "create", "create a new L2TP session", l2tp_act_session_create, &l2tp_args_session_create[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "delete", "delete an L2TP session", l2tp_act_session_delete, &l2tp_args_session_delete[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "modify", "modify an L2TP session", l2tp_act_session_modify, &l2tp_args_session_modify[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "show", "show an L2TP session", l2tp_act_session_show, &l2tp_args_session_show[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "list", "list all L2TP sessions on a specified tunnel", l2tp_act_session_list, &l2tp_args_session_list[0] },
	{ 1, CLI_NODE_TYPE_KEYWORD, "profile", "session profile commands" },
	{ 2, CLI_NODE_TYPE_COMMAND, "create", "create a new L2TP session profile", l2tp_act_session_profile_create, &l2tp_args_session_profile_create[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "delete", "delete an L2TP session profile", l2tp_act_session_profile_delete, &l2tp_args_session_profile_delete[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "modify", "modify an L2TP session profile", l2tp_act_session_profile_modify, &l2tp_args_session_profile_modify[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "show", "show an L2TP session profile", l2tp_act_session_profile_show, &l2tp_args_session_profile_show[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "list", "list all L2TP session profiles", l2tp_act_session_profile_list },
	{ 2, CLI_NODE_TYPE_COMMAND, "unset", "unset parameters of an L2TP session profile", l2tp_act_session_profile_unset, &l2tp_args_session_profile_unset[0], },
	{ 0, CLI_NODE_TYPE_KEYWORD, "ppp", "ppp commands" },
	{ 1, CLI_NODE_TYPE_KEYWORD, "profile", "ppp profile commands" },
	{ 2, CLI_NODE_TYPE_COMMAND, "create", "create a new L2TP ppp profile", l2tp_act_ppp_profile_create, &l2tp_args_ppp_profile_create[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "delete", "delete an L2TP ppp profile", l2tp_act_ppp_profile_delete, &l2tp_args_ppp_profile_delete[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "modify", "modify an L2TP ppp profile", l2tp_act_ppp_profile_modify, &l2tp_args_ppp_profile_modify[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "show", "show an L2TP ppp profile", l2tp_act_ppp_profile_show, &l2tp_args_ppp_profile_show[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "list", "list all L2TP ppp profiles", l2tp_act_ppp_profile_list },
	{ 2, CLI_NODE_TYPE_COMMAND, "unset", "unset parameters of an L2TP ppp profile", l2tp_act_ppp_profile_unset, &l2tp_args_ppp_profile_unset[0], },
	{ 0, CLI_NODE_TYPE_KEYWORD, "peer", "peer commands" },
	{ 1, CLI_NODE_TYPE_COMMAND, "show", "show a peer record", l2tp_act_peer_show, &l2tp_args_peer_show[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "list", "list all L2TP peer records", l2tp_act_peer_list },
	{ 1, CLI_NODE_TYPE_KEYWORD, "profile", "peer profile commands" },
	{ 2, CLI_NODE_TYPE_COMMAND, "create", "create a new L2TP peer profile", l2tp_act_peer_profile_create, &l2tp_args_peer_profile_create[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "delete", "delete an L2TP peer profile", l2tp_act_peer_profile_delete, &l2tp_args_peer_profile_delete[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "modify", "modify an L2TP peer profile", l2tp_act_peer_profile_modify, &l2tp_args_peer_profile_modify[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "show", "show an L2TP peer profile", l2tp_act_peer_profile_show, &l2tp_args_peer_profile_show[0], },
	{ 2, CLI_NODE_TYPE_COMMAND, "list", "list all L2TP peer profiles", l2tp_act_peer_profile_list },
	{ 2, CLI_NODE_TYPE_COMMAND, "unset", "unset parameters of an L2TP peer profile", l2tp_act_peer_profile_unset, &l2tp_args_peer_profile_unset[0], },
	{ 0, CLI_NODE_TYPE_KEYWORD, "user", "user-related commands" },
	{ 1, CLI_NODE_TYPE_COMMAND, "list", "list all connected L2TP users", l2tp_act_user_list },
	{ 0, CLI_NODE_TYPE_KEYWORD, "debug", "debug commands", },
	{ 1, CLI_NODE_TYPE_COMMAND, "modify", "modify debug settings", l2tp_act_debug_modify, &l2tp_args_debug_modify[0], },
	{ 1, CLI_NODE_TYPE_KEYWORD, "show", "show debug settings", l2tp_act_debug_show, &l2tp_args_debug_show[0], },
	{ 0, CLI_NODE_TYPE_END, NULL, },
};

/*****************************************************************************
 * Application init and cleanup
 *****************************************************************************/

static void cleanup(void)
{
	if (cl != NULL) {
		clnt_destroy(cl);
	}
	if (interactive) {
		cli_write_history_file(l2tp_histfile, l2tp_histfile_maxsize);
	}
}

int main(int argc, char *argv[])
{
	int result;
	int opt;
	int arg = 1;
	static char *exit_cmd[] = { "exit", NULL };
	char *hist_size;

	strcpy(server, "localhost");

	cli_init("l2tp");
	result = cli_add_commands(&cmds[0]);
	if (result < 0) {
		fprintf(stderr, "Application initialization error.\n");
		return result;
	}

	opterr = 0;		/* no error messages please */

	opt = getopt(argc, argv, "qR:t");
	switch (opt) {
	case 'q':
		opt_quiet = 1;
		arg++;
		break;
	case 'R':
		strncpy(server, optarg, sizeof(server));
		arg += 2;
		l2tp_set_prompt(server);
		break;
	case 't':
		opt_rpc_protocol = "tcp";
		arg++;
		break;
	default:
		break;
	}

	cl = clnt_create(server, L2TP_PROG, L2TP_VERSION, opt_rpc_protocol);
	if (cl == NULL) {
		clnt_pcreateerror(server);
		exit(1);
	}
	atexit(cleanup);

	/* If user supplied arguments, send them to the CLI now and immediately exit.
	 */
	if (argc > arg) {
		(void) cli_execute(argc - arg, &argv[arg]);
		(void) cli_execute(1, exit_cmd);
	} else {
		/* interactive mode */
		interactive = 1;
		l2tp_histfile = getenv("L2TP_HISTFILE");
		if (l2tp_histfile == NULL) {
			l2tp_histfile = "~/.l2tp_history";
		}
		hist_size = getenv("L2TP_HISTFILESIZE");
		if (hist_size != NULL) {
			l2tp_histfile_maxsize = strtoul(hist_size, NULL, 0);
		}

		cli_read_history_file(l2tp_histfile);
		cli_run();
	}

	return 0;
}
