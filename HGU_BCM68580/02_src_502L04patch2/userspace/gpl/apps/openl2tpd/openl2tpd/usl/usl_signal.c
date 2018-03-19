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
 * Signal notification.
 * Applications register callbacks to be called when specific signal is
 * generated.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "usl.h"
#include "usl_private.h"

struct usl_notifier {
	struct usl_list_head		list;
	usl_notify_fn_t			func;
	void				*arg;
};

typedef struct {
	volatile int	sighup:1;
	volatile int	sigterm:1;
	volatile int	sigchld:1;
	volatile int	sigusr1:1;
	volatile int	sigusr2:1;
	volatile int	running:1;
	sig_atomic_t	waiting;
	sigjmp_buf 	sigjmp;
} usl_signal_data_t;

static					USL_LIST_HEAD(usl_sig_notifier_list);
static usl_signal_data_t		usl_sigdata;

void (*usl_signal_terminate_hook)(void) = NULL;
void (*usl_signal_hangup_hook)(void) = NULL;

#ifdef DEBUG
/* Useful for test harnesses to run code for a few events then return */
int usl_main_loop_max_count = 0;
#endif

static void usl_signal_notify(int val)
{
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	struct usl_notifier *notifier;

	usl_list_for_each(walk, tmp, &usl_sig_notifier_list) {
		notifier = usl_list_entry(walk, struct usl_notifier, list);
		(*notifier->func)(notifier->arg, val);
	}
}

static void usl_signal_procgroup(int sig)
{
	struct sigaction act, oldact;

	act.sa_handler = SIG_IGN;
	act.sa_flags = 0;
	killpg(0, sig);
	sigaction(sig, &act, &oldact);
	sigaction(sig, &oldact, NULL);
}

static void usl_sighup(int sig)
{
	usl_sigdata.sighup = 1;
	if (usl_sigdata.running) {
		usl_signal_procgroup(sig);
	}
	usl_signal_notify(sig);
	if (usl_sigdata.waiting) {
		siglongjmp(usl_sigdata.sigjmp, 1);
	}
}

static void usl_sigterm(int sig)
{
	usl_sigdata.sigterm = 1;
	if (usl_sigdata.running) {
		usl_signal_procgroup(sig);
	}
	usl_signal_notify(sig);
	if (usl_sigdata.waiting) {
		siglongjmp(usl_sigdata.sigjmp, 1);
	}
}

static void usl_sigchld(int sig)
{
	/* Child processes are reaped in the main process loop */
	usl_sigdata.sigchld = 1;
	usl_signal_notify(sig);
}

static void usl_sigalrm(int sig)
{
	usl_timer_tick();
}

static void usl_sigusr1(int sig)
{
	usl_signal_notify(sig);
}

static void usl_sigusr2(int sig)
{
	usl_signal_notify(sig);
}

static void usl_sigbad(int sig)
{
	static sig_atomic_t crashed = 0;

	if (crashed) {
		_exit(127);
	}
	crashed = 1;
	USL_SYSLOG(LOG_ERR, "Fatal signal %d", sig);
	if (usl_sigdata.running) {
		usl_signal_procgroup(SIGTERM);
	}
	usl_signal_notify(sig);
	exit(127);
}

/* Allow an application to jump back into the main loop.
 */
void usl_signal_continue(void)
{
	siglongjmp(usl_sigdata.sigjmp, 1);
	/* NOTREACHED */
}

int usl_signal_notifier_add(usl_notify_fn_t func, void *arg)
{
	struct usl_notifier *notifier;

	notifier = (struct usl_notifier *) calloc(1, sizeof(struct usl_notifier));
	if (notifier == NULL) {
		USL_DEBUG("%s: out of memory", __func__);
		return -ENOMEM;
	}
	USL_LIST_HEAD_INIT(&notifier->list);
	notifier->func = func;
	notifier->arg = arg;
	usl_list_add(&notifier->list, &usl_sig_notifier_list);

	return 0;
}

void usl_signal_notifier_remove(usl_notify_fn_t func, void *arg)
{
	struct usl_list_head *walk;
	struct usl_list_head *tmp;
	struct usl_notifier *notifier;

	usl_list_for_each(walk, tmp, &usl_sig_notifier_list) {
		notifier = usl_list_entry(walk, struct usl_notifier, list);
		if (notifier->func == func && notifier->arg == arg) {
			usl_list_del(&notifier->list);
			USL_POISON_MEMORY(notifier, 0xf3, sizeof(*notifier));
			free(notifier);
			return;
		}
	}
}

void usl_main_loop(void)
{
	sigset_t mask;
#ifdef DEBUG
	int loop_count = 0;
#endif

	sigemptyset(&mask);

	for (;;) {
#ifdef DEBUG
		loop_count++;
		if ((usl_main_loop_max_count > 0) && (loop_count > usl_main_loop_max_count)) {
			loop_count = 0;
			usl_main_loop_max_count = 0;
			break;
		}
#endif
		if (sigsetjmp(usl_sigdata.sigjmp, 1) == 0) {
			sigprocmask(SIG_BLOCK, &mask, NULL);
			if (usl_sigdata.sighup || usl_sigdata.sigterm || usl_sigdata.sigchld) {
				usl_sigdata.waiting = 0;
				sigprocmask(SIG_UNBLOCK, &mask, NULL);
			} else {
				usl_sigdata.waiting = 1;
				sigprocmask(SIG_UNBLOCK, &mask, NULL);
				usl_fd_poll();
				continue;
			}
		}
		if (usl_sigdata.sighup) {
			usl_sigdata.sighup = 0;
			if (usl_signal_hangup_hook != NULL) {
				(*usl_signal_hangup_hook)();
			}
		}
		if (usl_sigdata.sigterm) {
			if (usl_signal_terminate_hook != NULL) {
				(*usl_signal_terminate_hook)();
			}
			exit(1);
			/* NOTREACHED */
		}
		if (usl_sigdata.sigchld) {
			usl_sigdata.sigchld = 0;
			usl_pid_reap_children(0);     /* Don't leave dead kids lying around */
		}

	}
}

/*****************************************************************************
 * Init and cleanup
 *****************************************************************************/

void usl_signal_init(void)
{
	struct sigaction sa;
	sigset_t mask;
	static int initialized = 0;

	if (initialized) {
		return;
	}
	initialized = 1;

	memset(&usl_sigdata, 0, sizeof(usl_sigdata));

	/* Mask out any uninteresting signals. */
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGCHLD);
	sigaddset(&mask, SIGALRM);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);

#define SIGNAL(s, handler)	do { \
		sa.sa_handler = handler; \
		if (sigaction(s, &sa, NULL) < 0) { \
			USL_SYSLOG(LOG_ERR, "Couldn't establish signal handler (%d): %m", s); \
			exit(1); \
		} \
	} while (0)

	sa.sa_mask = mask;
	sa.sa_flags = 0;
	signal(SIGPIPE, SIG_IGN);

	/* Install handlers for interesting signals */
	SIGNAL(SIGHUP, usl_sighup);
	SIGNAL(SIGINT, usl_sigterm);
	SIGNAL(SIGTERM, usl_sigterm);
	SIGNAL(SIGCHLD, usl_sigchld);
	SIGNAL(SIGUSR1, usl_sigusr1);
	SIGNAL(SIGUSR2, usl_sigusr2);
	SIGNAL(SIGALRM, usl_sigalrm);
	SIGNAL(SIGSYS, usl_sigbad);
#ifndef DEBUG /* Don't catch app failures when debugging */
	SIGNAL(SIGABRT, usl_sigbad);
	SIGNAL(SIGFPE, usl_sigbad);
	SIGNAL(SIGILL, usl_sigbad);
	SIGNAL(SIGQUIT, usl_sigbad);
	SIGNAL(SIGSEGV, usl_sigbad);
	SIGNAL(SIGBUS, usl_sigbad);
#endif /* DEBUG */
	SIGNAL(SIGPOLL, usl_sigbad);
	SIGNAL(SIGPROF, usl_sigbad);
	SIGNAL(SIGTRAP, usl_sigbad);
	SIGNAL(SIGVTALRM, usl_sigbad);
	SIGNAL(SIGXCPU, usl_sigbad);
	SIGNAL(SIGXFSZ, usl_sigbad);
}

void usl_signal_cleanup(void)
{
}

