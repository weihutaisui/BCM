/*****************************************************************************
 * Copyright (C) 2004,2005,2006,2007,2008 Katalix Systems Ltd
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA 
 *
 *****************************************************************************/

/*
 * Dummmy process for testing ppp_unix plugin interface.
 * Dump all arguments to a tmp file and sleep forever.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>

#define PPPD_DUMMY_TMP_FILE	"/tmp/pppd_dummy"

int main(int argc, char **argv)
{
	int arg;
	char filename[30];
	FILE *file;

	sprintf(&filename[0], "%s-%d", PPPD_DUMMY_TMP_FILE, getpid());
	file = fopen(filename, "w");
	
	if (file == NULL) {
		syslog(LOG_ERR, "Failed to open %s\n", filename);
		return;
	}

	for (arg = 1; arg < argc; arg++) {
		fprintf(file, "%s ", argv[arg]);
	}
	fprintf(file, "\n");
	fclose(file);

	/* Wait until we're killed */
	pause();

	return 0;
}
