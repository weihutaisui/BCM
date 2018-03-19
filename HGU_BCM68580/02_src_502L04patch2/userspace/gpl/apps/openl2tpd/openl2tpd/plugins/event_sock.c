#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "usl.h"
#include "event_sock.h"
#include "l2tp_private.h"

#define MAX_CLIENTS 5
static int num_clients;

struct event_sock_client {
	int sockfd;
	struct sockaddr_un remote;
	socklen_t remote_size;
	struct usl_list_head list;
	uint16_t event_mask;
};

static USL_LIST_HEAD(event_sock_client_list);

/* Used to record previous value of hook so that we can chain them */
static int (*event_sock_old_session_created_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id) = NULL;
static int (*event_sock_old_session_deleted_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id) = NULL;
static int (*event_sock_old_session_up_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id, uint16_t peer_tunnel_id, uint16_t peer_session_id) = NULL;
static int (*event_sock_old_session_down_hook)(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id) = NULL;

static void event_sock_send_to_clients(struct l2tp_event_hdr *event)
{
	int bytes = event->len + sizeof(*event);
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	struct event_sock_client *clnt;

	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	/* Send event to all subscribed clients */
	usl_list_for_each(walk, tmp, &event_sock_client_list) {
		clnt = usl_list_entry(walk, struct event_sock_client, list);
		if (clnt->event_mask & event->event) {
			L2TP_DEBUG(L2TP_FUNC, "%s: sending %d bytes to "
				"socket %d", __func__, bytes, clnt->sockfd);
			send(clnt->sockfd, event, bytes, 0);
		}
	}
}

int event_sock_send_session_and_tunnel_event(uint16_t event_code, uint16_t tunnel_id, uint16_t session_id)
{
	int payload = sizeof(tunnel_id) + sizeof(session_id);
	uint8_t data[payload + sizeof(struct l2tp_event_hdr)];
	struct l2tp_event_hdr *event = (struct l2tp_event_hdr *)&data[0];
	uint8_t *dptr;

	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	event->event = event_code;
	event->len = payload;
	
	dptr = &event->data[0];
	*(uint16_t *)dptr = tunnel_id;
	dptr += sizeof(tunnel_id);
	*(uint16_t *)dptr = session_id;
	
	event_sock_send_to_clients(event);

	return 0;
}

int event_sock_session_down(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	return event_sock_send_session_and_tunnel_event(L2TP_EVENT_SESSION_DOWN, tunnel_id, session_id);
}

int event_sock_session_created(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	return event_sock_send_session_and_tunnel_event(L2TP_EVENT_SESSION_CREATED, tunnel_id, session_id);
}

int event_sock_session_deleted(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id)
{
	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	return event_sock_send_session_and_tunnel_event(L2TP_EVENT_SESSION_DELETED, tunnel_id, session_id);
}

int event_sock_session_up(struct l2tp_session const *session, uint16_t tunnel_id, uint16_t session_id, uint16_t peer_tunnel_id, uint16_t peer_session_id)
{
	int payload = sizeof(tunnel_id) + sizeof(session_id) +
	             sizeof(peer_tunnel_id) + sizeof(peer_session_id);
	uint8_t data[payload + sizeof(struct l2tp_event_hdr)];
	struct l2tp_event_hdr *event = (struct l2tp_event_hdr *)&data[0];
	uint8_t *dptr;

	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	event->event = L2TP_EVENT_SESSION_UP;
	event->len = payload;
	
	dptr = &event->data[0];
	*(uint16_t *)dptr = tunnel_id;
	dptr += sizeof(tunnel_id);
	*(uint16_t *)dptr = session_id;
	dptr += sizeof(session_id);
	*(uint16_t *)dptr = peer_tunnel_id;
	dptr += sizeof(peer_tunnel_id);
	*(uint16_t *)dptr = peer_session_id;
	
	event_sock_send_to_clients(event);

	return 0;
}

void event_sock_client_disconnect(struct event_sock_client *clnt)
{
	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	if (!clnt)
		goto out;

	/* Close socket and delete client context */
	num_clients--;
	usl_list_del(&clnt->list);
	usl_fd_remove_fd(clnt->sockfd);
	close(clnt->sockfd);
	free(clnt);

out:
	return;
}

void event_sock_client_cb(int fd, void *arg)
{
	struct event_sock_client *clnt = (struct event_sock_client *)arg;
	uint8_t data[sizeof(struct l2tp_event_hdr) + sizeof(uint8_t)];
	struct l2tp_event_hdr *event = (struct l2tp_event_hdr *)&data[0];
	bool done = false;
	uint8_t *eptr = (uint8_t *)event;
	int len, remaining = sizeof(data);
	
	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	if (!arg)
		goto out;

	/* Get message from client */
	while (!done) {
		len = recv(clnt->sockfd, eptr, remaining, 0); 
		if (len < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				break;
			}
		} else if (len == 0) {
			/* Client closed their socket delete context */
			event_sock_client_disconnect(clnt);
			goto out;
		} else {
			remaining -= len;
			if (!remaining)
				done = true;
		}
	}

	/* Only message accepted from clients is event subscribe/unsubscribe */
	if (event->data[0]) 
		clnt->event_mask |= event->event;
	else 
		clnt->event_mask &= ~(event->event);

	L2TP_DEBUG(L2TP_FUNC, "%s: client event mask 0x%04X", __func__,
		clnt->event_mask);

out:
	return;
}

void event_sock_client_connect_cb(int fd, void *arg) 
{
	struct event_sock_client *clnt;

	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	if (num_clients >= MAX_CLIENTS)
		goto out;

	/* Create the client context */
	clnt = malloc(sizeof(struct event_sock_client));
	if (!clnt) {
		L2TP_DEBUG(L2TP_FUNC, "%s: Insufficient memory", __func__);
		goto out;
	}
	USL_LIST_HEAD_INIT(&clnt->list);

	/* Accept the connection */
	clnt->remote_size = sizeof(clnt->remote);
	clnt->sockfd = accept(fd, (struct sockaddr *)&clnt->remote,
				&clnt->remote_size);
	if (clnt->sockfd < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: Failed accepting client connect "
				"(%s)", __func__, strerror(errno));
		goto out_free;
	}
	num_clients++;

	usl_list_add(&clnt->list, &event_sock_client_list);
	usl_fd_add_fd(clnt->sockfd, event_sock_client_cb, clnt);

	return;

out_free:
	free(clnt);
out:
	return;
}

/*****************************************************************************
 * L2TP plugin interface
 *****************************************************************************/

const char openl2tp_plugin_version[] = "V1.0";

static int listen_sockfd;

int openl2tp_plugin_init(void)
{	
	struct sockaddr_un local;
	int len, ret;

	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	/* Remember the function pointers for the next plugin to call */
	event_sock_old_session_created_hook = l2tp_session_created_hook;
	event_sock_old_session_deleted_hook = l2tp_session_deleted_hook;
	event_sock_old_session_up_hook = l2tp_session_up_hook;
	event_sock_old_session_down_hook = l2tp_session_down_hook;

	/* Install our own function pointers */
	l2tp_session_created_hook = event_sock_session_created;
	l2tp_session_deleted_hook = event_sock_session_deleted;
	l2tp_session_up_hook = event_sock_session_up;
	l2tp_session_down_hook = event_sock_session_down;

	/* Create socket to listen for client connections */
	listen_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (listen_sockfd < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: Unable to open event interface "
			"listening socket (%s)", __func__, strerror(errno));
		goto out;
	}

	/* Bind listening socket to pipe named EVENT_SOCK_NAMED_PIPE */
	unlink(EVENT_SOCK_NAMED_PIPE);
	memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
	strncpy((char *)&local.sun_path, EVENT_SOCK_NAMED_PIPE,
		sizeof(local.sun_path) - 1);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	ret = bind(listen_sockfd, (struct sockaddr *)&local, len);
	if (ret < 0) {
		L2TP_DEBUG(L2TP_FUNC, "%s: Unable to bind event interface "
			"listening socket (%s)", __func__, strerror(errno));
		goto out;
	}
	listen(listen_sockfd, MAX_CLIENTS);

	/* Add client connect callback */
	usl_fd_add_fd(listen_sockfd, event_sock_client_connect_cb, NULL);

out:
	return 0;
}

void openl2tp_plugin_cleanup(void)
{
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	struct event_sock_client *clnt;

	L2TP_DEBUG(L2TP_FUNC, "%s: enter", __func__);

	l2tp_session_created_hook = event_sock_old_session_created_hook;
	l2tp_session_deleted_hook = event_sock_old_session_deleted_hook;
	l2tp_session_up_hook = event_sock_old_session_up_hook;
	l2tp_session_down_hook = event_sock_old_session_down_hook;

	/* Stop listening for new connections */
	usl_fd_remove_fd(listen_sockfd);
	close(listen_sockfd);
	unlink(EVENT_SOCK_NAMED_PIPE);

	/* For each client, close it's socket */
	usl_list_for_each(walk, tmp, &event_sock_client_list) {
		clnt = usl_list_entry(walk, struct event_sock_client, list);
		event_sock_client_disconnect(clnt);
	}
}

