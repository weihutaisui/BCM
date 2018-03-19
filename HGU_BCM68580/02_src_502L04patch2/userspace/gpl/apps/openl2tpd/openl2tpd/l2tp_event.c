/*****************************************************************************
 * Copyright (C) 2008 Katalix Systems Ltd
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
 * Event interface for openl2tpd.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "usl.h"

#include "l2tp_private.h"
#include "l2tp_event.h"


static int l2tp_event_fd = -1;
static uint8_t l2tp_event_msg[OPENL2TP_MSG_MAX_LEN];

/*****************************************************************************
 * Message handler
 *****************************************************************************/

static void l2tp_event_recv(int fd, void *arg)
{
	int len;
	int recv_len;
	struct openl2tp_event_msg *msg;
	struct openl2tp_event_tlv *tlv;
	uint16_t tunnel_id = 0;
	uint16_t session_id = 0;
	uint32_t send_accm = 0;
	uint32_t recv_accm = 0;
	int unit = 0;
	int up = 0;
	char *ifname = NULL;
	char *user_name = NULL;
	int tlv_flags[OPENL2TP_TLV_TYPE_MAX] = { 0, };

	for (;;) {
		recv_len = recv(fd, &l2tp_event_msg[0], sizeof(l2tp_event_msg),
				MSG_NOSIGNAL | MSG_DONTWAIT);
		if (recv_len < 0) {
			if ((errno == EINTR) || (errno == ERESTART)) {
				continue;
			} else {
				if (errno != EAGAIN) {
					L2TP_DEBUG(L2TP_DATA, "%s: recvmsg() returned %d (%d)", __func__, recv_len, errno);
				}
				goto out;
			}
		} else if (recv_len == 0) {
			goto out;
		}

		/* Parse message */
		msg = (void *) &l2tp_event_msg[0];

		if (l2tp_opt_trace_flags & L2TP_API) {
			l2tp_log(LOG_DEBUG, "API: message received (%d bytes): type=%hu len=%hu sig=0x%lx",
				 recv_len, msg->msg_type, msg->msg_len, (unsigned long) msg->msg_signature);
		}

		if (msg->msg_signature != OPENL2TP_MSG_SIGNATURE) {
			L2TP_DEBUG(L2TP_API, "message has bad signature");
			continue;
		}
		if (recv_len < sizeof(*msg)) {
			L2TP_DEBUG(L2TP_API, "message too short");
			continue;
		}
		if (msg->msg_len > (recv_len - sizeof(*msg))) {
			L2TP_DEBUG(L2TP_API, "message has bad length");
			continue;
		}

		for (len = 0; len < msg->msg_len; len += sizeof(*tlv) + ALIGN32(tlv->tlv_len)) {
			tlv = (void *) &msg->msg_data[len];

			L2TP_DEBUG(L2TP_API, "TLV: type=%hu len=%hu", tlv->tlv_type, tlv->tlv_len);

			if ((len + tlv->tlv_len) > msg->msg_len) {
				break;
			}

			switch (tlv->tlv_type) {
			case OPENL2TP_TLV_TYPE_TUNNEL_ID:
				if (tlv->tlv_len != sizeof(struct openl2tp_tlv_tunnel_id)) {
					L2TP_DEBUG(L2TP_API, "bad length");
					continue;
				}
				tunnel_id = ((struct openl2tp_tlv_tunnel_id *) &tlv->tlv_value[0])->tunnel_id;
				break;
			case OPENL2TP_TLV_TYPE_SESSION_ID:
				if (tlv->tlv_len != sizeof(struct openl2tp_tlv_session_id)) {
					L2TP_DEBUG(L2TP_API, "bad length");
					continue;
				}
				session_id = ((struct openl2tp_tlv_session_id *) &tlv->tlv_value[0])->session_id;
				break;
			case OPENL2TP_TLV_TYPE_PPP_ACCM:
				if (tlv->tlv_len != sizeof(struct openl2tp_tlv_ppp_accm)) {
					L2TP_DEBUG(L2TP_API, "bad length");
					continue;
				}
				send_accm = ((struct openl2tp_tlv_ppp_accm *) &tlv->tlv_value[0])->send_accm;
				recv_accm = ((struct openl2tp_tlv_ppp_accm *) &tlv->tlv_value[0])->recv_accm;
				break;
			case OPENL2TP_TLV_TYPE_PPP_UNIT:
				if (tlv->tlv_len != sizeof(struct openl2tp_tlv_ppp_unit)) {
					L2TP_DEBUG(L2TP_API, "bad length");
					continue;
				}
				unit = ((struct openl2tp_tlv_ppp_unit *) &tlv->tlv_value[0])->unit;
				break;
			case OPENL2TP_TLV_TYPE_PPP_IFNAME:
				if (tlv->tlv_len < (strlen((char *) &tlv->tlv_value[0]))) {
					L2TP_DEBUG(L2TP_API, "bad length: %Zd", strlen((char *) &tlv->tlv_value[0]) + 1);
					continue;
				}
				ifname = ((struct openl2tp_tlv_ppp_ifname *) &tlv->tlv_value[0])->ifname;
				break;
			case OPENL2TP_TLV_TYPE_PPP_USER_NAME:
				if (tlv->tlv_len < (strlen((char *) &tlv->tlv_value[0]))) {
					L2TP_DEBUG(L2TP_API, "bad length: %Zd", strlen((char *) &tlv->tlv_value[0]) + 1);
					continue;
				}
				user_name = ((struct openl2tp_tlv_ppp_user_name *) &tlv->tlv_value[0])->user_name;
				break;
			case OPENL2TP_TLV_TYPE_PPP_STATE:
				if (tlv->tlv_len != sizeof(struct openl2tp_tlv_ppp_state)) {
					L2TP_DEBUG(L2TP_API, "bad length");
					continue;
				}
				up = ((struct openl2tp_tlv_ppp_state *) &tlv->tlv_value[0])->up;
				break;
			}
			if (tlv->tlv_type < OPENL2TP_TLV_TYPE_MAX) {
				tlv_flags[tlv->tlv_type] = 1;
			}
		}

		/* Handle the message */
		switch (msg->msg_type) {
		case OPENL2TP_MSG_TYPE_NULL:
			break;
		case OPENL2TP_MSG_TYPE_PPP_UPDOWN_IND:
			if ((tlv_flags[OPENL2TP_TLV_TYPE_TUNNEL_ID]) &&
			    (tlv_flags[OPENL2TP_TLV_TYPE_SESSION_ID]) &&
			    (tlv_flags[OPENL2TP_TLV_TYPE_PPP_UNIT]) &&
			    (tlv_flags[OPENL2TP_TLV_TYPE_PPP_IFNAME]) &&
			    (tlv_flags[OPENL2TP_TLV_TYPE_PPP_STATE])) {
				if (l2tp_opt_trace_flags & L2TP_API) {
					l2tp_log(LOG_INFO, "API: PPP_UPDOWN_IND: tunl %hu/%hu: unit=%d up=%d ifname='%s' user='%s'",
						 tunnel_id, session_id, unit, up, ifname, user_name ? user_name : "");
				}
				l2tp_session_ppp_updown_ind(tunnel_id, session_id, unit, up, ifname, user_name);
			}
			break;
		case OPENL2TP_MSG_TYPE_PPP_ACCM_IND:
			if ((tlv_flags[OPENL2TP_TLV_TYPE_TUNNEL_ID]) &&
			    (tlv_flags[OPENL2TP_TLV_TYPE_SESSION_ID]) &&
			    (tlv_flags[OPENL2TP_TLV_TYPE_PPP_ACCM])) {
				if (l2tp_opt_trace_flags & L2TP_API) {
					l2tp_log(LOG_INFO, "API: PPP_ACCM_IND: tunl %hu/%hu: send_accm=0x%08lx recv_accm=0x%08lx",
						 tunnel_id, session_id, (unsigned long) send_accm, (unsigned long) recv_accm);
				}
				l2tp_session_ppp_accm_ind(tunnel_id, session_id, send_accm, recv_accm);
			}
			break;
		default:
			if (l2tp_opt_trace_flags & L2TP_API) {
				l2tp_log(LOG_ERR, "API: Invalid message received: type=%hu len=%hu",
					 msg->msg_type, msg->msg_len);
			}
			break;
		}
	}

out:
	return;
}

/*****************************************************************************
 * Init and cleanup
 *****************************************************************************/

void l2tp_event_init(void)
{
	int result;
	struct sockaddr_un addr = { 0, };

	(void) unlink(OPENL2TP_EVENT_SOCKET_NAME);

	l2tp_event_fd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (l2tp_event_fd < 0) {
		l2tp_log(LOG_ERR, "unable to open event socket: %s", strerror(errno));
		exit(1);
	}

	result = fcntl(l2tp_event_fd, F_SETFL, O_NONBLOCK);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to configure event socket: %s", strerror(errno));
		exit(1);
	}

	/* Prevent children inheriting our socket */
	fcntl(l2tp_event_fd, F_SETFD, FD_CLOEXEC); 

	addr.sun_family = AF_UNIX;
	strcpy(&addr.sun_path[0], OPENL2TP_EVENT_SOCKET_NAME);
	result = bind(l2tp_event_fd, (struct sockaddr *) &addr, sizeof(addr));
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to bind event socket: %s", strerror(errno));
		exit(1);
	}

	result = usl_fd_add_fd(l2tp_event_fd, l2tp_event_recv, NULL);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to register event message handler");
		exit(1);
	}
}

void l2tp_event_cleanup(void)
{
	if (l2tp_event_fd >= 0) {
		close(l2tp_event_fd);
	}
	(void) unlink(OPENL2TP_EVENT_SOCKET_NAME);
}
