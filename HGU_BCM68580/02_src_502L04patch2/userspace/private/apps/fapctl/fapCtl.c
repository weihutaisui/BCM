/*
<:copyright-BRCM:2009:proprietary:standard

   Copyright (c) 2009 Broadcom 
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


static char g_PgmName[128];

/***************************************************************************
 * File Name  : fapCtl.c
 * Description: Linux command line utility that controls the Broadcom
 *              FAP Driver.
 ***************************************************************************/

#include <fapCtl.h>
#include <ctype.h>
#include <net/if.h>

#include "fapctl_api.h"

/*** File Scoped Globals. ***/
COMMAND_INFO g_Cmds[] =
{
    {"hw",       {"--grp", ""},          HwHandler},
    {"status",            {""},          StatusHandler},
    {"init",              {""},          InitHandler},
    {"enable",            {""},          EnableHandler},
    {"disable",           {""},          DisableHandler},
    {"print",    {"--src", "--dst", ""}, PrintHandler},
    {"debug",    {"--log", ""},          DebugHandler},
    {"cpu",               {""},          CpuHandler},
    {"swq",               {""},          SwqHandler},
    {"dma",               {""},          DmaHandler},
    {"mem",               {""},          MemHandler},
    {"dm",       {"--fap", "--rgn", "--short", "--long", ""}, DymMemHandler},
    {"mtu",      {"--fap", "--flow", "--mtu", ""},          MtuHandler},
    {"tm",       {"--on", "--off", "--stats", "--ifcfg", "--type",
                  "--arbitercfg", "--sp", "--wrr", "--spwrr", "--wfq", "--lowprioq",
                  "--queuecfg", "--tmq2swq", "--queue", "--swqueue", "--min", "--max",
                  "--if", "--enable", "--disable", "--auto", "--queueweight", "--weight",
                  "--manual", "--kbps", "--mbs", "--mode", "--rate", "--ratio",
                  "--apply", "--status", "--maps", "--lan", "--wan", "--reset",
                  "--queueprofile", "--queueprofileid", "--minthreshold",
                  "--maxthreshold", "--dropprob", "--queuedropalg",
                  "--queueprofileidhi", "--prioritymask0", "--prioritymask1",
                  "--dt", "--red", "--wred", "--xtmchanneldropalg",
                  "--channel", ""}, TmHandler},
    {"flood",    {"--port", "--mask", "--drop", ""},  FloodHandler},
    {"arl",      {"--flush", "--show", ""}, arlHandler},
    {"4ketest",           {""},          Fap4keTest},
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
int fap_main(int argc, char **argv)
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
        FapCliHelp( );
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
            nExitCode = FAP_INVALID_COMMAND;
            fprintf( stderr, "%s: invalid command\n", g_PgmName );
        }
    }
    exit( nExitCode );
}

/*
 *------------------------------------------------------------------------------
 * Function Name: FapCliHelp
 * Description  : Displays the FAP CLI help
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
void FapCliHelp(void)
{
    printf(
        FAP_MODNAME " Control Utility:\n\n"

        "::::::  Usage  ::::::\n\n"

        ":::: Debugging :::::\n\n"
        "     fapctl hw      [ --grp <group_name> \n"
        "                       0 for ALL register \n"
        "                       1 for Control register  \n"
        "                       2 for Outgoing Msg. register  \n"
        "                       3 for Incoming Msg. register  \n"
        "                       4 for DMA registers  \n"
        "                       5 for TKNINTF register  \n"
        "                       6 for MSGID register  \n"
        "                       7 for DQM register  \n"
        "                       8 for DQM Control register  \n"
        "                       9 for DQM Data register  \n"
        "                       10 for DQM Mib register ]\n\n"
        "     fapctl debug   [ --log <log_level> ]\n"
        "     fapctl cpu\n"
        "     fapctl dma\n\n"
        // not ducumenting fapctl mem on purpose

        ":::: FAP System :::::\n\n"
        "     fapctl reset\n"
        "     fapctl init\n"
        "     fapctl enable\n"
        "     fapctl disable\n"
        "     fapctl sleep\n"
        "     fapctl wakeup\n"
        "     fapctl status\n"
        "     fapctl print [ --src <source_port_number> ] [ --dst <dest_port_number> ]\n"
        "     fapctl mtu [ --fap <fapIdx>] [ --flow <flow_id> ] --mtu <mtu> \n"
        "     fapctl tm [ --stats (--if <ifName>) ]\n"
        "               [ --status ]\n"
        "               [ --maps ]\n"
        "               [ --on ]\n"
        "               [ --off ]\n"
        "               [ --ifcfg --if <ifName> [--auto | --manual] --kbps <kbps> --mbs <bytes> (--rate | --ratio) ]\n"
        "               [ --mode --if <ifName> [--auto | --manual] ]\n"
        "               [ --type --if <ifName> [--lan | --wan] ]\n"
        "               [ --reset --if <ifName> [--auto | --manual] ]\n"
        "               [ --enable --if <ifName> [--auto | --manual] ]\n"
        "               [ --disable --if <ifName> [--auto | --manual] ]\n"
        "               [ --apply --if <ifName> ]\n"
        "               [ --queuecfg --if <ifName> [--auto | --manual] --queue <queue> "
        "[--min | --max] --kbps <kbps> --mbs <bytes> ]\n"
        "               [ --queueweight --if <ifName> [--auto | --manual] --queue <queue> --weight <weight> ]\n"
        "               [ --queueprofile --queueprofileid <id> --dropprob <dropprob> --minthreshold <threshold> "
        "--maxthreshold <threshold> ]\n"
        "               [ --queuedropalg --if <ifName> --queue <queue> (--dt | --red | --wred) (--queueprofileid <id> "
        "(--queueprofileidhi <id> --prioritymask0 <0xhex> --prioritymask1 <0xhex>))\n"
        "               [ --xtmchanneldropalg --channel <channel> (--dt | --red | --wred) (--queueprofileid <id> "
        "(--queueprofileidhi <id> --prioritymask0 <0xhex> --prioritymask1 <0xhex>))\n"
        "               [ --arbitercfg --if <ifName> [--auto | --manual] [--sp | --wrr | --spwrr | --wfq ] "
        "(--lowprioq) ]\n"
        "               [ --tmq2swq --if <ifName> --queue <queue> --swqueue <queue> ]\n"
        "     fapctl flood [ --port <port_number> --mask <port_mask> ] [ --port <port_number> --drop ]\n"
        "     fapctl arl [ --flush <port_mask> ] [ --show ]\n"
        "     fapctl dm [--rgn ( all | dsp | psm | qsm ) ]  [ --fap <fapIdx> ] [--short]\n\n"
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
    int nRet = FAP_SUCCESS, nNumOptInfo = 0, nArgType = 0;
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
                nRet = FAP_INVALID_NUMBER_OF_OPTIONS;
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
                    nRet = FAP_INVALID_OPTION;
                    fprintf( stderr, "%s: invalid option\n", g_PgmName );
                }
                else
                {
                    nRet = FAP_INVALID_NUMBER_OF_OPTIONS;
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

    } while ( (nRet == FAP_SUCCESS) && (nArgType!=ARG_TYPE_COMMAND) );

    if ( nRet == FAP_SUCCESS )
        nRet = (*pCmd->pfnCmdHandler)( OptInfo, nNumOptInfo );

    return nRet;
} /* ProcessCommand */

/*
 *------------------------------------------------------------------------------
 * Function Name: fapDevOpen
 * Description  : Opens the fap device.
 * Returns      : device handle if successsful or -1 if error
 *------------------------------------------------------------------------------
 */
int fapDevOpen(void)
{
    int nFd = open( FAPDRV_DEVICE_NAME, O_RDWR );
    if ( nFd == -1 )
    {
        fprintf( stderr, "%s: open <%s> error no %d\n",
                 g_PgmName, FAPDRV_DEVICE_NAME, errno );
        return FAP_ERROR;
    }
    return nFd;
} /* fapDevOpen */
/*
 *------------------------------------------------------------------------------
 * Function Name: fapDevIoctl
 * Description  : Ioctls into fap driver passing the IOCTL command, and 1 arg.
 *                CAUTION: Display is done in kernel context.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fapDevIoctl(fapIoctl_t ioctl_cmd, int arg1, int arg2)
{
    int devFd, ret = FAP_ERROR;
    int arg = ( arg1 & 0xFFFF )
            | ( arg2 & 0xFFFF ) << 16;

    if ( ( devFd = fapDevOpen() ) == FAP_ERROR )
        return FAP_ERROR;

    if ( (ret = ioctl( devFd, ioctl_cmd, arg )) == FAP_ERROR )
        fprintf( stderr, "%s: fapDevIoctl <%d> error\n",
                 g_PgmName, ioctl_cmd );

    close( devFd );
    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fapDevIoctlRaw
 * Description  : Ioctls into fap driver passing the IOCTL command, and 1 arg.
 *                CAUTION: Display is done in kernel context.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int fapDevIoctlRaw(fapIoctl_t ioctl_cmd, uint32_t arg)
{
    int devFd, ret = FAP_ERROR;

    if ( ( devFd = fapDevOpen() ) == FAP_ERROR )
        return FAP_ERROR;

    if ( (ret = ioctl( devFd, ioctl_cmd, arg )) == FAP_ERROR )
        fprintf( stderr, "%s: fapDevIoctlRaw <%d> error\n",
                 g_PgmName, ioctl_cmd );

    close( devFd );
    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: HwHandler
 * Description  : Processes the FAP hw command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int HwHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    int grp;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }
    else if ( nNumOptions > 1 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }
    else if ( !strcmp( pOptions->pOptName, "--grp" ) )
    {
        if ( pOptions->nNumParms != 1 )
        {
            fprintf( stderr, "%s: must specify the Hw group\n",
                     g_PgmName );
            return FAP_INVALID_OPTION;
        }
        grp = atoi( pOptions->pParms[0] );
        if ( grp < 0 || grp > 10 )
        {
             fprintf(stderr,"%s: Hw register group %d not in range <0..10>\n",
                        g_PgmName, grp );
             return FAP_INVALID_OPTION;
        }
        nRet = fapDevIoctlRaw( FAP_IOC_HW, grp);
    }
    else
    {
        fprintf( stderr, "%s: invalid option [%s]\n",
                   g_PgmName, pOptions->pOptName );
        return FAP_INVALID_OPTION;
    }
    if ( nRet == FAP_ERROR )
        fprintf( stderr, "%s: failed to get FAP Hw Information\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: DebugHandler
 * Description  : Processes the FAP debug command.
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
        return FAP_INVALID_OPTION;
    }
    else if ( nNumOptions > 1 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }
    else if ( !strcmp( pOptions->pOptName, "--log" ) )
    {
        if ( pOptions->nNumParms != 1 )
        {
            fprintf( stderr, "%s: must specify the Debug log level\n", g_PgmName );
            return FAP_INVALID_OPTION;
        }

        log = atoi( pOptions->pParms[0] );

        nRet = fapDevIoctlRaw( FAP_IOC_DEBUG, log);
    }
    else
    {
        fprintf( stderr, "%s: invalid option [%s]\n", g_PgmName, pOptions->pOptName );
        return FAP_INVALID_OPTION;
    }

    if ( nRet == FAP_ERROR )
    {
        fprintf( stderr, "%s: failed to set FAP Debug Level\n", g_PgmName );
    }

    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: DmaHandler
 * Description  : Processes the fap dma command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int DmaHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fapDevIoctlRaw(FAP_IOC_DMA_DEBUG, 0);
    if ( nRet == FAP_ERROR )
        fprintf( stderr, "%s: failed to print DMA info!\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: MemHandler
 * Description  : Processes the fap mem command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int MemHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fapDevIoctlRaw(FAP_IOC_MEM_DEBUG, 0);
    if ( nRet == FAP_ERROR )
        fprintf( stderr, "%s: failed to print MEM info!\n", g_PgmName );
    return nRet;
}



/*
 *------------------------------------------------------------------------------
 * Function Name: CpuHandler
 * Description  : Processes the fap cpu command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int CpuHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fapDevIoctlRaw( FAP_IOC_CPU, 0);
    if ( nRet == FAP_ERROR )
        fprintf( stderr, "%s: failed to show 4ke cpu utilization\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: SwqHandler
 * Description  : Processes the fap cpu command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int SwqHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fapDevIoctlRaw( FAP_IOC_SWQ, 0);
    if ( nRet == FAP_ERROR )
        fprintf( stderr, "%s: failed to show 4ke SWQueue's Info\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: StatusHandler
 * Description  : Processes the fap status command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int StatusHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fapDevIoctlRaw( FAP_IOC_STATUS, 0 );
    if ( nRet == FAP_ERROR )
        fprintf( stderr, "%s: failed to reset FAP\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: InitHandler
 * Description  : Processes the fap init command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int InitHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fapDevIoctlRaw( FAP_IOC_INIT,0 );
    if ( nRet == FAP_ERROR )
        fprintf( stderr, "%s: failed to initialize the FAP driver\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: EnableHandler
 * Description  : Processes the fap enable command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int EnableHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fapDevIoctlRaw( FAP_IOC_ENABLE, 0);
    if ( nRet == FAP_ERROR )
        fprintf( stderr, "%s: failed to enable FAP\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: DisableHandler
 * Description  : Processes the fap disable command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int DisableHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = fapDevIoctlRaw( FAP_IOC_DISABLE, 0 );
    if ( nRet == FAP_ERROR )
        fprintf( stderr, "%s: failed to disable FAP\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: PrintHandler
 * Description  : Processes the FAP print command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int PrintHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    int16_t sourcePort = -1;
    int16_t destPort = -1;
    uint32_t arg;

    if ( nNumOptions > 2 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }

    while (nNumOptions)
    {
        if ( strcmp( pOptions->pOptName, "--src" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: invalid source port number\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            sourcePort = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--dst" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: invalid destination port number\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            destPort = atoi( pOptions->pParms[0] );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n", g_PgmName, pOptions->pOptName );
            return FAP_INVALID_OPTION;
        }
        nNumOptions--;
        pOptions++;
    }

    arg = ((uint16_t)(sourcePort) << 16) | (uint16_t)(destPort & 0xFFFF);

    nRet = fapDevIoctlRaw( FAP_IOC_PRINT, arg );

    if ( nRet == FAP_ERROR )
    {
        fprintf( stderr, "%s: failed to set FAP Debug Level\n", g_PgmName );
    }

    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: MtuHandler
 * Description  : Processes the FAP mtu command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int MtuHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    int flow = -1;
    int fapIdx = -1;
    int mtu = 0;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }
    else if ( nNumOptions > 3 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }

    while (nNumOptions)
    {
        
        if ( strcmp( pOptions->pOptName, "--flow" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the flow number\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            flow = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--mtu" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the mtu\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }
            mtu = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--fap" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the fap index\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }
            fapIdx = atoi( pOptions->pParms[0] );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n", g_PgmName, pOptions->pOptName );
            return FAP_INVALID_OPTION;
        }
        nNumOptions--;
        pOptions++;
    }

    nRet = fapDevIoctlRaw( FAP_IOC_MTU, (mtu & 0xFFFF) | ((flow & 0x0FFF) << 16) | ((fapIdx & 0xF) << 28) );

    if ( nRet == FAP_ERROR )
    {
        fprintf( stderr, "%s: failed to set FAP Debug Level\n", g_PgmName );
    }

    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: FloodHandler
 * Description  : Processes the FAP flood command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int FloodHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    int port = -1;
    int mask = -1;
    int drop = 0;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }
    else if ( nNumOptions > 2 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }

    while (nNumOptions)
    {
        
        if ( strcmp( pOptions->pOptName, "--port" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the port number\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            port = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--mask" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the mask\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            mask = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--drop" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: Drop takes no parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            drop = 1;
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n", g_PgmName, pOptions->pOptName );
            return FAP_INVALID_OPTION;
        }
        nNumOptions--;
        pOptions++;
    }

    nRet = fapDevIoctlRaw( FAP_IOC_FLOODING_MASK,
                           ( (drop & 0xFF) << 16 | (port & 0xFF) << 8) | (mask & 0xFF) );

    if ( nRet == FAP_ERROR )
    {
        fprintf( stderr, "%s: failed to set FAP Debug Level\n", g_PgmName );
    }

    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: arlHandler
 * Description  : Processes the FAP ARL commands.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int arlHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = FAP_SUCCESS;
    fapIoctl_t ioctl_cmd = FAP_IOC_MAX;
    int arg = 0;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }
    else if ( nNumOptions > 1 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }

    while (nNumOptions)
    {
        if ( strcmp( pOptions->pOptName, "--flush" ) == 0 )
        {
            if ( pOptions->nNumParms > 1 )
            {
                fprintf( stderr, "%s: Too many arguments for ARL flush\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }
            else if ( pOptions->nNumParms == 0 )
            {
                arg = 0xFF;
            }
            else /* == 1 */
            {
                arg = atoi( pOptions->pParms[0] );
            }

            ioctl_cmd = FAP_IOC_ARL_FLUSH;
        }
        else if ( strcmp( pOptions->pOptName, "--show" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: ARL show takes no arguments\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            ioctl_cmd = FAP_IOC_ARL_SHOW;
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n", g_PgmName, pOptions->pOptName );
            return FAP_INVALID_OPTION;
        }
        nNumOptions--;
        pOptions++;
    }

    nRet = fapDevIoctlRaw(ioctl_cmd , arg);

    if ( nRet == FAP_ERROR )
    {
        fprintf( stderr, "%s: failed to process ARL command\n", g_PgmName );
    }

    return nRet;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: DymMemHandler
 * Description  : Processes the FAP mtu command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int DymMemHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    int type = 0;  //FAP_DM_DBG_DUMP_ALL_REGIONS
    int val = 0;
    int fapIdx = 0;
    int isShort = 1;

    if ( nNumOptions > 3 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }

    while (nNumOptions)
    {
        
        if ( strcmp( pOptions->pOptName, "--rgn" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the region\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if (strcmp( pOptions->pParms[0], "all" ) == 0)
            {
                type = 0; //FAP_DM_DBG_DUMP_ALL_REGIONS
            }
            else if (strcmp( pOptions->pParms[0], "dsp" ) == 0)
            {
                type = 1; //FAP_DM_DBG_DUMP_REGION
                val = 0; //FAP_DM_REGION_DSP
            }
            else if (strcmp( pOptions->pParms[0], "psm" ) == 0)
            {
                type = 1; //FAP_DM_DBG_DUMP_REGION
                val = 1; //FAP_DM_REGION_PSM
            }
            else if (strcmp( pOptions->pParms[0], "qsm" ) == 0)
            {
                type = 1; //FAP_DM_DBG_DUMP_REGION
                val = 2; //FAP_DM_REGION_QSM
            }
            else if (strcmp( pOptions->pParms[0], "hp" ) == 0)
            {
                type = 1; //FAP_DM_DBG_DUMP_REGION
                val = 3; //FAP_DM_REGION_HP
            }
        }
        else if ( strcmp ( pOptions->pOptName, "--short" ) == 0 )
        {
            isShort = 1;
        }
        else if ( strcmp ( pOptions->pOptName, "--long" ) == 0 )
        {
            isShort = 0;
        }
        else if ( strcmp( pOptions->pOptName, "--fap" ) == 0 || strcmp( pOptions->pOptName, "--fapIdx" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the fap index\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }
            fapIdx = atoi( pOptions->pParms[0] );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n", g_PgmName, pOptions->pOptName );
            return FAP_INVALID_OPTION;
        }
        nNumOptions--;
        pOptions++;
    }

    if (isShort)
        type |= 0x8; // FAP_DM_DBG_DUMP_REGION_SHORT_MASK;

    nRet = fapDevIoctlRaw( FAP_IOC_DM_DEBUG, (val & 0xFFFF) | ((type & 0x00FF) << 16) | ((fapIdx & 0xF) << 28) );

    if ( nRet == FAP_ERROR )
    {
        fprintf( stderr, "%s: failed to set FAP Debug Level\n", g_PgmName );
    }

    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: TmHandler
 * Description  : Processes the FAP Traffic Manager command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int TmHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = FAP_SUCCESS;
    fapIoctl_tm_t tm;
    char ifName[IFNAMSIZ] = FAPCTL_IFNAME_INVALID_STR;

    memset(&tm, FAP_DONT_CARE, sizeof(fapIoctl_tm_t));

    tm.shapingType = FAP_IOCTL_TM_SHAPING_TYPE_DISABLED;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return FAP_INVALID_OPTION;
    }

    while (nNumOptions)
    {
        if ( strcmp( pOptions->pOptName, "--on" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --on does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_MASTER_CONFIG;
            tm.enable = 1;
        }
        else if ( strcmp( pOptions->pOptName, "--off" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --off does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_MASTER_CONFIG;
            tm.enable = 0;
        }
        else if ( strcmp( pOptions->pOptName, "--ifcfg" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --ifcfg does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_PORT_CONFIG;
        }
        else if ( strcmp( pOptions->pOptName, "--queuecfg" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --queuecfg does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_CONFIG;
        }
        else if ( strcmp( pOptions->pOptName, "--queueweight" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --queueweight does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_WEIGHT;
        }
        else if ( strcmp( pOptions->pOptName, "--arbitercfg" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --arbitercfg does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_ARBITER_CONFIG;
        }
        else if ( strcmp( pOptions->pOptName, "--mode" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --mode does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_PORT_MODE;
        }
        else if ( strcmp( pOptions->pOptName, "--type" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --type does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_PORT_TYPE;
        }
        else if ( strcmp( pOptions->pOptName, "--apply" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --apply does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_PORT_APPLY;
        }
        else if ( strcmp( pOptions->pOptName, "--status" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --status does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_STATUS;
        }
        else if ( strcmp( pOptions->pOptName, "--stats" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --stats does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_STATS;
        }
        else if ( strcmp( pOptions->pOptName, "--maps" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --maps does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_DUMP_MAPS;
        }
        else if ( strcmp( pOptions->pOptName, "--tmq2swq" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --tmq2swq does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_MAP_TMQUEUE_TO_SWQUEUE;
        }
        else if ( strcmp( pOptions->pOptName, "--enable" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: enable does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_PORT_ENABLE;

            tm.enable = 1;
        }
        else if ( strcmp( pOptions->pOptName, "--disable" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: disable does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_PORT_ENABLE;

            tm.enable = 0;
        }
        else if ( strcmp( pOptions->pOptName, "--reset" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: enable does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_MODE_RESET;
        }
        else if ( strcmp( pOptions->pOptName, "--if" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the port number\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            strncpy(ifName, pOptions->pParms[0], IFNAMSIZ);
        }
        else if ( strcmp( pOptions->pOptName, "--queue" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the queue number\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.queue = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--channel" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the channel number\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.channel = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--swqueue" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the swqueue number\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.swQueue = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--auto" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --auto does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.mode = FAP_IOCTL_TM_MODE_AUTO;
        }
        else if ( strcmp( pOptions->pOptName, "--manual" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --manual does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.mode = FAP_IOCTL_TM_MODE_MANUAL;
        }
        else if ( strcmp( pOptions->pOptName, "--min" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --min does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.shaperType = FAP_IOCTL_TM_SHAPER_TYPE_MIN;
        }
        else if ( strcmp( pOptions->pOptName, "--max" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --max does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.shaperType = FAP_IOCTL_TM_SHAPER_TYPE_MAX;
        }
        else if ( strcmp( pOptions->pOptName, "--kbps" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify the bandwidth in kbps\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.kbps = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--mbs" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify MBS in bytes\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.mbs = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--weight" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify weight\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.weight = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--sp" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --sp does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.arbiterType = FAP_IOCTL_TM_ARBITER_TYPE_SP;
        }
        else if ( strcmp( pOptions->pOptName, "--wrr" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --wrr does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.arbiterType = FAP_IOCTL_TM_ARBITER_TYPE_WRR;
        }
        else if ( strcmp( pOptions->pOptName, "--spwrr" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --spwrr does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.arbiterType = FAP_IOCTL_TM_ARBITER_TYPE_SP_WRR;
        }
        else if ( strcmp( pOptions->pOptName, "--wfq" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --wfq does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.arbiterType = FAP_IOCTL_TM_ARBITER_TYPE_WFQ;
        }
        else if ( strcmp( pOptions->pOptName, "--lowprioq" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify lowprioq\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.arbiterArg = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--lan" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --lan does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.portType = FAP_IOCTL_TM_PORT_TYPE_LAN;
        }
        else if ( strcmp( pOptions->pOptName, "--wan" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --wan does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.portType = FAP_IOCTL_TM_PORT_TYPE_WAN;
        }
        else if ( strcmp( pOptions->pOptName, "--rate" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --rate does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.shapingType = FAP_IOCTL_TM_SHAPING_TYPE_RATE;
        }
        else if ( strcmp( pOptions->pOptName, "--ratio" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --ratio does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.shapingType = FAP_IOCTL_TM_SHAPING_TYPE_RATIO;
        }
        else if ( strcmp( pOptions->pOptName, "--queueprofile" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --queueprofile does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_PROFILE_CONFIG;
        }
        else if ( strcmp( pOptions->pOptName, "--queuedropalg" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --queuedropalg does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_QUEUE_DROP_ALG_CONFIG;
        }
        else if ( strcmp( pOptions->pOptName, "--xtmchanneldropalg" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --xtmchanneldropalg does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            if(!FAP_IS_DONT_CARE(tm.cmd))
            {
                fprintf( stderr, "%s: Mixed commands are not supported\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.cmd = FAP_IOCTL_TM_CMD_XTM_QUEUE_DROP_ALG_CONFIG;
        }
        else if ( strcmp( pOptions->pOptName, "--queueprofileid" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify queueprofileid\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.queueProfileId = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--dropprob" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify dropprob\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.dropProbability = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--minthreshold" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify minthreshold\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.minThreshold = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--maxthreshold" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify maxthreshold\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.maxThreshold = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--dt" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --dt does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.dropAlgorithm = FAP_IOCTL_TM_DROP_ALG_DT;
        }
        else if ( strcmp( pOptions->pOptName, "--red" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --red does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.dropAlgorithm = FAP_IOCTL_TM_DROP_ALG_RED;
        }
        else if ( strcmp( pOptions->pOptName, "--wred" ) == 0 )
        {
            if ( pOptions->nNumParms != 0 )
            {
                fprintf( stderr, "%s: --wred does not take parameters\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.dropAlgorithm = FAP_IOCTL_TM_DROP_ALG_WRED;
        }
        else if ( strcmp( pOptions->pOptName, "--queueprofileidhi" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify queueprofileidhi\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.queueProfileIdHi = atoi( pOptions->pParms[0] );
        }
        else if ( strcmp( pOptions->pOptName, "--prioritymask0" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify prioritymask0\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.priorityMask0 = (uint32_t)strtoul( pOptions->pParms[0] , NULL, 0 );
        }
        else if ( strcmp( pOptions->pOptName, "--prioritymask1" ) == 0 )
        {
            if ( pOptions->nNumParms != 1 )
            {
                fprintf( stderr, "%s: must specify prioritymask1\n", g_PgmName );
                return FAP_INVALID_OPTION;
            }

            tm.priorityMask1 = (uint32_t)strtoul( pOptions->pParms[0] , NULL, 0 );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n", g_PgmName, pOptions->pOptName );
            return FAP_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    /* Error checking */

    switch(tm.cmd)
    {
        case FAP_IOCTL_TM_CMD_MASTER_CONFIG:
            nRet = fapCtlTm_masterConfig(tm.enable);
            break;

        case FAP_IOCTL_TM_CMD_PORT_CONFIG:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.mode) ||
               FAP_IS_DONT_CARE(tm.kbps) || FAP_IS_DONT_CARE(tm.mbs))
            {
                fprintf( stderr, "Missing Arguments: TM Port Config\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_portConfig(ifName, tm.mode, tm.kbps,
                                       tm.mbs, tm.shapingType);
            break;

        case FAP_IOCTL_TM_CMD_PORT_MODE:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.mode))
            {
                fprintf( stderr, "Missing Arguments: TM Port Mode\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_setPortMode(ifName, tm.mode);
            break;

        case FAP_IOCTL_TM_CMD_PORT_TYPE:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.portType))
            {
                fprintf( stderr, "Missing Arguments: TM Port Type\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_setPortType(ifName, tm.portType);
            break;

        case FAP_IOCTL_TM_CMD_MODE_RESET:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.mode))
            {
                fprintf( stderr, "Missing Arguments: TM Port Reset\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_modeReset(ifName, tm.mode);
            break;

        case FAP_IOCTL_TM_CMD_PORT_ENABLE:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.mode))
            {
                fprintf( stderr, "Missing Arguments: TM Port Enable\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_portEnable(ifName, tm.mode, tm.enable);
            break;

        case FAP_IOCTL_TM_CMD_PORT_APPLY:
            if(!FAPCTL_IS_IFNAME_VALID(ifName))
            {
                fprintf( stderr, "Missing Arguments: TM Port Apply\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_apply(ifName);
            break;

        case FAP_IOCTL_TM_CMD_QUEUE_CONFIG:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.mode) ||
               FAP_IS_DONT_CARE(tm.queue) || FAP_IS_DONT_CARE(tm.shaperType) ||
               FAP_IS_DONT_CARE(tm.kbps) || FAP_IS_DONT_CARE(tm.mbs))
            {
                fprintf( stderr, "Missing Arguments: TM Queue Config\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_queueConfig(ifName, tm.mode, tm.queue,
                                        tm.shaperType, tm.kbps, tm.mbs);
            break;

        case FAP_IOCTL_TM_CMD_QUEUE_UNCONFIG:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.mode) ||
               FAP_IS_DONT_CARE(tm.queue))
            {
                fprintf( stderr, "Missing Arguments: TM Queue Unconfig\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_queueUnconfig(ifName, tm.mode, tm.queue);
            break;

        case FAP_IOCTL_TM_CMD_QUEUE_WEIGHT:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.mode) ||
               FAP_IS_DONT_CARE(tm.queue) || FAP_IS_DONT_CARE(tm.weight))
            {
                fprintf( stderr, "Missing Arguments: TM Queue Set Weight\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_setQueueWeight(ifName, tm.mode, tm.queue, tm.weight);
            break;

        case FAP_IOCTL_TM_CMD_ARBITER_CONFIG:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.mode) ||
               FAP_IS_DONT_CARE(tm.arbiterType) ||
               (tm.arbiterType == FAP_IOCTL_TM_ARBITER_TYPE_SP_WRR &&
                FAP_IS_DONT_CARE(tm.arbiterArg)))
            {
                fprintf( stderr, "Missing Arguments: TM Arbiter Configuration\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_arbiterConfig(ifName, tm.mode,
                                          tm.arbiterType, tm.arbiterArg);
            break;

        case FAP_IOCTL_TM_MAP_TMQUEUE_TO_SWQUEUE:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.queue) ||
               FAP_IS_DONT_CARE(tm.swQueue))
            {
                fprintf( stderr, "Missing Arguments: TM Queue To SW Queue Map\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_mapTmQueueToSwQueue(ifName, tm.queue, tm.swQueue);
            break;

        case FAP_IOCTL_TM_CMD_STATUS:
            fapCtlTm_status();
            break;

        case FAP_IOCTL_TM_CMD_STATS:
            fapCtlTm_stats(ifName);
            break;

        case FAP_IOCTL_TM_CMD_DUMP_MAPS:
            fapCtlTm_dumpMaps();
            break;

        case FAP_IOCTL_TM_CMD_QUEUE_PROFILE_CONFIG:
            if(FAP_IS_DONT_CARE(tm.queueProfileId) ||
               FAP_IS_DONT_CARE(tm.dropProbability) ||
               FAP_IS_DONT_CARE(tm.minThreshold) ||
               FAP_IS_DONT_CARE(tm.maxThreshold))
            {
                fprintf( stderr, "Missing Arguments: TM Queue DropAlg Config\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_queueProfileConfig(tm.queueProfileId, tm.dropProbability,
                                               tm.minThreshold, tm.maxThreshold);
            break;

        case FAP_IOCTL_TM_CMD_QUEUE_DROP_ALG_CONFIG:
            if(!FAPCTL_IS_IFNAME_VALID(ifName) || FAP_IS_DONT_CARE(tm.queue) ||
               FAP_IS_DONT_CARE(tm.dropAlgorithm))
            {
                fprintf( stderr, "Missing Arguments: TM Queue DropAlg Config\n");
                return FAP_INVALID_OPTION;
            }
            if ((tm.dropAlgorithm == FAP_IOCTL_TM_DROP_ALG_RED) &&
                FAP_IS_DONT_CARE(tm.queueProfileId))
            {
                fprintf( stderr, "Missing Arguments: TM Queue DropAlg Config\n");
                return FAP_INVALID_OPTION;
            }
            if ((tm.dropAlgorithm == FAP_IOCTL_TM_DROP_ALG_WRED) &&
                (FAP_IS_DONT_CARE(tm.queueProfileId) || FAP_IS_DONT_CARE(tm.queueProfileIdHi)))
            {
                fprintf( stderr, "Missing Arguments: TM Queue DropAlg Config\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_queueDropAlgConfig(ifName, tm.queue,
                                               tm.dropAlgorithm, tm.queueProfileId,
                                               tm.queueProfileIdHi,
                                               tm.priorityMask0,
                                               tm.priorityMask1);
            break;

        case FAP_IOCTL_TM_CMD_XTM_QUEUE_DROP_ALG_CONFIG:
            if(FAP_IS_DONT_CARE(tm.channel) || FAP_IS_DONT_CARE(tm.dropAlgorithm))
            {
                fprintf( stderr, "Missing Arguments: TM XTM Channel DropAlg Config\n");
                return FAP_INVALID_OPTION;
            }
            if ((tm.dropAlgorithm == FAP_IOCTL_TM_DROP_ALG_RED) &&
                FAP_IS_DONT_CARE(tm.queueProfileId))
            {
                fprintf( stderr, "Missing Arguments: TM XTM Channel DropAlg Config\n");
                return FAP_INVALID_OPTION;
            }
            if ((tm.dropAlgorithm == FAP_IOCTL_TM_DROP_ALG_WRED) &&
                (FAP_IS_DONT_CARE(tm.queueProfileId) || FAP_IS_DONT_CARE(tm.queueProfileIdHi)))
            {
                fprintf( stderr, "Missing Arguments: TM XTM Channel DropAlg Config\n");
                return FAP_INVALID_OPTION;
            }
            nRet = fapCtlTm_xtmChannelDropAlgConfig(tm.channel,
                                                    tm.dropAlgorithm, tm.queueProfileId,
                                                    tm.queueProfileIdHi,
                                                    tm.priorityMask0,
                                                    tm.priorityMask1);
            break;

        default:
            fprintf( stderr, "Missing TM Command");
            return FAP_INVALID_OPTION;
    }

    if ( nRet == FAP_ERROR )
    {
        fprintf( stderr, "%s: FAP TM Command <%d> failed\n", g_PgmName, tm.cmd );
    }

    return nRet;
}

int Fap4keTest(POPTION_INFO pOptions, int nNumOptions)
{    
    return fapDevIoctlRaw( FAP_IOC_DO_4KE_TEST, 0 );
}
