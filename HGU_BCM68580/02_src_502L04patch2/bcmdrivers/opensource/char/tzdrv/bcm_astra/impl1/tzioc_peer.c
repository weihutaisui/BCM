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

#include <linux/string.h>

#include "tzioc_drv.h"
#include "tzioc_msg.h"
#include "tzioc_peer.h"
#include "uappd_msg.h"

#if (TZIOC_CLIENT_NAME_LEN_MAX < UAPPD_NAME_LEN_MAX) || \
    (TZIOC_CLIENT_PATH_LEN_MAX < UAPPD_PATH_LEN_MAX)
#error "TZIOC client name/path maybe too long for uappd!"
#endif

int _tzioc_peer_start(
    struct tzioc_client *pClient,
    const char *pPeerName,
    const char *pPeerExec,
    bool bPeerShared)
{
    uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                sizeof(struct uappd_msg_uapp_start_cmd)];
    struct tzioc_msg_hdr *pHdr =
        (tzioc_msg_hdr *)&msg;
    struct uappd_msg_uapp_start_cmd *pCmd =
        (struct uappd_msg_uapp_start_cmd *)TZIOC_MSG_PAYLOAD(pHdr);
    int err = 0;

    pHdr->ucType = UAPPD_MSG_UAPP_START;
    pHdr->ucOrig = pClient->id;
    pHdr->ucDest = TZIOC_CLIENT_ID_UAPPD;
    pHdr->ucSeq  = 0;
    pHdr->ulLen  = sizeof(*pCmd);

    strncpy(pCmd->name, pPeerName, UAPPD_NAME_LEN_MAX);
    strncpy(pCmd->exec, pPeerExec, UAPPD_PATH_LEN_MAX);
    pCmd->shared = bPeerShared;
    pCmd->cookie = (uint32_t)pClient;

    err = _tzioc_msg_send(
        pClient,
        pHdr, (uint8_t *)pCmd);

    if (err) {
        LOGE("failed to send user app start msg");
    }

    /* immediately switch to TZOS */
    _tzioc_call_smc(0x7);
    return 0;
}

int _tzioc_peer_stop(
    struct tzioc_client *pClient,
    const char *pPeerName)
{
    uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                sizeof(struct uappd_msg_uapp_stop_cmd)];
    struct tzioc_msg_hdr *pHdr =
        (tzioc_msg_hdr *)&msg;
    struct uappd_msg_uapp_stop_cmd *pCmd =
        (struct uappd_msg_uapp_stop_cmd *)TZIOC_MSG_PAYLOAD(pHdr);
    int err = 0;

    pHdr->ucType = UAPPD_MSG_UAPP_STOP;
    pHdr->ucOrig = pClient->id;
    pHdr->ucDest = TZIOC_CLIENT_ID_UAPPD;
    pHdr->ucSeq  = 0;
    pHdr->ulLen  = sizeof(*pCmd);

    strncpy(pCmd->name, pPeerName, UAPPD_NAME_LEN_MAX);
    pCmd->cookie = (uint32_t)pClient;

    err = _tzioc_msg_send(
        pClient,
        pHdr, (uint8_t *)pCmd);

    if (err) {
        LOGE("failed to send user app stop msg");
    }

    /* immediately switch to TZOS */
    _tzioc_call_smc(0x7);
    return 0;
}

int _tzioc_peer_getid(
    struct tzioc_client *pClient,
    const char *pPeerName)
{
    uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                sizeof(struct uappd_msg_uapp_getid_cmd)];
    struct tzioc_msg_hdr *pHdr =
        (tzioc_msg_hdr *)&msg;
    struct uappd_msg_uapp_getid_cmd *pCmd =
        (struct uappd_msg_uapp_getid_cmd *)TZIOC_MSG_PAYLOAD(pHdr);
    int err = 0;

    pHdr->ucType = UAPPD_MSG_UAPP_GETID;
    pHdr->ucOrig = pClient->id;
    pHdr->ucDest = TZIOC_CLIENT_ID_UAPPD;
    pHdr->ucSeq  = 0;
    pHdr->ulLen  = sizeof(*pCmd);

    strncpy(pCmd->name, pPeerName, UAPPD_NAME_LEN_MAX);
    pCmd->cookie = (uint32_t)pClient;

    err = _tzioc_msg_send(
        pClient,
        pHdr, (uint8_t *)pCmd);

    if (err) {
        LOGE("failed to send user app get id msg");
    }

    /* immediately switch to TZOS */
    _tzioc_call_smc(0x7);
    return 0;
}
