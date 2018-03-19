/*****************************************************************************
 * Copyright (C) 2006,2007,2008 Katalix Systems Ltd
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

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include "usl.h"

#include "l2tp_private.h"

#define L2TP_STATUSFILE_DIR	"/var/run/openl2tpd"

static int (*l2tp_old_peer_created_hook)(struct in_addr src, struct in_addr dest) = NULL;
static int (*l2tp_old_peer_deleted_hook)(struct in_addr src, struct in_addr dest) = NULL;
static int (*l2tp_old_profile_created_hook)(enum l2tp_profile_type type, const char *name) = NULL;
static int (*l2tp_old_profile_deleted_hook)(enum l2tp_profile_type type, const char *name) = NULL;
static int (*l2tp_old_profile_modified_hook)(enum l2tp_profile_type type, const char *name) = NULL;
static int (*l2tp_old_tunnel_created_hook)(uint16_t tunnel_id) = NULL;
static int (*l2tp_old_tunnel_deleted_hook)(uint16_t tunnel_id) = NULL;
static int (*l2tp_old_tunnel_modified_hook)(uint16_t tunnel_id) = NULL;
static int (*l2tp_old_tunnel_up_hook)(uint16_t tunnel_id, uint16_t peer_tunnel_id) = NULL;
static int (*l2tp_old_tunnel_down_hook)(uint16_t tunnel_id) = NULL;
static int (*l2tp_old_session_created_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id) = NULL;
static int (*l2tp_old_session_deleted_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id) = NULL;
static int (*l2tp_old_session_modified_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id) = NULL;
static int (*l2tp_old_session_up_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id, uint16_t peer_tunnel_id, uint16_t peer_session_id) = NULL;
static int (*l2tp_old_session_down_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id) = NULL;


static FILE *l2tp_statusfile_file_create(const char *parent, const char *name)
{
	char filename[256];
	FILE *file;

	if (name != NULL) {
		sprintf(filename, L2TP_STATUSFILE_DIR "/%s/%s", parent, name);
	} else {
		sprintf(filename, L2TP_STATUSFILE_DIR "/%s", parent);
	}

	L2TP_DEBUG(L2TP_FUNC, "FUNC: creat(%s)", filename);
	file = fopen(filename, "w+");

	return file;
}

static int l2tp_statusfile_file_delete(const char *root, const char *parent, const char *name)
{
	int result;
	char filename[256];

	if (root == NULL) {
		if (name != NULL) {
			sprintf(filename, "%s/%s", parent, name);
		} else if (parent != NULL) {
			strcpy(filename, parent);
		} else {
			strcpy(filename, L2TP_STATUSFILE_DIR);
		}
	} else {
		if (name != NULL) {
			sprintf(filename, "%s/%s/%s", root, parent, name);
		} else if (parent != NULL) {
			sprintf(filename, "%s/%s", root, parent);
		} else {
			strcpy(filename, root);
		}
	}

	L2TP_DEBUG(L2TP_FUNC, "FUNC: unlink(%s)", filename);
#if 0
	result = 0;
#else
	result = unlink(filename);
#endif
	if (result < 0) {
		result = -errno;
	}

	return result;
}

static int l2tp_statusfile_dir_create(const char *parent, const char *name)
{
	int result;
	char dirname[256];

	if (name != NULL) {
		sprintf(dirname, L2TP_STATUSFILE_DIR "/%s/%s", parent, name);
	} else if (parent != NULL) {
		sprintf(dirname, L2TP_STATUSFILE_DIR "/%s", parent);
	} else {
		strcpy(dirname, L2TP_STATUSFILE_DIR);
	}
	
	L2TP_DEBUG(L2TP_FUNC, "FUNC: mkdir(%s)", dirname);
	result = mkdir(dirname, 0755);
	if (result < 0) {
		result = -errno;
	}
	if (result == -EEXIST) {
		result = 0;
	}

	return result;
}

static int l2tp_statusfile_dir_delete(const char *root, const char *parent, const char *name, int recursive)
{
	int result;
	char dirname[256];
	char filename[256];
	DIR *dir;
	struct dirent *entry;
	struct stat statbuf;

	if (root == NULL) {
		if (name != NULL) {
			sprintf(dirname, "%s/%s", parent, name);
		} else if (parent != NULL) {
			strcpy(dirname, parent);
		} else {
			strcpy(dirname, L2TP_STATUSFILE_DIR);
		}
	} else {
		if (name != NULL) {
			sprintf(dirname, "%s/%s/%s", root, parent, name);
		} else if (parent != NULL) {
			sprintf(dirname, "%s/%s", root, parent);
		} else {
			strcpy(dirname, root);
		}
	}

	if (recursive) {
		dir = opendir(dirname);
		if (dir == NULL) {
			result = -errno;
			goto out;
		}

		while ((entry = readdir(dir)) != NULL) {
			if ((strcmp(entry->d_name, ".") == 0) ||
			    (strcmp(entry->d_name, "..") == 0)) {
				continue;
			}
			sprintf(filename, "%s/%s", dirname, entry->d_name);
			result = stat(filename, &statbuf);
			if (result < 0) {
				continue;
			}
			if (S_ISDIR(statbuf.st_mode)) {
				(void) l2tp_statusfile_dir_delete(NULL, dirname, entry->d_name, recursive);
			} else {
				(void) l2tp_statusfile_file_delete(NULL, dirname, entry->d_name);
			}
		}
		L2TP_DEBUG(L2TP_FUNC, "FUNC: rmdir(%s)", dirname);
#if 0
		result = 0;
#else
		result = rmdir(dirname);
#endif
		closedir(dir);
	} else {
		L2TP_DEBUG(L2TP_FUNC, "FUNC: rmdir(%s)", dirname);
#if 0
		result = 0;
#else
		result = rmdir(dirname);
#endif
	}

out:
	return result;
}

static int l2tp_statusfile_profile_created_ind(enum l2tp_profile_type type, const char *name)
{
	int result = -EINVAL;
	const char *dirname = NULL;
	FILE *file;
	struct l2tp_api_peer_profile_msg_data peer;
	struct l2tp_api_tunnel_profile_msg_data tunnel;
	struct l2tp_api_session_profile_msg_data session;
	struct l2tp_api_ppp_profile_msg_data ppp;

	switch (type) {
	case L2TP_PROFILE_TYPE_PEER:
		dirname = "peer-profiles";
		break;
	case L2TP_PROFILE_TYPE_TUNNEL:
		dirname = "tunnel-profiles";
		break;
	case L2TP_PROFILE_TYPE_SESSION:
		dirname = "session-profiles";
		break;
	case L2TP_PROFILE_TYPE_PPP:
		dirname = "ppp-profiles";
		break;
		/* NODEFAULT */
	}

	result = l2tp_statusfile_dir_create(dirname, NULL);
	if (result < 0) {
		goto out;
	}
	file = l2tp_statusfile_file_create(dirname, name);
	if (file == NULL) {
		result = -errno;
		goto out;
	}

	switch (type) {
	case L2TP_PROFILE_TYPE_PEER:
		memset(&peer, 0, sizeof(peer));
		if (l2tp_peer_profile_get_1_svc((char *) name, &peer, NULL)) {
			l2tp_show_peer_profile(file, &peer);
		}
		break;
	case L2TP_PROFILE_TYPE_TUNNEL:
		memset(&tunnel, 0, sizeof(tunnel));
		if (l2tp_tunnel_profile_get_1_svc((char *) name, &tunnel, NULL)) {
			l2tp_show_tunnel_profile(file, &tunnel);
		}
		break;
	case L2TP_PROFILE_TYPE_SESSION:
		memset(&session, 0, sizeof(session));
		if (l2tp_session_profile_get_1_svc((char *) name, &session, NULL)) {
			l2tp_show_session_profile(file, &session);
		}
		break;
	case L2TP_PROFILE_TYPE_PPP:
		memset(&ppp, 0, sizeof(ppp));
		if (l2tp_ppp_profile_get_1_svc((char *) name, &ppp, NULL)) {
			l2tp_show_ppp_profile(file, &ppp);
		}
		break;
		/* NODEFAULT */
	}

	fclose(file);

out:
	return result;
}

static int l2tp_statusfile_profile_deleted_ind(enum l2tp_profile_type type, const char *name)
{
	int result = -EINVAL;

	switch (type) {
	case L2TP_PROFILE_TYPE_PEER:
		result = l2tp_statusfile_file_delete(L2TP_STATUSFILE_DIR, "peer-profiles", name);
		break;
	case L2TP_PROFILE_TYPE_TUNNEL:
		result = l2tp_statusfile_file_delete(L2TP_STATUSFILE_DIR, "tunnel-profiles", name);
		break;
	case L2TP_PROFILE_TYPE_SESSION:
		result = l2tp_statusfile_file_delete(L2TP_STATUSFILE_DIR, "session-profiles", name);
		break;
	case L2TP_PROFILE_TYPE_PPP:
		result = l2tp_statusfile_file_delete(L2TP_STATUSFILE_DIR, "ppp-profiles", name);
		break;
	/* NODEFAULT */
	}

	return 0;
}

static int l2tp_statusfile_profile_modified_ind(enum l2tp_profile_type type, const char *name)
{
	int result;

	(void) l2tp_statusfile_profile_deleted_ind(type, name);
	result = l2tp_statusfile_profile_created_ind(type, name);

	return result;
}

static int l2tp_statusfile_tunnel_created_ind(uint16_t tunnel_id)
{
	int result = 0;
	char name[16];
	char dir[32];
	FILE *file;
	struct l2tp_api_tunnel_msg_data tunnel;
	optstring tunnel_name = { 0, };

	if (l2tp_old_tunnel_created_hook != NULL) {
		result = (*l2tp_old_tunnel_created_hook)(tunnel_id);
		if (result < 0) {
			goto out;
		}
	}

	sprintf(&name[0], "%hu", tunnel_id);
	sprintf(&dir[0], "tunnels/%hu", tunnel_id);
	(void) l2tp_statusfile_dir_create("tunnels", NULL); 
	(void) l2tp_statusfile_dir_create("tunnels", name); 
	file = l2tp_statusfile_file_create(dir, "status");

	if (file == NULL) {
		result = -errno;
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
		goto out;
	}

	memset(&tunnel, 0, sizeof(tunnel));
	if (l2tp_tunnel_get_1_svc(tunnel_id, tunnel_name, &tunnel, NULL)) {
		l2tp_show_tunnel(file, &tunnel, 1, 0);
	}
	fclose(file);
out:
	return result;
}

static int l2tp_statusfile_tunnel_deleted_ind(uint16_t tunnel_id)
{
	int result;
	char name[16];

	if (l2tp_old_tunnel_deleted_hook != NULL) {
		result = (*l2tp_old_tunnel_deleted_hook)(tunnel_id);
		if (result < 0) {
			goto out;
		}
	}

	sprintf(&name[0], "%hu", tunnel_id);
	result = l2tp_statusfile_dir_delete(L2TP_STATUSFILE_DIR, "tunnels", name, 1);
	(void) l2tp_statusfile_dir_delete(L2TP_STATUSFILE_DIR, "tunnels", NULL, 0); 

	if (result < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

out:
	return result;
}

static int l2tp_statusfile_tunnel_modified_ind(uint16_t tunnel_id)
{
	int result;

	if (l2tp_old_tunnel_modified_hook != NULL) {
		result = (*l2tp_old_tunnel_modified_hook)(tunnel_id);
		if (result < 0) {
			goto out;
		}
	}

	(void) l2tp_statusfile_tunnel_deleted_ind(tunnel_id);
	result = l2tp_statusfile_tunnel_created_ind(tunnel_id);

	if (result < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

out:
	return result;
}

static int l2tp_statusfile_tunnel_up_ind(uint16_t tunnel_id, uint16_t peer_tunnel_id)
{
	int result;

	if (l2tp_old_tunnel_up_hook != NULL) {
		result = (*l2tp_old_tunnel_up_hook)(tunnel_id, peer_tunnel_id);
		if (result < 0) {
			goto out;
		}
	}

	(void) l2tp_statusfile_tunnel_deleted_ind(tunnel_id);
	result = l2tp_statusfile_tunnel_created_ind(tunnel_id);

	if (result < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

out:
	return result;
}

static int l2tp_statusfile_tunnel_down_ind(uint16_t tunnel_id)
{
	int result;

	if (l2tp_old_tunnel_down_hook != NULL) {
		result = (*l2tp_old_tunnel_down_hook)(tunnel_id);
		if (result < 0) {
			goto out;
		}
	}

	(void) l2tp_statusfile_tunnel_deleted_ind(tunnel_id);
	result = l2tp_statusfile_tunnel_created_ind(tunnel_id);

	if (result < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

out:
	return result;
}

static int l2tp_statusfile_session_created_ind(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	int result = 0;
	char name[32];
	char dir1[32];
	char dir2[32];
	FILE *file;
	struct l2tp_api_session_msg_data sess;
	optstring tunnel_name = { 0, };
	optstring session_name = { 0, };
	
	if (l2tp_old_session_created_hook != NULL) {
		result = (*l2tp_old_session_created_hook)(session, tunnel_id, session_id);
		if (result < 0) {
			goto out;
		}
	}

	sprintf(&dir1[0], "tunnels/%hu", tunnel_id);
	sprintf(&dir2[0], "tunnels/%hu/sessions", tunnel_id);
	sprintf(&name[0], "tunnels/%hu/sessions/%hu", tunnel_id, session_id);
	(void) l2tp_statusfile_dir_create("tunnels", NULL); 
	(void) l2tp_statusfile_dir_create(dir1, NULL); 
	(void) l2tp_statusfile_dir_create(dir2, NULL); 
	(void) l2tp_statusfile_dir_create(name, NULL);
	file = l2tp_statusfile_file_create(name, "status");

	if (file == NULL) {
		result = -errno;
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

	memset(&sess, 0, sizeof(sess));
	if (l2tp_session_get_1_svc(tunnel_id, tunnel_name, session_id, session_name, &sess, NULL)) {
		l2tp_show_session(file, &sess);
	}

	fclose(file);

out:
	return result;
}

static int l2tp_statusfile_session_deleted_ind(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	int result;
	char name[32];
	char dir[32];

	if (l2tp_old_session_deleted_hook != NULL) {
		result = (*l2tp_old_session_deleted_hook)(session, tunnel_id, session_id);
		if (result < 0) {
			goto out;
		}
	}

	sprintf(&dir[0], "%hu/sessions", tunnel_id);
	sprintf(&name[0], "%hu/sessions/%hu", tunnel_id, session_id);
	result = l2tp_statusfile_dir_delete(L2TP_STATUSFILE_DIR, "tunnels", name, 1);
	(void) l2tp_statusfile_dir_delete(L2TP_STATUSFILE_DIR, "tunnels", dir, 0); 

	if (result < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

out:
	return result;
}

static int l2tp_statusfile_session_modified_ind(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	int result;

	if (l2tp_old_session_modified_hook != NULL) {
		result = (*l2tp_old_session_modified_hook)(session, tunnel_id, session_id);
		if (result < 0) {
			goto out;
		}
	}

	(void) l2tp_statusfile_session_deleted_ind(session, tunnel_id, session_id);
	result = l2tp_statusfile_session_created_ind(session, tunnel_id, session_id);

	if (result < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

out:
	return result;
}

static int l2tp_statusfile_session_up_ind(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id, 
					  uint16_t peer_tunnel_id, uint16_t peer_session_id)
{
	int result;

	if (l2tp_old_session_up_hook != NULL) {
		result = (*l2tp_old_session_up_hook)(session, tunnel_id, session_id, peer_tunnel_id, peer_session_id);
		if (result < 0) {
			goto out;
		}
	}

	(void) l2tp_statusfile_session_deleted_ind(session, tunnel_id, session_id);
	result = l2tp_statusfile_session_created_ind(session, tunnel_id, session_id);

	if (result < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

out:
	return result;
}

static int l2tp_statusfile_session_down_ind(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	int result;

	if (l2tp_old_session_down_hook != NULL) {
		result = (*l2tp_old_session_down_hook)(session, tunnel_id, session_id);
		if (result < 0) {
			goto out;
		}
	}

	(void) l2tp_statusfile_session_deleted_ind(session, tunnel_id, session_id);
	result = l2tp_statusfile_session_created_ind(session, tunnel_id, session_id);

	if (result < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

out:
	return result;
}

static int l2tp_statusfile_peer_created_ind(struct in_addr src, struct in_addr dest)
{
	int result = 0;
	char name[32];
	char srcip[16];
	char destip[16];
	FILE *file;
	char *ip;
	struct l2tp_api_peer_msg_data peer;
	struct l2tp_api_ip_addr peer_addr = { src.s_addr, };
	struct l2tp_api_ip_addr local_addr = { dest.s_addr, };
	
	if (l2tp_old_peer_created_hook != NULL) {
		result = (*l2tp_old_peer_created_hook)(src, dest);
		if (result < 0) {
			goto out;
		}
	}

	ip = inet_ntoa(src);
	strcpy(&srcip[0], ip);
	ip = inet_ntoa(dest);
	strcpy(&destip[0], ip);
	sprintf(&name[0], "%s-%s", srcip, destip);
	(void) l2tp_statusfile_dir_create("peers", NULL); 
	file = l2tp_statusfile_file_create("peers", name);

	if (file == NULL) {
		result = -errno;
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

	memset(&peer, 0, sizeof(peer));
	if (l2tp_peer_get_1_svc(local_addr, peer_addr, &peer, NULL)) {
		l2tp_show_peer(file, &peer);
	}

	fclose(file);

out:
	return result;
}

static int l2tp_statusfile_peer_deleted_ind(struct in_addr src, struct in_addr dest)
{
	int result;
	char name[32];
	char srcip[16];
	char destip[16];
	char *ip;

	if (l2tp_old_peer_deleted_hook != NULL) {
		result = (*l2tp_old_peer_deleted_hook)(src, dest);
		if (result < 0) {
			goto out;
		}
	}

	ip = inet_ntoa(src);
	strcpy(&srcip[0], ip);
	ip = inet_ntoa(dest);
	strcpy(&destip[0], ip);
	sprintf(&name[0], "%s-%s", srcip, destip);
	result = l2tp_statusfile_file_delete(L2TP_STATUSFILE_DIR, "peers", name);
	(void) l2tp_statusfile_dir_delete(L2TP_STATUSFILE_DIR, "peers", NULL, 0); 

	if (result < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: result=%d", __func__, result);
	}

out:
	return result;
}


/*****************************************************************************
 * Module init
 *****************************************************************************/

void l2tp_statusfile_init(void)
{
	int result;
	FILE *file;
	struct l2tp_api_app_msg_data app;
	struct l2tp_api_system_msg_data sys;

	l2tp_old_peer_created_hook = l2tp_peer_created_hook;
	l2tp_peer_created_hook = l2tp_statusfile_peer_created_ind;
	l2tp_old_peer_deleted_hook = l2tp_peer_deleted_hook;
	l2tp_peer_deleted_hook = l2tp_statusfile_peer_deleted_ind;
	l2tp_old_profile_created_hook = l2tp_profile_created_hook;
	l2tp_profile_created_hook = l2tp_statusfile_profile_created_ind;
	l2tp_old_profile_deleted_hook = l2tp_profile_deleted_hook;
	l2tp_profile_deleted_hook = l2tp_statusfile_profile_deleted_ind;
	l2tp_old_profile_modified_hook = l2tp_profile_modified_hook;
	l2tp_profile_modified_hook = l2tp_statusfile_profile_modified_ind;
	l2tp_old_tunnel_created_hook = l2tp_tunnel_created_hook;
	l2tp_tunnel_created_hook = l2tp_statusfile_tunnel_created_ind;
	l2tp_old_tunnel_deleted_hook = l2tp_tunnel_deleted_hook;
	l2tp_tunnel_deleted_hook = l2tp_statusfile_tunnel_deleted_ind;
	l2tp_old_tunnel_modified_hook = l2tp_tunnel_modified_hook;
	l2tp_tunnel_modified_hook = l2tp_statusfile_tunnel_modified_ind;
	l2tp_old_tunnel_up_hook = l2tp_tunnel_up_hook;
	l2tp_tunnel_up_hook = l2tp_statusfile_tunnel_up_ind;
	l2tp_old_tunnel_down_hook = l2tp_tunnel_down_hook;
	l2tp_tunnel_down_hook = l2tp_statusfile_tunnel_down_ind;
	l2tp_old_session_created_hook = l2tp_session_created_hook;
	l2tp_session_created_hook = l2tp_statusfile_session_created_ind;
	l2tp_old_session_deleted_hook = l2tp_session_deleted_hook;
	l2tp_session_deleted_hook = l2tp_statusfile_session_deleted_ind;
	l2tp_old_session_modified_hook = l2tp_session_modified_hook;
	l2tp_session_modified_hook = l2tp_statusfile_session_modified_ind;
	l2tp_old_session_up_hook = l2tp_session_up_hook;
	l2tp_session_up_hook = l2tp_statusfile_session_up_ind;
	l2tp_old_session_down_hook = l2tp_session_down_hook;
	l2tp_session_down_hook = l2tp_statusfile_session_down_ind;

	result = mkdir(L2TP_STATUSFILE_DIR, 0755);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to create %s", L2TP_STATUSFILE_DIR);
		exit(1);
	}

	/* Create entry for system info */
	file = l2tp_statusfile_file_create("system", NULL);
	if (file == NULL) {
		l2tp_log(LOG_ERR, "unable to create %s", L2TP_STATUSFILE_DIR "/system");
		exit(1);
	}

	if (l2tp_app_info_get_1_svc(&app, NULL)) {
		l2tp_show_app_version(file, &app);
	}
	fprintf(file, "\n");
	if (l2tp_system_get_1_svc(&sys, NULL)) {
		l2tp_show_system_config(file, &sys);
	}
	fclose(file);

	/* Create entries for the default profiles */
	result = l2tp_statusfile_profile_created_ind(L2TP_PROFILE_TYPE_PEER, 
						     L2TP_API_PEER_PROFILE_DEFAULT_PROFILE_NAME);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to create %s", L2TP_STATUSFILE_DIR "/peer-profiles/"
			 L2TP_API_PEER_PROFILE_DEFAULT_PROFILE_NAME);
		exit(1);
	}
	result = l2tp_statusfile_profile_created_ind(L2TP_PROFILE_TYPE_TUNNEL, 
						     L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to create %s", L2TP_STATUSFILE_DIR "/tunnel-profiles/"
			 L2TP_API_TUNNEL_PROFILE_DEFAULT_PROFILE_NAME);
		exit(1);
	}
	result = l2tp_statusfile_profile_created_ind(L2TP_PROFILE_TYPE_SESSION, 
						     L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to create %s", L2TP_STATUSFILE_DIR "/session-profiles/"
			 L2TP_API_SESSION_PROFILE_DEFAULT_PROFILE_NAME);
		exit(1);
	}
	result = l2tp_statusfile_profile_created_ind(L2TP_PROFILE_TYPE_PPP, 
						     L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME);
	if (result < 0) {
		l2tp_log(LOG_ERR, "unable to create %s", L2TP_STATUSFILE_DIR "/ppp-profiles/"
			 L2TP_API_PPP_PROFILE_DEFAULT_PROFILE_NAME);
		exit(1);
	}
}

void l2tp_statusfile_cleanup(void)
{
	l2tp_peer_created_hook = l2tp_old_peer_created_hook;
	l2tp_peer_deleted_hook = l2tp_old_peer_deleted_hook;
	l2tp_profile_created_hook = l2tp_old_profile_created_hook;
	l2tp_profile_deleted_hook = l2tp_old_profile_deleted_hook;
	l2tp_profile_modified_hook = l2tp_old_profile_modified_hook;
	l2tp_tunnel_created_hook = l2tp_old_tunnel_created_hook;
	l2tp_tunnel_deleted_hook = l2tp_old_tunnel_deleted_hook;
	l2tp_tunnel_modified_hook = l2tp_old_tunnel_modified_hook;
	l2tp_tunnel_up_hook = l2tp_old_tunnel_up_hook;
	l2tp_tunnel_down_hook = l2tp_old_tunnel_down_hook;
	l2tp_session_created_hook = l2tp_old_session_created_hook;
	l2tp_session_deleted_hook = l2tp_old_session_deleted_hook;
	l2tp_session_modified_hook = l2tp_old_session_modified_hook;
	l2tp_session_up_hook = l2tp_old_session_up_hook;
	l2tp_session_down_hook = l2tp_old_session_down_hook;

	(void) l2tp_statusfile_dir_delete(NULL, NULL, NULL, 1);
}
