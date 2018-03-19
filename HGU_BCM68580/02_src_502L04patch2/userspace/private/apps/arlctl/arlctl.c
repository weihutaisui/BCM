/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

static char g_PgmName[128];

/***************************************************************************
 * File Name  : arlctl.c
 * Description: Linux command line utility that controls the Broadcom
 *              ARL Driver.
 ***************************************************************************/

#include <arlctl.h>
#include <ctype.h>

/*** File Scoped Globals. ***/
COMMAND_INFO g_Cmds[] =
{
    {"init",              {""},          InitHandler},
    {"show",              {""},          ShowHandler},
    {"flush",             {""},          FlushHandler},
    {"debug",    {"--log", ""},          DebugHandler},
    {""}
};
/*
 *------------------------------------------------------------------------------
 * Function Name: main
 * Description  : Main program function.
 * Returns      : 0 - success, non-0 - error.
 *------------------------------------------------------------------------------
 */
#ifdef BUILD_STATIC
int arl_main(int argc, char **argv)
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
        ArlCliHelp( );
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
            nExitCode = ARL_INVALID_COMMAND;
            fprintf( stderr, "%s: invalid command\n", g_PgmName );
        }
    }
    exit( nExitCode );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: ArlCliHelp
 * Description  : Displays the ARL CLI help
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
void ArlCliHelp(void)
{
    printf( 
        ARL_MODNAME " Control Utility:\n\n"
      
        "::::::  Usage  ::::::\n\n"

        "     arlctl init\n"
        "     arlctl show\n"
        "     arlctl flush\n"
        "     arlctl debug   [ --log <log_level> ]\n"

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
    int nRet = ARL_SUCCESS, nNumOptInfo = 0, nArgType = 0;
    OPTION_INFO OptInfo[MAX_OPTS]	, *pCurrOpt = NULL;

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
                nRet = ARL_INVALID_NUMBER_OF_OPTIONS;
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
                    nRet = ARL_INVALID_OPTION;
                    fprintf( stderr, "%s: invalid option\n", g_PgmName );
                }
                else
                {
                    nRet = ARL_INVALID_NUMBER_OF_OPTIONS;
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

    } while ( (nRet == ARL_SUCCESS) && (nArgType!=ARG_TYPE_COMMAND) );

    if ( nRet == ARL_SUCCESS )
        nRet = (*pCmd->pfnCmdHandler)( OptInfo, nNumOptInfo );

    return nRet;
} /* ProcessCommand */

/*
 *------------------------------------------------------------------------------
 * Function Name: arlDevOpen
 * Description  : Opens the arl device.
 * Returns      : device handle if successsful or -1 if error
 *------------------------------------------------------------------------------
 */
int arlDevOpen(void)
{
    int nFd = open( ARLDRV_DEVICE_NAME, O_RDWR );
	if ( nFd == -1 )
    {
		fprintf( stderr, "%s: open <%s> error no %d\n",
                 g_PgmName, ARLDRV_DEVICE_NAME, errno );
        return ARL_ERROR;
    }
    return nFd;
} /* arlDevOpen */
/*
 *------------------------------------------------------------------------------
 * Function Name: arlDevIoctl
 * Description  : Ioctls into ARL driver passing the IOCTL command, and 1 arg.
 *                CAUTION: Display is done in kernel context.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int arlDevIoctl(arlIoctl_t ioctl_cmd, int arg1, int arg2)
{
    int devFd, ret = ARL_ERROR;
    int arg = ( arg1 & 0xFFFF )
            | ( arg2 & 0xFFFF ) << 16;

    if ( ( devFd = arlDevOpen() ) == ARL_ERROR )
        return ARL_ERROR;

    if ( (ret = ioctl( devFd, ioctl_cmd, arg )) == ARL_ERROR )
        fprintf( stderr, "%s: arlDevIoctl <%d> error\n",
                 g_PgmName, ioctl_cmd );

    close( devFd );
    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: arlDevIoctlRaw
 * Description  : Ioctls into arl driver passing the IOCTL command, and 1 arg.
 *                CAUTION: Display is done in kernel context.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int arlDevIoctlRaw(arlIoctl_t ioctl_cmd, uint32_t arg)
{
    int devFd, ret = ARL_ERROR;

    if ( ( devFd = arlDevOpen() ) == ARL_ERROR )
        return ARL_ERROR;

    if ( (ret = ioctl( devFd, ioctl_cmd, arg )) == ARL_ERROR )
        fprintf( stderr, "%s: arlDevIoctlRaw <%d> error\n",
                 g_PgmName, ioctl_cmd );

    close( devFd );
    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: DebugHandler
 * Description  : Processes the ARL debug command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int DebugHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    int log;
	
    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return ARL_INVALID_OPTION;
    }
    else if ( nNumOptions > 1 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return ARL_INVALID_OPTION;
    }
    else if ( !strcmp( pOptions->pOptName, "--log" ) )
    {  
        if ( pOptions->nNumParms != 1 )
        {
            fprintf( stderr, "%s: must specify the Debug log level\n", g_PgmName );
            return ARL_INVALID_OPTION;
        }

        log = atoi( pOptions->pParms[0] );

        nRet = arlDevIoctlRaw( ARL_IOC_DEBUG, log);
    }
    else
    {
        fprintf( stderr, "%s: invalid option [%s]\n", g_PgmName, pOptions->pOptName );
        return ARL_INVALID_OPTION;
    }

    if ( nRet == ARL_ERROR )
    {
        fprintf( stderr, "%s: failed to set ARL Debug Level\n", g_PgmName );
    }

    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: InitHandler
 * Description  : Processes the ARL init command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int InitHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = arlDevIoctlRaw( ARL_IOC_INIT,0 );
    if ( nRet == ARL_ERROR )
        fprintf( stderr, "%s: failed to initialize the ARL driver\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: ShowHandler
 * Description  : Processes the ARL show command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int ShowHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = arlDevIoctlRaw( ARL_IOC_SHOW, 0 );
    if ( nRet == ARL_ERROR )
        fprintf( stderr, "%s: failed to show ARL\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: FlushHandler
 * Description  : Processes the ARL flush command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int FlushHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = arlDevIoctlRaw( ARL_IOC_FLUSH, 0 );
    if ( nRet == ARL_ERROR )
        fprintf( stderr, "%s: failed to flush ARL\n", g_PgmName );
    return nRet;
}
