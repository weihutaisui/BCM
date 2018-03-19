/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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


#ifdef DMP_DEVICE2_BASELINE_1


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "rut_wan.h"
#include "dal.h"

extern CmsRet cmsDalDpx_updateDnsproxy(void);



void getDeviceInfo_dev2(WEB_NTWK_VAR *webVar)
{
   char *strPtr = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DeviceInfoObject *deviceInfo = NULL;
   CmsRet ret;

   memset(webVar->boardID, 0, sizeof(webVar->boardID));
   memset(webVar->swVers, 0, sizeof(webVar->swVers));
   memset(webVar->cfeVers, 0, sizeof(webVar->cfeVers));
   memset(webVar->cmsBuildTimestamp, 0, sizeof(webVar->cmsBuildTimestamp));

   if ((ret = cmsObj_get(MDMOID_DEV2_DEVICE_INFO, &iidStack, 0, (void *) &deviceInfo)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get DEVICE_INFO, ret=%d", ret);
      return;
   }

   if (deviceInfo->additionalHardwareVersion != NULL &&
       (strPtr = strstr(deviceInfo->additionalHardwareVersion, "BoardId=")) != NULL)
   {
      strncpy(webVar->boardID, &(strPtr[8]), sizeof(webVar->boardID)-1);
   }

   if (deviceInfo->softwareVersion != NULL)
   {
      strncpy(webVar->swVers, deviceInfo->softwareVersion, sizeof(webVar->swVers)-1);
   }

   if (deviceInfo->additionalSoftwareVersion != NULL &&
       (strPtr = strstr(deviceInfo->additionalSoftwareVersion, "CFE=")) != NULL)
   {
      strncpy(webVar->cfeVers, &(strPtr[4]), sizeof(webVar->cfeVers)-1);
   }

   if (deviceInfo->X_BROADCOM_COM_SwBuildTimestamp != NULL)
   {
      strncpy(webVar->cmsBuildTimestamp, deviceInfo->X_BROADCOM_COM_SwBuildTimestamp, sizeof(webVar->cmsBuildTimestamp)-1);
   }

   webVar->numCpuThreads = deviceInfo->processorNumberOfEntries;

#ifdef DMP_X_BROADCOM_COM_DSL_1
   memset(webVar->dslPhyDrvVersion, 0, sizeof(webVar->dslPhyDrvVersion));
   if (deviceInfo->X_BROADCOM_COM_DslPhyDrvVersion != NULL)
   {
      strncpy(webVar->dslPhyDrvVersion, deviceInfo->X_BROADCOM_COM_DslPhyDrvVersion, sizeof(webVar->dslPhyDrvVersion)-1);
   }
#endif


#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   memset(webVar->voiceServiceVersion, 0, sizeof(webVar->voiceServiceVersion));
   if (deviceInfo->X_BROADCOM_COM_VoiceServiceVersion != NULL)
   {
      strncpy(webVar->voiceServiceVersion, deviceInfo->X_BROADCOM_COM_VoiceServiceVersion, sizeof(webVar->voiceServiceVersion)-1);
   }
#endif

   cmsObj_free((void **) &deviceInfo);
}



#ifdef SUPPORT_TR69C

CmsRet cmsDal_setTr69cCfg_dev2(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2ManagementServerObject *acsCfg = NULL;
   Tr69cCfgObject *tr69cCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("get tr69c cfg obj");
   if ((ret = cmsObj_get(MDMOID_TR69C_CFG, &iidStack, 0, (void *) &tr69cCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TR69C_CFG failed, ret=%d", ret);
      return ret;
   }

   tr69cCfg->loggingSOAP = (webVar->tr69cDebugEnable == FALSE) ? FALSE : TRUE;
   tr69cCfg->connectionRequestAuthentication = (webVar->tr69cNoneConnReqAuth == TRUE) ? FALSE : TRUE;

   cmsLog_debug("set tr69c cfg obj");
   ret = cmsObj_set(tr69cCfg, &iidStack);
   cmsObj_free((void **) &tr69cCfg);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of TR69C_CFG failed, ret=%d", ret);
   }


   cmsLog_debug("Get management server obj");
   if ((ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of DEV2_MANAGEMENT_SERVER failed, ret=%d", ret);
      return ret;
   }

   acsCfg->periodicInformEnable = (webVar->tr69cInformEnable == FALSE) ? FALSE : TRUE;
   acsCfg->periodicInformInterval = webVar->tr69cInformInterval;

   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->URL, webVar->tr69cAcsURL);
   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->username, webVar->tr69cAcsUser);
   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->password, webVar->tr69cAcsPwd);

   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->connectionRequestUsername, webVar->tr69cConnReqUser);
   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->connectionRequestPassword, webVar->tr69cConnReqPwd);
   REPLACE_STRING_IF_NOT_EQUAL(acsCfg->X_BROADCOM_COM_BoundIfName, webVar->tr69cBoundIfName);

   cmsLog_debug("set management server obj");
   ret = cmsObj_set(acsCfg, &iidStack);
   cmsObj_free((void **) &acsCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of DEV2_MANAGEMENT_SERVER failed, ret=%d", ret);
      return ret;
   }
   else
   {
      /* recreate /var/dnsinfo for updating the specific wan interface dns  for resolve needs */
      cmsDalDpx_updateDnsproxy();
   }

   cmsLog_debug("done, ret=%d", ret);

   return ret;
}


void cmsDal_getTr69cCfg_dev2(WEB_NTWK_VAR *webVar  __attribute__((unused)))
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2ManagementServerObject *acsCfg = NULL;
   Tr69cCfgObject *tr69cCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("update glbWebVar tr69c info from MDM");

   if ((ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MDMOID_DEV2_MANAGEMENT_SERVER failed, ret=%d", ret);
      return;
   }

   webVar->tr69cInformEnable = acsCfg->periodicInformEnable;
   webVar->tr69cInformInterval = acsCfg->periodicInformInterval;

   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cAcsURL, acsCfg->URL);
   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cAcsUser, acsCfg->username);
   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cAcsPwd, acsCfg->password);

   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cConnReqURL, acsCfg->connectionRequestURL);
   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cConnReqUser, acsCfg->connectionRequestUsername);
   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cConnReqPwd, acsCfg->connectionRequestPassword);

   STR_COPY_OR_NULL_TERMINATE(webVar->tr69cBoundIfName, acsCfg->X_BROADCOM_COM_BoundIfName);

   cmsObj_free((void **) &acsCfg);


   if ((ret = cmsObj_get(MDMOID_TR69C_CFG, &iidStack, 0, (void *) &tr69cCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TR69C_CFG failed, ret=%d", ret);
      return;
   }

   webVar->tr69cDebugEnable = (tr69cCfg->loggingSOAP == FALSE) ? FALSE : TRUE;
   webVar->tr69cNoneConnReqAuth = (tr69cCfg->connectionRequestAuthentication == TRUE) ? FALSE : TRUE;

   cmsObj_free((void **) &tr69cCfg);
}

#endif   /* SUPPORT_TR69C */


#ifdef DMP_DEVICE2_TIME_1

CmsRet cmsDal_setNtpCfg_dev2(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2TimeObject *ntpCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_DEV2_TIME, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of DEV2_TIME failed, ret=%d", ret);
      return ret;
   }

   ntpCfg->enable = webVar->NTPEnable;
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer1, webVar->NTPServer1);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer2, webVar->NTPServer2);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer3, webVar->NTPServer3);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer4, webVar->NTPServer4);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer5, webVar->NTPServer5);
   CMSMEM_REPLACE_STRING(ntpCfg->localTimeZone, webVar->localTimeZone);
   CMSMEM_REPLACE_STRING(ntpCfg->X_BROADCOM_COM_LocalTimeZoneName, webVar->localTimeZoneName);
   ntpCfg->X_BROADCOM_COM_DaylightSavingsUsed = (webVar->daylightSavingsUsed == FALSE) ? FALSE : TRUE;
   CMSMEM_REPLACE_STRING(ntpCfg->X_BROADCOM_COM_DaylightSavingsStart, webVar->daylightSavingsStart);
   CMSMEM_REPLACE_STRING(ntpCfg->X_BROADCOM_COM_DaylightSavingsEnd, webVar->daylightSavingsEnd);

   ret = cmsObj_set(ntpCfg, &iidStack);
   cmsObj_free((void **) &ntpCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of DEV2_TIME failed, ret=%d", ret);
   }

   return ret;
}

void cmsDal_getNtpCfg_dev2(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2TimeObject *ntpCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_DEV2_TIME, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of DEV2_TIME failed, ret=%d", ret);
      return;
   }

   webVar->NTPEnable = ntpCfg->enable;
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer1, ntpCfg->NTPServer1);
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer2, ntpCfg->NTPServer2);
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer3, ntpCfg->NTPServer3);
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer4, ntpCfg->NTPServer4);
   STR_COPY_OR_NULL_TERMINATE(webVar->NTPServer5, ntpCfg->NTPServer5);
   STR_COPY_OR_NULL_TERMINATE(webVar->localTimeZone, ntpCfg->localTimeZone);
   STR_COPY_OR_NULL_TERMINATE(webVar->localTimeZoneName, ntpCfg->X_BROADCOM_COM_LocalTimeZoneName);
   webVar->daylightSavingsUsed = (ntpCfg->X_BROADCOM_COM_DaylightSavingsUsed == FALSE) ? FALSE : TRUE;
   STR_COPY_OR_NULL_TERMINATE(webVar->daylightSavingsStart, ntpCfg->X_BROADCOM_COM_DaylightSavingsStart);
   STR_COPY_OR_NULL_TERMINATE(webVar->daylightSavingsEnd, ntpCfg->X_BROADCOM_COM_DaylightSavingsEnd);

   cmsObj_free((void **) &ntpCfg);
}

#endif   /* DMP_DEVICE2_TIME_1 */


UBOOL8 isWanIPIntfIpAdressExit(InstanceIdStack *parentIidStack, char **ipAddr, char **subnetMask)
{
   UBOOL8 found = FALSE;
   Dev2Ipv4AddressObject *ipv4Addr = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   
   while (!found &&
         (ret = cmsObj_getNextInSubTree(MDMOID_DEV2_IPV4_ADDRESS, 
                                        parentIidStack, 
                                        &iidStack, 
                                        (void **) &ipv4Addr)) == CMSRET_SUCCESS)
   {
      if (ipv4Addr->IPAddress && ipv4Addr->subnetMask)
      {
         *ipAddr = cmsMem_strdup(ipv4Addr->IPAddress);
         *subnetMask = cmsMem_strdup(ipv4Addr->subnetMask);
          found = TRUE;
      }
      else
      {
         cmsLog_debug("One or more DEV2_IPV4_ADDRESS parameters are null");
      }
      cmsObj_free((void **) &ipv4Addr);
   }

   return found;
      
}

void getDefaultWanConnParams_dev2(WEB_NTWK_VAR *webVar)
{
 
   Dev2IpInterfaceObject *ipIntf=NULL;
   Dev2PppInterfaceObject *pppIntfObj=NULL;
   Dev2PppInterfacePpoeObject *pppoeObj=NULL;
   CmsRet ret=CMSRET_SUCCESS;

   cmsLog_debug("loading defaults from ipIntf object");

   /* Initial wanIpAddress/wanSubnetMask first for WAN ipIntf */
   cmsUtl_strncpy(webVar->wanIpAddress, "0.0.0.0", sizeof(webVar->wanIpAddress));
   cmsUtl_strncpy(webVar->wanSubnetMask, "0.0.0.0", sizeof(webVar->wanSubnetMask));
   cmsUtl_strncpy(webVar->wanIntfGateway, "0.0.0.0", sizeof(webVar->wanIntfGateway));
   
   if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &ipIntf)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get default ipIntf, ret=%d", ret);
      return;
   }


   /*
    * Note we are getting the default values for this object.
    * This call should always work, and does not need an
    * iidStack pointer.
    */
    cmsLog_debug("loading defaults from pppIntf object");

   if ((ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &pppIntfObj)) != CMSRET_SUCCESS)

   {
      cmsLog_error("Failed to get pppoeObj, ret = %d", ret);
      cmsObj_free((void **) &ipIntf);
      return;   
   }
   if ((ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_PPOE, 
                         NULL, 
                         OGF_DEFAULT_VALUES, 
                         (void **) &pppoeObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get pppoeObj, ret = %d", ret);
      cmsObj_free((void **) &pppIntfObj);      
      cmsObj_free((void **) &ipIntf);
      return;
   }
  
   if (pppIntfObj->username != NULL)
   {
      cmsUtl_strcpy(webVar->pppUserName, pppIntfObj->username);
   }
   if (pppIntfObj->password != NULL)
   {
      cmsUtl_strcpy(webVar->pppPassword, pppIntfObj->password);
   }
   if (pppIntfObj->alias!= NULL)
   {
      cmsUtl_strcpy(webVar->serviceName, pppIntfObj->alias);
   }

   if (pppoeObj->serviceName != NULL)
   {
      cmsUtl_strcpy(webVar->pppServerName, pppoeObj->serviceName);
   }

   
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   webVar->enblIgmp = ipIntf->X_BROADCOM_COM_IGMPEnabled;
   webVar->enblIgmpMcastSource = ipIntf->X_BROADCOM_COM_IGMP_SOURCEEnabled;
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   webVar->noMcastVlanFilter  = ipIntf->X_BROADCOM_COM_NoMcastVlanFilter;
#endif

   webVar->enblService = 1;  /* always enable service even though data model default is false */
   webVar->ntwkPrtcl = 3;  /* magic number, PPPoE */
   webVar->encapMode = 0;

   webVar->enblAutoScan = 0; /* don't support autoscan anymore, delete? */

   webVar->enblOnDemand = 0; /* which means onDemand */
   webVar->pppTimeOut = 0; /* not in data model */
   webVar->pppIpExtension = pppoeObj->X_BROADCOM_COM_IPExtension;
   webVar->pppAuthMethod = 0; /* which means AUTO_AUTH */
   webVar->pppToBridge = pppoeObj->X_BROADCOM_COM_AddPppToBridge;
   webVar->useStaticIpAddress = pppIntfObj->X_BROADCOM_COM_UseStaticIPAddress;
   if (pppIntfObj->X_BROADCOM_COM_LocalIPAddress != NULL)
   {
      cmsUtl_strcpy(webVar->pppLocalIpAddress, pppIntfObj->X_BROADCOM_COM_LocalIPAddress);
   }
   else
   {
      cmsUtl_strcpy(webVar->pppLocalIpAddress, "0.0.0.0");
   }
   webVar->enblPppDebug = pppIntfObj->X_BROADCOM_COM_Enable_Debug;
#ifdef DMP_X_BROADCOM_COM_MLD_1
   webVar->enblMld = ipIntf->X_BROADCOM_COM_MLDEnabled;
   webVar->enblMldMcastSource = ipIntf->X_BROADCOM_COM_MLD_SOURCEEnabled;
#endif   /* DMP_X_BROADCOM_COM_MLD_1 */
   webVar->pcpMode = 0;
   strcpy(webVar->pcpServer, "");

   cmsObj_free((void **) &pppoeObj);
   cmsObj_free((void **) &pppIntfObj);      
   cmsObj_free((void **) &ipIntf);

   return;
}

#ifdef SUPPORT_DSL

#ifdef DMP_DEVICE2_DSL_1

void getDefaultAtmLinkCfg_dev2(WEB_NTWK_VAR *webVar)
{
   Dev2AtmLinkObject *atmLinkObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * Note we are getting the default values for this object.
    * This call should always work, and does not need an
    * iidStack pointer.
    */
   if ((ret = cmsObj_get(MDMOID_DEV2_ATM_LINK,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &atmLinkObj)) != CMSRET_SUCCESS)
{
      cmsLog_error("Could not get default DEV2_ATM_LINK, ret=%d", ret);
      return;
   }

   STR_COPY_OR_NULL_TERMINATE(webVar->linkType, atmLinkObj->linkType);
   cmsUtl_atmVpiVciStrToNum_dev2(atmLinkObj->destinationAddress,
                                 &(webVar->atmVpi),
                                 &(webVar->atmVci));

   STR_COPY_OR_NULL_TERMINATE(webVar->grpScheduler,
                              atmLinkObj->X_BROADCOM_COM_GrpScheduler);
   webVar->grpWeight     = atmLinkObj->X_BROADCOM_COM_GrpWeight;
   webVar->grpPrecedence = atmLinkObj->X_BROADCOM_COM_GrpPrecedence;

   STR_COPY_OR_NULL_TERMINATE(webVar->schedulerAlgorithm,
                              atmLinkObj->X_BROADCOM_COM_SchedulerAlgorithm);
   webVar->queueWeight     = atmLinkObj->X_BROADCOM_COM_QueueWeight;
   webVar->queuePrecedence = atmLinkObj->X_BROADCOM_COM_QueuePrecedence;

   STR_COPY_OR_NULL_TERMINATE(webVar->atmServiceCategory, MDMVS_UBR);
   webVar->atmPeakCellRate = 0;
   webVar->atmSustainedCellRate = 0;
   webVar->atmMaxBurstSize = 0;
   webVar->atmMinCellRate = -1;

   cmsObj_free((void **) &atmLinkObj);
}

#endif   /* DMP_DEVICE2_DSL_1 */


#ifdef DMP_DEVICE2_PTMLINK_1

void getDefaultPtmLinkCfg_dev2(WEB_NTWK_VAR *webVar)
{
   Dev2PtmLinkObject *ptmLinkObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * Note we are getting the default values for this object.
    * This call should always work, and does not need an
    * iidStack pointer.
    */
   if ((ret = cmsObj_get(MDMOID_DEV2_PTM_LINK,
                         NULL,
                         OGF_DEFAULT_VALUES,
                         (void *) &ptmLinkObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get default DEV2_PTM_LINK, ret=%d", ret);
      return;
   }

   webVar->portId     = ptmLinkObj->X_BROADCOM_COM_PTMPortId;
   webVar->ptmPriorityNorm = ptmLinkObj->X_BROADCOM_COM_PTMPriorityLow;
   webVar->ptmPriorityHigh = ptmLinkObj->X_BROADCOM_COM_PTMPriorityHigh;

   webVar->enblQos = ptmLinkObj->X_BROADCOM_COM_PTMEnbQos;

   STR_COPY_OR_NULL_TERMINATE(webVar->grpScheduler,
                              ptmLinkObj->X_BROADCOM_COM_GrpScheduler);
   webVar->grpWeight     = ptmLinkObj->X_BROADCOM_COM_GrpWeight;
   webVar->grpPrecedence = ptmLinkObj->X_BROADCOM_COM_GrpPrecedence;

   STR_COPY_OR_NULL_TERMINATE(webVar->schedulerAlgorithm,
                              ptmLinkObj->X_BROADCOM_COM_SchedulerAlgorithm);
   webVar->queueWeight     = ptmLinkObj->X_BROADCOM_COM_QueueWeight;
   webVar->queuePrecedence = ptmLinkObj->X_BROADCOM_COM_QueuePrecedence;

   cmsObj_free((void **) &ptmLinkObj);
}

#endif   /* DMP_DEVICE2_PTMLINK_1 */


#if defined DMP_DEVICE2_DSL_1 || defined DMP_DEVICE2_PTMLINK_1
void getDefaultWanDslLinkCfg_dev2(WEB_NTWK_VAR *webVar)
#else
void getDefaultWanDslLinkCfg_dev2(WEB_NTWK_VAR *webVar __attribute__((unused)))
#endif
{
#ifdef DMP_DEVICE2_DSL_1
   getDefaultAtmLinkCfg_dev2(webVar);
#endif /* DMP_DEVICE2_DSL_1 */

#ifdef DMP_DEVICE2_PTMLINK_1
   getDefaultPtmLinkCfg_dev2(webVar);
#endif /* DMP_DEVICE2_PTMLINK_1 */   
}

#endif /* SUPPORT_DSL */



CmsRet cmsDal_getEnblNatForWeb_dev2(char *varValue)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntf=NULL;
   UBOOL8 natEnable=FALSE;

   while ((natEnable==FALSE) && (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipIntf) == CMSRET_SUCCESS))
   {
      natEnable=qdmIpIntf_isNatEnabledOnIntfNameLocked(ipIntf->name);
      cmsObj_free((void **)&ipIntf);		
   }
	
   strcpy(varValue, natEnable ? "1":"0");
   return natEnable;
}

#ifdef DMP_DEVICE2_OPTICAL_1
UBOOL8 dalOptical_getIntfByIfName(const char *ifName, InstanceIdStack *iidStack, OpticalInterfaceObject **optIntfObj)
{
    return rutOptical_getIntfByIfName(ifName, iidStack, optIntfObj);
} 

UBOOL8 dalOptical_getIntfByIfNameEnabled(const char *ifName, InstanceIdStack *iidStack, OpticalInterfaceObject **optIntfObj, UBOOL8 enabled)
{
    return rutOptical_getIntfByIfNameEnabled(ifName, iidStack, optIntfObj, enabled);
} 

CmsRet dalOptical_getInterface(NameList **ifList, const char *ifName, UBOOL8 enable)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OpticalInterfaceObject *optIntfObj = NULL;

    if (rutOptical_getIntfByIfNameEnabled(ifName, &iidStack, &optIntfObj, enable) == TRUE)
        cmsDal_addNameToNameList(ifName, ifList); 

    return CMSRET_SUCCESS;
}

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
CmsRet cmsDal_getAvailableL2GponIntf_dev2(NameList **ifList)
{
    return dalOptical_getInterface(ifList, GPON_WAN_IF_NAME, FALSE);
} 
#endif  /* DMP_X_BROADCOM_COM_GPONWAN_1 */


#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
CmsRet cmsDal_getAvailableL2EponIntf_dev2(NameList **ifList)
{
    return dalOptical_getInterface(ifList, EPON_WAN_IF_NAME, FALSE);
} 
#endif  /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#endif /* DMP_DEVICE2_OPTICAL_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */

