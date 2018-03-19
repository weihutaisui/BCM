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
 * Process list management
 */

#include <string.h>

#include "usl.h"

/* We maintain a list of child process pids and functions to call when
 * they exit.
 */
struct usl_pid_child {
	pid_t			pid;
	usl_pid_done_fn_t	callback;
	void			*arg;
	struct usl_list_head	list;
};

static USL_LIST_HEAD(usl_child_list);

/* Called by application to register a child pid.
 */
int usl_pid_record_child(int pid, usl_pid_done_fn_t callback, void *arg)
{
	struct usl_pid_child *child;

	child = (struct usl_pid_child *) malloc(sizeof(struct usl_pid_child));
	if (child == NULL) {
		USL_SYSLOG(LOG_WARNING, "losing track of pid %d", pid);
		return -ENOMEM;
	} else {
		USL_LIST_HEAD_INIT(&child->list);
		child->pid = pid;
		child->callback = callback;
		child->arg = arg;
		usl_list_add(&child->list, &usl_child_list);
	}

	USL_DEBUG("%s: pid=%d", __func__, pid);
	return 0;
}

/* Check for dead child processes, and log a message for abnormal
 * terminations.
 */
int usl_pid_reap_children(int waitfor)
{
	int pid, status;
	struct usl_pid_child *child = NULL;
	int sig = 0;
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	int have_callback;

	if (usl_list_empty(&usl_child_list)) {
		return 0;
	}

	/* Wait for processes in our process group. */

	while (((pid = waitpid(WAIT_ANY, &status, (waitfor ? 0: WNOHANG))) != -1) && (pid != 0)) {
		have_callback = 0;
		usl_list_for_each(walk, tmp, &usl_child_list) {
			child = usl_list_entry(walk, struct usl_pid_child, list);
			if (child->pid == pid) {
				usl_list_del_init(&child->list);
				have_callback = 1;
				break;
			}
		}
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != 0) {
				/* If process returned non-zero status, fake it as aborted 
				 * to tell callback of failure condition 
				 */
				sig = SIGABRT;
			}
		} else if (WIFSIGNALED(status)) {
			sig = WTERMSIG(status);
			if (usl_debug) {
				USL_SYSLOG(LOG_INFO, "Child process %d terminated with signal %d", pid, sig);
			}
		}

		/* If a callback is registered, call it now */
		if (have_callback && (child->callback != NULL)) {
			(*child->callback)(child->arg, child->pid, sig);
		}

		if (have_callback) {
			USL_DEBUG("%s: pid %d reaped", __func__, child->pid);
			USL_POISON_MEMORY(child, 0xf1, sizeof(*child));
			free(child);
		}
	}

	if (pid == -1) {
		if (errno == ECHILD) {
			return -1;
		}
		if (errno != EINTR) {
			USL_SYSLOG(LOG_ERR, "Error waiting for child process: %m");
		}
	}

	return 0;
}

/* Create a child process. The parent waits for the child signal it's
 * started before returning. Copied from ppp-2.4.2.
 */
pid_t usl_pid_safe_fork(void)
{
	pid_t pid;
	int pipefd[2];
	char buf[1];

	if (pipe(pipefd) == -1) {
		pipefd[0] = pipefd[1] = -1;
	}
	pid = fork();
	if (pid < 0) {
		return -1;
	}

	if (pid > 0) {
		/* parent... */

		close(pipefd[1]);
		/* this read() blocks until the close(pipefd[1]) below */
		usl_fd_read(pipefd[0], buf, 1);
		close(pipefd[0]);

		return pid;
	}

	/* child... */

	close(pipefd[0]);
	/* this close unblocks the read() call above in the parent */
	close(pipefd[1]);

	return 0;
}

/*****************************************************************************
 * Init and cleanup
 *****************************************************************************/

int usl_pid_init(void)
{
	return 0;
}

void usl_pid_cleanup(void)
{
	struct usl_pid_child *child;
	struct usl_list_head *walk;
	struct usl_list_head *tmp;

	/* Kill our kids. Don't tell the police */
	usl_list_for_each(walk, tmp, &usl_child_list) {
		child = usl_list_entry(walk, struct usl_pid_child, list);
		if (child->pid > 0) {
			kill(child->pid, SIGTERM);
		}
	}
	/* Reap them */
	usl_pid_reap_children(1);

	/* Bury them */
	usl_list_for_each(walk, tmp, &usl_child_list) {
		child = usl_list_entry(walk, struct usl_pid_child, list);
		free(child);
	}
}
