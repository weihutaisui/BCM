/*
<:copyright-BRCM:2012:proprietary:standard

   Copyright (c) 2012 Broadcom 
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

/***************************************************************************
 * File Name  : gmacctl_cmds.c
 * Description: Linux command line utility that controls the Broadcom
 *              BCM63xx GMAC Driver.
 ***************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "gmacctl.h"
#include "bcmgmacctl.h"
#include "gmacctl_api.h"

void Usage(void);

int  gmacctlAppStatusHandler(POPTION_INFO pOptions, int nNumOptions);
int  gmacctlAppSetModeHandler(POPTION_INFO pOptions, int nNumOptions);
int  gmacctlAppGetModeHandler(POPTION_INFO pOptions, int nNumOptions);
int  gmacctlAppDumpMibHandler(POPTION_INFO pOptions, int nNumOptions);
#if defined(CC_GMAC_DEBUG)
int  gmacctlAppDebugHandler(POPTION_INFO pOptions, int nNumOptions);
#endif

char g_PgmName[128] = {0};

/*** File Scoped Globals. ***/
COMMAND_INFO g_Cmds[] =
{
    {"status",      {""},                       gmacctlAppStatusHandler},
    {"set",         {"--mode", ""},             gmacctlAppSetModeHandler},
    {"get",         {"--mode", ""},             gmacctlAppGetModeHandler},
    {"dump",        {"--mib", ""},              gmacctlAppDumpMibHandler},
#if defined(CC_GMAC_DEBUG)
    {"debug",       {"--drv", "--gmac", ""},    gmacctlAppDebugHandler},
#endif
    {""}
};

/*
 *------------------------------------------------------------------------------
 * Function Name: Usage
 * Description  : Displays the gmac usage
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
void Usage(void)
{
    printf( 
        "GMAC Control Utility:\n\n"
        "NOTE:\n"
        "1. GMAC should be present on the chip, and\n"
        "2. ROBO port should be already configured as WAN.\n\n"

        "::: Usage:\n\n"

        ":::::: GMAC Driver:\n"
        "       gmac status\n"
        "       gmac set --mode <0|1>\n"
        "          mode: 0 = ROBO port (def), 1 = Link Speed\n"
        "       gmac get --mode\n"
        "       gmac dump --mib <0|1>\n"
        "          mib: 0 = partial, 1 = all\n"
#if defined(CC_GMAC_DEBUG)
        "       gmac debug\n"
        "                      [ --drv    <0..5> ]\n"
        "                    | [ --gmac     <0..5> ]\n\n"
#endif
        );

    return;
} /* Usage */



/*
 *------------------------------------------------------------------------------
 * Function Name: gmacctlAppStatusHandler
 * Description  : Processes the GMAC status command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int gmacctlAppStatusHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = gmacctl_dump_status();

    if ( nRet == GMACCTL_ERROR )
        fprintf( stderr, "%s: failed to get GMAC status\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: SetHandler
 * Description  : Processes the GMAC config command
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int gmacctlAppSetModeHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    int mode;

    if ( nNumOptions == 0 )
    {
        mode = 0;
    }
    else if ( nNumOptions > 1 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return GMACCTL_INVALID_OPTION;
    }
    else if ( !strcmp( (pOptions)->pOptName, "--mode") )
    {
        mode = atoi( (pOptions)->pParms[0] );
    }
    else
    {
        fprintf( stderr, "%s: invalid option\n", g_PgmName );
        return GMACCTL_INVALID_OPTION;
    }

    nRet = gmacctl_set_mode( mode );
    if ( nRet == GMACCTL_ERROR )
        fprintf( stderr, "%s: failed to set the GMAC mode\n", g_PgmName );
    return nRet;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: gmacctlAppGetModeHandler
 * Description  : Processes the GMAC get mode command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int gmacctlAppGetModeHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    int mode;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return GMACCTL_INVALID_OPTION;
    }
    else if ( nNumOptions != 1 )
    {
        fprintf( stderr, "%s: incorrect number of options\n", g_PgmName );
        return GMACCTL_INVALID_OPTION;
    }

    while ( nNumOptions )
    {
        if ( strcmp( pOptions->pOptName, "--mode") )
        {
            fprintf( stderr, "%s: invalid option [%s]\n",
                     g_PgmName, pOptions->pOptName );
            return GMACCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    nRet = gmacctl_get_mode( &mode );

    if ( nRet == GMACCTL_ERROR )
        fprintf( stderr, "%s: failed to get GMAC mode \n", g_PgmName );
    else
        printf( "%s: Mode = %d \n", g_PgmName, mode );

    return nRet;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: gmacctlAppDumpMibHandler
 * Description  : Processes the GMAC dump MIB command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int gmacctlAppDumpMibHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    int mib = 0;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return GMACCTL_INVALID_OPTION;
    }
    else if ( nNumOptions > 1 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return GMACCTL_INVALID_OPTION;
    }
    else if ( !strcmp( (pOptions)->pOptName, "--mib") )
    {
        mib = atoi( (pOptions)->pParms[0] );
    }
    else
    {
        fprintf( stderr, "%s: invalid option\n", g_PgmName );
        return GMACCTL_INVALID_OPTION;
    }

    nRet = gmacctl_dump_mib( mib );

    if ( nRet == GMACCTL_ERROR )
        fprintf( stderr, "%s: failed to dump GMAC mib \n", g_PgmName );

    return nRet;
}


#if defined(CC_GMAC_DEBUG)
/*
 *------------------------------------------------------------------------------
 * Function Name: gmacctlAppDebugHandler
 * Description  : Processes the GMAC debug command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int  gmacctlAppDebugHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;

    while ( nNumOptions )
    {
        int level;

        if ( pOptions->nNumParms != 1 )
        {
           fprintf( stderr, "%s: did not specify debug level.\n", g_PgmName );
           return GMACCTL_INVALID_OPTION;
        }

        level = atoi( pOptions->pParms[0] );

        if ( !strcmp( pOptions->pOptName, "--drv") )
            nRet = gmacctlAppDebug( GMAC_DBG_DRV_LAYER, level );
        else if ( !strcmp( pOptions->pOptName, "--gmac") )
            nRet = gmacctlAppDebug( GMAC_DBG_FC_LAYER, level );
        else
        {
            fprintf( stderr, "%s: invalid option\n", g_PgmName );
            return GMACCTL_INVALID_OPTION;
        }
        nNumOptions--;
        pOptions++;

        if ( nRet == GMACCTL_ERROR )
            fprintf( stderr, "%s: failed debug request\n", g_PgmName );
    }

    return nRet;
}
#endif


