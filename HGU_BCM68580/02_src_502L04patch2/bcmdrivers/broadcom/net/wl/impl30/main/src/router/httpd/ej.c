/*
 * Tiny Embedded JavaScript parser
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
 * $Id: ej.c 592693 2015-10-14 07:27:15Z $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <httpd.h>

/* __start_section_name and __stop_section_name, gcc will include these variables.
 * ej_handlers is ELF section name
 */
extern struct ej_handler __start_ej_handlers;
extern struct ej_handler __stop_ej_handlers;
static char * get_arg(char *args, char **next);
static void call(char *func, FILE *stream);

/* Look for unquoted character within a string */
static char *
unqstrstr(char *haystack, char *needle)
{
	char *cur;
	int q;

	for (cur = haystack, q = 0;
	     cur < &haystack[strlen(haystack)] && !(!q && !strncmp(needle, cur, strlen(needle)));
	     cur++) {
		if (*cur == '"')
			q ? q-- : q++;
	}
	return (cur < &haystack[strlen(haystack)]) ? cur : NULL;
}

static char *
get_arg(char *args, char **next)
{
	char *arg, *end;

	/* Parse out arg, ... */
	if (!(end = unqstrstr(args, ","))) {
		end = args + strlen(args);
		*next = NULL;
	} else
		*next = end + 1;

	/* Skip whitespace and quotation marks on either end of arg */
	for (arg = args; isspace((int)*arg) || *arg == '"'; arg++);
	for (*end-- = '\0'; isspace((int)*end) || *end == '"'; end--)
		*end = '\0';

	return arg;
}

static void
call(char *func, FILE *stream)
{
	char *args, *end, *next;
	int argc;
	char * argv[16];
	struct ej_handler *handler = &__start_ej_handlers;

	/* Parse out ( args ) */
	if (!(args = strchr(func, '(')))
		return;
	if (!(end = unqstrstr(func, ")")))
		return;
	*args++ = *end = '\0';

	/* Set up argv list */
	for (argc = 0; argc < 16 && args && *args; argc++, args = next) {
		if (!(argv[argc] = get_arg(args, &next)))
			break;
	}

	/* Call handler */
	for (; handler < &__stop_ej_handlers; ++handler) {
		if (strcmp(handler->pattern, func) == 0) {
			handler->output(0, stream, argc, argv);
			break;
		}
	}
}

void
do_ej(char *path, FILE *stream)
{
	FILE *fp;
	int c;
	char pattern[1000], *asp = NULL, *func = NULL, *end = NULL;
	int len = 0;

	if (!(fp = fopen(path, "r")))
		return;

	while ((c = getc(fp)) != EOF) {

		/* Add to pattern space */
		pattern[len++] = c;
		pattern[len] = '\0';
		if (len == (sizeof(pattern) - 1))
			goto release;

		/* Look for <% ... */
		if (!asp && !strncmp(pattern, "<%", len)) {
			if (len == 2)
				asp = pattern + 2;
			continue;
		}

		/* Look for ... %> */
		if (asp) {
			if (unqstrstr(asp, "%>")) {
				for (func = asp; func < &pattern[len]; func = end) {
					/* Skip initial whitespace */
					for (; isspace((int)*func); func++);
					if (!(end = unqstrstr(func, ";")))
						break;
					*end++ = '\0';

					/* Call function */
					call(func, stream);
				}
				asp = NULL;
				len = 0;
			}
			continue;
		}

	release:
		/* Release pattern space */
		fputs(pattern, stream);
		len = 0;
	}

	fclose(fp);
}

int
ejArgs(int argc, char **argv, char *fmt, ...)
{
	va_list	ap;
	int arg;
	char *c;

	if (!argv)
		return 0;

	va_start(ap, fmt);
	for (arg = 0, c = fmt; c && *c && arg < argc;) {
		if (*c++ != '%')
			continue;
		switch (*c) {
		case 'd':
			*(va_arg(ap, int *)) = atoi(argv[arg]);
			break;
		case 's':
			*(va_arg(ap, char **)) = argv[arg];
			break;
		}
		arg++;
	}
	va_end(ap);

	return arg;
}
