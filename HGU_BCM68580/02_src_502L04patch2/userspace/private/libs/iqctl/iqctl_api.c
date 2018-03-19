/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include "iqctl_common.h"
#include "iqctl_api.h"


/*
 *------------------------------------------------------------------------------
 * Function Name: iqctl_open
 * Description  : Opens the ingress QoS device.
 * Returns      : device handle if successsful or -1 if error
 *------------------------------------------------------------------------------
 */
static int iqctl_open(void)
{
    int nFd = open( IQ_DRV_DEVICE_NAME, O_RDWR );
    if ( nFd == -1 )
    {
        fprintf( stderr, "open <%s> error no %d\n",
                 IQ_DRV_DEVICE_NAME, errno );
        return IQCTL_ERROR;
    }
    return nFd;
} /* iqctl_open */

/*
 *------------------------------------------------------------------------------
 * Function Name: iqctl_ioctl
 * Description  : Ioctls into Ingress Qos driver passing the IOCTL command, 
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
static int iqctl_ioctl(iqctl_ioctl_t ioctl_cmd, void *arg)
{
    int devFd, ret = IQCTL_ERROR;

    if ( ( devFd = iqctl_open() ) == IQCTL_ERROR )
        return IQCTL_ERROR;

    if ( (ret = ioctl( devFd, ioctl_cmd, (uintptr_t) arg )) == IQCTL_ERROR )
        fprintf( stderr, "iqctl_ioctl <%d> error\n", ioctl_cmd );

    close( devFd );
    return ret;
}


int bcm_iqctl_add_port( iqctl_proto_t proto, int dport, iqctl_ent_t ent, 
            iqctl_prio_t prio )
{
    int err = 0;
    iqctl_data_t iqdata;
    iqctl_data_t *iq = &iqdata;

    if ( (proto >= IQCTL_PROTO_MAX) || (ent >= IQCTL_ENT_MAX) || 
         (prio >= IQCTL_PRIO_MAX ) )
    {
        fprintf(stderr, "ioctl command invalid param\n");
        err = IQCTL_ERROR;
        return err;
    }

    iq->subsys = IQCTL_SUBSYS_DPORT;
    iq->op = IQCTL_OP_ADD;
    iq->proto = proto;
    iq->dport = dport;
    iq->prio = prio;
    iq->ent = ent;

    if ((err = iqctl_ioctl(IQCTL_IOCTL_SYS, (void *) iq))) {
        if (err == IQCTL_ERROR)
            fprintf(stderr, "ioctl command return error %d!\n", err);
    }

    return err;
}


int bcm_iqctl_rem_port( iqctl_proto_t proto, int dport, iqctl_ent_t ent )
{
    int err = 0;
    iqctl_data_t iqdata;
    iqctl_data_t *iq = &iqdata;

    if ( (proto >= IQCTL_PROTO_MAX) || (ent >= IQCTL_ENT_MAX) )
    {
        fprintf(stderr, "ioctl command invalid param\n");
        err = IQCTL_ERROR;
        return err;
    }

    iq->subsys = IQCTL_SUBSYS_DPORT;
    iq->op = IQCTL_OP_REM;
    iq->proto = proto;
    iq->dport = dport;
    iq->ent = ent;

    if ((err = iqctl_ioctl(IQCTL_IOCTL_SYS, (void *) iq))) {
        if (err == IQCTL_ERROR)
            fprintf(stderr, "ioctl command return error %d!\n", err);
    }

    return err;
}


int bcm_iqctl_get_port( iqctl_proto_t proto, int dport, iqctl_ent_t *ent_p,
            iqctl_prio_t *prio_p )
{
    int err = 0;
    iqctl_data_t iqdata;
    iqctl_data_t *iq = &iqdata;

    if ( (proto >= IQCTL_PROTO_MAX) || (ent_p == NULL) || (prio_p == NULL) )
    {
        fprintf(stderr, "ioctl command invalid param\n");
        err = IQCTL_ERROR;
        return err;
    }

    iq->subsys = IQCTL_SUBSYS_DPORT;
    iq->op = IQCTL_OP_GET;
    iq->proto = proto;
    iq->dport = dport;

    if ((err = iqctl_ioctl(IQCTL_IOCTL_SYS, (void *) iq))) {
        if (err == IQCTL_ERROR)
            fprintf(stderr, "ioctl command return error %d!\n", err);
        goto out;
    }

    *ent_p = iq->ent;
    *prio_p = iq->prio;

out:
    return err;
}


int bcm_iqctl_dump_porttbl( iqctl_proto_t proto )
{
    int err = 0;
    iqctl_data_t iqdata;
    iqctl_data_t *iq = &iqdata;

    if (proto >= IQCTL_PROTO_MAX)
    {
        fprintf(stderr, "ioctl command invalid param\n");
        err = IQCTL_ERROR;
        return err;
    }

    iq->subsys = IQCTL_SUBSYS_DPORTTBL;
    iq->op = IQCTL_OP_DUMP;
    iq->proto = proto;

    if ((err = iqctl_ioctl(IQCTL_IOCTL_SYS, (void *) iq))) {
        if (err == IQCTL_ERROR)
            fprintf(stderr, "ioctl command return error %d!\n", err);
    }

    return err;
}

int bcm_iqctl_flush_porttbl( iqctl_proto_t proto, iqctl_ent_t ent )
{
    int err = 0;
    iqctl_data_t iqdata;
    iqctl_data_t *iq = &iqdata;

    if (proto >= IQCTL_PROTO_MAX)
    {
        fprintf(stderr, "ioctl command invalid param\n");
        err = IQCTL_ERROR;
        return err;
    }

    iq->subsys = IQCTL_SUBSYS_DPORTTBL;
    iq->op = IQCTL_OP_FLUSH;
    iq->proto = proto;
    iq->ent = ent;

    if ((err = iqctl_ioctl(IQCTL_IOCTL_SYS, (void *) iq))) {
        if (err == IQCTL_ERROR)
            fprintf(stderr, "ioctl command return error %d!\n", err);
    }

    return err;
}


int bcm_iqctl_set_status( iqctl_status_t status )
{
    int err = 0;
    iqctl_data_t iqdata;
    iqctl_data_t *iq = &iqdata;

    if (status >= IQCTL_STATUS_MAX)
    {
        fprintf(stderr, "ioctl command invalid param\n");
        err = IQCTL_ERROR;
        return err;
    }

    iq->subsys = IQCTL_SUBSYS_STATUS;
    iq->op = IQCTL_OP_SET;
    iq->status = status;

    if ((err = iqctl_ioctl(IQCTL_IOCTL_SYS, (void *) iq))) {
        if (err == IQCTL_ERROR)
            fprintf(stderr, "ioctl command return error %d!\n", err);
    }

    return err;
}

int bcm_iqctl_get_status( iqctl_status_t *status_p )
{
    int err = 0;
    iqctl_data_t iqdata;
    iqctl_data_t *iq = &iqdata;

    if (status_p == NULL)
    {
        fprintf(stderr, "ioctl command invalid param\n");
        err = IQCTL_ERROR;
        return err;
    }

    iq->subsys = IQCTL_SUBSYS_STATUS;
    iq->op = IQCTL_OP_GET;
    if ((err = iqctl_ioctl(IQCTL_IOCTL_SYS, (void *) iq))) {
        if (err == IQCTL_ERROR)
            fprintf(stderr, "ioctl command return error %d!\n", err);
        goto out;
    }

    *status_p = iq->status;

out:
    return err;
}

int bcm_iqctl_set_defaultprio( iqctl_prototype_t prototype, int protoval, iqctl_prio_t prio )
{
    int err = 0;
    iqctl_data_t iqdata;
    iqctl_data_t *iq = &iqdata;

    if ( (prototype >= IQCTL_PROTOTYPE_MAX) || (prio >= IQCTL_PRIO_MAX) ||
         (protoval < 0) || (protoval > 255)  )
    {
        fprintf(stderr, "ioctl command invalid param\n");
        err = IQCTL_ERROR;
        return err;
    }

    iq->subsys = IQCTL_SUBSYS_PROTO;
    iq->op = IQCTL_OP_SET;
    iq->prototype = prototype;
    iq->protoval = protoval;
    iq->prio = prio;

    if ((err = iqctl_ioctl(IQCTL_IOCTL_SYS, (void *) iq))) {
        if (err == IQCTL_ERROR)
            fprintf(stderr, "ioctl command return error %d!\n", err);
    }

    return err;
}

int bcm_iqctl_rem_defaultprio( iqctl_prototype_t prototype, int protoval )
{
    int err = 0;
    iqctl_data_t iqdata;
    iqctl_data_t *iq = &iqdata;

    if ( (prototype >= IQCTL_PROTOTYPE_MAX) ||
         (protoval < 0) || (protoval > 255)  )
    {
        fprintf(stderr, "ioctl command invalid param\n");
        err = IQCTL_ERROR;
        return err;
    }

    iq->subsys = IQCTL_SUBSYS_PROTO;
    iq->op = IQCTL_OP_REM;
    iq->prototype = prototype;
    iq->protoval = protoval;

    if ((err = iqctl_ioctl(IQCTL_IOCTL_SYS, (void *) iq))) {
        if (err == IQCTL_ERROR)
            fprintf(stderr, "ioctl command return error %d!\n", err);
    }

    return err;
}

