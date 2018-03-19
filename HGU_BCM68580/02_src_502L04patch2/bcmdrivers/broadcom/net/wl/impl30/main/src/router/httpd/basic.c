/*
 * Basic skin (shtml)
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
 * $Id: basic.c 385831 2013-02-18 17:24:11Z $
 */

#include <stdio.h>
#include <httpd.h>

struct mime_handler mime_handlers[] = {
	{ "**.htm", "text/html", NULL, NULL, do_file, NULL },
	{ "**.html", "text/html", NULL, NULL, do_file, NULL },
	{ "**.gif", "image/gif", NULL, NULL, do_file, NULL },
	{ "**.jpg", "image/jpeg", NULL, NULL, do_file, NULL },
	{ "**.jpeg", "image/gif", NULL, NULL, do_file, NULL },
	{ "**.png", "image/png", NULL, NULL, do_file, NULL },
	{ "**.css", "text/css", NULL, NULL, do_file, NULL },
	{ "**.au", "audio/basic", NULL, NULL, do_file, NULL },
	{ "**.wav", "audio/wav", NULL, NULL, do_file, NULL },
	{ "**.avi", "video/x-msvideo", NULL, NULL, do_file, NULL },
	{ "**.mov", "video/quicktime", NULL, NULL, do_file, NULL },
	{ "**.mpeg", "video/mpeg", NULL, NULL, do_file, NULL },
	{ "**.vrml", "model/vrml", NULL, NULL, do_file, NULL },
	{ "**.midi", "audio/midi", NULL, NULL, do_file, NULL },
	{ "**.mp3", "audio/mpeg", NULL, NULL, do_file, NULL },
	{ "**.pac", "application/x-ns-proxy-autoconfig", NULL, NULL, do_file, NULL },
	{ "**.conf", "text/plain", NULL, NULL, denied_file, NULL },
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

struct ej_handler ej_handlers[] = {
	{ NULL, NULL }
};
