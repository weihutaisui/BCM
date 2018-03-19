%{

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>

#include "l2tp_config_types.h"
#include "l2tp_config_parse.h"

void yyfatal(const char *s);
void yyerror(const char *s);

int lineno;
int command;

%}

/* common section */
nl		\n
ws		[ \t]+
digit		[0-9]
letter		[0-9A-Za-z]
hexdigit	[0-9A-Fa-f]
dot		\.
hyphen		\-
underscore	\_
slash		\/
backslash	\\
semi		\;
equals		\=
yes		yes
no		no
on		on
off		off
comment		\#.*
quotedstring	\"[^"]*\"
decstring	{digit}+
hexstring	(0x|0X){hexdigit}+
ipaddress	{decstring}\.{decstring}\.{decstring}\.{decstring}
name		{letter}(({letter}|{digit}|{hyphen}|{underscore})*({letter}|{digit}))*
hostname	{name}(({dot}{name})+{dot}?)?
bool		yes|no|on|off
linecont	{backslash}{nl}

%%

system		{ command = 1; return(SYSTEM); }
tunnel		{ command = 1; return(TUNNEL); }
session		{ command = 1; return(SESSION); }
profile		{ command = 1; return(PROFILE); }
peer		{ command = 1; return(PEER); }
ppp		{ command = 1; return(PPP); }

create		{ return(CREATE); }
modify		{ return(MODIFY); }

udp_port				{ return(UDP_PORT); }
trace_flags				{ return(TRACE_FLAGS); }
max_tunnels				{ return(MAX_TUNNELS); }
max_sessions				{ return(MAX_SESSIONS); }
drain_tunnels				{ return(DRAIN_TUNNELS); }
tunnel_establish_timeout		{ return(TUNNEL_ESTABLISH_TIMEOUT); }
session_establish_timeout		{ return(SESSION_ESTABLISH_TIMEOUT); }
tunnel_persist_pend_timeout		{ return(TUNNEL_PERSIST_PEND_TIMEOUT); }
session_persist_pend_timeout		{ return(SESSION_PERSIST_PEND_TIMEOUT); }
deny_local_tunnel_creates		{ return(DENY_LOCAL_TUNNEL_CREATES); }
deny_remote_tunnel_creates		{ return(DENY_REMOTE_TUNNEL_CREATES); }

src_ipaddr				{ return(SRC_IPADDR); }
dest_ipaddr				{ return(DEST_IPADDR); }
peer_ipaddr				{ return(PEER_IPADDR); }
lac_lns					{ return(LAC_LNS); }

tunnel_id				{ return(TUNNEL_ID); }
session_id				{ return(SESSION_ID); }
tunnel_name				{ return(TUNNEL_NAME); }
session_name				{ return(SESSION_NAME); }
hide_avps				{ return(HIDE_AVPS); }
our_addr				{ return(OUR_ADDR); }
peer_addr				{ return(PEER_ADDR); }
our_udp_port				{ return(OUR_UDP_PORT); }
peer_udp_port				{ return(PEER_UDP_PORT); }
config_id				{ return(CONFIG_ID); }
auth_mode				{ return(AUTH_MODE); }
framing_cap				{ return(FRAMING_CAP); }
bearer_cap				{ return(BEARER_CAP); }
use_tiebreaker				{ return(USE_TIEBREAKER); }
allow_ppp_proxy				{ return(ALLOW_PPP_PROXY); }
use_ppp_proxy				{ return(USE_PPP_PROXY); }
use_udp_checksums			{ return(USE_UDP_CHECKSUMS); }
hello_timeout				{ return(HELLO_TIMEOUT); }
max_retries				{ return(MAX_RETRIES); }
rx_window_size				{ return(RX_WINDOW_SIZE); }
tx_window_size				{ return(TX_WINDOW_SIZE); }
retry_timeout				{ return(RETRY_TIMEOUT); }
idle_timeout				{ return(IDLE_TIMEOUT); }
do_pmtu_discovery			{ return(DO_PMTU_DISCOVERY); }
persist					{ return(PERSIST); }
mtu					{ return(MTU); }
secret					{ return(SECRET); }
host_name				{ return(HOST_NAME); }
tunnel_profile_name			{ return(TUNNEL_PROFILE_NAME); }
peer_profile_name			{ return(PEER_PROFILE_NAME); }
session_profile_name			{ return(SESSION_PROFILE_NAME); }
ppp_profile_name			{ return(PPP_PROFILE_NAME); }
interface_name				{ return(INTERFACE_NAME); }

session_type				{ return(SESSION_TYPE); }
user_name				{ return(USER_NAME); }
user_password				{ return(USER_PASSWORD); }
priv_group_id				{ return(PRIV_GROUP_ID); }
profile_name				{ return(PROFILE_NAME); }
sequencing_required			{ return(SEQUENCING_REQUIRED); }
use_sequence_numbers			{ return(USE_SEQUENCE_NUMBERS); }
no_ppp					{ return(NO_PPP); }
reorder_timeout				{ return(REORDER_TIMEOUT); }
framing_type				{ return(FRAMING_TYPE); }
bearer_type				{ return(BEARER_TYPE); }
minimum_bps				{ return(MINIMUM_BPS); }
maximum_bps				{ return(MAXIMUM_BPS); }
connect_speed				{ return(CONNECT_SPEED); }
peer_port				{ return(PEER_PORT); }
we_can_be_lac				{ return(WE_CAN_BE_LAC); }
we_can_be_lns				{ return(WE_CAN_BE_LNS); }
default_tunnel_profile_name		{ return(DEFAULT_TUNNEL_PROFILE_NAME); }
default_session_profile_name		{ return(DEFAULT_SESSION_PROFILE_NAME); }
default_ppp_profile_name		{ return(DEFAULT_PPP_PROFILE_NAME); }
netmask					{ return(NETMASK); }

initial_rcvd_lcp_confreq		{ return(INITIAL_RCVD_LCP_CONFREQ); }
calling_number				{ return(CALLING_NUMBER); }
proxy_auth_name				{ return(PROXY_AUTH_NAME); }
sub_address				{ return(SUB_ADDRESS); }
proxy_auth_type				{ return(PROXY_AUTH_TYPE); }
proxy_auth_response			{ return(PROXY_AUTH_RESPONSE); }
last_rcvd_lcp_confreq			{ return(LAST_RCVD_LCP_CONFREQ); }
called_number				{ return(CALLED_NUMBER); }
proxy_auth_challenge			{ return(PROXY_AUTH_CHALLENGE); }
last_sent_lcp_confreq			{ return(LAST_SENT_LCP_CONFREQ); }

asyncmap				{ return(ASYNCMAP); }
mru					{ return(MRU); }
auth_flags				{ return(AUTH_FLAGS); }
sync_mode				{ return(SYNC_MODE); }
chap_interval				{ return(CHAP_INTERVAL); }
chap_max_challenge			{ return(CHAP_MAX_CHALLENGE); }
chap_restart				{ return(CHAP_RESTART); }
pap_max_auth_requests			{ return(PAP_MAX_AUTH_REQUESTS); }
pap_restart_interval			{ return(PAP_RESTART_INTERVAL); }
pap_timeout				{ return(PAP_TIMEOUT); }
ipcp_max_config_requests		{ return(IPCP_MAX_CONFIG_REQUESTS); }
ipcp_max_config_naks			{ return(IPCP_MAX_CONFIG_NAKS); }
ipcp_max_terminate_requests		{ return(IPCP_MAX_TERMINATE_REQUESTS); }
ipcp_retransmit_interval		{ return(IPCP_RETRANSMIT_INTERVAL); }
lcp_echo_failure_count			{ return(LCP_ECHO_FAILURE_COUNT); }
lcp_echo_interval			{ return(LCP_ECHO_INTERVAL); }
lcp_max_config_requests			{ return(LCP_MAX_CONFIG_REQUESTS); }
lcp_max_config_naks			{ return(LCP_MAX_CONFIG_NAKS); }
lcp_max_terminate_requests		{ return(LCP_MAX_TERMINATE_REQUESTS); }
lcp_retransmit_interval			{ return(LCP_RETRANSMIT_INTERVAL); }
max_connect_time			{ return(MAX_CONNECT_TIME); }
max_failure_count			{ return(MAX_FAILURE_COUNT); }
local_ipaddr				{ return(LOCAL_IPADDR); }
remote_ipaddr				{ return(REMOTE_IPADDR); }
dns_ipaddr_pri				{ return(DNS_IPADDR_PRI); }
dns_ipaddr_sec				{ return(DNS_IPADDR_SEC); }
wins_ipaddr_pri				{ return(WINS_IPADDR_PRI); }
wins_ipaddr_sec				{ return(WINS_IPADDR_SEC); }
ip_pool_name				{ return(IP_POOL_NAME); }
use_radius				{ return(USE_RADIUS); }
radius_hint				{ return(RADIUS_HINT); }
multilink				{ return(MULTILINK); }
proxy_arp				{ return(PROXY_ARP); }
local_name				{ return(LOCAL_NAME); }
remote_name				{ return(REMOTE_NAME); }
default_route				{ return(USE_AS_DEFAULT_ROUTE); }
auth_pap				{ return(AUTH_PAP); }
auth_chap				{ return(AUTH_CHAP); }
auth_mschapv1				{ return(AUTH_MSCHAPV1); }
auth_mschapv2				{ return(AUTH_MSCHAPV2); }
auth_eap				{ return(AUTH_EAP); }
auth_none				{ return(AUTH_NONE); }
auth_peer				{ return(AUTH_PEER); }

{ws}		{ }
{linecont}	{ lineno++; }
{comment}	{ }
{semi}		{ command = 0; return(EOT); }
{equals}	{ return(EQUALS); }
{slash}		{ return SLASH; }
{nl}		{
			lineno++; 
			if (command) {
				command = 0;
				return EOT;
			}
		}

{bool}		{
			if ((strcasecmp(yytext, "yes") == 0) ||
			    (strcasecmp(yytext, "on") == 0) ||
			    (strcasecmp(yytext, "1") == 0)) {
				yylval.num = 1;
			} else if ((strcasecmp(yytext, "no") == 0) ||
				   (strcasecmp(yytext, "off") == 0) ||
				   (strcasecmp(yytext, "0") == 0)) {
				yylval.num = 0;
			} else {
				yylval.num = atoi(yytext);
			}
			return(BOOL);
		}

{decstring}	{
			char *bp;

			yylval.ulnum = strtoul(yytext, &bp, 10);
			return(DECSTRING);
		}

{hexstring}	{
			unsigned char *buf;
			int ret = 0;
			const char *pos = yytext + 2;
			int slen;
			int count = 0;
			int tmpval;

			yylval.val.len = strlen(yytext + 2) / 2;
			yylval.val.buf = malloc(yylval.val.len + 2 /* slack */);
			if (yylval.val.buf == NULL) {
				yyfatal("out of memory");
			}

			slen = strlen(yytext + 2);
			if (slen & 1) {
				yyfatal("odd number of hex characters cannot be converted");
			}

			buf = yylval.val.buf;
			for (count = 0; count < slen / 2; count++) {
				ret = sscanf(pos, "%02x", &tmpval);
				if (ret != 1) {
					yyfatal("can't parse hex digits");
				}
				*buf++ = tmpval;
				pos += 2;
			} 

			return(HEXSTRING);
		}

{quotedstring}	{
			char *p = yytext;
			while (*++p != '"') ;
			*p = '\0';
			yytext++;
			yylval.val.len = yyleng - 2;
			yylval.val.buf = (void *) strdup(yytext);
			if (!yylval.val.buf)
				yyfatal("insufficient memory");

			return(QUOTEDSTRING);
		}

{ipaddress}	{
			struct in_addr addr;
			if (!inet_aton(yytext, &addr))
				yyfatal("invalid IP address");
			yylval.ulnum = ntohl(addr.s_addr);
			return(IPADDRESS);
		}

[A-Za-z0-9:%\.\-\_/@+,\!]+ {
			yylval.val.len = yyleng;
			yylval.val.buf = (void *) strdup(yytext);
			if (!yylval.val.buf)
				yyfatal("insufficient memory");
			return(STRING);
		}

.		{
			yyfatal("Syntax error");
		}

%%

/*****************************************************************************
 * Parser interface API
 *****************************************************************************/

void yyfatal(const char *s)
{
	yyerror(s);
	exit(1);
}

void yyerror(const char *s)
{
	l2tp_log(LOG_ERR, "parse error: line %d: %s at [%s]", lineno, s, yytext);
}

extern int yyparse(void);

int l2tp_config_parse(FILE **fp)
{
	yyin = *fp;
	lineno = 1;
	command = 0;
	if (yyparse()) {
		l2tp_log(LOG_ERR, "parse failed, line %d", lineno);
		return -1;
	}

	return 0;;
}
