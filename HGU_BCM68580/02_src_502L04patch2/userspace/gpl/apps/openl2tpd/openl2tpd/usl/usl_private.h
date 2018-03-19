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

/* This header should not be included by applications.
 */

#ifndef USL_PRIVATE_H
#define USL_PRIVATE_H

/*****************************************************************************
 * USL debugging stuff
 *****************************************************************************/

#ifdef DEBUG
#define USL_DEBUG(fmt, args...)						\
	do {								\
		if (usl_debug) {					\
			(*usl_log_fn)(LOG_DEBUG, "DEBUG: " fmt, ##args);\
		}							\
	} while(0)

#else
#define USL_DEBUG(fmt, args...)			do { } while(0)
#endif /* DEBUG */

#define USL_SYSLOG(level, fmt, args...)					\
	do {								\
		(*usl_log_fn)(level, "SYSTEM: " fmt, ##args);		\
	} while(0)

extern int usl_debug;
extern void (*usl_log_fn)(int level, const char *fmt, ...);

/*****************************************************************************
 * USL private interface
 *****************************************************************************/

/* usl_timer.c */
extern void usl_timer_tick(void);
extern void *usl_timer_find(usl_timer_callback_fn_t callback, void *callback_arg);

/* usl_pid.c */
extern int usl_pid_reap_children(int waitfor);

/* usl_signal.c */
extern void usl_signal_continue(void) __attribute((noreturn));

/* usl_fd.c */
extern void usl_fd_poll(void);


#endif
