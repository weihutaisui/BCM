/***********************************************************************
 *
 *  Copyright (c) 2008-2010  Broadcom Corporation
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

#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>

#include "cms.h"
#include "cms_msg.h"
#include "cms_util.h"
#include "cms_phl.h"
#include "httpd.h"
#include "cgi_main.h"
#include "cgi_omci.h"
#include "omciutl_cmn.h"

// global variables
// in cgi_omci_cfg
extern UINT16 tcIdCur;
extern UINT16 numMsgRx;
extern UINT16 numMsgTx;
extern UINT16 numRetry;
extern omciPacket *packetList;
// in cgi_omci_macro
extern omciMacroState macroState;

UINT16 numSequence = 0;
UBOOL8 getNextInProgress = FALSE;

/*
 * local variables
 */
static char object[BUFLEN_264] = {0};
static char action[BUFLEN_264] = {0};

static omciCreateObject_t GetNextObjectsTable[] = {
   {"MacBpMacFilterTable", MDMOID_MAC_BRIDGE_PORT_FILTER_TABLE_DATA, MDMOID_LAYER2_DATA_SERVICES, 49, 0x8000},
   {"MacBpBridgeTable", MDMOID_MAC_BRIDGE_PORT_BRIDGE_TABLE_DATA, MDMOID_LAYER2_DATA_SERVICES, 50, 0x8000},
   {"OnuRemoteDebugReplyTable", MDMOID_ONU_REMOTE_DEBUG, MDMOID_GENERAL, 158, 0x2000},
   {"ExtVlanReceivedFrameVlanTaggingOperationTable", MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA, MDMOID_LAYER2_DATA_SERVICES, 171, 0x0400},
   {"McastGemIwtpMulticastAddressField", MDMOID_MULTICAST_GEM_INTERWORKING_TP, MDMOID_ANI_MANAGEMENT, 281, 0x0080},
   {"McastGEmIwtpIpv6MulticastAddressField", MDMOID_MULTICAST_GEM_INTERWORKING_TP, MDMOID_ANI_MANAGEMENT, 281, 0x0040},
   {"OmciMeTypeTable", MDMOID_OMCI, MDMOID_GENERAL, 287, 0x8000},
   {"OmciMessageTypeTable", MDMOID_OMCI, MDMOID_GENERAL, 287, 0x4000},
   {"MopDynamicAccessControlListTable", MDMOID_MULTICAST_OPERATIONS_PROFILE, MDMOID_LAYER2_DATA_SERVICES, 309, 0x0200},
   {"MopStaticAccessControlListTable", MDMOID_MULTICAST_OPERATIONS_PROFILE, MDMOID_LAYER2_DATA_SERVICES, 309, 0x0100},
   {"MopLostGroupsListTable", MDMOID_MULTICAST_OPERATIONS_PROFILE, MDMOID_LAYER2_DATA_SERVICES, 309, 0x0080},
   {"MsciMulticastServicePackageTable", MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO, MDMOID_LAYER2_DATA_SERVICES, 310, 0x0400},
   {"MsciAllowedPreviewGroupsTable", MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO, MDMOID_LAYER2_DATA_SERVICES, 310, 0x0200},
   {"MsmActiveGroupsListTable", MDMOID_MULTICAST_SUBSCRIBER_MONITOR, MDMOID_LAYER2_DATA_SERVICES, 311, 0x0800},
   {"MsmIpv6ActiveGroupsListTable", MDMOID_MULTICAST_SUBSCRIBER_MONITOR, MDMOID_LAYER2_DATA_SERVICES, 311, 0x0400},
   {"GeneralStatusDocTable", MDMOID_GENERIC_STATUS_PORTAL, MDMOID_GENERAL, 330, 0x8000},
   {"GeneralConfigDocTable", MDMOID_GENERIC_STATUS_PORTAL, MDMOID_GENERAL, 330, 0x4000},
   {"ESCOltRandomChallengeTable", MDMOID_ESC, MDMOID_MISCELLANEOUS_SERVICES, 332, 0x4000},
   {"ESCOnuRandomChallengeTable", MDMOID_ESC, MDMOID_MISCELLANEOUS_SERVICES, 332, 0x0800},
   {"ESCOnuAuthenticationResultTable", MDMOID_ESC, MDMOID_MISCELLANEOUS_SERVICES, 332, 0x0400},
   {"ESCOltAuthenticationResultTable", MDMOID_ESC, MDMOID_MISCELLANEOUS_SERVICES, 332, 0x0200},
   {"ESCBroadcastKeyTable", MDMOID_ESC, MDMOID_MISCELLANEOUS_SERVICES, 332, 0x0020},
   {"Ipv6HostCurrentAddressTable", MDMOID_IPV6_HOST_CONFIG_DATA, MDMOID_LAYER3_DATA_SERVICES, 347, 0x0080},
   {"Ipv6HostCurrentDefaultRouterTable", MDMOID_IPV6_HOST_CONFIG_DATA, MDMOID_LAYER3_DATA_SERVICES, 347, 0x0040},
   {"Ipv6HostCurrentDnsTable", MDMOID_IPV6_HOST_CONFIG_DATA, MDMOID_LAYER3_DATA_SERVICES, 347, 0x0020},
   {"Ipv6HostCurrentOnlinkPrefixTable", MDMOID_IPV6_HOST_CONFIG_DATA, MDMOID_LAYER3_DATA_SERVICES, 347, 0x0004},
#if defined(DMP_X_ITU_ORG_VOICE_SIP_1)
   {"SipConfigPortalTextTable", MDMOID_SIP_CONFIG_PORTAL, MDMOID_VOICE_SERVICES, 149, 0x8000},
   {"SipAgentResponseTable", MDMOID_SIP_AGENT_CONFIG_DATA, MDMOID_VOICE_SERVICES, 150, 0x0010},
#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */
#if defined(DMP_X_ITU_ORG_VOICE_MGC_1)
   {"MgcConfigPortalTextTable", MDMOID_MGC_CONFIG_PORTAL, MDMOID_VOICE_SERVICES, 154, 0x8000},
#endif /* DMP_X_ITU_ORG_VOICE_MGC_1 */
   };



#define NUM_GET_NEXT_OBJECTS (SINT32) (sizeof(GetNextObjectsTable) / sizeof(omciCreateObject_t))

									/* local functions */

static CmsRet cgiOmci_sendGetNextRequest(void);
static void cgiOmciGetNextView(FILE *fs);
static void writeOmciGetNextHeader(FILE *fs);
static void writeOmciGetNextScript(FILE *fs);
static void writeOmciGetNextObject(FILE *fst);
static void writeOmciGetNextButton(FILE *fs);

// Main entry for OMCI get next web page
void cgiOmciGetNext(char *query, FILE *fs) 
{
   object[0] = action[0];

   /* OMCI operations do not need CMS lock. */
   cmsLck_releaseLock();

   cgiGetValueByName(query, "selObject", object);
   cgiGetValueByName(query, "selAction", action);

   if (object[0] != '\0')
   {
      if (cmsUtl_strcmp(action, "getnext") == 0)
      {
         if (numMsgTx == 0)
         {
            numMsgRx = 0;
            cgiOmci_sendGetNextRequest();
         }
      }
      else if (cmsUtl_strcmp(action, "reset") == 0)
      {
          // action is reset so clear all selected value
         object[0] = action[0] = '\0';
      }
   }

   cgiOmciGetNextView(fs);
   cmsLck_acquireLockWithTimeout(HTTPD_LOCK_TIMEOUT);
}

static CmsRet cgiOmci_sendGetNextRequest(void)
{
   SINT32 numInfoEntries = 0, i = 0; 
   UINT16 j = 0, msgSize = sizeof(CmsMsgHeader) + sizeof(omciPacket);
   char fullPathName[BUFLEN_1024];
   OmciParamInfo_t *paramInfoList;
   OmciObjectInfo_t pathDesc;
   UINT16 meClass = 0, meInst = 0;
   char buf[msgSize];
   CmsMsgHeader *msg=(CmsMsgHeader *) buf;
   omciPacket *packet = (omciPacket *) (msg+1);
   OmciMdmOidClassId_t info;
   CmsRet ret = CMSRET_SUCCESS;
   char fileName[BUFLEN_264];
   char *pChar = NULL, parentName[BUFLEN_1024];
   FILE *fs = NULL;

   // remove the leaf name
   // ex: InternetGatewayDevice.X_ITU_T_ORG.G_988.Layer2DataServices.
   //     ExtendedVlanTaggingOperationConfigurationData.{i}.ReceivedFrameVlanTaggingOperationTable
   // ==> InternetGatewayDevice.X_ITU_T_ORG.G_988.Layer2DataServices.
   //     ExtendedVlanTaggingOperationConfigurationData.{i}.
   memset(parentName, 0, BUFLEN_1024);
   strcpy(parentName, object);
   pChar = strrchr(parentName, '.');
   *(pChar+1) = '\0';

   if (cmsUtl_strcmp(parentName, "") != 0 &&
       omciIpc_fullPathToPathDescriptor(parentName, &pathDesc) == 0)
   {
      memset(buf, 0, sizeof(CmsMsgHeader) + sizeof(omciPacket));

      msg->type = CMS_MSG_OMCI_COMMAND_REQUEST;
      msg->src = EID_HTTPD;
      msg->dst = EID_OMCID;
      msg->flags_event = 1;
      msg->dataLength = sizeof(omciPacket);
      msg->flags_request = 1;

      if (omciIpc_getParamInfo(pathDesc.oid, &paramInfoList, &numInfoEntries) == 0)
      {
         // initialize packet
         memset(packet, 0, sizeof(omciPacket));
         // convert object OID of the first item [0] in the list to management entity class ID
         memset(&info, 0, sizeof(OmciMdmOidClassId_t));
         info.mdmOid = (UINT16)pathDesc.oid;
         omciUtl_oidToClassId(&info);
         meClass = info.classId;
         // get Management Object ID of the second item [1] in the list
         strcpy(fullPathName, parentName);
         strcat(fullPathName, paramInfoList[1].paramName);
         // convert Management Object ID full path to path descriptor
         omciIpc_fullPathToPathDescriptor(fullPathName, &pathDesc);
         // get Management Object ID value using its full path
         ret = cgiOmci_getMeId(pathDesc.oid, pathDesc.iidStack, &meInst);
         if (ret != CMSRET_SUCCESS)
         {
             omciIpc_free(paramInfoList);
             return ret;
         }

         if (macroState == OMCI_MARCO_ON)
         {
            cgiOmci_makePathToOmci(fileName, BUFLEN_264, OMCI_CONF_NAME);
            fs = fopen(fileName, "ab");
            if (fs == NULL)
            {
               cmsLog_error("Could not write Get command to file since error in openning it");
               // free parameter info list
               omciIpc_free(paramInfoList);
               return CMSRET_OPEN_FILE_ERROR;
            }
         }
         for (i = 0; i < NUM_GET_NEXT_OBJECTS; i++)
         {
            if (strstr(object, GetNextObjectsTable[i].name) != NULL)
            {
               break;
            }
         }
         if (i < NUM_GET_NEXT_OBJECTS)
         {
            // message sequence number is position of parameter in the object
            msg->sequenceNumber = 1;
            // transaction ID
            OMCI_HTONS(&packet->tcId, tcIdCur);
            tcIdCur++;

            // message type
            packet->msgType = OMCI_MSG_TYPE_AR(OMCI_MSG_TYPE_GETNEXT);
            // device ID
            packet->devId = OMCI_PACKET_DEV_ID_A;
            OMCI_HTONS(&packet->classNo, meClass);
            OMCI_HTONS(&packet->instId, meInst);

            // attribute mask
            OMCI_HTONS(&OMCI_PACKET_MSG(packet)[0], GetNextObjectsTable[i].setByCreateMask);
            // sequence number
            OMCI_HTONS(&OMCI_PACKET_MSG(packet)[2], numSequence);

            cgiOmci_addCrc(packet, EID_HTTPD);

            if (macroState == OMCI_MARCO_ON)
            {
               for (j = 0; j < msgSize; j++)
               {
                  fputc(buf[j], fs);
               }
            }

            if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not send out CMS_MSG_OMCI_COMMAND_REQUEST, ret=%d, id = %d", ret, i);
            }
            else
            {
               cmsLog_notice("Send CMS_MSG_OMCI_COMMAND_REQUEST with msgType = %d, id = %d", OMCI_MSG_TYPE_GETNEXT, i);
               numMsgTx++;
            }
         }
         if (macroState == OMCI_MARCO_ON)
         {
            fclose(fs);
         }
         // free parameter info list
         omciIpc_free(paramInfoList);
      }
   }

#ifdef OMCI_CFG_DEBUG
   printf("===> cgiOmci_sendGetNextRequest, tcId = %d, msgType = %d, numMsgTx = %d\n",
   packet->tcId, OMCI_MSG_TYPE_GETNEXT, numMsgTx);
#endif
   return ret;	
}

// OMCI Get Next main page
static void cgiOmciGetNextView(FILE *fs)
{
   char msg[BUFLEN_1024*2];

#ifdef OMCI_CFG_DEBUG
printf("===> cgiOmciGetNextView: numSequence = %d, numMsgTx = %d, numMsgRx = %d\n", numSequence, numMsgTx, numMsgRx);
#endif

   writeOmciGetNextHeader(fs);
   writeOmciGetNextScript(fs);
   writeOmciGetNextObject(fs);
   writeOmciGetNextButton(fs);

   if (cmsUtl_strcmp(action, "getnext") == 0)
   {
      if (numMsgTx == numMsgRx)
      {
         cgiOmci_getResultMessage(msg, BUFLEN_1024*2);
         cgiOmci_writeOmciResult(fs, msg);
         numRetry = numMsgTx = numMsgRx = 0;
      }
      else
      {
         if (numRetry < NUM_RETRIES_MAX)
         {
            cgiOmci_writeOmciResult(fs, "HTTPD is sending GET NEXT OMCI command to OMCID. Please wait...");
            numRetry++;
         }
         else
         {
            sprintf(msg, "Failed to communicate with OMCID after %d retries.", numRetry);
            cgiOmci_writeOmciResult(fs, msg);
            numRetry = numMsgTx = numMsgRx = 0;
         }
      }
   }
   else
   {
      cgiOmci_writeOmciResult(fs, "");
   }

   cgiOmci_writeOmciEnd(fs);
}

// Header
static void writeOmciGetNextHeader(FILE *fs) 
{
   fprintf(fs, "<html>\n   <head>\n");
   fprintf(fs, "      <meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "         <link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "         <link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "            <script language=\"javascript\" src=\"util.js\"></script>\n");
   fprintf(fs, "   </head>\n");

   if ((numMsgTx == numMsgRx || numRetry == NUM_RETRIES_MAX) && getNextInProgress == FALSE)
   {
      fprintf(fs, "   <body>\n");
   }	
   else
   {
      fprintf(fs, "   <body onLoad='frmLoad()'>\n");
   }	
   fprintf(fs, "      <blockquote>\n         <form>\n");
}

// Scripts
static void writeOmciGetNextScript(FILE *fs) 
{
#ifdef OMCI_CFG_DEBUG
   printf("===> writeOmciGetNextScript: object = %s, action = %s\n", object, action);
#endif

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   if ((numMsgTx != numMsgRx && numRetry < NUM_RETRIES_MAX) || getNextInProgress == TRUE)
   {
      fprintf(fs, "function frmLoad() {\n");
      fprintf(fs, "   setTimeout('btnGetNext()', 500);\n");
      fprintf(fs, "   with ( document.forms[0] ) {\n");
      fprintf(fs, "      getNextBtn.disabled = true;\n");
      fprintf(fs, "      resetBtn.disabled = true;\n");
      fprintf(fs, "   }\n");
      fprintf(fs, "}\n\n");
   }

   fprintf(fs, "function btnGetNext() {\n");
   fprintf(fs, "   var loc = 'omcigetnext.cmd?';\n\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "      var object = getSelect(selObject);\n");
   fprintf(fs, "      loc += 'selObject=' + object;\n");
   fprintf(fs, "      loc += '&selAction=getnext';\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function btnReset() {\n");
   fprintf(fs, "   var loc = 'omcigetnext.cmd?action=reset';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}

static void writeOmciGetNextObject(FILE *fs) 
{
   SINT32 numEntries = 0, i = 0, j = 0; 
   char objectName[BUFLEN_1024];	
   char *mdmPath = NULL;
   OmciObjectInfo_t *objectInfoListP;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   INIT_INSTANCE_ID_STACK(&iidStack);

   // do nothing if get parameter names failed
   if (omciIpc_getObjectInfo(&objectInfoListP, &numEntries) != 0) return;

   fprintf(fs, "            <b>OMCI -- Get Next</b><br><br>\n");
   fprintf(fs, "            This page allows you to get next OMCI objects through OMCI protocol.<br><br>\n");
   if (macroState == OMCI_MARCO_ON)
   {
      fprintf(fs, "            <font color='red'><b>%s</b></font>\n", OMCI_MSG_MACRO_ON);
   }
   else
   {
      fprintf(fs, "            <font color='green'><b>%s</b></font>\n", OMCI_MSG_MACRO_OFF);
   }
   fprintf(fs, "            <br><br>\n");
   fprintf(fs, "            You can follow the steps below to customize your configuration.<br><br>\n");
   fprintf(fs, "            <font color='green'><b>1. Select the object in the Object list.</font></b><br><br>\n");
   fprintf(fs, "            <table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "               <tr>\n");
   fprintf(fs, "                  <td width='220'>Object:</td>\n");
   fprintf(fs, "                  <td><select name='selObject' size='1'>\n");
   fprintf(fs, "                        <option value=''>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</option>\n");

   for (i = 0; i < NUM_GET_NEXT_OBJECTS; i++)
   {
      // find the available fullpath of parent object
      // then concatenate it with its get next leaf parameter name to make
      // its full path name and add this name to the object list
      for (j = 0; j < numEntries; j++)
      {
         // find the available fullpath of parent object by using oid
         if (objectInfoListP[j].oid == GetNextObjectsTable[i].oid)
         {
            // get the full path of parent object
            if (omciIpc_pathDescriptorToFullPath(&(objectInfoListP[j]), &mdmPath) == 0)
            {
               strcpy(objectName, mdmPath);
               // concatenate the full path of parent object with leaf parameter name
               // to make the full path of child parameter and add it to the object list
               strcat(objectName, GetNextObjectsTable[i].name);
               if (cmsUtl_strcmp(object, objectName) != 0)
               {
                  fprintf(fs, "                        <option value='%s'>%s</option>\n", objectName, objectName);
               }
               else
               {
                  fprintf(fs, "                        <option value='%s' selected>%s</option>\n", objectName, objectName);
               }
               omciIpc_free(mdmPath);
            }
         }
      }
   }

   fprintf(fs, "                     </select>\n");
   fprintf(fs, "                  </td>\n");
   fprintf(fs, "               </tr>\n");
   fprintf(fs, "            </table>\n");

   omciIpc_free(objectInfoListP);
}

static void writeOmciGetNextButton(FILE *fs) 
{
   fprintf(fs, "            <br><br>\n");
   fprintf(fs, "            <font color='green'><b>3. Click on Get Next to run your command or Reset to clear your settings.</font></b><br><br>\n");
   fprintf(fs, "            <table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "               <tr>\n");
   fprintf(fs, "                     <td width='170'>&nbsp;</td>\n");
   fprintf(fs, "                     <td>\n");
   fprintf(fs, "                        <input type='button' onClick='btnGetNext()' value='GetNext' name='getNextBtn'>\n");
   fprintf(fs, "                     </td>\n");
   fprintf(fs, "                     <td>&nbsp;</td>\n");
   fprintf(fs, "                     <td>\n");
   fprintf(fs, "                        <input type='reset' onClick='btnReset()' value='Reset' name='resetBtn'>\n");
   fprintf(fs, "                     </td>\n");
   fprintf(fs, "                     <td>&nbsp;</td>\n");
   fprintf(fs, "               </tr>\n");
   fprintf(fs, "            </table>\n");
}

#endif   //DMP_X_ITU_ORG_GPON_1
