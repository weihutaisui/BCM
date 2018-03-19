/***********************************************************************
 *
 *  Copyright (c) 2005-2007  Broadcom Corporation
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

#include "cms.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_sec.h"
#include "cgi_main.h"
#include "secapi.h"
#include "syscall.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_qdm.h"


#ifdef DMP_X_BROADCOM_COM_SECURITY_1
static void getAllIfNamesContainingFilterName(const char *filtername, char *ifNames);
UBOOL8 matchFilterName(const char *filterName);

#define MAX_FILTER_NAMES  40
static char filternames[MAX_FILTER_NAMES][BUFLEN_128];
#endif




static void cgiScDisplayNoNat(FILE *fs)
{
   fprintf(fs, "<b>NAT is not enabled. A routed WAN service with NAT enabled is needed to configure this service.</b><br><br>\n");
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}

void cgiScVrtSrv(char *query, FILE *fs) 
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);

   if ( strcmp(action, "add") == 0 )
   {
      cgiScVrtSrvAdd(query, fs);
   }
   else if ( strcmp(action, "remove") == 0 )
   {
      cgiScVrtSrvRemove(query, fs);
   }
   else if ( strcmp(action, "save") == 0 )
   {
      cgiScVrtSrvSave(fs);
   }
   else
   {
      cgiScVrtSrvView(fs);
   }
}


void cgiScVrtSrvAdd(char *query, FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;
   char srvName[BUFLEN_64], srvAddr[BUFLEN_16], dstWanIf[BUFLEN_18];
   char cmdPR[BUFLEN_264];
   char cmdES[BUFLEN_264];
   char cmdEE[BUFLEN_264];
   char cmdIS[BUFLEN_264];
   char cmdIE[BUFLEN_264];
   char protocol[BUFLEN_4];;
   UINT16 EPS, EPE, IPS, IPE;
   char *cpPR = cmdPR, *cpES = cmdES, *cpEE = cmdEE, *cpIS = cmdIS, *cpIE = cmdIE;
   char *prCur = NULL, *esCur = NULL, *eeCur = NULL, *isCur = NULL, *ieCur = NULL;

   srvName[0] = cmdPR[0] = cmdES[0] = cmdEE[0] = cmdIS[0] = cmdIE[0] = '\0';
   EPS = EPE = IPS = IPE = 0;

   cgiGetValueByName(query, "dstWanIf", dstWanIf);   	
   cgiGetValueByName(query, "srvName", srvName);   
   cgiGetValueByName(query, "srvAddr", srvAddr);
   cgiGetValueByName(query, "eStart", cmdES);
   cgiGetValueByName(query, "eEnd", cmdEE);
   cgiGetValueByName(query, "proto", cmdPR);
   cgiGetValueByName(query, "iStart", cmdIS);
   cgiGetValueByName(query, "iEnd", cmdIE);

   /* all entries have to be paired. Only check on one. */
   while ((prCur = strchr(cpPR, ',')) != NULL) 
   {
      /* server IP address validation */
      if (cmsUtl_isValidIpAddress(AF_INET, srvAddr) == FALSE || !strcmp(srvAddr, "0.0.0.0"))
      {
         cmsLog_error("Invalid server IP address");
         sprintf(cmdPR, "Configure virtual server error because of invalid server IP = %s", srvAddr);
         cgiWriteMessagePage(fs, "Virtual Server Add Error", cmdPR, "scvrtsrv.cmd?action=view");
         return;		
      }
	  
      *prCur = '\0';

      /* protocol */
      strcpy(protocol, cpPR); 
      cpPR = ++prCur;

      /* external port start */
      esCur = strchr(cpES, ',');
      *esCur = '\0';
      if (cmsUtl_isValidPortNumber(cpES) == FALSE)
      {
         cmsLog_error("Invalid external port start");
         sprintf(cmdPR, "Configure virtual server error because of invalid external port start = %s", cpES);
         cgiWriteMessagePage(fs, "Virtual Server Add Error", cmdPR, "scvrtsrv.cmd?action=view");
         return;		
      }
      EPS= atoi(cpES);
      cpES = ++esCur;

      /* external port end */
      eeCur = strchr(cpEE, ',');
      *eeCur = '\0';
      if (cmsUtl_isValidPortNumber(cpEE) == FALSE)
      {
         cmsLog_error("Invalid external port end");
         sprintf(cmdPR, "Configure virtual server error because of invalid external port end = %s", cpEE);
         cgiWriteMessagePage(fs, "Virtual Server Add Error", cmdPR, "scvrtsrv.cmd?action=view");
         return;		
      }
      EPE= atoi(cpEE);
      cpEE = ++eeCur;

      /* internal port start */
      isCur = strchr(cpIS, ',');
      *isCur = '\0';	  
      if (cmsUtl_isValidPortNumber(cpIS) == FALSE)
      {
         cmsLog_error("Invalid internal port start");
         sprintf(cmdPR, "Configure virtual server error because of invalid internal port start = %s", cpIS);
         cgiWriteMessagePage(fs, "Virtual Server Add Error", cmdPR, "scvrtsrv.cmd?action=view");
         return;		
      }
      IPS= atoi(cpIS);

      cpIS = ++isCur;

      /* internal port end */
      ieCur = strchr(cpIE, ',');
      *ieCur = '\0';
      if (cmsUtl_isValidPortNumber(cpIE) == FALSE)
      {
         cmsLog_error("Invalid internal port end");
         sprintf(cmdPR, "Configure virtual server error because of invalid internal port end = %s", cpIE);
         cgiWriteMessagePage(fs, "Virtual Server Add Error", cmdPR, "scvrtsrv.cmd?action=view");
         return;		
      }
      IPE= atoi(cpIE);

      cpIE = ++ieCur;

      cmsLog_debug("dstWanIf=%s, srvName=%s, srvAddr=%s, proto=%s, eStart:eEnd=%u:%u, iStart:iEnd=%u:%u", dstWanIf, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE);	  

      if ( (ret = dalVirtualServer_addEntry(dstWanIf, srvName, srvAddr, protocol, EPS, EPE, IPS, IPE)) != CMSRET_SUCCESS ) 
      {
         sprintf(cmdPR, "Add virtual server named %s failed. " "Status: %d.", srvName, ret);
         cgiWriteMessagePage(fs, "Virtual Server Add Error", cmdPR, "scvrtsrv.cmd?action=view");
         return;
      }

   }

   glbSaveConfigNeeded = TRUE;
   cgiScVrtSrvView(fs);

}


void cgiScVrtSrvRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char *cp1 = NULL, *cp2 = NULL, *cp3 = NULL, *cp4 = NULL, *cp5 = NULL;
   char lst[1620], cmd[BUFLEN_264];   //lst[1620] is a temporary patch to support deletion of 32 entries at the same time
   char srvAddr[BUFLEN_16];
   char protocol[BUFLEN_16];
   UINT16 EPS, EPE, IPS, IPE;
   CmsRet ret = CMSRET_SUCCESS;   

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ",", &pLast);

   while ( pToken != NULL ) 
   {
      cp1 = strstr(pToken, "|");
      if ( cp1 == NULL ) continue;
      cp2 = strstr(cp1 + 1, "|");
      if ( cp2 == NULL ) continue;
      cp3 = strstr(cp2 + 1, "|");
      if ( cp3 == NULL ) continue;
      cp4 = strstr(cp3 + 1, "|");
      if ( cp4 == NULL ) continue;
      cp5 = strstr(cp4 + 1, "|");
      if ( cp5 == NULL ) continue;

      *cp1 = *cp2 = *cp3 = *cp4 = *cp5 = '\0';
      strcpy(srvAddr, pToken);
      EPS = atoi(cp1 + 1);
      EPE = atoi(cp2 + 1);
      strcpy(protocol, cp3 + 1);
      IPS = atoi(cp4 + 1);
      IPE = atoi(cp5 + 1);

      if ((ret = dalVirtualServer_deleteEntry(srvAddr, protocol, EPS, EPE, IPS, IPE)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalVirtualServer_deleteEntry failed for %s/%s/%u/%u/%u/%u (ret=%d)", srvAddr, protocol, EPS, EPE, IPS, IPE, ret);
         break;		 
      }
	  
      pToken = strtok_r(NULL, ",", &pLast);
   }

   if ( ret == CMSRET_SUCCESS ) 
   {
      cgiScVrtSrvView(fs);
      glbSaveConfigNeeded = TRUE;
   }
   else 
   {
      sprintf(cmd, "Cannot remove virtual server entry.<br>" "Status: %d.", ret);
      cgiWriteMessagePage(fs, "Virtual Server Remove Error", cmd, "scvrtsrv.cmd?action=view");
   }




#if 0//~ tr69c?
   char *pToken = NULL, *pLast = NULL;
   char *cp1 = NULL, *cp2 = NULL, *cp3 = NULL, *cp4 = NULL, *cp5 = NULL, *cp6= NULL, *cp7=NULL;
   char lst[WEB_BUF_SIZE_MAX], cmd[WEB_MD_BUF_SIZE_MAX];
   BcmCfm_Status status = BcmCfm_Ok;
   UINT32 index = 0;
   BcmCfm_SecVirtualServerCfg_t entry;

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);
   while ( pToken != NULL ) {
      cp1 = strstr(pToken, "|");
      if ( cp1 == NULL ) continue;
      cp2 = strstr(cp1 + 1, "|");
      if ( cp2 == NULL ) continue;
      cp3 = strstr(cp2 + 1, "|");
      if ( cp3 == NULL ) continue;
      cp4 = strstr(cp3 + 1, "|");
      if ( cp4 == NULL ) continue;
      cp5 = strstr(cp4 + 1, "|");
      if ( cp5 == NULL ) continue;
      cp6 = strstr(cp5 + 1, "|");
      if ( cp6 == NULL ) continue;
      cp7 = strstr(cp6 + 1, "|");
      if ( cp7 == NULL ) continue;

      *cp1 = *cp2 = *cp3 = *cp4 = *cp5 = *cp6 = *cp7 = '\0';
      entry.ipAddress = strtoul(pToken, (char **)NULL, 10);
      entry.externalPortStart = atoi(cp1 + 1);
      entry.externalPortEnd = atoi(cp2 + 1);
      entry.protocol.tcp = atoi(cp3 + 1);
      entry.protocol.udp = atoi(cp4 + 1);
      entry.protocol.icmp = atoi(cp5 + 1);
      entry.internalPortStart = atoi(cp6 + 1);
      entry.internalPortEnd = atoi(cp7 + 1);

      if ( cgiFindVrtSrvIndex(&entry, &index) == BcmCfm_Ok ) {
         if ( BcmCfm_objDelete(BCMCFM_OBJ_SEC_VIRTUAL_SERVER, index) == BcmCfm_Fail ) {
            status = BcmCfm_Fail;
            break;
         }
      } else {
         status = BcmCfm_Fail;
         break;
      }
      pToken = strtok_r(NULL, ", ", &pLast);
   }

   if ( status == BcmCfm_Ok ) {
      cgiScVrtSrvView(fs);
      BcmPsi_flush();
   } else {
      sprintf(cmd, "Cannot remove virtual server entry.<br>" "Status: %d.", status);
      cgiWriteMessagePage(fs, "Virtual Server Remove Error", cmd, "scvrtsrv.cmd?action=view");
   }
#endif //~   
}



void cgiScVrtSrvSave(FILE *fs)
{
   glbSaveConfigNeeded = TRUE;
   cgiScVrtSrvView(fs);
}

#ifdef DMP_BASELINE_1 
void cgiScVrtSrvViewbody_igd(FILE *fs) 
{
	InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
	InstanceIdStack wanpppiidStack = EMPTY_INSTANCE_ID_STACK;
	InstanceIdStack wanipiidStack = EMPTY_INSTANCE_ID_STACK;
	WanPppConnPortmappingObject *pppObj = NULL;
	WanIpConnPortmappingObject *ipObj = NULL;
	WanPppConnObject *wan_ppp_con = NULL;
	WanIpConnObject *wan_ip_con = NULL;
	CmsRet ret = CMSRET_SUCCESS;

	/* write table body */
	glbEntryCt = 0;
	/* Get the objects one after another till we fail. */
	while ( (ret = cmsObj_getNext
		  (MDMOID_WAN_PPP_CONN_PORTMAPPING, &iidStack, (void **) &pppObj)) == CMSRET_SUCCESS) 
	{
	   if ( pppObj->portMappingEnabled == TRUE) 
	   {
		  fprintf(fs, "   <tr>\n");
		  fprintf(fs, " 	 <td>%s</td>\n", pppObj->portMappingDescription);
		  fprintf(fs, " 	 <td>%d</td>\n", pppObj->externalPort);
		  fprintf(fs, " 	 <td>%d</td>\n", pppObj->X_BROADCOM_COM_ExternalPortEnd);
		  if ( !strcmp(pppObj->portMappingProtocol, "TCP") ) 
		  {
			 fprintf(fs, "		<td>TCP</td>\n");
		  }
		  else if ( !strcmp(pppObj->portMappingProtocol, "UDP") )
		  {
			 fprintf(fs, "		<td>UDP</td>\n");
		  }
		  else
		  {
			 fprintf(fs, "		<td>TCP/UDP</td>\n");
		  }
		  fprintf(fs, " 	 <td>%d</td>\n", pppObj->internalPort);
		  fprintf(fs, " 	 <td>%d</td>\n", pppObj->X_BROADCOM_COM_InternalPortEnd);
		  fprintf(fs, " 	 <td>%s</td>\n", pppObj->internalClient);
		  memcpy(&wanpppiidStack, &iidStack, sizeof(iidStack));
		  cmsObj_getAncestor(MDMOID_WAN_PPP_CONN, MDMOID_WAN_PPP_CONN_PORTMAPPING, 
											 &wanpppiidStack, (void **) &wan_ppp_con);
		  fprintf(fs, " 	 <td>%s</td>\n", wan_ppp_con->X_BROADCOM_COM_IfName);
		  cmsObj_free((void **) &wan_ppp_con);
		  fprintf(fs, " 	 <td align='center'><input type='checkbox' name='rml' value='%s|%u|%u|%s|%u|%u'></td>\n",
			 pppObj->internalClient, pppObj->externalPort, pppObj->X_BROADCOM_COM_ExternalPortEnd, 
			 pppObj->portMappingProtocol, pppObj->internalPort, pppObj->X_BROADCOM_COM_InternalPortEnd);
		  fprintf(fs, "   </tr>\n");
	   }
	   // Free the mem allocated this object by the get API.
	   cmsObj_free((void **) &pppObj);

	   glbEntryCt++;
	}

	while ( (ret = cmsObj_getNext
		  (MDMOID_WAN_IP_CONN_PORTMAPPING, &iidStack, (void **) &ipObj)) == CMSRET_SUCCESS) 
	{
	   if ( ipObj->portMappingEnabled == TRUE) 
	   {
		  fprintf(fs, "   <tr>\n");
		  fprintf(fs, " 	 <td>%s</td>\n", ipObj->portMappingDescription);
		  fprintf(fs, " 	 <td>%d</td>\n", ipObj->externalPort);
		  fprintf(fs, " 	 <td>%d</td>\n", ipObj->X_BROADCOM_COM_ExternalPortEnd);
		  if ( !strcmp(ipObj->portMappingProtocol, "TCP") ) 
		  {
			 fprintf(fs, "		<td>TCP</td>\n");
		  }
		  else if ( !strcmp(ipObj->portMappingProtocol, "UDP") )
		  {
			 fprintf(fs, "		<td>UDP</td>\n");
		  }
		  else
		  {
			 fprintf(fs, "		<td>TCP/UDP</td>\n");
		  }
		  
		  fprintf(fs, " 	 <td>%d</td>\n", ipObj->internalPort);
		  fprintf(fs, " 	 <td>%d</td>\n", ipObj->X_BROADCOM_COM_InternalPortEnd);
		  fprintf(fs, " 	 <td>%s</td>\n", ipObj->internalClient);
		  memcpy(&wanipiidStack, &iidStack, sizeof(iidStack));
		  cmsObj_getAncestor(MDMOID_WAN_IP_CONN, MDMOID_WAN_IP_CONN_PORTMAPPING, 
											 &wanipiidStack, (void **) &wan_ip_con);
		  fprintf(fs, " 	 <td>%s</td>\n", wan_ip_con->X_BROADCOM_COM_IfName);
		  cmsObj_free((void **) &wan_ip_con);
		  fprintf(fs, " 	 <td align='center'><input type='checkbox' name='rml' value='%s|%u|%u|%s|%u|%u'></td>\n",
			 ipObj->internalClient, ipObj->externalPort, ipObj->X_BROADCOM_COM_ExternalPortEnd, 
			 ipObj->portMappingProtocol, ipObj->internalPort, ipObj->X_BROADCOM_COM_InternalPortEnd);
		  fprintf(fs, "   </tr>\n");
	   }
	   // Free the mem allocated this object by the get API.
	   cmsObj_free((void **) &ipObj);

	   glbEntryCt++;
	}

	fprintf(fs, "</table><br>\n");

}
#endif

#ifdef DMP_DEVICE2_BASELINE_1
void cgiScVrtSrvViewbody_dev2(FILE *fs) 
{
	InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
	Dev2NatPortMappingObject *portmappingObj=NULL;	
	CmsRet ret = CMSRET_SUCCESS;
	char intfname[CMS_IFNAME_LENGTH]="";

	/* write table body */
	glbEntryCt = 0;
	/* Get the objects one after another till we fail. */
	while ( (ret = cmsObj_getNext
		  (MDMOID_DEV2_NAT_PORT_MAPPING, &iidStack, (void **) &portmappingObj)) == CMSRET_SUCCESS) 
	{
	   if ( portmappingObj->enable == TRUE) 
	   {
		  fprintf(fs, "   <tr>\n");
		  fprintf(fs, " 	 <td>%s</td>\n", portmappingObj->description);
		  fprintf(fs, " 	 <td>%d</td>\n", portmappingObj->externalPort);
		  fprintf(fs, " 	 <td>%d</td>\n", portmappingObj->externalPortEndRange);
		  if ( !strcmp(portmappingObj->protocol, "TCP") ) 
		  {
			 fprintf(fs, "		<td>TCP</td>\n");
		  }
		  else if ( !strcmp(portmappingObj->protocol, "UDP") )
		  {
			 fprintf(fs, "		<td>UDP</td>\n");
		  }
		  else
		  {
			 fprintf(fs, "		<td>TCP/UDP</td>\n");
		  }
		  fprintf(fs, " 	 <td>%d</td>\n", portmappingObj->internalPort);
		  fprintf(fs, " 	 <td>%d</td>\n", portmappingObj->X_BROADCOM_COM_InternalPortEndRange);
		  fprintf(fs, " 	 <td>%s</td>\n", portmappingObj->internalClient);
		  
          if (IS_EMPTY_STRING(portmappingObj->interface))
          {
             strncpy(intfname, "All Interfaces", sizeof(intfname));
          }
          else
          {
		     qdmIntf_fullPathToIntfnameLocked(portmappingObj->interface,intfname);
          }
		  fprintf(fs, "	   <td>%s</td>\n", intfname);

		  fprintf(fs, " 	 <td align='center'><input type='checkbox' name='rml' value='%s|%u|%u|%s|%u|%u'></td>\n",
			 portmappingObj->internalClient, portmappingObj->externalPort, portmappingObj->externalPortEndRange, 
			 portmappingObj->protocol, portmappingObj->internalPort, portmappingObj->X_BROADCOM_COM_InternalPortEndRange);
		  fprintf(fs, "   </tr>\n");
	   }
	   // Free the mem allocated this object by the get API.
	   cmsObj_free((void **) &portmappingObj);

	   glbEntryCt++;
	}

	fprintf(fs, "</table><br>\n");

}
#endif

void cgiScVrtSrvViewbody(FILE *fs) 
{
#if defined(SUPPORT_DM_LEGACY98)
	
      cgiScVrtSrvViewbody_igd(fs);

	
#elif defined(SUPPORT_DM_HYBRID)
	
	   cgiScVrtSrvViewbody_igd(fs);
	
#elif defined(SUPPORT_DM_PURE181)
	
       cgiScVrtSrvViewbody_dev2(fs);
	
#elif defined(SUPPORT_DM_DETECT)
	   if (cmsMdm_isDataModelDevice2())
	   {
              cgiScVrtSrvViewbody_dev2(fs);
	   }
	   else
	   {
              cgiScVrtSrvViewbody_igd(fs);
	   }
	
#endif
}

void cgiScVrtSrvView(FILE *fs) 
{
   char natStr[BUFLEN_16]={0};

   writeSecurityHeader(fs);
   writeScVrtSrvScript(fs);
   writeSecurityBody(fs);

   /* if NAT is not enabled, just let user know this service is not
   * available and return.
   */
   cmsDal_getEnblNatForWeb(natStr);
   if (cmsUtl_strcmp(natStr, "1"))
   {
      cgiScDisplayNoNat(fs);
      return;
   }

   fprintf(fs, "<b>NAT -- Virtual Servers Setup</b><br><br>\n");
   fprintf(fs, "Virtual Server allows you to direct incoming traffic from WAN side \n");
   fprintf(fs, "(identified by Protocol and External port) to the Internal server \n");
   fprintf(fs, "with private IP address on the LAN side. The Internal port is required \n");
   fprintf(fs, "only if the external port needs to be converted to a different \n");
   fprintf(fs, "port number used by the server on the LAN side. A maximum 32 entries can be \
                configured.<br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'><br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Server Name</td>\n");
   fprintf(fs, "      <td class='hd'>External Port Start</td>\n");
   fprintf(fs, "      <td class='hd'>External Port End</td>\n");
   fprintf(fs, "      <td class='hd'>Protocol</td>\n");
   fprintf(fs, "      <td class='hd'>Internal Port Start</td>\n");
   fprintf(fs, "      <td class='hd'>Internal Port End</td>\n");
   fprintf(fs, "      <td class='hd'>Server IP Address</td>\n");
   fprintf(fs, "      <td class='hd'>WAN Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   /* write table body */
   cgiScVrtSrvViewbody(fs);

   /* only display the following button  */
   if (glbEntryCt >= SEC_ADD_REMOVE_ROW) 
   {      
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
      fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   }
   glbEntryCt = SEC_PTR_TRG_MAX - glbEntryCt;
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}

void writeScVrtSrvScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'scvrtsrv.html';\n\n");
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
   fprintf(fs, "   var loc = 'scvrtsrv.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}



void cgiScPrtTrg(char *query, FILE *fs) {
   char action[WEB_MD_BUF_SIZE_MAX];

   cgiGetValueByName(query, "action", action);

   if ( strcmp(action, "add") == 0 )
      cgiScPrtTrgAdd(query, fs);
   else if ( strcmp(action, "remove") == 0 )
      cgiScPrtTrgRemove(query, fs);
   else if ( strcmp(action, "save") == 0 )
      cgiScPrtTrgSave(fs);
   else
      cgiScPrtTrgView(fs);
}

void cgiScPrtTrgAdd(char *query, FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;
   char appName[BUFLEN_64];
   char dstWanIf[BUFLEN_18];
   char cmdTP[BUFLEN_264];
   char cmdOP[BUFLEN_264];
   char cmdTS[BUFLEN_264];
   char cmdTE[BUFLEN_264];
   char cmdOS[BUFLEN_264];
   char cmdOE[BUFLEN_264];
   char TP[BUFLEN_18];
   char OP[BUFLEN_18];
   UINT16 TS, TE, OS, OE;
   char *cpTP = cmdTP, *cpOP = cmdOP, *cpTS = cmdTS, *cpTE = cmdTE, *cpOS = cmdOS, *cpOE = cmdOE;
   char *curTP = NULL, *curOP = NULL, *curTS = NULL, *curTE = NULL, *curOS = NULL, *curOE = NULL;
   char buf[128];

   appName[0] = dstWanIf[0] = cmdTP[0] = cmdOP[0] = cmdTS[0] = cmdTE[0] = cmdOS[0] = cmdOE[0] = '\0';
   TP[0] = OP[0] = '\0';
   TS = TE = OS = OE = 0;

   cgiGetValueByName(query, "dstWanIf", dstWanIf);   	
   cgiGetValueByName(query, "appName", appName);   
   cgiGetValueByName(query, "tProto", cmdTP);
   cgiGetValueByName(query, "oProto", cmdOP);
   cgiGetValueByName(query, "tStart", cmdTS);
   cgiGetValueByName(query, "tEnd", cmdTE);
   cgiGetValueByName(query, "oStart", cmdOS);
   cgiGetValueByName(query, "oEnd", cmdOE);

   /* all entries have to be paired. Only check on one. */
   while ((curTP = strchr(cpTP, ',')) != NULL) 
   {
      /* trigger protocol */
      *curTP = '\0';
      strcpy(TP, cpTP); 
      cpTP = ++curTP;

      /* open protocol */
      curOP = strchr(cpOP, ',');
      *curOP = '\0';
      strcpy(OP, cpOP); 
      cpOP = ++curOP;

      /* trigger port start */
      curTS = strchr(cpTS, ',');
      *curTS = '\0';
      if (cmsUtl_isValidPortNumber(cpTS) == FALSE)
      {
         cmsLog_error("Invalid trigger port start");
         sprintf(buf, "Configure port triggering error because of invalid trigger port start = %s", cpTS);
         cgiWriteMessagePage(fs, "Port Triggering Add Error", buf, "scprttrg.cmd?action=view");
         return;		
      }
      TS= atoi(cpTS);
      cpTS = ++curTS;

      /* trigger port end */
      curTE = strchr(cpTE, ',');
      *curTE = '\0';
      if (cmsUtl_isValidPortNumber(cpTE) == FALSE)
      {
         cmsLog_error("Invalid trigger port end");
         sprintf(buf, "Configure port triggering error because of invalid trigger port end = %s", cpTE);
         cgiWriteMessagePage(fs, "Port Triggering Add Error", buf, "scprttrg.cmd?action=view");
         return;		
      }
      TE= atoi(cpTE);
      cpTE = ++curTE;

      /* open port start */
      curOS = strchr(cpOS, ',');
      *curOS = '\0';	  
      if (cmsUtl_isValidPortNumber(cpOS) == FALSE)
      {
         cmsLog_error("Invalid open port start");
         sprintf(buf, "Configure port triggering error because of invalid open port start = %s", cpOS);
         cgiWriteMessagePage(fs, "Port Triggering Add Error", buf, "scprttrg.cmd?action=view");
         return;		
      }
      OS= atoi(cpOS);
      cpOS = ++curOS;

      /* open port end */
      curOE = strchr(cpOE, ',');
      *curOE = '\0';
      if (cmsUtl_isValidPortNumber(cpOE) == FALSE)
      {
         cmsLog_error("Invalid open port end");
         sprintf(buf, "Configure port triggering error because of invalid open port end = %s", cpOE);
         cgiWriteMessagePage(fs, "Port Triggering Add Error", buf, "scprttrg.cmd?action=view");
         return;		
      }
      OE= atoi(cpOE);
      cpOE = ++curOE;

      cmsLog_debug("dstWanIf=%s, appName=%s, tProto=%s, oProto=%s, tStart:tEnd=%hu:%hu, oStart:oEnd=%hu:%hu",
         dstWanIf, appName, TP, OP, TS, TE, OS, OE);	  

      if ( (ret = dalPortTriggering_addEntry(dstWanIf, appName, TP, OP, TS, TE, OS, OE)) != CMSRET_SUCCESS ) 
      {
         sprintf(buf, "Add port triggering named %s failed. " "Status: %d.", appName, ret);
         cgiWriteMessagePage(fs, "Port Triggering Add Error", buf, "scprttrg.cmd?action=view");
         return;
      }

   }

   glbSaveConfigNeeded = TRUE;
   cgiScPrtTrgView(fs);
}


void cgiScPrtTrgRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char *cp1 = NULL, *cp2 = NULL, *cp3 = NULL, *cp4 = NULL, *cp5 = NULL;
   char lst[BUFLEN_1024]={0};
   char dstWanIf[BUFLEN_18];
   char tProto[BUFLEN_18];
   UINT16 TS, TE, OS, OE;
   CmsRet ret = CMSRET_SUCCESS;


   cgiGetValueByName(query, "rmLst", lst);
   cmsLog_debug("Entered: lst==>%s<==", lst);

   pToken = strtok_r(lst, ",", &pLast);
   while ( pToken != NULL ) 
   {
      if ((cp1 = strstr(pToken, "|")) == NULL)
      {
         cmsLog_error("Could not find first separator");
         ret = CMSRET_INVALID_ARGUMENTS;
         break;
      }

      if ((cp2 = strstr(cp1 + 1, "|")) == NULL)
      {
         cmsLog_error("Could not find second separator");
         ret = CMSRET_INVALID_ARGUMENTS;
         break;
      }

      if ((cp3 = strstr(cp2 + 1, "|")) == NULL)
      {
         cmsLog_error("Could not find third separator");
         ret = CMSRET_INVALID_ARGUMENTS;
         break;
      }

      if ((cp4 = strstr(cp3 + 1, "|")) == NULL)
      {
         cmsLog_error("Could not find third separator");
         ret = CMSRET_INVALID_ARGUMENTS;
         break;
      }

      if ((cp5 = strstr(cp4 + 1, "|")) == NULL)
      {
         cmsLog_error("Could not find third separator");
         ret = CMSRET_INVALID_ARGUMENTS;
         break;
      }

      *cp1 = *cp2 = *cp3 = *cp4 = *cp5 = '\0';
      strcpy(dstWanIf, pToken);
      strcpy(tProto, cp1+1);
      TS = atoi(cp2 + 1);
      TE = atoi(cp3 + 1);
      OS = atoi(cp4 + 1);
      OE = atoi(cp5 + 1);
      if ((ret = dalPortTriggering_deleteEntry(dstWanIf, tProto, TS, TE, OS, OE)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalPortTriggering_deleteEntry failed for %s/%s/%hu/%hu/%hu/%hu (ret=%d)", dstWanIf, tProto, TS, TE, OS, OE, ret);
         break;		 
      }
	  
      pToken = strtok_r(NULL, ",", &pLast);
   }

   if ( ret == CMSRET_SUCCESS ) 
   {
      cgiScPrtTrgView(fs);
      glbSaveConfigNeeded = TRUE;
   }
   else 
   {
      char buf[BUFLEN_264];
      sprintf(buf, "Cannot remove port triggering entry.<br>" "Status: %d.", ret);
      cgiWriteMessagePage(fs, "Port Triggering Remove Error", buf, "scprttrg.cmd?action=view");
   }
}


void cgiScPrtTrgSave(FILE *fs)
{
   glbSaveConfigNeeded = TRUE;
   cgiScPrtTrgView(fs);
}

void cgiScPrtTrgViewBody_igd(FILE *fs) {
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack wanpppiidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack wanipiidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnPortTriggeringObject *pppObj = NULL;
   WanIpConnPortTriggeringObject *ipObj = NULL;
   WanPppConnObject *wan_ppp_con = NULL;
   WanIpConnObject *wan_ip_con = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   glbEntryCt = 0;
   /* Get the objects one after another till we fail. */
   while ( (ret = cmsObj_getNext
         (MDMOID_WAN_PPP_CONN_PORT_TRIGGERING, &iidStack, (void **) &pppObj)) == CMSRET_SUCCESS) 
   {
      if ( pppObj->enable == TRUE) 
      {
         memcpy(&wanpppiidStack, &iidStack, sizeof(iidStack));
         cmsObj_getAncestor(MDMOID_WAN_PPP_CONN, MDMOID_WAN_PPP_CONN_PORTMAPPING, 
                                            &wanpppiidStack, (void **) &wan_ppp_con);
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", pppObj->name);
         if ( !strcmp(pppObj->triggerProtocol, "TCP") ) 
         {
            fprintf(fs, "      <td>TCP</td>\n");
         }
         else if ( !strcmp(pppObj->triggerProtocol, "UDP") )
         {
            fprintf(fs, "      <td>UDP</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>TCP/UDP</td>\n");
         }
         fprintf(fs, "      <td>%d</td>\n", pppObj->triggerPortStart);
         fprintf(fs, "      <td>%d</td>\n", pppObj->triggerPortEnd);
         if ( !strcmp(pppObj->openProtocol, "TCP") ) 
         {
            fprintf(fs, "      <td>TCP</td>\n");
         }
         else if ( !strcmp(pppObj->openProtocol, "UDP") )
         {
            fprintf(fs, "      <td>UDP</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>TCP/UDP</td>\n");
         }
         fprintf(fs, "      <td>%d</td>\n", pppObj->openPortStart);
         fprintf(fs, "      <td>%d</td>\n", pppObj->openPortEnd);
         fprintf(fs, "      <td>%s</td>\n", wan_ppp_con->X_BROADCOM_COM_IfName);
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s|%s|%u|%u|%u|%u'></td>\n",
            wan_ppp_con->X_BROADCOM_COM_IfName, pppObj->triggerProtocol,
	    pppObj->triggerPortStart, pppObj->triggerPortEnd,
	    pppObj->openPortStart, pppObj->openPortEnd);
         fprintf(fs, "   </tr>\n");
         cmsObj_free((void **) &wan_ppp_con);
      }
      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &pppObj);

      glbEntryCt++;
   }

   /* Get the objects one after another till we fail. */
   while ( (ret = cmsObj_getNext
         (MDMOID_WAN_IP_CONN_PORT_TRIGGERING, &iidStack, (void **) &ipObj)) == CMSRET_SUCCESS) 
   {
      if ( ipObj->enable == TRUE) 
      {
         memcpy(&wanipiidStack, &iidStack, sizeof(iidStack));
         cmsObj_getAncestor(MDMOID_WAN_IP_CONN, MDMOID_WAN_IP_CONN_PORTMAPPING, 
                                            &wanipiidStack, (void **) &wan_ip_con);
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", ipObj->name);
         if ( !strcmp(ipObj->triggerProtocol, "TCP") ) 
         {
            fprintf(fs, "      <td>TCP</td>\n");
         }
         else if ( !strcmp(ipObj->triggerProtocol, "UDP") )
         {
            fprintf(fs, "      <td>UDP</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>TCP/UDP</td>\n");
         }
         fprintf(fs, "      <td>%d</td>\n", ipObj->triggerPortStart);
         fprintf(fs, "      <td>%d</td>\n", ipObj->triggerPortEnd);
         if ( !strcmp(ipObj->openProtocol, "TCP") ) 
         {
            fprintf(fs, "      <td>TCP</td>\n");
         }
         else if ( !strcmp(ipObj->openProtocol, "UDP") )
         {
            fprintf(fs, "      <td>UDP</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>TCP/UDP</td>\n");
         }
         fprintf(fs, "      <td>%d</td>\n", ipObj->openPortStart);
         fprintf(fs, "      <td>%d</td>\n", ipObj->openPortEnd);
         fprintf(fs, "      <td>%s</td>\n", wan_ip_con->X_BROADCOM_COM_IfName);
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s|%s|%u|%u|%u|%u'></td>\n",
            wan_ip_con->X_BROADCOM_COM_IfName, ipObj->triggerProtocol,
	    ipObj->triggerPortStart, ipObj->triggerPortEnd,
            ipObj->openPortStart, ipObj->openPortEnd);
         fprintf(fs, "   </tr>\n");
         cmsObj_free((void **) &wan_ip_con);
      }
      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &ipObj);

      glbEntryCt++;
   }


   fprintf(fs, "</table><br>\n");
   if (glbEntryCt >= SEC_ADD_REMOVE_ROW) {      // only display the following button 
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
      fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   }
   glbEntryCt = SEC_PTR_TRG_MAX - glbEntryCt;
      
}


void cgiScPrtTrgView(FILE *fs) {
   char natStr[BUFLEN_16]={0};
   
   writeSecurityHeader(fs);
   writeScPrtTrgScript(fs);
   writeSecurityBody(fs);

   /* if NAT is not enabled, just let user know this service is not
   * available and return.
   */
   cmsDal_getEnblNatForWeb(natStr);
   if (cmsUtl_strcmp(natStr, "1"))
   {
      cgiScDisplayNoNat(fs);
      return;
   }
   
   fprintf(fs, "<b>NAT -- Port Triggering Setup</b><br><br>\n");
   fprintf(fs, "Some applications require that specific ports in the Router's firewall be opened for access \
               by the remote parties. Port Trigger dynamically opens up the 'Open Ports' in the firewall when \
               an application on the LAN initiates a TCP/UDP connection to a remote party using the 'Triggering \
               Ports'. The Router allows the remote party from the WAN side to establish new connections back \
               to the application on the LAN side using the 'Open Ports'.  A maximum 32 entries can be \
               configured.<br><br>\n");  
   fprintf(fs, "<center>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'><br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd' rowspan=3>Application Name</td>\n");
   fprintf(fs, "      <td class='hd' colspan=3>Trigger</td>\n");
   fprintf(fs, "      <td class='hd' colspan=3>Open</td>\n");
   fprintf(fs, "      <td class='hd' rowspan=3>WAN Interface</td>\n");
   fprintf(fs, "      <td class='hd' rowspan=3>Remove</td>\n");
   fprintf(fs, "   </tr>\n");
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd' rowspan=2>Protocol</td>\n");
   fprintf(fs, "      <td class='hd' colspan=2>Port Range</td>\n");
   fprintf(fs, "      <td class='hd' rowspan=2>Protocol</td>\n");
   fprintf(fs, "      <td class='hd' colspan=2>Port Range</td>\n");
   fprintf(fs, "   </tr>\n");
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Start</td>\n");
   fprintf(fs, "      <td class='hd'>End</td>\n");
   fprintf(fs, "      <td class='hd'>Start</td>\n");
   fprintf(fs, "      <td class='hd'>End</td>\n");
   fprintf(fs, "   </tr>\n");

   /* write table body */
   cgiScPrtTrgViewBody(fs);

   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}


void writeScPrtTrgScript(FILE *fs) {
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'scprttrg.html';\n\n");
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
   fprintf(fs, "   var loc = 'scprttrg.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}

#ifdef DMP_X_BROADCOM_COM_SECURITY_1
void cgiScOutFlt(char *query, FILE *fs) 
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);

   if ( strcmp(action, "add") == 0 )
   {
      cgiScOutFltAdd(query, fs);
   }
   else if ( strcmp(action, "remove") == 0 )
   {
      cgiScOutFltRemove(query, fs);
   }
   else
   {
      cgiScOutFltView(fs);
   }
}

void cgiScOutFltAdd(char *query, FILE *fs) 
{
   char cmd[BUFLEN_128], ipver[BUFLEN_4], protocol[BUFLEN_4], name[BUFLEN_16], srcAddr[BUFLEN_64], srcMask[BUFLEN_16];
   char srcPort[BUFLEN_16], dstAddr[BUFLEN_64], dstMask[BUFLEN_16], dstPort[BUFLEN_16];
   CmsRet ret = CMSRET_SUCCESS;

   cmd[0] = ipver[0] = protocol[0] = name[0] = srcAddr[0] = srcMask[0] = srcPort[0] = dstAddr[0] = dstMask[0] = dstPort[0] = '\0';

   cgiGetValueByName(query, "fltName", name);
   cgiGetValueByName(query, "ipver", ipver);
   cgiGetValueByName(query, "protocol", protocol);
   cgiGetValueByName(query, "srcAddr", srcAddr);
   cgiGetValueByName(query, "srcPort", srcPort);
   cgiGetValueByName(query, "dstAddr", dstAddr);
   cgiGetValueByName(query, "dstPort", dstPort);

   if (strchr(srcAddr, ':') == NULL)
   {
      /* IPv4 address */
      struct in_addr addr, mask;

      cmsNet_inet_cidrton(srcAddr, &addr, &mask);
      if (addr.s_addr != 0)
      {
         strncpy(srcAddr, inet_ntoa(addr), sizeof(srcAddr));

         if (mask.s_addr != 0)
         {
            strncpy(srcMask, inet_ntoa(mask), sizeof(srcMask));
         }
      }
   }
   else
   {
      /* IPv6 address */
      *srcMask = '\0';
   }

   if (strchr(dstAddr, ':') == NULL)
   {
      /* IPv4 address */
      struct in_addr addr, mask;

      cmsNet_inet_cidrton(dstAddr, &addr, &mask);
      if (addr.s_addr != 0)
      {
         strncpy(dstAddr, inet_ntoa(addr), sizeof(dstAddr));

         if (mask.s_addr != 0)
         {
            strncpy(dstMask, inet_ntoa(mask), sizeof(dstMask));
         }
      }
   }
   else
   {
      /* IPv6 address */
      *dstMask = '\0';
   }

   /* We can enable IP filter out feature to support interface group (br0, br1, ...) by passing interface argument in the future */
   ret = dalSec_addIpFilterOut(name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort, NULL);
   
   if ( ret != CMSRET_SUCCESS ) 
   {
      sprintf(cmd, "Failed to add outgoing IP filter %s;  Check for duplicate filter name and/or filter rules.", name);      
      cgiWriteMessagePage(fs, "Outgoing IP filter Add Error", cmd, "scoutflt.cmd?action=view");
   } 
   else 
   {
      glbSaveConfigNeeded = TRUE;
      cgiScOutFltView(fs);
   }
}

void cgiScOutFltRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024], cmd[BUFLEN_264];
   CmsRet ret = CMSRET_SUCCESS;

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);
   while ( pToken != NULL ) 
   {
      ret = dalSec_deleteIpFilterOut(pToken);
      if ( ret != CMSRET_SUCCESS ) 
         break;

      pToken = strtok_r(NULL, ", ", &pLast);
   }

   if ( ret == CMSRET_SUCCESS ) 
   {
      glbSaveConfigNeeded = TRUE;
      cgiScOutFltView(fs);
   } 
   else 
   {
      sprintf(cmd, "Cannot remove outgoing IP filter named %s.<br>Status: %d.", pToken, ret);
      cgiWriteMessagePage(fs, "Outgoing IP Filter Remove Error", cmd, "scoutflt.cmd?action=view");
   }
}

void cgiScOutFltViewbody_igd(FILE *fs) 
{
	InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
	IpFilterCfgObject *obj = NULL;
	CmsRet ret = CMSRET_SUCCESS;

   while ( (ret = cmsObj_getNext
         (MDMOID_IP_FILTER_CFG, &iidStack, (void **) &obj)) == CMSRET_SUCCESS) 
   {
      if ( obj->enable == TRUE ) 
      {
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", obj->filterName);
         fprintf(fs, "      <td>%s</td>\n", obj->IPVersion);
         fprintf(fs, "      <td>%s</td>\n", obj->protocol);
   
         if ( cmsUtl_strcmp(obj->sourceIPAddress, "\0") != 0 ) 
         {
            if (strchr(obj->sourceIPAddress, ':') == NULL)
            {
               /* IPv4 address */
               if ( cmsUtl_strcmp(obj->sourceNetMask, "\0") != 0 )
               {
                  fprintf(fs, "      <td>%s/%d</td>\n", obj->sourceIPAddress,
                          cmsNet_getLeftMostOneBitsInMask(obj->sourceNetMask));
               }
               else
               {
                  fprintf(fs, "      <td>%s</td>\n", obj->sourceIPAddress);
               } 
            }
            else
            {
               /* IPv6 address */
               fprintf(fs, "      <td>%s</td>\n", obj->sourceIPAddress);
            }
         } 
         else
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }

         if ( obj->sourcePortStart != 0)
         {
            if ( obj->sourcePortEnd != 0)
            {
               fprintf(fs, "      <td>%d:%d</td>\n", obj->sourcePortStart, obj->sourcePortEnd);
            }
            else
            {
               fprintf(fs, "      <td>%d</td>\n", obj->sourcePortStart);
            }
         }
         else
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }

         if ( cmsUtl_strcmp(obj->destinationIPAddress, "\0") != 0 ) 
         {
            if (strchr(obj->destinationIPAddress, ':') == NULL)
            {
               /* IPv4 address */
               if ( cmsUtl_strcmp(obj->destinationNetMask, "\0") != 0 )
               {
                  fprintf(fs, "      <td>%s/%d</td>\n", obj->destinationIPAddress,
                          cmsNet_getLeftMostOneBitsInMask(obj->destinationNetMask));
               }
               else
               {
                  fprintf(fs, "      <td>%s</td>\n", obj->destinationIPAddress);
               } 
            }
            else
            {
               /* IPv6 address */
               fprintf(fs, "      <td>%s</td>\n", obj->destinationIPAddress);
            }
         } 
         else
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
   
         if ( obj->destinationPortStart != 0)
         {
            if ( obj->destinationPortEnd != 0)
            {
               fprintf(fs, "      <td>%d:%d</td>\n", obj->destinationPortStart, obj->destinationPortEnd);
            }
            else
            {
               fprintf(fs, "      <td>%d</td>\n", obj->destinationPortStart);
            }
         }
         else
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
   	  
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", obj->filterName);
         fprintf(fs, "   </tr>\n");
      }
      cmsObj_free((void **)&obj);
   }

   return;
}


void cgiScOutFltView(FILE *fs) 
{
   writeSecurityHeader(fs);
   writeScOutFltScript(fs);
   writeSecurityBody(fs);

   fprintf(fs, "<b>Outgoing IP Filtering Setup</b><br><br>\n");
   fprintf(fs, "By default, all outgoing IP traffic from LAN is allowed, but some IP traffic can be <b><font color='red'>BLOCKED</font></b> by setting up filters.<br><br>\n");
   fprintf(fs, "Choose Add or Remove to configure outgoing IP filters.<br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Filter Name</td>\n");
   fprintf(fs, "      <td class='hd'>IP Version</td>\n");
   fprintf(fs, "      <td class='hd'>Protocol</td>\n");
   fprintf(fs, "      <td class='hd'>SrcIP/ PrefixLength</td>\n");
   fprintf(fs, "      <td class='hd'>SrcPort</td>\n");
   fprintf(fs, "      <td class='hd'>DstIP/ PrefixLength</td>\n");
   fprintf(fs, "      <td class='hd'>DstPort</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   // write table body
   cgiScOutFltViewbody(fs);

   fprintf(fs, "</table><br>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}

void writeScOutFltScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'scoutflt.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (rml.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "         if ( rml[i].checked == true )\n");
   fprintf(fs, "            lst += rml[i].value + ', ';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( rml.checked == true )\n");
   fprintf(fs, "      lst = rml.value;\n");

   fprintf(fs, "   var loc = 'scoutflt.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


void cgiScInFlt(char *query, FILE *fs) 
{
   char action[WEB_MD_BUF_SIZE_MAX];

   cgiGetValueByName(query, "action", action);

   if ( strcmp(action, "add") == 0 )
      cgiScInFltAdd(query, fs);
   else if ( strcmp(action, "remove") == 0 )
      cgiScInFltRemove(query, fs);
   else
      cgiScInFltView(fs);
}


CmsRet addIpFilterInAll_igd(const char *name, const char *ipver, const char *protocol,
                            const char *srcAddr, const char *srcMask, const char *srcPort, 
                            const char *dstAddr, const char *dstMask, const char *dstPort)
{
  InstanceIdStack iidStack;
  WanPppConnObject *wan_ppp_con = NULL;
  WanIpConnObject *wan_ip_con = NULL; 
  LanIpIntfObject *ipIntfObj = NULL;
  CmsRet ret = CMSRET_SUCCESS;

  INIT_INSTANCE_ID_STACK(&iidStack);
  while ((ret == CMSRET_SUCCESS) &&
		 cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &wan_ip_con) == CMSRET_SUCCESS)
  {
	 if ( wan_ip_con->X_BROADCOM_COM_FirewallEnabled == TRUE) 
	 {
		ret = dalSec_addIpFilterIn(name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort, wan_ip_con->X_BROADCOM_COM_IfName);
	 }
	 cmsObj_free((void **) &wan_ip_con);
  }

  INIT_INSTANCE_ID_STACK(&iidStack);
  while ((ret == CMSRET_SUCCESS) &&
		 cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &wan_ppp_con) == CMSRET_SUCCESS)
  {
	 if ( wan_ppp_con->X_BROADCOM_COM_FirewallEnabled == TRUE ) 
	 {
		ret = dalSec_addIpFilterIn(name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort, wan_ppp_con->X_BROADCOM_COM_IfName);
	 }
	 cmsObj_free((void **) &wan_ppp_con);		  
  }

  INIT_INSTANCE_ID_STACK(&iidStack);
  while ((ret == CMSRET_SUCCESS) &&
		 cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **) &ipIntfObj) == CMSRET_SUCCESS)
  {
	 if ( ipIntfObj->X_BROADCOM_COM_FirewallEnabled == TRUE ) 
	 {
	 	ret = dalSec_addIpFilterIn(name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort, ipIntfObj->X_BROADCOM_COM_IfName);
	 }

	 cmsObj_free((void **) &ipIntfObj); 	  
  }

   return ret;  
}


void cgiScInFltAdd(char *query, FILE *fs)
{
   char cmd[BUFLEN_264], ipver[BUFLEN_4], protocol[BUFLEN_4], name[BUFLEN_16], srcAddr[BUFLEN_64], srcMask[BUFLEN_16], ifName[BUFLEN_32];
   char srcPort[BUFLEN_16], dstAddr[BUFLEN_64], dstMask[BUFLEN_16], dstPort[BUFLEN_16], interfaces[BUFLEN_128];
   char *pDest, *pCur;
   CmsRet ret = CMSRET_SUCCESS;

   cmd[0] = ipver[0] = protocol[0] = name[0] = srcAddr[0] = srcMask[0] = srcPort[0] = dstAddr[0] = dstMask[0] = dstPort[0] = interfaces[0] = ifName[0] = '\0';

   cgiGetValueByName(query, "fltName", name);
   cgiGetValueByName(query, "ipver", ipver);
   cgiGetValueByName(query, "protocol", protocol);
   cgiGetValueByName(query, "srcAddr", srcAddr);
   cgiGetValueByName(query, "srcPort", srcPort);
   cgiGetValueByName(query, "dstAddr", dstAddr);
   cgiGetValueByName(query, "dstPort", dstPort);
   cgiGetValueByName(query, "wanIf", interfaces);

   cmsLog_debug("fltName=%s interfaces=%s", name, interfaces);

   if (strchr(srcAddr, ':') == NULL)
   {
      /* IPv4 address */
      struct in_addr addr, mask;

      cmsNet_inet_cidrton(srcAddr, &addr, &mask);
      if (addr.s_addr != 0)
      {
         strncpy(srcAddr, inet_ntoa(addr), sizeof(srcAddr));

         if (mask.s_addr != 0)
         {
            strncpy(srcMask, inet_ntoa(mask), sizeof(srcMask));
         }
      }
   }
   else
   {
      /* IPv6 address */
      *srcMask = '\0';
   }

   if (strchr(dstAddr, ':') == NULL)
   {
      /* IPv4 address */
      struct in_addr addr, mask;

      cmsNet_inet_cidrton(dstAddr, &addr, &mask);
      if (addr.s_addr != 0)
      {
         strncpy(dstAddr, inet_ntoa(addr), sizeof(dstAddr));

         if (mask.s_addr != 0)
         {
            strncpy(dstMask, inet_ntoa(mask), sizeof(dstMask));
         }
      }
   }
   else
   {
      /* IPv6 address */
      *dstMask = '\0';
   }

   if (cmsUtl_strcmp(interfaces, "ALL") == 0)
   {
         ret = addIpFilterInAll(name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort);
   }
   else
   {
      pCur = interfaces;
      while (1) 
      {     
         pDest = strchr(pCur, '|');
         if (pDest != NULL) 
         {
            *pDest = '\0';
            strcpy(ifName, pCur);
            pCur = ++pDest;

            cmsLog_debug("adding ifName=%s", ifName);
            if ((ret = dalSec_addIpFilterIn(name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort, ifName) != CMSRET_SUCCESS))
            {
               break;
            }
         }
         else 
         {
            strcpy(ifName, pCur);
   
            cmsLog_debug("adding last ifName=%s", ifName);
            ret = dalSec_addIpFilterIn(name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort, ifName);
            break;
         }
   
      }
   }

   if (ret == CMSRET_SUCCESS) 
   {
      glbSaveConfigNeeded = TRUE;
      cgiScInFltView(fs);
   }
   else
   {
      sprintf(cmd, "Add incoming IP filter named %s failed. Status: %d.", name, ret);
      cgiWriteMessagePage(fs, "Incoming IP filter Add Error", cmd, "scinflt.cmd?action=view");
   } 

}

void cgiScInFltRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024], cmd[BUFLEN_264];
   CmsRet ret = CMSRET_SUCCESS;

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);
   while ( pToken != NULL ) 
   {
      ret = dalSec_deleteIpFilterIn(pToken);
      if ( ret != CMSRET_SUCCESS ) 
         break;

      pToken = strtok_r(NULL, ", ", &pLast);
   }

   if ( ret == CMSRET_SUCCESS ) 
   {
      glbSaveConfigNeeded = TRUE;
      cgiScInFltView(fs);
   } 
   else 
   {
      sprintf(cmd, "Cannot remove outgoing IP filter named %s.<br>Status: %d.", pToken, ret);
      cgiWriteMessagePage(fs, "Incoming IP Filter Remove Error", cmd, "scinflt.cmd?action=view");
   }
}


/** Print out a single firewall exception entry.
 *
 * Note that even though the first arg is of type WanPppConnFirewallExceptionObject,
 * this single function can be used for all 3 types of firewall exceptions because
 * all the firewall exception objects are exactly the same,
 * 
 * @param exceptionObj (IN) firewallExceptionObject.
 * @param fs           (IN) file stream to print out to.
 */
static void printFirewallExceptionEntry(const WanPppConnFirewallExceptionObject *exceptionObj, FILE *fs)
{
   char ifNames[BUFLEN_256]={0};

   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td>%s</td>\n", exceptionObj->filterName);
   getAllIfNamesContainingFilterName(exceptionObj->filterName, ifNames);
   fprintf(fs, "      <td>%s</td>\n", ifNames); 
   fprintf(fs, "      <td>%s</td>\n", exceptionObj->IPVersion);
   fprintf(fs, "      <td>%s</td>\n", exceptionObj->protocol);
   
   if ( cmsUtl_strcmp(exceptionObj->sourceIPAddress, "\0") != 0 ) 
   {
      if (strchr(exceptionObj->sourceIPAddress, ':') == NULL)
      {
         /* IPv4 address */
         if ( cmsUtl_strcmp(exceptionObj->sourceNetMask, "\0") != 0 )
         {
            fprintf(fs, "      <td>%s/%d</td>\n", exceptionObj->sourceIPAddress,
                    cmsNet_getLeftMostOneBitsInMask(exceptionObj->sourceNetMask));
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", exceptionObj->sourceIPAddress);
         } 
      }
      else
      {
         /* IPv6 address */
         fprintf(fs, "      <td>%s</td>\n", exceptionObj->sourceIPAddress);
      }
   } 
   else
   {
      fprintf(fs, "      <td>&nbsp</td>\n");
   }

   if ( exceptionObj->sourcePortStart != 0)
   {
      if ( exceptionObj->sourcePortEnd != 0)
      {
         fprintf(fs, "      <td>%d:%d</td>\n", exceptionObj->sourcePortStart, exceptionObj->sourcePortEnd);
      }
      else
      {
         fprintf(fs, "      <td>%d</td>\n", exceptionObj->sourcePortStart);
      }
   }
   else
   {
      fprintf(fs, "      <td>&nbsp</td>\n");
   }

   if ( cmsUtl_strcmp(exceptionObj->destinationIPAddress, "\0") != 0 ) 
   {
      if (strchr(exceptionObj->destinationIPAddress, ':') == NULL)
      {
         /* IPv4 address */
         if ( cmsUtl_strcmp(exceptionObj->destinationNetMask, "\0") != 0 )
         {
            fprintf(fs, "      <td>%s/%d</td>\n", exceptionObj->destinationIPAddress,
                    cmsNet_getLeftMostOneBitsInMask(exceptionObj->destinationNetMask));
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", exceptionObj->destinationIPAddress);
         } 
      }
      else
      {
         /* IPv6 address */
         fprintf(fs, "      <td>%s</td>\n", exceptionObj->destinationIPAddress);
      }
   } 
   else
   {
      fprintf(fs, "      <td>&nbsp</td>\n");
   }
   
   if ( exceptionObj->destinationPortStart != 0)
   {
      if ( exceptionObj->destinationPortEnd != 0)
      {
         fprintf(fs, "      <td>%d:%d</td>\n", exceptionObj->destinationPortStart, exceptionObj->destinationPortEnd);
      }
      else
      {
         fprintf(fs, "      <td>%d</td>\n", exceptionObj->destinationPortStart);
      }
   }
   else
   {
      fprintf(fs, "      <td>&nbsp</td>\n");
   }
   	  
   fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", exceptionObj->filterName);
   fprintf(fs, "   </tr>\n");


}


void cgiScInFltViewbody_igd(FILE *fs)
{
	   InstanceIdStack iidStack;
	   WanPppConnFirewallExceptionObject *pppConnFirewall = NULL;
	   WanIpConnFirewallExceptionObject *ipConnFirewall = NULL;
	   LanIpIntfFirewallExceptionObject *lanFirewall = NULL;
	   CmsRet ret = CMSRET_SUCCESS;

	   INIT_INSTANCE_ID_STACK(&iidStack);
	   while ( (ret = cmsObj_getNext
			 (MDMOID_WAN_PPP_CONN_FIREWALL_EXCEPTION, &iidStack, (void **) &pppConnFirewall)) == CMSRET_SUCCESS) 
	   {
		  if (pppConnFirewall->enable && !matchFilterName(pppConnFirewall->filterName))
		  {
			 printFirewallExceptionEntry(pppConnFirewall, fs);
		  }
	
		  cmsObj_free((void **) &pppConnFirewall);
	   }
	
	   INIT_INSTANCE_ID_STACK(&iidStack);
	   while ( (ret = cmsObj_getNext
			 (MDMOID_WAN_IP_CONN_FIREWALL_EXCEPTION, &iidStack, (void **) &ipConnFirewall)) == CMSRET_SUCCESS) 
	   {
		  if (ipConnFirewall->enable && !matchFilterName(ipConnFirewall->filterName))
		  {
			 printFirewallExceptionEntry((WanPppConnFirewallExceptionObject *) ipConnFirewall, fs);
		  }
	
		  cmsObj_free((void **) &ipConnFirewall);
	   }
	
	   INIT_INSTANCE_ID_STACK(&iidStack);
	   while ( (ret = cmsObj_getNext
			 (MDMOID_LAN_IP_INTF_FIREWALL_EXCEPTION, &iidStack, (void **) &lanFirewall)) == CMSRET_SUCCESS) 
	   {
		  if (lanFirewall->enable && !matchFilterName(lanFirewall->filterName))
		  {
			 printFirewallExceptionEntry((WanPppConnFirewallExceptionObject *) lanFirewall, fs);
		  }
	
		  cmsObj_free((void **) &lanFirewall);
	   }
}


void cgiScInFltView(FILE *fs) 
{

   memset(filternames, 0, sizeof(filternames));
   
   writeSecurityHeader(fs);
   writeScInFltScript(fs);
   writeSecurityBody(fs);

   fprintf(fs, "<b>Incoming IP Filtering Setup</b><br><br>\n");
   fprintf(fs, "When the firewall is enabled on a WAN or LAN interface, all incoming IP traffic is BLOCKED. However, some IP traffic can be <b><font color='green'>ACCEPTED</font></b> by setting up filters.<br><br>\n");
   fprintf(fs, "Choose Add or Remove to configure incoming IP filters.<br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Filter Name</td>\n");
   fprintf(fs, "      <td class='hd'>Interfaces</td>\n");   
   fprintf(fs, "      <td class='hd'>IP Version</td>\n");
   fprintf(fs, "      <td class='hd'>Protocol</td>\n");
   fprintf(fs, "      <td class='hd'>SrcIP/ PrefixLength</td>\n");
   fprintf(fs, "      <td class='hd'>SrcPort</td>\n");
   fprintf(fs, "      <td class='hd'>DstIP/ PrefixLength</td>\n");
   fprintf(fs, "      <td class='hd'>DstPort</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   // write table body
   cgiScInFltViewbody(fs);

   fprintf(fs, "</table><br>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}


void writeScInFltScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'scinflt.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (rml.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "         if ( rml[i].checked == true )\n");
   fprintf(fs, "            lst += rml[i].value + ', ';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( rml.checked == true )\n");
   fprintf(fs, "      lst = rml.value;\n");

   fprintf(fs, "   var loc = 'scinflt.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


/** keep track of which filter names we have already displayed.
 *  If the specified filter name has not been displayed, it is
 * added to filterNames.
 *
 * @param filterName (IN) filter name in question.
 *
 * @return TRUE if this is a filter name that we have already displayed.
 *         otherwise, return false and also add the filter name to
 *         the list of filter names we have displayed.
 */
UBOOL8 matchFilterName(const char *filterName)
{
   int index =0;
   UBOOL8 found = FALSE;

   while (index<MAX_FILTER_NAMES)
   {
      if (cmsUtl_strcmp(filterName, filternames[index]) == 0)
      {
         found = TRUE;
         break;
      }
      else if (!filternames[index][0])
      {
         strncpy(filternames[index], filterName, sizeof(filternames[index])-1);
         break;
      }

      index++;
   }

   return found;
}


/** Create a concatenated list of ifNames which contain the specified
 *  filter name.
 *
 * @param filtername (IN) The filtername to search for.
 * @param ifNames    (OUT) The concatenated list of all ifNames
 */
void getAllIfNamesContainingFilterName(const char *filtername, char *ifNames)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnFirewallExceptionObject *PppConnFirewall = NULL;
   WanIpConnFirewallExceptionObject *IpConnFirewall = NULL;
   LanIpIntfFirewallExceptionObject *lanFirewall = NULL;
   CmsRet ret = CMSRET_SUCCESS;


   while ( cmsObj_getNext(MDMOID_WAN_PPP_CONN_FIREWALL_EXCEPTION, &iidStack, (void **) &PppConnFirewall) == CMSRET_SUCCESS) 
   {
      if (cmsUtl_strcmp(PppConnFirewall->filterName, filtername) == 0 )
      {
         WanPppConnObject *wan_ppp_conn = NULL;
         InstanceIdStack parentIidStack = iidStack;
         if ((ret = cmsObj_getAncestor(MDMOID_WAN_PPP_CONN, MDMOID_WAN_PPP_CONN_FIREWALL_EXCEPTION, 
                                &parentIidStack, (void **) &wan_ppp_conn)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get WanPppConnObject. ret=%d", ret);
            cmsObj_free((void **) &PppConnFirewall);	
            return;
         }
      
         strcat(ifNames, wan_ppp_conn->X_BROADCOM_COM_IfName);
         strcat(ifNames, ",");
         cmsObj_free((void **) &wan_ppp_conn);
         cmsObj_free((void **) &PppConnFirewall);
      }
   }
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( cmsObj_getNext(MDMOID_WAN_IP_CONN_FIREWALL_EXCEPTION, &iidStack, (void **) &IpConnFirewall) == CMSRET_SUCCESS) 
   {
      if (cmsUtl_strcmp(IpConnFirewall->filterName, filtername) == 0 )
      {
         WanIpConnObject *wan_ip_conn = NULL;
         InstanceIdStack parentIidStack = iidStack;
         if ((ret = cmsObj_getAncestor(MDMOID_WAN_IP_CONN, MDMOID_WAN_IP_CONN_FIREWALL_EXCEPTION, 
                                &parentIidStack, (void **) &wan_ip_conn)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get WanIpConnObject. ret=%d", ret);
            cmsObj_free((void **) &IpConnFirewall);
            return;
         }
      
         strcat(ifNames, wan_ip_conn->X_BROADCOM_COM_IfName);
         strcat(ifNames, ",");
         cmsObj_free((void **) &wan_ip_conn);	
         cmsObj_free((void **) &IpConnFirewall);
      }
   }


   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( cmsObj_getNext(MDMOID_LAN_IP_INTF_FIREWALL_EXCEPTION, &iidStack, (void **) &lanFirewall) == CMSRET_SUCCESS) 
   {
      if (cmsUtl_strcmp(lanFirewall->filterName, filtername) == 0 )
      {
         LanIpIntfObject *ipIntfObj = NULL;
         InstanceIdStack parentIidStack = iidStack;
         if ((ret = cmsObj_getAncestor(MDMOID_LAN_IP_INTF, MDMOID_LAN_IP_INTF_FIREWALL_EXCEPTION, 
                                &parentIidStack, (void **) &ipIntfObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get lanIpIntfObject. ret=%d", ret);
            cmsObj_free((void **) &lanFirewall);
            return;
         }
      
         strcat(ifNames, ipIntfObj->X_BROADCOM_COM_IfName);
         strcat(ifNames, ",");
         cmsObj_free((void **) &ipIntfObj);	
         cmsObj_free((void **) &lanFirewall);
      }
   }


   /* remove the last comma */
   ifNames[strlen(ifNames)-1] = '\0';

   return;
}

#endif


void writeSecurityHeader(FILE *fs) 
{
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<script language=\"javascript\" src=\"portName.js\"></script>\n");
}

void writeSecurityBody(FILE *fs) 
{
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");
}

#ifndef BUILD_DM_WLAN_RETAIL /* should this be something else? */
void cgiScDmzHost(char *query, FILE *fs) 
{
   char addr[BUFLEN_16];
   CmsRet ret = CMSRET_SUCCESS;

   cgiGetValueByName(query, "address", addr);

   if (addr[0] != '\0')
   {
      if (cmsUtl_isValidIpAddress(AF_INET, addr) == FALSE || !strcmp(addr, "0.0.0.0"))
      {
         cmsLog_error("Invalid Dmz host IP address");
         return;		
      }   
   }

   if ((ret = dalDmzHost_addEntry(addr)) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalDmzHost_addEntry failed, ret=%d", ret);
      return;
   }
   else 
   {
      glbSaveConfigNeeded = TRUE;
   }

   do_ej("/webs/scdmz.html", fs);
}
#endif /* #ifndef BUILD_DM_WLAN_RETAIL */

#ifdef DMP_X_BROADCOM_COM_SECURITY_1
/* MAC filtering */
void cgiScMacFlt(char *query, FILE *fs) 
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   
   if ( strcmp(action, "add") == 0 )
   {
      cgiScMacFltAdd(query, fs);
   }
   else if ( strcmp(action, "remove") == 0 )
   {
      cgiScMacFltRemove(query, fs);
   }
   else if ( strcmp(action, "changemp") == 0 )
   {
      cgiScMacFltChangePolicy(query, fs);
   }
   else
   {
      cgiScMacFltView(fs);
   }
}

void cgiScMacFltAdd(char *query, FILE *fs) 
{
   char cmd[BUFLEN_264], destMac[BUFLEN_18], srcMac[BUFLEN_18], direction[BUFLEN_4], protocol[BUFLEN_4], ifName[BUFLEN_32];
   CmsRet ret = CMSRET_SUCCESS;

   cmd[0] = destMac[0] = srcMac[0] = direction[0] = ifName[0] = '\0';

   cgiGetValueByName(query, "protocol", protocol);
   cgiGetValueByName(query, "destMac", destMac);
   cgiGetValueByName(query, "srcMac", srcMac);
   cgiGetValueByName(query, "direction", direction);
   cgiGetValueByName(query, "wanIf", ifName);

   ret = dalSec_addMacFilter(protocol, srcMac, destMac, direction, ifName);

   if (ret == CMSRET_SUCCESS) 
   {
      glbSaveConfigNeeded = TRUE;
      cgiScMacFltView(fs);
   }
   else
   {
      sprintf(cmd, "Add Mac filter failed. Status: %d.", ret);
      cgiWriteMessagePage(fs, "Mac filter Add Error", cmd, "scmacflt.cmd?action=view");
   } 

}

void cgiScMacFltRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char *cp1 = NULL, *cp2 = NULL, *cp3 = NULL, *cp4 = NULL;
   char lst[BUFLEN_1024], cmd[BUFLEN_264];
   char protocol[BUFLEN_16], dstMAC[BUFLEN_18], srcMAC[BUFLEN_18], ifName[BUFLEN_32], direction[BUFLEN_16];
   CmsRet ret = CMSRET_SUCCESS;

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while ( pToken != NULL ) 
   {
      ret = CMSRET_INVALID_ARGUMENTS;
      cp1 = strstr(pToken, "|");
      if ( cp1 == NULL ) 
         break;
	  
      cp2 = strstr(cp1 + 1, "|");
      if ( cp2 == NULL ) 
         break;
	  
      cp3 = strstr(cp2 + 1, "|");
      if ( cp3 == NULL ) 
         break;
	  
      cp4 = strstr(cp3 + 1, "|");
      if ( cp4 == NULL ) 
         break;
	  
      *cp1 = *cp2 = *cp3 = *cp4 = '\0';

      strncpy(protocol, pToken, sizeof(protocol));

      if ( cmsUtl_isValidMacAddress((cp1 + 1)) == TRUE )
	  strncpy(dstMAC, (cp1 + 1), sizeof(dstMAC));
      else
	  strncpy(dstMAC, "\0", sizeof(dstMAC));
	  
      if ( cmsUtl_isValidMacAddress((cp2 + 1)) == TRUE )
         strncpy(srcMAC, (cp2 + 1), sizeof(srcMAC));
      else
         strncpy(srcMAC, "\0", sizeof(srcMAC));
	  
      strncpy(ifName, (cp3 + 1), sizeof(ifName));
      strncpy(direction, (cp4 + 1), sizeof(direction));

      ret = dalSec_deleteMacFilter(protocol, srcMAC, dstMAC, direction, ifName);
      if ( ret != CMSRET_SUCCESS) 
         break;

      pToken = strtok_r(NULL, ", ", &pLast);
   }

   if ( ret == CMSRET_SUCCESS) 
   {
      glbSaveConfigNeeded = TRUE;
      cgiScMacFltView(fs);
   } 
   else 
   {
      sprintf(cmd, "Cannot remove MAC filtering entry.<br>Status: %d.", ret);
      cgiWriteMessagePage(fs, "MAC filter entry Remove Error", cmd,
                          "scmacflt.cmd?action=view");
   }

}

void cgiScMacFltChangePolicy(char *query, FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;
   char cmd[BUFLEN_264], lst[BUFLEN_1024];
   char *pToken = NULL, *pLast = NULL;

   cmd[0] = lst[0] = '\0';
   cgiGetValueByName(query, "changeLst", lst);

   pToken = strtok_r(lst, ", ", &pLast);
   while ( pToken != NULL ) 
   {
      ret = dalSec_ChangeMacFilterPolicy(pToken);
      if ( ret != CMSRET_SUCCESS ) 
         break;

      pToken = strtok_r(NULL, ", ", &pLast);
   }

   if (ret == CMSRET_SUCCESS) 
   {
      glbSaveConfigNeeded = TRUE;
      cgiScMacFltView(fs);
   } 
   else 
   {
      sprintf(cmd, "Cannot change Mac filtering policy.<br>" "Status: %d.", ret);
      cgiWriteMessagePage(fs, "Mac filter policy change error", cmd,
                          "scmacflt.cmd?action=view");
   }
}

void writeScMacFltScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function chgPolicy(ChangePolicy) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (ChangePolicy.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < ChangePolicy.length; i++) {\n");
   fprintf(fs, "         if ( ChangePolicy[i].checked == true )\n");
   fprintf(fs, "            lst += ChangePolicy[i].value + ', ';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( ChangePolicy.checked == true )\n");
   fprintf(fs, "      lst = ChangePolicy.value;\n");

   fprintf(fs, "   var loc = 'scmacflt.cmd?action=changemp&changeLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'scmacflt.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (rml.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "         if ( rml[i].checked == true )\n");
   fprintf(fs, "            lst += rml[i].value + ', ';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( rml.checked == true )\n");
   fprintf(fs, "      lst = rml.value;\n");

   fprintf(fs, "   var loc = 'scmacflt.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


CmsRet cgiScMacFltViewBrWanbody_igd(FILE *fs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *wan_ip_conn = NULL;
   MacFilterObject *macFilterObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   while (cmsObj_getNext(MDMOID_WAN_IP_CONN, &parentIidStack, (void **) &wan_ip_conn) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(wan_ip_conn->connectionType, MDMVS_IP_BRIDGED) == 0 )
      {
         if(cmsObj_getNextInSubTree(MDMOID_MAC_FILTER, &parentIidStack, &iidStack, (void **)&macFilterObj) != CMSRET_SUCCESS)
         {
            cmsLog_error("Cannot get MacFilterCfgObject!!");
            cmsObj_free((void **) &wan_ip_conn);
            return ret;
         }
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", wan_ip_conn->X_BROADCOM_COM_IfName);
         if (cmsUtl_strcmp(macFilterObj->policy, MDMVS_FORWARD) == 0)
            fprintf(fs, "      <td><b><font color='green'>%s</font></b></td>\n", macFilterObj->policy);
         else
            fprintf(fs, "      <td><b><font color='red'>%s</font></b></td>\n", macFilterObj->policy);

         fprintf(fs, "      <td align='center'><input type='checkbox' name='ChangePolicy' value='%s'></td>\n",
                        wan_ip_conn->X_BROADCOM_COM_IfName);
         fprintf(fs, "   </tr>\n");

         cmsObj_free((void **) &macFilterObj);
      }
      cmsObj_free((void **) &wan_ip_conn);
   }

   return ret;
}

CmsRet cgiScMacFltViewCfgbody_igd(FILE *fs)
{
    char ifName[BUFLEN_32];
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
    WanIpConnObject *wan_ip_conn = NULL;
    MacFilterCfgObject *macFilterCfg = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    ifName[0] = '\0';
   INIT_INSTANCE_ID_STACK(&iidStack);
   // write table body
   while ( cmsObj_getNext(MDMOID_MAC_FILTER_CFG, &iidStack, (void **) &macFilterCfg) == CMSRET_SUCCESS) 
   {
      if ( macFilterCfg->enable == TRUE ) 
      {
         fprintf(fs, "   <tr>\n");

         parentIidStack = iidStack;
         if ((ret = cmsObj_getAncestor(MDMOID_WAN_IP_CONN, MDMOID_MAC_FILTER_CFG, 
                                                           &parentIidStack, (void **) &wan_ip_conn)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get WanIpConnObject. ret=%d", ret);
            cmsObj_free((void **) &macFilterCfg);
             return ret;
         }
      
         strncpy(ifName, wan_ip_conn->X_BROADCOM_COM_IfName, sizeof(ifName) - 1);
         cmsObj_free((void **) &wan_ip_conn);	
		 
         fprintf(fs, "      <td>%s</td>\n", ifName);
         fprintf(fs, "      <td>%s</td>\n", macFilterCfg->protocol);
   
         if ( cmsUtl_strcmp(macFilterCfg->destinationMAC, "\0") != 0 ) 
         {
            fprintf(fs, "      <td>%s</td>\n", macFilterCfg->destinationMAC);
         } 
         else
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }

         if ( cmsUtl_strcmp(macFilterCfg->sourceMAC, "\0") != 0 ) 
         {
            fprintf(fs, "      <td>%s</td>\n", macFilterCfg->sourceMAC);
         } 
         else
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         fprintf(fs, "      <td>%s</td>\n", macFilterCfg->direction);

         fprintf(fs, 
            "      <td align='center'><input type='checkbox' name='rml' value='%s|%s|%s|%s|%s'></td>\n",
            macFilterCfg->protocol, macFilterCfg->destinationMAC, macFilterCfg->sourceMAC, ifName, macFilterCfg->direction);

         fprintf(fs, "   </tr>\n");
      }
      cmsObj_free((void **)&macFilterCfg);
   }

    return ret;
}


void cgiScMacFltView(FILE *fs) 
{
   writeSecurityHeader(fs);
   writeScMacFltScript(fs);
   writeSecurityBody(fs);

   /* if no bridge existed in the system, just say the service is not
   * available and return.
   */
   if (!qdmIpIntf_isBridgedWanExistedLocked())
   {
      fprintf(fs, "<b>No Bridge WAN service found.  A bridge WAN service is needed to configure this service.</b><br><br>\n");
      fprintf(fs, "</center>\n");
      fprintf(fs, "</form>\n");
      fprintf(fs, "</blockquote>\n");
      fprintf(fs, "</body>\n");
      fprintf(fs, "</html>\n");
      fflush(fs);
      return;
   }

   fprintf(fs, "<b>MAC Filtering Setup</b><br><br>\n");
   
   fprintf(fs, "MAC Filtering is only effective on ATM PVCs configured in Bridge mode. \
      <b><font color='green'>FORWARDED</font></b> means that all MAC layer frames will be \
      <b><font color='green'>FORWARDED</font></b> except those matching with any of the \
      specified rules in the following table.  <b><font color='red'>BLOCKED</font></b> \
      means that all MAC layer frames will be <b><font color='red'>BLOCKED</font></b> \
      except those matching with any of the specified rules in the following table.<br><br>\n");
      
   fprintf(fs, "MAC Filtering Policy For Each Interface: <br>\n");
   fprintf(fs, "<b><font color='red'>WARNING: Changing from one policy to another of an interface will cause\
                  all defined rules for that interface to be REMOVED AUTOMATICALLY! You will need to create \
                  new rules for the new policy.</font></b><br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Policy</td>\n");
   fprintf(fs, "      <td class='hd'>Change</td>\n");
   fprintf(fs, "   </tr>\n");

   /* find all bridge wan */
   if(cgiScMacFltViewBrWanbody(fs) != CMSRET_SUCCESS)
   {
      return ;
   }

   fprintf(fs, "</table><br>\n");
   fprintf(fs, "<br><center><input type='button' onClick='chgPolicy(this.form.ChangePolicy)' value='Change Policy'></center><br>\n");
   fprintf(fs, "</center>\n");

   fprintf(fs, "Choose Add or Remove to configure MAC filtering rules.<br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Protocol</td>\n");
   fprintf(fs, "      <td class='hd'>Destination&nbsp;MAC</td>\n");
   fprintf(fs, "      <td class='hd'>Source&nbsp;MAC</td>\n");
   fprintf(fs, "      <td class='hd'>Frame&nbsp;Direction</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   if(cgiScMacFltViewCfgbody(fs) != CMSRET_SUCCESS)
   {
      return ;
   }

   fprintf(fs, "</table><br>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}
#endif
