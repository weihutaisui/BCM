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

#include "tzioc_api.h"
#include "tzioc_drv.h"
#include "tzioc_msg.h"
#include "tzioc_mem.h"
#include "tzioc_client.h"
#include "tzioc_peer.h"

tzioc_client_handle tzioc_client_open(
    const char *pName,
    tzioc_msg_proc_pfn pMsgProc,
    uint32_t ulPrivData,
    uint8_t *pId)
{
    struct tzioc_client *pClient;

    if (!pName || !pMsgProc || !pId) {
        LOGE("Invalid args");
        return 0;
    }

    pClient = _tzioc_kernel_client_open(
        pName,
        pMsgProc,
        ulPrivData);

    if (!pClient) {
        LOGE("Failed to open kernel client");
        return 0;
    }

    LOGD("client %s, handle %p, id %d",
         pClient->name, pClient, pClient->id);

    *pId = pClient->id;
    return (tzioc_client_handle)pClient;
}
EXPORT_SYMBOL(tzioc_client_open);

void tzioc_client_close(
    tzioc_client_handle hClient)
{
    struct tzioc_client *pClient = (struct tzioc_client *)hClient;

    if (!pClient) {
        LOGE("Invalid args");
        return;
    }

    LOGD("client %s, handle %p, id %d",
         pClient->name, pClient, pClient->id);

    _tzioc_kernel_client_close(pClient);
}
EXPORT_SYMBOL(tzioc_client_close);

int tzioc_peer_start(
    tzioc_client_handle hClient,
    const char *pPeerName,
    const char *pPeerExec,
    bool bPeerShared)
{
    struct tzioc_client *pClient = (struct tzioc_client *)hClient;
    int err = 0;

    if (!pClient || !pPeerName || !pPeerExec) {
        LOGE("Invalid args");
        return -EINVAL;
    }

    LOGD("client %s, peer %s",
         pClient->name, pPeerName);

    err = _tzioc_peer_start(
        pClient,
        pPeerName,
        pPeerExec,
        bPeerShared);

    if (err) {
        LOGE("Failed to start peer");
        return err;
    }

    return 0;
}
EXPORT_SYMBOL(tzioc_peer_start);

int tzioc_peer_stop(
    tzioc_client_handle hClient,
    const char *pPeerName)
{
    struct tzioc_client *pClient = (struct tzioc_client *)hClient;
    int err = 0;

    if (!pClient || !pPeerName) {
        LOGE("Invalid args");
        return -EINVAL;
    }

    LOGD("client %s, peer %s",
         pClient->name, pPeerName);

    err = _tzioc_peer_stop(
        pClient,
        pPeerName);

    if (err) {
        LOGE("Failed to stop peer");
        return err;
    }

    return 0;
}
EXPORT_SYMBOL(tzioc_peer_stop);

int tzioc_peer_getid(
    tzioc_client_handle hClient,
    const char *pPeerName)
{
    struct tzioc_client *pClient = (struct tzioc_client *)hClient;
    int err = 0;

    if (!pClient || !pPeerName) {
        LOGE("Invalid args");
        return -EINVAL;
    }

    LOGD("client %s, peer %s",
         pClient->name, pPeerName);

    err = _tzioc_peer_getid(
        pClient,
        pPeerName);

    if (err) {
        LOGE("Failed to get peer id");
        return err;
    }

    return 0;
}
EXPORT_SYMBOL(tzioc_peer_getid);

int tzioc_msg_send(
    tzioc_client_handle hClient,
    tzioc_msg_hdr *pHdr)
{
    struct tzioc_client *pClient = (struct tzioc_client *)hClient;

    if (!pClient || !pHdr ||
        pClient->id != pHdr->ucOrig) {
        LOGE("Invalid args");
        return -EINVAL;
    }

    return _tzioc_msg_send(
        pClient,
        pHdr,
        TZIOC_MSG_PAYLOAD(pHdr));
}
EXPORT_SYMBOL(tzioc_msg_send);

void *tzioc_mem_alloc(
    tzioc_client_handle hClient,
    uint32_t ulSize)
{
    struct tzioc_client *pClient = (struct tzioc_client *)hClient;

    if (!pClient || !ulSize) {
        LOGE("Invalid args");
        return NULL;
    }

    return _tzioc_mem_alloc(
        pClient,
        ulSize);
}
EXPORT_SYMBOL(tzioc_mem_alloc);

void tzioc_mem_free(
    tzioc_client_handle hClient,
    void *pBuff)
{
    struct tzioc_client *pClient = (struct tzioc_client *)hClient;

    if (!pClient || !pBuff) {
        LOGE("Invalid args");
        return;
    }

    _tzioc_mem_free(
        pClient,
        pBuff);
}
EXPORT_SYMBOL(tzioc_mem_free);

uint32_t tzioc_offset2vaddr(
    tzioc_client_handle hClient,
    uint32_t ulOffset)
{
    UNUSED(hClient);

    /* kernel space */
    return _tzioc_offset2addr(ulOffset);
}
EXPORT_SYMBOL(tzioc_offset2vaddr);

uint32_t tzioc_vaddr2offset(
    tzioc_client_handle hClient,
    uint32_t ulVaddr)
{
    UNUSED(hClient);

    /* kernel space */
    return _tzioc_addr2offset(ulVaddr);
}
EXPORT_SYMBOL(tzioc_vaddr2offset);

int tzioc_call_smc(
    tzioc_client_handle hClient,
    uint8_t ucMode)
{
    UNUSED(hClient);

    /* assuming ucMode == SMC callnum */
    return _tzioc_call_smc((uint32_t)ucMode);
}
EXPORT_SYMBOL(tzioc_call_smc);
