/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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

void writeHomePlugWebHeader(FILE *fs) {
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");
}

void writeAssocDevicesMDM(FILE* fs)
{
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   HomePlugInterfaceAssociatedDeviceObject* Obj = NULL;

   fprintf(fs, "<b>HomePlug -- associated devices</b><br><br>\n");
   /* write table headers */
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>MAC Address</td>\n");
   fprintf(fs, "      <td class='hd'>HPAV Rate (Tx/Rx)</td>\n");
   fprintf(fs, "   </tr>\n");
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   
   /* write table body */
   while ((ret = cmsObj_getNext(MDMOID_HOME_PLUG_INTERFACE_ASSOCIATED_DEVICE,
                                &iidStack,
                                (void **) &Obj)) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr>\n");
      fprintf(fs, "      <td>%s</td>\n", Obj->MACAddress);
      fprintf(fs, "      <td>%d Mbps / %d Mbps </td>\n", Obj->txPhyRate, 
							 Obj->rxPhyRate);
      fprintf(fs, "   </tr>\n");

      cmsObj_free((void **) &Obj);
   }
   
   /* close assocated devices the table */
   fprintf(fs, "</table><br>\n");
}

/**
 * @brief Creates an entire html table with the info of the HomePlug associated devices.
 *
 * @param[in] query Cgi query to resolve.
 * @param[out] fs html file to be filled.
 */
void cgiHomePlugCfg(char *query __attribute__((unused)), FILE *fs)
{
   /* write the html header */
   writeHomePlugWebHeader(fs);
   /* write the file with all the HomePlug associated devices in a html table */
   writeAssocDevicesMDM(fs);
   /* close html web contents */
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");

   fflush(fs);
}

/**
 * @brief Fills the html table contents for the HomePlug associated devices.
 *
 * @param[in] argc Number of arguments.
 * @param[in] argv Arguments.
 * @param[out] varValue text stream to fill.
 */
void cgiHomePlugAssoc(int argc __attribute__((unused)), 
                      char **argv __attribute__((unused)),
                      char *varValue)
{
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   HomePlugInterfaceAssociatedDeviceObject* Obj = NULL;

   INIT_INSTANCE_ID_STACK(&iidStack);
   
   /* 
    ** Write just the html table body.
    ** Assume two table columns, MAC address and HPAV Rate Tx/Rx, which are painted 
    ** by the html view.
   */   
   while ((ret = cmsObj_getNext(MDMOID_HOME_PLUG_INTERFACE_ASSOCIATED_DEVICE,
                                &iidStack,
                                (void **) &Obj)) == CMSRET_SUCCESS)
   {
      varValue += sprintf(varValue, "   <tr>\n");
      varValue += sprintf(varValue, "      <td>%s</td>\n", Obj->MACAddress);
      varValue += sprintf(varValue, "      <td>%d Mbps / %d Mbps </td>\n", Obj->txPhyRate, 
							 Obj->rxPhyRate);
      varValue += sprintf(varValue, "   </tr>\n");

      cmsObj_free((void **) &Obj);
   }
}

CmsRet cgiConfigHomePlugPassword(void)
{
   HomePlugInterfaceObject* HPAVIfaceObj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = cmsObj_getNext(MDMOID_HOME_PLUG_INTERFACE, 
                        &iidStack,
                        (void**) &HPAVIfaceObj);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to read HOME_PLUG_INTERFACE obj, ret=%d", ret);
   }
   else
   {
      /* previous buffer has to be freed before set a new password. */
      cmsMem_free(HPAVIfaceObj->networkPassword);
      HPAVIfaceObj->networkPassword = NULL; 
      HPAVIfaceObj->networkPassword = cmsMem_strdup(glbWebVar.homeplugNetworkPassword);
      if (HPAVIfaceObj->networkPassword == NULL)
      {
         cmsLog_error("not enough memory to set Network Password");
      }
      else if ((ret = cmsObj_set(HPAVIfaceObj, &iidStack)) != CMSRET_SUCCESS)
      {
        cmsLog_error("failed to set HOME_PLUG_INTERFACE obj, ret=%d", ret);
      }
      else
      {
         cmsLog_debug("Network Password changed with success");
      }
      cmsObj_free((void**) &HPAVIfaceObj);
   }

   return ret;
}

CmsRet cgiConfigHomePlug(void)
{
   HomePlugInterfaceObject* HPAVIfaceObj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = cmsObj_getNext(MDMOID_HOME_PLUG_INTERFACE, 
                        &iidStack,
                        (void**) &HPAVIfaceObj);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to read HOME_PLUG_INTERFACE obj, ret=%d", ret);
   }
   else
   {
      /* previous buffer has to be freed before set a new alias. */
      cmsMem_free(HPAVIfaceObj->alias);
      HPAVIfaceObj->alias = NULL; 
      HPAVIfaceObj->alias = cmsMem_strdup(glbWebVar.homeplugAlias);
      if (HPAVIfaceObj->alias == NULL)
      {
         cmsLog_error("not enough memory to set HomePlug obj");
      }
      else if ((ret = cmsObj_set(HPAVIfaceObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to set HOME_PLUG_INTERFACE obj, ret=%d", ret);
      }
      else
      {
         cmsLog_debug("HomePlug obj changed with success");
      }
      cmsObj_free((void**) &HPAVIfaceObj);
   }

   return ret;
}
