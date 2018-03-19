/***********************************************************************
<:copyright-BRCM:2012:proprietary:standard 

   Copyright (c) 2012 Broadcom 
   All Rights Reserved

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

/** command driven CLI code goes into this file */

#ifdef SUPPORT_CLI_CMD
#ifdef SUPPORT_DEBUG_TOOLS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// should be in string.h, but including it did not fix compiler warning
// so I just declare the function here.
extern char *strcasestr(const char *haystack, const char *needle);
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_data_model_selector.h"

#include "cli.h"

#ifdef WIRELESS
#include <wlcsm_lib_api.h>
#endif

void processLogLevelCmd(char *cmdLine)
{
   char *getfailed="get failed, ret=";
   char *currlog="current log level is";
   char *setfailed="set failed, ret=";
   char *setsuccess="new log level set.";

   char *ptr;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 getMode;
   CmsRet ret;

   cmsLog_debug("cmdLine is ->%s<-", cmdLine);

   if (!strncasecmp(cmdLine, "help", 4) || !strncasecmp(cmdLine, "--help", 6))
   {
      printf("usage: loglevel get appname\n");
      printf("       loglevel set appname loglevel\n");


      printf("where appname is one of: httpd, tr69c, smd, ssk, telnetd, sshd, consoled, upnp, dnsproxy");

#if defined(SUPPORT_IGMP) || defined(SUPPORT_MLD)
      printf(", mcpd");
#endif

#ifdef BRCM_VOICE_SUPPORT
      printf(", voice");
#  ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1
      printf(", dectd");
#  endif
#endif

#ifdef DMP_X_BROADCOM_COM_SNMP_1
      printf(", snmpd");
#endif

#ifdef BRCM_WLAN
      printf(", wlmngr");
#endif

#ifdef SUPPORT_OSGI_FELIX
      printf(", osgid");
#endif

#ifdef DMP_DEVICE2_SM_BASELINE_1
      printf(", linmosd");
      printf(", pmd");
      printf(", cwmpd");
      printf(", dad");
#endif

#ifdef DMP_X_BROADCOM_COM_EPON_1
      printf(", eponapp");
#endif

#ifdef DMP_X_BROADCOM_COM_GPON_1
      printf(", omcid");
#endif

#ifdef SUPPORT_CELLULAR
      printf(", cellularapp");
#endif

      printf("\n");
      printf("loglevel is one of \"Error\", \"Notice\", or \"Debug\" (use these exact strings).\n");
      return;
   }

   getMode = (strncasecmp(cmdLine, "get", 3) == 0);

   ptr = &(cmdLine[4]);
   if (!strncasecmp(ptr, "httpd", 5))
   {
      HttpdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_HTTPD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "tr69c", 5))
   {
      Tr69cCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_TR69C_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#ifdef SUPPORT_TR64C
   else if (!strncasecmp(ptr, "tr64c", 5))
   {
      Tr64cCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_TR64C_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif
#ifdef SUPPORT_VECTORINGD
   else if (!strncasecmp(ptr, "vectoringd", 10))
   {
      VectoringCfgObject *obj;

      if ((ret = cmsObj_get(MDMOID_VECTORING_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
        printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[11]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif
   else if (!strncasecmp(ptr, "smd", 3))
   {
      SmdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_SMD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[4]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "ssk", 3))
   {
      SskCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_SSK_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[4]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "sshd", 4))
   {
      SshdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_SSHD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[5]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "telnetd", 7))
   {
      TelnetdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_TELNETD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[8]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "consoled", 8))
   {
      ConsoledCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_CONSOLED_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[9]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "upnp", 4))
   {
      UpnpCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_UPNP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[5]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "dnsproxy", 8))
   {
      DnsProxyCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_DNS_PROXY_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[9]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#if defined(SUPPORT_IGMP) || defined(SUPPORT_MLD)
   else if (!strncasecmp(ptr, "mcpd", 4))
   {
      CmsMsgHeader msg = EMPTY_MSG_HEADER;
      CmsLogLevel logLevel;
      UBOOL8 valid=TRUE;

      /*
       * Instead of setting a data model object, which is the normal way,
       * we do a simplified method here by sending a message directly to
       * mcpd.  The disadvantage of this simplified approach is the
       * setting is not saved across reboots.
       */

      msg.type = CMS_MSG_SET_LOG_LEVEL;
      msg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
      msg.dst = EID_MCPD;
      msg.flags_request = 1;

      if (!strncmp(&(ptr[5]), "Error", 5))
      {
         logLevel = LOG_LEVEL_ERR;
      }
      else if (!strncmp(&(ptr[5]), "Notice", 6))
      {
         logLevel = LOG_LEVEL_NOTICE;
      }
      else if (!strncmp(&(ptr[5]), "Debug", 5))
      {
         logLevel = LOG_LEVEL_DEBUG;
      }
      else
      {
         printf("Loglevel must be Debug, Notice, or Error\n");
         valid = FALSE;
      }

      if (valid)
      {
         msg.wordData = logLevel;

         ret = cmsMsg_sendAndGetReplyWithTimeout(cliPrvtMsgHandle, &msg, 5000);
         if (ret != CMSRET_SUCCESS)
         {
            printf("Could not send set log level msg to mcpd, ret=%d", ret);
         }
      }
   }
#endif
#ifdef BRCM_VOICE_SUPPORT
   else if (!strncasecmp(ptr, "voice", 5))
   {
      VoiceObject *obj=NULL;

      /* Only single voiceservice is supported so return first one found */
      if ((ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->X_BROADCOM_COM_LoggingLevel);
         }
         else
         {
            cmsMem_free(obj->X_BROADCOM_COM_LoggingLevel);
            obj->X_BROADCOM_COM_LoggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1  /* aka dectd */
   else if (!strncasecmp(ptr, "dectd", 5))
   {
      DectdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_DECTD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
#endif /* BRCM_VOICE_SUPPORT */

#ifdef DMP_X_BROADCOM_COM_SNMP_1
   else if (!strncasecmp(ptr, "snmpd", 5))
   {
      SnmpdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_SNMPD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif  /* DMP_X_BROADCOM_COM_SNMP_1 */
#ifdef BRCM_WLAN
   else if (!strncasecmp(ptr, "wlmngr", 6))
   {
      /* wlmngr does not have loglevel param or object yet, so just send msg */
      if (getMode)
      {
         printf("loglevel get not supported for wlmngr yet\n");
      }
      else
      {
#if defined(SUPPORT_DM_DETECT)
        if(cmsMdm_isDataModelDevice2()) {
#endif
#if defined(SUPPORT_TR181_WLMNGR)
                wlcsm_mngr_dm_set_dbglevel(cmsUtl_logLevelStringToEnum(&(ptr[7])));
#endif
#if defined(SUPPORT_DM_DETECT)
          } else 
#endif
#if !defined(SUPPORT_TR181_WLMNGR) || defined(SUPPORT_DM_DETECT)
         {
         CmsMsgHeader msg = EMPTY_MSG_HEADER;
         CmsRet r2;

         msg.type = CMS_MSG_SET_LOG_LEVEL;
         msg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
         msg.dst = EID_WLMNGR;
         msg.flags_request = 1;
         msg.flags_bounceIfNotRunning = 1;
         msg.wordData = cmsUtl_logLevelStringToEnum(&(ptr[7]));
         r2 = cmsMsg_sendAndGetReplyWithTimeout(cliPrvtMsgHandle, &msg, CMSLCK_MAX_HOLDTIME);
         if (r2 != CMSRET_SUCCESS && r2 != CMSRET_MSG_BOUNCED)
         {
            cmsLog_error("update log level failed, ret=%d", r2);
         }
         else
         {
            printf("%s\n", setsuccess);
         }
         }
#endif
      }
   }
#endif /* BRCM_WLAN */
#ifdef DMP_X_ITU_ORG_GPON_1
   else if (!strncasecmp(ptr, "omcid", 5))
   {
      OmcidCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_OMCID_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_X_ITU_ORG_GPON_1 */
#ifdef SUPPORT_OSGI_FELIX
   else if (!strncasecmp(ptr, "osgid", 5))
   {
      OsgidCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_OSGID_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* SUPPORT_OSGI_FELIX */

#ifdef DMP_DEVICE2_SM_BASELINE_1
   else if (!strncasecmp(ptr, "linmosd", 7))
   {
      LinmosdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_LINMOSD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[8]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_DEVICE2_SM_BASELINE_1 */

#ifdef DMP_DEVICE2_SM_BASELINE_1
   else if (!strncasecmp(ptr, "spd", 3))
   {
      SpdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_SPD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[4]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_DEVICE2_SM_BASELINE_1 */

#ifdef DMP_DEVICE2_SM_BASELINE_1
   else if (!strncasecmp(ptr, "pmd", 3))
   {
      PmdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_PMD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[4]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_DEVICE2_SM_BASELINE_1 */

#ifdef DMP_DEVICE2_SM_BASELINE_1
   else if (!strncasecmp(ptr, "cwmpd", 5))
   {
      CwmpdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_CWMPD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_DEVICE2_SM_BASELINE_1 */

#ifdef DMP_DEVICE2_SM_BASELINE_1
   else if (!strncasecmp(ptr, "dad", 3))
   {
      DadCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_DAD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[4]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_DEVICE2_SM_BASELINE_1 */

#ifdef DMP_DEVICE2_SM_BASELINE_1
   else if (!strncasecmp(ptr, "dmad", 4))
   {
      DmadCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_DMAD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[5]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_DEVICE2_SM_BASELINE_1 */

#ifdef DMP_DEVICE2_SM_BASELINE_1
   else if (!strncasecmp(ptr, "uspd", 4))
   {
      UspdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_USPD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[5]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_DEVICE2_SM_BASELINE_1 */


#ifdef DMP_X_BROADCOM_COM_EPON_1
   else if (!strncasecmp(ptr, "eponapp", 7))
   {
      EponappCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_EPONAPP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[8]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif
#ifdef SUPPORT_XMPP
   else if (!strncasecmp(ptr, "xmppc", 5))
   {
      XmppcCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_XMPPC_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif  /* SUPPORT_XMPP */
#ifdef SUPPORT_CELLULAR
   else if (!strncasecmp(ptr, "cellularapp", 11))
   {
      CellularappCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_CELLULARAPP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingLevel);
         }
         else
         {
            cmsMem_free(obj->loggingLevel);
            obj->loggingLevel = cmsMem_strdup(&(ptr[12]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }
         cmsObj_free((void **) &obj);
      }
   }
#endif
   else
   {
      printf("invalid or unsupported app name %s\r\n", ptr);
   }


   return;
}


void processLogDestCmd(char *cmdLine)
{
   char *getfailed="get failed, ret=";
   char *currlog="current log dest is";
   char *setfailed="set failed, ret=";
   char *setsuccess="new log dest set.";

   char *ptr;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 getMode;
   CmsRet ret;

   if (!strncasecmp(cmdLine, "help", 4) || !strncasecmp(cmdLine, "--help", 6))
   {
      printf("usage: logdest get appname\r\n");
      printf("       logdest set appname logdest\r\n");
      printf("where appname is one of: httpd, tr69c, smd, ssk, telnetd, sshd, consoled, upnp, dnsproxy");
#ifdef SUPPORT_OSGI_FELIX
      printf(", osgid");
#endif

#ifdef DMP_DEVICE2_SM_BASELINE_1
      printf(", linmosd");
      printf(", spd");
      printf(", pmd");
      printf(", cwmpd");
      printf(", dad");
      printf(", dmad");
#endif

#ifdef DMP_X_BROADCOM_COM_EPON_1
      printf(", eponapp");
#endif

#ifdef DMP_X_BROADCOM_COM_GPON_1
      printf(", omcid");
#endif

#ifdef SUPPORT_XMPP
      printf(", xmppc");
#endif

      printf("\r\n");
      printf("loglevel is \"Standard Error\", \"Syslog\" or \"Telnet\".\r\n");
      return;
   }

   getMode = (strncasecmp(cmdLine, "get", 3) == 0);

   ptr = &(cmdLine[4]);
   if (!strncasecmp(ptr, "httpd", 5))
   {
      HttpdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_HTTPD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "tr69c", 5))
   {
      Tr69cCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_TR69C_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#ifdef SUPPORT_TR64C
   else if (!strncasecmp(ptr, "tr64c", 5))
   {
      Tr64cCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_TR64C_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif
   else if (!strncasecmp(ptr, "smd", 3))
   {
      SmdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_SMD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[4]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "ssk", 3))
   {
      SskCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_SSK_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[4]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "sshd", 4))
   {
      SshdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_SSHD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[5]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "telnetd", 7))
   {
      TelnetdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_TELNETD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[8]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "consoled", 8))
   {
      ConsoledCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_CONSOLED_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[9]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "upnp", 4))
   {
      UpnpCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_UPNP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[5]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
   else if (!strncasecmp(ptr, "dnsproxy", 8))
   {
      DnsProxyCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_DNS_PROXY_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[9]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }

#ifdef SUPPORT_OSGI_FELIX
   if (!strncasecmp(ptr, "osgid", 5))
   {
      OsgidCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_OSGID_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* SUPPORT_OSGI_FELIX */

#ifdef DMP_DEVICE2_SM_BASELINE_1
if (!strncasecmp(ptr, "linmosd", 7))
   {
      LinmosdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_LINMOSD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[8]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_DEVICE2_SM_BASELINE_1 */

#ifdef DMP_DEVICE2_SM_BASELINE_1
if (!strncasecmp(ptr, "pmd", 3))
   {
      PmdCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_PMD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[4]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_DEVICE2_SM_BASELINE_1 */

#ifdef DMP_X_BROADCOM_COM_EPON_1
   else if (!strncasecmp(ptr, "eponapp", 7))
   {
      EponappCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_EPONAPP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[8]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif
#ifdef DMP_X_BROADCOM_COM_GPON_1
   else if (!strncasecmp(ptr, "omcid", 5))
   {
      EponappCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_OMCID_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* DMP_X_BROADCOM_COM_GPON_1 */
#ifdef SUPPORT_XMPP
   if (!strncasecmp(ptr, "xmppc", 5))
   {
      XmppcCfgObject *obj=NULL;

      if ((ret = cmsObj_get(MDMOID_XMPPC_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
      {
         printf("%s %d\n", getfailed, ret);
      }
      else
      {
         if (getMode)
         {
            printf("%s %s\n", currlog, obj->loggingDestination);
         }
         else
         {
            cmsMem_free(obj->loggingDestination);
            obj->loggingDestination = cmsMem_strdup(&(ptr[6]));
            if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
               printf("%s %d\n", setfailed, ret);
            }
            else
            {
               printf("%s\n", setsuccess);
            }
         }

         cmsObj_free((void **) &obj);
      }
   }
#endif /* SUPPORT_XMPP */
   else
   {
      printf("invalid or unsupported app name %s\r\n", ptr);
   }

   return;
}


#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1
void processDnsproxyCmd(char *cmdLine)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret;


   if (!strcasecmp(cmdLine, "dumpstatus"))
   {
      msg.type = CMS_MSG_DNSPROXY_DUMP_STATUS;
   }
   else if (!strcasecmp(cmdLine, "dumpstats"))
   {
      msg.type = CMS_MSG_DNSPROXY_DUMP_STATS;
   }
   else
   {
      printf("usage: dnsproxy [dumpstatus | dumpstats]\n");
      printf("one of dumpstatus or dumpstats must be specified.\n");
      return;
   }

   msg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg.dst = EID_DNSPROXY;
   msg.flags_event = 1;
   msg.wordData = 0;
   msg.dataLength = 0;

   if ((ret = cmsMsg_send(cliPrvtMsgHandle, &msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d", ret);
   }

}
#endif  /* DMP_X_BROADCOM_COM_DNSPROXY_1 */


void processDumpCfgCmd(char *cmdLine)
{
   char *cfgBuf;
   UINT32 origCfgBufLen, cfgBufLen;
   CmsRet ret;
   UBOOL8 readFromFlash = TRUE;

   if (!strcasecmp(cmdLine, "help") || !strcasecmp(cmdLine, "-h") || !strcasecmp(cmdLine, "--help"))
   {
      printf("usage: dumpcfg [dynamic]\n");
      printf("by default, dump contents of config flash.\n");
      printf("dynamic: dump what would get written to config flash from MDM.\n");
      return;
   }

   if (strlen(cmdLine) > 0)
   {
      if (!strcasecmp(cmdLine, "dynamic"))
      {
         readFromFlash = FALSE;
      }
      else
      {
         printf("invalid arguments");
         return;
      }
   }


   origCfgBufLen = cfgBufLen = cmsImg_getConfigFlashSize();
   if (cfgBufLen == 0)
   {
      cmsLog_error("Could not get config flash size");
      return;
   }
   else
   {
      cmsLog_debug("configBufLen=%d", cfgBufLen);
   }

   cfgBuf = cmsMem_alloc(cfgBufLen, 0);
   if (cfgBuf == NULL)
   {
      cmsLog_error("malloc of %d bytes failed", cfgBuf);
      return;
   }


   if (readFromFlash)
   {
      ret = cmsMgm_readConfigFlashToBuf(cfgBuf, &cfgBufLen);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("read config failed, ret=%d cfgBufLen=%d", ret, cfgBufLen);
      }
      else if (cfgBufLen <= 1)
      {
         printf("config flash empty.\n");
      }
      else
      {
         printf("%s", cfgBuf);
         printf("dump bytes allocated=%d used=%d", origCfgBufLen, cfgBufLen);
      }
   }
   else
   {
      /* dump the current MDM into the memory buffer and print that out. */
      ret = cmsMgm_writeConfigToBuf(cfgBuf, &cfgBufLen);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("read config failed, ret=%d cfgBufLen=%d", ret, cfgBufLen);
      }
      else
      {
         printf("%s", cfgBuf);
         printf("dump bytes allocated=%d used=%d", origCfgBufLen, cfgBufLen);
      }
   }


   cmsMem_free(cfgBuf);

   return;
}


static void cmdMemInfoHelp(void)
{

   printf("usage: meminfo [app name] [operation]\n");
   printf("  App name can be httpd, tr69c, or ssk.  If app name is omitted, then the operation is done for CLI app.\n");
   printf("  operation is one of stats, traceAll, trace50, traceClones.  If operation is omitted, then stats.\n");
   printf("Examples:\n");
   printf("  meminfo : dumps the memory stats as seen by the CLI app.  Same as meminfo self stats.\n");
   printf("  meminfo ssk : send a message to ssk to tell it to dump its memory stats.  Same as meminfo ssk stats.\n");
   printf("  meminfo httpd trace50 : send a message to httpd to tell it to dump last 50 leak tracing records\n");
   printf("  meminfo traceClones : tell this CLI app to dump leak trace records with 5 or more clones.\n\n");

   return;
}

void processMeminfoCmd(char *cmdLine)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;

   msg.dst = EID_INVALID;
   msg.type = CMS_MSG_MEM_DUMP_STATS;  /* default op */

   if (!strcasecmp(cmdLine, "help") || !strcasecmp(cmdLine, "-h") || !strcasecmp(cmdLine, "--help"))
   {
      cmdMemInfoHelp();
   }

   if (!strncasecmp(cmdLine, "httpd", 5))
   {
      msg.dst = EID_HTTPD;
   }
   else if (!strncasecmp(cmdLine, "ssk", 3))
   {
      msg.dst = EID_SSK;
   }
   else if (!strncasecmp(cmdLine, "tr69c", 5))
   {
      msg.dst = EID_TR69C;
   }
   else if (!strncasecmp(cmdLine, "omcid", 5))
   {
      msg.dst = EID_OMCID;
   }

   if (strcasestr(cmdLine, "traceall"))
   {
      msg.type = CMS_MSG_MEM_DUMP_TRACEALL;
   }
   else if (strcasestr(cmdLine, "trace50"))
   {
      msg.type = CMS_MSG_MEM_DUMP_TRACE50;
   }
   else if (strcasestr(cmdLine, "traceClones"))
   {
      msg.type = CMS_MSG_MEM_DUMP_TRACECLONES;
   }

   if (msg.dst == EID_INVALID)
   {
      /* Run operation for this app */
      if (msg.type == CMS_MSG_MEM_DUMP_STATS)
      {
         cmsMem_dumpMemStats();
      }

#ifdef CMS_MEM_LEAK_TRACING
      else if (msg.type == CMS_MSG_MEM_DUMP_TRACEALL)
      {
         cmsMem_dumpTraceAll();
      }
      else if (msg.type == CMS_MSG_MEM_DUMP_TRACE50)
      {
         cmsMem_dumpTrace50();
      }
      else if (msg.type == CMS_MSG_MEM_DUMP_TRACECLONES)
      {
         cmsMem_dumpTraceClones();
      }
#endif /* CMS_MEM_LEAK_TRACING */
   }
   else
   {
      CmsRet ret;
      const CmsEntityInfo *eInfo = cmsEid_getEntityInfo(msg.dst);

      printf("sending msg 0x%x to %s\n", msg.type, eInfo->name);
      /* send the message to another app */
      msg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
      msg.flags_event = 1;
      if ((ret = cmsMsg_send(cliPrvtMsgHandle, &msg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("msg send failed, ret=%d", ret);
      }
   }

   return;
}


void processDumpMdmCmd(char *cmdLine)
{
   char *cfgBuf;
   UINT32 origCfgBufLen, cfgBufLen;
   UINT32 shmBytesInUse;
   CmsMemStats memStats;
   CmsRet ret;

   if (!strcasecmp(cmdLine, "help") || !strcasecmp(cmdLine, "-h") || !strcasecmp(cmdLine, "--help"))
   {
      printf("usage: dumpmdm\n");
      printf("dump entire contents of the MDM, this is not what would be written to the config flash.\n");
      return;
   }


   /*
    * The size of the buffer to hold the MDM is directly proportional to the amount
    * of shared memory in use.  But since the data in the MDM is more compact than
    * when it is dumped out, use a multipling factor of 3.
    */
   cmsMem_getStats(&memStats);
   shmBytesInUse = (memStats.shmAllocStart - MDM_SHM_ATTACH_ADDR) + memStats.shmBytesAllocd;

   origCfgBufLen = (shmBytesInUse * 3);
   cfgBufLen = origCfgBufLen;

   if ((cfgBuf = cmsMem_alloc(cfgBufLen, 0)) == NULL)
   {
      cmsLog_error("malloc of %d bytes failed", cfgBufLen);
      return;
   }

   /* dump the current MDM into the memory buffer and print that out. */
   ret = cmsMgm_writeMdmToBuf(cfgBuf, &cfgBufLen);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("read config failed, ret=%d allocated=%d cfgBufLen=%d", ret, origCfgBufLen, cfgBufLen);
   }
   else
   {
      printf("%s", cfgBuf);
      printf("dump bytes allocated=%d used=%d", origCfgBufLen, cfgBufLen);
   }

   cmsMem_free(cfgBuf);

#ifdef DMP_X_ITU_ORG_GPON_1
   /* Dump OMCI MIB. */ 
   {
      CmsMsgHeader cmdMsg;

      memset(&cmdMsg, 0, sizeof(CmsMsgHeader));
      cmdMsg.type = CMS_MSG_OMCI_DUMP_INFO_REQ;
      cmdMsg.src = EID_CONSOLED;
      cmdMsg.dst = EID_OMCID;
      cmdMsg.flags_request = 1;
      cmdMsg.dataLength = 0;
      cmdMsg.wordData = (UINT32)0;

      ret = cmsMsg_send(cliPrvtMsgHandle, (CmsMsgHeader*)&cmdMsg);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Send CMS_MSG_OMCI_DUMP_INFO_REQ failed, ret=%d", ret);
      }
   }
#endif
}


#define SETPV_MAX_NAME_VALUE_PAIRS  30

static void freeValueArray(char ***valueArray)
{
   UINT32 i;

   if (valueArray == NULL)
   {
      return;
   }

   /* free all the individual buffers in the array first */
   for (i=0; i < SETPV_MAX_NAME_VALUE_PAIRS; i++)
   {
      cmsMem_free((*valueArray)[i]);
   }

   /* free the array itself (an array of char *) */
   CMSMEM_FREE_BUF_AND_NULL_PTR(*valueArray);

   return;
}

/** Parse/convert string of up to SETPV_MAX_NAME_VALUE_PAIRS name/value pairs
 *  into an array of MdmPathDescriptors and array of char *.
 *
 *  @param (IN) cmdLine the string of name/value pairs
 *  @param (OUT) an array of MdmPathDescriptors.  This function will allocate
 *               an array of SETPV_MAX_NAME_VALUE_PAIRS MdmPathDescriptors.
 *               The caller is responsible for freeing it.
 *  @param (OUT) an array of values.  This function will allocate an array
 *               of SETPV_MAX_NAME_VALUE_PAIRS char *, each char * pointing
 *               to another allocated buffer big enough to hold the value.
 *               Caller is responsible for freeing.
 *
 *  @param (OUT) -1 on error, otherwise the number of name value pairs returned.
 */
static SINT32 getFullPathAndValuePairs(const char *cmdLine,
                                       MdmPathDescriptor **pathDescArray,
                                       char ***valueArray)
{
   MdmPathDescriptor *pdArray;
   char **valArray;
   UINT32 pdIdx=0;
   UINT32 valIdx=0;
   UINT32 i=0;


   pdArray = cmsMem_alloc(SETPV_MAX_NAME_VALUE_PAIRS * sizeof(MdmPathDescriptor), ALLOC_ZEROIZE);
   if (pdArray == NULL)
   {
      cmsLog_error("Could not allocate memory for MdmPathDescriptors");
      return -1;
   }

   valArray = cmsMem_alloc(SETPV_MAX_NAME_VALUE_PAIRS * sizeof(char *), ALLOC_ZEROIZE);
   if (valArray == NULL)
   {
      cmsLog_error("Could not allocate memory for char buf array");
      cmsMem_free(pdArray);
      return -1;
   }

   /* this is the main parsing body */
   while (cmdLine[i] != '\0')
   {
      char fullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN];
      char valueBuf[MDM_SINGLE_FULLPATH_BUFLEN];
      UINT32 j;
      CmsRet ret;

      /* first get the parameterName fullpath */
      memset(fullPathBuf, 0, sizeof(fullPathBuf));
      j = 0;
      while (cmdLine[i] != '\0' && cmdLine[i] != ' ')
      {
         fullPathBuf[j++] = cmdLine[i++];
      }

      /* convert parameterName fullpath to MdmPathDescriptor */
      ret = cmsMdm_fullPathToPathDescriptor(fullPathBuf, &(pdArray[pdIdx]));
      if (ret != CMSRET_SUCCESS)
      {
         printf("At %d, invalid fullpath %s\n", pdIdx, fullPathBuf);
         cmsMem_free(pdArray);
         freeValueArray(&valArray);
         return -1;
      }

      if (pdArray[pdIdx].paramName[0] == '\0')
      {
         printf("At %d, The fullpath must specify a parameter (not object) %s\n",
                pdIdx, fullPathBuf);
         cmsMem_free(pdArray);
         freeValueArray(&valArray);
         return -1;
      }

      pdIdx++;

      // skip over any white spaces after the paramName
      while (cmdLine[i] == ' ') { i++; }

      /* now get the value.  Currently, a space is not allowed inside the
       * value.  In the future, we could support spaces in the value if it
       * is enclosed inside double quotes, e.g. "hello there"
       */
      memset(valueBuf, 0, sizeof(valueBuf));
      j = 0;
      while (cmdLine[i] != '\0' && cmdLine[i] != ' ')
      {
         valueBuf[j++] = cmdLine[i++];
      }

      if (strlen(valueBuf) > 0)
      {
         /* special case check: convert "" to empty string */
         if (valueBuf[0] == '"' &&
             valueBuf[1] == '"' &&
             valueBuf[2] == '\0')
         {
            valueBuf[0] = '\0';
         }

         valArray[valIdx] = cmsMem_strdup(valueBuf);
         if (valArray[valIdx] == NULL)
         {
            cmsLog_error("strdup failed for %s", valueBuf);
            cmsMem_free(pdArray);
            freeValueArray(&valArray);
            return -1;
         }
      }

      valIdx++;

      // skip over any white spaces after the value
      while (cmdLine[i] == ' ') { i++; }
   }

   if (pdIdx != valIdx)
   {
      printf("Mis-match between number of parameter names (%d) and values (%d)\n", pdIdx, valIdx);
      cmsMem_free(pdArray);
      freeValueArray(&valArray);
      return -1;
   }

   if (pdIdx == 0)
   {
      printf("No parameter name or value detected\n");
      cmsMem_free(pdArray);
      freeValueArray(&valArray);
      return -1;
   }

   /* good return */
   *pathDescArray = pdArray;
   *valueArray = valArray;
   return pdIdx;
}


static void getFullPathAndValue(const char *cmdLine, char *fullPathBuf, char *valueBuf)
{
   int i=0;
   int j=0;

   /* copy fullpath to our local buf */
   while (cmdLine[i] != '\0' && cmdLine[i] != ' ')
   {
      fullPathBuf[j++] = cmdLine[i++];
   }

   if (valueBuf == NULL || cmdLine[i] == '\0')
   {
      return;
   }

   /* skip over space and get the value */
   i++;
   j=0;
   while (cmdLine[i] != '\0' && cmdLine[i] != ' ')
   {
      valueBuf[j++] = cmdLine[i++];
   }

   return;
}


void processMdmCmd(char *cmdLine)
{

   if (!strcasecmp(cmdLine, "help") || !strcasecmp(cmdLine, "-h") || !strcasecmp(cmdLine, "--help"))
   {
      printf("usage: mdm setpv <full path to parameter value> <param value> [plus %d more name/value pairs]\n",
                     SETPV_MAX_NAME_VALUE_PAIRS - 1);
      printf("       mdm getpv <full path to parameter value>\n");
      printf("       mdm addobj <full path to object>\n");
      printf("       mdm delobj <full path to object instance>\n");
      printf("       mdm setnonpersistent <full path to object instance>\n");
      printf("more subcommands will be added later.\n");
      return;
   }

   if (!strncasecmp(cmdLine, "setpv", 5))
   {
      MdmPathDescriptor *pathDescArray=NULL;
      char **valueArray=NULL;
      SINT32 num;

      num = getFullPathAndValuePairs(&cmdLine[6], &pathDescArray, &valueArray);
      if (num <= 0)
      {
         return;
      }

      /* fill CMS PHL structure and use CMS PHL API to send into MDM */
      {
         PhlSetParamValue_t *setParamValueList=NULL;
         PhlGetParamValue_t *getParamValueList=NULL;
         SINT32 numGetParamValues;
         CmsRet ret;

         /* we have to do a getParameterValues first to get the type of each param */
         ret = cmsPhl_getParameterValues(pathDescArray, num, TRUE,
                                         &getParamValueList,
                                         &numGetParamValues);
         if (ret != CMSRET_SUCCESS)
         {
            printf("Could not get param value, ret=%d\n", ret);
         }
         else if (numGetParamValues != num)
         {
            cmsLog_error("mismatch between numGetParamValues (%d) and num (%d)",
                         numGetParamValues, num);
         }

         if (ret == CMSRET_SUCCESS)
         {
            setParamValueList = cmsMem_alloc(num * sizeof(PhlSetParamValue_t), ALLOC_ZEROIZE);
            if (setParamValueList == NULL)
            {
               cmsLog_error("alloc of setParamValue list failed");
            }
            else
            {
               SINT32 setIdx=0;

               /* fill in each setParamValue element */
               while (setIdx < num)
               {
                  /* paramType doesn't need to be freed, pointing to a constant */
                  setParamValueList[setIdx].pParamType = (char *) getParamValueList[setIdx].pParamType;
                  setParamValueList[setIdx].pValue = valueArray[setIdx];
                  setParamValueList[setIdx].pathDesc = pathDescArray[setIdx];
                  setParamValueList[setIdx].status = CMSRET_SUCCESS;
                  setIdx++;
               }

               ret = cmsPhl_setParameterValues(setParamValueList, num);
               if (ret != CMSRET_SUCCESS)
               {
                  printf("set failed, ret=%d\n", ret);
               }
               else
               {
                  setIdx=0;
                  while (setIdx < num)
                  {
                     printf("[%2d] Set new value=%s\n", setIdx, valueArray[setIdx]);
                     printf("        Previous value=%s\n", getParamValueList[setIdx].pValue);
                     printf("        (Param=%s)\n", setParamValueList[setIdx].pathDesc.paramName);
                     printf("        (Type=%s)\n\n", getParamValueList[setIdx].pParamType);
                     setIdx++;
                  }
               }

               CMSMEM_FREE_BUF_AND_NULL_PTR(setParamValueList);
            }

            cmsPhl_freeGetParamValueBuf(getParamValueList, numGetParamValues);
         }

         freeValueArray(&valueArray);
         CMSMEM_FREE_BUF_AND_NULL_PTR(pathDescArray);
      }
   }
   else if (!strncasecmp(cmdLine, "getpv", 5))
   {
      char fullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};

      getFullPathAndValue(&cmdLine[6], fullPathBuf, NULL);

      if (fullPathBuf[0] == '\0')
      {
         printf("a fullpath must be specified.\n");
         return;
      }

      /* fill CMS PHL structure and use CMS PHL API to send into MDM */
      {
         PhlGetParamValue_t *getParamValueList=NULL;
         SINT32 numGetParamValues;
         MdmPathDescriptor pathDesc;
         CmsRet ret;

         memset(&pathDesc, 0, sizeof(pathDesc));
         ret = cmsMdm_fullPathToPathDescriptor(fullPathBuf, &pathDesc);
         if (ret != CMSRET_SUCCESS)
         {
            printf("Invalid fullpath %s\n", fullPathBuf);
            return;
         }

         if (pathDesc.paramName[0] == '\0')
         {
            printf("The fullpath must specify a parameter (not object)\n");
            return;
         }

         ret = cmsPhl_getParameterValues(&pathDesc, 1, TRUE,
                     &getParamValueList, &numGetParamValues);
         if (ret != CMSRET_SUCCESS)
         {
            printf("Could not get param value, ret=%d\n", ret);
         }
         else if (numGetParamValues != 1)
         {
            cmsLog_error("strange... got NumValues=%d, expected 1",
                         numGetParamValues);
         }

         printf("Param value=%s\n", getParamValueList->pValue);
         printf("(Param=%s)\n", fullPathBuf);
         printf("(Type=%s)\n\n", getParamValueList->pParamType);

         cmsPhl_freeGetParamValueBuf(getParamValueList, numGetParamValues);
      }
   }
   else if (!strncasecmp(cmdLine, "addobj", 6))
   {
      char fullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};
      char *strPtr=NULL;
      MdmPathDescriptor pathDesc;
      CmsRet ret;

      getFullPathAndValue(&cmdLine[7], fullPathBuf, NULL);

      if (fullPathBuf[0] == '\0')
      {
         printf("a fullpath must be specified.\n");
         return;
      }

      memset(&pathDesc, 0, sizeof(pathDesc));
      ret = cmsMdm_fullPathToPathDescriptor(fullPathBuf, &pathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         printf("Invalid fullpath %s\n", fullPathBuf);
         return;
      }

      if (pathDesc.paramName[0] != '\0')
      {
         printf("The fullpath must specify an object (not parameter)\n");
         return;
      }

      printf("creating instance oid %d iidStack=%s\n", pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));

      ret = cmsObj_addInstance(pathDesc.oid, &pathDesc.iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         printf("Add object failed, ret=%d\n", ret);
      }
      else
      {
         ret = cmsMdm_pathDescriptorToFullPath(&pathDesc, &strPtr);
         if (ret != CMSRET_SUCCESS)
         {
            printf("could not convert pathDesc to fullpath?!?, ret=%d\n", ret);
         }
         else
         {
            printf("Added new obj instance at %s\n", strPtr);
            CMSMEM_FREE_BUF_AND_NULL_PTR(strPtr);
         }
      }
   }
   else if (!strncasecmp(cmdLine, "delobj", 6))
   {
      char fullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};
      MdmPathDescriptor pathDesc;
      CmsRet ret;

      getFullPathAndValue(&cmdLine[7], fullPathBuf, NULL);

      if (fullPathBuf[0] == '\0')
      {
         printf("a fullpath must be specified.\n");
         return;
      }

      memset(&pathDesc, 0, sizeof(pathDesc));
      ret = cmsMdm_fullPathToPathDescriptor(fullPathBuf, &pathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         printf("Invalid fullpath %s\n", fullPathBuf);
         return;
      }

      if (pathDesc.paramName[0] != '\0')
      {
         printf("The fullpath must specify an object (not parameter)\n");
         return;
      }

      printf("deleting instance oid %d iidStack=%s\n", pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));

      ret = cmsObj_deleteInstance(pathDesc.oid, &pathDesc.iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         printf("Del object failed, ret=%d\n", ret);
      }
      else
      {
         printf("Deleted obj instance at %s\n", fullPathBuf);
      }
   }
   else if (!strncasecmp(cmdLine, "setnonpersistent", 16))
   {
      char fullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};
      MdmPathDescriptor pathDesc;
      CmsRet ret;

      getFullPathAndValue(&cmdLine[17], fullPathBuf, NULL);

      if (fullPathBuf[0] == '\0')
      {
         printf("a fullpath must be specified.\n");
         return;
      }

      memset(&pathDesc, 0, sizeof(pathDesc));
      ret = cmsMdm_fullPathToPathDescriptor(fullPathBuf, &pathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         printf("Invalid fullpath %s\n", fullPathBuf);
         return;
      }

      if (pathDesc.paramName[0] != '\0')
      {
         printf("The fullpath must specify an object (not parameter)\n");
         return;
      }

      printf("Setting NON-PERSISTENT on oid %d iidStack=%s\n", pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));

      ret = cmsObj_setNonpersistentInstance(pathDesc.oid, &pathDesc.iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         printf("Set NON-PERSISTENT failed, ret=%d\n", ret);
      }
      else
      {
         printf("Marked object at %s as NON-PERSISTENT\n", fullPathBuf);
      }
   }
   else
   {
      printf("No sub-command detected, type mdm -h for usage\n");
   }

}


#ifdef SUPPORT_DM_DETECT
void processDataModelCmd(char *cmdLine)
{
   if (!strcasecmp(cmdLine, "get"))
   {
      if (cmsMdm_isDataModelDevice2())
      {
         printf("current data model is Pure181\n");
      }
      else
      {
         printf("current data model is Hybrid\n");
      }
   }
   else if (!strcasecmp(cmdLine, "set"))
   {
      cmsUtil_setDataModelDevice2();
      printf("Data Model will be Pure181 on next reboot.\n");
   }
   else if (!strcasecmp(cmdLine, "clear"))
   {
      cmsUtil_clearDataModelDevice2();
      printf("Data Model will be Hybrid on next reboot.\n");
   }
   else if (!strcasecmp(cmdLine, "toggle"))
   {
      cmsUtil_toggleDataModel();

      {
         SINT32 rv;
         UINT8 dmc[CMS_DATA_MODEL_PSP_VALUE_LEN]={0};
         rv = cmsPsp_get(CMS_DATA_MODEL_PSP_KEY, dmc, sizeof(dmc));
         if (rv != CMS_DATA_MODEL_PSP_VALUE_LEN)
         {
            printf("error while trying to read data model mode from PSP, rv=%d\n", rv);
         }
         else
         {
            printf("Data Model will be %s on next reboot.\n",
                  (dmc[0] == 1) ? "Pure181" : "Hybrid");
         }
      }
   }
   else
   {
      printf("usage: datamodel get\n");
      printf("       datamodel set   (set mode to 1, meaning Pure181)\n");
      printf("       datamodel clear (set mode to 0, meaning Hybrid)\n");
      printf("       datamodel toggle\n\n");
   }

   return;
}
#endif  /* SUPPORT_DM_DETECT */


void processDumpEidInfoCmd(char *cmdLine)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret;

   if (!strcasecmp(cmdLine, "help") || !strcasecmp(cmdLine, "-h") || !strcasecmp(cmdLine, "--help"))
   {
      printf("usage: dumpeid [eid] \n");
      printf("request smd to dump its Entity Info database.  If an eid is\n");
      printf("given, then only that eid will be dumped.\n");
      return;
   }

   msg.dst = EID_SMD;
   msg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg.type = CMS_MSG_DUMP_EID_INFO;
   msg.flags_request = 1;

   if (cmsUtl_strlen(cmdLine))
   {
      if (CMSRET_SUCCESS != cmsUtl_strtoul(cmdLine, NULL, 0, &msg.wordData))
      {
         printf("argument must be a (eid) number.\n");
         return;
      }
   }

   if ((ret = cmsMsg_sendAndGetReply(cliPrvtMsgHandle, &msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d", ret);
   }
}


void processDumpSysInfoCmd(char *cmdLine __attribute__((unused)))
{
   int rc;

   printf("###DumpSysInfo: First dump system information\n");
   rc = system("/opt/scripts/dumpsysinfo.sh");
   if (rc != 0)
   {
      cmsLog_debug("dumpsysinfo shell script returned %d", rc);
   }

   printf("\n\n###DumpSysInfo: now dump CMS information\n");

   printf("\n#####Versions\n");
   processSwVersionCmd("");
   processSwVersionCmd("-b");
   processSwVersionCmd("-m");
   processSwVersionCmd("-c");
#ifdef DMP_X_BROADCOM_COM_ADSLWAN_1
   processSwVersionCmd("-d");
#endif
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   processSwVersionCmd("-v");
#endif

   printf("\n\n#####Memory Info\n");
   processMeminfoCmd("");

   printf("\n\n#####Dumping contents of saved config file\n");
   processDumpCfgCmd("");

   printf("\n\n#####Dumping MDM\n");
   processDumpMdmCmd("");

   printf("\n\n#####Dumping syslog");
   processSyslogCmd("dump");
}


void processExitOnIdleCmd(char *cmdLine)
{
   if (!strncasecmp(cmdLine, "get", 3))
   {
      printf("current timout is %d seconds\n", exitOnIdleTimeout);
   }
   else if (!strncasecmp(cmdLine, "set", 3))
   {
      exitOnIdleTimeout = atoi(&(cmdLine[4]));
      printf("timeout is set to %d seconds (for this session only, not saved to config)\n", exitOnIdleTimeout);
   }
   else
   {
      printf("usage: exitOnIdle get\n");
      printf("       exitOnIdle set <seconds>\n\n");
   }

   return;
}


#endif /* SUPPORT_DEBUG_TOOLS */

#endif /* SUPPORT_CLI_CMD */
