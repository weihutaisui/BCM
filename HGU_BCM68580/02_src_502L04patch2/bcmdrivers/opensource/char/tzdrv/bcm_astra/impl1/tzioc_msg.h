/***************************************************************************
 *  <:copyright-BRCM:2016:DUAL/GPL:standard
 *  
 *     Copyright (c) 2016 Broadcom 
 *     All Rights Reserved
 *  
 *  Unless you and Broadcom execute a separate written software license
 *  agreement governing use of this software, this software is licensed
 *  to you under the terms of the GNU General Public License version 2
 *  (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 *  with the following added to such license:
 *  
 *     As a special exception, the copyright holders of this software give
 *     you permission to link this software with independent modules, and
 *     to copy and distribute the resulting executable under terms of your
 *     choice, provided that you also meet, for each linked independent
 *     module, the terms and conditions of the license of that module.
 *     An independent module is a module which is not derived from this
 *     software.  The special exception does not apply to any modifications
 *     of the software.
 *  
 *  Not withstanding the above, under no circumstances may you combine
 *  this software in any way with any other Broadcom software provided
 *  under a license other than the GPL, without Broadcom's express prior
 *  written consent.
 *  
 * :>
 ***************************************************************************/

#ifndef TZIOC_MSG_H
#define TZIOC_MSG_H

#include <linux/types.h>
#include <linux/spinlock.h>

#include "tzioc_common.h"

/* msg module */
struct tzioc_msg_module {
    /* spinlocks */
    spinlock_t sndLock;
    spinlock_t rcvLock;

    /* msg control block */
    struct tzioc_msg_cb msgCB;
};

int __init _tzioc_msg_module_init(void);
int _tzioc_msg_module_deinit(void);

int _tzioc_msg_send(
    struct tzioc_client *pClient,
    struct tzioc_msg_hdr *pHdr,
    uint8_t *pPayload);

int _tzioc_msg_receive(
    struct tzioc_client *pClient,
    struct tzioc_msg_hdr *pHdr,
    uint8_t *pPayload,
    uint32_t ulSize);

#endif /* TZIOC_MSG_H */
