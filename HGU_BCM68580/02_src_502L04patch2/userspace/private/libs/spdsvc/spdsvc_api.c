/***********************************************************************
 *
 *  Copyright (c) 2015  Broadcom Corporation
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

#include "spdsvc_api.h"

/*
 * Macros
 */

//#define CC_SPDSVC_API_DEBUG

#if defined(CC_SPDSVC_API_DEBUG)
#define spdSvcApi_debug(fmt, arg...) printf("%s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#else
#define spdSvcApi_debug(fmt, arg...)
#endif

#define spdSvcApi_error(fmt, arg...) printf("ERROR[%s.%u]: " fmt, __FUNCTION__, __LINE__, ##arg)

/*
 * Local functions
 */

static int __sendSpdSvcIoctl(spdsvc_ioctl_t ioctlCmd, spdsvc_ioctl_arg_t *arg_p)
{
    int ret;
    int fd;

    fd = open(SPDSVC_DRV_DEVICE_NAME, O_RDWR);
    if(fd < 0)
    {
        spdSvcApi_error("%s: %s\n", SPDSVC_DRV_DEVICE_NAME, strerror(errno));

        return -EINVAL;
    }

    ret = ioctl(fd, ioctlCmd, (uint32_t)arg_p);
    if(ret)
    {
        spdSvcApi_error("%s\n", strerror(errno));
    }

    close(fd);

    return ret;
}


/*
 * Public functions
 */

int spdsvc_enable(spdsvc_config_t *config_p)
{
    spdsvc_ioctl_arg_t spdsvc;

    spdsvc.config = *config_p;

    return __sendSpdSvcIoctl(SPDSVC_IOCTL_ENABLE, &spdsvc);
}

int spdsvc_getOverhead(uint32_t *overhead_p)
{
    spdsvc_ioctl_arg_t spdsvc;
    int ret;

    ret = __sendSpdSvcIoctl(SPDSVC_IOCTL_GET_OVERHEAD, &spdsvc);
    if(ret)
    {
        return ret;
    }

    *overhead_p = spdsvc.overhead;

    return 0;
}

int spdsvc_getResult(spdsvc_result_t *result_p)
{
    spdsvc_ioctl_arg_t spdsvc;
    int ret;

    ret = __sendSpdSvcIoctl(SPDSVC_IOCTL_GET_RESULT, &spdsvc);
    if(ret)
    {
        return ret;
    }

    *result_p = spdsvc.result;

    spdSvcApi_debug("\n\tSPDSVC API: running %u, rx_packets %u, rx_bytes %u, tx_packets %u, tx_discards %u\n",
                    spdsvc.result.running, spdsvc.result.rx_packets, spdsvc.result.rx_bytes,
                    spdsvc.result.tx_packets, spdsvc.result.tx_discards);
    return 0;
}

int spdsvc_disable(void)
{
    spdsvc_ioctl_arg_t spdsvc;

    return __sendSpdSvcIoctl(SPDSVC_IOCTL_DISABLE, &spdsvc);
}
