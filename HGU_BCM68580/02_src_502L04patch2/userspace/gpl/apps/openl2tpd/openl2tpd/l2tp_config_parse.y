
%{
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <syslog.h>

#include "l2tp_config_types.h"

#include "l2tp_rpc.h"

static struct l2tp_api_system_msg_data system_config;
static struct l2tp_api_peer_profile_msg_data peer_profile;
static struct l2tp_api_tunnel_profile_msg_data tunnel_profile;
static struct l2tp_api_session_profile_msg_data session_profile;
static struct l2tp_api_ppp_profile_msg_data ppp_profile;
static struct l2tp_api_tunnel_msg_data tunnel;
static struct l2tp_api_session_msg_data session;

extern void l2tp_log(int level, char *fmt, ...);
extern void yyfatal(const char *s);

%}

%union {
	int num;
	unsigned long ulnum;
	l2tp_byte_vector_t val;
}

%token SYSTEM TUNNEL SESSION PROFILE PEER PPP
%token CREATE MODIFY
%token DIGITAL_ANALOG
%token TUNNEL_AUTH_MODE

%token PEER_IPADDR LAC_LNS

%token TUNNEL_ID TUNNEL_NAME
%token SESSION_ID SESSION_NAME

%token UDP_PORT
%token TRACE_FLAGS
%token MAX_TUNNELS
%token MAX_SESSIONS
%token DRAIN_TUNNELS
%token TUNNEL_ESTABLISH_TIMEOUT
%token SESSION_ESTABLISH_TIMEOUT
%token TUNNEL_PERSIST_PEND_TIMEOUT
%token SESSION_PERSIST_PEND_TIMEOUT
%token DENY_LOCAL_TUNNEL_CREATES
%token DENY_REMOTE_TUNNEL_CREATES
%token HIDE_AVPS
%token OUR_ADDR
%token PEER_ADDR
%token OUR_UDP_PORT
%token PEER_UDP_PORT
%token CONFIG_ID
%token AUTH_MODE
%token FRAMING_CAP
%token BEARER_CAP
%token USE_TIEBREAKER
%token ALLOW_PPP_PROXY
%token USE_PPP_PROXY
%token USE_UDP_CHECKSUMS
%token HELLO_TIMEOUT
%token MAX_RETRIES
%token RX_WINDOW_SIZE
%token TX_WINDOW_SIZE
%token RETRY_TIMEOUT
%token IDLE_TIMEOUT
%token DO_PMTU_DISCOVERY
%token PERSIST
%token MTU
%token SECRET
%token HOST_NAME
%token TUNNEL_PROFILE_NAME
%token PEER_PROFILE_NAME
%token SESSION_PROFILE_NAME
%token PPP_PROFILE_NAME
%token INTERFACE_NAME
%token SESSION_TYPE
%token USER_NAME
%token USER_PASSWORD
%token PRIV_GROUP_ID
%token PROFILE_NAME
%token SEQUENCING_REQUIRED
%token USE_SEQUENCE_NUMBERS
%token NO_PPP
%token REORDER_TIMEOUT
%token FRAMING_TYPE
%token BEARER_TYPE
%token MINIMUM_BPS
%token MAXIMUM_BPS
%token CONNECT_SPEED
%token PEER_PORT
%token WE_CAN_BE_LAC
%token WE_CAN_BE_LNS
%token DEFAULT_TUNNEL_PROFILE_NAME
%token DEFAULT_SESSION_PROFILE_NAME
%token DEFAULT_PPP_PROFILE_NAME
%token NETMASK
%token ASYNCMAP
%token MRU
%token AUTH_FLAGS
%token SYNC_MODE
%token CHAP_INTERVAL
%token CHAP_MAX_CHALLENGE
%token CHAP_RESTART
%token PAP_MAX_AUTH_REQUESTS
%token PAP_RESTART_INTERVAL
%token PAP_TIMEOUT
%token IPCP_MAX_CONFIG_REQUESTS
%token IPCP_MAX_CONFIG_NAKS
%token IPCP_MAX_TERMINATE_REQUESTS
%token IPCP_RETRANSMIT_INTERVAL
%token LCP_ECHO_FAILURE_COUNT
%token LCP_ECHO_INTERVAL
%token LCP_MAX_CONFIG_REQUESTS
%token LCP_MAX_CONFIG_NAKS
%token LCP_MAX_TERMINATE_REQUESTS
%token LCP_RETRANSMIT_INTERVAL
%token MAX_CONNECT_TIME
%token MAX_FAILURE_COUNT
%token LOCAL_IPADDR
%token REMOTE_IPADDR
%token DNS_IPADDR_PRI
%token DNS_IPADDR_SEC
%token WINS_IPADDR_PRI
%token WINS_IPADDR_SEC
%token IP_POOL_NAME
%token USE_RADIUS
%token RADIUS_HINT
%token USE_AS_DEFAULT_ROUTE
%token MULTILINK
%token PROXY_ARP
%token EOT
%token SLASH
%token BLCL
%token ELCL
%token EQUALS
%token DECSTRING
%token STRING
%token HEXSTRING
%token QUOTEDSTRING
%token BOOL
%token IPADDRESS

%token INITIAL_RCVD_LCP_CONFREQ
%token CALLING_NUMBER
%token PROXY_AUTH_NAME
%token SUB_ADDRESS
%token PROXY_AUTH_TYPE
%token PROXY_AUTH_RESPONSE
%token LAST_RCVD_LCP_CONFREQ
%token CALLED_NUMBER
%token PROXY_AUTH_CHALLENGE
%token LAST_SENT_LCP_CONFREQ
%token AUTH_PAP AUTH_CHAP AUTH_MSCHAPV1 AUTH_MSCHAPV2 AUTH_EAP AUTH_NONE AUTH_PEER
%token SRC_IPADDR DEST_IPADDR
%token LOCAL_NAME REMOTE_NAME

%type <ulnum> DECSTRING
%type <val> QUOTEDSTRING HEXSTRING STRING
%type <num> BOOL
%type <ulnum> IPADDRESS

%%
commands
	:	/* empty */
	|	EOT
	|	commands command
		{
		}
	;

command
	:	system_command
	|	peer_profile_command
	|	tunnel_profile_command
	|	session_profile_command
	|	ppp_profile_command
	|	tunnel_command
	|	session_command
	;

system_command
	:	SYSTEM MODIFY system_statements EOT
		{
			bool_t status;
			int result;
			status = l2tp_system_modify_1_svc(system_config, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "system: command failed: rc=%d", l2tp_strerror(-result));
		}
	;

system_statements
	: 	/* empty */
	|	system_statements system_statement
	;

system_statement
	:	TRACE_FLAGS EQUALS DECSTRING
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_TRACE_FLAGS;
			system_config.config.trace_flags = $3;
		}
	|	MAX_TUNNELS EQUALS DECSTRING
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_MAX_TUNNELS;
			system_config.config.max_tunnels = $3;
		}
	|	MAX_SESSIONS EQUALS DECSTRING
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_MAX_SESSIONS;
			system_config.config.max_sessions = $3;
		}
	|	DRAIN_TUNNELS EQUALS BOOL
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_DRAIN_TUNNELS;
			system_config.config.drain_tunnels = $3;
		}
	|	TUNNEL_ESTABLISH_TIMEOUT EQUALS DECSTRING
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_TUNNEL_ESTABLISH_TIMEOUT;
			system_config.config.tunnel_establish_timeout = $3;
		}
	|	SESSION_ESTABLISH_TIMEOUT EQUALS DECSTRING
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_SESSION_ESTABLISH_TIMEOUT;
			system_config.config.session_establish_timeout = $3;
		}
	|	TUNNEL_PERSIST_PEND_TIMEOUT EQUALS DECSTRING
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_TUNNEL_PERSIST_PEND_TIMEOUT;
			system_config.config.tunnel_persist_pend_timeout = $3;
		}
	|	SESSION_PERSIST_PEND_TIMEOUT EQUALS DECSTRING
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_SESSION_PERSIST_PEND_TIMEOUT;
			system_config.config.session_persist_pend_timeout = $3;
		}
	|	DENY_LOCAL_TUNNEL_CREATES EQUALS BOOL
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_DENY_LOCAL_TUNNEL_CREATES;
			system_config.config.deny_local_tunnel_creates = $3;
		}
	|	DENY_REMOTE_TUNNEL_CREATES EQUALS BOOL
		{
			system_config.config.flags |= L2TP_API_CONFIG_FLAG_DENY_REMOTE_TUNNEL_CREATES;
			system_config.config.deny_remote_tunnel_creates = $3;
		}
	;

peer_profile_command
	:	peer_profile_create_command
	|	peer_profile_modify_command
	;

peer_profile_create_command
	:	PEER PROFILE CREATE peer_profile_statements EOT
		{
			bool_t status;
			int result;
			if (peer_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_peer_profile_create_1_svc(peer_profile, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_PROFILE_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "peer profile create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&peer_profile, 0, sizeof(peer_profile));
		}
	;

peer_profile_modify_command
	:	PEER PROFILE MODIFY peer_profile_statements EOT
		{
			bool_t status;
			int result;
			if (peer_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_peer_profile_modify_1_svc(peer_profile, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "peer profile modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&peer_profile, 0, sizeof(peer_profile));
		}
	;

peer_profile_statements
	:	/* empty */
	|	peer_profile_statements peer_profile_statement
	;

peer_profile_statement
	:	PROFILE_NAME EQUALS STRING
		{
			peer_profile.profile_name = $3.buf;
		}
	|	PEER_IPADDR EQUALS IPADDRESS
		{
			struct in_addr addr;
			addr.s_addr = htonl($3);
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_PEER_IPADDR;
			peer_profile.peer_addr.s_addr = addr.s_addr;
		}
	|	PEER_PORT EQUALS DECSTRING
		{
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_PEER_PORT;
			peer_profile.peer_port = $3;
		}
	|	NETMASK EQUALS IPADDRESS
		{
			struct in_addr addr;
			addr.s_addr = htonl($3);
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_NETMASK;
			peer_profile.netmask.s_addr = addr.s_addr;
		}
	|	LAC_LNS EQUALS STRING
		{
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_LACLNS;
			if (strcasecmp($3.buf, "lac") == 0) {
				peer_profile.we_can_be_lac = 1;
				peer_profile.we_can_be_lns = 0;
			} else if (strcasecmp($3.buf, "lns") == 0) {
				peer_profile.we_can_be_lac = 0;
				peer_profile.we_can_be_lns = 1;
			} else if ((strcasecmp($3.buf, "laclns") == 0) ||
				   (strcasecmp($3.buf, "lnslac") == 0)) {
				peer_profile.we_can_be_lac = 1;
				peer_profile.we_can_be_lns = 1;
			} else {
				yyfatal("invalid lac_lns value");
			}
		}
	|	TUNNEL_PROFILE_NAME EQUALS STRING
		{
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_TUNNEL_PROFILE_NAME;
			peer_profile.default_tunnel_profile_name.optstring_u.value = $3.buf;
			peer_profile.default_tunnel_profile_name.valid = 1;
		}
	|	SESSION_PROFILE_NAME EQUALS STRING
		{
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_SESSION_PROFILE_NAME;
			peer_profile.default_session_profile_name.optstring_u.value = $3.buf;
			peer_profile.default_session_profile_name.valid = 1;
		}
	|	PPP_PROFILE_NAME EQUALS STRING
		{
			peer_profile.flags |= L2TP_API_PEER_PROFILE_FLAG_PPP_PROFILE_NAME;
			peer_profile.default_ppp_profile_name.optstring_u.value = $3.buf;
			peer_profile.default_ppp_profile_name.valid = 1;
		}
	;

tunnel_profile_command
	:	tunnel_profile_create_command
	|	tunnel_profile_modify_command
	;

tunnel_profile_create_command
	:	TUNNEL PROFILE CREATE tunnel_profile_statements EOT
		{
			bool_t status;
			int result;
			if (tunnel_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_tunnel_profile_create_1_svc(tunnel_profile, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_PROFILE_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "tunnel profile create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&tunnel_profile, 0, sizeof(tunnel_profile));
		}
	;

tunnel_profile_modify_command
	:	TUNNEL PROFILE MODIFY tunnel_profile_statements EOT
		{
			bool_t status;
			int result;
			if (tunnel_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_tunnel_profile_modify_1_svc(tunnel_profile, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "tunnel profile modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&tunnel_profile, 0, sizeof(tunnel_profile));
		}
	;

tunnel_profile_statements
	:	/* empty */
	|	tunnel_profile_statements tunnel_profile_statement
	;

tunnel_profile_statement
	:	PROFILE_NAME EQUALS STRING
		{
			tunnel_profile.profile_name = $3.buf;
		}
	|	HIDE_AVPS EQUALS BOOL
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS;
			tunnel_profile.hide_avps = $3;
		}
	|	AUTH_MODE EQUALS STRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_AUTH_MODE;
			if (strcasecmp($3.buf, "none") == 0) {
				tunnel_profile.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_NONE;
			} else if (strcasecmp($3.buf, "simple") == 0) {
				tunnel_profile.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_SIMPLE;
			} else if (strcasecmp($3.buf, "challenge") == 0) {
				tunnel_profile.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE;
			} else {
				yyfatal("invalid auth_mode");
			}
		}
	|	FRAMING_CAP EQUALS STRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_FRAMING_CAP;
			if (strcasecmp($3.buf, "sync") == 0) {
				tunnel_profile.framing_cap_sync = 1;
				tunnel_profile.framing_cap_async = 0;
			} else if (strcasecmp($3.buf, "async") == 0) {
				tunnel_profile.framing_cap_sync = 0;
				tunnel_profile.framing_cap_async = 1;
			} else if (strcasecmp($3.buf, "any") == 0) {
				tunnel_profile.framing_cap_sync = 1;
				tunnel_profile.framing_cap_async = 1;
			} else {
				yyfatal("invalid framing_cap");
			}
		}
	|	BEARER_CAP EQUALS STRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_BEARER_CAP;
			if (strcasecmp($3.buf, "analog") == 0) {
				tunnel_profile.bearer_cap_analog = 1;
				tunnel_profile.bearer_cap_digital = 0;
			} else if (strcasecmp($3.buf, "digital") == 0) {
				tunnel_profile.bearer_cap_analog = 0;
				tunnel_profile.bearer_cap_digital = 1;
			} else if (strcasecmp($3.buf, "any") == 0) {
				tunnel_profile.bearer_cap_analog = 1;
				tunnel_profile.bearer_cap_digital = 1;
			} else {
				yyfatal("invalid bearer_cap");
			}
		}
	|	USE_TIEBREAKER EQUALS BOOL
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_USE_TIEBREAKER;
			tunnel_profile.use_tiebreaker = $3;
		}
	|	HELLO_TIMEOUT EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HELLO_TIMEOUT;
			tunnel_profile.hello_timeout = $3;
		}
	|	MAX_RETRIES EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_MAX_RETRIES;
			tunnel_profile.max_retries = $3;
		}
	|	RX_WINDOW_SIZE EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_RX_WINDOW_SIZE;
			tunnel_profile.rx_window_size = $3;
		}
	|	TX_WINDOW_SIZE EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_TX_WINDOW_SIZE;
			tunnel_profile.tx_window_size = $3;
		}
	|	RETRY_TIMEOUT EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_RETRY_TIMEOUT;
			tunnel_profile.retry_timeout = $3;
		}
	|	IDLE_TIMEOUT EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_IDLE_TIMEOUT;
			tunnel_profile.idle_timeout = $3;
		}
	|	SECRET EQUALS STRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_SECRET;
			tunnel_profile.secret.optstring_u.value = $3.buf;
			tunnel_profile.secret.valid = 1;
		}
	|	ALLOW_PPP_PROXY EQUALS BOOL
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_ALLOW_PPP_PROXY;
			tunnel_profile.allow_ppp_proxy = $3;
		}
	|	TRACE_FLAGS EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS;
			tunnel_profile.trace_flags = $3;
		}
	|	USE_UDP_CHECKSUMS EQUALS BOOL
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_USE_UDP_CHECKSUMS;
			tunnel_profile.use_udp_checksums = $3;
		}
	|	HOST_NAME EQUALS STRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_HOST_NAME;
			tunnel_profile.host_name.optstring_u.value = $3.buf;
			tunnel_profile.host_name.valid = 1;
		}
	|	MAX_SESSIONS EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_MAX_SESSIONS;
			tunnel_profile.max_sessions = $3;
		}
	|	SRC_IPADDR EQUALS IPADDRESS
		{
			struct in_addr addr;
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_OUR_ADDR;
			addr.s_addr = htonl($3);
			tunnel_profile.our_addr.s_addr = addr.s_addr;
		}
	|	DEST_IPADDR EQUALS IPADDRESS
		{
			struct in_addr addr;
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_ADDR;
			addr.s_addr = htonl($3);
			tunnel_profile.peer_addr.s_addr = addr.s_addr;
		}
	|	OUR_UDP_PORT EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_OUR_UDP_PORT;
			tunnel_profile.our_udp_port = $3;
		}
	|	PEER_UDP_PORT EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_UDP_PORT;
			tunnel_profile.peer_udp_port = $3;
		}
	|	PEER_PROFILE_NAME EQUALS STRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PEER_PROFILE_NAME;
			tunnel_profile.peer_profile_name.optstring_u.value = $3.buf;
			tunnel_profile.peer_profile_name.valid = 1;
		}
	|	SESSION_PROFILE_NAME EQUALS STRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_SESSION_PROFILE_NAME;
			tunnel_profile.session_profile_name.optstring_u.value = $3.buf;
			tunnel_profile.session_profile_name.valid = 1;
		}
	|	PPP_PROFILE_NAME EQUALS STRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_PPP_PROFILE_NAME;
			tunnel_profile.ppp_profile_name.optstring_u.value = $3.buf;
			tunnel_profile.ppp_profile_name.valid = 1;
		}
	|	DO_PMTU_DISCOVERY EQUALS BOOL
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_DO_PMTU_DISCOVERY;
			tunnel_profile.do_pmtu_discovery = $3;
		}
	|	MTU EQUALS DECSTRING
		{
			tunnel_profile.flags |= L2TP_API_TUNNEL_PROFILE_FLAG_MTU;
			tunnel_profile.mtu = $3;
		}
	;

session_profile_command
	:	session_profile_create_command
	|	session_profile_modify_command
	;

session_profile_create_command
	:	SESSION PROFILE CREATE session_profile_statements EOT
		{
			bool_t status;
			int result;
			if (session_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_session_profile_create_1_svc(session_profile, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_PROFILE_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "session profile create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&session_profile, 0, sizeof(session_profile));
		}
	;

session_profile_modify_command
	:	SESSION PROFILE MODIFY session_profile_statements EOT
		{
			bool_t status;
			int result;
			if (session_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_session_profile_modify_1_svc(session_profile, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "session profile modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&session_profile, 0, sizeof(session_profile));
		}
	;

session_profile_statements
	:	/* empty */
	|	session_profile_statements session_profile_statement
	;

session_profile_statement
	:	PROFILE_NAME EQUALS STRING
		{
			session_profile.profile_name = $3.buf;
		}
	|	TRACE_FLAGS EQUALS DECSTRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_TRACE_FLAGS;
			session_profile.trace_flags = $3;
		}
	|	SEQUENCING_REQUIRED EQUALS BOOL
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_SEQUENCING_REQUIRED;
			session_profile.sequencing_required = $3;
		}
	|	PPP_PROFILE_NAME EQUALS STRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_PPP_PROFILE_NAME;
			OPTSTRING(session_profile.ppp_profile_name) = $3.buf;
			session_profile.ppp_profile_name.valid = 1;
		}
	|	SESSION_TYPE EQUALS STRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_SESSION_TYPE;
			session_profile.flags |= L2TP_API_SESSION_FLAG_SESSION_TYPE;
			if (strcasecmp($3.buf, "laic") == 0) {
				session_profile.session_type = L2TP_API_SESSION_TYPE_LAIC;
			} else if (strcasecmp($3.buf, "laoc") == 0) {
				session_profile.session_type = L2TP_API_SESSION_TYPE_LAOC;
			} else if (strcasecmp($3.buf, "lnic") == 0) {
				session_profile.session_type = L2TP_API_SESSION_TYPE_LNIC;
			} else if (strcasecmp($3.buf, "lnoc") == 0) {
				session_profile.session_type = L2TP_API_SESSION_TYPE_LNOC;
			} else {
				yyfatal("Bad session type: expecting laic|laoc|lnic|lnoc");
			}
		}
	|	PRIV_GROUP_ID EQUALS STRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_PRIV_GROUP_ID;
			OPTSTRING(session_profile.priv_group_id) = $3.buf;
			session_profile.priv_group_id.valid = 1;
		}
	|	FRAMING_TYPE EQUALS STRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_FRAMING_TYPE;
			session_profile.flags |= L2TP_API_SESSION_FLAG_FRAMING_TYPE;
			if (strcasecmp($3.buf, "sync") == 0) {
				session_profile.framing_type_sync = 1;
				session_profile.framing_type_async = 0;
			} else if (strcasecmp($3.buf, "async") == 0) {
				session_profile.framing_type_sync = 0;
				session_profile.framing_type_async = 1;
			} else if (strcasecmp($3.buf, "any") == 0) {
				session_profile.framing_type_sync = 1;
				session_profile.framing_type_async = 1;
			} else {
				yyfatal("invalid framing_type");
			}
		}
	|	BEARER_TYPE EQUALS STRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_BEARER_TYPE;
			session_profile.flags |= L2TP_API_SESSION_FLAG_BEARER_TYPE;
			if (strcasecmp($3.buf, "analog") == 0) {
				session_profile.bearer_type_analog = 1;
				session_profile.bearer_type_digital = 0;
			} else if (strcasecmp($3.buf, "digital") == 0) {
				session_profile.bearer_type_analog = 0;
				session_profile.bearer_type_digital = 1;
			} else if (strcasecmp($3.buf, "any") == 0) {
				session_profile.bearer_type_analog = 1;
				session_profile.bearer_type_digital = 1;
			} else {
				yyfatal("invalid bearer_type");
			}
		}
	|	MINIMUM_BPS EQUALS DECSTRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_MINIMUM_BPS;
			session_profile.minimum_bps = $3;
		}
	|	MAXIMUM_BPS EQUALS DECSTRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_MAXIMUM_BPS;
			session_profile.maximum_bps = $3;
		}
	|	CONNECT_SPEED EQUALS STRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_CONNECT_SPEED;
			int ints[2];
			int num_matches;
			session_profile.flags |= L2TP_API_SESSION_FLAG_CONNECT_SPEED;
			num_matches = sscanf($3.buf, "%d:%d", &ints[0], &ints[1]);
			if (num_matches >= 1) {
				session_profile.rx_connect_speed = ints[0];
				session_profile.tx_connect_speed = ints[0];
				if (num_matches == 2) {
					session_profile.tx_connect_speed = ints[1];
				}
			} else {
				yyfatal("Expecting connect_speed[:tx_connect_speed]");
			}
		}
	|	USE_PPP_PROXY EQUALS BOOL
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_USE_PPP_PROXY;
			session_profile.use_ppp_proxy = $3;
		}
	|	USE_SEQUENCE_NUMBERS EQUALS BOOL
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_USE_SEQUENCE_NUMBERS;
			session_profile.use_sequence_numbers = $3;
		}
	|	NO_PPP EQUALS BOOL
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_NO_PPP;
			session_profile.no_ppp = $3;
		}
	|	REORDER_TIMEOUT EQUALS DECSTRING
		{
			session_profile.flags |= L2TP_API_SESSION_PROFILE_FLAG_REORDER_TIMEOUT;
			session_profile.reorder_timeout = $3;
		}
	;

ppp_profile_command
	:	ppp_profile_create_command
	|	ppp_profile_modify_command
	;

ppp_profile_create_command
	:	PPP PROFILE CREATE ppp_profile_statements EOT
		{
			bool_t status;
			int result;
			if (ppp_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_ppp_profile_create_1_svc(ppp_profile, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_PROFILE_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "ppp profile create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&ppp_profile, 0, sizeof(ppp_profile));
		}
	;

ppp_profile_modify_command
	:	PPP PROFILE MODIFY ppp_profile_statements EOT
		{
			bool_t status;
			int result;
			if (ppp_profile.profile_name == NULL)
				yyfatal("missing profile name");
			status = l2tp_ppp_profile_modify_1_svc(ppp_profile, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "ppp profile modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&ppp_profile, 0, sizeof(ppp_profile));
		}
	;

ppp_profile_statements
	:	/* empty */
	|	ppp_profile_statements ppp_profile_statement
	;

ppp_profile_statement
	:	PROFILE_NAME EQUALS STRING
		{
			ppp_profile.profile_name = $3.buf;
		}
	|	TRACE_FLAGS EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS;
			ppp_profile.trace_flags = $3;
		}
	|	ASYNCMAP EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_ASYNCMAP;
			ppp_profile.asyncmap = $3;
		}
	|	MRU EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_MRU;
			ppp_profile.mru = $3;
		}
	|	MTU EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_MTU;
			ppp_profile.mtu = $3;
		}
	|	USE_RADIUS EQUALS BOOL
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_USE_RADIUS;
			ppp_profile.use_radius = $3;
		}
	|	RADIUS_HINT EQUALS STRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_RADIUS_HINT;
			OPTSTRING(ppp_profile.radius_hint) = $3.buf;
			ppp_profile.radius_hint.valid = 1;
		}
	|	AUTH_PAP EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_PAP;
			if ($3) {
				ppp_profile.auth_refuse_pap = 0;
			} else {
				ppp_profile.auth_refuse_pap = -1;
			}
		}
	|	AUTH_CHAP EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_CHAP;
			if ($3) {
				ppp_profile.auth_refuse_chap = 0;
			} else {
				ppp_profile.auth_refuse_chap = -1;
			}
		}
	|	AUTH_MSCHAPV1 EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAP;
			if ($3) {
				ppp_profile.auth_refuse_mschap = 0;
			} else {
				ppp_profile.auth_refuse_mschap = -1;
			}
		}
	|	AUTH_MSCHAPV2 EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAPV2;
			if ($3) {
				ppp_profile.auth_refuse_mschapv2 = 0;
			} else {
				ppp_profile.auth_refuse_mschapv2 = -1;
			}
		}
	|	AUTH_EAP EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_EAP;
			if ($3) {
				ppp_profile.auth_refuse_eap = 0;
			} else {
				ppp_profile.auth_refuse_eap = -1;
			}
		}
	|	AUTH_NONE EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_NONE;
			if ($3) {
				ppp_profile.auth_none = -1;
			} else {
				ppp_profile.auth_none = 0;
			}
		}
	|	AUTH_PEER EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_AUTH_PEER;
			if ($3) {
				ppp_profile.auth_peer = -1;
			} else {
				ppp_profile.auth_peer = 0;
			}
		}
	|	SYNC_MODE EQUALS STRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_SYNC_MODE;
			if (strcasecmp($3.buf, "sync") == 0) {
				ppp_profile.sync_mode = L2TP_API_PPP_SYNCMODE_SYNC;
			} else if (strcasecmp($3.buf, "async") == 0) {
				ppp_profile.sync_mode = L2TP_API_PPP_SYNCMODE_ASYNC;
			} else if (strcasecmp($3.buf, "any") == 0) {
				ppp_profile.sync_mode = L2TP_API_PPP_SYNCMODE_SYNC_ASYNC;
			} else {
				yyfatal("Bad sync mode: expecting sync|async|any");
			}
		}
	|	CHAP_INTERVAL EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_CHAP_INTERVAL;
			ppp_profile.chap_interval = $3;
		}
	|	CHAP_MAX_CHALLENGE EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_CHAP_MAX_CHALLENGE;
			ppp_profile.chap_max_challenge = $3;
		}
	|	CHAP_RESTART EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_CHAP_RESTART;
			ppp_profile.chap_restart = $3;
		}
	|	PAP_MAX_AUTH_REQUESTS EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_PAP_MAX_AUTH_REQUESTS;
			ppp_profile.pap_max_auth_requests = $3;
		}
	|	PAP_RESTART_INTERVAL EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_PAP_RESTART_INTERVAL;
			ppp_profile.pap_restart_interval = $3;
		}
	|	PAP_TIMEOUT EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_PAP_TIMEOUT;
			ppp_profile.pap_timeout = $3;
		}
	|	IDLE_TIMEOUT EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IDLE_TIMEOUT;
			ppp_profile.idle_timeout = $3;
		}
	|	IPCP_MAX_CONFIG_REQUESTS EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_REQUESTS;
			ppp_profile.ipcp_max_config_requests = $3;
		}
	|	IPCP_MAX_CONFIG_NAKS EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_NAKS;
			ppp_profile.ipcp_max_config_naks = $3;
		}
	|	IPCP_MAX_TERMINATE_REQUESTS EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_TERMINATE_REQUESTS;
			ppp_profile.lcp_max_terminate_requests = $3;
		}
	|	IPCP_RETRANSMIT_INTERVAL EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_IPCP_RETRANSMIT_INTERVAL;
			ppp_profile.ipcp_retransmit_interval = $3;
		}
	|	LCP_ECHO_FAILURE_COUNT EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_FAILURE_COUNT;
			ppp_profile.lcp_echo_failure_count = $3;
		}
	|	LCP_ECHO_INTERVAL EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_INTERVAL;
			ppp_profile.lcp_echo_interval = $3;
		}
	|	LCP_MAX_CONFIG_REQUESTS EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_REQUESTS;
			ppp_profile.lcp_max_config_requests = $3;
		}
	|	LCP_MAX_CONFIG_NAKS EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_NAKS;
			ppp_profile.lcp_max_config_naks = $3;
		}
	|	LCP_MAX_TERMINATE_REQUESTS EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_TERMINATE_REQUESTS;
			ppp_profile.lcp_max_terminate_requests = $3;
		}
	|	LCP_RETRANSMIT_INTERVAL EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_LCP_RETRANSMIT_INTERVAL;
			ppp_profile.lcp_retransmit_interval = $3;
		}
	|	MAX_CONNECT_TIME EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_MAX_CONNECT_TIME;
			ppp_profile.max_connect_time = $3;
		}
	|	MAX_FAILURE_COUNT EQUALS DECSTRING
		{
			ppp_profile.flags |= L2TP_API_PPP_PROFILE_FLAG_MAX_FAILURE_COUNT;
			ppp_profile.max_failure_count = $3;
		}
	|	DNS_IPADDR_PRI EQUALS IPADDRESS
		{
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_1;
			addr.s_addr = htonl($3);
			ppp_profile.dns_addr_1.s_addr = addr.s_addr;
		}
	|	DNS_IPADDR_SEC EQUALS IPADDRESS
		{
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_2;
			addr.s_addr = htonl($3);
			ppp_profile.dns_addr_2.s_addr = addr.s_addr;
		}
	|	WINS_IPADDR_PRI EQUALS IPADDRESS
		{
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_1;
			addr.s_addr = htonl($3);
			ppp_profile.wins_addr_1.s_addr = addr.s_addr;
		}
	|	WINS_IPADDR_SEC EQUALS IPADDRESS
		{
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_2;
			addr.s_addr = htonl($3);
			ppp_profile.wins_addr_2.s_addr = addr.s_addr;
		}
	|	LOCAL_IPADDR EQUALS IPADDRESS
		{
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_LOCAL_IP_ADDR;
			addr.s_addr = htonl($3);
			ppp_profile.local_ip_addr.s_addr = addr.s_addr;
		}
	|	REMOTE_IPADDR EQUALS IPADDRESS
		{
			struct in_addr addr;
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_PEER_IP_ADDR;
			addr.s_addr = htonl($3);
			ppp_profile.peer_ip_addr.s_addr = addr.s_addr;
		}
	|	IP_POOL_NAME EQUALS STRING
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_IP_POOL_NAME;
			OPTSTRING(ppp_profile.ip_pool_name) = $3.buf;
			ppp_profile.ip_pool_name.valid = 1;
		}
	|	USE_AS_DEFAULT_ROUTE EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_USE_AS_DEFAULT_ROUTE;
			ppp_profile.use_as_default_route = $3;
		}
	|	MULTILINK EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_MULTILINK;
			ppp_profile.multilink = $3;
		}
	|	PROXY_ARP EQUALS BOOL
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_PROXY_ARP;
			ppp_profile.proxy_arp = $3;
		}
	|	LOCAL_NAME EQUALS STRING
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_LOCAL_NAME;
			OPTSTRING(ppp_profile.local_name) = $3.buf;
			ppp_profile.local_name.valid = 1;
		}
	|	REMOTE_NAME EQUALS STRING
		{
			ppp_profile.flags2 |= L2TP_API_PPP_PROFILE_FLAG_REMOTE_NAME;
			OPTSTRING(ppp_profile.remote_name) = $3.buf;
			ppp_profile.remote_name.valid = 1;
		}
	;

tunnel_command
	:	tunnel_create_command
	|	tunnel_modify_command
	;

tunnel_create_command
	:	TUNNEL CREATE tunnel_statements EOT
		{
			bool_t status;
			int result;
			status = l2tp_tunnel_create_1_svc(tunnel, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_TUNNEL_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "tunnel create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&tunnel, 0, sizeof(tunnel));
		}
	;

tunnel_modify_command
	:	TUNNEL MODIFY tunnel_statements EOT
		{
			bool_t status;
			int result;
			status = l2tp_tunnel_modify_1_svc(tunnel, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "tunnel modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&tunnel, 0, sizeof(tunnel));
		}
	;

tunnel_statements
	:	/* empty */
	|	tunnel_statements tunnel_statement
	;

tunnel_statement
	:	HIDE_AVPS EQUALS BOOL
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_HIDE_AVPS;
			tunnel.hide_avps = $3;
		}
	|	SRC_IPADDR EQUALS IPADDRESS
		{
			struct in_addr addr;
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_OUR_ADDR;
			addr.s_addr = htonl($3);
			tunnel.our_addr.s_addr = addr.s_addr;
		}
	|	DEST_IPADDR EQUALS IPADDRESS
		{
			struct in_addr addr;
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PEER_ADDR;
			addr.s_addr = htonl($3);
			tunnel.peer_addr.s_addr = addr.s_addr;
		}
	|	CONFIG_ID EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_CONFIG_ID;
			tunnel.config_id = $3;
		}
	|	OUR_UDP_PORT EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_OUR_UDP_PORT;
			tunnel.our_udp_port = $3;
		}
	|	AUTH_MODE EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_AUTH_MODE;
			if (strcasecmp($3.buf, "none") == 0) {
				tunnel.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_NONE;
			} else if (strcasecmp($3.buf, "simple") == 0) {
				tunnel.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_SIMPLE;
			} else if (strcasecmp($3.buf, "challenge") == 0) {
				tunnel.auth_mode = L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE;
			} else {
				yyfatal("invalid auth_mode");
			}
		}
	|	FRAMING_CAP EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_FRAMING_CAP;
			if (strcasecmp($3.buf, "sync") == 0) {
				tunnel.framing_cap_sync = 1;
				tunnel.framing_cap_async = 0;
			} else if (strcasecmp($3.buf, "async") == 0) {
				tunnel.framing_cap_sync = 0;
				tunnel.framing_cap_async = 1;
			} else if (strcasecmp($3.buf, "any") == 0) {
				tunnel.framing_cap_sync = 1;
				tunnel.framing_cap_async = 1;
			} else {
				yyfatal("invalid framing_cap");
			}
		}
	|	BEARER_CAP EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_BEARER_CAP;
			if (strcasecmp($3.buf, "analog") == 0) {
				tunnel.bearer_cap_analog = 1;
				tunnel.bearer_cap_digital = 0;
			} else if (strcasecmp($3.buf, "digital") == 0) {
				tunnel.bearer_cap_analog = 0;
				tunnel.bearer_cap_digital = 1;
			} else if (strcasecmp($3.buf, "any") == 0) {
				tunnel.bearer_cap_analog = 1;
				tunnel.bearer_cap_digital = 1;
			} else {
				yyfatal("invalid bearer_cap");
			}
		}
	|	USE_TIEBREAKER EQUALS BOOL
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_USE_TIEBREAKER;
			tunnel.use_tiebreaker = $3;
		}
	|	PERSIST EQUALS BOOL
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PERSIST;
			tunnel.persist = $3;
		}
	|	HELLO_TIMEOUT EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_HELLO_TIMEOUT;
			tunnel.hello_timeout = $3;
		}
	|	MAX_RETRIES EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_MAX_RETRIES;
			tunnel.max_retries = $3;
		}
	|	RX_WINDOW_SIZE EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_RX_WINDOW_SIZE;
			tunnel.rx_window_size = $3;
		}
	|	TX_WINDOW_SIZE EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_TX_WINDOW_SIZE;
			tunnel.tx_window_size = $3;
		}
	|	RETRY_TIMEOUT EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_RETRY_TIMEOUT;
			tunnel.retry_timeout = $3;
		}
	|	IDLE_TIMEOUT EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_IDLE_TIMEOUT;
			tunnel.idle_timeout = $3;
		}
	|	PEER_UDP_PORT EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PEER_UDP_PORT;
			tunnel.peer_udp_port = $3;
		}
	|	SECRET EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_SECRET;
			OPTSTRING(tunnel.secret) = $3.buf;
			tunnel.secret.valid = 1;
		}
	|	ALLOW_PPP_PROXY EQUALS BOOL
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_ALLOW_PPP_PROXY;
			tunnel.allow_ppp_proxy = $3;
		}
	|	TRACE_FLAGS EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_TRACE_FLAGS;
			tunnel.trace_flags = $3;
		}
	|	USE_UDP_CHECKSUMS EQUALS BOOL
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_USE_UDP_CHECKSUMS;
			tunnel.use_udp_checksums = $3;
		}
	|	HOST_NAME EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_HOST_NAME;
			OPTSTRING(tunnel.host_name) = $3.buf;
			tunnel.host_name.valid = 1;
		}
	|	TUNNEL_ID EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_TUNNEL_ID;
			tunnel.tunnel_id = $3;
		}
	|	PROFILE_NAME EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PROFILE_NAME;
			OPTSTRING(tunnel.tunnel_profile_name) = $3.buf;
			tunnel.tunnel_profile_name.valid = 1;
		}
	|	MAX_SESSIONS EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_MAX_SESSIONS;
			tunnel.max_sessions = $3;
		}
	|	TUNNEL_NAME EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_TUNNEL_NAME;
			OPTSTRING(tunnel.tunnel_name) = $3.buf;
			tunnel.tunnel_name.valid = 1;
		}
	|	PEER_PROFILE_NAME EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PEER_PROFILE_NAME;
			OPTSTRING(tunnel.peer_profile_name) = $3.buf;
			tunnel.peer_profile_name.valid = 1;
		}
	|	SESSION_PROFILE_NAME EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_SESSION_PROFILE_NAME;
			OPTSTRING(tunnel.session_profile_name) = $3.buf;
			tunnel.session_profile_name.valid = 1;
		}
	|	PPP_PROFILE_NAME EQUALS STRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_PPP_PROFILE_NAME;
			OPTSTRING(tunnel.ppp_profile_name) = $3.buf;
			tunnel.ppp_profile_name.valid = 1;
		}
	|	DO_PMTU_DISCOVERY EQUALS BOOL
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_DO_PMTU_DISCOVERY;
			tunnel.do_pmtu_discovery = $3;
		}
	|	MTU EQUALS DECSTRING
		{
			tunnel.flags |= L2TP_API_TUNNEL_FLAG_MTU;
			tunnel.mtu = $3;
		}
	|	INTERFACE_NAME EQUALS STRING
		{
			tunnel.flags2 |= L2TP_API_TUNNEL_FLAG_INTERFACE_NAME;
			OPTSTRING(tunnel.interface_name) = $3.buf;
			tunnel.interface_name.valid = 1;
		}
	;

session_command
	:	session_create_command
	|	session_modify_command
	;

session_create_command
	:	SESSION CREATE session_statements EOT
		{
			bool_t status;
			int result;
			status = l2tp_session_create_1_svc(session, &result, NULL);
			if ((status != TRUE) || ((result < 0) && (result != -L2TP_ERR_SESSION_ALREADY_EXISTS)))
				l2tp_log(LOG_ERR, "session create: command failed: rc=%d", l2tp_strerror(-result));

			memset(&session, 0, sizeof(session));
		}
	;

session_modify_command
	:	SESSION MODIFY session_statements EOT
		{
			bool_t status;
			int result;
			status = l2tp_session_modify_1_svc(session, &result, NULL);
			if ((status != TRUE) || (result < 0))
				l2tp_log(LOG_ERR, "session modify: command failed: rc=%d", l2tp_strerror(-result));

			memset(&session, 0, sizeof(session));
		}
	;

session_statements
	:	/* empty */
	|	session_statements session_statement
	;

session_statement
	:	TRACE_FLAGS EQUALS DECSTRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_TRACE_FLAGS;
			session.trace_flags = $3;
		}
	|	SEQUENCING_REQUIRED EQUALS BOOL
		{
			session.flags |= L2TP_API_SESSION_FLAG_SEQUENCING_REQUIRED;
			session.sequencing_required = $3;
		}
	|	PPP_PROFILE_NAME EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_PPP_PROFILE_NAME;
			OPTSTRING(session.ppp_profile_name) = $3.buf;
			session.ppp_profile_name.valid = 1;
		}
	|	SESSION_TYPE EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_SESSION_TYPE;
			if (strcasecmp($3.buf, "laic") == 0) {
				session.session_type = L2TP_API_SESSION_TYPE_LAIC;
			} else if (strcasecmp($3.buf, "laoc") == 0) {
				session.session_type = L2TP_API_SESSION_TYPE_LAOC;
			} else if (strcasecmp($3.buf, "lnic") == 0) {
				session.session_type = L2TP_API_SESSION_TYPE_LNIC;
			} else if (strcasecmp($3.buf, "lnoc") == 0) {
				session.session_type = L2TP_API_SESSION_TYPE_LNOC;
			} else {
				yyfatal("Bad session type: expecting laic|laoc|lnic|lnoc");
			}
		}
	|	PRIV_GROUP_ID EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_PRIV_GROUP_ID;
			OPTSTRING(session.priv_group_id) = $3.buf;
			session.priv_group_id.valid = 1;
		}
	|	FRAMING_TYPE EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_FRAMING_TYPE;
			if (strcasecmp($3.buf, "sync") == 0) {
				session.framing_type_sync = 1;
				session.framing_type_async = 0;
			} else if (strcasecmp($3.buf, "async") == 0) {
				session.framing_type_sync = 0;
				session.framing_type_async = 1;
			} else if (strcasecmp($3.buf, "any") == 0) {
				session.framing_type_sync = 1;
				session.framing_type_async = 1;
			} else {
				yyfatal("invalid framing_type");
			}
		}
	|	BEARER_TYPE EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_BEARER_TYPE;
			if (strcasecmp($3.buf, "analog") == 0) {
				session.bearer_type_analog = 1;
				session.bearer_type_digital = 0;
			} else if (strcasecmp($3.buf, "digital") == 0) {
				session.bearer_type_analog = 0;
				session.bearer_type_digital = 1;
			} else if (strcasecmp($3.buf, "any") == 0) {
				session.bearer_type_analog = 1;
				session.bearer_type_digital = 1;
			} else {
				yyfatal("invalid bearer_type");
			}
		}
	|	MINIMUM_BPS EQUALS DECSTRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_MINIMUM_BPS;
			session.minimum_bps = $3;
		}
	|	MAXIMUM_BPS EQUALS DECSTRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_MAXIMUM_BPS;
			session.maximum_bps = $3;
		}
	|	CONNECT_SPEED EQUALS STRING
		{
			int ints[2];
			int num_matches;
			session.flags |= L2TP_API_SESSION_FLAG_CONNECT_SPEED;
			num_matches = sscanf($3.buf, "%d:%d", &ints[0], &ints[1]);
			if (num_matches >= 1) {
				session.rx_connect_speed = ints[0];
				session.tx_connect_speed = ints[0];
				if (num_matches == 2) {
					session.tx_connect_speed = ints[1];
				}
			} else {
				yyfatal("Expecting connect_speed[:tx_connect_speed]");
			}
		}
	|	USE_PPP_PROXY EQUALS BOOL
		{
			session.flags |= L2TP_API_SESSION_FLAG_USE_PPP_PROXY;
			session.use_ppp_proxy = $3;
		}
	|	PROXY_AUTH_TYPE EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_TYPE;
			if (strcasecmp($3.buf, "text") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PLAIN_TEXT;
			} else if (strcasecmp($3.buf, "chap") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_CHAP;
			} else if (strcasecmp($3.buf, "pap") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_PAP;
			} else if (strcasecmp($3.buf, "none") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_NO_AUTH;
			} else if (strcasecmp($3.buf, "mschap") == 0) {
				session.proxy_auth_type = L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_MSCHAPV1;
			} else {
				yyfatal("Bad auth type: expecting none|text|chap|pap|mschap");
			}
		}
	|	PROXY_AUTH_NAME EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_NAME;
			OPTSTRING(session.proxy_auth_name) = $3.buf;
			session.proxy_auth_name.valid = 1;
		}
	|	PROXY_AUTH_CHALLENGE EQUALS HEXSTRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_CHALLENGE;
			session.proxy_auth_challenge.proxy_auth_challenge_len = $3.len;
			session.proxy_auth_challenge.proxy_auth_challenge_val = $3.buf;
		}
	|	PROXY_AUTH_RESPONSE EQUALS HEXSTRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_PROXY_AUTH_RESPONSE;
			session.proxy_auth_response.proxy_auth_response_len = $3.len;
			session.proxy_auth_response.proxy_auth_response_val = $3.buf;
		}
	|	CALLING_NUMBER EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_CALLING_NUMBER;
			OPTSTRING(session.calling_number) = $3.buf;
			session.calling_number.valid = 1;
		}
	|	CALLING_NUMBER EQUALS DECSTRING
		{
			char str[16];
			session.flags |= L2TP_API_SESSION_FLAG_CALLING_NUMBER;
			sprintf(str, "%lu", $3);
			OPTSTRING(session.calling_number) = strdup(str);
			session.calling_number.valid = 1;
		}
	|	CALLED_NUMBER EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_CALLED_NUMBER;
			OPTSTRING(session.called_number) = $3.buf;
			session.called_number.valid = 1;
		}
	|	CALLED_NUMBER EQUALS DECSTRING
		{
			char str[16];
			session.flags |= L2TP_API_SESSION_FLAG_CALLED_NUMBER;
			sprintf(str, "%lu", $3);
			OPTSTRING(session.called_number) = strdup(str);
			session.called_number.valid = 1;
		}
	|	SUB_ADDRESS EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_SUB_ADDRESS;
			OPTSTRING(session.sub_address) = $3.buf;
			session.sub_address.valid = 1;
		}
	|	SUB_ADDRESS EQUALS DECSTRING
		{
			char str[16];
			session.flags |= L2TP_API_SESSION_FLAG_SUB_ADDRESS;
			sprintf(str, "%lu", $3);
			OPTSTRING(session.sub_address) = strdup(str);
			session.sub_address.valid = 1;
		}
	|	INITIAL_RCVD_LCP_CONFREQ EQUALS HEXSTRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_INITIAL_RCVD_LCP_CONFREQ;
			session.initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_len = $3.len;
			session.initial_rcvd_lcp_confreq.initial_rcvd_lcp_confreq_val = $3.buf;
		}
	|	LAST_SENT_LCP_CONFREQ EQUALS HEXSTRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_LAST_SENT_LCP_CONFREQ;
			session.last_sent_lcp_confreq.last_sent_lcp_confreq_len = $3.len;
			session.last_sent_lcp_confreq.last_sent_lcp_confreq_val = $3.buf;
		}
	|	LAST_RCVD_LCP_CONFREQ EQUALS HEXSTRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_LAST_RCVD_LCP_CONFREQ;
			session.last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_len = $3.len;
			session.last_rcvd_lcp_confreq.last_rcvd_lcp_confreq_val = $3.buf;
		}
	|	USER_NAME EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_USER_NAME;
			OPTSTRING(session.user_name) = $3.buf;
			session.user_name.valid = 1;
		}
	|	USER_PASSWORD EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_USER_PASSWORD;
			OPTSTRING(session.user_password) = $3.buf;
			session.user_password.valid = 1;
		}
	|	USE_SEQUENCE_NUMBERS EQUALS BOOL
		{
			session.flags |= L2TP_API_SESSION_FLAG_USE_SEQUENCE_NUMBERS;
			session.use_sequence_numbers = $3;
		}
	|	NO_PPP EQUALS BOOL
		{
			session.flags |= L2TP_API_SESSION_FLAG_NO_PPP;
			session.no_ppp = $3;
		}
	|	REORDER_TIMEOUT EQUALS DECSTRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_REORDER_TIMEOUT;
			session.reorder_timeout = $3;
		}
	|	TUNNEL_NAME EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_TUNNEL_NAME;
			OPTSTRING(session.tunnel_name) = $3.buf;
			session.tunnel_name.valid = 1;
		}
	|	TUNNEL_ID EQUALS DECSTRING
		{
			session.tunnel_id = $3;
		}
	|	SESSION_NAME EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_SESSION_NAME;
			OPTSTRING(session.session_name) = $3.buf;
			session.session_name.valid = 1;
		}
	|	SESSION_ID EQUALS DECSTRING
		{
			session.session_id = $3;
		}
	|	INTERFACE_NAME EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_INTERFACE_NAME;
			OPTSTRING(session.interface_name) = $3.buf;
			session.interface_name.valid = 1;
		}
	|	PROFILE_NAME EQUALS STRING
		{
			session.flags |= L2TP_API_SESSION_FLAG_PROFILE_NAME;
			OPTSTRING(session.profile_name) = $3.buf;
			session.profile_name.valid = 1;
		}
	;

%%

