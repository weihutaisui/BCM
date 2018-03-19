/* -*- c -*- */

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
 * NOTE: APPLICATIONS USING THIS INTERFACE OR DERIVED FILES THEREOF MUST
 *       EITHER BE LICENSED UNDER GPL OR UNDER AN L2TP COMMERCIAL LICENSE.
 *	 FROM KATALIX SYSTEMS LIMITED.
 *
 *****************************************************************************/

/*
 * L2TP application interface definition.
 *
 * l2tpd is controlled using RPC. This file specifies the interface.
 * Use it to generate RPC C code using rpcgen or other tools to
 * generate code for other environments such as Java.
 *
 * The interface consists of several object types: system, tunnel,
 * session, tunnel-profile, session-profile, ppp-profile,
 * peer-profile.  Each object supports one or more operations: create,
 * delete, modify, get, list.
 *
 * Each object/operation pair has a structure defined which is used to
 * carry data of the operation, either to or from the user.  A flags
 * field is defined for create/modify structures and is used to tell
 * which optional fields are filled in. Note that mandatory
 * (create-only) fields don't always have a flags bit defined.
 *
 * Unfortunately, the RPC IDL does not support the '<<' bitshift
 * operator often used in C and Java. Therefore, bitmask definitions
 * had to be manually calculated and specified in decimal here.
 *
 * RPC unions are avoided as much as possible because the generated
 * types can be cumbersome to use. Structures are shared by many 
 * create, modify and get operations, even though some fields in the
 * structures are unused for some operations.
 *
 * Fields are read-write, unless they are marked create-only or
 * read-only. All fields can be read.
 */

const L2TP_APP_COPYRIGHT_INFO		= "(c) Copyright 2004,2005,2006,2007,2008";
const L2TP_APP_VENDOR_INFO		= "Katalix Systems Ltd.";
const L2TP_APP_MAJOR_VERSION		= 1;
const L2TP_APP_MINOR_VERSION		= 6;

/* Debug masks, used in trace_flags fields */
const L2TP_DEBUG_PROTOCOL			= 1;
const L2TP_DEBUG_FSM				= 2;
const L2TP_DEBUG_API				= 4;
const L2TP_DEBUG_AVP				= 8;
const L2TP_DEBUG_AVP_HIDE			= 16;
const L2TP_DEBUG_AVP_DATA			= 32;
const L2TP_DEBUG_FUNC				= 64;
const L2TP_DEBUG_XPRT				= 128;
const L2TP_DEBUG_DATA				= 256;
const L2TP_DEBUG_SYSTEM				= 512;
const L2TP_DEBUG_PPP				= 1024;

/*****************************************************************************
 * API error codes
 * Offset all error codes away from the standard Unix error codes.
 * Openl2tpd returns these error codes rather than more general Unix
 * codes, such as EINVAL, EEXIST, ENOENT etc. However, sometimes Unix
 * error codes occur, e.g. ENOMEM or socket errors. A C client
 * application can use l2tp_strerror() to convert an error number into
 * a string.
 *****************************************************************************/

const L2TP_ERR_BASE				= 500;
const L2TP_ERR_PARAM_NOT_MODIFIABLE		= 500;
const L2TP_ERR_PEER_ADDRESS_MISSING		= 501;
const L2TP_ERR_PEER_NOT_FOUND			= 502;
const L2TP_ERR_PEER_PROFILE_NOT_FOUND		= 503;
const L2TP_ERR_PPP_PROFILE_NOT_FOUND		= 504;
const L2TP_ERR_PROFILE_ALREADY_EXISTS		= 505;
const L2TP_ERR_PROFILE_NAME_ILLEGAL		= 506;
const L2TP_ERR_PROFILE_NAME_MISSING		= 507;
const L2TP_ERR_SESSION_ALREADY_EXISTS		= 508;
const L2TP_ERR_SESSION_ID_ALLOC_FAILURE		= 509;
const L2TP_ERR_SESSION_LIMIT_EXCEEDED		= 510;
const L2TP_ERR_SESSION_NOT_FOUND		= 511;
const L2TP_ERR_SESSION_PROFILE_NOT_FOUND	= 512;
const L2TP_ERR_SESSION_SPEC_MISSING		= 513;
const L2TP_ERR_SESSION_TYPE_BAD			= 514;
const L2TP_ERR_SESSION_TYPE_ILLEGAL_FOR_TUNNEL	= 515;
const L2TP_ERR_TUNNEL_ADD_ADMIN_DISABLED	= 516;
const L2TP_ERR_TUNNEL_CREATE_ADMIN_DISABLED	= 517;
const L2TP_ERR_TUNNEL_ALREADY_EXISTS		= 518;
const L2TP_ERR_TUNNEL_ID_ALLOC_FAILURE		= 519;
const L2TP_ERR_TUNNEL_NOT_FOUND			= 520;
const L2TP_ERR_TUNNEL_PROFILE_NOT_FOUND		= 521;
const L2TP_ERR_TUNNEL_SPEC_MISSING		= 522;
const L2TP_ERR_TUNNEL_TOO_MANY_SESSIONS		= 523;
const L2TP_ERR_TUNNEL_TOO_MANY_SAME_IP		= 524;
const L2TP_ERR_TUNNEL_LIMIT_EXCEEDED		= 525;
const L2TP_ERR_SESSION_TYPE_NOT_SUPPORTED	= 526;
const L2TP_ERR_TUNNEL_HIDE_AVPS_NEEDS_SECRET	= 527;
const L2TP_ERR_MAX				= 528;

/*****************************************************************************
 * Types shared by several data structures
 *****************************************************************************/

/* TYPE: optional string.
 * Used to pass optional string values in the interface. If we used a plain
 * string type, the RPC user would have to provide a valid string even if 
 * no value was needed. Since strings are often optional parameters, we define
 * a type for it.
 */
union optstring switch (int valid) {
	case 1:
		string value<>;
	default:
		void;
};

%#define OPTSTRING(_var) _var.optstring_u.value
%#define OPTSTRING_PTR(_var) ((_var.valid == 1) ? _var.optstring_u.value : NULL)

enum l2tp_api_tunnel_auth_mode {
	L2TP_API_TUNNEL_AUTH_MODE_NONE,
	L2TP_API_TUNNEL_AUTH_MODE_SIMPLE,
	L2TP_API_TUNNEL_AUTH_MODE_CHALLENGE
};

enum l2tp_api_tunnel_mode {
	L2TP_API_TUNNEL_MODE_LACLNS,
	L2TP_API_TUNNEL_MODE_LAC,
	L2TP_API_TUNNEL_MODE_LNS
};

/* TYPE: IP address
 * Fakes the standard sockaddr_in type.
 */
struct l2tp_api_ip_addr {
	uint32_t	s_addr;
};

/*****************************************************************************
 * Application:-
 * TYPE: struct l2tp_api_app_msg_data
 * USE:  to retrieve build and version info
 *****************************************************************************/

const L2TP_API_APP_FEATURE_LOCAL_CONF_FILE		= 1;
const L2TP_API_APP_FEATURE_LAC_SUPPORT			= 2;
const L2TP_API_APP_FEATURE_LNS_SUPPORT			= 4;
const L2TP_API_APP_FEATURE_RPC_MANAGEMENT		= 8;
const L2TP_API_APP_FEATURE_LAIC_SUPPORT			= 16;
const L2TP_API_APP_FEATURE_LAOC_SUPPORT			= 32;
const L2TP_API_APP_FEATURE_LNIC_SUPPORT			= 64;
const L2TP_API_APP_FEATURE_LNOC_SUPPORT			= 128;
const L2TP_API_APP_FEATURE_LOCAL_STAT_FILE		= 256;

struct l2tp_api_app_msg_data {
	string 				build_date<16>;
	string 				build_time<16>;
	int 				major;
	int 				minor;	
	uint32_t			cookie;
	int				patches<>;
	uint32_t			features;
};

/*****************************************************************************
 * System:-
 * TYPE: struct l2tp_api_system_msg_data:
 * USE:  to change global parameters of l2tpd.
 * 	 Also reports global status and counters.
 *****************************************************************************/

const L2TP_API_CONFIG_FLAG_TRACE_FLAGS			= 1;
const L2TP_API_CONFIG_FLAG_MAX_TUNNELS			= 2;
const L2TP_API_CONFIG_FLAG_DRAIN_TUNNELS		= 4;
const L2TP_API_CONFIG_FLAG_MAX_SESSIONS			= 8;
const L2TP_API_CONFIG_FLAG_TUNNEL_ESTABLISH_TIMEOUT	= 16;
const L2TP_API_CONFIG_FLAG_SESSION_ESTABLISH_TIMEOUT	= 32;
const L2TP_API_CONFIG_FLAG_DENY_LOCAL_TUNNEL_CREATES	= 64;
const L2TP_API_CONFIG_FLAG_DENY_REMOTE_TUNNEL_CREATES	= 128;
const L2TP_API_CONFIG_FLAG_RESET_STATISTICS		= 256;
const L2TP_API_CONFIG_FLAG_TUNNEL_PERSIST_PEND_TIMEOUT	= 512;
const L2TP_API_CONFIG_FLAG_SESSION_PERSIST_PEND_TIMEOUT	= 1024;

struct l2tp_api_system_config {
	uint32_t			flags;				/* which fields are set? */
	int				udp_port;
	uint32_t			trace_flags;
	uint32_t			trace_flags_mask;
	int				max_tunnels;
	int				max_sessions;
	int				drain_tunnels;
	int				tunnel_establish_timeout;
	int				session_establish_timeout;
	int				tunnel_persist_pend_timeout;
	int				session_persist_pend_timeout;
	bool				deny_local_tunnel_creates;
	bool				deny_remote_tunnel_creates;
};

struct l2tp_api_system_msg_stats {
	uint32_t			rx;
	uint32_t			tx;
	uint32_t			rx_bad;
};

/* There are 17 standard L2TP message types */
const L2TP_API_MSG_TYPE_COUNT		= 17;

struct l2tp_api_system_stats {
	uint32_t			short_frames;
	uint32_t			wrong_version_frames;
	uint32_t			unexpected_data_frames;
	uint32_t			bad_rcvd_frames;
	uint32_t			total_rcvd_control_frames;
	uint32_t			total_sent_control_frames;
	uint32_t			total_control_frame_send_fails;
	uint32_t			total_retransmitted_control_frames;
	uint32_t			no_control_frame_resources;
	uint32_t			no_peer_resources;
	uint32_t			no_tunnel_resources;
	uint32_t			no_session_resources;
	uint32_t			no_ppp_resources;
	uint32_t			too_many_tunnels;
	uint32_t			too_many_sessions;
	uint32_t			auth_fails;
	uint32_t			no_matching_tunnel_id_discards;
	uint32_t			no_matching_session_id_discards;
	uint32_t			mismatched_tunnel_ids;
	uint32_t			mismatched_session_ids;
	uint32_t			encode_message_fails;
	uint32_t			tunnel_setup_failures;
	uint32_t			session_setup_failures;
	uint32_t			event_queue_full_errors;
	uint32_t			ignored_avps;
	uint32_t			vendor_avps;
	uint32_t			illegal_messages;
	uint32_t			unsupported_messages;
	struct l2tp_api_system_msg_stats messages<L2TP_API_MSG_TYPE_COUNT>;
};

struct l2tp_api_system_status {
	struct l2tp_api_system_stats	stats;
	uint32_t			num_tunnels;
	uint32_t			num_sessions;
};

struct l2tp_api_system_msg_data {
	struct l2tp_api_system_config	config;
	struct l2tp_api_system_status	status;
};

/*****************************************************************************
 * Peers:-
 * Peers are created automatically by the system to record information about
 * each peer to l2tpd. This provides a useful way of finding a list of all
 * remote systems that l2tpd is communicating with, how many tunnels
 * exist to each peer, etc etc.
 *
 * TYPE: struct l2tp_api_peer_msg_data
 * USE:  get peer
 * TYPE: struct l2tp_api_peer_list_msg_data
 * USE:  list peers
 *****************************************************************************/

struct l2tp_api_peer_msg_data {
	int				result_code;				/* read-only */
	struct l2tp_api_ip_addr		peer_addr;				/* read-only */
	struct l2tp_api_ip_addr		local_addr;				/* read-only */
	int				num_tunnels;				/* read-only */
};

struct l2tp_api_peer_list_entry {
	struct l2tp_api_ip_addr		peer_addr;				/* read-only */
	struct l2tp_api_ip_addr		local_addr;				/* read-only */
	struct l2tp_api_peer_list_entry *next;
};

struct l2tp_api_peer_list_msg_data {
	int				result;
	struct l2tp_api_peer_list_entry *peers;
};

/*****************************************************************************
 * Peer profiles:-
 * These are used to call out behavior characteristics for the peer. For 
 * incoming tunnel setup requests, the system tries to find a peer profile
 * of a name that matches the peer's L2TP HOST_NAME AVP.
 *
 * TYPE: struct l2tp_api_peer_profile_msg_data
 * USE:  create/delete/modify/get peer profiles
 * TYPE: struct l2tp_api_peer_profile_list_msg_data
 * USE:  list peer profiles
 *****************************************************************************/

const L2TP_API_PEER_PROFILE_DEFAULT_PROFILE_NAME	= "default";

const L2TP_API_PEER_PROFILE_FLAG_LACLNS			= 1;
const L2TP_API_PEER_PROFILE_FLAG_TUNNEL_PROFILE_NAME	= 2;
const L2TP_API_PEER_PROFILE_FLAG_SESSION_PROFILE_NAME	= 4;
const L2TP_API_PEER_PROFILE_FLAG_PPP_PROFILE_NAME	= 8;
const L2TP_API_PEER_PROFILE_FLAG_PEER_IPADDR		= 16;
const L2TP_API_PEER_PROFILE_FLAG_PEER_PORT		= 32;
const L2TP_API_PEER_PROFILE_FLAG_NETMASK		= 64;

struct l2tp_api_peer_profile_msg_data {
	uint32_t			flags;
	int				result_code;			/* read-only */
	struct l2tp_api_ip_addr		peer_addr;
	uint16_t			peer_port;
	bool				we_can_be_lac;
	bool				we_can_be_lns;
	string				profile_name<>;			/* create-only */
	optstring			default_tunnel_profile_name;
	optstring			default_session_profile_name;
	optstring			default_ppp_profile_name;
	struct l2tp_api_ip_addr		netmask;
	int				use_count;			/* read-only */
};

struct l2tp_api_peer_profile_list_entry {
	string				profile_name<>;
	struct l2tp_api_peer_profile_list_entry *next;
};

struct l2tp_api_peer_profile_list_msg_data {
	int					result;
	int					num_profiles;
	struct l2tp_api_peer_profile_list_entry	*profiles;
};

struct l2tp_api_peer_profile_unset_msg_data {
	uint32_t			flags;
	int				result_code;			/* read-only */
	string				profile_name<>;
};

/*****************************************************************************
 * Tunnels
 * TYPE: struct l2tp_api_tunnel_msg_data
 * USE:  create/delete/modify/get tunnels
 * TYPE: struct l2tp_api_tunnel_list_msg_data
 * USE:  list tunnels
 *****************************************************************************/

struct l2tp_api_tunnel_peer {
	/* status received from peer - read-only */
	bool			framing_cap_sync;
	bool			framing_cap_async;
	bool			bearer_cap_digital;
	bool			bearer_cap_analog;
	uint8_t			protocol_version_ver;
	uint8_t			protocol_version_rev;
	uint16_t		rx_window_size;
	uint16_t		firmware_revision;
	optstring		host_name;
	optstring		vendor_name;
	uint8_t			tiebreaker<8>;
	uint16_t		result_code_result;
	uint16_t		result_code_error;
	optstring		result_code_message;
};

struct l2tp_api_tunnel_stats {
	uint32_t		retransmits;
	uint32_t		tx_zlbs;
	uint32_t		tx_zlb_fails;
	uint32_t		rx_zlbs;
	uint32_t		data_pkt_discards;
	uint32_t		duplicate_pkt_discards;
	uint32_t		rx_hellos;
	uint32_t		tx_hellos;
	uint32_t		tx_hello_fails;
	uint16_t		ns;
	uint16_t		nr;
	uint16_t		peer_ns;
	uint16_t		peer_nr;
	uint16_t		cwnd;
	uint16_t		ssthresh;
	uint16_t		congpkt_acc;
	uint64_t		control_rx_packets;
	uint64_t		control_rx_bytes;
	uint64_t		control_rx_oos_packets;
	uint64_t		control_rx_oos_discards;
	uint64_t		control_tx_packets;
	uint64_t		control_tx_bytes;
	uint64_t		data_rx_packets;
	uint64_t		data_rx_bytes;
	uint64_t		data_rx_errors;
	uint64_t		data_rx_oos_packets;
	uint64_t		data_rx_oos_discards;
	uint64_t		data_tx_packets;
	uint64_t		data_tx_bytes;
	uint64_t		data_tx_errors;
	bool			using_ipsec;
};

/* flags */
const L2TP_API_TUNNEL_FLAG_HIDE_AVPS		= 1;
const L2TP_API_TUNNEL_FLAG_OUR_ADDR		= 2;
const L2TP_API_TUNNEL_FLAG_PEER_ADDR		= 4;
const L2TP_API_TUNNEL_FLAG_CONFIG_ID		= 8;
const L2TP_API_TUNNEL_FLAG_OUR_UDP_PORT		= 16;
const L2TP_API_TUNNEL_FLAG_AUTH_MODE		= 32;
const L2TP_API_TUNNEL_FLAG_FRAMING_CAP		= 64;
const L2TP_API_TUNNEL_FLAG_BEARER_CAP		= 128;
const L2TP_API_TUNNEL_FLAG_USE_TIEBREAKER	= 256;
const L2TP_API_TUNNEL_FLAG_PERSIST		= 512;
const L2TP_API_TUNNEL_FLAG_HELLO_TIMEOUT	= 1024;
const L2TP_API_TUNNEL_FLAG_MAX_RETRIES		= 2048;
const L2TP_API_TUNNEL_FLAG_RX_WINDOW_SIZE	= 4096;
const L2TP_API_TUNNEL_FLAG_TX_WINDOW_SIZE	= 8192;
const L2TP_API_TUNNEL_FLAG_RETRY_TIMEOUT	= 16384;
const L2TP_API_TUNNEL_FLAG_IDLE_TIMEOUT		= 32768;
const L2TP_API_TUNNEL_FLAG_PEER_UDP_PORT	= 65536;
const L2TP_API_TUNNEL_FLAG_SECRET		= 131072;
const L2TP_API_TUNNEL_FLAG_ALLOW_PPP_PROXY	= 262144;
const L2TP_API_TUNNEL_FLAG_TRACE_FLAGS		= 524288;
const L2TP_API_TUNNEL_FLAG_USE_UDP_CHECKSUMS	= 1048576;
const L2TP_API_TUNNEL_FLAG_HOST_NAME		= 2097152;
const L2TP_API_TUNNEL_FLAG_TUNNEL_ID		= 4194304;
const L2TP_API_TUNNEL_FLAG_PROFILE_NAME		= 8388608;
const L2TP_API_TUNNEL_FLAG_MAX_SESSIONS		= 16777216;
const L2TP_API_TUNNEL_FLAG_TUNNEL_NAME		= 33554432;
const L2TP_API_TUNNEL_FLAG_UNUSED_2		= 67108864;
const L2TP_API_TUNNEL_FLAG_PEER_PROFILE_NAME	= 134217728;
const L2TP_API_TUNNEL_FLAG_SESSION_PROFILE_NAME	= 268435456;
const L2TP_API_TUNNEL_FLAG_PPP_PROFILE_NAME	= 536870912;
const L2TP_API_TUNNEL_FLAG_DO_PMTU_DISCOVERY	= 1073741824;
const L2TP_API_TUNNEL_FLAG_MTU			= 2147483648;

/* flags2 */
const L2TP_API_TUNNEL_FLAG_INTERFACE_NAME	= 1;

struct l2tp_api_tunnel_msg_data {
	uint32_t			flags;
	uint32_t			flags2;
       	uint16_t			tunnel_id; 		/* used to identify tunnel */
       	uint16_t			peer_tunnel_id;		/* read-only */
	int				result_code;		/* valid for GET only */
	int				hide_avps;
	optstring			tunnel_name;
	struct l2tp_api_ip_addr		our_addr; 		/* create-only */
	struct l2tp_api_ip_addr		peer_addr;		/* create-only */
	uint16_t			our_udp_port;		/* create-only */
	uint16_t			peer_udp_port; 		/* create-only */
	int				config_id; 		/* create-only */
	enum l2tp_api_tunnel_auth_mode	auth_mode; 		/* create-only */
	bool				framing_cap_sync;	/* create-only */
	bool				framing_cap_async;	/* create-only */
	bool				bearer_cap_digital;	/* create-only */
	bool				bearer_cap_analog;	/* create-only */
	bool				use_tiebreaker;		/* create-only */
	bool				allow_ppp_proxy; 	/* create-only */
	bool				use_udp_checksums;
	bool				created_by_admin; 	/* read-only */
	optstring			create_time;		/* read-only */
	int				max_sessions;
	int				num_sessions;		/* read-only */
	int				use_count;		/* read-only */
	int				num_establish_retries;	/* read-only */
	enum l2tp_api_tunnel_mode	mode;			/* create-only */
	int				hello_timeout;
	int				max_retries;
	uint16_t			rx_window_size;
	uint16_t			tx_window_size;
	uint16_t			actual_tx_window_size;	/* read-only */
	int				retry_timeout;
	int				idle_timeout;
	uint32_t			trace_flags;
	uint32_t			trace_flags_mask;
	bool				do_pmtu_discovery;
	bool				persist;
	int				mtu;
	optstring			state;			/* read-only */
	optstring			secret;			/* create-only */
	optstring			host_name; 		/* create-only */
	optstring			tunnel_profile_name;	/* create-only */
	optstring			peer_profile_name;
	optstring			session_profile_name;
	optstring			ppp_profile_name;
	optstring			interface_name;
	uint8_t				tiebreaker<8>;		/* read-only */
	uint16_t			result_code_result;	/* read-only */
	uint16_t			result_code_error;	/* read-only */
	optstring			result_code_message;	/* read-only */
	struct l2tp_api_tunnel_peer	peer;			/* read-only */
	struct l2tp_api_tunnel_stats	stats;			/* read-only */
};

struct l2tp_api_tunnel_list_msg_data {
	int				result;			/* success or reason for error */
	uint16_t			tunnel_ids<>;		/* list of tunnel ids */
};

/*****************************************************************************
 * Tunnel profiles
 * TYPE: struct l2tp_api_tunnel_profile_msg_data
 * USE:  create/delete/modify/get tunnel profiles
 * TYPE: struct l2tp_api_tunnel_profile_list_msg_data
 * USE:  list tunnel profiles
 *****************************************************************************/

/* Default values for tunnel profile attributes.
 * These are used if an explicit value is not provided by the user.
 */
const L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME		= "default";
const L2TP_API_TUNNEL_PROFILE_DEFAULT_AUTH_MODE			= L2TP_API_TUNNEL_AUTH_MODE_NONE;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_FRAMING_CAP_SYNC		= 1;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_FRAMING_CAP_ASYNC		= 1;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_BEARER_CAP_DIGITAL	= 1;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_BEARER_CAP_ANALOG		= 1;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_USE_TIEBREAKER		= 0;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_HELLO_TIMEOUT		= 60;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_MAX_RETRIES		= 5;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_RX_WINDOW_SIZE		= 10;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_TX_WINDOW_SIZE		= 10;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_RETRY_TIMEOUT		= 1;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_IDLE_TIMEOUT		= 0;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_ZLB_TIMEOUT		= 5;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_TRACE_FLAGS		= 0;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_ALLOW_PPP_PROXY		= 0;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_USE_UDP_CHECKSUMS		= 1;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_DO_PMTU_DISCOVERY		= 0;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_MTU			= 1460;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_HIDE_AVPS			= 0;
const L2TP_API_TUNNEL_PROFILE_DEFAULT_MAX_SESSIONS		= 0;

/* flags */
const L2TP_API_TUNNEL_PROFILE_FLAG_HIDE_AVPS		= 1;       
const L2TP_API_TUNNEL_PROFILE_FLAG_AUTH_MODE		= 2;       
const L2TP_API_TUNNEL_PROFILE_FLAG_FRAMING_CAP		= 4;       
const L2TP_API_TUNNEL_PROFILE_FLAG_BEARER_CAP		= 8;       
const L2TP_API_TUNNEL_PROFILE_FLAG_USE_TIEBREAKER	= 16;      
const L2TP_API_TUNNEL_PROFILE_FLAG_UNUSED_1		= 32;      
const L2TP_API_TUNNEL_PROFILE_FLAG_HELLO_TIMEOUT	= 64;      
const L2TP_API_TUNNEL_PROFILE_FLAG_MAX_RETRIES		= 128;     
const L2TP_API_TUNNEL_PROFILE_FLAG_RX_WINDOW_SIZE	= 256;     
const L2TP_API_TUNNEL_PROFILE_FLAG_TX_WINDOW_SIZE	= 512;     
const L2TP_API_TUNNEL_PROFILE_FLAG_RETRY_TIMEOUT	= 1024;    
const L2TP_API_TUNNEL_PROFILE_FLAG_IDLE_TIMEOUT		= 2048;    
const L2TP_API_TUNNEL_PROFILE_FLAG_SECRET		= 4096;    
const L2TP_API_TUNNEL_PROFILE_FLAG_ALLOW_PPP_PROXY	= 8192;    
const L2TP_API_TUNNEL_PROFILE_FLAG_TRACE_FLAGS		= 16384;   
const L2TP_API_TUNNEL_PROFILE_FLAG_USE_UDP_CHECKSUMS	= 32768;   
const L2TP_API_TUNNEL_PROFILE_FLAG_HOST_NAME		= 65536;   
const L2TP_API_TUNNEL_PROFILE_FLAG_MAX_SESSIONS		= 131072;   
const L2TP_API_TUNNEL_PROFILE_FLAG_OUR_ADDR		= 262144;
const L2TP_API_TUNNEL_PROFILE_FLAG_PEER_ADDR		= 524288;
const L2TP_API_TUNNEL_PROFILE_FLAG_OUR_UDP_PORT		= 1048576;
const L2TP_API_TUNNEL_PROFILE_FLAG_PEER_UDP_PORT	= 2097152;
const L2TP_API_TUNNEL_PROFILE_FLAG_UNUSED_2		= 4194304;
const L2TP_API_TUNNEL_PROFILE_FLAG_PEER_PROFILE_NAME	= 8388608;
const L2TP_API_TUNNEL_PROFILE_FLAG_SESSION_PROFILE_NAME	= 16777216;
const L2TP_API_TUNNEL_PROFILE_FLAG_PPP_PROFILE_NAME	= 33554432;
const L2TP_API_TUNNEL_PROFILE_FLAG_DO_PMTU_DISCOVERY	= 67108864;
const L2TP_API_TUNNEL_PROFILE_FLAG_MTU			= 134217728;

struct l2tp_api_tunnel_profile_msg_data {
	uint32_t			flags;
	string				profile_name<>;		/* create-only */
	int				result_code;		/* read-only */
	int				hide_avps;
	enum l2tp_api_tunnel_auth_mode	auth_mode;
	bool				framing_cap_sync;
	bool				framing_cap_async;
	bool				bearer_cap_digital;
	bool				bearer_cap_analog;
	bool				use_tiebreaker;
	bool				allow_ppp_proxy;
	bool				use_udp_checksums;
	int				hello_timeout;
	int				max_retries;
	uint16_t			rx_window_size;
	uint16_t			tx_window_size;
	int				retry_timeout;
	int				idle_timeout;
	uint32_t			trace_flags;
	uint32_t			trace_flags_mask;
	int				max_sessions;
	struct l2tp_api_ip_addr		our_addr;
	struct l2tp_api_ip_addr		peer_addr;
	uint16_t			our_udp_port;
	uint16_t			peer_udp_port;
	bool				do_pmtu_discovery;
	int				mtu;
	optstring			peer_profile_name;
	optstring			session_profile_name;
	optstring			ppp_profile_name;
	optstring			secret;
	optstring			host_name;
};

struct l2tp_api_tunnel_profile_list_entry {
	string						profile_name<>;
	struct l2tp_api_tunnel_profile_list_entry	*next;
};

struct l2tp_api_tunnel_profile_list_msg_data {
	int						result;
	int						num_profiles;
	struct l2tp_api_tunnel_profile_list_entry	*profiles;
};

struct l2tp_api_tunnel_profile_unset_msg_data {
	uint32_t			flags;
	int				result_code;			/* read-only */
	string				profile_name<>;
};

/*****************************************************************************
 * Sessions 
 * TYPE: struct l2tp_api_session_msg_data
 * USE:  create/delete/modify/get sessions
 * TYPE: struct l2tp_api_session_list_msg_data
 * USE:  list sessions
 *****************************************************************************/

const L2TP_API_SESSION_DEFAULT_SESSION_TYPE			= L2TP_API_SESSION_TYPE_UNSPECIFIED;

/* flags */
const L2TP_API_SESSION_FLAG_TRACE_FLAGS			= 1;
const L2TP_API_SESSION_FLAG_SEQUENCING_REQUIRED		= 2;
const L2TP_API_SESSION_FLAG_PPP_PROFILE_NAME		= 4;
const L2TP_API_SESSION_FLAG_SESSION_TYPE		= 8;
const L2TP_API_SESSION_FLAG_PRIV_GROUP_ID		= 16;
const L2TP_API_SESSION_FLAG_FRAMING_TYPE		= 32;
const L2TP_API_SESSION_FLAG_BEARER_TYPE			= 64;
const L2TP_API_SESSION_FLAG_MINIMUM_BPS			= 128;
const L2TP_API_SESSION_FLAG_MAXIMUM_BPS			= 256;
const L2TP_API_SESSION_FLAG_CONNECT_SPEED		= 512;
const L2TP_API_SESSION_FLAG_USE_PPP_PROXY		= 1024;
const L2TP_API_SESSION_FLAG_PROXY_AUTH_TYPE		= 8192;
const L2TP_API_SESSION_FLAG_PROXY_AUTH_NAME		= 16384;
const L2TP_API_SESSION_FLAG_PROXY_AUTH_CHALLENGE	= 32768;
const L2TP_API_SESSION_FLAG_PROXY_AUTH_RESPONSE		= 65536;
const L2TP_API_SESSION_FLAG_CALLING_NUMBER		= 131072;
const L2TP_API_SESSION_FLAG_CALLED_NUMBER		= 262144;
const L2TP_API_SESSION_FLAG_SUB_ADDRESS			= 524288;
const L2TP_API_SESSION_FLAG_INITIAL_RCVD_LCP_CONFREQ	= 1048576;
const L2TP_API_SESSION_FLAG_LAST_SENT_LCP_CONFREQ	= 2097152;
const L2TP_API_SESSION_FLAG_LAST_RCVD_LCP_CONFREQ	= 4194304;
const L2TP_API_SESSION_FLAG_USER_NAME			= 8388608;
const L2TP_API_SESSION_FLAG_USER_PASSWORD		= 16777216;
const L2TP_API_SESSION_FLAG_USE_SEQUENCE_NUMBERS	= 33554432;
const L2TP_API_SESSION_FLAG_REORDER_TIMEOUT		= 67108864;
const L2TP_API_SESSION_FLAG_TUNNEL_NAME			= 134217728;
const L2TP_API_SESSION_FLAG_SESSION_NAME		= 268435456;
const L2TP_API_SESSION_FLAG_INTERFACE_NAME		= 536870912;
const L2TP_API_SESSION_FLAG_PROFILE_NAME		= 1073741824;
const L2TP_API_SESSION_FLAG_NO_PPP			= 2147483648;

enum l2tp_api_session_proxy_auth_type {
	L2TP_API_SESSION_PROXY_AUTH_TYPE_RESERVED 		= 0,
	L2TP_API_SESSION_PROXY_AUTH_TYPE_PLAIN_TEXT 		= 1,
	L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_CHAP 		= 2,
	L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_PAP 		= 3,
	L2TP_API_SESSION_PROXY_AUTH_TYPE_NO_AUTH 		= 4,
	L2TP_API_SESSION_PROXY_AUTH_TYPE_PPP_MSCHAPV1 		= 5
};

struct l2tp_api_session_call_errors {
	uint32_t			crc_errors;
	uint32_t			framing_errors;
	uint32_t			hardware_overruns;
	uint32_t			buffer_overruns;
	uint32_t			timeout_errors;
	uint32_t			alignment_errors;
};

struct l2tp_api_session_stats {
	uint64_t			data_rx_packets;
	uint64_t			data_rx_bytes;
	uint64_t			data_rx_errors;
	uint64_t			data_rx_oos_packets;
	uint64_t			data_rx_oos_discards;
	uint64_t			data_tx_packets;
	uint64_t			data_tx_bytes;
	uint64_t			data_tx_errors;
};

struct l2tp_api_session_peer_info {
	uint16_t			result_code;
	uint16_t			error_code;
	optstring			error_message;
	uint32_t			minimum_bps;
	uint32_t			maximum_bps;
	uint32_t			connect_speed;
	uint32_t			rx_connect_speed;
	enum l2tp_api_session_proxy_auth_type proxy_auth_type;
	uint8_t				proxy_auth_id;
	optstring			proxy_auth_name;
	uint8_t				proxy_auth_challenge<>;
	uint8_t				proxy_auth_response<>;
	optstring			private_group_id;
	bool				framing_type_sync;
	bool				framing_type_async;
	bool				bearer_type_digital;
	bool				bearer_type_analog;
	bool				sequencing_required;
	uint32_t			call_serial_number;
	uint32_t			physical_channel_id;
	uint8_t				initial_rcvd_lcp_confreq<>;
	uint8_t				last_sent_lcp_confreq<>;
	uint8_t				last_rcvd_lcp_confreq<>;
	optstring			calling_number;
	optstring			called_number;
	optstring			sub_address;
	uint16_t			q931_cause_code;
	uint16_t			q931_cause_msg;
	optstring			q931_advisory_msg;
	struct l2tp_api_session_call_errors call_errors;
	uint32_t			send_accm;
	uint32_t			recv_accm;
};

enum l2tp_api_session_type {
	L2TP_API_SESSION_TYPE_UNSPECIFIED,
	L2TP_API_SESSION_TYPE_LAIC,
	L2TP_API_SESSION_TYPE_LAOC,
	L2TP_API_SESSION_TYPE_LNIC,
	L2TP_API_SESSION_TYPE_LNOC
};

struct l2tp_api_session_msg_data {
	uint32_t			flags; 			/* which fields are set? */
	uint16_t			tunnel_id;		/* read-only, or used to identify session */
       	uint16_t			session_id; 		/* read-only, or used to identify session */
	int				result_code;		/* read-only */
	optstring			tunnel_name; 		/* alternative for tunnel_id to identify tunnel */
       	uint16_t			peer_session_id;	/* read-only */
	optstring			state;			/* read-only */
	optstring			session_name;
	optstring			interface_name;		/* create-only */
	enum l2tp_api_session_type	session_type;		/* create-only */
	optstring			user_name; 		/* create-only */
	optstring			user_password; 		/* create-only */
	optstring			priv_group_id; 		/* create-only */
	optstring			profile_name; 		/* create-only */
	bool				created_by_admin;	/* read-only */
	optstring			create_time;		/* read-only */
	bool				no_ppp;			/* create-only */
	bool				sequencing_required;
	bool				use_sequence_numbers;
	int				reorder_timeout;
	bool				framing_type_sync; 	/* create-only */
	bool				framing_type_async; 	/* create-only */
	bool				bearer_type_digital;	/* create-only */
	bool				bearer_type_analog;	/* create-only */
	uint32_t			call_serial_number;	/* read-only */
	uint32_t			physical_channel_id;	/* read-only */
	uint32_t			trace_flags;
	uint32_t			trace_flags_mask;
	int				minimum_bps;		/* create-only */
	int				maximum_bps;		/* create-only */
	int				tx_connect_speed;	/* create-only */
	int				rx_connect_speed;	/* create-only, defaults to tx_connect_speed */
	struct l2tp_api_session_peer_info peer;			/* read-only, status received from peer */
	optstring			ppp_profile_name;	/* create-only */
	struct l2tp_api_session_stats 	stats;	 		/* read-only */

	/* These create-only attributes are for PPP-proxy support. Not yet implemented */
	bool				use_ppp_proxy;
	enum l2tp_api_session_proxy_auth_type proxy_auth_type;
	optstring			proxy_auth_name;
	uint8_t				proxy_auth_challenge<>;
	uint8_t				proxy_auth_response<>;
	optstring			calling_number;
	optstring			called_number;
	optstring			sub_address;
	uint8_t				initial_rcvd_lcp_confreq<>;
	uint8_t				last_sent_lcp_confreq<>;
	uint8_t				last_rcvd_lcp_confreq<>;
};

struct l2tp_api_session_list_msg_data {
	int				result;			/* success or reason for error */
	uint16_t			session_ids<>;		/* list of session ids */
};

struct l2tp_api_session_incall_msg_data {
	int				ppp_unit;
	string				tunnel_profile_name<>;
	struct l2tp_api_session_msg_data session;
};

/*****************************************************************************
 * Session profiles 
 * TYPE: struct l2tp_api_session_profile_msg_data
 * USE:  create/delete/modify/get session profiles
 * TYPE: struct l2tp_api_session_profile_list_msg_data
 * USE:  list session profiles
 *****************************************************************************/

/* Default values for session profile attributes.
 * These are used if an explicit value is not provided by the user.
 */
const L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME			= "default";
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_TRACE_FLAGS		= 0;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_SEQUENCING_REQUIRED	= 0;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_TYPE			= L2TP_API_SESSION_TYPE_UNSPECIFIED;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_FRAMING_TYPE_SYNC	= 1;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_FRAMING_TYPE_ASYNC	= 1;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_BEARER_TYPE_ANALOG	= 1;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_BEARER_TYPE_DIGITAL	= 1;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_MINIMUM_BPS		= 0;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_MAXIMUM_BPS		= 0;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_TX_CONNECT_SPEED		= 1000000;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_RX_CONNECT_SPEED		= 0;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_USE_PPP_PROXY		= FALSE;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_USE_SEQUENCE_NUMBERS	= FALSE;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_REORDER_TIMEOUT		= 0;
const L2TP_API_SESSION_PROFILE_DEFAULT_SESSION_NO_PPP			= FALSE;

/* flags */
const L2TP_API_SESSION_PROFILE_FLAG_TRACE_FLAGS				= 1;
const L2TP_API_SESSION_PROFILE_FLAG_SEQUENCING_REQUIRED			= 2;
const L2TP_API_SESSION_PROFILE_FLAG_PPP_PROFILE_NAME			= 4;
const L2TP_API_SESSION_PROFILE_FLAG_SESSION_TYPE			= 8;
const L2TP_API_SESSION_PROFILE_FLAG_PRIV_GROUP_ID			= 16;
const L2TP_API_SESSION_PROFILE_FLAG_FRAMING_TYPE			= 32;
const L2TP_API_SESSION_PROFILE_FLAG_BEARER_TYPE				= 64;
const L2TP_API_SESSION_PROFILE_FLAG_MINIMUM_BPS				= 128;
const L2TP_API_SESSION_PROFILE_FLAG_MAXIMUM_BPS				= 256;
const L2TP_API_SESSION_PROFILE_FLAG_CONNECT_SPEED			= 512;
const L2TP_API_SESSION_PROFILE_FLAG_USE_PPP_PROXY			= 1024;
const L2TP_API_SESSION_PROFILE_FLAG_USE_SEQUENCE_NUMBERS		= 2048;
const L2TP_API_SESSION_PROFILE_FLAG_REORDER_TIMEOUT			= 4096;
const L2TP_API_SESSION_PROFILE_FLAG_NO_PPP				= 8192;

struct l2tp_api_session_profile_msg_data {
	uint32_t			flags;
	int				result_code;		/* read-only */
	string				profile_name<>;		/* create-only */
	bool				sequencing_required;
	bool				use_sequence_numbers;
	bool				no_ppp;
	int				reorder_timeout;
	bool				do_pmtu_discovery;
	int				mtu;
	int				mru;
	uint32_t			trace_flags;
	uint32_t			trace_flags_mask;
	optstring			ppp_profile_name;
	enum l2tp_api_session_type	session_type;
	optstring			priv_group_id;
	bool				framing_type_sync;
	bool				framing_type_async;
	bool				bearer_type_digital;
	bool				bearer_type_analog;
	int				minimum_bps;
	int				maximum_bps;
	int				tx_connect_speed;
	int				rx_connect_speed;	/* defaults to tx_connect_speed */
	bool				use_ppp_proxy;
};

struct l2tp_api_session_profile_list_entry {
	string						profile_name<>;
	struct l2tp_api_session_profile_list_entry	*next;
};

struct l2tp_api_session_profile_list_msg_data {
	int						result;
	int						num_profiles;
	struct l2tp_api_session_profile_list_entry	*profiles;
};

struct l2tp_api_session_profile_unset_msg_data {
	uint32_t			flags;
	int				result_code;			/* read-only */
	string				profile_name<>;
};

/*****************************************************************************
 * PPP profiles 
 * TYPE: struct l2tp_api_ppp_profile_msg_data
 * USE:  create/delete/modify/get ppp profiles
 * TYPE: struct l2tp_api_ppp_profile_list_msg_data
 * USE:  list ppp profiles
 *****************************************************************************/

/* There are more than 32 configurable parameters of PPP so we use
 * two separate flags fields.
 */
/* flags1... */
const L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS			= 1;
const L2TP_API_PPP_PROFILE_FLAG_ASYNCMAP			= 2;
const L2TP_API_PPP_PROFILE_FLAG_MRU				= 4;
const L2TP_API_PPP_PROFILE_FLAG_MTU				= 8;
const L2TP_API_PPP_PROFILE_FLAG_USE_RADIUS			= 32;
const L2TP_API_PPP_PROFILE_FLAG_RADIUS_HINT			= 64;
const L2TP_API_PPP_PROFILE_FLAG_SYNC_MODE			= 256;
const L2TP_API_PPP_PROFILE_FLAG_CHAP_INTERVAL			= 512;
const L2TP_API_PPP_PROFILE_FLAG_CHAP_MAX_CHALLENGE		= 1024;
const L2TP_API_PPP_PROFILE_FLAG_CHAP_RESTART			= 2048;
const L2TP_API_PPP_PROFILE_FLAG_PAP_MAX_AUTH_REQUESTS		= 4096;
const L2TP_API_PPP_PROFILE_FLAG_PAP_RESTART_INTERVAL		= 8192;
const L2TP_API_PPP_PROFILE_FLAG_PAP_TIMEOUT			= 16384;
const L2TP_API_PPP_PROFILE_FLAG_IDLE_TIMEOUT			= 32768;
const L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_REQUESTS	= 65536;
const L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_NAKS		= 131072;
const L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_TERMINATE_REQUESTS	= 262144;
const L2TP_API_PPP_PROFILE_FLAG_IPCP_RETRANSMIT_INTERVAL	= 524288;
const L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_FAILURE_COUNT		= 1048576;
const L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_INTERVAL		= 2097152;
const L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_REQUESTS		= 4194304;
const L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_NAKS		= 8388608;
const L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_TERMINATE_REQUESTS	= 16777216;
const L2TP_API_PPP_PROFILE_FLAG_LCP_RETRANSMIT_INTERVAL		= 33554432;
const L2TP_API_PPP_PROFILE_FLAG_MAX_CONNECT_TIME		= 67108864;
const L2TP_API_PPP_PROFILE_FLAG_MAX_FAILURE_COUNT		= 134217728;

/* flags2... */
const L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_1			= 1;
const L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_2			= 2;
const L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_1			= 4;
const L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_2			= 8;
const L2TP_API_PPP_PROFILE_FLAG_LOCAL_IP_ADDR			= 16;
const L2TP_API_PPP_PROFILE_FLAG_PEER_IP_ADDR			= 32;
const L2TP_API_PPP_PROFILE_FLAG_IP_POOL_NAME			= 64;
const L2TP_API_PPP_PROFILE_FLAG_USE_AS_DEFAULT_ROUTE		= 128;
const L2TP_API_PPP_PROFILE_FLAG_MULTILINK			= 256;
const L2TP_API_PPP_PROFILE_FLAG_LOCAL_NAME			= 512;
const L2TP_API_PPP_PROFILE_FLAG_REMOTE_NAME			= 1024;
const L2TP_API_PPP_PROFILE_FLAG_PROXY_ARP			= 2048;
const L2TP_API_PPP_PROFILE_FLAG_AUTH_NONE			= 4096;
const L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_EAP			= 8192;
const L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAPV2		= 16384;
const L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAP		= 32768;
const L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_CHAP		= 65536;
const L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_PAP			= 131072;
const L2TP_API_PPP_PROFILE_FLAG_AUTH_PEER			= 262144;

/* Default values for PPP profile attributes.
 * These are used if an explicit value is not provided by the user.
 */
const L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME			= "default";
const L2TP_API_PPP_PROFILE_DEFAULT_TRACE_FLAGS			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_ASYNCMAP			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_MRU				= 1500;
const L2TP_API_PPP_PROFILE_DEFAULT_MTU				= 1500;
const L2TP_API_PPP_PROFILE_DEFAULT_AUTH_NONE			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_AUTH_PEER			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_PAP		= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_CHAP		= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_MSCHAP		= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_MSCHAPV2		= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_EAP		= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_SYNC_MODE			= L2TP_API_PPP_SYNCMODE_SYNC_ASYNC;
const L2TP_API_PPP_PROFILE_DEFAULT_CHAP_INTERVAL		= 10;
const L2TP_API_PPP_PROFILE_DEFAULT_CHAP_MAX_CHALLENGE		= 10;
const L2TP_API_PPP_PROFILE_DEFAULT_CHAP_RESTART			= 3;
const L2TP_API_PPP_PROFILE_DEFAULT_PAP_MAX_AUTH_REQUESTS	= 10;
const L2TP_API_PPP_PROFILE_DEFAULT_PAP_RESTART_INTERVAL		= 3;
const L2TP_API_PPP_PROFILE_DEFAULT_PAP_TIMEOUT			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_IDLE_TIMEOUT			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_IPCP_MAX_CONFIG_REQUESTS	= 10;
const L2TP_API_PPP_PROFILE_DEFAULT_IPCP_MAX_CONFIG_NAKS		= 10;
const L2TP_API_PPP_PROFILE_DEFAULT_IPCP_MAX_TERMINATE_REQUESTS	= 3;
const L2TP_API_PPP_PROFILE_DEFAULT_IPCP_RETRANSMIT_INTERVAL	= 3;
const L2TP_API_PPP_PROFILE_DEFAULT_LCP_ECHO_FAILURE_COUNT	= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_LCP_ECHO_INTERVAL		= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_LCP_MAX_CONFIG_REQUESTS	= 10;
const L2TP_API_PPP_PROFILE_DEFAULT_LCP_MAX_CONFIG_NAKS		= 10;
const L2TP_API_PPP_PROFILE_DEFAULT_LCP_MAX_TERMINATE_REQUESTS	= 3;
const L2TP_API_PPP_PROFILE_DEFAULT_LCP_RETRANSMIT_INTERVAL	= 3;
const L2TP_API_PPP_PROFILE_DEFAULT_MAX_CONNECT_TIME		= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_MAX_FAILURE_COUNT		= 10;
const L2TP_API_PPP_PROFILE_DEFAULT_LOCAL_IP_ADDR		= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_PEER_IP_ADDR			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_DNS_ADDR_1			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_DNS_ADDR_2			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_WINS_ADDR_1			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_WINS_ADDR_2			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_USE_RADIUS			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_RADIUS_HINT			= "";
const L2TP_API_PPP_PROFILE_DEFAULT_USE_AS_DEFAULT_ROUTE		= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_MULTILINK			= 0;
const L2TP_API_PPP_PROFILE_DEFAULT_PROXY_ARP			= 0;

enum l2tp_api_ppp_sync_mode {
	L2TP_API_PPP_SYNCMODE_SYNC_ASYNC,
	L2TP_API_PPP_SYNCMODE_SYNC,
	L2TP_API_PPP_SYNCMODE_ASYNC
};

struct l2tp_api_ppp_profile_msg_data {
	uint32_t			flags;
	uint32_t			flags2;
	int				result_code;		/* read-only */
	uint32_t			trace_flags;
	uint32_t			trace_flags_mask;
	string				profile_name<>;
	uint32_t			asyncmap;
	uint16_t			mru;
	uint16_t			mtu;
	bool				auth_none;
	bool				auth_peer;
	bool				auth_refuse_pap;
	bool				auth_refuse_chap;
	bool				auth_refuse_mschap;
	bool				auth_refuse_mschapv2;
	bool				auth_refuse_eap;
	enum l2tp_api_ppp_sync_mode	sync_mode;
	int				chap_interval;
	int				chap_max_challenge;
	int				chap_restart;
	int				pap_max_auth_requests;
	int				pap_restart_interval;
	int				pap_timeout;
	int				idle_timeout;
	int				ipcp_max_config_requests;
	int				ipcp_max_config_naks;
	int				ipcp_max_terminate_requests;
	int				ipcp_retransmit_interval;
	int				lcp_echo_failure_count;
	int				lcp_echo_interval;
	int				lcp_max_config_requests;
	int				lcp_max_config_naks;
	int				lcp_max_terminate_requests;
	int				lcp_retransmit_interval;
	int				max_connect_time;
	int				max_failure_count;
	struct l2tp_api_ip_addr		local_ip_addr;
	struct l2tp_api_ip_addr		peer_ip_addr;
	struct l2tp_api_ip_addr		dns_addr_1;
	struct l2tp_api_ip_addr		dns_addr_2;
	struct l2tp_api_ip_addr		wins_addr_1;
	struct l2tp_api_ip_addr		wins_addr_2;
	optstring			ip_pool_name;
	bool				use_radius;
	optstring			radius_hint;
	bool				use_as_default_route;
	bool				multilink;
	optstring			local_name;
	optstring			remote_name;
	bool				proxy_arp;
};

struct l2tp_api_ppp_profile_list_entry {
	string						profile_name<>;
	struct l2tp_api_ppp_profile_list_entry		*next;
};

struct l2tp_api_ppp_profile_list_msg_data {
	int						result;
	int						num_profiles;
	struct l2tp_api_ppp_profile_list_entry		*profiles;
};

struct l2tp_api_ppp_profile_unset_msg_data {
	uint32_t			flags;
	uint32_t			flags2;
	int				result_code;			/* read-only */
	string				profile_name<>;
};

/*****************************************************************************
 * Users:-
 * The PPP username is recorded for each L2TP session. This API allows
 * the manager to obtain the list of all connected users.
 *
 * TYPE: struct l2tp_api_user_list_msg_data
 * USE:  list users
 *****************************************************************************/

struct l2tp_api_user_list_entry {
	string				user_name<>;		/* read-only */
	string				create_time<>;		/* read-only */
	bool				created_by_admin;	/* read-only */
	uint16_t			tunnel_id;		/* read-only */
	uint16_t			session_id;		/* read-only */
	
	struct l2tp_api_user_list_entry *next;
};

struct l2tp_api_user_list_msg_data {
	int				result;
	int				num_users;
	struct l2tp_api_user_list_entry *users;
};

/*****************************************************************************
 * Test control.
 * This is only compiled in for testing but should never be deleted from this
 * interface definition.
 * TYPE: struct l2tp_api_test_msg_data
 * USE:  modify/get test characteristics
 *****************************************************************************/

/* flags */
const L2TP_API_TEST_FLAG_FAKE_RX_DROP			= 1;
const L2TP_API_TEST_FLAG_FAKE_TX_DROP			= 2;
const L2TP_API_TEST_FLAG_FAKE_TRIGGER_TYPE		= 4;
const L2TP_API_TEST_FLAG_CLEAR_FAKE_TRIGGER		= 8;
const L2TP_API_TEST_FLAG_HOLD_TUNNELS			= 16;
const L2TP_API_TEST_FLAG_HOLD_SESSIONS			= 32;
const L2TP_API_TEST_FLAG_NO_RANDOM_IDS			= 64;
const L2TP_API_TEST_FLAG_RESET_IDS			= 128;
const L2TP_API_TEST_FLAG_DO_TRANSPORT_TEST		= 256;
const L2TP_API_TEST_FLAG_TUNNEL_ID			= 512;
const L2TP_API_TEST_FLAG_SESSION_ID			= 1024;
const L2TP_API_TEST_FLAG_DEFAULT_CONFIG			= 2048;
const L2TP_API_TEST_FLAG_SHOW_PROFILE_USAGE		= 4096;

enum l2tp_api_test_trigger_type {
	L2TP_API_TEST_FAKE_TRIGGER_OFF,
	L2TP_API_TEST_FAKE_TRIGGER_ON,
	L2TP_API_TEST_FAKE_TRIGGER_ONCE,
	L2TP_API_TEST_FAKE_TRIGGER_LOW,
	L2TP_API_TEST_FAKE_TRIGGER_MEDIUM,
	L2TP_API_TEST_FAKE_TRIGGER_HIGH
};

struct l2tp_api_test_msg_data {
	uint32_t			flags;
	bool				fake_rx_drop;
	bool				fake_tx_drop;
	bool				fake_trigger_fired;	/* read-only */
	bool				hold_tunnels;
	bool				hold_sessions;
	bool				no_random_ids;
	bool				show_profile_usage;
	int				num_rx_drops;		/* read-only */
	int				num_tx_drops;		/* read-only */
	enum l2tp_api_test_trigger_type	fake_trigger_type;
	uint16_t			tunnel_id;
	uint16_t			session_id;
	int				num_tunnel_id_hash_hits;
	int				num_tunnel_id_hash_misses;
	int				num_tunnel_name_hash_hits;
	int				num_tunnel_name_hash_misses;
	int				num_session_id_hash_hits;
	int				num_session_id_hash_misses;
};

/*****************************************************************************
 * API definition
 *****************************************************************************/

program L2TP_PROG {
	version L2TP_VERSION {
		struct l2tp_api_app_msg_data L2TP_APP_INFO_GET(void) = 1;
		int L2TP_SYSTEM_MODIFY(struct l2tp_api_system_msg_data params) = 2;
		struct l2tp_api_system_msg_data L2TP_SYSTEM_GET(void) = 3;
		int L2TP_PEER_PROFILE_CREATE(struct l2tp_api_peer_profile_msg_data params) = 4;
		int L2TP_PEER_PROFILE_DELETE(string name) = 5;
		int L2TP_PEER_PROFILE_MODIFY(struct l2tp_api_peer_profile_msg_data params) = 6;
		struct l2tp_api_peer_profile_msg_data L2TP_PEER_PROFILE_GET(string name) = 7;
		struct l2tp_api_peer_profile_list_msg_data L2TP_PEER_PROFILE_LIST(void) = 8;
		int L2TP_TUNNEL_CREATE(struct l2tp_api_tunnel_msg_data params) = 9;
		int L2TP_TUNNEL_DELETE(uint16_t tunnel_id, optstring tunnel_name, optstring reason) = 10;
		int L2TP_TUNNEL_MODIFY(struct l2tp_api_tunnel_msg_data params) = 11;
		struct l2tp_api_tunnel_msg_data L2TP_TUNNEL_GET(uint16_t tunnel_id, optstring tunnel_name) = 12;
		struct l2tp_api_tunnel_list_msg_data L2TP_TUNNEL_LIST(void) = 13;
		int L2TP_TUNNEL_PROFILE_CREATE(struct l2tp_api_tunnel_profile_msg_data params) = 14;
		int L2TP_TUNNEL_PROFILE_DELETE(string name) = 15;
		int L2TP_TUNNEL_PROFILE_MODIFY(struct l2tp_api_tunnel_profile_msg_data params) = 16;
		struct l2tp_api_tunnel_profile_msg_data  L2TP_TUNNEL_PROFILE_GET(string name) = 17;
		struct l2tp_api_tunnel_profile_list_msg_data L2TP_TUNNEL_PROFILE_LIST(void) = 18;
		int L2TP_SESSION_CREATE(struct l2tp_api_session_msg_data params) = 19;
		int L2TP_SESSION_DELETE(uint16_t tunnel_id, optstring tunnel_name, 
					uint16_t session_id, optstring session_name, optstring reason) = 20;
		int L2TP_SESSION_MODIFY(struct l2tp_api_session_msg_data params) = 21;
		struct l2tp_api_session_msg_data  L2TP_SESSION_GET(uint16_t tunnel_id, optstring tunnel_name,
								   uint16_t session_id, optstring session_name) = 22;
		struct l2tp_api_session_list_msg_data L2TP_SESSION_LIST(uint16_t tunnel_id, optstring tunnel_name) = 23;
		int L2TP_SESSION_INCALL_IND(struct l2tp_api_session_incall_msg_data params) = 24;
		int L2TP_SESSION_PROFILE_CREATE(struct l2tp_api_session_profile_msg_data params) = 25;
		int L2TP_SESSION_PROFILE_DELETE(string name) = 26;
		int L2TP_SESSION_PROFILE_MODIFY(struct l2tp_api_session_profile_msg_data params) = 27;
		struct l2tp_api_session_profile_msg_data  L2TP_SESSION_PROFILE_GET(string name) = 28;
		struct l2tp_api_session_profile_list_msg_data L2TP_SESSION_PROFILE_LIST(void) = 29;
		int L2TP_PPP_PROFILE_CREATE(struct l2tp_api_ppp_profile_msg_data params) = 30;
		int L2TP_PPP_PROFILE_DELETE(string name) = 31;
		int L2TP_PPP_PROFILE_MODIFY(struct l2tp_api_ppp_profile_msg_data params) = 32;
		struct l2tp_api_ppp_profile_msg_data  L2TP_PPP_PROFILE_GET(string name) = 33;
		struct l2tp_api_ppp_profile_list_msg_data L2TP_PPP_PROFILE_LIST(void) = 34;
		struct l2tp_api_peer_msg_data L2TP_PEER_GET(struct l2tp_api_ip_addr local_addr, struct l2tp_api_ip_addr peer_addr) = 35;
		struct l2tp_api_peer_list_msg_data L2TP_PEER_LIST(void) = 36;
		struct l2tp_api_user_list_msg_data L2TP_USER_LIST(void) = 37;
		int L2TP_PEER_PROFILE_UNSET(struct l2tp_api_peer_profile_unset_msg_data params) = 38;
		int L2TP_TUNNEL_PROFILE_UNSET(struct l2tp_api_tunnel_profile_unset_msg_data params) = 39;
		int L2TP_SESSION_PROFILE_UNSET(struct l2tp_api_session_profile_unset_msg_data params) = 40;
		int L2TP_PPP_PROFILE_UNSET(struct l2tp_api_ppp_profile_unset_msg_data params) = 41;
		int L2TP_TEST_LOG(string text) = 97;
		int L2TP_TEST_MODIFY(struct l2tp_api_test_msg_data params) = 98;
		struct l2tp_api_test_msg_data L2TP_TEST_GET(void) = 99;
	} = 1;			/* version 1 */
} = 300773;			/* official number registered at rpc@sun.com */
