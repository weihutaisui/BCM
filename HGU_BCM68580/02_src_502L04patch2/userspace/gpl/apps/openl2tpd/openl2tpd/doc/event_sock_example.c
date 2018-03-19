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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "event_sock.h"

int open_named_pipe(const char *name)
{
	int sfd, len, ret;
	struct sockaddr_un remote;

	sfd = socket(AF_UNIX, SOCK_STREAM, 0);	

	memset(&remote, 0, sizeof(remote));
	remote.sun_family = AF_UNIX;
	strncpy((char *)&remote.sun_path, name, sizeof(remote.sun_path) - 1);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	ret = connect(sfd, (struct sockaddr *)&remote, len);
	if (ret < 0) {
		fprintf(stderr, "Unable to open named pipe '%s' (%s)\n",
			name, strerror(errno));
		close(sfd);
		sfd = -1;
	}

	return sfd;
}

int enable_all_events(int sfd)
{
	uint8_t data[sizeof(struct l2tp_event) + sizeof(uint8_t)];
	struct l2tp_event *event = (struct l2tp_event *)&data[0];

	event->event = (L2TP_EVENT_SESSION_CREATED | L2TP_EVENT_SESSION_DELETED
			| L2TP_EVENT_SESSION_UP | L2TP_EVENT_SESSION_DOWN);
	event->len = 1;
	event->data[0] = 1;

	send(sfd, event, sizeof(data), 0);

	return EXIT_SUCCESS;
}

static char *event_string(uint16_t event)
{
	switch (event) {
		case L2TP_EVENT_SESSION_CREATED:
			return "session created";
		case L2TP_EVENT_SESSION_DELETED:
			return "session deleted";
		case L2TP_EVENT_SESSION_UP:
			return "session up";
		case L2TP_EVENT_SESSION_DOWN:
			return "session down";
		default :
			return "unknown";
	}

	return "";
}

int read_event(int sfd)
{
	uint8_t data[sizeof(struct l2tp_event) + 8];
	struct l2tp_event *event = (struct l2tp_event *)&data[0];
	bool done = false;
	uint8_t *eptr = (uint8_t *)event;
	int len, remaining = sizeof(*event);

	/* Get header from client */
	while (!done) {
		len = recv(sfd, eptr, remaining, 0); 
		if (len < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				break;
			}
		} else if (len == 0) {
			return -ENOTCONN;
		} else {
			remaining -= len;
			eptr += len;
			if (!remaining)
				done = true;
		}
	}

	/* Get rest of message */
	remaining = event->len;
	done = false;
	while (!done) {
		len = recv(sfd, eptr, remaining, 0); 
		if (len < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				break;
			}
		} else if (len == 0) {
			return -ENOTCONN;
		} else {
			remaining -= len;
			if (!remaining)
				done = true;
		}
	}

	fprintf(stdout, "Received %s event and %d bytes\n",
		event_string(event->event),
		event->len);

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	int sfd;
	fd_set rfds;
	int ret;

	sfd = open_named_pipe(EVENT_SOCK_NAMED_PIPE);
	if (sfd < 0)
		goto error;

	enable_all_events(sfd);

	FD_ZERO(&rfds);
	FD_SET(sfd, &rfds);

	while (1) {
		ret = select(sfd + 1, &rfds, NULL, NULL, NULL);
		if (ret == -1) {
			perror("select");
		} else {
			ret = read_event(sfd);
			if (ret == -ENOTCONN)
				goto out;
		}
	}

out:
	close(sfd);

	return EXIT_SUCCESS;
error:
	return EXIT_FAILURE;
}
