/*****************************************************************************
 * Copyright (C) 2004,2005,2006,2007,2008 Katalix Systems Ltd
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 *
 *****************************************************************************/

/*
 * File descriptor callback event scheduling.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <syslog.h>
#include <netdb.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/time.h>
#include <sys/select.h>

#include "usl.h"

/* Each registered fd has a user callback to be called when the fd
 * has data ready. This struct is that context.
 */
struct fd_entry {
	int			fd;
	void			(*callback)(int fd, void *arg);
	void			*arg;
	struct usl_list_head	list;
};

static USL_LIST_HEAD(usl_fd_idle_list);
static USL_LIST_HEAD(usl_fd_active_list);

static int		usl_fd_count = 0;
static fd_set		usl_fd_poll_set;

void (*usl_fd_poll_hook)(fd_set *fds);

/* Search both idle and active fd lists for the fd entry 
 */
static struct fd_entry *usl_fd_find(int fd)
{
	struct fd_entry *entry = NULL;
	struct usl_list_head *walk;
	struct usl_list_head *tmp;

	/* Find the fd entry */
	usl_list_for_each(walk, tmp, &usl_fd_idle_list) {
		entry = usl_list_entry(walk, struct fd_entry, list);
		if (entry->fd == fd) {
			return entry;
		}
	}

	usl_list_for_each(walk, tmp, &usl_fd_active_list) {
		entry = usl_list_entry(walk, struct fd_entry, list);
		if (entry->fd == fd) {
			return entry;
		}
	}

	return NULL;
}

/* Called to rebuild the fd_set used by usl_fd_poll() if one or more
 * file descriptors are closed before they are unregistered.
 */
static void usl_fd_update_poll_set(void)
{
	struct fd_entry *entry = NULL;
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	int fd;
	struct stat stat_buf;
	int max_fd = 0;

	FD_ZERO(&usl_fd_poll_set);
	usl_list_for_each(walk, tmp, &usl_fd_idle_list) {
		entry = usl_list_entry(walk, struct fd_entry, list);
		fd = entry->fd;
		if (fstat(fd, &stat_buf) == 0) {
			FD_SET(fd, &usl_fd_poll_set);
			if (max_fd < fd) {
				max_fd = fd;
			}
		} else {
			usl_list_del(&entry->list);
			USL_POISON_MEMORY(entry, 0xf2, sizeof(*entry));
			free(entry);
		}
	}
	usl_list_for_each(walk, tmp, &usl_fd_active_list) {
		entry = usl_list_entry(walk, struct fd_entry, list);
		fd = entry->fd;
		if (fstat(fd, &stat_buf) == 0) {
			FD_SET(fd, &usl_fd_poll_set);
			if (max_fd < fd) {
				max_fd = fd;
			}
		} else {
			usl_list_del(&entry->list);
			USL_POISON_MEMORY(entry, 0xf2, sizeof(*entry));
			free(entry);
		}
	}

	usl_fd_count = max_fd + 1;
}

/* Called to register a function to be called when a message is
 * received on the specified file descriptor. One callback may be
 * registered per fd.
 */
int usl_fd_add_fd(int fd, usl_fd_callback_fn_t callback, void *arg)
{
	struct fd_entry *entry;

	/* Check that the fd isn't already registsred. */
	entry = usl_fd_find(fd);
	if (entry != NULL) {
		return -EEXIST;
	}

	entry = (struct fd_entry *) calloc(1,  sizeof(struct fd_entry));
	if (entry == NULL) {
		return -ENOMEM;
	}

	USL_DEBUG("%s: fd=%d func=%p arg=%p", __func__, fd, callback, arg);

	/* Keep track of the max fd in use for select() */
	if (fd >= usl_fd_count) {
		usl_fd_count = fd+1;
	}

	FD_SET(fd, &usl_fd_poll_set);
	entry->fd = fd;
	entry->callback = callback;
	entry->arg = arg;
	USL_LIST_HEAD_INIT(&entry->list);
	usl_list_add(&entry->list, &usl_fd_idle_list);

	return 0;
}

/* Change the callback or callback arg of a registered fd. 
 */
int usl_fd_modify_fd(int fd, usl_fd_callback_fn_t callback, void *arg)
{
	struct fd_entry *entry;

	entry = usl_fd_find(fd);
	if (entry == NULL) {
		USL_SYSLOG(LOG_ERR, "Modify: fd %d not found", fd);
		return -ENOENT;
	}

	USL_DEBUG("%s: fd=%d func=%p arg=%p", __func__, fd, callback, arg);

	entry->callback = callback;
	entry->arg = arg;

	return 0;
}

/* Remove a previously registered callback for the specified file
 * descriptor.
 */
int usl_fd_remove_fd(int fd)
{
	struct fd_entry *entry;

	entry = usl_fd_find(fd);
	if (entry == NULL) {
		USL_SYSLOG(LOG_ERR, "Remove: fd %d not found", fd);
		return -ENOENT;
	}

	USL_DEBUG("%s: fd=%d", __func__, fd);

	FD_CLR(fd,  &usl_fd_poll_set);

	usl_list_del(&entry->list);
	USL_POISON_MEMORY(entry, 0xf2, sizeof(*entry));
	free(entry);

	return 0;
}

/* Poll all fd's which have registered callbacks for work to do and
 * dispatch each in turn. Blocks until an fd is ready.
 */
void usl_fd_poll(void)
{
	struct fd_entry *entry = NULL;
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	fd_set r_poll;
	int num;

	while (usl_list_empty(&usl_fd_active_list)) {
		/* No active fd's, so block until one does become active */
		r_poll = usl_fd_poll_set;
		num = select(usl_fd_count+1, &r_poll, NULL, NULL, NULL);

		if (num < 0) {
			if (errno == EINTR) {
				return;
			}
			if (errno == EBADF) {
				usl_fd_update_poll_set();
				return;
			}
			USL_SYSLOG(LOG_ERR, "%s: select", strerror(errno));
			continue;
		}

		/* Find entries in the idle_fd list that have their fd bit set.
		 * For each one, move its fd_entry to the active list.
		 */
		usl_list_for_each(walk, tmp, &usl_fd_idle_list) {
			entry = usl_list_entry(walk, struct fd_entry, list);
			if (!FD_ISSET(entry->fd, &r_poll)) {
				continue;
			}
			usl_list_move(&entry->list, &usl_fd_active_list);
		}
	}

	/* There are one or more entries in the active list.  Take an
	 * entry off the active list and call its registered callback.
	 * Move the entry back to the idle list to let another fd have
	 * a go next time.
	 */
	walk = usl_fd_active_list.next;
	entry = usl_list_entry(walk, struct fd_entry, list);
	usl_list_move(&entry->list, &usl_fd_idle_list);
	(*entry->callback)(entry->fd, entry->arg);

	/* Let an application do stuff on every event */
	if (usl_fd_poll_hook != NULL) {
		(*usl_fd_poll_hook)(&usl_fd_poll_set);
	}
}

/* Read a full `count' bytes from fd, unless end-of-file or an error
 * other than EINTR is encountered. Copied from pppd sources.
 */
size_t usl_fd_read(int fd, void *buf, size_t count)
{
	size_t chars_read;
	char *ptr = buf;

	for (chars_read = 0; chars_read < count; ) {
		size_t nb = read(fd, ptr, count - chars_read);
		if (nb < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		if (nb == 0)
			break;
		chars_read += nb;
		ptr += nb;
	}
	return chars_read;
}

/*****************************************************************************
 * Init and cleanup
 *****************************************************************************/

int usl_fd_init(void)
{
	FD_ZERO(&usl_fd_poll_set);
	USL_LIST_HEAD_INIT(&usl_fd_idle_list);
	USL_LIST_HEAD_INIT(&usl_fd_active_list);

	usl_fd_poll_hook = NULL;

	return 0;
}

void usl_fd_cleanup(void)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct fd_entry *entry;

	usl_list_for_each(walk, tmp, &usl_fd_idle_list) {
		entry = usl_list_entry(walk, struct fd_entry, list);
		usl_list_del(&entry->list);
		USL_POISON_MEMORY(entry, 0xf2, sizeof(*entry));
		free(entry);
	}
	usl_list_for_each(walk, tmp, &usl_fd_active_list) {
		entry = usl_list_entry(walk, struct fd_entry, list);
		usl_list_del(&entry->list);
		USL_POISON_MEMORY(entry, 0xf2, sizeof(*entry));
		free(entry);
	}
}

