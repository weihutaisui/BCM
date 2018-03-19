/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

/***************************************************************************
 * File Name  : omcipmdrvcmd.c
 * Description: Linux command line utility that controls the Broadcom
 *              BCM968xx PLOAM Driver.
 ***************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "os_defs.h"

#include "omcipmdrvctl.h"
#include "omcipm_api.h"

static int helpHandler(POPTION_INFO pOptions, int nNumOptions);
static int getFecCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getGemPortCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getGalEnetCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getEnetCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getEnet2CountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getEnet3CountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getEnetDnCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getEnetUpCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getBridgeCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getBridgePortCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getMocaEnetCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getMocaIntfCountersHandler(POPTION_INFO pOptions, int nNumOptions);
static int getMocaIntfEntryCountersHandler(POPTION_INFO pOptions, int nNumOptions);

char g_PgmName[128] = {0};

/*** File Scoped Globals. ***/
COMMAND_INFO g_Cmds[] =
{
    {"--help",             {""}, helpHandler},
    {"getFec",             {""}, getFecCountersHandler},
    {"getGemPort",         {"--port", ""}, getGemPortCountersHandler},
    {"getGalEnet",         {"--port", ""}, getGalEnetCountersHandler},
    {"getEnet",            {"--port", ""}, getEnetCountersHandler},
    {"getEnet2",           {"--port", ""}, getEnet2CountersHandler},
    {"getEnet3",           {"--port", ""}, getEnet3CountersHandler},
    {"getEnetDn",          {"--port", ""}, getEnetDnCountersHandler},
    {"getEnetUp",          {"--port", ""}, getEnetUpCountersHandler},
    {"getBridge",          {"--port", ""}, getBridgeCountersHandler},
    {"getBridgePort",      {"--port", ""}, getBridgePortCountersHandler},
    {"getMocaEnet",        {"--port", ""}, getMocaEnetCountersHandler},
    {"getMocaIntf",        {""}, getMocaIntfCountersHandler},
    {"getMocaIntfEntry",   {"--port", ""}, getMocaIntfEntryCountersHandler},
    {""}
};

/*
 *------------------------------------------------------------------------------
 * Function Name: usage
 * Description  : Displays the OMCI PM To Drivers control utility usage
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
void usage(void)
{
    printf(
        "\n\nOMCI PM To Drivers Control Utility:\n\n"
        "::: Usage:\n\n"
        "       %s --help\n\n"

        ":::::: GPON Counters ::::::\n\n"
        "       %s getFec\n"
        "       %s getGemPort\n"
        "                    [ --port <0..31> ]\n"
        "       %s getGalEnet\n"
        "                    [ --port <0..31> ]\n\n"

        ":::::: Ethernet Counters ::::::\n\n"
        "       %s getEnet\n"
        "                    [ --port <0..3> ]\n"
        "       %s getEnet2\n"
        "                    [ --port <0..3> ]\n"
        "       %s getEnet3\n"
        "                    [ --port <0..3> ]\n"
        "       %s getEnetDn\n"
        "                    [ --port <0..4095> ]\n"
        "       %s getEnetUp\n"
        "                    [ --port <0..4095> ]\n"
        "       %s getBridge\n"
        "                    [ --port <0..3> ]\n"
        "       %s getBridgePort\n"
        "                    [ --port <0..3> ]\n\n"

        ":::::: MoCA Counters ::::::\n\n"
        "       %s getMocaEnet\n"
        "                    [ --port <0..1> ]\n"
        "       %s getMocaIntf\n"
        "       %s getMocaIntfEntry\n"
        "                    [ --port <0..16> ]\n\n"

        , g_PgmName, g_PgmName, g_PgmName, g_PgmName
        , g_PgmName, g_PgmName, g_PgmName, g_PgmName
        , g_PgmName, g_PgmName, g_PgmName, g_PgmName
        , g_PgmName, g_PgmName );
} /* usage */

/*
 *------------------------------------------------------------------------------
 * Function Name: omcipmdrvctl_getPortId
 * Description  : Retrieve value of port ID from command line
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static omcipmdrvCtl_cmd_t omcipmdrvctl_getPortId(POPTION_INFO pOptions,
    char *name, int max, int *port)
{
    omcipmdrvCtl_cmd_t ret = OMCIPMDRVCTL_SUCCESS;

    if (pOptions->pOptName == NULL)
    {
        fprintf(stderr,
                "%s: invalid number of options\n",
                g_PgmName);
        ret = OMCIPMDRVCTL_INVALID_NUMBER_OF_OPTIONS;
    }
    else if (!strcmp(pOptions->pOptName, "--port"))
    {
        if (pOptions->nNumParms != 1)
        {
            fprintf(stderr,
                    "%s: identify %s <0..%d>\n",
                    g_PgmName, name, max);
            ret = OMCIPMDRVCTL_INVALID_OPTION;
        }
        *port = strtoul(pOptions->pParms[0], NULL, 0);
        if (*port > max)
        {
            fprintf(stderr,
                    "%s: invalid %s %d\n",
                    g_PgmName, name, *port);
            ret = OMCIPMDRVCTL_INVALID_PARAMETER;
        }
    }
    else    /* should never reach here */
    {
        fprintf(stderr, "%s: invalid option [%s]\n",
                g_PgmName, pOptions->pOptName);
        ret = OMCIPMDRVCTL_INVALID_OPTION;
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: helpHandler
 * Description  : Displays the OMCI PM To Drivers control utility usage
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int helpHandler(POPTION_INFO pOptions, int nNumOptions)
{
    usage();
    return 0;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getFecCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_FEC) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getFecCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_OMCI_PM_FEC_COUNTER counters;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_FEC, 0, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n========== FEC Counters ==========\n\n");
        printf("   Corrected Bytes           : %u\n", counters.correctedBytes);
        printf("   Corrected Code Words      : %u\n", counters.correctedCodeWords);
        printf("   Uncorrected Code Words    : %u\n", counters.uncorrectedCodeWords);
        printf("   Total Code Words          : %u\n", counters.totalCodeWords);
        printf("   FEC Seconds               : %u\n", counters.fecSeconds);
        printf("\n==================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getGemPortCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_GEM_PORT) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getGemPortCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_GEM_PORT_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "GEM port index", 255, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_GEM_PORT, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n====== Counters For GEM Port #%d ======\n\n", portId);
        printf("   Transmitted GEM Frames     : %u\n", counters.transmittedGEMFrames);
        printf("   Received GEM Frames        : %u\n", counters.receivedGEMFrames);
        printf("   Received Payload Bytes     : %u\n", counters.receivedPayloadBytes);
        printf("   Transmitted Payload Bytes  : %u\n", counters.transmittedPayloadBytes);
        printf("\n=======================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getGalEnetCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_GAL_ENET) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getGalEnetCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_GAL_ETHERNET_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "GEM port index", 31, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_GAL_ENET, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n=== Counters For GAL Ethernet Port #%d ===\n\n", portId);
        printf("   Discarded Frames  : %u\n", counters.discardedFrames);
        printf("   Transmitted Frames: %u\n", counters.transmittedFrames);
        printf("   Received Frames   : %u\n", counters.receivedFrames);
        printf("\n==========================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getEnetCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENET) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getEnetCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_ETHERNET_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "Ethernet port ID", 4095, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENET, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n====== Counters For Ethernet Port #%d ======\n\n", portId);
        printf("   FCS Errors                  : %u\n", counters.fcsErrors);
        printf("   Excessive Collision         : %u\n", counters.excessiveCollisionCounter);
        printf("   Late Collision              : %u\n", counters.lateCollisionCounter);
        printf("   Frame Too Long              : %u\n", counters.frameTooLongs);
        printf("   Buffer Overflows On Receive : %u\n", counters.bufferOverflowsOnReceive);
        printf("   Buffer Overflows On Transmit: %u\n", counters.bufferOverflowsOnTransmit);
        printf("   Single Collision Frame      : %u\n", counters.singleCollisionFrameCounter);
        printf("   Multiple Collisions Frame   : %u\n", counters.multipleCollisionsFrameCounter);
        printf("   SQE                         : %u\n", counters.sqeCounter);
        printf("   Deferred Transmission       : %u\n", counters.deferredTransmissionCounter);
        printf("   Internal MAC Transmit Error : %u\n", counters.internalMacTransmitErrorCounter);
        printf("   Carrier Sense Error         : %u\n", counters.carrierSenseErrorCounter);
        printf("   Alignment Error             : %u\n", counters.alignmentErrorCounter);
        printf("   Internal MAC Receive Error  : %u\n", counters.internalMacReceiveErrorCounter);
        printf("\n===========================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getEnet2CountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENET2) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getEnet2CountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_ETHERNET_2_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "Ethernet port ID", 4095, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENET2, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n==== Counters For Ethernet 2 Port #%d ====\n\n", portId);
        printf("   PPPOE Filter Frame: %u\n", counters.pppoeFilterFrameCounter);
        printf("\n=========================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getEnet3CountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENET3) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getEnet3CountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_ETHERNET_3_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "Ethernet port ID", 4095, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENET3, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n====== Counters For Ethernet 3 Port #%d ======\n\n", portId);
        printf("   Received Drop Events         : %u\n", counters.dropEvents);
        printf("   Received Octets              : %u\n", counters.octets);
        printf("   Received Packets             : %u\n", counters.packets);
        printf("   Received Broadcast Packets   : %u\n", counters.broadcastPackets);
        printf("   Received Multicast Packets   : %u\n", counters.multicastPackets);
        printf("   Received Undersize Packets   : %u\n", counters.undersizePackets);
        printf("   Received Fragments           : %u\n", counters.fragments);
        printf("   Received Jabbers             : %u\n", counters.jabbers);
        printf("   Received Packets64Octets     : %u\n", counters.packets64Octets);
        printf("   Received Packets127Octets    : %u\n", counters.packets127Octets);
        printf("   Received Packets255Octets    : %u\n", counters.packets255Octets);
        printf("   Received Packets511Octets    : %u\n", counters.packets511Octets);
        printf("   Received Packets1023ctets    : %u\n", counters.packets1023Octets);
        printf("   Received Packets1518Octets   : %u\n", counters.packets1518Octets);
        printf("\n=============================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getEnetDnCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENETDN) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getEnetDnCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "Ethernet port ID", 4095, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENETDN, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n====== Counters For Ethernet DS Port #%d ======\n\n", portId);
        printf("   Drop Events         : %u\n", counters.dropEvents);
        printf("   Octets              : %u\n", counters.octets);
        printf("   Packets             : %u\n", counters.packets);
        printf("   Broadcast Packets   : %u\n", counters.broadcastPackets);
        printf("   Multicast Packets   : %u\n", counters.multicastPackets);
        printf("   CRC Errored Packets : %u\n", counters.crcErroredPackets);
        printf("   Undersize Packets   : %u\n", counters.undersizePackets);
        printf("   Oversize Packets    : %u\n", counters.oversizePackets);
        printf("   Packets64Octets     : %u\n", counters.packets64Octets);
        printf("   Packets127Octets    : %u\n", counters.packets127Octets);
        printf("   Packets255Octets    : %u\n", counters.packets255Octets);
        printf("   Packets511Octets    : %u\n", counters.packets511Octets);
        printf("   Packets1023ctets    : %u\n", counters.packets1023Octets);
        printf("   Packets1518Octets   : %u\n", counters.packets1518Octets);
        printf("\n=============================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getEnetDUpCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENETUP) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getEnetUpCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_ETHERNET_UPDN_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "Ethernet port ID", 4095, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ENETUP, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n====== Counters For Ethernet US Port #%d ======\n\n", portId);
        printf("   Drop Events         : %u\n", counters.dropEvents);
        printf("   Octets              : %u\n", counters.octets);
        printf("   Packets             : %u\n", counters.packets);
        printf("   Broadcast Packets   : %u\n", counters.broadcastPackets);
        printf("   Multicast Packets   : %u\n", counters.multicastPackets);
        printf("   CRC Errored Packets : %u\n", counters.crcErroredPackets);
        printf("   Undersize Packets   : %u\n", counters.undersizePackets);
        printf("   Oversize Packets    : %u\n", counters.oversizePackets);
        printf("   Packets64Octets     : %u\n", counters.packets64Octets);
        printf("   Packets127Octets    : %u\n", counters.packets127Octets);
        printf("   Packets255Octets    : %u\n", counters.packets255Octets);
        printf("   Packets511Octets    : %u\n", counters.packets511Octets);
        printf("   Packets1023ctets    : %u\n", counters.packets1023Octets);
        printf("   Packets1518Octets   : %u\n", counters.packets1518Octets);
        printf("\n=============================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getBridgeCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_BRIDGE) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getBridgeCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_MAC_BRIDGE_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "Bridge omci index", 65535, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_BRIDGE, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n==== Counters For Bridge Port #%d ====\n\n", portId);
        printf("   Learning Discarded Entries: %u\n", counters.learningDiscaredEntries);
        printf("\n======================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getBridgePortCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_BRIDGE_PORT) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getBridgePortCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "Bridge port ID", 3, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_BRIDGE_PORT, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n==== Counters For Bridge Port #%d ====\n\n", portId);
        printf("   Forwarded Frames         : %u\n", counters.forwardedFrames);
        printf("   Delay Discarded Frames   : %u\n", counters.delayDiscardedFrames);
        printf("   MTU Discarded Frames     : %u\n", counters.mtuDiscardedFrames);
        printf("   Received Discarded Frames: %u\n", counters.receivedFrames);
        printf("   Received Frames          : %u\n", counters.receivedDiscardedFrames);
        printf("\n======================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getMocaEnetCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_MOCA_ENET) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getMocaEnetCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_MOCA_ETHERNET_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "MoCA port ID", 1, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_MOCA_ENET, portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n====== Counters For MoCA Ethernet Port #%d ======\n\n", portId);
        printf("   Incoming Unicast Packets  : %u\n", counters.incomingUnicastPackets);
        printf("   Incoming Discarded Packets: %u\n", counters.incomingDiscardedPackets);
        printf("   Incoming Errored Packets  : %u\n", counters.incomingErroredPackets);
        printf("   Incoming Unknown Packets  : %u\n", counters.incomingUnknownPackets);
        printf("   Incoming Multicast Packets: %u\n", counters.incomingMulticastPackets);
        printf("   Incoming Broadcast Packets: %u\n", counters.incomingBroadcastPackets);
        printf("   Incoming High Octets      : %u\n", counters.incomingOctets_hi);
        printf("   Incoming Low Octets       : %u\n", counters.incomingOctets_low);
        printf("   Outgoing Unicast Packets  : %u\n", counters.outgoingUnicastPackets);
        printf("   Outgoing Discarded Packets: %u\n", counters.outgoingDiscardedPackets);
        printf("   Outgoing Errored Pakcets  : %u\n", counters.outgoingErroredPackets);
        printf("   Outgoing Unknown Pakcets  : %u\n", counters.outgoingUnknownPackets);
        printf("   Outgoing Multicast Packets: %u\n", counters.outgoingMulticastPackets);
        printf("   Outgoing Broadcast Packets: %u\n", counters.outgoingBroadcastPackets);
        printf("   Outgoing High Octets      : %u\n", counters.outgoingOctets_hi);
        printf("   Outgoing Low Octets       : %u\n", counters.outgoingOctets_low);
        printf("\n=================================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getMocaIntfCountersHandler
 * Description  : Execute bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_MOCA_INTF) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getMocaIntfCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_OMCI_PM_MOCA_INTERFACE_COUNTER counters;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_MOCA_INTF, 0, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n===== Counters For MoCA Interface =====\n\n");
        printf("   Physical TX Broadcast Rate: %u\n", counters.phyTxBroadcastRate);
        printf("   Physical TX Rate          : %u\n", counters.phyTxRate);
        printf("   TX Power Control Reduction: %u\n", counters.txPowerControlReduction);
        printf("   Physical RX Rate          : %u\n", counters.phyRxRate);
        printf("   RX Power Level            : %u\n", counters.rxPowerLevel);
        printf("   Physical RX Broadcast Rate: %u\n", counters.phyRxBroadcastRate);
        printf("   RX Broadcast Power Level  : %u\n", counters.rxBroadcastPowerLevel);
        printf("   TX Packets                : %u\n", counters.txPackets);
        printf("   RX Packets                : %u\n", counters.rxPackets);
        printf("   Errored Missed RX Packets : %u\n", counters.erroredMissedRxPackets);
        printf("   Errored RX Packets        : %u\n", counters.erroredRxPackets);
        printf("   Node MAC address          : %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
           counters.mac[0], counters.mac[1], counters.mac[2],
           counters.mac[3], counters.mac[4], counters.mac[5]);
        printf("\n=======================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getMocaIntfEntryCountersHandler
 * Description  : Execute bcm_omcipm_getCountersNext(BCM_OMCI_PM_CLASS_MOCA_INTF) function.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int getMocaIntfEntryCountersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, portId = 0;
    BCM_OMCI_PM_MOCA_INTERFACE_COUNTER counters;

    ret = omcipmdrvctl_getPortId(pOptions, "MoCA port ID", 16, &portId);

    if (ret != OMCIPMDRVCTL_SUCCESS)
        return ret;

    memset(&counters, 0, sizeof(counters));
    ret = bcm_omcipm_getCountersNext(BCM_OMCI_PM_CLASS_MOCA_INTF,
                                     (UINT16 *)&portId, (void *)&counters);

    if (ret == OMCI_PM_STATUS_SUCCESS)
    {
        printf("\n===== Counters For MoCA Interface Entry #%d =====\n\n", portId);
        printf("   Physical TX Broadcast Rate: %u\n", counters.phyTxBroadcastRate);
        printf("   Physical TX Rate          : %u\n", counters.phyTxRate);
        printf("   TX Power Control Reduction: %u\n", counters.txPowerControlReduction);
        printf("   Physical RX Rate          : %u\n", counters.phyRxRate);
        printf("   RX Power Level            : %u\n", counters.rxPowerLevel);
        printf("   Physical RX Broadcast Rate: %u\n", counters.phyRxBroadcastRate);
        printf("   RX Broadcast Power Level  : %u\n", counters.rxBroadcastPowerLevel);
        printf("   TX Packets                : %u\n", counters.txPackets);
        printf("   RX Packets                : %u\n", counters.rxPackets);
        printf("   Errored Missed RX Packets : %u\n", counters.erroredMissedRxPackets);
        printf("   Errored RX Packets        : %u\n", counters.erroredRxPackets);
        printf("   Node MAC address          : %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
           counters.mac[0], counters.mac[1], counters.mac[2],
           counters.mac[3], counters.mac[4], counters.mac[5]);
        printf("\n=======================================\n\n");
    }

    return ret;
}

