/*
 * Copyright (c) 2003-2012 Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 * $Id: upnp.h,v 1.19.20.2 2003/10/31 21:31:36 mthawani Exp $
 */
 
#ifndef _upnp_h_
#define _upnp_h_

#include <time.h>
#include <stdarg.h>
#include <netinet/in.h>  
#include <arpa/inet.h> 
#include <sys/types.h>   
#include <sys/socket.h> 

#include "tr64defs.h"
#include "uio.h"

#define  SSDP_IP   "239.255.255.250"
#define  SSDP_PORT 1900

/* number of seconds to wait before refershing device advertisements. */
#if defined(BCMDBG)
//#define SSDP_REFRESH   40
#define SSDP_REFRESH   5000
#else
/** UDA specifies that lease time must be no less than 30 minutes;
 * devices compliant with TR64 DCP requires lease time to be
 * no less than 24 hours (86400 seconds) and should be no less than
 * 2 weeks (1209600) 
 */
#define SSDP_REFRESH   1209600
#endif
/** resend advertisement less than 1/2 of the lease time 
 */
#define UPNP_REFRESH_MAX   ((SSDP_REFRESH/2) - 1)
#define UPNP_REFRESH_MIN   ((SSDP_REFRESH/4))

#define NOTIFY_RECEIPT_TIMEOUT 30
#define HTTP_REQUEST_TIMEOUT   15

#if !defined(FALSE) || !defined(TRUE)
#define TRUE 1
#define FALSE (!TRUE)
#endif

#define UPNP_BUFSIZE 2500
#define MAX_VALUE_LEN 33

#define ARRAYSIZE(a)  (sizeof(a)/sizeof(a[0]))

#ifdef UPNP_DEBUG
#define IFDEBUG(a)  a
#else
#define IFDEBUG(a)  
#endif

#define SERVER "LINUX/2.4 UPnP/1.0 BRCM400/1.0" /* server field for ssdp advertisement calls */
#define UPNP_MAX_VAL_LEN 100

#define MAX_HTTP_CONNECTIONS 5

struct Service;
struct Device;
struct iface;
struct net_connection;
struct VarTemplate;
struct StateVar;
struct var_entry;

struct Device;
struct Service;
struct DeviceTemplate;
struct DerviceTemplate;
struct Action;
struct Param;

typedef struct Device Device, *PDevice;
typedef struct Service Service, *PService;
typedef struct DeviceTemplate DeviceTemplate, *PDeviceTemplate;
typedef struct ServiceTemplate ServiceTemplate, *PServiceTemplate;
typedef struct Action Action, *PAction;
typedef struct Param Param, *PParam;

typedef enum { SSDP_REPLY = 2, SSDP_ALIVE=1, SSDP_BYEBYE=0 } ssdp_t;
typedef enum { CONNECTION_RECV = 0, CONNECTION_DELETE = 1 } caction_t;
typedef enum { SERVICE_CREATE = 0, SERVICE_DESTROY = 1 } service_state_t;
typedef enum { DEVICE_CREATE = 0, DEVICE_DESTROY = 1 } device_state_t;

typedef unsigned int u_int32;
typedef unsigned short u_int16;
typedef unsigned char u_int8;

typedef void (*PFDEVFOREACH)(PDevice, va_list);
typedef int (*PFDEVINIT)(PDevice, device_state_t, va_list);  
typedef void (*PFDEVXML)(PDevice, UFILE*);  
typedef void (*PFSVCXML)(PService, UFILE*);  

typedef void (*PFSVCFOREACH)(PService, va_list);
typedef int (*PFSVCINIT)(PService, service_state_t);  

typedef int (*PFVACTION)(UFILE *, PService, PAction, struct var_entry *, int);
typedef int (*PFGETVAR)(PService, int);
typedef int (*PFSETVAR)(PService, const struct VarTemplate *, struct StateVar *);
typedef void (*CONNECTION_HANDLER)(caction_t, struct net_connection *, void *);

typedef  unsigned long vartype_t;

#define VAR_USHORT	(1 << 0)
#define VAR_ULONG	(1 << 1)
#define VAR_STRING	(1 << 2) 
#define VAR_BOOL	(1 << 3)
#define VAR_SHORT	(1 << 4)
#define VAR_LONG	(1 << 5)
#define VAR_BYTE	(1 << 6) 
#define VAR_UBYTE	(1 << 7)
#define VAR_UUID  (1 << 8)

#define VAR_CHANGED	(1 << 9)
#define VAR_EVENTED	(1 << 10)

#define VAR_RANGE	(1 << 11)
#define VAR_LIST	(1 << 12)



//#define VAR_TYPE_MASK	0x000000FF
#define VAR_TYPE_MASK	0x000001FF
#define VAR_TYPE_SHIFT	12


struct Param {
    char *name;
    int related;
    int flags;
    char            *value;
};

struct Action {
    char *name;
    PFVACTION func;
    Param *params;
};

#define VAR_IN  1
#define VAR_OUT 2


typedef struct {
    char *minimum;
    char *maximum;
    char *step;
} allowedValueRange, *PallowedValueRange;

typedef char *allowedValueList[];

typedef union {
    char **    list;
    allowedValueRange   *range;
} allowedValue, *PallowedValue;


typedef struct var_entry {
    char            *name;
    char            *value;
    vartype_t            type;
} var_entry_t, *pvar_entry_t;

typedef struct VarTemplate {
    const char *name;
    const char *value;
    unsigned long flags;
    allowedValue allowed;
} VarTemplate, *PVarTemplate;

typedef struct Subscription {
    char *      cb_host;
    char *      cb_path;
    int         cb_port;
    char        sid[50];
    unsigned int	event_key;
    struct sockaddr_in	addr;
    int         addrlen;
    int         sock;                      // socket used to send out last notify 
    time_t      expires;
    // timer_t  hevent;   /* event handle for triggering initial evented variable update. */
    struct Subscription *next;
} Subscription, *PSubscription;

/* Structure for storing Service identifiers and state table */
struct ServiceTemplate {
    char	*name;     /* name of the service, e.g. "WLANHostConfigManagement" */
    
    /* pointers to functions. May be NULL. */
    PFSVCINIT    svcinit;	/* service initialization */
    PFGETVAR     getvars;   	/* state variable handler */
    PFSVCXML     svcxml;	/* xml generator */

    int		 nvariables;
    VarTemplate	*variables;
    PAction 	*actions;
    int		 count;
    char	*serviceid;
    char	*schema;
};

typedef struct StateVar {
    unsigned long            flags;
    char            value[UPNP_MAX_VAL_LEN];
}  __attribute__ ((packed)) StateVar, *PStateVar;

struct Service { 
    PServiceTemplate	template;
    PDevice		device;
    Subscription    *subscriptions;
    struct Service  *next;
    StateVar        *vars;
    u_int32	    flags;
    void            *opaque;   // opaque user data
    int instance;
};

struct DeviceTemplate {
    char *		type;
    char *		udn;
    PFDEVINIT		devinit;   
    PFDEVXML		devxml;
    int			nservices;
    PServiceTemplate *	services;
    int			ndevices;
    PDeviceTemplate	devicelist;  // templates for sub-devices
    char *schema;
};

/*
  
  udn - the device definition contains a unique string (a unique
  device number) that should be substituted in the XML device
  description, and in device advertisements.  This string looks like a
  UUID (or GUID) and it generated at runtime.  Ideally this string is
  unuque across all devices in all routers in the world.  It should be
  generated based upon the lac address of the publically accessible
  interface, but that work is not completed.
  
*/
struct Device {
    DeviceTemplate  *template;
    char            *udn;
    Service         *services;
    struct Device   *parent;	// parent device (NULL if root)
    struct Device   *subdevs;	// subdevices
    struct Device   *next;	// link to next device on global device list
    void            *opaque;	// opaque user data
    char            *friendlyname;
    int             instance;
};

#define ISROOT(pd) (pd->parent == NULL)

typedef void (*event_callback_t)(timer_t, void *);

struct iface {
    struct iface *next;

    char *ifname;

    // The address of the HTTP server
    struct in_addr inaddr;

    struct net_connection *http_connection;
    struct net_connection *ssdp_connection;
};

/* 
   Used to store information about network connections that have not
   completed yet.  This might include http connections that have been
   accepted or partially read, but which have not yet been completed by
   the client.  It may also include connections for GENA notifications
   which have been send but not yet acknowledged.  
*/

struct net_connection {
    int			    fd;
    time_t		    expires;  // 0 means never expires.
    CONNECTION_HANDLER	    func;
    void *		    arg;
    struct net_connection * next;
};

typedef uint soap_error_t;

typedef struct Error {
    soap_error_t error_code;
    char *error_string;
} Error, *PError;


#define MATCH_PREFIX(a, b)  (strncmp((a),(b),sizeof(b)-1)==0)
#define IMATCH_PREFIX(a, b)  (strncasecmp((a),(b),sizeof(b)-1)==0)

typedef enum {
    HTTP_OK = 200, 
    HTTP_BAD_REQUEST = 400,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_PRECONDITION = 412,
    HTTP_SERVER_ERROR = 500, 
    HTTP_NOT_IMPLEMENTED = 501
} http_error_t;


struct http_connection {
    struct net_connection   net;
    char		    *buf;
    u_int16		 bytes_recvd;
    u_int16		 maxbytes;
    UFILE		 *up;
    authState   state;
    sessionAuth auth;
    char        *method;
    char        setPasswordFlag;
};

extern void soap_error(UFILE *, soap_error_t error_code);
extern void soap_success( UFILE *up, PService psvc, PAction ac, pvar_entry_t args, int nargs );
extern void http_error(UFILE *, http_error_t error_code);
extern void http_response(UFILE *, http_error_t, const char *, int );
extern void soap_register_errors(Error *errors);

#ifdef VXWORKS
#include "wsIntrn.h"
#include "utils.h"

#define log printf

#elif defined(linux)
typedef void (*VOIDFUNCPTR)(void);
#define log printf
#endif

typedef void (*voidfp_t)(void);

#define HTTP_BUF_LOWATER 200
#define HTTP_BUFSIZE (2*HTTP_BUF_LOWATER)
#define HTTP_BUF_INCREMENT (2*HTTP_BUF_LOWATER)

#if defined(BCMDBG)
extern void upnp_dumpaction(PService , char *, pvar_entry_t , int );
extern void upnp_dumpresponse(const char *, const char *, pvar_entry_t , int );
#endif

extern int upnp_main(char *ifname);
extern void mark_changed(PService psvc, int varindex);
extern timer_t enqueue_event(struct itimerspec *value, event_callback_t func, void *arg);
extern void update_all_subscriptions(PService psvc);
extern PDevice device_iterator(PDevice pdev);
extern PDevice rootdev(PDevice pdev);
extern int NotImplemented(UFILE *, PService psvc, PAction ac, pvar_entry_t args, int nargs);
extern int DefaultAction(UFILE *, struct Service *psvc, PAction ac, pvar_entry_t args, int nargs);

extern int unique;

     // extern void forall_devices(PFDEVFOREACH func, ...);
     // extern void forall_services(PFSVCFOREACH func, ...);

#define forall_devices(pdev) \
  for (pdev = device_iterator(NULL); pdev; pdev = device_iterator(pdev)) 

#define forall_services(pdev, psvc) \
      for (psvc = pdev->services; psvc; psvc = psvc->next) 

#define	PHY_TYPE_A		0
#define	PHY_TYPE_B		1
#define	PHY_TYPE_G		2

#define SOAP_INVALIDACTION              401
#define SOAP_INVALIDARGS                402
#define SOAP_OUTOFSYNC                  403
#define SOAP_INVALIDVAR                 404
#define SOAP_ACTIONFAILED               501
#define SOAP_ARGUMENTVALUEINVALID       600
#define SOAP_ARGUMENTVALUEOUTOFRANGE    601
#define SOAP_OPITIONALACTIONNOTIMPLEMENTED  602
#define SOAP_OUTOFMEMEORY                   603
#define SOAP_HUMANINTERVENTIONREQUIRED      604
#define SOAP_STRINGARGUMENTTOOLONG          605
#define SOAP_ACTIONNOTAUTHORIZED            606
#define SOAP_VALUEALREADYSPECIFIED          701
#define SOAP_VALUESPECIFIEDISINVALID        702
#define SOAP_INACTIVECONNECTIONSTATEREQUIRED 703
#define SOAP_CONNECTIONSETUPFAILED          704
#define SOAP_CONNECTIONSETUPINPROGRESS      705
#define SOAP_CONNECTIONNOTCONFIGURED	706 
#define SOAP_DISCONNECTINPROGRESS	707 
#define SOAP_INVALIDLAYER2ADDRESS           708
#define SOAP_INTERNETACCESSDISABLED         709
#define SOAP_INVALIDCONNECTIONTYPE	710 
#define SOAP_CONNECTIONALREADYTERMNATED	    711 
#define SOAP_NULLVALUEATSPECIFIEDARRAYINDEX 712
#define SOAP_SPECIFIEDARRAYINDEXINVALID	713 
#define SOAP_NOSUCHENTRYINARRAY		714 
#define SOAP_WILDCARDNOTPERMITTEDINSRCIP    715
#define SOAP_WILDCARDNOTPERMITTEDINEXTPORT  716
#define SOAP_CONFLICTINMAPPINGENTRY	718 
#define SOAP_ACTIONDISALLOWEDWHENAUTOCONFIGENABLED  719
#define SOAP_INVALIDDEVICEUUID                      720
#define SOAP_INVALIDSERVICEID                       721
#define SOAP_INVALIDCONNSERVICESELECTION            723
#define SOAP_SAMEPORTVALUESREQUIRED                 724
#define SOAP_ONLYPERMANENTLEASESSUPPORTED	725
#define SOAP_REMOTEHOSTONLYSUPPORTSWILDCARD         726
#define SOAP_EXTERNALPORTONLYSUPPORTSWILDCARD       727
#define SOAP_INVALIDCHANNEL                         728
#define SOAP_INVALIDMACADDRESS                      729
#define SOAP_INVALIDDATATRANSMISSIONRATES           730
#define SOAP_INVALIDWEPKEY                          731
#define SOAP_NOWEPKEYISSET                          732
#define SOAP_NOPSKKEYISSET                          733
#define SOAP_NOEAPSERVER                            734
#define SOAP_SETMACADDRESSNOTPERMITTED              897
#define SOAP_WRITEACCESSDISABLED                    898
#define SOAP_SESSIONIDEXPIRED                       899

#endif /* _upnp_h_ */
