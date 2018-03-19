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
#include "omci_ipc.h"

//#define OMCI_CFG_DEBUG

// global variables
// in cgi_omci_cfg
extern UINT16 tcIdCur;
extern UINT16 numMsgRx;
extern UINT16 numMsgTx;
extern UINT16 numRetry;
// in cgi_omci_macro
extern omciMacroState macroState;

/*
 * local variables
 */
static char object[BUFLEN_264] = {0};
static char action[BUFLEN_264] = {0};
static char *createdObjectParams[NUM_PACKETS_MAX];
static UINT16 numParams = 0;

static omciCreateObject_t CreatedObjectsTable[] = {
   /* Equipment management. */
   {"EquipmentProtectionProfile.", MDMOID_EQPT_PROTECT_PROFILE, MDMOID_EQUIPMENT_MANAGEMENT, 159, 0xFFCC},
   {"ProtectionData.", MDMOID_PROTECTION_DATA, MDMOID_EQUIPMENT_MANAGEMENT, 279, 0xFC00},
   /* ANI management. */
   {"GemInterworkingTp.", MDMOID_GEM_INTERWORKING_TP, MDMOID_ANI_MANAGEMENT, 266, 0xF200},
   {"GemPortNetworkCtp.", MDMOID_GEM_PORT_NETWORK_CTP, MDMOID_ANI_MANAGEMENT, 268, 0xFAC0},
   {"GalEthernetProfile.", MDMOID_GAL_ETHERNET_PROFILE, MDMOID_ANI_MANAGEMENT, 272, 0x8000},
   {"GalEthernetPmHistoryData.", MDMOID_GAL_ETHERNET_PM_HISTORY_DATA, MDMOID_ANI_MANAGEMENT, 276, 0x4000},
   {"GemTrafficDescriptor.", MDMOID_GEM_TRAFFIC_DESCRIPTOR, MDMOID_TRAFFIC_MANAGEMENT, 280, 0xFF00},
   {"MulticastGemInterworkingTp.", MDMOID_MULTICAST_GEM_INTERWORKING_TP, MDMOID_ANI_MANAGEMENT, 281, 0xF300},
   {"FecPmHistoryData.", MDMOID_FEC_PM_HISTORY_DATA, MDMOID_ANI_MANAGEMENT, 312, 0x4000},
   {"GemPortPmHistoryData.", MDMOID_GEM_PORT_PM_HISTORY_DATA, MDMOID_ANI_MANAGEMENT, 341, 0x4000},
   {"EnergyConsumptionPmHistoryData.", MDMOID_ENERGY_CONSUMPTION_PM_HISTORY_DATA, MDMOID_ANI_MANAGEMENT, 343, 0x4000},
   /* Layer 2 management. */
   {"MacBridgeServiceProfile.", MDMOID_MAC_BRIDGE_SERVICE_PROFILE, MDMOID_LAYER2_DATA_SERVICES, 45, 0xFFC0},
   {"MacBridgeConfigData.", MDMOID_MAC_BRIDGE_CONFIG_DATA, MDMOID_LAYER2_DATA_SERVICES, 46, 0x0000},
   {"MacBridgePortConfigData.", MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA, MDMOID_LAYER2_DATA_SERVICES, 47, 0xFF88},
   {"MacBridgePmHistoryData.", MDMOID_MAC_BRIDGE_PM_HISTORY_DATA, MDMOID_LAYER2_DATA_SERVICES, 51, 0x4000},
   {"MacBridgePortPmHistoryData.", MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA, MDMOID_LAYER2_DATA_SERVICES, 52, 0x4000},
   {"VlanTaggingOperationConfigurationData.", MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA, MDMOID_LAYER2_DATA_SERVICES, 78, 0xF800},
   {"VlanTaggingFilterData.", MDMOID_VLAN_TAGGING_FILTER_DATA, MDMOID_LAYER2_DATA_SERVICES, 84, 0xE000},
   {"MapperServiceProfile.", MDMOID_MAPPER_SERVICE_PROFILE, MDMOID_LAYER2_DATA_SERVICES, 130, 0xFFD8},
   {"ExtendedVlanTaggingOperationConfigurationData.", MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA, MDMOID_LAYER2_DATA_SERVICES, 171, 0x8200},
   {"Dot1RateLimiter.", MDMOID_DOT1_RATE_LIMITER, MDMOID_LAYER2_DATA_SERVICES, 298, 0xF800},
   {"MulticastOperationsProfile.", MDMOID_MULTICAST_OPERATIONS_PROFILE, MDMOID_LAYER2_DATA_SERVICES, 309, 0xFC79},
   {"MulticastSubscriberConfigInfo.", MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO, MDMOID_LAYER2_DATA_SERVICES, 310, 0xF800},
   {"MulticastSubscriberMonitor.", MDMOID_MULTICAST_SUBSCRIBER_MONITOR, MDMOID_LAYER2_DATA_SERVICES, 311, 0x8000},
   {"DownstreamEthernetFramePmHistoryData.", MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA, MDMOID_LAYER2_DATA_SERVICES, 321, 0x4000},
   {"UpstreamEthernetFramePmHistoryData.", MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA, MDMOID_LAYER2_DATA_SERVICES, 322, 0x4000},
   {"EthernetFrameExtendedPM.", MDMOID_ETHERNET_FRAME_EXTENDED_P_M, MDMOID_LAYER2_DATA_SERVICES, 334, 0x4000},
   {"EthernetFrameExtendedPM64Bit.", MDMOID_ETHERNET_FRAME_EXTENDED_P_M64_BIT, MDMOID_LAYER2_DATA_SERVICES, 425, 0x4000},
   /* Layer 3 management. */
   {"IpHostPmHistoryData.", MDMOID_IP_HOST_PM_HISTORY_DATA, MDMOID_LAYER3_DATA_SERVICES, 135, 0x4000},
   {"TcpUdpConfigData.", MDMOID_TCP_UDP_CONFIG_DATA, MDMOID_LAYER3_DATA_SERVICES, 136, 0xF000},
   {"TcpUdpPmHistoryData.", MDMOID_TCP_UDP_PM_HISTORY_DATA, MDMOID_LAYER3_DATA_SERVICES, 342, 0x4000},
   /* Ethernet management. */
   {"EthernetPmHistoryData.", MDMOID_ETHERNET_PM_HISTORY_DATA, MDMOID_ETHERNET_SERVICES, 24, 0x4000},
   {"EthernetPmHistoryData2.", MDMOID_ETHERNET_PM_HISTORY_DATA2, MDMOID_ETHERNET_SERVICES, 89, 0x4000},
   {"EthernetPmHistoryData3.", MDMOID_ETHERNET_PM_HISTORY_DATA3, MDMOID_ETHERNET_SERVICES, 296, 0x4000},
   /* General. */
   {"NetworkAddress.", MDMOID_NETWORK_ADDRESS, MDMOID_GENERAL, 137, 0xC000},
   {"LargeString.", MDMOID_LARGE_STRING, MDMOID_GENERAL, 157, 0x0000},
   {"ThresholdData1.", MDMOID_THRESHOLD_DATA1, MDMOID_GENERAL, 273, 0xFE00},
   {"ThresholdData2.", MDMOID_THRESHOLD_DATA2, MDMOID_GENERAL, 274, 0xFE00},
   {"OctetString.", MDMOID_OCTET_STRING, MDMOID_GENERAL, 307, 0x0000},
   {"GeneralPurposeBuffer.", MDMOID_GENERAL_PURPOSE_BUFFER, MDMOID_GENERAL, 308, 0x8000},
   {"GenericStatusPortal.", MDMOID_GENERIC_STATUS_PORTAL, MDMOID_GENERAL, 330, 0x2000},
   {"ThresholdData64Bit.", MDMOID_THRESHOLD_DATA64_BIT, MDMOID_GENERAL, 426, 0x0000},
#ifdef DMP_X_ITU_ORG_VOICE_1
   /* Voice. */
   {"VoiceServiceProfile.", MDMOID_VOICE_SERVICE, MDMOID_VOICE_SERVICES, 58, 0xFF84},
   {"VoIpVoiceCtp.", MDMOID_VO_IP_VOICE_CTP, MDMOID_VOICE_SERVICES, 139, 0xF000},
   {"CallControlPmHistoryData.", MDMOID_CALL_CONTROL_PM_HISTORY_DATA, MDMOID_VOICE_SERVICES, 140, 0x4000},
   {"VoIpMediaProfile.", MDMOID_VO_IP_MEDIA_PROFILE, MDMOID_VOICE_SERVICES, 142, 0xFFFF},
   {"RtpProfileData.", MDMOID_RTP_PROFILE_DATA, MDMOID_VOICE_SERVICES, 143, 0xFE00},
   {"RtpPmHistoryData.", MDMOID_RTP_PM_HISTORY_DATA, MDMOID_VOICE_SERVICES, 144, 0x4000},
   {"NetworkDialPlanTable.", MDMOID_NETWORK_DIAL_PLAN_TABLE, MDMOID_VOICE_SERVICES, 145, 0x7800},
   {"VoIpAppServiceProfile.", MDMOID_VO_IP_APP_SERVICE_PROFILE, MDMOID_VOICE_SERVICES, 146, 0xFF00},
   {"VoiceFeatureAccessCodes.", MDMOID_VOICE_FEATURE_ACCESS_CODES, MDMOID_VOICE_SERVICES, 147, 0x0000},
   {"AuthenticationSecurityMethod.", MDMOID_AUTHENTICATION_SECURITY_METHOD, MDMOID_GENERAL, 148, 0x0000},
#ifdef DMP_X_ITU_ORG_VOICE_SIP_1
   {"SipAgentConfigData.", MDMOID_SIP_AGENT_CONFIG_DATA, MDMOID_VOICE_SERVICES, 150, 0xF16C},
   {"SipAgentPmHistoryData.", MDMOID_SIP_AGENT_PM_HISTORY_DATA, MDMOID_VOICE_SERVICES, 151, 0x4000},
   {"SipCallInitPmHistoryData.", MDMOID_SIP_CALL_INIT_PM_HISTORY_DATA, MDMOID_VOICE_SERVICES, 152, 0x4000},
   {"SipUserData.", MDMOID_SIP_USER_DATA, MDMOID_VOICE_SERVICES, 153, 0xDFC0},
#endif /* DMP_X_ITU_ORG_VOICE_SIP_1 */
#ifdef DMP_X_ITU_ORG_VOICE_MGC_1
   {"MgcConfigData.", MDMOID_MGC_CONFIG_DATA, MDMOID_VOICE_SERVICES, 155, 0xFA60},
   {"MgcPmHistoryData.", MDMOID_MGC_PM_HISTORY_DATA, MDMOID_VOICE_SERVICES, 156, 0x4000},
#endif /* DMP_X_ITU_ORG_VOICE_MGC_1 */
#endif /* DMP_X_ITU_ORG_VOICE_1 */
   /* MOCA. */
   {"MocaEthernetPmHistoryData.", MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA, MDMOID_MOCA_SERVICES, 163, 0x4000},
   {"MocaInterfacePmHistoryData.", MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA, MDMOID_MOCA_SERVICES, 164, 0x4000},
   };

#define NUM_CREATED_OBJECTS (SINT32) (sizeof(CreatedObjectsTable) / sizeof(omciCreateObject_t))

/* local functions */

static void cgiOmci_initParamNamesForCreatedObject(void);
static CmsRet cgiOmci_sendCreateRequest(char *query);
static void cgiOmciCreateView(FILE *fs);
static void writeOmciCreateHeader(FILE *fs);
static void writeOmciCreateScript(FILE *fs);
static void writeOmciCreateObject(FILE *fst);
static void writeOmciCreateParameters(FILE *fs);
static void writeOmciCreateButton(FILE *fs);

/* public functions */

// Main entry for OMCI create web page
void cgiOmciCreate(char *query, FILE *fs) 
{
   object[0] = action[0] = '\0';

   /* OMCI operations do not need CMS lock. */
   cmsLck_releaseLock();

   cgiGetValueByName(query, "selObject", object);
   cgiGetValueByName(query, "selAction", action);

   if (object[0] != '\0')
   {
      if (cmsUtl_strcmp(action, "add") == 0)
      {
          // get parameter names of the created object
         cgiOmci_initParamNamesForCreatedObject();
      }
      else if (cmsUtl_strcmp(action, "create") == 0)
      {
         if (numMsgTx == 0)
         {
            numMsgRx = 0;
            cgiOmci_sendCreateRequest(query);
         }
      }
      else if (cmsUtl_strcmp(action, "reset") == 0)
      {
          // action is reset so clear all selected value
         object[0] = action[0] = '\0';
      }
   }
   else
   {
      // after upload macro file, then
      // reset numMsgTx and numMsgRx
      numMsgTx = numMsgRx = 0;
   }

   cmsLck_acquireLockWithTimeout(HTTPD_LOCK_TIMEOUT);

   cgiOmciCreateView(fs);
}

/* local functions */

static void cgiOmci_initParamNamesForCreatedObject(void)
{
   SINT32 numInfoEntries = 0, i = 0; 
   OmciParamInfo_t *paramInfoList;
   OmciObjectInfo_t pathDesc;

   // reset number of parameters in the created object
   // max number of parameters should be NUM_PACKETS_MAX (16)
   numParams = 0;
   for (i = 0; i < NUM_PACKETS_MAX; i++)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(createdObjectParams[i]);
   }

   // convert object name to path descriptor
   if (cmsUtl_strcmp(object, "") != 0 &&
       omciIpc_fullPathToPathDescriptor(object, &pathDesc) == 0)
   {
         // get parameter names of created object
         if (omciIpc_getParamInfo(pathDesc.oid, &paramInfoList, &numInfoEntries) == 0)
         {
            UINT16 setByCreateMask = 0;
            UBOOL8 isSetByCreateAttribute = FALSE;

            // find setByCreateMask for this created object by using its oid
            for (i = 0; i < NUM_CREATED_OBJECTS; i++)
            {
               if (CreatedObjectsTable[i].oid == pathDesc.oid)
               {
                  setByCreateMask = CreatedObjectsTable[i].setByCreateMask;
                  break;
               }
            }

            // 1st parameter is ManagedEntityId that is always set-by-create attribute
            createdObjectParams[numParams++] = cmsMem_strdup(paramInfoList[0].paramName);
            for (i = 1; i < numInfoEntries; i++)
            {
               isSetByCreateAttribute = (setByCreateMask >> (16 - i)) & 0x01;
               // only count set-by-create attribute
               if (paramInfoList[i].paramName[0] != '\0' && isSetByCreateAttribute == TRUE)
               {
                  createdObjectParams[numParams++] = cmsMem_strdup(paramInfoList[i].paramName);
               }
            }
            // free parameter information list
            omciIpc_free(paramInfoList);
         }
   }

#ifdef OMCI_CFG_DEBUG
   printf("===> cgiOmci_initParamNamesForCreatedObject: object = %s, action = %s, numParams = %d\n",
   object, action, numParams);
#endif
}

static CmsRet cgiOmci_sendCreateRequest(char *query) 
{
   SINT32 paramIndex = 0, msgIndex = 0;
   SINT32 numInfoEntries = 0, i = 0; 
   UINT16 j = 0, msgSize = sizeof(CmsMsgHeader) + sizeof(omciPacket);
   char paramName[BUFLEN_16], paramValue[BUFLEN_256];
   OmciParamInfo_t *paramInfoList;
   OmciObjectInfo_t pathDesc;
   UINT16 meClass = 0, meInst = 0;
   char buf[msgSize];
   CmsMsgHeader *msg=(CmsMsgHeader *) buf;
   omciPacket *packet = (omciPacket *) (msg+1);
   OmciMdmOidClassId_t info;
   CmsRet ret = CMSRET_SUCCESS;

#ifdef OMCI_CFG_DEBUG
   printf("===> cgiOmci_sendCreateRequest, query = %s\n", query);
#endif

   // convert object name to path descriptor
   if (cmsUtl_strcmp(object, "") != 0 &&
       omciIpc_fullPathToPathDescriptor(object, &pathDesc) == 0)
   {
      memset(buf, 0, sizeof(CmsMsgHeader) + sizeof(omciPacket));

      msg->type = CMS_MSG_OMCI_COMMAND_REQUEST;
      msg->src = EID_HTTPD;
      msg->dst = EID_OMCID;
      msg->flags_event = 1;
      msg->dataLength = sizeof(omciPacket);
      msg->flags_request = 1;

         // initialize packet
         memset(packet, 0, sizeof(omciPacket));
         // convert object OID of the first item [0] in the list to management entity class ID
         memset(&info, 0, sizeof(OmciMdmOidClassId_t));
         info.mdmOid = (UINT16)pathDesc.oid;
         omciUtl_oidToClassId(&info);
         meClass = info.classId;
         // get ManagedEntityId which is the first parameter in queury
         cgiGetValueByName(query, "param0", paramValue);
         meInst = strtoul(paramValue, (char **)NULL, 10);
         // message sequence number is position of parameter in the object
         msg->sequenceNumber = 1;
         // transaction ID
         OMCI_HTONS(&packet->tcId, tcIdCur);
         tcIdCur++;

         // message type
         packet->msgType = OMCI_MSG_TYPE_AR(OMCI_MSG_TYPE_CREATE);
         // device ID
         packet->devId = OMCI_PACKET_DEV_ID_A;
         OMCI_HTONS(&packet->classNo, meClass);
         OMCI_HTONS(&packet->instId, meInst);

         // get parameter names of created object
         if (omciIpc_getParamInfo(pathDesc.oid, &paramInfoList, &numInfoEntries) == 0)
         {
            UINT16 setByCreateMask = 0;
            UBOOL8 isSetByCreateAttribute = FALSE;

            // find setByCreateMask for this created object by using its oid
            for (i = 0; i < NUM_CREATED_OBJECTS; i++)
            {
               if (CreatedObjectsTable[i].oid == pathDesc.oid)
               {
                  setByCreateMask = CreatedObjectsTable[i].setByCreateMask;
                  break;
               }
            }
            msgIndex = OMCI_CREATE_OVERHEAD;
            for (i = 1, paramIndex = 1; i < numInfoEntries; i++)
            {
               isSetByCreateAttribute = (setByCreateMask >> (16 - i)) & 0x01;
               // only configure set-by-create attribute
               if (paramInfoList[i].paramName[0] != '\0' && isSetByCreateAttribute == TRUE)
               {
                  sprintf(paramName, "param%d", paramIndex);
                  cgiGetValueByName(query, paramName, paramValue);
                  cgiOmci_setParameterValue(packet, &msgIndex, pathDesc, i - 1, paramValue);
                  paramIndex++;
#ifdef OMCI_CFG_DEBUG
printf("===> cgiOmci_sendCreateRequest, paramName = %s, paramValue = %s, msgIndex = %d\n", paramInfoList[i].paramName, paramValue, msgIndex);
#endif
               }
            }
            // free paramInfoList
            omciIpc_free(paramInfoList);

            cgiOmci_addCrc(packet, EID_HTTPD);

            if (macroState == OMCI_MARCO_ON)
            {
               char fileName[BUFLEN_264];
               FILE *fs = NULL;

               cgiOmci_makePathToOmci(fileName, BUFLEN_264, OMCI_CONF_NAME);
               fs = fopen(fileName, "ab");
               if (fs != NULL)
               {
                  for (j = 0; j < msgSize; j++)
                  {
                     fputc(buf[j], fs);
                  }
                  fclose(fs);
               }
               else
               {
                  cmsLog_error("Could not write Create command to file since error in openning it");
               }
               if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Could not send out CMS_MSG_OMCI_COMMAND_REQUEST, ret=%d, id = %d", ret, i);
               }
               else
               {
                  cmsLog_notice("Send CMS_MSG_OMCI_COMMAND_REQUEST with msgType = %d, id = %d", OMCI_MSG_TYPE_CREATE, i);
                  numMsgTx++;
               }
            }
            else
            {
               if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Could not send out CMS_MSG_OMCI_COMMAND_REQUEST, ret=%d, id = %d", ret, i);
               }
               else
               {
                  cmsLog_notice("Send CMS_MSG_OMCI_COMMAND_REQUEST with msgType = %d, id = %d", OMCI_MSG_TYPE_CREATE, i);
                  numMsgTx++;
               }
            }
         }
   }

   return ret;
}

// OMCI create main page
static void cgiOmciCreateView(FILE *fs)
{
   char msg[BUFLEN_1024*2];

#ifdef OMCI_CFG_DEBUG
   printf("===> cgiOmciCreateView: object = %s, action = %s, numMsgTx = %d, numMsgRx = %d\n",
   object, action, numMsgTx, numMsgRx);
#endif

   writeOmciCreateHeader(fs);
   writeOmciCreateScript(fs);
   writeOmciCreateObject(fs);
   writeOmciCreateParameters(fs);
   writeOmciCreateButton(fs);

   if (cmsUtl_strcmp(action, "create") == 0)
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
            cgiOmci_writeOmciResult(fs, "HTTPD is sending CREATE OMCI command to OMCID. Please wait...");
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
static void writeOmciCreateHeader(FILE *fs) 
{
   fprintf(fs, "<html>\n   <head>\n");
   fprintf(fs, "      <meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "         <link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "         <link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "            <script language=\"javascript\" src=\"util.js\"></script>\n");
   fprintf(fs, "   </head>\n");
   if (numMsgTx == numMsgRx || numRetry == NUM_RETRIES_MAX)
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
static void writeOmciCreateScript(FILE *fs) 
{
   SINT32 i = 0; 

#ifdef OMCI_CFG_DEBUG
   printf("===> writeOmciCreateScript: object = %s, action = %s, numParams = %d\n",
   object, action, numParams);
#endif

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   if (numMsgTx != numMsgRx && numRetry < NUM_RETRIES_MAX)
   {
      fprintf(fs, "function frmLoad() {\n");
      fprintf(fs, "   setTimeout('btnCreate()', 1000);\n");
      fprintf(fs, "   with ( document.forms[0] ) {\n");
      fprintf(fs, "      createBtn.disabled = true;\n");
      fprintf(fs, "      resetBtn.disabled = true;\n");
      fprintf(fs, "   }\n");
      fprintf(fs, "}\n\n");
   }

   fprintf(fs, "function objectSelect() {\n");
   fprintf(fs, "   var loc = 'omcicreate.cmd?';\n\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "      var object = getSelect(selObject);\n");
   fprintf(fs, "      loc += 'selObject=' + object;\n");
   fprintf(fs, "      loc += '&selAction=add';\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function btnCreate() {\n");
   fprintf(fs, "   var loc = 'omcicreate.cmd?';\n\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "      var object = getSelect(selObject);\n");
   fprintf(fs, "      loc += 'selObject=' + object;\n");
   fprintf(fs, "      loc += '&selAction=create';\n");
   for (i = 0; i < numParams && createdObjectParams[i] != NULL; i++)
   {
      fprintf(fs, "      loc += '&param%d=' + param%d.value;\n", i, i);
   }
   fprintf(fs, "   }\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function btnReset() {\n");
   fprintf(fs, "   var loc = 'omcicreate.cmd?action=reset';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}

static void writeOmciCreateObject(FILE *fs) 
{
   SINT32 numEntries = 0, i = 0, j = 0; 
   char objectName[BUFLEN_1024];	
   char *mdmPath = NULL;
   OmciObjectInfo_t *objectInfoListP;
#ifdef OMCI_CFG_DEBUG
   printf("===> writeOmciCreateObject: object = %s, action = %s, numParams = %d\n",
   object, action, numParams);
#endif

   // do nothing if get parameter names failed
   if (omciIpc_getSupportedObjectInfo(&objectInfoListP, &numEntries) != 0) return;
	
   fprintf(fs, "            <b>OMCI -- Creation</b><br><br>\n");            
   fprintf(fs, "            This page allows you to create OMCI objects through OMCI protocol.<br><br>\n");
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
   fprintf(fs, "                  <td><select name='selObject' size='1' onChange='objectSelect()'>\n");
   fprintf(fs, "                        <option value=''>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</option>\n");

   for (i = 0; i < NUM_CREATED_OBJECTS; i++)
   {
      {
         // created object is a child of another instance object
         // so find the available fullpath of parent objects
         // then concatenate it with its created object name to make
         // its full path name and add this name to the object list
         for (j = 0; j < numEntries; j++)
         {
            // find the available fullpath of parent objects by using parent oid
            if (objectInfoListP[j].oid == CreatedObjectsTable[i].parentOid)
            {
               // get the full path of parent object      
               if (omciIpc_pathDescriptorToFullPath(&(objectInfoListP[j]), &mdmPath) == 0)
               {
                  strcpy(objectName, mdmPath);
                  // concatenate the full path of parent object with child object
                  // to make the full path of child object and add it to the object list
                  strcat(objectName, CreatedObjectsTable[i].name);
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
   }

   fprintf(fs, "                     </select>\n");
   fprintf(fs, "                  </td>\n");
   fprintf(fs, "               </tr>\n");
   fprintf(fs, "            </table>\n");

   omciIpc_free(objectInfoListP);
}

static void writeOmciCreateParameters(FILE *fs) 
{
   SINT32 i = 0; 

#ifdef OMCI_CFG_DEBUG
   printf("===> writeOmciCreateParameters: object = %s, action = %s, numParams = %d\n",
   object, action, numParams);
#endif

   fprintf(fs, "            <br><br>\n");
   fprintf(fs, "            <table border='0' cellpadding='0' cellspacing='0'>\n");
   for (i = 0; i < numParams && createdObjectParams[i] != NULL; i++)
   {
      fprintf(fs, "               <tr>\n");
      fprintf(fs, "                  <td width='220'>%s:</td>\n", createdObjectParams[i]);
      fprintf(fs, "                  <td><input type='text' name='param%d' size='44'></td>\n", i);
      fprintf(fs, "               </tr>\n");
   }
   fprintf(fs, "            </table>\n");
}

static void writeOmciCreateButton(FILE *fs) 
{
   fprintf(fs, "            <br><br>\n");
   fprintf(fs, "            <font color='green'><b>3. Click on Create to run your command or Reset to clear your settings.</font></b><br><br>\n");
   fprintf(fs, "            <table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "               <tr>\n");
   fprintf(fs, "                     <td width='170'>&nbsp;</td>\n");
   fprintf(fs, "                     <td>\n");
   fprintf(fs, "                        <input type='button' onClick='btnCreate()' value='Create' name='createBtn'>\n");
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
