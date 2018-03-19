/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#ifndef _APPDEFS_H_
#define _APPDEFS_H_

/* Buffer sizes */

/** Maximum chunked buffer size.
 * 
 * 1MB should be enough to hold the largest possible chunked message
 * that we can receive.  Note this constant is not used for firmware
 * download.
 */
#define MAXWEBBUFSZ     (1024 * 1024) 

/** Maximum image download size for chunk encoded downloads.
 * Chunk encoded downloads do not have a content length, so we have to make
 * our best guess as to how big of a buffer to allocate before starting
 * the download.  The size really depends on customer's expectation of
 * image sizes, physical memory available on the system, and flash size.
 */
#define MAX_IMAGE_BUF_SIZE  (7 * 1024 * 1024)


/* compile time options */
/* xml debug output options */
//#define      DUMPSOAPOUT       /* DUMP xml msg sent to ACS to stderr */
//#define      DUMPSOAPIN          /* DUMP xml msg from ACS to stderr */
/* xml formatting options */
/* #define    OMIT_INDENT     */  /* don't intend xml tag lines - smaller messages */

/* use openSSL lib to support https: */
/*#define    USE_SSL  */


/* Allow Reboot and Factory Reset on ACS disconnect error */
#define ALLOW_DISCONNECT_ERROR   /* acsDisconnect will call reboot and reset clean up*/
                                 /* even if there is an ACS disconnect error */
/* Authentication options */
#define ALLOW_AUTH_RECONNECT     /* Some web servers such as Apache will close */
                                 /* the connection when sending a 401 status     */
                                 /* This allows the CPE to close the current connection */
                                 /* and reconnect to the server with the Authorization */
                                 /* header in the first POST message. The CPE will */
                                 /* attempt this if the connection is closed following */
                                 /* the 401 status from the server */
                                 /* Undefining this will prohibit the CPE from sending */
                                 /* the Authorization on a new connection */

/* Generic compile time flags combination may be defined for specific ACS below */

#define GENERATE_SOAPACTION_HDR  /* generate SOAPAction header in POST reqs*/

/* TR-069 schema flags */
//#define SUPPRESS_SOAP_ARRAYTYPE      /* suppress generation of soap-env arraytype */
                           /* such as
                              <Event SOAP-END:arrayType="cwmp:EventStruct[x]">
                              SUPPRESS_SOAP_ARRAYTYPE generates
                               <Event>
                           ***/

/* ACS Connection Initiation */
#define ACSREALM        "IgdAuthentication"
#define ACSDOMAIN       "/"



/* Timer valuse */
                              
#define ACSINFORMDELAY     500      /* initial delay (ms) after pgm start before */
                                    /* sending inform to ACS */
#define CHECKWANINTERVAL   (60*1000) /* check wan up */
#define ACSRESPONSETIME    (180*1000) /* MAX Time to wait on ACS response */

/* Retry intervals for ACS connect failures    */
/* Retry time decays by 
 * CONN_DECAYTIME*<consecutivefailures> upto a
 * maximum of CONNECT_DECAYTIME*CONNDECAYMAX
 */ 
#define CONN_DECAYMAX   6     /* maximum number for decaying multiple */
#define CONN_DECAYTIME  10    /* decay time per multiple */


/** On tr69c startup, max number of milliseconds to wait for the
 *  first message from smd.
 *
 * When smd starts tr69c, it might send a message to tr69c telling it
 * why it started it.  I'm afraid if I make this too long, it will cause
 * problems when smd starts tr69c due to connection request on tr69c
 * server socket.  If I make it too short, there could be a race 
 * condition between smd sending the first message and tr69c reading.
 */
#define BOOTUP_MSG_RECEIVE_TIMEOUT  150

/** The number of milliseconds to wait for the MDM lock.
 * 
 * In setting this timeout, consider the following scenario: httpd writes
 * to a parameter that has active notification attribute set.  httpd sends
 * notification to tr69c.  Meanwhile, httpd is writing the new MDM changes
 * to the flash, and various debug messages are coming out on the serial 
 * console port (httpd is holding the write lock during this time).
 * tr69c wakes up and tries to grab the write lock, it must wait
 * long enough for httpd to finish all of its activities and release the
 * write lock.
 */
#define TR69C_LOCK_TIMEOUT        (6*MSECS_IN_SEC)


/** An identifer we use when registering for periodic inform delayed messages
 *  from smd.
 */
#define PERIODIC_INFORM_TIMEOUT_ID   (0x11114455)


/** Original code uses a small delay timer for tiggering actions to
 *  terminate the tr69c process.  Not clear why there has to be a delay.
 *  But we define one also, at 250ms.
 */
#define DELAYED_TERMINAL_ACTION_DELAY   250

                                
 
/* Constants                           */

#define USER_AGENT_NAME    "BCM_TR69_CPE_04_00"

#define TR69_DIR        "/var/tr69"
#define TR69_PID_FILE   "/var/tr69/tr69pid"

#define SHELL           "/bin/sh"

#define USE_CERTIFICATES
#ifdef ORIGINAL_OPENSSL
/* ACS Server Certificate File path */
#define	ROOT_CERT_FILE	"/var/cert/acscert.cacert"
#else // BCM SSL
#define	ROOT_CERT_FILE	"/etc/acscert.cacert"
#endif
#define	CERT_PATH	"/var/cert"
/* CPE Client Certificate File path */
#define	CLIENT_CERT_FILE	"/var/cert/cpecert.cert"
/* CPE Client Private Key File path */
#define	CLIENT_PRIVATE_KEY_FILE	"/var/cert/cpecert.priv"

#ifdef USE_CERTIFICATES
#define ACS_CIPHERS     "RSA:DES:SHA+RSA:RC4:SAH+MEDIUM"
#else
//#define ACS_CIPHERS "EXPORT40:SSLv3"
#define ACS_CIPHERS     "SSLv3"
#endif


 
/************************************************************/
/* compile time conditions for specific ACS                 */
/* Uncomment the required definition                        */ 
/************************************************************/
//#define SUPPORT_ACS_CISCO
//#define SUPPORT_ACS_GS
//#define SUPPORT_ACS_DIMARK
//#define SUPPORT_ACS_PIRELLI
//#define SUPPORT_ACS_2WIRE
//#define SUPPORT_ACS_SUPPORT_SOFT_TOT

/* set conditional compile flags based on ACS choice */

#ifdef SUPPORT_ACS_CISCO
#define FORCE_NULL_AFTER_INFORM      /* Set timer to force null http after sending Inform*/
#define SUPPRESS_EMPTY_PARAM         /* Cisco doesn't handle <param></param> form */
/*#define SUPPRESS_XML_NEWLINES */  /* replaces \n with space to avoid cisco tool parser problem*/                                        
#endif /* SUPPORT_ACS_CISCO */

/* #ifdef */ /* next ACS ????? */

/*#endif*/   /*        */

#endif /* _APPDEFS_H_ */
