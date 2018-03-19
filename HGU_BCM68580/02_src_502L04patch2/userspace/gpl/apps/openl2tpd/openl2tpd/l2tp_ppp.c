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

#include "usl.h"
#include "l2tp_private.h"

struct l2tp_ppp_profile {
	struct usl_list_head		list;
	char				*profile_name;
	int				trace_flags;
	uint32_t			asyncmap;
	uint16_t			mru;
	uint16_t			mtu;
	l2tp_api_ppp_sync_mode		sync_mode;
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
	uint32_t			local_ip_addr;
	uint32_t			peer_ip_addr;
	uint32_t			dns_addr_1;
	uint32_t			dns_addr_2;
	uint32_t			wins_addr_1;
	uint32_t			wins_addr_2;
	uint32_t			flags;
	uint32_t			flags2;
	char				*ip_pool_name;
	int				use_radius;
	char				*radius_hint;
	char				*local_name;
	char				*remote_name;
	int				use_as_default_route;
	int				multilink:1;
	int				proxy_arp:1;
	int				auth_none:1;
	int				auth_peer:1;
	int				auth_refuse_pap:1;
	int				auth_refuse_chap:1;
	int				auth_refuse_mschap:1;
	int				auth_refuse_mschapv2:1;
	int				auth_refuse_eap:1;
};

static struct l2tp_ppp_profile *l2tp_ppp_defaults;

static USL_LIST_HEAD(l2tp_ppp_profile_list);

static struct l2tp_ppp_profile *l2tp_ppp_profile_find(const char *name)
{
	struct usl_list_head *walk = l2tp_ppp_profile_list.next;
	struct l2tp_ppp_profile *profile;

	while (walk != &l2tp_ppp_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_ppp_profile, list);
		if (strcmp(&profile->profile_name[0], name) == 0) {
			return profile;
		}
		walk = walk->next;
	}

	return NULL;
}

/*****************************************************************************
 * Management API
 *****************************************************************************/

/* Called by create and modify API functions. Create modifies default values.
 */
static int l2tp_ppp_profile_modify(l2tp_api_ppp_profile_msg_data *msg, struct l2tp_ppp_profile *profile)
{
	int result = 0;

	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS) {
		if (msg->trace_flags_mask == 0) {
			msg->trace_flags_mask = 0xffffffff;
		}
		profile->trace_flags &= ~(msg->trace_flags_mask);
		profile->trace_flags |= (msg->trace_flags & msg->trace_flags_mask);
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_ASYNCMAP) {
		profile->asyncmap = msg->asyncmap;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_MRU) {
		profile->mru = msg->mru;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_MTU) {
		profile->mtu = msg->mtu;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_USE_RADIUS) {
		profile->use_radius = msg->use_radius;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_RADIUS_HINT) {
		L2TP_SET_OPTSTRING_VAR(profile, radius_hint);
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_NONE) {
		profile->auth_none = msg->auth_none ? -1 : 0;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_PEER) {
		profile->auth_peer = msg->auth_peer ? -1 : 0;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_PAP) {
		profile->auth_refuse_pap = msg->auth_refuse_pap ? -1 : 0;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_CHAP) {
		profile->auth_refuse_chap = msg->auth_refuse_chap ? -1 : 0;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAP) {
		profile->auth_refuse_mschap = msg->auth_refuse_mschap ? -1 : 0;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAPV2) {
		profile->auth_refuse_mschapv2 = msg->auth_refuse_mschapv2 ? -1 : 0;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_EAP) {
		profile->auth_refuse_eap = msg->auth_refuse_eap ? -1 : 0;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_SYNC_MODE) {
		profile->sync_mode = msg->sync_mode;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_CHAP_INTERVAL) {
		profile->chap_interval = msg->chap_interval;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_CHAP_MAX_CHALLENGE) {
		profile->chap_max_challenge = msg->chap_max_challenge;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_CHAP_RESTART) {
		profile->chap_restart = msg->chap_restart;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_PAP_MAX_AUTH_REQUESTS) {
		profile->pap_max_auth_requests = msg->pap_max_auth_requests;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_PAP_RESTART_INTERVAL) {
		profile->pap_restart_interval = msg->pap_restart_interval;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_PAP_TIMEOUT) {
		profile->pap_timeout = msg->pap_timeout;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_IDLE_TIMEOUT) {
		profile->idle_timeout = msg->idle_timeout;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_REQUESTS) {
		profile->ipcp_max_config_requests = msg->ipcp_max_config_requests;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_NAKS) {
		profile->ipcp_max_config_naks = msg->ipcp_max_config_naks;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_TERMINATE_REQUESTS) {
		profile->ipcp_max_terminate_requests = msg->ipcp_max_terminate_requests;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_RETRANSMIT_INTERVAL) {
		profile->ipcp_retransmit_interval = msg->ipcp_retransmit_interval;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_FAILURE_COUNT) {
		profile->lcp_echo_failure_count = msg->lcp_echo_failure_count;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_INTERVAL) {
		profile->lcp_echo_interval = msg->lcp_echo_interval;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_REQUESTS) {
		profile->lcp_max_config_requests = msg->lcp_max_config_requests;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_NAKS) {
		profile->lcp_max_config_naks = msg->lcp_max_config_naks;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_TERMINATE_REQUESTS) {
		profile->lcp_max_terminate_requests = msg->lcp_max_terminate_requests;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_LCP_RETRANSMIT_INTERVAL) {
		profile->lcp_retransmit_interval = msg->lcp_retransmit_interval;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_MAX_CONNECT_TIME) {
		profile->max_connect_time = msg->max_connect_time;
	}
	if (msg->flags & L2TP_API_PPP_PROFILE_FLAG_MAX_FAILURE_COUNT) {
		profile->max_failure_count = msg->max_failure_count;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_LOCAL_IP_ADDR) {
		profile->local_ip_addr = msg->local_ip_addr.s_addr;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_PEER_IP_ADDR) {
		profile->peer_ip_addr = msg->peer_ip_addr.s_addr;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_1) {
		profile->dns_addr_1 = msg->dns_addr_1.s_addr;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_2) {
		profile->dns_addr_2 = msg->dns_addr_2.s_addr;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_1) {
		profile->wins_addr_1 = msg->wins_addr_1.s_addr;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_2) {
		profile->wins_addr_2 = msg->wins_addr_2.s_addr;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_IP_POOL_NAME) {
		L2TP_SET_OPTSTRING_VAR(profile, ip_pool_name);
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_USE_AS_DEFAULT_ROUTE) {
		profile->use_as_default_route = msg->use_as_default_route;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_MULTILINK) {
		profile->multilink = msg->multilink;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_PROXY_ARP) {
		profile->proxy_arp = msg->proxy_arp;
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_LOCAL_NAME) {
		L2TP_SET_OPTSTRING_VAR(profile, local_name);
	}
	if (msg->flags2 & L2TP_API_PPP_PROFILE_FLAG_REMOTE_NAME) {
		L2TP_SET_OPTSTRING_VAR(profile, remote_name);
	}

out:
	return result;
}

bool_t l2tp_ppp_profile_create_1_svc(l2tp_api_ppp_profile_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_ppp_profile *profile;
	char *name;

	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, msg.profile_name);

	if ((msg.profile_name == NULL) || strlen(msg.profile_name) == 0) {
		*result = -L2TP_ERR_PROFILE_NAME_MISSING;
		goto out;
	}
	name = msg.profile_name;

	profile = l2tp_ppp_profile_find(name);
	if (profile != NULL) {
		*result = -L2TP_ERR_PROFILE_ALREADY_EXISTS;
		goto out;
	}
	
	profile = calloc(1, sizeof(struct l2tp_ppp_profile));
	if (profile == NULL) {
		goto nomem1;
	}
	profile->profile_name = strdup(name);
	if (profile->profile_name == NULL) {
		goto nomem2;
	}

	/* Fill with default values from preconfigured defaults */
	profile->trace_flags = l2tp_ppp_defaults->trace_flags;
	profile->asyncmap = l2tp_ppp_defaults->asyncmap;
	profile->mru = l2tp_ppp_defaults->mru;
	profile->mtu = l2tp_ppp_defaults->mtu;
	profile->auth_none = l2tp_ppp_defaults->auth_none;
	profile->auth_peer = l2tp_ppp_defaults->auth_peer;
	profile->auth_refuse_pap = l2tp_ppp_defaults->auth_refuse_pap;
	profile->auth_refuse_chap = l2tp_ppp_defaults->auth_refuse_chap;
	profile->auth_refuse_mschap = l2tp_ppp_defaults->auth_refuse_mschap;
	profile->auth_refuse_mschapv2 = l2tp_ppp_defaults->auth_refuse_mschapv2;
	profile->auth_refuse_eap = l2tp_ppp_defaults->auth_refuse_eap;
	profile->sync_mode = l2tp_ppp_defaults->sync_mode;
	profile->chap_interval = l2tp_ppp_defaults->chap_interval;
	profile->chap_max_challenge = l2tp_ppp_defaults->chap_max_challenge;
	profile->chap_restart = l2tp_ppp_defaults->chap_restart;
	profile->pap_max_auth_requests = l2tp_ppp_defaults->pap_max_auth_requests;
	profile->pap_restart_interval = l2tp_ppp_defaults->pap_restart_interval;
	profile->pap_timeout = l2tp_ppp_defaults->pap_timeout;
	profile->idle_timeout = l2tp_ppp_defaults->idle_timeout;
	profile->ipcp_max_config_requests = l2tp_ppp_defaults->ipcp_max_config_requests;
	profile->ipcp_max_config_naks = l2tp_ppp_defaults->ipcp_max_config_naks;
	profile->ipcp_max_terminate_requests = l2tp_ppp_defaults->ipcp_max_terminate_requests;
	profile->ipcp_retransmit_interval = l2tp_ppp_defaults->ipcp_retransmit_interval;
	profile->lcp_echo_failure_count = l2tp_ppp_defaults->lcp_echo_failure_count;
	profile->lcp_echo_interval = l2tp_ppp_defaults->lcp_echo_interval;
	profile->lcp_max_config_requests = l2tp_ppp_defaults->lcp_max_config_requests;
	profile->lcp_max_config_naks = l2tp_ppp_defaults->lcp_max_config_naks;
	profile->lcp_max_terminate_requests = l2tp_ppp_defaults->lcp_max_terminate_requests;
	profile->lcp_retransmit_interval = l2tp_ppp_defaults->lcp_retransmit_interval;
	profile->max_connect_time = l2tp_ppp_defaults->max_connect_time;
	profile->max_failure_count = l2tp_ppp_defaults->max_failure_count;
	profile->local_ip_addr = l2tp_ppp_defaults->local_ip_addr;
	profile->peer_ip_addr = l2tp_ppp_defaults->peer_ip_addr;
	profile->dns_addr_1 = l2tp_ppp_defaults->dns_addr_1;
	profile->dns_addr_2 = l2tp_ppp_defaults->dns_addr_2;
	profile->wins_addr_1 = l2tp_ppp_defaults->wins_addr_1;
	profile->wins_addr_2 = l2tp_ppp_defaults->wins_addr_2;
	if (l2tp_ppp_defaults->ip_pool_name != NULL) {
		profile->ip_pool_name = strdup(l2tp_ppp_defaults->ip_pool_name);
		if (profile->ip_pool_name == NULL) {
			*result = -ENOMEM;
			goto err;
		}
	}
	profile->use_radius = l2tp_ppp_defaults->use_radius;
	if (l2tp_ppp_defaults->radius_hint != NULL) {
		profile->radius_hint = strdup(l2tp_ppp_defaults->radius_hint);
		if (profile->radius_hint == NULL) {
			*result = -ENOMEM;
			goto err;
		}
	}
	profile->use_as_default_route = l2tp_ppp_defaults->use_as_default_route;
	profile->multilink = l2tp_ppp_defaults->multilink;
	profile->proxy_arp = l2tp_ppp_defaults->proxy_arp;

	if (l2tp_ppp_defaults->local_name != NULL) {
		profile->local_name = strdup(l2tp_ppp_defaults->local_name);
		if (profile->local_name == NULL) {
			*result = -ENOMEM;
			goto err;
		}
	}
	if (l2tp_ppp_defaults->remote_name != NULL) {
		profile->remote_name = strdup(l2tp_ppp_defaults->remote_name);
		if (profile->remote_name == NULL) {
			*result = -ENOMEM;
			goto err;
		}
	}

	/* Override defaults by user-supplied params */
	*result = l2tp_ppp_profile_modify(&msg, profile);

	if (*result < 0) {
		goto err;
	}

	/* Remember all non-default parameters */
	profile->flags |= msg.flags;
	profile->flags2 |= msg.flags2;

	L2TP_DEBUG(L2TP_API, "%s: flags=%x/%x", __func__, profile->flags, profile->flags2);

	USL_LIST_HEAD_INIT(&profile->list);
	usl_list_add(&profile->list, &l2tp_ppp_profile_list);

	L2TP_DEBUG(L2TP_FUNC, "FUNC: ppp profile %s created", profile->profile_name);

	/* Give plugins visibility of ppp profile created */
	if (l2tp_profile_created_hook != NULL) {
		(*l2tp_profile_created_hook)(L2TP_PROFILE_TYPE_PPP, profile->profile_name);
	}

out:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);

	return TRUE;
nomem2:
	free(profile);
nomem1:
	*result = -ENOMEM;
	goto out;
err:
	free(profile);
	goto out;
}

bool_t l2tp_ppp_profile_delete_1_svc(char *name, int *result, struct svc_req *req)
{
	struct l2tp_ppp_profile *profile;

	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, name);

	if ((name == NULL) || strlen(name) == 0) {
		*result = -L2TP_ERR_PROFILE_NAME_MISSING;
		goto out;
	}

	/* Prevent deletion of default profile */
	if (strcmp(name, L2TP_API_PEER_PROFILE_DEFAULT_PROFILE_NAME) == 0) {
		*result = -L2TP_ERR_PROFILE_NAME_ILLEGAL;
		goto out;
	}

	profile = l2tp_ppp_profile_find(name);
	if (profile == NULL) {
		*result = -L2TP_ERR_PPP_PROFILE_NOT_FOUND;
		goto out;
	}

	L2TP_DEBUG(L2TP_FUNC, "FUNC: ppp profile %s deleted", profile->profile_name);

	/* Give plugins visibility of ppp profile deleted */
	if (l2tp_profile_deleted_hook != NULL) {
		(*l2tp_profile_deleted_hook)(L2TP_PROFILE_TYPE_PPP, profile->profile_name);
	}

	usl_list_del(&profile->list);

	if (profile->radius_hint != NULL) {
		free(profile->radius_hint);
	}
	if (profile->ip_pool_name != NULL) {
		free(profile->ip_pool_name);
	}
	if (profile->profile_name != NULL) {
		free(profile->profile_name);
	}
	if (profile->local_name != NULL) {
		free(profile->local_name);
	}
	if (profile->remote_name != NULL) {
		free(profile->remote_name);
	}
	USL_POISON_MEMORY(profile, 0xe5, sizeof(*profile));
	free(profile);
	*result = 0;
	
out:	
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);
	return TRUE;
}

bool_t l2tp_ppp_profile_modify_1_svc(l2tp_api_ppp_profile_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_ppp_profile *profile;
	char *name;

	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, msg.profile_name);

	if ((msg.profile_name == NULL) || strlen(msg.profile_name) == 0) {
		*result = -L2TP_ERR_PROFILE_NAME_MISSING;
		goto out;
	}
	name = msg.profile_name;

	profile = l2tp_ppp_profile_find(name);
	if (profile == NULL) {
		*result = -L2TP_ERR_PPP_PROFILE_NOT_FOUND;
		goto out;
	}
	
	*result = l2tp_ppp_profile_modify(&msg, profile);
	if (*result < 0) {
		goto out;
	}

	/* Remember all non-default parameters */
	profile->flags |= msg.flags;
	profile->flags2 |= msg.flags2;

	L2TP_DEBUG(L2TP_API, "%s: flags=%x/%x", __func__, profile->flags, profile->flags2);

	L2TP_DEBUG(L2TP_FUNC, "FUNC: ppp profile %s modified", profile->profile_name);

	/* Give plugins visibility of ppp profile modified */
	if (l2tp_profile_modified_hook != NULL) {
		(*l2tp_profile_modified_hook)(L2TP_PROFILE_TYPE_PPP, profile->profile_name);
	}

out:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);
	return TRUE;
}

int l2tp_ppp_profile_get(char *name, struct l2tp_api_ppp_profile_msg_data *result)
{
	struct l2tp_ppp_profile *profile;

	if ((name == NULL) || strlen(name) == 0) {
		result->profile_name = strdup("");
		result->result_code = -L2TP_ERR_PROFILE_NAME_MISSING;
		goto out;
	}

	profile = l2tp_ppp_profile_find(name);
	if (profile == NULL) {
		result->profile_name = strdup(name);
		result->result_code = -L2TP_ERR_PPP_PROFILE_NOT_FOUND;
		goto out;
	}
	
	memset(result, 0, sizeof(*result));
	result->flags = profile->flags;
	result->flags2 = profile->flags2;
	result->profile_name = strdup(profile->profile_name);
	if (result->profile_name == NULL) {
		result->result_code = -ENOMEM;
	}
	result->trace_flags = profile->trace_flags;
	result->asyncmap = profile->asyncmap;
	result->mru = profile->mru;
	result->mtu = profile->mtu;
	result->auth_none = profile->auth_none;
	result->auth_peer = profile->auth_peer;
	result->auth_refuse_pap = profile->auth_refuse_pap;
	result->auth_refuse_chap = profile->auth_refuse_chap;
	result->auth_refuse_mschap = profile->auth_refuse_mschap;
	result->auth_refuse_mschapv2 = profile->auth_refuse_mschapv2;
	result->auth_refuse_eap = profile->auth_refuse_eap;
	result->sync_mode = profile->sync_mode;
	result->chap_interval = profile->chap_interval;
	result->chap_max_challenge = profile->chap_max_challenge;
	result->chap_restart = profile->chap_restart;
	result->pap_max_auth_requests = profile->pap_max_auth_requests;
	result->pap_restart_interval = profile->pap_restart_interval;
	result->pap_timeout = profile->pap_timeout;
	result->idle_timeout = profile->idle_timeout;
	result->ipcp_max_config_requests = profile->ipcp_max_config_requests;
	result->ipcp_max_config_naks = profile->ipcp_max_config_naks;
	result->ipcp_max_terminate_requests = profile->ipcp_max_terminate_requests;
	result->ipcp_retransmit_interval = profile->ipcp_retransmit_interval;
	result->lcp_echo_failure_count = profile->lcp_echo_failure_count;
	result->lcp_echo_interval = profile->lcp_echo_interval;
	result->lcp_max_config_requests = profile->lcp_max_config_requests;
	result->lcp_max_config_naks = profile->lcp_max_config_naks;
	result->lcp_max_terminate_requests = profile->lcp_max_terminate_requests;
	result->lcp_retransmit_interval = profile->lcp_retransmit_interval;
	result->max_connect_time = profile->max_connect_time;
	result->max_failure_count = profile->max_failure_count;
	result->local_ip_addr.s_addr = profile->local_ip_addr;
	result->peer_ip_addr.s_addr = profile->peer_ip_addr;
	result->dns_addr_1.s_addr = profile->dns_addr_1;
	result->dns_addr_2.s_addr = profile->dns_addr_2;
	result->wins_addr_1.s_addr = profile->wins_addr_1;
	result->wins_addr_2.s_addr = profile->wins_addr_2;
	if (profile->ip_pool_name != NULL) {
		OPTSTRING(result->ip_pool_name) = strdup(profile->ip_pool_name);
		if (OPTSTRING(result->ip_pool_name) == NULL) {
			result->result_code = -ENOMEM;
			goto out;
		}
		result->ip_pool_name.valid = 1;
	}
	result->use_radius = profile->use_radius;
	if (profile->radius_hint != NULL) {
		OPTSTRING(result->radius_hint) = strdup(profile->radius_hint);
		if (OPTSTRING(result->radius_hint) == NULL) {
			result->result_code = -ENOMEM;
			goto out;
		}
		result->radius_hint.valid = 1;
	}
	result->use_as_default_route = profile->use_as_default_route;
	result->multilink = profile->multilink;
	result->proxy_arp = profile->proxy_arp;
	if (profile->local_name != NULL) {
		OPTSTRING(result->local_name) = strdup(profile->local_name);
		if (OPTSTRING(result->local_name) == NULL) {
			result->result_code = -ENOMEM;
			goto out;
		}
		result->local_name.valid = 1;
	}
	if (profile->remote_name != NULL) {
		OPTSTRING(result->remote_name) = strdup(profile->remote_name);
		if (OPTSTRING(result->remote_name) == NULL) {
			result->result_code = -ENOMEM;
			goto out;
		}
		result->remote_name.valid = 1;
	}

out:
	L2TP_DEBUG(L2TP_API, "%s: flags=%x/%x result=%d", __func__, result->flags, result->flags2, result->result_code);
	return result->result_code;
}

void l2tp_ppp_profile_msg_free(struct l2tp_api_ppp_profile_msg_data *msg)
{
	if (OPTSTRING_PTR(msg->ip_pool_name) != NULL) {
		free(OPTSTRING(msg->ip_pool_name));
	}
	if (OPTSTRING_PTR(msg->radius_hint) != NULL) {
		free(OPTSTRING(msg->radius_hint));
	}
	if (OPTSTRING_PTR(msg->local_name) != NULL) {
		free(OPTSTRING(msg->local_name));
	}
	if (OPTSTRING_PTR(msg->remote_name) != NULL) {
		free(OPTSTRING(msg->remote_name));
	}
	if (msg->profile_name != NULL) {
		free(msg->profile_name);
	}
	free(msg);
}

bool_t l2tp_ppp_profile_get_1_svc(char *name, struct l2tp_api_ppp_profile_msg_data *result, struct svc_req *req)
{
	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, name);

	memset(result, 0, sizeof(*result));
	result->result_code = l2tp_ppp_profile_get(name, result);

	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, result->result_code);

	return TRUE;
}

bool_t l2tp_ppp_profile_list_1_svc(l2tp_api_ppp_profile_list_msg_data *result, struct svc_req *req)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_ppp_profile *profile;
	struct l2tp_api_ppp_profile_list_entry *entry;
	struct l2tp_api_ppp_profile_list_entry *tmpe;
	int num_profiles = 0;

	L2TP_DEBUG(L2TP_API, "%s: enter", __func__);

	memset(result, 0, sizeof(*result));

	result->profiles = calloc(1, sizeof(*result->profiles));
	if (result->profiles == NULL) {
		result->result = -ENOMEM;
		goto error;
	}
	entry = result->profiles;
	usl_list_for_each(walk, tmp, &l2tp_ppp_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_ppp_profile, list);

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

bool_t l2tp_ppp_profile_unset_1_svc(l2tp_api_ppp_profile_unset_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_ppp_profile *profile;

	L2TP_DEBUG(L2TP_API, "%s: profile: %s", __func__, msg.profile_name);

	if ((msg.profile_name == NULL) || strlen(msg.profile_name) == 0) {
		*result = -L2TP_ERR_PROFILE_NAME_MISSING;
		goto out;
	}

	profile = l2tp_ppp_profile_find(msg.profile_name);
	if (profile == NULL) {
		*result = -L2TP_ERR_PPP_PROFILE_NOT_FOUND;
		goto out;
	}

	/* Set all requested fields to their default values */
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_TRACE_FLAGS) {
		if (l2tp_opt_debug) {
			profile->trace_flags = -1;
		} else {
			profile->trace_flags = L2TP_API_PPP_PROFILE_DEFAULT_TRACE_FLAGS;
		}
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_ASYNCMAP) {
		profile->asyncmap = L2TP_API_PPP_PROFILE_DEFAULT_ASYNCMAP;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_MRU) {
		profile->mru = L2TP_API_PPP_PROFILE_DEFAULT_MRU;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_MTU) {
		profile->mtu = L2TP_API_PPP_PROFILE_DEFAULT_MTU;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_USE_RADIUS) {
		profile->use_radius = L2TP_API_PPP_PROFILE_DEFAULT_USE_RADIUS;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_RADIUS_HINT) {
		if (profile->radius_hint != NULL) {
			free(profile->radius_hint);
		}
		profile->radius_hint = NULL;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_NONE) {
		profile->auth_none = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_NONE;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_PEER) {
		profile->auth_peer = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_PEER;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_PAP) {
		profile->auth_refuse_pap = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_PAP;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_CHAP) {
		profile->auth_refuse_chap = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_CHAP;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAP) {
		profile->auth_refuse_mschap = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_MSCHAP;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_MSCHAPV2) {
		profile->auth_refuse_mschapv2 = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_MSCHAPV2;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_AUTH_REFUSE_EAP) {
		profile->auth_refuse_eap = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_EAP;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_SYNC_MODE) {
		profile->sync_mode = L2TP_API_PPP_PROFILE_DEFAULT_SYNC_MODE;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_CHAP_INTERVAL) {
		profile->chap_interval = L2TP_API_PPP_PROFILE_DEFAULT_CHAP_INTERVAL;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_CHAP_MAX_CHALLENGE) {
		profile->chap_max_challenge = L2TP_API_PPP_PROFILE_DEFAULT_CHAP_MAX_CHALLENGE;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_CHAP_RESTART) {
		profile->chap_restart = L2TP_API_PPP_PROFILE_DEFAULT_CHAP_RESTART;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_PAP_MAX_AUTH_REQUESTS) {
		profile->pap_max_auth_requests = L2TP_API_PPP_PROFILE_DEFAULT_PAP_MAX_AUTH_REQUESTS;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_PAP_RESTART_INTERVAL) {
		profile->pap_restart_interval = L2TP_API_PPP_PROFILE_DEFAULT_PAP_RESTART_INTERVAL;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_PAP_TIMEOUT) {
		profile->pap_timeout = L2TP_API_PPP_PROFILE_DEFAULT_PAP_TIMEOUT;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_IDLE_TIMEOUT) {
		profile->idle_timeout = L2TP_API_PPP_PROFILE_DEFAULT_IDLE_TIMEOUT;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_REQUESTS) {
		profile->ipcp_max_config_requests = L2TP_API_PPP_PROFILE_DEFAULT_IPCP_MAX_CONFIG_REQUESTS;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_CONFIG_NAKS) {
		profile->ipcp_max_config_naks = L2TP_API_PPP_PROFILE_DEFAULT_IPCP_MAX_CONFIG_NAKS;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_MAX_TERMINATE_REQUESTS) {
		profile->ipcp_max_terminate_requests = L2TP_API_PPP_PROFILE_DEFAULT_IPCP_MAX_TERMINATE_REQUESTS;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_IPCP_RETRANSMIT_INTERVAL) {
		profile->ipcp_retransmit_interval = L2TP_API_PPP_PROFILE_DEFAULT_IPCP_RETRANSMIT_INTERVAL;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_FAILURE_COUNT) {
		profile->lcp_echo_failure_count = L2TP_API_PPP_PROFILE_DEFAULT_LCP_ECHO_FAILURE_COUNT;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_LCP_ECHO_INTERVAL) {
		profile->lcp_echo_interval = L2TP_API_PPP_PROFILE_DEFAULT_LCP_ECHO_INTERVAL;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_REQUESTS) {
		profile->lcp_max_config_requests = L2TP_API_PPP_PROFILE_DEFAULT_LCP_MAX_CONFIG_REQUESTS;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_CONFIG_NAKS) {
		profile->lcp_max_config_naks = L2TP_API_PPP_PROFILE_DEFAULT_LCP_MAX_CONFIG_NAKS;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_LCP_MAX_TERMINATE_REQUESTS) {
		profile->lcp_max_terminate_requests = L2TP_API_PPP_PROFILE_DEFAULT_LCP_MAX_TERMINATE_REQUESTS;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_LCP_RETRANSMIT_INTERVAL) {
		profile->lcp_retransmit_interval = L2TP_API_PPP_PROFILE_DEFAULT_LCP_RETRANSMIT_INTERVAL;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_MAX_CONNECT_TIME) {
		profile->max_connect_time = L2TP_API_PPP_PROFILE_DEFAULT_MAX_CONNECT_TIME;
	}
	if (msg.flags & L2TP_API_PPP_PROFILE_FLAG_MAX_FAILURE_COUNT) {
		profile->max_failure_count = L2TP_API_PPP_PROFILE_DEFAULT_MAX_FAILURE_COUNT;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_LOCAL_IP_ADDR) {
		profile->local_ip_addr = L2TP_API_PPP_PROFILE_DEFAULT_LOCAL_IP_ADDR;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_PEER_IP_ADDR) {
		profile->peer_ip_addr = L2TP_API_PPP_PROFILE_DEFAULT_PEER_IP_ADDR;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_1) {
		profile->dns_addr_1 = L2TP_API_PPP_PROFILE_DEFAULT_DNS_ADDR_1;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_DNS_ADDR_2) {
		profile->dns_addr_2 = L2TP_API_PPP_PROFILE_DEFAULT_DNS_ADDR_2;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_1) {
		profile->wins_addr_1 = L2TP_API_PPP_PROFILE_DEFAULT_WINS_ADDR_1;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_WINS_ADDR_2) {
		profile->wins_addr_2 = L2TP_API_PPP_PROFILE_DEFAULT_WINS_ADDR_2;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_IP_POOL_NAME) {
		if (profile->ip_pool_name != NULL) {
			free(profile->ip_pool_name);
		}
		profile->ip_pool_name = NULL;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_USE_AS_DEFAULT_ROUTE) {
		profile->use_as_default_route = L2TP_API_PPP_PROFILE_DEFAULT_USE_AS_DEFAULT_ROUTE;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_MULTILINK) {
		profile->multilink = L2TP_API_PPP_PROFILE_DEFAULT_MULTILINK;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_PROXY_ARP) {
		profile->proxy_arp = L2TP_API_PPP_PROFILE_DEFAULT_PROXY_ARP;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_LOCAL_NAME) {
		if (profile->local_name != NULL) {
			free(profile->local_name);
		}
		profile->local_name = NULL;
	}
	if (msg.flags2 & L2TP_API_PPP_PROFILE_FLAG_REMOTE_NAME) {
		if (profile->remote_name != NULL) {
			free(profile->remote_name);
		}
		profile->remote_name = NULL;
	}

	/* Clear all requested flags */
	profile->flags &= ~(msg.flags);
	profile->flags2 &= ~(msg.flags2);

	L2TP_DEBUG(L2TP_API, "%s: flags=%x/%x", __func__, profile->flags, profile->flags2);

	L2TP_DEBUG(L2TP_FUNC, "FUNC: ppp profile %s unset", profile->profile_name);

	/* Give plugins visibility of ppp profile modified */
	if (l2tp_profile_modified_hook != NULL) {
		(*l2tp_profile_modified_hook)(L2TP_PROFILE_TYPE_PPP, profile->profile_name);
	}

out:
	L2TP_DEBUG(L2TP_API, "%s: result=%d", __func__, *result);
	return TRUE;
}


/*****************************************************************************
 * Init and cleanup
 *****************************************************************************/

/* Called to reset the profiles back to their initial values. Since
 * we're only concerned with profiles in this module, we can just call
 * cleanup and reinit here. 
 */
void l2tp_ppp_reinit(void)
{
	l2tp_ppp_cleanup();
	l2tp_ppp_init();
}

void l2tp_ppp_init(void)
{
	l2tp_ppp_defaults = calloc(1, sizeof(*l2tp_ppp_defaults));
	if (l2tp_ppp_defaults == NULL) {
		goto nomem;
	}
	l2tp_ppp_defaults->profile_name = strdup(L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME);
	if (l2tp_opt_debug) {
		l2tp_ppp_defaults->trace_flags = -1;
	} else {
		l2tp_ppp_defaults->trace_flags = L2TP_API_PPP_PROFILE_DEFAULT_TRACE_FLAGS;
	}
	l2tp_ppp_defaults->asyncmap = L2TP_API_PPP_PROFILE_DEFAULT_ASYNCMAP;
	l2tp_ppp_defaults->mru = L2TP_API_PPP_PROFILE_DEFAULT_MRU;
	l2tp_ppp_defaults->mtu = L2TP_API_PPP_PROFILE_DEFAULT_MTU;
	l2tp_ppp_defaults->auth_none = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_NONE;
	l2tp_ppp_defaults->auth_peer = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_PEER;
	l2tp_ppp_defaults->auth_refuse_pap = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_PAP;
	l2tp_ppp_defaults->auth_refuse_chap = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_CHAP;
	l2tp_ppp_defaults->auth_refuse_mschap = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_MSCHAP;
	l2tp_ppp_defaults->auth_refuse_mschapv2 = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_MSCHAPV2;
	l2tp_ppp_defaults->auth_refuse_eap = L2TP_API_PPP_PROFILE_DEFAULT_AUTH_REFUSE_EAP;
	l2tp_ppp_defaults->sync_mode = L2TP_API_PPP_PROFILE_DEFAULT_SYNC_MODE;
	l2tp_ppp_defaults->chap_interval = L2TP_API_PPP_PROFILE_DEFAULT_CHAP_INTERVAL;
	l2tp_ppp_defaults->chap_max_challenge = L2TP_API_PPP_PROFILE_DEFAULT_CHAP_MAX_CHALLENGE;
	l2tp_ppp_defaults->chap_restart = L2TP_API_PPP_PROFILE_DEFAULT_CHAP_RESTART;
	l2tp_ppp_defaults->pap_max_auth_requests = L2TP_API_PPP_PROFILE_DEFAULT_PAP_MAX_AUTH_REQUESTS;
	l2tp_ppp_defaults->pap_restart_interval = L2TP_API_PPP_PROFILE_DEFAULT_PAP_RESTART_INTERVAL;
	l2tp_ppp_defaults->pap_timeout = L2TP_API_PPP_PROFILE_DEFAULT_PAP_TIMEOUT;
	l2tp_ppp_defaults->idle_timeout = L2TP_API_PPP_PROFILE_DEFAULT_IDLE_TIMEOUT;
	l2tp_ppp_defaults->ipcp_max_config_requests = L2TP_API_PPP_PROFILE_DEFAULT_IPCP_MAX_CONFIG_REQUESTS;
	l2tp_ppp_defaults->ipcp_max_config_naks = L2TP_API_PPP_PROFILE_DEFAULT_IPCP_MAX_CONFIG_NAKS;
	l2tp_ppp_defaults->ipcp_max_terminate_requests = L2TP_API_PPP_PROFILE_DEFAULT_IPCP_MAX_TERMINATE_REQUESTS;
	l2tp_ppp_defaults->ipcp_retransmit_interval = L2TP_API_PPP_PROFILE_DEFAULT_IPCP_RETRANSMIT_INTERVAL;
	l2tp_ppp_defaults->lcp_echo_failure_count = L2TP_API_PPP_PROFILE_DEFAULT_LCP_ECHO_FAILURE_COUNT;
	l2tp_ppp_defaults->lcp_echo_interval = L2TP_API_PPP_PROFILE_DEFAULT_LCP_ECHO_INTERVAL;
	l2tp_ppp_defaults->lcp_max_config_requests = L2TP_API_PPP_PROFILE_DEFAULT_LCP_MAX_CONFIG_REQUESTS;
	l2tp_ppp_defaults->lcp_max_config_naks = L2TP_API_PPP_PROFILE_DEFAULT_LCP_MAX_CONFIG_NAKS;
	l2tp_ppp_defaults->lcp_max_terminate_requests = L2TP_API_PPP_PROFILE_DEFAULT_LCP_MAX_TERMINATE_REQUESTS;
	l2tp_ppp_defaults->lcp_retransmit_interval = L2TP_API_PPP_PROFILE_DEFAULT_LCP_RETRANSMIT_INTERVAL;
	l2tp_ppp_defaults->max_connect_time = L2TP_API_PPP_PROFILE_DEFAULT_MAX_CONNECT_TIME;
	l2tp_ppp_defaults->max_failure_count = L2TP_API_PPP_PROFILE_DEFAULT_MAX_FAILURE_COUNT;
	l2tp_ppp_defaults->local_ip_addr = L2TP_API_PPP_PROFILE_DEFAULT_LOCAL_IP_ADDR;
	l2tp_ppp_defaults->peer_ip_addr = L2TP_API_PPP_PROFILE_DEFAULT_PEER_IP_ADDR;
	l2tp_ppp_defaults->dns_addr_1 = L2TP_API_PPP_PROFILE_DEFAULT_DNS_ADDR_1;
	l2tp_ppp_defaults->dns_addr_2 = L2TP_API_PPP_PROFILE_DEFAULT_DNS_ADDR_2;
	l2tp_ppp_defaults->wins_addr_1 = L2TP_API_PPP_PROFILE_DEFAULT_WINS_ADDR_1;
	l2tp_ppp_defaults->wins_addr_2 = L2TP_API_PPP_PROFILE_DEFAULT_WINS_ADDR_2;
	l2tp_ppp_defaults->use_radius = L2TP_API_PPP_PROFILE_DEFAULT_USE_RADIUS;
	l2tp_ppp_defaults->use_as_default_route = L2TP_API_PPP_PROFILE_DEFAULT_USE_AS_DEFAULT_ROUTE;
	l2tp_ppp_defaults->multilink = L2TP_API_PPP_PROFILE_DEFAULT_MULTILINK;
	l2tp_ppp_defaults->proxy_arp = L2TP_API_PPP_PROFILE_DEFAULT_PROXY_ARP;
	if (strlen(L2TP_API_PPP_PROFILE_DEFAULT_RADIUS_HINT) > 0) {
		l2tp_ppp_defaults->radius_hint = strdup(L2TP_API_PPP_PROFILE_DEFAULT_RADIUS_HINT);
		if (l2tp_ppp_defaults->radius_hint == NULL) {
			goto nomem;
		}
	}
	l2tp_ppp_defaults->local_name = NULL;
	l2tp_ppp_defaults->remote_name = NULL;

	USL_LIST_HEAD_INIT(&l2tp_ppp_defaults->list);
	usl_list_add(&l2tp_ppp_defaults->list, &l2tp_ppp_profile_list);

	return;

nomem:
	fprintf(stderr, "Out of memory\n");
	exit(1);
}

void l2tp_ppp_cleanup(void)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_ppp_profile *profile;

	usl_list_for_each(walk, tmp, &l2tp_ppp_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_ppp_profile, list);
		usl_list_del(&profile->list);
		if (profile->profile_name != NULL) {
			free(profile->profile_name);
		}
		USL_POISON_MEMORY(profile, 0xe5, sizeof(*profile));
		free(profile);
	}
}

