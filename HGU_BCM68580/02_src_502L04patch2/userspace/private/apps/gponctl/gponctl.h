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

/*
 *******************************************************************************
 * File Name  : gponctl.h
 * Description: Command line parsing for the Broadcom GPON Control Utility
 *******************************************************************************
 */

/*** Includes. ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** Defines. ***/

/* Limit values */
#define GPON_CMD_NAME_LEN                       24
#define GPON_MAX_OPTS                           16
#define GPON_MAX_SUB_CMDS                       16
#define GPON_MAX_PARMS                          32

/* Argument type values. */
typedef enum {
    GPON_ARG_TYPE_COMMAND=1,
    GPON_ARG_TYPE_OPTION,
    GPON_ARG_TYPE_PARAMETER
} gponCtl_arg_t;

/* Return codes. */

typedef enum {
    GPONCTL_SUCCESS=0,
    GPONCTL_GENERAL_ERROR,
    GPONCTL_NOT_FOUND,
    GPONCTL_ALLOC_ERROR,
    GPONCTL_INVALID_COMMAND,
    GPONCTL_INVALID_OPTION,
    GPONCTL_INVALID_PARAMETER,
    GPONCTL_INVALID_NUMBER_OF_OPTIONS,
    GPONCTL_INVALID_NUMBER_OF_PARAMETERS
} gponCtl_cmd_t;


/*** Typedefs. ***/

typedef struct
{
    char    * pOptName;
    char    * pParms[GPON_MAX_PARMS];
    int     nNumParms;
} GPON_OPTION_INFO, *PGPON_OPTION_INFO;

typedef int (*GPON_FN_COMMAND_HANDLER) (PGPON_OPTION_INFO pOptions, int nNumOptions);

typedef struct
{
    char    szCmdName[GPON_CMD_NAME_LEN];
    char    *pszOptionNames[GPON_MAX_OPTS];
    GPON_FN_COMMAND_HANDLER pfnCmdHandler;
} GPON_COMMAND_INFO, *PGPON_COMMAND_INFO;

extern char g_gponctl_PgmName[];
extern GPON_COMMAND_INFO g_gponctl_Cmds[];
extern void usage(void);

/*** Common Command Line Parsing Prototypes. ***/

int  getArgType(char *pszArg, PGPON_COMMAND_INFO pCmds, char **ppszOptions);
PGPON_COMMAND_INFO getCommand(char *pszArg, PGPON_COMMAND_INFO pCmds);
int  processCommand(PGPON_COMMAND_INFO pCmd, int argc, char **argv,
                           PGPON_COMMAND_INFO pCmds, int *pnArgNext);
void dumpOption(char *pszCmdName, PGPON_OPTION_INFO pOptions,int nNumOptions);

/*** gponctl entry point (main) ***/
#ifdef BUILD_STATIC
int gponctl_main(int argc, char **argv);
#endif /*BUILD_STATIC*/
