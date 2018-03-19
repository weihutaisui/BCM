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
*      TMCtl command line utility local header file.
*
*****************************************************************************/
#ifndef TMCTL_CMDS_H
#define TMCTL_CMDS_H

/* ---- Include Files ----------------------------------------------------- */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "tmctl_api.h"


/* ---- Constants and Types ----------------------------------------------- */

#define TMCTL_CMD_NAME_LEN                  16
#define TMCTL_CMD_MAX_OPTS                  16
#define TMCTL_CMD_MAX_PARMS                 16

#define TMCTL_CMD_EXEC_OK                   0
#define TMCTL_CMD_PARSE_OK                  0
#define TMCTL_INVALID_COMMAND               103
#define TMCTL_INVALID_OPTION                104
#define TMCTL_INVALID_PARAMETER             105
#define TMCTL_INVALID_NUMBER_OF_OPTIONS     106
#define TMCTL_INVALID_NUMBER_OF_PARAMETERS  107
#define TMCTL_CMD_EXEC_ERROR                108

typedef struct
{
    char *pOptName;
    char *pParms[TMCTL_CMD_MAX_PARMS];
    int nNumParms;
} OPTION_INFO, *POPTION_INFO;

typedef int (*FN_COMMAND_HANDLER)(OPTION_INFO *pOptions, int optNum);

typedef struct
{
    char szCmdName[TMCTL_CMD_NAME_LEN];
    char *pszOptionNames[TMCTL_CMD_MAX_OPTS];
    FN_COMMAND_HANDLER pfnCmdHandler;
} COMMAND_INFO, *PCOMMAND_INFO;


/* ---- Variable Externs -------------------------------------------------- */

extern COMMAND_INFO tmctl_cmds[];


/* ---- Function Prototypes ----------------------------------------------- */

#ifdef TMCTL_CMD_DEBUG
#define tmctlDebugPrint(fmt, arg...) \
  printf("%s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#else
#define tmctlDebugPrint(fmt, arg...)
#endif /* TMCTL_CMD_DEBUG */


void tmctl_usage(void);


#endif /* TMCTL_CMDS_H */
