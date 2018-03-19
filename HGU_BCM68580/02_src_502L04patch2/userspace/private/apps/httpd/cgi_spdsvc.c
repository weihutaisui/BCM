/***********************************************************************
 *
 *  Copyright (c) 2014  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard

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
#include <time.h>
#include <unistd.h>
#include <net/route.h>
#include <sys/ioctl.h>

#include "httpd.h"
#include "cgi_main.h"
#include "cms_qdm.h"
#include "cgi_sts.h"
#include "cgi_diag.h"
#include "cgi_util.h"
#include "sysdiag.h"

#include "cms_dal.h"
#include "cms_msg.h"

void cgiGetSpdsvcInfo(int argc, char **argv, char *varValue)
{
   if (argc != 3)
      return;
   
   if (!strcmp(argv[2], "resultHistoList"))
   {
      dalSpdsvc_getSpeedTestResults(varValue);
   }
   else
   {
      dalSpdsvc_getSpeedTestParams(varValue);
   }
}

void cgiSpeedTestCmd(char *query, FILE* fs)
{
   char strTcpPort[BUFLEN_8];
   char strDuration[BUFLEN_8];
   char strPktLength[BUFLEN_8];
   char strKbps[BUFLEN_8];
   char strSteps[BUFLEN_8];
   char strLoss[BUFLEN_8];
   char strLatency[BUFLEN_8];
   char strLossPercentage[BUFLEN_8];
   
   cgiGetValueByName(query, "mode", glbWebVar.speedTestMode);
   cgiGetValueByName(query, "direction", glbWebVar.speedTestDirection);
   cgiGetValueByName(query, "dataPath", glbWebVar.speedTestDataPath);
   cgiGetValueByName(query, "serverIpAddr", glbWebVar.speedTestServerAddr);
   cgiGetValueByName(query, "tcpPort", strTcpPort);
   glbWebVar.speedTestTcpPort = atoi(strTcpPort); 
   cgiGetValueByName(query, "duration", strDuration);
   glbWebVar.speedTestDuration = atoi(strDuration); 
   cgiGetValueByName(query, "pktLength", strPktLength);
   glbWebVar.speedTestPktLength = atoi(strPktLength); 
   cgiGetValueByName(query, "kbps", strKbps);
   glbWebVar.speedTestKbps = atoi(strKbps); 
   if (cmsUtl_strcmp(glbWebVar.speedTestMode, "client_bw") == 0)
   {
      cgiGetValueByName(query, "algo", glbWebVar.speedTestAlgorithm);
      cgiGetValueByName(query, "steps", strSteps);
      sscanf(strSteps, "%u", &(glbWebVar.speedTestSteps));
      cgiGetValueByName(query, "loss", strLoss);
      sscanf(strLoss, "%u", &(glbWebVar.speedTestLoss));
      if (cmsUtl_strcmp(glbWebVar.speedTestAlgorithm, MDMVS_BIN) == 0 ||
          cmsUtl_strcmp(glbWebVar.speedTestAlgorithm, MDMVS_RAMP) == 0)
      {
         cgiGetValueByName(query, "latency", strLatency);
         sscanf(strLatency, "%d", &(glbWebVar.speedTestLatency));
      }
      else
         glbWebVar.speedTestLatency = -1;
      if (cmsUtl_strcmp(glbWebVar.speedTestAlgorithm, MDMVS_RAMP) == 0)
      {
         cgiGetValueByName(query, "lossPercentage", strLossPercentage);
         sscanf(strLossPercentage, "%d", &(glbWebVar.speedTestLossPercentage));
      }
   }

   dalSpdsvc_runSpeedTest(&glbWebVar);
   do_ej("/webs/speedsvc.html", fs);  
}
