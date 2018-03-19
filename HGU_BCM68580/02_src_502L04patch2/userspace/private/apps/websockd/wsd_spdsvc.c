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

#ifdef SUPPORT_WEB_SOCKETS
#ifdef DMP_X_BROADCOM_COM_SPDSVC_1


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "websockets/libwebsockets.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#include "wsd_main.h"

#define SPACES_STRING    "     "    // 5 space characters

void wsd_parse_speed_service_info
    (const char *info,
     struct psd_speed_service *psd)
{
    char strTcpPort[BUFLEN_8];
    char strDuration[BUFLEN_8];
    char strPktLength[BUFLEN_8];
    char strKbps[BUFLEN_8];
    char strSteps[BUFLEN_8];
    char strLoss[BUFLEN_8];
    char strLatency[BUFLEN_8];
    char strLossPercentage[BUFLEN_8];
   
    // parse speed service information with '|' as delimeter
    sscanf(info,
           "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%s",
           psd->mode, psd->serverAddr, strTcpPort, psd->direction,
           psd->dataPath, strDuration, strPktLength, strKbps,
           psd->algorithm, strSteps, strLoss, strLatency, strLossPercentage);
/*
printf("info=%s\nmode=%s, direction=%s, dataPath=%s, serverAddr=%s\ntcpPort=%s, duration=%s, pktLength=%s, kpbs=%s\nalgo=%s, steps=%s, loss=%s, latency=%s\n\n",
           info, psd->mode, psd->direction, psd->dataPath, psd->serverAddr,
           strTcpPort, strDuration, strPktLength, strKbps, psd->algorithm,
           strSteps, strLoss, strLatency, strLossPercentage);
*/
    sscanf(strTcpPort, "%u", &(psd->tcpPort));
    sscanf(strDuration, "%u", &(psd->duration));
    sscanf(strPktLength, "%u", &(psd->pktLength));
    sscanf(strKbps, "%u", &(psd->kbps));
    sscanf(strSteps, "%u", &(psd->steps));
    sscanf(strLoss, "%u", &(psd->loss));
    sscanf(strLatency, "%d", &(psd->latency));
    sscanf(strLossPercentage, "%u", &(psd->lossPercentage));
}


CmsRet wsd_run_speed_test(const struct psd_speed_service *psd)
{
    SpeedServiceObject *spdsvcObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(3 * MSECS_IN_SEC)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        return ret;
    }

    if ((ret = cmsObj_get(MDMOID_SPEED_SERVICE, &iidStack, 0, (void **) &spdsvcObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not get SpeedServiceObject, ret=%d", ret);
        return ret;
    }

    CMSMEM_REPLACE_STRING(spdsvcObj->diagnosticsState, MDMVS_REQUESTED);
    CMSMEM_REPLACE_STRING(spdsvcObj->mode, psd->mode);
    CMSMEM_REPLACE_STRING(spdsvcObj->direction, psd->direction);
    CMSMEM_REPLACE_STRING(spdsvcObj->dataPath, psd->dataPath);
    CMSMEM_REPLACE_STRING(spdsvcObj->serverIpAddr, psd->serverAddr);
    spdsvcObj->tcpPort = psd->tcpPort;
    spdsvcObj->udpPort = 0;
    spdsvcObj->stepDuration = psd->duration;
    spdsvcObj->packetLength= psd->pktLength;
    spdsvcObj->startingBwKbps = psd->kbps;

    if (cmsUtl_strcmp(psd->mode, MDMVS_CLIENT_BW) == 0)
    {
        CMSMEM_REPLACE_STRING(spdsvcObj->algorithm, psd->algorithm);
        spdsvcObj->maxSteps = psd->steps;
        spdsvcObj->acceptablePercentageLoss = psd->loss;
        if (cmsUtl_strcmp(spdsvcObj->algorithm, MDMVS_BIN) == 0 ||
            cmsUtl_strcmp(spdsvcObj->algorithm, MDMVS_RAMP) == 0)
           spdsvcObj->latencyTolerancePercentage = psd->latency;
        else
           spdsvcObj->latencyTolerancePercentage = -1;
        if (cmsUtl_strcmp(spdsvcObj->algorithm, MDMVS_RAMP) == 0)
           spdsvcObj->maxLossPercentage = psd->lossPercentage;
    }

    if ((ret = cmsObj_set(spdsvcObj, &iidStack)) != CMSRET_SUCCESS)
        cmsLog_error("could not set SpeedServiceObject, ret=%d", ret);

    cmsObj_free((void **) &spdsvcObj);

    cmsLck_releaseLock();

    return ret;
}


int wsd_callback_speed_service
    (struct lws *wsi,
     enum lws_callback_reasons reason,
     void *user,
     void *in,
     size_t len)
{
    int ret = 0;
    int m = 0, n = 0;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    struct psd_speed_service *psd = (struct psd_speed_service *)user;

    switch (reason) {

        case LWS_CALLBACK_ESTABLISHED:
            // initialize speed service information
            memset(psd, 0, sizeof(struct psd_speed_service));
            lwsl_info("wsd_callback_speed_service: LWS_CALLBACK_ESTABLISHED\n");
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            // only write out if the session is validated.
            if (psd->sessionKey != 0)
            {
                n = sprintf((char *)p, "%u|%u", psd->status, psd->kbps);

                m = lws_write(wsi, p, n, LWS_WRITE_TEXT);
                if (m < n)
                {
                    lwsl_err("ERROR %d writing to speed_service socket\n", n);
                    ret = -1;
                }
            }
            break;

        case LWS_CALLBACK_RECEIVE:
            // sessionKey should be sent as single input
            // when websockets is open, other information
            // should be sent later without sessionKey
            if (psd->sessionKey == 0)
            {
                // haven't validated session key yet
                // sessionKey should be in the input
                // and should be cached for the whole session
                sscanf((const char *)in, "sessionKey=%d", &psd->sessionKey);
                if (wsd_validate_session_key(psd->sessionKey) == FALSE)
                    psd->sessionKey = 0;
            }

            // only continue if the session is validated.
            // and sessionKey is NOT in the input
            if (psd->sessionKey != 0 &&
                cmsUtl_strstr((const char *)in, "sessionKey") == NULL)
            {
                // parse speed service information with '|' as delimeter
                wsd_parse_speed_service_info((const char *)in, psd);

                // run speed test with speed service information
                psd->status = (UINT32) wsd_run_speed_test(psd);

                // if error then write back error code
                if (psd->status != CMSRET_SUCCESS)
                    lws_callback_on_writable(wsi);
            }
            break;

        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            //dump_handshake_info(wsi);
            break;

        default:
            break;
    }

    return ret;
}


#endif   // DMP_X_BROADCOM_COM_SPDSVC_1
#endif   // SUPPORT_WEB_SOCKETS

