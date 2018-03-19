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

#ifndef USL_H
#define USL_H

#include <string.h>
#include <setjmp.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <sys/wait.h>
#ifdef USL_DMALLOC
#include <dmalloc.h>
#endif

#include "usl_fsm.h"
#include "usl_list.h"

#define USL_VERSION	"0.7"

#ifdef DEBUG
#define USL_DEBUG(fmt, args...)						\
	do {								\
		if (usl_debug) {					\
			(*usl_log_fn)(LOG_DEBUG, "DEBUG: " fmt, ##args);\
		}							\
	} while(0)

#define USL_POISON_MEMORY(addr, fill, size)				\
	memset(addr, fill, size)
#else
#define USL_DEBUG(fmt, args...)			do { } while(0)
#define USL_POISON_MEMORY(addr, fill, size)	do { } while(0)
#endif /* DEBUG */

#define USL_SYSLOG(level, fmt, args...)					\
	do {								\
		(*usl_log_fn)(level, "SYSTEM: " fmt, ##args);		\
	} while(0)

/* Number of timer ticks per second. */
#define USL_TIMER_HZ			4

/* Macro for use in timer_create() and timer_duration_set() calls */
#define USL_TIMER_TICKS(secs)		((secs) * USL_TIMER_HZ)
#define USL_TIMER_250MSEC		1
#define USL_TIMER_500MSEC		2

/*****************************************************************************
 * Hash utilities
 * Hash courtesy of the R5 hash in reiserfs modulo sign bits
 ****************************************************************************/

#define usl_hash_init_name()		0

/* partial hash update function. Assume roughly 4 bits per character */
static inline unsigned long usl_hash_partial_name(unsigned long c, unsigned long prevhash)
{
	return (prevhash + (c << 4) + (c >> 4)) * 11;
}

/* Finally: cut down the number of bits to a int value (and try to avoid
 * losing bits)
 */
static inline unsigned long usl_hash_end_name(unsigned long hash)
{
	return (unsigned int) hash;
}

/* Compute the hash for a name string. */
static inline unsigned int usl_hash_full_name(const unsigned char *name, unsigned int len)
{
	unsigned long hash = usl_hash_init_name();

	while (len--) {
		hash = usl_hash_partial_name(*name++, hash);
	}
	return usl_hash_end_name(hash);
}

/*****************************************************************************
 * USL public interface
 *****************************************************************************/

typedef void (*usl_timer_callback_fn_t)(void *callback_arg);
typedef int (*usl_timer_interval_adjust_fn_t)(int start_interval, int current_interval);
typedef void (*usl_fd_callback_fn_t)(int fd, void *arg);
typedef void (*usl_notify_fn_t)(void *callback_arg, int sig);
typedef void (*usl_pid_done_fn_t)(void *arg, pid_t pid, int sig);

extern int usl_debug;
extern void (*usl_log_fn)(int level, const char *fmt, ...);
extern void (*usl_timer_tick_hook)(void);
extern void (*usl_signal_terminate_hook)(void);
extern void (*usl_signal_hangup_hook)(void);

extern void (*usl_fd_poll_hook)(fd_set *fds);

/* usl_timer.c */
extern void *usl_timer_create(unsigned int expires, unsigned int interval, 
			      usl_timer_callback_fn_t callback, void *callback_arg,
			      usl_timer_interval_adjust_fn_t adjuster);
extern void usl_timer_delete(void *handle);
extern void usl_timer_stop(void *handle);
extern int usl_timer_is_running(void *handle);
extern void usl_timer_restart(void *handle);
extern void usl_timer_interval_set(void *handle, unsigned int interval);
extern void usl_timer_expiry_set(void *handle, unsigned int expiry);
extern void *usl_timer_find(usl_timer_callback_fn_t callback, void *callback_arg);
extern int usl_timer_init(void);
extern void usl_timer_cleanup(void);

extern void (*usl_timer_tick_hook)(void);

/* usl_fd.c */
extern int usl_fd_add_fd(int fd, usl_fd_callback_fn_t callback, void *arg);
extern int usl_fd_modify_fd(int fd, usl_fd_callback_fn_t callback, void *arg);
extern int usl_fd_remove_fd(int fd);
extern void usl_fd_poll(void);
extern size_t usl_fd_read(int fd, void *buf, size_t count);
extern int usl_fd_init(void);
extern void usl_fd_cleanup(void);

extern void (*usl_fd_poll_hook)(fd_set *fds);


/* usl_signal.c */
extern void usl_main_loop(void);
extern int usl_signal_notifier_add(usl_notify_fn_t func, void *arg);
extern void usl_signal_notifier_remove(usl_notify_fn_t func, void *arg);
extern void usl_signal_init(void);
extern void usl_signal_cleanup(void);

extern void (*usl_signal_terminate_hook)(void);
extern void (*usl_signal_hangup_hook)(void);

/* usl_misc.c */
extern void usl_daemonize(void);
extern void usl_set_debug(int debug, void (*log_fn)(int level, const char *fmt, ...));

/* usl_pid.c */
extern int usl_pid_record_child(int pid, usl_pid_done_fn_t callback, void *arg);
extern int usl_pid_reap_children(int waitfor);
extern pid_t usl_pid_safe_fork(void);
extern int usl_pid_init(void);
extern void usl_pid_cleanup(void);


#endif
