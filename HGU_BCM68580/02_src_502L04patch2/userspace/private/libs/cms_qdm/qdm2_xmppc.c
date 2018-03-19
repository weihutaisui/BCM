/***********************************************************************
 *
 *  Copyright (c) 2014  Broadcom Corporation
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
 
#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"

#ifdef DMP_DEVICE2_XMPPADVANCED_1

/* given the iidStack of a connection, return the server of choice to use */
CmsRet qdmXmpp_getServerForConnectionLocked_dev2(const InstanceIdStack *iidStack, char *serverAddress, UINT32 *serverPort)
{
   InstanceIdStack serverIidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2XmppConnObject *xmppConn = NULL;
   Dev2XmppConnServerObject *serverObj;
   UINT32 priority=65535;
   int weight=-1;
   CmsRet ret;
   int numOfServer = 0;

   if ((ret = cmsObj_get(MDMOID_DEV2_XMPP_CONN,iidStack,0,(void **) &xmppConn)) == CMSRET_SUCCESS)
   {
      numOfServer = xmppConn->serverNumberOfEntries;
      cmsObj_free((void**)&xmppConn);
      if (numOfServer <= 0)
      {
         return (CMSRET_INTERNAL_ERROR);
      }
      else if (numOfServer == 1)
      {
         if (cmsObj_getNextInSubTree(MDMOID_DEV2_XMPP_CONN_SERVER,iidStack,&serverIidStack,(void**)&serverObj) == CMSRET_SUCCESS)
         {
            cmsUtl_strcpy(serverAddress, serverObj->serverAddress);
            *serverPort = serverObj->port;
            cmsObj_free((void**)&serverObj);
            return (CMSRET_SUCCESS);
         }
      }
      else
      {
         /* need to find out the lowest of priority or weight of the server */
         while ((ret = cmsObj_getNextInSubTree(MDMOID_DEV2_XMPP_CONN_SERVER,iidStack,&serverIidStack,(void**)&serverObj)) == CMSRET_SUCCESS)
         {
            if (serverObj->priority < priority)
            {
               priority = serverObj->priority;
            }
            else if (serverObj->priority == priority)
            {
               if ((serverObj->weight != -1) || (serverObj->weight > weight))
               {
                  /* we will pick the first lowest priority if weight is -1 */
                  weight = serverObj->weight;
               }
            }
            cmsObj_free((void**)&serverObj);
         }

         /* now that we have the priority and weight, go look for it again and return to caller */
         INIT_INSTANCE_ID_STACK(&serverIidStack);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_DEV2_XMPP_CONN_SERVER,iidStack,&serverIidStack,(void**)&serverObj)) == CMSRET_SUCCESS)
         {
            if ((serverObj->priority == priority) && (serverObj->weight == weight))
            {
               cmsUtl_strcpy(serverAddress, serverObj->serverAddress);
               *serverPort = serverObj->port;
               cmsObj_free((void**)&serverObj);
               return ret;
            }
            cmsObj_free((void**)&serverObj);
         }
      } /* more than 1 server are configured for this connection */
   }
   return ret;
}
#endif /* DMP_DEVICE2_XMPPADVANCED_1 */

CmsRet qdmXmpp_getJabberIdLocked_dev2(const InstanceIdStack *iidStack, char *jabberID)
{
   Dev2XmppConnObject *xmppConn = NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_DEV2_XMPP_CONN,iidStack,0,(void **) &xmppConn)) == CMSRET_SUCCESS)
   {
      sprintf(jabberID, "%s", xmppConn->username);
      if (xmppConn->domain != NULL && xmppConn->domain[0] != '\0')
      {
         sprintf(jabberID, "%s@%s", jabberID, xmppConn->domain);
      }
      if (xmppConn->resource != NULL && xmppConn->resource[0] != '\0')
      {
         sprintf(jabberID, "%s/%s", jabberID, xmppConn->resource);
      }
      cmsObj_free((void**)&xmppConn);
   }
   return (ret);
}

