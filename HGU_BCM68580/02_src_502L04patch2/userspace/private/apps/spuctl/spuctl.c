/***********************************************************************
 *
 *  Copyright (c) 2007-2010  Broadcom Corporation
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
 * File Name  : spuctl.c
 *
 * Description: Linux command line utility that controls the SPU driver
 *              - starts and stops the SPU subsystem.
 ***************************************************************************/

/** Includes. **/

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include "devctl_spu.h"
#include "bcmspucfg.h"

/** Defines. **/

#define SPU_VERSION                      "1.0"

/* Limit values */
#define CMD_NAME_LEN                        16
#define MAX_OPTS                            16
#define MAX_SUB_CMDS                        16
#define MAX_PARMS                           16

/* Argument type values. */
#define ARG_TYPE_COMMAND                    1
#define ARG_TYPE_OPTION                     2
#define ARG_TYPE_PARAMETER                  3

/** More Typedefs. **/

extern int SpuCtl_Test(void);

typedef struct
{
    char *pszOptName;
    char *pszParms[MAX_PARMS];
    int nNumParms;
} OPTION_INFO, *POPTION_INFO;

typedef int (*FN_COMMAND_HANDLER) (POPTION_INFO pOptions, int nNumOptions);

typedef struct
{
    char szCmdName[CMD_NAME_LEN];
    char *pszOptionNames[MAX_OPTS];
    FN_COMMAND_HANDLER pfnCmdHandler;
} COMMAND_INFO, *PCOMMAND_INFO;

/** Prototypes. **/

static int GetArgType( char *pszArg, PCOMMAND_INFO pCmds, char **ppszOptions );
static PCOMMAND_INFO GetCommand( char *pszArg, PCOMMAND_INFO pCmds );
static int ProcessCommand( PCOMMAND_INFO pCmd, int argc, char **argv,
    PCOMMAND_INFO pCmds, int *pnArgNext );
static int StartHandler( POPTION_INFO pOptions, int nNumOptions );
#if defined(SUPPORT_SPU_TEST)
static int TestHandler( POPTION_INFO pOptions, int nNumOptions );
#endif
static int StopHandler( POPTION_INFO pOptions, int nNumOptions );
static int ShowHandler( POPTION_INFO pOptions, int nNumOptions );
static int HelpHandler( POPTION_INFO pOptions, int nNumOptions );

/** Globals. **/

static COMMAND_INFO g_Cmds[] =
    {
     {"start", {""}, StartHandler},
     {"stop", {""}, StopHandler},
     {"showstats", {""}, ShowHandler},
#if defined(SUPPORT_SPU_TEST)
     {"test", {""}, TestHandler},
#endif      
     {"--help", {""}, HelpHandler},
     {""}
    } ;

static char g_szPgmName [80] = {0} ;


/***************************************************************************
 * Function Name: main
 * Description  : Main program function.
 * Returns      : 0 - success, non-0 - error.
 ***************************************************************************/
int main(int argc, char **argv)
{
    int nExitCode = 0;
    PCOMMAND_INFO pCmd;

    /* Save the name that started this program into a global variable. */
    strcpy( g_szPgmName, *argv );

    if( argc == 1 )
        HelpHandler( NULL, 0 );

    argc--, argv++;
    while( argc && nExitCode == 0 )
    {
        if( GetArgType( *argv, g_Cmds, NULL ) == ARG_TYPE_COMMAND )
        {
            int argnext = 0;
            pCmd = GetCommand( *argv, g_Cmds );
            nExitCode = ProcessCommand(pCmd, --argc, ++argv, g_Cmds, &argnext);
            argc -= argnext;
            argv += argnext;
        }
        else
        {
            nExitCode = -EINVAL;
            fprintf( stderr, "%s: invalid command\n", g_szPgmName );
        }
    }

    exit( nExitCode );
}


/***************************************************************************
 * Function Name: GetArgType
 * Description  : Determines if the specified command line argument is a
 *                command, option or option parameter.
 * Returns      : ARG_TYPE_COMMAND, ARG_TYPE_OPTION, ARG_TYPE_PARAMETER
 ***************************************************************************/
static int GetArgType( char *pszArg, PCOMMAND_INFO pCmds, char **ppszOptions )
{
    int nArgType = ARG_TYPE_PARAMETER;

    /* See if the argument is a option. */
    if( ppszOptions )
    {
        do
        {
            if( !strcmp( pszArg, *ppszOptions) )
            {
                nArgType = ARG_TYPE_OPTION;
                break;
            }
        } while( *++ppszOptions );
    }

    /* Next, see if the argument is an command. */
    if( nArgType == ARG_TYPE_PARAMETER )
    {
        while( pCmds->szCmdName[0] != '\0' )
        {
            if( !strcmp( pszArg, pCmds->szCmdName ) )
            {
                nArgType = ARG_TYPE_COMMAND;
                break;
            }

            pCmds++;
        }
    }

    /* Otherwise, assume that it is a parameter. */

    return( nArgType );
} /* GetArgType */


/***************************************************************************
 * Function Name: GetCommand
 * Description  : Returns the COMMAND_INFO structure for the specified
 *                command name.
 * Returns      : COMMAND_INFOR structure pointer
 ***************************************************************************/
static PCOMMAND_INFO GetCommand( char *pszArg, PCOMMAND_INFO pCmds )
{
    PCOMMAND_INFO pCmd = NULL;

    while( pCmds->szCmdName[0] != '\0' )
    {
        if( !strcmp( pszArg, pCmds->szCmdName ) )
        {
            pCmd = pCmds;
            break;
        }

        pCmds++;
    }

    return( pCmd );
} /* GetCommand */


/***************************************************************************
 * Function Name: ProcessCommand
 * Description  : Gets the options and option paramters for a command and
 *                calls the command handler function to process the command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int ProcessCommand( PCOMMAND_INFO pCmd, int argc, char **argv,
    PCOMMAND_INFO pCmds, int *pnArgNext )
{
    int nRet = 0;
    OPTION_INFO OptInfo[MAX_OPTS];
    OPTION_INFO *pCurrOpt = NULL;
    int nNumOptInfo = 0;
    int nArgType = 0;

    memset( OptInfo, 0x00, sizeof(OptInfo) );
    *pnArgNext = 0;

    do
    {
        if( argc == 0 )
            break;

        nArgType = GetArgType( *argv, pCmds, pCmd->pszOptionNames );
        switch( nArgType )
        {
        case ARG_TYPE_OPTION:
            if( nNumOptInfo < MAX_OPTS )
            {
                pCurrOpt = &OptInfo[nNumOptInfo++];
                pCurrOpt->pszOptName = *argv;
            }
            else
            {
                nRet = -EINVAL;
                fprintf( stderr, "%s: too many options\n", g_szPgmName );
            }
            (*pnArgNext)++;
            break;

        case ARG_TYPE_PARAMETER:
            if( pCurrOpt && pCurrOpt->nNumParms < MAX_PARMS )
            {
                pCurrOpt->pszParms[pCurrOpt->nNumParms++] = *argv;
            }
            else
            {
                if( pCurrOpt )
                {
                    nRet = -EINVAL;
                    fprintf( stderr, "%s: invalid option\n", g_szPgmName );
                }
                else
                {
                    nRet = -EINVAL;
                    fprintf( stderr, "%s: too many options\n", g_szPgmName );
                }
            }
            (*pnArgNext)++;
            break;

        case ARG_TYPE_COMMAND:
            /* The current command is done. */
            break;
        }

        argc--, argv++;
    } while( (nRet == 0) && (nArgType != ARG_TYPE_COMMAND) );


    if( nRet == 0 )
        nRet = (*pCmd->pfnCmdHandler) (OptInfo, nNumOptInfo);
    else
        HelpHandler( NULL, 0 );

    return( nRet );
} /* ProcessCommand */

/***************************************************************************
 * Function Name: StartHandler
 * Description  : Processes the spuctl start command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int StartHandler( POPTION_INFO pOptions, int nNumOptions )
{
    int err;
    system("setkey -FP");
    system("setkey -F");
    system("setkey -f /var/setkey.conf");

    err = SpuCtl_Initialize();
    if ( err )
    {
        printf("Start spu command failed\n");
    }
    return( err );
} /* StartHandler */

#if defined(SUPPORT_SPU_TEST)
/***************************************************************************
 * Function Name: TestHandler
 * Description  : Processes the spuctl Encrypt command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int TestHandler( POPTION_INFO pOptions, int nNumOptions )
{
    int err;
    err = SpuCtl_Test();
    if ( err )
    {
        printf("Test spu command failed\n");
    }
    return( err );
} /* ShowHandler */
#endif

/***************************************************************************
 * Function Name: StopHandler
 * Description  : Processes the spuctl stop command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int StopHandler( POPTION_INFO pOptions, int nNumOptions )
{
    int err;
    system("setkey -FP");
    system("setkey -F");
    sleep(1);
    err = SpuCtl_Uninitialize();
    if ( err )
    {
        printf("Stop spu command failed\n");
    }
    return( err );
} /* StopHandler */

/***************************************************************************
 * Function Name: ShowHandler
 * Description  : Processes the spuctl show command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int ShowHandler( POPTION_INFO pOptions, int nNumOptions )
{
    SPU_STAT_PARMS stats;
    int ret = 0;
    
    ret = SpuCtl_Show(&stats);
    if ( ret )
    {
        printf("Show spu stats command failed\n");
    }
    else
    {
       printf("Encryption stats\n");
       printf("     Ingress %d\n", stats.encIngress);
       printf("     Fallback %d\n", stats.encFallback);
       printf("     Egress %d\n", stats.encSpuEgress);
       printf("     Error %d\n", stats.encErrors);
       printf("     Dropped %d\n", stats.encDrops);

       printf("Decryption stats\n");
       printf("     Ingress %d\n", stats.decIngress);
       printf("     Fallback %d\n", stats.decFallback);
       printf("     Egress %d\n", stats.decSpuEgress);
       printf("     Error %d\n", stats.decErrors);
       printf("     Dropped %d\n", stats.decDrops);
    }
    return(ret );

} /* ShowHandler */

/***************************************************************************
 * Function Name: HelpHandler
 * Description  : Processes the spuctl help command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int HelpHandler( POPTION_INFO pOptions, int nNumOptions )
{
    fprintf( stderr,
    "Usage: %s start -> Initialize SPU Device\n"
    "       %s stop  -> Un-Initialize SPU Device\n"
    "       %s showstats -> Show SPU Statistics\n"
#if defined(SUPPORT_SPU_TEST)
    "       %s test ->  test SPU Device\n"
#endif
    "       %s --help\n",
    g_szPgmName, g_szPgmName, g_szPgmName,
#if defined(SUPPORT_SPU_TEST)
    g_szPgmName,
#endif
    g_szPgmName);

    return( 0 );
} /* HelpHandler */
