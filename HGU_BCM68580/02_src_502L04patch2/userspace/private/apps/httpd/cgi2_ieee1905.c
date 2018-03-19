/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard

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
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/syslog.h>
#include <net/if_arp.h>
#include <net/route.h>
#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_main.h"
#include "syscall.h"
#include "httpd.h"
#include "cms_util.h"
#include "cgi_main.h"
#include "cms_dal.h"
#include "cgi_util.h"

#include "i5api.h"
#include "ieee1905_datamodel.h"

void writeIeee1905WebHeader(FILE *fs) {
    fprintf(fs, "<html><head>\n");
    fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
    fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
    // note: uncomment the following line to have this frame refresh automatically every minute:
    fprintf(fs, "<META http-equiv=\"refresh\" content=\"20\">\n");
    fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
    fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");
}


void writeIeee1905WebFooter(FILE *fs) {
    fprintf(fs, "</form>\n");
    fprintf(fs, "</blockquote>\n");
    fprintf(fs, "</body>\n");
    fprintf(fs, "</html>\n");
}

static i5_dm_network_topology_type * get1905DeviceList(void) {
    i5_dm_network_topology_type *dm;
    int rc;
    i5_dm_device_type *dev;
    int devI = 0;
    int i = 0;

    void * nextPtr;         // incremented every time we read a full entry.

    i5_dm_interface_type                *intf;
    i5_dm_legacy_neighbor_type          *legNeighbor;
    i5_dm_1905_neighbor_type            *neighbor;
    i5_dm_bridging_tuple_info_type      *bt;
        
    rc = i5apiTransaction(I5_API_CMD_RETRIEVE_DM, NULL, 0, (void **)&dm, 0);
    if (rc < 0) {
        printf("Could not retrieve device list from 1905 (%d)\n", rc);
        return NULL;
    }

    // process the message (reconstruct linked list pointers):
    // Note: not fixing parent pointers, etc.  Just next pointers for now..
    
    nextPtr = dm+1;
    dev = &dm->device_list;
    dev->ll.next = NULL;

    for (devI = 0; devI < dm->DevicesNumberOfEntries; devI++) {

        dev->ll.next = dm->DevicesNumberOfEntries-devI?nextPtr:NULL;
        dev = nextPtr;
        nextPtr = dev+1;

        intf = &dev->interface_list;
        intf->ll.next = NULL;
        for( i = 0; i < dev->InterfaceNumberOfEntries; i++) {
            intf->ll.next = dev->InterfaceNumberOfEntries-i?nextPtr:NULL;
            intf = nextPtr;
            nextPtr = intf+1;
        }

        legNeighbor = &dev->legacy_list;
        legNeighbor->ll.next = NULL;
        for( i = 0; i < dev->LegacyNeighborNumberOfEntries; i++) {
            legNeighbor->ll.next = dev->LegacyNeighborNumberOfEntries-i?nextPtr:NULL;
            legNeighbor = nextPtr;
            nextPtr = legNeighbor+1;
        }

        neighbor = &dev->neighbor1905_list;
        neighbor->ll.next = NULL;
        for( i = 0; i < dev->Ieee1905NeighborNumberOfEntries; i++) {
            neighbor->ll.next = dev->Ieee1905NeighborNumberOfEntries-i?nextPtr:NULL;
            neighbor = nextPtr;
            nextPtr = neighbor+1;
        }

        bt = &dev->bridging_tuple_list;
        bt->ll.next = NULL;
        for( i = 0; i < dev->BridgingTuplesNumberOfEntries; i++) {
            bt->ll.next = dev->BridgingTuplesNumberOfEntries-i?nextPtr:NULL;
            bt = nextPtr;
            nextPtr = bt+1;
        }
    }
    return dm;
}

static void free1905DeviceList(i5_dm_network_topology_type *dm) {
    free(dm);
}

/* Returns full ieee1905 status web page (on behalf of ieee1905sts.cmd) */
void cgiIeee1905Status(char *query __attribute__((unused)), FILE *fs)
{
    i5_dm_network_topology_type *dm;
    i5_dm_device_type *dev;
    i5_dm_device_type *self;
    Dev2Ieee1905AlObject *ieee1905AlObj = NULL;
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    char macStr[MAC_STR_LEN];

    ret = cmsObj_get(MDMOID_DEV2_IEEE1905_AL, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ieee1905AlObj);
    if (ret != CMSRET_SUCCESS) {
        writeIeee1905WebHeader(fs);
        fprintf(fs, "<b>nVoy (1905) Status</b><br><br>"); 
        fprintf(fs, "<b>Internal Error</b>"); 
        fflush(fs);
        writeIeee1905WebFooter(fs);
        return;
    }

    if (!ieee1905AlObj->enable) {
        writeIeee1905WebHeader(fs);
        fprintf(fs, "<b>nVoy (1905) Status</b><br><br>"); 
        fprintf(fs, "Disabled"); 
        fflush(fs);
        writeIeee1905WebFooter(fs);
        cmsObj_free((void **)&ieee1905AlObj);
        return;
    }
    cmsObj_free((void **)&ieee1905AlObj);

    dm = get1905DeviceList();
    if (!dm) {
        writeIeee1905WebHeader(fs);
        fprintf(fs, "<b>nVoy (1905) Status</b><br><br>"); 
        fprintf(fs, "<b>Error, could not retrieve ieee1905 information</b>");        
        fflush(fs);
        writeIeee1905WebFooter(fs);
        cmsObj_free((void **)&ieee1905AlObj);
        return;
    }
    
    /* write the html header */
    writeIeee1905WebHeader(fs);

    // get last entry (which will be local device):
    
    dev = &dm->device_list;
    while (dev->ll.next) {
        dev = dev->ll.next;    
    }
        
    if (dev != &dm->device_list) {        
        self = dev;
        cmsUtl_macNumToStr((UINT8 *) self->DeviceId, macStr);
    }
    else {
        self = NULL;
        sprintf(macStr, "NoMacAddr");    
    }

    fprintf(fs, "<b>nVoy (1905) Status -- %s : %s</b><br><br>", 
            self ? self->friendlyName:"Unknown", macStr);

    /* draw the table: */

    fprintf(fs, "<b>Connected nVoy Devices</b><br>");

    fprintf(fs, "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\" position=\"absolute\" border-collapse=\"collapse\">");

    /* header row: */
    fprintf(fs, "<tr position=\"absolute\">");
    fprintf(fs, "<td class=\"hd\">MacAddr</td>");
    fprintf(fs, "<td class=\"hd\">Name</td>");
    fprintf(fs, "</tr>");

    dev = dm->device_list.ll.next;
    while (dev && dev->ll.next) {
        // it would be really nice to insert a link to other devices here
        fprintf(fs, "<tr position=\"absolute\">");
        cmsUtl_macNumToStr((UINT8 *)dev->DeviceId,macStr);
        fprintf(fs, "<td>%s</td><td>%s</td>", macStr, dev->friendlyName);
        fprintf(fs, "</tr>");
        dev = dev->ll.next;
    }
    fprintf(fs, "</table>");

    free1905DeviceList(dm);

    /* write the html footer */
    writeIeee1905WebFooter(fs);

    fflush(fs);
}


CmsRet cgiDoIeee1905Config(void)
{
   // This is called when user presses Apply/Save button on ieee1905cfg.html
   Dev2Ieee1905AlObject *ieee1905AlObj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = cmsObj_get(MDMOID_DEV2_IEEE1905_AL, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ieee1905AlObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to read 1905 obj, ret=%d", ret);
      return ret;
   }
   else
   {
      /* previous buffer has to be freed before set a new alias. */
      cmsMem_free(ieee1905AlObj->deviceFriendlyName);
      ieee1905AlObj->deviceFriendlyName = cmsMem_strdup(glbWebVar.ieee1905Dfname);
      ieee1905AlObj->enable = glbWebVar.ieee1905IsEnabled;
      ieee1905AlObj->isRegistrar = glbWebVar.ieee1905IsRegistrar;
      ieee1905AlObj->APFreqBand24Enable = glbWebVar.ieee1905ApFreq24En;
      ieee1905AlObj->APFreqBand5Enable = glbWebVar.ieee1905ApFreq5En;

      if (ieee1905AlObj->deviceFriendlyName == NULL)
      {
         cmsLog_error("not enough memory to set 1905 obj");
         cmsObj_free((void**)&ieee1905AlObj);
         return CMSRET_INTERNAL_ERROR;
      }

      ret = cmsObj_set(ieee1905AlObj, &iidStack);
      glbSaveConfigNeeded = TRUE;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to set 1905 obj, ret=%d", ret);
      }
      else
      {
         cmsLog_debug("Ieee1905 obj changed with success");
      }
      cmsObj_free((void**)&ieee1905AlObj);
   }

   return CMSRET_SUCCESS;
}
#endif

