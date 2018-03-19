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

int (*l2tp_peer_created_hook)(struct in_addr src, struct in_addr dest) = NULL;
int (*l2tp_peer_deleted_hook)(struct in_addr src, struct in_addr dest) = NULL;

static USL_LIST_HEAD(l2tp_peer_list);
static USL_LIST_HEAD(l2tp_peer_profile_list);

static struct l2tp_peer_profile *l2tp_peer_profile_default;


static void l2tp_peer_link(struct l2tp_peer *peer)
{
	L2TP_DEBUG(L2TP_FUNC, "%s: %x/%x", __func__, ntohl(peer->if_local_addr.s_addr), ntohl(peer->if_peer_addr.s_addr));

	usl_list_add(&peer->list, &l2tp_peer_list);
}

static void l2tp_peer_unlink(struct l2tp_peer *peer, int force)
{
	L2TP_DEBUG(L2TP_FUNC, "%s: %x/%x", __func__, ntohl(peer->if_local_addr.s_addr), ntohl(peer->if_peer_addr.s_addr));

	if ((peer->use_count == 0) || force) {
		usl_list_del(&peer->list);
		l2tp_peer_free(peer);
	}
}

struct l2tp_peer *l2tp_peer_find(struct in_addr const *src, struct in_addr const *dest)
{
	struct l2tp_peer *peer;
	struct usl_list_head *tmp;
	struct usl_list_head *walk;

	usl_list_for_each(walk, tmp, &l2tp_peer_list) {
		peer = usl_list_entry(walk, struct l2tp_peer, list);
		if (((src == NULL) || (peer->if_local_addr.s_addr == src->s_addr)) &&
		    ((dest == NULL) || (peer->if_peer_addr.s_addr == dest->s_addr))) {
			return peer;
		}
	}

	return NULL;
}

void l2tp_peer_inc_use_count(struct l2tp_peer *peer)
{
	peer->use_count++;
	L2TP_DEBUG(L2TP_FUNC, "%s: %x/%x: count now %d", __func__, ntohl(peer->if_local_addr.s_addr), 
		   ntohl(peer->if_peer_addr.s_addr), peer->use_count);
}

void l2tp_peer_dec_use_count(struct l2tp_peer *peer)
{
	peer->use_count--;
	L2TP_DEBUG(L2TP_FUNC, "%s: %x/%x: count now %d", __func__, ntohl(peer->if_local_addr.s_addr), 
		   ntohl(peer->if_peer_addr.s_addr), peer->use_count);
	if (peer->use_count < 0) {
		l2tp_log(LOG_ERR, "Peer %x/%x use count gone negative! Caller %p",
			 ntohl(peer->if_local_addr.s_addr), ntohl(peer->if_peer_addr.s_addr), __builtin_return_address(0));
		return;
	}
	if (peer->use_count == 0) {
		l2tp_peer_unlink(peer, 0);
	}
}

void l2tp_peer_free(struct l2tp_peer *peer)
{
	if (peer->use_count != 0) {
		l2tp_log(LOG_ERR, "Free peer %x/%x when use_count=%d", 
			 ntohl(peer->if_local_addr.s_addr), ntohl(peer->if_peer_addr.s_addr), peer->use_count);
		return;
	}

	if (l2tp_peer_deleted_hook != NULL) {
		(*l2tp_peer_deleted_hook)(peer->if_local_addr, peer->if_peer_addr);
	}

	L2TP_DEBUG(L2TP_FUNC, "%s: %x/%x", __func__, ntohl(peer->if_local_addr.s_addr), ntohl(peer->if_peer_addr.s_addr));
	USL_POISON_MEMORY(peer, 0xe9, sizeof(*peer));
	free(peer);
}

struct l2tp_peer *l2tp_peer_alloc(struct in_addr src, struct in_addr dest)
{
	struct l2tp_peer *peer;

	peer = calloc(1, sizeof(struct l2tp_peer));
	if (peer == NULL) {
		l2tp_stats.no_peer_resources++;
		goto error;
	}

	peer->if_local_addr.s_addr = src.s_addr;
	peer->if_peer_addr.s_addr = dest.s_addr;

	USL_LIST_HEAD_INIT(&peer->list);

	L2TP_DEBUG(L2TP_FUNC, "%s: %x/%x", __func__, ntohl(peer->if_local_addr.s_addr), ntohl(peer->if_peer_addr.s_addr));

	l2tp_peer_link(peer);

	if (l2tp_peer_created_hook != NULL) {
		(*l2tp_peer_created_hook)(peer->if_local_addr, peer->if_peer_addr);
	}

out:
	return peer;

error:
	if (peer != NULL) {
		free(peer);
	}

	goto out;
}

/*****************************************************************************
 * Peer profiles
 *****************************************************************************/

struct l2tp_peer_profile *l2tp_peer_profile_find(char *name)
{
	struct l2tp_peer_profile *profile;
	struct usl_list_head *tmp;
	struct usl_list_head *walk;

	usl_list_for_each(walk, tmp, &l2tp_peer_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_peer_profile, list);
		if (strcmp(profile->profile_name, name) == 0) {
			return profile;
		}
	}

	return NULL;
}

static int l2tp_peer_netmask_len(struct in_addr netmask)
{
	int bitpos;
	in_addr_t mask = ntohl(netmask.s_addr);

	for (bitpos = 0; bitpos < 32; bitpos++) {
		if (mask & (1 << bitpos)) {
			return 32 - bitpos;
		}
	}

	return 32;
}

/* Finding a profile by address involves taking account of netmasks.
 * We find the best match (longest netmask.
 */
struct l2tp_peer_profile *l2tp_peer_profile_find_by_addr(struct in_addr peer_addr)
{
	struct l2tp_peer_profile *profile;
	struct l2tp_peer_profile *candidate = NULL;
	int candidate_netmask_len = 0;
	struct usl_list_head *tmp;
	struct usl_list_head *walk;

	L2TP_DEBUG(L2TP_FUNC, "%s: peer_addr=%s", __func__, inet_ntoa(peer_addr));
	usl_list_for_each(walk, tmp, &l2tp_peer_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_peer_profile, list);
		if (profile->peer_addr.sin_addr.s_addr == peer_addr.s_addr) {
			L2TP_DEBUG(L2TP_FUNC, "%s: peer_addr=%s: profile %s is exact match", __func__, 
				   inet_ntoa(peer_addr), profile->profile_name);
			return profile;
		}
		L2TP_DEBUG(L2TP_FUNC, "%s: profile %s has netmask %#x, netmask_len=%d", __func__, 
			   profile->profile_name, ntohl(profile->netmask.s_addr), profile->netmask_len);
		if ((profile->peer_addr.sin_addr.s_addr & profile->netmask.s_addr) == (peer_addr.s_addr & profile->netmask.s_addr)) {
			if (profile->netmask_len > candidate_netmask_len) {
				L2TP_DEBUG(L2TP_FUNC, "%s: peer_addr=%s found candidate %s, netmask_len=%d", __func__, 
					   inet_ntoa(peer_addr), profile->profile_name, profile->netmask_len);
				candidate = profile;
				candidate_netmask_len = profile->netmask_len;
			}
		}
	}

	if (candidate != NULL) {
		L2TP_DEBUG(L2TP_FUNC, "%s: peer_addr=%s: profile %s is best match", __func__, 
			   inet_ntoa(peer_addr), candidate->profile_name);
	} else {
		L2TP_DEBUG(L2TP_FUNC, "%s: peer_addr=%s: no profile found", __func__, 
			   inet_ntoa(peer_addr));
	}
	return candidate;
}

static void l2tp_peer_profile_free(struct l2tp_peer_profile *profile)
{
	if (profile->profile_name != NULL) free(profile->profile_name);
	if (profile->default_tunnel_profile_name != NULL) free(profile->default_tunnel_profile_name);
	if (profile->default_session_profile_name != NULL) free(profile->default_session_profile_name);
	if (profile->default_ppp_profile_name != NULL) free(profile->default_ppp_profile_name);
	USL_POISON_MEMORY(profile, 0xe6, sizeof(*profile));
	free(profile);
}

static struct l2tp_peer_profile *l2tp_peer_profile_alloc(char *name)
{
	struct l2tp_peer_profile *profile;

	profile = calloc(1, sizeof(struct l2tp_peer_profile));
	if (profile == NULL) {
		l2tp_stats.no_peer_resources++;
		goto error;
	}
	profile->profile_name = strdup(name);
	if (profile->profile_name == NULL) {
		l2tp_stats.no_peer_resources++;
		goto error;
	}

	/* Fill with defaults */
	profile->we_can_be_lac = l2tp_peer_profile_default->we_can_be_lac;
	profile->we_can_be_lns = l2tp_peer_profile_default->we_can_be_lns;
	profile->default_tunnel_profile_name = strdup(l2tp_peer_profile_default->default_tunnel_profile_name);
	profile->default_session_profile_name = strdup(l2tp_peer_profile_default->default_session_profile_name);
	profile->default_ppp_profile_name = strdup(l2tp_peer_profile_default->default_ppp_profile_name);
	if ((profile->default_tunnel_profile_name == NULL) ||
	    (profile->default_session_profile_name == NULL) ||
	    (profile->default_ppp_profile_name == NULL)) {
		l2tp_stats.no_peer_resources++;
		goto error;
	}
	profile->netmask.s_addr = INADDR_BROADCAST;
	profile->netmask_len = 32;

	USL_LIST_HEAD_INIT(&profile->list);

out:
	return profile;

error:
	if (profile != NULL) {
		l2tp_peer_profile_free(profile);
	}

	goto out;
}

/*****************************************************************************
 * Peer profile management interface
 *****************************************************************************/

static int l2tp_peer_profile_fill(struct l2tp_api_peer_profile_msg_data *msg, struct l2tp_peer_profile *profile)
{
	int result = 0;

	if (msg->flags & L2TP_API_PEER_PROFILE_FLAG_LACLNS) {
		profile->we_can_be_lac = msg->we_can_be_lac ? -1 : 0;
		profile->we_can_be_lns = msg->we_can_be_lns ? -1 : 0;
	}
	if (msg->flags & L2TP_API_PEER_PROFILE_FLAG_TUNNEL_PROFILE_NAME) {
		if (profile->default_tunnel_profile_name != NULL) {
			free(profile->default_tunnel_profile_name);
		}
		profile->default_tunnel_profile_name = strdup(OPTSTRING(msg->default_tunnel_profile_name));
		if (profile->default_tunnel_profile_name == NULL) {
			result = -ENOMEM;
			goto error;
		}
	}
	if (msg->flags & L2TP_API_PEER_PROFILE_FLAG_SESSION_PROFILE_NAME) {
		if (profile->default_session_profile_name != NULL) {
			free(profile->default_session_profile_name);
		}
		profile->default_session_profile_name = strdup(OPTSTRING(msg->default_session_profile_name));
		if (profile->default_session_profile_name == NULL) {
			result = -ENOMEM;
			goto error;
		}
	}
	if (msg->flags & L2TP_API_PEER_PROFILE_FLAG_PPP_PROFILE_NAME) {
		if (profile->default_ppp_profile_name != NULL) {
			free(profile->default_ppp_profile_name);
		}
		profile->default_ppp_profile_name = strdup(OPTSTRING(msg->default_ppp_profile_name));
		if (profile->default_ppp_profile_name == NULL) {
			result = -ENOMEM;
			goto error;
		}
	}
	if (msg->flags & L2TP_API_PEER_PROFILE_FLAG_PEER_IPADDR) {
		profile->peer_addr.sin_addr.s_addr = msg->peer_addr.s_addr;
	}

	if (msg->flags & L2TP_API_PEER_PROFILE_FLAG_PEER_PORT) {
		profile->peer_addr.sin_port = msg->peer_port;
	}
	if (msg->flags & L2TP_API_PEER_PROFILE_FLAG_NETMASK) {
		struct in_addr addr;
		addr.s_addr = msg->netmask.s_addr;
		profile->netmask = addr;
		profile->netmask_len = l2tp_peer_netmask_len(addr);
	}

error:
	return result;
}

bool_t l2tp_peer_profile_create_1_svc(struct l2tp_api_peer_profile_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_peer_profile *profile;

	profile = l2tp_peer_profile_find(msg.profile_name);
	if (profile != NULL) {
		*result = -L2TP_ERR_PROFILE_ALREADY_EXISTS;
		goto out;
	}

	profile = l2tp_peer_profile_alloc(msg.profile_name);
	if (profile == NULL) {
		*result = -ENOMEM;
		goto out;
	}

	/* Override defaults with user values */
	*result = l2tp_peer_profile_fill(&msg, profile);
	if (*result < 0) {
		goto error;
	}

	/* Remember all non-default parameters */
	profile->flags |= msg.flags;

	usl_list_add(&profile->list, &l2tp_peer_profile_list);

	L2TP_DEBUG(L2TP_FUNC, "FUNC: peer profile %s created", profile->profile_name);

	/* Give plugins visibility of peer profile created */
	if (l2tp_profile_created_hook != NULL) {
		(*l2tp_profile_created_hook)(L2TP_PROFILE_TYPE_PEER, profile->profile_name);
	}

out:
	return TRUE;

error:
	l2tp_peer_profile_free(profile);
	goto out;
}

bool_t l2tp_peer_profile_delete_1_svc(char *name, int *result, struct svc_req *req)
{
	struct l2tp_peer_profile *profile;

	if ((name == NULL) || strlen(name) == 0) {
		*result = -L2TP_ERR_PROFILE_NAME_MISSING;
		goto out;
	}

	/* Prevent deletion of default profile */
	if (strcmp(name, L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME) == 0) {
		*result = -L2TP_ERR_PROFILE_NAME_ILLEGAL;
		goto out;
	}

	profile = l2tp_peer_profile_find(name);
	if (profile == NULL) {
		*result = -L2TP_ERR_PEER_PROFILE_NOT_FOUND;
		goto out;
	}

	L2TP_DEBUG(L2TP_FUNC, "FUNC: peer profile %s deleted", profile->profile_name);

	/* Give plugins visibility of peer profile deleted */
	if (l2tp_profile_deleted_hook != NULL) {
		(*l2tp_profile_deleted_hook)(L2TP_PROFILE_TYPE_PEER, profile->profile_name);
	}

	usl_list_del(&profile->list);
	l2tp_peer_profile_free(profile);
	*result = 0;

out:
	return TRUE;
}

bool_t l2tp_peer_profile_modify_1_svc(l2tp_api_peer_profile_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_peer_profile *profile;

	profile = l2tp_peer_profile_find(msg.profile_name);
	if (profile == NULL) {
		*result = -L2TP_ERR_PEER_PROFILE_NOT_FOUND;
		goto out;
	}

	*result = l2tp_peer_profile_fill(&msg, profile);
	if (*result < 0) {
		goto out;
	}

	/* Remember all non-default parameters */
	profile->flags |= msg.flags;

	L2TP_DEBUG(L2TP_FUNC, "FUNC: peer profile %s modified", profile->profile_name);

	/* Give plugins visibility of peer profile modified */
	if (l2tp_profile_modified_hook != NULL) {
		(*l2tp_profile_modified_hook)(L2TP_PROFILE_TYPE_PEER, profile->profile_name);
	}


out:
	return TRUE;
}

bool_t l2tp_peer_profile_get_1_svc(char *name, l2tp_api_peer_profile_msg_data *result, struct svc_req *req)
{
	struct l2tp_peer_profile *profile;

	memset(result, 0, sizeof(*result));

	result->result_code = 0;

	profile = l2tp_peer_profile_find(name);
	if (profile == NULL) {
		result->profile_name = strdup(name);
		result->result_code = -L2TP_ERR_PEER_PROFILE_NOT_FOUND;
		goto out;
	}

	result->flags = profile->flags;
	result->peer_addr.s_addr = profile->peer_addr.sin_addr.s_addr;
	result->peer_port = profile->peer_addr.sin_port;
	result->we_can_be_lac = profile->we_can_be_lac;
	result->we_can_be_lns = profile->we_can_be_lns;
	result->profile_name = strdup(profile->profile_name);
	if (result->profile_name == NULL) {
		goto error;
	}
	if (profile->default_tunnel_profile_name != NULL) {
		OPTSTRING(result->default_tunnel_profile_name) = strdup(profile->default_tunnel_profile_name);
		if (OPTSTRING(result->default_tunnel_profile_name) == NULL) {
			goto error;
		}
		result->default_tunnel_profile_name.valid = 1;
	} else {
		result->default_tunnel_profile_name.valid = 0;
	}
	if (profile->default_session_profile_name != NULL) {
		OPTSTRING(result->default_session_profile_name) = strdup(profile->default_session_profile_name);
		if (OPTSTRING(result->default_session_profile_name) == NULL) {
			goto error;
		}
		result->default_session_profile_name.valid = 1;
	} else {
		result->default_session_profile_name.valid = 0;
	}
	if (profile->default_ppp_profile_name != NULL) {
		OPTSTRING(result->default_ppp_profile_name) = strdup(profile->default_ppp_profile_name);
		if (OPTSTRING(result->default_ppp_profile_name) == NULL) {
			goto error;
		}
		result->default_ppp_profile_name.valid = 1;
	} else {
		result->default_ppp_profile_name.valid = 0;
	}
	result->netmask.s_addr = profile->netmask.s_addr;
	
out:
	return TRUE;

error:
	if (result->profile_name != NULL) free(result->profile_name);
	if (OPTSTRING_PTR(result->default_tunnel_profile_name) != NULL) free(OPTSTRING(result->default_tunnel_profile_name));
	if (OPTSTRING_PTR(result->default_session_profile_name) != NULL) free(OPTSTRING(result->default_session_profile_name));
	if (OPTSTRING_PTR(result->default_ppp_profile_name) != NULL) free(OPTSTRING(result->default_ppp_profile_name));

	return FALSE;
}

bool_t l2tp_peer_profile_list_1_svc(struct l2tp_api_peer_profile_list_msg_data *result, struct svc_req *req)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_peer_profile *profile;
	struct l2tp_api_peer_profile_list_entry *entry;
	struct l2tp_api_peer_profile_list_entry *tmpe;
	int num_profiles = 0;

	memset(result, 0, sizeof(*result));

	result->profiles = calloc(1, sizeof(*result->profiles));
	if (result->profiles == NULL) {
		result->result = -ENOMEM;
		goto error;
	}
	entry = result->profiles;
	usl_list_for_each(walk, tmp, &l2tp_peer_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_peer_profile, list);

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

bool_t l2tp_peer_profile_unset_1_svc(l2tp_api_peer_profile_unset_msg_data msg, int *result, struct svc_req *req)
{
	struct l2tp_peer_profile *profile;

	profile = l2tp_peer_profile_find(msg.profile_name);
	if (profile == NULL) {
		*result = -L2TP_ERR_PEER_PROFILE_NOT_FOUND;
		goto out;
	}

	if (msg.flags & L2TP_API_PEER_PROFILE_FLAG_LACLNS) {
		profile->we_can_be_lac = -1;
		profile->we_can_be_lns = -1;
	}
	if (msg.flags & L2TP_API_PEER_PROFILE_FLAG_TUNNEL_PROFILE_NAME) {
		if (profile->default_tunnel_profile_name != NULL) {
			free(profile->default_tunnel_profile_name);
		}
		profile->default_tunnel_profile_name = NULL;
	}
	if (msg.flags & L2TP_API_PEER_PROFILE_FLAG_SESSION_PROFILE_NAME) {
		if (profile->default_session_profile_name != NULL) {
			free(profile->default_session_profile_name);
		}
		profile->default_session_profile_name = NULL;
	}
	if (msg.flags & L2TP_API_PEER_PROFILE_FLAG_PPP_PROFILE_NAME) {
		if (profile->default_ppp_profile_name != NULL) {
			free(profile->default_ppp_profile_name);
		}
		profile->default_ppp_profile_name = NULL;
	}
	if (msg.flags & L2TP_API_PEER_PROFILE_FLAG_PEER_IPADDR) {
		profile->peer_addr.sin_addr.s_addr = INADDR_ANY;
	}

	if (msg.flags & L2TP_API_PEER_PROFILE_FLAG_PEER_PORT) {
		profile->peer_addr.sin_port = 0;
	}
	if (msg.flags & L2TP_API_PEER_PROFILE_FLAG_NETMASK) {
		profile->netmask.s_addr = INADDR_BROADCAST;
		profile->netmask_len = 0;
	}

	/* Reset requested flags */
	profile->flags &= (~msg.flags);

	L2TP_DEBUG(L2TP_FUNC, "FUNC: peer profile %s unset", profile->profile_name);

	/* Give plugins visibility of peer profile modified */
	if (l2tp_profile_modified_hook != NULL) {
		(*l2tp_profile_modified_hook)(L2TP_PROFILE_TYPE_PEER, profile->profile_name);
	}


out:
	return TRUE;
}


/*****************************************************************************
 * Peers
 *****************************************************************************/

bool_t l2tp_peer_get_1_svc(struct l2tp_api_ip_addr local_addr, struct l2tp_api_ip_addr peer_addr, struct l2tp_api_peer_msg_data *result, struct svc_req *req)
{
	struct l2tp_peer *peer;
	struct usl_list_head *tmp;
	struct usl_list_head *walk;

	memset(result, 0, sizeof(*result));

	usl_list_for_each(walk, tmp, &l2tp_peer_list) {
		peer = usl_list_entry(walk, struct l2tp_peer, list);
		if ((peer->if_peer_addr.s_addr == peer_addr.s_addr) &&
		    ((local_addr.s_addr == INADDR_ANY) || (peer->if_local_addr.s_addr == local_addr.s_addr))) {
			result->result_code = 0;
			result->peer_addr.s_addr = peer->if_peer_addr.s_addr;
			result->local_addr.s_addr = peer->if_local_addr.s_addr;
			result->num_tunnels = peer->use_count;
			goto out;
		}
	}

	result->result_code = -L2TP_ERR_PEER_NOT_FOUND;
out:
	return TRUE;
}

bool_t l2tp_peer_list_1_svc(struct l2tp_api_peer_list_msg_data *result, struct svc_req *req)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_peer *peer;
	struct l2tp_api_peer_list_entry *entry;
	struct l2tp_api_peer_list_entry *tmpe;

	memset(result, 0, sizeof(*result));

	result->peers = calloc(1, sizeof(*result->peers));
	if (result->peers == NULL) {
		result->result = -ENOMEM;
		goto error;
	}
	entry = result->peers;
	usl_list_for_each(walk, tmp, &l2tp_peer_list) {
		peer = usl_list_entry(walk, struct l2tp_peer, list);

		entry->peer_addr.s_addr = peer->if_peer_addr.s_addr;
		entry->local_addr.s_addr = peer->if_local_addr.s_addr;

		tmpe = calloc(1, sizeof(*result->peers));
		if (tmpe == NULL) {
			result->result = -ENOMEM;
			goto error;
		}
		entry->next = tmpe;
		entry = tmpe;
	}

	return TRUE;

error:
	for (entry = result->peers; entry != NULL; ) {
		tmpe = entry->next;
		free(entry);
		entry = tmpe;
	}

	return TRUE;
}

/*****************************************************************************
 * Module init
 *****************************************************************************/

void l2tp_peer_reinit(void)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_peer_profile *profile;

	/* Remove all profiles */
	usl_list_for_each(walk, tmp, &l2tp_peer_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_peer_profile, list);
		usl_list_del(&profile->list);
		l2tp_peer_profile_free(profile);
	}

	l2tp_peer_profile_default = calloc(1, sizeof(*l2tp_peer_profile_default));

	/* Fill with defaults */
	l2tp_peer_profile_default->profile_name = strdup(L2TP_API_PEER_PROFILE_DEFAULT_PROFILE_NAME);
	l2tp_peer_profile_default->flags = 0;
	l2tp_peer_profile_default->we_can_be_lac = -1;
	l2tp_peer_profile_default->we_can_be_lns = -1;
	l2tp_peer_profile_default->default_tunnel_profile_name = strdup(L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME);
	l2tp_peer_profile_default->default_session_profile_name = strdup(L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME);
	l2tp_peer_profile_default->default_ppp_profile_name = strdup(L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME);
	if ((l2tp_peer_profile_default->profile_name == NULL) ||
	    (l2tp_peer_profile_default->default_tunnel_profile_name == NULL) ||
	    (l2tp_peer_profile_default->default_session_profile_name == NULL) ||
	    (l2tp_peer_profile_default->default_ppp_profile_name == NULL)) {
		fprintf(stderr, "out of memory");
		exit(1);
	}
	l2tp_peer_profile_default->netmask.s_addr = INADDR_BROADCAST;
	l2tp_peer_profile_default->netmask_len = 32;

	USL_LIST_HEAD_INIT(&l2tp_peer_profile_default->list);
	usl_list_add(&l2tp_peer_profile_default->list, &l2tp_peer_profile_list);
}

void l2tp_peer_init(void)
{
	l2tp_peer_reinit();
}

void l2tp_peer_cleanup(void)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_peer *peer;
	struct l2tp_peer_profile *profile;

	usl_list_for_each(walk, tmp, &l2tp_peer_list) {
		peer = usl_list_entry(walk, struct l2tp_peer, list);
		l2tp_peer_unlink(peer, 0);
	}

	usl_list_for_each(walk, tmp, &l2tp_peer_profile_list) {
		profile = usl_list_entry(walk, struct l2tp_peer_profile, list);
		usl_list_del(&profile->list);
		l2tp_peer_profile_free(profile);
	}
}

