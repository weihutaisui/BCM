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
 * File Name  : fbctl_cmds.c
 * Description: Linux command line utility that controls the Broadcom
 *              Flow Bonding Driver.
 ***************************************************************************/

#include <fbctl_api.h>
#include <fbctl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>


void Usage(void);

int  fbCtlStatusHandler(POPTION_INFO pOptions, int nNumOptions);
int  fbCtlEnableHandler(POPTION_INFO pOptions, int nNumOptions);
int  fbCtlDisableHandler(POPTION_INFO pOptions, int nNumOptions);
int  fbCtlIntervalHandler(POPTION_INFO pOptions, int nNumOptions);
int  fbCtlAddIfHandler(POPTION_INFO pOptions, int nNumOptions);
int  fbCtlDeleteIfHandler(POPTION_INFO pOptions, int nNumOptions);
int  fbCtlTokensHandler(POPTION_INFO pOptions, int nNumOptions);
int  fbCtlTestHandler(POPTION_INFO pOptions, int nNumOptions);
#if defined(CC_CONFIG_FBOND_DEBUG)
int  fbCtlDebugHandler(POPTION_INFO pOptions, int nNumOptions);
#endif


char g_PgmName[128] = {0};

/*** File Scoped Globals. ***/
COMMAND_INFO g_Cmds[] =
{
    {"status",      {""},                       fbCtlStatusHandler},
    {"enable",      {""},                       fbCtlEnableHandler},
    {"disable",     {""},                       fbCtlDisableHandler},
    {"interval",    {""},                       fbCtlIntervalHandler},
    {"addif",       {""},                       fbCtlAddIfHandler},
    {"deleteif",    {""},                       fbCtlDeleteIfHandler},
    {"tokens",      {""},                       fbCtlTokensHandler},
    {"test",        {""},                       fbCtlTestHandler},
#if defined(CC_CONFIG_FBOND_DEBUG)
    {"debug",       {"--drv", "--fb", ""},      fbCtlDebugHandler},
#endif
    {""}
};

/*
 *------------------------------------------------------------------------------
 * Function Name: Usage
 * Description  : Displays the fb usage
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
void Usage(void)
{
    printf( 
        "Flow Bond Control Utility:\n\n"
        "::: Usage:\n\n"

        ":::::: Flow Bond SW System :\n"
        "       fb status\n"
        "       fb enable\n"
        "       fb disable\n"
        "       fb interval <msec>\n"
        "       fb addif <ifname>\n"
        "       fb tokens <ifname> <tokens> <max_tokens>\n"
        "       fb test <num_flows>\n"
#if defined(CC_CONFIG_FBOND_DEBUG)
        "       fb debug\n"
        "              [ --drv <0..5> ]\n"
        "            | [ --fb  <0..5> ]\n\n"
#endif

        );

    return;
} /* Usage */



/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlStatusHandler
 * Description  : Processes the flow bond status command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlStatusHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fbCtlStatus();
    if ( nRet == FBOND_ERROR )
        fprintf( stderr, "%s: failed to get Flow Bond status\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlEnableHandler
 * Description  : Processes the flow bond enable command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlEnableHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fbCtlEnable();/* Functional interface return status */
    if ( nRet == FBOND_ERROR )
        fprintf( stderr, "%s: failed to enable Flow Bond\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlDisableHandler
 * Description  : Processes the flow bond disable command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlDisableHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fbCtlDisable();
    if ( nRet == FBOND_ERROR )
        fprintf( stderr, "%s: failed to disable Flow Bond\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlIntervalHandler
 * Description  : Sets the interval in msec when flows inactive are cleared
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlIntervalHandler(POPTION_INFO pOptions, int nNumOptions)
{
    unsigned int interval;
    
    if ( nNumOptions == 0 || (pOptions && pOptions->nNumParms < 1))
    {
        fprintf( stderr, "%s: No interval specified\n", g_PgmName );
        return INVALID_OPTION;
    }
    else if ( nNumOptions > 1 || pOptions->nNumParms > 1)
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return INVALID_OPTION;
    }

    interval = atoi(pOptions->pParms[0]);
    return fbCtlInterval( interval );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlAddIfHandler
 * Description  : Add an interface to a flow bond group.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlAddIfHandler(POPTION_INFO pOptions, int nNumOptions)
{
    unsigned int ifindex;
    
    if ( nNumOptions == 0 || (pOptions && pOptions->nNumParms < 1))
    {
        fprintf( stderr, "%s: No ifname specified\n", g_PgmName );
        return INVALID_OPTION;
    }
    else if ( nNumOptions > 1 || pOptions->nNumParms > 1)
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return INVALID_OPTION;
    }

    if ((ifindex = if_nametoindex(pOptions->pParms[0])) == 0)
    {
        fprintf( stderr, "%s: interface %s not found\n", g_PgmName, pOptions->pParms[0]);
        return INVALID_OPTION;
    }
    return fbCtlAddIf( 0, ifindex );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlDeleteIfHandler
 * Description  : Remove an interface from a flow bond group.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlDeleteIfHandler(POPTION_INFO pOptions, int nNumOptions)
{
    unsigned int ifindex;
    
    if ( nNumOptions == 0 || (pOptions && pOptions->nNumParms < 1))
    {
        fprintf( stderr, "%s: No ifname specified\n", g_PgmName );
        return INVALID_OPTION;
    }
    else if ( nNumOptions > 1 || pOptions->nNumParms > 1)
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return INVALID_OPTION;
    }

    if ((ifindex = if_nametoindex(pOptions->pParms[0])) == 0)
    {
        fprintf( stderr, "%s: interface %s not found\n", g_PgmName, pOptions->pParms[0]);
        return INVALID_OPTION;
    }
    return fbCtlDeleteIf( 0, ifindex );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlTokensHandler
 * Description  : Sets the number of tokens associated with an interface.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlTokensHandler(POPTION_INFO pOptions, int nNumOptions)
{
    unsigned int ifindex;
    unsigned int tokens;
    unsigned int max_tokens;
    
    if ( nNumOptions == 0 || (pOptions && pOptions->nNumParms < 3))
    {
        fprintf( stderr, "%s: No ifname, tokens or max_tokens specified\n", g_PgmName );
        return INVALID_OPTION;
    }
    else if ( nNumOptions > 1 || pOptions->nNumParms > 3)
    {
        fprintf( stderr, "%s: Too many parameters specified\n", g_PgmName );
        return INVALID_OPTION;
    }

    if ((ifindex = if_nametoindex(pOptions->pParms[0])) == 0)
    {
        fprintf( stderr, "%s: interface %s not found\n", g_PgmName, pOptions->pParms[0]);
        return INVALID_OPTION;
    }
    tokens = atoi(pOptions->pParms[1]);
    max_tokens = atoi(pOptions->pParms[2]);
    return fbCtlTokens( 0, ifindex, tokens, max_tokens );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlTestHandler
 * Description  : Randomly assign flows and prints status
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fbCtlTestHandler(POPTION_INFO pOptions, int nNumOptions)
{
    unsigned int num_flows;
    
    if ( nNumOptions == 0 || (pOptions && pOptions->nNumParms < 1))
    {
        fprintf( stderr, "%s: No number of flows specified\n", g_PgmName );
        return INVALID_OPTION;
    }
    else if ( nNumOptions > 1 || pOptions->nNumParms > 1)
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return INVALID_OPTION;
    }

    num_flows = atoi(pOptions->pParms[0]);
    return fbCtlTest( num_flows );
}

#if defined(CC_CONFIG_FBOND_DEBUG)
/*
 *------------------------------------------------------------------------------
 * Function Name: fbCtlDebugHandler
 * Description  : Processes the flow bond debug command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int  fbCtlDebugHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = FBOND_SUCCESS;

    /* Brain dead parser ... using cut-n-paste ... yukky ... but no time */
    while ( nNumOptions )
    {
        int level;

        if ( pOptions->nNumParms != 1 )
        {
           fprintf( stderr, "%s: did not specify debug level.\n", g_PgmName );
           return INVALID_OPTION;
        }

        level = atoi( pOptions->pParms[0] );

        if ( !strcmp( pOptions->pOptName, "--drv") )
            nRet = fbCtlDebug( FBOND_DBG_DRV_LAYER, level );
        else if ( !strcmp( pOptions->pOptName, "--fb") )
            nRet = fbCtlDebug( FBOND_DBG_FB_LAYER, level );
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n",
                     g_PgmName, pOptions->pOptName );
            return INVALID_OPTION;
        }
        nNumOptions--;
        pOptions++;

        if ( nRet == FBOND_ERROR )
            fprintf( stderr, "%s: failed debug request\n", g_PgmName );
    }

    return nRet;
}
#endif


