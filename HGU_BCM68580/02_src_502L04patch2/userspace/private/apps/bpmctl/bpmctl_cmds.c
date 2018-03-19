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

/***************************************************************************
 * File Name  : bpmctl_cmds.c
 * Description: Linux command line utility that controls the Broadcom
 *              BCM63xx BPM Driver.
 ***************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bpmctl_common.h"
#include "bpmctl.h"
#include "bpmctl_api.h"

void Usage(void);

int  bpmctlStatusHandler(POPTION_INFO pOptions, int nNumOptions);
int  bpmctlThreshHandler(POPTION_INFO pOptions, int nNumOptions);
int  bpmctlThreshHandler(POPTION_INFO pOptions, int nNumOptions);
int  bpmctlBuffersHandler(POPTION_INFO pOptions, int nNumOptions);
#if defined(CC_BPM_DEBUG)
int  bpmctlDebugHandler(POPTION_INFO pOptions, int nNumOptions);
#endif


char g_PgmName[128] = {0};

/*** File Scoped Globals. ***/
COMMAND_INFO g_Cmds[] =
{
    {"status",      {""},                       bpmctlStatusHandler},
    {"thresh",      {""},                       bpmctlThreshHandler},
    {"buffers",     {""},                       bpmctlBuffersHandler},
#if defined(CC_BPM_DEBUG)
    {"debug",       {"--drv", "--iq", ""},      bpmctlDebugHandler},
#endif
    {""}
};

/*
 *------------------------------------------------------------------------------
 * Function Name: Usage
 * Description  : Displays the iq usage
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
void Usage(void)
{
    printf( 
        "BPM Control Utility:\n\n"
        "::: Usage:\n\n"

        ":::::: BPM SW System :\n"
        "       bpm status\n"
        "       bpm thresh\n"
        "       bpm buffers\n"
#if defined(CC_BPM_DEBUG)
        "       bpm debug\n"
        "                      [ --drv    <0..3> ]\n"
        "                    | [ --bpm     <0..3> ]\n\n"
#endif
        );

    return;
} /* Usage */



/*
 *------------------------------------------------------------------------------
 * Function Name: bpmctlStatusHandler
 * Description  : Processes the BPM status command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int bpmctlStatusHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = bcm_bpmctl_dump_status();

    if ( nRet == BPMCTL_ERROR )
        fprintf( stderr, "%s: failed to get BPM status\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: bpmctlThreshHandler
 * Description  : Processes the BPM status command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int bpmctlThreshHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = bcm_bpmctl_dump_thresh();

    if ( nRet == BPMCTL_ERROR )
        fprintf( stderr, "%s: failed to get BPM thresh\n", g_PgmName );
    return nRet;
}



/*
 *------------------------------------------------------------------------------
 * Function Name: bpmctlBuffersHandler
 * Description  : Processes the BPM show buffers command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int bpmctlBuffersHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = bcm_bpmctl_dump_buffers( );

    if ( nRet == BPMCTL_ERROR )
        fprintf( stderr, "%s: failed to get BPM buffers\n", g_PgmName );
    return nRet;
}


#if defined(CC_BPM_DEBUG)
/*
 *------------------------------------------------------------------------------
 * Function Name: bpmctlDebugHandler
 * Description  : Processes the BPM debug command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int  bpmctlDebugHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;

    while ( nNumOptions )
    {
        int level;

        if ( pOptions->nNumParms != 1 )
        {
           fprintf( stderr, "%s: did not specify debug level.\n", g_PgmName );
           return BPMCTL_INVALID_OPTION;
        }

        level = atoi( pOptions->pParms[0] );

        if ( !strcmp( pOptions->pOptName, "--drv") )
            nRet = bpmctlDebug( BPM_DBG_DRV_LAYER, level );
        else if ( !strcmp( pOptions->pOptName, "--bpm") )
            nRet = bpmctlDebug( BPM_DBG_FC_LAYER, level );
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n",
                     g_PgmName, pOptions->pOptName );
            return BPMCTL_INVALID_OPTION;
        }
        nNumOptions--;
        pOptions++;

        if ( nRet == BPMCTL_ERROR )
            fprintf( stderr, "%s: failed debug request\n", g_PgmName );
    }

    return nRet;
}
#endif


