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

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>

#include "usl.h"
#include "l2tp_private.h"

#define IPSEC_SETKEY_CMD	"/sbin/setkey"
#define IPSEC_SETKEY_FILE	"/tmp/openl2tpd-tmp"
#define IPSEC_SETKEY_ACTION	IPSEC_SETKEY_CMD " -f " IPSEC_SETKEY_FILE

/* We keep a list of every SPD entry that we install */
struct ipsec_spd {
	struct usl_list_head		list;
	struct sockaddr_in		src;
	struct sockaddr_in		dest;
	const struct l2tp_tunnel	*tunnel;
};

/*
 * Plugin to interface to IPSec subsystem.
 */

static void (*old_l2tp_net_socket_create_hook)(const struct l2tp_tunnel *tunnel, struct sockaddr_in *src) = NULL;
static void (*old_l2tp_net_socket_connect_hook)(const struct l2tp_tunnel *tunnel, int fd, struct sockaddr_in *src, struct sockaddr_in *dest) = NULL;
static void (*old_l2tp_net_socket_disconnect_hook)(const struct l2tp_tunnel *tunnel, struct sockaddr_in *src, struct sockaddr_in *dest) = NULL;

static USL_LIST_HEAD(ipsec_spd_list);

static struct ipsec_spd *ipsec_spd_find(struct sockaddr_in *src, struct sockaddr_in *dest)
{
	struct ipsec_spd *spd;
	struct usl_list_head *tmp;
	struct usl_list_head *walk;

	usl_list_for_each(walk, tmp, &ipsec_spd_list) {
		spd = usl_list_entry(walk, struct ipsec_spd, list);
		if ((src->sin_addr.s_addr != spd->src.sin_addr.s_addr) ||
		    (src->sin_port != spd->src.sin_port)) {
			continue;
		}
		if (dest != NULL) {
			if ((dest->sin_addr.s_addr != spd->dest.sin_addr.s_addr) ||
			    (dest->sin_port != spd->dest.sin_port)) {
				continue;
			}
		}
		return spd;
	}

	return NULL;
}

/*****************************************************************************
 * IPSec SPD database support routines.
 * We use the ipsec-tools setkey utility rather than dealing directly with
 * the PF_KEY messages for simplicity.
 *****************************************************************************/

static int ipsec_spd_delete(const struct l2tp_tunnel *tunnel, struct sockaddr_in *src, struct sockaddr_in *dest)
{
	int rc;
	struct ipsec_spd *spd;
	char server[16];
	char client[16];
	char *ip;
	FILE *f = fopen(IPSEC_SETKEY_FILE, "w");

	if (f == NULL) {
		rc = -errno;
		goto out;
	}

	ip = inet_ntoa(src->sin_addr);
	strcpy(server, ip);
	ip = inet_ntoa(dest->sin_addr);
	strcpy(client, ip);

	/* If the local L2TP client created the tunnel, delete the initial outbound SPD entry */
	fprintf(f, "spddelete -4n %s[%hu] 0.0.0.0/0[any] udp -P out;\n",
		server, ntohs(src->sin_port));
	L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: spddelete -4n %s[%hu] 0.0.0.0/0[any] udp -P out;",
		   __func__, l2tp_tunnel_id(tunnel), server, ntohs(src->sin_port));

	/* Now delete the actual SPD entries, one for each direction */
	fprintf(f, "spddelete -4n %s[%hu] %s[%hu] udp -P out;\n",
		server, ntohs(src->sin_port), client, ntohs(dest->sin_port));
	L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: spddelete -4n %s[%hu] %s[%hu] udp -P out;",
		   __func__, l2tp_tunnel_id(tunnel), server, ntohs(src->sin_port), client, ntohs(dest->sin_port));
	fprintf(f, "spddelete -4n %s[%hu] %s[%hu] udp -P in;\n",
		client, ntohs(dest->sin_port), server, ntohs(src->sin_port));
	L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: spddelete -4n %s[%hu] %s[%hu] udp -P in;",
		    __func__, l2tp_tunnel_id(tunnel), client, ntohs(dest->sin_port), server, ntohs(src->sin_port));
	fclose(f);

	rc = system(IPSEC_SETKEY_ACTION);
	if (rc != 0) {
		L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: spd_delete: %s: failed: rc=%d",  __func__,
			   l2tp_tunnel_id(tunnel), IPSEC_SETKEY_ACTION, rc);
		rc = -1;
	}

	spd = ipsec_spd_find(src, dest);
	if (spd != NULL) {
		usl_list_del(&spd->list);
		free(spd);
	}

	(void) unlink(IPSEC_SETKEY_FILE);

out:
	return rc;
}

static int ipsec_spd_add(const struct l2tp_tunnel *tunnel, struct sockaddr_in *src, struct sockaddr_in *dest)
{
	int rc = 0;
	struct ipsec_spd *spd;
	char server[16];
	char client[16];
	char *ip;
	FILE *f = fopen(IPSEC_SETKEY_FILE, "w");

	if (f == NULL) {
		rc = -errno;
		goto out;
	}

	ip = inet_ntoa(src->sin_addr);
	strcpy(server, ip);
	ip = inet_ntoa(dest->sin_addr);
	strcpy(client, ip);

	spd = calloc(1, sizeof(*spd));
	if (spd == NULL) {
		rc = -ENOMEM;
		goto out;
	}
	USL_LIST_HEAD_INIT(&spd->list);
	spd->src = *src;
	spd->dest = *dest;
	spd->tunnel = tunnel;
	usl_list_add(&spd->list, &ipsec_spd_list);

	if (l2tp_tunnel_is_created_by_admin(tunnel)) {
		/* If the local L2TP client created the tunnel, delete the initial outbound SPD entry */
		fprintf(f, "spddelete -4n %s[%hu] 0.0.0.0/0[any] udp -P out;\n",
			server, ntohs(src->sin_port));
		L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: spddelete -4n %s[%hu] 0.0.0.0/0[any] udp -P out;",
			   __func__, l2tp_tunnel_id(tunnel), server, ntohs(src->sin_port));
	}

	/* Now add the actual SPD entries, one for each direction */
	fprintf(f, "spdadd -4n %s[%hu] %s[%hu] udp -P out ipsec esp/transport//require;\n",
		server, ntohs(src->sin_port), client, ntohs(dest->sin_port));
	L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: spdadd -4n %s[%hu] %s[%hu] udp -P out ipsec esp/transport//require;",
		   __func__, l2tp_tunnel_id(tunnel), server, ntohs(src->sin_port), client, ntohs(dest->sin_port));

	fprintf(f, "spdadd -4n %s[%hu] %s[%hu] udp -P in ipsec esp/transport//require;\n",
		client, ntohs(dest->sin_port), server, ntohs(src->sin_port));
	L2TP_DEBUG(L2TP_PROTOCOL,
		   "%s: tunl %hu: spdadd -4n %s[%hu] %s[%hu] udp -P in ipsec esp/transport//require;",
		   __func__, l2tp_tunnel_id(tunnel), client, ntohs(dest->sin_port), server, ntohs(src->sin_port));
	fclose(f);

	rc = system(IPSEC_SETKEY_ACTION);
	if (rc != 0) {
		L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: spd_add: %s: failed: rc=%d", __func__,
			   l2tp_tunnel_id(tunnel), IPSEC_SETKEY_ACTION, rc);
		(void) ipsec_spd_delete(tunnel, src, dest);
		rc = -1;
	}

	(void) unlink(IPSEC_SETKEY_FILE);

	if (rc != 0) {
		usl_list_del(&spd->list);
		free(spd);
	}
out:
	return rc;
}

static int ipsec_spd_add_outbound(const struct l2tp_tunnel *tunnel, struct sockaddr_in *src)
{
	int rc = 0;
	char server[16];
	char *ip;
	FILE *f;

	if (l2tp_tunnel_is_created_by_admin(tunnel)) {
		f = fopen(IPSEC_SETKEY_FILE, "w");
		if (f == NULL) {
			rc = -errno;
			goto out;
		}

		ip = inet_ntoa(src->sin_addr);
		strcpy(server, ip);

		fprintf(f, "spdadd -4n %s[%hu] 0.0.0.0/0[any] udp -P out ipsec esp/transport//require;\n",
			server, ntohs(src->sin_port));
		L2TP_DEBUG(L2TP_PROTOCOL, 
			   "%s: tunl %hu: spdadd -4n %s[%hu] 0.0.0.0/0[any] udp -P out ipsec esp/transport//require;\n",
			   __func__, l2tp_tunnel_id(tunnel), server, ntohs(src->sin_port));
		fclose(f);

		rc = system(IPSEC_SETKEY_ACTION);
		if (rc != 0) {
			L2TP_DEBUG(L2TP_PROTOCOL, "%s: tunl %hu: spd_add_outbound: %s: failed: rc=%d", __func__,
				   l2tp_tunnel_id(tunnel), IPSEC_SETKEY_ACTION, rc);
			rc = -1;
		}

		(void) unlink(IPSEC_SETKEY_FILE);
	}

out:
	return rc;
}

/*****************************************************************************
 * IPSec hooks
 *****************************************************************************/

static void ipsec_net_socket_create(const struct l2tp_tunnel *tunnel, struct sockaddr_in *src)
{
	int rc;

	if (old_l2tp_net_socket_create_hook != NULL) {
		(*old_l2tp_net_socket_create_hook)(tunnel, src);
	}

	if (src->sin_addr.s_addr != INADDR_ANY) {
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, 
				"tunl %hu: setting up outbound ipsec SPD entry from %x/%hu", 
				l2tp_tunnel_id(tunnel),
				ntohl(src->sin_addr.s_addr), ntohs(src->sin_port));
	}

	rc = ipsec_spd_add_outbound(tunnel, src);
	if (rc < 0) {
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_WARNING, 
				"tunl %hu: failed to up outbound ipsec SPD entry from %x/%hu", 
				l2tp_tunnel_id(tunnel),
				ntohl(src->sin_addr.s_addr), ntohs(src->sin_port));
	}
}

static void ipsec_net_socket_connect(const struct l2tp_tunnel *tunnel, int fd, struct sockaddr_in *src, struct sockaddr_in *dest)
{
	int rc;

	if (old_l2tp_net_socket_connect_hook != NULL) {
		(*old_l2tp_net_socket_connect_hook)(tunnel, fd, src, dest);
	}

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "tunl %hu: setting up ipsec SPD entry for %x/%hu - %x/%hu",
			l2tp_tunnel_id(tunnel),
			ntohl(src->sin_addr.s_addr), ntohs(src->sin_port),
			ntohl(dest->sin_addr.s_addr), ntohs(dest->sin_port));

	rc = ipsec_spd_add(tunnel, src, dest);
	if (rc < 0) {
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_WARNING, 
				"tunl %hu: failed to set up ipsec SPD entry for %x/%hu - %x/%hu",
				l2tp_tunnel_id(tunnel),
				ntohl(src->sin_addr.s_addr), ntohs(src->sin_port),
				ntohl(dest->sin_addr.s_addr), ntohs(dest->sin_port));
	} else {
		/* connect() once more to force route and xfrm (IPSec) cache refresh.  
		 * NOTE: even if the call fails the socket stays connected only the 
		 * route cache is reset so we don't have to care about the actual result 
		 */
		(void) connect(fd, (struct sockaddr*) dest, sizeof(*dest));
	}
}

static void ipsec_net_socket_disconnect(const struct l2tp_tunnel *tunnel, struct sockaddr_in *src, struct sockaddr_in *dest)
{
	int rc;

	if (old_l2tp_net_socket_disconnect_hook != NULL) {
		(*old_l2tp_net_socket_disconnect_hook)(tunnel, src, dest);
	}

	l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_INFO, "tunl %hu: deleting ipsec SPD entry for %x/%hu - %x/%hu",
			l2tp_tunnel_id(tunnel),
			ntohl(src->sin_addr.s_addr), ntohs(src->sin_port),
			ntohl(dest->sin_addr.s_addr), ntohs(dest->sin_port));

	rc = ipsec_spd_delete(tunnel, src, dest);
	if (rc < 0) {
		l2tp_tunnel_log(tunnel, L2TP_PROTOCOL, LOG_WARNING, 
				"tunl %hu: failed to delete ipsec SPD entry for %x/%hu - %x/%hu",
				l2tp_tunnel_id(tunnel),
				ntohl(src->sin_addr.s_addr), ntohs(src->sin_port),
				ntohl(dest->sin_addr.s_addr), ntohs(dest->sin_port));
	}
}

/*****************************************************************************
 * L2TP plugin interface
 *****************************************************************************/

const char openl2tp_plugin_version[] = "V1.0";

int openl2tp_plugin_init(void)
{
	int rc;

	/* Check for setkey availability */
	rc = system(IPSEC_SETKEY_CMD " -nD");
	if (rc != 0) {
		l2tp_log(LOG_WARNING, "IPSec support disabled. No setkey found.");
		goto out;
	}

	/* Hook up our handlers of socket use */
	old_l2tp_net_socket_create_hook = l2tp_net_socket_create_hook;
	old_l2tp_net_socket_connect_hook = l2tp_net_socket_connect_hook;
	old_l2tp_net_socket_disconnect_hook = l2tp_net_socket_disconnect_hook;

	l2tp_net_socket_create_hook = ipsec_net_socket_create;
	l2tp_net_socket_connect_hook = ipsec_net_socket_connect;
	l2tp_net_socket_disconnect_hook = ipsec_net_socket_disconnect;

	l2tp_log(LOG_INFO, "L2TP/IPSec ephemeral port support enabled.");
out:
	return rc;
}

void openl2tp_plugin_cleanup(void)
{
	int rc;
	struct ipsec_spd *spd;
	struct usl_list_head *tmp;
	struct usl_list_head *walk;

	l2tp_net_socket_create_hook = old_l2tp_net_socket_create_hook;
	l2tp_net_socket_connect_hook = old_l2tp_net_socket_connect_hook;
	l2tp_net_socket_disconnect_hook = old_l2tp_net_socket_disconnect_hook;

	/* Cleanup SPD entries */
	usl_list_for_each(walk, tmp, &ipsec_spd_list) {
		spd = usl_list_entry(walk, struct ipsec_spd, list);
		rc = ipsec_spd_delete(spd->tunnel, &spd->src, &spd->dest);
		if (rc < 0) {
			syslog(LOG_WARNING, "Failed to cleanup ipsec SPD entry for %x/%hu - %x/%hu",
			       ntohl(spd->src.sin_addr.s_addr), ntohs(spd->src.sin_port),
			       ntohl(spd->dest.sin_addr.s_addr), ntohs(spd->dest.sin_port));
		}
	}

	/* Just in case the app was killed while it was using setkey */
	(void) unlink(IPSEC_SETKEY_FILE);
}

