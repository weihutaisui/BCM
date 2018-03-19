/* milli_httpd - pretty small HTTP server
** A combination of
** micro_httpd - really small HTTP server
** and
** mini_httpd - small HTTP server
**
** Copyright © 1999,2000 by Jef Poskanzer <jef@acme.com>.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>

#include <httpd.h>
#include <bcmnvram.h>
#include <arpa/inet.h>
#include <shutils.h>

#if defined(__ECOS)
#include <signal.h>
#include <sys/ioctl.h>
extern int snprintf(char *str, size_t count, const char *fmt, ...);
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
#define main			milli
#define	MAX_BACKLOG		10
#else
#include <error.h>
#include <sys/signal.h>
#define	MAX_BACKLOG		10
#endif // endif

#define SERVER_NAME "httpd"
#define SERVER_PORT 80
#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"
#define CONFFILE "httpd.conf"
#define MAXCONFLINE 255

/* A multi-family sockaddr. */
typedef union {
    struct sockaddr sa;
    struct sockaddr_in sa_in;
    } usockaddr;

/* Globals. */
static FILE *conn_fp;
static char auth_userid[AUTH_MAX];
static char auth_passwd[AUTH_MAX];
static char auth_realm[AUTH_MAX];
static char *conffile = CONFFILE;
static struct hostentry *hosts = NULL;

extern int internal_init(void);

/* Signal states. */
static int sighup;

/* Forwards. */
static void sig_handler( int sig );
static void load_config(void);
static void free_config();
static struct hostentry* find_host( char *host );
static int initialize_listen_socket( usockaddr* usaP );
static int auth_check( char* dirname, char* authorization );
static void send_authenticate( char* realm );
static void send_error( int status, char* title, char* extra_header, char* text );
static void send_headers( int status, char* title, char* extra_header, char* mime_type );

static int match( const char* pattern, const char* string );
static int match_one( const char* pattern, int patternlen, const char* string );
static void handle_request(void);

static int
initialize_listen_socket( usockaddr* usaP )
    {
    int listen_fd;
    int i;
    char *ipaddr=NULL,*proto=NULL;
    struct in_addr addr;
    int disable_router;

    /* Allow wild card binds only if the lan protocol is dhcp
       If lan_proto=static and there is no ip address error out
    */

    memset( usaP, 0, sizeof(usockaddr) );

    ipaddr = nvram_get("lan_ipaddr");
    proto = nvram_get("lan_proto");
    disable_router = nvram_match("router_disable","1");

    if ( !ipaddr && !proto)
       	{
    	perror("httpd:No LAN IP address and protocol specified.\n");
	return -1;
        }

    if ( !ipaddr && !strcmp(proto,"static") )
    	{
	perror("httpd:No static IP address specified for LAN.\n");
	return -1;
   	}

    if (!strcmp(proto,"dhcp") || disable_router)
    	usaP->sa_in.sin_addr.s_addr = htonl( INADDR_ANY );
    else
    	{
	if (!ipaddr)
		{
		perror("httpd:No IP address specified for LAN.\n");
		return -1;
		}
	if ( !inet_aton(ipaddr,&addr) )
		{
		perror("httpd:Invalid IP address specified for LAN.\n");
		return -1;
		}
	usaP->sa_in.sin_addr = addr;
	}

    usaP->sa.sa_family = AF_INET;
    usaP->sa_in.sin_port = htons( SERVER_PORT );

    listen_fd = socket( usaP->sa.sa_family, SOCK_STREAM, 0 );
    if ( listen_fd < 0 )
	{
	perror( "socket" );
	return -1;
	}
    (void) fcntl( listen_fd, F_SETFD, 1 );
    i = 1;
    if ( setsockopt( listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i) ) < 0 )
	{
	perror( "setsockopt" );
	return -1;
	}
    if ( bind( listen_fd, &usaP->sa, sizeof(struct sockaddr_in) ) < 0 )
	{
	perror( "bind" );
	return -1;
	}
    if ( listen( listen_fd, MAX_BACKLOG ) < 0 )
	{
	perror( "listen" );
	return -1;
	}
    return listen_fd;
    }

static int
auth_check( char* dirname, char* authorization )
    {
    char authinfo[500];
    char* authpass;
    int l;

    /* Is this directory unprotected? */
    if ( !strlen(auth_passwd) )
	/* Yes, let the request go through. */
	return 1;

    /* Basic authorization info? */
    if ( !authorization || strncmp( authorization, "Basic ", 6 ) != 0 ) {
	send_authenticate( dirname );
	return 0;
    }

    /* Decode it. */
    l = b64_decode( &(authorization[6]), (unsigned char *)authinfo, sizeof(authinfo) );
    authinfo[l] = '\0';
    /* Split into user and password. */
    authpass = strchr( authinfo, ':' );
    if ( authpass == (char*) 0 ) {
	/* No colon?  Bogus auth info. */
	send_authenticate( dirname );
	return 0;
    }
    *authpass++ = '\0';

    /* Is this the right user and password? */
    if ( strcmp( auth_userid, authinfo ) == 0 && strcmp( auth_passwd, authpass ) == 0 )
	return 1;

    send_authenticate( dirname );
    return 0;
    }

static void
send_authenticate( char* realm )
    {
    char header[10000];

    (void) snprintf(
	header, sizeof(header), "WWW-Authenticate: Basic realm=\"%s\"", realm );
    send_error( 401, "Unauthorized", header, "Authorization required." );
    }

static void
send_error( int status, char* title, char* extra_header, char* text )
    {
    send_headers( status, title, extra_header, "text/html" );
    (void) fprintf( conn_fp, "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\n<BODY BGCOLOR=\"#cc9999\"><H4>%d %s</H4>\n", status, title, status, title );
    (void) fprintf( conn_fp, "%s\n", text );
    (void) fprintf( conn_fp, "</BODY></HTML>\n" );
    (void) fflush( conn_fp );
    }

static void
send_headers( int status, char* title, char* extra_header, char* mime_type )
    {
    time_t now;
    char timebuf[100];

    (void) fprintf( conn_fp, "%s %d %s\r\n", PROTOCOL, status, title );
    (void) fprintf( conn_fp, "Server: %s\r\n", SERVER_NAME );
    now = time( (time_t*) 0 );
    (void) strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &now ) );
    (void) fprintf( conn_fp, "Date: %s\r\n", timebuf );
    if ( extra_header != (char*) 0 )
	(void) fprintf( conn_fp, "%s\r\n", extra_header );
    if ( mime_type != (char*) 0 )
	(void) fprintf( conn_fp, "Content-Type: %s\r\n", mime_type );
    (void) fprintf( conn_fp, "Connection: close\r\n" );
    (void) fprintf( conn_fp, "\r\n" );
    }

/*
* base64 encoder
*
* encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
char *b64_encode( unsigned char *src ,int src_len, unsigned char* space, int space_len)
{
    static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    unsigned char *out = space;
    unsigned char *in=src;
    int sub_len,len;
    int out_len;

   out_len=0;

   if (src_len < 1 ) return NULL;
   if (!src) return NULL;
   if (!space) return NULL;
   if (space_len < 1) return NULL;

   /* Required space is 4/3 source length  plus one for NULL terminator*/
   if ( space_len < ((1 +src_len/3) * 4 + 1) )return NULL;

   memset(space,0,space_len);

   for (len=0;len < src_len;in=in+3, len=len+3)
   {

    sub_len = ( ( len + 3  < src_len ) ? 3: src_len - len);

    /* This is a little inefficient on space but covers ALL the
       corner cases as far as length goes */
    switch(sub_len)
    	{
    	case 3:
        	out[out_len++] = cb64[ in[0] >> 2 ];
		out[out_len++] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ] ;
		out[out_len++] = cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] ;
		out[out_len++] = cb64[ in[2] & 0x3f ];
		break;
    	case 2:
        	out[out_len++] = cb64[ in[0] >> 2 ];
		out[out_len++] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ] ;
		out[out_len++] = cb64[ ((in[1] & 0x0f) << 2) ];
		out[out_len++] = (unsigned char) '=';
    		break;
    	case 1:
        	out[out_len++] = cb64[ in[0] >> 2 ];
		out[out_len++] = cb64[ ((in[0] & 0x03) << 4)  ] ;
		out[out_len++] = (unsigned char) '=';
		out[out_len++] = (unsigned char) '=';
		break;
    	default:
		break;
    		/* do nothing*/
    	}
   }
   out[out_len]='\0';
   return (char *)out;
}

/* Base-64 decoding.  This represents binary data as printable ASCII
** characters.  Three 8-bit binary bytes are turned into four 6-bit
** values, like so:
**
**   [11111111]  [22222222]  [33333333]
**
**   [111111] [112222] [222233] [333333]
**
** Then the 6-bit values are represented using the characters "A-Za-z0-9+/".
*/

static int b64_decode_table[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
    };

/* Do base-64 decoding on a string.  Ignore any non-base64 bytes.
** Return the actual number of bytes generated.  The decoded size will
** be at most 3/4 the size of the encoded, and may be smaller if there
** are padding characters (blanks, newlines).
*/
int
b64_decode( const char* str, unsigned char* space, int size )
    {
    const char* cp;
    int space_idx, phase;
    int d, prev_d=0;
    unsigned char c;

    space_idx = 0;
    phase = 0;
    for ( cp = str; *cp != '\0'; ++cp )
	{
	d = b64_decode_table[(int)*cp];
	if ( d != -1 )
	    {
	    switch ( phase )
		{
		case 0:
		++phase;
		break;
		case 1:
		c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
		if ( space_idx < size )
		    space[space_idx++] = c;
		++phase;
		break;
		case 2:
		c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
		if ( space_idx < size )
		    space[space_idx++] = c;
		++phase;
		break;
		case 3:
		c = ( ( ( prev_d & 0x03 ) << 6 ) | d );
		if ( space_idx < size )
		    space[space_idx++] = c;
		phase = 0;
		break;
		}
	    prev_d = d;
	    }
	}
    return space_idx;
    }

/* Simple shell-style filename matcher.  Only does ? * and **, and multiple
** patterns separated by |.  Returns 1 or 0.
*/
int
match( const char* pattern, const char* string )
    {
    const char* or;

    for (;;)
	{
	or = strchr( pattern, '|' );
	if ( or == (char*) 0 )
	    return match_one( pattern, strlen( pattern ), string );
	if ( match_one( pattern, or - pattern, string ) )
	    return 1;
	pattern = or + 1;
	}
    }

static int
match_one( const char* pattern, int patternlen, const char* string )
    {
    const char* p;

    for ( p = pattern; p - pattern < patternlen; ++p, ++string )
	{
	if ( *p == '?' && *string != '\0' )
	    continue;
	if ( *p == '*' )
	    {
	    int i, pl;
	    ++p;
	    if ( *p == '*' )
		{
		/* Double-wildcard matches anything. */
		++p;
		i = strlen( string );
		}
	    else
		/* Single-wildcard matches anything but slash. */
		i = strcspn( string, "/" );
	    pl = patternlen - ( p - pattern );
	    for ( ; i >= 0; --i )
		if ( match_one( p, pl, &(string[i]) ) )
		    return 1;
	    return 0;
	    }
	if ( *p != *string )
	    return 0;
	}
    if ( *string == '\0' )
	return 1;
    return 0;
    }

#ifndef DSLCPE
void
do_file(char *path, FILE *stream)
{
	FILE *fp;
	int c;

	if (!(fp = fopen(path, "r")))
		return;
	while ((c = getc(fp)) != EOF)
		fputc(c, stream);
	fclose(fp);
}

#endif
void
denied_file(char *path, FILE *stream)
{
    send_error( 400, "Bad Request", (char*) 0, "Illegal filename." );
}

static void
free_config()
{
    /* Free the list of hosts */
    while (hosts) {
        struct hostentry *tmp = hosts->next;
        if(hosts->domain) {
            free(hosts->domain);
            hosts->domain = NULL;
        }
        free(hosts);
        hosts = tmp;
    }
}

static void
load_config()
{
    FILE *f;
    char *line;
    char buff[MAXCONFLINE];

    /* Cleanup any prior config */
    free_config();

    /* Open the config file for read */
    f = fopen(conffile, "r");
    if (!f) {
        /* not really an error, since there might not be a need for a config file... */
        syslog(LOG_WARNING, "unable to read %s: %m", conffile);
    }
    else {
        syslog(LOG_INFO, "reading %s", conffile);
        while ((line = fgets(buff, MAXCONFLINE, f))) {
            char *token = strtok(line, " \t\n");
            struct hostentry *host;

            if (!token)
                continue;

            /* host entries are:
             *    host mysite.local mysite
             * to redirect http://mysite.local/bla.asp to http://192.168.1.1/mysite/bla.asp
            */
            if (strcmp(token, "host") == 0) {
                char *domain = strtok(NULL, " \t\n");
                char *subdir = strtok(NULL, " \t\n");
                size_t domainLen, subdirLen;

                if (!domain || !subdir)
                    continue;

                domainLen = strlen(domain);
                subdirLen = strlen(subdir);

                /* Allocate memory for the structure and strings */
                if (!(host = malloc(sizeof (struct hostentry))))
                    continue;
                if (!(host->domain = malloc(domainLen + 1))) {
                    free(host);
                    continue;
                }
                if (!(host->subdir = malloc(subdirLen + 1))) {
                    free(host->domain);
                    free(host);
                    continue;
                }

                /* Fill the struct and add it to the list (in reverse order) */
                syslog(LOG_INFO, "Host \"%s\" will redirect to \"%s\"", domain, subdir);
                strcpy(host->domain, domain);
                strcpy(host->subdir, subdir);
                host->subdirLen = subdirLen;
                host->next = hosts;
                hosts = host;
            }
        }

        fclose(f);
    }
}

static struct hostentry*
find_host( char *host )
{
    /* Search the host list for a match */
    struct hostentry *cur = hosts;
    while (cur) {
        syslog(LOG_DEBUG, "comparing \"%s\"==\"%s\"", host, cur->domain);
        if(cur->domain && strcasecmp(host, cur->domain) == 0) {
            return cur;
        }

        cur = cur->next;
    }

    return NULL;
}

static void
handle_request(void)
{
    char line[10000], *cur;
    char *method, *path, *protocol, *authorization, *boundary;
    char *cp;
    char *file;
    char *hostfile, *hosthdr;
    int len;
    struct mime_handler *handler;
    int cl = 0, flags;
    struct hostentry *host;

    /* Initialize the request variables. */
    authorization = boundary = NULL;
    host = NULL;
    hostfile = NULL;
    bzero( line, sizeof (line) );

    /* Parse the first line of the request. */
    if ( fgets( line, sizeof(line), conn_fp ) == (char*) 0 ) {
	send_error( 400, "Bad Request", (char*) 0, "No request found." );
	return;
    }
    method = path = line;
    strsep(&path, " ");
    if (path == NULL) {
	send_error( 400, "Bad Request", (char*) 0, "Can't parse request." );
	return;
    }
    while (*path == ' ') path++;
    protocol = path;
    strsep(&protocol, " ");
    if (protocol == NULL) {
	send_error( 400, "Bad Request", (char*) 0, "Can't parse request." );
	return;
    }
    while (*protocol == ' ') protocol++;
    cp = protocol;
    strsep(&cp, " ");
    if ( !method || !path || !protocol ) {
	send_error( 400, "Bad Request", (char*) 0, "Can't parse request." );
	return;
    }
    cur = protocol + strlen(protocol) + 1;

    /* Parse the rest of the request headers. */
    while ( fgets( cur, line + sizeof(line) - cur, conn_fp ) != (char*) 0 )
	{

	if ( strcmp( cur, "\n" ) == 0 || strcmp( cur, "\r\n" ) == 0 ){
	    break;
	}
	else if ( strncasecmp( cur, "Authorization:", 14 ) == 0 )
	    {
	    cp = &cur[14];
	    cp += strspn( cp, " \t" );
	    authorization = cp;
	    cur = cp + strlen(cp) + 1;
	    }
	else if ( strncasecmp( cur, "Host:", 5 ) == 0 )
	    {
	    cp = &cur[5];
	    cp += strspn( cp, " \t" );
        hosthdr = cp;
        cp += strcspn( cp, "\r\n\0" );
	    *cp = '\0';
        syslog(LOG_INFO, "Host: %s", hosthdr);
        host = find_host(hosthdr);
	    cur = cp + strlen(cp) + 1;
	    }
	else if (strncasecmp( cur, "Content-Length:", 15 ) == 0) {
		cp = &cur[15];
		cp += strspn( cp, " \t" );
		cl = strtoul( cp, NULL, 0 );
	}
	else if ((cp = strstr( cur, "boundary=" ))) {
            boundary = &cp[9];
	    for( cp = cp + 9; *cp && *cp != '\r' && *cp != '\n'; cp++ );
	    *cp = '\0';
	    cur = ++cp;
	}

	}

    /*
     * Fixed the Windows VISTA stop probing UPnP WFA device, if the httpd
     * doesn't respond to the http head request.
     */
    if ( strcasecmp( method, "head" ) == 0) {
	send_headers( 200, "Ok", (char *)0, "text/html" );
	return;
    }

    if ( strcasecmp( method, "get" ) != 0 && strcasecmp(method, "post") != 0 ) {
	send_error( 501, "Not Implemented", (char*) 0, "That method is not implemented." );
	return;
    }
    if ( path[0] != '/' ) {
	send_error( 400, "Bad Request", (char*) 0, "Bad filename." );
	return;
    }
    file = &(path[1]);
    len = strlen( file );
    if ( file[0] == '/' || strcmp( file, ".." ) == 0 || strncmp( file, "../", 3 ) == 0 || strstr( file, "/../" ) != (char*) 0 || strcmp( &(file[len-3]), "/.." ) == 0 ) {
	send_error( 400, "Bad Request", (char*) 0, "Illegal filename." );
	return;
    }
    if ( file[0] == '\0' || file[len-1] == '/' )
	file = "index.asp";

    /* prepend the host's subdir, if we have one */
    if (host) {
        hostfile = malloc(host->subdirLen + strlen(file) + 2);
        strcpy(hostfile, host->subdir);
        if(hostfile[host->subdirLen - 1] != '/')
            strcat(hostfile, "/");
        strcat(hostfile, file);
        syslog(LOG_INFO, "Host redirection: \"%s\" --> \"%s\"", file, hostfile);
        file = hostfile;
    }

    for (handler = &mime_handlers[0]; handler->pattern; handler++) {
	    if (match(handler->pattern, file)) {
		    if (handler->auth) {
			    handler->auth(auth_userid, auth_passwd, auth_realm);
			    if (!auth_check(auth_realm, authorization)) {
                    if(hostfile)
                        free(hostfile);
				    return;
                }
		    }
		    if (strcasecmp(method, "post") == 0 && !handler->input) {
			    send_error(501, "Not Implemented", NULL, "That method is not implemented.");
                if(hostfile)
                    free(hostfile);
			    return;
		    }
		    if (handler->input) {
			    handler->input(file, conn_fp, cl, boundary);
#if defined(linux)
			    if ((flags = fcntl(fileno(conn_fp), F_GETFL)) != -1 &&
				fcntl(fileno(conn_fp), F_SETFL, flags | O_NONBLOCK) != -1) {
				    /* Read up to two more characters */
				    if (fgetc(conn_fp) != EOF)
					    (void) fgetc(conn_fp);
				    fcntl(fileno(conn_fp), F_SETFL, flags);
			    }
#elif defined(__ECOS)
			    flags = 1;
			    if (ioctl(fileno(conn_fp), FIONBIO, (int) &flags) != -1) {
				    /* Read up to two more characters */
				    if (fgetc(conn_fp) != EOF)
					    (void) fgetc(conn_fp);
				    flags = 0;
				    ioctl(fileno(conn_fp), FIONBIO, (int) &flags);
			    }
#endif // endif
		    }
		    send_headers( 200, "Ok", handler->extra_header, handler->mime_type );
		    if (handler->output)
			    handler->output(file, conn_fp);
		    break;
	    }
    }

    if (!handler->pattern)
	    send_error( 404, "Not Found", (char*) 0, "File not found." );

    if(hostfile)
        free(hostfile);
}

#if defined(__ECOS)
#undef	fclose
extern int fclose(FILE *);
#endif // endif

static void
sig_handler( int sig )
{
    if (sig == SIGHUP)
        sighup = 1;
}

int main(int argc, char **argv)
{
	usockaddr usa;
	int listen_fd;
	int conn_fd;

	socklen_t sz = sizeof(usa);

	/* Ignore broken pipes */
	signal(SIGPIPE, SIG_IGN);

	/* Support HUP for config reload */
	sighup = 1; /* load config the first time through */
	signal(SIGHUP, sig_handler);

    /* If we received a parameter, take it as the path to the conf file */
    if(argc > 1) {
        conffile = argv[1];
    }

	/* Initialize listen socket */
	if ((listen_fd = initialize_listen_socket(&usa)) < 0) {
		fprintf(stderr, "%s:can't bind to any address\n",argv[0] );
		return errno;
	}

	/* Initialize internal structures */
	if (internal_init()){
		fprintf(stderr, "%s:error initializing internal structures\n",argv[0] );
		goto errout;
	}
#if !defined(DEBUG) && !defined(__ECOS)
	{
	FILE *pid_fp;
	/* Daemonize and log PID */
	if (daemon(1, 1) == -1) {
		perror("daemon");
		exit(errno);
	}
	if (!(pid_fp = fopen("/var/run/httpd.pid", "w"))) {
		perror("/var/run/httpd.pid");
		return errno;
	}
	fprintf(pid_fp, "%d", getpid());
	fclose(pid_fp);
	}
#endif // endif

	/* Loop forever handling requests */
	for (;;) {
		if ((conn_fd = accept(listen_fd, &usa.sa, &sz)) < 0) {
#ifdef	__ECOS
			if (errno == ECONNABORTED)
				continue;
#endif // endif
			perror("accept");
			goto errout;
		}
		if (!(conn_fp = fdopen(conn_fd, "r+"))) {
			close(conn_fd);
			perror("fdopen");
			return errno;
		}

        /* Reload config, if signaled */
        if (sighup) {
            load_config();
            sighup = 0;
        }

		handle_request();
		fflush(conn_fp);
		fclose(conn_fp);
		close(conn_fd);
	}

	shutdown(listen_fd, 2);
	close(listen_fd);
	return 0;

errout:
	close(listen_fd);
	return errno;
}
