/***********************************************************************
 *
 *  Copyright (c) 2014  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard

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


/*****************************************************************************
*    Description:
*
*      TMCtl command line utility.
*
*****************************************************************************/

/* ---- Include Files ----------------------------------------------------- */

#include "tmctl_cmds.h"


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Constants and Types --------------------------------------- */

#define ARG_TYPE_COMMAND        1
#define ARG_TYPE_OPTION         2
#define ARG_TYPE_PARAMETER      3


/* ---- Private Function Prototypes --------------------------------------- */

static int tmctl_getArgType(char *pszArg, PCOMMAND_INFO pCmds,
  char **ppszOptions);
static PCOMMAND_INFO tmctl_getCommand(char *pszArg, PCOMMAND_INFO pCmds);
static int tmctl_processCommand(PCOMMAND_INFO pCmd, int argc, char **argv,
  PCOMMAND_INFO pCmds, int *pnArgNext);
static void tmctl_dumpOption(char *pszCmdName, POPTION_INFO pOptions,
  int optNum);


/* ---- Private Variables ------------------------------------------------- */

static char *pgmName = "tmctl";


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  main
*  PURPOSE:   TMCtl main program function.
*  PARAMETERS:
*      argc - number of command line arguments.
*      argv - array of command line argument strings.
*  RETURNS:
*      0 - success, others - error.
*  NOTES:
*      None.
*****************************************************************************/
#ifdef BUILD_STATIC
int tmctl_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    PCOMMAND_INFO pCmd;
    int argnext = 0;
    int ret = 0;

    if (argc == 1)
    {
        tmctl_usage();
        exit(ret);
    }

    argc--;
    argv++;
    while (argc && (ret == 0))
    {
        if (tmctl_getArgType(*argv, tmctl_cmds, NULL) == ARG_TYPE_COMMAND)
        {
            argnext = 0;
            pCmd = tmctl_getCommand(*argv, tmctl_cmds);
            argc--;
            argv++;
            ret = tmctl_processCommand(pCmd, argc, argv, tmctl_cmds, &argnext);
            printf("ret code = %d.\n", ret);
            argc -= argnext;
            argv += argnext;
        }
        else
        {
            ret = TMCTL_INVALID_COMMAND;
            fprintf(stderr, "%s: invalid command.\n", pgmName);
        }
    }

    exit(ret);
}

/*****************************************************************************
*  FUNCTION:  tmctl_getArgType
*  PURPOSE:   Determines if the specified command line argument is a command,
*             option, or option parameter.
*  PARAMETERS:
*      pszArg - pointer to argument string.
*      pCmds - pointer to command info.
*      ppszOptions - pointer to array of options.
*  RETURNS:
*      ARG_TYPE_COMMAND, ARG_TYPE_OPTION, ARG_TYPE_PARAMETER.
*  NOTES:
*      None.
*****************************************************************************/
static int tmctl_getArgType(char *pszArg, PCOMMAND_INFO pCmds,
  char **ppszOptions)
{
    int nArgType = ARG_TYPE_PARAMETER;

    if (ppszOptions)
    {
        do
        {
            if (!strcmp(pszArg, *ppszOptions))
            {
                nArgType = ARG_TYPE_OPTION;
                break;
            }
        } while (*++ppszOptions);
    }

    if (nArgType == ARG_TYPE_PARAMETER)
    {
        while (pCmds->szCmdName[0] != '\0')
        {
            if (!strcmp(pszArg, pCmds->szCmdName))
            {
                nArgType = ARG_TYPE_COMMAND;
                break;
            }
            pCmds++;
        }
    }

    return nArgType;
}

/*****************************************************************************
*  FUNCTION:  tmctl_getCommand
*  PURPOSE:   Returns the COMMAND_INFO structure for the specified command.
*  PARAMETERS:
*      pszArg - pointer to command name.
*      pCmds - pointer to command info.
*  RETURNS:
*      Pointer to command info.
*  NOTES:
*      None.
*****************************************************************************/
static PCOMMAND_INFO tmctl_getCommand(char *pszArg, PCOMMAND_INFO pCmds)
{
    PCOMMAND_INFO pCmd = NULL;

    while (pCmds->szCmdName[0] != '\0')
    {
        if (!strcmp(pszArg, pCmds->szCmdName))
        {
            pCmd = pCmds;
            break;
        }
        pCmds++;
    }

    return pCmd;
}

/*****************************************************************************
*  FUNCTION:  tmctl_processCommand
*  PURPOSE:   Gets the options and option parameters for a command and
*             calls the command handler function to process the command.
*  PARAMETERS:
*      pCmd - pointer to command info.
*      argc - number of command line arguments.
*      argv - array of command line argument strings.
*      pCmds - pointer to command info.
*      pnArgNext - pointer to next argument.
*  RETURNS:
*      0 - success, others - error.
*  NOTES:
*      None.
*****************************************************************************/
static int tmctl_processCommand(PCOMMAND_INFO pCmd, int argc, char **argv,
  PCOMMAND_INFO pCmds, int *pnArgNext)
{
    int ret = 0;
    int optNum = 0;
    int nArgType = 0;
    OPTION_INFO optInfo[TMCTL_CMD_MAX_OPTS];
    OPTION_INFO *pCurrOpt = NULL;

    memset(optInfo, 0x00, sizeof(optInfo));
    *pnArgNext = 0;

    do
    {
        if (argc == 0)
        {
            break;
        }

        nArgType = tmctl_getArgType(*argv, pCmds, pCmd->pszOptionNames);
        switch (nArgType)
        {
            case ARG_TYPE_OPTION:
            {
                if (optNum < TMCTL_CMD_MAX_OPTS)
                {
                    pCurrOpt = &optInfo[optNum++];
                    pCurrOpt->pOptName = *argv;
                }
                else
                {
                    ret = TMCTL_INVALID_NUMBER_OF_OPTIONS;
                    fprintf(stderr, "%s: too many options, %d\n",
                      pgmName, optNum);
                }
                (*pnArgNext)++;
            }
            break;

            case ARG_TYPE_PARAMETER:
            {
                if (pCurrOpt && pCurrOpt->nNumParms < TMCTL_CMD_MAX_PARMS)
                {
                    pCurrOpt->pParms[pCurrOpt->nNumParms++] = *argv;
                }
                else
                {
                    ret = TMCTL_INVALID_OPTION;
                    fprintf(stderr, "%s: invalid option\n", pgmName);
                }
                (*pnArgNext)++;
            }
            break;

            case ARG_TYPE_COMMAND:
            default:
            break;
        }

        argc--;
        argv++;

    } while ((ret == 0) && (nArgType != ARG_TYPE_COMMAND));

    if (ret == 0)
    {
        tmctl_dumpOption(pCmd->szCmdName, optInfo, optNum);
        ret = (*pCmd->pfnCmdHandler)(optInfo, optNum);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  tmctl_dumpOption
*  PURPOSE:   Debug function that dumps the options and parameters for a
**            particular command.
*  PARAMETERS:
*      pszCmdName - command name string.
*      pOptions - pointer to option array.
*      optNum - number of options.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void tmctl_dumpOption(char *pszCmdName, POPTION_INFO pOptions,
  int optNum)
{
    POPTION_INFO pOpt;
    int i, j;

    tmctlDebugPrint("cmd = %s\n", pszCmdName);
    tmctlDebugPrint("option = %s\n", pOptions->pOptName);
    for (i = 0; i < optNum; i++)
    {
        pOpt = pOptions + i;
        tmctlDebugPrint("opt=%s, %d parms=",
          pOpt->pOptName, pOpt->nNumParms);
        for (j = 0; j < pOpt->nNumParms; j++)
        {
            tmctlDebugPrint("%s ", pOpt->pParms[j]);
        }
        tmctlDebugPrint("\n");
    }
    tmctlDebugPrint("\n");
}
