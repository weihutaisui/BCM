/*
 * milli_httpd - pretty small HTTP server
 *
 *
 * $Id: httpd.h,v 1.2 2001/09/29 23:00:32 mhuang Exp $
 */

#ifndef _httpd_h_
#define _httpd_h_

#include <stdio.h>
#include "cms.h"
#include "cms_util.h"
#ifdef BUILD_WLAN
#include <wlcsm_lib_api.h>
#include <wlcsm_linux.h>
#endif

#if defined(DEBUG) && defined(DMALLOC)
#include <dmalloc.h>
#endif

#if ((defined(SUPPORT_HTTPD_SSL) || defined(SUPPORT_HTTPD_BASIC_SSL)) && defined(SUPPORT_OPENSSL))
#define SUPPORT_HTTPS
#endif   /* (SUPPORT_HTTPD_SSL || SUPPORT_HTTPD_BASIC_SSL) && SUPPORT_OPENSSL */

#if defined(SUPPORT_HTTPS)
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <bcm_sslconf.h>
extern BIO  *io;
#endif   /* SUPPORT_HTTPS */

/** Global var for exit-on-idle timeout.
 *
 * By using a variable instead of a constant define, we allow this
 * port to be changed by command line.  Useful for debugging.
 */
extern UINT32 exitOnIdleTimeout;


/** TCP port that httpd is listening on.
 *
 * HTTPD is traditionally on port 80.  HTTPS is on port 443
 */
#define UNKNOWN_SERVER_PORT -1
#ifdef DESKTOP_LINUX
#define HTTP_SERVER_PORT    CMS_DESKTOP_SERVER_PORT_OFFSET + HTTPD_PORT
#define HTTPS_SERVER_PORT   CMS_DESKTOP_SERVER_PORT_OFFSET + HTTPD_PORT_SSL
#else
#define HTTP_SERVER_PORT    HTTPD_PORT
#define HTTPS_SERVER_PORT   HTTPD_PORT_SSL
#endif /* DESKTOP_LINUX */

/** Global var for server port.
 *
 * By using a variable instead of a constant define, we allow this
 * port to be changed by command line.  Useful for debugging.
 */
extern SINT32 serverPort;


/** How many milliseconds to wait for the MDM lock.
 *
 * 6 seconds is probably just long enough to not annoy users
 * too much.  If we can't get the lock after 6 seconds, 
 * we will abort the operation, which means the user will lose 
 * his writes or we cannot see a web page.  Also, our target for
 * maximum lock hold time is 5 seconds.
 *
 */
#define HTTPD_LOCK_TIMEOUT  (6 * MSECS_IN_SEC)


/** The amount of time, in milliseconds, that the current session key is valid.
 *
 * This is used to secure the cgi interface from Cross Site Forgery attacks.
 */
#define HTTPD_SESSION_KEY_VALID (600 * MSECS_IN_SEC)


/** Global variable for httpd state.
 *
 * Used for reboot, upload, etc.  See WEB_STS_xxx defines.
 */
extern SINT32 glbStsFlag;


/** Handle to the messaging library. */
extern void *msgHandle;


/** A global var used by handler functions to indicate a need
 *  to write config to flash.
 */
extern UBOOL8 glbSaveConfigNeeded;


/* Basic authorization userid and passwd limit */
#define AUTH_MAX 64

/* Web status */
#define WEB_STS_ERROR      -1
#define WEB_STS_OK          0
#define WEB_STS_REBOOT      1
#define WEB_STS_RESTORE     2
#define WEB_STS_UPLOAD      3
#define WEB_STS_EXIT        4
#define WEB_STS_LAN_CHANGED_EXIT 5
#define WEB_STS_OMCI_DOWNLOAD    6


/* Regular file handler */
extern void do_file(char *path, FILE *stream);

/* Authentication file handler */
extern void do_auth(char *userid, char *passwd, char *realm,
                    char *sptPasswd, char *usrPasswd);

/* Embedded JavaScript handler */
extern void do_ej(char *path, FILE *stream);

/* CGI file handler */
extern void do_cgi(char *path, FILE *stream);

extern void do_selt_cgi(char *path, FILE *fs);

/** Length used for the filename variable, which gets passed around to various
 * functions.
 */
#define HTTPD_BUFLEN_10K 10000


/** Create a full path to the specified web page
 *
 * This function allows us to find the web page on DESKTOP_LINUX and 
 * modem builds.
 * 
 * @param path   (OUT) buffer to store the created full path.
 * @param pathLen (IN) Length of buf.
 * @param webpage (IN) The web page name, which will become the last element in
 *                     the full path.
 */
extern void makePathToWebPage(char *path, UINT32 pathLen, const char *webpage);


#ifdef BRCM_WLAN
extern void do_wl_cgi(char *path, FILE *stream);
#ifdef SUPPORT_WLAN_VISUALIZATION
extern void do_wl_json_get(char *path, FILE *fs);
extern void do_wl_vis_dbdownload(char *path, FILE *fs);
extern void do_wl_json_set(char *path, FILE *fs,int len,const char*boundry);
#endif
void do_wl_status_get(char *path, FILE *fs);
#endif

#ifdef SES
/* SecureEZSetup file handler */
extern void do_ezconfig_asp(char *path, FILE *stream);
/* SecureEZSetup setup apply handler */
extern void BcmWl_EZC_Apply(void);
/* SecureEZSetup restore default setting handler */
extern void BcmWl_EZC_Restore(void);
#endif

#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
/* SNTP CGI handler */
extern void do_sntp_cgi(char *path, FILE *stream);
#endif

#ifdef SUPPORT_IPP
/* IPP CGI handler */
extern void do_ipp_cgi(char *path, FILE *stream);
#endif

#ifdef DMP_X_BROADCOM_COM_DLNA_1
/* DMS CGI handler */
extern void do_dms_cgi(char *path, FILE *stream);
#endif

#ifdef SUPPORT_TOD
/* ToD CGI handler */
extern void do_tod_cgi(char *path, FILE *stream);
#endif

/* Test CGI file handler */
extern void do_test_cgi(char *path, FILE *stream);

/* Test CGI file handler */
extern void do_cmd_cgi(char *path, FILE *stream);
extern UBOOL8  isPageAllowed(const char *filename, UINT32 autoLevel);

/* Generic MIME type handler */
#ifdef BRCM_WLAN 
struct cpe_mime_handler {
#else
struct mime_handler {
#endif
	char *pattern;
	char *mime_type;
	void (*output)(char *path, FILE *stream);
	void (*auth)(char *userid, char *passwd, char *realm,
                char *sptPasswd, char *usrPasswd);
	void (*set)(char *path, FILE *stream,int len,const char *boundary); // SET operation
};
#ifndef BRCM_WLAN 
extern struct mime_handler mime_handlers[];
#else
extern struct cpe_mime_handler cpe_mime_handlers[];
struct ej_handler {
	    char *pattern;
	    int (*output)(int eid, FILE *wp, int argc,char **argv);
};
extern struct ej_handler ej_handlers[];

/* wireless router mime handler is defined differently than BCA httpd */
struct mime_handler {
	char *pattern;
	char *mime_type;
	char *extra_header;
	void (*input)(char *path, FILE *stream, int len, char *boundary);
	void (*output)(char *path, FILE *stream);
	void (*auth)(char *userid, char *passwd, char *realm);
};
extern struct mime_handler mime_handlers[]; /*  wl router's mime type to handle */
extern int wl_handle_request(FILE* conn_fp,char *file,char *method, char *authorization,unsigned char *isPasswordProtectedPage,int cl) ;
#endif

/* CGI helper function */
extern void parse_cgi(char *path, void (*set)(char *name, char *value));

/* CGI upload handler, in cgi_upload.c */
extern int  do_upload_pre(FILE *stream, int content_len, int upload_type);
extern void do_upload_post(void);

/** Global variable for upload status.
 */
extern CmsRet glbUploadStatus;

#define WEB_UPLOAD_IMAGE           (1) /* for image upload           */
#define WEB_UPLOAD_SETTINGS        (2) /* for setting upload         */
#define WEB_PMD_UPLOAD_CALIBRATION (3) /* for PMD calibration upload */


/** Max length of boundary marker in POST method.
 *
 * There were some customer reports that 256 was not long enough
 * and increasing it fixed the problem, but I don't see why the
 * boundary length needs to be more than 256.  (Typically, even
 * 70 bytes should be enough).
 */
#define POST_BOUNDARY_LENGTH   (256)


/** Global variable for boundary string in the header.
 */
extern char glbBoundary[POST_BOUNDARY_LENGTH];



/* Embedded JavaScript function handler */
#ifdef BRCM_WLAN 
struct cpe_ej_handler {
#else
struct ej_handler {
#endif
	char *pattern;
	void (*output)(int argc, char **argv, FILE *stream);
};

#ifdef BRCM_WLAN 
extern struct cpe_ej_handler cpe_ej_handlers[];
#else
extern struct ej_handler ej_handlers[];
#endif

/** Main body of httpd.
 *
 * @param openServerSocket (IN) If true, httpd should open its own server socket.
 *                              Normally, httpd inherits its server socket from smd.
 *
 * @return 0 for normal exit, otherwise error code.
 */
int web_main(UBOOL8 openServerSocket);

UBOOL8 httpd_gets(char *line, int len);

#ifdef BRCM_WLAN
/* CGI helper functions */
extern void init_cgi(char *query);
extern char * get_cgi(char *name);
extern void set_cgi(char *name, char *value);
extern int count_cgi(void);

/* GoAhead 2.1 compatibility */
typedef FILE * webs_t;
typedef char char_t;
#define T(s) (s)
#define __TMPVAR(x) tmpvar ## x
#define _TMPVAR(x) __TMPVAR(x)
#define TMPVAR _TMPVAR(__LINE__)
#define websWrite(wp, fmt, args...) ({ int TMPVAR = fprintf(wp, fmt, ## args); fflush(wp); TMPVAR; })
#define websError(wp, code, msg, args...) fprintf(wp, msg, ## args)
#define websHeader(wp) fputs("<html lang=\"en\">", wp)
#define websFooter(wp) fputs("</html>", wp)
#define websDone(wp, code) fflush(wp)
#define websGetVar(wp, var, default) (get_cgi(var) ? : default)
#define websSetVar(wp, var, value) set_cgi(var, value)
#define websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query) ({ do_ej(path, wp); fflush(wp); 1; })
#define websWriteData(wp, buf, nChars) ({ int TMPVAR = fwrite(buf, 1, nChars, wp); fflush(wp); TMPVAR; })
#define websWriteDataNonBlock websWriteData
#endif

#endif /* _httpd_h_ */
