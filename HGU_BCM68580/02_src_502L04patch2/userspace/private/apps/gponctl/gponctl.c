/*
* <:copyright-BRCM:2007-2010:proprietary:standard
* 
*    Copyright (c) 2007-2010 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :>

*/


/***************************************************************************
 * File Name  : gponctl.c
 * Description: Linux command line utility that controls the Broadcom
 *              PLOAM and OMCI Drivers.
 *              PLOAM and OMCI specific command line parsing is
 *              implemented in the gponctl_ploam.c and gponctl_omci.c
 ***************************************************************************/

#include "gponctl.h"

/*
 *------------------------------------------------------------------------------
 * Function Name: main
 * Description  : Main program function.
 * Returns      : 0 - success, non-0 - error.
 *------------------------------------------------------------------------------
 */
#ifdef BUILD_STATIC
int gponctl_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    int nExitCode = 0;
    PGPON_COMMAND_INFO pCmd;
        /* Save the name that started this program into a global variable. */
    strcpy( g_gponctl_PgmName, *argv );
    if ( argc == 1 )
    {
        usage( );
    }
    else
    {
        argc--, argv++;
        while ( argc && nExitCode == 0 )
        {
            if ( getArgType( *argv, g_gponctl_Cmds, NULL ) == GPON_ARG_TYPE_COMMAND )
            {
                int argnext = 0;
                pCmd = getCommand( *argv, g_gponctl_Cmds );
                argc--; argv++;
                nExitCode = processCommand( pCmd, argc, argv, g_gponctl_Cmds, &argnext );
                argc -= argnext;
                argv += argnext;
            }
            else
            {
                nExitCode = GPONCTL_INVALID_COMMAND;
                fprintf( stderr, "%s: invalid command\n", g_gponctl_PgmName );
            }
        }
    }

#ifndef BUILD_STATIC
    // Exit only on Linux systems
    exit( nExitCode );
#else
    return nExitCode;
#endif
}


/*
 *------------------------------------------------------------------------------
 * Function Name: getArgType
 * Description  : Determines if the specified command line argument is a
 *                command, option or option parameter.
 * Returns      : GPON_ARG_TYPE_COMMAND, GPON_ARG_TYPE_OPTION, GPON_ARG_TYPE_PARAMETER
 *------------------------------------------------------------------------------
 */
int getArgType(char *pszArg, PGPON_COMMAND_INFO pCmds, char **ppszOptions)
{
    int nArgType = GPON_ARG_TYPE_PARAMETER;

    if ( ppszOptions )  /* See if the argument is a option. */
    {
        do
        {
            if ( !strcasecmp( pszArg, *ppszOptions ) )
            {
                nArgType = GPON_ARG_TYPE_OPTION;
                break;
            }
        } while ( *++ppszOptions );
    }

        /* Next, see if the argument is an command. */
    if ( nArgType == GPON_ARG_TYPE_PARAMETER )
    {
        while ( pCmds->szCmdName[0] != '\0' )
        {
            if ( !strcasecmp( pszArg, pCmds->szCmdName ) )
            {
                nArgType = GPON_ARG_TYPE_COMMAND;
                break;
            }
            pCmds++;
        }
    }

    return nArgType;    /* Otherwise, assume that it is a parameter. */
} /* getArgType */


/*
 *------------------------------------------------------------------------------
 * Function Name: getCommand
 * Description  : Returns the GPON_COMMAND_INFO structure for the specified
 *                command name.
 * Returns      : GPON_COMMAND_INFOR structure pointer
 *------------------------------------------------------------------------------
 */
PGPON_COMMAND_INFO getCommand(char *pszArg, PGPON_COMMAND_INFO pCmds)
{
    PGPON_COMMAND_INFO pCmd = NULL;

    while ( pCmds->szCmdName[0] != '\0' )
    {
        if ( !strcasecmp( pszArg, pCmds->szCmdName ) )
        {
            pCmd = pCmds;
            break;
        }
        pCmds++;
    }

    return pCmd;
} /* getCommand */


/*
 *------------------------------------------------------------------------------
 * Function Name: processCommand
 * Description  : Gets the options and option paramters for a command and
 *                calls the command handler function to process the command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int processCommand(PGPON_COMMAND_INFO pCmd, int argc, char **argv,
                   PGPON_COMMAND_INFO pCmds, int *pnArgNext)
{
    int nRet = GPONCTL_SUCCESS, nNumOptInfo = 0, nArgType = 0;
    GPON_OPTION_INFO OptInfo[GPON_MAX_OPTS], *pCurrOpt = NULL;

    memset( OptInfo, 0x00, sizeof(OptInfo) );
    *pnArgNext = 0;

    do
    {
        if ( argc == 0 ) break;

        nArgType = getArgType( *argv, pCmds, pCmd->pszOptionNames );
        switch ( nArgType )
        {
        case GPON_ARG_TYPE_OPTION:
            if ( nNumOptInfo < GPON_MAX_OPTS )
            {
                pCurrOpt = &OptInfo[nNumOptInfo++];
                pCurrOpt->pOptName = *argv;
            }
            else
            {
                nRet = GPONCTL_INVALID_NUMBER_OF_OPTIONS;
                fprintf( stderr, "%s: too many options\n", g_gponctl_PgmName );
            }
            (*pnArgNext)++;
            break;

        case GPON_ARG_TYPE_PARAMETER:
            if ( pCurrOpt && pCurrOpt->nNumParms < GPON_MAX_PARMS )
                pCurrOpt->pParms[pCurrOpt->nNumParms++] = *argv;
            else
            {
                if ( pCurrOpt )
                {
                    nRet = GPONCTL_INVALID_OPTION;
                    fprintf( stderr, "%s: invalid option\n", g_gponctl_PgmName );
                }
                else
                {
                    nRet = GPONCTL_INVALID_NUMBER_OF_OPTIONS;
                    fprintf( stderr, "%s: too many options\n", g_gponctl_PgmName );
                }
            }
            (*pnArgNext)++;
            break;

        case GPON_ARG_TYPE_COMMAND:
            /* The current command is done. */
            break;
        } /* switch ( nArgType ) */

        argc--, argv++;

    } while ( (nRet == GPONCTL_SUCCESS) && (nArgType!=GPON_ARG_TYPE_COMMAND) );

    if ( nRet == GPONCTL_SUCCESS )
        nRet = (*pCmd->pfnCmdHandler)( OptInfo, nNumOptInfo );

    return nRet;
} /* processCommand */


/*
 *------------------------------------------------------------------------------
 * Function Name: dumpOption
 * Description  : Debug function that dumps the options and parameters
 *                for a particular command.
 * Returns      : None.
 *------------------------------------------------------------------------------
 */
void dumpOption(char *pszCmdName, PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    PGPON_OPTION_INFO pOpt;
    int i, j;

    printf( "cmd=%s\n", pszCmdName );
    for ( i = 0; i < nNumOptions; i++ )
    {
        pOpt = pOptions + i;
        printf( "opt=%s, %d parms=", pOpt->pOptName, pOpt->nNumParms );
        for ( j = 0; j < pOpt->nNumParms; j++ )
        {
            printf( "%s ", pOpt->pParms[j] );
        }
        printf( "\n" );
    }
    printf( "\n" );
}
