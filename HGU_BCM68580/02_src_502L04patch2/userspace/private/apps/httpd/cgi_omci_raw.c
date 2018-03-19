/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

#ifdef DMP_X_ITU_ORG_GPON_1	/* aka SUPPORT_OMCI */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "cms.h"
#include "cms_msg.h"
#include "cms_util.h"
#include "cms_phl.h"
#include "httpd.h"
#include "cgi_main.h"
#include "cgi_omci.h"


// globals
UINT8 cgiOmciRawMsgInProgress = 0;
omciPacket rawPacket;
UINT16 rawPacketcnt = 0;
omci_raw_buffer_row_t omci_raw_buffer[OMCI_RAW_BUFFER_SIZE];
UINT16 omci_raw_buffer_wptr = 0;
UINT16 omci_raw_buffer_rptr = 0;

// globals from cgi_omci_cfg

extern UINT16 numMsgRx;


static CmsRet cgiOmci_sendRaw (UINT8 * rawmsg, UINT16 rawmsg_len);


// Main entry for OMCI raw message interface
void
cgiOmciRaw (char *query, FILE * fs)
{
    UINT8 *msg = NULL;
    char buf[4000];
    UINT32 bsize;
    int i;
    int n;
    int numpackets;
    omci_raw_buffer_row_t *row;

    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *omciSys = NULL;

    cmsLog_debug("get OMCI system obj");
    if (cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &omciSys) != CMSRET_SUCCESS)
    {
        cmsLog_error("get of OMCI system object failed");
        return;
    }

    if (omciSys->omciRawEnable == FALSE)
    {
        cmsLog_notice("Cannot accept OMCI raw messages since this feature is not enabled");
        cmsObj_free((void**)&omciSys);
        return;
    }

    if (sscanf (query, "send=%3960s", buf) == 1)
    {
        if (CMSRET_SUCCESS == cmsUtl_hexStringToBinaryBuf (buf, &msg, &bsize))
        {
            if (bsize >= OMCI_PACKET_A_HDR_SIZE)
            {
                fprintf (fs, "OK\n");
                cgiOmci_sendRaw (msg, bsize);
            }
            CMSMEM_FREE_BUF_AND_NULL_PTR (msg);
        }
    }
    else if (strcmp (query, "get") == 0)
    {
        numpackets =
            ((int) omci_raw_buffer_wptr - (int) omci_raw_buffer_rptr +
            OMCI_RAW_BUFFER_SIZE) % OMCI_RAW_BUFFER_SIZE;
        fprintf (fs, "%d:\n", numpackets);
        for (n = 0; n < numpackets; n++)
        {
            int len;

            row = &omci_raw_buffer[omci_raw_buffer_rptr];
            omci_raw_buffer_rptr =
                (omci_raw_buffer_rptr + 1) % OMCI_RAW_BUFFER_SIZE;
            len = OMCI_PACKET_SIZE(&row->packet);
            msg = (UINT8 *) & row->packet;
            fprintf (fs, "%d.%03d:", (SINT32)(row->timestamp.tv_sec),
                     (SINT32)(row->timestamp.tv_usec / 1000));
            for (i = 0; i < len; i++)
            {
                fprintf (fs, "%02x", msg[i] & 0xff);
            }
            fprintf (fs, "\n");
        }
        fprintf (fs, "\n");
    }
    cmsObj_free((void**)&omciSys);
}


static CmsRet
cgiOmci_sendRaw (UINT8 * rawmsg, UINT16 rawmsg_len)
{
    UINT16 msgSize = sizeof (CmsMsgHeader) + sizeof (omciPacket);
    char buf[msgSize];
    CmsMsgHeader *msg = (CmsMsgHeader *) buf;
    omciPacket *packet = (omciPacket *) (msg + 1);
    CmsRet ret = CMSRET_SUCCESS;

    memset (buf, 0, sizeof (CmsMsgHeader) + sizeof (omciPacket));
    memcpy (packet, rawmsg, rawmsg_len);
    msg->type = CMS_MSG_OMCI_COMMAND_REQUEST;
    msg->src = EID_HTTPD;
    msg->dst = EID_OMCID;
    msg->flags_event = 1;
    msg->dataLength = sizeof (omciPacket);
    msg->flags_request = 1;
    msg->sequenceNumber = 1;

    //numMsgRx = 0;
    cgiOmciRawMsgInProgress = 1;

    cgiOmci_addCrc(packet, EID_HTTPD);

    if ((ret = cmsMsg_send (msgHandle, msg)) != CMSRET_SUCCESS)
    {
        cmsLog_error
            ("Could not send out CMS_MSG_OMCI_COMMAND_REQUEST with msgType=%d, ret=%d",
             packet->msgType, ret);
    }
    else
    {
        cmsLog_notice ("Send CMS_MSG_OMCI_COMMAND_REQUEST with msgType = %d",
                       packet->msgType);
    }

    return (ret);
}


CmsRet
cgiOmci_handlePmdDebug (const CmsMsgHeader * msgRes)
{
    CmsRet ret = CMSRET_SUCCESS;
    omciPacket *packetRes = (omciPacket *) (msgRes + 1);

    cgiOmci_queueRawPacket (packetRes);

    return ret;
}


void
cgiOmci_queueRawPacket (omciPacket * packet)
{
    memcpy (&omci_raw_buffer[omci_raw_buffer_wptr].packet, packet,
            sizeof (omciPacket));
    gettimeofday (&omci_raw_buffer[omci_raw_buffer_wptr].timestamp, NULL);
    omci_raw_buffer_wptr = (omci_raw_buffer_wptr + 1) % OMCI_RAW_BUFFER_SIZE;
    if (omci_raw_buffer_wptr == omci_raw_buffer_rptr)
    {
        omci_raw_buffer_rptr =
            (omci_raw_buffer_rptr + 1) % OMCI_RAW_BUFFER_SIZE;
    }
}


#endif //DMP_X_ITU_ORG_GPON_1
