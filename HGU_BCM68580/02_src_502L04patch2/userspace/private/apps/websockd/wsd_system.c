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


#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "websockets/libwebsockets.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#include "wsd_main.h"


enum wsd_cpu_line {
    WSD_CPU_ALL_LINE = 4,
    WSD_CPU_0_LINE,
    WSD_CPU_1_LINE,
    WSD_CPU_2_LINE,
    WSD_CPU_3_LINE
};


static void wsd_get_cpu_info
    (UINT32 *cpu,
     UINT32 *cpu0,
     UINT32 *cpu1,
     UINT32 *cpu2,
     UINT32 *cpu3)
{
    char cmd[BUFLEN_128]={0};
    char line[BUFLEN_512]={0};
    char in1[BUFLEN_32]={0}, in2[BUFLEN_32]={0};
    float in3 = 0, in4 = 0, in5 = 0, in6 = 0;
    float in7 = 0, in8 = 0, in9 = 0, in10 = 0, in11 = 0;
    UINT32 count = 0;
    FILE *fs = NULL;

    snprintf(cmd, sizeof(cmd), "mpstat -P ALL 1 1 > /var/cpuinfo 2> /dev/null");
#ifndef DESKTOP_LINUX   
    system(cmd);
#endif   
    fs = fopen("/var/cpuinfo", "r");
    if (fs == NULL)
    {
        cmsLog_error("Could not open /var/cpuinfo");
        return;
    }

    while ( fgets(line, sizeof(line), fs) )
    {
        // skip header lines
        if (++count < WSD_CPU_ALL_LINE) continue;
        // stop when line is empty
        if (line[0] == '\0') break;

        sscanf(line, "%s %s %f %f %f %f %f %f %f %f %f",
               in1, in2, &in3, &in4, &in5, &in6,
               &in7, &in8, &in9, &in10, &in11);

        switch (count)
        {
            case WSD_CPU_ALL_LINE:
                *cpu = (UINT32)(100 - in11);
                break;
            case WSD_CPU_0_LINE:
                *cpu0 = (UINT32)(100 - in11);
                break;
            case WSD_CPU_1_LINE:
                *cpu1 = (UINT32)(100 - in11);
                break;
            case WSD_CPU_2_LINE:
                *cpu2 = (UINT32)(100 - in11);
                break;
            case WSD_CPU_3_LINE:
                *cpu3 = (UINT32)(100 - in11);
                break;
        }
    }

    fclose(fs);
    unlink("/var/cpuinfo");
}
  

int wsd_callback_cpu_mem
    (struct lws *wsi,
     enum lws_callback_reasons reason,
     void *user,
     void *in,
     size_t len)
{
    int ret = 0;
    int m = 0, n = 0;
    UINT32 cpu = 0, cpu0 = 0, cpu1 = 0, cpu2 = 0, cpu3 = 0;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    struct sysinfo info;
    struct psd_session_key *psd = (struct psd_session_key *)user;

    switch (reason) {

        case LWS_CALLBACK_ESTABLISHED:
            lwsl_info("wsd_callback_cpu_mem: LWS_CALLBACK_ESTABLISHED\n");
            psd->sessionKey = 0;
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            // only write out if the session is validated.
            lwsl_info("wsd_callback_cpu_mem: LWS_CALLBACK_SERVER_WRITEABLE\n");
            if (psd->sessionKey != 0)
            {
                wsd_get_cpu_info(&cpu, &cpu0, &cpu1, &cpu2, &cpu3);

                sysinfo(&info);

                if (info.mem_unit == 0)
                {
                    info.mem_unit = 1;
                }
                info.mem_unit *= 1024;
                info.totalram /= info.mem_unit;
                info.freeram /= info.mem_unit;

                n = sprintf((char *)p, "%u|%u|%u|%u|%u|%lu",
                             cpu, cpu0, cpu1, cpu2, cpu3,
                             info.totalram - info.freeram);

                m = lws_write(wsi, p, n, LWS_WRITE_TEXT);
                if (m < n)
                {
                    lwsl_err("ERROR %d writing to sys/mem socket\n", n);
                    ret = -1;
                }
            }
            break;

        case LWS_CALLBACK_RECEIVE:
            lwsl_info("wsd_callback_cpu_mem: LWS_CALLBACK_RECEIVE\n");
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
                lws_callback_on_writable(wsi);
            break;

        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            lwsl_info("wsd_callback_cpu_mem: LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION\n");
            dump_handshake_info(wsi);
            break;

        default:
            break;
    }

    return ret;
}


#endif   // SUPPORT_WEB_SOCKETS

