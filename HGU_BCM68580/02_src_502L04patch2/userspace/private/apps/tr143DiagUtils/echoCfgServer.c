/***********************************************************************
 *
 *  Copyright (c) 2012-2013  Broadcom Corporation
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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/ioctl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>


#include "tr143_defs.h"
#include "cms.h"
#include "cms_core.h"
#include "cms_qdm.h"
#include "cms_msg.h"

#define DEFAULT_TIME "0001-01-01T00:00:00Z"

#if defined(DMP_UDPECHOPLUS_1) || defined(DMP_DEVICE2_UDPECHOPLUS_1)
struct UdpEchoPlus_t {
   unsigned  int TestGenSN;
   unsigned  int TestRespSN;
   unsigned  int TestRespRecvTimeStamp;
   unsigned  int TestRespReplyTimeStamp;
   unsigned  int TestRespReplyFailureCount;
};
#endif

struct echoStats_t {
   unsigned int pktRcvd, pktRspd;
   unsigned int bytesRcvd, bytesRspd;
   char tmFirst[64], tmlast[64];
   unsigned int TestRespSN, TestRespFC;
};


char ifname[32], s_ip[24];
int port, echoPlus = 0;
void *msgHandle=NULL;
int pipefd[2];
struct echoStats_t echoStats;

void saveToMdm();
void saveToMdm_igd();
void saveToMdm_dev2();

#if defined(SUPPORT_DM_LEGACY98)
#define saveToMdm()           saveToMdm_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define saveToMdm()           saveToMdm_igd()
#elif defined(SUPPORT_DM_PURE181)
#define saveToMdm()           saveToMdm_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define saveToMdm()  (cmsMdm_isDataModelDevice2() ? \
                                      saveToMdm_dev2() : \
                                      saveToMdm_igd())
#endif
void saveToMdm_igd()
{
   int ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UDPEchoCfgObject *obj;

   if ((ret = cmsLck_acquireLockWithTimeout(TR143_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   if ((ret = cmsObj_get(MDMOID_UDP_ECHO_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get MDMOID_UDP_ECHO_CFG error, ret=%d", ret);
      cmsLck_releaseLock();
      return;
   }

   CMSMEM_REPLACE_STRING(obj->timeFirstPacketReceived, echoStats.tmFirst);
   CMSMEM_REPLACE_STRING(obj->timeLastPacketReceived, echoStats.tmlast);
   obj->bytesReceived = echoStats.bytesRcvd;
   obj->bytesResponded = echoStats.bytesRspd;
   obj->packetsReceived = echoStats.pktRcvd;
   obj->packetsResponded = echoStats.pktRspd;
   obj->X_BROADCOM_COM_PacketsRespondedFail = echoStats.TestRespFC;

   if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
      cmsLog_error("set MDMOID_UDP_ECHO_CFG error, ret=%d", ret);

   cmsObj_free((void **) &obj);
   cmsLck_releaseLock();
}

void saveToMdm_dev2()
{
   int ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpDiagUDPEchoConfigObject *obj;

   if ((ret = cmsLck_acquireLockWithTimeout(TR143_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   if ((ret = cmsObj_get(MDMOID_UDP_ECHO_CONFIG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get MDMOID_UDP_ECHO_CONFIG error, ret=%d", ret);
      cmsLck_releaseLock();
      return;
   }

   CMSMEM_REPLACE_STRING(obj->timeFirstPacketReceived, echoStats.tmFirst);
   CMSMEM_REPLACE_STRING(obj->timeLastPacketReceived, echoStats.tmlast);
   obj->bytesReceived = echoStats.bytesRcvd;
   obj->bytesResponded = echoStats.bytesRspd;
   obj->packetsReceived = echoStats.pktRcvd;
   obj->packetsResponded = echoStats.pktRspd;
   obj->X_BROADCOM_COM_PacketsRespondedFail = echoStats.TestRespFC;

   if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
      cmsLog_error("set MDMOID_UDP_ECHO_CONFIG error, ret=%d", ret);

   cmsObj_free((void **) &obj);
   cmsLck_releaseLock();
}

int setupConfig(void);
int setupConfig_igd(void);
int setupConfig_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define setupConfig()           setupConfig_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define setupConfig()           setupConfig_igd()
#elif defined(SUPPORT_DM_PURE181)
#define setupConfig()           setupConfig_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define setupConfig()  (cmsMdm_isDataModelDevice2() ? \
                                      setupConfig_dev2() : \
                                      setupConfig_igd())
#endif
int setupConfig_igd()
{
   UDPEchoCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   int ret, l, retCode = 0;
   char buf[256];

   if ((ret = cmsLck_acquireLockWithTimeout(TR143_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return -1;
   }

   if ((ret = cmsObj_get(MDMOID_UDP_ECHO_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get MDMOID_UDP_ECHO_CFG object failed, ret=%d", ret);
      retCode = -1;
      goto errOut1;
   }

   if (!obj->enable)
   {
      cmsLog_notice("udp echo config not enabled");
      retCode = -1;
      goto errOut2;
   }

   if (obj->interface) 
      strncpy(buf, obj->interface, sizeof(buf));
   else
      buf[0] = '\0';

   if (obj->sourceIPAddress && obj->sourceIPAddress[0] != '\0') 
      strncpy(s_ip, obj->sourceIPAddress, sizeof(s_ip));
   else
      s_ip[0] = '\0';

   port = obj->UDPPort;
#if defined(DMP_UDPECHOPLUS_1)
   echoPlus = obj->echoPlusEnabled;
#endif
   strncpy(echoStats.tmFirst, obj->timeFirstPacketReceived, sizeof(echoStats.tmFirst));
   strncpy(echoStats.tmlast, obj->timeLastPacketReceived, sizeof(echoStats.tmlast));
   echoStats.bytesRcvd = obj->bytesReceived;
   echoStats.bytesRspd = obj->bytesResponded;
   echoStats.pktRcvd = obj->packetsReceived;
   echoStats.pktRspd = obj->packetsResponded;
   echoStats.TestRespSN = obj->packetsResponded;
   echoStats.TestRespFC = obj->X_BROADCOM_COM_PacketsRespondedFail;

   cmsLog_setLevel(obj->X_BROADCOM_COM_LogLevel);

   if (buf[0])
   {
      l = strlen(buf);
      if ((buf[l-1]) != '.' && l < sizeof(buf) -1) 
      {
         buf[l] = '.';
         buf[l + 1] = '\0';
      }
      if ((ret = qdmIntf_fullPathToIntfnameLocked(buf, ifname))!= CMSRET_SUCCESS)
      {
         cmsLog_error("invalid interface\n", ret);
         retCode = -1;
         goto errOut2;
      }
   }
   else
      ifname[0] = '\0';

   cmsLog_debug("if_name:%s, s_ip:%s, port:%d, echoPlus:%d\n", ifname, s_ip, port, echoPlus);

errOut2:	
   cmsObj_free((void **) &obj);
errOut1:	
   cmsLck_releaseLock();
   return retCode;
}

int setupConfig_dev2()
{
   Dev2IpDiagUDPEchoConfigObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   int ret, l, retCode = 0;
   char buf[256];

   if ((ret = cmsLck_acquireLockWithTimeout(TR143_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return -1;
   }

   if ((ret = cmsObj_get(MDMOID_UDP_ECHO_CONFIG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get MDMOID_UDP_ECHO_CONFIG object failed, ret=%d", ret);
      retCode = -1;
      goto errOut1;
   }

   if (!obj->enable)
   {
      cmsLog_notice("udp echo config not enabled");
      retCode = -1;
      goto errOut2;
   }

   if (obj->interface) 
      strncpy(buf, obj->interface, sizeof(buf));
   else
      buf[0] = '\0';

   if (obj->sourceIPAddress && obj->sourceIPAddress[0] != '\0') 
      strncpy(s_ip, obj->sourceIPAddress, sizeof(s_ip));
   else
      s_ip[0] = '\0';

   port = obj->UDPPort;
#ifdef DMP_DEVICE2_UDPECHOPLUS_1
   echoPlus = obj->echoPlusEnabled & obj->echoPlusSupported;
#endif
   strncpy(echoStats.tmFirst, obj->timeFirstPacketReceived, sizeof(echoStats.tmFirst));
   strncpy(echoStats.tmlast, obj->timeLastPacketReceived, sizeof(echoStats.tmlast));
   echoStats.bytesRcvd = obj->bytesReceived;
   echoStats.bytesRspd = obj->bytesResponded;
   echoStats.pktRcvd = obj->packetsReceived;
   echoStats.pktRspd = obj->packetsResponded;
   echoStats.TestRespSN = obj->packetsResponded;
   echoStats.TestRespFC = obj->X_BROADCOM_COM_PacketsRespondedFail;

   cmsLog_setLevel(obj->X_BROADCOM_COM_LogLevel);

   if (buf[0])
   {
      l = strlen(buf);
      if ((buf[l-1]) != '.' && l < sizeof(buf) -1) 
      {
         buf[l] = '.';
         buf[l + 1] = '\0';
      }
      if ((ret = qdmIntf_fullPathToIntfnameLocked(buf, ifname))!= CMSRET_SUCCESS)
      {
         cmsLog_error("invalid interface\n", ret);
         retCode = -1;
         goto errOut2;
      }
   }
   else
      ifname[0] = '\0';

   cmsLog_debug("if_name:%s, s_ip:%s, port:%d, echoPlus:%d\n", ifname, s_ip, port, echoPlus);

errOut2:	
   cmsObj_free((void **) &obj);
errOut1:	
   cmsLck_releaseLock();
   return retCode;
}

void sig_handler(int signal)
{
   write(pipefd[0], "0", 1);
}

int echo_loop()
{
   fd_set fdset;
   int fd, ret, cliLen, len;
   struct sockaddr_in inAddr;
   unsigned int buf[1500];
#if defined(DMP_UDPECHOPLUS_1) || defined(DMP_DEVICE2_UDPECHOPLUS_1)
   struct UdpEchoPlus_t *pbuf;
#endif

   fd = socket (AF_INET, SOCK_DGRAM, 0);
   if (fd < 0)
   {
      cmsLog_error("creat socket error");
      return -1;
   }

   if (pipe(pipefd) < 0)
   {
      cmsLog_error("create pipe error");
      close(fd);
      return -1;
   }

   if (ifname[0])
   {
      struct ifreq ifr;

      strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
      if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr)) < 0)
      {
         cmsLog_error("bind to %s error", ifname);
         return -1;
      }
      else
         cmsLog_debug("listening on %s", ifname);
   }

   memset(&inAddr, 0, sizeof(inAddr));
   inAddr.sin_family = AF_INET;
   inAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   inAddr.sin_port   = htons(port);
   if (bind(fd, (struct sockaddr *)&inAddr, sizeof(inAddr)) < 0)
   {
      close(fd);
      cmsLog_error("bind error\n");
      return -1;
   }

   while (1)
   {
      FD_ZERO(&fdset);
      FD_SET(pipefd[0], &fdset);
      FD_SET(fd, &fdset);

      ret = select((pipefd[0] > fd ? pipefd[0] : fd) + 1, &fdset, NULL, NULL, NULL);

      if (ret < 0)
      {
         cmsLog_debug("select error!\n");
         break;
      }

      if (FD_ISSET(pipefd[0], &fdset)) 
      {
         cmsLog_debug("recv term signal\n");
         break;
      }

      cliLen = sizeof (inAddr);
      len= recvfrom (fd, buf, sizeof(buf), 0, (struct sockaddr *) &inAddr, (socklen_t *) &cliLen);
      if (len <= 0) continue;

      cmsLog_notice("recv %d bytes from %s:%u \n", len, inet_ntoa (inAddr.sin_addr), ntohs (inAddr.sin_port));

      if (s_ip[0] && strcmp(s_ip,  inet_ntoa (inAddr.sin_addr)))
      {
         cmsLog_debug("sourceIPAddress not match, %s != %s\n", s_ip, inet_ntoa (inAddr.sin_addr));
         continue;
      }

      if (!strcmp(echoStats.tmFirst, DEFAULT_TIME))
         cmsTms_getXSIDateTime(0, echoStats.tmFirst, sizeof(echoStats.tmFirst));	
      cmsTms_getXSIDateTime(0, echoStats.tmlast, sizeof(echoStats.tmlast));	
      echoStats.bytesRcvd += len + 8;
      echoStats.pktRcvd++;

#if defined(DMP_UDPECHOPLUS_1) || defined(DMP_DEVICE2_UDPECHOPLUS_1)
      if (echoPlus && len >= sizeof(struct UdpEchoPlus_t))
      {
         pbuf = (struct UdpEchoPlus_t *)buf;
         time((time_t *)&pbuf->TestRespRecvTimeStamp);
         pbuf->TestRespSN = echoStats.TestRespSN;
         pbuf->TestRespReplyFailureCount = echoStats.TestRespFC;
         time((time_t *)&pbuf->TestRespReplyTimeStamp);
      }
#endif

      if (sendto (fd, buf, len, 0, (struct sockaddr *) &inAddr, cliLen) < 0)
         echoStats.TestRespFC++;
      else
      {
         echoStats.bytesRspd += len+8;
         echoStats.pktRspd++;
         echoStats.TestRespSN++;
      }

      saveToMdm();
   }

   close(fd);
   close(pipefd[0]);
   close(pipefd[1]);
   return 0;
}

void saveToflash(void)
{
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(TR143_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not acquire lock (ret=%d), abort config save", ret);
      return;
   }


   if ((ret = cmsMgm_saveConfigToFlash()) != CMSRET_SUCCESS)
      cmsLog_error("saveConfigToFlash failed, ret=%d", ret);
   else
      cmsLog_debug("config saved to flash");

   cmsLck_releaseLock();

   return;
}


int main (int argc, char *argv[])
{
   SINT32 shmId=UNINITIALIZED_SHM_ID;
   int ret, ch;

   while ((ch = getopt(argc, argv, "m:")) != -1)
   {
      switch (ch) {
      case 'm':
         shmId = atoi(optarg);
         break;
      }
   }

   if (shmId == UNINITIALIZED_SHM_ID)
   {
      cmsLog_error("Did not get ShmId, cannot attach to MDM");
      exit(-1);
   }

   if (setsid() == -1)
      cmsLog_error("Could not detach from terminal");
   else
      cmsLog_debug("detached from terminal");
   signal(SIGTERM, sig_handler);
   signal(SIGINT, sig_handler);

   cmsLog_init(EID_UDPECHO);
   cmsLog_setLevel(LOG_LEVEL_ERR);

   if ((ret = cmsMsg_init(EID_UDPECHO, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_init failed, ret=%d", ret);
      goto errOut3;
   }

   cmsLog_notice("calling cmsMdm_init with shmId=%d", shmId);
   if ((ret = cmsMdm_init(EID_UDPECHO, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_init error ret=%d", ret);
      goto errOut2;
   }


   if ((ret = setupConfig()) < 0)
   {
      cmsLog_error("setupConfig error ret=%d", ret);
      goto errOut1;
   }

   cmsLog_notice("echo config server start ......\n");
   echo_loop();

errOut1:
   cmsMdm_cleanup();
errOut2:
   cmsMsg_cleanup(&msgHandle);
errOut3:
   cmsLog_cleanup();
   return 0;
}



