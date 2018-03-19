/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2013:proprietary:standard
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
 * :>
 *
 * $Change: 160447 $
 ***********************************************************************/

/*
 * IEEE1905 control utility
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <ctype.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

#include "i5ctl.h"
#include "ieee1905_datamodel.h"
#include "i5ctl_wlcfg.h"

#include "ieee1905_wlmetric.h"
#include "ieee1905_trace.h"
#include "ieee1905_utils.h"
#include "ieee1905_glue.h"

#if defined(BRCM_CMS_BUILD)
#include "mdm_object.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_lck.h"
#include "cms_mem.h"
#include "cms_msg.h"
#include <string.h>
#endif

static int i5CtlDataModelCmdHandler(void *pCmd, int argc, char *argv[]);
static int i5CtlTraceCmdHandler(void *pCmd, int argc, char *argv[]);
static int i5CtlTraceTimeCmdHandler(void *pCmd, int argc, char *argv[]);
#if defined(SUPPORT_HOMEPLUG)
static int i5CtlPlcCmdHandler(void *pCmd,int argc, char *argv[]);
#endif
#if defined(SUPPORT_IEEE1905_FM)
static int i5CtlFmCmdHandler(void *pCmd,int argc, char *argv[]);
#endif
static int i5CtlStopCmdHandler(void *pCmd, int argc, char *argv[]);
static int i5CtlStartCmdHandler(void *pCmd, int argc, char *argv[]);
static int i5CtlRestartCmdHandler(void *pCmd, int argc, char *argv[]);
static int i5JsonLegHandler(void *pCmd, int argc, char *argv[]);
static int i5CtlNoParamsCmdHandler(void *pCmd, int argc, char *argv[]);
static int i5CtlDisplayAlMac(void *pCmd,int argc, char *argv[]);
static int i5CtlSendMessage (void *pCmd,int argc, char *argv[]);
static int i5CtlSetLqInterval (void *pCmd,int argc, char *argv[]);
static int i5CtlSendMessageBytes (void *pCmd, int argc, char *argv[]);
#if defined(WIRELESS)      
static int i5CtlSetWifiPwdSsid (void *pCmd, int argc, char *argv[]);
static int i5CtlFakeWifiBwPerm (void *pCmd, int argc, char *argv[]);
static int i5CtlReleaseWifiBw (void *pCmd, int argc, char *argv[]);
static int i5CtlFakeWifiBwTemp (void *pCmd, int argc, char *argv[]);
#endif
#if defined(SUPPORT_HOMEPLUG)
static int i5CtlSetPlcPwd (void *pCmd, int argc, char *argv[]);
static int i5CtlFakePlcBwPerm (void *pCmd, int argc, char *argv[]);
static int i5CtlReleasePlcBw (void *pCmd, int argc, char *argv[]);
static int i5CtlFakePlcBwTemp (void *pCmd, int argc, char *argv[]);
#endif
static int i5CtlShowConfig(void *pCmd, int argc, char *argv[]);

t_I5_CTL_CMD i5CtlCmds[] = {
    {"dm",    "Display data model", I5_API_CMD_RETRIEVE_DM, i5CtlDataModelCmdHandler, 0},
    {"tr",    "Enable function call tracing (0-Msg, 1-Tlv, 2-Dm)", I5_API_CMD_TRACE, i5CtlTraceCmdHandler, 3},
    {"tsenable",    "Toggle timestamps on call tracing (0-off, 1-on)", I5_API_CMD_TRACE_TIME, i5CtlTraceTimeCmdHandler, 0},
#if defined(WIRELESS)      
    {"wlcfg", "Wifi Autoconfig UnitTesting", I5_API_CMD_WLCFG, i5CtlWlCfgCmdHandler, 1},
#endif      
#if defined(SUPPORT_HOMEPLUG)      
    {"plc",   "PLC actions (0 - start UKE, 1 - randomize)", I5_API_CMD_PLC, i5CtlPlcCmdHandler, 2},
#endif      
#if defined(SUPPORT_IEEE1905_FM)
    {"fmshow", "Display Flow Manager Database", I5_API_CMD_FLOWSHOW, i5CtlFmCmdHandler, 0},
#endif
    {"stop", "Stop ieee1905 daemon", I5_API_CMD_STOP, i5CtlStopCmdHandler, 0},
    {"start", "Start ieee1905 daemon", I5_API_CMD_START, i5CtlStartCmdHandler, 0},
    {"restart", "Stop and start ieee1905 daemon", I5_API_CMD_STOP, i5CtlRestartCmdHandler, 0},
    {"link", "Trigger Link Metric Queries", I5_API_CMD_LINKUPDATE, i5CtlNoParamsCmdHandler, 0},
    {"jsonLegacy", "JSON reports Legacy (0-off, 1-on)", I5_API_CMD_JSON_LEG, i5JsonLegHandler, 2},
    {"pushButton", "Simulate Security button push", I5_API_CMD_PUSH_BUTTON, i5CtlNoParamsCmdHandler, 0},
    {"showAlMac", "Display the 1905 AL MAC Address", I5_API_CMD_SHOW_AL_MAC, i5CtlDisplayAlMac, 0},
    {"sendMsg", "Send a 1905 Message (MA:CA:DD:RE:SS:xx msgId)", I5_API_CMD_SEND_MESSAGE, i5CtlSendMessage, 3},
    {"linkInterval", "Set Auto Link Query Interval in msec", I5_API_CMD_SET_LQ_INTERVAL, i5CtlSetLqInterval, 2},
    {"sendMsgBytes", "Send a raw Msg [-i MA:CA:DD:RE:SS:xx] bytes ", I5_API_CMD_SEND_BYTES, i5CtlSendMessageBytes, 0xFFFF},
#if defined(WIRELESS)      
    {"setWiFiPass", "Set Wi-Fi password, SSID [interface]", I5_API_CMD_SET_WIFI_PASS_SSID, i5CtlSetWifiPwdSsid, 3},
    {"setWiFiBwPerm", "Override Wi-Fi bandwidth, Available BW(Mbps), Total BW(Mbps)", I5_API_CMD_SET_WIFI_OVERRIDE_BW, i5CtlFakeWifiBwPerm, 3},
    {"releaseWiFiBw", "Stop override of Wi-Fi bandwidth", I5_API_CMD_SET_WIFI_RELEASE_BW, i5CtlReleaseWifiBw, 0},
    {"setWiFiBwOnce", "Override Wi-Fi bandwidth once, Available BW(Mbps), Total BW(Mbps)", I5_API_CMD_SET_WIFI_BOUNCE_BW, i5CtlFakeWifiBwTemp, 3},
#endif
#if defined(SUPPORT_HOMEPLUG)      
    {"setPlcPass", "Set PLC password, generate NMK", I5_API_CMD_SET_PLC_PASS_NMK, i5CtlSetPlcPwd, 2},
    {"setPlcBwPerm", "Override PLC bandwidth, Available BW(Mbps), Total BW(Mbps)", I5_API_CMD_SET_PLC_OVERRIDE_BW, i5CtlFakePlcBwPerm, 3},
    {"releasePlcBw", "Stop override of PLC bandwidth", I5_API_CMD_SET_PLC_RELEASE_BW, i5CtlReleasePlcBw, 0},
    {"setPlcBwOnce", "Override PLC bandwidth once, Available BW(Mbps), Total BW(Mbps)", I5_API_CMD_SET_PLC_BOUNCE_BW, i5CtlFakePlcBwTemp, 3},
#endif
    {"dumpMsgs", "Show Messages", I5_API_CMD_SHOW_MSGS, i5CtlNoParamsCmdHandler, 0},
    {"dumpSockets", "Show Socket List", I5_API_CMD_SHOW_SOCKETS, i5CtlNoParamsCmdHandler, 0},
    {"showConfig", "Show configuration", I5_API_CMD_GET_CONFIG, i5CtlShowConfig, 0},
};

typedef struct t_i5_ctl_tr_info
{
  char              traceName[16];
  unsigned int      traceModule;
} t_I5_CTL_TR_INFO;

t_I5_CTL_TR_INFO i5CtlTraceInfo[] = {
   {"msg", i5TraceMessage},
   {"tlv", i5TraceTlv},
   {"dm",  i5TraceDm},
   {"if",  i5TraceInterface},
   {"flow", i5TraceFlow},
   {"timer", i5TraceTimer},
   {"socket", i5TraceSocket},
   {"main", i5TraceMain},
   {"security", i5TraceSecurity},
   {"udp", i5TraceUdpSocket},
   {"plc", i5TracePlc},
   {"wl", i5TraceWlcfg},
   {"ctl", i5TraceControl},
   {"netlink", i5TraceNetlink},
   {"json", i5TraceJson},
   {"ethstat", i5TraceEthStat},
   {"brutil", i5TraceBrUtil},
   {"glue", i5TraceGlue},
   {"cms", i5TraceCmsUtil},
   {"dbg", i5TraceNoMod},
   {"all", 255},
   {"packet", i5TracePacket} 
};

static void i5CtlShowUsage(char *prog_name)
{
    int i;
    printf("Usage: %s <option>\n", prog_name);
    for (i = 0; i < (sizeof(i5CtlCmds)/sizeof(i5CtlCmds[0])); i++) {
        printf("        %s  -%s\n", i5CtlCmds[i].cmdstr, i5CtlCmds[i].description);
    }
}

t_I5_CTL_CMD *i5CtlCmdLookup(const char *cmdName)
{
    int i;
    for (i = 0; i < (sizeof(i5CtlCmds)/sizeof(i5CtlCmds[0])); i++) {
        if (!strcmp(cmdName, i5CtlCmds[i].cmdstr)) {
            return &i5CtlCmds[i];
        }
    }

    return NULL;
}

void i5CtlDataModelAlMacPrint(unsigned char *pBuf)
{
  printf("AL_ID=" I5_MAC_DELIM_FMT "\n", I5_MAC_PRM(pBuf));
}

void i5CtlDataModelPrint(char *pBuf)
{ 
  int devNum;
  int entryNum;
  i5_dm_network_topology_type *pDmNet = (i5_dm_network_topology_type *)pBuf;

  pBuf += sizeof(i5_dm_network_topology_type);
  for( devNum = 0; devNum < pDmNet->DevicesNumberOfEntries; devNum++) {
    i5_dm_device_type *device = (i5_dm_device_type *)pBuf;

    printf("\nDevice: " I5_MAC_DELIM_FMT " \"%s\"\n", 
           I5_MAC_PRM (device->DeviceId), device->friendlyName);
    pBuf += sizeof(i5_dm_device_type);
    
    for( entryNum = 0; entryNum < device->InterfaceNumberOfEntries; entryNum++) {
      i5_dm_interface_type *interfaceType = (i5_dm_interface_type *)pBuf;
      printf("  Interface: " I5_MAC_DELIM_FMT " %s (%s)%s", I5_MAC_PRM(interfaceType->InterfaceId),
                                                                                 (interfaceType->Status == 2) ? "DOWN" : "UP",
                                                                                 i5UtilsGetNameForMediaType(interfaceType->MediaType),
                                                                                 (interfaceType->MediaType == I5_MEDIA_TYPE_UNKNOWN) ? "" : "\n" );
      if (interfaceType->MediaType == I5_MEDIA_TYPE_UNKNOWN) {
        printf("  OUI 0x(%x:%x:%x) VARIANT 0x(%x) '%s'\n", interfaceType->netTechOui[0],  interfaceType->netTechOui[1], interfaceType->netTechOui[2],
                                                           interfaceType->netTechVariant, interfaceType->netTechName);
      }
      pBuf += sizeof(i5_dm_interface_type);
    }
    
    for( entryNum = 0; entryNum < device->LegacyNeighborNumberOfEntries; entryNum++) {
      i5_dm_legacy_neighbor_type *legacy = (i5_dm_legacy_neighbor_type *)pBuf;
      printf("  Legacy Neighbor: LcIf " I5_MAC_DELIM_FMT " , ", I5_MAC_PRM(legacy->LocalInterfaceId));
      printf("NbIf " I5_MAC_DELIM_FMT "\n", I5_MAC_PRM(legacy->NeighborInterfaceId));
      pBuf += sizeof(i5_dm_legacy_neighbor_type);
    }

    for( entryNum = 0; entryNum < device->Ieee1905NeighborNumberOfEntries; entryNum++) {
      i5_dm_1905_neighbor_type *neighbor = (i5_dm_1905_neighbor_type *)pBuf;
      printf("  Neighbor: LcIf " I5_MAC_DELIM_FMT ", ", I5_MAC_PRM (neighbor->LocalInterfaceId));
      printf("NbAL " I5_MAC_DELIM_FMT ", ", I5_MAC_PRM (neighbor->Ieee1905Id));
      printf("NbIf " I5_MAC_DELIM_FMT ", Bridge %d, (via %s/%d) (%d/%d/%d)Mbps\n", I5_MAC_PRM(neighbor->NeighborInterfaceId), 
                                                              neighbor->IntermediateLegacyBridge, 
                                                              neighbor->localIfname, neighbor->localIfindex,
                                                              neighbor->MacThroughputCapacity - neighbor->availableThroughputCapacity,
                                                              neighbor->availableThroughputCapacity, neighbor->MacThroughputCapacity);
      pBuf += sizeof(i5_dm_1905_neighbor_type);
    }

    for( entryNum = 0; entryNum < device->BridgingTuplesNumberOfEntries; entryNum++) {
      int i;
      i5_dm_bridging_tuple_info_type *bridging = (i5_dm_bridging_tuple_info_type *)pBuf;
      printf("  Bridging Tuple (%s)\n", bridging->ifname);
      printf("    Interfaces:\n");
      for( i=0; i<bridging->forwardingInterfaceListNumEntries*6; i+=6 ) {
        printf("          " I5_MAC_DELIM_FMT "\n", I5_MAC_PRM(&bridging->ForwardingInterfaceList[i]));
      }
      pBuf += sizeof(i5_dm_bridging_tuple_info_type);
    }
  }
}

static int i5CtlDataModelCmdHandler(void *pCmd,int argc, char *argv[])
{
  t_I5_CTL_CMD *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  void *pBuf = 0;
  int rc;

  rc = i5apiTransaction(pI5Cmd->cmd, 0, 0, &pBuf, 0);
  if ( rc != -1 ) {
    i5CtlDataModelPrint(pBuf);
    free(pBuf);
  }
   
  return 0;
}

//I5_API_CMD_TRACE_TIME
static int i5CtlTraceTimeCmdHandler(void *pCmd, int argc, char *argv[])
{
    int                 sd;
    int                 value;
    int                 rc;
    
    sd = i5apiOpen();
    t_I5_CTL_CMD       *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
    
    if (-1 == sd) {
      return sd;
    }  

    if (argc > 3)
        return -1;

    if (argc == 3)
        value = atoi(argv[2]);
    else 
        value = 2;
    
    rc = i5apiSendMessage(sd, pI5Cmd->cmd, &value, sizeof(value));
    if ( -1 == rc ) {
      printf("Failed to send data to daemon\n");
    }
    i5apiClose(sd);  
    return 0;    
}

static int i5CtlTraceCmdHandler(void *pCmd,int argc, char *argv[])
{
  t_I5_CTL_CMD       *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  t_I5_API_TRACE_MSG  msg;
  int                 sd;
  int                 rc;
  int                 i;

  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }
  
  msg.depth = atoi(argv[3]);
  msg.ifindex = 0;
  msg.module_id = -1;

  if (isdigit(argv[2][0])) {
    msg.module_id = atoi(argv[2]);
    msg.depth = atoi(argv[3]);
  }
  else {
    if ( strlen(argv[2]) < 2 ) {
      i5apiClose(sd);
      return -1;
    }

    for (i = 0; i < (sizeof(i5CtlTraceInfo)/sizeof(i5CtlTraceInfo[0])); i++) {
        int cmplen = (strlen(argv[2]) > strlen(i5CtlTraceInfo[i].traceName)) ? strlen(i5CtlTraceInfo[i].traceName) : strlen(argv[2]);
        if (0 == strncmp(argv[2], i5CtlTraceInfo[i].traceName, cmplen)) {
            msg.module_id = i5CtlTraceInfo[i].traceModule;
            break;
        }
    }

    if ((msg.module_id == i5TracePacket) && (argc == 5)) {
        msg.ifindex = if_nametoindex(argv[4]);
    }
  }
   
  rc = i5apiSendMessage(sd, pI5Cmd->cmd, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}

#if defined(SUPPORT_HOMEPLUG)
static int i5CtlPlcCmdHandler(void *pCmd,int argc, char *argv[])
{
  t_I5_CTL_CMD     *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  t_I5_API_PLC_MSG  msg;
  int               sd;
  int               rc;

  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  msg.subcmd     = atoi(argv[2]);

  rc = i5apiSendMessage(sd, pI5Cmd->cmd, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}
#endif

#if defined(SUPPORT_IEEE1905_FM)
static int i5CtlFmCmdHandler(void *pCmd,int argc, char *argv[])
{
  t_I5_CTL_CMD *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  int           sd;
  int           rc;

  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  rc = i5apiSendMessage(sd, pI5Cmd->cmd, 0, 0);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }

  i5apiClose(sd);  
   
  return 0;   
}
#endif /* defined(SUPPORT_IEEE1905_FM) */


int main(int argc, char *argv[])
{
  t_I5_CTL_CMD *pCmd;

  if (argc == 1) {
    i5CtlShowUsage(argv[0]);
    return -1;
  }

  pCmd = i5CtlCmdLookup(argv[1]);
  if (pCmd == NULL) {
    printf("unknown command [%s]\n", argv[1]);
    i5CtlShowUsage(argv[0]);
    return -1;
  }

  /* 0xFFFF will mean "any number of arguments */
  if ((argc < pCmd->nargs + 1) && (pCmd->nargs != 0xFFFF)) {
    printf("incorrect number of arguments\n");
    i5CtlShowUsage(argv[0]);
    return -1;
  }

  /* found a match - call handler */
  pCmd->func((void *)pCmd, argc, &argv[0]);

  return 0;
}

static int _i5CtlIsDaemonRunning( ) {
  DIR        *dir;
  int         retPid = -1;

  dir = opendir("/proc");
  if (dir) {
    struct dirent* de = 0;
    int pid;
    int res;
    
    while ((de = readdir(dir)) != 0) {
      if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
        continue;
      }

      res = sscanf(de->d_name, "%d", &pid);
      if (res == 1) {
        char  buf[32] = {0};
        FILE *cmdline;
        
        snprintf(buf, 32, "/proc/%d/cmdline", pid);
        cmdline = fopen(buf, "r");
        if ( cmdline != NULL ) {
          if (fgets(&buf[0], 32, cmdline) != NULL ) {
            if (strstr(buf, "ieee1905") != 0) {
              retPid = pid;
              fclose(cmdline);
              break;
            }
          }
          fclose(cmdline);
        }
      }
    }
    closedir(dir);
  }
  return retPid;
}

static int _i5CtlStartArguments2String( int argc, char *argv[], char *pBuf, int maxLen, int reformatArgs )
{
  int i;
  int retVal = -1;

  pBuf[0] = '\0';
  if ( reformatArgs ) {
    int depth = 0;
    int moduleId = -1;
    int ifIndex = 0;
    char *str;
    char *endptr;

    while ( 1 ) {
      if ( argc < 4 ) {
        break;
      }

      str = argv[3];
      errno = 0;
      depth = strtol(str, &endptr, 10);
      if ( (errno != 0) || (endptr == str)) {
        break;
      }

      str = argv[2];
      moduleId = strtol(str, &endptr, 10);
      if ( errno != 0 ) {
        break;
      }

      /* digit not found - look for string match */
      if ( endptr == str ) {
        moduleId = -1;
        if ( strlen(argv[2]) < 2 ) {
          break;
        }

        for (i = 0; i < (sizeof(i5CtlTraceInfo)/sizeof(i5CtlTraceInfo[0])); i++) {
          int cmplen = (strlen(argv[2]) > strlen(i5CtlTraceInfo[i].traceName)) ? strlen(i5CtlTraceInfo[i].traceName) : strlen(argv[2]);
          if (0 == strncmp(argv[2], i5CtlTraceInfo[i].traceName, cmplen)) {
            moduleId = i5CtlTraceInfo[i].traceModule;
            break;
          }
        }
      }

      if ((moduleId == i5TracePacket) && (argc == 5)) {
        ifIndex = if_nametoindex(argv[4]);
      }
      retVal = 0;
      break;
    }

    if ( moduleId != -1 ) {
      snprintf(pBuf, maxLen, "-t m=%d,l=%d,i=%d", moduleId, depth, ifIndex);
    }
  }
  else {
    int bufIdx = 0;
    /* ignore first 2 arguments */
    for (i = 2; i < argc; i++) {
      bufIdx += snprintf(pBuf+bufIdx, maxLen-bufIdx, "%s%s", argv[i], i<argc-1?" ":"");
      if ( bufIdx >= (maxLen-1) ) {
        break;
      }
    }
    retVal = 0;
  }

  return retVal;
}

#if defined(BRCM_CMS_BUILD)
static int i5Start1905FromSmd(void *parms, int length) {
    CmsMsgHeader *msgHdr;
    CmsRet ret = CMSRET_INTERNAL_ERROR;
    void *msgHandle=NULL;

    if ( (msgHdr = cmsMem_alloc( sizeof(CmsMsgHeader) + length,
                                 ALLOC_ZEROIZE)) == NULL)
    {
      printf("%s.%d: cmsMem_alloc returned NULL\n", __func__, __LINE__);
      return -1;
    }
    
    if ((ret = cmsMsg_initWithFlags(EID_I5CTL, 0, &msgHandle)) != CMSRET_SUCCESS)
    {
       printf("%s.%d: cmsMsg_initWithFlags returned %d\n", __func__, __LINE__, ret);
       return -1;
    }
    msgHdr->src = EID_I5CTL;
    msgHdr->dst = EID_SMD;
    msgHdr->type = CMS_MSG_START_APP;
    msgHdr->wordData = EID_1905;
    msgHdr->dataLength = length;
    memcpy((char *)(msgHdr+1), parms, length);

    ret = cmsMsg_sendAndGetReplyWithTimeout(msgHandle, msgHdr, (5*MSECS_IN_SEC));
    if (ret == CMS_INVALID_PID)
    {
        printf("Failed to start 1905, ret=%d", ret);
        ret = -1;
    }
    else
    {
        printf("Started ieee1905, pid %d\n", ret);
        ret = 0;
    }
    cmsMsg_cleanup(&msgHandle);
    cmsMem_free(msgHdr);
    return ret;
}
#endif

static int i5CtlStopCmdHandler(void *pCmd,int argc, char *argv[])
{
  pid_t         pid;
  t_I5_CTL_CMD *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  int           sd;
  int           rc = 0;
  int           waitCount = 10;

  pid = _i5CtlIsDaemonRunning();
  if ( pid > 0 ) {
    printf("Stopping ieee1905 process with pid %d\n", pid);
    sd = i5apiOpen();
    if (-1 == sd) {
      return sd;
    }
    rc = i5apiSendMessage(sd, pI5Cmd->cmd, 0, 0);
    if ( -1 == rc ) {
      printf("Failed to stop ieee1905_process\n");
      i5apiClose(sd);
      return rc;
    }
    i5apiClose(sd);

    while ( waitCount > 0 ) {
      pid = _i5CtlIsDaemonRunning();
      if ( pid < 0 ) {
        break;
      }
      sleep(1);
      waitCount--;
    }
    if ( waitCount == 0 ) {
      printf("Failed to stop ieee1905\n");
      return -1;
    }
  }
  else {
    printf("Ieee1905 process already stopped\n");
  }

  return rc;
}

static int i5CtlStartCmdHandler(void *pCmd,int argc, char *argv[])
{
  char        parms[128];
  int         c;
  int         pid;
#if !defined(BRCM_CMS_BUILD)
  struct stat statbuf;
  int         waitCount = 10;
  char        cmd[128+32];
#endif

  pid = _i5CtlIsDaemonRunning();
  if ( pid > 0 ) {
    printf("ieee1905 already running\n");
    return 0;
  }

  /* The start command supports two formats:
     i5 start -t m=0,l=1,i=0 
        -- multiple trace options can be specified
        -- allows other options to be specified
     i5 start <mod index> <level> <interface> 
        -- simpler format but only one trace option can be specified
        -- no other options can be specified
  */
  memset(parms, 0, 128);
  c = getopt(argc, argv, "t:");

  /* parms will be NULL terminated */
  _i5CtlStartArguments2String(argc, argv, parms, 128, ((c == -1) ? 1 : 0));

#if defined(BRCM_CMS_BUILD)
  return i5Start1905FromSmd(parms, strlen(parms)+1);;
#else
  if ((stat("", &statbuf)) == 0) {
    snprintf(cmd, 128+32, "./ieee1905 %s &", parms);
  }
  else {
    snprintf(cmd, 128+32, "/bin/ieee1905 %s &", parms);
  }
  printf("Starting %s\n", cmd);
  system(cmd);

  while ( waitCount > 0 ) {
    pid = _i5CtlIsDaemonRunning();
    if ( pid > 0 ) {
      break;
    }
    sleep(1);
    waitCount--;
  }
  if ( waitCount == 0 ) {
    printf("Failed to start ieee1905\n");
    return -1;
  }

  return 0;
#endif
}

static int i5CtlRestartCmdHandler(void *pCmd,int argc, char *argv[])
{
  i5CtlStopCmdHandler(pCmd, argc, argv);
  i5CtlStartCmdHandler(pCmd, argc, argv);

  return 0;
}

static int i5JsonLegHandler(void *pCmd, int argc, char *argv[])
{
  t_I5_CTL_CMD     *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  t_I5_API_PLC_MSG  msg;
  int               sd;
  int               rc;

  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  msg.subcmd = atoi(argv[2]);

  rc = i5apiSendMessage(sd, pI5Cmd->cmd, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}

static int i5CtlNoParamsCmdHandler(void *pCmd, int argc, char *argv[])
{
  t_I5_CTL_CMD *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  int           sd;
  int           rc = 0;

  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  rc = i5apiSendMessage(sd, pI5Cmd->cmd, 0, 0);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }

  i5apiClose(sd);  
   
  return rc;     
}

static int i5CtlDisplayAlMac(void *pCmd,int argc, char *argv[])
{
  t_I5_CTL_CMD *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  void *pBuf = 0;
  int rc;

  rc = i5apiTransaction(pI5Cmd->cmd, 0, 0, &pBuf, 0);
  if ( rc != -1 ) {
    i5CtlDataModelAlMacPrint(pBuf);
  }
  free(pBuf);
   
  return 0;
}

static int i5CtlSendMessage (void *pCmd,int argc, char *argv[])
{
  t_I5_API_GOLDEN_NODE_SEND_MSG  msg;
  
  if (NULL == i5String2MacAddr(argv[2], msg.macAddr)) {
     printf("MAC Address must be of form xx:xx:xx:xx:xx:xx\n");
     return -1;
  } 
  else {
    t_I5_CTL_CMD                   *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
    void *pBuf = 0;
    int rc;

    msg.messageId = atoi(argv[3]);

    rc = i5apiTransaction(pI5Cmd->cmd, &msg, sizeof msg, &pBuf, 0);
    if ( rc != -1 ) {
      t_I5_CTL_CMDRESP_SEND_MSG_TYPE *response = pBuf;
      i5CtlDataModelAlMacPrint(response->srcMacAddr);
      printf("MID=%d\n",response->messageId);
      free (pBuf);
    }
    else {
      printf("Failed to send data to daemon\n");
    }
  }
  return 0;
}

static int i5CtlSetLqInterval (void *pCmd,int argc, char *argv[])
{
  t_I5_CTL_CMD                   *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  t_I5_API_LINK_METRIC_INTERVAL  msg;
  int sd;
  int rc;
  
  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  msg.intervalMsec = atoi(argv[2]);

  rc = i5apiSendMessage(sd, pI5Cmd->cmd, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}

/*
 * Format
 * ncap [0]5 [-i xx:xx:xx:xx:xx:xx] bytes ....
 *
 * When -i is present, extract MAC ADDR and put in packet
 *                   , otherwise send all zeroes for MAC ADDR
 * If command parse, open standard in
 *                 , parse standard in's text as hex bytes and load into message
 */
static int i5CtlSendMessageBytes (void *pCmd, int argc, char *argv[])
{
  t_I5_CTL_CMD                         *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  unsigned char                        outputInterface[MAC_ADDR_LEN] = {};
  t_I5_API_GOLDEN_NODE_SEND_MSG_BYTES  *msg = NULL;
  unsigned int                         msgSize = 0;
  unsigned int                         copyIndex = 0;
  int sd;

  char buf[100]; /* 32 bytes * 3 chars each + overhead */
  unsigned char packet[1500];
  unsigned int packetSize = 0;

  if (argc > 3) {
    int argIndex = 2; /* first possible location for a switch */
    /* check if user wants to specify device */
    while (argIndex < argc) {
      if (strncmp(argv[argIndex], "-i", 2) == 0) {
        if (argc > argIndex + 1) {
          if (NULL == i5String2MacAddr(argv[argIndex+1], outputInterface)) {
            printf("MAC Address must be of form xx:xx:xx:xx:xx:xx\n");
            return -1;
          } 
          else {
            /* MAC address found and parsed */
            printf("MAC Address =" I5_MAC_DELIM_FMT "\n", I5_MAC_PRM(outputInterface) );
            break;
          }
        }
      }
      argIndex ++;
    }
  }

  while (feof(stdin) == 0) {
    int bytesFromThisLine = 0;
    if (fgets(buf, sizeof(buf), stdin) != NULL ) {
      int parseIndex = 0;
      
      while ((parseIndex < strlen(buf)) && (bytesFromThisLine < 16)) {
        
        char *nextSpace = strchr(&buf[parseIndex], (int)' ');
        int fieldLength = 1; /* in case the "nextSpace" is NULL, read whatever is left */

        if (NULL != nextSpace) {
          fieldLength = nextSpace - &buf[parseIndex];
        }
        
        if ((fieldLength > 0) && (fieldLength < 3) && (1 == sscanf(&buf[parseIndex], "%hhx", &packet[packetSize]))) {
          bytesFromThisLine++;
          packetSize ++;
        }

        if (NULL == nextSpace) {
          /* returning NULL means we hit end of string without finding a space, so we're done */
          break;
        }
        parseIndex += fieldLength + 1; /* go one character past the space */
      }
      
    }
  }

  msgSize = MAC_ADDR_LEN + packetSize;
  msg = malloc(msgSize);
  if (NULL == msg) {
    printf("Unable to allocate msg\n");
    return -1;
  }
  
  memcpy (msg->macAddr, outputInterface, MAC_ADDR_LEN);
  for ( ; copyIndex < packetSize; copyIndex++) {
    msg->message[copyIndex] = (unsigned char)packet[copyIndex];
  }
  
  sd = i5apiOpen();
  if (-1 == sd) {
    free(msg);
    return sd;
  }

  if ( -1 == i5apiSendMessage(sd, pI5Cmd->cmd, msg, msgSize) ) {
    printf("Failed to send data to daemon\n");
  } 
  else {
    printf("OK\n");
  }
  i5apiClose(sd);  

  free(msg);
   
  return 0;
}

#if defined(WIRELESS)      
static int i5CtlSetWifiPwdSsid (void *pCmd, int argc, char *argv[])
{
  t_I5_CTL_CMD               *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  t_I5_API_PASSWORD_SSID_MSG msg;
  int sd;
  int rc;

  if ( argc < 4 ) {
    return -1;
  }

  if (strlen(argv[2]) > I5_PASSWORD_MAX_LENGTH) {
    return -1;
  }
  if (strlen(argv[3]) > I5_SSID_MAX_LENGTH) {
    return -1;
  }

  if ( argc == 5 ) {
    strncpy(msg.ifname, argv[4], I5_MAX_IFNAME-1);
    msg.ifname[I5_MAX_IFNAME-1] = '\0';
  }
  else {
    snprintf(msg.ifname, I5_MAX_IFNAME, "%s0", I5_GLUE_WLCFG_WL_NAME_STRING);
  }
  
  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  strncpy((char *)msg.password, argv[2], I5_PASSWORD_MAX_LENGTH);
  msg.password[I5_PASSWORD_MAX_LENGTH] = '\0';
  strncpy((char *)msg.ssid,     argv[3], I5_SSID_MAX_LENGTH);
  msg.ssid[I5_SSID_MAX_LENGTH] = '\0';

  rc = i5apiSendMessage(sd, pI5Cmd->cmd, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}

static int i5CtlFakeWifiBwPerm (void *pCmd, int argc, char *argv[])
{
  t_I5_API_OVERRIDE_BW_MSG   msg;
  int sd;
  int rc;

  if (argc < 4) {
    printf("Insufficient arguments\n");
    return -1;
  }

  msg.overrideCount = 0xff; // permanent override of bandwidth
  msg.availBwMbps = atoi(argv[2]);
  msg.macThroughBwMbps = atoi(argv[3]);
  
  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  rc = i5apiSendMessage(sd, I5_API_CMD_SET_WIFI_OVERRIDE_BW, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}

static int i5CtlReleaseWifiBw (void *pCmd, int argc, char *argv[])
{
  t_I5_API_OVERRIDE_BW_MSG   msg;
  int sd;
  int rc;

  msg.overrideCount = 0; // No override
  msg.availBwMbps = 0;
  msg.macThroughBwMbps = 0;
  
  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  rc = i5apiSendMessage(sd, I5_API_CMD_SET_WIFI_OVERRIDE_BW, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}

static int i5CtlFakeWifiBwTemp (void *pCmd, int argc, char *argv[])
{
  t_I5_API_OVERRIDE_BW_MSG   msg;
  int sd;
  int rc;

  if (argc < 4) {
    printf("Insufficient arguments\n");
    return -1;
  }

  msg.overrideCount = 1; // single override of bandwidth
  msg.availBwMbps = atoi(argv[2]);
  msg.macThroughBwMbps = atoi(argv[3]);
  
  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  rc = i5apiSendMessage(sd, I5_API_CMD_SET_WIFI_OVERRIDE_BW, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}      
#endif  // defined (WIRELESS)

#if defined(SUPPORT_HOMEPLUG)      
static int i5CtlSetPlcPwd (void *pCmd, int argc, char *argv[])
{
  t_I5_CTL_CMD               *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  t_I5_API_PASSWORD_SSID_MSG msg;
  int sd;
  int rc;

  if (strlen(argv[2]) > I5_PASSWORD_MAX_LENGTH) {
    return -1;
  }
  
  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  strncpy((char *)msg.password, argv[2], I5_PASSWORD_MAX_LENGTH);
  msg.password[I5_PASSWORD_MAX_LENGTH] = '\0';
  memset(msg.ssid, 0, I5_SSID_MAX_LENGTH+1);

  rc = i5apiSendMessage(sd, pI5Cmd->cmd, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}

static int i5CtlFakePlcBwPerm (void *pCmd, int argc, char *argv[])
{
  t_I5_API_OVERRIDE_BW_MSG   msg;
  int sd;
  int rc;

  if (argc < 4) {
    printf("Insufficient arguments\n");
    return -1;
  }

  msg.overrideCount = 0xff; // permanent override of bandwidth
  msg.availBwMbps = atoi(argv[2]);
  msg.macThroughBwMbps = atoi(argv[3]);
  
  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  rc = i5apiSendMessage(sd, I5_API_CMD_SET_PLC_OVERRIDE_BW, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;
}

static int i5CtlReleasePlcBw (void *pCmd, int argc, char *argv[])
{
  t_I5_API_OVERRIDE_BW_MSG   msg;
  int sd;
  int rc;

  msg.overrideCount = 0; // No override
  msg.availBwMbps = 0;
  msg.macThroughBwMbps = 0;
  
  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  rc = i5apiSendMessage(sd, I5_API_CMD_SET_PLC_OVERRIDE_BW, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;  
}
static int i5CtlFakePlcBwTemp (void *pCmd, int argc, char *argv[])
{
  t_I5_API_OVERRIDE_BW_MSG   msg;
  int sd;
  int rc;

  if (argc < 4) {
    printf("Insufficient arguments\n");
    return -1;
  }

  msg.overrideCount = 1; // single override of bandwidth
  msg.availBwMbps = atoi(argv[2]);
  msg.macThroughBwMbps = atoi(argv[3]);
  
  sd = i5apiOpen();
  if (-1 == sd) {
    return sd;
  }

  rc = i5apiSendMessage(sd, I5_API_CMD_SET_PLC_OVERRIDE_BW, &msg, sizeof msg);
  if ( -1 == rc ) {
    printf("Failed to send data to daemon\n");
  }
  i5apiClose(sd);  
   
  return 0;

}      
#endif // defined (SUPPORT_HOMEPLUG)   

static int i5CtlShowConfig(void *pCmd, int argc, char *argv[])
{
  t_I5_CTL_CMD         *pI5Cmd = (t_I5_CTL_CMD *)pCmd;
  t_I5_API_CONFIG_BASE  cfg = { 0 };
  t_I5_API_CONFIG_BASE *pCfg = &cfg;
  int                   rc = 0;

  cfg.subcmd = I5_API_CONFIG_BASE;
  rc = i5apiTransaction(pI5Cmd->cmd, &cfg, sizeof(t_I5_API_CONFIG_BASE), (void **)&pCfg, sizeof(t_I5_API_CONFIG_BASE));
  if ( rc == sizeof(t_I5_API_CONFIG_BASE) ) {
    printf("1905 Configuration\n");
    printf("   Friendly Name: %s\n", pCfg->deviceFriendlyName);
    printf("   %s, Enabled Bands: ", pCfg->isRegistrar ? "Registrar" : "Enrollee");
    if ( pCfg->apFreqBand24En || pCfg->apFreqBand5En ) {
      if ( pCfg->apFreqBand24En ) {
         printf("2.4GHz ");
      }
      if ( pCfg->apFreqBand24En ) {
         printf("5GHz");
      }
      printf("\n");
    }
    else
    {
       printf("None\n");
    }
  }
  else {
    printf("Unexpected message length\n");
  }
  return 0;
}

