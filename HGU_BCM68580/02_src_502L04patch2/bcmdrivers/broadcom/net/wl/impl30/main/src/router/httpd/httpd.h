/*
 * milli_httpd - pretty small HTTP server
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
 * $Id: httpd.h 592693 2015-10-14 07:27:15Z $
 */

#ifndef _httpd_h_
#define _httpd_h_

#if defined(DEBUG) && defined(DMALLOC)
#include <dmalloc.h>
#endif // endif

/* Basic authorization userid and passwd limit */
#define AUTH_MAX 64

/* Generic MIME type handler */
struct mime_handler {
	char *pattern;
	char *mime_type;
	char *extra_header;
	void (*input)(char *path, FILE *stream, int len, char *boundary);
	void (*output)(char *path, FILE *stream);
	void (*auth)(char *userid, char *passwd, char *realm);
};
extern struct mime_handler mime_handlers[];

/* CGI helper functions */
extern void init_cgi(char *query);
extern char * get_cgi(char *name);
extern void set_cgi(char *name, char *value);
extern int count_cgi(void);
extern int b64_decode( const char* str, unsigned char* space, int size );
extern char *b64_encode( unsigned char *src ,int src_len, unsigned char* space, int space_len);

/* Regular file handler */
extern void do_file(char *path, FILE *stream);

/* GoAhead 2.1 compatibility */
typedef FILE * webs_t;
typedef char char_t;
#define T(s) (s)
#define __TMPVAR(x) tmpvar ## x
#define _TMPVAR(x) __TMPVAR(x)
#define TMPVAR _TMPVAR(__LINE__)
#define websWrite(wp, fmt, args...) ({ int TMPVAR = fprintf(wp, fmt , ## args); fflush(wp); TMPVAR; })
#define websError(wp, code, msg, args...) fprintf(wp, msg , ## args)
#define websHeader(wp) fputs("<html lang=\"en\">", wp)
#define websFooter(wp) fputs("</html>", wp)
#define websDone(wp, code) fflush(wp)
#define websGetVar(wp, var, default) (get_cgi(var) ? : default)
#define websSetVar(wp, var, value) set_cgi(var, value)
#define websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query) ({ do_ej(path, wp); fflush(wp); 1; })
#define websWriteData(wp, buf, nChars) ({ int TMPVAR = fwrite(buf, 1, nChars, wp); fflush(wp); TMPVAR; })
#define websWriteDataNonBlock websWriteData
#ifdef BCMDBG
#define a_assert(a) assert((a))
#else
#define a_assert(a)
#endif // endif

extern int ejArgs(int argc, char_t **argv, char_t *fmt, ...);

/* GoAhead 2.1 Embedded JavaScript compatibility */
extern void do_ej(char *path, FILE *stream);
struct ej_handler {
	char *pattern;
	int (*output)(int eid, webs_t wp, int argc, char_t **argv);
};
extern struct ej_handler ej_handlers[];

#if defined(__ECOS)
#define fopen(path, mode)	tar_fopen((path), (mode))
#define fclose(fp)		tar_fclose((fp))
#undef getc
#define getc(fp)		tar_fgetc((fp))
extern FILE * tar_fopen(const char *path, const char *mode);
extern void tar_fclose(FILE *fp);
extern int tar_fgetc(FILE *fp);
#endif // endif

/* Host name based virtual server entries */
struct hostentry {
    char *domain;
    char *subdir;
    size_t subdirLen;
    struct hostentry *next;
};

/* macro to place ej_handler at ELF section "ej_handlers" */
#define REG_EJ_HANDLER(name)    \
	static struct ej_handler __ej_handler__##name   \
	__attribute__((__section__("ej_handlers")))     \
	__attribute__((__used__)) = {                   \
		.pattern = #name,                       \
		.output = ej_##name,                    \
	}
#endif /* _httpd_h_ */
