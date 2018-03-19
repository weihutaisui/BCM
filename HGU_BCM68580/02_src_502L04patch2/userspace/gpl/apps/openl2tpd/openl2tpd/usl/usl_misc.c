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

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "usl.h"

int usl_debug;
void (*usl_log_fn)(int level, const char *fmt, ...) = syslog;

/* The proper way to make ourself a daemon.
 */
void usl_daemonize(void)
{
        pid_t pid;
	FILE *f;

        pid = fork();
        if (pid < 0) exit(1);         /* fork failed */
        if (pid > 0) _exit(0);        /* parent exits */

        setsid();

        pid = fork();
        if (pid < 0) exit(1);         /* fork failed */
        if (pid > 0) _exit(0);        /* parent exits */

        if (chdir("/") < 0) {
		perror("chdir");
		exit(1);
	}

        f = freopen("/dev/null", "r", stdin);
	if (f == NULL) {
		perror("freopen");
		exit(1);
	}
        f = freopen("/dev/null", "w", stdout);
	if (f == NULL) {
		perror("freopen");
		exit(1);
	}
        f = freopen("/dev/null", "w", stderr);
	if (f == NULL) {
		perror("freopen");
		exit(1);
	}
}

/* Allows an app to register the function to be called to log debug
 * messages. syslog() is used by default.
 */
void usl_set_debug(int debug, void (*log_fn)(int level, const char *fmt, ...))
{
        usl_debug = debug;
        usl_log_fn = log_fn;
}
