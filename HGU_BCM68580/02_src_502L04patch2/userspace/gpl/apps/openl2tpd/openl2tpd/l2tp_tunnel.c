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

#include "md5.h"
#include "usl.h"
#include "l2tp_private.h"
#include "hash.h"

#define L2TP_TUNNEL_MAX_CONFIGID		100	/* limits the number of tunnels between the same two IP hosts */
#define L2TP_TUNNEL_ESTABLISH_TIMEOUT		120 	/* 2 minutes */
#define L2TP_TUNNEL_PERSIST_PEND_TIMEOUT	300	/* 5 minutes */
#define L2TP_TUNNEL_MAX_COUNT			0 	/* unlimited */

#define L2TP_TUNNEL_HASH_BITS			5
#define L2TP_TUNNEL_HASH_SIZE			(1 << L2TP_TUNNEL_HASH_BITS)

/* Each tunnel is represented by an instance of this structure. 
 * Each tunnel lives on 3 lists:-
 * list       - regular unsorted list, used when we need to do something
 *	        for all lists
 * id_hlist   - hashed by tunnel_id
 * name_hlist - hashed by name, not used if tunnel isn't given a name
 *
 * All configuration parameters are stored here, as well as status
 * information from the peer.
 */
struct l2tp_tunnel {
	struct usl_list_head		list;					/* unordered list of tunnels */
	struct usl_hlist_node		id_hlist;				/* our list of tunnels hashed by id */
	struct usl_hlist_node		name_hlist;				/* our list of tunnels hashed by name */
	struct usl_list_head		session_list;				/* list of sessions on this tunnel */
	struct usl_hlist_head		session_id_hlist[L2TP_TUNNEL_HASH_SIZE]; /* hashed list of sessions, hashed by id */
	struct usl_fsm_instance 	fsmi;					/* state machine data */
	struct l2tp_peer		*my_peer;
	void				*setup_timer;
	void				*cleanup_timer;
	int				hold:1;					/* suppress context delete (for post analysis) */
	int				up:1; 					/* debounce tunnel_up event */
	int				down:1;					/* debounce tunnel_down event */
	int				close_acked:1;				/* peer has acknowledged close */
	int				sccrq_sent:1;				/* for peer collision detection */
	int				we_can_be_lac:1;
	int				we_can_be_lns:1;
	int				fd_is_connected:1;
	int				fd;					/* UDP socket fd */
	void				*xprt;					/* transport handle */
	/* Config data, set up by management or by profiles. Most are
	 * derived from RFC2661. 
	 */
	struct {
		uint32_t			flags;				/* remembers which fields have been set */
		uint32_t			flags2;
		struct sockaddr_in		local_addr;
		struct sockaddr_in		peer_addr;
		int				config_id;
		int				hide_avps;
		char				*secret;
		int				secret_len;
		enum l2tp_api_tunnel_auth_mode	auth_mode;
		char				*host_name;
		int				host_name_len;
		int				framing_cap_sync:1;
		int				framing_cap_async:1;
		int				bearer_cap_digital:1;
		int				bearer_cap_analog:1;
		int				use_tiebreaker:1;
		int				allow_ppp_proxy:1;
		int				use_udp_checksums:1;
		int				do_pmtu_discovery:1;
		int				persist:1;			/* recreate if tunnel fails? */
		int				mtu;
		int				hello_timeout;
		int				max_retries;
		uint16_t			rx_window_size;
		uint16_t			tx_window_size;
		int				retry_timeout;
		int				idle_timeout;
		int				trace_flags;			/* controls debug trace */
		int				max_sessions;
		char				*peer_profile_name;
		char				*tunnel_profile_name;
		char				*session_profile_name;
		char				*ppp_profile_name;
		char				*tunnel_name;			/* assigned by operator */
		char				*interface_name;		/* not yet used */
	} config;
	/* status info, not set up by management or profiles */
	struct {
		uint16_t			tunnel_id;			/* assigned tunnel id */
		int				created_by_admin;
		int				num_sessions;
		int				use_count;			/* see _inc_use_count(), _dec_use_count() */
		enum l2tp_api_tunnel_mode	mode;
		int				num_establish_retries;		/* counts setup attempts */
		struct l2tp_avp_tiebreaker	tiebreaker;
		int				have_tiebreaker;
		int				actual_tx_window_size;		/* negotiated with peer */
		char				*create_time;
	} status;
	/* Stored info from the tunnel peer */
	struct {
		struct l2tp_avp_host_name		*host_name;
		struct l2tp_avp_protocol_version	protocol_version;
		struct l2tp_avp_tunnel_id		tunnel_id;
		struct l2tp_avp_framing_cap		framing_cap;
		struct l2tp_avp_bearer_cap		bearer_cap;
		struct l2tp_avp_rx_window_size		rx_window_size;
		struct l2tp_avp_challenge		*challenge;
		int					challenge_len;
		struct l2tp_avp_challenge_response	*challenge_response;
		struct l2tp_avp_tiebreaker		tiebreaker;
		int					have_tiebreaker;
		struct l2tp_avp_firmware_revision	firmware_revision;
		struct l2tp_avp_vendor_name		*vendor_name;
		struct l2tp_avp_result_code		*result_code;
		int					result_code_len;
	} peer;
	/* Temporary state, filled in by FSM actions */
	struct l2tp_avp_challenge			*my_challenge;
	int						my_challenge_len;
	struct l2tp_avp_result_code			*result_code;		/* we send this info to peer when closing tunnels */
	int						result_code_len;
};

/* Tunnel profiles are named sets of tunnel configuration
 * parameters. 
 */
struct l2tp_tunnel_profile {
	struct usl_list_head				list;
	char						*profile_name;
	uint32_t					flags;			/* remembers which fields have been set */
	struct in_addr					our_addr;
	struct in_addr					peer_addr;
	uint16_t					our_udp_port;
	uint16_t					peer_udp_port;
	int						hide_avps;
	char						*secret;
	int						secret_len;
	enum l2tp_api_tunnel_auth_mode			auth_mode;
	char						*host_name;
	int						host_name_len;
	int						framing_cap_sync:1;
	int						framing_cap_async:1;
	int						bearer_cap_digital:1;
	int						bearer_cap_analog:1;
	int						use_tiebreaker:1;
	int						allow_ppp_proxy:1;
	int						use_udp_checksums:1;
	int						do_pmtu_discovery:1;
	int						mtu;
	int						hello_timeout;
	int						max_retries;
	uint16_t					rx_window_size;
	uint16_t					tx_window_size;
	int						retry_timeout;
	int						idle_timeout;
	int						trace_flags;
	int						max_sessions;
	char						*peer_profile_name;
	char						*session_profile_name;
	char						*ppp_profile_name;
};

static void l2tp_tunnel_recreate(struct l2tp_tunnel *tunnel);
static int l2tp_tunnel_clone(struct l2tp_tunnel *tunnel, struct l2tp_api_tunnel_msg_data *result);
static int l2tp_tunnel_param_defaults(struct l2tp_tunnel *tunnel, char *tunnel_profile_name, char *peer_profile_name);
static void l2tp_tunnel_deleted_ind(struct l2tp_tunnel *tunnel);
static int l2tp_tunnel_is_ok_to_create(struct l2tp_tunnel *tunnel, int local_request);
static void l2tp_tunnel_idle_timeout(void *arg);

/* Hooks, overridable by plugins */
int (*l2tp_tunnel_created_hook)(uint16_t tunnel_id) = NULL;
int (*l2tp_tunnel_deleted_hook)(uint16_t tunnel_id) = NULL;
int (*l2tp_tunnel_modified_hook)(uint16_t tunnel_id) = NULL;
int (*l2tp_tunnel_up_hook)(uint16_t tunnel_id, uint16_t peer_tunnel_id) = NULL;
int (*l2tp_tunnel_down_hook)(uint16_t tunnel_id) = NULL;

/* Local data */

static struct l2tp_tunnel_profile *l2tp_tunnel_defaults;
static int l2tp_tunnel_count;
static int l2tp_tunnel_event_pipe[2] = { -1, -1 };

static USL_LIST_HEAD(l2tp_tunnel_list);
static USL_LIST_HEAD(l2tp_tunnel_profile_list);

/* Hashed tunnel list.
 * We keep separate lists for searching by tunnel id or name.
 *
 * Although openl2tpd uses random numbers for its tunnel and session
 * ids, some other L2TP implementations (most notably Microsoft) do
 * not. So we still hash the id rather than using a bitmask, even
 * though in some cases they are random.
 */
static struct usl_hlist_head l2tp_tunnel_id_list[L2TP_TUNNEL_HASH_SIZE];
static struct usl_hlist_head l2tp_tunnel_name_list[L2TP_TUNNEL_HASH_SIZE];

static inline struct usl_hlist_head *l2tp_tunnel_id_hash(uint16_t tunnel_id)
{
	unsigned long hash_val = (unsigned long) tunnel_id;
	return &l2tp_tunnel_id_list[hash_long(hash_val, L2TP_TUNNEL_HASH_BITS)];
}

static inline struct usl_hlist_head *l2tp_tunnel_name_hash(const char *tunnel_name)
{
	unsigned hash = usl_hash_full_name((unsigned char *) tunnel_name, strlen(tunnel_name));
	return &l2tp_tunnel_name_list[hash & (L2TP_TUNNEL_HASH_SIZE - 1)];
}

static inline struct usl_hlist_head *l2tp_tunnel_session_id_hash(struct l2tp_tunnel *tunnel, uint16_t session_id)
{
	unsigned long hash_val = (unsigned long) session_id;
	return &tunnel->session_id_hlist[hash_long(hash_val, L2TP_TUNNEL_HASH_BITS)];
}

/* These may be changed by the "system modify" command */
static int l2tp_tunnel_max_count;
static int l2tp_tunnel_drain;
static int l2tp_tunnel_establish_timeout;
static int l2tp_tunnel_deny_local_creates;
static int l2tp_tunnel_deny_remote_creates;
static int l2tp_tunnel_persist_pend_timeout;

/* If we need to set a result code but we run out of memory, use
 * this preallocated one.
 */
#define L2TP_API_TUNNEL_EMERG_RESULT_CODE_SIZE		128
static struct l2tp_avp_result_code *l2tp_tunnel_emergency_result_code;

#undef RESC
#undef ERRC
#define RESC(x)	L2TP_AVP_RESULT_STOPCCN_##x
#define ERRC(x)	L2TP_AVP_ERROR_##x

/* A translation table for converting STOPCCN result codes into
 * English text. See RFC2661.
 */
static const struct l2tp_result_codes l2tp_tunnel_stopccn_result_codes[] = {
	{ RESC(RESERVED),	ERRC(NO_ERROR),		"Reserved" },
	{ RESC(NORMAL_STOP),	ERRC(NO_ERROR),		"General request to clear control connection" },
	{ RESC(GENERAL_ERROR),	ERRC(NO_ERROR),		"No general error" },
	{ RESC(GENERAL_ERROR),	ERRC(NO_TUNNEL_YET),	"No control connection exists yet for this LAC-LNS pair" },
	{ RESC(GENERAL_ERROR),	ERRC(BAD_LENGTH),	"Length is wrong" },
	{ RESC(GENERAL_ERROR),	ERRC(BAD_VALUE),	"One of the field values was out of range or reserved field was non-zero" },
	{ RESC(GENERAL_ERROR),	ERRC(NO_RESOURCE),	"Insufficient resources to handle this operation now" },
	{ RESC(GENERAL_ERROR),	ERRC(BAD_SESSION_ID),	"The Session ID is invalid in this context" },
	{ RESC(GENERAL_ERROR),	ERRC(VENDOR_ERROR),	"A generic vendor-specific error occurred in the LAC" },
	{ RESC(GENERAL_ERROR),	ERRC(TRY_ANOTHER),	"Try another" },
	{ RESC(GENERAL_ERROR),	ERRC(MBIT_SHUTDOWN),	"Unknown mandatory AVP received" },
	{ RESC(ALREADY_EXISTS),	ERRC(NO_ERROR),		"Control channel already exists" },
	{ RESC(AUTH_FAILED),	ERRC(NO_ERROR),		"Requester is not authorized to establish a control channel" },
	{ RESC(BAD_PROTOCOL),	ERRC(NO_ERROR),		"The protocol version of the requester is not supported" },
	{ RESC(BEING_SHUTDOWN),	ERRC(NO_ERROR),		"Requester is being shut down" },
	{ RESC(STATE_ERROR),	ERRC(NO_ERROR),		"Finite State Machine error" },
	{ -1, -1, NULL },
};

/*****************************************************************************
 * Public interface
 *****************************************************************************/

/* A bunch of access functions allowing external modules (including
 * plugins) access to tunnel info. External modules should increase
 * the tunnel reference count using l2tp_tunnel_inc_use_count() while
 * they hold a pointer to a tunnel instance.
 */
struct usl_list_head *l2tp_tunnel_session_list(struct l2tp_tunnel *tunnel)
{
	return &tunnel->session_list;
}

struct usl_hlist_head *l2tp_tunnel_session_id_hlist(struct l2tp_tunnel *tunnel, uint16_t session_id)
{
	return l2tp_tunnel_session_id_hash(tunnel, session_id);
}

uint16_t l2tp_tunnel_id(struct l2tp_tunnel const *tunnel)
{
	return tunnel->status.tunnel_id;
}

uint16_t l2tp_tunnel_peer_id(struct l2tp_tunnel const *tunnel)
{
	return tunnel->peer.tunnel_id.value;
}

int l2tp_tunnel_get_fd(struct l2tp_tunnel const *tunnel)
{
	return tunnel->fd;
}

int l2tp_tunnel_is_lac(struct l2tp_tunnel const *tunnel)
{
	return tunnel->status.mode == L2TP_API_TUNNEL_MODE_LAC;
}

int l2tp_tunnel_is_lns(struct l2tp_tunnel const *tunnel)
{
	return tunnel->status.mode == L2TP_API_TUNNEL_MODE_LNS;
}

int l2tp_tunnel_can_be_lac(struct l2tp_tunnel const *tunnel)
{
	return tunnel->we_can_be_lac;
}

int l2tp_tunnel_can_be_lns(struct l2tp_tunnel const *tunnel)
{
	return tunnel->we_can_be_lns;
}

int l2tp_tunnel_is_persistent(struct l2tp_tunnel const *tunnel)
{
	return tunnel->config.persist;
}

int l2tp_tunnel_is_fd_connected(struct l2tp_tunnel const *tunnel)
{
	return tunnel->fd_is_connected;
}

int l2tp_tunnel_is_created_by_admin(struct l2tp_tunnel const *tunnel)
{
	return tunnel->status.created_by_admin;
}

struct l2tp_xprt *l2tp_tunnel_get_xprt(struct l2tp_tunnel const *tunnel)
{
	return tunnel->xprt;
}

int l2tp_tunnel_get_mtu(struct l2tp_tunnel const *tunnel)
{
	return tunnel->config.mtu;
}

int l2tp_tunnel_get_mtu_discovery(struct l2tp_tunnel const *tunnel)
{
	return tunnel->config.do_pmtu_discovery;
}

struct sockaddr_in const *l2tp_tunnel_get_peer_addr(struct l2tp_tunnel const *tunnel)
{
	return &tunnel->config.peer_addr;
}

struct sockaddr_in const *l2tp_tunnel_get_local_addr(struct l2tp_tunnel const *tunnel)
{
	return &tunnel->config.local_addr;
}

int l2tp_tunnel_is_hide_avps(struct l2tp_tunnel const *tunnel)
{
	return tunnel->config.hide_avps;
}

void l2tp_tunnel_get_secret(struct l2tp_tunnel const *tunnel, char **secret, int *secret_len)
{
	*secret = tunnel->config.secret;
	*secret_len = tunnel->config.secret_len;
}

int l2tp_tunnel_get_trace_flags(struct l2tp_tunnel const *tunnel)
{
	return tunnel->config.trace_flags;
}

struct l2tp_peer *l2tp_tunnel_get_peer(struct l2tp_tunnel const *tunnel)
{
	return tunnel->my_peer;
}

/* Gives external modules access to the profile names configured for
 * the tunnel, which are therefore defaults for sessions, ppp etc. 
 */
void l2tp_tunnel_get_profile_names(struct l2tp_tunnel const *tunnel, char **tunnel_profile_name, 
				   char **session_profile_name, char **ppp_profile_name)
{
	if (tunnel_profile_name != NULL) {
		*tunnel_profile_name = tunnel->config.tunnel_profile_name ? 
			tunnel->config.tunnel_profile_name : L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME;
	}
	if (session_profile_name != NULL) {
		*session_profile_name = tunnel->config.session_profile_name ? 
			tunnel->config.session_profile_name : L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME;
	}
	if (ppp_profile_name != NULL) {
		*ppp_profile_name = tunnel->config.ppp_profile_name ? 
			tunnel->config.ppp_profile_name : L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME;
	}
}

/* Called when tunnel addresses are known. 
 */
void l2tp_tunnel_set_addresses(struct l2tp_tunnel *tunnel, struct sockaddr_in *src, struct sockaddr_in *dest)
{
	struct l2tp_peer *peer = tunnel->my_peer;

	if (src != NULL) {
		tunnel->config.local_addr = *src;
		if (peer->if_local_addr.s_addr == INADDR_ANY) {
			peer->if_local_addr.s_addr = src->sin_addr.s_addr;
		}
	}
	if (dest != NULL) {
		tunnel->config.peer_addr = *dest;
	}

	tunnel->fd_is_connected = 1;
	if (tunnel->config.do_pmtu_discovery) {
		int mtu;
		int result;
		socklen_t optlen = sizeof(mtu);
		result = l2tp_net_modify_socket(tunnel->fd, -1, 1);
		if (result == 0) {
			result = getsockopt(tunnel->fd, SOL_IP, IP_MTU, &mtu, &optlen);
			if (result == 0) {
				l2tp_tunnel_update_mtu(tunnel, tunnel->fd, mtu);
			}
		}
		L2TP_DEBUG(L2TP_FUNC, "%s: tunl %hu: fd=%d, set pmtu discovery w/ initial mtu %d, result=%d",
			   __func__, tunnel->status.tunnel_id, tunnel->fd, mtu, result);
	}
}

/* A heavily used function called whenever tunnel-related trace
 * messages are being logged.  Individual categories of message can be
 * enabled/disabled per tunnel instance. 
 */
void l2tp_tunnel_log(struct l2tp_tunnel const *tunnel, int category, int level, const char *fmt, ...)
{
	if ((tunnel != NULL) && (category & tunnel->config.trace_flags)) {
		va_list ap;

		va_start(ap, fmt);
		l2tp_vlog(level, fmt, ap);
		va_end(ap);
	}
}

/* Used by the state machine implementation to trace state changes.
 */
static void l2tp_tunnel_fsm_log(struct usl_fsm_instance const *fsmi, int level, const char *fmt, ...)
{
	struct l2tp_tunnel *tunnel = ((void *) fsmi) - offsetof(struct l2tp_tunnel, fsmi);
	
	if (tunnel->config.trace_flags & L2TP_FSM) {
		va_list ap;

		va_start(ap, fmt);
		l2tp_vlog(level, fmt, ap);
		va_end(ap);
	}
}

/* Helper for logging info about errors.
 */
static void l2tp_tunnel_log_error(struct l2tp_tunnel *tunnel,
				  struct l2tp_avp_result_code *result_code,
				  int result_code_len)
{
	const struct l2tp_result_codes *entry = &l2tp_tunnel_stopccn_result_codes[0];

	if (result_code == NULL) {
		goto out;
	}

	while (entry->result_code >= 0) {
		if (entry->result_code == result_code->result_code) {
			if (entry->error_code == result_code->error_code) {
				l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: STOPCCN error %d/%d: %s%s%s",
						tunnel->status.tunnel_id, 
						result_code->result_code,
						result_code->error_code,
						entry->error_string ? entry->error_string : "",
						(result_code_len > sizeof(*result_code)) && 
						result_code->error_message ? " - " : "",
						(result_code_len > sizeof(*result_code)) && 
						result_code->error_message ? result_code->error_message : "");
				break;
			}
		} 
		entry++;
	}
out:
	return;
}

/* We come here to generate an event on a tunnel state machine
 * instance. Events are handled inline.
 */
static void l2tp_tunnel_handle_event(struct l2tp_tunnel *tunnel, int event)
{
	/* Bump the use count on the tunnel while the event is handled in case the event
	 * causes the tunnel to be deleted. The context must not be deleted until after 
	 * the FSM event handler has returned.
	 */
	l2tp_tunnel_inc_use_count(tunnel);
	usl_fsm_handle_event(&tunnel->fsmi, event, tunnel, NULL, NULL);
	l2tp_tunnel_dec_use_count(tunnel);
}

struct l2tp_tunnel *l2tp_tunnel_find_by_id(uint16_t tunnel_id)
{
	struct usl_hlist_node *tmp;
	struct usl_hlist_node *walk;
	struct l2tp_tunnel *tunnel;

	usl_hlist_for_each(walk, tmp, l2tp_tunnel_id_hash(tunnel_id)) {
		tunnel = usl_hlist_entry(walk, struct l2tp_tunnel, id_hlist);
		if (tunnel->status.tunnel_id == tunnel_id) {
			l2tp_test_tunnel_id_hash_inc_stats(1);
			return tunnel;
		}
		l2tp_test_tunnel_id_hash_inc_stats(0);
	}

	return NULL;
}

struct l2tp_tunnel *l2tp_tunnel_find_by_name(const char *tunnel_name)
{
	struct usl_hlist_node *tmp;
	struct usl_hlist_node *walk;
	struct l2tp_tunnel *tunnel;

	usl_hlist_for_each(walk, tmp, l2tp_tunnel_name_hash(tunnel_name)) {
		tunnel = usl_hlist_entry(walk, struct l2tp_tunnel, name_hlist);
		if ((tunnel->config.tunnel_name != NULL) && 
		    (strcmp(tunnel->config.tunnel_name, tunnel_name) == 0)) {
			l2tp_test_tunnel_name_hash_inc_stats(1);
			return tunnel;
		}
		l2tp_test_tunnel_name_hash_inc_stats(0);
	}

	return NULL;
}

struct l2tp_tunnel *l2tp_tunnel_find_by_addr(struct sockaddr_in const *peer_addr, uint16_t peer_tunnel_id, 
					     int config_id, int tunnel_id_valid)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_tunnel *tunnel;

	usl_list_for_each(walk, tmp, &l2tp_tunnel_list) {
		tunnel = usl_list_entry(walk, struct l2tp_tunnel, list);
		if ((config_id != 0) && (tunnel->my_peer == NULL)) {
			continue;
		}
		if (tunnel->config.peer_addr.sin_addr.s_addr != peer_addr->sin_addr.s_addr) {
			continue;
		}
		if ((tunnel_id_valid) && (tunnel->peer.tunnel_id.value != peer_tunnel_id)) {
			continue;
		}
		if ((config_id != 0) && (tunnel->config.config_id != config_id)) {
			continue;
		}
		return tunnel;
	}

	return NULL;
}

static struct l2tp_tunnel *l2tp_tunnel_find_by_socket_fd(int fd)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_tunnel *tunnel;

	usl_list_for_each(walk, tmp, &l2tp_tunnel_list) {
		tunnel = usl_list_entry(walk, struct l2tp_tunnel, list);
		if (tunnel->fd == fd) {
			return tunnel;
		}
	}

	return NULL;
}

void l2tp_tunnel_update_mtu(struct l2tp_tunnel *tunnel, int fd, int mtu)
{
	if (tunnel == NULL) {
		tunnel = l2tp_tunnel_find_by_socket_fd(fd);
		if (tunnel == NULL) {
			goto out;
		}
	}
	if (!tunnel->fd_is_connected) {
		goto out;
	}

	/* Allow for L2TP and PPP headers */
	mtu -= 40;

	/* PPP MTUs must be <= 16384 */
	if (mtu > 16384) {
		mtu = 16384;
	}
	
	if (mtu != tunnel->config.mtu) {
		l2tp_tunnel_log(tunnel, L2TP_DATA, LOG_INFO, "tunl %hu: update mtu from %d to %d",
				tunnel->status.tunnel_id, tunnel->config.mtu, mtu);
		tunnel->config.mtu = mtu;

		/* Tell all sessions */
		l2tp_session_tunnel_modified(tunnel);
	}
out:
	return;
}

/* Tunnel ids are assigned random numbers, unless we're in a test
 * mode. Try 10 times to find an unused id, then give up.
 */
static uint16_t l2tp_tunnel_allocate_id(void)
{
	uint16_t tunnel_id;
	int tries;
	struct l2tp_tunnel *tunnel;

	for (tries = 0; tries < 10; tries++) {
		if (!l2tp_test_is_no_random_ids()) {
			tunnel_id = l2tp_make_random_id();
		} else {
			tunnel_id = l2tp_test_alloc_tunnel_id();
		}
		if (tunnel_id == 0) {
			continue;
		}
		tunnel = l2tp_tunnel_find_by_id(tunnel_id);
		if (tunnel == NULL) {
			return tunnel_id;
		}
	}

	return 0;
}

/* A config_id is an SNMP thing. It is used as a uniqifier when
 * tunnels are referenced by src-ip and dest-ip addresses. It is
 * assigned a number starting at 1.
 */
static int l2tp_tunnel_alloc_config_id(struct sockaddr_in const *peer_addr)
{
	int config_id = 1;

	for (config_id = 1; config_id < L2TP_TUNNEL_MAX_CONFIGID; config_id++) {
		if (l2tp_tunnel_find_by_addr(peer_addr, 0, config_id, 0) == NULL) {
			return config_id;
		}
	}
	
	return -L2TP_ERR_TUNNEL_TOO_MANY_SAME_IP;
}

/* We come here to record info about an event. This info is sent to
 * the peer when the tunnel closes.
 */
static void l2tp_tunnel_set_result(struct l2tp_tunnel *tunnel, uint16_t result_code,
				   uint16_t error_code, char *error_string)
{
	int len = 2;

	L2TP_DEBUG(L2TP_FUNC, "%s: tunl %hu: code=%hu error=%hu msg=%s", __func__,
		   tunnel->status.tunnel_id, result_code, error_code, error_string ? error_string : "");

	/* Don't overwrite a result that is already present */
	if (tunnel->result_code != NULL) {
		L2TP_DEBUG(L2TP_FUNC, "%s: tunl %hu: preserving current data: code=%hu error=%hu msg=%s", __func__,
			   tunnel->status.tunnel_id, tunnel->result_code->result_code, 
			   tunnel->result_code->error_code, tunnel->result_code->error_message);
		goto out;
	}

	/* Build result_code data structure */

	if ((error_code != 0) || (error_string != NULL)) {
		len += 2;
		if (error_string != NULL) {
			len += strlen(error_string) + 1;
		}
	}

	tunnel->result_code = malloc(len <= 5 ? 5 : len);
	if (tunnel->result_code == NULL) {
		/* Use emergency result code */
		tunnel->result_code = l2tp_tunnel_emergency_result_code;
	}
	tunnel->result_code->result_code = result_code;
	if (len > 2) {
		tunnel->result_code->error_code = error_code;
	} else {
		tunnel->result_code->error_code = 0;
	}
	if (len > 4) {
		strncpy(&tunnel->result_code->error_message[0], error_string, len - sizeof(struct l2tp_avp_result_code));
	} else {
		tunnel->result_code->error_message[0] = '\0';
	}
	tunnel->result_code_len = len;
out:
	return;
}

/* Come here to signal an error and tear the tunnel down.
 */
void l2tp_tunnel_protocol_error(struct l2tp_tunnel const *tunnel, int code, const char *str)
{
	l2tp_tunnel_set_result((struct l2tp_tunnel *) tunnel, code, 0, (char *) str);
	l2tp_tunnel_queue_event(tunnel->status.tunnel_id, L2TP_CCE_EVENT_CLOSE_REQ);	
}

/*****************************************************************************
 * Helpers to build AVPs.
 *****************************************************************************/

/* Used for optional tunnel authentication.
 */
static int l2tp_tunnel_build_challenge(struct l2tp_tunnel *tunnel)
{
	int result = 0;

	if (tunnel->config.secret == NULL) {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_AUTH_FAILED, 0,
				       "no tunnel secret available");
		result = -EPERM;
		goto out;
	}

	if (tunnel->my_challenge != NULL) {
		free(tunnel->my_challenge);
		tunnel->my_challenge = NULL;
		tunnel->my_challenge_len = 0;
	}

	tunnel->my_challenge_len = 16; /* could be random length */
	tunnel->my_challenge = malloc(tunnel->my_challenge_len);
	if (tunnel->my_challenge == NULL) {
		tunnel->my_challenge_len = 0;
		result = -ENOMEM;
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, 
				       L2TP_AVP_ERROR_NO_RESOURCE, NULL);
		l2tp_stats.no_control_frame_resources++;
		goto out;
	}

	l2tp_make_random_vector(tunnel->my_challenge, tunnel->my_challenge_len);
	L2TP_DEBUG(L2TP_PROTOCOL, "%s: value=%s", __func__, l2tp_buffer_hexify(tunnel->my_challenge, tunnel->my_challenge_len));
out:
	return result;
}

/* Used for optional tunnel authentication.
 */
static void l2tp_tunnel_build_challenge_response(struct l2tp_tunnel *tunnel, 
						 uint8_t id,
						 struct l2tp_avp_challenge *challenge, 
						 int challenge_len,
						 struct l2tp_avp_challenge_response *challenge_response)
{
	struct MD5Context ctx;
	uint8_t *digest = &challenge_response->value[0];
	uint8_t chap_id = id;

	L2TP_DEBUG(L2TP_PROTOCOL, "%s: id=%02x secret=%s challenge=%s", __func__,
		   id, tunnel->config.secret, l2tp_buffer_hexify(challenge, challenge_len));

	/* The challenge response is built by doing an MD5 hash of the following
	 * octet stream:
	 * ID + secret + challenge
	 */
	MD5Init(&ctx);
	MD5Update(&ctx, &chap_id, 1);
	MD5Update(&ctx, tunnel->config.secret, tunnel->config.secret_len);
	MD5Update(&ctx, &challenge->value[0], challenge_len);
	MD5Final(digest, &ctx);

	L2TP_DEBUG(L2TP_PROTOCOL, "%s: value=%s", __func__, l2tp_buffer_hexify(&challenge_response->value[0], 16));
}

static int l2tp_tunnel_build_result_code(struct l2tp_tunnel *tunnel)
{
	/* If there is a stored result_code in the tunnel, use it.
	 * Else build one with "general clear request".
	 */
	if (tunnel->result_code == NULL) {
		tunnel->result_code = malloc(sizeof(struct l2tp_avp_result_code));
		if (tunnel->result_code == NULL) {
			tunnel->result_code = l2tp_tunnel_emergency_result_code;
		}
		tunnel->result_code->result_code = 1; /* general clear request */
		tunnel->result_code->error_code = 0;
		tunnel->result_code_len = sizeof(*tunnel->result_code);
	}
	return 0;
}

static int l2tp_tunnel_build_tiebreaker(struct l2tp_tunnel *tunnel, 
					struct l2tp_avp_tiebreaker *tiebreaker)
{
	/* FIXME: Not sure of the criteria to use for generating a tie breaker.
	 * Use a random byte sequence for now.
	 */
	l2tp_make_random_vector(&tiebreaker->value[0], sizeof(tiebreaker->value));
	return 0;
}

static struct l2tp_tunnel_profile *l2tp_tunnel_profile_find(const char *name)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_tunnel_profile *profile;

	usl_list_for_each(walk, tmp, &l2tp_tunnel_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_tunnel_profile, list);
		if (strcmp(&profile->profile_name[0], name) == 0) {
			return profile;
		}
	}

	return NULL;
}

/* Come here to add a session to a tunnel in cases where it is already linked 
 * into the session list and has been allowed to create, i.e. to recreate 
 * sessions in persistent tunnels.
 */
void l2tp_tunnel_session_add_again(struct l2tp_tunnel *tunnel, struct usl_hlist_node *hlist, uint16_t session_id)
{
	usl_hlist_add_head(hlist, l2tp_tunnel_session_id_hash(tunnel, session_id));
	tunnel->status.num_sessions++;
	L2TP_DEBUG(L2TP_FUNC, "%s: FUNC: tunl %hu: num_sessions=%d", __func__, tunnel->status.tunnel_id, tunnel->status.num_sessions);
}

int l2tp_tunnel_session_add(struct l2tp_tunnel *tunnel, struct usl_list_head *list, struct usl_hlist_node *hlist, uint16_t session_id)
{
	int result = 0;

	/* This will be decremented by l2tp_tunnel_session_remove() if
	 * something goes wrong below. 
	 */
	tunnel->status.num_sessions++;

	/* Check if new session is administratively disabled */
	if (l2tp_tunnel_drain) {
		result = -L2TP_ERR_TUNNEL_ADD_ADMIN_DISABLED;
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu: new session rejected: drain_tunnels is set", 
				tunnel->status.tunnel_id);
		goto out;
	}
	if (tunnel->config.max_sessions != 0) {
		if (tunnel->status.num_sessions > tunnel->config.max_sessions) {
			result = -L2TP_ERR_TUNNEL_TOO_MANY_SESSIONS;
			l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu: new session rejected: tunnel session limit (%d) exceeded",
					tunnel->status.tunnel_id, tunnel->config.max_sessions);
			l2tp_stats.too_many_sessions++;
			goto out;
		}
	}

	/* If session count was zero and the setup timer was
	 * running, delete the timer. 
	 */
	if ((tunnel->status.num_sessions == 0) && (tunnel->setup_timer != NULL)) {
		usl_timer_delete(tunnel->setup_timer);
		tunnel->setup_timer = NULL;
	}

	usl_list_add(list, &tunnel->session_list);
	usl_hlist_add_head(hlist, l2tp_tunnel_session_id_hash(tunnel, session_id));

out:
	L2TP_DEBUG(L2TP_FUNC, "%s: FUNC: tunl %hu: num_sessions=%d", __func__, tunnel->status.tunnel_id, tunnel->status.num_sessions);
	return result;
}

void l2tp_tunnel_session_remove(struct l2tp_tunnel *tunnel, struct usl_list_head *list, struct usl_hlist_node *hlist, int persist)
{
#if 1
	if (!persist) {
		usl_list_del(list);
	}
#else
	usl_list_del(list);
#endif
	if (!usl_hlist_unhashed(hlist)) {
		usl_hlist_del(hlist);
	}
	tunnel->status.num_sessions--;
	L2TP_DEBUG(L2TP_FUNC, "%s: FUNC: tunl %hu: num_sessions=%d", __func__, tunnel->status.tunnel_id, tunnel->status.num_sessions);

	/* If session count is now zero and the idle_count parameter is set, start a
	 * timer to cleanup the tunnel. If another session comes along before the timer
	 * expires, continue as normal.
	 */
	if ((tunnel->status.num_sessions == 0) && (tunnel->config.idle_timeout != 0)) {
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu: last session closed - starting cleanup timer",
				tunnel->status.tunnel_id);
		tunnel->setup_timer = usl_timer_create(USL_TIMER_TICKS(tunnel->config.idle_timeout), 0, l2tp_tunnel_idle_timeout, tunnel, NULL);
	}
}

int l2tp_tunnel_send_hello(void *tun)
{
	struct l2tp_tunnel *tunnel = tun;
	struct l2tp_avp_desc avps[L2TP_AVP_TYPE_NUM_AVPS];
	struct l2tp_avp_message_type msg_type;
	struct l2tp_packet *pkt = NULL;
	int result;

	memset(&avps, 0, sizeof(avps));

	/* HELLO messages have only one AVP -- the message type */
	msg_type.type = L2TP_AVP_TYPE_MESSAGE;
	avps[L2TP_AVP_TYPE_MESSAGE].value = (void *) &msg_type;
	avps[L2TP_AVP_TYPE_MESSAGE].value_len = sizeof(msg_type);
	
	/* build and send HELLO */
	result = l2tp_avp_message_encode(L2TP_AVP_MSG_HELLO, &pkt, 0, avps, tunnel);
	if (result < 0) {
		l2tp_stats.encode_message_fails++;
		goto error;
	}

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: sending HELLO", tunnel->status.tunnel_id);

	result = l2tp_net_send(tunnel, tunnel->peer.tunnel_id.value, 0, pkt, L2TP_AVP_MSG_HELLO);

out:
	return result;

error:
	if (pkt != NULL) {
		l2tp_pkt_free(pkt);
	}
	goto out;
}

/* Set our transmit window size to the lower of our configured value and
 * the peer's advertised rx window size. 
 */
static int l2tp_tunnel_adjust_tx_window_size(struct l2tp_tunnel *tunnel)
{
	int result = 0;

	if (tunnel->peer.rx_window_size.value < tunnel->config.tx_window_size) {
		struct l2tp_xprt_tunnel_modify_data xprt_params;

		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: adjust tx_window_size: peer=%hu, ours=%hu",
				tunnel->status.tunnel_id, tunnel->peer.rx_window_size.value, tunnel->config.tx_window_size);

		memset(&xprt_params, 0, sizeof(xprt_params));
		xprt_params.tx_window_size = tunnel->peer.rx_window_size.value;
		xprt_params.flags |= L2TP_XPRT_TUN_FLAG_TX_WINDOW_SIZE;

		result = l2tp_xprt_tunnel_modify(tunnel->xprt, &xprt_params);
		if (result == 0) {
			tunnel->status.actual_tx_window_size = tunnel->peer.rx_window_size.value;
		}
	} else {
		tunnel->status.actual_tx_window_size = tunnel->config.tx_window_size;
	}

	return result;
}

/*****************************************************************************
 * Timers
 *****************************************************************************/

static void l2tp_tunnel_persist_timeout(void *arg)
{	
	struct l2tp_tunnel *tunnel = arg;

	L2TP_DEBUG(L2TP_FUNC, "FUNC: tunl %hu: persist timer %p expired", tunnel->status.tunnel_id, tunnel->cleanup_timer);

	/* Clone the tunnel config and recreate. */
	l2tp_tunnel_recreate(tunnel);

	/* Now delete the original tunnel */
	l2tp_tunnel_dec_use_count(tunnel);
}

static void l2tp_tunnel_cleanup_timeout(void *arg)
{	
	struct l2tp_tunnel *tunnel = arg;

	L2TP_DEBUG(L2TP_FUNC, "FUNC: tunl %hu: cleanup timer %p expired", tunnel->status.tunnel_id, tunnel->cleanup_timer);

	/* If tunnel is persistent, don't close it now, start a timer to delay trying
	 * to restablish it. Hold the tunnel ref count to prevent it being deleted.
	 */
	if (tunnel->config.persist) {
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "tunl %hu: scheduling recreate of persistent tunnel", 
				tunnel->status.tunnel_id);
		if (tunnel->cleanup_timer != NULL) {
			usl_timer_delete(tunnel->cleanup_timer);
		}
		tunnel->cleanup_timer = usl_timer_create(USL_TIMER_TICKS(l2tp_tunnel_persist_pend_timeout), 0, 
							 l2tp_tunnel_persist_timeout, tunnel, NULL);
		if (tunnel->cleanup_timer == NULL) {
			l2tp_tunnel_dec_use_count(tunnel);
			l2tp_stats.tunnel_setup_failures++;
		}
	}

	/* Tell plugins that tunnel is being deleted */
	l2tp_tunnel_deleted_ind(tunnel);

	/* Stop the tunnel's hello and retry timers and purge packet
	 * queues. 
	 */
	if (tunnel->xprt != NULL) {
		l2tp_xprt_tunnel_down(tunnel->xprt);
	}

	/* This will delete the tunnel context, unless someone still
	 * holds a reference to it 
	 */
	l2tp_tunnel_dec_use_count(tunnel);
}

static void l2tp_tunnel_setup_timeout(void *arg)
{	
	struct l2tp_tunnel *tunnel = arg;

	L2TP_DEBUG(L2TP_FUNC, "FUNC: tunl %hu: setup timer %p expired", tunnel->status.tunnel_id, tunnel->setup_timer);
	usl_timer_delete(tunnel->setup_timer);
	tunnel->setup_timer = NULL;

	l2tp_tunnel_log(tunnel, L2TP_FSM, LOG_INFO, "FSM: tunl %hu: establish timeout", tunnel->status.tunnel_id);

	l2tp_stats.tunnel_setup_failures++;
	l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, 
			       L2TP_AVP_ERROR_NO_ERROR,
			       "tunnel establish timeout");
	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_CLOSE_REQ);
}

static void l2tp_tunnel_idle_timeout(void *arg)
{	
	struct l2tp_tunnel *tunnel = arg;

	L2TP_DEBUG(L2TP_FUNC, "FUNC: tunl %hu: idle timer %p expired", tunnel->status.tunnel_id, tunnel->setup_timer);
	usl_timer_delete(tunnel->setup_timer);
	tunnel->setup_timer = NULL;

	l2tp_tunnel_log(tunnel, L2TP_FSM, LOG_INFO, "FSM: tunl %hu: no-session timeout", tunnel->status.tunnel_id);

	l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, 
			       L2TP_AVP_ERROR_NO_ERROR,
			       "tunnel no_session timeout");
	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_CLOSE_REQ);
}

/*****************************************************************************
 * Operational state changes
 *****************************************************************************/

static void l2tp_tunnel_created_ind(struct l2tp_tunnel *tunnel)
{
	l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu created", tunnel->status.tunnel_id);

	/* Give plugins visibility of tunnel created */
	if (l2tp_tunnel_created_hook != NULL) {
		(*l2tp_tunnel_created_hook)(tunnel->status.tunnel_id);
	}
}

static void l2tp_tunnel_deleted_ind(struct l2tp_tunnel *tunnel)
{
	l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu deleted", tunnel->status.tunnel_id);

	/* Give plugins visibility of tunnel deleted */
	if (l2tp_tunnel_deleted_hook != NULL) {
		(*l2tp_tunnel_deleted_hook)(tunnel->status.tunnel_id);
	}
}

static void l2tp_tunnel_up_ind(struct l2tp_tunnel *tunnel)
{
	if (!tunnel->up) {
		tunnel->down = 0;
		tunnel->up = -1;

		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu up", tunnel->status.tunnel_id);

		/* If setup timer is running, delete it */
		if (tunnel->setup_timer != NULL) {
			usl_timer_delete(tunnel->setup_timer);
			tunnel->setup_timer = NULL;
		}

		/* Give plugins visibility of tunnel up */
		if (l2tp_tunnel_up_hook != NULL) {
			(*l2tp_tunnel_up_hook)(tunnel->status.tunnel_id, tunnel->peer.tunnel_id.value);
		}

		/* tell all waiting sessions */
		l2tp_session_tunnel_updown_event(tunnel, 1);
	}
}

static void l2tp_tunnel_down_ind(struct l2tp_tunnel *tunnel)
{
	tunnel->sccrq_sent = 0;

	if (!tunnel->down) {
		tunnel->down = -1;
		tunnel->up = 0;

		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu down", tunnel->status.tunnel_id);

		/* tell all sessions */
		l2tp_session_tunnel_updown_event(tunnel, 0);

		/* Give plugins visibility of tunnel down */
		if (l2tp_tunnel_down_hook != NULL) {
			(*l2tp_tunnel_down_hook)(tunnel->status.tunnel_id);
		}
	}
}

/*****************************************************************************
 * CCE State machine actions
 *****************************************************************************/

static void l2tp_tunnel_act_cleanup(void *tun, void *session, void *arg3)
{
	struct l2tp_tunnel *tunnel = tun;

	L2TP_DEBUG(L2TP_FUNC, "%s: %s: use_count=%d", __func__, tunnel->fsmi.name, tunnel->status.use_count);
	l2tp_tunnel_down_ind(tunnel);
	l2tp_xprt_tunnel_going_down(tunnel->xprt);

#ifdef L2TP_TEST
	if (l2tp_test_is_hold_tunnels() && tunnel->hold) {
		/* Leave tunnel context for examination. */
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu: down and held", tunnel->status.tunnel_id);
		return;
	}
#endif /* L2TP_TEST */

	/* Start a cleanup timer, after which the tunnel context will be deleted */
	if (tunnel->cleanup_timer == NULL) {
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_DEBUG, "FUNC: tunl %hu: starting cleanup timer", tunnel->status.tunnel_id);
		tunnel->cleanup_timer = usl_timer_create(USL_TIMER_TICKS(60), 0, l2tp_tunnel_cleanup_timeout, tunnel, NULL);
		if (tunnel->cleanup_timer == NULL) {
			/* Kill tunnel immediately if no memory - serious problems! */
			l2tp_tunnel_cleanup_timeout(tunnel);
		}
		L2TP_DEBUG(L2TP_FUNC, "%s: tunl %hu: allocated cleanup_timer=%p", __func__, tunnel->status.tunnel_id, tunnel->cleanup_timer);
	}
}

static void l2tp_tunnel_act_send_sccrq(void *tun, void *session, void *arg3)
{
	struct l2tp_tunnel *tunnel = tun;
	struct l2tp_avp_desc avps[L2TP_AVP_TYPE_NUM_AVPS];
	struct l2tp_avp_message_type msg_type;
	struct l2tp_avp_tunnel_id tunnel_id;
	struct l2tp_avp_rx_window_size rx_window_size;
	struct l2tp_avp_framing_cap framing_cap;
	struct l2tp_avp_bearer_cap bearer_cap;
	struct l2tp_packet *pkt = NULL;
	int result;

	/* Check if administrator allows remote tunnel creation */
	result = l2tp_tunnel_is_ok_to_create(tunnel, tunnel->status.created_by_admin);
	if (result < 0) {
		goto error;
	}

	memset(&avps, 0, sizeof(avps));

	/* First, setup the mandatory AVPs */
	msg_type.type = L2TP_AVP_TYPE_MESSAGE;
	avps[L2TP_AVP_TYPE_MESSAGE].value = (void *) &msg_type;
	avps[L2TP_AVP_TYPE_MESSAGE].value_len = sizeof(msg_type);
	avps[L2TP_AVP_TYPE_PROTOCOL_VERSION].value = (void *) &l2tp_avp_my_protocol_version;
	avps[L2TP_AVP_TYPE_PROTOCOL_VERSION].value_len = sizeof(l2tp_avp_my_protocol_version);
	if (tunnel->config.host_name != NULL) {
		avps[L2TP_AVP_TYPE_HOST_NAME].value = (void *) tunnel->config.host_name;
		avps[L2TP_AVP_TYPE_HOST_NAME].value_len = tunnel->config.host_name_len;
	} else {
		avps[L2TP_AVP_TYPE_HOST_NAME].value = (void *) l2tp_avp_my_host_name;
		avps[L2TP_AVP_TYPE_HOST_NAME].value_len = l2tp_avp_my_host_name_len;
	}
	framing_cap.value = 0;
	framing_cap.bits.async = tunnel->config.framing_cap_async;
	framing_cap.bits.sync = tunnel->config.framing_cap_sync;
	avps[L2TP_AVP_TYPE_FRAMING_CAP].value = (void *) &framing_cap;
	avps[L2TP_AVP_TYPE_FRAMING_CAP].value_len = sizeof(framing_cap);
	tunnel_id.value = tunnel->status.tunnel_id;
	avps[L2TP_AVP_TYPE_TUNNEL_ID].value = (void *) &tunnel_id;
	avps[L2TP_AVP_TYPE_TUNNEL_ID].value_len = sizeof(tunnel_id);

	/* Now the optional AVPs */
	rx_window_size.value = tunnel->config.rx_window_size;
	avps[L2TP_AVP_TYPE_RX_WINDOW_SIZE].value = (void *) &rx_window_size;
	avps[L2TP_AVP_TYPE_RX_WINDOW_SIZE].value_len = sizeof(rx_window_size);
	bearer_cap.value = 0;
	bearer_cap.bits.analog = tunnel->config.bearer_cap_analog;
	bearer_cap.bits.digital = tunnel->config.bearer_cap_digital;
	avps[L2TP_AVP_TYPE_BEARER_CAP].value = (void *) &bearer_cap;
	avps[L2TP_AVP_TYPE_BEARER_CAP].value_len = sizeof(bearer_cap);
	if (tunnel->config.auth_mode == L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE) {
		result = l2tp_tunnel_build_challenge(tunnel);
		if (result < 0) {
			goto error;
		}
		avps[L2TP_AVP_TYPE_CHALLENGE].value = (void *) tunnel->my_challenge;
		avps[L2TP_AVP_TYPE_CHALLENGE].value_len = tunnel->my_challenge_len;
	}
	if (tunnel->config.use_tiebreaker) {
		l2tp_tunnel_build_tiebreaker(tunnel, &tunnel->status.tiebreaker);
		avps[L2TP_AVP_TYPE_TIEBREAKER].value = (void *) &tunnel->status.tiebreaker;
		avps[L2TP_AVP_TYPE_TIEBREAKER].value_len = sizeof(tunnel->status.tiebreaker);
		tunnel->status.have_tiebreaker = 1;
	}
	avps[L2TP_AVP_TYPE_FIRMWARE_REVISION].value = (void *) &l2tp_avp_my_firmware_revision;
	avps[L2TP_AVP_TYPE_FIRMWARE_REVISION].value_len = sizeof(l2tp_avp_my_firmware_revision);
	avps[L2TP_AVP_TYPE_VENDOR_NAME].value = (void *) l2tp_avp_my_vendor_name;
	avps[L2TP_AVP_TYPE_VENDOR_NAME].value_len = l2tp_avp_my_vendor_name_len;
	
	/* if configured to do so, start an establish timer */
	if (l2tp_tunnel_establish_timeout != 0) {
		if (tunnel->setup_timer != NULL) {
			usl_timer_delete(tunnel->setup_timer);
			tunnel->setup_timer = NULL;
		}
		tunnel->setup_timer = usl_timer_create(USL_TIMER_TICKS(l2tp_tunnel_establish_timeout), 0, l2tp_tunnel_setup_timeout, tunnel, NULL);
		if (tunnel->setup_timer == NULL) {
			/* Kill tunnel immediately if no memory - serious problems! */
			l2tp_tunnel_setup_timeout(tunnel);
		}
		L2TP_DEBUG(L2TP_FUNC, "%s: tunl %hu: allocated setup_timer=%p", __func__, tunnel->status.tunnel_id, tunnel->setup_timer);
	}

	/* build and send SCCRQ */
	result = l2tp_avp_message_encode(L2TP_AVP_MSG_SCCRQ, &pkt, tunnel->config.hide_avps, avps, tunnel);
	if (result < 0) {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, L2TP_AVP_ERROR_NO_ERROR, "failed to encode SCCRQ message");
		l2tp_stats.encode_message_fails++;
		goto error;
	}

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: sending SCCRQ", l2tp_tunnel_id(tunnel));

	tunnel->sccrq_sent = -1;
	result = l2tp_net_send(tunnel, 0, 0, pkt, L2TP_AVP_MSG_SCCRQ);

	if (!l2tp_tunnel_is_fd_connected(tunnel)) {
		l2tp_net_get_socket_addresses(tunnel->fd, &tunnel->config.local_addr, NULL, tunnel); 
	}

	return;

error:
	l2tp_tunnel_log_error(tunnel, tunnel->result_code, tunnel->result_code_len);

	if (pkt != NULL) {
		l2tp_pkt_free(pkt);
	}
	l2tp_tunnel_queue_event(tunnel->status.tunnel_id, L2TP_CCE_EVENT_CLOSE_REQ);
}

static void l2tp_tunnel_act_send_sccrp(void *tun, void *sess, void *arg3)
{
	/* build and send SCCRP */

	struct l2tp_tunnel *tunnel = tun;
	struct l2tp_avp_desc avps[L2TP_AVP_TYPE_NUM_AVPS];
	struct l2tp_avp_message_type msg_type;
	struct l2tp_avp_tunnel_id tunnel_id;
	struct l2tp_avp_rx_window_size rx_window_size;
	struct l2tp_avp_challenge *challenge = NULL;
	struct l2tp_avp_challenge_response *challenge_response = NULL;
	struct l2tp_packet *pkt = NULL;
	struct l2tp_avp_framing_cap framing_cap;
	struct l2tp_avp_bearer_cap bearer_cap;
	int result;

	/* If we don't know the peer's tunnel_id, we can't send SCCRP. This can happen
	 * if AVP hiding is being used by the peer and the configured tunnel secrets
	 * do not match (we can't decode the peer's TUNNEL_ID AVP).
	 */
	if (tunnel->peer.tunnel_id.value == 0) {
		l2tp_tunnel_queue_event(tunnel->status.tunnel_id, L2TP_CCE_EVENT_CLOSE_REQ);
		l2tp_stats.tunnel_setup_failures++;
		goto out;
	}

	/* Check if administrator allows remote tunnel creation */
	result = l2tp_tunnel_is_ok_to_create(tunnel, tunnel->status.created_by_admin);
	if (result < 0) {
		goto error;
	}

	/* Adjust our tx_window_size if necessary */
	result = l2tp_tunnel_adjust_tx_window_size(tunnel);
	if (result < 0) {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, 0,
				       "failed to set tx_window_size");
		goto error;
	}

	memset(&avps, 0, sizeof(avps));

	/* First, setup the mandatory AVPs */
	msg_type.type = L2TP_AVP_TYPE_MESSAGE;
	avps[L2TP_AVP_TYPE_MESSAGE].value = (void *) &msg_type;
	avps[L2TP_AVP_TYPE_MESSAGE].value_len = sizeof(msg_type);
	avps[L2TP_AVP_TYPE_PROTOCOL_VERSION].value = (void *) &l2tp_avp_my_protocol_version;
	avps[L2TP_AVP_TYPE_PROTOCOL_VERSION].value_len = sizeof(l2tp_avp_my_protocol_version);
	framing_cap.value = 0;
	framing_cap.bits.async = tunnel->config.framing_cap_async;
	framing_cap.bits.sync = tunnel->config.framing_cap_sync;
	avps[L2TP_AVP_TYPE_FRAMING_CAP].value = (void *) &framing_cap;
	avps[L2TP_AVP_TYPE_FRAMING_CAP].value_len = sizeof(framing_cap);
	if (tunnel->config.host_name != NULL) {
		avps[L2TP_AVP_TYPE_HOST_NAME].value = (void *) tunnel->config.host_name;
		avps[L2TP_AVP_TYPE_HOST_NAME].value_len = tunnel->config.host_name_len;
	} else {
		avps[L2TP_AVP_TYPE_HOST_NAME].value = (void *) l2tp_avp_my_host_name;
		avps[L2TP_AVP_TYPE_HOST_NAME].value_len = l2tp_avp_my_host_name_len;
	}
	tunnel_id.value = tunnel->status.tunnel_id;
	avps[L2TP_AVP_TYPE_TUNNEL_ID].value = (void *) &tunnel_id;
	avps[L2TP_AVP_TYPE_TUNNEL_ID].value_len = sizeof(tunnel_id);

	/* Now the optional AVPs */
	bearer_cap.value = 0;
	bearer_cap.bits.analog = tunnel->config.bearer_cap_analog;
	bearer_cap.bits.digital = tunnel->config.bearer_cap_digital;
	avps[L2TP_AVP_TYPE_BEARER_CAP].value = (void *) &bearer_cap;
	avps[L2TP_AVP_TYPE_BEARER_CAP].value_len = sizeof(bearer_cap);
	avps[L2TP_AVP_TYPE_FIRMWARE_REVISION].value = (void *) &l2tp_avp_my_firmware_revision;
	avps[L2TP_AVP_TYPE_FIRMWARE_REVISION].value_len = sizeof(l2tp_avp_my_firmware_revision);
	avps[L2TP_AVP_TYPE_VENDOR_NAME].value = (void *) l2tp_avp_my_vendor_name;
	avps[L2TP_AVP_TYPE_VENDOR_NAME].value_len = l2tp_avp_my_vendor_name_len;
	rx_window_size.value = tunnel->config.rx_window_size;
	avps[L2TP_AVP_TYPE_RX_WINDOW_SIZE].value = (void *) &rx_window_size;
	avps[L2TP_AVP_TYPE_RX_WINDOW_SIZE].value_len = sizeof(rx_window_size);
	if (tunnel->config.auth_mode == L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE) {
		if (tunnel->config.secret == NULL) {
			l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_AUTH_FAILED, 0,
					       "no tunnel secret available");
			result = -EPERM;
			goto error;
		}

		l2tp_tunnel_build_challenge(tunnel);
		if (result < 0) {
			l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, 
					       L2TP_AVP_ERROR_NO_RESOURCE, NULL);
			l2tp_stats.no_control_frame_resources++;
			goto error;
		}
		avps[L2TP_AVP_TYPE_CHALLENGE].value = (void *) tunnel->my_challenge;
		avps[L2TP_AVP_TYPE_CHALLENGE].value_len = tunnel->my_challenge_len;
	}

	/* If SCCRQ included a challenge, build "challenge response" AVP */
	if (tunnel->peer.challenge != NULL) {
		challenge_response = malloc(sizeof(*challenge_response));
		if (challenge_response == NULL) {
			result = -ENOMEM;
			l2tp_stats.no_control_frame_resources++;
			goto error;
		}
		if (tunnel->config.secret == NULL) {
			l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_AUTH_FAILED, 0,
					       "no tunnel secret available");
			result = -EPERM;
			goto error;
		}
		l2tp_tunnel_build_challenge_response(tunnel, 
						     L2TP_AVP_MSG_SCCRP,
						     tunnel->peer.challenge, 
						     tunnel->peer.challenge_len, 
						     challenge_response);
		avps[L2TP_AVP_TYPE_CHALLENGE_RESPONSE].value = (void *) challenge_response;
		avps[L2TP_AVP_TYPE_CHALLENGE_RESPONSE].value_len = sizeof(challenge_response->value);
	}

	/* Check that a secret is available if AVP hiding is enabled */
	if (tunnel->config.hide_avps && (tunnel->config.secret == NULL)) {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_AUTH_FAILED, 0,
				       "no tunnel secret available");
		result = -EPERM;
		goto error;
	}
	
	/* build and send SCCRP */
	result = l2tp_avp_message_encode(L2TP_AVP_MSG_SCCRP, &pkt, tunnel->config.hide_avps, avps, tunnel);
	if (result < 0) {
		l2tp_stats.encode_message_fails++;
		goto error;
	}

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: sending SCCRP to peer %hu", 
			l2tp_tunnel_id(tunnel), l2tp_tunnel_peer_id(tunnel));

	result = l2tp_net_send(tunnel, tunnel->peer.tunnel_id.value, 0, pkt, L2TP_AVP_MSG_SCCRP);

out:
	if (challenge != NULL) {
		free(challenge);
	}
	if (challenge_response != NULL) {
		free(challenge_response);
	}

	return;

error:
	l2tp_stats.tunnel_setup_failures++;
	l2tp_tunnel_log_error(tunnel, tunnel->result_code, tunnel->result_code_len);

	if (pkt != NULL) {
		l2tp_pkt_free(pkt);
	}
	l2tp_tunnel_queue_event(tunnel->status.tunnel_id, L2TP_CCE_EVENT_SCCRP_DENY);
	goto out;
}

static void l2tp_tunnel_act_send_scccn(void *tun, void *sess, void *arg3)
{
	/* build and send SCCCN */

	struct l2tp_tunnel *tunnel = tun;
	struct l2tp_avp_desc avps[L2TP_AVP_TYPE_NUM_AVPS];
	struct l2tp_avp_message_type msg_type;
	struct l2tp_avp_challenge_response *challenge_response = NULL;
	struct l2tp_packet *pkt = NULL;
	int result;

	/* Adjust our tx_window_size if necessary */
	result = l2tp_tunnel_adjust_tx_window_size(tunnel);
	if (result < 0) {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, 0,
				       "failed to set tx_window_size");
		goto error;
	}

	memset(&avps, 0, sizeof(avps));

	/* First, setup the mandatory AVPs */
	msg_type.type = L2TP_AVP_TYPE_MESSAGE;
	avps[L2TP_AVP_TYPE_MESSAGE].value = (void *) &msg_type;
	avps[L2TP_AVP_TYPE_MESSAGE].value_len = sizeof(msg_type);

	/* Now the optional AVPs */

	/* If SCCRP included a challenge, build "challenge response" AVP */
	if (tunnel->peer.challenge != NULL) {
		challenge_response = malloc(sizeof(*challenge_response));
		if (challenge_response == NULL) {
			result = -ENOMEM;
			l2tp_stats.no_control_frame_resources++;
			goto error;
		}
		if (tunnel->config.secret == NULL) {
			l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_AUTH_FAILED, 0,
					       "no tunnel secret available");
			result = -EPERM;
			goto error;
		}
		l2tp_tunnel_build_challenge_response(tunnel, 
						     L2TP_AVP_MSG_SCCCN,
						     tunnel->peer.challenge, 
						     tunnel->peer.challenge_len, 
						     challenge_response);
		avps[L2TP_AVP_TYPE_CHALLENGE_RESPONSE].value = (void *) challenge_response;
		avps[L2TP_AVP_TYPE_CHALLENGE_RESPONSE].value_len = sizeof(challenge_response->value);
	}

	/* build and send SCCCN */
	result = l2tp_avp_message_encode(L2TP_AVP_MSG_SCCCN, &pkt, tunnel->config.hide_avps, avps, tunnel);
	if (result < 0) {
		l2tp_stats.encode_message_fails++;
		goto error;
	}

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: sending SCCCN to peer %hu", 
			l2tp_tunnel_id(tunnel), l2tp_tunnel_peer_id(tunnel));

	result = l2tp_net_send(tunnel, tunnel->peer.tunnel_id.value, 0, pkt, L2TP_AVP_MSG_SCCCN);

	l2tp_tunnel_up_ind(tunnel);

out:
	if (challenge_response != NULL) {
		free(challenge_response);
	}

	return;

error:
	l2tp_stats.tunnel_setup_failures++;
	if (pkt != NULL) {
		l2tp_pkt_free(pkt);
	}
	l2tp_tunnel_queue_event(tunnel->status.tunnel_id, L2TP_CCE_EVENT_SCCCN_DENY);
	goto out;
}

static void l2tp_tunnel_act_send_stopccn(void *tun, void *sess, void *arg3)
{
	/* build and send STOPCCN */

	struct l2tp_tunnel *tunnel = tun;
	struct l2tp_avp_desc avps[L2TP_AVP_TYPE_NUM_AVPS];
	struct l2tp_avp_message_type msg_type;
	struct l2tp_avp_tunnel_id tunnel_id;
	struct l2tp_packet *pkt;
	int result;

	memset(&avps, 0, sizeof(avps));

	/* If we don't know the peer's tunnel_id, we can't sent a
	 * STOPCCN. This can happen if AVP hiding is enabled and the
	 * configured tunnels don't match such that we can't decode
	 * the peer's TUNNEL_ID AVP in the SCCRP.
	 */
	if (tunnel->peer.tunnel_id.value == 0) {
		goto out;
	}

	/* First, setup the mandatory AVPs */
	msg_type.type = L2TP_AVP_TYPE_MESSAGE;
	avps[L2TP_AVP_TYPE_MESSAGE].value = (void *) &msg_type;
	avps[L2TP_AVP_TYPE_MESSAGE].value_len = sizeof(msg_type);
	tunnel_id.value = tunnel->status.tunnel_id;
	avps[L2TP_AVP_TYPE_TUNNEL_ID].value = (void *) &tunnel_id;
	avps[L2TP_AVP_TYPE_TUNNEL_ID].value_len = sizeof(tunnel_id);
	l2tp_tunnel_build_result_code(tunnel);
	avps[L2TP_AVP_TYPE_RESULT_CODE].value = (void *) tunnel->result_code;
	avps[L2TP_AVP_TYPE_RESULT_CODE].value_len = tunnel->result_code_len;
	
	/* build and send StopCCN */
	result = l2tp_avp_message_encode(L2TP_AVP_MSG_STOPCCN, &pkt, tunnel->config.hide_avps, avps, tunnel);
	if (result < 0) {
		l2tp_stats.encode_message_fails++;
		if (pkt != NULL) {
			l2tp_pkt_free(pkt);
		}
		goto out;
	}

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: sending STOPCCN to peer %hu", 
			l2tp_tunnel_id(tunnel), l2tp_tunnel_peer_id(tunnel));

	result = l2tp_net_send(tunnel, tunnel->peer.tunnel_id.value, 0, pkt, L2TP_AVP_MSG_STOPCCN);
	
out:
	l2tp_tunnel_down_ind(tunnel);
	l2tp_xprt_tunnel_going_down(tunnel->xprt);

#ifdef L2TP_TEST
	if (l2tp_test_is_hold_tunnels() && tunnel->hold) {
		/* Leave tunnel context for examination. */
		L2TP_DEBUG(L2TP_FUNC, "%s: tunnel %hu down and held", __func__, tunnel->status.tunnel_id);
		return;
	}
#endif /* L2TP_TEST */

	L2TP_DEBUG(L2TP_FUNC, "%s: tunl %hu, cleaning up tunnel", __func__,
		   tunnel->status.tunnel_id);

	/* Put tunnel on cleanup list */
	l2tp_tunnel_act_cleanup(tunnel, sess, NULL);
}

static void l2tp_tunnel_act_requeue_sccrq(void *tun, void *session, void *arg3)
{
	struct l2tp_tunnel *tunnel = tun;

	l2tp_tunnel_log(tunnel, L2TP_FSM, LOG_DEBUG, "FSM: tunl %hu: requeuing SCCRQ", tunnel->status.tunnel_id);

	l2tp_tunnel_act_send_sccrq(tun, session, arg3);
}

static void l2tp_tunnel_act_tunopen(void *tun, void *session, void *arg3)
{
	struct l2tp_tunnel *tunnel = tun;

	l2tp_tunnel_up_ind(tunnel);
}

static void l2tp_tunnel_act_null(void *tun, void *session, void *arg3)
{
}

/*****************************************************************************
 * Control Connection Establishment state machine.
 * Ref RFC2661, 7.2.1
 *
 * This implementation has several extra events not defined in the RFC.
 * These events were needed to pass all control events through the state machine.
 * The new events are:-
 * OPEN_REQ  - after a new tunnel context is created, it is given an OPEN_REQ
 *	       event to start things off.
 * CLOSE_REQ - when a manager requests to delete a tunnel, this event is sent.
 *	       This was added to distinguish administrative requests to close
 *	       a tunnel from network events.
 * XPRT_DOWN - when the tunnel's transport layer goes down (message sequence
 *	       errors or lack of responses from peer) this event is generated.
 *	       This event was added to distinguish a local network decision
 *	       to shitdown a tunnel versus a protocol message.
 *
 * Note that all events are valid in any state, even though some events are
 * ignored in some states.
 *****************************************************************************/

/* CCE state machine states */
#define L2TP_CCE_STATE_IDLE		0
#define L2TP_CCE_STATE_WAITCTLREPLY	1
#define L2TP_CCE_STATE_WAITCTLCONN	2
#define L2TP_CCE_STATE_ESTABLISHED	3
#define L2TP_CCE_STATE_CLOSING		4
#define L2TP_CCE_STATE_COUNT		5

#define L2TP_CCE_STATE_NAMES		\
	{ "IDLE", "WAITCTLREPLY", "WAITCTLCONN", "ESTABLISHED", "CLOSING" }

#define STATE(state)			L2TP_CCE_STATE_##state
#define EVENT(event)			L2TP_CCE_EVENT_##event
#define ACTION(stem)			l2tp_tunnel_act_##stem

static const char *l2tp_cce_state_names[] = L2TP_CCE_STATE_NAMES;
static const char *l2tp_cce_event_names[] = L2TP_CCE_EVENT_NAMES;

static const struct usl_fsm_table l2tp_cce_fsm = {
	"CCE", 
	l2tp_tunnel_fsm_log,
	L2TP_CCE_STATE_COUNT,
	&l2tp_cce_state_names[0],
	L2TP_CCE_EVENT_COUNT, 
	&l2tp_cce_event_names[0], 
	{
	/* state		event			action			new state */
	{ STATE(IDLE), 		EVENT(OPEN_REQ),	ACTION(send_sccrq), 	STATE(WAITCTLREPLY) },
	{ STATE(IDLE),		EVENT(SCCRQ_ACCEPT),	ACTION(send_sccrp),	STATE(WAITCTLCONN) },
	{ STATE(IDLE),		EVENT(SCCRQ_DENY),	ACTION(send_stopccn),	STATE(IDLE) },
	{ STATE(IDLE),		EVENT(LOSETIE),		ACTION(send_stopccn),	STATE(IDLE) },
	{ STATE(IDLE),		EVENT(SCCRP_ACCEPT),	ACTION(send_stopccn),	STATE(IDLE) },
	{ STATE(IDLE),		EVENT(SCCRP_DENY),	ACTION(send_stopccn),	STATE(IDLE) },
	{ STATE(IDLE),		EVENT(SCCCN_ACCEPT),	ACTION(send_stopccn),	STATE(IDLE) },
	{ STATE(IDLE),		EVENT(SCCCN_DENY),	ACTION(send_stopccn),	STATE(IDLE) },
	{ STATE(IDLE),		EVENT(STOPCCN),		ACTION(cleanup),	STATE(CLOSING) },
	{ STATE(IDLE),		EVENT(CLOSE_REQ),	ACTION(cleanup),	STATE(CLOSING) },
	{ STATE(IDLE),		EVENT(XPRT_DOWN),	ACTION(cleanup),	STATE(CLOSING) },
	{ STATE(WAITCTLREPLY),	EVENT(OPEN_REQ),	ACTION(null),		STATE(CLOSING) },
	{ STATE(WAITCTLREPLY),	EVENT(SCCRP_ACCEPT),	ACTION(send_scccn),	STATE(ESTABLISHED) },
	{ STATE(WAITCTLREPLY),	EVENT(SCCRP_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLREPLY),	EVENT(LOSETIE),		ACTION(requeue_sccrq),	STATE(CLOSING) },
	{ STATE(WAITCTLREPLY),	EVENT(SCCCN_ACCEPT),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLREPLY),	EVENT(SCCCN_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLREPLY),	EVENT(STOPCCN),		ACTION(cleanup),	STATE(CLOSING) },
	{ STATE(WAITCTLREPLY),	EVENT(CLOSE_REQ),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLREPLY),	EVENT(XPRT_DOWN),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLCONN),	EVENT(OPEN_REQ),	ACTION(null),		STATE(WAITCTLCONN) },
	{ STATE(WAITCTLCONN),	EVENT(SCCRP_ACCEPT),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLCONN),	EVENT(SCCRP_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLCONN),	EVENT(SCCRQ_ACCEPT),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLCONN),	EVENT(SCCRQ_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLCONN),	EVENT(LOSETIE),		ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLCONN),	EVENT(SCCCN_ACCEPT),	ACTION(tunopen),	STATE(ESTABLISHED) },
	{ STATE(WAITCTLCONN),	EVENT(SCCCN_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLCONN),	EVENT(STOPCCN),		ACTION(cleanup),	STATE(CLOSING) },
	{ STATE(WAITCTLCONN),	EVENT(CLOSE_REQ),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(WAITCTLCONN),	EVENT(XPRT_DOWN),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(OPEN_REQ),	ACTION(tunopen),	STATE(ESTABLISHED) },
	{ STATE(ESTABLISHED),	EVENT(SCCRQ_ACCEPT),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(SCCRQ_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(LOSETIE),		ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(SCCRP_ACCEPT),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(SCCRP_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(SCCCN_ACCEPT),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(SCCCN_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(STOPCCN),		ACTION(cleanup),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(CLOSE_REQ),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(ESTABLISHED),	EVENT(XPRT_DOWN),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(CLOSING), 	EVENT(OPEN_REQ),	ACTION(send_sccrq), 	STATE(WAITCTLREPLY) },
	{ STATE(CLOSING),	EVENT(SCCRQ_ACCEPT),	ACTION(send_sccrp),	STATE(WAITCTLCONN) },
	{ STATE(CLOSING),	EVENT(SCCRQ_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(CLOSING),	EVENT(LOSETIE),		ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(CLOSING),	EVENT(SCCRP_ACCEPT),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(CLOSING),	EVENT(SCCRP_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(CLOSING),	EVENT(SCCCN_ACCEPT),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(CLOSING),	EVENT(SCCCN_DENY),	ACTION(send_stopccn),	STATE(CLOSING) },
	{ STATE(CLOSING),	EVENT(STOPCCN),		ACTION(null),		STATE(CLOSING) },
	{ STATE(CLOSING),	EVENT(CLOSE_REQ),	ACTION(null),		STATE(CLOSING) },
	{ STATE(CLOSING),	EVENT(XPRT_DOWN),	ACTION(null),		STATE(CLOSING) },
	{ 0,			0,			NULL,			0 }
	}
};

#undef STATE
#undef EVENT
#undef ACTION

/*****************************************************************************
 * Internal implementation 
 *****************************************************************************/

static int l2tp_tunnel_store_avps(struct l2tp_tunnel *tunnel, struct l2tp_avp_desc *avps)
{
	int avp_len;

	if (avps[L2TP_AVP_TYPE_HOST_NAME].value != NULL) {
		if (tunnel->peer.host_name != NULL) {
			free(tunnel->peer.host_name);
		}
		avp_len = avps[L2TP_AVP_TYPE_HOST_NAME].value_len;
		tunnel->peer.host_name = malloc(avp_len + 1);
		if (tunnel->peer.host_name == NULL) {
			l2tp_stats.no_avp_resources++;
			return -ENOMEM;
		}
		memcpy(&tunnel->peer.host_name[0], avps[L2TP_AVP_TYPE_HOST_NAME].value, avp_len);
		tunnel->peer.host_name->string[avp_len] = '\0';
	}
	if (avps[L2TP_AVP_TYPE_PROTOCOL_VERSION].value != NULL) {
		tunnel->peer.protocol_version.ver = avps[L2TP_AVP_TYPE_PROTOCOL_VERSION].value->protocol_version.ver;
		tunnel->peer.protocol_version.rev = avps[L2TP_AVP_TYPE_PROTOCOL_VERSION].value->protocol_version.rev;
	}
	if (avps[L2TP_AVP_TYPE_FRAMING_CAP].value != NULL) {
		tunnel->peer.framing_cap.value = avps[L2TP_AVP_TYPE_FRAMING_CAP].value->framing_cap.value;
	}
	if (avps[L2TP_AVP_TYPE_BEARER_CAP].value != NULL) {
		tunnel->peer.bearer_cap.value = avps[L2TP_AVP_TYPE_BEARER_CAP].value->bearer_cap.value;
	}
	if (avps[L2TP_AVP_TYPE_RX_WINDOW_SIZE].value != NULL) {
		tunnel->peer.rx_window_size.value = avps[L2TP_AVP_TYPE_RX_WINDOW_SIZE].value->rx_window_size.value;
	}
	if (avps[L2TP_AVP_TYPE_FIRMWARE_REVISION].value != NULL) {
		tunnel->peer.firmware_revision.value = avps[L2TP_AVP_TYPE_FIRMWARE_REVISION].value->firmware_revision.value;
	}
	if (avps[L2TP_AVP_TYPE_VENDOR_NAME].value != NULL) {
		if (tunnel->peer.vendor_name != NULL) {
			free(tunnel->peer.vendor_name);
		}
		avp_len = avps[L2TP_AVP_TYPE_VENDOR_NAME].value_len;
		tunnel->peer.vendor_name = malloc(avp_len + 1);
		if (tunnel->peer.vendor_name == NULL) {
			l2tp_stats.no_avp_resources++;
			return -ENOMEM;
		}
		memcpy(&tunnel->peer.vendor_name[0], avps[L2TP_AVP_TYPE_VENDOR_NAME].value, avp_len);
		tunnel->peer.vendor_name->string[avp_len] = '\0';
	}
	if (avps[L2TP_AVP_TYPE_TIEBREAKER].value != NULL) {
		memcpy(&tunnel->peer.tiebreaker.value[0], avps[L2TP_AVP_TYPE_TIEBREAKER].value, sizeof(tunnel->peer.tiebreaker.value));
		tunnel->peer.have_tiebreaker = 1;
	}
	if (avps[L2TP_AVP_TYPE_CHALLENGE].value != NULL) {
		if (tunnel->peer.challenge != NULL) {
			free(tunnel->peer.challenge);
		}
		avp_len = avps[L2TP_AVP_TYPE_CHALLENGE].value_len;
		tunnel->peer.challenge = malloc(avp_len);
		if (tunnel->peer.challenge == NULL) {
			l2tp_stats.no_avp_resources++;
			return -ENOMEM;
		}
		memcpy(tunnel->peer.challenge, avps[L2TP_AVP_TYPE_CHALLENGE].value, avp_len);
		tunnel->peer.challenge_len = avp_len;
	}
	if (avps[L2TP_AVP_TYPE_CHALLENGE_RESPONSE].value != NULL) {
		if (tunnel->peer.challenge_response != NULL) {
			free(tunnel->peer.challenge_response);
		}
		avp_len = avps[L2TP_AVP_TYPE_CHALLENGE_RESPONSE].value_len;
		tunnel->peer.challenge_response = malloc(avp_len);
		if (tunnel->peer.challenge_response == NULL) {
			l2tp_stats.no_avp_resources++;
			return -ENOMEM;
		}
		memcpy(tunnel->peer.challenge_response, avps[L2TP_AVP_TYPE_CHALLENGE_RESPONSE].value, avp_len);
	}
	if (avps[L2TP_AVP_TYPE_RESULT_CODE].value != NULL) {
		if (tunnel->peer.result_code != NULL) {
			free(tunnel->peer.result_code);
		}
		avp_len = avps[L2TP_AVP_TYPE_RESULT_CODE].value_len;
		if (avp_len < 4) avp_len = 4;
		tunnel->peer.result_code = malloc(avp_len + 1);
		memcpy(tunnel->peer.result_code, avps[L2TP_AVP_TYPE_RESULT_CODE].value, avp_len);
		tunnel->peer.result_code->error_message[avp_len - 4] = '\0';
	}
	

	return 0;
}

/* Verify the stored challenge and challenge response against the tunnel secret.
 */
static int l2tp_tunnel_peer_challenge_ok(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel,
					 int msg_type)
{
	struct l2tp_avp_challenge_response challenge_response;

	if (msg_type == 0) {
		/* Don't need to check */
		return 0;
	}

	if (tunnel->peer.challenge_response == NULL) {
		/* No challenge_response AVP received */
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_BAD_PROTOCOL, 0,
				       "no challenge response AVP received");
		return -EPROTO;
	}
	if (tunnel->my_challenge == NULL) {
		/* No challenge data recorded */
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_BAD_PROTOCOL, 0,
				       "no challenge data available");
		return -EPROTO;
	}
	if (tunnel->config.secret == NULL) {
		/* No tunnel secret */
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_AUTH_FAILED, 0,
				       "no tunnel secret available");
		return -EINVAL;
	}

	/* Build the challenge response that we expect to receive */
	l2tp_tunnel_build_challenge_response(tunnel, msg_type, 
					     ((void *) tunnel->my_challenge), tunnel->my_challenge_len,
					     &challenge_response);

	/* Compare with the challenge response actually received */
	if (memcmp(&tunnel->peer.challenge_response->value[0],
		   &challenge_response.value[0],
		   sizeof(challenge_response)) != 0) {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_AUTH_FAILED, 0, NULL);
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_WARNING, "PROTO: tunl %hu: tunnel authentication failed", tunnel->status.tunnel_id);
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: challresp expected: %s", tunnel->status.tunnel_id, 
				l2tp_buffer_hexify(&challenge_response.value[0], sizeof(challenge_response)));
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: challresp received: %s", tunnel->status.tunnel_id, 
				l2tp_buffer_hexify(&tunnel->peer.challenge_response->value[0], sizeof(tunnel->peer.challenge_response->value)));
		return -EPERM;
	} else {
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: tunnel authentication succeeded", tunnel->status.tunnel_id);
	}

	return 0;
}

/* Simple authentication means just check that there's a configured peer
 * profile that matches the peer's hostname or IP address.
 */
static int l2tp_tunnel_peer_ok(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel)
{
	int result = -EPERM;
	struct l2tp_peer_profile *profile;

	if (tunnel->peer.host_name != NULL) {
		profile = l2tp_peer_profile_find(&tunnel->peer.host_name->string[0]);
		if (profile != NULL) {
			result = 0;
			goto out;
		}
	}

	profile = l2tp_peer_profile_find_by_addr(peer->if_peer_addr);
	if (profile != NULL) {
		result = 0;
	}

out:
	if (result == 0) {
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, 
				"PROTO: tunl %hu: tunnel authentication succeeded", 
				tunnel->status.tunnel_id);
	} else {
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_WARNING, 
				"PROTO: tunl %hu: tunnel authentication failed: no matching peer profile", 
				tunnel->status.tunnel_id);
	}

	return result;
}

static int l2tp_tunnel_check_authorized(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel,
					int msg_type)
{
	int result = -EPERM;

	/* If we sent a challenge, check the challenge response regardless of tunnel
	 * authentication settings.
	 */
	if ((tunnel->my_challenge != NULL) && (tunnel->peer.challenge_response != NULL)) {
		result = l2tp_tunnel_peer_challenge_ok(peer, tunnel, msg_type);
		goto out;
	}

	/* Else use configured authentication setting.
	 */
	switch (tunnel->config.auth_mode) {
	case L2TP_API_TUNNEL_AUTH_MODE_NONE:
		result = 0;
		break;
	case L2TP_API_TUNNEL_AUTH_MODE_SIMPLE:
		result = l2tp_tunnel_peer_ok(peer, tunnel);
		break;
	case L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE:
		result = l2tp_tunnel_peer_challenge_ok(peer, tunnel, msg_type);
		break;
	/* NODEFAULT */
	}
		
out:
	if (result < 0) {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_AUTH_FAILED, 0, NULL);
	}

	return result;
}

static int l2tp_tunnel_check_for_peer_collision(struct l2tp_tunnel *tunnel)
{
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	struct l2tp_peer *peer2;
	struct l2tp_tunnel *tunnel2;
	struct l2tp_peer *peer;

	/* Handle peer collision. Ref RFC2661 Sect 4.3.3, Tiebreaker AVP.
	 *
	 *      The Tie Breaker AVP, Attribute Type 5, indicates that the sender
	 *      wishes a single tunnel to exist between the given LAC-LNS pair.
	 *
	 *      The Tie Breaker Value is an 8 octet value that is used to choose a
	 *      single tunnel where both LAC and LNS request a tunnel
	 *      concurrently. The recipient of a SCCRQ must check to see if a
	 *      SCCRQ has been sent to the peer, and if so, must compare its Tie
	 *      Breaker value with the received one. The lower value "wins", and
	 *      the "loser" MUST silently discard its tunnel. In the case where a
	 *      tie breaker is present on both sides, and the value is equal, both
	 *      sides MUST discard their tunnels.
	 *
	 *      If a tie breaker is received, and an outstanding SCCRQ had no tie
	 *      breaker value, the initiator which included the Tie Breaker AVP
	 *      "wins". If neither side issues a tie breaker, then two separate
	 *      tunnels are opened.
	 *
	 */

	peer = tunnel->my_peer;
	usl_list_for_each(walk, tmp, &l2tp_tunnel_list) {
		tunnel2 = usl_list_entry(walk, struct l2tp_tunnel, list);
		peer2 = tunnel2->my_peer;
		if ((tunnel2 != tunnel) &&
		    (tunnel2->sccrq_sent) &&
		    (peer2 != NULL) && 
		    (tunnel2->config.local_addr.sin_addr.s_addr == tunnel->config.local_addr.sin_addr.s_addr) &&
		    (tunnel2->config.peer_addr.sin_addr.s_addr == tunnel->config.peer_addr.sin_addr.s_addr)) {
			/* We found another tunnel between same two IP addresses. */
			L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: looking at tunl %hu which is between same IP as this tunnel",
				   __func__, tunnel->status.tunnel_id, tunnel2->status.tunnel_id);
			if (tunnel->peer.have_tiebreaker) {
				/* Case 1: SCCRQ from peer included a tiebreaker.
				 * For each tunnel between the same two IP addresses, compare tiebreakers.
				 * If other tunnel doesn't have a tiebreaker, it loses.
				 */
				if (tunnel2->config.use_tiebreaker) {
					L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: tiebreaker %s", __func__,
						   tunnel2->status.tunnel_id, l2tp_buffer_hexify(&tunnel2->status.tiebreaker.value[0], 8));
					L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: received tiebreaker %s", __func__,
						   tunnel->status.tunnel_id, l2tp_buffer_hexify(&tunnel->peer.tiebreaker.value[0], 8));
												
					if (memcmp(&tunnel->peer.tiebreaker.value[0], 
						   &tunnel2->status.tiebreaker.value[0], 
						   sizeof(tunnel->peer.tiebreaker.value)) >= 0) {
						/* received tiebreaker >= first tiebreaker so we lose */
						l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, 
								"PROTO: tunl %hu: lost tiebreaker with tunl %hu, closing",
								tunnel->status.tunnel_id, tunnel2->status.tunnel_id);
						l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_LOSETIE);
						goto out;
					} else {
						/* received tiebreaker < first tiebreaker so we win. Close the older tunnel. */
						l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, 
								"PROTO: tunl %hu: won tiebreaker with tunl %hu, closing other tunnel",
								tunnel->status.tunnel_id, tunnel2->status.tunnel_id);
						l2tp_tunnel_handle_event(tunnel2, L2TP_CCE_EVENT_LOSETIE);
					}
				} else {
					/* if the other tunnel didn't include a tiebreaker, it loses */
					l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, 
							"PROTO: tunl %hu: won tiebreaker with tunl %hu, closing other tunnel",
							tunnel->status.tunnel_id, tunnel2->status.tunnel_id);
					l2tp_tunnel_handle_event(tunnel2, L2TP_CCE_EVENT_LOSETIE);
				}
			} else {
				/* Case 2: SCCRQ from peer did not include a tiebreaker.
				 * If another tunnel between the same IP addresses included a tiebreaker, we lose.
				 * If other tunnel doesn't have a tiebreaker either, it's OK to continue.
				 */
				if (tunnel2->config.use_tiebreaker) {
					/* other tunnel sent a tiebreaker so we lose */
					l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, 
							"PROTO: tunl %hu: lost tiebreaker with tunl %hu, closing",
							tunnel->status.tunnel_id, tunnel2->status.tunnel_id);
					l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_LOSETIE);
					goto out;
				}
			}
		}
	}

	return 0;

out:
	return -EPERM;
}

/*****************************************************************************
 * Tunnel message handlers, called when control messages are received.. 
 * There is one routine per message type.
 *****************************************************************************/

static void l2tp_tunnel_handle_msg_sccrq(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel,
					 uint16_t session_id, struct l2tp_avp_desc *avps)
{
	int result = 0;

	if ((avps[L2TP_AVP_TYPE_HOST_NAME].value == NULL) ||
	    (avps[L2TP_AVP_TYPE_PROTOCOL_VERSION].value == NULL) ||
	    (avps[L2TP_AVP_TYPE_FRAMING_CAP].value == NULL) ||
	    (avps[L2TP_AVP_TYPE_TUNNEL_ID].value == NULL)) {
		l2tp_stats.messages[L2TP_AVP_MSG_SCCRQ].rx_bad++;
		goto error;
	}

	tunnel->status.created_by_admin = 0;
	tunnel->peer.tunnel_id = avps[L2TP_AVP_TYPE_TUNNEL_ID].value->tunnel_id;

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: SCCRQ received from peer %hu", 
			tunnel->status.tunnel_id, tunnel->peer.tunnel_id.value);
	result = l2tp_xprt_set_peer_tunnel_id(tunnel->xprt, tunnel->peer.tunnel_id.value);
	if (result < 0) {
		l2tp_stats.socket_errors++;
		goto error;
	}

	/* This value will be overwritten by the value in RX_WINDOW_SIZE, if present */
	tunnel->peer.rx_window_size.value = 4;

	result = l2tp_tunnel_store_avps(tunnel, avps);
	l2tp_stats.messages[L2TP_AVP_MSG_SCCRQ].rx++;
	if (result < 0) {
		l2tp_stats.no_control_frame_resources++;
		goto error;
	}

	/* Check if administrator allows remote tunnel creation */
	result = l2tp_tunnel_is_ok_to_create(tunnel, FALSE);
	if (result < 0) {
		goto error;
	}

	/* Check for peer collision, as per RFC2661, section 4.3.3, page 22. 
	 */
	result = l2tp_tunnel_check_for_peer_collision(tunnel);
	if (result < 0) {
		goto error;
	}

	/* Check if tunnel is authorized */
	if (l2tp_tunnel_check_authorized(peer, tunnel, 0) < 0) {
		l2tp_stats.auth_fails++;
		l2tp_tunnel_log_error(tunnel, tunnel->result_code, tunnel->result_code_len);
		goto error;
	}

	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_SCCRQ_ACCEPT);
	return;

error:
	l2tp_stats.tunnel_setup_failures++;
	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_SCCRQ_DENY);
}

static void l2tp_tunnel_handle_msg_sccrp(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel,
					 uint16_t session_id, struct l2tp_avp_desc *avps)
{
	int result = 0;

	if ((avps[L2TP_AVP_TYPE_HOST_NAME].value == NULL) ||
	    (avps[L2TP_AVP_TYPE_PROTOCOL_VERSION].value == NULL) ||
	    (avps[L2TP_AVP_TYPE_FRAMING_CAP].value == NULL) ||
	    (avps[L2TP_AVP_TYPE_TUNNEL_ID].value == NULL)) {
		l2tp_stats.messages[L2TP_AVP_MSG_SCCRP].rx_bad++;
		goto error;
	}

       	tunnel->peer.tunnel_id = avps[L2TP_AVP_TYPE_TUNNEL_ID].value->tunnel_id;

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: SCCRP received from peer %hu", 
			tunnel->status.tunnel_id, tunnel->peer.tunnel_id.value);
	result = l2tp_xprt_set_peer_tunnel_id(tunnel->xprt, tunnel->peer.tunnel_id.value);
	if (result < 0) {
		l2tp_stats.socket_errors++;
		goto error;
	}

	/* This value will be overwritten by the value in RX_WINDOW_SIZE, if present */
	tunnel->peer.rx_window_size.value = 4;

	result = l2tp_tunnel_store_avps(tunnel, avps);
	l2tp_stats.messages[L2TP_AVP_MSG_SCCRP].rx++;
	if (result < 0) {
		l2tp_stats.no_control_frame_resources++;
		goto error;
	}

	/* Check if tunnel is authorized */
	if (l2tp_tunnel_check_authorized(peer, tunnel, L2TP_AVP_MSG_SCCRP) < 0) {
		l2tp_stats.auth_fails++;
		l2tp_tunnel_log_error(tunnel, tunnel->result_code, tunnel->result_code_len);
		goto error;
	}
	
	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_SCCRP_ACCEPT);
	return;

error:
	l2tp_stats.tunnel_setup_failures++;
	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_SCCRP_DENY);
}

static void l2tp_tunnel_handle_msg_scccn(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel,
					 uint16_t session_id, struct l2tp_avp_desc *avps)
{
	int result = 0;

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: SCCCN received from peer %hu", 
			tunnel->status.tunnel_id, tunnel->peer.tunnel_id.value);

	result = l2tp_tunnel_store_avps(tunnel, avps);
	l2tp_stats.messages[L2TP_AVP_MSG_SCCCN].rx++;
	if (result < 0) {
		l2tp_stats.no_control_frame_resources++;
		goto error;
	}

	/* Check if tunnel is authorized */
	if (l2tp_tunnel_check_authorized(peer, tunnel, L2TP_AVP_MSG_SCCCN) < 0) {
		l2tp_stats.auth_fails++;
		l2tp_tunnel_log_error(tunnel, tunnel->result_code, tunnel->result_code_len);
		goto error;
	}

	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_SCCCN_ACCEPT);
	return;

error:
	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_SCCCN_DENY);
	l2tp_stats.tunnel_setup_failures++;
}

static void l2tp_tunnel_handle_msg_stopccn(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel,
					   uint16_t session_id, struct l2tp_avp_desc *avps)
{
	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: STOPCCN received", 
			tunnel->status.tunnel_id);
	L2TP_DEBUG(L2TP_FUNC, "%s: %s: use_count=%d", __func__, tunnel->fsmi.name, tunnel->status.use_count);

	if ((avps[L2TP_AVP_TYPE_TUNNEL_ID].value == NULL) ||
	    (avps[L2TP_AVP_TYPE_RESULT_CODE].value == NULL)) {
		l2tp_stats.messages[L2TP_AVP_MSG_STOPCCN].rx_bad++;
		return;
	}
	if (tunnel->peer.tunnel_id.value != 0) {
		if (avps[L2TP_AVP_TYPE_TUNNEL_ID].value->tunnel_id.value != tunnel->peer.tunnel_id.value) {
			l2tp_stats.mismatched_tunnel_ids++;
			L2TP_DEBUG(L2TP_DATA, "%s: tunl %hu: mismatched tunnels: got %hu, expected %hu",
				   __func__, tunnel->status.tunnel_id,
				   avps[L2TP_AVP_TYPE_TUNNEL_ID].value->tunnel_id.value,
				   tunnel->peer.tunnel_id.value);
			return;
		}
	} else {
		/* If the peer tunnel_id is still zero, this STOPCCN is being sent to reject the
		 * tunnel setup. We must give the peer tunnel_id to the transport in order that
		 * it may properly clean up the tunnel. (This peer tunnel_id is usually set
		 * when SCCRP is received.)
		 */
		L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: STOPCCN from peer %hu",
			   __func__, tunnel->status.tunnel_id,
			   avps[L2TP_AVP_TYPE_TUNNEL_ID].value->tunnel_id.value);
		tunnel->peer.tunnel_id = avps[L2TP_AVP_TYPE_TUNNEL_ID].value->tunnel_id;
		l2tp_xprt_set_peer_tunnel_id(tunnel->xprt, tunnel->peer.tunnel_id.value);
	}

	if (avps[L2TP_AVP_TYPE_RESULT_CODE].value != NULL) {
		if (tunnel->peer.result_code != NULL) {
			free(tunnel->peer.result_code);
		}
		tunnel->peer.result_code_len = avps[L2TP_AVP_TYPE_RESULT_CODE].value_len;
		tunnel->peer.result_code = malloc((tunnel->peer.result_code_len > sizeof(*tunnel->peer.result_code)) ?
						   tunnel->peer.result_code_len + 1 : sizeof(*tunnel->peer.result_code));
		if (tunnel->peer.result_code == NULL) {
			l2tp_stats.no_control_frame_resources++;
			goto out;
		}
		memset(tunnel->peer.result_code, 0, sizeof(*tunnel->peer.result_code));
		memcpy(tunnel->peer.result_code, avps[L2TP_AVP_TYPE_RESULT_CODE].value, tunnel->peer.result_code_len);
		if (tunnel->peer.result_code_len > sizeof(*tunnel->peer.result_code)) {
			tunnel->peer.result_code->error_message[tunnel->peer.result_code_len - sizeof(*tunnel->peer.result_code)] = '\0';
		}
	}

       	l2tp_tunnel_log_error(tunnel, tunnel->peer.result_code, tunnel->peer.result_code_len);
out:
	l2tp_stats.messages[L2TP_AVP_MSG_STOPCCN].rx++;
	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_STOPCCN);
}

static void l2tp_tunnel_handle_msg_null(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel,
					uint16_t session_id, struct l2tp_avp_desc *avps)
{
}

static void l2tp_tunnel_handle_msg_hello(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel,
					 uint16_t session_id, struct l2tp_avp_desc *avps)
{
	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "PROTO: tunl %hu: HELLO received from peer %hu", 
			tunnel->status.tunnel_id, tunnel->peer.tunnel_id.value);

	l2tp_stats.messages[L2TP_AVP_MSG_HELLO].rx++;
	l2tp_xprt_hello_rcvd(tunnel->xprt);
}

struct l2tp_msg_table_entry {
	void 		(*func)(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel, 
				uint16_t session_id, struct l2tp_avp_desc *avps);
};

/* Message handler table. Indexed by message type. Additional handlers
 * may be registered using a public API. 
 */
static struct l2tp_msg_table_entry l2tp_msg_table[L2TP_AVP_MSG_COUNT] = {
	{ l2tp_tunnel_handle_msg_null },
	{ l2tp_tunnel_handle_msg_sccrq },
	{ l2tp_tunnel_handle_msg_sccrp },
	{ l2tp_tunnel_handle_msg_scccn },
	{ l2tp_tunnel_handle_msg_stopccn },
	{ l2tp_tunnel_handle_msg_null },
	{ l2tp_tunnel_handle_msg_hello }
};

int l2tp_tunnel_register_message_handler(int msg_type,
					 void (*func)(struct l2tp_peer *peer, 
						      struct l2tp_tunnel *tunnel, 
						      uint16_t session_id, 
						      struct l2tp_avp_desc *avps))
{
	int result = -EINVAL;

	if ((msg_type > 0) && (msg_type <= L2TP_AVP_MSG_COUNT)) {
		if (l2tp_msg_table[msg_type].func != NULL) {
			result = -EEXIST;
			goto out;
		}
		l2tp_msg_table[msg_type].func = func;
		result = 0;
	}
out:
	return result;
}

void l2tp_tunnel_unregister_message_handler(int msg_type)
{
	if ((msg_type > 0) && (msg_type <= L2TP_AVP_MSG_COUNT)) {
		l2tp_msg_table[msg_type].func = NULL;
	}
}

/* We come here when the transport layer receives a valid L2TP control
 * frame.  The tunnel context is already known (derived from the
 * tunnel_id of the L2TP header).
 */
void l2tp_tunnel_recv(struct l2tp_tunnel *tunnel, uint16_t tunnel_id, 
		      uint16_t session_id, struct l2tp_packet *pkt)
{
	uint16_t msg_type;
	int result;
	struct l2tp_avp_desc avp_desc[L2TP_AVP_TYPE_NUM_AVPS];

	BUG_TRAP(tunnel == NULL);
	BUG_TRAP(pkt->num_bufs != 1);
	memset(&avp_desc, 0, sizeof(avp_desc));
	result = l2tp_avp_message_decode(pkt->iov[0].iov_len - pkt->avp_offset, 
					 pkt->iov[0].iov_base + pkt->avp_offset, 
					 &avp_desc[0], tunnel);
	if (result == -ENOMEM) {
		l2tp_stats.no_control_frame_resources++;
		goto out;
	} else if (result < 0) {
		l2tp_stats.bad_rcvd_frames++;
		goto out;
	} else {
		/* We now have a decoded set of AVP data in avp_desc.
		 * Deliver the parsed message to the tunnel handler who
		 * will work out what to do with it. 
		 */
		msg_type = avp_desc[0].value->message_type.type;
		if (msg_type <= L2TP_AVP_MSG_COUNT) {
			(*l2tp_msg_table[msg_type].func)(tunnel->my_peer, tunnel, session_id, &avp_desc[0]);
		} else {
			l2tp_stats.bad_rcvd_frames++;
			goto out;
		}
	}
out:
	return;
}

/*****************************************************************************
 * Event queue
 *
 * Events are short messages indicating the tunnel instance and the
 * event id. The messages are written to a pipe (socket), which is
 * serviced by the USL l2tp_tunnel_do_queued_event() callback.
 *****************************************************************************/

struct l2tp_tunnel_event_msg {
	uint16_t 	tunnel_id;
	int 		event;
};

static void l2tp_tunnel_do_queued_event(int fd, void *arg)
{
	struct l2tp_tunnel_event_msg msg;
	struct l2tp_tunnel *tunnel;
	int result;
	int bytes_read = 0;

	for (;;) {
		result = read(fd, ((void *) &msg) + bytes_read, sizeof(msg) - bytes_read);
		if (result <= 0) {
			if (errno == EINTR) {
				continue;
			}
			break;
		}
		bytes_read += result;
		if (bytes_read == sizeof(msg)) {
			tunnel = l2tp_tunnel_find_by_id(msg.tunnel_id);
			if (tunnel != NULL) {
				l2tp_tunnel_handle_event(tunnel, msg.event);
			} else {
				L2TP_DEBUG(L2TP_FUNC, "%s: tunl %hu: tunnel not found: discarded event %d", __func__, 
					   msg.tunnel_id, msg.event);
			}
			break;
		}
	}
}

int l2tp_tunnel_queue_event(uint16_t tunnel_id, int event)
{
	struct l2tp_tunnel_event_msg msg = { 0, };
	int result;

	msg.tunnel_id = tunnel_id;
	msg.event = event;

	result = write(l2tp_tunnel_event_pipe[1], &msg, sizeof(msg));
	if (result < 0) {
		l2tp_stats.event_queue_full_errors++;
		l2tp_log(LOG_ERR, "L2TP: tunnel event pipe full: event %d for tunnel %hu lost", event, tunnel_id);
	}

	return result;
}

/*****************************************************************************
 * Context management
 *****************************************************************************/

void l2tp_tunnel_inc_use_count(struct l2tp_tunnel *tunnel)
{
	tunnel->status.use_count++;
	L2TP_DEBUG(L2TP_FUNC, "%s: %s: count now %d", __func__, tunnel->fsmi.name, tunnel->status.use_count);
}

void l2tp_tunnel_dec_use_count(struct l2tp_tunnel *tunnel)
{
	tunnel->status.use_count--;
	L2TP_DEBUG(L2TP_FUNC, "%s: %s: count now %d", __func__, tunnel->fsmi.name, tunnel->status.use_count);
	if (tunnel->status.use_count < 0) {
		l2tp_log(LOG_ERR, "tunl %hu: use count gone negative! Caller %p",
			 tunnel->status.tunnel_id, __builtin_return_address(0));
		tunnel->status.use_count = 0;
	} else if (tunnel->status.use_count == 0) {
		l2tp_tunnel_unlink(tunnel, 0);
	}
}

static int l2tp_tunnel_is_ok_to_create(struct l2tp_tunnel *tunnel, int local_request)
{
	int result = 0;

	/* Check if new tunnel creation is administratively disabled */
	if (((local_request) && (l2tp_tunnel_deny_local_creates)) ||
	    ((!local_request) && (l2tp_tunnel_deny_remote_creates)) ||
	    (l2tp_tunnel_drain)) {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, 0,
				       "new tunnel creation administratively disabled");
		l2tp_tunnel_log(tunnel, L2TP_API, LOG_INFO, "API: tunl %hu: rejected: new tunnel creation is disabled", tunnel->status.tunnel_id);
		result = -L2TP_ERR_TUNNEL_CREATE_ADMIN_DISABLED;
		goto out;
	}

	/* Check if global tunnel count will be exceeded */
	if (l2tp_tunnel_max_count != 0) {
		if (l2tp_tunnel_count > l2tp_tunnel_max_count) {
			l2tp_stats.too_many_tunnels++;
			l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, 0,
					       "too many tunnels");
			l2tp_tunnel_log(tunnel, L2TP_API, LOG_INFO, "API: tunl %hu: rejected: tunnel limit (%d) exceeded", 
					tunnel->status.tunnel_id, l2tp_tunnel_max_count);
			result = -L2TP_ERR_TUNNEL_LIMIT_EXCEEDED;
			goto out;
		}
	}

out:
	return result;
}

/* Called to recreate a persistent tunnel. 
 * Note that the regular RPC management interface is used to recreate the tunnel,
 * although in this case it is called directly with a locally-built req struct.
 */
static void l2tp_tunnel_recreate(struct l2tp_tunnel *tunnel)
{
	int result;
	bool_t req_ok;
	struct l2tp_api_tunnel_msg_data msg;
	struct l2tp_tunnel *new_tunnel;
	uint16_t new_tunnel_id;
	int index;
	struct usl_list_head *walk;

	/* Make sure the tunnel being cloned has no timers still running */
	if (tunnel->setup_timer != NULL) {
		usl_timer_delete(tunnel->setup_timer);
		tunnel->setup_timer = NULL;
	}
	if (tunnel->cleanup_timer != NULL) {
		usl_timer_delete(tunnel->cleanup_timer);
		tunnel->cleanup_timer = NULL;
	}

	/* Clean up state of the old tunnel before we recreate */
	if (tunnel->xprt != NULL) {
		(void) l2tp_xprt_tunnel_delete(tunnel->xprt);
		if (l2tp_net_socket_disconnect_hook != NULL) {
			(*l2tp_net_socket_disconnect_hook)(tunnel, &tunnel->config.local_addr, &tunnel->config.peer_addr);
		}
		tunnel->xprt = NULL;
	}
	if (tunnel->fd >= 0) {
		close(tunnel->fd);
		tunnel->fd = -1;
	}
	tunnel->down = -1;
	tunnel->up = 0;
	tunnel->fd_is_connected = 0;

	/* Clone the tunnel */
	result = l2tp_tunnel_clone(tunnel, &msg);
	if (result < 0) {
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_ERR, "tunl %hu: unable to clone params", tunnel->status.tunnel_id);
		l2tp_stats.tunnel_setup_failures++;
		goto out;
	}
	
	/* Remove original tunnel from tunnel lists */
	usl_list_del_init(&tunnel->list);
	if (!usl_hlist_unhashed(&tunnel->id_hlist)) {
		usl_hlist_del_init(&tunnel->id_hlist);
	}
	if (!usl_hlist_unhashed(&tunnel->name_hlist)) {
		usl_hlist_del_init(&tunnel->name_hlist);
	}

	l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_DEBUG, "tunl %hu: recreating persistent tunnel", tunnel->status.tunnel_id);
	req_ok = l2tp_tunnel_create_1_svc(msg, &result, NULL);
	if ((!req_ok) || (result < 0)) {
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_ERR, "tunl %hu: failed to recreate: %s", 
				tunnel->status.tunnel_id, l2tp_strerror(-result));
		l2tp_stats.tunnel_setup_failures++;
	}

	/* Find the new tunnel */
	new_tunnel_id = (uint16_t) (result & 0xffff);
	l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_DEBUG, "tunl %hu: created new tunnel %hu", tunnel->status.tunnel_id, new_tunnel_id);
	new_tunnel = l2tp_tunnel_find_by_id(new_tunnel_id);
	if (new_tunnel == NULL) {
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_ERR, "tunl %hu: failed to recreate", new_tunnel_id);
		l2tp_stats.tunnel_setup_failures++;
		goto out;
	}

	/* Move the old tunnel's sessions to the new one.
	 * We need to fixup the list head pointers here.
	 */
	new_tunnel->session_list = tunnel->session_list;
	walk = &new_tunnel->session_list;
	for (;;) {
		if (walk->prev == &tunnel->session_list) {
			walk->prev = &new_tunnel->session_list;
		}
		if (walk->next == &tunnel->session_list) {
			walk->next = &new_tunnel->session_list;
			break;
		}
		walk = walk->next;
	}
	/* Sessions will re-add themselves to the tunnel's hashed list
	 * in l2tp_session_tunnel_updown_event().
	 */
	for (index = 0; index < L2TP_TUNNEL_HASH_SIZE; index++) {
		USL_HLIST_HEAD_INIT(&new_tunnel->session_id_hlist[index]);
	}

	/* Forget the sessions in the old tunnel */
	USL_LIST_HEAD_INIT(&tunnel->session_list);
	for (index = 0; index < L2TP_TUNNEL_HASH_SIZE; index++) {
		USL_HLIST_HEAD_INIT(&tunnel->session_id_hlist[index]);
	}

out:
	return;
}

static void l2tp_tunnel_free(struct l2tp_tunnel *tunnel)
{
	L2TP_DEBUG(L2TP_FUNC, "%s: tunl %hu", __func__, tunnel->status.tunnel_id);
	if (tunnel->status.use_count != 0) {
		l2tp_log(LOG_ERR, "tunl %hu: free when use_count=%d", tunnel->status.tunnel_id, tunnel->status.use_count);
		return;
	}

	l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu: deleting context", tunnel->status.tunnel_id);

	if (tunnel->setup_timer != NULL) {
		usl_timer_delete(tunnel->setup_timer);
	}
	if (tunnel->cleanup_timer != NULL) {
		usl_timer_delete(tunnel->cleanup_timer);
	}

	if (tunnel->xprt != NULL) {
		(void) l2tp_xprt_tunnel_delete(tunnel->xprt);
		if (l2tp_net_socket_disconnect_hook != NULL) {
			(*l2tp_net_socket_disconnect_hook)(tunnel, &tunnel->config.local_addr, &tunnel->config.peer_addr);
		}
	}

	/* Delete all allocate AVPs */
	if (tunnel->peer.host_name != NULL) {
		free(tunnel->peer.host_name);
	}
	if (tunnel->peer.challenge != NULL) {
		free(tunnel->peer.challenge);
	}
	if (tunnel->peer.challenge_response != NULL) {
		free(tunnel->peer.challenge_response);
	}
	if (tunnel->peer.vendor_name != NULL) {
		free(tunnel->peer.vendor_name);
	}
	if (tunnel->peer.result_code != NULL) {
		free(tunnel->peer.result_code);
	}
	if (tunnel->my_challenge != NULL) {
		free(tunnel->my_challenge);
	}
	if (tunnel->result_code != NULL) {
		free(tunnel->result_code);
	}

	/* Delete config */
	if (tunnel->config.secret != NULL) {
		free(tunnel->config.secret);
	}
	if (tunnel->config.host_name != NULL) {
		free(tunnel->config.host_name);
	}
	if (tunnel->config.peer_profile_name != NULL) {
		free(tunnel->config.peer_profile_name);
	}
	if (tunnel->config.tunnel_profile_name != NULL) {
		free(tunnel->config.tunnel_profile_name);
	}
	if (tunnel->config.session_profile_name != NULL) {
		free(tunnel->config.session_profile_name);
	}
	if (tunnel->config.ppp_profile_name != NULL) {
		free(tunnel->config.ppp_profile_name);
	}
	if (tunnel->config.interface_name != NULL) {
		free(tunnel->config.interface_name);
	}
	if (tunnel->config.tunnel_name != NULL) {
		free(tunnel->config.tunnel_name);
	}

	if (tunnel->status.create_time != NULL) {
		free(tunnel->status.create_time);
	}

	if (tunnel->my_peer != NULL) {
		l2tp_peer_dec_use_count(tunnel->my_peer);
	}

	l2tp_tunnel_count--;

	USL_POISON_MEMORY(tunnel, 0xee, sizeof(*tunnel));
	free(tunnel);
}

struct l2tp_tunnel *l2tp_tunnel_alloc(uint16_t tunnel_id, char *tunnel_profile_name, char *peer_profile_name, int created_by_admin, int *ret)
{
	struct l2tp_tunnel *tunnel = NULL;
	int result = 0;

	/* Allocate a new tunnel */
	tunnel = calloc(1, sizeof(struct l2tp_tunnel));
	if (tunnel == NULL) {
		result = -ENOMEM;
		l2tp_stats.no_tunnel_resources++;
		goto out;
	}

	l2tp_tunnel_count++;

	if (tunnel_id == 0) {
		tunnel_id = l2tp_tunnel_allocate_id();
		if (tunnel_id == 0) {
			l2tp_stats.tunnel_id_alloc_fails++;
			result = -L2TP_ERR_TUNNEL_ID_ALLOC_FAILURE;
			goto out;
		}
	}
	tunnel->status.tunnel_id = tunnel_id;

	tunnel->fsmi.table = &l2tp_cce_fsm;
	sprintf(&tunnel->fsmi.name[0], "%hu", tunnel_id);

	tunnel->down = -1;
	tunnel->up = 0;
	tunnel->fd = -1;
	tunnel->fd_is_connected = 0;

#ifdef L2TP_FEATURE_LAC_SUPPORT
	tunnel->we_can_be_lac = -1;
	tunnel->status.mode = L2TP_API_TUNNEL_MODE_LAC;
#endif
#ifdef L2TP_FEATURE_LNS_SUPPORT
	tunnel->we_can_be_lns = -1;
	tunnel->status.mode = L2TP_API_TUNNEL_MODE_LNS;
#endif

	/* If we support both LAC and LNS, default tunnel mode is LAC
	 * if we were created by management request or LNS if we were
	 * created by network request. May be overridden later when
	 * the tunnel is associated with a peer.
	 */
#if defined(L2TP_FEATURE_LAC_SUPPORT) && defined(L2TP_FEATURE_LNS_SUPPORT)
	tunnel->status.mode = created_by_admin ? L2TP_API_TUNNEL_MODE_LAC : L2TP_API_TUNNEL_MODE_LNS;
#endif
	tunnel->status.created_by_admin = created_by_admin;

	/* Fill with default values. Use profile if one is specified. */
	result = l2tp_tunnel_param_defaults(tunnel, tunnel_profile_name, peer_profile_name);
	if (result < 0) {
		goto out;
	}

	tunnel->status.create_time = l2tp_system_time();
	if (tunnel->status.create_time == NULL) {
		result = -ENOMEM;
		goto out;
	}

	USL_HLIST_NODE_INIT(&tunnel->id_hlist);
	USL_HLIST_NODE_INIT(&tunnel->name_hlist);
	USL_LIST_HEAD_INIT(&tunnel->session_list);

	l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %hu: allocated context using profile '%s'%s", 
			tunnel->status.tunnel_id, tunnel->config.tunnel_profile_name,
			tunnel->status.created_by_admin ? "" : ", created by network request");

	/* Bump the tunnel's use count. When this drops to zero, the tunnel context will be freed */
	l2tp_tunnel_inc_use_count(tunnel);

	*ret = result;
	return tunnel;

out:
	*ret = result;
	if (tunnel != NULL) {
		l2tp_tunnel_free(tunnel);
	}
	return NULL;
}

void l2tp_tunnel_link(struct l2tp_tunnel *tunnel)
{
	L2TP_DEBUG(L2TP_FUNC, "%s: %s", __func__, tunnel->fsmi.name);

	usl_list_add(&tunnel->list, &l2tp_tunnel_list);
	usl_hlist_add_head(&tunnel->id_hlist, l2tp_tunnel_id_hash(tunnel->status.tunnel_id));
	if (tunnel->config.tunnel_name != NULL) {
		usl_hlist_add_head(&tunnel->name_hlist, l2tp_tunnel_name_hash(tunnel->config.tunnel_name));
	}
}

void l2tp_tunnel_unlink(struct l2tp_tunnel *tunnel, int force)
{
	L2TP_DEBUG(L2TP_FUNC, "%s: %s: use_count=%d", __func__, tunnel->fsmi.name, tunnel->status.use_count);

	if ((tunnel->status.use_count == 0) || force) {
		usl_list_del(&tunnel->list);
		if (!usl_hlist_unhashed(&tunnel->id_hlist)) {
			usl_hlist_del(&tunnel->id_hlist);
		}
		if (!usl_hlist_unhashed(&tunnel->name_hlist)) {
			usl_hlist_del(&tunnel->name_hlist);
		}
		l2tp_tunnel_free(tunnel);
	}
}

/* Called by transport when the tunnel has acknowledged tunnel close messages
 * so is safe to cleanup. We delay a further 2 seconds in case this breaks 
 * some other implementations.
 */
void l2tp_tunnel_close_now(struct l2tp_tunnel *tunnel)
{
	if (!tunnel->close_acked) {
		tunnel->close_acked = 1;
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "tunl %hu: tunnel close acknowledged by peer", tunnel->status.tunnel_id);

		if (tunnel->cleanup_timer != NULL) {
			usl_timer_delete(tunnel->cleanup_timer);
		}
		tunnel->cleanup_timer = usl_timer_create(USL_TIMER_TICKS(2), 0, l2tp_tunnel_cleanup_timeout, tunnel, NULL);
		if (tunnel->cleanup_timer == NULL) {
			/* Kill tunnel immediately if no memory - serious problems! */
			l2tp_tunnel_cleanup_timeout(tunnel);
		}
	}
	
}

/* Come here to create a transport (handles reliable control message
 * delivery) for the tunnel. Creates a UDP socket to the peer.
 */
int l2tp_tunnel_xprt_create(struct l2tp_peer *peer, struct l2tp_tunnel *tunnel, struct sockaddr_in const *peer_addr)
{
	struct l2tp_xprt_tunnel_create_data params;
	int result;
	int new_fd = -1;

	if (tunnel->fd != -1) {
		result = -EALREADY;
		goto out;
	}

	L2TP_DEBUG(L2TP_API, "%s: creating socket to peer %x/%hu using local %x/%hu", __func__,
		   ntohl(tunnel->config.peer_addr.sin_addr.s_addr), ntohs(tunnel->config.peer_addr.sin_port),
		   ntohl(tunnel->config.local_addr.sin_addr.s_addr), ntohs(tunnel->config.local_addr.sin_port));
	new_fd = l2tp_net_create_socket(&tunnel->config.local_addr, tunnel->config.use_udp_checksums, tunnel);
	if (new_fd < 0) {
		result = -errno;
		goto error;
	}

	tunnel->my_peer = peer;
	l2tp_peer_inc_use_count(peer);

	tunnel->fd = new_fd;

	if (!tunnel->status.created_by_admin) {
		tunnel->config.config_id = l2tp_tunnel_alloc_config_id(peer_addr);
		if (tunnel->config.config_id < 0) {
			l2tp_stats.too_many_tunnels++;
			result = tunnel->config.config_id;;
			goto err_close;
		}
	}

	/* Set tunnel mode.. If either LAC or LNS operation is
	 * indicated, do nothing.
	 */
	if ((tunnel->we_can_be_lac) && (!tunnel->we_can_be_lns)) {
		tunnel->status.mode = L2TP_API_TUNNEL_MODE_LAC;
	} else if ((!tunnel->we_can_be_lac) && (tunnel->we_can_be_lns)) {
		tunnel->status.mode = L2TP_API_TUNNEL_MODE_LNS;
	}

	params.max_retries = tunnel->config.max_retries;
	params.rx_window_size = tunnel->config.rx_window_size;
	params.tx_window_size = tunnel->config.tx_window_size;
	params.hello_timeout = tunnel->config.hello_timeout;
	params.retry_timeout = tunnel->config.retry_timeout;
	params.udp_checksum_enabled = tunnel->config.use_udp_checksums;
	params.our_addr = tunnel->config.local_addr.sin_addr.s_addr;
	params.peer_addr = tunnel->config.peer_addr.sin_addr.s_addr;
	params.our_udp_port = tunnel->config.local_addr.sin_port;
	params.peer_udp_port = tunnel->config.peer_addr.sin_port;

	result = usl_fd_add_fd(new_fd, l2tp_net_recv_unconn, tunnel);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to register socket %d", new_fd);
		goto out;
	}

	result = l2tp_xprt_tunnel_create(new_fd, tunnel->status.tunnel_id, &params, tunnel, &tunnel->xprt);
	if (result < 0) {
		goto error;
	}

out:
	return result;

error:
	if (new_fd >= 0) {
		usl_fd_remove_fd(new_fd);
	}
err_close:
	if (new_fd >= 0) {
		close(new_fd);
	}
	goto out;
}

/*****************************************************************************
 * Management API
 * All l2tp_tunnel_xxx_1_svc() functions are RPC callbacks.
 *****************************************************************************/

/* Modify tunnel global parameters. We come here via the handler for
 * l2tp_app_config_modify().
 */
void l2tp_tunnel_globals_modify(struct l2tp_api_system_msg_data *msg, int *result)
{
	if (msg->config.flags & L2TP_API_CONFIG_FLAG_MAX_TUNNELS) {
		l2tp_tunnel_max_count = msg->config.max_tunnels;
	}
	if (msg->config.flags & L2TP_API_CONFIG_FLAG_DRAIN_TUNNELS) {
		l2tp_tunnel_drain = msg->config.drain_tunnels;
	}
	if (msg->config.flags & L2TP_API_CONFIG_FLAG_TUNNEL_ESTABLISH_TIMEOUT) {
		/* establish timeout is either disabled or at least 2 minutes */
		if ((msg->config.tunnel_establish_timeout == 0) ||
		    (msg->config.tunnel_establish_timeout >= 120)) {
			l2tp_tunnel_establish_timeout = msg->config.tunnel_establish_timeout;
		}
	}
	if (msg->config.flags & L2TP_API_CONFIG_FLAG_TUNNEL_PERSIST_PEND_TIMEOUT) {
		/* persist pend timeout is at least 2 minutes */
		if (msg->config.tunnel_persist_pend_timeout >= 120) {
			l2tp_tunnel_persist_pend_timeout = msg->config.tunnel_persist_pend_timeout;
		}
	}
	if (msg->config.flags & L2TP_API_CONFIG_FLAG_DENY_LOCAL_TUNNEL_CREATES) {
		l2tp_tunnel_deny_local_creates = msg->config.deny_local_tunnel_creates;
	}
	if (msg->config.flags & L2TP_API_CONFIG_FLAG_DENY_REMOTE_TUNNEL_CREATES) {
		l2tp_tunnel_deny_remote_creates = msg->config.deny_remote_tunnel_creates;
	}
}

/* Get tunnel global parameters. We come here via the handler for
 * l2tp_app_config_get().
 */
void l2tp_tunnel_globals_get(struct l2tp_api_system_msg_data *msg)
{
	msg->config.max_tunnels = l2tp_tunnel_max_count;
	msg->config.drain_tunnels = l2tp_tunnel_drain;
	msg->config.tunnel_establish_timeout = l2tp_tunnel_establish_timeout;
	msg->config.tunnel_persist_pend_timeout = l2tp_tunnel_persist_pend_timeout;
	msg->status.num_tunnels = l2tp_tunnel_count;
	msg->config.deny_local_tunnel_creates = l2tp_tunnel_deny_local_creates;
	msg->config.deny_remote_tunnel_creates = l2tp_tunnel_deny_remote_creates;
}

/* Derive parameter defaults from the specified tunnel and peer profiles.
 * The rules are:-
 *   if no peer profile specified, use the default
 *   if no tunnel profile specified, use the one specified in the peer profile
 *   use session profile specified in the peer profile
 *   use ppp profile specified in the peer profile
 */
static int l2tp_tunnel_param_defaults(struct l2tp_tunnel *tunnel, char *tunnel_profile_name, char *peer_profile_name)
{
	struct l2tp_tunnel_profile *tunnel_profile;
	struct l2tp_peer_profile *peer_profile;
	char *session_profile_name = NULL;
	char *ppp_profile_name = NULL;
	int result = 0;

	if (peer_profile_name == NULL) {
		peer_profile_name = L2TP_API_PEER_PROFILE_DEFAULT_PROFILE_NAME;
	}
	peer_profile = l2tp_peer_profile_find(peer_profile_name);
	if (peer_profile == NULL) {
		L2TP_DEBUG(L2TP_API, "Peer profile '%s' not found", peer_profile_name);
		result = -L2TP_ERR_PEER_PROFILE_NOT_FOUND;
		goto out;
	}
	if (tunnel_profile_name == NULL) {
		tunnel_profile_name = peer_profile->default_tunnel_profile_name;
	}
	
	tunnel_profile = l2tp_tunnel_profile_find(tunnel_profile_name);
	if (tunnel_profile == NULL) {
		L2TP_DEBUG(L2TP_API, "Tunnel profile '%s' not found", tunnel_profile_name);
		result = -L2TP_ERR_TUNNEL_PROFILE_NOT_FOUND;
		goto out;
	}

	tunnel->config.trace_flags = tunnel_profile->trace_flags;
	tunnel->config.hide_avps = tunnel_profile->hide_avps;
	tunnel->config.auth_mode = tunnel_profile->auth_mode;
	tunnel->config.framing_cap_sync = tunnel_profile->framing_cap_sync;
	tunnel->config.framing_cap_async = tunnel_profile->framing_cap_async;
	tunnel->config.bearer_cap_digital = tunnel_profile->bearer_cap_digital;
	tunnel->config.bearer_cap_analog = tunnel_profile->bearer_cap_analog;
	tunnel->config.use_tiebreaker = tunnel_profile->use_tiebreaker;
	tunnel->config.allow_ppp_proxy = tunnel_profile->allow_ppp_proxy;
	tunnel->config.hello_timeout = tunnel_profile->hello_timeout;
	tunnel->config.max_retries = tunnel_profile->max_retries;
	tunnel->config.rx_window_size = tunnel_profile->rx_window_size;
	tunnel->config.tx_window_size = tunnel_profile->tx_window_size;
	tunnel->config.retry_timeout = tunnel_profile->retry_timeout;
	tunnel->config.idle_timeout = tunnel_profile->idle_timeout;
	tunnel->config.use_udp_checksums = tunnel_profile->use_udp_checksums;
	tunnel->config.do_pmtu_discovery = tunnel_profile->do_pmtu_discovery;
	tunnel->config.mtu = tunnel_profile->mtu;

	/* src_ipaddr and udp ports can come from the tunnel profile */
	tunnel->config.local_addr.sin_family = AF_INET;
	tunnel->config.local_addr.sin_addr.s_addr = tunnel_profile->our_addr.s_addr;
	tunnel->config.local_addr.sin_port = htons(tunnel_profile->our_udp_port);
	tunnel->config.peer_addr.sin_family = AF_INET;
	tunnel->config.peer_addr.sin_port = htons(tunnel_profile->peer_udp_port);

	tunnel->config.max_sessions = tunnel_profile->max_sessions;
	if (tunnel_profile->secret != NULL) {
		if (tunnel->config.secret != NULL) {
			free(tunnel->config.secret);
		}
		tunnel->config.secret = strdup(tunnel_profile->secret);
		tunnel->config.secret_len = strlen(tunnel_profile->secret);
		if (tunnel->config.secret == NULL) {
			result = -ENOMEM;
			goto out;
		}
	}
	if (tunnel_profile->host_name != NULL) {
		if (tunnel->config.host_name != NULL) {
			free(tunnel->config.host_name);
		}
		tunnel->config.host_name = strdup(tunnel_profile->host_name);
		tunnel->config.host_name_len = strlen(tunnel_profile->host_name);
		if (tunnel->config.host_name == NULL) {
			result = -ENOMEM;
			goto out;
		}
	}
	if (tunnel->config.peer_profile_name != NULL) {
		free(tunnel->config.peer_profile_name);
	}
	tunnel->config.peer_profile_name = strdup(peer_profile_name);
	if (tunnel->config.peer_profile_name == NULL) {
		result = -ENOMEM;
		goto out;
	}
	if (tunnel->config.tunnel_profile_name != NULL) {
		free(tunnel->config.tunnel_profile_name);
	}
	tunnel->config.tunnel_profile_name = strdup(tunnel_profile_name);
	if (tunnel->config.tunnel_profile_name == NULL) {
		result = -ENOMEM;
		goto out;
	}

	/* Inherit session profile from peer profile if it isn't specified in
	 * the tunnel profile.
	 */
	if (tunnel->config.session_profile_name != NULL) {
		free(tunnel->config.session_profile_name);
	}
	if ((tunnel_profile->session_profile_name != NULL) && 
	    (strcmp(tunnel_profile->session_profile_name, L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME) != 0)) {
		session_profile_name = tunnel_profile->session_profile_name;
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %s: inherited session profile '%s' from tunnel profile",
				tunnel->fsmi.name, session_profile_name);
	} else if ((peer_profile->default_session_profile_name != NULL) && 
		   (strcmp(peer_profile->default_session_profile_name, L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME) != 0)) {
		session_profile_name = peer_profile->default_session_profile_name;
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %s: inherited session profile '%s' from peer profile",
				tunnel->fsmi.name, session_profile_name);
	} else {
		session_profile_name = L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME;
	}
	tunnel->config.session_profile_name = strdup(session_profile_name);
	if (tunnel->config.session_profile_name == NULL) {
		result = -ENOMEM;
		goto out;
	}

	/* Inherit ppp profile from tunnel profile or peer profile if
	 * it isn't specified in the tunnel profile.
	 */
	if (tunnel->config.ppp_profile_name != NULL) {
		free(tunnel->config.ppp_profile_name);
	}
	if ((tunnel_profile->ppp_profile_name != NULL) &&
	    (strcmp(tunnel_profile->ppp_profile_name, L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME) != 0)) {
		ppp_profile_name = tunnel_profile->ppp_profile_name;
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %s: inherited ppp profile '%s' from tunnel profile",
				tunnel->fsmi.name, ppp_profile_name);
	} else if ((peer_profile->default_ppp_profile_name != NULL) &&
		   (strcmp(peer_profile->default_ppp_profile_name, L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME) != 0)) {
		ppp_profile_name = peer_profile->default_ppp_profile_name;
		l2tp_tunnel_log(tunnel, L2TP_FUNC, LOG_INFO, "FUNC: tunl %s: inherited ppp profile '%s' from peer profile",
				tunnel->fsmi.name, ppp_profile_name);
	} else {
		ppp_profile_name = L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME;
	}
	tunnel->config.ppp_profile_name = strdup(ppp_profile_name);
	if (tunnel->config.ppp_profile_name == NULL) {
		result = -ENOMEM;
		goto out;
	}

out:
	return result;
}

/* Tunnels can be referenced by tunnel_id or tunnel_name. This helper is
 * called by each management operation to locate the tunnel instance.
 * If both id and name are supplied, just use the id.
 */
static struct l2tp_tunnel *l2tp_tunnel_get_instance(uint16_t tunnel_id, char *tunnel_name)
{
	struct l2tp_tunnel *tunnel = NULL;

	/* If caller specified a tunnel_id, find the tunnel by id. */
	if (tunnel_id != 0) {
		tunnel = l2tp_tunnel_find_by_id(tunnel_id);
		if (tunnel != NULL) {
			goto out;
		}
	}

	/* If caller specified a name, find the tunnel by name. */
	if (tunnel_name != NULL) {
		tunnel = l2tp_tunnel_find_by_name(tunnel_name);
	}

out:
	return tunnel;
}

/* Called to init tunnel config data from user parameters. Used by
 * create and modify operations. 
 */
static int l2tp_tunnel_config_set(struct l2tp_tunnel *tunnel, struct l2tp_api_tunnel_msg_data *msg, int create)
{
	int result = -EINVAL;

	if (create) {
		/* These parameters cannot be modified once the tunnel is created */
		if (msg->flags & L2TP_API_TUNNEL_FLAG_HIDE_AVPS) {
			tunnel->config.hide_avps = msg->hide_avps;
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_AUTH_MODE) {
			tunnel->config.auth_mode = msg->auth_mode;
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_FRAMING_CAP) {
			tunnel->config.framing_cap_async = msg->framing_cap_async;
			tunnel->config.framing_cap_sync = msg->framing_cap_sync;
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_BEARER_CAP) {
			tunnel->config.bearer_cap_digital = msg->bearer_cap_digital;
			tunnel->config.bearer_cap_analog = msg->bearer_cap_analog;
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_USE_TIEBREAKER) {
			tunnel->config.use_tiebreaker = msg->use_tiebreaker;
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_ALLOW_PPP_PROXY) {
			tunnel->config.allow_ppp_proxy = msg->allow_ppp_proxy;
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_RX_WINDOW_SIZE) {
			/* RFC2661: rx_window_size must be at least 4 */
			tunnel->config.rx_window_size = (msg->rx_window_size < 4) ? 4 : msg->rx_window_size;
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_TX_WINDOW_SIZE) {
			/* RFC2661: tx_window_size can be 1 */
			tunnel->config.tx_window_size = (msg->tx_window_size > 0) ? msg->tx_window_size : 1;
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_DO_PMTU_DISCOVERY) {
			tunnel->config.do_pmtu_discovery = msg->do_pmtu_discovery;
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_SECRET) {
			L2TP_SET_OPTSTRING_VAR_WITH_LEN(&tunnel->config, secret);
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_HOST_NAME) {
			L2TP_SET_OPTSTRING_VAR_WITH_LEN(&tunnel->config, host_name);
		}
		if (msg->flags & L2TP_API_TUNNEL_FLAG_PROFILE_NAME) {
			L2TP_SET_OPTSTRING_VAR_WITH_DEFAULT(&tunnel->config, tunnel_profile_name, 
							    L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME);
		}
	} else {
		if (msg->flags & (L2TP_API_TUNNEL_FLAG_HIDE_AVPS |
				  L2TP_API_TUNNEL_FLAG_AUTH_MODE |
				  L2TP_API_TUNNEL_FLAG_FRAMING_CAP |
				  L2TP_API_TUNNEL_FLAG_BEARER_CAP |
				  L2TP_API_TUNNEL_FLAG_USE_TIEBREAKER |
				  L2TP_API_TUNNEL_FLAG_ALLOW_PPP_PROXY |
				  L2TP_API_TUNNEL_FLAG_RX_WINDOW_SIZE |
				  L2TP_API_TUNNEL_FLAG_TX_WINDOW_SIZE |
				  L2TP_API_TUNNEL_FLAG_DO_PMTU_DISCOVERY |
				  L2TP_API_TUNNEL_FLAG_SECRET |
				  L2TP_API_TUNNEL_FLAG_HOST_NAME |
				  L2TP_API_TUNNEL_FLAG_PROFILE_NAME)) {
			result = -L2TP_ERR_PARAM_NOT_MODIFIABLE;
			goto out;
		}
	}

	/* These parameters can be modified after the tunnel has been created */
	if (msg->flags & L2TP_API_TUNNEL_FLAG_MTU) {
		tunnel->config.mtu = msg->mtu;
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_HELLO_TIMEOUT) {
		tunnel->config.hello_timeout = msg->hello_timeout;
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_MAX_RETRIES) {
		tunnel->config.max_retries = msg->max_retries;
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_RETRY_TIMEOUT) {
		tunnel->config.retry_timeout = msg->retry_timeout;
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_IDLE_TIMEOUT) {
		tunnel->config.idle_timeout = msg->idle_timeout;
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_TRACE_FLAGS) {
		if (msg->trace_flags_mask == 0) {
			msg->trace_flags_mask = 0xffffffff;
		}
		tunnel->config.trace_flags &= ~(msg->trace_flags_mask);
		tunnel->config.trace_flags |= (msg->trace_flags & msg->trace_flags_mask);
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_MAX_SESSIONS) {
		tunnel->config.max_sessions = msg->max_sessions;
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_USE_UDP_CHECKSUMS) {
		tunnel->config.use_udp_checksums = msg->use_udp_checksums;
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_TUNNEL_NAME) {
		L2TP_SET_OPTSTRING_VAR(&tunnel->config, tunnel_name);
		if (!usl_hlist_unhashed(&tunnel->name_hlist)) {
			usl_hlist_del_init(&tunnel->name_hlist);
		}
		if (tunnel->config.tunnel_name != NULL) {
			usl_hlist_add_head(&tunnel->name_hlist, l2tp_tunnel_name_hash(tunnel->config.tunnel_name));
		}
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_PEER_PROFILE_NAME) {
		L2TP_SET_OPTSTRING_VAR_WITH_DEFAULT(&tunnel->config, peer_profile_name, 
						    L2TP_API_PEER_PROFILE_DEFAULT_PROFILE_NAME);
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_SESSION_PROFILE_NAME) {
		L2TP_SET_OPTSTRING_VAR_WITH_DEFAULT(&tunnel->config, session_profile_name, 
						    L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME);
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_PPP_PROFILE_NAME) {
		L2TP_SET_OPTSTRING_VAR_WITH_DEFAULT(&tunnel->config, ppp_profile_name, 
						    L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME);
	}

	if (msg->flags2 & L2TP_API_TUNNEL_FLAG_INTERFACE_NAME) {
		L2TP_SET_OPTSTRING_VAR(&tunnel->config, interface_name);
		l2tp_warn_not_yet_supported("tunnel interface_name");
	}
	if (msg->flags & L2TP_API_TUNNEL_FLAG_PERSIST) {
		if (msg->persist && !tunnel->config.persist) {
			l2tp_tunnel_inc_use_count(tunnel);
			l2tp_session_tunnel_modified(tunnel);
		} else if (!msg->persist && tunnel->config.persist) {
			l2tp_tunnel_dec_use_count(tunnel);
			l2tp_session_tunnel_modified(tunnel);
		}
		tunnel->config.persist = msg->persist;
	}

	result = 0;

out:
	return result;
}

bool_t l2tp_tunnel_create_1_svc(struct l2tp_api_tunnel_msg_data msg, int *retval, struct svc_req *req)
{
	struct l2tp_peer *peer = NULL;
	struct l2tp_tunnel *tunnel = NULL;
	struct sockaddr_in peer_addr;
	int result = 0;
	int config_id = 0;
	struct in_addr local_addr;

	L2TP_DEBUG(L2TP_API, "%s: tunl %hu", __func__, msg.tunnel_id);

	if ((msg.flags & L2TP_API_TUNNEL_FLAG_PEER_ADDR) == 0) {
		result = -L2TP_ERR_PEER_ADDRESS_MISSING;
		goto out;
	}
	if ((msg.flags & L2TP_API_TUNNEL_FLAG_PEER_UDP_PORT) == 0) {
		msg.peer_udp_port = l2tp_opt_udp_port;
	}
	if ((msg.flags & L2TP_API_TUNNEL_FLAG_TUNNEL_ID) == 0) {
		msg.tunnel_id = 0;
	}
	if ((msg.flags & L2TP_API_TUNNEL_FLAG_OUR_UDP_PORT) == 0) {
		msg.our_udp_port = 0;
	}

	memset(&peer_addr, 0, sizeof(peer_addr));
	peer_addr.sin_family = AF_INET;
	peer_addr.sin_addr.s_addr = msg.peer_addr.s_addr;
	peer_addr.sin_port = htons(msg.peer_udp_port);

	/* If caller specified a tunnel_id or tunnel_name, check that a
	 * tunnel doesn't already exist.
	 */
	tunnel = l2tp_tunnel_get_instance(msg.tunnel_id, OPTSTRING_PTR(msg.tunnel_name));
	if (tunnel != NULL) {
		result = -L2TP_ERR_TUNNEL_ALREADY_EXISTS;
		goto out;
	}

	/* Derive a config_id to uniquify the tunnel if multiple tunnels
	 * are created to the same peer.
	 */
	if ((msg.flags & L2TP_API_TUNNEL_FLAG_CONFIG_ID) == 0) {
		config_id = l2tp_tunnel_alloc_config_id(&peer_addr);
		if (config_id < 0) {
			l2tp_stats.too_many_tunnels++;
			goto out;
		}
	} else {
		config_id = msg.config_id;
		if (l2tp_tunnel_find_by_addr(&peer_addr, 0, config_id, 0)) {
			result = -L2TP_ERR_TUNNEL_ALREADY_EXISTS;
			goto out;
		}
	}

	/* If local address isn't specified, derive it. */
	if (msg.our_addr.s_addr == INADDR_ANY) {
		result = l2tp_net_get_local_address_for_peer(&peer_addr, &local_addr);
		if (result < 0) {
			goto out;
		}
	} else {
		local_addr.s_addr = msg.our_addr.s_addr;
	}

	/* Find or allocate a peer context. */
	peer = l2tp_peer_find(&local_addr, &peer_addr.sin_addr);
	if (peer == NULL) {
		peer = l2tp_peer_alloc(local_addr, peer_addr.sin_addr);
		if (peer == NULL) {
			result = -ENOMEM;
			goto out;
		}
	}

	/* Allocate a new tunnel context */
	tunnel = l2tp_tunnel_alloc(msg.tunnel_id, OPTSTRING_PTR(msg.tunnel_profile_name), OPTSTRING_PTR(msg.peer_profile_name), 1, &result);
	if (tunnel == NULL) {
		goto out;
	}

	/* Retain local address info */
	tunnel->config.local_addr.sin_family = AF_INET;
	if (tunnel->config.local_addr.sin_addr.s_addr == INADDR_ANY) {
		tunnel->config.local_addr.sin_addr.s_addr = local_addr.s_addr;
	}
	if (tunnel->config.local_addr.sin_port == 0) {
		tunnel->config.local_addr.sin_port = htons(msg.our_udp_port);
	}

	/* Retain peer address info */
	tunnel->config.peer_addr.sin_family = AF_INET;
	tunnel->config.peer_addr.sin_addr.s_addr = peer_addr.sin_addr.s_addr;
	tunnel->config.peer_addr.sin_port = peer_addr.sin_port;

	tunnel->config.config_id = config_id;

	l2tp_tunnel_link(tunnel);

	/* Override defaults with user supplied values */
	result = l2tp_tunnel_config_set(tunnel, &msg, 1);
	if (result < 0) {
		goto out;
	}

	result = l2tp_tunnel_xprt_create(peer, tunnel, &peer_addr);
	if (result < 0) {
		goto out;
	}

#ifdef DEBUG
	{
		char src[16];
		char dest[16];
		char *ip;
		ip = inet_ntoa(tunnel->config.local_addr.sin_addr);
		strcpy(src, ip);
		ip = inet_ntoa(tunnel->config.peer_addr.sin_addr);
		strcpy(dest, ip);
		L2TP_DEBUG(L2TP_API, "%s: src=%s dest=%s configId=%d secret=%s", __func__,
			   src, dest, tunnel->config.config_id, 
			   tunnel->config.secret ? tunnel->config.secret : "");
	}
#endif

#ifdef L2TP_TEST
	if (l2tp_test_is_hold_tunnels()) {
		tunnel->hold = 1;
	}
#endif /* L2TP_TEST */

	result = l2tp_tunnel_is_ok_to_create(tunnel, tunnel->status.created_by_admin);
	if (result < 0) {
		l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_CLOSE_REQ);
		*retval = result;
		goto out2;
	}

	/* Remember all non-default parameters */
	tunnel->config.flags |= msg.flags;
	tunnel->config.flags2 |= msg.flags2;

	*retval = tunnel->status.tunnel_id;
	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_OPEN_REQ);
	l2tp_tunnel_created_ind(tunnel);

out2:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *retval);

	return TRUE;

out:
	*retval = result;
	goto out2;
}

bool_t l2tp_tunnel_delete_1_svc(u_short tunnel_id, optstring tunnel_name, optstring reason, int *result, struct svc_req *req)
{
	struct l2tp_tunnel *tunnel;

	L2TP_DEBUG(L2TP_API, "%s: tunl %hu", __func__, tunnel_id);

	/* Find tunnel context using either tunnel_id or tunnel_name.
	 */
	tunnel = l2tp_tunnel_get_instance(tunnel_id, OPTSTRING_PTR(tunnel_name));
	if (tunnel == NULL) {
		*result = -L2TP_ERR_TUNNEL_NOT_FOUND;
		goto out;
	}

	l2tp_tunnel_inc_use_count(tunnel);

	tunnel->hold = 0;
	if (tunnel->config.persist) {
		l2tp_tunnel_dec_use_count(tunnel);
		tunnel->config.persist = 0;
		l2tp_session_tunnel_modified(tunnel);
	}

	if (OPTSTRING_PTR(reason)) {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR, 0, OPTSTRING_PTR(reason));
	} else {
		l2tp_tunnel_set_result(tunnel, L2TP_AVP_RESULT_STOPCCN_NORMAL_STOP, 0, NULL);
	}
	l2tp_tunnel_handle_event(tunnel, L2TP_CCE_EVENT_CLOSE_REQ);
	*result = 0;
	l2tp_tunnel_dec_use_count(tunnel);

out:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);
	return TRUE;
}

bool_t l2tp_tunnel_modify_1_svc(struct l2tp_api_tunnel_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_tunnel *tunnel;
	struct l2tp_xprt_tunnel_modify_data xprt_params;

	L2TP_DEBUG(L2TP_API, "%s: tunl %hu", __func__, msg.tunnel_id);

	/* Find tunnel context using either tunnel_id or tunnel_name.
	 */
	tunnel = l2tp_tunnel_get_instance(msg.tunnel_id, OPTSTRING_PTR(msg.tunnel_name));
	if (tunnel == NULL) {
		*result = -L2TP_ERR_TUNNEL_NOT_FOUND;
		goto out2;
	}

	l2tp_tunnel_inc_use_count(tunnel);

	*result = l2tp_tunnel_config_set(tunnel, &msg, 0);
	if (*result < 0) {
		goto out;
	}

	/* Handle any modified transport parameters */
	memset(&xprt_params, 0, sizeof(xprt_params));
	if (msg.flags & L2TP_API_TUNNEL_FLAG_HELLO_TIMEOUT) {
		xprt_params.hello_timeout = tunnel->config.hello_timeout;
		xprt_params.flags |= L2TP_XPRT_TUN_FLAG_HELLO_TIMEOUT;
	}
	if (msg.flags & L2TP_API_TUNNEL_FLAG_MAX_RETRIES) {
		xprt_params.max_retries = tunnel->config.max_retries;
		xprt_params.flags |= L2TP_XPRT_TUN_FLAG_MAX_RETRIES;
	}
	if (msg.flags & L2TP_API_TUNNEL_FLAG_RETRY_TIMEOUT) {
		xprt_params.retry_timeout = tunnel->config.retry_timeout;
		xprt_params.flags |= L2TP_XPRT_TUN_FLAG_RETRY_TIMEOUT;
	}
	if (msg.flags & L2TP_API_TUNNEL_FLAG_USE_UDP_CHECKSUMS) {
		*result = l2tp_net_modify_socket(tunnel->fd, tunnel->config.use_udp_checksums, -1);
		if (*result < 0) {
			l2tp_tunnel_log(tunnel, L2TP_API, LOG_WARNING, "API: tunl %hu:: failed to modify UDP checksum setting: %s",
					tunnel->status.tunnel_id, strerror(-(*result)));
			goto out;
		}
	}

	if ((xprt_params.flags != 0) && (tunnel->my_peer != NULL)) {
		*result = l2tp_xprt_tunnel_modify(tunnel->xprt, &xprt_params);
	}

	if (*result == 0) {
		/* Remember all non-default parameters */
		tunnel->config.flags |= msg.flags;
		tunnel->config.flags2 |= msg.flags2;

		/* Sessions need to know */
		l2tp_session_tunnel_modified(tunnel);

       		/* Give plugins visibility of tunnel modified */
		if (l2tp_tunnel_modified_hook != NULL) {
			(*l2tp_tunnel_modified_hook)(tunnel->status.tunnel_id);
		}
	}

out:
	l2tp_tunnel_dec_use_count(tunnel);
out2:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);
	return TRUE;
}

bool_t l2tp_tunnel_get_1_svc(u_short tunnel_id, optstring tunnel_name, struct l2tp_api_tunnel_msg_data *result, struct svc_req *req)
{
	struct l2tp_tunnel *tunnel;

	L2TP_DEBUG(L2TP_API, "%s: tunl %hu", __func__, tunnel_id);

	memset(result, 0, sizeof(*result));

	/* Find tunnel context using either tunnel_id or tunnel_name.
	 */
	tunnel = l2tp_tunnel_get_instance(tunnel_id, OPTSTRING_PTR(tunnel_name));
	if (tunnel == NULL) {
		result->result_code = -L2TP_ERR_TUNNEL_NOT_FOUND;
		goto out2;
	}

	l2tp_tunnel_inc_use_count(tunnel);

	result->result_code = 0;
	result->flags = tunnel->config.flags;
	result->hide_avps = tunnel->config.hide_avps;
	result->auth_mode = tunnel->config.auth_mode;
	result->framing_cap_sync = tunnel->config.framing_cap_sync;
	result->framing_cap_async = tunnel->config.framing_cap_async;
	result->bearer_cap_digital = tunnel->config.bearer_cap_digital;
	result->bearer_cap_analog = tunnel->config.bearer_cap_analog;
	result->use_tiebreaker = tunnel->config.use_tiebreaker;
	result->allow_ppp_proxy = tunnel->config.allow_ppp_proxy;
	result->use_udp_checksums = tunnel->config.use_udp_checksums;
	result->do_pmtu_discovery = tunnel->config.do_pmtu_discovery;
	result->mtu = tunnel->config.mtu;
	result->max_sessions = tunnel->config.max_sessions;
	result->num_sessions = tunnel->status.num_sessions;
	result->use_count = tunnel->status.use_count;
	result->num_establish_retries = tunnel->status.num_establish_retries;
	result->mode = tunnel->status.mode;
	result->hello_timeout = tunnel->config.hello_timeout;
	result->max_retries = tunnel->config.max_retries;
	result->rx_window_size = tunnel->config.rx_window_size;
	result->tx_window_size = tunnel->config.tx_window_size;
	result->actual_tx_window_size = tunnel->status.actual_tx_window_size;
	result->retry_timeout = tunnel->config.retry_timeout;
	result->idle_timeout = tunnel->config.idle_timeout;
	result->our_addr.s_addr = tunnel->config.local_addr.sin_addr.s_addr;
	result->peer_addr.s_addr = tunnel->config.peer_addr.sin_addr.s_addr;
	result->our_udp_port = ntohs(tunnel->config.local_addr.sin_port);
	result->peer_udp_port = ntohs(tunnel->config.peer_addr.sin_port);
#ifdef L2TP_TEST
	/* Hack to support regression tests. Don't report port numbers if
	 * no_random_ids is on, unless user has set our_udp_port.
	 */
	if (l2tp_test_is_no_random_ids() && (result->our_udp_port != 1701)) {
		result->our_udp_port = 0;
		result->peer_udp_port = 0;
	}
#endif /* L2TP_TEST */
	result->config_id = tunnel->config.config_id;
	result->trace_flags = tunnel->config.trace_flags;
	result->tunnel_id = tunnel->status.tunnel_id;
	result->created_by_admin = tunnel->status.created_by_admin;
	result->persist = tunnel->config.persist;
	if ((tunnel->fsmi.state == L2TP_CCE_STATE_CLOSING) && (result->persist)) {
		/* Fake RETRY state */
		OPTSTRING(result->state) = strdup("RETRY");
	} else {
		OPTSTRING(result->state) = strdup(usl_fsm_state_name(&tunnel->fsmi));
	}
	if (OPTSTRING(result->state) == NULL) {
		result->result_code = -ENOMEM;
		goto nomem;
	}
	result->state.valid = 1;
	result->tiebreaker.tiebreaker_len = 0;
	result->tiebreaker.tiebreaker_val = '\0';
	if (tunnel->status.have_tiebreaker) {
		result->tiebreaker.tiebreaker_val = malloc(8);
		if (result->tiebreaker.tiebreaker_val == NULL) {
			goto nomem;
		}
		memcpy(result->tiebreaker.tiebreaker_val, &tunnel->status.tiebreaker.value[0], 8);
#ifdef L2TP_TEST
		if (l2tp_test_is_no_random_ids()) {
			memset(result->tiebreaker.tiebreaker_val, 0, 8);
		}
#endif /* L2TP_TEST */
		result->tiebreaker.tiebreaker_len = 8;
	}
	/* result_code may be 2, 4 or >4 bytes in size, depending on whether the error_code
	 * and error_message values are present.
	 */
	if ((tunnel->result_code != NULL) && (tunnel->result_code_len > 0)) {
		result->result_code_result = tunnel->result_code->result_code;
		if (tunnel->result_code_len >= 4) {
			result->result_code_error = tunnel->result_code->error_code;
			if ((tunnel->result_code_len > 4) && (tunnel->result_code->error_message[0] != '\0')) {
				OPTSTRING(result->result_code_message) = strdup(&tunnel->result_code->error_message[0]);
				if (OPTSTRING(result->result_code_message) == NULL) {
					goto nomem;
				}
				result->result_code_message.valid = 1;
			}
		} else {
			result->result_code_error = 0;
			result->result_code_message.valid = 0;
		}
	} else {
		result->result_code_result = 0;
		result->result_code_error = 0;
		result->result_code_message.valid = 0;
	}

	/* Don't return the abbreviated weekday (first 4 chars) in the create time */
	if (!l2tp_test_is_no_random_ids()) {
		OPTSTRING(result->create_time) = strdup(&tunnel->status.create_time[3]);
		result->create_time.valid = 1;
	}

	result->peer.protocol_version_ver = tunnel->peer.protocol_version.ver;
	result->peer.protocol_version_rev = tunnel->peer.protocol_version.rev;
	result->peer_tunnel_id = tunnel->peer.tunnel_id.value;
	result->peer.framing_cap_sync = tunnel->peer.framing_cap.bits.sync;
	result->peer.framing_cap_async = tunnel->peer.framing_cap.bits.async;
	result->peer.bearer_cap_digital = tunnel->peer.bearer_cap.bits.digital;
	result->peer.bearer_cap_analog = tunnel->peer.bearer_cap.bits.analog;
	result->peer.rx_window_size = tunnel->peer.rx_window_size.value;
	result->peer.firmware_revision = tunnel->peer.firmware_revision.value;
	result->peer.tiebreaker.tiebreaker_len = 0;
	result->peer.tiebreaker.tiebreaker_val = '\0';
	if (tunnel->peer.have_tiebreaker) {
		result->peer.tiebreaker.tiebreaker_val = malloc(8);
		if (result->peer.tiebreaker.tiebreaker_val == NULL) {
			goto nomem;
		}
		memcpy(result->peer.tiebreaker.tiebreaker_val, &tunnel->peer.tiebreaker.value[0], 8);
#ifdef L2TP_TEST
		if (l2tp_test_is_no_random_ids()) {
			memset(result->peer.tiebreaker.tiebreaker_val, 0, 8);
		}
#endif /* L2TP_TEST */
		result->peer.tiebreaker.tiebreaker_len = 8;
	}
	if (tunnel->config.tunnel_name != NULL) {
		OPTSTRING(result->tunnel_name) = strdup(tunnel->config.tunnel_name);
		if (OPTSTRING(result->tunnel_name) == NULL) {
			goto nomem;
		}
		result->tunnel_name.valid = 1;
	}
	if (tunnel->config.tunnel_profile_name != NULL) {
		OPTSTRING(result->tunnel_profile_name) = strdup(tunnel->config.tunnel_profile_name);
		if (OPTSTRING(result->tunnel_profile_name) == NULL) {
			goto nomem;
		}
		result->tunnel_profile_name.valid = 1;
	}
	if (tunnel->config.peer_profile_name != NULL) {
		OPTSTRING(result->peer_profile_name) = strdup(tunnel->config.peer_profile_name);
		if (OPTSTRING(result->peer_profile_name) == NULL) {
			goto nomem;
		}
		result->peer_profile_name.valid = 1;
	}
	if (tunnel->config.session_profile_name != NULL) {
		OPTSTRING(result->session_profile_name) = strdup(tunnel->config.session_profile_name);
		if (OPTSTRING(result->session_profile_name) == NULL) {
			goto nomem;
		}
		result->session_profile_name.valid = 1;
	}
	if (tunnel->config.ppp_profile_name != NULL) {
		OPTSTRING(result->ppp_profile_name) = strdup(tunnel->config.ppp_profile_name);
		if (OPTSTRING(result->ppp_profile_name) == NULL) {
			goto nomem;
		}
		result->ppp_profile_name.valid = 1;
	}
	if (tunnel->config.interface_name != NULL) {
		OPTSTRING(result->interface_name) = strdup(tunnel->config.interface_name);
		if (OPTSTRING(result->interface_name) == NULL) {
			goto nomem;
		}
		result->interface_name.valid = 1;
	}
	if (tunnel->config.host_name != NULL) {
		OPTSTRING(result->host_name) = strdup(tunnel->config.host_name);
		if (OPTSTRING(result->host_name) == NULL) {
			goto nomem;
		}
		result->host_name.valid = 1;
	}
	if (tunnel->config.secret != NULL) {
		OPTSTRING(result->secret) = strdup(tunnel->config.secret);
		if (OPTSTRING(result->secret) == NULL) {
			goto nomem;
		}
		result->secret.valid = 1;
	}
	if (tunnel->peer.vendor_name != NULL) {
		OPTSTRING(result->peer.vendor_name) = strdup(tunnel->peer.vendor_name->string);
		if (OPTSTRING(result->peer.vendor_name) == NULL) {
			goto nomem;
		}
		result->peer.vendor_name.valid = 1;
	}

	/* peer.result_code may be 2, 4 or >4 bytes in size, depending on whether the error_code
	 * and error_message values are present.
	 */
	if ((tunnel->peer.result_code != NULL) && (tunnel->peer.result_code_len > 0)) {
		result->peer.result_code_result = tunnel->peer.result_code->result_code;
		if (tunnel->peer.result_code_len >= 4) {
			result->peer.result_code_error = tunnel->peer.result_code->error_code;
			if ((tunnel->peer.result_code->error_message[0] != '\0') && (tunnel->peer.result_code_len > 4)) {
				OPTSTRING(result->peer.result_code_message) = strdup(&tunnel->peer.result_code->error_message[0]);
				if (OPTSTRING(result->peer.result_code_message) == NULL) {
					goto nomem;
				}
				result->peer.result_code_message.valid = 1;
			}
		} else {
			result->peer.result_code_error = 0;
			result->peer.result_code_message.valid = 0;
		}
	} else {
		result->peer.result_code_result = 0;
		result->peer.result_code_error = 0;
		result->peer.result_code_message.valid = 0;
	}

	if (tunnel->xprt != NULL) {
		int retval = l2tp_xprt_tunnel_get(tunnel->xprt, result);
		if (retval < 0) {
			result->result_code = retval;
			goto out;
		}
	}

out:
	l2tp_tunnel_dec_use_count(tunnel);
out2:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, result->result_code);
	return TRUE;

nomem:
	result->result_code = -ENOMEM;
	if (OPTSTRING(result->state) != NULL) {
		free(OPTSTRING(result->state));
	}
	if (OPTSTRING(result->peer.host_name) != NULL) {
		free(OPTSTRING(result->peer.host_name));
	}
	if (OPTSTRING(result->tunnel_profile_name) != NULL) {
		free(OPTSTRING(result->tunnel_profile_name));
	}
	if (OPTSTRING(result->host_name) != NULL) {
		free(OPTSTRING(result->host_name));
	}
	if (OPTSTRING(result->secret) != NULL) {
		free(OPTSTRING(result->secret));
	}
	if (OPTSTRING(result->tunnel_name) != NULL) {
		free(OPTSTRING(result->tunnel_name));
	}
	if (OPTSTRING(result->peer.vendor_name) != NULL) {
		free(OPTSTRING(result->peer.vendor_name));
	}
	if (result->peer.tiebreaker.tiebreaker_val != NULL) {
		free(result->peer.tiebreaker.tiebreaker_val);
	}
	if (OPTSTRING(result->result_code_message) != NULL) {
		free(OPTSTRING(result->result_code_message));
	}
	if (OPTSTRING(result->peer.result_code_message) != NULL) {
		free(OPTSTRING(result->peer.result_code_message));
	}

	goto out;
}

bool_t l2tp_tunnel_list_1_svc(struct l2tp_api_tunnel_list_msg_data *result, struct svc_req *req)
{
	struct l2tp_tunnel *tunnel;
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	int count = 0;

	L2TP_DEBUG(L2TP_API, "%s: enter", __func__);

	memset(result, 0, sizeof(*result));
	if (l2tp_tunnel_count == 0) {
		goto out;
	}

	/* Allocate an array to put the tunnel_ids */
	result->tunnel_ids.tunnel_ids_val = malloc(l2tp_tunnel_count * sizeof(*result->tunnel_ids.tunnel_ids_val));
	if (result->tunnel_ids.tunnel_ids_val == NULL) {
		goto out;
	}
	memset(result->tunnel_ids.tunnel_ids_val, 0, l2tp_tunnel_count * sizeof(*result->tunnel_ids.tunnel_ids_val));

	/* Fill in tunnel_ids array.
	 */
	count = 0;
	usl_list_for_each(walk, tmp, &l2tp_tunnel_list) {
		tunnel = usl_list_entry(walk, struct l2tp_tunnel, list);
		result->tunnel_ids.tunnel_ids_val[count] = tunnel->status.tunnel_id;
		count++;
		if (count > l2tp_tunnel_count) {
			/* Another tunnel was added since we counted the list above. */
			break;
		}
	}

 out:
	result->tunnel_ids.tunnel_ids_len = count;
	if (count == 0) {
		result->tunnel_ids.tunnel_ids_val = calloc(1, sizeof(result->tunnel_ids.tunnel_ids_val[0]));
		if (result->tunnel_ids.tunnel_ids_val == NULL) {
			return FALSE;
		}
	}

	return TRUE;
}

/* Called when a persistent tunnel is about to be deleted. 
 * Clones the tunnel parameters, building a l2tp_api_tunnel_msg_data 
 * struct to be passed to l2tp_tunnel_create_1_svc as if it came from
 * an operator request.
 */
static int l2tp_tunnel_clone(struct l2tp_tunnel *tunnel, struct l2tp_api_tunnel_msg_data *result)
{
	int retval = 0;

	L2TP_DEBUG(L2TP_API, "%s: tunl %hu", __func__, tunnel->status.tunnel_id);

	l2tp_tunnel_inc_use_count(tunnel);

	memset(result, 0, sizeof(*result));

	result->flags = tunnel->config.flags;
	result->hide_avps = tunnel->config.hide_avps;
	result->auth_mode = tunnel->config.auth_mode;
	result->framing_cap_sync = tunnel->config.framing_cap_sync;
	result->framing_cap_async = tunnel->config.framing_cap_async;
	result->bearer_cap_digital = tunnel->config.bearer_cap_digital;
	result->bearer_cap_analog = tunnel->config.bearer_cap_analog;
	result->use_tiebreaker = tunnel->config.use_tiebreaker;
	result->allow_ppp_proxy = tunnel->config.allow_ppp_proxy;
	result->use_udp_checksums = tunnel->config.use_udp_checksums;
	result->do_pmtu_discovery = tunnel->config.do_pmtu_discovery;
	result->mtu = tunnel->config.mtu;
	result->max_sessions = tunnel->config.max_sessions;
	result->hello_timeout = tunnel->config.hello_timeout;
	result->max_retries = tunnel->config.max_retries;
	result->rx_window_size = tunnel->config.rx_window_size;
	result->tx_window_size = tunnel->config.tx_window_size;
	result->retry_timeout = tunnel->config.retry_timeout;
	result->idle_timeout = tunnel->config.idle_timeout;
	result->our_addr.s_addr = tunnel->config.local_addr.sin_addr.s_addr;
	result->peer_addr.s_addr = tunnel->config.peer_addr.sin_addr.s_addr;
	result->our_udp_port = htons(tunnel->config.local_addr.sin_port);
	result->peer_udp_port = htons(tunnel->config.peer_addr.sin_port);
	result->config_id = tunnel->config.config_id;
	result->trace_flags = tunnel->config.trace_flags;
	result->tunnel_id = tunnel->status.tunnel_id;
	result->persist = tunnel->config.persist;

	if (tunnel->config.tunnel_name != NULL) {
		OPTSTRING(result->tunnel_name) = strdup(tunnel->config.tunnel_name);
		if (OPTSTRING(result->tunnel_name) == NULL) {
			goto nomem;
		}
		result->tunnel_name.valid = 1;
	}
	if (tunnel->config.tunnel_profile_name != NULL) {
		OPTSTRING(result->tunnel_profile_name) = strdup(tunnel->config.tunnel_profile_name);
		if (OPTSTRING(result->tunnel_profile_name) == NULL) {
			goto nomem;
		}
		result->tunnel_profile_name.valid = 1;
	}
	if (tunnel->config.peer_profile_name != NULL) {
		OPTSTRING(result->peer_profile_name) = strdup(tunnel->config.peer_profile_name);
		if (OPTSTRING(result->peer_profile_name) == NULL) {
			goto nomem;
		}
		result->peer_profile_name.valid = 1;
	}
	if (tunnel->config.session_profile_name != NULL) {
		OPTSTRING(result->session_profile_name) = strdup(tunnel->config.session_profile_name);
		if (OPTSTRING(result->session_profile_name) == NULL) {
			goto nomem;
		}
		result->session_profile_name.valid = 1;
	}
	if (tunnel->config.ppp_profile_name != NULL) {
		OPTSTRING(result->ppp_profile_name) = strdup(tunnel->config.ppp_profile_name);
		if (OPTSTRING(result->ppp_profile_name) == NULL) {
			goto nomem;
		}
		result->ppp_profile_name.valid = 1;
	}
	if (tunnel->config.interface_name != NULL) {
		OPTSTRING(result->interface_name) = strdup(tunnel->config.interface_name);
		if (OPTSTRING(result->interface_name) == NULL) {
			goto nomem;
		}
		result->interface_name.valid = 1;
	}
	if (tunnel->config.host_name != NULL) {
		OPTSTRING(result->host_name) = strdup(tunnel->config.host_name);
		if (OPTSTRING(result->host_name) == NULL) {
			goto nomem;
		}
		result->host_name.valid = 1;
	}
	if (tunnel->config.secret != NULL) {
		OPTSTRING(result->secret) = strdup(tunnel->config.secret);
		if (OPTSTRING(result->secret) == NULL) {
			goto nomem;
		}
		result->secret.valid = 1;
	}

out:
	l2tp_tunnel_dec_use_count(tunnel);
	retval = 0;
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, retval);
	return retval;

nomem:
	retval = -ENOMEM;
	if (OPTSTRING(result->peer_profile_name) != NULL) {
		free(OPTSTRING(result->peer_profile_name));
	}
	if (OPTSTRING(result->session_profile_name) != NULL) {
		free(OPTSTRING(result->session_profile_name));
	}
	if (OPTSTRING(result->ppp_profile_name) != NULL) {
		free(OPTSTRING(result->ppp_profile_name));
	}
	if (OPTSTRING(result->tunnel_profile_name) != NULL) {
		free(OPTSTRING(result->tunnel_profile_name));
	}
	if (OPTSTRING(result->interface_name) != NULL) {
		free(OPTSTRING(result->interface_name));
	}
	if (OPTSTRING(result->host_name) != NULL) {
		free(OPTSTRING(result->host_name));
	}
	if (OPTSTRING(result->secret) != NULL) {
		free(OPTSTRING(result->secret));
	}
	if (OPTSTRING(result->tunnel_name) != NULL) {
		free(OPTSTRING(result->tunnel_name));
	}

	goto out;
}


/******************************************************************************
 * Tunnel profiles
 *****************************************************************************/

static int l2tp_tunnel_profile_set(struct l2tp_api_tunnel_profile_msg_data *msg, struct l2tp_tunnel_profile *profile)
{
	int result = 0;

	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS) {
		if (msg->trace_flags_mask == 0) {
			msg->trace_flags_mask = 0xffffffff;
		}
		profile->trace_flags &= ~(msg->trace_flags_mask);
		profile->trace_flags |= (msg->trace_flags & msg->trace_flags_mask);
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_OUR_UDP_PORT) {
		profile->our_udp_port = msg->our_udp_port;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_PEER_UDP_PORT) {
		profile->peer_udp_port = msg->peer_udp_port;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_OUR_ADDR) {
		profile->our_addr.s_addr = msg->our_addr.s_addr;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_PEER_ADDR) {
		profile->peer_addr.s_addr = msg->peer_addr.s_addr;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS) {
		profile->hide_avps = msg->hide_avps;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_AUTH_MODE) {
		profile->auth_mode = msg->auth_mode;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_FRAMING_CAP) {
		profile->framing_cap_sync = msg->framing_cap_sync;
		profile->framing_cap_async = msg->framing_cap_async;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_BEARER_CAP) {
		profile->bearer_cap_digital = msg->bearer_cap_digital;
		profile->bearer_cap_analog = msg->bearer_cap_analog;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_USE_TIEBREAKER) {
		profile->use_tiebreaker = msg->use_tiebreaker;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_HELLO_TIMEOUT) {
		profile->hello_timeout = msg->hello_timeout;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_MAX_RETRIES) {
		profile->max_retries = msg->max_retries;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_RX_WINDOW_SIZE) {
		/* RFC2661: rx_window_size must be at least 4 */
		profile->rx_window_size = (msg->rx_window_size < 4) ? 4 : msg->rx_window_size;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_TX_WINDOW_SIZE) {
		/* RFC2661: tx_window_size can be 1 */
		profile->tx_window_size = (msg->tx_window_size > 0) ? msg->tx_window_size : 1;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_RETRY_TIMEOUT) {
		profile->retry_timeout = msg->retry_timeout;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_IDLE_TIMEOUT) {
		profile->idle_timeout = msg->idle_timeout;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_ALLOW_PPP_PROXY) {
		profile->allow_ppp_proxy = msg->allow_ppp_proxy;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_USE_UDP_CHECKSUMS) {
		profile->use_udp_checksums = msg->use_udp_checksums;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_DO_PMTU_DISCOVERY) {
		profile->do_pmtu_discovery = msg->do_pmtu_discovery;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_MTU) {
		profile->mtu = msg->mtu;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_MAX_SESSIONS) {
		profile->max_sessions = msg->max_sessions;
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_HOST_NAME) {
		L2TP_SET_OPTSTRING_VAR_WITH_LEN(profile, host_name);
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_SECRET) {
		L2TP_SET_OPTSTRING_VAR_WITH_LEN(profile, secret);
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_PEER_PROFILE_NAME) {
		L2TP_SET_OPTSTRING_VAR(profile, peer_profile_name);
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_SESSION_PROFILE_NAME) {
		L2TP_SET_OPTSTRING_VAR(profile, session_profile_name);
	}
	if (msg->flags & L2TP_API_TUNNEL_PROFILE_FLAG_PPP_PROFILE_NAME) {
		L2TP_SET_OPTSTRING_VAR(profile, ppp_profile_name);
	}

out:
	return result;
}

static void l2tp_tunnel_profile_free(struct l2tp_tunnel_profile *profile)
{
	if (profile->secret != NULL) {
		free(profile->secret);
	}
	if (profile->host_name != NULL) {
		free(profile->host_name);
	}
	if (profile->profile_name != NULL) {
		free(profile->profile_name);
	}
	if (profile->peer_profile_name != NULL) {
		free(profile->peer_profile_name);
	}
	if (profile->session_profile_name != NULL) {
		free(profile->session_profile_name);
	}
	if (profile->ppp_profile_name != NULL) {
		free(profile->ppp_profile_name);
	}
	USL_POISON_MEMORY(profile, 0xed, sizeof(*profile));
	free(profile);
}

bool_t l2tp_tunnel_profile_create_1_svc(struct l2tp_api_tunnel_profile_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_tunnel_profile *profile;

	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, msg.profile_name);

	profile = l2tp_tunnel_profile_find(msg.profile_name);
	if (profile != NULL) {
		*result = -L2TP_ERR_PROFILE_ALREADY_EXISTS;
		goto out;
	}
	
	profile = calloc(1, sizeof(struct l2tp_tunnel_profile));
	if (profile == NULL) {
		*result = -ENOMEM;
		goto error;
	}
	profile->profile_name = strdup(msg.profile_name);
	if (profile->profile_name == NULL) {
		*result = -ENOMEM;
		goto error;
	}

	/* Fill with default values */
	profile->trace_flags = l2tp_tunnel_defaults->trace_flags;
	profile->hide_avps = l2tp_tunnel_defaults->hide_avps;
	profile->auth_mode = l2tp_tunnel_defaults->auth_mode;
	profile->framing_cap_sync = l2tp_tunnel_defaults->framing_cap_sync;
	profile->framing_cap_async = l2tp_tunnel_defaults->framing_cap_async;
	profile->bearer_cap_digital = l2tp_tunnel_defaults->bearer_cap_digital;
	profile->bearer_cap_analog = l2tp_tunnel_defaults->bearer_cap_analog;
	profile->use_tiebreaker = l2tp_tunnel_defaults->use_tiebreaker;
	profile->allow_ppp_proxy = l2tp_tunnel_defaults->allow_ppp_proxy;
	profile->hello_timeout = l2tp_tunnel_defaults->hello_timeout;
	profile->max_retries = l2tp_tunnel_defaults->max_retries;
	profile->rx_window_size = l2tp_tunnel_defaults->rx_window_size;
	profile->tx_window_size = l2tp_tunnel_defaults->tx_window_size;
	profile->retry_timeout = l2tp_tunnel_defaults->retry_timeout;
	profile->idle_timeout = l2tp_tunnel_defaults->idle_timeout;
	profile->use_udp_checksums = l2tp_tunnel_defaults->use_udp_checksums;
	profile->do_pmtu_discovery = l2tp_tunnel_defaults->do_pmtu_discovery;
	profile->mtu = l2tp_tunnel_defaults->mtu;
	profile->our_addr.s_addr = l2tp_tunnel_defaults->our_addr.s_addr;
	profile->our_udp_port = l2tp_tunnel_defaults->our_udp_port;
	profile->peer_addr.s_addr = l2tp_tunnel_defaults->peer_addr.s_addr;
	profile->peer_udp_port = l2tp_tunnel_defaults->peer_udp_port;
	profile->max_sessions = l2tp_tunnel_defaults->max_sessions;

	/* Override by user-supplied params */
	*result = l2tp_tunnel_profile_set(&msg, profile);
	if (*result < 0) {
		goto error;
	}

	/* Remember all non-default parameters */
	profile->flags |= msg.flags;

	USL_LIST_HEAD_INIT(&profile->list);
	usl_list_add(&profile->list, &l2tp_tunnel_profile_list);

	L2TP_DEBUG(L2TP_FUNC, "FUNC: tunnel profile %s created", profile->profile_name);

	/* Give plugins visibility of tunnel profile created */
	if (l2tp_profile_created_hook != NULL) {
		(*l2tp_profile_created_hook)(L2TP_PROFILE_TYPE_TUNNEL, profile->profile_name);
	}

out:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);
	return TRUE;

error:
	l2tp_tunnel_profile_free(profile);
	goto out;
}

bool_t l2tp_tunnel_profile_modify_1_svc(struct l2tp_api_tunnel_profile_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_tunnel_profile *profile;

	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, msg.profile_name);

	profile = l2tp_tunnel_profile_find(msg.profile_name);
	if (profile == NULL) {
		*result = -L2TP_ERR_TUNNEL_PROFILE_NOT_FOUND;
		goto out;
	}

	*result = l2tp_tunnel_profile_set(&msg, profile);

	if (*result == 0) {
		/* Remember all non-default parameters */
		profile->flags |= msg.flags;

		L2TP_DEBUG(L2TP_FUNC, "FUNC: tunnel profile %s modified", profile->profile_name);

		/* Give plugins visibility of tunnel profile modified */
		if (l2tp_profile_modified_hook != NULL) {
			(*l2tp_profile_modified_hook)(L2TP_PROFILE_TYPE_TUNNEL, profile->profile_name);
		}
	}

out:	
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);
	return TRUE;
}

bool_t l2tp_tunnel_profile_delete_1_svc(char *profile_name, int *result, struct svc_req *req)
{
	struct l2tp_tunnel_profile *profile;

	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, profile_name);

	if ((profile_name == NULL) || strlen(profile_name) == 0) {
		*result = -L2TP_ERR_PROFILE_NAME_MISSING;
		goto out;
	}

	/* Prevent deletion of default profile */
	if (strcmp(profile_name, L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME) == 0) {
		*result = -L2TP_ERR_PROFILE_NAME_ILLEGAL;
		goto out;
	}

	profile = l2tp_tunnel_profile_find(profile_name);
	if (profile == NULL) {
		*result = -L2TP_ERR_TUNNEL_PROFILE_NOT_FOUND;
		goto out;
	}

	usl_list_del(&profile->list);

	L2TP_DEBUG(L2TP_FUNC, "FUNC: tunnel profile %s deleted", profile->profile_name);

	/* Give plugins visibility of tunnel profile deleted */
	if (l2tp_profile_deleted_hook != NULL) {
		(*l2tp_profile_deleted_hook)(L2TP_PROFILE_TYPE_TUNNEL, profile->profile_name);
	}

	l2tp_tunnel_profile_free(profile);
	*result = 0;

out:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);
	return TRUE;
}

bool_t l2tp_tunnel_profile_get_1_svc(char *profile_name, struct l2tp_api_tunnel_profile_msg_data *result, struct svc_req *req)
{
	struct l2tp_tunnel_profile *profile;

	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, profile_name);

	memset(result, 0, sizeof(*result));

	profile = l2tp_tunnel_profile_find(profile_name);
	if (profile == NULL) {
		result->profile_name = strdup(profile_name);
		result->result_code = -L2TP_ERR_TUNNEL_PROFILE_NOT_FOUND;
		goto out;
	}

	result->trace_flags = profile->trace_flags;
	result->flags = profile->flags;
	result->hide_avps = profile->hide_avps;
	result->auth_mode = profile->auth_mode;
	result->framing_cap_sync = profile->framing_cap_sync;
	result->framing_cap_async = profile->framing_cap_async;
	result->bearer_cap_digital = profile->bearer_cap_digital;
	result->bearer_cap_analog = profile->bearer_cap_analog;
	result->use_tiebreaker = profile->use_tiebreaker;
	result->hello_timeout = profile->hello_timeout;
	result->max_retries = profile->max_retries;
	result->rx_window_size = profile->rx_window_size;
	result->tx_window_size = profile->tx_window_size;
	result->retry_timeout = profile->retry_timeout;
	result->idle_timeout = profile->idle_timeout;
	result->allow_ppp_proxy = profile->allow_ppp_proxy;
	result->use_udp_checksums = profile->use_udp_checksums;
	result->do_pmtu_discovery = profile->do_pmtu_discovery;
	result->mtu = profile->mtu;
	result->max_sessions = profile->max_sessions;
	result->our_addr.s_addr = profile->our_addr.s_addr;
	result->our_udp_port = profile->our_udp_port;
	result->peer_addr.s_addr = profile->peer_addr.s_addr;
	result->peer_udp_port = profile->peer_udp_port;
	if (profile->profile_name != NULL) {
		result->profile_name = strdup(profile->profile_name);
		if (result->profile_name == NULL) {
			goto nomem;
		}
	} else {
		result->profile_name = strdup("");
		if (result->profile_name == NULL) {
			goto nomem;
		}
	}
	if (profile->host_name != NULL) {
		OPTSTRING(result->host_name) = strdup(profile->host_name);
		if (OPTSTRING(result->host_name) == NULL) {
			goto nomem;
		}
		result->host_name.valid = 1;
	} else {
		result->host_name.valid = 0;
	}
	if (profile->secret != NULL) {
		OPTSTRING(result->secret) = strdup(profile->secret);
		if (OPTSTRING(result->secret) == NULL) {
			goto nomem;
		}
		result->secret.valid = 1;
	} else {
		result->secret.valid = 0;
	}
	if (profile->peer_profile_name != NULL) {
		OPTSTRING(result->peer_profile_name) = strdup(profile->peer_profile_name);
		if (OPTSTRING(result->peer_profile_name) == NULL) {
			goto nomem;
		}
		result->peer_profile_name.valid = 1;
	} else {
		result->peer_profile_name.valid = 0;;
	}
	if (profile->session_profile_name != NULL) {
		OPTSTRING(result->session_profile_name) = strdup(profile->session_profile_name);
		if (OPTSTRING(result->session_profile_name) == NULL) {
			goto nomem;
		}
		result->session_profile_name.valid = 1;
	} else {
		result->session_profile_name.valid = 0;;
	}
	if (profile->ppp_profile_name != NULL) {
		OPTSTRING(result->ppp_profile_name) = strdup(profile->ppp_profile_name);
		if (OPTSTRING(result->ppp_profile_name) == NULL) {
			goto nomem;
		}
		result->ppp_profile_name.valid = 1;
	} else {
		result->ppp_profile_name.valid = 0;;
	}

out:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, result->result_code);
	return TRUE;

nomem:
	result->result_code = -ENOMEM;
	if (result->profile_name != NULL) {
		free(result->profile_name);
	}
	if (OPTSTRING(result->host_name) != NULL) {
		free(OPTSTRING(result->host_name));
	}
	if (OPTSTRING(result->peer_profile_name) != NULL) {
		free(OPTSTRING(result->peer_profile_name));
	}
	if (OPTSTRING(result->session_profile_name) != NULL) {
		free(OPTSTRING(result->session_profile_name));
	}
	if (OPTSTRING(result->ppp_profile_name) != NULL) {
		free(OPTSTRING(result->ppp_profile_name));
	}
	if (OPTSTRING(result->secret) != NULL) {
		free(OPTSTRING(result->secret));
	}
	
	goto out;
}

bool_t l2tp_tunnel_profile_list_1_svc(struct l2tp_api_tunnel_profile_list_msg_data *result, struct svc_req *req)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_tunnel_profile *profile;
	struct l2tp_api_tunnel_profile_list_entry *entry;
	struct l2tp_api_tunnel_profile_list_entry *tmpe;
	int num_profiles = 0;

	L2TP_DEBUG(L2TP_API, "%s: enter", __func__);

	memset(result, 0, sizeof(*result));

	result->profiles = calloc(1, sizeof(*result->profiles));
	if (result->profiles == NULL) {
		result->result = -ENOMEM;
		goto error;
	}
	entry = result->profiles;
	usl_list_for_each(walk, tmp, &l2tp_tunnel_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_tunnel_profile, list);

		entry->profile_name = strdup(profile->profile_name);
		if (entry->profile_name == NULL) {
			result->result = -ENOMEM;
			goto error;
		}

		tmpe = calloc(1, sizeof(*result->profiles));
		if (tmpe == NULL) {
			result->result = -ENOMEM;
			goto error;
		}
		entry->next = tmpe;
		entry = tmpe;
		num_profiles++;
	}

	entry->profile_name = strdup("");
	if (entry->profile_name == NULL) {
		goto error;
	}
	result->num_profiles = num_profiles;

	return TRUE;

error:
	for (entry = result->profiles; entry != NULL; ) {
		tmpe = entry->next;
		free(entry->profile_name);
		free(entry);
		entry = tmpe;
	}

	return TRUE;
}

bool_t l2tp_tunnel_profile_unset_1_svc(struct l2tp_api_tunnel_profile_unset_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_tunnel_profile *profile;

	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, msg.profile_name);

	profile = l2tp_tunnel_profile_find(msg.profile_name);
	if (profile == NULL) {
		*result = -L2TP_ERR_TUNNEL_PROFILE_NOT_FOUND;
		goto out;
	}

	/* If secret is being unset, check that hide_avps is either off or is also unset */
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_SECRET) {
		if (profile->hide_avps && ((msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS) == 0)) {
			*result = -L2TP_ERR_TUNNEL_HIDE_AVPS_NEEDS_SECRET;
			goto out;
		}
	}	

	/* Reset requested parameters to their default values */
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS) {
		if (l2tp_opt_debug) {
			profile->trace_flags = -1;
		} else {
			profile->trace_flags = L2TP_API_TUNNEL_PROFILE_DEFAULT_TRACE_FLAGS;
		}
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_OUR_UDP_PORT) {
		profile->our_udp_port = 0;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_PEER_UDP_PORT) {
		profile->peer_udp_port = 0;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_OUR_ADDR) {
		profile->our_addr.s_addr = INADDR_ANY;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_PEER_ADDR) {
		profile->peer_addr.s_addr = INADDR_ANY;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS) {
		profile->hide_avps = L2TP_API_TUNNEL_PROFILE_DEFAULT_HIDE_AVPS;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_AUTH_MODE) {
		profile->auth_mode = L2TP_API_TUNNEL_PROFILE_DEFAULT_AUTH_MODE;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_FRAMING_CAP) {
		profile->framing_cap_sync = L2TP_API_TUNNEL_PROFILE_DEFAULT_FRAMING_CAP_SYNC;
		profile->framing_cap_async = L2TP_API_TUNNEL_PROFILE_DEFAULT_FRAMING_CAP_ASYNC;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_BEARER_CAP) {
		profile->bearer_cap_digital = L2TP_API_TUNNEL_PROFILE_DEFAULT_BEARER_CAP_DIGITAL;
		profile->bearer_cap_analog = L2TP_API_TUNNEL_PROFILE_DEFAULT_BEARER_CAP_ANALOG;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_USE_TIEBREAKER) {
		profile->use_tiebreaker = L2TP_API_TUNNEL_PROFILE_DEFAULT_USE_TIEBREAKER;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_HELLO_TIMEOUT) {
		profile->hello_timeout = L2TP_API_TUNNEL_PROFILE_DEFAULT_HELLO_TIMEOUT;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_MAX_RETRIES) {
		profile->max_retries = L2TP_API_TUNNEL_PROFILE_DEFAULT_MAX_RETRIES;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_RX_WINDOW_SIZE) {
		profile->rx_window_size = L2TP_API_TUNNEL_PROFILE_DEFAULT_RX_WINDOW_SIZE;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_TX_WINDOW_SIZE) {
		profile->tx_window_size = L2TP_API_TUNNEL_PROFILE_DEFAULT_TX_WINDOW_SIZE;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_RETRY_TIMEOUT) {
		profile->retry_timeout = L2TP_API_TUNNEL_PROFILE_DEFAULT_RETRY_TIMEOUT;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_IDLE_TIMEOUT) {
		profile->idle_timeout = L2TP_API_TUNNEL_PROFILE_DEFAULT_IDLE_TIMEOUT;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_ALLOW_PPP_PROXY) {
		profile->allow_ppp_proxy = L2TP_API_TUNNEL_PROFILE_DEFAULT_ALLOW_PPP_PROXY;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_USE_UDP_CHECKSUMS) {
		profile->use_udp_checksums = L2TP_API_TUNNEL_PROFILE_DEFAULT_USE_UDP_CHECKSUMS;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_DO_PMTU_DISCOVERY) {
		profile->do_pmtu_discovery = L2TP_API_TUNNEL_PROFILE_DEFAULT_DO_PMTU_DISCOVERY;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_MTU) {
		profile->mtu = L2TP_API_TUNNEL_PROFILE_DEFAULT_MTU;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_MAX_SESSIONS) {
		profile->max_sessions = L2TP_API_TUNNEL_PROFILE_DEFAULT_MAX_SESSIONS;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_HOST_NAME) {
		if (profile->host_name != NULL) {
			free(profile->host_name);
		}
		profile->host_name = NULL;
		profile->host_name_len = 0;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_SECRET) {
		if (profile->secret != NULL) {
			free(profile->secret);
		}
		profile->secret = NULL;
		profile->secret_len = 0;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_PEER_PROFILE_NAME) {
		if (profile->peer_profile_name != NULL) {
			free(profile->peer_profile_name);
		}
		profile->peer_profile_name = NULL;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_SESSION_PROFILE_NAME) {
		if (profile->session_profile_name != NULL) {
			free(profile->session_profile_name);
		}
		profile->session_profile_name = NULL;
	}
	if (msg.flags & L2TP_API_TUNNEL_PROFILE_FLAG_PPP_PROFILE_NAME) {
		if (profile->ppp_profile_name != NULL) {
			free(profile->ppp_profile_name);
		}
		profile->ppp_profile_name = NULL;
	}

	/* Unset the specified flags */
	profile->flags &= ~(msg.flags);

	L2TP_DEBUG(L2TP_FUNC, "FUNC: tunnel profile %s unset", profile->profile_name);

	/* Give plugins visibility of tunnel profile modified */
	if (l2tp_profile_modified_hook != NULL) {
		(*l2tp_profile_modified_hook)(L2TP_PROFILE_TYPE_TUNNEL, profile->profile_name);
	}

out:	
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);
	return TRUE;
}


/*****************************************************************************
 * Init and cleanup
 *****************************************************************************/

void l2tp_tunnel_reinit(void)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_tunnel_profile *profile;

	l2tp_tunnel_max_count = L2TP_TUNNEL_MAX_COUNT;
	l2tp_tunnel_drain = 0;
	l2tp_tunnel_establish_timeout = L2TP_TUNNEL_ESTABLISH_TIMEOUT;
	l2tp_tunnel_persist_pend_timeout = L2TP_TUNNEL_PERSIST_PEND_TIMEOUT;
	l2tp_tunnel_deny_local_creates = 0;
	l2tp_tunnel_deny_remote_creates = 0;

	/* First remove all profiles */
	usl_list_for_each(walk, tmp, &l2tp_tunnel_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_tunnel_profile, list);
		usl_list_del(&profile->list);
		l2tp_tunnel_profile_free(profile);
	}

	/* Now create a default profile */
	l2tp_tunnel_defaults = calloc(1, sizeof(*l2tp_tunnel_defaults));
	if (l2tp_tunnel_defaults == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	l2tp_tunnel_defaults->profile_name = strdup(L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME);
	l2tp_tunnel_defaults->peer_profile_name = strdup(L2TP_API_PEER_PROFILE_DEFAULT_PROFILE_NAME);
	l2tp_tunnel_defaults->session_profile_name = strdup(L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME);
	l2tp_tunnel_defaults->ppp_profile_name = strdup(L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME);
	if ((l2tp_tunnel_defaults->profile_name == NULL) ||
	    (l2tp_tunnel_defaults->peer_profile_name == NULL) ||
	    (l2tp_tunnel_defaults->session_profile_name == NULL) ||
	    (l2tp_tunnel_defaults->ppp_profile_name == NULL)) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	if (l2tp_opt_debug) {
		l2tp_tunnel_defaults->trace_flags = -1;
	} else {
		l2tp_tunnel_defaults->trace_flags = L2TP_API_TUNNEL_PROFILE_DEFAULT_TRACE_FLAGS;
	}
	l2tp_tunnel_defaults->auth_mode = L2TP_API_TUNNEL_PROFILE_DEFAULT_AUTH_MODE;
	l2tp_tunnel_defaults->framing_cap_sync = L2TP_API_TUNNEL_PROFILE_DEFAULT_FRAMING_CAP_SYNC;
	l2tp_tunnel_defaults->framing_cap_async = L2TP_API_TUNNEL_PROFILE_DEFAULT_FRAMING_CAP_ASYNC;
	l2tp_tunnel_defaults->bearer_cap_digital = L2TP_API_TUNNEL_PROFILE_DEFAULT_BEARER_CAP_DIGITAL;
	l2tp_tunnel_defaults->bearer_cap_analog = L2TP_API_TUNNEL_PROFILE_DEFAULT_BEARER_CAP_ANALOG;
	l2tp_tunnel_defaults->use_tiebreaker = L2TP_API_TUNNEL_PROFILE_DEFAULT_USE_TIEBREAKER;
	l2tp_tunnel_defaults->allow_ppp_proxy = L2TP_API_TUNNEL_PROFILE_DEFAULT_ALLOW_PPP_PROXY;
	l2tp_tunnel_defaults->use_udp_checksums = L2TP_API_TUNNEL_PROFILE_DEFAULT_USE_UDP_CHECKSUMS;
	l2tp_tunnel_defaults->do_pmtu_discovery = L2TP_API_TUNNEL_PROFILE_DEFAULT_DO_PMTU_DISCOVERY;
	l2tp_tunnel_defaults->mtu = L2TP_API_TUNNEL_PROFILE_DEFAULT_MTU;
	l2tp_tunnel_defaults->hello_timeout = L2TP_API_TUNNEL_PROFILE_DEFAULT_HELLO_TIMEOUT;
	l2tp_tunnel_defaults->max_retries = L2TP_API_TUNNEL_PROFILE_DEFAULT_MAX_RETRIES;
	l2tp_tunnel_defaults->rx_window_size = L2TP_API_TUNNEL_PROFILE_DEFAULT_RX_WINDOW_SIZE;
	l2tp_tunnel_defaults->tx_window_size = L2TP_API_TUNNEL_PROFILE_DEFAULT_TX_WINDOW_SIZE;
	l2tp_tunnel_defaults->retry_timeout = L2TP_API_TUNNEL_PROFILE_DEFAULT_RETRY_TIMEOUT;
	l2tp_tunnel_defaults->idle_timeout = L2TP_API_TUNNEL_PROFILE_DEFAULT_IDLE_TIMEOUT;
	USL_LIST_HEAD_INIT(&l2tp_tunnel_defaults->list);
	usl_list_add(&l2tp_tunnel_defaults->list, &l2tp_tunnel_profile_list);

}

void l2tp_tunnel_init(void)
{
	int result;

	l2tp_tunnel_count = 0;

	l2tp_tunnel_reinit();

	memset(&l2tp_tunnel_id_list, 0, sizeof(l2tp_tunnel_id_list));
	memset(&l2tp_tunnel_name_list, 0, sizeof(l2tp_tunnel_name_list));

	l2tp_tunnel_emergency_result_code = malloc(sizeof(struct l2tp_avp_result_code) + 
						   L2TP_API_TUNNEL_EMERG_RESULT_CODE_SIZE);
	if (l2tp_tunnel_emergency_result_code == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	memset(l2tp_tunnel_emergency_result_code, 0, 
	       sizeof(struct l2tp_avp_result_code) + L2TP_API_TUNNEL_EMERG_RESULT_CODE_SIZE);

	result = pipe(&l2tp_tunnel_event_pipe[0]);
	if (result < 0) {
		fprintf(stderr, "Failed to create internal tunnel event pipe: %s\n", strerror(-errno));
		exit(1);
	}
	(void) fcntl(l2tp_tunnel_event_pipe[0], F_SETFD, FD_CLOEXEC);
	(void) fcntl(l2tp_tunnel_event_pipe[1], F_SETFD, FD_CLOEXEC);
	result = usl_fd_add_fd(l2tp_tunnel_event_pipe[0], l2tp_tunnel_do_queued_event, NULL);
	if (result < 0) {
		fprintf(stderr, "Failed to setup tunnel event pipe: %s\n", strerror(-errno));
		exit(1);
	}

	L2TP_DEBUG(L2TP_FUNC, "tunnel: size=%zu", sizeof(struct l2tp_tunnel));
}

void l2tp_tunnel_cleanup(void)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_tunnel_profile *profile;
	struct l2tp_tunnel *tunnel;

	usl_list_for_each(walk, tmp, &l2tp_tunnel_list) {
		tunnel = usl_list_entry(walk, struct l2tp_tunnel, list);
		l2tp_tunnel_unlink(tunnel, 1);
	}

	if (l2tp_tunnel_emergency_result_code != NULL) {
		free(l2tp_tunnel_emergency_result_code);
		l2tp_tunnel_emergency_result_code = NULL;
	}

	if (l2tp_tunnel_event_pipe[0] >= 0) {
		usl_fd_remove_fd(l2tp_tunnel_event_pipe[0]);
		close(l2tp_tunnel_event_pipe[0]);
		l2tp_tunnel_event_pipe[0] = -1;
	}
	if (l2tp_tunnel_event_pipe[1] >= 0) {
		close(l2tp_tunnel_event_pipe[1]);
		l2tp_tunnel_event_pipe[1] = -1;
	}

	usl_list_for_each(walk, tmp, &l2tp_tunnel_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_tunnel_profile, list);
		usl_list_del(&profile->list);
		l2tp_tunnel_profile_free(profile);
	}
}

