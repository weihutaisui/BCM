/***********************************************************************
 *
 *  Copyright (c) 2007 Broadcom
 *  All Rights Reserved
 *
 * <:label-BRCM:2007:proprietary:omcid
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
:>
 *
 ************************************************************************/


#include <fcntl.h>
#include <errno.h>
#include "omci_api.h"


/*
 * macros
 */

/*
 * local variables
 */
static int gponFileDesc = 0;

/*
 * public functions
 */
int gpon_omci_api_init(omciAccessType accessType)
{
    unsigned int flags = O_RDWR;

    /* select flags */
    switch(accessType)
    {
        case OMCI_ACCESS_TYPE_BLOCKING:
            break;

        case OMCI_ACCESS_TYPE_NONBLOCKING:
            flags |= O_NONBLOCK;
            break;

        default:
            ERROR("Invalid Access Type: %d", accessType);
            return -1;
    }

    DEBUG("Opening %s...", BCMGPON_FILE);
#ifndef DESKTOP_LINUX
    gponFileDesc = open(BCMGPON_FILE, flags);
    if (gponFileDesc < 0)
    {
        ERROR("Error opening file: %s : %s", BCMGPON_FILE, strerror(errno));
        return -1;
    }
#endif /* DESKTOP_LINUX */

    return 0;
}

void gpon_omci_api_exit(void)
{
    DEBUG("Closing %s", BCMGPON_FILE);
#ifndef DESKTOP_LINUX
    close(gponFileDesc);
#endif /* DESKTOP_LINUX */
}

void gpon_omci_api_get_handle(int *pHandle)
{
    *pHandle = gponFileDesc;
}

int gpon_omci_api_transmit(omciPacket *pPacket __attribute__((unused)), int size)
{
    int count;

#ifdef DESKTOP_LINUX
    count = size;
#else /* DESKTOP_LINUX */
    count = write(gponFileDesc, (unsigned char *)pPacket, size);
    if(count < 0)
    {
        ERROR("Error writing to file: %s", strerror(errno));
    }
#endif /* DESKTOP_LINUX */

    return count;
}

int gpon_omci_api_receive(omciPacket *pPacket __attribute__((unused)),
  int size __attribute__((unused)))
{
    int count;

#ifdef DESKTOP_LINUX
    count = 0;
#else /* DESKTOP_LINUX */
    count = read(gponFileDesc, (unsigned char *)pPacket, size);
    if(count < 0)
    {
        ERROR("Error reading from file: %s", strerror(errno));
    }
#endif /* DESKTOP_LINUX */

    return count;
}

