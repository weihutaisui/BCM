/***********************************************************************
 *
 *  Copyright (c) 2008-2010  Broadcom Corporation
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

/** Includes. **/

#include <stdio.h>
#include "bcmpwrmngtcfg.h"
#include "devctl_pwrmngt.h"
#include "ethswctl_api.h"

/** Defines. **/

#define PWRMNGT_VERSION                      "1.0"

/* Limit values */
#define CMD_NAME_LEN                        64
#define MAX_OPTS                            64
#define MAX_SUB_CMDS                        64
#define MAX_PARMS                           64

/* Argument type values. */
#define ARG_TYPE_COMMAND                    1
#define ARG_TYPE_OPTION                     2
#define ARG_TYPE_PARAMETER                  3

/** More Typedefs. **/
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
static int StopHandler( POPTION_INFO pOptions, int nNumOptions );
static int ConfigHandler( POPTION_INFO pOptions, int nNumOptions );
static int ShowHandler( POPTION_INFO pOptions, int nNumOptions );
static int HelpHandler( POPTION_INFO pOptions, int nNumOptions );

/** Globals. **/

static COMMAND_INFO g_Cmds[] =
    {
     {"start", {""}, StartHandler},
     {"config", {"--cpuspeed", "--cpur4kwait", "--wait", "--sr",
                 "--eth", "--ethapd", "--eee", "--dgm", 
#if defined(SUPPORT_MOCA_AVS)
                 "--mocaavs",
#else
                 "--avs",
#endif
                 "--avslog",
                 ""}, ConfigHandler},
     {"show", {""}, ShowHandler},
     {"stop", {""}, StopHandler},
     {"help", {""}, HelpHandler},
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
            nExitCode = -1;
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
                nRet = -1;
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
                    nRet = -1;
                    fprintf( stderr, "%s: invalid option\n", g_szPgmName );
                }
                else
                {
                    nRet = -1;
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
    } while( nRet == 0 && nArgType != ARG_TYPE_COMMAND );


    if( nRet == 0 )
        nRet = (*pCmd->pfnCmdHandler) (OptInfo, nNumOptInfo);
    else
        HelpHandler( NULL, 0 );

    return( nRet );
} /* ProcessCommand */

/***************************************************************************
 * Function Name: StartHandler
 * Description  : Processes and intialize the Power Management start 
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int StartHandler( POPTION_INFO pOptions, int nNumOptions )
{
    int nRet = 0;
    PWRMNGT_CONFIG_PARAMS configParms;

    memset(&configParms, 0, sizeof(PWRMNGT_CONFIG_PARAMS));
    /* set default values */
    configParms.cpuspeed                 = 256; // Using 256 to represent automated mode
    configParms.cpur4kwait               = PWRMNGT_ENABLE;
    configParms.dramSelfRefresh          = PWRMNGT_ENABLE;

    if( nRet == 0 ) {

        nRet = PwrMngtCtl_Initialize (&configParms);
    }

    return( nRet );
} /* StartHandler */

/***************************************************************************
 * Function Name: StopHandler
 * Description  : Processes the pmctl stop command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int StopHandler( POPTION_INFO pOptions, int nNumOptions )
{
    return( PwrMngtCtl_Uninitialize () );
} /* StopHandler */

/***************************************************************************
 * Function Name: ConfigHandler
 * Description  : Processes the pmctl config command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int ConfigHandler ( POPTION_INFO pOptions, int nNumOptions )
{
    int nRet = 0;
    PWRMNGT_CONFIG_PARAMS configParms;
    UINT32 configMask ;

    memset (&configParms, 0x00, sizeof(configParms)) ;
    configMask = 0x0 ;

    while( nRet == 0 && nNumOptions )
    {
       if( !strcmp(pOptions->pszOptName, "--cpuspeed") ) {

          if( pOptions->nNumParms == 1)
          {
              if((atoi(pOptions->pszParms[0]) < PWRMNGT_MIPS_CPU_SPEED_MIN_VAL) || 
                (atoi(pOptions->pszParms[0]) > PWRMNGT_MIPS_CPU_SPEED_MAX_VAL))
                nRet = -1 ;
             else
                configParms.cpuspeed = atoi(pOptions->pszParms[0]);
          }
          else {
             nRet = -1;
          }

          configMask |= PWRMNGT_CFG_PARAM_CPUSPEED_MASK ;
          if( nRet != 0 )
             fprintf( stderr, "%s: invalid parameter for option %s\n",
                   g_szPgmName, pOptions->pszOptName );
       }
       else if( !strcmp(pOptions->pszOptName, "--cpur4kwait") ||
                !strcmp(pOptions->pszOptName, "--wait"))
       {
          if(pOptions->nNumParms == 1) 
          {
             if(!strcmp(pOptions->pszParms[0], "on"))
                configParms.cpur4kwait = PWRMNGT_ENABLE;
             else if( !strcmp( pOptions->pszParms[0], "off" ) )
                configParms.cpur4kwait = PWRMNGT_DISABLE;
             else
                nRet = -1;
           }
           else {
                nRet = -1;
           }
           configMask |= PWRMNGT_CFG_PARAM_CPU_R4K_WAIT_MASK ;
           if( nRet != 0 )
             fprintf( stderr, "%s: invalid parameter for option %s\n",
                   g_szPgmName, pOptions->pszOptName );
       }
       else if( !strcmp(pOptions->pszOptName, "--sr") )
       {
          if(pOptions->nNumParms == 1) 
          {
             if(!strcmp(pOptions->pszParms[0], "on"))
                configParms.dramSelfRefresh = PWRMNGT_ENABLE;
             else if( !strcmp( pOptions->pszParms[0], "off" ) )
                configParms.dramSelfRefresh = PWRMNGT_DISABLE;
             else
                nRet = -1;
           }
           else {
                nRet = -1;
           }
           configMask |= PWRMNGT_CFG_PARAM_MEM_SELF_REFRESH_MASK ;
           if( nRet != 0 )
             fprintf( stderr, "%s: invalid parameter for option %s\n",
                   g_szPgmName, pOptions->pszOptName );
       } 
#if defined(SUPPORT_ETH_PWRSAVE)
       else if( !strcmp(pOptions->pszOptName, "--ethapd"))
       {
          if(pOptions->nNumParms == 1) 
          {
             if(!strcmp(pOptions->pszParms[0], "on"))
                bcm_phy_apd_set(1);
             else if( !strcmp( pOptions->pszParms[0], "off" ) )
                bcm_phy_apd_set(0);
             else
                nRet = -1;
           }
           else {
                nRet = -1;
           }
           if( nRet != 0 )
             fprintf( stderr, "%s: invalid parameter for option %s\n",
                   g_szPgmName, pOptions->pszOptName );
       }      
#endif
#if defined(SUPPORT_ENERGY_EFFICIENT_ETHERNET)
       else if( !strcmp(pOptions->pszOptName, "--eee"))
       {
          if(pOptions->nNumParms == 1) 
          {
             if(!strcmp(pOptions->pszParms[0], "on"))
                bcm_phy_eee_set(1);
             else if( !strcmp( pOptions->pszParms[0], "off" ) )
                bcm_phy_eee_set(0);
             else
                nRet = -1;
           }
           else {
                nRet = -1;
           }
           if( nRet != 0 )
             fprintf( stderr, "%s: invalid parameter for option %s\n",
                   g_szPgmName, pOptions->pszOptName );
       }      
#endif
#if defined(SUPPORT_ETH_DEEP_GREEN_MODE)
       else if( !strcmp(pOptions->pszOptName, "--dgm"))
       {
          if(pOptions->nNumParms == 1) 
          {
             if(!strcmp(pOptions->pszParms[0], "on"))
                bcm_DeepGreenMode_set(1);
             else if( !strcmp( pOptions->pszParms[0], "off" ) )
                bcm_DeepGreenMode_set(0);
             else
                nRet = -1;
           }
           else {
                nRet = -1;
           }
           if( nRet != 0 )
             fprintf( stderr, "%s: invalid parameter for option %s\n",
                   g_szPgmName, pOptions->pszOptName );
       }
#endif
#if defined(SUPPORT_MOCA_AVS) || defined(SUPPORT_AVS_PWRSAVE)
#if defined(SUPPORT_MOCA_AVS)
       else if( !strcmp(pOptions->pszOptName, "--mocaavs") )
#else
       else if( !strcmp(pOptions->pszOptName, "--avs") )
#endif
       {
          if(pOptions->nNumParms == 1) 
          {
             if(!strcmp(pOptions->pszParms[0], "on"))
                configParms.avs = PWRMNGT_ENABLE;
             else if( !strcmp( pOptions->pszParms[0], "off" ) )
                configParms.avs = PWRMNGT_DISABLE;
             else if( !strcmp( pOptions->pszParms[0], "stopped" ) )
                configParms.avs = PWRMNGT_STOP;
             else if( !strcmp( pOptions->pszParms[0], "deep" ) )
                configParms.avs = PWRMNGT_ENABLE_DEEP;
             else
                nRet = -1;
           }
           else {
                nRet = -1;
           }
           configMask |= PWRMNGT_CFG_PARAM_MEM_AVS_MASK;
           if( nRet != 0 )
             fprintf( stderr, "%s: invalid parameter %s for option %s\n",
                   g_szPgmName, pOptions->pszParms[0], pOptions->pszOptName );
       }      
       else if( !strcmp(pOptions->pszOptName, "--avslog") )
       {
          if(pOptions->nNumParms == 1) 
          {
              configParms.avsLog = atoi(pOptions->pszParms[0]);
              configMask |= PWRMNGT_CFG_PARAM_MEM_AVS_LOG_MASK;
          }
       }      
#endif
       else {
           nRet = -1;

           fprintf( stderr, "%s: invalid parameter for option %s\n",
                   g_szPgmName, pOptions->pszOptName );
       }
       nNumOptions--;
       pOptions++;
    }

    if( (nRet == 0) && configMask ) {

        nRet = PwrMngtCtl_SetConfig( &configParms, configMask, NULL ) ;
    }

    return( nRet );
} /* ConfigHandler */

/***************************************************************************
 * Function Name: ShowHandler
 * Description  : Processes the pmctl show command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int ShowHandler( POPTION_INFO pOptions, int nNumOptions )
{
    PWRMNGT_CONFIG_PARAMS configParms;

    int ret = 0;

    memset(&configParms, 0, sizeof(PWRMNGT_CONFIG_PARAMS));
    ret = PwrMngtCtl_GetConfig(&configParms, PWRMNGT_CFG_PARAM_ALL_MASK);

    printf("\nPower Management Configuration\n");
    printf("%-26s %s\n", "Functional Block", "Status");
#if defined(SUPPORT_HOSTMIPS_PWRSAVE)
    if (configParms.cpuspeed == 256) {
        if (configParms.cpur4kwait) {
            printf("%-26s %s\n", "CPU Speed Divisor", "auto");
        }
        else {
            printf("%-26s %s\n", "CPU Speed Divisor", "auto (wait disabled)");
        }
    }
    else {
        printf("%-26s %d\n", "CPU Speed Divisor", configParms.cpuspeed);
    }
#endif
    printf("%-26s %s\n", "CPU Wait",
             configParms.cpur4kwait == PWRMNGT_ENABLE ? "ENABLED" : "DISABLED");
#if defined(SUPPORT_DDR_SELF_REFRESH_PWRSAVE)
    printf("%-26s %s\n", "DRAM Self-Refresh",
             configParms.dramSelfRefresh == PWRMNGT_ENABLE ? "ENABLED" : "DISABLED");
#endif
#if defined(SUPPORT_ETH_PWRSAVE)
    {
        unsigned int apd_en;
        bcm_phy_apd_get(&apd_en);
        printf("%-26s %s\n", "Ethernet Auto Power Down",
                 apd_en ? "ENABLED" : "DISABLED");
    }
#endif
#if defined(SUPPORT_ENERGY_EFFICIENT_ETHERNET)
    {
        unsigned int eee_en;
        bcm_phy_eee_get(&eee_en);
        printf("%-26s %s\n", "Energy Efficient Ethernet",
                 eee_en ? "ENABLED" : "DISABLED");
    }
#endif
#if defined(SUPPORT_ETH_DEEP_GREEN_MODE)
    {
        unsigned int dgm_en = 0;
        unsigned int dgm_status_en = 1;
        bcm_DeepGreenMode_get(&dgm_en);
        bcm_DeepGreenMode_get(&dgm_status_en);
        printf("%-26s %s (status:%s)\n", "Switch Deep Green Mode",
                 dgm_en ? "ENABLED" : "DISABLED",
                 dgm_status_en ? "Activated" : "Deactivated");
    }
#endif
#if defined(SUPPORT_MOCA_AVS) || defined(SUPPORT_AVS_PWRSAVE)
    printf("%-26s %s\n", "Adaptive Voltage Scaling",
             configParms.avs == PWRMNGT_ENABLE ? "ENABLED" :
            (configParms.avs == PWRMNGT_DISABLE ? "DISABLED" :
            (configParms.avs == PWRMNGT_STOP ? "STOPPED" : "DEEP")));
    printf("%-26s %d\n", "AVS Log Period (sec)", configParms.avsLog);
#endif

    return( ret );

} /* ShowHandler */
/***************************************************************************
 * Function Name: HelpHandler
 * Description  : Processes the mocactl help command.
 * Returns      : 0 - success, non-0 - error
 ***************************************************************************/
static int HelpHandler( POPTION_INFO pOptions, int nNumOptions )
{
    fprintf( stderr,
    "Usage: %s start\n"
    "       %s stop\n"
    "       %s config ...\n"
#if defined(SUPPORT_HOSTMIPS_PWRSAVE)
    "                 [--cpuspeed <0|1|2|4|8|256>]: 1-8 -> sets speed @ 1/1, 1/2, 1/4 or 1/8th of fullspeed in ASYNC mode.\n"
    "                                               0   -> 1/1 in SYNC mode\n"
    "                                               256 -> 1/8 ASYNC when entering wait, 1/1 SYNC otherwise.\n"
#endif
    "                 [--wait on|off]             : keeps CPU in sleep state without spinning\n"
#if defined(SUPPORT_DDR_SELF_REFRESH_PWRSAVE)
 #if defined(SUPPORT_DDR_AUTO_SELF_REFRESH)
    "                 [--sr on|off]               : DRAM Self Refresh Mode Enable\n"
 #else
    "                 [--sr on|off]               : DRAM Self Refresh Mode Enable (requires Wait enabled)\n"
 #endif

#endif
#if defined(SUPPORT_ETH_PWRSAVE)
    "                 [--ethapd on|off]           : Ethernet Auto Power Down Enable\n"
#endif
#if defined(SUPPORT_ENERGY_EFFICIENT_ETHERNET)
    "                 [--eee    on|off]           : Energy Efficient Ethernet Enable (all ports)\n"
#endif
#if defined(SUPPORT_ETH_DEEP_GREEN_MODE)
    "                 [--dgm on|off]              : Switch Deep Green Mode Enable\n"
#endif
#if defined(SUPPORT_MOCA_AVS) || defined(SUPPORT_AVS_PWRSAVE)
#if defined(SUPPORT_MOCA_AVS)
    "                 [--mocaavs on|off|stopped|deep] : Adaptive Voltage Scaling Enable for MoCA chip\n"
#else
    "                 [--avs on|off|stopped|deep] : Adaptive Voltage Scaling Enable\n"
#endif
    "                                               on      -> to enable AVS while guaranteeing best performance\n"
    "                                               stopped -> for testing only, to hold the voltage to its current setting\n"
    "                                               deep    -> for more savings, with possible impact on WiFi data rates\n"
    "                                                          (The deep option only makes a difference on some chips)\n"
#endif
    "       %s show\n"
    "       %s help\n",
    g_szPgmName, g_szPgmName, g_szPgmName, g_szPgmName, g_szPgmName) ;

    return( 0 );
} /* HelpHandler */
