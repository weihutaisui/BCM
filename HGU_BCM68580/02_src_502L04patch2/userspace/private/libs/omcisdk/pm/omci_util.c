/***********************************************************************
 *
 *  Copyright (c) 2007 Broadcom
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


#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "owsvc_api.h"
#include "omciutl_cmn.h"
#include "omci_util.h"
#include "omci_pm_util.h"
#include "rut_gpon.h"


static const char invalid[] = "*** Invalid ***";

const char const *omciMsgName[OMCI_PACKET_MT_MAX] = {
    [0]  = invalid,
    [1]  = invalid,
    [2]  = invalid,
    [3]  = invalid,
    [4]  = "Create",
    [5]  = "CreateCompleteConnection",
    [6]  = "Delete",
    [7]  = "DeleteCompleteConnection",
    [8]  = "Set",
    [9]  = "Get",
    [10] = "GetCompleteConnection",
    [11] = "GetAllAlarms",
    [12] = "GetAllAlarmsNext",
    [13] = "MIBUpload",
    [14] = "MIBUploadNext",
    [15] = "MIBReset",
    [16] = "Alarm",
    [17] = "AttributeValueChange",
    [18] = "Test",
    [19] = "StartSoftwareDownload",
    [20] = "DownloadSection",
    [21] = "EndSoftwareDownload",
    [22] = "ActivateSoftware",
    [23] = "CommitSoftware",
    [24] = "SynchronizeTime",
    [25] = "Reboot",
    [26] = "GetNext",
    [27] = "TestResult",
    [28] = "GetCurrentData",
    [29] = "SetTable",
    [30] = invalid,
    [31] = invalid
};


/*
 * local functions
 */

void omciUtl_dumpMem(unsigned char *pBuf, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (i % 4 == 0)
        {
            printf("\n0x%04X: ", i);
        }
        printf("%02X ", *pBuf++);
    }
    printf("\n\n");
}

void omciUtl_dumpMemToFile(FILE *fs, unsigned char *pBuf, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (i % 4 == 0)
        {
            fprintf(fs, "\n0x%04X: ", i);
        }
        fprintf(fs, "%02X ", *pBuf++);
    }
    fprintf(fs, "\n\n");
}

void omciUtl_dumpPacketDetailed(omciPacket *pPacket, int len)
{
    UINT16 tcId;
    UINT16 meClass;
    UINT16 meInst;

    tcId = OMCI_PACKET_TC_ID(pPacket);
    meClass = OMCI_PACKET_ME_CLASS(pPacket);
    meInst = OMCI_PACKET_ME_INST(pPacket);

    printf("Length         : %d\n\n", len);
    //    dump_mem((unsigned char *)(pPacket), len);

    printf("Transaction ID : 0x%02X%02X\n", pPacket->tcId[0], pPacket->tcId[1]);
    printf("      Priority : 0x%04X\n", OMCI_PACKET_PRIORITY(pPacket));
    printf("          TcId : 0x%04X\n", tcId);

    printf("Message Type   : 0x%02X\n", pPacket->msgType);
    printf("            DB : 0x%02X\n", OMCI_PACKET_DB(pPacket));
    printf("            AR : 0x%02X\n", OMCI_PACKET_AR(pPacket));
    printf("            AK : 0x%02X\n", OMCI_PACKET_AK(pPacket));
    //printf("            MT :   %02d '%s'\n", OMCI_PACKET_MT(pPacket),
    //       omciMsgName[OMCI_PACKET_MT(pPacket)]);

    printf("Device ID      : 0x%02X\n", pPacket->devId);

    printf("ME Class       : %05d (0x%04X)\n", meClass, meClass);
    printf("ME Instance    : %05d (0x%04X)\n", meInst, meInst);

    printf("\nHeader  :");
    omciUtl_dumpMem((unsigned char *)(pPacket), OMCI_PACKET_A_HDR_SIZE);

    printf("Message :");
    omciUtl_dumpMem((unsigned char *)(OMCI_PACKET_MSG(pPacket)), OMCI_PACKET_A_MSG_SIZE);

    if (OMCI_CHECK_DEV_ID_B(pPacket))
    {
        printf("MIC :");
        omciUtl_dumpMem((unsigned char *)OMCI_PACKET_CRC(pPacket), OMCI_PACKET_MIC_SIZE);
    }
    else
    {
        printf("Trailer :");
        omciUtl_dumpMem((unsigned char *)(pPacket->A.trailer), OMCI_PACKET_A_TRAILER_SIZE);
    }
}

void omciUtl_dumpPacketDetailedToFile(FILE *fs, omciPacket *pPacket, int len)
{
    UINT16 tcId;
    UINT16 meClass;
    UINT16 meInst;

    tcId = OMCI_PACKET_TC_ID(pPacket);
    meClass = OMCI_PACKET_ME_CLASS(pPacket);
    meInst = OMCI_PACKET_ME_INST(pPacket);

    fprintf(fs, "Length         : %d\n\n", len);
    //    dump_mem((unsigned char *)(pPacket), len);

    fprintf(fs, "Transaction ID : 0x%02X%02X\n", pPacket->tcId[0], pPacket->tcId[1]);
    fprintf(fs, "      Priority : 0x%04X\n", OMCI_PACKET_PRIORITY(pPacket));
    fprintf(fs, "          TcId : 0x%04X\n", tcId);

    fprintf(fs, "Message Type   : 0x%02X\n", pPacket->msgType);
    fprintf(fs, "            DB : 0x%02X\n", OMCI_PACKET_DB(pPacket));
    fprintf(fs, "            AR : 0x%02X\n", OMCI_PACKET_AR(pPacket));
    fprintf(fs, "            AK : 0x%02X\n", OMCI_PACKET_AK(pPacket));
    //fprintf(fs, "            MT :   %02d '%s'\n", OMCI_PACKET_MT(pPacket),
    //       omciMsgName[OMCI_PACKET_MT(pPacket)]);

    fprintf(fs, "Device ID      : 0x%02X\n", pPacket->devId);

    fprintf(fs, "ME Class       : %05d (0x%04X)\n", meClass, meClass);
    fprintf(fs, "ME Instance    : %05d (0x%04X)\n", meInst, meInst);

    fprintf(fs, "\nHeader  :");
    omciUtl_dumpMemToFile(fs, (unsigned char *)(pPacket), OMCI_PACKET_A_HDR_SIZE);

    fprintf(fs, "Message :");
    omciUtl_dumpMemToFile(fs, (unsigned char *)(OMCI_PACKET_MSG(pPacket)), OMCI_PACKET_A_MSG_SIZE);

    if (OMCI_CHECK_DEV_ID_B(pPacket))
    {
        fprintf(fs, "MIC :");
        omciUtl_dumpMemToFile(fs, (unsigned char *)OMCI_PACKET_CRC(pPacket), OMCI_PACKET_MIC_SIZE);
    }
    else
    {
        fprintf(fs, "Trailer :");
        omciUtl_dumpMemToFile(fs, (unsigned char *)(pPacket->A.trailer), OMCI_PACKET_A_TRAILER_SIZE);
    }
}

void omciUtl_dumpMemWidth(unsigned char *pBuf, int len, int width)
{
    int i;

    for (i = 0; i < len; i++) {
        if (i % width == 0)
        {
            printf("\n0x%04X: ", i);
        }
        printf("%02X ", *pBuf++);
        if ((i+1) % 4 == 0)
        {
            printf(" ");
        }
    }
}

void omciUtl_dumpMemWidthToFile(FILE *fs, unsigned char *pBuf, int len, int width)
{
    int i;

    for (i = 0; i < len; i++) {
        if (i % width == 0)
        {
            fprintf(fs, "\n0x%04X: ", i);
        }
        fprintf(fs, "%02X ", *pBuf++);
        if ((i+1) % 4 == 0)
        {
            fprintf(fs, " ");
        }
    }
}

void omciUtl_dumpPacket(omciPacket *pPacket, int len)
{
    UINT16 tcId;
    UINT16 meClass;
    UINT16 meInst;

    tcId = OMCI_PACKET_TC_ID(pPacket);
    meClass = OMCI_PACKET_ME_CLASS(pPacket);
    meInst = OMCI_PACKET_ME_INST(pPacket);

#if 0
    omciUtl_dumpPacketDetailed(pPacket, len);
#else
    printf("Transaction ID <0x%02X%02X> : Prio <%d>, tcId <%d>\n",
           pPacket->tcId[0], pPacket->tcId[1], OMCI_PACKET_PRIORITY(pPacket), tcId);

    printf("Message Type <0x%02X> : DB <0x%02X>, AR <0x%02X>, AK <0x%02X>, MT <%d> <%s>\n",
           pPacket->msgType, OMCI_PACKET_DB(pPacket), OMCI_PACKET_AR(pPacket),
           OMCI_PACKET_AK(pPacket), OMCI_PACKET_MT(pPacket), omciMsgName[OMCI_PACKET_MT(pPacket)]);

    printf("Device ID <0x%02X>\n", pPacket->devId);

    printf("ME Class <%d>, Instance <%d>\n", meClass, meInst);

    omciUtl_dumpMemWidth((unsigned char *)(pPacket),
                         OMCI_PACKET_A_HDR_SIZE, OMCI_PACKET_A_HDR_SIZE);
    omciUtl_dumpMemWidth((unsigned char *)(OMCI_PACKET_MSG(pPacket)),
                         OMCI_PACKET_A_MSG_SIZE, 16);
    if (len == OMCI_PACKET_A_SIZE)
    {
        omciUtl_dumpMemWidth((unsigned char *)(pPacket->A.trailer),
                             OMCI_PACKET_A_TRAILER_SIZE, OMCI_PACKET_A_TRAILER_SIZE);
    }
    printf("\n\n");
#endif
}

void omciUtl_dumpPacketToFile(FILE *fs, omciPacket *pPacket, int len __attribute__((unused)))
{
    UINT16 tcId;
    UINT16 meClass;
    UINT16 meInst;

    tcId = OMCI_PACKET_TC_ID(pPacket);
    meClass = OMCI_PACKET_ME_CLASS(pPacket);
    meInst = OMCI_PACKET_ME_INST(pPacket);

#if 0
    omciUtl_dumpPacketDetailedToFile(fs, pPacket, len);
#else
    fprintf(fs, "Transaction ID <0x%02X%02X> : Prio <%d>, tcId <%d>\n",
            pPacket->tcId[0], pPacket->tcId[1],
            OMCI_PACKET_PRIORITY(pPacket), tcId);

    fprintf(fs, "Message Type <0x%02X> : DB <0x%02X>, AR <0x%02X>, AK <0x%02X>, MT <%d> <%s>\n",
            pPacket->msgType, OMCI_PACKET_DB(pPacket), OMCI_PACKET_AR(pPacket),
            OMCI_PACKET_AK(pPacket), OMCI_PACKET_MT(pPacket), omciMsgName[OMCI_PACKET_MT(pPacket)]);

    fprintf(fs, "Device ID <0x%02X>\n", pPacket->devId);

    fprintf(fs, "ME Class <%d>, Instance <%d>\n", meClass, meInst);

    omciUtl_dumpMemWidthToFile(fs, (unsigned char *)(pPacket),
        OMCI_PACKET_A_HDR_SIZE, OMCI_PACKET_A_HDR_SIZE);
    omciUtl_dumpMemWidthToFile(fs, (unsigned char *)(OMCI_PACKET_MSG(pPacket)),
        OMCI_PACKET_A_MSG_SIZE, 16);
    omciUtl_dumpMemWidthToFile(fs, (unsigned char *)(pPacket->A.trailer),
        OMCI_PACKET_A_TRAILER_SIZE, OMCI_PACKET_A_TRAILER_SIZE);
    fprintf(fs, "\n\n");
#endif
}

CmsRet omciUtl_sendAttributeValueChange(UINT32 oid, UINT16 meId, UINT16 attrMask,
   UINT8 *attrValue, UINT16 attrSize)
{
    CmsRet ret = CMSRET_SUCCESS;

#if !defined(OMCID_DISABLE_AVC)
    int j = 0;
#ifndef DESKTOP_LINUX
    int count = 0;
    int gponFileDesc = 0;
#endif /* DESKTOP_LINUX */
    omciPacket packet;
    OmciMdmOidClassId_t info;


    memset(&packet, 0, sizeof(omciPacket));

    /* update header */
    packet.tcId[0] = 0;
    packet.tcId[1] = 0;
    packet.msgType = OMCI_MSG_TYPE_ATTRIBUTEVALUECHANGE;
    packet.devId   = OMCI_PACKET_DEV_ID_A;
    // convert object OID to management entity class ID
    memset(&info, 0, sizeof(OmciMdmOidClassId_t));
    info.mdmOid = oid;
    omciUtl_oidToClassId(&info);
    OMCI_HTONS(&packet.classNo, info.classId);
    OMCI_HTONS(&packet.instId, meId);

    /* update message contents */
    // attribute mask start at msg[0] and has 2 bytes
    OMCI_HTONS(&OMCI_PACKET_MSG(&packet)[0], attrMask);
    // attribute value starts at msg[2] and end at attrSize or msg[32]
    for (j = 0; j < attrSize && j < 31; j++)
    {
        OMCI_PACKET_MSG(&packet)[j+2] = attrValue[j];
    }

    cmsLog_notice(" Attribute Value Change Message:\n");
    OMCI_DUMP_PACKET(&packet, OMCI_PACKET_A_SIZE);

#ifndef DESKTOP_LINUX
    // BCMGPON_FILE is defined in omci_api.h
    cmsLog_notice("Opening %s...", BCMGPON_FILE);
    gponFileDesc = open(BCMGPON_FILE, O_WRONLY);
    if (gponFileDesc < 0)
    {
        cmsLog_error("Error opening file: %s", strerror(errno));
        return CMSRET_INTERNAL_ERROR;
    }

    /* transmit response packet */
    count = write(gponFileDesc, (unsigned char *)&packet,
                  OMCI_PACKET_A_SIZE - OMCI_PACKET_MIC_SIZE);
    if (count < 0)
    {
        cmsLog_error("Error writing to file: %s", strerror(errno));
        ret = CMSRET_INTERNAL_ERROR;
    }

    cmsLog_notice("Closing %s", BCMGPON_FILE);
    (void)close(gponFileDesc);
#endif /* DESKTOP_LINUX */

#endif

    return ret;
}

CmsRet omciUtl_HexToAscii(char *string, const char *binString, int len)
{
    char buf[3];
    int i;

    if (len%2)
    {
        cmsLog_error("String must have an even number of chars (len = %d)", len);
        return CMSRET_INVALID_ARGUMENTS;
    }

    for (i=0; i<len; i+=2)
    {
        (void)snprintf(buf, 3, "%02X", binString[i/2] & 0xFF);
        string[i]   = buf[0];
        string[i+1] = buf[1];
        //        printf("%s ", buf);
    }
    //    printf("\n");

    /* set terminating null character */
    string[len] = '\0';

    return CMSRET_SUCCESS;
}

CmsRet omciUtl_AsciiToHex(char *binString, const char *string, int len)
{
    char buf[3];
    int i;

    if (len%2)
    {
        cmsLog_error("String must have an even number of chars (len = %d)", len);
        return CMSRET_INVALID_ARGUMENTS;
    }

    /* set terminating null character */
    buf[2] = '\0';

    errno = 0;
    for (i=0; i<len; i+=2)
    {
        buf[0] = string[i];
        buf[1] = string[i+1];
        //        printf("%d: %s: ", i, buf);

        binString[i/2] = (char)strtoul(buf, (char **)NULL, 16);
        if (errno)
        {
            cmsLog_error("%s: %s", strerror(errno), buf);
            return CMSRET_INVALID_ARGUMENTS;
        }
        //        printf("%d \n", binString[i/2] & 0xFF);
    }
    //    printf("\n");

    return CMSRET_SUCCESS;
}

#ifdef DESKTOP_LINUX
int omciUtil_ioctlWrap(int fd __attribute__((unused)),
  int req __attribute__((unused)),
  void *argp __attribute__((unused)))
{
    cmsLog_debug("fd=%d, req=%d, argp=0x%x", fd, req, argp);
    return 0;
}
#else
int omciUtil_ioctlWrap(int fd, int req, void *argp)
{
    int rc;

    rc = ioctl(fd, req, argp);
    return rc;
}
#endif /* DESKTOP_LINUX */


OmciEthPortType omciUtil_getPortType(UINT8 port, UINT32 typesAll)
{
    OmciEthPortType portType = OMCI_ETH_PORT_TYPE_NONE;
    OmciEthPortType_t eth;

    eth.types.all = typesAll;

    switch (port)
    {
        case 0:
            portType = eth.types.ports.eth0;
            break;
        case 1:
            portType = eth.types.ports.eth1;
            break;
        case 2:
            portType = eth.types.ports.eth2;
            break;
        case 3:
            portType = eth.types.ports.eth3;
            break;
        case 4:
            portType = eth.types.ports.eth4;
            break;
        case 5:
            portType = eth.types.ports.eth5;
            break;
        case 6:
            portType = eth.types.ports.eth6;
            break;
        case 7:
            portType = eth.types.ports.eth7;
            break;
        default:
            cmsLog_error("Invalid port=%d, val=0x%08x", port, typesAll);
            break;
    }

    return portType;
}
