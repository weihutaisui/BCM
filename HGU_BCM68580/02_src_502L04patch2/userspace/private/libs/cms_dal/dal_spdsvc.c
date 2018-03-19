/*
* <:copyright-BRCM:2014:proprietary:standard
* 
*    Copyright (c) 2014 Broadcom 
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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"

void dalSpdsvc_getSpeedTestParams(char *varValue)
{
   int testDone = 1;
   SpeedServiceObject *spdsvcObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   varValue[0] = '\0';

   if ((ret = cmsObj_get(MDMOID_SPEED_SERVICE, &iidStack, 0, (void **) &spdsvcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get SpeedServiceObject, ret=%d", ret);
      return;
   }

   if (cmsUtl_strcmp(spdsvcObj->diagnosticsState, MDMVS_REQUESTED) == 0)
   {
      testDone = 0;
   }

   sprintf(varValue,"%s|%s|%u|%s|%s|%u|%u|%u|%s|%u|%u|%d|%u|%u",
         spdsvcObj->mode, spdsvcObj->serverIpAddr,
         spdsvcObj->tcpPort, spdsvcObj->direction,
         spdsvcObj->dataPath, spdsvcObj->stepDuration,
         spdsvcObj->packetLength, spdsvcObj->startingBwKbps, 
         spdsvcObj->algorithm, spdsvcObj->maxSteps, 
         spdsvcObj->acceptablePercentageLoss,
         spdsvcObj->latencyTolerancePercentage,
         spdsvcObj->maxLossPercentage,
         testDone);

   cmsObj_free((void **) &spdsvcObj);
}

void dalSpdsvc_getSpeedTestResults(char *varValue)
{
   SpeedServiceObject *spdsvcObj = NULL;
   ResultHistoryObject *resultObj = NULL;
   InstanceIdStack spdsvcIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack resultIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   int i;
   char results[5][BUFLEN_256] = {{'\0'},{'\0'},{'\0'},{'\0'},{'\0'}};
   varValue[0] = '\0';

   if ((ret = cmsObj_get(MDMOID_SPEED_SERVICE, &spdsvcIidStack, 0, (void **) &spdsvcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get SpeedServiceObject, ret=%d", ret);
      return;
   }

   if (!strcmp(spdsvcObj->diagnosticsState, MDMVS_NONE))
   {
      cmsLog_notice("There is no finished test");
      cmsObj_free((void **) &spdsvcObj);
      return;
   }

   /* Get the latest test result*/
   if (spdsvcObj->lastRunTime != NULL)
   {
      sprintf(varValue, "%s;%u;%u;%u;%u;%u;%u;%u;%s|",
              spdsvcObj->lastRunTime, spdsvcObj->goodPut,
              spdsvcObj->payloadRate, spdsvcObj->packetLoss,
              spdsvcObj->avgLatency, spdsvcObj->adjustReceivedRate, 
              spdsvcObj->receivedTime, spdsvcObj->overhead,
              spdsvcObj->direction); 
   }
   cmsObj_free((void **) &spdsvcObj);

   i = 0;
   while ((ret = cmsObj_getNext(MDMOID_RESULT_HISTORY, &resultIidStack, (void **) &resultObj)) == CMSRET_SUCCESS)
   {
      sprintf(results[i++], "%s;%u;%u;%u;%u;%u;%u;%u;%s|",
              resultObj->runTime, resultObj->goodPut,
              resultObj->payloadRate, resultObj->packetLoss,
              resultObj->avgLatency, resultObj->adjustReceivedRate, 
              resultObj->receivedTime, resultObj->overhead,
              resultObj->direction); 
      cmsObj_free((void **) &resultObj);
   }

   /* reverse the result history sequence to let the latest one in front of others. */
   while(i--)
      strcat(varValue, results[i]);

}

CmsRet dalSpdsvc_runSpeedTest(const WEB_NTWK_VAR *webVar)
{
   SpeedServiceObject *spdsvcObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_SPEED_SERVICE, &iidStack, 0, (void **) &spdsvcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get SpeedServiceObject, ret=%d", ret);
      return ret;
   }

   CMSMEM_REPLACE_STRING(spdsvcObj->diagnosticsState, MDMVS_REQUESTED);
   CMSMEM_REPLACE_STRING(spdsvcObj->mode, webVar->speedTestMode);
   CMSMEM_REPLACE_STRING(spdsvcObj->direction, webVar->speedTestDirection);
   CMSMEM_REPLACE_STRING(spdsvcObj->dataPath, webVar->speedTestDataPath);
   CMSMEM_REPLACE_STRING(spdsvcObj->serverIpAddr, webVar->speedTestServerAddr);
   spdsvcObj->tcpPort = webVar->speedTestTcpPort;
   spdsvcObj->udpPort = 0;
   spdsvcObj->stepDuration = webVar->speedTestDuration;
   spdsvcObj->packetLength= webVar->speedTestPktLength;
   spdsvcObj->startingBwKbps = webVar->speedTestKbps;

   if (cmsUtl_strcmp(webVar->speedTestMode, MDMVS_CLIENT_BW) == 0)
   {
      CMSMEM_REPLACE_STRING(spdsvcObj->algorithm, webVar->speedTestAlgorithm);
      spdsvcObj->maxSteps = webVar->speedTestSteps;
      spdsvcObj->acceptablePercentageLoss = webVar->speedTestLoss;
      if (cmsUtl_strcmp(spdsvcObj->algorithm, MDMVS_BIN) == 0 ||
          cmsUtl_strcmp(spdsvcObj->algorithm, MDMVS_RAMP) == 0)
         spdsvcObj->latencyTolerancePercentage = webVar->speedTestLatency;
      else
         spdsvcObj->latencyTolerancePercentage = -1;
      if (cmsUtl_strcmp(spdsvcObj->algorithm, MDMVS_RAMP) == 0)
         spdsvcObj->maxLossPercentage = webVar->speedTestLossPercentage;
   }

   if ((ret = cmsObj_set(spdsvcObj, &iidStack)) != CMSRET_SUCCESS)
      cmsLog_error("could not set SpeedServiceObject, ret=%d", ret);

   cmsObj_free((void **) &spdsvcObj);
   return ret;
}
