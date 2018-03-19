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


#ifdef SUPPORT_XMPP


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_qdm.h"
#include "cgi_main.h"
#include "httpd.h"


extern int glbEntryCt; // used for web page to know how many entries can be added. In cgimain.c
#define MAX_XMPP_CONN_ENTRY 32 // max XMPP connection entries.


void writeXmppConnHeader(FILE *fs) 
{
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<script language=\"javascript\" src=\"portName.js\"></script>\n");
}


static void writeXmppConnScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'xmppconn.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (rml.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "         if ( rml[i].checked == true )\n");
   fprintf(fs, "            lst += rml[i].value + ',';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( rml.checked == true )\n");
   fprintf(fs, "      lst = rml.value;\n");
   fprintf(fs, "   var loc = 'xmppconncfg.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


static void writeXmppConnBody(FILE *fs) 
{
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");
}


static void writeXmppConnEnd(FILE *fs) 
{
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");
}


static void cgiXmppConnView(FILE *fs) 
{
   Dev2XmppConnObject *xmppConn = NULL;
   Dev2XmppConnServerObject *xmppConnServer = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack serverIidStack = EMPTY_INSTANCE_ID_STACK;

   writeXmppConnHeader(fs);
   writeXmppConnScript(fs);
   writeXmppConnBody(fs);

   fprintf(fs, "<b>XMPP -- Connection Setup</b><br><br>\n");
   fprintf(fs, "XMMP connection allows CPE to connect with XMPP server \n");
   fprintf(fs, "to advertise IP addresses of devices on the LAN side. \n");
   fprintf(fs, "A maximum %d entries can be configured.<br><br>\n<center>\n", MAX_XMPP_CONN_ENTRY);
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>User Name</td>\n");
   fprintf(fs, "      <td class='hd'>Domain</td>\n");   
   fprintf(fs, "      <td class='hd'>Resource</td>\n");
   fprintf(fs, "      <td class='hd'>Jabber ID</td>\n");
#ifdef DMP_DEVICE2_XMPPADVANCED_1
   fprintf(fs, "      <td class='hd'>Use TLS</td>\n");
   fprintf(fs, "      <td class='hd'>Established TLS</td>\n");
   fprintf(fs, "      <td class='hd'>Server Address</td>\n");
   fprintf(fs, "      <td class='hd'>Server Port</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>Last Change Date</td>\n");
   fprintf(fs, "      <td class='hd'>Status</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   /* write table body */
   while (cmsObj_getNext(MDMOID_DEV2_XMPP_CONN,
                         &iidStack,
                         (void **) &xmppConn) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr>\n");
      fprintf(fs, "      <td align='center'>%s</td>\n", xmppConn->username);
      fprintf(fs, "      <td align='center'>%s</td>\n", xmppConn->domain);
      fprintf(fs, "      <td align='center'>%s</td>\n", xmppConn->resource);
      fprintf(fs, "      <td align='center'>%s</td>\n", xmppConn->jabberID);
#ifdef DMP_DEVICE2_XMPPADVANCED_1
      if (xmppConn->useTLS == TRUE)
         fprintf(fs, "      <td align='center'>yes</td>\n");
      else
         fprintf(fs, "      <td align='center'>no</td>\n");
      if (xmppConn->TLSEstablished == TRUE)
         fprintf(fs, "      <td align='center'>yes</td>\n");
      else
         fprintf(fs, "      <td align='center'>no</td>\n");

      /* for webgui, there is only one server instance per connection */
      INIT_INSTANCE_ID_STACK(&serverIidStack);
      if (cmsObj_getNextInSubTree(MDMOID_DEV2_XMPP_CONN_SERVER,&iidStack,&serverIidStack,(void**)&xmppConnServer) == CMSRET_SUCCESS)
      {
         fprintf(fs, "      <td align='center'>%s</td>\n", xmppConnServer->serverAddress);
         fprintf(fs, "      <td align='center'>%d</td>\n", xmppConnServer->port);
         cmsObj_free((void **) &xmppConnServer);
      }
#endif
      fprintf(fs, "      <td align='center'>%s</td>", xmppConn->lastChangeDate);
      fprintf(fs, "      <td align='center'>%s</td>\n", xmppConn->status);
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", xmppConn->username);
      fprintf(fs, "   </tr>\n");

      cmsObj_free((void **) &xmppConn);
      glbEntryCt++;
   }

   /* write table end */
   fprintf(fs, "</table><br><br>\n");

   /* only display the following button  */
   if (glbEntryCt < MAX_XMPP_CONN_ENTRY) 
   {      
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   }
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");

   writeXmppConnEnd(fs);
   fflush(fs);
}

#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID) || defined(SUPPORT_DM_DETECT)
static CmsRet cgiXmppSetManagementServer_igd(InstanceIdStack* iidStack, char *jabberID, char *username)
{
   InstanceIdStack mngrServerIidStack = EMPTY_INSTANCE_ID_STACK;
   ManagementServerObject *mngrServerObj = NULL;
   MdmPathDescriptor xmppConnPathDesc;
   char *xmppConnFullPathStr=NULL;
   char newStr[BUFLEN_1024]={0};
   CmsRet ret = CMSRET_SUCCESS;

   /* get Management Server Object and modify some fields */
   if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &mngrServerIidStack, 0, (void **) &mngrServerObj)) == CMSRET_SUCCESS)
   {
      /* assuming the CPE user always have cpe in it */
      if (strstr(username,"cpe"))
      {
         INIT_PATH_DESCRIPTOR(&xmppConnPathDesc);
         xmppConnPathDesc.oid = MDMOID_DEV2_XMPP_CONN;
         xmppConnPathDesc.iidStack = *iidStack;
         cmsMdm_pathDescriptorToFullPathNoEndDot(&xmppConnPathDesc,&xmppConnFullPathStr);
         CMSMEM_REPLACE_STRING(mngrServerObj->connReqXMPPConnection,xmppConnFullPathStr);
         CMSMEM_REPLACE_STRING(mngrServerObj->connReqJabberID,jabberID);
         CMSMEM_FREE_BUF_AND_NULL_PTR(xmppConnFullPathStr);
      }/* cpe xmpp connection */
      else
      {
         /* all the others are really acs related */
         if ((mngrServerObj->connReqAllowedJabberIDs != NULL) &&
             (mngrServerObj->connReqAllowedJabberIDs[0] != '\0'))
         {
            sprintf(newStr,"%s, %s",mngrServerObj->connReqAllowedJabberIDs,jabberID);
            CMSMEM_REPLACE_STRING(mngrServerObj->connReqAllowedJabberIDs,newStr);
         }
         else
         {
            CMSMEM_REPLACE_STRING(mngrServerObj->connReqAllowedJabberIDs,jabberID);
         }
      }
      
      ret = cmsObj_set(mngrServerObj, &mngrServerIidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to set mngrServerObj, ret=%d", ret);
      }
      cmsObj_free((void **) &mngrServerObj);
   } /* management server */
   return ret;
}
#endif

#if defined(SUPPORT_DM_PURE181) || defined(SUPPORT_DM_DETECT)
static CmsRet cgiXmppSetManagementServer_dev2(InstanceIdStack* iidStack, char *jabberID, char *username)
{
   InstanceIdStack mngrServerIidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2ManagementServerObject *mngrServerObj = NULL;
   MdmPathDescriptor xmppConnPathDesc;
   char *xmppConnFullPathStr=NULL;
   char newStr[BUFLEN_1024]={0};
   CmsRet ret = CMSRET_SUCCESS;

   /* get Management Server Object and modify some fields */
   if ((ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, &mngrServerIidStack, 0, (void **) &mngrServerObj)) == CMSRET_SUCCESS)
   {
      /* assuming the CPE user always have cpe in it */
      if (strstr(username,"cpe"))
      {
         INIT_PATH_DESCRIPTOR(&xmppConnPathDesc);
         xmppConnPathDesc.oid = MDMOID_DEV2_XMPP_CONN;
         xmppConnPathDesc.iidStack = *iidStack;
         cmsMdm_pathDescriptorToFullPathNoEndDot(&xmppConnPathDesc,&xmppConnFullPathStr);
         CMSMEM_REPLACE_STRING(mngrServerObj->connReqXMPPConnection,xmppConnFullPathStr);
         CMSMEM_REPLACE_STRING(mngrServerObj->connReqJabberID,jabberID);
         CMSMEM_FREE_BUF_AND_NULL_PTR(xmppConnFullPathStr);
      }/* cpe xmpp connection */
      else
      {
         /* all the others are really acs related */
         if ((mngrServerObj->connReqAllowedJabberIDs != NULL) &&
             (mngrServerObj->connReqAllowedJabberIDs[0] != '\0'))
         {
            sprintf(newStr,"%s, %s",mngrServerObj->connReqAllowedJabberIDs,jabberID);
            CMSMEM_REPLACE_STRING(mngrServerObj->connReqAllowedJabberIDs,newStr);
         }
         else
         {
            CMSMEM_REPLACE_STRING(mngrServerObj->connReqAllowedJabberIDs,jabberID);
         }
      }
      
      ret = cmsObj_set(mngrServerObj, &mngrServerIidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to set mngrServerObj, ret=%d", ret);
      }
      cmsObj_free((void **) &mngrServerObj);
   } /* management server */
   return ret;
}
#endif

static void cgiXmppConnAdd(char *query, FILE *fs) 
{
   char username[BUFLEN_256], password[BUFLEN_256];
   char domain[BUFLEN_64], resource[BUFLEN_64];
   char enblConn[BUFLEN_4], useTLS[BUFLEN_4];
   char serverAddr[BUFLEN_256], serverPort[BUFLEN_16];
   CmsRet ret = CMSRET_SUCCESS;
   Dev2XmppConnObject *xmppConn = NULL;
   Dev2XmppConnServerObject *xmppConnServer = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char jabberID[BUFLEN_1024]={0};
   username[0] = password[0] = domain[0] = resource[0] = '\0';
   enblConn[0] = useTLS[0];

   cgiGetValueByName(query, "username", username);
   cgiGetValueByName(query, "password", password);   
   cgiGetValueByName(query, "domain", domain);
   cgiGetValueByName(query, "resource", resource);
   cgiGetValueByName(query, "useTLS", useTLS);
   cgiGetValueByName(query, "enblConn", enblConn);
   cgiGetValueByName(query, "serverAddr", serverAddr);
   cgiGetValueByName(query, "serverPort", serverPort);

   if (username[0] == '\0')
   {
      cmsLog_error("XMPP username cannot be empty");
      cgiWriteMessagePage(fs, "XMPP Connection Add Error",
                          "Configure XMPP connection error because of username is empty",
                          "xmppconncfg.cmd?action=view");
      return;
   }

   if (password[0] == '\0')
   {
      cmsLog_error("XMPP password cannot be empty");
      cgiWriteMessagePage(fs, "XMPP Connection Add Error",
                          "Configure XMPP connection error because of password is empty",
                          "xmppconncfg.cmd?action=view");
      return;
   }

   /* adding a new XMPP connection entry */
   cmsLog_debug("Adding new XMPP connection entry with %s/%s/%s/%s/%d/%d", username, password, domain, resource, atoi(useTLS), atoi(enblConn));

   /* add new instance */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_XMPP_CONN, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not create new XMPP connection, ret=%d", ret);
      cgiWriteMessagePage(fs, "XMPP Connection Add Error",
                          "Could not CREATE new XMPP connection",
                          "xmppconncfg.cmd?action=view");
      return;
   }

   /* get the instance of new XMPP connection entry */
   if ((ret = cmsObj_get(MDMOID_DEV2_XMPP_CONN, &iidStack, 0, (void **) &xmppConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Dev2XmppConnObject, ret=%d", ret);
      cgiWriteMessagePage(fs, "XMPP Connection Add Error",
                          "Could not GET new XMPP connection",
                          "xmppconncfg.cmd?action=view");
      return;
   }   
   cmsObj_free((void **) &xmppConn);
   
   /* adding a new XMPP connection's server entry, and set the server parameters before enabling the connection */
   cmsLog_debug("Adding new server object under  XMPP connection entry with address %s and port %d", serverAddr, atoi(serverPort));
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_XMPP_CONN_SERVER, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not create new XMPP server instance, ret=%d", ret);
      cgiWriteMessagePage(fs, "XMPP Server Add Error",
                          "Could not CREATE new server instance for XMPP connection",
                          "xmppconncfg.cmd?action=view");
      return;
   }

   /* get the instance of new XMPP connection entry */
   if ((ret = cmsObj_get(MDMOID_DEV2_XMPP_CONN_SERVER, &iidStack, 0, (void **) &xmppConnServer)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Dev2XmppConnServerObject, ret=%d", ret);
      cgiWriteMessagePage(fs, "XMPP Connection Add Error",
                          "Could not GET new XMPP connection's server instance",
                          "xmppconncfg.cmd?action=view");
      return;
   }       
   xmppConnServer->enable = TRUE;
   xmppConnServer->serverAddress = cmsMem_strdup(serverAddr);
   xmppConnServer->port = atoi(serverPort);
   /* set XMPP connection's server entry */
   ret = cmsObj_set(xmppConnServer, &iidStack);
   cmsObj_free((void **) &xmppConnServer);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set Dev2XmppConnServerObject, ret=%d", ret);
      return;
   }
   if(cmsObj_getAncestor(MDMOID_DEV2_XMPP_CONN, MDMOID_DEV2_XMPP_CONN_SERVER, &iidStack, (void **)&xmppConn) == CMSRET_SUCCESS)
   {
      /* fill values for the instance of new XMPP connection entry */
      xmppConn->username = cmsMem_strdup(username);
      xmppConn->password = cmsMem_strdup(password);
      xmppConn->domain = cmsMem_strdup(domain);
      xmppConn->resource = cmsMem_strdup(resource);
      xmppConn->useTLS = atoi(useTLS);
      xmppConn->enable = atoi(enblConn);

      /* set the instance of new XMPP connection entry */
      ret = cmsObj_set(xmppConn, &iidStack);
      cmsObj_free((void **) &xmppConn);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set Dev2XmppConnObject, ret=%d", ret);
         if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_XMPP_CONN, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to delete created Dev2XmppConnObject, ret=%d", ret);
         }
         cgiWriteMessagePage(fs, "XMPP Connection Add Error",
                             "Could not SET new XMPP connection",
                             "xmppconncfg.cmd?action=view");
         return;
      }
      else
      {
         qdmXmpp_getJabberIdLocked_dev2(&iidStack, jabberID);

#if defined(SUPPORT_DM_LEGACY98)
         cgiXmppSetManagementServer_igd(&iidStack, jabberID, username);
#elif defined(SUPPORT_DM_HYBRID)
         cgiXmppSetManagementServer_igd(&iidStack, jabberID, username);
#elif defined(SUPPORT_DM_PURE181)
         cgiXmppSetManagementServer_dev2(&iidStack, jabberID, username);
#elif defined(SUPPORT_DM_DETECT)
         if (cmsMdm_isDataModelDevice2())
         {
            cgiXmppSetManagementServer_dev2(&iidStack, jabberID, username);
         }
         else
         {
            cgiXmppSetManagementServer_igd(&iidStack, jabberID, username);
         }
#endif
      }
   }

   glbSaveConfigNeeded = TRUE;
   cgiXmppConnView(fs);
}


static void cgiXmppConnRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024];
   UBOOL8 found = FALSE;
   Dev2XmppConnObject *xmppConn = NULL;
   Dev2XmppConnServerObject *xmppConnServer = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack serverIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack mngrServerIidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2ManagementServerObject *mngrServerObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   memset(lst, 0, BUFLEN_1024);
   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ",", &pLast);

   while ( pToken != NULL ) 
   {
      found = FALSE;
      INIT_INSTANCE_ID_STACK(&iidStack);

      while ((!found) &&
             (cmsObj_getNext(MDMOID_DEV2_XMPP_CONN,
                             &iidStack,
                             (void **) &xmppConn) == CMSRET_SUCCESS))
      {
         if (cmsUtl_strcmp(pToken, xmppConn->username) == 0)
         {
            found = TRUE;

            /* take care of management server object's XMPP related parameters */
            if ((ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, &mngrServerIidStack, 0, (void **) &mngrServerObj)) == CMSRET_SUCCESS)
            {
               if (strstr(xmppConn->username,"cpe"))
               {
                  CMSMEM_FREE_BUF_AND_NULL_PTR(mngrServerObj->connReqXMPPConnection);
                  CMSMEM_FREE_BUF_AND_NULL_PTR(mngrServerObj->connReqJabberID);
               }
               else
               {
                  /* look for the allowed jabberID that matches this and delete */
                  if (cmsUtl_isFullPathInCSL(xmppConn->jabberID,mngrServerObj->connReqAllowedJabberIDs) == TRUE)
                  {
                     cmsUtl_deleteFullPathFromCSL(xmppConn->jabberID,mngrServerObj->connReqAllowedJabberIDs);
                  }
               }
               ret = cmsObj_set(mngrServerObj, &mngrServerIidStack);
               if (ret != CMSRET_SUCCESS)
               {
                  cmsLog_error("failed to set mngrServerObj, ret=%d", ret);
               }
               cmsObj_free((void **) &mngrServerObj);
            }

            CMSMEM_FREE_BUF_AND_NULL_PTR(xmppConn->username);
            CMSMEM_FREE_BUF_AND_NULL_PTR(xmppConn->password);
            CMSMEM_FREE_BUF_AND_NULL_PTR(xmppConn->domain);
            CMSMEM_FREE_BUF_AND_NULL_PTR(xmppConn->resource);
            /* for webgui, there is only one server instance per connection */
            if (cmsObj_getNextInSubTree(MDMOID_DEV2_XMPP_CONN_SERVER,&iidStack,&serverIidStack,(void**)&xmppConnServer) == CMSRET_SUCCESS)
            {
               CMSMEM_FREE_BUF_AND_NULL_PTR(xmppConnServer->serverAddress);
               cmsObj_free((void **) &xmppConnServer);
            }
            /* this will also delete the child server instance */
            cmsObj_deleteInstance(MDMOID_DEV2_XMPP_CONN, &iidStack);
         }
         cmsObj_free((void **) &xmppConn);
      }
      pToken = strtok_r(NULL, ",", &pLast);
   }

   cgiXmppConnView(fs);
   glbSaveConfigNeeded = TRUE;
}


void cgiXmppConnCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);

   if ( strcmp(action, "add") == 0 )
   {
      cgiXmppConnAdd(query, fs);
   }
   else if ( strcmp(action, "remove") == 0 )
   {
      cgiXmppConnRemove(query, fs);
   }
   else
   {
      cgiXmppConnView(fs);
   }
}


#endif   /* SUPPORT_XMPP */
