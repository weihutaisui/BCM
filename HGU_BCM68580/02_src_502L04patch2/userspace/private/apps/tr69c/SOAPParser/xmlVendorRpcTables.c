#ifdef SUPPORT_TR69C_VENDOR_RPC

/* 
 * <:copyright-BRCM:2014:proprietary:standard 
 * 
 *    Copyright (c) 2014 Broadcom 
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
 */

/*----------------------------------------------------------------------*
 * File Name  : xmlVendorSpecificRpcTables.c
 *
 * Description: SOAP xmlTables and data structures for vendor specific RPC
 *----------------------------------------------------------------------*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <syslog.h>

#include "cms_util.h"
#include "inc/tr69cdefs.h"   /* defines for RPCAction and ACSState */
#include "inc/utils.h"
#include "nanoxml.h"
#include "RPCState_public.h"

/* define XmlNodeDesc for each RPC here */
/* The Vendor RPC must follow the following format definition for the parser to recognize. 
 * XML format complies with specification, and XMLFUNC and XmlNodeDesc description is found in
 * nanoxml.h 
 *
 * 
 * This is an example of the GetParameterNames Soap body (TR69 protocol stack parses the header.
 * There needs to be function to get each parameter in the RPC method, i.e. fParameterPath, and fNextLevel.
 *
 *                  <SOAP-ENV:Body>
 *                      <cwmp:GetParameterNames>
 *                          <ParameterPath>InternetGatewayDevice.</ParameterPath>
 *                          <NextLevel>0</NextLevel>
 *                      </cwmp:GetParameterNames>
 *                   </SOAP-ENV:Body>
 *
 * XMLFUNC(fParameterPath); -- functions to parse the XML data value in the XML body for RPC 
 * XMLFUNC( fNextLevel);    -- functions to parse the XML data value in the XML body for RPC 
 * static XmlNodeDesc getParameterNamesDesc[] = {
 *    {NULL,  "ParameterPath", fParameterPath, NULL},   // data value 
 *    {NULL,  "NextLevel",fNextLevel, NULL},            // data value 
 *    {NULL, NULL, NULL, NULL}
 * }
 * <ns:GetParameterNames>
 *     <ParameterPath>parameterpath</ParameterPath>
 *
 * static XML_STATUS fGetParameterNames(const char *name, TOKEN_TYPE ttype, const char *value)
 * {
 *    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
 *    if (ttype == TAGBEGIN && rpcAction)
 *        rpcAction->rpcMethod = rpcGetParameterNames;
 *    return XML_STS_OK;
 * }
 * static XML_STATUS fParameterPath(const char *name, TOKEN_TYPE ttype, const char *value)
 * {
 *    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
 *    if (ttype == TAGDATA && rpcAction)
 *        rpcAction->ud.paramNamesReq.parameterPath = cmsMem_strdup(value);
 *
 *    return XML_STS_OK;
 * }
 * static XML_STATUS fNextLevel(const char *name, TOKEN_TYPE ttype, const char *value)
 * {
 *    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
 *    if (ttype == TAGDATA && rpcAction)
 *        rpcAction->ud.paramNamesReq.nextLevel = testBoolean(value);
 *    return XML_STS_OK;
 * }
 */
extern VendorRPCAction *pVendorRpcAction;
extern VendorSpecificRPCItem *vendorSpecificRPCList;

XMLFUNC(fVendorCommandKey);
static XmlNodeDesc vendorGetDataModelInfoDesc[] = {
    {NULL,  "CommandKey", fVendorCommandKey, NULL}, 
    {NULL, NULL, NULL, NULL}
};

/* register VendorSpecificRPC if any */
void registerVendorSpecificRPCs(void)
{
   cmsLog_debug("enter");

   /* pass in eRPCMethods enum, XML parser of vendorRPC desc in this file, and the RPC handler in vendorRRPC.c */
   registerVendorRPC(VENDOR_GET_DATA_MODEL_INFO,vendorGetDataModelInfoDesc,doGetCurrentDataModel);
}

void unregisterVendorSpecificRPCs(void)
{
   VendorSpecificRPCItem *pRpcItem = vendorSpecificRPCList;
   VendorSpecificRPCItem *ptr=pRpcItem;

   cmsLog_debug("enter");

   /* free all the items in vendorSpecificRPCList */
   while (ptr != NULL)
   {
      pRpcItem = ptr;
      CMSMEM_FREE_BUF_AND_NULL_PTR(pRpcItem->rpcName);
      ptr=pRpcItem->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(pRpcItem);
   }
}

VendorRPCAction* newVendorRPCAction(void)
{
   return ((VendorRPCAction *)cmsMem_alloc(sizeof(VendorRPCAction), ALLOC_ZEROIZE));
}

void freeVendorRPCAction(VendorRPCAction* pRpcAction)
{
   if (pRpcAction == NULL)
   {
      return;
   }
   /* free whatever pre-allocated structure/parameters vendor has with each type of RPC */
   if (cmsUtl_strcmp(pRpcAction->rpcName,VENDOR_GET_DATA_MODEL_INFO) == 0)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pRpcAction->ud.getDataModelInfoReq.commandKey);
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(pRpcAction->rpcName);
   CMSMEM_FREE_BUF_AND_NULL_PTR(pRpcAction);
}


/* Begin of  RPC's set functions */

static XML_STATUS fVendorCommandKey(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA){
        pVendorRpcAction->ud.getDataModelInfoReq.commandKey = cmsMem_strdup(value);
    }
    return XML_STS_OK;
}

/* end of RPC's set function */


#endif /* SUPPORT_TR69C_VENDOR_RPC */
