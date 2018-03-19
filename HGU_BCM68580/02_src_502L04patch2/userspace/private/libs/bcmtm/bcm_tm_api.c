/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2015:proprietary:standard

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
#include <ctype.h>
#include <net/if.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "os_defs.h"

#include "bcmnet.h"
#include "bcm_tm_api.h"


/*
 * Macros
 */

//#define CC_BCM_TM_API_DEBUG

#if defined(CC_BCM_TM_API_DEBUG)
#define bcmTmApi_debug(fmt, arg...) printf("%s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#else
#define bcmTmApi_debug(fmt, arg...)
#endif

#define bcmTmApi_error(fmt, arg...) printf("ERROR[%s.%u]: " fmt, __FUNCTION__, __LINE__, ##arg)

/*
 * Local functions
 */

static int __getEthPortNumber(char *ifName)
{
    int socketFd;

    socketFd = socket(AF_INET, SOCK_DGRAM, 0);

    if(socketFd <= 0)
    {
        bcmTmApi_error("%s\n", strerror(errno));

        return -EINVAL;
    }

    {
        struct interface_data enetif;
        struct ifreq intf;
        int ret;

        strncpy(intf.ifr_name, "bcmsw", sizeof(intf.ifr_name));
        strncpy(enetif.ifname, ifName, sizeof(enetif.ifname));

        enetif.switch_port_id = -1;

        intf.ifr_data = (char *)&enetif;

        if((ret = ioctl(socketFd, SIOCGSWITCHPORT, &intf)) < 0)
        {
            bcmTmApi_error("SIOCGSWITCHPORT <%s>: %s\n", enetif.ifname, strerror(errno));

            close(socketFd);

            return ret;
        }

        close(socketFd);

        return enetif.switch_port_id;
    }
}

static int __getPhyPort(char *ifName, bcmTmDrv_phyType_t *phyType_p, int *port_p)
{
    char *index_p;

    if(strstr(ifName, BCM_TM_DRV_PHY_TYPE_ETH_PREFIX))
    {
        *phyType_p = BCM_TM_DRV_PHY_TYPE_ETH;
        *port_p = __getEthPortNumber(ifName);

        return 0;
    }
    else if(strstr(ifName, BCM_TM_DRV_PHY_TYPE_PTM_PREFIX) ||
            strstr(ifName, BCM_TM_DRV_PHY_TYPE_ATM_PREFIX))
    {
        *phyType_p = BCM_TM_DRV_PHY_TYPE_XTM;
        *port_p = -1;

        bcmTmApi_error("XTM is not yet supported\n");

        return -1;
    }
    else if((index_p = strstr(ifName, BCM_TM_DRV_PHY_TYPE_DPI_PREFIX)))
    {
        index_p += strlen(BCM_TM_DRV_PHY_TYPE_DPI_PREFIX);

        *phyType_p = BCM_TM_DRV_PHY_TYPE_DPI;
        *port_p = atoi(index_p);

        return 0;
    }
    else
    {
        bcmTmApi_error("Could not find interface %s\n", ifName);

        return -EINVAL;
    }
}

static int __sendBcmTmCmd(bcmTmIoctl_cmd_t cmd, bcmTmDrv_arg_t *arg_p)
{
    int ret;
    int fd;

    fd = open(BCM_TM_DRV_DEVICE_NAME, O_RDWR);
    if(fd < 0)
    {
        bcmTmApi_error("%s: %s\n", BCM_TM_DRV_DEVICE_NAME, strerror(errno));

        return -EINVAL;
    }

    ret = ioctl(fd, cmd, (uint32_t)arg_p);
    if(ret)
    {
        bcmTmApi_error("%s\n", strerror(errno));
    }

    close(fd);

    return ret;
}


/*
 * Public functions
 */

/*******************************************************************************
 *
 * Function: bcmTmApi_masterConfig
 *
 * Globally enables or disables the BCM TM function. Disabling the BCM TM does
 * not cause the existing settings to be lost.
 *
 *******************************************************************************/
int bcmTmApi_masterConfig(int enable)
{
    bcmTmDrv_arg_t tm;

    bcmTmApi_debug("enable %d", enable);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    tm.enable = enable;

    return __sendBcmTmCmd(BCM_TM_IOCTL_MASTER_CONFIG, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_portConfig
 *
 * Configures the shaper bit rate (kbps) and Maximum Burst Size (mbs) of the
 * specified port/mode. The mbs setting specifies the amount of bytes that are
 * allowed to be transmitted above the shaper bit rate when the input bit rate
 * exceeds the shaper bit rate. Once the mbs is achieved, no bursts will be
 * allowed until the input bit rate becomes lower than the shaper bit rate.
 *
 * This API does not apply the specified settings. This can only
 * be done via the bcmTmApi_apply API.
 *
 *******************************************************************************/
int bcmTmApi_portConfig(char *ifName, bcmTmDrv_mode_t mode, int kbps,
                        int mbs, bcmTmDrv_shapingType_t shapingType)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u, kbps %d, mbs %d, shapingType %d",
                   ifName, mode, kbps, mbs, shapingType);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;
    tm.kbps = kbps;
    tm.mbs = mbs;
    tm.shapingType = shapingType;

    return __sendBcmTmCmd(BCM_TM_IOCTL_PORT_CONFIG, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_getPortConfig
 *
 * Returns the shaper bit rate (kbps), Maximum Burst Size (mbs), and Shaping
 * Typeof the specified port/mode.
 *
 *******************************************************************************/
int bcmTmApi_getPortConfig(char *ifName, bcmTmDrv_mode_t mode, int *kbps_p,
                           int *mbs_p, bcmTmDrv_shapingType_t *shapingType_p)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u", ifName, mode);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;

    ret = __sendBcmTmCmd(BCM_TM_IOCTL_GET_PORT_CONFIG, &tm);
    if(ret < 0)
    {
        return -EINVAL;
    }

    *kbps_p = tm.kbps;
    *mbs_p = tm.mbs;
    *shapingType_p = tm.shapingType;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcmTmApi_getPortCapability
 *
 * Returns the scheduling type, max queues, max strict priority queues, 
 * port shaper, and queue shaper support of the specified port.
 *
 *******************************************************************************/
int bcmTmApi_getPortCapability(char *ifName, uint32_t *schedType_p, int *maxQueues_p,
                               int *maxSpQueues_p, uint8_t *portShaper_p, uint8_t *queueShaper_p)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s", ifName);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }

    ret = __sendBcmTmCmd(BCM_TM_IOCTL_GET_PORT_CAPABILITY, &tm);
    if(ret < 0)
    {
        return -EINVAL;
    }

    *schedType_p = tm.portCapability.schedType;
    *maxQueues_p = tm.portCapability.maxQueues;
    *maxSpQueues_p = tm.portCapability.maxSpQueues;
    *portShaper_p = tm.portCapability.portShaper;
    *queueShaper_p = tm.portCapability.queueShaper;    

    return 0;
}

/*******************************************************************************
 *
 * Function: bcmTmApi_queueConfig
 *
 * Configures the shaper bit rate (kbps) and Maximum Burst Size (mbs) of the
 * specified port/queue/mode/shaperType. A shaper type can be either
 * minimum rate or maximum rate.
 *
 * Each queue supports a minimum rate shaper and a maximum rate shaper.
 * The minimum queue rates have precedence over the maximum queue rates among
 * queues of a given port. The minimum queue rates are guaranteed as long as the
 * sum of the minimum queue rates is smaller than or equal to the port's rate.
 * When the sum of the minimum queue rates is smaller than the port's rate, the
 * remaining bandwith (after satisfying the minimum rate of each queue) is
 * distributed amongst the queues based on the arbitration scheme in use (SP,
 * WRR, etc), and is capped at the maximum rate of each queue.
 *
 * The mbs setting specifies the amount of bytes that are allowed to be
 * transmitted above the shaper bit rate when the input bit rate
 * exceeds the shaper bit rate. Once the mbs is achieved, no bursts will be
 * allowed until the input bit rate becomes lower than the shaper bit rate.
 *
 * This API does not apply the specified settings. This can only
 * be done via the bcmTmApi_apply API.
 *
 *******************************************************************************/
int bcmTmApi_queueConfig(char *ifName, bcmTmDrv_mode_t mode, int queue,
                         bcmTmDrv_shaperType_t shaperType, int kbps, int mbs)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u, queue %u, shaperType %u, kbps %d, mbs %d",
                   ifName, mode, queue, shaperType, kbps, mbs);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;
    tm.queue = queue;
    tm.shaperType = shaperType;
    tm.kbps = kbps;
    tm.mbs = mbs;

    return __sendBcmTmCmd(BCM_TM_IOCTL_QUEUE_CONFIG, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_queueUnconfig
 *
 * Clear the configured bit of the specified queue.
 * This API does not need to apply. The configured bit is used to detect if a 
 * queue is configured by BCM TM API or not.
 *
 *******************************************************************************/
int bcmTmApi_queueUnconfig(char *ifName, bcmTmDrv_mode_t mode, int queue)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u, queue %u", ifName, mode, queue);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;
    tm.queue = queue;

    return __sendBcmTmCmd(BCM_TM_IOCTL_QUEUE_UNCONFIG, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_getQueueConfig
 *
 * Returns the max/min shaping bit rate (kbps), Maximum Burst Size (mbs),
 * weight, and queue size of the specified port, mode, and queue. 
 *
 *******************************************************************************/
int bcmTmApi_getQueueConfig(char *ifName, bcmTmDrv_mode_t mode, int queue,
                            int *maxRateKbps_p, int *minRateKbps_p, int *mbs_p,
                            int *weight_p, int *qsize_p)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u, queue %d", ifName, mode, queue);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;
    tm.queue = queue;

    ret = __sendBcmTmCmd(BCM_TM_IOCTL_GET_QUEUE_CONFIG, &tm);
    if(ret < 0)
    {
        return -EINVAL;
    }

    *maxRateKbps_p = tm.kbps;
    *minRateKbps_p = tm.minKbps;
    *mbs_p = tm.mbs;
    *weight_p = tm.weight;
    *qsize_p = tm.qsize;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcmTmApi_setQueueCap
 *
 * Configures the queue level cap, which must be a value greater than zero
 * and smaller than or equal to the number of entries assigned to the queue.
 *
 * This API does not apply the specified settings. This can only be done via
 * the bcmTmDrv_apply API.
 *
 *******************************************************************************/
int bcmTmApi_setQueueCap(char *ifName, bcmTmDrv_mode_t mode, int queue,
                         int nbrOfEntriesCap)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u, queue %u, nbrOfEntriesCap %d",
                   ifName, mode, queue, nbrOfEntriesCap);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;
    tm.queue = queue;
    tm.nbrOfEntriesCap = nbrOfEntriesCap;

    return __sendBcmTmCmd(BCM_TM_IOCTL_SET_QUEUE_CAP, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_allocQueueProfileId
 *
 * Obtain a free/available queue profile ID
 *
 *******************************************************************************/
int bcmTmApi_allocQueueProfileId(int *queueProfileId_p)
{
    bcmTmDrv_arg_t tm;
    int ret;

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __sendBcmTmCmd(BCM_TM_IOCTL_ALLOC_QUEUE_PROFILE_ID, &tm);
    if(ret < 0)
    {
        return -EINVAL;
    }

    *queueProfileId_p = tm.queueProfileId;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcmTmApi_freeQueueProfileId
 *
 * free a queue profile ID
 *
 *******************************************************************************/
int bcmTmApi_freeQueueProfileId(int queueProfileId)
{
    bcmTmDrv_arg_t tm;

    bcmTmApi_debug("queueProfileId %d", queueProfileId);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    tm.queueProfileId = queueProfileId;

    return __sendBcmTmCmd(BCM_TM_IOCTL_FREE_QUEUE_PROFILE_ID, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_queueProfileConfig
 *
 * Configures the queue profile parameters, such as, drop probability,
 * minimum threshold, and maximum threshold
 *
 *******************************************************************************/
int bcmTmApi_queueProfileConfig(int queueProfileId, int dropProbability,
                                int minThreshold, int maxThreshold)
{
    bcmTmDrv_arg_t tm;

    bcmTmApi_debug("queueProfileId %d, dropProbability %d, minThreshold %d, "
                   "maxThreshold %d", queueProfileId, dropProbability,
                   minThreshold, maxThreshold);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    tm.queueProfileId = queueProfileId;
    tm.dropProbability = dropProbability;
    tm.minThreshold = minThreshold;
    tm.maxThreshold = maxThreshold;

    return __sendBcmTmCmd(BCM_TM_IOCTL_QUEUE_PROFILE_CONFIG, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_getQueueProfileConfig
 *
 * Returns the queue profile parameters, such as, drop probability,
 * minimum threshold, and maximum threshold with given queue profile ID
 *
 *******************************************************************************/
int bcmTmApi_getQueueProfileConfig(int queueProfileId, int *dropProbability_p,
                                   int *minThreshold_p, int *maxThreshold_p)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("queueProfileId %d", queueProfileId);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    tm.queueProfileId = queueProfileId;

    ret = __sendBcmTmCmd(BCM_TM_IOCTL_GET_QUEUE_PROFILE_CONFIG, &tm);
    if(ret < 0)
    {
        return -EINVAL;
    }

    *dropProbability_p = tm.dropProbability;
    *minThreshold_p = tm.minThreshold;
    *maxThreshold_p = tm.maxThreshold;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcmTmApi_queueDropAlgConfig
 *
 * Configures the drop algorithm, queueProfile indices of the
 * specified port, mode, and queue.
 *
 *******************************************************************************/
int bcmTmApi_queueDropAlgConfig(char *ifName, int queue, bcmTmDrv_dropAlg_t dropAlgorithm,
                                int queueProfileIdLo, int queueProfileIdHi,
                                uint32_t priorityMask0, uint32_t priorityMask1)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, queue %u, dropAlgorithm %d, queueProfileIdLo %d, "
                   "queueProfileIdHi %d, priorityMask0 0x%x, priorityMask1 0x%x",
                   ifName, queue, dropAlgorithm, queueProfileIdLo, queueProfileIdHi,
                   priorityMask0, priorityMask1);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.queue = queue;
    tm.dropAlgorithm = dropAlgorithm;
    tm.queueProfileId = queueProfileIdLo;
    tm.queueProfileIdHi = queueProfileIdHi;
    tm.priorityMask0 = priorityMask0;
    tm.priorityMask1 = priorityMask1;

    return __sendBcmTmCmd(BCM_TM_IOCTL_QUEUE_DROP_ALG_CONFIG, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_getQueueDropAlgConfig
 *
 * Returns the drop algorithm, queue profile indices of the
 * specified port, mode, and queue.
 *
 *******************************************************************************/
int bcmTmApi_getQueueDropAlgConfig(char *ifName, int queue, bcmTmDrv_dropAlg_t *dropAlgorithm_p,
                                   int *queueProfileIdLo_p, int *queueProfileIdHi_p,
                                   uint32_t *priorityMask0_p, uint32_t *priorityMask1_p)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, queue %d", ifName, queue);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.queue = queue;

    ret = __sendBcmTmCmd(BCM_TM_IOCTL_GET_QUEUE_DROP_ALG_CONFIG, &tm);
    if(ret < 0)
    {
        return -EINVAL;
    }

    *dropAlgorithm_p = tm.dropAlgorithm;
    *queueProfileIdLo_p = tm.queueProfileId;
    *queueProfileIdHi_p = tm.queueProfileIdHi;
    *priorityMask0_p = tm.priorityMask0;
    *priorityMask1_p = tm.priorityMask1;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcmTmApi_xtmChannelDropAlgConfig
 *
 * Configures the drop algorithm, queueProfile indices of the
 * specified XTM channel
 *
 *******************************************************************************/
int bcmTmApi_xtmChannelDropAlgConfig(int channel, bcmTmDrv_dropAlg_t dropAlgorithm,
                                     int queueProfileIdLo, int queueProfileIdHi,
                                     uint32_t priorityMask0, uint32_t priorityMask1)
{
    bcmTmDrv_arg_t tm;

    bcmTmApi_debug("channel %u, dropAlgorithm %d, queueProfileIdLo %d, "
                   "queueProfileIdHi %d, priorityMask0 0x%x, priorityMask1 0x%x",
                   channel, dropAlgorithm, queueProfileIdLo, queueProfileIdHi,
                   priorityMask0, priorityMask1);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    tm.channel = channel;
    tm.dropAlgorithm = dropAlgorithm;
    tm.queueProfileId = queueProfileIdLo;
    tm.queueProfileIdHi = queueProfileIdHi;
    tm.priorityMask0 = priorityMask0;
    tm.priorityMask1 = priorityMask1;

    return __sendBcmTmCmd(BCM_TM_IOCTL_XTM_QUEUE_DROP_ALG_CONFIG, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_getXtmChannelDropAlgConfig
 *
 * Returns the drop algorithm, queue profile indices of the
 * specified XTM channel
 *
 *******************************************************************************/
int bcmTmApi_getXtmChannelDropAlgConfig(int channel, bcmTmDrv_dropAlg_t *dropAlgorithm_p,
                                        int *queueProfileIdLo_p, int *queueProfileIdHi_p,
                                        uint32_t *priorityMask0_p, uint32_t *priorityMask1_p)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("channel %d", channel);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    tm.channel = channel;

    ret = __sendBcmTmCmd(BCM_TM_IOCTL_GET_XTM_QUEUE_DROP_ALG_CONFIG, &tm);
    if(ret < 0)
    {
        return -EINVAL;
    }

    *dropAlgorithm_p = tm.dropAlgorithm;
    *queueProfileIdLo_p = tm.queueProfileId;
    *queueProfileIdHi_p = tm.queueProfileIdHi;
    *priorityMask0_p = tm.priorityMask0;
    *priorityMask1_p = tm.priorityMask1;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcmTmApi_getQueueStats
 *
 * Returns the transmitted packets, transmitted bytes, dropped packets,
 * and dropped bytes of the specified port, mode, and queue.
 *
 *******************************************************************************/
int bcmTmApi_getQueueStats(char *ifName, bcmTmDrv_mode_t mode, int queue,
                           uint32_t *txPackets_p, uint32_t *txBytes_p,
                           uint32_t *droppedPackets_p, uint32_t *droppedBytes_p,
                           uint32_t *bps_p)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u, queue %d", ifName, mode, queue);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;
    tm.queue = queue;

    ret = __sendBcmTmCmd(BCM_TM_IOCTL_GET_QUEUE_STATS, &tm);
    if(ret < 0)
    {
        return -EINVAL;
    }

    *txPackets_p = tm.queueStats.txPackets;
    *txBytes_p = tm.queueStats.txBytes;
    *droppedPackets_p = tm.queueStats.droppedPackets;
    *droppedBytes_p = tm.queueStats.droppedBytes;
    *bps_p = tm.queueStats.bps;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcmTmApi_setQueueWeight
 *
 * Configures the given Queue's weight.
 *
 * This API does not apply the specified settings. This can only
 * be done via the bcmTmApi_apply API.
 *
 *******************************************************************************/
int bcmTmApi_setQueueWeight(char *ifName, bcmTmDrv_mode_t mode, int queue, int weight)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u, queue %u, weight %d",
                   ifName, mode, queue, weight);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;
    tm.queue = queue;
    tm.weight = weight;

    return __sendBcmTmCmd(BCM_TM_IOCTL_QUEUE_WEIGHT, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_arbiterConfig
 *
 * Configures the Arbiter type of the given port to SP, WRR, SP+WRR, or WFQ.
 * arbiterArg is a generic argument that is passed to arbiters. Currently it is
 * only used in SP+WRR, where it indicates the lowest priority queue in the
 * SP Tier.
 *
 * This API does not apply the corresponding mode settings. This
 * can only be done via the bcmTmApi_apply API.
 *
 *******************************************************************************/
int bcmTmApi_arbiterConfig(char *ifName, bcmTmDrv_mode_t mode,
                           bcmTmDrv_arbiterType_t arbiterType, int arbiterArg)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, arbiterType %u, arbiterArg %u",
                   ifName, arbiterType, arbiterArg);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;
    tm.arbiterType = arbiterType;
    tm.arbiterArg = arbiterArg;

    return __sendBcmTmCmd(BCM_TM_IOCTL_ARBITER_CONFIG, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_getArbiterConfig
 *
 * Returns the Arbiter type of the given port and mode.
 * arbiterArg is a generic argument that is passed to arbiters.
 * Currently it is only used in SP+WRR, where it indicates the lowest 
 * priority queue in the SP Tier.
 *
 *******************************************************************************/
int bcmTmApi_getArbiterConfig(char *ifName, bcmTmDrv_mode_t mode,
                              bcmTmDrv_arbiterType_t *arbiterType_p, int *arbiterArg_p)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u", ifName, mode);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;

    ret = __sendBcmTmCmd(BCM_TM_IOCTL_GET_ARBITER_CONFIG, &tm);
    if(ret < 0)
    {
        return -EINVAL;
    }

    *arbiterType_p = tm.arbiterType;
    *arbiterArg_p = tm.arbiterArg;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcmTmApi_setPortMode
 *
 * Sets the mode of the given port to AUTO or MANUAL.
 *
 * This API does not apply the corresponding mode settings. This
 * can only be done via the bcmTmApi_apply API.
 *
 *******************************************************************************/
int bcmTmApi_setPortMode(char *ifName, bcmTmDrv_mode_t mode)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u", ifName, mode);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;

    return __sendBcmTmCmd(BCM_TM_IOCTL_PORT_MODE, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_modeReset
 *
 * Resets the current configuration of the given port and mode.
 *
 * This API does not apply the corresponding mode settings. This
 * can only be done via the bcmTmApi_apply API.
 *
 *******************************************************************************/
int bcmTmApi_modeReset(char *ifName, bcmTmDrv_mode_t mode)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u", ifName, mode);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;

    return __sendBcmTmCmd(BCM_TM_IOCTL_MODE_RESET, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_portEnable
 *
 * Enables or Disables the given port and mode.
 *
 * This API does not apply the corresponding mode settings. This
 * can only be done via the bcmTmApi_apply API.
 *
 *******************************************************************************/
int bcmTmApi_portEnable(char *ifName, bcmTmDrv_mode_t mode, int enable)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s, mode %u, enable %u", ifName, mode, enable);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }
    tm.mode = mode;
    tm.enable = enable;

    return __sendBcmTmCmd(BCM_TM_IOCTL_PORT_ENABLE, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_apply
 *
 * Applies the settings corresponding to the current mode of the specified port.
 *
 * This API also allows enabling or disabling the specified port.
 *
 *******************************************************************************/
int bcmTmApi_apply(char *ifName)
{
    bcmTmDrv_arg_t tm;
    int ret;

    bcmTmApi_debug("ifName %s", ifName);

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    ret = __getPhyPort(ifName, &tm.phy, &tm.port);
    if(ret)
    {
        bcmTmApi_error("Invalid ifName %s\n", ifName);
        return -EINVAL;
    }

    return __sendBcmTmCmd(BCM_TM_IOCTL_PORT_APPLY, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_status
 *
 * Prints the status of all ENABLED ports.
 *
 *******************************************************************************/
int bcmTmApi_status(void)
{
    bcmTmDrv_arg_t tm;

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    return __sendBcmTmCmd(BCM_TM_IOCTL_STATUS, &tm);
}

/*******************************************************************************
 *
 * Function: bcmTmApi_stats
 *
 * Prints the statistics counters of all ENABLED ports.
 *
 *******************************************************************************/
int bcmTmApi_stats(char *ifName)
{
    bcmTmDrv_arg_t tm;
    int ret;

    memset(&tm, 0, sizeof(bcmTmDrv_arg_t));

    if(BCM_TM_API_IS_IFNAME_VALID(ifName))
    {
        ret = __getPhyPort(ifName, &tm.phy, &tm.port);
        if(ret)
        {
            bcmTmApi_error("Invalid ifName %s\n", ifName);
            return -EINVAL;
        }
    }
    else
    {
        tm.port = -1;
    }

    return __sendBcmTmCmd(BCM_TM_IOCTL_STATS, &tm);
}
