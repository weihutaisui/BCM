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
 * File Name  : iqctl_cmds.c
 * Description: Linux command line utility that controls the Broadcom
 *              BCM63xx Ingress QoS Driver.
 ***************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "iqctl.h"
#include "iqctl_common.h"
#include "iqctl_api.h"

void Usage(void);

int  iqctlStatusHandler(POPTION_INFO pOptions, int nNumOptions);
int  iqctlEnableHandler(POPTION_INFO pOptions, int nNumOptions);
int  iqctlDisableHandler(POPTION_INFO pOptions, int nNumOptions);
int  iqctlFlushHandler(POPTION_INFO pOptions, int nNumOptions);
int  iqctlAddportHandler(POPTION_INFO pOptions, int nNumOptions);
int  iqctlRemportHandler(POPTION_INFO pOptions, int nNumOptions);
int  iqctlGetportHandler(POPTION_INFO pOptions, int nNumOptions);
int  iqctlDumpporttblHandler(POPTION_INFO pOptions, int nNumOptions);
int  iqctlSetDefaultPrioHandler(POPTION_INFO pOptions, int nNumOptions);
int  iqctlRemDefaultPrioHandler(POPTION_INFO pOptions, int nNumOptions);
#if defined(CC_IQ_DEBUG)
int  iqctlDebugHandler(POPTION_INFO pOptions, int nNumOptions);
#endif


char g_PgmName[128] = {0};

/*** File Scoped Globals. ***/
COMMAND_INFO g_Cmds[] =
{
    {"status",      {""},                       iqctlStatusHandler},
    {"enable",      {""},                       iqctlEnableHandler},
    {"disable",     {""},                       iqctlDisableHandler},
    {"flush",       {"--proto", ""},            iqctlFlushHandler},
    {"addport",     {"--proto", "--dport", "--ent", "--prio", ""},
                                                iqctlAddportHandler},
    {"remport",     {"--proto", "--dport", "--ent", ""},
                                                iqctlRemportHandler},
    {"getport",     {"--proto", "--dport", ""},
                                                iqctlGetportHandler},
    {"porttbl",     {"--proto", ""},            iqctlDumpporttblHandler},
#if !defined(SUPPORT_RDPA)
    {"setdefaultprio", {"--prototype", "--protoval", "--prio", ""},   
                                                iqctlSetDefaultPrioHandler},
    {"remdefaultprio", {"--prototype", "--protoval", ""},   
                                                iqctlRemDefaultPrioHandler},
#endif
#if defined(CC_IQ_DEBUG)
    {"debug",       {"--drv", "--iq", ""},      iqctlDebugHandler},
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
        "Ingress QoS Control Utility:\n\n"
        "::: Usage:\n\n"

        ":::::: Ingress QoS SW System :\n"
        "       iq status\n"
        "       iq enable\n"
        "       iq disable\n"
        "       iq flush --proto <0|1>\n"
        "       iq addport\n"
        "          --proto <0|1> --dport <1..65534> --ent <0|1> --prio <0|1>\n"
        "       iq remport --proto <0|1> --dport <1..65534> --ent <0|1>\n"
        "       iq getport --proto <0|1> --dport <1..65534> \n"
        "       iq porttbl --proto <0|1> \n"
        "          proto: 0 = TCP, 1 = UDP\n"
        "          ent: 0 = dynamic, 1 = static\n"
        "          prio: 0 = low, 1 = high\n"
#if !defined(SUPPORT_RDPA)
        "       iq setdefaultprio --prototype <0> --protoval <0..255> --prio <0/1>\n"
        "          prototype: 0 = ipproto\n"
        "          protoval: protocol value (0 to 255)\n"
        "          prio: 0 = low, 1 = high\n"
        "       iq remdefaultprio --prototype <0> --protoval <0..255>\n"
        "          prototype: 0 = ipproto\n"
        "          protoval: protocol value (0 to 255)\n"
#endif
#if defined(CC_IQ_DEBUG)
        "       iq debug\n"
        "                      [ --drv    <0..5> ]\n"
        "                    | [ --iq     <0..5> ]\n\n"
#endif

        );

    return;
} /* Usage */



/*
 *------------------------------------------------------------------------------
 * Function Name: iqctlStatusHandler
 * Description  : Processes the ingress QoS status command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int iqctlStatusHandler(POPTION_INFO pOptions, int nNumOptions)
{
    iqctl_status_t status;
    int nRet = bcm_iqctl_get_status( &status );

    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to get Ingress QoS status\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: iqctlEnableHandler
 * Description  : Processes the ingress QoS enable command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int iqctlEnableHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = bcm_iqctl_set_status( IQCTL_STATUS_ENABLE );
    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to enable Ingress QoS\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: DisableHandler
 * Description  : Processes the ingress QoS disable command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int iqctlDisableHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet = bcm_iqctl_set_status( IQCTL_STATUS_DISABLE );
    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to disable Ingress QoS\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: FlushHandler
 * Description  : Processes the ingress QoS flush command, to remove
 *                dynamic entries.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int iqctlFlushHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    iqctl_proto_t proto;
    iqctl_ent_t ent = IQCTL_ENT_DYN;

    if ( nNumOptions == 0 )
    {
        proto = IQCTL_PROTO_UDP;
    }
    else if ( nNumOptions > 1 )
    {
        fprintf( stderr, "%s: Too many options specified\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }
    else if ( !strcmp( (pOptions)->pOptName, "--proto") )
    {
        proto = atoi( (pOptions)->pParms[0] );
    }
    else
    {
        fprintf( stderr, "%s: invalid option\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }

    nRet = bcm_iqctl_flush_porttbl( proto, ent );
    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to flush Ingress QoS table\n", g_PgmName );
    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: iqctlAddportHandler
 * Description  : Processes the ingress QoS add L4 port command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int iqctlAddportHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    iqctl_proto_t proto = IQCTL_PROTO_TCP;
    int dport = 0;
    iqctl_ent_t ent = IQCTL_ENT_STAT;
    iqctl_prio_t prio = IQCTL_PRIO_LOW;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }
    else if ( nNumOptions != 4 )
    {
        fprintf( stderr, "%s: incorrect number of options\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }

    while ( nNumOptions )
    {
        if ( pOptions->nNumParms != 1 )
        {
            fprintf( stderr, "%s: specify a value\n", g_PgmName );
            return IQCTL_INVALID_OPTION;
        }

        if ( !strcmp( pOptions->pOptName, "--proto") )
        {
            proto = atoi( pOptions->pParms[0] );
        }
        else if ( !strcmp( pOptions->pOptName, "--dport") )
        {
            dport = atoi( pOptions->pParms[0] );
        }
        else if ( !strcmp( pOptions->pOptName, "--ent") )
        {
            ent = atoi( pOptions->pParms[0] );
        }
        else if ( !strcmp( pOptions->pOptName, "--prio") )
        {
            prio = atoi( pOptions->pParms[0] );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n",
                     g_PgmName, pOptions->pOptName );
            return IQCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    nRet = bcm_iqctl_add_port( proto, dport, ent, prio );

    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to configure Ingress QoS \n", g_PgmName );

    return nRet;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: iqctlRemportHandler
 * Description  : Processes the ingress QoS remove L4 port command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int iqctlRemportHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    iqctl_proto_t proto = IQCTL_PROTO_TCP;
    int dport = 0;
    iqctl_ent_t ent = IQCTL_ENT_STAT;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }
    else if ( nNumOptions != 3 )
    {
        fprintf( stderr, "%s: incorrect number of options\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }

    while ( nNumOptions )
    {
        if ( pOptions->nNumParms != 1 )
        {
            fprintf( stderr, "%s: specify a value\n", g_PgmName );
            return IQCTL_INVALID_OPTION;
        }

        if ( !strcmp( pOptions->pOptName, "--proto") )
        {
            proto = atoi( pOptions->pParms[0] );
        }
        else if ( !strcmp( pOptions->pOptName, "--dport") )
        {
            dport = atoi( pOptions->pParms[0] );
        }
        else if ( !strcmp( pOptions->pOptName, "--ent") )
        {
            ent = atoi( pOptions->pParms[0] );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n",
                     g_PgmName, pOptions->pOptName );
            return IQCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    nRet = bcm_iqctl_rem_port( proto, dport, ent );

    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to configure Ingress QoS \n", g_PgmName );

    return nRet;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: iqctlGetportHandler
 * Description  : Processes the ingress QoS get L4 port config command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int iqctlGetportHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    iqctl_proto_t proto = IQCTL_PROTO_TCP;
    int dport = 0;
    iqctl_ent_t ent;
    iqctl_prio_t prio;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }
    else if ( nNumOptions != 2 )
    {
        fprintf( stderr, "%s: incorrect number of options\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }

    while ( nNumOptions )
    {
        if ( pOptions->nNumParms != 1 )
        {
            fprintf( stderr, "%s: specify a value\n", g_PgmName );
            return IQCTL_INVALID_OPTION;
        }

        if ( !strcmp( pOptions->pOptName, "--proto") )
        {
            proto = atoi( pOptions->pParms[0] );
        }
        else if ( !strcmp( pOptions->pOptName, "--dport") )
        {
            dport = atoi( pOptions->pParms[0] );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n",
                     g_PgmName, pOptions->pOptName );
            return IQCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    nRet = bcm_iqctl_get_port( proto, dport, &ent, &prio );

    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to configure Ingress QoS \n", g_PgmName );

    return nRet;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: iqctlDumpporttblHandler
 * Description  : Processes the ingress QoS dump L4 port table config command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int iqctlDumpporttblHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    iqctl_proto_t proto;

    if ( nNumOptions == 0 )
    {
        proto = IQCTL_PROTO_UDP;
    }
    else if ( nNumOptions != 1 )
    {
        fprintf( stderr, "%s: incorrect number of options\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }
    else if ( !strcmp( pOptions->pOptName, "--proto") )
    {
        proto = atoi( pOptions->pParms[0] );
    }
    else
    {
        fprintf( stderr, "%s: invalid option [%s]\n",
                 g_PgmName, pOptions->pOptName );
        return IQCTL_INVALID_OPTION;
    }
    nRet = bcm_iqctl_dump_porttbl( proto );

    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to configure Ingress QoS \n", g_PgmName );

    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: iqctlSetDefaultPrioHandler
 * Description  : Processes the ingress QoS set default protocol prio command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int  iqctlSetDefaultPrioHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    iqctl_prototype_t prototype = IQCTL_PROTO_TCP;
    uint16_t protoval = 0;
    iqctl_prio_t prio = IQCTL_PRIO_HIGH;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }
    else if ( nNumOptions != 3 )
    {
        fprintf( stderr, "%s: incorrect number of options\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }

    while ( nNumOptions )
    {
        if ( pOptions->nNumParms != 1 )
        {
            fprintf( stderr, "%s: specify a value\n", g_PgmName );
            return IQCTL_INVALID_OPTION;
        }

        if ( !strcmp( pOptions->pOptName, "--prototype") )
        {
            prototype = atoi( pOptions->pParms[0] );
        }
        else if ( !strcmp( pOptions->pOptName, "--protoval") )
        {
            protoval = atoi( pOptions->pParms[0] );
        }
        else if ( !strcmp( pOptions->pOptName, "--prio") )
        {
            prio = atoi( pOptions->pParms[0] );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n",
                     g_PgmName, pOptions->pOptName );
            return IQCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    nRet = bcm_iqctl_set_defaultprio( prototype, protoval, prio );

    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to configure Ingress QoS \n", g_PgmName );

    return nRet;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: iqctlRemDefaultPrioHandler
 * Description  : Processes the ingress QoS set default protocol prio command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int  iqctlRemDefaultPrioHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;
    iqctl_prototype_t prototype = IQCTL_PROTO_TCP;
    uint16_t protoval = 0;

    if ( nNumOptions == 0 )
    {
        fprintf( stderr, "%s: No options specified\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }
    else if ( nNumOptions != 2 )
    {
        fprintf( stderr, "%s: incorrect number of options\n", g_PgmName );
        return IQCTL_INVALID_OPTION;
    }

    while ( nNumOptions )
    {
        if ( pOptions->nNumParms != 1 )
        {
            fprintf( stderr, "%s: specify a value\n", g_PgmName );
            return IQCTL_INVALID_OPTION;
        }

        if ( !strcmp( pOptions->pOptName, "--prototype") )
        {
            prototype = atoi( pOptions->pParms[0] );
        }
        else if ( !strcmp( pOptions->pOptName, "--protoval") )
        {
            protoval = atoi( pOptions->pParms[0] );
        }
        else
        {
            fprintf( stderr, "%s: invalid option [%s]\n",
                     g_PgmName, pOptions->pOptName );
            return IQCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    nRet = bcm_iqctl_rem_defaultprio( prototype, protoval );

    if ( nRet == IQCTL_ERROR )
        fprintf( stderr, "%s: failed to configure Ingress QoS \n", g_PgmName );

    return nRet;
}

#if defined(CC_IQ_DEBUG)
/*
 *------------------------------------------------------------------------------
 * Function Name: iqctlDebugHandler
 * Description  : Processes the ingress QoS debug command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int  iqctlDebugHandler(POPTION_INFO pOptions, int nNumOptions)
{
    int nRet;

    while ( nNumOptions )
    {
        int level;

        if ( pOptions->nNumParms != 1 )
        {
           fprintf( stderr, "%s: did not specify debug level.\n", g_PgmName );
           return IQCTL_INVALID_OPTION;
        }

        level = atoi( pOptions->pParms[0] );

        if ( !strcmp( pOptions->pOptName, "--drv") )
            nRet = iqctlDebug( IQ_DBG_DRV_LAYER, level );
        else if ( !strcmp( pOptions->pOptName, "--iq") )
            nRet = iqctlDebug( IQ_DBG_FC_LAYER, level );
        else
        {
            fprintf( stderr, "%s: invalid option\n", g_PgmName );
            return IQCTL_INVALID_OPTION;
        }
        nNumOptions--;
        pOptions++;

        if ( nRet == IQCTL_ERROR )
            fprintf( stderr, "%s: failed debug request\n", g_PgmName );
    }

    return nRet;
}
#endif


