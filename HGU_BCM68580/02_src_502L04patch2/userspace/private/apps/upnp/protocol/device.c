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

#include <stdarg.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "upnp_osl.h"
#include "upnp_dbg.h"
#include "upnp.h"
#include "../igd/igd.h"

#include "cms_log.h"
#include "cms_core.h"
#include "cms_util.h"

#define DEFAULT_DEV_MFR   	       "Broadcom"
#define DEFAULT_DEV_MFR_URL            "http://www.broadcom.com/"
#define DEFAULT_DEV_MODEL_DESCRIPTION  "Broadcom single-chip ADSL router"
#define DEFAULT_DEV_MODEL_NAME              "BCM963xx"
#define DEFAULT_DEV_MODEL_NO           "1.0"
#define DEFAULT_DEV_SERIAL_NO           "1"
#define DEFAULT_DEV_MODEL_URL          "http://www.broadcom.com/"

char *g_devMfr = NULL;
char *g_devMfrUrl = NULL;
char *g_devMdlDesc = NULL;
char *g_devMdlName = NULL;
char *g_devMdlNum = NULL;
char *g_devSerialNum = NULL;
char *g_devMdlUrl = NULL;


extern PService init_service(PServiceTemplate svctmpl, PDevice pdev,int dynInstId);
extern void destroy_service(PService);

void device_devicelist(PDevice pdev, UFILE *up);
void device_servicelist(PDevice pdev, UFILE *up);

PDevice root_devices = NULL;

static int get_lan_ipaddr(const char *lan_ifname, struct in_addr *lan_ip)
{
   int socketfd;
   struct ifreq lan;

   if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
     printf("app: Error openning socket when getting LAN info\n");
     return -1;
   }

   strcpy(lan.ifr_name,lan_ifname);
   if (ioctl(socketfd,SIOCGIFADDR,&lan) < 0) {
     printf("app: Error getting LAN IP address\n");
     close(socketfd);
     return -1;
   }
   *lan_ip = ((struct sockaddr_in *)&(lan.ifr_addr))->sin_addr;

   close(socketfd);
   return 0;
}

/* recursive routine to initialize a device, and all its subdevices.
   Also all services within a device are intialized by calling their
   specific initialization function, if available.  */
PDevice init_device(PDevice parent, PDeviceTemplate pdevtmpl,int InstId, ...)
{
    static int indent = 0;
    int i;
    PFDEVINIT func;
    PDevice pdev, subdev;
    PService psvc;
    va_list ap;

    UPNP_TRACE(("%*sInitializing %sdevice \"%s\".\r\n", indent, "", (parent ? "" : "root "), pdevtmpl->type));

    if (pdevtmpl->schema == NULL)
	pdevtmpl->schema = "schemas-upnp-org";

    pdev = (Device *) cmsMem_alloc(sizeof(Device), ALLOC_ZEROIZE);
    if(pdev == NULL){
       UPNP_ERROR(("cmsMem_alloc failed \n"));
       return NULL;
    }
    pdev->parent = parent;
    pdev->template = pdevtmpl;

	 pdev->instance = InstId;

    // call the device's intialization function, if defined.
    if ((func = pdevtmpl->devinit) != NULL) {
//	va_start( ap, pdevtmpl);
	va_start( ap, InstId);
	(*func)(pdev, DEVICE_CREATE, ap);
	va_end( ap);
    }

    // we do a top down, depth-first traversal of the device heirarchy.
    // sub-devices will be initialized before we complete initialization of the root device.
    //
    for (i = 0; i < pdevtmpl->ndevices; i++) {
	indent += 4;
	subdev = init_device(pdev, &(pdevtmpl->devicelist[i]),0);
	indent -= 4;
	subdev->next = pdev->subdevs;
	pdev->subdevs = subdev;
    }

    // Initialize each service in this device.
    //
    for (i = 0; i < pdevtmpl->nservices; i++) {
	psvc = init_service(pdevtmpl->services[i], pdev,0);
	
	psvc->next = pdev->services;
	pdev->services = psvc;
    }

    if (ISROOT(pdev)) {
	pdev->next = root_devices;
	root_devices = pdev;
    }

    return pdev;
}

void destroy_device(PDevice pdev)
{
    static int indent = 0;
    PFDEVINIT func;
    PDevice psubdev, nextdev, *ppdev;
    PService psvc, nextsvc;

    UPNP_TRACE(("%*sDestroying %sdevice \"%s\".\r\n", indent, "", (ISROOT(pdev) ? "root " : ""), pdev->template->type));

    // destroy all subdevices in this device.
    if (pdev->subdevs) {
	for (psubdev = pdev->subdevs; psubdev; psubdev = nextdev) {
	    nextdev = psubdev->next;
	    indent += 4;
	    destroy_device(psubdev);
	    indent -= 4;
	}
    }
    
    // destroy all service in this device.
    if (pdev->services) {
	for (psvc = pdev->services; psvc; psvc = nextsvc) {
	    nextsvc = psvc->next;
	    destroy_service(psvc);
	}
    }

    // call this device's destroy function, if defined.
    if ((func = pdev->template->devinit) != NULL) {
	((*func)(pdev, DEVICE_DESTROY, (va_list) {0} ));
    }

    // remove the device from the root device list
    if (ISROOT(pdev)) {
	for (ppdev = &root_devices; *ppdev; ppdev = &(*ppdev)->next) {
	    if (*ppdev == pdev) {
		*ppdev = (*ppdev)->next;
		break;
	    }
	}
    }
   
    cmsMem_free(pdev->friendlyname);
    // finally, free the memory allocated in init_device();
    cmsMem_free(pdev);
}


/* Print an XML device description for a device and all its subdevices.
   We used to just print the static XML device description from a file, but now that the 
   IGD is more dynamic and can adjust to different gateway configurations,
   we must dynamically generate the XML.
 */
void device_xml(PDevice pdev, UFILE *up)
{
    PFDEVXML func;
    char *friendlyname;
    struct in_addr inaddr;

    // call the device's xml function, if defined.
    if ((func = pdev->template->devxml) != NULL) {
	(*func)(pdev, up);
	return;
    }

    if (ISROOT(pdev)) {
        get_lan_ipaddr(g_lan_ifname, &inaddr);
	uprintf(up, 
		"<?xml version=\"1.0\"?>\r\n"
		"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\r\n"
		"<specVersion>\r\n"
		"<major>1</major>\r\n"
		"<minor>0</minor>\r\n"
		"</specVersion>\r\n"
		"<URLBase>http://%s:5431/</URLBase>\r\n", inet_ntoa(inaddr)
		);
    }

    if (pdev->friendlyname)
	friendlyname = pdev->friendlyname;
    else {
	friendlyname = pdev->template->type;
    }



    uprintf(up, "<device>\r\n");
    uprintf(up, "<deviceType>%s</deviceType>\r\n", pdev->template->type);
    if (ISROOT(pdev)) {
        get_lan_ipaddr(g_lan_ifname, &inaddr);
        uprintf(up, "<presentationURL>http://%s:80/</presentationURL>\r\n", inet_ntoa(inaddr));
    }
    uprintf(up, "<friendlyName>%s</friendlyName>\r\n", friendlyname);
    uprintf(up, "<manufacturer>%s</manufacturer>\r\n", g_devMfr);
    uprintf(up, "<manufacturerURL>%s</manufacturerURL>\r\n", g_devMfrUrl);
    uprintf(up, "<modelDescription>%s</modelDescription>\r\n", g_devMdlDesc);
    uprintf(up, "<modelName>%s</modelName>\r\n", g_devMdlName);
    uprintf(up, "<modelNumber>%s</modelNumber>\r\n", g_devMdlNum);
    uprintf(up, "<modelURL>%s</modelURL>\r\n",g_devMdlUrl);
    uprintf(up, "<UDN>%s</UDN>\r\n", pdev->udn);

    // generate XML for any services in this device.
    device_servicelist(pdev, up);

    // generate XML for any subdevices in this device.
    device_devicelist(pdev, up);
	
    uprintf(up, "</device>\r\n");

    if (ISROOT(pdev)) {
	uprintf(up, "</root>\r\n");
    }
}
    
    
void device_devicelist(PDevice pdev, UFILE *up)
{
    PDevice  psubdev;

    // generate XML for any subdevices in this device.
    if (pdev->subdevs) {
	uprintf(up, "<deviceList>\r\n");
	for (psubdev = pdev->subdevs; psubdev; psubdev = psubdev->next) 
	    device_xml(psubdev, up);
	uprintf(up, "</deviceList>\r\n");
    }
}


void device_servicelist(PDevice pdev, UFILE *up)
{
    char svcurl[200];
    PService psvc;

    // generate XML for any services in this device.
    if (pdev->services) {
	uprintf(up, "<serviceList>\r\n");
	forall_services(pdev, psvc) {
            char *wanif;

    	    wanif = g_wan_ifname;
            if ( strstr ( wanif, "ppp" ) != NULL ) {
                if ( strcmp ( psvc->template->name, "WANIPConnection:1" ) == 0 ) {
                    continue;
                }
            } else  {/*for IP dont include PPP */
                if ( strcmp (psvc->template->name, "WANPPPConnection:1" ) == 0 ) {
                    continue;
                }
            }

	    snprintf(svcurl, sizeof(svcurl), "/%s/%s", pdev->udn, psvc->template->name);

	    uprintf(up, "<service>\r\n");
	    uprintf(up, "<serviceType>urn:%s:service:%s</serviceType>\r\n", 
		    psvc->template->schema, psvc->template->name);
	    if (psvc->template->serviceid) {
		uprintf(up, "<serviceId>%s%d</serviceId>\r\n", 
			psvc->template->serviceid, psvc->instance);
	    } else {
		uprintf(up, "<serviceId>urn:upnp-org:serviceId:%s%d</serviceId>\r\n", 
			psvc->template->name, psvc->instance);
	    }
	    uprintf(up, "<controlURL>/%s/%s</controlURL>\r\n", pdev->udn, psvc->template->name);
	    uprintf(up, "<eventSubURL>/%s/%s</eventSubURL>\r\n", pdev->udn, psvc->template->name);
	    uprintf(up, "<SCPDURL>/dynsvc/%s.xml</SCPDURL>\r\n", psvc->template->name);
	    uprintf(up, "</service>\r\n");
	}
	uprintf(up, "</serviceList>\r\n");
    }
}


/* Given a device pointer, return the root device for that device. */
PDevice rootdev(PDevice pdev)
{
    while (pdev->parent) 
	pdev = pdev->parent;
    return pdev;
}


/* Device iterator used by the forall_devices() macro.
   
   The first call to device_iterator() should have NULL as its argument.  
   Subsequent calls will return the next device in depth first, pre-order.  
*/
PDevice device_iterator(PDevice pdev)
{
    PDevice nextdev;

    if (pdev == NULL) {
	nextdev = root_devices;
    } else {
	if (pdev->subdevs) {
	    nextdev = pdev->subdevs;
	} else {
	    nextdev = pdev;
	    while (nextdev) {
		if (nextdev->next) {
		    nextdev = nextdev->next;
		    break;
		} else {
		    nextdev = nextdev->parent;
		}
	    }
	} 
    }

    return nextdev;
}

void get_deviceInfo(void)
{

   if (upnp_getDeviceInfo() != 0)
   {
      /* reading from CMS MDM failed, just put in some default values */
      CMSMEM_REPLACE_STRING(g_devMfr,DEFAULT_DEV_MFR);
      CMSMEM_REPLACE_STRING(g_devMdlName,DEFAULT_DEV_MODEL_NAME);
      CMSMEM_REPLACE_STRING(g_devMdlDesc,DEFAULT_DEV_MODEL_DESCRIPTION);
      CMSMEM_REPLACE_STRING(g_devSerialNum,DEFAULT_DEV_SERIAL_NO);
   }

   /* set default values for variables not defined in MDMOID_IGD_DEVICE_INFO */
      CMSMEM_REPLACE_STRING(g_devMfrUrl,DEFAULT_DEV_MFR_URL);
      CMSMEM_REPLACE_STRING(g_devMdlUrl,DEFAULT_DEV_MODEL_URL);
      CMSMEM_REPLACE_STRING(g_devMdlNum,DEFAULT_DEV_MODEL_NO);

}

