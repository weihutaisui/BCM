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
 * Timer implementation.
 * Timers are synchronous - their handlers are invoked outside interrupt
 * context, which avoids having user code use locking around critical
 * regions. 
 *
 * FIXME: does not handle timer tick wrap. Tick will wrap in ~300000 hours
 * when USL_TIMER_HZ==4.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <syslog.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>

#include "usl.h"

struct usl_timer
{
#ifdef DEBUG
#define USL_TIMER_MAGIC			0x4b414c58
	int				magic;
	void				*creator;
#endif
	int				running;
	int				deleting;
	unsigned int			start_expiry;
	unsigned int			expiry;
	unsigned int			start_interval;
	unsigned int			interval;
	usl_timer_callback_fn_t		callback;
	void 				*callback_arg;
	usl_timer_interval_adjust_fn_t	adjuster;
	struct usl_ord_list_head 	list;
	struct usl_list_head 		expire_list;
};

static int usl_timer_cmp(void *t1, void *t2, int cmp_len);
static void usl_timer_delete_1(struct usl_timer *tim);

/* We keep an ordered list of timers */
static USL_ORD_LIST_HEAD(usl_timer_list, usl_timer_cmp, NULL, 0);

static struct itimerval 		usl_itimval;
static volatile unsigned int		usl_tick;
static volatile int			usl_tick_pending;
static int				usl_tick_pipe[2];

/* A hook to allow apps to do something on every timer tick */
void (*usl_timer_tick_hook)(void);

/* Come here every interval timer tick.
 * Timers are synchronous - their handlers are invoked outside interrupt
 * context, which avoids having user code use locking around critical
 * regions. When a tick interrupt occurs, we write a message to an internal
 * pipe.
 */
void usl_timer_tick(void)
{
	int result;
	char msg = '\0';

	usl_tick++;

	if (!usl_tick_pending) {
		usl_tick_pending = 1;
		result = write(usl_tick_pipe[1], &msg, sizeof(msg));
	}
	if (result < 0)
	    USL_DEBUG("usl_timer_tick() error");
}

/* Come here via a select() on the timer pipe fd.
 * Loop over all timers, calling the user callback function for any timers
 * that have expired.
 * Since the timer list is ordered by expiry time, as soon as we hit a timer
 * that has not yet expired, we exit the loop.
 */
static void usl_timer_tick_handler(int fd, void *arg)
{
	struct usl_timer *tim;
	struct usl_ord_list_head *walk;
	struct usl_ord_list_head *tmp;
	struct usl_list_head *iwalk;
	struct usl_list_head *itmp;
	int result;
	char msg;
	USL_LIST_HEAD(expire_list);

	result = usl_fd_read(usl_tick_pipe[0], &msg, sizeof(msg));
	if (result < 0)
	    USL_DEBUG("usl_timer_tick_handler() error");
	usl_tick_pending = 0;

	usl_list_for_each(walk, tmp, &usl_timer_list) {
		tim = usl_list_entry(walk, struct usl_timer, list);
#ifdef DEBUG
		if (tim->magic != USL_TIMER_MAGIC) {
			USL_SYSLOG(LOG_ERR, "Corrupt timer list. Aborting");
			abort();
		}
#endif /* DEBUG */
		if (tim->deleting) {
			/* Delete a timer if it has been marked for deletion */
			usl_timer_delete_1(tim);
			continue;
		}

		if (usl_tick > tim->expiry) {
			if (!tim->running) {
				continue;
			}

			/* timer expired */
			USL_DEBUG("%s: timer %p expired", __func__, tim);
			usl_list_add_tail(&tim->expire_list, &expire_list);
			if (tim->interval > 0) {
				/* periodic timer - prepare new expiry */
				if (tim->adjuster != NULL) {
					tim->interval = (*tim->adjuster)(tim->start_interval, tim->interval);
				}
				USL_DEBUG("%s: timer %p restarting with interval=%d", __func__, tim, tim->interval);
				tim->expiry = usl_tick + tim->interval;
			} else {
				/* one-shot timer - mark it as no longer running and change its expiry so
				 * that it gets put back at the end of the timer list 
				 */
				tim->expiry = UINT_MAX;
				tim->running = 0;
			}
		} else {
			break;
		}
	}

	/* For each entry in expire_list, re-add the timer to
	 * usl_timer_list at its new position. Then call the timer
	 * expiry callback.
	 */
	usl_list_for_each(iwalk, itmp, &expire_list) {
		tim = usl_list_entry(iwalk, struct usl_timer, expire_list);
#ifdef DEBUG
		if (tim->magic != USL_TIMER_MAGIC) {
			USL_SYSLOG(LOG_ERR, "Corrupt timer list. Aborting");
			abort();
		}
#endif /* DEBUG */
		usl_list_del_init(&tim->expire_list);
		if (tim->deleting) {
			continue;
		}
		usl_ord_list_del_init(&tim->list);
		usl_ord_list_add(&tim->list, &usl_timer_list);
		(*tim->callback)(tim->callback_arg);
	}

	/* Run application timer tick hook */
	if (usl_timer_tick_hook != NULL) {
		(*usl_timer_tick_hook)();
	}
}

/* Create and start a new timer.
 */
void *usl_timer_create(unsigned int expires, unsigned int interval, 
		       usl_timer_callback_fn_t callback, void *callback_arg, 
		       usl_timer_interval_adjust_fn_t adjuster)
{
	struct usl_timer *tim;
	void *key;

	tim = calloc(1, sizeof(*tim));
	if (tim == NULL) {
		return tim;
	}
	tim->start_expiry = expires;
	tim->expiry = usl_tick + expires;
	tim->interval = interval;
	tim->start_interval = tim->interval;
	tim->callback = callback;
	tim->callback_arg = callback_arg;
	tim->adjuster = adjuster;
	tim->running = 1;
	tim->deleting = 0;
	USL_LIST_HEAD_INIT(&tim->expire_list);
#ifdef DEBUG
	tim->magic = USL_TIMER_MAGIC;
	tim->creator = __builtin_return_address(0);
#endif
	key = &tim->expiry;
	USL_ORD_LIST_HEAD_INIT(&tim->list, usl_timer_cmp, key, sizeof(tim->expiry)); 
	usl_ord_list_add(&tim->list, &usl_timer_list);

	USL_DEBUG("%s: created timer %p expire %d", __func__, tim, expires);
	return tim;
}

/* Delete a timer (internal).
 * The timer is finally deleted, called synchronously from the tick 
 * handler to avoid races.
 */
static void usl_timer_delete_1(struct usl_timer *tim)
{
	usl_ord_list_del(&tim->list);
	USL_POISON_MEMORY(tim, 0xf0, sizeof(*tim));
	free(tim);
}

/* Delete a timer.
 * Marks the timer for deletion. The timer is actually deleted by 
 * the timer tick handler. It is added back at the head of  the 
 * timer list so that it will be cleaned up at the next tick.
 */
void usl_timer_delete(void *handle)
{
	struct usl_timer *tim = handle;

	USL_DEBUG("%s: timer %p", __func__, tim);

	tim->deleting = 1;
	tim->running = 0;

	usl_ord_list_del_init(&tim->list);
	tim->expiry = usl_tick;
	tim->interval = 0;
	usl_ord_list_add(&tim->list, &usl_timer_list);
}

/* Stop a timer. 
 * Don't care if the timer is already stopped.
 */
void usl_timer_stop(void *handle)
{
	struct usl_timer *tim = handle;

	USL_DEBUG("%s: timer %p", __func__, tim);

	tim->running = 0;
}

/* Check if a timer is running.
 */
int usl_timer_is_running(void *handle)
{
	struct usl_timer *tim = handle;

	USL_DEBUG("%s: %p", __func__, tim);
	
	return tim->running;
}

/* Restart a timer.
 * Don't care if the timer is not yet running.
 */
void usl_timer_restart(void *handle)
{
	struct usl_timer *tim = handle;

	USL_DEBUG("%s: %p", __func__, tim);

	if (!tim->deleting) {
		usl_ord_list_del_init(&tim->list);
		tim->running = 1;
		tim->expiry = usl_tick + tim->start_expiry;
		tim->interval = tim->start_interval;
		usl_ord_list_add(&tim->list, &usl_timer_list);
	}
}

/* Set a timer's interval period.
 */
void usl_timer_interval_set(void *handle, unsigned int interval)
{
	struct usl_timer *tim = handle;

	USL_DEBUG("%s: %p, interval=%d", __func__, tim, interval);

	tim->start_interval = interval;
}

/* Set a timer's expiry time..
 */
void usl_timer_expiry_set(void *handle, unsigned int expiry)
{
	struct usl_timer *tim = handle;

	USL_DEBUG("%s: %p, expiry=%d", __func__, tim, expiry);

	tim->start_expiry = expiry;
}

/* Find a timer handle.
 * This is useful where an implementation does not wish to
 * keep the timer handle for later reference. It is provided
 * to aid porting applications that use timers where no handle
 * is returned.
 * Use is discouraged since finding a timer involves linear
 * walk of the timer list.
 */
void *usl_timer_find(usl_timer_callback_fn_t callback, void *callback_arg)
{
	struct usl_timer *tim;
	struct usl_ord_list_head *walk;
	struct usl_ord_list_head *tmp;

	usl_list_for_each(walk, tmp, &usl_timer_list) {
		tim = usl_list_entry(walk, struct usl_timer, list);
#ifdef DEBUG
		if (tim->magic != USL_TIMER_MAGIC) {
			USL_SYSLOG(LOG_ERR, "Corrupt timer list. Aborting");
			abort();
		}
#endif /* DEBUG */
		if ((tim->callback == callback) && (tim->callback_arg == callback_arg)) {
			return tim;
		}
	}

	return NULL;
}

/* Internal comparison function for the usl_ord_list_head
 * timer list. This is used by usl_ord_list_add() to add an
 * entry to a list in order.
 */
static int usl_timer_cmp(void *t1, void *t2, int cmp_len)
{
	unsigned int expire1, expire2;

	expire1 = *((unsigned int *) t1);
	expire2 = *((unsigned int *) t2);

	if (expire1 > expire2) {
		return 1;
	} else if (expire1 < expire2) {
		return -1;
	}
	return 0;
}

/*****************************************************************************
 * Init and cleanup
 *****************************************************************************/

int usl_timer_init(void)
{
	int result;

	usl_timer_tick_hook = NULL;

	result = pipe(&usl_tick_pipe[0]);
	if (result < 0) {
		fprintf(stderr, "Failed to create internal pipe: %s\n", strerror(-errno));
		goto out;
	}
	(void) fcntl(usl_tick_pipe[0], F_SETFD, FD_CLOEXEC);
	(void) fcntl(usl_tick_pipe[1], F_SETFD, FD_CLOEXEC);
	result = usl_fd_add_fd(usl_tick_pipe[0], usl_timer_tick_handler, NULL);
	if (result < 0) {
		goto out;
	}

	usl_tick = 0;
	memset(&usl_itimval, 0, sizeof(usl_itimval));
	usl_itimval.it_interval.tv_sec = 0;
	usl_itimval.it_interval.tv_usec = 1000000 / USL_TIMER_HZ;
	usl_itimval.it_value.tv_sec = 0;
	usl_itimval.it_value.tv_usec = 1000000 / USL_TIMER_HZ;
  
	result = setitimer(ITIMER_REAL, &usl_itimval, NULL);
out:
	return result;
}

void usl_timer_cleanup(void)
{
	struct usl_timer *tim;
	struct usl_ord_list_head *walk;
	struct usl_ord_list_head *tmp;

	/* stop the interval timer tick */
	usl_itimval.it_interval.tv_sec = 0;
	(void) setitimer(ITIMER_REAL, &usl_itimval, NULL);

	usl_list_for_each(walk, tmp, &usl_timer_list) {
		tim = usl_list_entry(walk, struct usl_timer, list);
#ifdef DEBUG
		if (tim->magic != USL_TIMER_MAGIC) {
			USL_SYSLOG(LOG_ERR, "Corrupt timer list. Aborting");
			goto out;
		}
#endif /* DEBUG */
		usl_ord_list_del(&tim->list);
		free(tim);
	}

#ifdef DEBUG
out:
#endif
	close(usl_tick_pipe[0]);
	close(usl_tick_pipe[1]);
}
