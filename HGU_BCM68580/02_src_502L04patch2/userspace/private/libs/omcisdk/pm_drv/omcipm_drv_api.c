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
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include "bcmtypes.h"
#include "omcipm_api.h"
#include "ethswctl_api.h"
#include "omci_pm.h"

#ifdef SUPPORT_GPONCTL
#include "gponctl_api.h"
#endif

#ifdef SUPPORT_MOCA
//#define __OS_DEFS_H__    // so that osdefs.h is not included in devctl_moca.h
#include "linux/mocalib.h"
#endif

#include "bcm/bcmswapitypes.h"
#include "bcm/bcmswapistat.h"

#define BCM_OMCI_TYPE_STR_SIZE         128
#define OMCIPM_DRV_MOCA_NODE_ID_MAX    16
#define SYSFS_CLASS_NET "/sys/class/net/"
#define SYSFS_PATH_MAX  256


static PM_GET_RTP_STATS_CALLBACK omcipmRtpStatsCbFunc = NULL;
static PM_GET_DNS_STATS_CALLBACK omcipmDnsStatsCbFunc = NULL;

typedef BCM_OMCI_PM_STATUS (*PM_ENET_FUNC)(UINT16 physPortId, void *Counters);

/* get interface statistics based on device name */
static void getIntfStats(const char *devName,
  UINT32 *byteRx, UINT32 *packetRx,
  UINT32 *byteMultiRx, UINT32 *packetMultiRx,
  UINT32 *packetUniRx, UINT32 *packetBcastRx,
  UINT32 *errRx, UINT32 *dropRx,
  UINT32 *byteTx, UINT32 *packetTx,
  UINT32 *byteMultiTx, UINT32 *packetMultiTx,
  UINT32 *packetUniTx, UINT32 *packetBcastTx,
  UINT32 *errTx, UINT32 *dropTx)
{
   int count = 0;
   char *pChar = NULL;
   char line[BUFLEN_512], buf[BUFLEN_512];
   char dummy[BUFLEN_32];
   char rxByte[BUFLEN_32];
   char rxPacket[BUFLEN_32];
   char rxMultiByte[BUFLEN_32];
   char rxPacketMulti[BUFLEN_32];
   char rxPacketUni[BUFLEN_32];
   char rxPacketBcast[BUFLEN_32];
   char rxErr[BUFLEN_32];
   char rxDrop[BUFLEN_32];
   char txByte[BUFLEN_32];
   char txPacket[BUFLEN_32];
   char txMultiByte[BUFLEN_32];
   char txPacketMulti[BUFLEN_32];
   char txPacketUni[BUFLEN_32];
   char txPacketBcast[BUFLEN_32];
   char txErr[BUFLEN_32];
   char txDrop[BUFLEN_32];
   char *pcDevNameStart;

   if (devName == NULL)
   {
      *byteRx = 0;
      *packetRx = 0;
      *byteMultiRx = 0;
      *packetMultiRx = 0;
      *packetUniRx = 0;
      *packetBcastRx = 0;
      *errRx = 0;
      *dropRx = 0;
      *byteTx = 0;
      *packetTx = 0;
      *byteMultiTx = 0;
      *packetMultiTx = 0;
      *packetUniTx = 0;
      *packetBcastTx = 0;
      *errTx = 0;
      *dropTx = 0;
      return;
   }

   /* getstats put device statistics into this file, read the stats */
   /* Be sure to read the page with the extended stats */
   FILE* fs = fopen("/proc/net/dev_extstats", "r");
   if ( fs == NULL )
   {
      return;
   }

   // find interface
   while ( fgets(line, sizeof(line), fs) )
   {
      /* read pass 2 header lines */
      if ( count++ < 2 )
      {
         continue;
      }

      /* normally line will have the following example value
       * "eth0: 19140785 181329 0 0 0 0 0 0 372073009 454948 0 0 0 0 0 0"
       * but when the number is too big then line will have the following example value
       * "eth0:19140785 181329 0 0 0 0 0 0 372073009 454948 0 0 0 0 0 0"
       * so to make the parsing correctly, the following codes are added
       * to insert space between ':' and number
       */
      pChar = strchr(line, ':');
      if ( pChar != NULL )
      {
         pChar++;
      }
      if ( pChar != NULL && isdigit(*pChar) )
      {
         strcpy(buf, pChar);
         *pChar = ' ';
         strcpy(++pChar, buf);
      }

      /* Find and test the interface name to see if it's the one we want.
         If so, then store statistic values.      */
      pcDevNameStart = strstr(line, devName);
      if ( (pcDevNameStart != NULL) && *(pcDevNameStart + strlen(devName)) == ':' )
      {
         sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
           dummy, rxByte, rxPacket, rxErr, rxDrop, dummy, dummy, dummy, rxPacketMulti,
           txByte, txPacket, txErr, txDrop, dummy, dummy, dummy, dummy,
           txPacketMulti, rxMultiByte, txMultiByte, rxPacketUni, txPacketUni,
           rxPacketBcast, txPacketBcast, dummy);
         *byteRx= (UINT32)strtoull(rxByte, NULL, 10);
         *packetRx = (UINT32)strtoull(rxPacket, NULL, 10);
         *byteMultiRx = (UINT32)strtoull(rxMultiByte, NULL, 10);
         *packetMultiRx = (UINT32)strtoull(rxPacketMulti, NULL, 10);
         *packetUniRx = (UINT32)strtoull(rxPacketUni, NULL, 10);
         *packetBcastRx = (UINT32)strtoull(rxPacketBcast, NULL, 10);
         *errRx = (UINT32)strtoull(rxErr, NULL, 10);
         *dropRx = (UINT32)strtoull(rxDrop, NULL, 10);
         *byteTx = (UINT32)strtoull(txByte, NULL, 10);
         *packetTx = (UINT32)strtoull(txPacket, NULL, 10);
         *byteMultiTx = (UINT32)strtoull(txMultiByte, NULL, 10);
         *packetMultiTx = (UINT32)strtoull(txPacketMulti, NULL, 10);
         *packetUniTx = (UINT32)strtoull(txPacketUni, NULL, 10);
         *packetBcastTx = (UINT32)strtoull(txPacketBcast, NULL, 10);
         *errTx = (UINT32)strtoull(txErr, NULL, 10);
         *dropTx = (UINT32)strtoull(txDrop, NULL, 10);

         /* Interface found - break out of while() loop */
         break;
      } /* devName */
   } /* while */

   fclose(fs);
}

static FILE *fpopen(const char *dir, const char *name)
{
    char path[SYSFS_PATH_MAX];

    snprintf(path, SYSFS_PATH_MAX, "%s/%s", dir, name);
    return fopen(path, "r");
}

/* Fetch an integer attribute out of sysfs. */
static int fetch_int(const char *dev, const char *name)
{
    FILE *f = fpopen(dev, name);
    int value = -1;

    if (!f)
    {
        fprintf(stderr, "%s: %s\n", dev, strerror(errno));
    }
    else
    {
        if (fscanf(f, "%d", &value) < 1)
        {
            fprintf(stderr, "%s: %s\n", dev, strerror(errno));
        }
        fclose(f);
    }

    return value;
}

void bcm_omcipm_usrRtpStatsCbRegister(PM_GET_RTP_STATS_CALLBACK cbFuncP)
{
    omcipmRtpStatsCbFunc = cbFuncP;
}

void bcm_omcipm_usrDnsStatsCbRegister(PM_GET_DNS_STATS_CALLBACK cbFuncP)
{
    omcipmDnsStatsCbFunc = cbFuncP;
}

#ifdef DMP_X_ITU_ORG_VOICE_1
static BCM_OMCI_PM_STATUS omcipm_getCountersRTP(UINT16 physPortId, void *pCounters)
{
    BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;

    if (omcipmRtpStatsCbFunc != NULL)
    {
        sts = omcipmRtpStatsCbFunc(physPortId, pCounters);
    }

    return sts;
}
#endif /* DMP_X_ITU_ORG_VOICE_1 */

#ifdef SUPPORT_GPONCTL
static BCM_OMCI_PM_STATUS omcipm_getCountersGemPort(UINT16 physPortId, void *counters)
{
    int ret;
    BCM_Ploam_GemPortCounters gemPort;
    BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;
    PBCM_OMCI_PM_GEM_PORT_COUNTER pCounters = (PBCM_OMCI_PM_GEM_PORT_COUNTER)counters;

    memset(&gemPort, 0, sizeof(BCM_Ploam_GemPortCounters));

    gemPort.reset = 0;
    gemPort.gemPortIndex = physPortId;
    gemPort.gemPortID = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;

    ret = gponCtl_getGemPortCounters(&gemPort);
    if (ret == 0)
    {
        // Deliver GEM port stats to calling function.
        pCounters->transmittedGEMFrames = gemPort.txFrames;
        pCounters->receivedGEMFrames = gemPort.rxFrames;
        pCounters->receivedPayloadBytes = gemPort.rxBytes;
        pCounters->transmittedPayloadBytes = gemPort.txBytes;
    }
    else
    {
        OMCIPM_LOG_ERROR("%s", strerror(errno));
        sts = OMCI_PM_STATUS_ERROR;
    }

    return sts;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersFec(void *counters)
{
    int ret = 0;
    BCM_Ploam_fecCounters fec_counter;
    BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;
    PBCM_OMCI_PM_FEC_COUNTER pCounters = (PBCM_OMCI_PM_FEC_COUNTER)counters;

    memset(&fec_counter, 0, sizeof(BCM_Ploam_fecCounters));

    ret = gponCtl_getFecCounters(&fec_counter);
    if (ret)
    {
        OMCIPM_LOG_ERROR("%s", strerror(errno));
        sts = OMCI_PM_STATUS_ERROR;
        goto out;
    }

    /*Compliant*/
    pCounters->correctedBytes = fec_counter.fecByte;
    /*Compliant*/
    pCounters->correctedCodeWords = fec_counter.fecCerr;
    /*Compliant*/
    pCounters->uncorrectedCodeWords = fec_counter.fecUerr;
    /*Compliant*/
    pCounters->totalCodeWords = fec_counter.fecCWs;
    /*Not Compliant. Always returns 0*/
    pCounters->fecSeconds = fec_counter.fecSecs;

out:
    return sts;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersGalEnet(UINT16 physPortId, void *counters)
{
    int ret = 0;
    BCM_Ploam_GemPortCounters gemPort;
    BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;
    PBCM_OMCI_PM_GAL_ETHERNET_COUNTER pCounters = (PBCM_OMCI_PM_GAL_ETHERNET_COUNTER)counters;

    memset(&gemPort, 0, sizeof(BCM_Ploam_GemPortCounters));

    gemPort.reset = 0;
    gemPort.gemPortIndex = physPortId;
    gemPort.gemPortID = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;

    ret = gponCtl_getGemPortCounters(&gemPort);
    if (ret)
    {
        OMCIPM_LOG_ERROR("%s", strerror(errno));
        sts = OMCI_PM_STATUS_ERROR;
        goto out;
    }

    /*Partially Compliant: not count erroneous FCS*/
    pCounters->discardedFrames = gemPort.rxDroppedFrames;
    /*Compliant - Non-standard Counter*/
    pCounters->transmittedFrames = gemPort.txFrames;
    /*Compliant - Non-standard Counter*/
    pCounters->receivedFrames = gemPort.rxFrames;

out:
    return sts;
}
#endif   // SUPPORT_GPONCTL

static BCM_OMCI_PM_STATUS omcipm_getCountersEnet_lan(UINT16 physPortId,
  PBCM_OMCI_PM_ETHERNET_COUNTER pCounters)
{
#ifndef BRCM_FTTDP
    bcm_error_t ret;
    struct emac_stats counter;
    struct rdpa_port_stats port_stat;
#endif

#ifndef BRCM_FTTDP
    ret = bcm_stat_get_emac(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &counter);
    if (ret != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting EMAC statistic returns error: %d", ret);
        return OMCI_PM_STATUS_ERROR;
    }

    ret = bcm_stat_get_port(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &port_stat);
    if (ret != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting Port statistic returns error: %d", ret);
        return OMCI_PM_STATUS_ERROR;
    }

    /* FCS errors: counts frames received that failed the frame check sequence (FCS). (4 bytes) */
    pCounters->fcsErrors = counter.rx_fcs_error;
    /* Excessive collision counter: counts frames whose transmission
    failed due to excessive collisions. (4 bytes) */
    pCounters->excessiveCollisionCounter = counter.tx_excessive_collision;
    /* Late collision counter: counts the number of times that a collision was
    detected later than 512 bit times into the transmission of a packet. (4 bytes) */
    pCounters->lateCollisionCounter = counter.tx_late_collision;
    /* Frames too long: counts received frames that exceeded the maximum
    permitted frame size. (4 bytes) */
    pCounters->frameTooLongs = counter.rx_oversize_packet;
    /* Buffer overflows on receive: counts the number of times that the receive
    buffer overflowed. (4 bytes) */
    pCounters->bufferOverflowsOnReceive = counter.rx_overflow;
    /* Buffer overflows on transmit: counts the number of times that the transmit
    buffer overflowed. (4 bytes) */
    pCounters->bufferOverflowsOnTransmit = port_stat.tx_discard;
    /* Single collision frame counter: counts successfully transmitted frames whose
    transmission was delayed by exactly one collision. (4 bytes) */
    pCounters->singleCollisionFrameCounter = counter.tx_single_collision;
    /* Multiple collisions frame counter: counts successfully transmitted frames
    whose transmission was delayed by more than one collision. (4 bytes) */
    pCounters->multipleCollisionsFrameCounter = counter.tx_multiple_collision;
    /* SQE counter: counts the number of times that the SQE test error message
    was generated. (4 bytes) */
    pCounters->sqeCounter = counter.rx_fcs_error;
    /* Deferred transmission counter: counts frames whose first transmission
    attempt was delayed because the medium was busy. (4 bytes) */
    pCounters->deferredTransmissionCounter = counter.tx_deferral_packet;
    /* Internal MAC transmit error counter */
    pCounters->internalMacTransmitErrorCounter = counter.tx_error;
    /* Carrier sense error counter: counts the number of times that carrier sense
    was lost or never asserted when attempting to transmit a frame. (4 bytes)*/
    pCounters->carrierSenseErrorCounter = counter.rx_carrier_sense_error;
    /* Alignment error counter: counts received frames that were not an integral
    number of octets in length and did not pass the FCS check. (4 bytes) */
    pCounters->alignmentErrorCounter = counter.rx_alignment_error;
    /* Internal MAC receive error counter: Not supported */
    pCounters->internalMacReceiveErrorCounter = 0;
#endif

    return OMCI_PM_STATUS_SUCCESS;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnet_veip(UINT16 physPortId
  __attribute__((unused)), PBCM_OMCI_PM_ETHERNET_COUNTER pCounters)
{
    DIR *dir;
    char path[SYSFS_PATH_MAX];

    snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/statistics",
      GPON_WAN_IF_NAME);

    dir = opendir(path);
    if (dir == NULL)
    {
        OMCIPM_LOG_ERROR("path '%s' is not a directory\n", path);
        return OMCI_PM_STATUS_ERROR;
    }

    pCounters->excessiveCollisionCounter = fetch_int(path, "collisions");
    pCounters->lateCollisionCounter = fetch_int(path, "tx_window_errors");
    pCounters->bufferOverflowsOnReceive = fetch_int(path, "tx_dropped");
    pCounters->bufferOverflowsOnTransmit = fetch_int(path, "rx_dropped") +
      fetch_int(path, "rx_missed_errors");
    pCounters->sqeCounter = fetch_int(path, "tx_heartbeat_errors");
    pCounters->internalMacTransmitErrorCounter = fetch_int(path, "tx_errors");
    (void)closedir(dir);

    return OMCI_PM_STATUS_SUCCESS;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnet(UINT16 physPortId,
  PBCM_OMCI_PM_ETHERNET_COUNTER pCounters)
{
    PM_ENET_FUNC func;

    memset(pCounters, 0, sizeof(BCM_OMCI_PM_ETHERNET_COUNTER));

    if (physPortId == PORT_VEIP)
        func = (PM_ENET_FUNC)omcipm_getCountersEnet_veip;
    else
        func = (PM_ENET_FUNC)omcipm_getCountersEnet_lan;

    return func(physPortId, pCounters);
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnet2_lan(UINT16 physPortId __attribute__((unused)),
  PBCM_OMCI_PM_ETHERNET_2_COUNTER pCounters)
{
    BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;

    /*Not Compliant - Always returns 0 since ONU is in bridge mode only*/
    pCounters->pppoeFilterFrameCounter = 0;

    return sts;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnet2_veip(UINT16 physPortId __attribute__((unused)),
  PBCM_OMCI_PM_ETHERNET_2_COUNTER pCounters)
{
    BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;

    /*Not Compliant - Always returns 0 since ONU is in bridge mode only*/
    pCounters->pppoeFilterFrameCounter = 0;

    return sts;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnet2(UINT16 physPortId,
  PBCM_OMCI_PM_ETHERNET_2_COUNTER pCounters)
{
    PM_ENET_FUNC func;

    memset(pCounters, 0, sizeof(BCM_OMCI_PM_ETHERNET_2_COUNTER));

    if (physPortId == PORT_VEIP)
        func = (PM_ENET_FUNC)omcipm_getCountersEnet2_veip;
    else
        func = (PM_ENET_FUNC)omcipm_getCountersEnet2_lan;

    return func(physPortId, pCounters);
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnet3_lan(UINT16 physPortId,
  PBCM_OMCI_PM_ETHERNET_3_COUNTER pCounters)
{
    bcm_error_t ret;
#ifndef BRCM_FTTDP
    struct emac_stats emac_stat;
#else
    struct rdpa_port_stats port_stat;
#endif

#ifndef BRCM_FTTDP
    ret = bcm_stat_get_emac(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &emac_stat);
    if (ret != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting EMAC statistic returns error: %d", ret);
        return OMCI_PM_STATUS_ERROR;
    }

    /* Drop events: The total number of events in which packets were dropped due to lack of
    resources. (4 bytes)*/
    pCounters->dropEvents = emac_stat.rx_alignment_error + emac_stat.rx_frame_length_error +
        emac_stat.rx_code_error + emac_stat.rx_carrier_sense_error +  emac_stat.rx_fcs_error +
        emac_stat.tx_error;
    /* Octets: The total number of octets received from the CPE (4 bytes) */
    pCounters->octets = emac_stat.rx_byte;
    /* Packets: The total number of packets received (4 bytes) */
    pCounters->packets = emac_stat.rx_packet;
    /* Broadcast packets: The total number of received good packets directed to the
    broadcast address. (4 bytes)*/
    pCounters->broadcastPackets = emac_stat.rx_broadcast_packet;
    /* Multicast packets: The total number of received good packets directed to
    a multicast address. (4 bytes)*/
    pCounters->multicastPackets = emac_stat.rx_multicast_packet;
    /* Undersize packets: The total number of packets received that were less than 64 octets long
    but were otherwise well formed (4 bytes)*/
    pCounters->undersizePackets = emac_stat.rx_undersize_packet;
    /* Fragments: The total number of packets received that were less than 64 octets long,
    and had either a bad frame check sequence (FCS) with an integral number of octets (FCS error)
    or a bad FCS with a non-integral number of octets (alignment error). (4 bytes) */
    pCounters->fragments = emac_stat.rx_fragments;
    /* Jabbers: The total number of packets received that were longer than 1518 octets,
    and had either a bad frame check sequence (FCS) with an integral number of octets (FCS error)
    or a bad FCS with a non-integral number of octets (alignment error). (4 bytes) */
    pCounters->jabbers = emac_stat.rx_jabber;
    /* Rx Packets 64 octets */
    pCounters->packets64Octets = emac_stat.rx_frame_64;
    /* Rx Packets 65 to 127 octets */
    pCounters->packets127Octets = emac_stat.rx_frame_65_127;
    /* Rx Packets 128 to 255 octets */
    pCounters->packets255Octets = emac_stat.rx_frame_128_255;
    /* Rx Packets 256 to 511 octets */
    pCounters->packets511Octets = emac_stat.rx_frame_256_511;
    /* Rx Packets 512 to 1023 octets */
    pCounters->packets1023Octets = emac_stat.rx_frame_512_1023;
    /* Rx Packets 1024 to 1518 octets */
    pCounters->packets1518Octets = emac_stat.rx_frame_1024_1518;
#else
    ret = bcm_stat_get_port(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &port_stat);
    if (ret != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting Port statistic returns error: %d", ret);
        return OMCI_PM_STATUS_ERROR;
    }

    /* Octets: The total number of octets received from the CPE (4 bytes) */
    pCounters->octets = port_stat.rx_valid_bytes;
    /* Packets: The total number of packets received (4 bytes) */
    pCounters->packets = port_stat.rx_valid_pkt;
    /* Broadcast packets: The total number of received good packets directed to the
    broadcast address. (4 bytes)*/
    pCounters->broadcastPackets = port_stat.rx_broadcast_pkt;
    /* Multicast packets: The total number of received good packets directed to
    a multicast address. (4 bytes)*/
    pCounters->multicastPackets = port_stat.rx_multicast_pkt;
#endif

    return OMCI_PM_STATUS_SUCCESS;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnet3_veip(UINT16 physPortId __attribute__((unused)),
  PBCM_OMCI_PM_ETHERNET_3_COUNTER pCounters)
{
    UINT32 rxByte = 0, rxPacket = 0, rxErr = 0, rxDrop = 0;
    UINT32 txByte = 0, txPacket = 0, txErr = 0, txDrop = 0;
    UINT32 byteMultiRx = 0, packetMultiRx = 0, packetUniRx = 0, packetBcastRx = 0;
    UINT32 byteMultiTx = 0, packetMultiTx = 0, packetUniTx = 0, packetBcastTx = 0;

    getIntfStats(GPON_WAN_IF_NAME,
        &rxByte, &rxPacket, &byteMultiRx, &packetMultiRx, &packetUniRx, &packetBcastRx, &rxErr, &rxDrop,
        &txByte, &txPacket, &byteMultiTx, &packetMultiTx, &packetUniTx, &packetBcastTx, &txErr, &txDrop);

    pCounters->dropEvents = txDrop;
    pCounters->octets = txByte;
    pCounters->packets = txPacket;
    pCounters->broadcastPackets = packetBcastTx;
    pCounters->multicastPackets = packetMultiTx;

    return OMCI_PM_STATUS_SUCCESS;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnet3(UINT16 physPortId __attribute__((unused)),
  PBCM_OMCI_PM_ETHERNET_2_COUNTER pCounters)
{
    PM_ENET_FUNC func;

    memset(pCounters, 0, sizeof(BCM_OMCI_PM_ETHERNET_3_COUNTER));

    if (physPortId == PORT_VEIP)
        func = (PM_ENET_FUNC)omcipm_getCountersEnet3_veip;
    else
        func = (PM_ENET_FUNC)omcipm_getCountersEnet3_lan;

    return func(physPortId, pCounters);
}

#ifdef OMCIPM_TEST
static struct emac_stats emac_stat0;

void emac_stat_inc(void)
{
    emac_stat0.tx_error += 2;
    emac_stat0.tx_byte += 2;
    emac_stat0.tx_packet += 2;
    emac_stat0.tx_broadcast_packet += 2;
    emac_stat0.tx_multicast_packet += 2;
    emac_stat0.tx_fcs_error += 2;
    emac_stat0.tx_undersize_frame += 2;
    emac_stat0.tx_oversize_frame += 2;
    emac_stat0.tx_frame_64 += 2;
    emac_stat0.tx_frame_65_127 += 2;
    emac_stat0.tx_frame_128_255 += 2;
    emac_stat0.tx_frame_256_511 += 2;
    emac_stat0.tx_frame_512_1023 += 2;
    emac_stat0.tx_frame_1024_1518 += 2;
    emac_stat0.rx_alignment_error += 3;
    emac_stat0.rx_byte += 3;
    emac_stat0.rx_packet += 3;
    emac_stat0.rx_frame_64 += 3;
    emac_stat0.rx_frame_65_127 += 3;
    emac_stat0.rx_frame_128_255 += 3;
}
#endif /* OMCIPM_TEST */

static BCM_OMCI_PM_STATUS getCountersEnetDn_veip(UINT16 physPortId __attribute__((unused)),
  PBCM_OMCI_PM_ETHERNET_UPDN_COUNTER pCounters)
{
    DIR *dir;
    char path[SYSFS_PATH_MAX];

    UINT32 rxByte = 0, rxPacket = 0, rxErr = 0, rxDrop = 0;
    UINT32 txByte = 0, txPacket = 0, txErr = 0, txDrop = 0;
    UINT32 byteMultiRx = 0, packetMultiRx = 0, packetUniRx = 0, packetBcastRx = 0;
    UINT32 byteMultiTx = 0, packetMultiTx = 0, packetUniTx = 0, packetBcastTx = 0;

    snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/statistics", GPON_WAN_IF_NAME);

    dir = opendir(path);
    if (dir == NULL)
    {
        OMCIPM_LOG_ERROR("path '%s' is not a directory\n", path);
        return OMCI_PM_STATUS_ERROR;
    }

    getIntfStats(GPON_WAN_IF_NAME,
        &rxByte, &rxPacket, &byteMultiRx, &packetMultiRx,
        &packetUniRx, &packetBcastRx, &rxErr, &rxDrop,
        &txByte, &txPacket, &byteMultiTx, &packetMultiTx,
        &packetUniTx, &packetBcastTx, &txErr, &txDrop);

    pCounters->dropEvents = rxDrop;
    pCounters->octets = rxByte;
    pCounters->packets = rxPacket;
    pCounters->broadcastPackets = packetBcastRx;
    pCounters->multicastPackets = packetMultiRx;

    pCounters->crcErroredPackets = fetch_int(path, "rx_crc_errors") +
      fetch_int(path, "rx_frame_errors");
    pCounters->oversizePackets = fetch_int(path, "rx_over_errors");
    pCounters->undersizePackets = fetch_int(path, "rx_length_errors") -
      pCounters->oversizePackets;
    (void)closedir(dir);

    return OMCI_PM_STATUS_SUCCESS;
}

static BCM_OMCI_PM_STATUS getCountersEnetDn_lan(UINT16 physPortId,
  PBCM_OMCI_PM_ETHERNET_UPDN_COUNTER pCounters)
{
    bcm_error_t rc;
    struct rdpa_port_stats port_stat;
#ifndef BRCM_FTTDP
    struct emac_stats emac_stat;
#endif

    rc = bcm_stat_get_port(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &port_stat);
    if (rc != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting Port statistic returns error: %d", rc);
        return OMCI_PM_STATUS_ERROR;
    }

#ifndef BRCM_FTTDP
    rc = bcm_stat_get_emac(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &emac_stat);
#ifdef OMCIPM_TEST
    emac_stat_inc();
    memcpy(&emac_stat, &emac_stat0, sizeof(emac_stat));
#endif /* OMCIPM_TEST */
    if (rc != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting EMAC statistic returns error: %d", rc);
        return OMCI_PM_STATUS_ERROR;
    }

    /* Drop events: The total number of events in which frames were dropped due to lack of
    resources. (4 bytes)*/
    pCounters->dropEvents += emac_stat.tx_error + port_stat.tx_discard;
    /* Octets: The total number of octets transmit (4 bytes) */
    pCounters->octets += emac_stat.tx_byte;
    /* Frames: The total number of frames transmit, including bad frames, broadcast frames
    and multicast frames. (4 bytes)*/
    pCounters->packets += emac_stat.tx_packet;
    /* Broadcast frames: The total number of transmit good frames directed to the broadcast
    address. (4 bytes) */
    pCounters->broadcastPackets += emac_stat.tx_broadcast_packet;
    /* Multicast frames: The total number of transmit good frames directed to a multicast
    address. (4 bytes) */
    pCounters->multicastPackets += emac_stat.tx_multicast_packet;
    /* CRC errored frames: The total number of frames transmit that had a length of
    between 64 and 1518 octets, but had either a bad frame check sequence (FCS)
    with an integral number of octets (FCS error) or a bad FCS with a non-integral
    number of octets (alignment error). (4 bytes) */
    pCounters->crcErroredPackets += emac_stat.tx_fcs_error;
    /* Undersize frames: The total number of frames transmit that were less than 64 octets long
    but were otherwise well formed (4 bytes) */
    pCounters->undersizePackets += emac_stat.tx_undersize_frame;
    /* Oversize frames: The total number of frames transmit that were longer than 1518 octets
    and were otherwise well formed. (4 bytes) */
    pCounters->oversizePackets += emac_stat.tx_oversize_frame;
    /* Tx Packets 64 octets */
    pCounters->packets64Octets += emac_stat.tx_frame_64;
    /* Tx Packets 65 to 127 octets */
    pCounters->packets127Octets += emac_stat.tx_frame_65_127;
    /* Tx Packets 128 to 255 octets */
    pCounters->packets255Octets += emac_stat.tx_frame_128_255;
    /* Tx Packets 256 to 511 octets */
    pCounters->packets511Octets += emac_stat.tx_frame_256_511;
    /* Tx Packets 512 to 1023 octets */
    pCounters->packets1023Octets += emac_stat.tx_frame_512_1023;
    /* Tx Packets 1024 to 1518 octets */
    pCounters->packets1518Octets += emac_stat.tx_frame_1024_1518;
#else
    /* Octets: The total number of octets transmit (4 bytes) */
    pCounters->octets += port_stat.tx_valid_bytes;
    /* Frames: The total number of frames transmit, including bad frames, broadcast frames
    and multicast frames. (4 bytes)*/
    pCounters->packets += port_stat.tx_valid_pkt;
    /* Broadcast frames: The total number of transmit good frames directed to the broadcast
    address. (4 bytes) */
    pCounters->broadcastPackets += port_stat.tx_broadcast_pkt;
    /* Multicast frames: The total number of transmit good frames directed to a multicast
    address. (4 bytes) */
    pCounters->multicastPackets += port_stat.tx_multicast_pkt;
#endif

    return OMCI_PM_STATUS_SUCCESS;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnetDn(UINT16 physPortId,
  PBCM_OMCI_PM_ETHERNET_UPDN_COUNTER pCounters)
{
    PM_ENET_FUNC func;

    memset(pCounters, 0, sizeof(BCM_OMCI_PM_ETHERNET_UPDN_COUNTER));

    if (physPortId == PORT_VEIP)
        func = (PM_ENET_FUNC)getCountersEnetDn_veip;
    else
        func = (PM_ENET_FUNC)getCountersEnetDn_lan;

    return func(physPortId, pCounters);
}

static BCM_OMCI_PM_STATUS getCountersEnetUp_veip(UINT16 physPortId __attribute__((unused)),
  PBCM_OMCI_PM_ETHERNET_UPDN_COUNTER pCounters)
{
    UINT32 rxByte = 0, rxPacket = 0, rxErr = 0, rxDrop = 0;
    UINT32 txByte = 0, txPacket = 0, txErr = 0, txDrop = 0;
    UINT32 byteMultiRx = 0, packetMultiRx = 0, packetUniRx = 0, packetBcastRx = 0;
    UINT32 byteMultiTx = 0, packetMultiTx = 0, packetUniTx = 0, packetBcastTx = 0;

    getIntfStats(GPON_WAN_IF_NAME,
      &rxByte, &rxPacket, &byteMultiRx, &packetMultiRx,
      &packetUniRx, &packetBcastRx, &rxErr, &rxDrop,
      &txByte, &txPacket, &byteMultiTx, &packetMultiTx,
      &packetUniTx, &packetBcastTx, &txErr, &txDrop);

    pCounters->dropEvents = txDrop;
    pCounters->octets = txByte;
    pCounters->packets = txPacket;
    pCounters->broadcastPackets = packetBcastTx;
    pCounters->multicastPackets = packetMultiTx;

    return OMCI_PM_STATUS_SUCCESS;
}

static BCM_OMCI_PM_STATUS getCountersEnetUp_lan(UINT16 physPortId,
  PBCM_OMCI_PM_ETHERNET_UPDN_COUNTER pCounters)
{
    bcm_error_t ret;
#ifndef BRCM_FTTDP
    struct emac_stats emac_stat;

    ret = bcm_stat_get_emac(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &emac_stat);
#ifdef OMCIPM_TEST
    emac_stat_inc();
    memcpy(&emac_stat, &emac_stat0, sizeof(emac_stat));
#endif /* OMCIPM_TEST */
    if (ret != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting EMAC statistic returns error: %d", ret);
        return OMCI_PM_STATUS_ERROR;
    }

    /* Drop events: The total number of events in which frames were dropped due to lack of
    resources. (4 bytes) */
    pCounters->dropEvents += emac_stat.rx_alignment_error + emac_stat.rx_frame_length_error
    + emac_stat.rx_code_error + emac_stat.rx_carrier_sense_error + emac_stat.rx_fcs_error;
    /* Octets: The total number of octets receive (4 bytes) */
    pCounters->octets += emac_stat.rx_byte;
    /* Frames: The total number of frames receive, including bad frames, broadcast frames
    and multicast frames. (4 bytes) */
    pCounters->packets += emac_stat.rx_packet;
    /* Broadcast frames: The total number of receive good frames directed to the broadcast
    address. (4 bytes) */
    pCounters->broadcastPackets += emac_stat.rx_broadcast_packet;
    /* Multicast frames: The total number of receive good frames directed to a multicast
    address. (4 bytes) */
    pCounters->multicastPackets += emac_stat.rx_multicast_packet;
    /* CRC errored frames: The total number of frames receive that had a length of
    between 64 and 1518 octets, but had either a bad frame check sequence (FCS)
    with an integral number of octets (FCS error) or a bad FCS with a non-integral
    number of octets (alignment error). (4 bytes) */
    pCounters->crcErroredPackets += emac_stat.rx_fcs_error;
    /* Undersize frames: The total number of frames receive that were less than 64 octets long
    but were otherwise well formed (4 bytes) */
    pCounters->undersizePackets += emac_stat.rx_undersize_packet;
    /* Oversize frames: The total number of frames receive that were longer than 1518 octets
    and were otherwise well formed. (4 bytes) */
    pCounters->oversizePackets += emac_stat.rx_oversize_packet;
    /* Rx Packets 64 octets */
    pCounters->packets64Octets += emac_stat.rx_frame_64;
    /* Rx Packets 65 to 127 octets */
    pCounters->packets127Octets += emac_stat.rx_frame_65_127;
    /* Rx Packets 128 to 255 octets */
    pCounters->packets255Octets += emac_stat.rx_frame_128_255;
    /* Rx Packets 256 to 511 octets */
    pCounters->packets511Octets += emac_stat.rx_frame_256_511;
    /* Rx Packets 512 to 1023 octets */
    pCounters->packets1023Octets += emac_stat.rx_frame_512_1023;
    /* Rx Packets 1024 to 1518 octets */
    pCounters->packets1518Octets += emac_stat.rx_frame_1024_1518;
#else
    struct rdpa_port_stats port_stat;

    ret = bcm_stat_get_port(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &port_stat);
    if (ret != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting Port statistic returns error: %d", ret);
        return OMCI_PM_STATUS_ERROR;
    }

    /* Octets: The total number of octets receive (4 bytes) */
    pCounters->octets += port_stat.rx_valid_bytes;
    /* Frames: The total number of frames receive, including bad frames, broadcast frames
    and multicast frames. (4 bytes) */
    pCounters->packets += port_stat.rx_valid_pkt;
    /* Broadcast frames: The total number of receive good frames directed to the broadcast
    address. (4 bytes) */
    pCounters->broadcastPackets += port_stat.rx_broadcast_pkt;
    /* Multicast frames: The total number of receive good frames directed to a multicast
    address. (4 bytes) */
    pCounters->multicastPackets += port_stat.rx_multicast_pkt;
#endif

    return OMCI_PM_STATUS_SUCCESS;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersEnetUp(UINT16 physPortId, void *pCounters)
{
    PM_ENET_FUNC func;

    memset(pCounters, 0, sizeof(BCM_OMCI_PM_ETHERNET_UPDN_COUNTER));

    if (physPortId == PORT_VEIP)
        func = (PM_ENET_FUNC)getCountersEnetUp_veip;
    else
        func = (PM_ENET_FUNC)getCountersEnetUp_lan;

    return func(physPortId, pCounters);
}

static BCM_OMCI_PM_STATUS omcipm_getCountersBridge(UINT16 bridgeId, void *counters)
{
    PBCM_OMCI_PM_MAC_BRIDGE_COUNTER pCounters = (PBCM_OMCI_PM_MAC_BRIDGE_COUNTER)counters;
    char name[32];
    DIR *dir;
    char path[SYSFS_PATH_MAX];

    memset(pCounters, 0, sizeof(BCM_OMCI_PM_MAC_BRIDGE_COUNTER));

    snprintf(name, sizeof(name), "bronu%d", bridgeId);
    snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/bridge", name);

    dir = opendir(path);
    if (dir == NULL)
    {
        OMCIPM_LOG_ERROR("path '%s' is not a directory\n", path);
        return OMCI_PM_STATUS_ERROR;
    }

    pCounters->learningDiscaredEntries = fetch_int(path, "mac_entry_discard_counter");
    (void)closedir(dir);

    return OMCI_PM_STATUS_SUCCESS;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersBridgePort(UINT16 physPortId, void *counters)
{
    bcm_error_t ret;
    struct emac_stats emac_stat;
    struct rdpa_port_stats port_stat;
    PBCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER pCounters = (PBCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER)counters;

    memset(pCounters, 0, sizeof(BCM_OMCI_PM_MAC_BRIDGE_PORT_COUNTER));

    ret = bcm_stat_get_emac(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &emac_stat);
    if (ret != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting EMAC statistic returns error: %d", ret);
        return OMCI_PM_STATUS_ERROR;
    }

    ret = bcm_stat_get_port(0, bcm_enet_map_oam_idx_to_phys_port(physPortId), &port_stat);
    if (ret != BCM_E_NONE)
    {
        OMCIPM_LOG_ERROR("Getting Port statistic returns error: %d", ret);
        return OMCI_PM_STATUS_ERROR;
    }

    /* Forwarded frame counter: counts frames transmitted successfully on this port. (4 bytes) */
    pCounters->forwardedFrames = emac_stat.tx_packet;
    /* Delay exceeded discard counter: counts frames discarded on this port because
    transmission was delayed. (4 bytes) */
    pCounters->delayDiscardedFrames = emac_stat.tx_error;
    /* MTU exceeded discard counter: counts frames discarded on this port because
    the MTU was exceeded. (4 bytes) */
    pCounters->mtuDiscardedFrames = port_stat.rx_discard_max_length;
    /* Received frame counter: counts frames received on this port. (4 bytes) */
    pCounters->receivedFrames = emac_stat.rx_packet;
    /* Received and discarded counter: counts frames received on this port that were
    discarded due to errors. (4 bytes) */
    pCounters->receivedDiscardedFrames = emac_stat.rx_alignment_error +
     emac_stat.rx_frame_length_error + emac_stat.rx_code_error +
     emac_stat.rx_carrier_sense_error + emac_stat.rx_fcs_error;

    return OMCI_PM_STATUS_SUCCESS;
}

#ifdef SUPPORT_MOCA
static BCM_OMCI_PM_STATUS omcipm_getCountersMocaEnet(UINT16 physPortId, void *counters)
{
    struct moca_gen_stats moca;
    struct moca_ext_octet_count moca_octets;
    struct moca_node_stats_in    node_stats_in;
    struct moca_node_stats       node_stats;
    struct moca_network_status   moca_net;
    void * pMoca;
    BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;
    PBCM_OMCI_PM_MOCA_ETHERNET_COUNTER pCounters = (PBCM_OMCI_PM_MOCA_ETHERNET_COUNTER)counters;
    static UINT8 ErrRptd=0;
    UINT32 i;

    pMoca = moca_open(NULL);

    if (pMoca != NULL)
    {
        int ret = moca_get_gen_stats(pMoca, 0, &moca);

        if (ret == MOCA_API_SUCCESS)
            ret = moca_get_ext_octet_count(pMoca, &moca_octets);

        if (ret == MOCA_API_SUCCESS)
            ret = moca_get_network_status(pMoca, &moca_net);

        switch (ret)
        {
            /*No error - just break out*/
            case MOCA_API_SUCCESS:
                ErrRptd = 0;
                break;
            /*Error that do not need to display error message*/
            case MOCA_API_CANT_CONNECT:
                sts = OMCI_PM_STATUS_STATE_ERROR;
                goto out;
            /*Error that do not need to display error message*/
            case MOCA_API_TIMEOUT:
                sts = OMCI_PM_STATUS_TIMEOUT;
                goto out;
            /*Errors that need to display error messages*/
            default:
                // avoid flooding msgs by printing the error msg only if it hasn't been reported
                if (0==ErrRptd)
                {
                    OMCIPM_LOG_ERROR("Getting MoCA statistics returns error: %d", ret);
                    ErrRptd=1;
                }
                sts = OMCI_PM_STATUS_ERROR;
                goto out;
        }

        pCounters->incomingUnicastPackets = moca.ecl_tx_ucast_pkts;
        pCounters->incomingDiscardedPackets = moca.ecl_tx_mcast_drops +
                                              moca.ecl_tx_ucast_drops;

        pCounters->incomingErroredPackets = 0;

        for (i = 0; i < OMCIPM_DRV_MOCA_NODE_ID_MAX; i++)
        {
            if (moca_net.connected_nodes & (1 << i))
            {
               memset(&node_stats, 0, sizeof(node_stats));
               node_stats_in.reset_stats = 0;
               node_stats_in.index = i;
               ret = moca_get_node_stats(pMoca, &node_stats_in, &node_stats);
               if (ret == MOCA_API_SUCCESS)
                  pCounters->incomingErroredPackets += node_stats.primary_ch_rx_cw_corrected +
                    node_stats.primary_ch_rx_cw_uncorrected +
                    node_stats.secondary_ch_rx_cw_corrected +
                    node_stats.secondary_ch_rx_cw_uncorrected;
            }
        }

        pCounters->incomingUnknownPackets = moca.ecl_tx_ucast_unknown + moca.ecl_tx_mcast_unknown;
        pCounters->incomingMulticastPackets = moca.ecl_tx_mcast_pkts;
        pCounters->incomingBroadcastPackets = moca.ecl_tx_bcast_pkts;
        pCounters->incomingOctets_hi = moca_octets.in_octets_hi;
        pCounters->incomingOctets_low = moca_octets.in_octets_lo;
        pCounters->outgoingUnicastPackets = moca.ecl_rx_ucast_pkts;
        pCounters->outgoingDiscardedPackets = moca.ecl_rx_mcast_filter_pkts +
          moca.ecl_rx_ucast_drops;
        /*Not Compliant - Always returns 0*/
        pCounters->outgoingErroredPackets = 0;
        /*Not Compliant - Always returns 0*/
        pCounters->outgoingUnknownPackets = 0;
        /*Not Compliant - Always returns 0*/
        pCounters->outgoingMulticastPackets = 0; //moca.ecl_rx_mcast_pkts;
        pCounters->outgoingBroadcastPackets = moca.ecl_rx_bcast_pkts;
        pCounters->outgoingOctets_hi = moca_octets.out_octets_hi;
        pCounters->outgoingOctets_low = moca_octets.out_octets_lo;
    }
    else
    {
        sts = OMCI_PM_STATUS_STATE_ERROR;
    }

out:
    if (pMoca != NULL)
    {
        moca_close(pMoca);
    }
    return sts;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersMocaIntf(void *counters)
{
    int ret = MOCA_API_SUCCESS;
    BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;
    UINT32   i = 0, j = 0;
    PBCM_OMCI_PM_MOCA_INTERFACE_COUNTER pCounters = (PBCM_OMCI_PM_MOCA_INTERFACE_COUNTER)counters;
    void * pMoca;
    struct moca_interface_status          moca_if;
    struct moca_network_status            moca_net;
    struct moca_gen_stats                 stats;
    struct moca_gen_node_status           moca_node;
    struct moca_node_stats                node_stats;
    struct moca_node_stats_in             node_stats_in;
    struct moca_gen_node_ext_status_in    moca_ext_in;
    struct moca_gen_node_ext_status       moca_ext;

    memset(&moca_if, 0, sizeof(moca_if));
    memset(&moca_net, 0, sizeof(moca_net));
    memset(&stats, 0, sizeof(stats));
    memset(&moca_node, 0, sizeof(moca_node));
    memset(&node_stats, 0, sizeof(node_stats));
    memset(&moca_ext, 0, sizeof(moca_ext));

    pMoca = moca_open(NULL);

    if (pMoca == NULL)
    {
        sts = OMCI_PM_STATUS_STATE_ERROR;
        goto out;
    }

    ret = moca_get_interface_status(pMoca, &moca_if);
    switch (ret)
    {
        /*No error - just break out*/
        case MOCA_API_SUCCESS:
            break;
        /*Error that do not need to display error message*/
        case MOCA_API_CANT_CONNECT:
            sts = OMCI_PM_STATUS_STATE_ERROR;
            goto out;
        /*Error that do not need to display error message*/
        case MOCA_API_TIMEOUT:
            sts = OMCI_PM_STATUS_TIMEOUT;
            goto out;
        /*Errors that need to display error messages*/
        default:
            OMCIPM_LOG_ERROR("Getting MoCA status returns error: %d", ret);
            sts = OMCI_PM_STATUS_ERROR;
            goto out;
    }

    ret = moca_get_gen_stats(pMoca, 0, &stats);
    switch (ret)
    {
        /*No error - just break out*/
        case MOCA_API_SUCCESS:
            break;
        /*Error that do not need to display error message*/
        case MOCA_API_CANT_CONNECT:
            sts = OMCI_PM_STATUS_STATE_ERROR;
            goto out;
        /*Error that do not need to display error message*/
        case MOCA_API_TIMEOUT:
            sts = OMCI_PM_STATUS_TIMEOUT;
            goto out;
        /*Errors that need to display error messages*/
        default:
            OMCIPM_LOG_ERROR("Getting MoCA statistics returns error: %d", ret);
            sts = OMCI_PM_STATUS_ERROR;
            goto out;
    }

    // if link is down or no valid node ID then return OK
    // and do nothing so that all values of counters are zero
    // Only fetch node statistics if the link is up
    if (moca_if.link_status == MOCA_LINK_UP &&
        moca_net.node_id < OMCIPM_DRV_MOCA_NODE_ID_MAX)
    {
        ret = moca_get_gen_node_status(pMoca, moca_net.node_id, &moca_node);
        switch (ret)
        {
            /*No error - just break out*/
            case MOCA_API_SUCCESS:
                break;
            /*Error that do not need to display error message*/
            case MOCA_API_CANT_CONNECT:
                sts = OMCI_PM_STATUS_STATE_ERROR;
                goto out;
            /*Error that do not need to display error message*/
            case MOCA_API_TIMEOUT:
                sts = OMCI_PM_STATUS_TIMEOUT;
                goto out;
            /*Errors that need to display error messages*/
            default:
                OMCIPM_LOG_ERROR("Getting MoCA node status returns error: %d", ret);
                sts = OMCI_PM_STATUS_ERROR;
                goto out;
        }

        moca_ext_in.index = moca_net.node_id;

        if (moca_net.network_moca_version == MoCA_VERSION_2_0)
            moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_TX_BC_NPER;
        else
            moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_TX_BCAST;

        ret = moca_get_gen_node_ext_status(pMoca, &moca_ext_in, &moca_ext);
        switch (ret)
        {
            /*No error - just break out*/
            case MOCA_API_SUCCESS:
                break;
            /*Error that do not need to display error message*/
            case MOCA_API_CANT_CONNECT:
                sts = OMCI_PM_STATUS_STATE_ERROR;
                goto out;
            /*Error that do not need to display error message*/
            case MOCA_API_TIMEOUT:
                sts = OMCI_PM_STATUS_TIMEOUT;
                goto out;
            /*Errors that need to display error messages*/
            default:
                OMCIPM_LOG_ERROR("Getting MoCA extended status returns error: %d", ret);
                sts = OMCI_PM_STATUS_ERROR;
                goto out;
        }

        /* get status and statistics info of self node ID */
        pCounters->phyTxBroadcastRate = moca_ext.phy_rate;


        if (moca_net.network_moca_version == MoCA_VERSION_2_0)
            moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_TX_UC_NPER;
        else
            moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_TX_UCAST;

        ret = moca_get_gen_node_ext_status(pMoca, &moca_ext_in, &moca_ext);
        switch (ret)
        {
            /*No error - just break out*/
            case MOCA_API_SUCCESS:
                break;
            /*Error that do not need to display error message*/
            case MOCA_API_CANT_CONNECT:
                sts = OMCI_PM_STATUS_STATE_ERROR;
                goto out;
            /*Error that do not need to display error message*/
            case MOCA_API_TIMEOUT:
                sts = OMCI_PM_STATUS_TIMEOUT;
                goto out;
            /*Errors that need to display error messages*/
            default:
                OMCIPM_LOG_ERROR("Getting MoCA extended status returns error: %d", ret);
                sts = OMCI_PM_STATUS_ERROR;
                goto out;
        }

        pCounters->phyTxRate = moca_ext.phy_rate;
        pCounters->txPowerControlReduction = moca_ext.tx_power;

        if (moca_net.network_moca_version == MoCA_VERSION_2_0)
            moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_RX_UC_NPER;
        else
            moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_RX_UCAST;

        ret = moca_get_gen_node_ext_status(pMoca, &moca_ext_in, &moca_ext);
        switch (ret)
        {
            /*No error - just break out*/
            case MOCA_API_SUCCESS:
                break;
            /*Error that do not need to display error message*/
            case MOCA_API_CANT_CONNECT:
                sts = OMCI_PM_STATUS_STATE_ERROR;
                goto out;
            /*Error that do not need to display error message*/
            case MOCA_API_TIMEOUT:
                sts = OMCI_PM_STATUS_TIMEOUT;
                goto out;
            /*Errors that need to display error messages*/
            default:
                OMCIPM_LOG_ERROR("Getting MoCA extended status returns error: %d", ret);
                sts = OMCI_PM_STATUS_ERROR;
                goto out;
        }

        pCounters->phyRxRate = moca_ext.phy_rate;
        pCounters->rxPowerLevel = moca_ext.rx_power;

        if (moca_net.network_moca_version == MoCA_VERSION_2_0)
            moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_RX_BC_NPER;
        else
            moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_RX_BCAST;

        ret = moca_get_gen_node_ext_status(pMoca, &moca_ext_in, &moca_ext);
        switch (ret)
        {
            /*No error - just break out*/
            case MOCA_API_SUCCESS:
                break;
            /*Error that do not need to display error message*/
            case MOCA_API_CANT_CONNECT:
                sts = OMCI_PM_STATUS_STATE_ERROR;
                goto out;
            /*Error that do not need to display error message*/
            case MOCA_API_TIMEOUT:
                sts = OMCI_PM_STATUS_TIMEOUT;
                goto out;
            /*Errors that need to display error messages*/
            default:
                OMCIPM_LOG_ERROR("Getting MoCA extended status returns error: %d", ret);
                sts = OMCI_PM_STATUS_ERROR;
                goto out;
        }

        pCounters->phyRxBroadcastRate = moca_ext.phy_rate;
        pCounters->rxBroadcastPowerLevel = moca_ext.rx_power;

        pCounters->txPackets = stats.ecl_tx_total_pkts;
        pCounters->rxPackets = stats.ecl_rx_total_pkts;
        /*Not Compliant - Only returns errored packet without missed TCA packets*/

        pCounters->erroredMissedRxPackets = 0;
        pCounters->erroredRxPackets = 0;
        for (i = 0; i < OMCIPM_DRV_MOCA_NODE_ID_MAX; i++)
        {
            if (moca_net.connected_nodes & (1 << i))
            {
               memset(&node_stats, 0, sizeof(node_stats));
               node_stats_in.reset_stats = 0;
               node_stats_in.index = i;

               ret = moca_get_node_stats(pMoca, &node_stats_in, &node_stats);
               if (ret == MOCA_API_SUCCESS)
               {
                   pCounters->erroredMissedRxPackets +=
                     node_stats.primary_ch_rx_no_sync +
                     node_stats.secondary_ch_rx_no_sync;
                   pCounters->erroredRxPackets +=
                     node_stats.primary_ch_rx_cw_corrected +
                     node_stats.primary_ch_rx_cw_uncorrected +
                     node_stats.secondary_ch_rx_cw_corrected +
                     node_stats.secondary_ch_rx_cw_uncorrected;
               }
            }
        }

        for (j = 0; j < BCM_OMCI_MAC_STR_SIZE; j++)
            pCounters->mac[j] = moca_node.eui.addr[j];
    }

out:
    if (pMoca != NULL)
    {
        moca_close(pMoca);
    }
    return sts;
}

static BCM_OMCI_PM_STATUS omcipm_getCountersNextMocaIntf(UINT16 *physPortId, void *counters)
{
    int ret = MOCA_API_SUCCESS;
    BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;
    UINT32   nodeId = 0, j = 0;
    PBCM_OMCI_PM_MOCA_INTERFACE_COUNTER pCounters =
      (PBCM_OMCI_PM_MOCA_INTERFACE_COUNTER)counters;
    void * pMoca;
    struct moca_interface_status          moca_if;
    struct moca_network_status            moca_net;
    struct moca_gen_stats                 stats;
    struct moca_gen_node_status           moca_node;
    struct moca_node_stats                node_stats;
    struct moca_node_stats_in             node_stats_in;
    struct moca_gen_node_ext_status_in    moca_ext_in;
    struct moca_gen_node_ext_status       moca_ext;

    memset(&moca_if, 0, sizeof(moca_if));
    memset(&moca_net, 0, sizeof(moca_net));
    memset(&stats, 0, sizeof(stats));
    memset(&moca_node, 0, sizeof(moca_node));
    memset(&node_stats, 0, sizeof(node_stats));
    memset(&moca_ext, 0, sizeof(moca_ext));

    pMoca = moca_open(NULL);

    if (pMoca == NULL)
    {
        sts = OMCI_PM_STATUS_STATE_ERROR;
        goto out;
    }

    ret = moca_get_interface_status(pMoca, &moca_if);
    switch (ret)
    {
        /*No error - just break out*/
        case MOCA_API_SUCCESS:
            break;
        /*Error that do not need to display error message*/
        case MOCA_API_CANT_CONNECT:
            sts = OMCI_PM_STATUS_STATE_ERROR;
            goto out;
        /*Error that do not need to display error message*/
        case MOCA_API_TIMEOUT:
            sts = OMCI_PM_STATUS_TIMEOUT;
            goto out;
        /*Errors that need to display error messages*/
        default:
            OMCIPM_LOG_ERROR("Getting MoCA status returns error: %d", ret);
            sts = OMCI_PM_STATUS_ERROR;
            goto out;
    }

    ret = moca_get_gen_stats(pMoca, 0, &stats);
    switch (ret)
    {
        /*No error - just break out*/
        case MOCA_API_SUCCESS:
            break;
        /*Error that do not need to display error message*/
        case MOCA_API_CANT_CONNECT:
            sts = OMCI_PM_STATUS_STATE_ERROR;
            goto out;
        /*Error that do not need to display error message*/
        case MOCA_API_TIMEOUT:
            sts = OMCI_PM_STATUS_TIMEOUT;
            goto out;
        /*Errors that need to display error messages*/
        default:
            OMCIPM_LOG_ERROR("Getting MoCA statistics returns error: %d", ret);
            sts = OMCI_PM_STATUS_ERROR;
            goto out;
    }

    // if link is down or no valid node ID then return OK
    // and do nothing so that all values of counters are zero
    // Only fetch node statistics if the link is up
    if (moca_if.link_status == MOCA_LINK_UP &&
        moca_net.node_id < OMCIPM_DRV_MOCA_NODE_ID_MAX)
    {
        for (nodeId = (*physPortId + 1); nodeId < OMCIPM_DRV_MOCA_NODE_ID_MAX; nodeId++)
        {
            if ((moca_net.connected_nodes >> nodeId) & 0x01)
                break;
        }

        if (nodeId == OMCIPM_DRV_MOCA_NODE_ID_MAX)
        {
            OMCIPM_LOG_ERROR("Cannot find any active node ID > %d", *physPortId);
            sts = OMCI_PM_STATUS_NOT_FOUND;
            goto out;
        }
        else
        {
            printf("[NOTICE omcipm] Active Node ID: %lu\n", nodeId);
        }

        ret = moca_get_gen_node_status(pMoca, nodeId, &moca_node);
        switch (ret)
        {
            /*No error - just break out*/
            case MOCA_API_SUCCESS:
                break;
            /*Error that do not need to display error message*/
            case MOCA_API_CANT_CONNECT:
                sts = OMCI_PM_STATUS_STATE_ERROR;
                goto out;
            /*Error that do not need to display error message*/
            case MOCA_API_TIMEOUT:
                sts = OMCI_PM_STATUS_TIMEOUT;
                goto out;
            /*Errors that need to display error messages*/
            default:
                OMCIPM_LOG_ERROR("Getting MoCA node status returns error: %d", ret);
                sts = OMCI_PM_STATUS_ERROR;
                goto out;
        }
        node_stats_in.reset_stats = 0;
        node_stats_in.index = nodeId;
        ret = moca_get_node_stats(pMoca, &node_stats_in, &node_stats);
        switch (ret)
        {
            /*No error - just break out*/
            case MOCA_API_SUCCESS:
                break;
            /*Error that do not need to display error message*/
            case MOCA_API_CANT_CONNECT:
                sts = OMCI_PM_STATUS_STATE_ERROR;
                goto out;
            /*Error that do not need to display error message*/
            case MOCA_API_TIMEOUT:
                sts = OMCI_PM_STATUS_TIMEOUT;
                goto out;
            /*Errors that need to display error messages*/
            default:
                OMCIPM_LOG_ERROR("Getting MoCA interface statistics returns error: %d", ret);
                sts = OMCI_PM_STATUS_ERROR;
                goto out;
        }
    }

    moca_ext_in.index = nodeId;

    if (moca_net.network_moca_version == MoCA_VERSION_2_0)
        moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_TX_BC_NPER;
    else
        moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_TX_BCAST;

    ret = moca_get_gen_node_ext_status(pMoca, &moca_ext_in, &moca_ext);
    switch (ret)
    {
        /*No error - just break out*/
        case MOCA_API_SUCCESS:
            break;
        /*Error that do not need to display error message*/
        case MOCA_API_CANT_CONNECT:
            sts = OMCI_PM_STATUS_STATE_ERROR;
            goto out;
        /*Error that do not need to display error message*/
        case MOCA_API_TIMEOUT:
            sts = OMCI_PM_STATUS_TIMEOUT;
            goto out;
        /*Errors that need to display error messages*/
        default:
            OMCIPM_LOG_ERROR("Getting MoCA extended status returns error: %d", ret);
            sts = OMCI_PM_STATUS_ERROR;
            goto out;
    }

    pCounters->phyTxBroadcastRate = moca_ext.phy_rate;

    if (moca_net.network_moca_version == MoCA_VERSION_2_0)
        moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_TX_UC_NPER;
    else
        moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_TX_UCAST;

    ret = moca_get_gen_node_ext_status(pMoca, &moca_ext_in, &moca_ext);
    switch (ret)
    {
        /*No error - just break out*/
        case MOCA_API_SUCCESS:
            break;
        /*Error that do not need to display error message*/
        case MOCA_API_CANT_CONNECT:
            sts = OMCI_PM_STATUS_STATE_ERROR;
            goto out;
        /*Error that do not need to display error message*/
        case MOCA_API_TIMEOUT:
            sts = OMCI_PM_STATUS_TIMEOUT;
            goto out;
        /*Errors that need to display error messages*/
        default:
            OMCIPM_LOG_ERROR("Getting MoCA extended status returns error: %d", ret);
            sts = OMCI_PM_STATUS_ERROR;
            goto out;
    }

    pCounters->phyTxRate = moca_ext.phy_rate;
    pCounters->txPowerControlReduction = moca_ext.tx_power;

    if (moca_net.network_moca_version == MoCA_VERSION_2_0)
        moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_RX_UC_NPER;
    else
        moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_RX_UCAST;

    ret = moca_get_gen_node_ext_status(pMoca, &moca_ext_in, &moca_ext);
    switch (ret)
    {
        /*No error - just break out*/
        case MOCA_API_SUCCESS:
            break;
        /*Error that do not need to display error message*/
        case MOCA_API_CANT_CONNECT:
            sts = OMCI_PM_STATUS_STATE_ERROR;
            goto out;
        /*Error that do not need to display error message*/
        case MOCA_API_TIMEOUT:
            sts = OMCI_PM_STATUS_TIMEOUT;
            goto out;
        /*Errors that need to display error messages*/
        default:
            OMCIPM_LOG_ERROR("Getting MoCA extended status returns error: %d", ret);
            sts = OMCI_PM_STATUS_ERROR;
            goto out;
    }

    pCounters->phyRxRate = moca_ext.phy_rate;
    pCounters->rxPowerLevel = moca_ext.rx_power;

    if (moca_net.network_moca_version == MoCA_VERSION_2_0)
        moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_RX_BC_NPER;
    else
        moca_ext_in.profile_type = MOCA_EXT_STATUS_PROFILE_RX_BCAST;

    ret = moca_get_gen_node_ext_status(pMoca, &moca_ext_in, &moca_ext);
    switch (ret)
    {
        /*No error - just break out*/
        case MOCA_API_SUCCESS:
            break;
        /*Error that do not need to display error message*/
        case MOCA_API_CANT_CONNECT:
            sts = OMCI_PM_STATUS_STATE_ERROR;
            goto out;
        /*Error that do not need to display error message*/
        case MOCA_API_TIMEOUT:
            sts = OMCI_PM_STATUS_TIMEOUT;
            goto out;
        /*Errors that need to display error messages*/
        default:
            OMCIPM_LOG_ERROR("Getting MoCA extended status returns error: %d", ret);
            sts = OMCI_PM_STATUS_ERROR;
            goto out;
    }

    pCounters->phyRxBroadcastRate = moca_ext.phy_rate;
    pCounters->rxBroadcastPowerLevel = moca_ext.rx_power;


    /* if it's self node then get counters from stats */
    if (nodeId == moca_net.node_id)
    {
        pCounters->txPackets = stats.ecl_tx_total_pkts;
        pCounters->rxPackets = stats.ecl_rx_total_pkts;
        /*Not Compliant - Only returns errored packet without missed TCA packets*/
        pCounters->erroredMissedRxPackets = node_stats.primary_ch_rx_no_sync +
          node_stats.secondary_ch_rx_no_sync;
        pCounters->erroredRxPackets = node_stats.primary_ch_rx_cw_corrected +
          node_stats.primary_ch_rx_cw_uncorrected +
          node_stats.secondary_ch_rx_cw_corrected +
          node_stats.secondary_ch_rx_cw_uncorrected;
    }
    /* otherwise Not Compliant - Always returns 0*/
    else
    {
        pCounters->txPackets = node_stats.tx_packets;
        pCounters->rxPackets = node_stats.rx_packets;
        pCounters->erroredMissedRxPackets = 0;
        pCounters->erroredRxPackets = 0;
    }

    for (j = 0; j < BCM_OMCI_MAC_STR_SIZE; j++)
        pCounters->mac[j] = moca_node.eui.addr[j];

    /* return the found node ID */
    *physPortId = nodeId;

out:
    if (pMoca != NULL)
    {
        moca_close(pMoca);
    }
    return sts;
}
#endif   // SUPPORT_MOCA

static BCM_OMCI_PM_STATUS omcipm_getCountersIpHost(void *counters)
{
  BCM_OMCI_PM_STATUS sts = OMCI_PM_STATUS_SUCCESS;
  BCM_OMCI_PM_IP_HOST_COUNTER* pCounters = (BCM_OMCI_PM_IP_HOST_COUNTER*)counters;
  FILE *fptr;
  char buf1[1024], buf2[1024];
  unsigned int p1, p2, p3, p4, p5, p6;
  int num = 0;

  // Init data structure and message buffers.
  memset(pCounters, 0, sizeof(BCM_OMCI_PM_IP_HOST_COUNTER));

  /* get icmpErrors
   *   read /proc/net/snmp
   *   Icmp: InMsgs InErrors InDestUnreachs InTimeExcds InParmProbs InSrcQuenchs InRedirects InEchos InEchoReps InTimestamps InTimestampReps InAddrMasks InAddrMaskReps OutMsgs OutErrors OutDestUnreachs OutTimeExcds OutParmProbs OutSrcQuenchs OutRedirects OutEchos OutEchoReps OutTimestamps OutTimestampReps OutAddrMasks OutAddrMaskReps
   *   Icmp: 3 0 1 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
   *   count: InDestUnreachs(type=3)
   *             InTimeExcds(type=11)
   *             InParmProbs(type=12)
   *             InSrcQuenchs(type=4)
   */
  fptr = fopen("/proc/net/snmp", "r");
  if (fptr == NULL)
  {
      OMCIPM_LOG_ERROR("Can't open /proc/net/snmp");
      return OMCI_PM_STATUS_NOT_FOUND;
  }

  while (fgets(buf1, sizeof buf1, fptr))
  {
      if (!fgets(buf2, sizeof buf2, fptr))
          break;
      if (strncmp(buf1, "Icmp:", 5) == 0)
      {
          num = sscanf(buf2, "Icmp: %u %u %u %u %u %u",
            &p1, &p2, &p3, &p4, &p5, &p6);
          break;
      }
  }
  fclose(fptr);

  if (num != 6)
  {
      OMCIPM_LOG_ERROR("Can't get icmp error counter. num=%d", num);
      return OMCI_PM_STATUS_RESOURCE_ERROR;
  }

  pCounters->icmpErrors = p3 + p4 + p5 + p6;

  /* send message to dnsproxy to get dns error counter */
#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1
#ifdef DMP_X_ITU_ORG_GPON_1
  if (omcipmDnsStatsCbFunc != NULL)
  {
    sts = omcipmDnsStatsCbFunc((void*)&pCounters->dnsErrors);
  }
#endif
#endif
  return sts;
}

BCM_OMCI_PM_STATUS bcm_omcipm_getCounters(BCM_OMCI_PM_CLASS_ID classId,
  UINT16 physPortId,
  void *counters)
{
    BCM_OMCI_PM_STATUS ret = OMCI_PM_STATUS_NOT_SUPPORTED;

    switch (classId)
    {
#ifdef SUPPORT_GPONCTL
        case BCM_OMCI_PM_CLASS_GEM_PORT:
            ret = omcipm_getCountersGemPort(physPortId, counters);
            break;
        case BCM_OMCI_PM_CLASS_FEC:
            ret = omcipm_getCountersFec(counters);
            break;
        case BCM_OMCI_PM_CLASS_GAL_ENET:
            ret = omcipm_getCountersGalEnet(physPortId, counters);
            break;
#endif  // SUPPORT_GPONCTL

        case BCM_OMCI_PM_CLASS_ENET:
            ret = omcipm_getCountersEnet(physPortId, counters);
            break;
        case BCM_OMCI_PM_CLASS_ENET2:
            ret = omcipm_getCountersEnet2(physPortId, counters);
            break;
        case BCM_OMCI_PM_CLASS_ENET3:
            ret = omcipm_getCountersEnet3(physPortId, counters);
            break;
        case BCM_OMCI_PM_CLASS_ENETDN:
            ret = omcipm_getCountersEnetDn(physPortId, counters);
            break;
        case BCM_OMCI_PM_CLASS_ENETUP:
            ret = omcipm_getCountersEnetUp(physPortId, counters);
            break;
        case BCM_OMCI_PM_CLASS_BRIDGE:
            ret = omcipm_getCountersBridge(physPortId, counters);
            break;
        case BCM_OMCI_PM_CLASS_BRIDGE_PORT:
            ret = omcipm_getCountersBridgePort(physPortId, counters);
            break;

#ifdef DMP_X_ITU_ORG_VOICE_1
        case BCM_OMCI_PM_CLASS_RTP:
            ret = omcipm_getCountersRTP(physPortId, counters);
            break;
#endif  // DMP_X_ITU_ORG_VOICE_1

#ifdef SUPPORT_MOCA
        case BCM_OMCI_PM_CLASS_MOCA_ENET:
            ret = omcipm_getCountersMocaEnet(physPortId, counters);
            break;
        case BCM_OMCI_PM_CLASS_MOCA_INTF:
            ret = omcipm_getCountersMocaIntf(counters);
            break;
#endif  // SUPPORT_MOCA

        case BCM_OMCI_PM_CLASS_IPHOST:
            ret = omcipm_getCountersIpHost(counters);
            break;
        default:
            break;
    }

    return ret;
}

BCM_OMCI_PM_STATUS bcm_omcipm_getCountersNext(BCM_OMCI_PM_CLASS_ID classId,
  UINT16 *physPortId __attribute__((unused)),
  void *counters __attribute__((unused)))
{
    BCM_OMCI_PM_STATUS ret = OMCI_PM_STATUS_NOT_SUPPORTED;

    switch (classId)
    {
#ifdef SUPPORT_MOCA
        case BCM_OMCI_PM_CLASS_MOCA_INTF:
            ret = omcipm_getCountersNextMocaIntf(physPortId, counters);
            break;
#endif
        default:
            break;
    }

    return ret;
}
