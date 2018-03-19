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

#ifdef SUPPORT_CLI_CMD

#include "cms_util.h"
#include "cms_core.h"
#include "cms_qdm.h"
#include "cms_cli.h"
#include "cms_msg.h"
#include "cms_seclog.h"
#include "cms_boardcmds.h"
#include "../../../../shared/opensource/include/bcm963xx/bcm_hwdefs.h"
#include "prctl.h"
#include "cli.h"
#include "adslctlapi.h"
#include "devctl_adsl.h"
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "otp_ioctl.h"

/* processing commands that are implemented in this file can be declared here */
static void processHelpCmd(char *cmdLine);
static void processLogoutCmd(char *cmdLine);
static void processRebootCmd(char *cmdLine);
#ifdef SUPPORT_IEEE1905_GOLDENNODE
static void processNcapCmd(char *cmdLine);
#endif
#ifdef SUPPORT_DSL
static void processDslCmd(char *cmdName, char *cmdLine);
static void processAdslCtlCmd(char *cmdLine);
static void processXdslCtlCmd(char *cmdLine);
#ifdef DMP_X_BROADCOM_COM_SELT_1
static void processSeltCtlCmd(char *cmdLine);
#endif
#ifdef SUPPORT_DSL_BONDING
static void processXdslCtl0Cmd(char *cmdLine);
static void processXdslCtl1Cmd(char *cmdLine);
#endif
static void processXtmCtlCmd(char *cmdLine);
#endif /* SUPPORT_DSL */

#ifdef SUPPORT_DDNSD
static void processDDnsCmd(char *cmdLine);
#endif

#ifdef CMS_SECURITY_LOG
static void processSeclogCmd(char *cmdLine);
#endif
#ifdef BRCM_PROFILER_ENABLED
static void processProfilerCtlCmd(char *cmdLine);
#endif
#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
static void processSntpCmd(char *cmdLine);
#endif
#ifdef DMP_X_ITU_ORG_GPON_1
static void processShowOmciStatsCmd(char *cmdLine);
#endif
static void processArpCmd(char *cmdLine);
static void processDefaultGatewayCmd(char *cmdLine);
static void processDhcpServerCmd(char *cmdLine);
static void processDnsCmd(char *cmdLine);
static void processLanCmd(char *cmdLine);
static void processLanHostsCmd(char *cmdLine);
static void processPasswdCmd(char *cmdLine);
static void processPppCmd(char *cmdLine);
static void processRestoreDefaultCmd(char *cmdLine);
static void processRouteCmd(char *cmdLine);
static void processSaveCmd(char *cmdLine);
static void processUptimeCmd(char *cmdLine);

#ifdef DMP_X_BROADCOM_COM_EPON_1
static void processEponCfgCmd(char *cmdLine);
#endif

#ifdef SUPPORT_SECURE_BOOT
static void processBtrmCfgCmd(char *cmdLine);
#endif

typedef void (*CLI_CMD_FNC) (char *cmdLine);

typedef struct {
   char *cmdName;
   char *cmdHelp;
   UINT8  perm;          /**< permission bit required to execute this item */
   UBOOL8 isLockNeeded;  /**< Framework will acquire lock before calling processing func */
   CLI_CMD_FNC cliProcessingFunc;
} CLI_CMD_ITEM;

#define LOCK_NEEDED TRUE


static const CLI_CMD_ITEM cliCmdTable[] = {
   { "?", "List of all commands.",     PERM3, 0,           processHelpCmd },
   { "help", "List of all commands.",  PERM3, 0,           processHelpCmd },
   { "logout", "Logout from CLI.",     PERM3, 0,           processLogoutCmd },
   { "exit", "Logout from CLI.",       PERM3, 0,           processLogoutCmd },
   { "quit", "Logout from CLI.",       PERM3, 0,           processLogoutCmd },
   { "reboot", "Reboot the system.",   PERM3, LOCK_NEEDED, processRebootCmd },
#ifdef SUPPORT_IEEE1905_GOLDENNODE
   { "ncap", "Execute NCAP commands",  PERM3, 0,           processNcapCmd },
#endif
#ifdef SUPPORT_DSL
   { "adsl", "adsl",                   PERM2, LOCK_NEEDED, processAdslCtlCmd },
   { "xdslctl", "xdslctl",                   PERM2, LOCK_NEEDED, processXdslCtlCmd },
#ifdef DMP_X_BROADCOM_COM_SELT_1
   { "seltctl", "seltctl",                 PERM2, LOCK_NEEDED, processSeltCtlCmd },
#endif
#ifdef SUPPORT_DSL_BONDING
   { "xdslctl0", "xdslctl0",                   PERM2, LOCK_NEEDED, processXdslCtl0Cmd },
   { "xdslctl1", "xdslctl1",                   PERM2, LOCK_NEEDED, processXdslCtl1Cmd },
#endif
   { "xtm", "xtm",                     PERM2, 0,           processXtmCtlCmd },
#endif
   { "brctl", "brctl",                 PERM2, 0,           NULL },
   { "cat", "cat",                     PERM2, 0,           NULL },
   { "virtualserver", "show/enable/disable virtual servers", PERM2, LOCK_NEEDED, processVirtualServerCmd },
#ifdef SUPPORT_DDNSD
   { "ddns", "ddns",                  PERM2, LOCK_NEEDED, processDDnsCmd },
#endif
#ifdef SUPPORT_DEBUG_TOOLS
   { "df", "file system info",        PERM2, 0,           NULL },
   { "loglevel", "get or set logging level", PERM2, LOCK_NEEDED, processLogLevelCmd },
   { "logdest", "get or set logging destination", PERM2, LOCK_NEEDED, processLogDestCmd },
   { "dumpcfg", "inspect config info in flash", PERM2, LOCK_NEEDED, processDumpCfgCmd },
   { "dumpmdm", "inspect MDM",                  PERM1, LOCK_NEEDED, processDumpMdmCmd },
   { "dumpeid", "ask smd to dump EID DB",       PERM2, 0,           processDumpEidInfoCmd },
   { "mdm",     "various MDM debug operations", PERM2, LOCK_NEEDED, processMdmCmd },
   { "meminfo", "Get system and CMS memory info", PERM2, 0,         processMeminfoCmd },
   { "kill", "send signal to process",   PERM2, 0,        NULL },
   { "dumpsysinfo", "Dump system info for error report", PERM2, LOCK_NEEDED, processDumpSysInfoCmd },
   { "exitOnIdle", "get/set exit-on-idle timeout value (in seconds)", PERM3, 0, processExitOnIdleCmd },
#ifdef SUPPORT_DM_DETECT
   { "datamodel", "data model detection settings", PERM2, LOCK_NEEDED, processDataModelCmd },
#endif
#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1
   { "dnsproxy", "dnsproxy",             PERM3, 0,        processDnsproxyCmd },
#endif
#endif /* SUPPORT_DEBUG_TOOLS */
   { "syslog", "system log commands",           PERM2, LOCK_NEEDED, processSyslogCmd },
   { "echo", "echo",                     PERM2, 0,           NULL },
   { "ifconfig", "inspect network info", PERM2, 0,        NULL },
   { "ping", "ping",                     PERM3, 0,        NULL },
   { "ps", "process info",               PERM2, 0,        NULL },
   { "pwd", "present working directory", PERM2, 0,        NULL },
#ifdef CMS_SECURITY_LOG
   { "seclog", "security log commands",  PERM2, 0,           processSeclogCmd },
#endif
#ifdef BRCM_PROFILER_ENABLED
   { "profiler", "profiler",         PERM2, LOCK_NEEDED,  processProfilerCtlCmd },
#endif /* BRCM_PROFILER_ENABLED */
#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
   { "sntp", "sntp",                 PERM2, LOCK_NEEDED,  processSntpCmd },
#endif
   { "sysinfo", "sysinfo",           PERM2, 0,            NULL },
   { "tftp", "tftp",                 PERM2, 0,            NULL },
#ifdef BRCM_VOICE_SUPPORT
   { "voice", "voice",               PERM3, 0,  processVoiceCtlCmd },
#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1
   { "dect", "dect",                 PERM3, 0,  processDectCmd },
#endif
#endif

#ifdef BRCM_WLAN
   { "wlctl", "wlctl",               PERM2, 0,            NULL },
#endif
#ifdef DMP_X_ITU_ORG_GPON_1
   { "showOmciStats", "Show the OMCI Statistics", PERM2, LOCK_NEEDED, processShowOmciStatsCmd },
   { "laser", "send commands to laser driver",  PERM2, 0,           processLaserCmd },
   { "omci", "send commands to OMCID",  PERM2, 0,           processOmciCmd },
   { "omcipm", "send commands to OMCIPMD",  PERM2, 0,       processOmcipmCmd },
#endif
#ifdef DMP_X_BROADCOM_COM_EPON_1
   { "laser", "send commands to laser driver",  PERM2, 0,           processLaserCmd },
#endif

   { "arp", "arp",                      PERM2, 0,           processArpCmd },     
   { "defaultgateway", "defaultgatway", PERM2, LOCK_NEEDED, processDefaultGatewayCmd },
   { "dhcpserver", "dhcpserver",        PERM2, LOCK_NEEDED, processDhcpServerCmd },
   { "dns", "dns",                      PERM2, LOCK_NEEDED, processDnsCmd },
   { "lan", "lan",                      PERM2, LOCK_NEEDED, processLanCmd },
   { "lanhosts", "show hosts on LAN",   PERM3, LOCK_NEEDED, processLanHostsCmd },
#ifndef SUPPORT_IEEE1905_GOLDENNODE
   { "passwd", "passwd",                PERM3, 0, processPasswdCmd },
#endif
   { "ppp", "ppp",                      PERM2, LOCK_NEEDED, processPppCmd },
   { "restoredefault", "restore config flash to default", PERM3, LOCK_NEEDED, processRestoreDefaultCmd },
   { "route", "route",                  PERM2, LOCK_NEEDED, processRouteCmd },
   { "save", "save",                    PERM3, LOCK_NEEDED, processSaveCmd },
   { "swversion", "get software version", PERM3, LOCK_NEEDED, processSwVersionCmd },
   { "uptime", "get system uptime",     PERM3, LOCK_NEEDED, processUptimeCmd },
#ifndef SUPPORT_IEEE1905_GOLDENNODE
   { "wan", "wan",                        PERM3, LOCK_NEEDED, processWanCmd },
#endif
#ifdef SUPPORT_MOCA
   { "moca", "moca control & stats",    PERM2, LOCK_NEEDED, processMocaCmd },
#endif
#ifdef SUPPORT_MODSW_CLI
#ifdef SUPPORT_OSGI_FELIX
   { "osgid", "osgid",                  PERM2, 0,           processOsgidCmd },
#endif
#endif
#if defined(SUPPORT_IGMP) || defined(SUPPORT_MLD)
   { "mcpctl", "display mcpd information", PERM2, 0,        NULL },
#endif
#ifdef DMP_X_BROADCOM_COM_EPON_1
   { "eponcfg", "epon device info cfg", PERM2, LOCK_NEEDED, processEponCfgCmd },
#endif
#ifdef SUPPORT_SECURE_BOOT
   { "btrmcfg", "bootrom config", 	PERM2, 0,	processBtrmCfgCmd },
#endif
#ifdef DMP_X_BROADCOM_COM_TM_1
   { "QoS", "QoS configuration",        PERM2, 0,       processQoSCmd },
#endif
};

#define NUM_CLI_CMDS (sizeof(cliCmdTable) / sizeof(CLI_CMD_ITEM))


/* all hidden commands are just directly executed */
static const char *cliHiddenCmdTable[] = {
   "dumpmem", "ebtables", "iptables",
   "logread", "setmem", "sh", "pspctl"
};

#define NUM_HIDDEN_CMDS (sizeof(cliHiddenCmdTable) / sizeof(char *))

UBOOL8 cliCmdSaveNeeded = FALSE;


/* this is defined in rut_util.c and is used for user requst to disconnect/connect wan connection temperaroly */
extern CmsRet rut_sendMsgToSsk(CmsMsgType msgType, UINT32 wordData, void *msgData, UINT32 msgDataLen);

CmsRet cli_sendMsgToSsk(CmsMsgType msgType, UINT32 wordData, void *msgData, UINT32 msgDataLen)
{
   return rut_sendMsgToSsk(msgType, wordData, msgData, msgDataLen);
}


UBOOL8 cli_processCliCmd(const char *cmdLine)
{
   UBOOL8 found=FALSE;
   char compatLine[CLI_MAX_BUF_SZ] = {0};
   UINT32 i, cmdNameLen, compatLineLen;


   /*
    * To maintain compatiblity with previous software, if cmd starts with netCtl,
    * just strip it off.
    */
   if (!strncasecmp(cmdLine, "netctl", 6))
   {
      strcpy(compatLine, &(cmdLine[7]));
   }
   else
   {
      strcpy(compatLine, cmdLine);
   }


   /*
    * Figure out the length of the command (the first word).
    */
   compatLineLen = strlen(compatLine);
   cmdNameLen = compatLineLen;
   for (i=0; i < compatLineLen; i++)
   {
      if (compatLine[i] == ' ')
      {
         cmdNameLen = i;
         break;
      }
   }


   for (i=0; i < NUM_CLI_CMDS; i++)
   {
      /*
       * Note that strcasecmp is used here, which means a command should not differ from
       * another command only in capitalization.
       */
      if ((cmdNameLen == strlen(cliCmdTable[i].cmdName)) &&
          (!strncasecmp(compatLine, cliCmdTable[i].cmdName, cmdNameLen)) &&
          (currPerm & cliCmdTable[i].perm))
      {
         if (cliCmdTable[i].isLockNeeded) 
         {
            CmsRet ret;

            /* get lock if indicated on the cliCmdTable */
            if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
            {
               cmsLog_error("failed to get lock, ret=%d", ret);
               printf("Could not run command due to lock failure.\r\n");
               cmsLck_dumpInfo();
               found = TRUE;
               break;
            }
         }

         if (cliCmdTable[i].cliProcessingFunc != NULL)
         {
            if (compatLineLen == cmdNameLen)
            {
               /* there is no additional args, just pass in null terminator */
               (*(cliCmdTable[i].cliProcessingFunc))(&(compatLine[cmdNameLen]));
            }
            else
            {
               /* pass the additional args to the processing func */
               (*(cliCmdTable[i].cliProcessingFunc))(&(compatLine[cmdNameLen + 1]));
            }

         }
         else
         {
            prctl_runCommandInShellWithTimeout(compatLine);
         }

         if (cliCmdTable[i].isLockNeeded)
         {
            cmsLck_releaseLock();
         }

 
         /* See if we need to flush config */
         if (cliCmdSaveNeeded)
         {
            CmsRet ret;

            /* regardless of success or failure, clear cliCmdSaveNeeded */
            cliCmdSaveNeeded = FALSE;

            if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
            {
               cmsLog_error("failed to get lock, ret=%d", ret);
               printf("Could not run command due to lock failure.\r\n");
               found = TRUE;
               break;
            }

            if ((ret = cmsMgm_saveConfigToFlash()) != CMSRET_SUCCESS)
            {
               cmsLog_error("saveConfigToFlash failed, ret=%d", ret);
            }
            
            cmsLck_releaseLock();
         }

         found = TRUE;
         break;
      }
   }

   return found;
}


UBOOL8 cli_processHiddenCmd(const char *cmdLine)
{
   UINT32 i;
   UBOOL8 found = FALSE;
   UINT32 cmdNameLen, compatLineLen;

   /* only admin and support are allowed to run hidden/shell commands */
   if ((currPerm & PERM2) == 0)
   {
      return FALSE;
   }

   /*
    * Figure out the length of the command (the first word).
    */
   compatLineLen = strlen(cmdLine);
   cmdNameLen = compatLineLen;
   for (i=0; i < compatLineLen; i++)
   {
      if (cmdLine[i] == ' ')
      {
         cmdNameLen = i;
         break;
      }
   }
   
   for (i=0; i < NUM_HIDDEN_CMDS; i++)
   {
      if ((cmdNameLen == strlen(cliHiddenCmdTable[i])) &&
          (!strncasecmp(cmdLine, cliHiddenCmdTable[i], cmdNameLen)))
      {
         if (!strncasecmp(cmdLine, "sh", cmdNameLen))
         {
            /* special handling for sh, spawn with SIGINT set to default */
            SpawnProcessInfo spawnInfo;
            SpawnedProcessInfo procInfo;
            CmsRet r2;

            memset(&spawnInfo, 0, sizeof(spawnInfo));

            spawnInfo.exe = "/bin/sh";
            spawnInfo.args = "-c sh";
            spawnInfo.spawnMode = SPAWN_AND_RETURN;
            spawnInfo.stdinFd = 0;
            spawnInfo.stdoutFd = 1;
            spawnInfo.stderrFd = 2;
            spawnInfo.serverFd = -1;
            spawnInfo.maxFd = 50;
            spawnInfo.inheritSigint = FALSE;

            memset(&procInfo, 0, sizeof(procInfo));

            r2 = prctl_spawnProcess(&spawnInfo, &procInfo);
            if (r2 == CMSRET_SUCCESS)
            {
                /* now wait for sh to finish */
                CollectProcessInfo collectInfo;

                collectInfo.collectMode = COLLECT_PID;
                collectInfo.pid = procInfo.pid;

                r2 = prctl_collectProcess(&collectInfo, &procInfo);
            }
            else
            {
                cmsLog_error("failed to spawn sh, r2=%d", r2);
            }
         }
         else
         {
             prctl_runCommandInShellBlocking((char *) cmdLine);
         }
         found = TRUE;
         break;
      }
   }

   return found;
}


/*******************************************************************
 *
 * Command processing commands start here.
 *
 *******************************************************************
 */

void processHelpCmd(char *cmdLine __attribute((unused)))
{

   UINT32 i;

   for (i=0; i < NUM_CLI_CMDS; i++)
   {
      if (currPerm & cliCmdTable[i].perm)
      {
         printf("%s\r\n", cliCmdTable[i].cmdName);
      }
   }

   return;
}


void processLogoutCmd(char *cmdLine __attribute__((unused)))
{
   cli_keepLooping = FALSE;
   return;
}

void processRebootCmd(char *cmdLine __attribute__((unused)))
{
   printf("\r\nThe system shell is being reset. Please wait...\r\n");
   fflush(stdout);

   cmsUtil_sendRequestRebootMsg(cliPrvtMsgHandle);

   return;
}

#ifdef SUPPORT_IEEE1905_GOLDENNODE
void processNcapCmd(char *cmdLine)
{
  if (strpbrk (cmdLine, ";><|\\\%&^") != NULL) {
    printf("Illegal command string\n");
    return;
  }
  if (strlen(cmdLine) > 255) {
    printf("Command Too Long\n");
    return;
  }
  else {
    char cmd[300];
    sprintf(cmd, "ncap %s 2> /var/ncaperr", cmdLine);
    prctl_runCommandInShellBlocking(cmd);
  }
}
#endif

#ifdef SUPPORT_DSL
static void processAdslCtlCmd(char *cmdLine)
{
    processDslCmd("adsl", cmdLine);
}

static void processXdslCtlCmd(char *cmdLine)
{
    processDslCmd("xdslctl", cmdLine);
}

#ifdef DMP_X_BROADCOM_COM_SELT_1
static void processSeltCtlCmd(char *cmdLine)
{
    processDslCmd("seltctl", cmdLine);
}
#endif

#ifdef SUPPORT_DSL_BONDING
static void processXdslCtl0Cmd(char *cmdLine)
{
    processDslCmd("xdslctl0", cmdLine);
}
static void processXdslCtl1Cmd(char *cmdLine)
{
    processDslCmd("xdslctl1", cmdLine);
}
#endif

void processDslCmd(char *cmdName, char *cmdLine)
{
    FILE* fs = NULL;
    char cmd[BUFLEN_256];
    
    if (NULL != strstr(cmdLine, "profile")) {
        unsigned char lineId;
#ifdef SUPPORT_DSL_BONDING
        if( 0 == strcmp("xdslctl1", cmdName))
           lineId = 1;
        else
#endif
           lineId = 0;
        if(NULL != strstr(cmdLine, "save")) {
            /* Save current driver configuration to the flash */
            cliCmdSaveNeeded = TRUE;
            DslIntfCfgMdmUpdate(lineId);
            return;
        }
        else if(NULL != strstr(cmdLine, "restore")) {
            /* Restore driver configuration; useful when user play around with the driver
                configuration through the CLI or DslDiags and want to restore it without rebooting */
            DslCfgProfileUpdate(lineId);
            return;
        }
    }
    
    // execute command with err output to adslerr
    sprintf(cmd, "%s %s 2> /var/adslerr", cmdName, cmdLine);
    prctl_runCommandInShellBlocking(cmd);
    
    /* check for presence of error output file */
    fs = fopen("/var/adslerr", "r");
    if (fs == NULL) return;

    prctl_runCommandInShellBlocking("cat /var/adslerr");
    fclose(fs);

    /* remove error file if it is there, could just call unlink("/var/adlserr") */
    prctl_runCommandInShellBlocking("rm /var/adslerr");

    return;
}

#ifdef DMP_ADSLWAN_1
void DslCfgProfileUpdate_igd(unsigned char lineId)
{
    adslCfgProfile  adslCfg;
    CmsRet          cmsRet;
    WanDslIntfCfgObject *dslIntfCfg = NULL;
    InstanceIdStack         iidStack = EMPTY_INSTANCE_ID_STACK;
    
    cmsRet = cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfCfg);
    if (cmsRet != CMSRET_SUCCESS) {
        printf("%s: could not get DSL intf cfg, ret=%d", __FUNCTION__, cmsRet);
        return;
    }
    memset((void *)&adslCfg, 0, sizeof(adslCfg));
    xdslUtil_CfgProfileInit(&adslCfg, dslIntfCfg);
    cmsObj_free((void **) &dslIntfCfg);
    
    cmsRet = xdslCtl_Configure(lineId, &adslCfg);
    if (cmsRet != CMSRET_SUCCESS)
        printf ("%s: could not configure DLS driver, ret=%d\n", __FUNCTION__, cmsRet);
    
    cmsLog_debug("DslCfgProfile is updated.");
}

void DslIntfCfgMdmUpdate_igd(unsigned char lineId)
{
    int     nRet;
    long    dataLen;
    char    oidStr[] = { 95 };      /* kOidAdslPhyCfg */
    adslCfgProfile  adslCfg;
    CmsRet          cmsRet;
    WanDslIntfCfgObject *dslIntfCfg = NULL;
    InstanceIdStack         iidStack = EMPTY_INSTANCE_ID_STACK;
    
    dataLen = sizeof(adslCfgProfile);
    nRet = xdslCtl_GetObjectValue(lineId, oidStr, sizeof(oidStr), (char *)&adslCfg, &dataLen);
    
    if( nRet != BCMADSL_STATUS_SUCCESS) {
        printf("%s: could not get adsCfg, ret=%d", __FUNCTION__, nRet);
        return;
    }
    
    cmsRet = cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfCfg);
    if (cmsRet != CMSRET_SUCCESS) {
        printf("%s: could not get DSL intf cfg, ret=%d", __FUNCTION__, cmsRet);
        return;
    }
    
    xdslUtil_IntfCfgInit(&adslCfg, dslIntfCfg);
    
    cmsRet = cmsObj_set(dslIntfCfg, &iidStack);
    if (cmsRet != CMSRET_SUCCESS)
        printf("%s: could not set DSL intf cfg, ret=%d", __FUNCTION__, cmsRet);
    
    cmsObj_free((void **) &dslIntfCfg);
    
    cmsLog_debug("DslIntfCfgMdm is updated.");
}
#endif /* DMP_ADSLWAN_1 */

void processXtmCtlCmd(char *cmdLine)
{
   char xtmCmd[BUFLEN_128];
   char cmd[BUFLEN_128];
   FILE* fs = NULL;

   /* put back the command name 'xtm' */
   sprintf(xtmCmd, "xtm %s", cmdLine);
   printf("processXtmCtlCmd: %s\n", xtmCmd);

   /* if not operate command then just execute command and return */
   if ( strstr(cmdLine, "operate") == NULL )
   {
      prctl_runCommandInShellBlocking(xtmCmd);
      return;
   }

   if ( strstr(cmdLine, "tdte") != NULL )
   {
      /* if add or delete tdte */
      if ( strstr(cmdLine, "add") != NULL )
      {
//         if ( AtmTd_validateCmd(atmCmd) == FALSE )
//         {
//            fprintf(stderr, "atm: operate tdte error -- invalid parameters.\n");
//            return 0;
//         }
         /* execute command with err output to atmerr */
         sprintf(cmd, "%s 2> /var/xtmerr", xtmCmd);
         prctl_runCommandInShellBlocking(cmd);
         /* read xtmerr, if there is no err then need to configure XTM profile */
         fs = fopen("/var/xtmerr", "r");
         if (fs != NULL)
         {
            if ( fgets(cmd, BUFLEN_128, fs) == NULL || strlen(cmd) <= 1 )
               cliCmdSaveNeeded = TRUE;
//               AtmTd_trffDscrConfig(atmCmd);
            else
               prctl_runCommandInShellBlocking("cat /var/xtmerr");
            
            fclose(fs);
            prctl_runCommandInShellBlocking("rm /var/xtmerr");
         }
      }
      else if (strstr(cmdLine, "delete") != NULL)
      {
         /* need to getObjectId before execute command to remove it */
//         AtmTd_getObjectIdFromDeleteCmd(atmCmd);
         /* execute command with err output to atmerr */
         sprintf(cmd, "%s 2> /var/xtmerr", xtmCmd);
         prctl_runCommandInShellBlocking(cmd);
         /* read xtmerr, if there is no err then need to configure XTM profile in PSI */
         fs = fopen("/var/xtmerr", "r");
         if (fs != NULL)
         {
            if ( fgets(cmd, BUFLEN_128, fs) == NULL || strlen(cmd) <= 1 )
               cliCmdSaveNeeded = TRUE;
//               AtmTd_removeFromPsiOnly(objectId);
            else
               prctl_runCommandInShellBlocking("cat /var/xtmerr");
            
            fclose(fs);
            prctl_runCommandInShellBlocking("rm /var/xtmerr");
         }
      }
      else
      {
         sprintf(cmd, "%s", xtmCmd);
         prctl_runCommandInShellBlocking(cmd);
      }
   }
   else if (strstr(cmdLine, "intf") != NULL)
   {
      /* if state intf */
      if (strstr(cmdLine, "state") != NULL)
      {
         /* execute command with err output to atmerr */
         sprintf(cmd, "%s 2> /var/xtmerr", xtmCmd);
         prctl_runCommandInShellBlocking(cmd);
         /* read xtmerr, if there is no err then need to configure XTM profile in PSI */
         fs = fopen("/var/xtmerr", "r");
         if (fs != NULL)
         {
            if ( fgets(cmd, BUFLEN_128, fs) == NULL || strlen(cmd) <= 1 )
               cliCmdSaveNeeded = TRUE;
//               AtmPrt_portIfcConfig(atmCmd);
            else
               prctl_runCommandInShellBlocking("cat /var/xtmerr");

            fclose(fs);
            prctl_runCommandInShellBlocking("rm /var/xtmerr");
         }
      }
      else
      {
         prctl_runCommandInShellBlocking(xtmCmd);
      }
   }
   else if (strstr(cmdLine, "vcc") != NULL)
   {
      /* if add, delete, addq, deleteq, or state vcc */
      if ((strstr(cmdLine, "add") != NULL && strstr(cmdLine, "addpripkt") == NULL) ||
          (strstr(cmdLine, "delete") != NULL && strstr(cmdLine, "deletepripkt") == NULL) ||
           strstr(cmdLine, "addq") != NULL ||
           strstr(cmdLine, "deleteq") != NULL ||
          strstr(cmdLine, "state") != NULL )
      {
         if ((strstr(cmdLine, "delete") != NULL || strstr(cmdLine, "deleteq") != NULL) /*&&
             (AtmVcc_isDeletedVccInUse(atmCmd) == TRUE)*/)
         {
            printf("app: cannot delete the PVC since it is in use.\n");
            return;
         }
         /* execute command with err output to xtmerr */
         sprintf(cmd, "%s 2> /var/xtmerr", xtmCmd);
         prctl_runCommandInShellBlocking(cmd);
         /* read xtmerr, if there is no err then need to configure XTM profile in PSI */
         fs = fopen("/var/xtmerr", "r");
         if (fs != NULL)
         {
            if (fgets(cmd, BUFLEN_128, fs) == NULL || strlen(cmd) <= 1)
               cliCmdSaveNeeded = TRUE;
//               AtmVcc_vccCfg(atmCmd);
            else
               prctl_runCommandInShellBlocking("cat /var/xtmerr");

            fclose(fs);
            prctl_runCommandInShellBlocking("rm /var/xtmerr");
         }
   }
   else
      {
         prctl_runCommandInShellBlocking(xtmCmd);
      }
   }
   else
   {
      prctl_runCommandInShellBlocking(xtmCmd);
   }

   return;
}

#endif  /* SUPPORT_DSL */


#ifdef SUPPORT_DDNSD
static void processDDnsCmd(char *cmdLine __attribute__((unused)))
{
   /* todo */
#ifdef later
  DDNS_STATUS sts;
  char *pToken = strtok( cmdLine, " " );

  // First token should be "ddns" -- next we check first argument
  pToken = strtok( NULL, " " );

  if ( pToken == NULL || strcmp( pToken, "--help" ) == 0 ) {
    printf( "ddns add hostname --username username --password password --interface interface --service tzo|dyndns\n" );
    printf( "     remove hostname\n" );
    printf( "     show\n" );
    printf( "ddns --help" );
  } else if ( strcmp( pToken, "add" ) == 0 ) {
    char hostname[IFC_MEDIUM_LEN];
    char username[IFC_MEDIUM_LEN];
    char password[IFC_PASSWORD_LEN];
    char iface[IFC_TINY_LEN];
    UINT16 service = 255;

    hostname[0] = username[0] = password[0] = iface[0] = '\0';

    pToken = strtok( NULL, " " ); // Get the hostname

    if (pToken == NULL ) {
      printf( "Add what?\n" );
    } else {
      strncpy( hostname, pToken, IFC_MEDIUM_LEN );

      pToken = strtok( NULL, " " );
      while( pToken != NULL ) {
        if ( strcmp( pToken, "--service" ) == 0 ) {
          if ( service != 255 ) {
            printf( "Already specified service.\n" );
            return;
          } else {
            pToken = strtok( NULL, " " );
            if ( pToken ) {
              if( strcmp( pToken, "tzo" ) == 0 ) {
                service = 0;
              } else if ( strcmp( pToken, "dyndns" ) == 0 ) {
                service = 1;
              } else {
                printf( "Bad service type: %s\n", pToken );
                return;
              }
            } else {
              printf( "Missing argument to --service.\n");
              return;
            }
          }
        } else if ( strcmp( pToken, "--username" ) == 0 ) {
          // Process --username option and argument
          if ( username[0] != '\0' ) {
            printf("Already specified username.\n" );
            return;
          } else {
            pToken = strtok( NULL, " " );
            if( pToken ) {
              strncpy( username, pToken, IFC_MEDIUM_LEN );
            } else {
              printf( "Missing argument to --username" );
              return;
            }
          }
        } else if ( strcmp( pToken, "--password" ) == 0 ) {
          // Process --password option and argument
          if ( password[0] != '\0' ) {
            printf("Already specified password.\n" );
            return;
          } else {
            pToken = strtok( NULL, " " );
            if( pToken ) {
              strncpy( password, pToken, IFC_PASSWORD_LEN );
            } else {
              printf( "Missing argument to --password" );
              return;
            }
          }
        } else if ( strcmp( pToken, "--interface" ) == 0 ) {
          // Process --password option and argument
          if ( iface[0] != '\0' ) {
            printf("Already specified interface.\n" );
            return;
          } else {
            pToken = strtok( NULL, " " );
            if( pToken ) {
              strncpy( iface, pToken, IFC_TINY_LEN );
            } else {
              printf( "Missing argument to --interface" );
              return;
            }
          }
        } else {
          printf("Unknown option %s\n", pToken );
          return;
        }
        pToken = strtok( NULL, " " );
      }
      if( hostname[0] != '\0' && username[0] != '\0' && password[0] != '\0' && iface[0] != '\0' ) {
        sts = BcmDDns_add( hostname, username, password, iface, service );
        if (sts == DDNS_OK)
           BcmDDns_Store();
        BcmDDns_serverRestart();
        BcmPsi_flush();
      } else {
        printf("Missing options to 'add' command.\n" );
      }
    }
  } else if ( strcmp( pToken, "remove" ) == 0 ) {
    pToken = strtok( NULL, " " );
    if ( pToken == NULL ) {
      printf( "Missing hostname to remove.\n" );
    } else {
      sts = BcmDDns_remove( pToken );
      if( sts != DDNS_OK ) {
        printf( "Hostname %s does not exist.\n", pToken );
      }
      else {
        BcmDDns_Store();
        BcmPsi_flush();
      }
    }
  } else if ( strcmp( pToken, "show" ) == 0 ) {
    char hostname[IFC_MEDIUM_LEN];
    char username[IFC_MEDIUM_LEN];
    char password[IFC_PASSWORD_LEN];
    char iface[IFC_TINY_LEN];
    UINT16 service;

    void *node = BcmDDns_getDDnsCfg( NULL, hostname, username, password, iface, &service );

    while( node != NULL ) {
      printf( "%s\t%s\t%s\t%s\n", hostname, username, iface, service?"dyndns":"tzo" );
      node = BcmDDns_getDDnsCfg( node, hostname, username, password, iface, &service );
    }
  } else {
    printf( "Unknown command %s\n", pToken );
  }
#endif /* later */
  return;
}
#endif  /* DDNS */


void cmdSyslogHelp(void)
{
   printf("\nUsage: syslog dump\n");
   printf("       syslog help\n");
}

void processSyslogCmd_igd(char *cmdLine)
{
   IGDDeviceInfoObject *infoObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("cmdLine =>%s<=", cmdLine);

   if (!strcasecmp(cmdLine, "dump"))
   {
      if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &infoObj)) == CMSRET_SUCCESS)
      {
         printf("==== Dump of Syslog ====\n");
         printf("%s\n", infoObj->deviceLog);
         cmsObj_free((void **) &infoObj);
      }
      else
      {
         printf("Could not get deviceInfo object, ret=%d", ret);
      }
   }
   else
   {
      cmdSyslogHelp();
   }
}

void processSyslogCmd(char *cmdLine)
{
#if defined(SUPPORT_DM_LEGACY98)
   processSyslogCmd_igd(cmdLine);
#elif defined(SUPPORT_DM_HYBRID)
   processSyslogCmd_igd(cmdLine);
#elif defined(SUPPORT_DM_PURE181)
   processSyslogCmd_dev2(cmdLine);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2() == TRUE)
      processSyslogCmd_dev2(cmdLine);
   else
      processSyslogCmd_igd(cmdLine);
#endif
}


#ifdef CMS_SECURITY_LOG
static void cmdSeclogHelp(void)
{
   printf("\nUsage: seclog dump\n");
   printf("       seclog reset\n");
   printf("       seclog help\n");
}

void processSeclogCmd(char *cmdLine)
{
   CmsRet ret;
   CmsSecurityLogFile * log;

   cmsLog_debug("cmdLine =>%s<=", cmdLine);

   if (!strcasecmp(cmdLine, "dump"))
   {
      if ((log = cmsMem_alloc(sizeof(*log), 0)) == NULL)
      {
         cmsLog_error("malloc of %d bytes failed", sizeof(*log));
         return;
      }

      if ((ret = cmsLog_getSecurityLog(log)) == CMSRET_SUCCESS)
      {
         printf("==== Dump of Seclog ====\n");
         cmsLog_printSecurityLog(log);
      }
      else
      {
         printf("Could not get security log, ret=%d", ret);
      }

      cmsMem_free(log);
   }
   else if (!strcasecmp(cmdLine, "reset"))
   {
      if ((ret = cmsLog_resetSecurityLog()) == CMSRET_SUCCESS)
      {
         printf("==== Security Log reset ====\n");
      }
      else
      {
         printf("Error resetting security log, ret=%d", ret);
      }
   }
   else
   {
      cmdSeclogHelp();
   }
}
#endif


#ifdef BRCM_PROFILER_ENABLED
void processProfilerCtlCmd(char *cmdLine)
{
   /* todo: need to port this code */
#ifdef later
   if ( strstr(cmdLine, "init") != NULL )
   {
      BcmProfiler_InitData();
   }
   else if ( strstr(cmdLine, "rsdump") != NULL )
   {
      BcmProfiler_RecSeq_Dump();
   }
   else if ( strstr(cmdLine, "dump") != NULL )
   {
      BcmProfiler_Dump();
   }
   else if ( strstr(cmdLine, "start") != NULL )
   {
      BcmProfiler_Start();
   }
   else if ( strstr(cmdLine, "stop") != NULL )
   {
      BcmProfiler_Stop();
   }
   else
   {
      BcmProfiler_ShowCmdSyntax();
   }
#endif /* later */

 return;
} 
#endif /* BRCM_PROFILER_ENABLED */


#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 

/* mwang_todo: these timezones should probably be put into the data model.
 * there is another copy of this list in sntpcfg.html and maybe more copies elsewhere.
 * timeZone that is defined here is also used in cli2_cmd.c
 */
char *timeZones[] =
{"International Date Line West",
 "Midway Island, Samoa",
 "Hawaii",
 "Alaska",
 "Pacific Time, Tijuana",
 "Arizona",
 "Chihuahua, La Paz, Mazatlan",
 "Mountain Time",
 "Central America",
 "Central Time",
 "Guadalajara, Mexico City, Monterrey",
 "Saskatchewan",
 "Bogota, Lima, Quito",
 "Eastern Time",
 "Indiana",
 "Atlantic Time",
 "Caracas, La Paz",
 "Santiago",
 "Newfoundland",
 "Brasilia",
 "Buenos Aires, Georgetown",
 "Greenland",
 "Mid-Atlantic",
 "Azores",
 "Cape Verde Is.",
 "Casablanca, Monrovia",
 "Greenwich Mean Time: Dublin, Edinburgh, Lisbon, London",
 "Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna",
 "Belgrade, Bratislava, Budapest, Ljubljana, Prague",
 "Brussels, Copenhagen, Madrid, Paris",
 "Sarajevo, Skopje, Warsaw, Zagreb",
 "West Central Africa",
 "Athens, Istanbul, Minsk",
 "Bucharest",
 "Cairo",
 "Harare, Pretoria",
 "Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius",
 "Jerusalem",
 "Baghdad",
 "Kuwait, Riyadh",
 "Moscow, St. Petersburg, Volgograd",
 "Nairobi",
 "Tehran",
 "Abu Dhabi, Muscat",
 "Baku, Tbilisi, Yerevan",
 "Kabul",
 "Ekaterinburg",
 "Islamabad, Karachi, Tashkent",
 "Chennai, Kolkata, Mumbai, New Delhi",
 "Kathmandu",
 "Almaty, Novosibirsk",
 "Astana, Dhaka",
 "Sri Jayawardenepura",
 "Rangoon",
 "Bangkok, Hanoi, Jakarta",
 "Krasnoyarsk",
 "Beijing, Chongquing, Hong Kong, Urumqi",
 "Irkutsk, Ulaan Bataar",
 "Kuala Lumpur, Singapore",
 "Perth",
 "Taipei",
 "Osaka, Sapporo, Tokyo",
 "Seoul",
 "Yakutsk",
 "Adelaide",
 "Darwin",
 "Brisbane",
 "Canberra, Melbourne, Sydney",
 "Guam, Port Moresby",
 "Hobart",
 "Vladivostok",
 "Magadan, Solomon Is., New Caledonia",
 "Auckland, Wellington",
 "Fiji, Kamchatka, Marshall Is.",
 NULL };

#ifdef DMP_TIME_1

static CmsRet disableSntp_igd(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   TimeServerCfgObject *ntpCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TIME_SERVER_CFG failed, ret=%d", ret);
      return ret;
   }

   ntpCfg->enable = FALSE;

   ret = cmsObj_set(ntpCfg, &iidStack);
   cmsObj_free((void **) &ntpCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of TIME_SERVER_CFG failed, ret=%d", ret);
   }

   return ret;
}

static CmsRet configureSntp_igd(char **sntpServers, char *timeZoneName)
{
   TimeServerCfgObject *ntpCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TIME_SERVER_CFG failed, ret=%d", ret);
      return ret;
   }

   ntpCfg->enable = TRUE;
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer1, sntpServers[0]);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer2, sntpServers[1]);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer3, sntpServers[2]);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer4, sntpServers[3]);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer5, sntpServers[4]);
   CMSMEM_REPLACE_STRING(ntpCfg->localTimeZoneName, timeZoneName);

   ret = cmsObj_set(ntpCfg, &iidStack);
   cmsObj_free((void **) &ntpCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of TIME_SERVER_CFG failed, ret=%d", ret);
   }

   return ret;
}

void processSntpCmd_igd(char *cmdLine)
{
   int i = 0;
   char *pToken = strtok(cmdLine, " ");
   char *sntpServers[STNP_SERVER_MAX];
   char *timezone = NULL;
   int error = 0;
   int done = 0;
   TimeServerCfgObject *ntpCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;


   /* get default sntp object first: in case disable is called with no parameters */
   for (i = 0; i < STNP_SERVER_MAX; i++)
   {
      sntpServers[i] = NULL;
   }
   if ((cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void **) &ntpCfg)) == CMSRET_SUCCESS)
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
      timezone = cmsMem_strdup(ntpCfg->localTimeZoneName);
   }
   

   if (pToken != NULL)
   {
      if (strcasecmp(pToken, "disable") == 0)
      {
         cliCmdSaveNeeded = TRUE;
         disableSntp_igd();
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
      configureSntp_igd(sntpServers, pToken);
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

#endif   /* DMP_TIME_1 */

static void processSntpCmd(char *cmdLine)
{
#if defined(SUPPORT_DM_LEGACY98)
   processSntpCmd_igd(cmdLine);
#elif defined(SUPPORT_DM_HYBRID)
   processSntpCmd_igd(cmdLine);
#elif defined(SUPPORT_DM_PURE181)
   processSntpCmd_dev2(cmdLine);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2() == TRUE)
      processSntpCmd_dev2(cmdLine);
   else
      processSntpCmd_igd(cmdLine);
#endif
}

#endif // defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1)

/***************************************************************************
// Function Name: cmdArpAddDel
// Description  : arp add or delete
// Parameters   : type - 0=add, 1=delete.
//                         ipAddr - ip address
//                         macAddr - mac address
//                         msg - return error message
// Returns      : 0 - failed.
//                      1 -  success.
****************************************************************************/
int cmdArpAddDel(int type, char *ipAddr, char *macAddr, char *msg) 
{
   int i = 0, sockfd = 0;
   struct arpreq req;
   struct sockaddr sa;
   struct sockaddr_in *addr = (struct sockaddr_in*)&sa;
   unsigned char *ptr = NULL, *mac = NULL, *mac1 = NULL;

   if (cli_isIpAddress(ipAddr) == FALSE)
      {   
      strcpy(msg, "Invalid ip address.");
      return 0;
      }
   //only type=0 for adding has mac address
   if (type == 0 && cli_isMacAddress(macAddr) == FALSE)
      {
      strcpy(msg, "Ivalid mac address.");
      return 0;
      }

   addr->sin_family = AF_INET;
   addr->sin_port = 0;
   inet_aton(ipAddr, &addr->sin_addr);
   memcpy((char *)&req.arp_pa, (char *)&sa, sizeof(struct sockaddr));
   bzero((char*)&sa,sizeof(struct sockaddr));
   ptr = (unsigned char *)sa.sa_data;
   if (type == 0)
   {
      mac = (unsigned char *)macAddr;
      for ( i = 0; i < 6; i++ ) 
      {
         *ptr = (unsigned char)(strtol((char*)mac,(char**)&mac1,16));
         mac1++;
         ptr++;
         mac = mac1;
      }
   }   
   sa.sa_family = ARPHRD_ETHER;
   memcpy((char *)&req.arp_ha, (char *)&sa, sizeof(struct sockaddr));
   req.arp_flags = ATF_PERM;
   req.arp_dev[0] = '\0';

   if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   {
      strcpy(msg, "Error in create socket.");
      return 0;
   } 
   else 
   {
      if (type ==0 && ioctl(sockfd, SIOCSARP, &req) < 0) 
      {
         strcpy(msg, "Error in SIOCSARP.");
         return 0;
      }
      if (type ==1 && ioctl(sockfd, SIOCDARP, &req) < 0) 
      {
         strcpy(msg, "Error in SIOCDARP.");
         return 0;
      }
   }
   if ( sockfd > 0 )
      close(sockfd);

   return 1;
}

void cmdArpHelp(int argc)
{
   if ( argc <= 1 )
      fprintf(stdout,
         "\nUsage: arp add <IP address> <MAC address>\n"
         "       arp delete <IP address>\n"
         "       arp show\n"
         "       arp --help\n");
   else
      fprintf(stderr, "arp: invalid number of parameters " \
         "for option '--help'\n");
}
void processArpCmd(char *cmdLine)
{
   const int maxOpt=3;
   SINT32 argc = 0;
   char *argv[maxOpt];
   char *last = NULL;
   char msg[BUFLEN_64];

   /* parse the command line and build the argument vector */
   argv[0] = strtok_r(cmdLine, " ", &last);
   if (argv[0] != NULL)
   {
      for (argc = 1; argc < maxOpt; argc++)
      {
         argv[argc] = strtok_r(NULL, " ", &last);
         if (argv[argc] == NULL)
            break;
      }
   }

   if (argv[0] == NULL)
   {
      cmdArpHelp(argc);
   }
   else if (strcasecmp(argv[0], "add") == 0)
   {  
      if (argc != 3)
	  {
         fprintf(stderr, "arp: invalid number of parameters " \
                "for option 'add'\n");
		 return;
	  }
      if (cmdArpAddDel(0, argv[1], argv[2], msg) == 0)
         fprintf(stderr, "arp: %s\n", msg);       
   }
   else if (strcasecmp(argv[0], "delete") == 0)
   {  
      if (argc != 2)
	  {
         fprintf(stderr, "arp: invalid number of parameters " \
                "for option 'delete'\n");
		 return;
	  }
      if (cmdArpAddDel(1, argv[1], NULL, msg) == 0)
         fprintf(stderr, "arp: %s\n", msg);       
   }
   else if (strcasecmp(argv[0], "show") == 0)
   {  
      if(argc != 1)
         fprintf(stderr, "arp: invalid number of parameters " \
               "for option 'show'\n");
	  else
	  {
      printf("\n");
      prctl_runCommandInShellBlocking("cat /proc/net/arp");
      printf("\n");
	  }	
   }
   else if (strcasecmp(argv[0], "--help") == 0)
   {
      cmdArpHelp(argc);
   }
   else 
   {
      fprintf(stderr, "\nInvalid option '%s'\n", argv[0]);
   }
   return;
}

void cmdDefaultGatewayHelp(int argc)
{
   if ( argc <= 1 )
      fprintf(stdout,
         "\nUsage: defaultgateway config [<interface(s) sperated by ',' with NO SPACE.  eg. ppp0 OR for multiple interfaces ppp0,ppp1>]\n"
         "       defaultgateway show\n"
         "       defaultgateway --help\n");
   else
      fprintf(stderr, "defaultgateway: invalid number of parameters " \
         "for option '--help'\n");
}
void processDefaultGatewayCmd(char *cmdLine)
{
   const int maxOpt=2;
   SINT32 argc = 0;
   char *argv[maxOpt];
   char *last = NULL;
   char ifaceList[BUFLEN_256];
   CmsRet ret;

   /* parse the command line and build the argument vector */
   argv[0] = strtok_r(cmdLine, " ", &last);
   if (argv[0] != NULL)
   {
      for (argc = 1; argc < maxOpt; argc++)
      {
         argv[argc] = strtok_r(NULL, " ", &last);
         if (argv[argc] == NULL)
            break;
      }
   }

   if (argv[0] == NULL)
   {
      cmdDefaultGatewayHelp(argc);
   }
   else if (strcasecmp(argv[0], "config") == 0)
   {  
      if (argc != 2)
      {
         fprintf(stderr, "defaultgateway: invalid number of parameters " \
                         "for option 'config'\n");
		   return;
	   }

      if ((ret = dalRt_setDefaultGatewayList(argv[1])) != CMSRET_SUCCESS)
      {
         fprintf(stderr, "Config defaultwatewayList failed, ret=%d flags=%s", ret, argv[1]);
      }
      else
      {
         printf("Default Gateway(s): %s\n",argv[1]);
         cliCmdSaveNeeded = TRUE;
      }		 
   }
   else if (strcasecmp(argv[0], "show") == 0)
   {  
      if(argc != 1)
      {
         fprintf(stderr, "defaultgateway: invalid number of parameters " \
                         "for option 'show'\n");
      }                         
      else
	   {
   		if ((ret = dalRt_getDefaultGatewayList(ifaceList)) != CMSRET_SUCCESS)
   		{
            fprintf(stderr, "defaultgateway: dalRt_getDefaultGatewayList failed !" \
                            "for option 'show'\n");
   		}
   		else
   		{
      		 if (strlen(ifaceList) == 0)
      		    strcpy(ifaceList, "None");
      		 printf("Default Gateway(s): %s\n",ifaceList);
         }	
      }
   }
   else if (strcasecmp(argv[0], "--help") == 0)
   {
      cmdDefaultGatewayHelp(argc);
   }
   else 
   {
      printf("\nInvalid option '%s'\n", argv[0]);
   }
   return;
}

void cmdDhcpServerHelp(int argc)
{
   if ( argc <= 1 )
      fprintf(stdout,
         "\nUsage: dhcpserver config <start IP address> <end IP address> <leased time (hour)>\n"
         "       dhcpserver show\n"
         "       dhcpserver --help\n");
   else
      fprintf(stderr, "dhcpserver: invalid number of parameters " \
         "for option '--help'\n");
}
void processDhcpServerCmd(char *cmdLine)
{
   const int maxOpt=4;
   SINT32 argc = 0;
   char *argv[maxOpt];
   char *last = NULL;

   /* parse the command line and build the argument vector */
   argv[0] = strtok_r(cmdLine, " ", &last);
   if (argv[0] != NULL)
   {
      for (argc = 1; argc < maxOpt; argc++)
      {
         argv[argc] = strtok_r(NULL, " ", &last);
         if (argv[argc] == NULL)
            break;
      }
   }

   if (argv[0] == NULL)
   {
      cmdDhcpServerHelp(argc);
   }
   else if (strcasecmp(argv[0], "config") == 0)
   {  
      SINT32 lease;
      
      if (argc != 4)
      {
         fprintf(stderr, "dhcpserver: invalid number of parameters " \
                         "for option 'config'\n");
		 return;
	  }
      if (cli_isIpAddress(argv[1]) == 0)
      {
         fprintf(stderr, "dhcpserver: invalid start IP address '%s' for option 'config'.\n", argv[1]);
         return;
      }
      if (cli_isIpAddress(argv[2]) == 0)
      {
         fprintf(stderr, "dhcpserver: invalid end IP address '%s' for option 'config'.\n", argv[2]);
         return;
      }
      lease = atoi(argv[3]) * 3600;
      if(lease <= 0)
      {
         fprintf(stderr, "dhcpserver: invalid end leased time '%s' for option 'config'.\n", argv[3]);
         return;
      }
      cliCmd_updateDhcpServer(argv[1], argv[2], lease);      
   }
   else if (strcasecmp(argv[0], "show") == 0)
   {  
      if(argc != 1)
         fprintf(stderr, "dhcpserver: invalid number of parameters " \
                         "for option 'show'\n");
      else
      {
         cliCmd_showDhcpServer();
      }	
   }
   else if (strcasecmp(argv[0], "--help") == 0)
   {
      cmdDhcpServerHelp(argc);
   }
   else 
   {
      printf("\nInvalid option '%s'\n", argv[0]);
   }
   return;
}


void cmdDnsHelp(int argc)
{
   if ( argc <= 1 )
      fprintf(stdout,
         "\nUsage: dns config auto [<interface(s) sperated by ',' with NO SPACE.  eg. ppp0 OR for multiple interfaces ppp0,ppp1>]\n"
         "\nUsage: dns config static [<primary DNS> [<secondary DNS>]]\n"
         "       dns show\n"
         "       dns --help\n");
   else
      fprintf(stderr, "dns: invalid number of parameters " \
         "for option '--help'\n");
}
void processDnsCmd(char *cmdLine)
{
   const int maxOpt=4;
   SINT32 argc = 0;
   char *argv[maxOpt];
   char *last = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* parse the command line and build the argument vector */
   argv[0] = strtok_r(cmdLine, " ", &last);
   if (argv[0] != NULL)
   {
      for (argc = 1; argc < maxOpt; argc++)
      {
         argv[argc] = strtok_r(NULL, " ", &last);
         if (argv[argc] == NULL)
            break;
      }
   }

   if (ret == CMSRET_SUCCESS)
   {
      if (argv[0] == NULL)
      {
         cmdDnsHelp(argc);
      }
      else if (strcasecmp(argv[0], "config") == 0)
      {  
         if ( strcasecmp(argv[1], "static") == 0 ) 
         {  
            if (argc < 3)
            {
               fprintf(stderr, "dns: invalid number of parameters " \
                       "for option '%s'\n", argv[1]);
			      return;
            }
            else if ( cli_isIpAddress(argv[2]) == FALSE ) 
			   {
               fprintf(stderr, "dns: invalid primary dns '%s' for " \
                       "option '%s'\n", argv[2], "static");
               return;
            } 
			   else if ( argc == 4 && cli_isIpAddress(argv[3]) == FALSE ) 
			   {
               fprintf(stderr, "dns: invalid secondary dns '%s' for " \
                       "option '%s'\n", argv[3], "static");
               return;
            }
            dalDns_setStaticIpvxDnsServers(CMS_AF_SELECT_IPV4,
                                           argv[2],
                                           (argc == 4 ? argv[3]:""));
            cliCmdSaveNeeded = TRUE;			
         } 
		   else if ( strcasecmp(argv[1], "auto") == 0 ) 
		   {
            if (argc != 3)
            {
               fprintf(stderr, "dns: invalid number of parameters " \
                       "for option '%s'\n", argv[1]);
			      return;
            }
            dalDns_setIpvxDnsIfNameList(CMS_AF_SELECT_IPV4, argv[2]);
            cliCmdSaveNeeded = TRUE;			
         } 
   		else 
   		{
            fprintf(stderr, "dns: invalid parameter %s " \
                    "for option '%s'\n", argv[1], argv[0]);
            return;
         }
      }		 
      else if (strcasecmp(argv[0], "show") == 0)
      {  
         
         if(argc != 1)
            fprintf(stderr, "dns: invalid number of parameters " \
               "for option 'show'\n");
   		else
         {
            char staticDnsServers[CMS_MAX_ACTIVE_DNS_IP * CMS_IPADDR_LENGTH]={0};
            char staticDns1[CMS_IPADDR_LENGTH]={0};
            char staticDns2[CMS_IPADDR_LENGTH]={0};
            char activeDns1[CMS_IPADDR_LENGTH]={0};
            char activeDns2[CMS_IPADDR_LENGTH]={0};

            qdmDns_getActiveIpvxDnsIpLocked(CMS_AF_SELECT_IPV4, activeDns1, activeDns2);

            qdmDns_getStaticIpvxDnsServersLocked(CMS_AF_SELECT_IPV4, staticDnsServers);
            cmsUtl_parseDNS(staticDnsServers, staticDns1, staticDns2, TRUE);

            if (!cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, activeDns1))
            {
               if (!cmsUtl_strcmp(activeDns1, staticDns1))
               {
                  printf("DNS: static\nPrimary DNS Server = %s\nSecondary DNS Server = %s\n",
                         staticDns1, staticDns2);
               }
               else
               {
                  printf("DNS: auto\nPrimary DNS Server = %s\nSecondary DNS Server = %s",
                         activeDns1, activeDns2);
               }
            }
            else
            {
               printf("DNS: no servers configured\n");
            }
         }
      }
      else if (strcasecmp(argv[0], "--help") == 0)
      {
         cmdDnsHelp(argc);
      }
      else 
      {
         printf("\nInvalid option '%s'\n", argv[0]);
         ret = CMSRET_INVALID_ARGUMENTS;
      }
   }
   return;
}



#ifdef DMP_BASELINE_1

CmsRet cliCmd_configLanIp_igd(UBOOL8 isPrimary, const char *ip, const char *mask)
{
   InstanceIdStack lanHostIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanIpIidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostCfgObject *lanHostObj = NULL;
   LanIpIntfObject *lanIpObj = NULL;
   UBOOL8 found = FALSE;
   CmsRet ret;

   cmsLog_debug("Entered: isPrimary=%d ip=%s mask=%s", isPrimary, ip, mask);

   // CLI can only configure the first LAN bridge
   if ((ret = cmsObj_getNext(MDMOID_LAN_HOST_CFG, &lanHostIidStack, (void **) &lanHostObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Lan Host CfgMangmnt, ret = %d", ret);
      return ret;
   }

   // Don't need it, so free it now
   cmsObj_free((void **) &lanHostObj);


   while ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF,
                                         &lanHostIidStack,
                                         &lanIpIidStack,
                                         (void **) &lanIpObj)) == CMSRET_SUCCESS)
   {
      /* Assume that bridge name associated with major ip address does not have ":" */
      if (isPrimary && cmsUtl_strstr(lanIpObj->X_BROADCOM_COM_IfName, ":") == NULL)
      {
         /* do not free lanIpObj */
         found = TRUE;
         break;
      }
   /* Assume that bridge name associated with secondary ip address have ":" */
      else if (!isPrimary && cmsUtl_strstr(lanIpObj->X_BROADCOM_COM_IfName, ":") != NULL)
      {
         /* do not free lanIpObj */
         found = TRUE;
         break;
      }

      cmsObj_free((void **) &lanIpObj);
   }

   if (isPrimary)
   {
      if (found == FALSE)
      {
         cmsLog_error("Failed to get IPInterface, ret = %d", ret);
         return ret;
      }
      else
      {
         /* update IpInterface with the new values */
         CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceIPAddress, ip);
         CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceSubnetMask, mask);
         if ((ret = cmsObj_set(lanIpObj, &lanIpIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set IPInterface, ret = %d", ret);
         }
      }
   }
   else
   {
      /* secondary IP case (very rare, and probably not working) */
      if (found == FALSE) //always create for lan2
      {
         /* could not find it, create a new one */
         if ((ret = cmsObj_addInstance(MDMOID_LAN_IP_INTF, &lanHostIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not create new lan ip interface instance , ret=%d", ret);
            return ret;
         }

         if ((ret = cmsObj_get(MDMOID_LAN_IP_INTF, &lanHostIidStack, 0, (void **) &lanIpObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get LanIpIntfObject, ret=%d", ret);
            return ret;
         }
      }

      /* to update secondary ip address if necessary */
      CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceIPAddress, ip);
      CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceSubnetMask, mask);
      lanIpObj->enable = TRUE;
      if((ret = cmsObj_set(lanIpObj, &lanIpIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set LanIpIntfObject, ret=%d", ret);
      }
   }

   cmsObj_free((void **) &lanIpObj);

   return ret;
}


CmsRet cliCmd_deleteLanIp_igd(UBOOL8 isPrimary)
{
   InstanceIdStack lanHostIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanIpIidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostCfgObject *lanHostObj = NULL;
   LanIpIntfObject *lanIpObj = NULL;
   UBOOL8 found = FALSE;
   CmsRet ret;

   cmsLog_debug("Entered: isPrimary=%d", isPrimary);

   if (isPrimary)
   {
      fprintf(stderr, "lan: in TR98 or Hybrid TR98 mode, can only delete secondary ipaddr\n");
      return CMSRET_INVALID_ARGUMENTS;
   }

   // CLI can only delete the secondary IP addr in the first LAN bridge
   if ((ret = cmsObj_getNext(MDMOID_LAN_HOST_CFG, &lanHostIidStack, (void **) &lanHostObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Lan Host CfgMangmnt, ret = %d", ret);
      return ret;
   }

   // Don't need it, so free it now
   cmsObj_free((void **) &lanHostObj);


   while (!found &&
          (ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF,
                                         &lanHostIidStack,
                                         &lanIpIidStack,
                                         (void **) &lanIpObj)) == CMSRET_SUCCESS)
   {
   /* Assume that bridge name associated with secondary ip address have ":" */
      if (!isPrimary && cmsUtl_strstr(lanIpObj->X_BROADCOM_COM_IfName, ":") != NULL)
      {
         found = TRUE;
      }

      cmsObj_free((void **) &lanIpObj);
   }

   if (found)
   {
      /* secondary IP case (very rare, and probably not working) */
      /* Delete secondary IP addr object, iidStack is point to correct place */
      if((ret = cmsObj_deleteInstance(MDMOID_LAN_IP_INTF, &lanIpIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete LanIpIntfObject, ret=%d", ret);
      }
   }

   return ret;
}


CmsRet cliCmd_updateDhcpServer_igd
   (const char *minAddress,
    const char *maxAddress,
    UINT32 leaseTime)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostCfgObject *lanHostCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_getNext(MDMOID_LAN_HOST_CFG,
                             &iidStack,
                             (void *) &lanHostCfg)) == CMSRET_SUCCESS)
   {
      /* update dhcp start/end ip info */
      CMSMEM_REPLACE_STRING(lanHostCfg->minAddress, minAddress);
      CMSMEM_REPLACE_STRING(lanHostCfg->maxAddress, maxAddress);
      lanHostCfg->DHCPLeaseTime = leaseTime;

      if ((ret = cmsObj_set(lanHostCfg, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set Lan Host CfgMangmnt, ret = %d", ret);
      }
      
      cmsObj_free((void **) &lanHostCfg);   
   }
   else
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);

   return ret;
}


CmsRet cliCmd_showDhcpServer_igd(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostCfgObject *lanHostCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_getNext(MDMOID_LAN_HOST_CFG,
                             &iidStack,
                             (void *) &lanHostCfg)) == CMSRET_SUCCESS)
   {
      printf("dhcpserver: %s\n", lanHostCfg->DHCPServerEnable ? "enable" : "disable");
      printf("start ip address: %s\n", lanHostCfg->minAddress);
      printf("end ip address: %s\n", lanHostCfg->maxAddress);
      printf("leased time: %d hours\n", lanHostCfg->DHCPLeaseTime/3600);         

      cmsObj_free((void **) &lanHostCfg);   
   }
   else
      cmsLog_error("Could not get current lanHostCfg, ret=%d", ret);

   return ret;
}


CmsRet cliCmd_configDhcpServer_igd(UBOOL8 enable)
{
   InstanceIdStack lanHostIidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostCfgObject *lanHostObj = NULL;
   CmsRet ret;

   cmsLog_debug("Entered: enable=%d", enable);

   // CLI can only configure the first LAN bridge
   if ((ret = cmsObj_getNext(MDMOID_LAN_HOST_CFG, &lanHostIidStack, (void **) &lanHostObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Lan Host CfgMangmnt, ret = %d", ret);
      return ret;
   }

   lanHostObj->DHCPServerEnable = (enable ? TRUE : FALSE);
   if ((ret = cmsObj_set(lanHostObj, &lanHostIidStack)) != CMSRET_SUCCESS)
    {
       cmsLog_error("Failed to set Lan Host CfgMangmnt, ret = %d", ret);
    }

   return ret;
}


CmsRet cliCmd_configDhcpClient_igd(UBOOL8 enable)
{
   InstanceIdStack lanHostIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanIpIidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostCfgObject *lanHostObj = NULL;
   LanIpIntfObject *lanIpObj = NULL;
   CmsRet ret;

   cmsLog_debug("Entered: enable=%d", enable);

   // CLI can only configure the first LAN bridge
   if ((ret = cmsObj_getNext(MDMOID_LAN_HOST_CFG, &lanHostIidStack, (void **) &lanHostObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Lan Host CfgMangmnt, ret = %d", ret);
      return ret;
   }

   // Don't need it, so free it now
   cmsObj_free((void **) &lanHostObj);

   // set dhcp client mode on the ip interface object
   if ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF,
                                      &lanHostIidStack,
                                      &lanIpIidStack,
                                      (void **) &lanIpObj)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceAddressingType,
                            (enable ? MDMVS_DHCP : MDMVS_STATIC));
      if((ret = cmsObj_set(lanIpObj, &lanIpIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set LanIpIntfObject, ret=%d", ret);
      }
      cmsObj_free((void **) &lanIpObj);
   }

   return ret;
}

#endif  /* DMP_BASELINE_1 */

static void cmdLanHelp(int argc)
{
   if ( argc <= 1 )
      fprintf(stdout,
         "\nUsage: lan config [--ipaddr <primary|secondary> <IP address> <subnet mask>]\n"
         "                  [--dhcpserver <enable|disable>]\n"
         "                  [--dhcpclient <enable|disable>]\n"
         "       lan delete --ipaddr <primary|secondary>\n"
         "       lan show [<primary|secondary>]\n"
         "       lan --help\n");
   else
      fprintf(stderr, "lan: invalid number of parameters " \
         "for option '--help'\n");
}

void processLanCmd(char *cmdLine)
{
   const int maxOpt=7;
   SINT32 argc = 0;
   char *argv[maxOpt];
   char *last = NULL;
   CmsRet ret=CMSRET_SUCCESS;

   /* parse the command line and build the argument vector */
   argv[0] = strtok_r(cmdLine, " ", &last);
   if (argv[0] != NULL)
   {
      for (argc = 1; argc < maxOpt; argc++)
      {
         argv[argc] = strtok_r(NULL, " ", &last);
         if (argv[argc] == NULL)
            break;
      }
   }

   if (argv[0] == NULL)
   {
      cmdLanHelp(argc);
   }
   else if (strcasecmp(argv[0], "config") == 0)
   {
      SINT32 i;
	  char *ip=NULL, *mask=NULL;
	  SINT32 dhcp = -1, primary = -1;
      SINT32 dhcpClientMode=-1;

	  for(i = 1; i < argc; i++)
	  {
	      if (strcmp(argv[i], "--ipaddr") == 0)
	      {
	         i++;
			 if (i >= argc)
			 {
			    fprintf(stderr, "lan: invalid number of parameters for option 'config'\n");
				return;
			 }
			 if (strcmp(argv[i], "primary") && strcmp(argv[i], "secondary"))
			 {
			    fprintf(stderr, "lan: invalid parameters for option 'config'\n");
				return;
			 }
			 if (strcmp(argv[i], "secondary"))
			    primary = 1;
			 else
			 	primary = 0;
			 
			 i++;
			 if (i >= argc)
			 {
			    fprintf(stderr, "lan: invalid number of parameters for option 'config'\n");
				return;
			 }
			 if (cli_isIpAddress(argv[i]) == FALSE)
			 {
			    fprintf(stderr, "lan: invalid ip address %s for option 'config'\n",argv[i]);
				return;
			 }
			 ip = argv[i];
			 
			 i++;
			 if (i >= argc)
			 {
			    fprintf(stderr, "lan: invalid number of parameters for option 'config'\n");
				return;
			 }
			 if (cli_isIpAddress(argv[i]) == FALSE)
			 {
			    fprintf(stderr, "lan: invalid subnet mask %s for option 'config'\n",argv[i]);
				return;
			 }
			 mask = argv[i];	
	      }
		  else if (strcmp(argv[i], "--dhcpserver") == 0)
		  {
	         i++;
			 if (i >= argc)
			 {
			    fprintf(stderr, "lan: invalid number of parameters for option 'config'\n");
				return;
			 }
			 if (strcmp(argv[i], "enable") && strcmp(argv[i], "disable"))
			 {
			    fprintf(stderr, "lan: invalid parameters for option 'config'\n");
				return;
			 }
			 if (strcmp(argv[i], "disable"))
			    dhcp = 1;
			 else
			 	dhcp = 0;
		  }
         else if (!cmsUtl_strcmp(argv[i], "--dhcpclient"))
         {
            i++;
            if ((i >= argc) || (strcmp(argv[i], "enable") && strcmp(argv[i], "disable")))
            {
               fprintf(stderr, "lan: must specify enable or disable\n");
               return;
            }
            if (primary != -1 || dhcp != -1)
            {
               fprintf(stderr, "lan: cannot specify ipaddr or dhcp server with dhcpclient option\n");
               return;
            }

            dhcpClientMode = strcmp(argv[i], "disable");
         }
		  else
		  {
			 fprintf(stderr, "lan: invalid parameter %s for option 'config'\n", argv[i]);
			 return;
		  }
	  }

      //configure lan or lan2 ip
      if (primary != -1)
      {
         ret = cliCmd_configLanIp((UBOOL8) primary, ip, mask);
      }

      // configure DHCP server
      if (dhcp != -1)
      {
         ret = cliCmd_configDhcpServer((UBOOL8) dhcp);
      }

      // configure DHCP client
      if (dhcpClientMode != -1)
      {
         ret = cliCmd_configDhcpClient((UBOOL8) dhcpClientMode);
      }
   }
   else if (strcasecmp(argv[0], "delete") == 0)
   {  
      if(argc != 3)
      {
         fprintf(stderr, "lan: invalid number of parameters for option 'delete'\n");
         ret = CMSRET_INVALID_ARGUMENTS;
      }
      else if (strcmp(argv[1], "--ipaddr") != 0)
      {
         fprintf(stderr, "lan: invalid parameter %s for option 'delete'\n", argv[1]);
         ret = CMSRET_INVALID_ARGUMENTS;
      }
      else
      {
         if (strcmp(argv[2], "primary") == 0)
         {
            ret = cliCmd_deleteLanIp(TRUE);
         }
         else if (strcmp(argv[2], "secondary") == 0)
         {
            ret = cliCmd_deleteLanIp(FALSE);
         }
         else
         {
            fprintf(stderr, "lan: third arg must be either primary or secondary\n");
            ret = CMSRET_INVALID_ARGUMENTS;
         }
      }
   }
   else if (strcasecmp(argv[0], "show") == 0)
   {  
      if(argc == 1)
      {
	     prctl_runCommandInShellBlocking("ifconfig br0");
	     prctl_runCommandInShellBlocking("ifconfig br0:0");		 	  	
      }
	  else if (argc == 2 && strcmp(argv[1], "primary") == 0)
	     prctl_runCommandInShellBlocking("ifconfig br0");
	  else if (argc == 2 && strcmp(argv[1], "secondary") == 0)
	     prctl_runCommandInShellBlocking("ifconfig br0:0");		 	  	
      else
         fprintf(stderr, "lan: invalid parameters %s for option 'show'\n",argv[1]);
   }
   else if (strcasecmp(argv[0], "--help") == 0)
   {
      cmdLanHelp(argc);
   }
   else 
   {
      printf("\nInvalid option '%s'\n", argv[0]);
      cmdLanHelp(0);
   }

   if (ret != CMSRET_SUCCESS)
   {
      printf("Command failed, ret=%d\n", ret);
   }
   return;
}




static void cmdLanHostsHelp(void)
{
   printf("\nUsage: lanhosts show all\n");
   printf("       lanhosts show brx\n");
   printf("       lanhosts help\n");
}

static void printLanHostsHeader(void)
{
   printf("   MAC Addr          IP Addr     Lease Time Remaining    Hostname\n");
}

static void printHostsInLanDev(const InstanceIdStack *parentIidStack)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostEntryObject *entryObj = NULL;
   CmsRet ret;

   while ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_HOST_ENTRY, parentIidStack, &iidStack, (void **) &entryObj)) == CMSRET_SUCCESS)
   {
      printf("%s  %s        %d              %s\n", entryObj->MACAddress, entryObj->IPAddress, entryObj->leaseTimeRemaining, entryObj->hostName);
      cmsObj_free((void **) &entryObj);
   }
}

void processLanHostsCmd_igd(char *cmdLine)
{
   LanDevObject *lanDevObj=NULL;
   LanIpIntfObject *ipIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2;
   CmsRet ret;

   cmsLog_debug("cmdLine =>%s<=", cmdLine);

   if (!strcasecmp(cmdLine, "show all"))
   {
      while ((ret = cmsObj_getNext(MDMOID_LAN_DEV, &iidStack, (void **) &lanDevObj)) == CMSRET_SUCCESS)
      {
         cmsObj_free((void **) &lanDevObj);

         /* just use the first LAN IP INTF from each LAN Device */
         INIT_INSTANCE_ID_STACK(&iidStack2);
         if ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF, &iidStack, &iidStack2, (void **) &ipIntfObj)) != CMSRET_SUCCESS)
         {
            /* weird, each LANDevice should have at least 1 IP Intf. */
            cmsLog_error("could not find ip intf under LANDevice %s", cmsMdm_dumpIidStack(&iidStack));
         }
         else
         {
            printf("Bridge %s\n", ipIntfObj->X_BROADCOM_COM_IfName);
            cmsObj_free((void **) &ipIntfObj);

            printLanHostsHeader();
            printHostsInLanDev(&iidStack);
         }
      }
   }
   else if (!strncasecmp(cmdLine, "show br", 7))
   {
      printLanHostsHeader();
      ret = dalLan_getLanDevByBridgeIfName(&cmdLine[5], &iidStack, &lanDevObj);
      if (ret != CMSRET_SUCCESS)
      {
         printf("Could not find bridge ifName %s", &cmdLine[5]);
      }
      else
      {
         cmsObj_free((void **) &lanDevObj);
         printHostsInLanDev(&iidStack);
      }
   }
   else
   {
      cmdLanHostsHelp();
   }
}



void processLanHostsCmd(char *cmdLine)
{
#if defined(SUPPORT_DM_LEGACY98)
   processLanHostsCmd_igd(cmdLine);
#elif defined(SUPPORT_DM_HYBRID)
   processLanHostsCmd_igd(cmdLine);
#elif defined(SUPPORT_DM_PURE181)
   processLanHostsCmd_dev2(cmdLine);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2() == TRUE)
      processLanHostsCmd_dev2(cmdLine);
   else
      processLanHostsCmd_igd(cmdLine);
#endif
}



void processPasswdCmd(char *cmdLine)
{
   CmsRet ret;
   char username[BUFLEN_32], password[BUFLEN_48];
   char *pc = NULL;
   NetworkAccessMode accessMode;
   UBOOL8 authSuccess;
   CmsSecurityLogData logData;
   HttpLoginType authLevel = LOGIN_INVALID;   
   
   memset(&logData, 0, sizeof(CmsSecurityLogData));

   if (!strncasecmp(cmdLine, "help", 4) || !strncasecmp(cmdLine, "--help", 6))
   {
      printf("\nUsage: passwd \n");
      printf("       passwd --help\n");
      return;
   }

   username[0] = '\0';
   password[0] = '\0';
   printf("Username: ");

   // When the serial port is not configured, telnet sessions need
   // stdout to be flushed here.
   fflush(stdout);

   // Read username string, while checking for idle timeout
   if ((ret = cli_readString(username, sizeof(username))) != CMSRET_SUCCESS)
   {
      printf("read string failed\n");
      return;
   }

   pc = getpass("Password: ");
   if ( pc == NULL )
   {
      printf("Invalid password\n");
      return;
   }
   strcpy(password, pc);
   bzero(pc, strlen(pc));

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
   }

   if ((ret = cmsDal_getCurrentLoginCfg(&glbWebVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current password info, ret=%d", ret);
      return;
   }

   /* for verification we need to specify the network access
      based on the user who's password is being changed and not
      the user who is changing the password */
   if ( (0 == strcmp(username, glbWebVar.adminUserName)) ||
        (0 == strcmp(username, glbWebVar.usrUserName)) )
   {
      accessMode = NETWORK_ACCESS_LAN_SIDE;
   }
   else if (0 == strcmp(username, glbWebVar.sptUserName))
   {
      accessMode = NETWORK_ACCESS_WAN_SIDE;
   }
   else
   {
      printf("unrecognized username\n");
      return;
   }

   authSuccess = cmsDal_authenticate(&authLevel, accessMode, username, password);

   cmsLck_releaseLock();

   pc = NULL;
   pc = getpass("New Password: ");
   if ( pc == NULL )
   {
      printf("Invalid password\n");
      return;
   }
   strcpy(password, pc);
   bzero(pc, strlen(pc));

   pc = NULL;
   pc = getpass("Confirm New Password: ");
   if ( pc == NULL )
   {
      printf("Invalid password\n");
      return;
   }

   if ( 0 != strcmp(pc, password) )
   {
      cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "New passwords do not match");
      printf("Passwords do not match\n");
      return;
   }
   bzero(pc, strlen(pc));

#ifdef SUPPORT_HASHED_PASSWORDS
   pc = cmsUtil_pwEncrypt(password, cmsUtil_cryptMakeSalt());
#else
   pc = &password[0];
#endif

   if (FALSE == authSuccess)
   {
      cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Invalid username or password - account %s", &username[0]);
      printf("Authentication failed\n");
      return;
   }

   CMSLOG_SEC_SET_APP_NAME(&logData, &currAppName[0]);
   CMSLOG_SEC_SET_USER(&logData, &currUser[0]);
   if (currAppPort != 0 )
   {
      CMSLOG_SEC_SET_PORT(&logData, currAppPort);
      CMSLOG_SEC_SET_SRC_IP(&logData, &currIpAddr[0]);
   }

   if (0 == strcmp(username, glbWebVar.adminUserName))
   {
      if ((currPerm & PERM_ADMIN) == 0)
      {
         cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Account %s", &username[0]);
         printf("You are not allowed to change admin password\n");
         return;
      }

      memset(glbWebVar.adminPassword, 0, sizeof(glbWebVar.adminPassword));
      strncpy(glbWebVar.adminPassword, pc, sizeof(glbWebVar.adminPassword));
   }
   else if (0 == strcmp(username, glbWebVar.sptUserName))
   {
      if ((currPerm & (PERM_ADMIN|PERM_SUPPORT)) == 0)
      {
         cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Account %s", &username[0]);
         printf("You are not allowed to change support password\n");
         return;
      }

      memset(glbWebVar.sptPassword, 0, sizeof(glbWebVar.sptPassword));
      strncpy(glbWebVar.sptPassword, pc, sizeof(glbWebVar.sptPassword));
   }
   else if (0 == strcmp(username, glbWebVar.usrUserName))
   {
      if ((currPerm & (PERM_ADMIN|PERM_USER)) == 0)
      {
         cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Account %s", &username[0]);
         printf("You are not allowed to change user password\n");
         return;
      }

      memset(glbWebVar.usrPassword, 0, sizeof(glbWebVar.usrPassword));
      strncpy(glbWebVar.usrPassword, pc, sizeof(glbWebVar.usrPassword));
   }
   else
   {
      printf("Invalid user name %s\n", username);
      cmsLog_security(LOG_SECURITY_PWD_CHANGE_FAIL, &logData, "Invalid user Account %s", &username[0]);
      return;
   }

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
   }

   if ((ret = cmsDal_setLoginCfg(&glbWebVar)) != CMSRET_SUCCESS)
   {
      printf("Could not set new password info, ret=%d\n", ret);
   }
   else
   {
      cmsLog_security(LOG_SECURITY_PWD_CHANGE_SUCCESS, &logData, "Account %s", &username[0]);
      printf("new password info set.\n");
   }

   cmsLck_releaseLock();

   return;
}


void cmdPppHelp(int argc)
{
   if ( argc <= 1 )
      fprintf(stdout,
         "\nUsage: ppp config <ppp interface name (eg. ppp0)> <up|down>\n"
         "       ppp --help\n"
         "       connect or disconnect ppp\n");
   else
      fprintf(stderr, "ppp: invalid number of parameters " \
         "for option '--help'\n");
}


UBOOL8 isPPPExist_igd(const char *pppName)

{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppConn = NULL;
   UBOOL8 found = FALSE;
   while (!found && 
      cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **)&pppConn) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(((WanPppConnObject *) pppConn)->X_BROADCOM_COM_IfName, pppName))
      {
         found = TRUE;
      }
      cmsObj_free((void **)&pppConn);
   }

   return found;
}



void processPppCmd(char *cmdLine)
{
   const int maxOpt=5;
   SINT32 argc = 0;
   char *argv[maxOpt];
   char *last = NULL;

   /* parse the command line and build the argument vector */
   argv[0] = strtok_r(cmdLine, " ", &last);
   if (argv[0] != NULL)
   {
      for (argc = 1; argc < maxOpt; argc++)
      {
         argv[argc] = strtok_r(NULL, " ", &last);
         if (argv[argc] == NULL)
            break;
      }
   }

   if (argv[0] == NULL)
   {
      cmdPppHelp(argc);
   }
   else if (strcasecmp(argv[0], "config") == 0)
   {  
      SINT32 up;
      
      if (argc != 3)
      {
         fprintf(stderr, "ppp: invalid number of parameters for option 'config'\n");
         return;	  
      }
      // ppp config <ppp interface name> <up|down>
      if (cmsUtl_strcmp(argv[2], "up") == 0)
      {
         up =USER_REQUEST_CONNECTION_UP;
      }
      else if (cmsUtl_strcmp(argv[2], "down") == 0)
      {
         up = USER_REQUEST_CONNECTION_DOWN;
      }
      else
      {
         fprintf(stderr, "ppp: invalid parameter");
         return;	     
      }

      
      if (!isPPPExist(argv[1]))
      {
         fprintf(stderr, "ppp: can not find PPP %s\n", argv[1]);	     
      }      
      else
      {
        if (cli_sendMsgToSsk
           (CMS_MSG_REQUEST_FOR_PPP_CHANGE, up, argv[1], strlen(argv[1])+1) != CMSRET_SUCCESS)
        {
           fprintf(stderr, "Fail to send message to ssk");
        }		 
      }
   }      
   else if (strcasecmp(argv[0], "--help") == 0)
   {
      cmdPppHelp(argc);
   }
   else 
   {
      printf("\nInvalid option '%s'\n", argv[0]);
   }
   return;
}


void processRestoreDefaultCmd(char *cmdLine __attribute__((unused)))
{
   cmsLog_notice("starting user requested restore to defaults and reboot");

   /* this invalidates the config flash, which will cause us to use
    * default values in MDM next time we reboot. */
   cmsMgm_invalidateConfigFlash();

   printf("\r\nThe system shell is being reset. Please wait...\r\n");
   fflush(stdout);

   cmsUtil_sendRequestRebootMsg(cliPrvtMsgHandle);

   return;
}


void processRouteCmd(char *cmdLine)
{
   char *pToken = NULL, *pLast = NULL;;
   char argument[6][BUFLEN_32];
   int i = 0;
   UBOOL8 isGW = FALSE;
   UBOOL8 isDEV = FALSE;
   int isMetric = 0;
   CmsRet ret = CMSRET_SUCCESS;

   argument[0][0] = argument[1][0] = argument[2][0] = argument[3][0] = argument[4][0] = argument[5][0] = '\0';
   
   pToken = strtok_r(cmdLine, ", ", &pLast);
   while ( pToken != NULL ) 
   {
      if (!strcmp(pToken, "gw"))
      {
         isGW = TRUE;
      }
      else if (!strcmp(pToken, "dev"))
      {
         isDEV = TRUE;
      }
      else if (!strcmp(pToken, "metric"))
      {
         isMetric = 1;
      }
      else
      {
         strcpy(&argument[i][0], pToken);
         i++;
      }
	
      pToken = strtok_r(NULL, ", ", &pLast);
   }

   if (!strcasecmp(&argument[0][0], "help") || !strcasecmp(&argument[0][0], "--help") || argument[0][0] == '\0')
   {
      printf("Usage: route add <IP address> <subnet mask> |metric hops| <|<gw gtwy_IP>| |<dev interface>|>\n");
      printf("       route delete <IP address> <subnet mask>\n");
      printf("       route show\n");
      printf("       route help\n");
      return;
   }

   if(!strcasecmp(&argument[0][0], "show"))
   {
      FILE* fs = NULL;

      /* execute command with err output to rterr */
      prctl_runCommandInShellBlocking("route 2> /var/rterr");
	  
      fs = fopen("/var/rterr", "r");
      if (fs != NULL) 
      {
         prctl_runCommandInShellBlocking("cat /var/rterr");
         fclose(fs);
         prctl_runCommandInShellBlocking("rm /var/rterr");
      }

      return;
   }

   if(cmsUtl_isValidIpAddress(AF_INET, &argument[1][0]) == FALSE)
   {
      printf("Invalid destination IP address\n");
      return;
   }

   if(cmsUtl_isValidIpAddress(AF_INET, &argument[2][0]) == FALSE)
   {
      printf("Invalid destination subnet mask\n");
      return;
   }

   if (!strcasecmp(&argument[0][0], "add"))
   {
      if (isGW == FALSE && isDEV == FALSE) 
      {
         printf("Please at least enter gateway IP or interface\n");
         return;
      }
	   
      if ( isGW == TRUE) 
      {
         if (cmsUtl_isValidIpAddress(AF_INET, &argument[3+isMetric][0]) == FALSE) 
         {
            printf("\n Error: Invalid gateway IP address");
            return;
         }

         if ( isMetric )
         {
            ret = dalStaticRoute_addEntry(&argument[1][0], &argument[2][0], &argument[3+isMetric][0], &argument[4+isMetric][0], &argument[3][0]);
         }
         else
         {
            ret = dalStaticRoute_addEntry(&argument[1][0], &argument[2][0], &argument[3+isMetric][0], &argument[4+isMetric][0], NULL);
         }

         if ( ret != CMSRET_SUCCESS )
         {
            cmsLog_error("dalStaticRoute_addEntry failed, ret=%d", ret);
            printf("\nError happens when add the route.\n");
            return;
         }
      }
      else 
      {
         if ( isMetric )
         {
            ret = dalStaticRoute_addEntry(&argument[1][0], &argument[2][0], &argument[4+isMetric][0], &argument[3+isMetric][0], &argument[3][0]);
         }
         else
         {
            ret = dalStaticRoute_addEntry(&argument[1][0], &argument[2][0], &argument[4+isMetric][0], &argument[3+isMetric][0], NULL);
         }

         if ( ret != CMSRET_SUCCESS )
         {
            cmsLog_error("dalStaticRoute_addEntry failed, ret=%d", ret);
            printf("\nError happens when add the route.\n");
            return;
         }
      }
   }
   else if(!strcasecmp(&argument[0][0], "delete")) 
   {
      if ((ret = dalStaticRoute_deleteEntry(&argument[1][0], &argument[2][0])) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dalStaticRoute_deleteEntry failed, ret=%d", ret);
         printf("\nError happens when delete the route.\n");
         return;
      }
   }
   else 
   {
      printf("Invalid route command.\n");
      return;
   }
		
   return;
}



void processSaveCmd(char *cmdLine __attribute__((unused)))
{
   CmsRet ret;

   ret = cmsMgm_saveConfigToFlash();
   if (ret != CMSRET_SUCCESS)
   {
      printf("Could not save config to flash, ret=%d\n", ret);
   }
   else
   {
      printf("config saved.\n");
   }

   return;
}


void processSwVersionCmd_igd(char *cmdLine)
{
   IGDDeviceInfoObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &obj);
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
#ifdef SUPPORT_DSL
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
#ifdef DMP_ADSLWAN_1
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


void processSwVersionCmd(char *cmdLine)
{
#if defined(SUPPORT_DM_LEGACY98)
   processSwVersionCmd_igd(cmdLine);
#elif defined(SUPPORT_DM_HYBRID)
   processSwVersionCmd_igd(cmdLine);
#elif defined(SUPPORT_DM_PURE181)
   processSwVersionCmd_dev2(cmdLine);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2() == TRUE)
      processSwVersionCmd_dev2(cmdLine);
   else
      processSwVersionCmd_igd(cmdLine);
#endif
}


void processUptimeCmd_igd(void)
{
   IGDDeviceInfoObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   UINT32 s=0;
   char uptimeString[BUFLEN_512];

   ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &obj);
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


static void processUptimeCmd(char *cmdLine __attribute__((unused)))
{
#if defined(DMP_BASELINE_1) && !defined(DMP_DEVICE2_BASELINE_1)
   /* legacy TR-98 */
   processUptimeCmd_igd();
#elif defined(DMP_BASELINE_1) && defined(DMP_DEVICE2_BASELINE_1)
   /* hybrid TR-98+TR-181  --or-- DM Switching mode */
   /* for now, assume hybrid, which uses the IGD tree */
   processUptimeCmd_igd();
#else
   /* Pure TR-181 mode */
   processUptimeCmd_dev2();
#endif
}


UINT32 hexStringToBinaryBuf(const char *hexStr, UINT8 *binaryBuf)
{
   UINT32 i = 0, j = 0;
   UINT32 len = strlen(hexStr);
   char tmpbuf[3];

   for (i = 0, j = 0; j < len; i++, j += 2)
   {
      tmpbuf[0] = hexStr[j];
      tmpbuf[1] = hexStr[j+1];
      tmpbuf[2] = 0;

      binaryBuf[i] = (UINT8) strtoul(tmpbuf, NULL, 16);
   }

   return len/2;
}

#ifdef DMP_X_BROADCOM_COM_EPON_1

#define OAM_VENDOR_NUM 8
static char *eponVendorName[OAM_VENDOR_NUM] = {
    "bcm", "ctc", "ntt", "dasan", "dpoe", "kt", "pmc", "cuc"};


static void eponBitVendorStr(unsigned char oamSel, char *vendorStr, int len)
{
    int i;
    
    if ((vendorStr == NULL) || (!oamSel))
        return;

    memset(vendorStr, 0, len);
    for (i = 0; i < OAM_VENDOR_NUM; i ++)
    {
        if (oamSel&(1<<i))
        {
        strcat(vendorStr, eponVendorName[i]);
        strcat(vendorStr, "/");
        }
    }

    //remove the last character '/'.
    i = strlen(vendorStr);
    vendorStr[i-1] = '\0';
}
    
static void dumpXPONObjInfo(void)
{
    XponObject *obj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;
    unsigned char oamSelStr[32];


    /* Get from XPON */ 
    if ((ret = cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
    {
       cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
       cmsLck_releaseLock();
       return;
    }

    printf("XPON Obj Info:\n");

    if (obj->authenticationId != NULL)
    {
        printf("CtcAuthId:\t%s\n", obj->authenticationId);
    }

    if (obj->authenticationPassword != NULL)
    {
        printf("CtcAuthPass:\t%s\n", obj->authenticationPassword);
    }

    if (obj->dpoeFileName != NULL)
    {
        printf("FileName:\t%s\n", obj->dpoeFileName);
    }

    printf("MaxLinks:\t%d\n", obj->maxLinkNum);
    printf("SchMode:\t%d\n", obj->eponSchMode);
    printf("IdleTimeOffset:\t%d\n", obj->eponIdleTimeOffset);

    eponBitVendorStr(obj->oamSelection, (char *)oamSelStr, sizeof(oamSelStr));
    printf("OamSel:\t0x%x, %s\n", obj->oamSelection, oamSelStr);

    if (obj->date != NULL)
        {
        printf("Date:\t\t%s\n", obj->date);
        }

    if (obj->dpoeMfgTime != NULL)
        {
        printf("DpoeMfgTime:\t%s\n", obj->dpoeMfgTime);
        }

    if (obj->extendedId != NULL)
        {
        printf("Extended ID:\t%s\n", obj->extendedId);
        }

    printf("EponTxLaserPowerOff:\t%s\n", 
        obj->eponTxLaserPowerOff?"True":"False");
    printf("FailSafe:\t%d\n", obj->failSafe);
        
    cmsObj_free((void **) &obj);
}

static void saveXponObjInfo(UINT8 type, UINT8 *data, UINT16 len)    
{
    XponObject *obj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    if ((ret = cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
        cmsLck_releaseLock();
        return;
    }

    switch (type)
    {
        case 0: // maxLinkNum
            obj->maxLinkNum = *data;
            break;
            
        case 1: //OamSel            
            obj->oamSelection = *data;
            break;

        case 2: //date
            CMSMEM_FREE_BUF_AND_NULL_PTR(obj->date);
            obj->date = cmsMem_alloc(len+1, ALLOC_ZEROIZE);
            memcpy(obj->date, data, len);
            break;
            
        case 3: //ctcauthid
            CMSMEM_FREE_BUF_AND_NULL_PTR(obj->authenticationId);
            obj->authenticationId = cmsMem_alloc(len+1, ALLOC_ZEROIZE);
            memcpy(obj->authenticationId, data, len);
            break;
            
        case 4: //ctcauthpass
            CMSMEM_FREE_BUF_AND_NULL_PTR(obj->authenticationPassword);
            obj->authenticationPassword = cmsMem_alloc(len+1, ALLOC_ZEROIZE);
            memcpy(obj->authenticationPassword, data, len);
            break;
         case 5: // schmode
            obj->eponSchMode = *data;
            break;
            
        case 6: // idletimeoffset
            obj->eponIdleTimeOffset = *data;
            break;
                       
        case 7: //extendedId
            CMSMEM_FREE_BUF_AND_NULL_PTR(obj->extendedId);
            obj->extendedId = cmsMem_alloc(len+1, ALLOC_ZEROIZE);
            memcpy(obj->extendedId, data, len);
            break;
            
        case 8:
            obj->eponTxLaserPowerOff = *data;
            break;

        case 9:
            obj->failSafe = *data;
            break;

        default:
            break;
    }

    ret = cmsObj_set(obj, &iidStack);
    if (ret != CMSRET_SUCCESS)
        printf("set cms error (%d)\n", ret);

    ret = cmsMgm_saveConfigToFlash();
    if (ret != CMSRET_SUCCESS)
    {
        printf("Could not save config to flash, ret=%d\n", ret);
    }
    
    cmsObj_free((void **) &obj);
}
static UBOOL8 checkDateStr(UINT8 *dataStr, UINT16 len)
{
    UINT16 i;
    if (len > 8)
        return FALSE;

    for(i = 0; i < 8; i++)
    {
        if ((dataStr[i] > '9') || (dataStr[i] < '0'))
            return FALSE;        
    }
    return TRUE;
}
void cmdEponCfgHelp(int argc)
{
    if ( argc <= 1 )
        fprintf(stdout,
         "\nUsage: eponcfg date <YYYMMDD>\n"
         "       eponcfg oamsel <bcm/ctc/ntt/dasan/dpoe/kt/pmc/cuc>\n"
         "       eponcfg ctcauthid <user>\n"
         "       eponcfg ctcauthpass <pass>\n"
         "       eponcfg maxlink <num>\n"   
         "       eponcfg show\n"
         "       eponcfg schmode <0/1/2>\n"
         "       eponcfg idletime <#ns>\n"
         "       eponcfg extendedId <info>\n"      
         "       eponcfg txlaseroff <true/false>\n"
         "       eponcfg failsafe <0/1>\n"
         "       eponcfg --help\n");
    else
        fprintf(stderr, "eponcfg: invalid number of parameters " \
         "for option '--help'\n");
}


static unsigned char eponLookupVendorType(char *vendorStr)
{
    int i;
    unsigned char vendorType = 0;

    for (i = 0; i < OAM_VENDOR_NUM; i ++)
    {
    if (!strncmp(vendorStr, eponVendorName[i], sizeof(eponVendorName[i])))
        {//find the rigth vendor name
        vendorType = 1<<i;
        break;
        }
    }

    return vendorType;
}


void processEponCfgCmd(char *cmdLine)
{
    const int maxOpt=3;
    SINT32 argc = 0;
    char *argv[maxOpt];
    char *last = NULL;
    char dataBuf[BUFLEN_64];

    /* parse the command line and build the argument vector */
    argv[0] = strtok_r(cmdLine, " ", &last);
    if (argv[0] != NULL)
    {
        for (argc = 1; argc < maxOpt; argc++)
        {
        argv[argc] = strtok_r(NULL, " ", &last);
        if (argv[argc] == NULL)
            break;
        }
    }

    if (argv[0] == NULL)
    {
        cmdEponCfgHelp(argc);
    }
    else if (strcasecmp(argv[0], "date") == 0)
    {  
        if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'date'\n");
            return;
        }
        if (checkDateStr((UINT8 *)argv[1], strlen(argv[1])))
            saveXponObjInfo(2, argv[1], strlen(argv[1]));
        else
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'date'\n");
            return;
        }
    }
    else if (strcasecmp(argv[0], "oamsel") == 0)
    {  
        if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'oamsel'\n");
            return;
        }
        else
        {
            char *valPtr = argv[1];
            char *tmpPtr;
            unsigned char vendorMask = 0;
            tmpPtr = strtok_r(valPtr, "/", &last);
            while (valPtr)
                {
                vendorMask |= eponLookupVendorType(valPtr);
                valPtr = tmpPtr;
                tmpPtr = strtok_r(NULL, "/", &last);
                }

            dataBuf[0] = (vendorMask > 0) ? vendorMask : 2;
            saveXponObjInfo(1, (UINT8 *)dataBuf, 1);
        }
    }
    else if (strcasecmp(argv[0], "ctcauthid") == 0)
    {  
        if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'ctcauthid'\n");
            return;
        }
        saveXponObjInfo(3, (UINT8 *)argv[1], strlen(argv[1]));
    }
    else if (strcasecmp(argv[0], "ctcauthpass") == 0)
    {  
        if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'ctcauthpass'\n");
            return;
        }
        saveXponObjInfo(4, (UINT8 *)argv[1], strlen(argv[1]));
    }
    else if (strcasecmp(argv[0], "maxlink") == 0)
    {  
        if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'maxlink'\n");
            return;
        }
        dataBuf[0] = strtoul(argv[1], NULL, 0);
        saveXponObjInfo(0, (UINT8 *)dataBuf, 1);
    }   
    else if (strcasecmp(argv[0], "show") == 0)
    {  
      if (argc != 1)
         fprintf(stderr, "eponcfg: invalid number of parameters " \
               "for option 'show'\n");
      else
      {
          printf("\n");
          dumpXPONObjInfo();
          printf("\n");
      }	
    }
    else if (strcasecmp(argv[0], "schmode") == 0)
    {  
      if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'schmode'\n");
            return;
        }
        dataBuf[0] = strtoul(argv[1], NULL, 0);
        saveXponObjInfo(5, (UINT8 *)dataBuf, 1);
    }
    else if (strcasecmp(argv[0], "idletime") == 0)
    {  
      if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'idletime'\n");
            return;
        }
        dataBuf[0] = strtoul(argv[1], NULL, 0);
        saveXponObjInfo(6, (UINT8 *)dataBuf, 1);
    }
    else if (strcasecmp(argv[0], "extendedId") == 0)
    {  
        if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'extendedId'\n");
            return;
        }
        saveXponObjInfo(7, (UINT8 *)argv[1], strlen(argv[1]));
    }
    else if(strcasecmp(argv[0], "txlaseroff") == 0)
    {
        if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'txlaseroff'\n");
            return;
        }
        else
        {
            if (strcasecmp(argv[1],"true") == 0)
            {
            dataBuf[0] = TRUE;
            }
            else if (strcasecmp(argv[1], "false") == 0)
            {
            dataBuf[0] = FALSE;
            }
            else
            {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "only 'true' or 'false' \n");
            return;
            }
            saveXponObjInfo(8, (UINT8 *)dataBuf, 1);
        }
    }
    else if (strcasecmp(argv[0], "failsafe") == 0)
    {  
        if (argc != 2)
        {
            fprintf(stderr, "eponcfg: invalid number of parameters " \
                "for option 'failsafe'\n");
            return;
        }
        dataBuf[0] = strtoul(argv[1], NULL, 0);
        saveXponObjInfo(9, (UINT8 *)dataBuf, 1);
    }     
    else if (strcasecmp(argv[0], "--help") == 0)
    {
      cmdEponCfgHelp(argc);
    }
    else 
    {
      fprintf(stderr, "\nInvalid option '%s'\n", argv[0]);
    }
    return;
}
#endif



#ifdef SUPPORT_SECURE_BOOT

#define OTP_DEVICE_NAME  "/dev/otp"
int cmdBtrmMrktId(int isSet, int isMid, int id)
{
    int otpFd;
    int rc;

    OTP_IOCTL_PARMS ioctlParms;
    ioctlParms.result = -1;

    otpFd = open(OTP_DEVICE_NAME, O_RDWR);
    if ( otpFd != -1 )
    {
        if (isSet)
        {
            if (isMid)
            {
                /* Fusing a manufacturing market id */
                ioctlParms.action = OTP_MID_BITS;
                ioctlParms.id     = id;
            }
            else 
            {  
                /* Fusing an operator market id */
                ioctlParms.action = OTP_OID_BITS;
                ioctlParms.id     = id;
            }
            rc = ioctl(otpFd, OTP_IOCTL_SET, &ioctlParms);
        } 
        else 
        {  
            if (isMid)
            {
                /* Retrieving the manufacturing market id */
                ioctlParms.action = OTP_MID_BITS;
            }
            else 
            {  
                /* Retrieving the operator market id */
                ioctlParms.action = OTP_OID_BITS;
            }
            rc = ioctl(otpFd, OTP_IOCTL_GET, &ioctlParms);
        }
        
        if (rc < 0)
            printf("%s@%d otp failure\n", __FUNCTION__, __LINE__);
    }
    else
       printf("Unable to open device %s", OTP_DEVICE_NAME);

    close(otpFd);

    return ioctlParms.result;
}


int cmdBtrmSingleBit(int isSet, int isBtrm)
{
    int otpFd;
    int rc;

    OTP_IOCTL_PARMS ioctlParms;
    ioctlParms.result = -1;

    otpFd = open(OTP_DEVICE_NAME, O_RDWR);
    if ( otpFd != -1 )
    {
        if (isBtrm)
            ioctlParms.action = OTP_BTRM_ENABLE_BIT;
        else
            ioctlParms.action = OTP_OPERATOR_ENABLE_BIT;

        if (isSet)
            rc = ioctl(otpFd, OTP_IOCTL_SET, &ioctlParms);
        else
            rc = ioctl(otpFd, OTP_IOCTL_GET, &ioctlParms);

        if (rc < 0)
            printf("%s@%d otp failure\n", __FUNCTION__, __LINE__);
    }
    else
       printf("Unable to open device %s", OTP_DEVICE_NAME);

    close(otpFd);

    return ioctlParms.result;
}


void cmdBtrmCfgHelp(void)
{
    fprintf(stdout,
     "\nUsage: btrmcfg set btrm_enable\n"
     "       btrmcfg set op_enable\n"
     "       btrmcfg set mid <16bit market identifier>\n"
     "       btrmcfg set oid <16bit market identifier>\n"
     "       btrmcfg get btrm_enable\n"
     "       btrmcfg get op_enable\n"
     "       btrmcfg get mid\n"
     "       btrmcfg get oid\n"
     "       btrmcfg --help\n");
}

void processBtrmCfgCmd(char *cmdLine)
{
    const int maxOpt=3;
    SINT32 argc = 0;
    char *argv[maxOpt];
    char *last = NULL;
    int result;

    /* parse the command line and build the argument vector */
    argv[0] = strtok_r(cmdLine, " ", &last);
    if (argv[0] != NULL)
    {
        for (argc = 1; argc < maxOpt; argc++)
        {
        argv[argc] = strtok_r(NULL, " ", &last);
        if (argv[argc] == NULL)
            break;
        }
    }

    if (argv[0] == NULL)
    {
        cmdBtrmCfgHelp();
    }
    else if ((strcasecmp(argv[0], "get") == 0) && (argc > 1))
    {  
        if (strcasecmp(argv[1], "btrm_enable") == 0)
        {
            result = cmdBtrmSingleBit(0,1);
            fprintf(stdout,"\nThe customer bootrom enable otp bit is fused to %d\n", result);
        }
	else if (strcasecmp(argv[1], "op_enable") == 0)
        {
            result = cmdBtrmSingleBit(0,0);
            fprintf(stdout,"\nThe operator enable otp bit is fused to %d\n", result);
        }
        else if (strcasecmp(argv[1], "mid") == 0)
        {
            result = cmdBtrmMrktId(0, 1, 0);
            fprintf(stdout,"\nThe manufacturing market id bits are fused to 0x%x\n", result);
        }
        else if (strcasecmp(argv[1], "oid") == 0)
        {
            result = cmdBtrmMrktId(0, 0, 0);
            fprintf(stdout,"\nThe operator market id bits are fused to 0x%x\n", result);
        }
	else
            cmdBtrmCfgHelp();
    }
    else if ((strcasecmp(argv[0], "set") == 0) && (argc > 1))
    {  
        if (strcasecmp(argv[1], "btrm_enable") == 0)
            cmdBtrmSingleBit(1,1);
	else if (strcasecmp(argv[1], "op_enable") == 0)
            cmdBtrmSingleBit(1,0);
	else if ((strcasecmp(argv[1], "mid") == 0) || (strcasecmp(argv[1], "oid") == 0))
        {
            if (argc > 2)
            {
		sscanf(argv[2], "0x%x", &result);
                if (strcasecmp(argv[1], "mid") == 0)
                    cmdBtrmMrktId(1, 1, result);
                else
                    cmdBtrmMrktId(1, 0, result);
            }
	    else
                cmdBtrmCfgHelp();
        }
	else
            cmdBtrmCfgHelp();
    }
    else 
      cmdBtrmCfgHelp();

    return;
}

#endif /* SUPPORT_SECURE_BOOT */


#ifdef DMP_X_ITU_ORG_GPON_1
void processShowOmciStatsCmd(char *cmdLine __attribute__((unused)))
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciStatsGponOmciStatsObject *obj;

    ret = cmsObj_get(MDMOID_BCM_OMCI_STATS_GPON_OMCI_STATS, &iidStack, 0, (void **) &obj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not get OMCI Statitics object, ret=%d", ret);
    }
    else
    {
        printf("\nGPON OMCI Statistics\n\n");
        printf("rxPackets : %d\n", obj->rxGoodPackets);
        printf("   rxBaseLinePackets  : %d\n", obj->rxBaseLinePackets);
        printf("   rxExtendedPackets  : %d\n", obj->rxExtendedPackets);
        printf("rxErrors  : %d\n",
               obj->rxLengthErrors +
               obj->rxCrcErrors +
               obj->rxOtherErrors);
        printf("   rxLengthErrors : %d\n", obj->rxLengthErrors);
        printf("   rxCrcErrors    : %d\n", obj->rxCrcErrors);
        printf("   rxOtherErrors  : %d\n\n", obj->rxOtherErrors);
        printf("txPackets : %d\n",
               obj->txAvcPackets +
               obj->txResponsePackets +
               obj->txAlarmPackets +
               obj->txRetransmissions);
        printf("   txAttrValChg      : %d\n", obj->txAvcPackets);
        printf("   txResponses       : %d\n", obj->txResponsePackets);
        printf("   txRetransmissions : %d\n", obj->txRetransmissions);
        printf("   txAlarmPackets    : %d\n", obj->txAlarmPackets);
        printf("txErrors  : %d\n", obj->txErrors);

        printf("\n");

        cmsObj_free((void **) &obj);
    }
}

#endif // #ifdef DMP_X_ITU_ORG_GPON_1
#endif /* SUPPORT_CLI_CMD */
