/***********************************************************************
 *
 *  Copyright (c) 2013 Broadcom Corporation
 *  All Rights Reserved
 *
 *
<:label-BRCM:2013:proprietary:standard

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
 * File Name  : fbctl.c
 * Description: Linux ioctl library that controls the Broadcom
 *              Flow Bond.
 ***************************************************************************/

/*** Includes. ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include <fbond.h>
#include <fbctl_api.h>

static int  fbDevOpen(void);
static int  fbDevIoctl(FbondIoctl_t ioctl, int arg);
static int  fbDevIoctl2(FbondIoctl_t ioctl, int arg1, int arg2);

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlStatus
 * Description  : Displays flow bond status.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlStatus(void)
{
    return fbDevIoctl2( FBOND_IOCTL_STATUS, 0, 0 );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlEnable
 * Description  : Enables flow bond.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlEnable(void)
{
    return fbDevIoctl2( FBOND_IOCTL_ENABLE, 0, 0 );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlDisable
 * Description  : Disables flow bond.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlDisable(void)
{
    return fbDevIoctl2( FBOND_IOCTL_DISABLE, 0, 0 );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlInterval
 * Description  : Sets the interval in msec when flows inactive are cleared
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlInterval(unsigned int interval)
{
   return fbDevIoctl( FBOND_IOCTL_INTERVAL, interval);
}
/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlAddIf
 * Description  : Adds an interface to flow bond.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlAddIf(unsigned int groupindex, unsigned int ifindex)
{
   FbondIoctlAddDeleteIf_t addIfArgs;

   addIfArgs.groupindex = groupindex;
   addIfArgs.ifindex = ifindex;
   return fbDevIoctl( FBOND_IOCTL_ADDIF, (int) &addIfArgs );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlDeleteIf
 * Description  : Adds an interface to flow bond.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlDeleteIf(unsigned int groupindex, unsigned int ifindex)
{
   FbondIoctlAddDeleteIf_t deleteIfArgs;

   deleteIfArgs.groupindex = groupindex;
   deleteIfArgs.ifindex = ifindex;
   return fbDevIoctl( FBOND_IOCTL_DELETEIF, (int) &deleteIfArgs );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlTokens
 * Description  : Sets number of tokens for an interface in flow bond.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlTokens(unsigned int groupindex, unsigned int ifindex,
                unsigned int tokens, unsigned int max_tokens)
{
   FbondIoctlTokens_t tokensArgs;

   tokensArgs.groupindex = groupindex;
   tokensArgs.ifindex = ifindex;
   tokensArgs.tokens = tokens;
   tokensArgs.max_tokens = max_tokens;
   return fbDevIoctl( FBOND_IOCTL_TOKENS, (int) &tokensArgs );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlTest
 * Description  : Randomly assign flows and prints status
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlTest(unsigned int num_flows)
{
    return fbDevIoctl( FBOND_IOCTL_TEST, num_flows);
}

#if defined(CC_CONFIG_FBOND_DEBUG)

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlDebug
 * Description  : Sets the debug level for the layer in flow bond.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int  fbCtlDebug(int layer, int level)
{
    if ( layer >= FBOND_DBG_LAYER_MAX )
        fprintf( stderr, "invalid layer <%d>\n", layer );

    return fbDevIoctl2( FBOND_IOCTL_DEBUG, layer, level );
}
#endif

/*
 *------------------------------------------------------------------------------
 * Function Name: fbDevOpen
 * Description  : Opens the flow bond device.
 * Returns      : device handle if successsful or -1 if error
 *------------------------------------------------------------------------------
 */
static int fbDevOpen(void)
{
    int nFd = open( FBOND_DRV_DEVICE_NAME, O_RDWR );
    if ( nFd == -1 )
    {
        fprintf( stderr, "open <%s> error no %d\n",
                 FBOND_DRV_DEVICE_NAME, errno );
        return FBOND_ERROR;
    }
    return nFd;
} /* fbDevOpen */

/*
 *------------------------------------------------------------------------------
 * Function Name: fbDevIoctl
 * Description  : Ioctls into fbond driver passing the IOCTL command
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int fbDevIoctl(FbondIoctl_t ioctl_cmd, int arg)
{
    int devFd, ret = FBOND_ERROR;

    if ( ( devFd = fbDevOpen() ) == FBOND_ERROR )
        return FBOND_ERROR;

    ret = ioctl( devFd, ioctl_cmd, arg );

    close( devFd );
    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbDevIoctl2
 * Description  : Ioctls into fbond driver passing the IOCTL command, and the
 *                fbond (arg1) and id (arg2) passed as a 16bit tuple.
 *                If arg1 == -1, then arg2 is passed directly.
 *                CAUTION: Display is done in kernel context.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int fbDevIoctl2(FbondIoctl_t ioctl_cmd, int arg1, int arg2)
{
    int arg = 0;

    if ( arg1 == -1 )
        arg = arg2;
    else
        arg = ( ( (arg1 & 0xFF) << 8) | (arg2 & 0xFF) ) ;

    return fbDevIoctl( ioctl_cmd, arg );
}

