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

/*
 *******************************************************************************
 * File Name  : pktCmfCtl.h
 * Description: Command line parsing for the Broadcom Packet CMF Control Utility
 *******************************************************************************
 */

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
#include "fap.h"

/*** Defines. ***/

/* Limit values */
#define CMD_NAME_LEN                            16
#define MAX_OPTS                                64
#define MAX_PARMS                               16

/* Argument type values. */
#define ARG_TYPE_COMMAND                        1
#define ARG_TYPE_OPTION                         2
#define ARG_TYPE_PARAMETER                      3

/* Return codes. */
#define FAP_GENERAL_ERROR                 100
#define FAP_NOT_FOUND                     101
#define FAP_ALLOC_ERROR                   102
#define FAP_INVALID_COMMAND               103

#define FAP_INVALID_OPTION                104
#define FAP_INVALID_PARAMETER             105
#define FAP_INVALID_NUMBER_OF_OPTIONS     106
#define FAP_INVALID_NUMBER_OF_PARAMETERS  107


/*** Typedefs. ***/

typedef struct
{
    char    * pOptName;
    char    * pParms[MAX_PARMS];
    int     nNumParms;
} OPTION_INFO, *POPTION_INFO;

typedef int (*FN_COMMAND_HANDLER) (POPTION_INFO pOptions, int nNumOptions);

typedef struct
{
    char    szCmdName[CMD_NAME_LEN];
    char    *pszOptionNames[MAX_OPTS];
    FN_COMMAND_HANDLER pfnCmdHandler;
} COMMAND_INFO, *PCOMMAND_INFO;


extern COMMAND_INFO g_Cmds[];

/*** Common Command Line Parsing Prototypes. ***/
void FapCliHelp(void);
int fapDevOpen(void);
int fapDevIoctlRaw(fapIoctl_t ioctl_cmd, uint32_t arg);

void Usage(void);
int  GetArgType(char *pszArg, PCOMMAND_INFO pCmds, char **ppszOptions);
PCOMMAND_INFO GetCommand(char *pszArg, PCOMMAND_INFO pCmds);
int  ProcessCommand(PCOMMAND_INFO pCmd, int argc, char **argv,
                           PCOMMAND_INFO pCmds, int *pnArgNext);
int HwHandler(POPTION_INFO pOptions, int nNumOptions);
int StatusHandler(POPTION_INFO pOptions, int nNumOptions);
int  ResetHandler(POPTION_INFO pOptions, int nNumOptions);
int  InitHandler(POPTION_INFO pOptions, int nNumOptions);
int  EnableHandler(POPTION_INFO pOptions, int nNumOptions);
int  DisableHandler(POPTION_INFO pOptions, int nNumOptions);
int PrintHandler(POPTION_INFO pOptions, int nNumOptions);
int DebugHandler(POPTION_INFO pOptions, int nNumOptions);
int CpuHandler(POPTION_INFO pOptions, int nNumOptions);
int SwqHandler(POPTION_INFO pOptions, int nNumOptions);
int DmaHandler(POPTION_INFO pOptions, int nNumOptions);
int MemHandler(POPTION_INFO pOptions, int nNumOptions);
int MtuHandler(POPTION_INFO pOptions, int nNumOptions);
int FloodHandler(POPTION_INFO pOptions, int nNumOptions);
int arlHandler(POPTION_INFO pOptions, int nNumOptions);
int DymMemHandler(POPTION_INFO pOptions, int nNumOptions);
int TmHandler(POPTION_INFO pOptions, int nNumOptions);
int Fap4keTest(POPTION_INFO pOptions, int nNumOptions);


