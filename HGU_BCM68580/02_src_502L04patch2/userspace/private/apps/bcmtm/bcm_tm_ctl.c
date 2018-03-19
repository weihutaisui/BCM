/*
<:copyright-BRCM:2015:proprietary:standard

   Copyright (c) 2015 Broadcom 
   All Rights Reserved

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
*/


/*
 *******************************************************************************
 * File Name  : bcm_tm_ctl.c
 * Description: Command line parsing for the Broadcom Traffic Manager
 *              Control Utility
 *******************************************************************************
 */

#include <bcm_tm_ctl.h>
#include <ctype.h>
#include <net/if.h>

/*** Auxiliary functions ***/

static char g_PgmName[128];

/*
 *------------------------------------------------------------------------------
 * Function Name: bcmTmHandler
 * Description  : Processes the BCM Traffic Manager commands
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int bcmTmHandler(POPTION_INFO pOptions, int nNumOptions, bcmTmIoctl_cmd_t cmd)
{
    char ifName[IFNAMSIZ] = BCM_TM_API_IFNAME_INVALID_STR;
    bcmTmDrv_arg_t tm;
    int nRet = BCM_TM_SUCCESS;

    memset(&tm, BCM_TM_DONT_CARE, sizeof(bcmTmDrv_arg_t));

    tm.shapingType = BCM_TM_DRV_SHAPING_TYPE_DISABLED;

    /* Parse Options */

//    fprintf( stderr, "cmd %u, nNumOptions %u\n", cmd, nNumOptions );

    while (nNumOptions)
    {
//        fprintf( stderr, "%s, %d\n", pOptions->pOptName, pOptions->nNumParms );

        if ( strcmp( pOptions->pOptName, "--if" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the interface name\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            strncpy(ifName, pOptions->pParms[0], IFNAMSIZ);
        }
        else if ( strcmp( pOptions->pOptName, "--queue" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the queue number\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.queue = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--channel" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the channel number\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.channel = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--auto" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --auto does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.mode = BCM_TM_DRV_MODE_AUTO;
        }
        else if ( strcmp( pOptions->pOptName, "--manual" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --manual does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.mode = BCM_TM_DRV_MODE_MANUAL;
        }
        else if ( strcmp( pOptions->pOptName, "--min" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --min does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.shaperType = BCM_TM_DRV_SHAPER_TYPE_MIN;
        }
        else if ( strcmp( pOptions->pOptName, "--max" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --max does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.shaperType = BCM_TM_DRV_SHAPER_TYPE_MAX;
        }
        else if ( strcmp( pOptions->pOptName, "--kbps" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the bandwidth in kbps\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.kbps = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--mbs" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify MBS in bytes\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.mbs = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--cap" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify cap\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.nbrOfEntriesCap = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--weight" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify weight\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.weight = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--sp" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --sp does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.arbiterType = BCM_TM_DRV_ARBITER_TYPE_SP;
        }
        else if ( strcmp( pOptions->pOptName, "--wrr" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --wrr does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.arbiterType = BCM_TM_DRV_ARBITER_TYPE_WRR;
        }
        else if ( strcmp( pOptions->pOptName, "--spwrr" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --spwrr does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.arbiterType = BCM_TM_DRV_ARBITER_TYPE_SP_WRR;
        }
        else if ( strcmp( pOptions->pOptName, "--wfq" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --wfq does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.arbiterType = BCM_TM_DRV_ARBITER_TYPE_WFQ;
        }
        else if ( strcmp( pOptions->pOptName, "--lowprioq" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify lowprioq\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.arbiterArg = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--rate" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --rate does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.shapingType = BCM_TM_DRV_SHAPING_TYPE_RATE;
        }
        else if ( strcmp( pOptions->pOptName, "--ratio" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --ratio does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.shapingType = BCM_TM_DRV_SHAPING_TYPE_RATIO;
        }
        else if ( strcmp( pOptions->pOptName, "--queueprofileid" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify queueprofileid\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.queueProfileId = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--dropprob" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify dropprob\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.dropProbability = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--minthreshold" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify minthreshold\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.minThreshold = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--maxthreshold" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify maxthreshold\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.maxThreshold = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--dt" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --dt does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.dropAlgorithm = BCM_TM_DRV_DROP_ALG_DT;
        }
        else if ( strcmp( pOptions->pOptName, "--red" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --red does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.dropAlgorithm = BCM_TM_DRV_DROP_ALG_RED;
        }
        else if ( strcmp( pOptions->pOptName, "--wred" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --wred does not take parameters\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.dropAlgorithm = BCM_TM_DRV_DROP_ALG_WRED;
        }
        else if ( strcmp( pOptions->pOptName, "--queueprofileidhi" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify queueprofileidhi\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.queueProfileIdHi = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--prioritymask0" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify prioritymask0\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.priorityMask0 = (uint32_t)strtoul( pOptions->pParms[0] , NULL, 0 );
        }
        else if ( strcmp( pOptions->pOptName, "--prioritymask1" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify prioritymask1\n", g_PgmName );
                return BCM_TM_INVALID_OPTION;
            }

            tm.priorityMask1 = (uint32_t)strtoul( pOptions->pParms[0] , NULL, 0 );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n", g_PgmName, pOptions->pOptName );
            return BCM_TM_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    /* Call API */

    switch(cmd)
    {
        case BCM_TM_IOCTL_MASTER_ON:
            nRet = bcmTmApi_masterConfig(1);
            break;

        case BCM_TM_IOCTL_MASTER_OFF:
            nRet = bcmTmApi_masterConfig(0);
            break;

        case BCM_TM_IOCTL_PORT_CONFIG:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode) ||
               BCM_TM_IS_DONT_CARE(tm.kbps) || BCM_TM_IS_DONT_CARE(tm.mbs))
            {
                fprintf( stderr, "Missing Arguments: TM Port Config\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_portConfig(ifName, tm.mode, tm.kbps,
                                       tm.mbs, tm.shapingType);
            break;

        case BCM_TM_IOCTL_PORT_MODE:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode))
            {
                fprintf( stderr, "Missing Arguments: TM Port Mode\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_setPortMode(ifName, tm.mode);
            break;

        case BCM_TM_IOCTL_MODE_RESET:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode))
            {
                fprintf( stderr, "Missing Arguments: TM Port Reset\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_modeReset(ifName, tm.mode);
            break;

        case BCM_TM_IOCTL_PORT_ENABLE:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode))
            {
                fprintf( stderr, "Missing Arguments: TM Port Enable\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_portEnable(ifName, tm.mode, 1);
            break;

        case BCM_TM_IOCTL_PORT_DISABLE:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode))
            {
                fprintf( stderr, "Missing Arguments: TM Port Enable\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_portEnable(ifName, tm.mode, 0);
            break;

        case BCM_TM_IOCTL_PORT_APPLY:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName))
            {
                fprintf( stderr, "Missing Arguments: TM Port Apply\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_apply(ifName);
            break;

        case BCM_TM_IOCTL_QUEUE_CONFIG:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode) ||
               BCM_TM_IS_DONT_CARE(tm.queue) || BCM_TM_IS_DONT_CARE(tm.shaperType) ||
               BCM_TM_IS_DONT_CARE(tm.kbps) || BCM_TM_IS_DONT_CARE(tm.mbs))
            {
                fprintf( stderr, "Missing Arguments: TM Queue Config\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_queueConfig(ifName, tm.mode, tm.queue,
                                        tm.shaperType, tm.kbps, tm.mbs);
            break;

        case BCM_TM_IOCTL_QUEUE_UNCONFIG:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode) ||
               BCM_TM_IS_DONT_CARE(tm.queue))
            {
                fprintf( stderr, "Missing Arguments: TM Queue Unconfig\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_queueUnconfig(ifName, tm.mode, tm.queue);
            break;

        case BCM_TM_IOCTL_SET_QUEUE_CAP:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode) ||
               BCM_TM_IS_DONT_CARE(tm.queue) || BCM_TM_IS_DONT_CARE(tm.nbrOfEntriesCap))
            {
                fprintf( stderr, "Missing Arguments: TM Queue Set Cap\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_setQueueCap(ifName, tm.mode, tm.queue,
                                        tm.nbrOfEntriesCap);
            break;

        case BCM_TM_IOCTL_QUEUE_WEIGHT:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode) ||
               BCM_TM_IS_DONT_CARE(tm.queue) || BCM_TM_IS_DONT_CARE(tm.weight))
            {
                fprintf( stderr, "Missing Arguments: TM Queue Set Weight\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_setQueueWeight(ifName, tm.mode, tm.queue, tm.weight);
            break;

        case BCM_TM_IOCTL_ARBITER_CONFIG:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.mode) ||
               BCM_TM_IS_DONT_CARE(tm.arbiterType) ||
               (tm.arbiterType == BCM_TM_DRV_ARBITER_TYPE_SP_WRR &&
                BCM_TM_IS_DONT_CARE(tm.arbiterArg)))
            {
                fprintf( stderr, "Missing Arguments: TM Arbiter Configuration\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_arbiterConfig(ifName, tm.mode,
                                          tm.arbiterType, tm.arbiterArg);
            break;

        case BCM_TM_IOCTL_STATUS:
            bcmTmApi_status();
            break;

        case BCM_TM_IOCTL_STATS:
            bcmTmApi_stats(ifName);
            break;

        case BCM_TM_IOCTL_QUEUE_PROFILE_CONFIG:
            if(BCM_TM_IS_DONT_CARE(tm.queueProfileId) ||
               BCM_TM_IS_DONT_CARE(tm.dropProbability) ||
               BCM_TM_IS_DONT_CARE(tm.minThreshold) ||
               BCM_TM_IS_DONT_CARE(tm.maxThreshold))
            {
                fprintf( stderr, "Missing Arguments: TM Queue DropAlg Config\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_queueProfileConfig(tm.queueProfileId, tm.dropProbability,
                                               tm.minThreshold, tm.maxThreshold);
            break;

        case BCM_TM_IOCTL_QUEUE_DROP_ALG_CONFIG:
            if(!BCM_TM_API_IS_IFNAME_VALID(ifName) || BCM_TM_IS_DONT_CARE(tm.queue) ||
               BCM_TM_IS_DONT_CARE(tm.dropAlgorithm))
            {
                fprintf( stderr, "Missing Arguments: TM Queue DropAlg Config\n");
                return BCM_TM_INVALID_OPTION;
            }
            if ((tm.dropAlgorithm == BCM_TM_DRV_DROP_ALG_RED) &&
                BCM_TM_IS_DONT_CARE(tm.queueProfileId))
            {
                fprintf( stderr, "Missing Arguments: TM Queue DropAlg Config\n");
                return BCM_TM_INVALID_OPTION;
            }
            if ((tm.dropAlgorithm == BCM_TM_DRV_DROP_ALG_WRED) &&
                (BCM_TM_IS_DONT_CARE(tm.queueProfileId) || BCM_TM_IS_DONT_CARE(tm.queueProfileIdHi)))
            {
                fprintf( stderr, "Missing Arguments: TM Queue DropAlg Config\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_queueDropAlgConfig(ifName, tm.queue,
                                               tm.dropAlgorithm, tm.queueProfileId,
                                               tm.queueProfileIdHi,
                                               tm.priorityMask0,
                                               tm.priorityMask1);
            break;

        case BCM_TM_IOCTL_XTM_QUEUE_DROP_ALG_CONFIG:
            if(BCM_TM_IS_DONT_CARE(tm.channel) || BCM_TM_IS_DONT_CARE(tm.dropAlgorithm))
            {
                fprintf( stderr, "Missing Arguments: TM XTM Channel DropAlg Config\n");
                return BCM_TM_INVALID_OPTION;
            }
            if ((tm.dropAlgorithm == BCM_TM_DRV_DROP_ALG_RED) &&
                BCM_TM_IS_DONT_CARE(tm.queueProfileId))
            {
                fprintf( stderr, "Missing Arguments: TM XTM Channel DropAlg Config\n");
                return BCM_TM_INVALID_OPTION;
            }
            if ((tm.dropAlgorithm == BCM_TM_DRV_DROP_ALG_WRED) &&
                (BCM_TM_IS_DONT_CARE(tm.queueProfileId) || BCM_TM_IS_DONT_CARE(tm.queueProfileIdHi)))
            {
                fprintf( stderr, "Missing Arguments: TM XTM Channel DropAlg Config\n");
                return BCM_TM_INVALID_OPTION;
            }
            nRet = bcmTmApi_xtmChannelDropAlgConfig(tm.channel,
                                                    tm.dropAlgorithm, tm.queueProfileId,
                                                    tm.queueProfileIdHi,
                                                    tm.priorityMask0,
                                                    tm.priorityMask1);
            break;

        default:
            fprintf( stderr, "Missing TM Command");
            return BCM_TM_INVALID_OPTION;
    }

    if ( nRet == BCM_TM_ERROR )
    {
        fprintf( stderr, "%s: BCM TM Command <%d> failed\n", g_PgmName, cmd );
    }

    return nRet;
}

int onHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_MASTER_ON);
}

int offHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_MASTER_OFF);
}

int ifcfgHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_PORT_CONFIG);
}

int queuecfgHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_QUEUE_CONFIG);
}

int queueCapHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_SET_QUEUE_CAP);
}

int queueweightHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_QUEUE_WEIGHT);
}

int arbitercfgHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_ARBITER_CONFIG);
}

int modeHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_PORT_MODE);
}

int applyHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_PORT_APPLY);
}

int statusHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_STATUS);
}

int statsHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_STATS);
}

int enableHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_PORT_ENABLE);
}

int disableHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_PORT_DISABLE);
}

int resetHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_MODE_RESET);
}

int queueprofileHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_QUEUE_PROFILE_CONFIG);
}

int queuedropalgHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_QUEUE_DROP_ALG_CONFIG);
}

int xtmchanneldropalgHandler(POPTION_INFO pOptions, int nNumOptions)
{
    return bcmTmHandler(pOptions, nNumOptions, BCM_TM_IOCTL_XTM_QUEUE_DROP_ALG_CONFIG);
}

/*** File Scoped Globals. ***/

static COMMAND_INFO g_Cmds[] =
{
    {"stats",              {"--if", ""}, statsHandler},
    {"status",             {""}, statusHandler},
    {"on",                 {""}, onHandler},
    {"off",                {""}, offHandler},
    {"ifcfg",              {"--if", "--auto", "--manual", "--kbps", "--mbs", "--rate", "--ratio", ""}, ifcfgHandler},
    {"mode",               {"--if", "--auto", "--manual", ""}, modeHandler},
    {"reset",              {"--if", "--auto", "--manual", ""}, resetHandler},
    {"enable",             {"--if", "--auto", "--manual", ""}, enableHandler},
    {"disable",            {"--if", "--auto", "--manual", ""}, disableHandler},
    {"apply",              {"--if", ""}, applyHandler},
    {"queuecfg",           {"--if", "--auto", "--manual", "--queue", "--min", "--max", "--kbps", "--mbs", ""}, queuecfgHandler},
    {"queuecap",           {"--if", "--auto", "--manual", "--queue", "--cap", ""}, queueCapHandler},
    {"queueweight",        {"--if", "--auto", "--manual", "--queue", "--weight", ""}, queueweightHandler},
    {"queueprofile",       {"--queueprofileid", "--dropprob", "--minthreshold", "--maxthreshold", ""}, queueprofileHandler},
    {"queuedropalg",       {"--if", "--queue", "--dt", "--red", "--wred", "--queueprofileid", "--queueprofileidhi",
                            "--prioritymask0", "--prioritymask1", ""}, queuedropalgHandler},
    {"xtmchanneldropalg",  {"--channel", "--dt", "--red", "--wred", "--queueprofileid", "--queueprofileidhi",
                            "--prioritymask0", "--prioritymask1", ""}, xtmchanneldropalgHandler},
    {"arbitercfg",         {"--if", "--auto", "--manual", "--sp", "--wrr", "--spwrr", "--wfq", "--lowprioq", ""}, arbitercfgHandler},
    {""}
};

/*
 *------------------------------------------------------------------------------
 * Function Name: bcmTmCliHelp
 * Description  : Displays the BCM TM CLI help
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
void bcmTmCliHelp(void)
{
    printf(
        BCM_TM_MODNAME " Control Utility:\n\n"

        "::::::  Usage  ::::::\n\n"

        "      bcmtmctl on\n"
        "               off\n"
        "               status\n"
        "               stats [ (--if <ifName>) ]\n"
        "               mode [ --if <ifName> [--auto | --manual] ]\n"
        "               reset [ --if <ifName> [--auto | --manual] ]\n"
        "               enable [ --if <ifName> [--auto | --manual] ]\n"
        "               disable [ --if <ifName> [--auto | --manual] ]\n"
        "               apply [ --if <ifName> ]\n"
        "               ifcfg [ --if <ifName> [--auto | --manual] --kbps <kbps> --mbs <bytes> (--rate | --ratio) ]\n"
        "               arbitercfg [ --if <ifName> [--auto | --manual] [--sp | --wrr | --spwrr | --wfq ] (--lowprioq) ]\n"
        "               queuecfg [ --if <ifName> [--auto | --manual] --queue <queue> "
        "[--min | --max] --kbps <kbps> --mbs <bytes> ]\n"
        "               queuecap [ --if <ifName> [--auto | --manual] --queue <queue> --cap <entries> ]\n"
        "               queueweight [ --if <ifName> [--auto | --manual] --queue <queue> --weight <weight> ]\n"
        "               queueprofile [ --queueprofileid <id> --dropprob <dropprob> --minthreshold <threshold> "
        "--maxthreshold <threshold> ]\n"
        "               queuedropalg [ --if <ifName> --queue <queue> (--dt | --red | --wred) (--queueprofileid <id> "
        "(--queueprofileidhi <id> --prioritymask0 <0xhex> --prioritymask1 <0xhex>))\n"
        "               xtmchanneldropalg [ --channel <channel> (--dt | --red | --wred) (--queueprofileid <id> "
        "(--queueprofileidhi <id> --prioritymask0 <0xhex> --prioritymask1 <0xhex>))\n"
        "\n"
        );

    return;
} /* Usage */
/*
 *------------------------------------------------------------------------------
 * Function Name: GetArgType
 * Description  : Determines if the specified command line argument is a
 *                command, option or option parameter.
 * Returns      : ARG_TYPE_COMMAND, ARG_TYPE_OPTION, ARG_TYPE_PARAMETER
 *------------------------------------------------------------------------------
 */
int GetArgType(char *pszArg, PCOMMAND_INFO pCmds, char **ppszOptions)
{
    int nArgType = ARG_TYPE_PARAMETER;

    if ( ppszOptions )  /* See if the argument is a option. */
    {
        do
        {
            if ( !strcmp( pszArg, *ppszOptions ) )
            {
                nArgType = ARG_TYPE_OPTION;
                break;
            }
        } while ( *++ppszOptions );
    }

        /* Next, see if the argument is an command. */
    if ( nArgType == ARG_TYPE_PARAMETER )
    {
        while ( pCmds->szCmdName[0] != '\0' )
        {
            if ( !strcmp( pszArg, pCmds->szCmdName ) )
            {
                nArgType = ARG_TYPE_COMMAND;
                break;
            }
            pCmds++;
        }
    }

    return nArgType;    /* Otherwise, assume that it is a parameter. */
} /* GetArgType */

/*
 *------------------------------------------------------------------------------
 * Function Name: GetCommand
 * Description  : Returns the COMMAND_INFO structure for the specified
 *                command name.
 * Returns      : COMMAND_INFOR structure pointer
 *------------------------------------------------------------------------------
 */
PCOMMAND_INFO GetCommand(char *pszArg, PCOMMAND_INFO pCmds)
{
    PCOMMAND_INFO pCmd = NULL;

    while ( pCmds->szCmdName[0] != '\0' )
    {
        if ( !strcmp( pszArg, pCmds->szCmdName ) )
        {
            pCmd = pCmds;
            break;
        }
        pCmds++;
    }

    return pCmd;
} /* GetCommand */


/*
 *------------------------------------------------------------------------------
 * Function Name: ProcessCommand
 * Description  : Gets the options and option paramters for a command and
 *                calls the command handler function to process the command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int ProcessCommand(PCOMMAND_INFO pCmd, int argc, char **argv,
                   PCOMMAND_INFO pCmds, int *pnArgNext)
{
    int nRet = BCM_TM_SUCCESS, nNumOptInfo = 0, nArgType = 0;
    OPTION_INFO OptInfo[MAX_OPTS], *pCurrOpt = NULL;

    memset( OptInfo, 0x00, sizeof(OptInfo) );
    *pnArgNext = 0;

    do
    {
        if ( argc == 0 ) break;

        nArgType = GetArgType( *argv, pCmds, pCmd->pszOptionNames );
        switch ( nArgType )
        {
        case ARG_TYPE_OPTION:
            if ( nNumOptInfo < MAX_OPTS )
            {
                pCurrOpt = &OptInfo[nNumOptInfo++];
                pCurrOpt->pOptName = *argv;
            }
            else
            {
                nRet = BCM_TM_INVALID_NUMBER_OF_OPTIONS;
                fprintf( stderr, "%s: too many options\n", g_PgmName );
            }
            (*pnArgNext)++;
            break;

        case ARG_TYPE_PARAMETER:
            if ( pCurrOpt && pCurrOpt->nNumParms < MAX_PARMS )
                pCurrOpt->pParms[pCurrOpt->nNumParms++] = *argv;
            else
            {
                if ( pCurrOpt )
                {
                    nRet = BCM_TM_INVALID_OPTION;
                    fprintf( stderr, "%s: invalid option\n", g_PgmName );
                }
                else
                {
                    nRet = BCM_TM_INVALID_NUMBER_OF_OPTIONS;
                    fprintf( stderr, "%s: too many options\n", g_PgmName );
                }
            }
            (*pnArgNext)++;
            break;

        case ARG_TYPE_COMMAND:
            /* The current command is done. */
            break;
        } /* switch ( nArgType ) */

        argc--, argv++;

    } while ( (nRet == BCM_TM_SUCCESS) && (nArgType!=ARG_TYPE_COMMAND) );

    if ( nRet == BCM_TM_SUCCESS )
        nRet = (*pCmd->pfnCmdHandler)( OptInfo, nNumOptInfo );

    return nRet;
} /* ProcessCommand */

/*
 *------------------------------------------------------------------------------
 * Function Name: main
 * Description  : Main program function.
 * Returns      : 0 - success, non-0 - error.
 *------------------------------------------------------------------------------
 */
#ifdef BUILD_STATIC
int bcmtm_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    int nExitCode = 0;
    PCOMMAND_INFO pCmd;
    /* Save the name that started this program into a global variable. */
    strcpy( g_PgmName, *argv );
    if ( argc == 1 )
    {
        bcmTmCliHelp( );
        exit( nExitCode );
    }
    argc--, argv++;
    while ( argc && nExitCode == 0 )
    {
        if ( GetArgType( *argv, g_Cmds, NULL ) == ARG_TYPE_COMMAND )
        {
            int argnext = 0;
            pCmd = GetCommand( *argv, g_Cmds );
            argc--; argv++;
            nExitCode = ProcessCommand( pCmd, argc, argv, g_Cmds, &argnext );
            argc -= argnext;
            argv += argnext;
        }
        else
        {
            nExitCode = BCM_TM_INVALID_COMMAND;
            fprintf( stderr, "%s: invalid command\n", g_PgmName );
        }
    }
    exit( nExitCode );
}
