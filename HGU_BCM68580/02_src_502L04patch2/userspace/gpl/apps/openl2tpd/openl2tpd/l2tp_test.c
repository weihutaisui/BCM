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
 * Test code
 */
#include "usl.h"
#include "l2tp_private.h"
#include "l2tp_rpc.h"

#ifdef L2TP_TEST

extern int l2tp_xprt_test(uint16_t tunnel_id);


struct l2tp_test_data {
	struct {
		int			fake_rx_drop:1;
		int			fake_tx_drop:1;
		int			hold_tunnels:1;
		int			hold_sessions:1;
		int			no_random_ids:1;
		int			show_profile_usage:1;
	} flags;
	enum l2tp_api_test_trigger_type	fake_trigger_type;
	int				fake_trigger_fired;
	int				num_rx_drops;
	int				num_tx_drops;
	int 				alloc_tunnel_id;
	int				alloc_session_id;
	int 				tunnel_id;
	int				session_id;
	int				num_tunnel_id_hash_hits;
	int				num_tunnel_id_hash_misses;
	int				num_tunnel_name_hash_hits;
	int				num_tunnel_name_hash_misses;
	int				num_session_id_hash_hits;
	int				num_session_id_hash_misses;
};

static struct l2tp_test_data l2tp_test_data = { { 0, } };


static int l2tp_test_is_faked_this_time(void)
{
	int result = FALSE;
	uint32_t rand_val;
	static int contiguous_count = 0;

	switch (l2tp_test_data.fake_trigger_type) {
	case L2TP_API_TEST_FAKE_TRIGGER_OFF:
		break;
	case L2TP_API_TEST_FAKE_TRIGGER_ON:
		result = TRUE;
		break;
	case L2TP_API_TEST_FAKE_TRIGGER_ONCE:
		result = l2tp_test_data.fake_trigger_fired ? FALSE : TRUE;
		break;
	case L2TP_API_TEST_FAKE_TRIGGER_LOW: /* 1% */
		l2tp_make_random_vector(&rand_val, sizeof(rand_val));
		if (rand_val < 0x28f5c28) {
			result = TRUE;
		}
		break;
	case L2TP_API_TEST_FAKE_TRIGGER_MEDIUM:	/* 5% */
		l2tp_make_random_vector(&rand_val, sizeof(rand_val));
		if (rand_val < 0xccccccc) {
			result = TRUE;
		}
		break;
	case L2TP_API_TEST_FAKE_TRIGGER_HIGH: /* 20% */
		l2tp_make_random_vector(&rand_val, sizeof(rand_val));
		if (rand_val < 0x33333333) {
			result = TRUE;
		}
		break;
	}

	if (result) {
		l2tp_test_data.fake_trigger_fired = TRUE;
		contiguous_count++;

		/* Max 4 contiguous errors */
		if (l2tp_test_data.fake_trigger_type != L2TP_API_TEST_FAKE_TRIGGER_ON) {
			if (contiguous_count > 4) {
				result = FALSE;
				contiguous_count = 0;
			}
		}
	} else {
		contiguous_count = 0;
	}

	return result;
}

int l2tp_test_is_fake_rx_drop(uint16_t tunnel_id, uint16_t session_id)
{
	/* If a tunnel_id/session_id have been specified for the test, check for match */
	if (((l2tp_test_data.tunnel_id != 0) && (l2tp_test_data.tunnel_id != tunnel_id)) ||
	    ((l2tp_test_data.session_id != 0) && (l2tp_test_data.session_id != session_id))) {
		return FALSE;
	}
	if (l2tp_test_data.flags.fake_rx_drop && l2tp_test_is_faked_this_time()) {
		l2tp_test_data.num_rx_drops++;
		return TRUE;
	}
	return FALSE;
}

int l2tp_test_is_fake_tx_drop(uint16_t tunnel_id, uint16_t session_id)
{
	if (l2tp_test_data.flags.fake_tx_drop && l2tp_test_is_faked_this_time()) {
		l2tp_test_data.num_tx_drops++;
		return TRUE;
	}
	return FALSE;
}

int l2tp_test_is_hold_tunnels(void)
{
	if (l2tp_test_data.flags.hold_tunnels) {
		return TRUE;
	}
	return FALSE;
}

int l2tp_test_is_hold_sessions(void)
{
	if (l2tp_test_data.flags.hold_sessions) {
		return TRUE;
	}
	return FALSE;
}

int l2tp_test_is_no_random_ids(void)
{
	if (l2tp_test_data.flags.no_random_ids) {
		return TRUE;
	}
	return FALSE;
}

int l2tp_test_is_show_profile_usage(void)
{
	if (l2tp_test_data.flags.show_profile_usage) {
		return TRUE;
	}
	return FALSE;
}

uint16_t l2tp_test_alloc_tunnel_id(void)
{
	return ++l2tp_test_data.alloc_tunnel_id;
}

uint16_t l2tp_test_alloc_session_id(void)
{
	return ++l2tp_test_data.alloc_session_id;
}

void l2tp_test_tunnel_id_hash_inc_stats(int hit)
{
	if (hit) {
		l2tp_test_data.num_tunnel_id_hash_hits++;
	} else {
		l2tp_test_data.num_tunnel_id_hash_misses++;
	}
}

void l2tp_test_tunnel_name_hash_inc_stats(int hit)
{
	if (hit) {
		l2tp_test_data.num_tunnel_name_hash_hits++;
	} else {
		l2tp_test_data.num_tunnel_name_hash_misses++;
	}
}

void l2tp_test_session_id_hash_inc_stats(int hit)
{
	if (hit) {
		l2tp_test_data.num_session_id_hash_hits++;
	} else {
		l2tp_test_data.num_session_id_hash_misses++;
	}
}

#endif /* L2TP_TEST */

bool_t l2tp_test_modify_1_svc(l2tp_api_test_msg_data msg, int *result, struct svc_req *req)
{
#ifdef L2TP_TEST
	*result = 0;

	if (msg.flags & L2TP_API_TEST_FLAG_DEFAULT_CONFIG) {
		l2tp_restore_default_config();
	}
	if (msg.flags & L2TP_API_TEST_FLAG_FAKE_RX_DROP) {
		l2tp_test_data.flags.fake_rx_drop = msg.fake_rx_drop ? -1 : 0;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_FAKE_TX_DROP) {
		l2tp_test_data.flags.fake_tx_drop = msg.fake_tx_drop ? -1 : 0;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_FAKE_TRIGGER_TYPE) {
		l2tp_test_data.fake_trigger_type = msg.fake_trigger_type;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_CLEAR_FAKE_TRIGGER) {
		l2tp_test_data.fake_trigger_fired = FALSE;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_HOLD_TUNNELS) {
		l2tp_test_data.flags.hold_tunnels = msg.hold_tunnels ? -1 : 0;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_HOLD_SESSIONS) {
		l2tp_test_data.flags.hold_sessions = msg.hold_sessions ? -1 : 0;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_NO_RANDOM_IDS) {
		l2tp_test_data.flags.no_random_ids = msg.no_random_ids ? -1 : 0;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_RESET_IDS) {
		l2tp_test_data.alloc_tunnel_id = 0;
		l2tp_test_data.alloc_session_id = 0;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_SHOW_PROFILE_USAGE) {
		l2tp_test_data.flags.show_profile_usage = msg.show_profile_usage ? -1 : 0;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_DO_TRANSPORT_TEST) {
		if (msg.flags & L2TP_API_TEST_FLAG_TUNNEL_ID) {
			*result = l2tp_xprt_test(msg.tunnel_id);
		} else {
			*result = -EINVAL;
		}
		goto out;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_TUNNEL_ID) {
		l2tp_test_data.tunnel_id = msg.tunnel_id;
	}
	if (msg.flags & L2TP_API_TEST_FLAG_SESSION_ID) {
		l2tp_test_data.session_id = msg.session_id;
	}
	
out:
	return TRUE;
#else /* L2TP_TEST */
	*result = -EOPNOTSUPP;
	return FALSE;
#endif /* L2TP_TEST */
}

bool_t l2tp_test_get_1_svc(l2tp_api_test_msg_data *result, struct svc_req *req)
{
#ifdef L2TP_TEST
	result->fake_rx_drop = l2tp_test_data.flags.fake_rx_drop;
	result->fake_tx_drop = l2tp_test_data.flags.fake_tx_drop;
	result->fake_trigger_fired = l2tp_test_data.fake_trigger_fired;
	result->fake_trigger_type = l2tp_test_data.fake_trigger_type;
	result->num_rx_drops = l2tp_test_data.num_rx_drops;
	result->num_tx_drops = l2tp_test_data.num_tx_drops;
	result->hold_tunnels = l2tp_test_data.flags.hold_tunnels;
	result->hold_sessions = l2tp_test_data.flags.hold_sessions;
	result->no_random_ids = l2tp_test_data.flags.no_random_ids;
	result->show_profile_usage = l2tp_test_data.flags.show_profile_usage;
	result->tunnel_id = l2tp_test_data.tunnel_id;
	result->session_id = l2tp_test_data.session_id;
	result->num_tunnel_id_hash_hits = l2tp_test_data.num_tunnel_id_hash_hits;
	result->num_tunnel_id_hash_misses = l2tp_test_data.num_tunnel_id_hash_misses;
	result->num_tunnel_name_hash_hits = l2tp_test_data.num_tunnel_name_hash_hits;
	result->num_tunnel_name_hash_misses = l2tp_test_data.num_tunnel_name_hash_misses;
	result->num_session_id_hash_hits = l2tp_test_data.num_session_id_hash_hits;
	result->num_session_id_hash_misses = l2tp_test_data.num_session_id_hash_misses;
	result->flags = 0;

	return TRUE;
#else /* L2TP_TEST */
	return FALSE;
#endif /* L2TP_TEST */
}

bool_t l2tp_test_log_1_svc(char *message, int *result, struct svc_req *req)
{
#ifdef L2TP_TEST
	l2tp_log(LOG_INFO, "APP: %s", message);
	*result = 0;
	return TRUE;
#else /* L2TP_TEST */
	return FALSE;
#endif /* L2TP_TEST */
}

