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

#include <linux/slab.h>

#include "tzioc_drv.h"
#include "tzioc_msg.h"

/* static pointer to msg module */
static struct tzioc_msg_module *pMsgMod;
static struct tzioc_msg_cb *pMsgCB;

/* exported msg control block to common code */
struct tzioc_msg_cb *pTziocMsgCB;

int __init _tzioc_msg_module_init(void)
{
    /* alloc msg module */
    pMsgMod = kzalloc(sizeof(struct tzioc_msg_module), GFP_KERNEL);
    if (!pMsgMod) {
        LOGE("Failed to alloc TZIOC msg module");
        return -ENOMSG;
    }

    /* remsgber msg module in TZIOC device */
    tdev->pMsgMod = pMsgMod;

    /* init spinlocks */
    spin_lock_init(&pMsgMod->sndLock);
    spin_lock_init(&pMsgMod->rcvLock);

    /* init shared memory */
    __tzioc_ring_init(
        &tdev->psmem->n2tRing,
        0,
        0,
        TZIOC_RING_WRITE,
        _tzioc_offset2addr);

    __tzioc_ring_init(
        &tdev->psmem->t2nRing,
        0,
        0,
        TZIOC_RING_READ,
        _tzioc_offset2addr);

    /* init msg control block */
    pMsgCB = &pMsgMod->msgCB;
    pMsgCB->pSndRing = &tdev->psmem->n2tRing;
    pMsgCB->pRcvRing = &tdev->psmem->t2nRing;

    /* export msg control block to common code */
    pTziocMsgCB = pMsgCB;

    LOGI("TZIOC msg module initialized");
    return 0;
}

int _tzioc_msg_module_deinit(void)
{
    /* reset exported msg control block to common code */
    pTziocMsgCB = NULL;

    /* deinit msg control block */
    pMsgCB = NULL;

    /* reset msg module in TZIOC device */
    tdev->pMsgMod = NULL;

    /* free msg module control block */
    kfree(pMsgMod);

    LOGI("TZIOC msg module deinitialized");
    return 0;
}

int _tzioc_msg_send(
    struct tzioc_client *pClient,
    struct tzioc_msg_hdr *pHdr,
    uint8_t *pPayload)
{
    int err = 0;

    spin_lock(&pMsgMod->sndLock);
    err = __tzioc_msg_send(pHdr, pPayload);
    spin_unlock(&pMsgMod->sndLock);

    if (err) {
        LOGE("Failed to send msg, client %d", pClient->id);
        return err;
    }
    return err;
}

int _tzioc_msg_receive(
    struct tzioc_client *pClient,
    struct tzioc_msg_hdr *pHdr,
    uint8_t *pPayload,
    uint32_t ulSize)
{
    int err = 0;

    spin_lock(&pMsgMod->rcvLock);
    err = __tzioc_msg_receive(pHdr, pPayload, ulSize);
    spin_unlock(&pMsgMod->rcvLock);

    if (err &&
        err != -ENOMSG &&
        err != -ENOSPC) {
        LOGE("Failed to receive msg, client %d", pClient->id);
        return err;
    }
    return err;
}
