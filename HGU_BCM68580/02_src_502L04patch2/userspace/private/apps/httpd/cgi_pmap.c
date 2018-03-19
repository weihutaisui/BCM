/***********************************************************************
 *
 *  Copyright (c) 2006-2008  Broadcom Corporation
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


#include "cms.h"

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
#include <sys/utsname.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <linux/if.h>
#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_main.h"
#include "syscall.h"
#include "sysdiag.h"
#include "httpd.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"

#define END_PMAP_ID             6 
#define LAN_DATA_PATH           "/var/lan"
#define PMAP_MAX_ENTRY          16


extern char glbErrMsg[BUFLEN_264];
extern int glbEntryCt;   // used for web page to know how many entries can be added. In cgimain.c
static void writePortMapScript(FILE *fs, UBOOL8 checked, unsigned int numOfGroup);
static void cgiPortMapApplySave(char *query, FILE *fs);

void cgiPortMapCfg(char *query, FILE *fs) {
   char action[BUFLEN_264];
   cgiGetValueByName(query, "action", action);

   /*
    * Get latest switch info from the MDM in case some other
    * mgmt entity has changed it.
    */
   dalEsw_getEthernetSwitchInfo(&glbWebVar);
   
   if ( strcmp(action, "add") == 0 )
      cgiPortMapAdd(query, fs);
   else if ( strcmp(action, "savapply") == 0 )
      cgiPortMapApplySave(query, fs);
   else if ( strcmp(action, "view") == 0 )
      cgiPortMapView(fs);
   else
      cgiPortMapView(fs);
}

void cgiPortMapAdd(char *query, FILE *fs) {
   char grpName[BUFLEN_32];
   char grpedIntf[BUFLEN_1024];
   char wanIfName[BUFLEN_32];
   char cmd[BUFLEN_1024];
   char vendorIdKey[BUFLEN_32];
   char vendorIdValue[DHCP_VENDOR_ID_LEN + 1];
   int  index = 0;
   char *pIfName = NULL;
   char *pIfNameEnd = NULL;
   char ifName[BUFLEN_32];
   char vendorIds[MAX_PORTMAPPING_DHCP_VENDOR_IDS * (DHCP_VENDOR_ID_LEN + 1)];
   UINT32 vendorIdsTotalLen=0;
   char *vendorIdAggregateString;
   char *vptr;
   CmsRet ret;


   grpName[0]   = '\0';
   grpedIntf[0] = '\0';
   wanIfName[0] = '\0';
   cmd[0]       = '\0';
   ifName[0]    = '\0';

   cgiGetValueByName(query, "groupName", grpName);
   cgiGetValueByName(query, "choiceBox", grpedIntf);
   cgiGetValueByName(query, "wanIfName", wanIfName);

   cmsLog_debug("groupName=%s choiceBox=%s wanIfName=%s", grpName, grpedIntf, wanIfName);

   // Set the L2BridgeEntry object to create the new bridge group.
   if (dalPMap_addBridge(INTFGRP_BR_HOST_MODE, grpName) != CMSRET_SUCCESS) {
         sprintf(cmd, "Configure port map failed. Group name %s cannot set the object",
                 grpName);
         cgiWriteMessagePage(fs, "Port Map Add Error", cmd,
                             "portmapcfg.cmd?action=viewcfg");
         return;
   }


   // Extract the interfaces filter information.
   pIfName = grpedIntf;
   while (pIfName <= (grpedIntf + strlen(grpedIntf))) {
      if ((pIfNameEnd = strstr(pIfName, "|")) != NULL) {
         memset(ifName, '\0', BUFLEN_32);
         memcpy(ifName, pIfName, (pIfNameEnd - pIfName));
         if (*ifName != '\0')
         {
            cmsLog_debug("associate filter intf %s to bridge %s", ifName, grpName);
            if (dalPMap_assocFilterIntfToBridge(ifName, grpName) != CMSRET_SUCCESS) {
               sprintf(cmd, "Configure interface grouping failed. Group name %s cannot set the object",
                       grpName);
               cgiWriteMessagePage(fs, "Port Map Filter Add Error", cmd,
                                   "portmapcfg.cmd?action=viewcfg");
               return;
            }
         }
      } else {
         break;
      }
      pIfName = pIfNameEnd + 1;
   }
   
   
   // Extract DHCP vendor ID info
   for (index = 0; index < MAX_PORTMAPPING_DHCP_VENDOR_IDS; index++) {
         sprintf(vendorIdKey, "vendorid%d", index);
         memset(vendorIdValue, 0, sizeof(vendorIdValue));
         cgiGetValueByName(query, vendorIdKey, vendorIdValue);

         vptr = &(vendorIds[index * (DHCP_VENDOR_ID_LEN + 1)]);
         strncpy(vptr, vendorIdValue, DHCP_VENDOR_ID_LEN + 1);
         cmsLog_debug("got vendorid[%d]=%s (strlen=%d)", index, vptr, strlen(vptr));
         vendorIdsTotalLen += strlen(vptr); 
   }

   if (vendorIdsTotalLen > 0)
   {
      vendorIdAggregateString = cmsUtl_getAggregateStringFromDhcpVendorIds(vendorIds);
      if ((ret = dalPMap_addFilterDhcpVendorId(grpName, vendorIdAggregateString)) != CMSRET_SUCCESS)
      {
         cmsLog_debug("failed to add DhcpVendorId aggregate string");
      }
      
      CMSMEM_FREE_BUF_AND_NULL_PTR(vendorIdAggregateString);
   }
   
   
   /* associate  WAN interface to this bridge group, if it is real interface (nas_xx_xx)
    * if it is "None", it is a multiSubnet case and skip the assoication 
    */
   if ((*wanIfName != '\0') && (cmsUtl_strcmp(wanIfName, "None") != 0))
   {
      if (dalPMap_assocFilterIntfToBridge(wanIfName, grpName) != CMSRET_SUCCESS) 
      {
         sprintf(cmd, "Configure interface grouping failed. Group name %s cannot set the object",
            grpName);
         cgiWriteMessagePage(fs, "Interface grouping Filter Add Error", cmd,
                             "portmapcfg.cmd?action=viewcfg");      
      }
      
   }

   glbSaveConfigNeeded = TRUE;
   cgiPortMapView(fs);
}


static CmsRet portmapDeleteBridges(char *lst)
{
   CmsRet ret = CMSRET_SUCCESS;
   char   *pToken = NULL;
   char   *pLast = NULL;
   char   groupName[BUFLEN_32]={0};

   cmsLog_debug("list of bridges to remove = %s", lst);

   pToken = strtok_r(lst, ",", &pLast);
   while ( pToken != NULL )
   {
      cmsUtl_strncpy(groupName, pToken, sizeof(groupName));

      cmsLog_debug("delete all dhcp vendor id filters from bridge %s", groupName);
      dalPMap_deleteFilterDhcpVendorId(groupName);

      cmsLog_debug("disassociate all filter intf from bridge %s", groupName);
      dalPMap_disassocAllFilterIntfFromBridge(groupName); 
      
      cmsLog_debug("delete intf group/bridge %s", groupName);
      dalPMap_deleteBridge(groupName);

      pToken = strtok_r(NULL, ", ", &pLast);
   }  

   return ret;
}


static void cgiPortMapApplySave(char *query, FILE *fs) {
   char rmList[WEB_BUF_SIZE_MAX]={0};
   char str[BUFLEN_264]={0};
   CmsRet ret;
   

   cgiGetValueByName(query, "rml", rmList);
   cmsLog_debug("rmlist=%s", rmList);


   ret = portmapDeleteBridges(rmList);
   if( ret != CMSRET_SUCCESS)
   {
      cgiWriteMessagePage(fs, "Port Map Remove Error", str,
                          "portmapcfg.cmd?action=viewcfg");
   }
   else
   {
      glbSaveConfigNeeded = TRUE;

      cgiPortMapView(fs);
   }

   return;
}


UINT32 cgiGetNumberOfIntfGroups_igd()
{
   UINT32 numOfBr = 0;
   InstanceIdStack brdIidStack = EMPTY_INSTANCE_ID_STACK;
   L2BridgingEntryObject *pBridgeObj = NULL;
   CmsRet ret;

   while ((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_ENTRY, &brdIidStack,
                      (void **)&pBridgeObj)) == CMSRET_SUCCESS) {
     /* mwang_todo: the rcl handler function should update the numberOfBridgingEntries in the layer2bridging object.
      * this function just need to get that object and the count, instead of looping here */
      numOfBr++;
      cmsObj_free((void **)&pBridgeObj);
   }

   return numOfBr;
}


void cgiGetIntfGroupInfo_igd(UINT32 brIdx,
             char *groupName, UINT32 groupNameLen,
             char *intfList, UINT32 intfListLen, UINT32 *numIntf,
             char *wanIntfList, UINT32 wanIntfListLen, UINT32 *numWanIntf, char *wanIfName,
             char *vendorIdList, UINT32 vendorIdListLen, UINT32 *numVendorIds)
{

   L2BridgingEntryObject          *pBridgeObj = NULL;
   L2BridgingFilterObject         *pBridgeFltObj = NULL;
   InstanceIdStack iidStack    = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack fltIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UBOOL8 found=FALSE;
   UINT32 i=0;


   while (!found &&
          (ret = cmsObj_getNext(MDMOID_L2_BRIDGING_ENTRY, &iidStack,
                               (void **)&pBridgeObj)) == CMSRET_SUCCESS)
   {
      if (i != brIdx)
      {
         cmsObj_free((void **) &pBridgeObj);
         i++;
         continue;
      }

      /* this is the bridge entry we want, gather info */
      found = TRUE;

      //Group name
      if (pBridgeObj->bridgeName) {
         cmsUtl_strncpy(groupName, pBridgeObj->bridgeName, groupNameLen);
      }

      // Get the interface names associated with this bridge
      INIT_INSTANCE_ID_STACK(&fltIidStack);
      while ((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_FILTER, &fltIidStack,
                               (void **)&pBridgeFltObj)) == CMSRET_SUCCESS)
      {
         if ((pBridgeFltObj->filterBridgeReference == (SINT32) pBridgeObj->bridgeKey) &&
             (cmsUtl_strcmp(pBridgeFltObj->filterInterface, MDMVS_LANINTERFACES)))
         {
            L2BridgingIntfObject *availIntfObj=NULL;
            InstanceIdStack availIntfIidStack =  EMPTY_INSTANCE_ID_STACK;
            char lanIfName[BUFLEN_32]={0};
            UINT32 key;
            CmsRet r3;

            cmsUtl_strtoul(pBridgeFltObj->filterInterface, NULL, 0, &key);
            if((r3 = dalPMap_getAvailableInterfaceByKey(key, &availIntfIidStack, &availIntfObj)) != CMSRET_SUCCESS)
            {
               cmsLog_error("could not find avail intf for key %u", key);
            }
            else
            {
               /* we only want to list LAN interface in this section.  WAN interfaces will be in a separate drop down selection menu */
               if (!cmsUtl_strcmp(availIntfObj->interfaceType, MDMVS_LANINTERFACE))
               {
                  if (intfList[0] != '\0')
                  {
                     strcat(intfList, "|");
                  }
                  //attach interface name
                  dalPMap_availableInterfaceReferenceToIfName(availIntfObj->interfaceReference, lanIfName);
   #ifdef SUPPORT_LANVLAN
                  if (strstr(lanIfName, "eth"))
                  {
			
                     char lanIfName2[BUFLEN_32]={0};
                     strncpy(lanIfName2, lanIfName, sizeof(lanIfName2));
                     snprintf(lanIfName, sizeof(lanIfName), "%s.%d", lanIfName2, pBridgeFltObj->X_BROADCOM_COM_VLANIDFilter);
                  }
   #endif
                  cmsUtl_strncat(intfList, intfListLen, lanIfName);
                  (*numIntf)++;
               }
               else if (!cmsUtl_strcmp(availIntfObj->interfaceType, MDMVS_WANINTERFACE))
               {
                  if (wanIntfList[0] != '\0')
                  {
                     strcat(wanIntfList, "|");
                  }
                  //attach interface name
                  dalPMap_availableInterfaceReferenceToIfName(availIntfObj->interfaceReference, wanIfName);
                  cmsUtl_strncat(wanIntfList, wanIntfListLen, wanIfName);
                  (*numWanIntf)++;
               }

               cmsObj_free((void **) &availIntfObj);
            }
         }

         cmsObj_free((void **)&pBridgeFltObj);
      }


      // Get the DHCP vendor ids associated with this bridge
      if ((ret = dalPMap_getFilterDhcpVendorIdByBridgeName(groupName, &fltIidStack, &pBridgeFltObj)) == CMSRET_SUCCESS)
      {
         char *vendorIdString;

         cmsLog_debug("got dhcp vendor id filter object for bridge %s, aggregateString=%s", groupName, pBridgeFltObj->sourceMACFromVendorClassIDFilter);

         if ((vendorIdString = cmsUtl_getDhcpVendorIdsFromAggregateString(pBridgeFltObj->sourceMACFromVendorClassIDFilter)) != NULL)
         {
            char *vptr;
            UINT32 j;

            for (j=0; j < MAX_PORTMAPPING_DHCP_VENDOR_IDS; j++)
            {
               vptr = &(vendorIdString[j * (DHCP_VENDOR_ID_LEN+1)]);
               if (*vptr != '\0')
               {
                  if (vendorIdList[0] != '\0')
                  {
                     strcat(vendorIdList, "|");
                  }

                  cmsUtl_strncat(vendorIdList, vendorIdListLen, vptr);
                  (*numVendorIds)++;
               }
            }

            CMSMEM_FREE_BUF_AND_NULL_PTR(vendorIdString);
         }

         cmsObj_free((void **) &pBridgeFltObj);
      }

      cmsObj_free((void **) &pBridgeObj);
      i++;
   }

   return;
}


void cgiPortMapView(FILE *fs) {
   char   groupName[BUFLEN_64]={0};
   char   intfList[BUFLEN_512]={0};
   char   wanIntfList[BUFLEN_264]={0};
   char   wanIfName[BUFLEN_32]={0};
   char   vendorIdList[MAX_PORTMAPPING_DHCP_VENDOR_IDS * (DHCP_VENDOR_ID_LEN + 1)]={0};
   UINT32 numOfVendorIds;
   UINT32 numOfIntf;
   UINT32 numOfWanIntf;
   UINT32 rowspan;
   UINT32 numOfBr;
   UINT32 brIdx;


   glbEntryCt = 0;

   numOfBr = cgiGetNumberOfIntfGroups();
   cmsLog_debug("numOfBr=%d", numOfBr);


  fprintf(fs, "<html><head>\n");
  fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
  fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
  fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
  fprintf(fs, "<script language=\"javascript\" src=\"portName.js\"></script>\n");

  // write Java Script
  /* the second arg indicates whether virtual ports is currently in the
   * temorarily enabled state in httpd. */

  writePortMapScript(fs, TRUE, numOfBr);

  // write body
  fprintf(fs, "</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");

  fprintf(fs, "<b>Interface Grouping -- A maximum %d entries can be configured</b><br><br>\n",
			PMAP_MAX_ENTRY);
  fprintf(fs, "Interface Grouping supports multiple ports to PVC and bridging groups. Each group\n");
  fprintf(fs, "will perform as an independent network. To support this feature, you must\n");
  fprintf(fs, "create mapping groups with appropriate LAN and WAN interfaces using\n");
  fprintf(fs, "the Add button. The Remove button will remove the grouping and add the\n");
  fprintf(fs,
          "ungrouped interfaces to the Default group. Only the default group has IP interface.<br><br>\n");


  fprintf(fs, "<br><br>\n");
  fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
  // write table header
  fprintf(fs, "   <tr>\n");
  fprintf(fs, "      <td class='hd'>Group Name</td>\n");
//  fprintf(fs, "      <td class='hd'>Enable/Disable</td>\n");
  fprintf(fs, "      <td class='hd'>Remove</td>\n");
  fprintf(fs, "      <td class='hd'>WAN Interface</td>\n");
  fprintf(fs, "      <td class='hd'>LAN Interfaces</td>\n");
  fprintf(fs, "      <td class='hd'>DHCP Vendor IDs</td>\n");
  fprintf(fs, "   </tr>\n");

  for (brIdx=0; brIdx < numOfBr; brIdx++)
  {
     memset(groupName, 0, sizeof(groupName));
     numOfIntf = 0;
     numOfVendorIds = 0;
     numOfWanIntf = 0;

     //Initialization for getting interfaces information
     memset(intfList, 0, sizeof(intfList));
     memset(wanIntfList, 0, sizeof(wanIntfList));
     memset(vendorIdList, 0, sizeof(vendorIdList));
     memset(wanIfName, 0, sizeof(wanIfName));

     // call a data model indpendent function to get the data for display
     cgiGetIntfGroupInfo(brIdx,
                         groupName, sizeof(groupName),
                         intfList, sizeof(intfList), &numOfIntf,
                         wanIntfList, sizeof(wanIntfList), &numOfWanIntf, wanIfName,
                         vendorIdList, sizeof(vendorIdList), &numOfVendorIds);

     rowspan = (numOfVendorIds > numOfIntf) ? numOfVendorIds : numOfIntf;
     rowspan = (numOfWanIntf > rowspan) ? numOfWanIntf : rowspan;
     cmsLog_debug("intfList=%s", intfList);
     cmsLog_debug("wanIntfList=%s", wanIntfList);
     cmsLog_debug("vendorIdList=%s", vendorIdList);
     cmsLog_debug("rowspan=%d", rowspan);
     cmsLog_debug("groupName=%s", groupName);
     
     //Group Name
     fprintf(fs, "   <tr>\n");
     fprintf(fs, "      <td rowspan=%d >%s</td>\n", rowspan, groupName);

     //Group enable/disable, remove and edit. "default" Group can't be removed and edited. 
     if(strcmp("Default", groupName)) {
        if (strcmp(BEEP_NETWORKING_GROUP_SECONDARY, groupName) &&
            strcmp(BEEP_NETWORKING_GROUP_WANONLY, groupName))
        {
           // This is not the default group nor BEEP groups, some commented code deleted here (for edit)
           fprintf(fs, "      <td align='center' rowspan=%d><input type='checkbox' name='rml' value='%s'></td>\n",
                    rowspan, groupName);
        }

        /* use blank for multiSubnet */
        if (wanIfName[0] == '\0')
        {
            strncpy(wanIfName, "&nbsp;", sizeof(wanIfName));
        }

        fprintf(fs, "      <td align='center' rowspan=%d>%s</td>\n", rowspan, wanIfName);
     } else {
        fprintf(fs, "      <td align='center' rowspan=%d>&nbsp</td>\n", rowspan);
     }
     
     /* last 3 columns, filter interface (wan/lan) and filter dhcp vendor id */
     if(numOfIntf == 0 && numOfVendorIds == 0)  {
        //No interface and no vendor ids, this is for the default bridge row
        fprintf(fs, "      <td align='center' rowspan=%d >&nbsp</td>\n", rowspan);
        fprintf(fs, "   </tr>\n");
     } else {
        fprintf(fs, "<script language=\"javascript\">\n");
        fprintf(fs, "<!-- hide\n");
        fprintf(fs, "{\n");
        fprintf(fs, "   var i = 0;\n");
        fprintf(fs, "   var brdId = '%s';\n", glbWebVar.boardID);
        fprintf(fs, "   var wanInterfaceInfo = '%s';\n", wanIntfList);
        fprintf(fs, "   var interfaceInfo = '%s';\n", intfList);
        fprintf(fs, "   var vendorIdInfo = '%s';\n", vendorIdList);
        fprintf(fs, "   var wanInterfaces = wanInterfaceInfo.split('|');\n");
        fprintf(fs, "   var interfaces = interfaceInfo.split('|');\n");
        fprintf(fs, "   var vendorIds = vendorIdInfo.split('|');\n");
        fprintf(fs, "   var intfDisp = '';\n");
        fprintf(fs, "   var brdIntf = '';\n");
        fprintf(fs, "   var blankcell = ' ';\n");
        fprintf(fs, "   var numWanRows = wanInterfaces.length;\n");
        fprintf(fs, "   var numrows = interfaces.length;\n");
        
        fprintf(fs, "   if(vendorIds.length > numrows)\n");
        fprintf(fs, "   {\n");
        fprintf(fs, "      numrows = vendorIds.length;\n");
        fprintf(fs, "   }\n");
        fprintf(fs, "   if(numWanRows > numrows)\n");
        fprintf(fs, "   {\n");
        fprintf(fs, "      numrows = numWanRows;\n");
        fprintf(fs, "   }\n");
          
        
        fprintf(fs, "   for (i = 0; i < numrows; i++) {\n");
        //table header
        fprintf(fs, "      if(i > 0)\n");
        fprintf(fs, "      {\n");
        fprintf(fs, "         document.write(\"<tr>\");\n");
        fprintf(fs, "      }\n");

        if(strcmp("Default", groupName) == 0) 
        {
            // column 3: filter interface name (WAN side interface) only for Default group
            fprintf(fs, "      if (i < wanInterfaces.length)\n");
            fprintf(fs, "      {\n");
            fprintf(fs, "         document.write(\"<td align='center'>\"+wanInterfaces[i]+\"</td>\");\n");
            fprintf(fs, "      } else {\n");
            fprintf(fs, "         document.write(\"<td align='center'>\"+blankcell+\"</td>\");\n");
            fprintf(fs, "      }\n");
        }
       
        // column 4: filter interface name (LAN side interface)
        fprintf(fs, "      if (i < interfaces.length)\n");
        fprintf(fs, "      {\n");
        fprintf(fs, "         brdIntf = brdId + '|' + interfaces[i];\n");
        fprintf(fs, "         intfDisp = getUNameByLName(brdIntf);\n");
        fprintf(fs, "         brdIntf = '';\n");
        fprintf(fs, "         document.write(\"<td align='center'>\"+intfDisp+\"</td>\");\n");
        fprintf(fs, "      } else {\n");
        fprintf(fs, "         document.write(\"<td align='center'>\"+blankcell+\"</td>\");\n");
        fprintf(fs, "      }\n");

        // last column (5): dhcp vendor id
        fprintf(fs, "      if (i < vendorIds.length)\n");
        fprintf(fs, "      {\n");
        fprintf(fs, "         document.write(\"<td align='center'>\"+vendorIds[i]+\"</td>\");\n");
        fprintf(fs, "      } else {\n");
        fprintf(fs, "         document.write(\"<td align='center'>\"+blankcell+\"</td>\");\n");
        fprintf(fs, "      }\n");
        
        fprintf(fs, "      document.write(\"</tr>\");\n");
        fprintf(fs, "   }\n");

        fprintf(fs, "}\n");
        fprintf(fs, "// done hiding -->\n");
        fprintf(fs, "</script>\n");
     }
  }// end of loop over the bridges

  fprintf(fs, "</table><br>\n");
  if (glbEntryCt < PMAP_MAX_ENTRY) 
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");

  fprintf(fs, "<input type='button' onClick='savAppClick(this.form.brdSts,this.form.intfSts,this.form.rml)' value='Remove'>\n");

  fprintf(fs, "</form>\n");
  fprintf(fs, "</blockquote>\n");
  fprintf(fs, "</body>\n");
  fprintf(fs, "</html>\n");
  fflush(fs);
}

static void writePortMapScript(FILE *fs, UBOOL8 checked __attribute__((unused)), unsigned int numOfGroup) {
   
    cmsLog_debug("virtport enabled=%d numOfBridges=%d", checked, numOfGroup);
    
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function frmLoad() {\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'portmapadd.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function editClick(name) {\n");
   fprintf(fs, "   var gName = '';\n");
   fprintf(fs, "   gName += name;\n");
   fprintf(fs, "   var loc = 'portmapedit.cgi?groupName=' + name;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   //Save and Apply Click
   fprintf(fs, "function savAppClick(brdSts,intfSts,rml) {\n");
   //process bridge status
   fprintf(fs, "   var brdStsLst = '1';\n");
#ifdef dont_support
   /* we don't want to support enable/disable of bridge status in WebUI */
   if(numOfGroup > 1)
   {
      fprintf(fs, "   if (brdSts.length > 0)\n");
      fprintf(fs, "      for (i = 0; i < brdSts.length; i++) {\n");
      fprintf(fs, "         brdStsLst += '|';\n");
      fprintf(fs, "         if ( brdSts[i].checked == true )\n");
      fprintf(fs, "            brdStsLst += '1';\n");
      fprintf(fs, "         else\n");
      fprintf(fs, "            brdStsLst += '0';\n");
      fprintf(fs, "      }\n");
      fprintf(fs, "   else if(brdSts.checked == true)");
      fprintf(fs, "      brdStsLst += '|1';");
      fprintf(fs, "   else");
      fprintf(fs, "      brdStsLst += '|0';");
   }
#endif
   fprintf(fs, "\n");
   //process interface status
   fprintf(fs, "   var intfStsLst = '';\n");
#ifdef dont_support
   /* we don't want to support enable/disable of bridge status in WebUI */
   fprintf(fs, "   if (intfSts.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < intfSts.length; i++) {\n");
   fprintf(fs, "         if (i > 0)\n");
   fprintf(fs, "            intfStsLst += '|';\n");
   fprintf(fs, "         if ( intfSts[i].checked == true )\n");
   fprintf(fs, "            intfStsLst += '1';\n");
   fprintf(fs, "         else\n");
   fprintf(fs, "            intfStsLst += '0';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if(intfSts.checked == true)");
   fprintf(fs, "      intfStsLst += '1';");
   fprintf(fs, "   else");
   fprintf(fs, "      intfStsLst += '0';");
   fprintf(fs, "\n");
#endif
   //process remove list
   fprintf(fs, "   var rmlLst = '';\n");
   if(numOfGroup > 1)
   {
      fprintf(fs, "   if (rml.length > 0)\n");
      fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
      fprintf(fs, "         if ( rml[i].checked == true )\n");
      fprintf(fs, "            rmlLst += rml[i].value + ', ';\n");
      fprintf(fs, "      }\n");
      fprintf(fs, "   else if ( rml.checked == true )\n");
      fprintf(fs, "      rmlLst = rml.value;\n");
   }
   fprintf(fs, "\n");
   fprintf(fs, "   var loc = 'portmapcfg.cmd?action=savapply&intfSts=' + intfStsLst + '&brdSts=' + brdStsLst + '&rml=' + rmlLst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   
   // Config Enable/Disable
   fprintf(fs, "function pmapCfgClick() {\n");
   fprintf(fs, "with ( document.forms[0] ) {\n");
   fprintf(fs, "   var lName = '';\n");
   fprintf(fs, "   var brdId = '%s';\n", glbWebVar.boardID);
   fprintf(fs, "   var loc = 'portmapcfg.cmd?action=config';\n\n");
   fprintf(fs, "   lName = getLNameByUName(brdId + '|' + ifName);\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


/* Edit is not supported now. Need to deleate that later */
#ifdef dont_support

static char pmapCheckIfcString(char *list, char *ifcname, char **loc) {
    char r1, r2;
    char *l;
    char *end = list + strlen(list)-1;
    l = strstr(list, ifcname);

    if (!strlen(ifcname))
        return 0;

    for (;;) {
        r1 =0; r2=0;
        if (l != NULL) {
            if (l == list) {
                r1 = 1;
            }
            else { // need to see | if not the first one
                if (*(l-1) == '|') {
                    r1 = 1;
                }
            }

            if (*(l + strlen(ifcname)) == 0 ||
                *(l + strlen(ifcname)) == ':' ||
                *(l + strlen(ifcname)) == '|') { //need to see :if not last one
                r2 = 1;
            }

            if (r1 && r2) {
                if (loc != NULL) {
                    *loc = l;
                }
                return 1;
            }
            else {
                if (l < end) {
                    l = strstr(l+1, ifcname);
                }
            }
        }
        else {
            return 0;
        }
    }
}



void cgiPortMapEdit(char *query, FILE *fs) {
   CmsRet ret = CMSRET_SUCCESS;
   char grpName[BUFLEN_32];
   char grpedIntf[BUFLEN_1024];
   char cmd[WEB_BUF_SIZE_MAX];

   grpName[0]     = '\0';
   grpedIntf[0] = '\0';
   cmd[0]          = '\0';
   cgiGetValueByName(query, "groupName", grpName);
   cgiGetValueByName(query, "choiceBox", grpedIntf);

   // Edit group directly
   ret = portmapEdit(grpName, grpedIntf);
   if (ret == CMSRET_SUCCESS) {
      cgiPortMapView(fs);
      glbSaveConfigNeeded = TRUE;
   } else {
      sprintf(cmd, "Configure port map failed to apply configuration. " \
              "Status: %d.", ret);
      cgiWriteMessagePage(fs, "Port Map Edit Error", cmd,
                          "portmapcfg.cmd?action=viewcfg");
   }

      return;
}

static CmsRet portmapEdit(char *groupName, char *grpedIntf)
{
   CmsRet ret = CMSRET_SUCCESS;
   char   orgGrpedIntf[PMAP_INTF_LIST_SIZE];
   char   ifName[BUFLEN_32];
   char   *pIfNameStart, *pIfNameEnd;
    
   if(groupName == NULL || *groupName == '\0' || grpedIntf == NULL)
      return CMSRET_INVALID_ARGUMENTS;

   ret = dalPMap_disassocAllFilterIntfFromBridge(groupName);
   if (ret != CMSRET_SUCCESS) {
      cmsLog_error("could not disassociate interfaces from bridge status, ret=%d", ret);
      return ret;
   }
   // add new interface to group
   pIfNameStart = grpedIntf;
   while (pIfNameStart <= (grpedIntf + strlen(grpedIntf))) 
   {
      if ((pIfNameEnd = strstr(pIfNameStart, "|")) != NULL) 
      {
         memset(ifName, '\0', BUFLEN_16);
         memcpy(ifName, pIfNameStart, (pIfNameEnd - pIfNameStart));
         if ((*ifName != '\0') && !pmapCheckIfcString(orgGrpedIntf, ifName, NULL))
         {            
            dalPMap_assocFilterIntfToBridge(ifName, groupName);
         }
      } 
      else 
      {
         break;
      }
      pIfNameStart = pIfNameEnd + 1;
   }

   return ret;
}

#endif /* dont_support */
