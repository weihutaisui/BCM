/*
 * CGI helper functions
 *
 * Copyright (C) 2017, Broadcom. All Rights Reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: cgi.c 707888 2017-06-29 06:28:21Z $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#ifdef BCMDBG
#include <assert.h>
#else
#define assert(a)
#endif // endif

#if defined(linux)
/* Use SVID search */
#define __USE_GNU
#include <search.h>
#elif defined(__ECOS)
#include <search.h>
#define table htable
#endif // endif

/* CGI hash table */
static struct hsearch_data htab;
static int htab_count;

static void
unescape(char *s)
{
	unsigned int c;

	while ((s = strpbrk(s, "%+"))) {
		/* Parse %xx */
		if (*s == '%') {
			sscanf(s + 1, "%02x", &c);
			*s++ = (char) c;
			memmove(s, s + 2, strlen(s) - 1);
		}
		/* Space is special */
		else if (*s == '+')
			*s++ = ' ';
	}
}

char *
get_cgi(char *name)
{
	ENTRY e, *ep;

	if (!htab.table)
		return NULL;

	e.key = name;
	hsearch_r(e, FIND, &ep, &htab);

	return ep ? ep->data : NULL;
}

void
set_cgi(char *name, char *value)
{
	ENTRY e, *ep;

	if (!htab.table)
		return;

	e.key = name;
	hsearch_r(e, FIND, &ep, &htab);
	if (ep)
		ep->data = value;
	else {
		e.data = value;
		hsearch_r(e, ENTER, &ep, &htab);
		htab_count++;
	}
	assert(ep);
}

void
init_cgi(char *query)
{
	int len, nel;
	char *q, *name, *value;

	htab_count = 0;

	/* Clear variables */
	if (!query) {
		hdestroy_r(&htab);
		return;
	}

	/* Parse into individual assignments */
	q = query;
	len = strlen(query);
	nel = 1;
	while (strsep(&q, "&;"))
		nel++;
	hcreate_r(nel, &htab);

	for (q = query; q < (query + len);) {
		/* Unescape each assignment */
		unescape(name = value = q);

		/* Skip to next assignment */
		for (q += strlen(q); q < (query + len) && !*q; q++);

		/* Assign variable */
		name = strsep(&value, "=");
		if (value)
			set_cgi(name, value);
	}
}

int
count_cgi()
{
	return htab_count;
}
