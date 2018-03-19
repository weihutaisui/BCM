/***********************************************************************
 *
 *  Copyright (c) 2014 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:omcid

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

#include <net/if.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "owsvc_api.h"
#include "omciobj_defs.h"

#include "mdmlite_api.h"
#include "omci_api.h"
#include "omci_pm_test.h"
#include "omci_pm_alarm.h"
#include "omci_util.h"
#include "laser.h"
#include "omci_msg.h"
#include "owrut_api.h"


static BCM_COMMON_DECLARE_LL(testLL);


/*========================= PRIVATE FUNCTIONS ==========================*/


static BCM_OMCIPM_TEST_ENTRY* test_find(UINT16 tcID)
{
    BCM_OMCIPM_TEST_ENTRY* entry = BCM_COMMON_LL_GET_HEAD(testLL);

    while (entry != NULL)
    {
        if (entry->tcID == tcID)
            break;

        entry = BCM_COMMON_LL_GET_NEXT(entry);
    }

    return entry;
}

static UBOOL8 test_exist(UINT16 tcID)
{
    return ((test_find(tcID) == NULL) ? FALSE : TRUE);
}

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
static UBOOL8 test_getCurrentIpAddress(
    UINT16 objID,
    char *ipStr)
{
    UBOOL8 found = FALSE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    IpHostConfigDataObject *ipHost = NULL;
    struct in_addr inAddr;

    // CMS lock should be acquired already in omci_msg_handler_Test() function

    // look for IP Host that has its managedEntityId
    // match with managedEntityId of Broadcom IP Host
    while ((!found) &&
           (_cmsObj_getNext(MDMOID_IP_HOST_CONFIG_DATA,
                           &iidStack, (void **) &ipHost)
           == CMSRET_SUCCESS))
    {
        found = (ipHost->managedEntityId == objID);

        if (found == TRUE)
        {
            inAddr.s_addr = ipHost->currentAddress;
            strcpy(ipStr, inet_ntoa(inAddr));
        }

        _cmsObj_free((void **) &ipHost);
    }

    return found;
}
#endif

static void test_fill_result_header(
    BCM_OMCIPM_TEST_ENTRY *e,
    omciPacket *pPacket)
{
    // Setup OMCI transaction identifier MSB & LSB.
    OMCI_HTONS(&pPacket->tcId, e->tcID);

    // Setup OMCI message type.
    pPacket->msgType = OMCI_MSG_TYPE_TESTRESULT;

    // Setup OMCI device identifier type.
    pPacket->devId = OMCI_PACKET_DEV_ID_A;

    // Setup OMCI message identifier.
    OMCI_HTONS(&pPacket->classNo, e->meID);
    OMCI_HTONS(&pPacket->instId, e->objID);
}

static void test_iphost_timeout(BCM_OMCIPM_TEST_ENTRY *e)
{
    omciPacket packet;

    memset(&packet, 0, sizeof(omciPacket));

    //fill test result header
    test_fill_result_header(e, &packet);

    //set test result as timeout
    OMCI_PACKET_MSG(&packet)[0] = 0;

    //set number of meaningful bytes
    OMCI_PACKET_MSG(&packet)[1] = 0;

    omci_alarm_sendRaw("test_iphost_timeout", &packet, 0, 0);
}

static void test_aniG_timeout(BCM_OMCIPM_TEST_ENTRY *e)
{
    omciPacket packet;
    int msgOffset = 0;
    int laserFd = 0;
    UINT32 temperature = 0;
    UINT32 vcc = 0;
    UINT32 bias = 0;
    UINT32 txPower = 0;
    UINT32 rxPower = 0;
    SINT32 txDb, rxDb;

    memset(&packet, 0, sizeof(omciPacket));

    //fill test result header
    test_fill_result_header(e, &packet);
#ifndef DESKTOP_LINUX
    laserFd = open("/dev/laser_dev", O_RDWR);
    if (laserFd < 0)
    {
        cmsLog_error("Laser driver open error");
    }
    else
#endif /* DESKTOP_LINUX */
    {
        if (omciUtil_ioctlWrap(laserFd, LASER_IOCTL_GET_TEMPTURE, &temperature) < 0)
        {
            temperature = 0;
            cmsLog_error("Laser driver IOCTL error on LASER_IOCTL_GET_TEMPTURE");
        }

        // unit is 100 uV
        if (omciUtil_ioctlWrap(laserFd, LASER_IOCTL_GET_VOTAGE, &vcc) < 0)
        {
            vcc = 0;
            cmsLog_error("Laser driver IOCTL error on LASER_IOCTL_GET_VOTAGE");
        }
        else
        {
            //interprete to 20mV
            vcc = vcc / 200;
        }

        // unit is 2 uA
        if (omciUtil_ioctlWrap(laserFd, LASER_IOCTL_GET_BIAS_CURRENT, &bias) < 0)
        {
            bias = 0;
            cmsLog_error("Laser driver IOCTL error on LASER_IOCTL_GET_BIAS_CURRENT");
        }

        // unit is 0.1 uW
        if (omciUtil_ioctlWrap(laserFd, LASER_IOCTL_GET_TX_PWR, &txPower) < 0)
        {
            txDb = 0;
            cmsLog_error("Laser driver IOCTL error on LASER_IOCTL_GET_TX_PWR");
        }
        else
        {
            txDb = convertPointOneMicroWattsToOmcidB("TX", txPower, 
              -32768, 32767, 2, 0);
        }

        // unit is 0.1 uW
        if (omciUtil_ioctlWrap(laserFd, LASER_IOCTL_GET_RX_PWR, &rxPower) < 0)
        {
            rxDb = 0;
            cmsLog_error("Laser driver IOCTL error on LASER_IOCTL_GET_RX_PWR");
        }
        else
        {
            rxDb = convertPointOneMicroWattsToOmcidB("RX", rxPower, 
              -32768, 32767, 2, 0);
        }

#ifndef DESKTOP_LINUX
        close(laserFd);
#endif /* DESKTOP_LINUX */

        // fill test result
        // fill vcc, type=1
        packet.A.msg[msgOffset++] = 1;
        packet.A.msg[msgOffset++] = (UINT8)((vcc >> 8) & 0xff);
        packet.A.msg[msgOffset++] = (UINT8)(vcc & 0xff);
        // fill rxPower, type=3
        packet.A.msg[msgOffset++] = 3;
        packet.A.msg[msgOffset++] = (UINT8)((rxDb >> 8) & 0xff);
        packet.A.msg[msgOffset++] = (UINT8)(rxDb & 0xff);
        // fill txPower, type=5
        packet.A.msg[msgOffset++] = 5;
        packet.A.msg[msgOffset++] = (UINT8)((txDb >> 8) & 0xff);
        packet.A.msg[msgOffset++] = (UINT8)(txDb & 0xff);
        // fill bias, type=9
        packet.A.msg[msgOffset++] = 9;
        packet.A.msg[msgOffset++] = (UINT8)((bias >> 8) & 0xff);
        packet.A.msg[msgOffset++] = (UINT8)(bias & 0xff);
        // fill temperature, type=12
        packet.A.msg[msgOffset++] = 12;
        packet.A.msg[msgOffset++] = (UINT8)((temperature >> 8) & 0xff);
        packet.A.msg[msgOffset++] = (UINT8)(temperature & 0xff);
    }
    omci_alarm_sendRaw("test_aniG", &packet, 0, 0);
}

static void test_timeout(BCM_OMCIPM_TEST_ENTRY *e)
{
    switch( e->meID)
    {
        case 134: //IP host config
            switch (e->testMode)
            {
                case 1: // ping test
                case 2: // trace route
                    test_iphost_timeout(e);
                    break;
                default:
                    // not support
                    break;
            }
            break;
        case 263: //ANI-G
            test_aniG_timeout(e);
            break;
        case 6: //circuit pack
        case 53: //PPTP POTS UNI
        case 80: //PPTP ISDN UNI
        case 256: //ONT-G
        case 304: //dot1ag MEP
        default:
            // not support
            break;
    }
}

/*========================= PUBLIC FUNCTIONS ==========================*/

UINT8 omci_test_iphost_ping(
    UINT16 tcID,
    UINT16 meID,
    UINT16 objID,
    UINT8 test,
    UINT8 *ipAddr)
{
    BCM_OMCIPM_TEST_ENTRY* newEntry = NULL;
    char cmdLine[BUFLEN_1024];
    char host[BUFLEN_16];
    int pid = 0;
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    char currIpAddr[BUFLEN_16];
#endif

    // check duplicated tcID
    if (test_exist(tcID) == TRUE)
        return OMCI_MSG_RESULT_PARM_ERROR;

    sprintf(host, "%u.%u.%u.%u", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    // specify source ip address of icmp packet
    if (test_getCurrentIpAddress(objID, currIpAddr) == TRUE)
        sprintf(cmdLine, "-q -m -I %s -i %u -c %d -s %d %s", currIpAddr, tcID,
                    OMCI_TEST_PING_COUNT, OMCI_TEST_PING_DATA_LEN, host);
    else
#endif
        sprintf(cmdLine, "-q -m -i %u -c %d -s %d %s", tcID,
                    OMCI_TEST_PING_COUNT, OMCI_TEST_PING_DATA_LEN, host);

    cmsLog_debug("ping command string=%s", cmdLine);

    pid = _owapi_rut_startPing(cmdLine);

    if (pid == CMS_INVALID_PID)
    {
        cmsLog_error("failed to start PING test.");
        return OMCI_MSG_RESULT_PROC_ERROR;
    }
    else
    {
        cmsLog_debug("Start PING msg sent, new PING pid=%d", pid);
    }

    // Attempt to allocate entry & test result.
    newEntry = (BCM_OMCIPM_TEST_ENTRY*)cmsMem_alloc(sizeof(BCM_OMCIPM_TEST_ENTRY),
      ALLOC_ZEROIZE);

    if (newEntry == NULL)
        return OMCI_MSG_RESULT_PROC_ERROR;

    // Setup test rec.
    newEntry->tcID = tcID;
    newEntry->meID = meID;
    newEntry->objID = objID;
    newEntry->testMode = test;
    newEntry->expireTime = time(NULL) + OMCI_TEST_PING_TIMEOUT;

    // Append new test entry to linked-list.
    BCM_COMMON_LL_APPEND(&testLL, newEntry);

    return OMCI_MSG_RESULT_SUCCESS;
}

void omci_test_iphost_ping_result(
    OmciPingDataMsgBody *msgPtr)
{
    int i = 0, msgOffset = 0;
    BCM_OMCIPM_TEST_ENTRY *entry;
    OmciPingDataMsgBody *pingInfo = msgPtr;
    omciPacket packet;

    cmsLog_debug("Receive IPHost Ping result message tcid=%d result=%d",
                 pingInfo->tcid, pingInfo->result);

    memset(&packet, 0, sizeof(omciPacket));

    //find test entry by tcid
    entry = test_find(pingInfo->tcid);
    if (entry == FALSE)
    {
        cmsLog_notice("Can not find tcid %d", pingInfo->tcid);
        return;
    }

    cmsLog_debug("Find matched entry meClass=%d Instance=%d",
                 entry->meID, entry->objID);

    //fill test result header
    test_fill_result_header(entry, &packet);

    // test successfully
    if (pingInfo->result == CMSRET_SUCCESS)
    {
        OMCI_PACKET_MSG(&packet)[msgOffset++] = 0x01;
        //fill meaningful bytes
        OMCI_PACKET_MSG(&packet)[msgOffset++] = OMCI_TEST_PING_COUNT * 2;
        //fill response time
        for (i = 0; i < OMCI_TEST_PING_COUNT; i++)
        {
            if (pingInfo->msg.responseTime[i] == 0)
            {
                //reponse timeout
                OMCI_PACKET_MSG(&packet)[msgOffset++] = 0xff;
                OMCI_PACKET_MSG(&packet)[msgOffset++] = 0xff;
            }
            else
            {
                OMCI_PACKET_MSG(&packet)[msgOffset++] =
                  (pingInfo->msg.responseTime[i] >> 8) & 0xff;
                OMCI_PACKET_MSG(&packet)[msgOffset++] =
                  pingInfo->msg.responseTime[i] & 0xff;
            }
        }
    }
    else // test failed
    {
        // unexpected reply
        if (pingInfo->msg.icmpReply[0])
        {
            //fill test result
            OMCI_PACKET_MSG(&packet)[msgOffset++] = 0x03;
            //fill meaningful bytes
            OMCI_PACKET_MSG(&packet)[msgOffset++] = 30;
            //copy reply packet
            memcpy(&OMCI_PACKET_MSG(&packet)[msgOffset], pingInfo->msg.icmpReply, 30);
        }
        else
        {
            //fill test result as timeout
            OMCI_PACKET_MSG(&packet)[msgOffset++] = 0x00;
            //fill meaningful bytes
            OMCI_PACKET_MSG(&packet)[msgOffset++] = 0;
        }
    }

    omci_alarm_sendRaw("test_iphost_ping", &packet, 0, 0);

    // Remove entry from list.
    BCM_COMMON_LL_REMOVE(&testLL, entry);
}

UINT8 omci_test_iphost_trace_route(
    UINT16 tcID,
    UINT16 meID,
    UINT16 objID,
    UINT8 test,
    UINT8 *ipAddr)
{
    BCM_OMCIPM_TEST_ENTRY* newEntry = NULL;
    char cmdLine[BUFLEN_1024];
    char host[BUFLEN_16];
    int pid = 0;
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    char currIpAddr[BUFLEN_16];
#endif

    // check duplicated tcID
    if (test_exist(tcID))
        return OMCI_MSG_RESULT_PARM_ERROR;

    sprintf(host, "%u.%u.%u.%u", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);

    //send 1 probe per ttl, wait for 3 seconds per probe
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
    //specify source ip address of traceroute packet
    if (test_getCurrentIpAddress(objID, currIpAddr) == TRUE)
        sprintf(cmdLine, "-q 1 -w 3 -s %s -m %d -M -T %u %s",
          currIpAddr, OMCI_TEST_TRACERT_MAX_HOP, tcID, host);
    else
#endif
        sprintf(cmdLine, "-q 1 -w 3 -m %d -M -T %u %s",
          OMCI_TEST_TRACERT_MAX_HOP, tcID, host);

    cmsLog_notice("traceroute command string=%s", cmdLine);

    pid = _owapi_rut_startTraceroute(cmdLine);

    if (pid == CMS_INVALID_PID)
    {
        cmsLog_error("failed to start Traceroute test.");
        return OMCI_MSG_RESULT_PROC_ERROR;
    }
    else
    {
        cmsLog_notice("Start Traceroute msg sent, new traceroute pid=%d", pid);
    }

    // Attempt to allocate entry & test result.
    newEntry = (BCM_OMCIPM_TEST_ENTRY*)cmsMem_alloc(sizeof(BCM_OMCIPM_TEST_ENTRY),
      ALLOC_ZEROIZE);

    if (newEntry == NULL)
        return OMCI_MSG_RESULT_PROC_ERROR;

    // Setup test rec.
    newEntry->tcID = tcID;
    newEntry->meID = meID;
    newEntry->objID = objID;
    newEntry->testMode = test;
    newEntry->expireTime = time(NULL) + OMCI_TEST_TRACERT_TIMEOUT;
    // Append new test entry to linked-list.
    BCM_COMMON_LL_APPEND(&testLL, newEntry);

    return OMCI_MSG_RESULT_SUCCESS;
}

void omci_test_iphost_trace_route_result(
    OmciTracertDataMsgBody *msgPtr)
{
    int msgOffset = 0;
    BCM_OMCIPM_TEST_ENTRY *entry = NULL;
    OmciTracertDataMsgBody *tracertInfo = msgPtr;
    omciPacket packet;

    cmsLog_debug("Receive IPHost Traceroute result message tcid=%d result=%d",
                 tracertInfo->tcid, tracertInfo->result);

    memset(&packet, 0, sizeof(omciPacket));

    //find test entry by tcid
    entry = test_find(tracertInfo->tcid);
    if (entry == NULL)
    {
        cmsLog_notice("Can not find tcid %d", tracertInfo->tcid);
        return;
    }

    cmsLog_debug("Find matched entry meClass=%d Instance=%d",
                 entry->meID, entry->objID);

    //fill test result header
    test_fill_result_header(entry, &packet);

    // test successfully
    if (tracertInfo->result == 0x2) /* time exceeded */
    {
        OMCI_PACKET_MSG(&packet)[msgOffset++] = 0x02;
        //fill meaningful bytes
        OMCI_PACKET_MSG(&packet)[msgOffset++] = tracertInfo->hops * sizeof(UINT32);
        //fill hops
        memcpy(&OMCI_PACKET_MSG(&packet)[msgOffset], tracertInfo->msg.neighbour,
               tracertInfo->hops * sizeof(UINT32));
    }
    else if (tracertInfo->result == 0x3)
    {
        // unexpected reply
        //fill test result
        OMCI_PACKET_MSG(&packet)[msgOffset++] = 0x03;
        //fill meaningful bytes
        OMCI_PACKET_MSG(&packet)[msgOffset++] = 30;
        //copy reply packet
        memcpy(&OMCI_PACKET_MSG(&packet)[msgOffset], tracertInfo->msg.icmpReply, 30);
    }
    else // timeout
    {
        //fill test result as timeout
        OMCI_PACKET_MSG(&packet)[msgOffset++] = 0x00;
        //fill meaningful bytes
        OMCI_PACKET_MSG(&packet)[msgOffset++] = 0;
    }

    omci_alarm_sendRaw("test_iphost_trace_route", &packet, 0, 0);

    // Remove entry from list.
    BCM_COMMON_LL_REMOVE(&testLL, entry);
}

UINT8 omci_test_aniG(UINT16 tcID, UINT16 meID, UINT16 objID)
{
    BCM_OMCIPM_TEST_ENTRY* newEntry;

    // check duplicated tcID
    if (test_exist(tcID) == TRUE)
        return OMCI_MSG_RESULT_PARM_ERROR;

    // Attempt to allocate entry & test result.
    newEntry = (BCM_OMCIPM_TEST_ENTRY*)cmsMem_alloc(
      sizeof(BCM_OMCIPM_TEST_ENTRY), ALLOC_ZEROIZE);

    if (newEntry == NULL)
        return OMCI_MSG_RESULT_PROC_ERROR;

    // Setup test rec.
    newEntry->tcID = tcID;
    newEntry->meID = meID;
    newEntry->objID = objID;
    newEntry->expireTime = time(NULL);
    // Append new test entry to linked-list.
    BCM_COMMON_LL_APPEND(&testLL, newEntry);

    return OMCI_MSG_RESULT_SUCCESS;
}

void omci_poll_test(void)
{
    BCM_OMCIPM_TEST_ENTRY* entryPtr;
    BCM_OMCIPM_TEST_ENTRY* nextEntryPtr;
    time_t currentTime = time(NULL);

    // Get 1st entry (if any).
    entryPtr = BCM_COMMON_LL_GET_HEAD(testLL);

    // Loop until all alarm entries are released as defined by end-of-list.
    while (entryPtr != NULL)
    {
        // Get list's next entry.
        nextEntryPtr = BCM_COMMON_LL_GET_NEXT(entryPtr);

        // check expire time
        if (entryPtr->expireTime <= currentTime)
        {
            cmsLog_notice("Test timeout tcid=%d meClass=%d Instance=%d type=%d",
              entryPtr->tcID, entryPtr->meID, entryPtr->objID, entryPtr->testMode);

            // handle test timeout
            test_timeout(entryPtr);

            // Remove entry from list.
            BCM_COMMON_LL_REMOVE(&testLL, entryPtr);

            // Free current entry.
            CMSMEM_FREE_BUF_AND_NULL_PTR(entryPtr);
        }

        // Setup list's next entry.
        entryPtr = nextEntryPtr;
    }
}

void omci_test_releaseAll(void)
{
    BCM_OMCIPM_TEST_ENTRY* entryPtr = NULL;
    BCM_OMCIPM_TEST_ENTRY* nextEntryPtr = NULL;

    // Get 1st entry (if any).
    entryPtr = BCM_COMMON_LL_GET_HEAD(testLL);

    // Loop until all alarm entries are released as defined by end-of-list.
    while (entryPtr != NULL)
    {
        // Get list's next entry.
        nextEntryPtr = BCM_COMMON_LL_GET_NEXT(entryPtr);

        // Remove entry from list.
        BCM_COMMON_LL_REMOVE(&testLL, entryPtr);

        // Free current entry.
        CMSMEM_FREE_BUF_AND_NULL_PTR(entryPtr);

        // Setup list's next entry.
        entryPtr = nextEntryPtr;
    }
}
