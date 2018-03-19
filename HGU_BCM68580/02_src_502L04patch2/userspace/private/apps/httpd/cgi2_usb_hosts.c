/***********************************************************************
 *
 *  Copyright (c) 2005-2014  Broadcom Corporation
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


#ifdef DMP_DEVICE2_USBHOSTSBASIC_1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "httpd.h"

#include "cgi_main.h"
#include "cgi_ntwk.h"



static void cgiUsbHostsView(FILE *fs)
{
   Dev2UsbHostObject *usbHostObj = NULL;
   InstanceIdStack usbHostIidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2UsbHostDeviceObject *usbHostDevObj = NULL;
   InstanceIdStack usbHostDevIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   writeNtwkHeader(fs);
   fprintf(fs, "<b>Device Info -- USB Hosts</b><br><br>\n");

   /*Usb host controller table*/   
   fprintf(fs, "Usb Host controller<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>Type</td>\n");
   fprintf(fs, "      <td class='hd'>USBVersion</td>\n");
   fprintf(fs, "   </tr>\n");

   while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_USB_HOST, &usbHostIidStack, OGF_NO_VALUE_UPDATE, (void **)&usbHostObj)) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr>\n");
      fprintf(fs, "      <td>%s</td>\n", usbHostObj->name);
      fprintf(fs, "      <td>%s</td>\n", usbHostObj->type);
      fprintf(fs, "      <td>%s</td>\n", usbHostObj->USBVersion);
      fprintf(fs, "   </tr>\n");
      cmsObj_free((void **) &usbHostObj);
   }
   fprintf(fs, "</table><br>\n");


   /*Usb device table*/   
   fprintf(fs, "Usb Devices<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>HostCtrl</td>\n");
   fprintf(fs, "      <td class='hd'>DevNum</td>\n");
   fprintf(fs, "      <td class='hd'>UsbVer</td>\n");
   fprintf(fs, "      <td class='hd'>DevCls</td>\n");
   fprintf(fs, "      <td class='hd'>DevSubCls</td>\n");
   fprintf(fs, "      <td class='hd'>DevVer</td>\n");
   fprintf(fs, "      <td class='hd'>DevProt</td>\n");
   fprintf(fs, "      <td class='hd'>ProdId</td>\n");
   fprintf(fs, "      <td class='hd'>VndrId</td>\n");
   fprintf(fs, "      <td class='hd'>Manufr</td>\n");
   fprintf(fs, "      <td class='hd'>ProdCls</td>\n");
   fprintf(fs, "      <td class='hd'>SerialNum</td>\n");
   fprintf(fs, "      <td class='hd'>Port</td>\n");
   fprintf(fs, "      <td class='hd'>Rate</td>\n");
   fprintf(fs, "      <td class='hd'>MaxChild</td>\n");
   fprintf(fs, "   </tr>\n");

   INIT_INSTANCE_ID_STACK(&usbHostIidStack);
   while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_USB_HOST, &usbHostIidStack, OGF_NO_VALUE_UPDATE, (void **)&usbHostObj)) == CMSRET_SUCCESS)
   {
      while (cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_USB_HOST_DEVICE, &usbHostIidStack, &usbHostDevIidStack,
                                     OGF_NO_VALUE_UPDATE, (void **) &usbHostDevObj) == CMSRET_SUCCESS)
      {
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", usbHostObj->name);
         fprintf(fs, "      <td>%d</td>\n", usbHostDevObj->deviceNumber);
         fprintf(fs, "      <td>%s</td>\n", usbHostDevObj->USBVersion);
         fprintf(fs, "      <td>%s</td>\n", usbHostDevObj->deviceClass);
         fprintf(fs, "      <td>%s</td>\n", usbHostDevObj->deviceSubClass);
         fprintf(fs, "      <td>%d</td>\n", usbHostDevObj->deviceVersion);
         fprintf(fs, "      <td>%s</td>\n", usbHostDevObj->deviceProtocol);
         fprintf(fs, "      <td>%04x</td>\n", usbHostDevObj->productID);
         fprintf(fs, "      <td>%04x</td>\n", usbHostDevObj->vendorID);
         fprintf(fs, "      <td>%s</td>\n", usbHostDevObj->manufacturer);
         fprintf(fs, "      <td>%s</td>\n", usbHostDevObj->productClass);
         fprintf(fs, "      <td>%s</td>\n", usbHostDevObj->serialNumber);
         fprintf(fs, "      <td>%s</td>\n", usbHostDevObj->X_BROADCOM_COM_DeviceKey);
         fprintf(fs, "      <td>%s</td>\n", usbHostDevObj->rate);
         fprintf(fs, "      <td>%d</td>\n", usbHostDevObj->maxChildren);
         fprintf(fs, "   </tr>\n");
         cmsObj_free((void **) &usbHostDevObj);
      }
	  
      cmsObj_free((void **) &usbHostObj);
   }
   

   fprintf(fs, "</table><br>\n");

   
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");

   fflush(fs);
}



void cgiUsbHostsCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];
   cmsLog_debug("query = %s", query);

   cgiGetValueByName(query, "action", action);

   cmsLog_debug("action = %s", action);

   if ( strcmp(action, "view") == 0 )
   {
      cgiUsbHostsView(fs);
   }
   else
   {
      //Sarah: todo usb config   
      cmsLog_error("action %s not support", action);
   }
}

#endif   /* DMP_DEVICE2_USBHOSTSBASIC_1 */
