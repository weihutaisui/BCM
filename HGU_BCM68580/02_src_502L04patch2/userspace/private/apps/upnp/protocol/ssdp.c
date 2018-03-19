/*
 *
 * <:copyright-BRCM:2012:proprietary:standard
 * 
 *    Copyright (c) 2012 Broadcom 
 *    All Rights Reserved
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
*/

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"

// #include <bits/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>

extern struct iface *global_lans;
extern struct net_connection *net_connections;
extern PDevice root_devices;

extern char *strip_chars(char *str, char *reject);
extern PDevice find_dev_by_udn(char *udn);
extern int osl_join_multicast(struct iface *pif, int fd, ulong ipaddr, ushort port);

static void advertise_device(PDevice pdev, ssdp_t sstype, struct iface *pif, struct sockaddr *addr, int addrlen);
static void process_msearch(char *, struct iface *, struct sockaddr *, int);
static void ssdp_packet( UFILE *, ssdp_t, char * ntst, char *Usn, const char * location, int Duration);

const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";

void send_advertisements(ssdp_t sstype);
void ssdp_receive(caction_t flag, struct net_connection *nc, struct iface *pif);

/* create a UDP socket bound to the SSDP multicast address. */
struct net_connection *make_ssdp_socket(struct iface *pif)
{
    struct net_connection *c = NULL;
    int                fd;
    u_char             ttl;
    
    fd = socket( AF_INET, SOCK_DGRAM, 0 );
    UPNP_SOCKET(("%s: socket returns %d\n", __FUNCTION__, fd));
    if (fd < 0) {
	goto error;
    }

    if (!osl_join_multicast(pif, fd, inet_addr(SSDP_IP), SSDP_PORT)) 
	goto error;
	
    // set the multicast TTL
    ttl = 4;
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl))) {
	goto error;
    }
    
    c = (struct net_connection *) cmsMem_alloc(sizeof(struct net_connection), ALLOC_ZEROIZE);
    if (c == NULL) 
       goto error;
    
    
    c->fd = fd;
    c->expires = 0;  // never expires.
    c->func = (CONNECTION_HANDLER) ssdp_receive;
    c->arg = pif;
    
    c->next = net_connections;
    net_connections = (struct net_connection *) c;
    
    /* a successful return ... */
    return c;

 error:
    UPNP_ERROR(("make_ssdp_socket failed\n"));
    UPNP_SOCKET(("%s: close %d\n", __FUNCTION__, fd));
    close(fd);
    fd = -1;
    return NULL;

}

void ssdp_receive(caction_t flag, struct net_connection *nc, struct iface *pif)
{
    char buf[UPNP_BUFSIZE];
    struct sockaddr_in srcaddr;
    int nbytes, addrlen;
    
    UPNP_TRACE((__FUNCTION__, "\n"));
    if (flag == CONNECTION_RECV) {
	addrlen = sizeof(srcaddr);
	memset(&srcaddr, 0, addrlen);

	//memset(buf, 0, sizeof(buf));
	nbytes = recvfrom(nc->fd, buf, sizeof(buf), 0, (struct sockaddr*)&srcaddr, (socklen_t *)&addrlen);
	buf[nbytes] = '\0';
	if (MATCH_PREFIX(buf, "M-SEARCH * HTTP/1.1")) {
	    process_msearch(buf, pif, (struct sockaddr *)&srcaddr, addrlen);
	} 
	else if (MATCH_PREFIX(buf, "NOTIFY ")) 
	    ;
	else {
	    UPNP_INFORM(("unknown multicast message:\n%s\r\n", buf));
	}
    } else if (flag == CONNECTION_DELETE) {
	close(nc->fd);
	cmsMem_free(nc);
    }
}

static void process_msearch_all(char *st, struct iface *pif, struct sockaddr *srcaddr, int addrlen)
{
    PDevice pdev;

    UPNP_TRACE((__FUNCTION__, "\n"));
    forall_devices(pdev) {
	advertise_device(pdev, SSDP_REPLY, pif, srcaddr,  addrlen);
    }
}

static const char *location(PDevice pdev, struct iface *pif)
{
    static char location[100];
    const unsigned char *bytep = (const unsigned char *) &(pif->inaddr.s_addr);

    snprintf(location, sizeof(location), "http://%d.%d.%d.%d:%d/dyndev/%s", 
	     bytep[0], bytep[1], bytep[2], bytep[3], HTTP_PORT, rootdev(pdev)->udn);

    return location;
}

static void process_msearch_rootdevice(char *st, struct iface *pif, struct sockaddr *srcaddr, int addrlen)
{
    UFILE *upkt;
    char tmpbuf[100];
    PDevice pdev;
    
    UPNP_TRACE((__FUNCTION__, "\n"));
    if ((upkt = usopen(NULL, 0))) {
	
	for (pdev = root_devices; pdev; pdev = pdev->next) {
	    snprintf(tmpbuf, sizeof(tmpbuf), "%s::upnp:rootdevice", pdev->udn);

	    // build the SSDP packet
	    ssdp_packet( upkt, SSDP_REPLY, st, tmpbuf, location(pdev, pif), SSDP_REFRESH);
	    
       if (sendto(pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), 0, srcaddr, addrlen) < 0)
          UPNP_ERROR(("sendto failed errno=%d :fd=%d,buf=%p,buflen=%d,addrp=%p, addrlen=%d\n", errno, 
                   pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), srcaddr, addrlen));
	    uflush(upkt); 	// reset the packet buffer before we build another.
	}
	uclose(upkt);
    }
}


static void process_msearch_uuid(char *st, struct iface *pif, struct sockaddr *srcaddr, int addrlen)
{
    UFILE *upkt;
    PDevice pdev;

    UPNP_TRACE((__FUNCTION__, "\n"));
    if ((upkt = usopen(NULL, 0))) {
	if ((pdev = find_dev_by_udn(st)) != NULL) {

	    // build the SSDP packet
	    ssdp_packet( upkt, SSDP_REPLY, pdev->udn, pdev->udn,
			 location(pdev, pif), SSDP_REFRESH);

       if (sendto(pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), 0, srcaddr, addrlen) < 0)
          UPNP_ERROR(("sendto failed errno=%d :fd=%d,buf=%p,buflen=%d,addrp=%p, addrlen=%d\n", errno, 
                   pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), srcaddr, addrlen));
	}    

	uclose(upkt);
    }
}


static void process_msearch_devicetype(char *st, struct iface *pif, struct sockaddr *addr, int addrlen)
{
    UFILE *upkt;
    PDevice pdev;
    char tmpbuf[100];

    UPNP_TRACE((__FUNCTION__, "\n"));
    if ((upkt = usopen(NULL, 0))) {
	forall_devices(pdev) {
	    if (strcmp(pdev->template->type, st) == 0) {
		snprintf(tmpbuf, sizeof(tmpbuf), "%s::%s", pdev->udn, pdev->template->type);

		// build the SSDP packet
		ssdp_packet( upkt, SSDP_REPLY, pdev->template->type, tmpbuf,
			     location(pdev, pif), SSDP_REFRESH);
      if (sendto(pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), 0, addr, addrlen) < 0)
         UPNP_ERROR(("sendto failed errno=%d :fd=%d,buf=%p,buflen=%d,addrp=%p, addrlen=%d\n", errno, 
                  pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), addr, addrlen));
		uflush(upkt); 	// reset the packet buffer before we build another.
	    }
	}

	uclose(upkt);
    }
}


static void process_msearch_service(char *st, struct iface *pif, struct sockaddr *addr, int addrlen)
{
    UFILE *upkt;
    char *name = NULL, *p;
    int namelen = 0;
    PDevice pdev;
    PService psvc;
    char tmpbuf[100];
    
    UPNP_TRACE((__FUNCTION__, "\n"));
    p = strstr(st, ":service:");
    if (p) {
	name = p+strlen(":service:");
	namelen = strlen(name);
    }
    
    if (namelen) {
	UPNP_TRACE(("%s: looking for service \"%s\"\n", __FUNCTION__, name));
	if ((upkt = usopen(NULL, 0))) {
	    forall_devices(pdev) {
		const char *loc = location(pdev, pif);
		forall_services(pdev, psvc) {
		    UPNP_TRACE(("\tcomparing to \"%s\"\n", psvc->template->name));
		    if (strlen(psvc->template->name) == namelen && strncmp(psvc->template->name, name, namelen) == 0) {
			UPNP_TRACE(("\tmatched!\n"));
			snprintf(tmpbuf, sizeof(tmpbuf), "%s::%s", pdev->udn, st);

			// build the SSDP packet
			ssdp_packet(upkt, SSDP_REPLY, st, tmpbuf,
				    loc, SSDP_REFRESH);

         if (sendto(pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), 0, addr, addrlen) < 0)
            UPNP_ERROR(("sendto failed errno=%d :fd=%d,buf=%p,buflen=%d,addrp=%p, addrlen=%d\n", errno, 
                     pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), addr, addrlen));
			uflush(upkt); 	// reset the packet buffer before we build another.
		    } else {
			UPNP_TRACE(("\tdoes not match\n"));
		    }
		}
	    }

	    uclose(upkt);
	}
    } else {
	UPNP_TRACE(("%s: namelen == 0\n", __FUNCTION__));
    }
}

/* process an SSDP M-SEARCH request. */
static void process_msearch(char *msg, struct iface *pif, struct sockaddr *srcaddr, int addrlen)
{
    char *line, *body, *p, *mxend;
    char *st = NULL;   // the ST: header
    char *mx = NULL;   // the MX: header
    char *man = NULL;  // the MAN: header
    long int mxval;

    UPNP_TRACE((__FUNCTION__, "\n"));
    if ( (body = strstr(msg, "\r\n\r\n" )) != NULL )
	body += 4;
    else if ( (body = strstr(msg, "\r\n" )) != NULL )
	body += 2; 
    else {
	UPNP_INFORM(("M-SEARCH is missing blank line after header!\n%s", msg));
	return;
    }

    p = msg;
    while (p != NULL && p < body) {
	line = strsep(&p, "\r\n");
	if (IMATCH_PREFIX(line, "ST:")) {
	    st = strip_chars(&line[3], " \t");
	} else if (IMATCH_PREFIX(line, "MX:")) {
	    mx = strip_chars(&line[3], " \t");
	} else if (IMATCH_PREFIX(line, "MAN:")) {
	    man = strip_chars(&line[4], " \t");
	}
    }

    if (!st || !mx || !man) 
	UPNP_INFORM(("M-SEARCH is missing required ST:, MX:, or MAN: header!\r\n"));
    else if (!man || (strcmp(man, "\"ssdp:discover\"") != 0)) 
	UPNP_INFORM(("M-SEARCH has invalid MAN: header - \"%s\"\r\n", man));
    else {
	mxval = strtol(mx, &mxend, 10);
	if (mxend == mx || *mxend != '\0' || mxval < 0) {
	    UPNP_INFORM(("M-SEARCH has invalid MX: header - \"%s\"\r\n", mx));
	    return;
	}
	if (strcmp(st, "ssdp:all") == 0) {
	    // Is client searching for any and all devices and services?
	    //
	    process_msearch_all(st, pif, srcaddr, addrlen);
	}
	else if (strcmp(st, "upnp:rootdevice") == 0) {
	    // Is client searching for root devices?
	    //
	    process_msearch_rootdevice(st, pif, srcaddr, addrlen);
	}
	else if (MATCH_PREFIX(st, "uuid:")) {
	    // Is client searching for a particular device ID?
	    //
	    process_msearch_uuid(st, pif, srcaddr, addrlen);
	}
	else if (MATCH_PREFIX(st, "urn:") && strstr(st, ":device:")) {
	    // Is client searching for a particular device type?
	    //
	    process_msearch_devicetype(st, pif, srcaddr, addrlen);
	}
	else if (MATCH_PREFIX(st, "urn:") && strstr(st, ":service:")) {
	    // Is client searching for a particular service type?
	    //
	    process_msearch_service(st, pif, srcaddr, addrlen);
	} else {
	    UPNP_INFORM(("unrecognized ST: header - \"%s\"\r\n", st));
	}
    }
}


/* Should really call this twice to give the UDP packets a better chance of reaching their destination. 
*/
static void advertise_device(PDevice pdev, ssdp_t sstype, struct iface *pif, struct sockaddr *addr, int addrlen)
{
    UFILE *upkt;
    PService psvc;
    char tmpbuf[100], svctype[100];
    const char *loc = location(pdev, pif);
    char *wanif;

    // create a growable string UFILE.
    if ((upkt = usopen(NULL, 0))) {

	if (ISROOT(pdev)) {
	    snprintf(tmpbuf, sizeof(tmpbuf), "%s::upnp:rootdevice", pdev->udn);
	    ssdp_packet(
		upkt, sstype, "upnp:rootdevice", tmpbuf,
		loc, SSDP_REFRESH);
       if (sendto(pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), 0, addr, addrlen) < 0)
          UPNP_ERROR(("sendto failed errno=%d :fd=%d,buf=%p,buflen=%d,addrp=%p, addrlen=%d\n", errno, 
                   pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), addr, addrlen));
	    uflush(upkt); 	// reset the packet buffer before we build another.
	}

	// advertise device by UDN
	ssdp_packet( upkt, sstype, pdev->udn, pdev->udn, 
		     loc, SSDP_REFRESH);
   if (sendto(pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), 0, addr, addrlen) < 0)
      UPNP_ERROR(("sendto failed errno=%d :fd=%d,buf=%p,buflen=%d,addrp=%p, addrlen=%d\n", errno, 
               pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), addr, addrlen));
	uflush(upkt); 	// reset the packet buffer before we build another.
    
	// advertise device by combination of UDN and type
	snprintf(tmpbuf, sizeof(tmpbuf), "%s::%s", pdev->udn, pdev->template->type);
	ssdp_packet( upkt, sstype, pdev->template->type, tmpbuf,
		     loc, SSDP_REFRESH);
   if (sendto(pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), 0, addr, addrlen) < 0)
      UPNP_ERROR(("sendto failed errno=%d :fd=%d,buf=%p,buflen=%d,addrp=%p, addrlen=%d\n", errno, 
               pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), addr, addrlen));
	uflush(upkt); 	// reset the packet buffer before we build another.

        wanif = g_wan_ifname;

	// advertise the services...
	for (psvc = pdev->services; psvc; psvc = psvc->next) {
            if ( strncasecmp ( wanif, "ppp", strlen ( "ppp" ) ) == 0 ) {
                /*
                 * If our WAN interface is PPP, then do not advertise IP service to
                 * the control point.
                 */
                if ( strstr ( psvc->template->name, "WANIPConnection" )) {
                    continue;
                }
	        snprintf(svctype, sizeof(svctype), "urn:%s:service:%s", 
                         psvc->template->schema, psvc->template->name);
	        snprintf(tmpbuf, sizeof(tmpbuf), "%s::%s", pdev->udn, svctype);

	        ssdp_packet( upkt, sstype, svctype, tmpbuf,
			     loc, SSDP_REFRESH);

           if (sendto(pif->ssdp_connection->fd, ubuffer(upkt),
                    utell(upkt), 0, addr, addrlen) == -1) 
              UPNP_ERROR(("sendto failed errno=%d :fd=%d,buf=%p,buflen=%d,addrp=%p, addrlen=%d\n", errno, 
                       pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), addr, addrlen));
	        uflush(upkt); 	// reset the packet buffer before we build another.
	    } else {
                /*
                 * If our WAN interface is IP, then do not advertise PPP service to
                 * the control point.
                 */
                if ( ( strstr ( psvc->template->name, "WANPPPConnection" ) ) ) {
                    continue;
                }
	        snprintf(svctype, sizeof(svctype), "urn:%s:service:%s",
                         psvc->template->schema, psvc->template->name);
	        snprintf(tmpbuf, sizeof(tmpbuf), "%s::%s", pdev->udn, svctype);

	        ssdp_packet( upkt, sstype, svctype, tmpbuf,
			     loc, SSDP_REFRESH);

	        if (sendto(pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), 0, addr, addrlen) == -1) 
              UPNP_ERROR(("sendto failed errno=%d :fd=%d,buf=%p,buflen=%d,addrp=%p, addrlen=%d\n", errno, 
                       pif->ssdp_connection->fd, ubuffer(upkt), utell(upkt), addr, addrlen));
	        uflush(upkt); 	// reset the packet buffer before we build another.
            }
        }

	// release the growable string UFILE.
	uclose(upkt);
    }

}

void periodic_advertiser(timer_t t, ssdp_t sstype)
{
	 cmsLog_debug(" ssdp type=%d\n", sstype);
    send_advertisements(sstype);
}

/* Construct the multicast address and send out the appropriate device advertisement packets. */
void send_advertisements(ssdp_t sstype)
{
    struct sockaddr_in addr;
    struct iface *pif;
    PDevice pdev;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SSDP_IP);
    addr.sin_port = htons(SSDP_PORT);

    for (pif = global_lans; pif; pif = pif->next) {
	forall_devices(pdev) {
	    advertise_device(pdev, sstype, pif, (struct sockaddr *)&addr,  sizeof(addr));
	}
    }
}


static void ssdp_packet(
    UFILE *up, ssdp_t ptype,
    char * ntst, char *Usn, 
    const char * location, int Duration)
{

    UPNP_TRACE((__FUNCTION__, "\n"));
    switch (ptype) {
    case SSDP_REPLY:
	uprintf(up, "HTTP/1.1 200 OK\r\n");
	uprintf(up, "Server: Custom/1.0 UPnP/1.0 Proc/Ver\r\n");
	uprintf(up, "EXT:\r\n");
	uprintf(up, "Location: %s\r\n", location);
	uprintf(up, "Cache-Control:max-age=%d\r\n", Duration);
	uprintf(up, "ST:%s\r\n", ntst);
	uprintf(up, "USN:%s\r\n",Usn);
	// now = time( (time_t*) 0 );
	// (void) strftime( date, sizeof(date), rfc1123_fmt, gmtime( &now ) );
	// uprintf(up, "DATE: %s\r\n", date);
	break;
    case SSDP_ALIVE:
	uprintf(up, "NOTIFY * HTTP/1.1 \r\n");
	uprintf(up, "HOST: %s:%d\r\n", SSDP_IP, SSDP_PORT);
	uprintf(up, "CACHE-CONTROL: max-age=%d\r\n", Duration);
	uprintf(up, "Location: %s\r\n", location);
	uprintf(up, "NT: %s\r\n", ntst);
	uprintf(up, "NTS: ssdp:alive\r\n");
	uprintf(up, "SERVER:%s\r\n", SERVER);
    uprintf(up, "USN: %s\r\n",Usn);
	break;
    case SSDP_BYEBYE:
	uprintf(up, "NOTIFY * HTTP/1.1 \r\n");
	uprintf(up, "HOST: %s:%d\r\n", SSDP_IP, SSDP_PORT);

	// Following two header is added to interop with Windows Millenium but this is not
	// a part of UPNP spec 1.0 
	uprintf(up, "CACHE-CONTROL: max-age=%d\r\n",Duration);
	uprintf(up, "Location: %s\r\n", location);
	uprintf(up, "NT: %s\r\n", ntst);
	uprintf(up, "NTS: ssdp:byebye\r\n");  
    uprintf(up, "USN: %s\r\n",Usn);
	break;
    default:
	UPNP_ERROR(("Bad SSDP packet type supplied - %d\r\n", ptype));
    }

    /*     uprintf(up, "USN: %s\r\n",Usn); */
    uprintf(up, "\r\n");
}


