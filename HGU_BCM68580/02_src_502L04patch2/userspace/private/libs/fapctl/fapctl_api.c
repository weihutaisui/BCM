/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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
#include "fapctl_api.h"


/*
 * Macros
 */

#define FAPCTL_IOCTL_FILE_NAME "/dev/bcmfap"

//#define CC_FAPCTL_DEBUG

#if defined(CC_FAPCTL_DEBUG)
#define fapCtl_debug(fmt, arg...) printf("%s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#else
#define fapCtl_debug(fmt, arg...)
#endif

#define fapCtl_error(fmt, arg...) printf("ERROR[%s.%u]: " fmt, __FUNCTION__, __LINE__, ##arg)

/*
 * Local functions
 */

static int __getPortNumber(char *ifName)
{
    int socketFd;

    socketFd = socket(AF_INET, SOCK_DGRAM, 0);

    if(socketFd <= 0)
    {
        fapCtl_error("%s\n", strerror(errno));

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
            fapCtl_error("SIOCGSWITCHPORT <%s>: %s\n", enetif.ifname, strerror(errno));

            close(socketFd);

            return ret;
        }

        close(socketFd);

        return enetif.switch_port_id;
    }
}

static int __sendFapCmd(fapIoctl_t fapIoctl, void *cmd_p)
{
    int ret;
    int fd;

    fd = open(FAPCTL_IOCTL_FILE_NAME, O_RDWR);
    if(fd < 0)
    {
        fapCtl_error("%s: %s\n", FAPCTL_IOCTL_FILE_NAME, strerror(errno));

        return -EINVAL;
    }

    ret = ioctl(fd, fapIoctl, (uint32_t)cmd_p);
    if(ret)
    {
        fapCtl_error("%s\n", strerror(errno));
    }

    close(fd);

    return ret;
}


/*
 * Public functions
 */

/*******************************************************************************
 *
 * Function: fapCtlTm_masterConfig
 *
 * Globally enables or disables the FAP TM function. Disabling the FAP TM does
 * not cause the existing settings to be lost.
 *
 *******************************************************************************/
int fapCtlTm_masterConfig(int enable)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("enable %d", enable);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_MASTER_CONFIG;
    tm.enable = enable;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_portConfig
 *
 * Configures the shaper bit rate (kbps) and Maximum Burst Size (mbs) of the
 * specified port/mode. The mbs setting specifies the amount of bytes that are
 * allowed to be transmitted above the shaper bit rate when the input bit rate
 * exceeds the shaper bit rate. Once the mbs is achieved, no bursts will be
 * allowed until the input bit rate becomes lower than the shaper bit rate.
 *
 * This API does not apply the specified settings into the FAP. This can only
 * be done via the fapCtlTm_apply API.
 *
 *******************************************************************************/
int fapCtlTm_portConfig(char *ifName, fapIoctl_tmMode_t mode, int kbps,
                        int mbs, fapIoctl_tmShapingType_t shapingType)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, mode %u, kbps %d, mbs %d, shapingType %d",
                 ifName, mode, kbps, mbs, shapingType);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_PORT_CONFIG;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;
    tm.kbps = kbps;
    tm.mbs = mbs;
    tm.shapingType = shapingType;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_getPortConfig
 *
 * Returns the shaper bit rate (kbps), Maximum Burst Size (mbs), and Shaping
 * Typeof the specified port/mode.
 *
 *******************************************************************************/
int fapCtlTm_getPortConfig(char *ifName, fapIoctl_tmMode_t mode, int *kbps_p,
                           int *mbs_p, fapIoctl_tmShapingType_t *shapingType_p)
{
    fapIoctl_tm_t tm;
    int ret;

    fapCtl_debug("ifName %s, mode %u", ifName, mode);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_GET_PORT_CONFIG;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;

    ret = __sendFapCmd(FAP_IOC_TM, &tm);
    if(ret < 0)
    {
        return FAP_ERROR;
    }

    *kbps_p = tm.kbps;
    *mbs_p = tm.mbs;
    *shapingType_p = tm.shapingType;

    return FAP_SUCCESS;
}

/*******************************************************************************
 *
 * Function: fapCtlTm_getPortCapability
 *
 * Returns the scheduling type, max queues, max strict priority queues, 
 * port shaper, and queue shaper support of the specified port.
 *
 *******************************************************************************/
int fapCtlTm_getPortCapability(char *ifName, uint32_t *schedType_p, int *maxQueues_p,
                               int *maxSpQueues_p, uint8_t *portShaper_p, uint8_t *queueShaper_p)
{
    fapIoctl_tm_t tm;
    int ret;

    fapCtl_debug("ifName %s", ifName);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_GET_PORT_CAPABILITY;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }

    ret = __sendFapCmd(FAP_IOC_TM, &tm);
    if(ret < 0)
    {
        return FAP_ERROR;
    }

    *schedType_p = tm.portCapability.schedType;
    *maxQueues_p = tm.portCapability.maxQueues;
    *maxSpQueues_p = tm.portCapability.maxSpQueues;
    *portShaper_p = tm.portCapability.portShaper;
    *queueShaper_p = tm.portCapability.queueShaper;    

    return FAP_SUCCESS;
}

/*******************************************************************************
 *
 * Function: fapCtlTm_queueConfig
 *
 * Configures the shaper bit rate (kbps) and Maximum Burst Size (mbs) of the
 * specified port/queue/mode/shaperType. The shaper type can be either the
 * minimum rate or maximum rate, as defined in fap4keTm_shaperType_t.
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
 * This API does not apply the specified settings into the FAP. This can only
 * be done via the fapCtlTm_apply API.
 *
 *******************************************************************************/
int fapCtlTm_queueConfig(char *ifName, fapIoctl_tmMode_t mode, int queue,
                         fapIoctl_tmShaperType_t shaperType, int kbps, int mbs)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, mode %u, queue %u, shaperType %u, kbps %d, mbs %d",
                 ifName, mode, queue, shaperType, kbps, mbs);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_CONFIG;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;
    tm.queue = queue;
    tm.shaperType = shaperType;
    tm.kbps = kbps;
    tm.mbs = mbs;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}


/*******************************************************************************
 *
 * Function: fapCtlTm_queueUnconfig
 *
 * Clear the configured bit of the specified queue.
 * This API does not need to apply. The configured bit is used to detect if a 
 * queue is configured by FAP TM API or not.
 *
 *******************************************************************************/
int fapCtlTm_queueUnconfig(char *ifName, fapIoctl_tmMode_t mode, int queue)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, mode %u, queue %u", ifName, mode, queue);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_UNCONFIG;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;
    tm.queue = queue;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_getQueueConfig
 *
 * Returns the max/min shaping bit rate (kbps), Maximum Burst Size (mbs),
 * weight, and queue size of the specified port, mode, and queue. 
 *
 *******************************************************************************/
int fapCtlTm_getQueueConfig(char *ifName, fapIoctl_tmMode_t mode, int queue,
                            int *maxRateKbps_p, int *minRateKbps_p, int *mbs_p,
                            int *weight_p, int *qsize_p)
{
    fapIoctl_tm_t tm;
    int ret;

    fapCtl_debug("ifName %s, mode %u, queue %d", ifName, mode, queue);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_GET_QUEUE_CONFIG;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;
    tm.queue = queue;

    ret = __sendFapCmd(FAP_IOC_TM, &tm);
    if(ret < 0)
    {
        return FAP_ERROR;
    }

    *maxRateKbps_p = tm.kbps;
    *minRateKbps_p = tm.minKbps;
    *mbs_p = tm.mbs;
    *weight_p = tm.weight;
    *qsize_p = tm.qsize;

    return FAP_SUCCESS;
}

/*******************************************************************************
 *
 * Function: fapCtlTm_allocQueueProfileId
 *
 * Obtain a free/available queue profile ID
 *
 *******************************************************************************/
int fapCtlTm_allocQueueProfileId(int *queueProfileId_p)
{
    fapIoctl_tm_t tm;
    int ret;

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_ALLOC_QUEUE_PROFILE_ID;

    ret = __sendFapCmd(FAP_IOC_TM, &tm);
    if(ret < 0)
    {
        return FAP_ERROR;
    }

    *queueProfileId_p = tm.queueProfileId;

    return FAP_SUCCESS;
}

/*******************************************************************************
 *
 * Function: fapCtlTm_freeQueueProfileId
 *
 * free a queue profile ID
 *
 *******************************************************************************/
int fapCtlTm_freeQueueProfileId(int queueProfileId)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("queueProfileId %d", queueProfileId);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_FREE_QUEUE_PROFILE_ID;
    tm.queueProfileId = queueProfileId;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_queueProfileConfig
 *
 * Configures the queue profile parameters, such as, drop probability,
 * minimum threshold, and maximum threshold
 *
 *******************************************************************************/
int fapCtlTm_queueProfileConfig(int queueProfileId, int dropProbability,
                                int minThreshold, int maxThreshold)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("queueProfileId %d, dropProbability %d, minThreshold %d, "
                 "maxThreshold %d", queueProfileId, dropProbability,
                 minThreshold, maxThreshold);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_PROFILE_CONFIG;
    tm.queueProfileId = queueProfileId;
    tm.dropProbability = dropProbability;
    tm.minThreshold = minThreshold;
    tm.maxThreshold = maxThreshold;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_getQueueProfileConfig
 *
 * Returns the queue profile parameters, such as, drop probability,
 * minimum threshold, and maximum threshold with given queue profile ID
 *
 *******************************************************************************/
int fapCtlTm_getQueueProfileConfig(int queueProfileId, int *dropProbability_p,
                                int *minThreshold_p, int *maxThreshold_p)
{
    fapIoctl_tm_t tm;
    int ret;

    fapCtl_debug("queueProfileId %d", queueProfileId);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_GET_QUEUE_PROFILE_CONFIG;
    tm.queueProfileId = queueProfileId;

    ret = __sendFapCmd(FAP_IOC_TM, &tm);
    if(ret < 0)
    {
        return FAP_ERROR;
    }

    *dropProbability_p = tm.dropProbability;
    *minThreshold_p = tm.minThreshold;
    *maxThreshold_p = tm.maxThreshold;

    return FAP_SUCCESS;
}

/*******************************************************************************
 *
 * Function: fapCtlTm_queueDropAlgConfig
 *
 * Configures the drop algorithm, queueProfile indices of the
 * specified port, mode, and queue.
 *
 *******************************************************************************/
int fapCtlTm_queueDropAlgConfig(char *ifName, int queue, fapIoctl_tmDropAlg_t dropAlgorithm,
                                int queueProfileIdLo, int queueProfileIdHi,
                                uint32_t priorityMask0, uint32_t priorityMask1)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, queue %u, dropAlgorithm %d, queueProfileIdLo %d, "
                 "queueProfileIdHi %d, priorityMask0 0x%x, priorityMask1 0x%x",
                 ifName, queue, dropAlgorithm, queueProfileIdLo, queueProfileIdHi,
                 priorityMask0, priorityMask1);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_DROP_ALG_CONFIG;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.queue = queue;
    tm.dropAlgorithm = dropAlgorithm;
    tm.queueProfileId = queueProfileIdLo;
    tm.queueProfileIdHi = queueProfileIdHi;
    tm.priorityMask0 = priorityMask0;
    tm.priorityMask1 = priorityMask1;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_queueDropAlgConfigExt
 *
 * Configures the drop algorithm of a specified port and queue in another way.
 *
 *******************************************************************************/
int fapCtlTm_queueDropAlgConfigExt(char *ifName, int queue, fapIoctl_tmDropAlg_t dropAlgorithm,
                                   int dropProbabilityLo, int minThresholdLo, int maxThresholdLo,
                                   int dropProbabilityHi, int minThresholdHi, int maxThresholdHi,
                                   uint32_t priorityMask0, uint32_t priorityMask1)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, queue %u, dropAlgorithm %d, "
                 "dropProbabilityLo %d, minThresholdLo %d, maxThresholdLo %d, "
                 "dropProbabilityHi %d, minThresholdHi %d, maxThresholdHi %d. "
                 "priorityMask0 0x%x, priorityMask1 0x%x",
                 ifName, queue, dropAlgorithm,
                 dropProbabilityLo, minThresholdLo, maxThresholdLo,
                 dropProbabilityHi, minThresholdHi, maxThresholdHi,
                 priorityMask0, priorityMask1);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_DROP_ALG_CONFIG_EXT;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.queue = queue;
    tm.dropAlgorithm = dropAlgorithm;
    tm.dropProbability = dropProbabilityLo;
    tm.minThreshold = minThresholdLo;
    tm.maxThreshold = maxThresholdLo;
    tm.dropProbabilityHi = dropProbabilityHi;
    tm.minThresholdHi = minThresholdHi;
    tm.maxThresholdHi = maxThresholdHi;
    tm.priorityMask0 = priorityMask0;
    tm.priorityMask1 = priorityMask1;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_getQueueDropAlgConfig
 *
 * Returns the drop algorithm, queue profile indices of the
 * specified port, mode, and queue.
 *
 *******************************************************************************/
int fapCtlTm_getQueueDropAlgConfig(char *ifName, int queue, fapIoctl_tmDropAlg_t *dropAlgorithm_p,
                                   int *queueProfileIdLo_p, int *queueProfileIdHi_p,
                                   uint32_t *priorityMask0_p, uint32_t *priorityMask1_p)
{
    fapIoctl_tm_t tm;
    int ret;

    fapCtl_debug("ifName %s, queue %d", ifName, queue);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_GET_QUEUE_DROP_ALG_CONFIG;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.queue = queue;

    ret = __sendFapCmd(FAP_IOC_TM, &tm);
    if(ret < 0)
    {
        return FAP_ERROR;
    }

    *dropAlgorithm_p = tm.dropAlgorithm;
    *queueProfileIdLo_p = tm.queueProfileId;
    *queueProfileIdHi_p = tm.queueProfileIdHi;
    *priorityMask0_p = tm.priorityMask0;
    *priorityMask1_p = tm.priorityMask1;

    return FAP_SUCCESS;
}

/*******************************************************************************
 *
 * Function: fapCtlTm_xtmChannelDropAlgConfig
 *
 * Configures the drop algorithm, queueProfile indices of the
 * specified XTM channel
 *
 *******************************************************************************/
int fapCtlTm_xtmChannelDropAlgConfig(int channel, fapIoctl_tmDropAlg_t dropAlgorithm,
                                     int queueProfileIdLo, int queueProfileIdHi,
                                     uint32_t priorityMask0, uint32_t priorityMask1)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("channel %u, dropAlgorithm %d, queueProfileIdLo %d, "
                 "queueProfileIdHi %d, priorityMask0 0x%x, priorityMask1 0x%x",
                 channel, dropAlgorithm, queueProfileIdLo, queueProfileIdHi,
                 priorityMask0, priorityMask1);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_XTM_QUEUE_DROP_ALG_CONFIG;
    tm.channel = channel;
    tm.dropAlgorithm = dropAlgorithm;
    tm.queueProfileId = queueProfileIdLo;
    tm.queueProfileIdHi = queueProfileIdHi;
    tm.priorityMask0 = priorityMask0;
    tm.priorityMask1 = priorityMask1;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_xtmChannelDropAlgConfigExt
 *
 * Configures the drop algorithm of a specified XTM channel in another way.
 *
 *******************************************************************************/
int fapCtlTm_xtmChannelDropAlgConfigExt(int channel, fapIoctl_tmDropAlg_t dropAlgorithm,
                                        int dropProbabilityLo, int minThresholdLo, int maxThresholdLo,
                                        int dropProbabilityHi, int minThresholdHi, int maxThresholdHi)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("channel %u, dropAlgorithm %d, queueProfileIdLo %d, "
                 "dropProbabilityLo %d, minThresholdLo %d, maxThresholdLo %d, "
                 "dropProbabilityHi %d, minThresholdHi %d, maxThresholdHi %d.\n",
                 chnl, dropAlgorithm,
                 dropProbabilityLo, minThresholdLo, maxThresholdLo,
                 dropProbabilityHi, minThresholdHi, maxThresholdHi);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_XTM_QUEUE_DROP_ALG_CONFIG_EXT;
    tm.channel = channel;
    tm.dropAlgorithm = dropAlgorithm;
    tm.dropProbability = dropProbabilityLo;
    tm.minThreshold = minThresholdLo;
    tm.maxThreshold = maxThresholdLo;
    tm.dropProbabilityHi = dropProbabilityHi;
    tm.minThresholdHi = minThresholdHi;
    tm.maxThresholdHi = maxThresholdHi;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_getXtmChannelDropAlgConfig
 *
 * Returns the drop algorithm, queue profile indices of the
 * specified XTM channel
 *
 *******************************************************************************/
int fapCtlTm_getXtmChannelDropAlgConfig(int channel, fapIoctl_tmDropAlg_t *dropAlgorithm_p,
                                        int *queueProfileIdLo_p, int *queueProfileIdHi_p,
                                        uint32_t *priorityMask0_p, uint32_t *priorityMask1_p)
{
    fapIoctl_tm_t tm;
    int ret;

    fapCtl_debug("channel %d", channel);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_GET_XTM_QUEUE_DROP_ALG_CONFIG;
    tm.channel = channel;

    ret = __sendFapCmd(FAP_IOC_TM, &tm);
    if(ret < 0)
    {
        return FAP_ERROR;
    }

    *dropAlgorithm_p = tm.dropAlgorithm;
    *queueProfileIdLo_p = tm.queueProfileId;
    *queueProfileIdHi_p = tm.queueProfileIdHi;
    *priorityMask0_p = tm.priorityMask0;
    *priorityMask1_p = tm.priorityMask1;

    return FAP_SUCCESS;
}

/*******************************************************************************
 *
 * Function: fapCtlTm_getQueueStats
 *
 * Returns the transmitted packets, transmitted bytes, dropped packets,
 * and dropped bytes of the specified port, mode, and queue.
 *
 *******************************************************************************/
int fapCtlTm_getQueueStats(char *ifName, fapIoctl_tmMode_t mode, int queue,
                           uint32_t *txPackets_p, uint32_t *txBytes_p, uint32_t *droppedPackets_p, uint32_t *droppedBytes_p)
{
    fapIoctl_tm_t tm;
    int ret;

    fapCtl_debug("ifName %s, mode %u, queue %d", ifName, mode, queue);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_GET_QUEUE_STATS;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;
    tm.queue = queue;

    ret = __sendFapCmd(FAP_IOC_TM, &tm);
    if(ret < 0)
    {
        return FAP_ERROR;
    }

    *txPackets_p = tm.queueStats.txPackets;
    *txBytes_p = tm.queueStats.txBytes;
    *droppedPackets_p = tm.queueStats.droppedPackets;
    *droppedBytes_p = tm.queueStats.droppedBytes;

    return FAP_SUCCESS;
}

/*******************************************************************************
 *
 * Function: fapCtlTm_setQueueWeight
 *
 * Configures the given Queue's weight.
 *
 * This API does not apply the specified settings into the FAP. This can only
 * be done via the fapCtlTm_apply API.
 *
 *******************************************************************************/
int fapCtlTm_setQueueWeight(char *ifName, fapIoctl_tmMode_t mode, int queue, int weight)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, mode %u, queue %u, weight %d",
                 ifName, mode, queue, weight);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_WEIGHT;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;
    tm.queue = queue;
    tm.weight = weight;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_arbiterConfig
 *
 * Configures the Arbiter type of the given port to SP, WRR, SP+WRR, or WFQ.
 * arbiterArg is a generic argument that is passed to arbiters. Currently it is
 * only used in SP+WRR, where it indicates the lowest priority queue in the
 * SP Tier.
 *
 * This API does not apply the corresponding mode settings into the FAP. This
 * can only be done via the fapCtlTm_apply API.
 *
 *******************************************************************************/
int fapCtlTm_arbiterConfig(char *ifName, fapIoctl_tmMode_t mode,
                           fapIoctl_tmArbiterType_t arbiterType, int arbiterArg)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, arbiterType %u, arbiterArg %u",
                 ifName, arbiterType, arbiterArg);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_ARBITER_CONFIG;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;
    tm.arbiterType = arbiterType;
    tm.arbiterArg = arbiterArg;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_getArbiterConfig
 *
 * Returns the Arbiter type of the given port and mode.
 * arbiterArg is a generic argument that is passed to arbiters.
 * Currently it is only used in SP+WRR, where it indicates the lowest 
 * priority queue in the SP Tier.
 *
 *******************************************************************************/
int fapCtlTm_getArbiterConfig(char *ifName, fapIoctl_tmMode_t mode,
                              fapIoctl_tmArbiterType_t *arbiterType_p, int *arbiterArg_p)
{
    fapIoctl_tm_t tm;
    int ret;

    fapCtl_debug("ifName %s, mode %u", ifName, mode);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_GET_ARBITER_CONFIG;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;

    ret = __sendFapCmd(FAP_IOC_TM, &tm);
    if(ret < 0)
    {
        return FAP_ERROR;
    }

    *arbiterType_p = tm.arbiterType;
    *arbiterArg_p = tm.arbiterArg;

    return FAP_SUCCESS;
}

/*******************************************************************************
 *
 * Function: fapCtlTm_setPortMode
 *
 * Sets the mode of the given port to AUTO or MANUAL.
 *
 * This API does not apply the corresponding mode settings into the FAP. This
 * can only be done via the fapCtlTm_apply API.
 *
 *******************************************************************************/
int fapCtlTm_setPortMode(char *ifName, fapIoctl_tmMode_t mode)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, mode %u", ifName, mode);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_PORT_MODE;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_modeReset
 *
 * Resets the current configuration of the given port and mode.
 *
 * This API does not apply the corresponding mode settings into the FAP. This
 * can only be done via the fapCtlTm_apply API.
 *
 *******************************************************************************/
int fapCtlTm_modeReset(char *ifName, fapIoctl_tmMode_t mode)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, mode %u", ifName, mode);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_MODE_RESET;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_setPortType
 *
 * Sets the type of the given port to LAN or WAN.
 *
 * This API does not apply the corresponding mode settings into the FAP. This
 * can only be done via the fapCtlTm_apply API.
 *
 *******************************************************************************/
int fapCtlTm_setPortType(char *ifName, fapIoctl_tmPortType_t portType)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, portType %u", ifName, portType);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_PORT_TYPE;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.portType = portType;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_portEnable
 *
 * Enables or Disables the given port and mode.
 *
 * This API does not apply the corresponding mode settings into the FAP. This
 * can only be done via the fapTm_apply API.
 *
 *******************************************************************************/
int fapCtlTm_portEnable(char *ifName, fapIoctl_tmMode_t mode, int enable)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, mode %u, enable %u", ifName, mode, enable);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_PORT_ENABLE;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.mode = mode;
    tm.enable = enable;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_apply
 *
 * Applies the settings corresponding to the current mode of the specified port
 * into the FAP.
 *
 * This API also allows enabling or disabling the specified port.
 *
 *******************************************************************************/
int fapCtlTm_apply(char *ifName)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s", ifName);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_PORT_APPLY;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_mapTmQueueToSwQueue
 *
 * Maps a FAP TM queue to a HW Switch Queue, on a per-port basis.
 *
 * This API applies the configuration immediately to the FAP(s).
 *
 *******************************************************************************/
int fapCtlTm_mapTmQueueToSwQueue(char *ifName, int queue, int swQueue)
{
    fapIoctl_tm_t tm;

    fapCtl_debug("ifName %s, queue %u, swQueue %u", ifName, queue, swQueue);

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_MAP_TMQUEUE_TO_SWQUEUE;
    tm.port = __getPortNumber(ifName);
    if(tm.port < 0)
    {
        fapCtl_error("Invalid ifName %s\n", ifName);
        return FAP_ERROR;
    }
    tm.queue = queue;
    tm.swQueue = swQueue;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_status
 *
 * Prints the status of all ENABLED ports.
 *
 *******************************************************************************/
int fapCtlTm_status(void)
{
    fapIoctl_tm_t tm;

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_STATUS;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_stats
 *
 * Prints the statistics counters of all ENABLED ports.
 *
 *******************************************************************************/
int fapCtlTm_stats(char *ifName)
{
    fapIoctl_tm_t tm;

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_STATS;

    if(FAPCTL_IS_IFNAME_VALID(ifName))
    {
        tm.port = __getPortNumber(ifName);
        if(tm.port < 0)
        {
            fapCtl_error("Invalid ifName %s\n", ifName);
            return FAP_ERROR;
        }
    }
    else
    {
        tm.port = -1;
    }

    return __sendFapCmd(FAP_IOC_TM, &tm);
}

/*******************************************************************************
 *
 * Function: fapCtlTm_dumpMaps
 *
 * Prints the Port to Scheduler and Priority to Queue mappings.
 *
 *******************************************************************************/
int fapCtlTm_dumpMaps(void)
{
    fapIoctl_tm_t tm;

    memset(&tm, 0, sizeof(fapIoctl_tm_t));

    tm.cmd = FAP_IOCTL_TM_CMD_DUMP_MAPS;

    return __sendFapCmd(FAP_IOC_TM, &tm);
}
