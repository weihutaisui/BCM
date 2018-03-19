/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom Corporation
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
 * File Name  : fcctl_api.c
 * Description: Linux command line utility that controls the Broadcom
 *              Flow Cache.
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

#include <fcache.h>
#include <fcctl_api.h>
#include <flwstats.h>

static int  fcDevOpen(void);
static int  fcDevIoctl(FcacheIoctl_t ioctl, int arg1, int arg2);

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlStatus
 * Description  : Displays flow cache status.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fcCtlStatus(FcStatusInfo_t *fcStatusInfo_p)
{
    return fcDevIoctl( FCACHE_IOCTL_STATUS, -1, (int)fcStatusInfo_p );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlEnable
 * Description  : Enables flow cache.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fcCtlEnable(void)
{
    return fcDevIoctl( FCACHE_IOCTL_ENABLE, 0, 0 );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlDisable
 * Description  : Disables flow cache.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fcCtlDisable(void)
{
    return fcDevIoctl( FCACHE_IOCTL_DISABLE, 0, 0 );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlFlush
 * Description  : Flushes one specific or all flows in flow cache.
 *              : flowid=0 to flush all flows; non-0 a specific flow
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fcCtlFlush(int flowid)
{
    FcFlushParams_t params;
    if ( flowid == 0 )
    {
        params.flags = FCACHE_FLUSH_ALL;
    }
    else
    {
        params.flags = FCACHE_FLUSH_FLOW;
        params.flowid = flowid;
    }
    return fcDevIoctl( FCACHE_IOCTL_FLUSH, -1, (int)&params );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlFlushDev
 * Description  : Flushes all flows for a specific device
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fcCtlFlushDev(int ifindex)
{
    FcFlushParams_t params;
    params.flags = FCACHE_FLUSH_DEV;
    params.devid = ifindex;
    return fcDevIoctl( FCACHE_IOCTL_FLUSH, -1, (int)&params );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlFlushParams
 * Description  : Flushes all flows that match given parameters.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fcCtlFlushParams(FcFlushParams_t * fcFlushParams_p)
{
    return fcDevIoctl( FCACHE_IOCTL_FLUSH, -1, (int)fcFlushParams_p );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlResetStats
 * Description  : Flushes all flows in flow cache.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fcCtlResetStats(void)
{
    return fcDevIoctl( FCACHE_IOCTL_RESET_STATS, 0, 0 );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlConfig
 * Description  : Configures the flow cache parameters.
 * Parameters   :
 *       option : one of the option to be configured.
 *         arg1 : parameter value
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fcCtlConfig(int option, int arg1)
{
    int ret = FCACHE_ERROR;

    switch (option)
    {
#if defined(PKTCMF_AVAIL)
        case FCACHE_CONFIG_OPT_DEFER:
            ret = fcDevIoctl( FCACHE_IOCTL_DEFER, -1, arg1 );
            break;

        case FCACHE_CONFIG_OPT_MONITOR:
            ret = fcDevIoctl( FCACHE_IOCTL_MONITOR, -1, arg1 );
            break;
#endif

        case FCACHE_CONFIG_OPT_MCAST:
            ret = fcDevIoctl( FCACHE_IOCTL_MCAST, -1, arg1 );
            break;

        case FCACHE_CONFIG_OPT_MCAST_LEARN:
            ret = fcDevIoctl( FCACHE_IOCTL_MCAST_LEARN, -1, arg1 );
            break;

        case FCACHE_CONFIG_OPT_IPV6:
            ret = fcDevIoctl( FCACHE_IOCTL_IPV6, -1, arg1 );
            break;

        case FCACHE_CONFIG_OPT_TIMER:
            ret = fcDevIoctl( FCACHE_IOCTL_TIMER, -1, arg1 );
            break;

        case FCACHE_CONFIG_OPT_GRE:
            ret = fcDevIoctl( FCACHE_IOCTL_GRE, -1, arg1 );
            break;
        case FCACHE_CONFIG_OPT_L2TP:
            ret = fcDevIoctl( FCACHE_IOCTL_L2TP, -1, arg1 );
            break;

        case FCACHE_CONFIG_OPT_ACCEL_MODE:
            ret = fcDevIoctl( FCACHE_IOCTL_ACCEL_MODE, -1, arg1 );
            break;

        default:
            fprintf( stderr, "invalid config option <%d>\n", option );
            ret = FCACHE_ERROR;
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlDump
 * Description  : Dumps the flow information.
 * Parameters   :
 *       option : one of the option to be dumped.
 *         arg1 : parameter value
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fcCtlDump(int option, int arg1)
{
    int ret = FCACHE_ERROR;

    switch (option)
    {
        case FCACHE_DUMP_OPT_FLOW_INFO:
            ret = fcDevIoctl( FCACHE_IOCTL_DUMP_FLOW_INFO, -1, arg1 );
            break;

        default:
            fprintf( stderr, "invalid dump option <%d>\n", option );
            ret = FCACHE_ERROR;
    }

    return ret;
}

#if defined(CC_CONFIG_FCACHE_DEBUG)

/*
 *------------------------------------------------------------------------------
 * Function Name: fcCtlDebug
 * Description  : Sets the debug level for the layer in flow cache.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int  fcCtlDebug(int layer, int level)
{
    if ( layer >= FCACHE_DBG_LAYER_MAX )
        fprintf( stderr, "invalid layer <%d>\n", layer );

    return fcDevIoctl( FCACHE_IOCTL_DEBUG, layer, level );
}
#endif

/*
 *---------------------------------------------------------------------------
 * Function Name: fcCtlCreateFlwStatsQuery
 * Description  : Creates a statistics query for later use.
 * Parameters   :
 *  queryInfo_p : Pointer to an  FlwStatsQueryInfo_t structure where 
 *                the flwStatsQueryTuple_t portion will be used to set
 *                up the query.  The flwStats_t portion will be ignored.
 *                A unique handle would be returned in queryInfo_p for later
 *                use.
 * Returns      : 0 - success, non-0 - error
 *---------------------------------------------------------------------------
 */
int fcCtlCreateFlwStatsQuery(FlwStatsQueryInfo_t *queryInfo_p)
{
	int retVal=0;
	
	/* Issue IOCTL to create a new flowstats query */
    retVal = fcDevIoctl( FCACHE_IOCTL_CREATE_FLWSTATS, -1, (int)queryInfo_p );
	
	return(retVal);
}

/*
 *-----------------------------------------------------------------------------
 * Function Name: fcCtlGetFlwStatsQuery
 * Description  : Retrieves statistics for a query previously set up by a 
 *                call to fcCtlCreateFlwStatsQuery().
 * Parameters   :
 *  queryInfo_p : Handle returned by earlier fcCtlCreateFlwStatsQuery() is
 *                passed as input. Results are stored in flwSt field in
 *                queryInfo_p and the query matching the handle
 *                will be filled in queryTuple field
 * Returns      : 0 - success, non-0 - error
 *-----------------------------------------------------------------------------
 */
int fcCtlGetFlwStatsQuery(FlwStatsQueryInfo_t *queryInfo_p)
{
	int retVal = 0;
	
	/* Issue IOCTL to run a flowstats query */
    retVal = fcDevIoctl( FCACHE_IOCTL_GET_FLWSTATS, -1, (int)(queryInfo_p) );	
	
	return retVal;
}


/*
 *-----------------------------------------------------------------------------
 * Function Name: fcCtlDeleteFlwStatsQuery
 * Description  : Deletes one or more queries previously set up by a call  
 *                to fcCtlCreateFlwStatsQuery().
 * Parameters   :
 *  queryInfo_p : Handle returned by earlier fcCtlCreateFlwStatsQuery() is
 *                passed as input. queryTuple and flwSt fields in queryInfo_p
 *                will be ignored. The special value 
 *                ALL_STATS_QUERIES_HANDLE may be used to delete all queries.
 * Returns      : 0 - success, non-0 - error
 *-----------------------------------------------------------------------------
 */
int fcCtlDeleteFlwStatsQuery(FlwStatsQueryInfo_t *queryInfo_p)
{
	/* Issue IOCTL to delete a flowstats query */
    return fcDevIoctl( FCACHE_IOCTL_DELETE_FLWSTATS, -1, (int)(queryInfo_p) );	
}


/*
 *-----------------------------------------------------------------------------
 * Function Name: fcCtlClearFlwStatsQuery
 * Description  : Zeroes out the counters for one or more queries previously  
 *                set up by a call to fcCtlCreateFlwStatsQuery().
 
 * Parameters   :
 *  queryInfo_p : Handle returned by earlier fcCtlCreateFlwStatsQuery() is
 *                passed as input. queryTuple and flwSt fields in queryInfo_p
 *                will be ignored.  The special value
 *                ALL_STATS_QUERIES_HANDLE may be used to clear the 
 *                counters for all queries.
 * Returns      : 0 - success, non-0 - error
 *-----------------------------------------------------------------------------
 */
int fcCtlClearFlwStatsQuery(FlwStatsQueryInfo_t *queryInfo_p)
{
	/* Issue IOCTL to clear a flowstats query */
    return fcDevIoctl(FCACHE_IOCTL_CLEAR_FLWSTATS, -1, (int)(queryInfo_p));
}


/*
 *-----------------------------------------------------------------------------
 * Function Name: fcCtlGetQueryEntryNumber
 * Description  : Get Query entry number.
 * Returns      : >=0 - number of queries in fc, <0 - error
 *-----------------------------------------------------------------------------
 */
int fcCtlGetQueryEntryNumber(void)
{	
	/* Issue IOCTL to get number of queries in fc */
    return fcDevIoctl( FCACHE_IOCTL_GET_FLWSTATS_NUM, -1, 0);
}


/*
 *-----------------------------------------------------------------------------
 * Function Name: fcCtlDumpFlwStats
 * Description  : Dumps all active queries to console.
 * Returns      : None
 *-----------------------------------------------------------------------------
 */
int fcCtlDumpFlwStats(FlwStatsDumpInfo_t *flwDumpInfo_p)
{	
	/* Issue IOCTL to delete a flowstats query */
    return fcDevIoctl( FCACHE_IOCTL_DUMP_FLWSTATS, -1, (int)(flwDumpInfo_p) );
}


/*
 *------------------------------------------------------------------------------
 * Function Name: fcDevOpen
 * Description  : Opens the flow cache device.
 * Returns      : device handle if successsful or -1 if error
 *------------------------------------------------------------------------------
 */
static int fcDevOpen(void)
{
    int nFd = open( FCACHE_DRV_DEVICE_NAME, O_RDWR );
    if ( nFd == -1 )
    {
        fprintf( stderr, "open <%s> error no %d\n",
                 FCACHE_DRV_DEVICE_NAME, errno );
        return FCACHE_ERROR;
    }
    return nFd;
} /* fcDevOpen */

/*
 *------------------------------------------------------------------------------
 * Function Name: fcDevIoctl
 * Description  : Ioctls into fcache driver passing the IOCTL command, and the
 *                fcache (arg1) and id (arg2) passed as a 16bit tuple.
 *                If arg1 == -1, then arg2 is passed directly.
 *                CAUTION: Display is done in kernel context.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int fcDevIoctl(FcacheIoctl_t ioctl_cmd, int arg1, int arg2)
{
    int devFd, arg = 0, ret = FCACHE_ERROR;

    if ( arg1 == -1 )
        arg = arg2;
    else
        arg = ( ( (arg1 & 0xFF) << 8) | (arg2 & 0xFF) ) ;

    if ( ( devFd = fcDevOpen() ) == FCACHE_ERROR )
        return FCACHE_ERROR;

    if ( (ret = ioctl( devFd, ioctl_cmd, arg )) == FCACHE_ERROR )
        fprintf( stderr, "fcDevIoctl <%d> error\n", ioctl_cmd );

    close( devFd );
    return ret;
}


