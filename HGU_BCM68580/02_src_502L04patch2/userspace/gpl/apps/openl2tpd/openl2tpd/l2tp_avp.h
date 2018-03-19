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

#ifndef L2TP_AVP_H
#define L2TP_AVP_H

/* We use C bitfields to represent bits in the header. We need to
 * take account of different bit-orders for little and big endian,
 * I use a macro to do that.
 */
#if (__BYTE_ORDER == __BIG_ENDIAN)
#define X(a,b)	a,b
#elif (__BYTE_ORDER == __LITTLE_ENDIAN)
#define X(a,b)	b,a
#else
#error	"Adjust your <endian.h> defines"
#endif	

#define L2TP_AVP_VERSION_RFC2661		0x01

#define L2TP_AVP_TYPE_MESSAGE			0
#define L2TP_AVP_TYPE_RESULT_CODE		1
#define L2TP_AVP_TYPE_PROTOCOL_VERSION		2
#define L2TP_AVP_TYPE_FRAMING_CAP		3
#define L2TP_AVP_TYPE_BEARER_CAP		4
#define L2TP_AVP_TYPE_TIEBREAKER		5
#define L2TP_AVP_TYPE_FIRMWARE_REVISION		6
#define L2TP_AVP_TYPE_HOST_NAME			7
#define L2TP_AVP_TYPE_VENDOR_NAME		8
#define L2TP_AVP_TYPE_TUNNEL_ID			9
#define L2TP_AVP_TYPE_RX_WINDOW_SIZE		10
#define L2TP_AVP_TYPE_CHALLENGE			11
#define L2TP_AVP_TYPE_Q931_CAUSE_CODE		12
#define L2TP_AVP_TYPE_CHALLENGE_RESPONSE	13
#define L2TP_AVP_TYPE_SESSION_ID		14
#define L2TP_AVP_TYPE_CALL_SERIAL_NUMBER	15
#define L2TP_AVP_TYPE_MINIMUM_BPS		16
#define L2TP_AVP_TYPE_MAXIMUM_BPS		17
#define L2TP_AVP_TYPE_BEARER_TYPE		18
#define L2TP_AVP_TYPE_FRAMING_TYPE		19
#define L2TP_AVP_TYPE_PACKET_PROC_DELAY		20   /* Draft only (ignored) */
#define L2TP_AVP_TYPE_CALLED_NUMBER		21
#define L2TP_AVP_TYPE_CALLING_NUMBER		22
#define L2TP_AVP_TYPE_SUB_ADDRESS		23
#define L2TP_AVP_TYPE_CONNECT_SPEED		24
#define L2TP_AVP_TYPE_PHYSICAL_CHANNEL_ID	25
#define L2TP_AVP_TYPE_INITIAL_RCVD_LCP_CONFREQ	26
#define L2TP_AVP_TYPE_LAST_SENT_LCP_CONFREQ	27
#define L2TP_AVP_TYPE_LAST_RCVD_LCP_CONFREQ	28
#define L2TP_AVP_TYPE_PROXY_AUTH_TYPE		29
#define L2TP_AVP_TYPE_PROXY_AUTH_NAME		30
#define L2TP_AVP_TYPE_PROXY_AUTH_CHALLENGE	31
#define L2TP_AVP_TYPE_PROXY_AUTH_ID		32
#define L2TP_AVP_TYPE_PROXY_AUTH_RESPONSE	33
#define L2TP_AVP_TYPE_CALL_ERRORS		34
#define L2TP_AVP_TYPE_ACCM			35
#define L2TP_AVP_TYPE_RANDOM_VECTOR		36
#define L2TP_AVP_TYPE_PRIV_GROUP_ID		37
#define L2TP_AVP_TYPE_RX_CONNECT_SPEED		38
#define L2TP_AVP_TYPE_SEQUENCING_REQUIRED	39
#define L2TP_AVP_TYPE_NUM_AVPS			41

/*****************************************************************************
 * result & error code definitions
 *****************************************************************************/

#define L2TP_AVP_RESULT_STOPCCN_RESERVED	0
#define L2TP_AVP_RESULT_STOPCCN_NORMAL_STOP	1
#define L2TP_AVP_RESULT_STOPCCN_GENERAL_ERROR	2
#define L2TP_AVP_RESULT_STOPCCN_ALREADY_EXISTS	3
#define L2TP_AVP_RESULT_STOPCCN_AUTH_FAILED	4
#define L2TP_AVP_RESULT_STOPCCN_BAD_PROTOCOL	5
#define L2TP_AVP_RESULT_STOPCCN_BEING_SHUTDOWN	6
#define L2TP_AVP_RESULT_STOPCCN_STATE_ERROR	7

#define L2TP_AVP_RESULT_CDN_RESERVED		0
#define L2TP_AVP_RESULT_CDN_LOST_CARRIER	1
#define L2TP_AVP_RESULT_CDN_GENERAL_ERROR	2
#define L2TP_AVP_RESULT_CDN_ADMIN		3
#define L2TP_AVP_RESULT_CDN_NO_RESOURCES	4
#define L2TP_AVP_RESULT_CDN_NOT_AVAILABLE	5
#define L2TP_AVP_RESULT_CDN_INVALID_DEST	6
#define L2TP_AVP_RESULT_CDN_NO_CARRIER		7
#define L2TP_AVP_RESULT_CDN_BUSY_SIGNAL		8
#define L2TP_AVP_RESULT_CDN_NO_DIAL_TONE	9
#define L2TP_AVP_RESULT_CDN_NO_ANSWER		10
#define L2TP_AVP_RESULT_CDN_INVALID_XPRT	11

#define L2TP_AVP_ERROR_NO_ERROR			0
#define L2TP_AVP_ERROR_NO_TUNNEL_YET		1
#define L2TP_AVP_ERROR_BAD_LENGTH		2
#define L2TP_AVP_ERROR_BAD_VALUE		3
#define L2TP_AVP_ERROR_NO_RESOURCE		4
#define L2TP_AVP_ERROR_BAD_SESSION_ID		5
#define L2TP_AVP_ERROR_VENDOR_ERROR		6
#define L2TP_AVP_ERROR_TRY_ANOTHER		7
#define L2TP_AVP_ERROR_MBIT_SHUTDOWN		8

/*****************************************************************************
 * AVP structures
 *
 * Each AVP is encoded as:
 *
 *  0			1		    2			3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |M|H| rsvd  |      Length	   |	       Vendor ID	   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |	     Attribute Type	   |	    Attribute Value...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *		       [until Length is reached]...		   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *****************************************************************************/

#define L2TP_AVP_HEADER_LEN			6
#define L2TP_AVP_MAX_LEN			1024

struct l2tp_avp_hdr {
	uint16_t	flag_len;
#define L2TP_AVP_HDR_MBIT			0x8000
#define L2TP_AVP_HDR_HBIT			0x4000
#define L2TP_AVP_HDR_LEN(_len)			((_len) & 0x03ff)
	uint16_t	vendor_id;
	uint16_t	type;
	uint8_t		value[0];		/* variable length */
} __attribute__((packed));

struct l2tp_avp_message_type {
	uint16_t	type;
#define L2TP_AVP_MSG_ILLEGAL			0
#define L2TP_AVP_MSG_SCCRQ			1
#define L2TP_AVP_MSG_SCCRP			2
#define L2TP_AVP_MSG_SCCCN			3
#define L2TP_AVP_MSG_STOPCCN			4
#define L2TP_AVP_MSG_RESERVED1			5
#define L2TP_AVP_MSG_HELLO			6
#define L2TP_AVP_MSG_OCRQ			7
#define L2TP_AVP_MSG_OCRP			8
#define L2TP_AVP_MSG_OCCN			9
#define L2TP_AVP_MSG_ICRQ			10
#define L2TP_AVP_MSG_ICRP			11
#define L2TP_AVP_MSG_ICCN			12
#define L2TP_AVP_MSG_RESERVED2			13
#define L2TP_AVP_MSG_CDN			14
#define L2TP_AVP_MSG_WEN			15
#define L2TP_AVP_MSG_SLI			16
#define L2TP_AVP_MSG_COUNT			17
} __attribute__((packed));

struct l2tp_avp_random_vector {
	uint8_t		value[0];
} __attribute__((packed));

struct l2tp_avp_result_code {
	uint16_t	result_code;
	uint16_t	error_code;
	char		error_message[0];
} __attribute__((packed));

struct l2tp_avp_protocol_version {
	uint8_t		ver;
	uint8_t		rev;
} __attribute__((packed));

struct l2tp_avp_framing_cap {
	union {
		struct {
			uint32_t	X(X(rsvd:30, async:1), sync:1);
		} bits;
		uint32_t		value;
	};
} __attribute__((packed));

struct l2tp_avp_bearer_cap {
	union {
		struct {
			uint32_t	X(X(rsvd:30, analog:1), digital:1);
		} bits;
		uint32_t		value;
	};
} __attribute__((packed));

struct l2tp_avp_tiebreaker {
	uint8_t		value[8];
} __attribute__((packed));

struct l2tp_avp_firmware_revision {
	uint16_t	value;
} __attribute__((packed));

struct l2tp_avp_host_name {
	char		string[0];
} __attribute__((packed));

struct l2tp_avp_vendor_name {
	char		string[0];
} __attribute__((packed));

struct l2tp_avp_tunnel_id {
	uint16_t	value;
} __attribute__((packed));

struct l2tp_avp_rx_window_size {
	uint16_t	value;
} __attribute__((packed));

struct l2tp_avp_challenge {
	uint8_t		value[0];
} __attribute__((packed));

struct l2tp_avp_challenge_response {
	uint8_t		value[16];
} __attribute__((packed));

struct l2tp_avp_q931_cause_code {
	uint16_t	cause_code;
	uint16_t	cause_msg;
	char		advisory_msg[0];
} __attribute__((packed));

struct l2tp_avp_session_id {
	uint16_t	value;
} __attribute__((packed));

struct l2tp_avp_call_serial_number {
	uint32_t	value;
} __attribute__((packed));

struct l2tp_avp_minimum_bps {
	uint32_t	value;
} __attribute__((packed));

struct l2tp_avp_maximum_bps {
	uint32_t	value;
} __attribute__((packed));

struct l2tp_avp_bearer_type {
	union {
		struct {
			uint32_t	X(X(rsvd:30, analog:1), digital:1);
		} bits;
		uint32_t		value;
	};
} __attribute__((packed));

struct l2tp_avp_framing_type {
	union {
		struct {
			uint32_t	X(X(rsvd:30, async:1), sync:1);
		} bits;
		uint32_t		value;
	};
} __attribute__((packed));

struct l2tp_avp_called_number {
	char		string[0];
} __attribute__((packed));

struct l2tp_avp_calling_number {
	char		string[0];
} __attribute__((packed));

struct l2tp_avp_sub_address {
	char		string[0];
} __attribute__((packed));

struct l2tp_avp_connect_speed {
	uint32_t	value;
} __attribute__((packed));

struct l2tp_avp_rx_connect_speed {
	uint32_t	value;
} __attribute__((packed));

struct l2tp_avp_physical_channel_id {
	uint32_t	value;
} __attribute__((packed));

struct l2tp_avp_priv_group_id {
	char		string[0];
} __attribute__((packed));

struct l2tp_avp_sequencing_required {
	char		dummy[0];		/* actually has no data */
} __attribute__((packed));

struct l2tp_avp_initial_rcvd_lcp_confreq {
	uint8_t		value[0];
} __attribute__((packed));

struct l2tp_avp_last_sent_lcp_confreq {
	uint8_t		value[0];
} __attribute__((packed));

struct l2tp_avp_last_rcvd_lcp_confreq {
	uint8_t		value[0];
} __attribute__((packed));

struct l2tp_avp_proxy_auth_type {
	uint16_t	value;
#define L2TP_AVP_PROXY_AUTH_RESERVED	0
#define L2TP_AVP_PROXY_AUTH_TEXT	1
#define L2TP_AVP_PROXY_AUTH_PPP_CHAP	2
#define L2TP_AVP_PROXY_AUTH_PPP_PAP	3
#define L2TP_AVP_PROXY_AUTH_NONE	4
#define L2TP_AVP_PROXY_AUTH_MS_CHAPV1	5
} __attribute__((packed));

struct l2tp_avp_proxy_auth_name {
	char		string[0];
} __attribute__((packed));

struct l2tp_avp_proxy_auth_challenge {
	uint8_t		value[0];
} __attribute__((packed));

struct l2tp_avp_proxy_auth_id {
	uint8_t		reserved;
	uint8_t		id;
} __attribute__((packed));

struct l2tp_avp_proxy_auth_response {
	uint8_t		value[0];
} __attribute__((packed));

struct l2tp_avp_call_errors {
	uint16_t	reserved;
	uint32_t	crc_errors;
	uint32_t	framing_errors;
	uint32_t	hardware_overruns;
	uint32_t	buffer_overruns;
	uint32_t	timeout_errors;
	uint32_t	alignment_errors;
} __attribute__((packed));

struct l2tp_avp_accm {
	uint16_t	reserved;
	uint32_t	send_accm;
	uint32_t	recv_accm;
} __attribute__((packed));

union l2tp_avp_data {
	struct l2tp_avp_message_type		message_type;
	struct l2tp_avp_random_vector		random_vector;
	struct l2tp_avp_result_code		result_code;
	struct l2tp_avp_protocol_version	protocol_version;
	struct l2tp_avp_framing_cap		framing_cap;
	struct l2tp_avp_bearer_cap		bearer_cap;
	struct l2tp_avp_tiebreaker		tiebreaker;
	struct l2tp_avp_firmware_revision	firmware_revision;
	struct l2tp_avp_host_name		host_name;
	struct l2tp_avp_vendor_name		vendor_name;
	struct l2tp_avp_tunnel_id		tunnel_id;
	struct l2tp_avp_rx_window_size		rx_window_size;
	struct l2tp_avp_challenge		challenge;
	struct l2tp_avp_challenge_response	challenge_response;
	struct l2tp_avp_q931_cause_code		q931_cause_code;
	struct l2tp_avp_session_id		session_id;
	struct l2tp_avp_call_serial_number	call_serial_number;
	struct l2tp_avp_minimum_bps		minimum_bps;
	struct l2tp_avp_maximum_bps		maximum_bps;
	struct l2tp_avp_bearer_type		bearer_type;
	struct l2tp_avp_framing_type		framing_type;
	struct l2tp_avp_called_number		called_number;
	struct l2tp_avp_calling_number		calling_number;
	struct l2tp_avp_sub_address		sub_address;
	struct l2tp_avp_connect_speed		connect_speed;
	struct l2tp_avp_rx_connect_speed	rx_connect_speed;
	struct l2tp_avp_physical_channel_id	physical_channel_id;
	struct l2tp_avp_priv_group_id		priv_group_id;
	struct l2tp_avp_sequencing_required	sequencing_required;
	struct l2tp_avp_initial_rcvd_lcp_confreq	initial_rcvd_lcp_confreq;
	struct l2tp_avp_last_sent_lcp_confreq	last_sent_lcp_confreq;
	struct l2tp_avp_last_rcvd_lcp_confreq	last_rcvd_lcp_confreq;
	struct l2tp_avp_proxy_auth_type		proxy_auth_type;
	struct l2tp_avp_proxy_auth_name		proxy_auth_name;
	struct l2tp_avp_proxy_auth_challenge	proxy_auth_challenge;
	struct l2tp_avp_proxy_auth_id		proxy_auth_id;
	struct l2tp_avp_proxy_auth_response	proxy_auth_response;
	struct l2tp_avp_call_errors		call_errors;
	struct l2tp_avp_accm			accm;
};

#undef X

/* This structure is used internally to pass AVPs. 
 * If passed as an array, it is indexed by AVP type.
 */
struct l2tp_avp_desc {
	union l2tp_avp_data	*value;
	size_t			value_len;
};

extern struct l2tp_avp_firmware_revision 	l2tp_avp_my_firmware_revision;
extern struct l2tp_avp_host_name 		*l2tp_avp_my_host_name;
extern int			 		l2tp_avp_my_host_name_len;
extern struct l2tp_avp_vendor_name 		*l2tp_avp_my_vendor_name;
extern int			 		l2tp_avp_my_vendor_name_len;
extern struct l2tp_avp_protocol_version 	l2tp_avp_my_protocol_version;

#endif
