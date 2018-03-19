/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
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
:>
*/

/** cmd driven CLI code goes into this file */

#ifdef DMP_DEVICE2_BASELINE_1

#ifdef SUPPORT_CLI_CMD

#include <time.h>

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "qdm_intf.h"
#include "prctl.h"
#include "cli.h"
#include "adslctlapi.h"
#include "devctl_adsl.h"

void processSwVersionCmd_dev2(char *cmdLine)
{
   Dev2DeviceInfoObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   ret = cmsObj_get(MDMOID_DEV2_DEVICE_INFO, &iidStack, 0, (void **) &obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get device info object, ret=%d", ret);
      return;
   }

   if (!strcasecmp(cmdLine, "--help") || !strcasecmp(cmdLine, "-h"))
   {
      printf("usage: swversion \n");
      printf("       [-b | --buildtimestamp]\n");
      printf("       [-c | --cfe]\n");
#ifdef DMP_X_BROADCOM_COM_DSL_1
      printf("       [-d | --dsl]\n");
#endif
      printf(".......[-m | --model]\n");
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
      printf("       [-v | --voice]\n");
#endif
   }
   else if (!strcasecmp(cmdLine, "--buildtimestamp") || !strcasecmp(cmdLine, "-b"))
   {
      printf("%s\n", obj->X_BROADCOM_COM_SwBuildTimestamp);
   }
   else if (!strcasecmp(cmdLine, "--cfe") || !strcasecmp(cmdLine, "-c"))
   {
      char *start=NULL;
      if (obj->additionalSoftwareVersion)
      {
          start = strstr(obj->additionalSoftwareVersion, "CFE=");
      }
      if (start)
      {
         printf("%s\n", start+4);
      }
      else
      {
          printf("Could not find CFE version\n");
      }
   }
   else if (!strcasecmp(cmdLine, "--model") || !strcasecmp(cmdLine, "-m"))
   {
      printf("%s\n", obj->modelName);
   }
#ifdef DMP_X_BROADCOM_COM_DSL_1
   else if (!strcasecmp(cmdLine, "--dsl") || !strcasecmp(cmdLine, "-d"))
   {
      printf("%s\n", obj->X_BROADCOM_COM_DslPhyDrvVersion);
   }
#endif
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   else if (!strcasecmp(cmdLine, "--voice") || !strcasecmp(cmdLine, "-v"))
   {
      printf("%s\n", obj->X_BROADCOM_COM_VoiceServiceVersion);
   }
#endif
   else
   {
      printf("%s\n", obj->softwareVersion);
   }

   cmsObj_free((void **) &obj);

   return;
}


// rutSys_getDeviceLog() is in rut_system.c
char *rutSys_getDeviceLog(UINT16 *logLen);

void processSyslogCmd_dev2(char *cmdLine)
{
   UINT16 logLen = 0;
   char *log = NULL;

   cmsLog_debug("cmdLine =>%s<=", cmdLine);

   if (!strcasecmp(cmdLine, "dump"))
   {
      log = rutSys_getDeviceLog(&logLen);

      if (log != NULL && logLen != 0)
      {
         printf("%s\n", log);
      }

      /*
       * log may not be NULL, but logLen==0.
       * So put the free out here to make
       * sure the log buffer is freed.
       */
      CMSMEM_FREE_BUF_AND_NULL_PTR(log);
   }
   else
   {
      cmdSyslogHelp();
   }
}


void processUptimeCmd_dev2(void)
{
   Dev2DeviceInfoObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   UINT32 s = 0;
   char uptimeString[BUFLEN_512];

   ret = cmsObj_get(MDMOID_DEV2_DEVICE_INFO, &iidStack, 0, (void **) &obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get device info object, ret=%d", ret);
      return;
   }

   s = obj->upTime;

   cmsObj_free((void **) &obj);

   cmsTms_getDaysHoursMinutesSeconds(s, uptimeString, sizeof(uptimeString));

   printf("%s\n", uptimeString);

   return;
}




CmsRet cliCmd_configLanIp_dev2(UBOOL8 isPrimary, const char *ip, const char *mask)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   Dev2Ipv4AddressObject *ipv4Addr = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

   cmsLog_debug("Entered: isPrimary=%d ip=%s mask=%s", isPrimary, ip, mask);

   if (isPrimary == FALSE)
   {
      fprintf(stderr, "lan: in TR181 mode, only config of primary ipaddr is supported\n");
      return CMSRET_INVALID_ARGUMENTS;
   }

   // for now just configure the first LAN bridge (br0) just like TR98.
   // But we could easily support config of any IP interface.
   // XXX Assumes interface has only 1 IP addr obj, and only IPv4
    while (found == FALSE &&
           cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &ipIntfObj) == CMSRET_SUCCESS)
    {
       if (cmsUtl_strcmp(ipIntfObj->name, "br0") == 0)
       {
          found = TRUE;
          if (cmsObj_getNextInSubTree(MDMOID_DEV2_IPV4_ADDRESS,
                                      &iidStack, &iidStackChild,
                                      (void **) &ipv4Addr) == CMSRET_SUCCESS)
          {
             if (!cmsUtl_strcmp(ipv4Addr->addressingType, MDMVS_STATIC))
             {
                /* can only manually configure a static IP addr obj */
                CMSMEM_REPLACE_STRING(ipv4Addr->IPAddress, ip);
                CMSMEM_REPLACE_STRING(ipv4Addr->subnetMask, mask);

                if ((ret = cmsObj_set(ipv4Addr, &iidStackChild)) != CMSRET_SUCCESS)
                {
                   cmsLog_error("Failed to set Dev2Ipv4AddressObject, ret=%d", ret);
                }
             }
             else
             {
                cmsLog_error("cannot configure a dynamically acquired IP addr");
                ret = CMSRET_REQUEST_DENIED;
             }

             cmsObj_free((void **) &ipv4Addr);
          }
       }

       cmsObj_free((void **) &ipIntfObj);
    }

    if (found == FALSE)
    {
       cmsLog_error("Failed to get IPInterface, ret = %d", ret);
    }

    return ret;
}


CmsRet cliCmd_deleteLanIp_dev2(UBOOL8 isPrimary)
{

   cmsLog_debug("Entered: isPrimary=%d", isPrimary);

   fprintf(stderr, "lan: in TR181 mode, delete operation is not supported\n");
   return CMSRET_INVALID_ARGUMENTS;

#ifdef no_delete
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   Dev2Ipv4AddressObject *ipv4Addr = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;

   // for now just configure the first LAN bridge (br0) just like TR98.
   // But we could easily support config of any IP interface.
   // XXX Assumes interface has only 1 IP addr obj, and only IPv4
    while (found == FALSE &&
           cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **) &ipIntfObj) == CMSRET_SUCCESS)
    {
       if (cmsUtl_strcmp(ipIntfObj->name, "br0") == 0)
       {
          found = TRUE;
          if (cmsObj_getNextInSubTree(MDMOID_DEV2_IPV4_ADDRESS,
                                      &iidStack, &iidStackChild,
                                      (void **) &ipv4Addr) == CMSRET_SUCCESS)
          {
             if (!cmsUtl_strcmp(ipv4Addr->addressingType, MDMVS_STATIC))
             {
                /* can only manually delete a static IP addr obj */
                if((ret = cmsObj_deleteInstance(MDMOID_DEV2_IPV4_ADDRESS, &iidStackChild)) != CMSRET_SUCCESS)
                {
                   cmsLog_error("Failed to delete LanIpIntfObject, ret=%d", ret);
                }
             }
             else
             {
                cmsLog_error("cannot delete a dynamically acquired IP addr");
                ret = CMSRET_REQUEST_DENIED;
             }

             cmsObj_free((void **) &ipv4Addr);
          }
       }

       cmsObj_free((void **) &ipIntfObj);
    }

    if (found == FALSE)
    {
       cmsLog_error("Failed to get IPInterface, ret = %d", ret);
    }

    return ret;
#endif  /*  no_delete */
}


CmsRet cliCmd_updateDhcpServer_dev2
   (const char *minAddress,
    const char *maxAddress,
    UINT32 leaseTime)
{
    Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    char ifName[CMS_IFNAME_LENGTH]={0};
    UBOOL8 found = FALSE;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;;

    while (found == FALSE &&
           cmsObj_getNext(MDMOID_DEV2_DHCPV4_SERVER_POOL, &iidStack,
                          (void **)&dhcpv4ServerPoolObj) == CMSRET_SUCCESS)
    {
        if (qdmIntf_fullPathToIntfnameLocked_dev2(dhcpv4ServerPoolObj->interface, ifName) == CMSRET_SUCCESS &&
            cmsUtl_strcmp(ifName, "br0") == 0)
        {
            found = TRUE;
            CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->minAddress, minAddress);
            CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->maxAddress, maxAddress);
            dhcpv4ServerPoolObj->leaseTime = leaseTime;

            if ((ret = cmsObj_set(dhcpv4ServerPoolObj, &iidStack)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Failed to set dhcpv4ServerPoolObj, ret=%d", ret);
            }
        }

        cmsObj_free((void **)&dhcpv4ServerPoolObj);
    }

    if (found == FALSE)
    {
       cmsLog_error("Could not find DHCP server pointing to br0");
    }

    return ret;
}


CmsRet cliCmd_showDhcpServer_dev2(void)
{
    Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    char ifName[CMS_IFNAME_LENGTH]={0};
    UBOOL8 found = FALSE;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;;

    while (found == FALSE &&
           cmsObj_getNext(MDMOID_DEV2_DHCPV4_SERVER_POOL, &iidStack,
                          (void **)&dhcpv4ServerPoolObj) == CMSRET_SUCCESS)
    {
        if (qdmIntf_fullPathToIntfnameLocked_dev2(dhcpv4ServerPoolObj->interface, ifName) == CMSRET_SUCCESS &&
            cmsUtl_strcmp(ifName, "br0") == 0)
        {
            found = TRUE;
            printf("dhcpserver: %s\n", dhcpv4ServerPoolObj->enable ? "enable" : "disable");
            printf("start ip address: %s\n", dhcpv4ServerPoolObj->minAddress);
            printf("end ip address: %s\n", dhcpv4ServerPoolObj->maxAddress);
            printf("leased time: %d hours\n", dhcpv4ServerPoolObj->leaseTime/3600);         
        }

        cmsObj_free((void **)&dhcpv4ServerPoolObj);
    }

    if (found == FALSE)
    {
       cmsLog_error("Could not find DHCP server pointing to br0");
    }

    return ret;
}


CmsRet cliCmd_configDhcpServer_dev2(UBOOL8 enable)
{
   Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char ifName[CMS_IFNAME_LENGTH]={0};
   UBOOL8 found=FALSE;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;;

   cmsLog_debug("Entered: enable=%d", enable);


    while (found == FALSE &&
           cmsObj_getNext(MDMOID_DEV2_DHCPV4_SERVER_POOL, &iidStack,
                          (void **)&dhcpv4ServerPoolObj) == CMSRET_SUCCESS)
    {
       if (qdmIntf_fullPathToIntfnameLocked_dev2(dhcpv4ServerPoolObj->interface, ifName) == CMSRET_SUCCESS &&
           cmsUtl_strcmp(ifName, "br0") == 0)
       {
          found = TRUE;
          dhcpv4ServerPoolObj->enable = enable;
          if ((ret = cmsObj_set(dhcpv4ServerPoolObj, &iidStack)) != CMSRET_SUCCESS)
          {
             cmsLog_error("Failed to set dhcpv4ServerPoolObj, ret=%d", ret);
          }
       }

       cmsObj_free((void **)&dhcpv4ServerPoolObj);
    }

    if (found == FALSE)
    {
       cmsLog_error("Could not find DHCP server pointing to br0");
    }

    return ret;
}


CmsRet cliCmd_configDhcpClient_dev2(UBOOL8 enable)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   Dev2Dhcpv4ClientObject *clientObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char *ipIntfFullPath=NULL;
   UBOOL8 found=FALSE;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;;

   cmsLog_debug("Entered: enable=%d", enable);

   /* get fullpath to IP.Interface object for br0 */
   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ipIntfObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ipIntfObj->name, "br0") == 0)
      {
         MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
         found = TRUE;
         pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
         pathDesc.iidStack = iidStack;
         ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &ipIntfFullPath);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not convert pathDesc to fullPath");
            cmsObj_free((void **) &ipIntfObj);
            return CMSRET_INTERNAL_ERROR;
         }
      }

      cmsObj_free((void **) &ipIntfObj);
   }

   /*
    * Normally, if dhcp client is disabled, the object will not exist.
    * And when we disable it, we will just delete it.  But just to be
    * on the safe side, during an enable, if we find an existing one,
    * then just modify the enable param on the existing object.
    */
   found = FALSE;
   INIT_INSTANCE_ID_STACK(&iidStack);
   if (enable)
   {
      while (!found &&
             cmsObj_getNextFlags(MDMOID_DEV2_DHCPV4_CLIENT,
                                 &iidStack,
                                 OGF_NO_VALUE_UPDATE,
                                 (void **)&clientObj) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcasecmp(clientObj->interface, ipIntfFullPath))
         {
            found = TRUE;

            clientObj->enable = (enable ? TRUE : FALSE);
            ret = cmsObj_set(clientObj, &iidStack);
         }
         cmsObj_free((void **) &clientObj);
      }
   }

   /*
    * Expecting !found, so use DAL functions to add (enable) or
    * delete (disable) DHCP client.
    */
   if (!found)
   {
      if (enable)
      {
         UINT32 lease = 0;
         char vid[BUFLEN_256], uid[BUFLEN_256];
         char iaid[BUFLEN_16], duid[BUFLEN_256];
         char ipAddr[BUFLEN_16], leasedTime[BUFLEN_16];
         char buf[BUFLEN_16], serverAddr[BUFLEN_16];
#ifdef SUPPORT_HOMEPLUG
         UBOOL8 op125 = TRUE;
#else
         UBOOL8 op125 =FALSE; 
#endif

         memset(vid, 0, BUFLEN_256);
         memset(duid, 0, BUFLEN_256);
         memset(iaid, 0, BUFLEN_16);
         memset(uid, 0, BUFLEN_256);
         memset(buf, 0, BUFLEN_16);
         memset(ipAddr, 0, BUFLEN_16);
         memset(serverAddr, 0, BUFLEN_16);
         memset(leasedTime, 0, BUFLEN_16);

         qdmDhcpv4Client_getSentOption_dev2(ipIntfFullPath, 60, vid, BUFLEN_256);
         qdmDhcpv4Client_getSentOption_dev2(ipIntfFullPath, 61, duid, BUFLEN_256);
         qdmDhcpv4Client_getSentOption_dev2(ipIntfFullPath, 61, iaid, BUFLEN_8);
         qdmDhcpv4Client_getSentOption_dev2(ipIntfFullPath, 77, uid, BUFLEN_256);
#ifndef SUPPORT_HOMEPLUG
         if (qdmDhcpv4Client_getSentOption_dev2(ipIntfFullPath,
                                                125,
                                                buf,
                                                BUFLEN_16) == CMSRET_SUCCESS)
            op125 = TRUE;
#endif

         qdmDhcpv4Client_getReqOption_dev2(ipIntfFullPath, 50, ipAddr, BUFLEN_16);

         if (qdmDhcpv4Client_getReqOption_dev2(ipIntfFullPath,
                                               51,
                                               leasedTime,
                                               BUFLEN_16) == TRUE)
            sscanf(leasedTime, "%d", &lease);

         qdmDhcpv4Client_getReqOption_dev2(ipIntfFullPath, 54, serverAddr, BUFLEN_16);

         dalDhcp_addIpIntfClient_dev2(ipIntfFullPath,
                                      vid, duid, iaid, uid, op125,
                                      ipAddr, serverAddr, lease);
      }
      else
      {
         /* when we disable, we just delete the client obj */
         dalDhcp_deleteIpIntfClient_dev2(ipIntfFullPath);
      }
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   return ret;
}




static void cmdLanHostsHelp_dev2(void)
{
   printf("\nUsage: lanhosts show all\n");
   printf("       lanhosts help\n");
}

void processLanHostsCmd_dev2(char *cmdLine)
{
   Dev2HostObject *hostObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   cmsLog_debug("cmdLine =>%s<=", cmdLine);

   if (!strcasecmp(cmdLine, "show all"))
   {
      /* print header */
      printf("   MAC Addr          IP Addr     Lease Time Remaining    Hostname\n");

      while (cmsObj_getNext(MDMOID_DEV2_HOST,
                            &iidStack, 
                            (void **)&hostObj) == CMSRET_SUCCESS)
      {
         /* print host */
         printf("%s  %s        %d              %s\n",
                hostObj->physAddress, hostObj->IPAddress,
                hostObj->leaseTimeRemaining, hostObj->hostName);

         cmsObj_free((void **) &hostObj);
      }
   }
   else
   {
      cmdLanHostsHelp_dev2();
   }
}


#ifdef DMP_DEVICE2_TIME_1

extern char *timeZones[];

static CmsRet disableSntp_dev2(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2TimeObject *ntpCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_DEV2_TIME, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TIME failed, ret=%d", ret);
      return ret;
   }

   ntpCfg->enable = FALSE;

   ret = cmsObj_set(ntpCfg, &iidStack);
   cmsObj_free((void **) &ntpCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of TIME failed, ret=%d", ret);
   }

   return ret;
}

static CmsRet configureSntp_dev2(char **sntpServers, char *timeZoneName)
{
   Dev2TimeObject *ntpCfg = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_DEV2_TIME, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TIME failed, ret=%d", ret);
      return ret;
   }

   ntpCfg->enable = TRUE;
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer1, sntpServers[0]);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer2, sntpServers[1]);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer3, sntpServers[2]);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer4, sntpServers[3]);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer5, sntpServers[4]);
   CMSMEM_REPLACE_STRING(ntpCfg->X_BROADCOM_COM_LocalTimeZoneName, timeZoneName);

   ret = cmsObj_set(ntpCfg, &iidStack);
   cmsObj_free((void **) &ntpCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of TIME failed, ret=%d", ret);
   }

   return ret;
}

void processSntpCmd_dev2(char *cmdLine)
{
   int i = 0;
   char *pToken = strtok(cmdLine, " ");
   char *sntpServers[STNP_SERVER_MAX];
   char *timezone = NULL;
   int error = 0;
   int done = 0;
   Dev2TimeObject *ntpCfg = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;


   /* get default sntp object first: in case disable is called with no parameters */
   for (i = 0; i < STNP_SERVER_MAX; i++)
   {
      sntpServers[i] = NULL;
   }
   if ((cmsObj_get(MDMOID_DEV2_TIME, &iidStack, 0, (void **) &ntpCfg)) == CMSRET_SUCCESS)
   {
      if (ntpCfg->NTPServer1 != NULL)
      {      
         sntpServers[0] = cmsMem_strdup(ntpCfg->NTPServer1);
      }
      if (ntpCfg->NTPServer2 != NULL)
      {
         sntpServers[1] = cmsMem_strdup(ntpCfg->NTPServer2);
      }
      if (ntpCfg->NTPServer3 != NULL)
      {
         sntpServers[2] = cmsMem_strdup(ntpCfg->NTPServer3);
      }
      if (ntpCfg->NTPServer4 != NULL)
      {
         sntpServers[3] = cmsMem_strdup(ntpCfg->NTPServer4);
      }
      if (ntpCfg->NTPServer5 != NULL)
      {
         sntpServers[4] = cmsMem_strdup(ntpCfg->NTPServer5);
      }
      timezone = cmsMem_strdup(ntpCfg->X_BROADCOM_COM_LocalTimeZoneName);
   }
   

   if (pToken != NULL)
   {
      if (strcasecmp(pToken, "disable") == 0)
      {
         cliCmdSaveNeeded = TRUE;
         disableSntp_dev2();
         done = 1;
      }
      else if (strcasecmp( pToken, "date" ) == 0)
      {
         // Print the date
         time_t cur_time = time(NULL);
         printf("%s", ctime(&cur_time));
         done = 1;
      }
      else if (strcasecmp(pToken, "zones") == 0)
      {
         printf( "Timezones supported:\n" );
         for(i = 0; timeZones[i] != NULL; i++)
         {
            if (i > 0 && i % 22 == 0)
            {
               printf( "Press <enter> for more." );
               getchar();
            }
            printf("%s\n", timeZones[i]);
            done = 1;
         }
      }
      else if (strcasecmp(pToken,"-s") == 0) 
      {
         for (i = 0; i < STNP_SERVER_MAX && strcmp(pToken, "-s") == 0; i++)
         {
            pToken = strtok(NULL, " ");
            if (pToken == NULL && i == 0)
            {
               cmsLog_error("No argument to -s option.\n");
               error = 1;
               break;
            }
            else if (pToken != NULL)
            {
               CMSMEM_REPLACE_STRING(sntpServers[i], pToken);
            }
            pToken = strtok(NULL, " ");
         }
         printf("pToken after server %s\n",pToken);

         if ((!error) && (0 == strcmp(pToken, "-t")))
         {
            pToken = strtok(NULL, "\n"); // Rest of string
            if (pToken == NULL)
            {
               cmsLog_error("Missing argument to -t option.\n");
               error = 1;
            }
            else
            {
               if(pToken[0] == '"')  // Trim leading quotes
                  pToken++;
               if (pToken[strlen(pToken)-1] == '"') // Trim trailing quotes
                  pToken[strlen(pToken)-1] = '\0';
               CMSMEM_REPLACE_STRING(timezone, pToken);
            }
         }
         else
         {
            cmsLog_error("-t timezone is required\n");
            error = 1;
         } /* get default obj ok */
      } /* -s pToken != NULL */
      else 
      {
         error = 1;
      }
   } /* pToken != NULL */
   else 
   {
      error = 1;
   }
   if (!done && !error)
   {
      cliCmdSaveNeeded = TRUE;
      configureSntp_dev2(sntpServers, pToken);
   }
   if (error)
   {
      printf("\n");
      printf( "sntp -s server [ -s server2 ] -t \"timezone\"\n" );
      printf( "     disable\n");
      printf( "     date\n" );
      printf( "     zones\n" );
      printf( "sntp --help\n" );
   }
   /* clean up */
   for (i = 0; i < STNP_SERVER_MAX; i++)
   {
      if (sntpServers[i] != NULL)
      {
         cmsMem_free(sntpServers[i]);
      }
   }
   if (timezone != NULL)
   {
      cmsMem_free(timezone);
   }
} 

#endif   /* DMP_DEVICE2_TIME_1 */

#ifdef DMP_DEVICE2_DSL_1 
void DslCfgProfileUpdate_dev2(unsigned char lineId)
{
    adslCfgProfile  adslCfg;
    CmsRet          cmsRet;
    Dev2DslLineObject *dslLineObj = NULL;
    InstanceIdStack         iidStack = EMPTY_INSTANCE_ID_STACK;
    UBOOL8 found = FALSE;
    
    while (!found && (cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **) &dslLineObj) == CMSRET_SUCCESS))
    {
       if (dslLineObj->X_BROADCOM_COM_BondingLineNumber == lineId)
       {
          found = TRUE;
          break;
       }
       cmsObj_free((void **) &dslLineObj);
    }
    if (!found) {
       printf("%s: could not get DSL Line Obj line %d", __FUNCTION__, lineId);
       return;
    }
    memset((void *)&adslCfg, 0, sizeof(adslCfg));
    xdslUtil_CfgProfileInit_dev2(&adslCfg, dslLineObj);
    cmsObj_free((void **) &dslLineObj);
    
    cmsRet = xdslCtl_Configure(lineId, &adslCfg);
    if (cmsRet != CMSRET_SUCCESS)
        printf ("%s: could not configure DLS driver, ret=%d\n", __FUNCTION__, cmsRet);
    
    cmsLog_debug("DslCfgProfile is updated.");
}

void DslIntfCfgMdmUpdate_dev2(unsigned char lineId)
{
    int     nRet;
    long    dataLen;
    char    oidStr[] = { 95 };      /* kOidAdslPhyCfg */
    adslCfgProfile  adslCfg;
    CmsRet          cmsRet;
    Dev2DslLineObject *dslLineObj = NULL;
    InstanceIdStack         iidStack = EMPTY_INSTANCE_ID_STACK;
    UBOOL8 found = FALSE;

    dataLen = sizeof(adslCfgProfile);
    nRet = xdslCtl_GetObjectValue(lineId, oidStr, sizeof(oidStr), (char *)&adslCfg, &dataLen);
    
    if( nRet != BCMADSL_STATUS_SUCCESS) {
        printf("%s: could not get adsCfg, ret=%d", __FUNCTION__, nRet);
        return;
    }

    while (!found && (cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **) &dslLineObj) == CMSRET_SUCCESS))
    {
       if (dslLineObj->X_BROADCOM_COM_BondingLineNumber == lineId)
       {
          found = TRUE;
          break;
       }
       cmsObj_free((void **) &dslLineObj);
    }
    if (!found)
    {    
       printf("%s: could not get DSL Line Obj for line %d", __FUNCTION__, lineId);
       return;
    }
    
    xdslUtil_IntfCfgInit_dev2(&adslCfg, dslLineObj);
    
    cmsRet = cmsObj_set(dslLineObj, &iidStack);
    if (cmsRet != CMSRET_SUCCESS)
    {
       printf("%s: could not set DSL Line Obj for line %d, ret=%d", __FUNCTION__, lineId,cmsRet);
    }
    cmsObj_free((void **) &dslLineObj);
    
    cmsLog_debug("DslIntfCfgMdm is updated.");
}
#endif /* DMP_DEVICE2_DSL_1 */


#ifdef  DMP_DEVICE2_PPPINTERFACE_1
#ifdef  DMP_DEVICE2_PPPINTERFACE_2
UBOOL8 isPPPExist_dev2(const char *pppName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2PppInterfaceObject *pppIntf = NULL;
   UBOOL8 found = FALSE;

   while (!found && 
       cmsObj_getNext(MDMOID_DEV2_PPP_INTERFACE, &iidStack, (void **)&pppIntf) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(pppIntf->name, pppName))
      {
         found = TRUE;
      }
      cmsObj_free((void **)&pppIntf);
   }

   return found;
   
}
#endif   /* DMP_DEVICE2_PPPINTERFACE_2 */
#endif   /* DMP_DEVICE2_PPPINTERFACE_1 */


#ifdef DMP_DEVICE2_NAT_1

void virtualServerShow_dev2(void)
{
   UINT32 index = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortMappingObject *portMappingObj = NULL;

   while (cmsObj_getNextFlags(MDMOID_DEV2_NAT_PORT_MAPPING,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &portMappingObj) == CMSRET_SUCCESS) 
   {
      printf("[%2d] enabled=%d %s %d-%d\n",
             index,
             portMappingObj->enable,
             portMappingObj->description,
             portMappingObj->externalPort,
             portMappingObj->externalPortEndRange);

      cmsObj_free((void **) &portMappingObj);
      index++;
   }

   if (index == 0)
   {
      printf("There are no virtual server entries to show.\n");
   }
}


void virtualServerEnableDisable_dev2(UBOOL8 doEnable, UINT32 entryNum)
{
   UBOOL8 found = FALSE;
   UINT32 index = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortMappingObject *portMappingObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_NAT_PORT_MAPPING,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &portMappingObj) == CMSRET_SUCCESS) 
   {
      if (index == entryNum)
      {
         found = TRUE;

         if (portMappingObj->enable != doEnable)
         {
            portMappingObj->enable = doEnable;

            ret = cmsObj_set(portMappingObj, &iidStack);

            if (ret != CMSRET_SUCCESS)
            {
               printf("Set of entry %d to enable=%d failed, ret=%d.\n",
                      entryNum, doEnable, ret);
            }
            else
            {
               printf("Set of entry %d to enable=%d successfully.\n",
                      entryNum, doEnable);
            }
         }
         else
         {
            printf("Entry %d is already enable=%d.\n", entryNum, doEnable);
         }
      }

      cmsObj_free((void **) &portMappingObj);
      index++;
   }

   if (found == FALSE)
   {
      printf("Could not find virtualserver entry %d.\n", entryNum);
   }
}

#endif   /* DMP_DEVICE2_NAT_1 */


#endif   /* SUPPORT_CLI_CMD */

#endif   /* DMP_DEVICE2_BASELINE_1 */

