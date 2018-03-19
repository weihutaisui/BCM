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

#ifndef TZIOC_API_H
#define TZIOC_API_H

#include <linux/types.h>
#include <linux/errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TZIOC_CLIENT_DEFINES
#define TZIOC_CLIENT_DEFINES

#define TZIOC_CLIENT_NAME_LEN_MAX       32
#define TZIOC_CLIENT_PATH_LEN_MAX       128

#endif /* TZIOC_CLIENT_DEFINES */

/*****************************************************************************
Summary:
    TZIOC msg.

Description:
    Each TZIOC msg contains a header and a immediately following payload.

    - The msg type defines and the assocaited payload formats are client
      specific. It is expected that the receiving peer client knows how to
      interpret the msgs.

    - The originating client ID is obtained through tzioc_client_open().

    - The destination client ID is obtained through tzioc_client_connect().
******************************************************************************/

#ifndef TZIOC_MSG_DEFINES
#define TZIOC_MSG_DEFINES

typedef struct tzioc_msg_hdr {
    uint8_t  ucType;                    /* msg type */
    uint8_t  ucOrig;                    /* originating client ID */
    uint8_t  ucDest;                    /* destination client ID */
    uint8_t  ucSeq;                     /* msg sequence number */
    uint32_t ulLen;                     /* byte length of msg payload */
} tzioc_msg_hdr;

#define TZIOC_MSG_PAYLOAD(pHdr)         ((uint8_t *)pHdr + sizeof(tzioc_msg_hdr))

#define TZIOC_MSG_SIZE_MAX              1024
#define TZIOC_MSG_PAYLOAD_MAX           (TZIOC_MSG_SIZE_MAX - \
                                         sizeof(struct tzioc_msg_hdr))
#endif /* TZIOC_MSG_DEFINES */

/*****************************************************************************
Summary:
    TZIOC msg processing function pointer.

Description:
    This is used in client registration through ***kernel*** API. When a msg
    destined for the client is received, the TZIOC kernel driver calls this
    the registered function in its own context.

    ulPrivData is private data of the client. It is passed in when
    tzioc_client_open() is called.

    This function returns 0 on success, or errno on failures.
******************************************************************************/

#ifndef TZIOC_MSG_PROC_PFN
#define TZIOC_MSG_PROC_PFN

typedef int (*tzioc_msg_proc_pfn)(
    tzioc_msg_hdr *pHdr,
    uint32_t ulPrivData);

#endif

/*****************************************************************************
Summary:
    TZIOC client handle.

Description:
    This is an opaque handle of local TZIOC client that is created with
    tzioc_client_open().

    It is freed with tzioc_client_close().
******************************************************************************/

typedef uint32_t tzioc_client_handle;

/*****************************************************************************
Summary:
    Open and register a local TZIOC client.

Description:
    Each client must have a unique name in its own world. If multiple client
    tasks try to register with the same local client name, only the first one
    can register with TZIOC successfully.

Input:
    pName - pointer to local TZIOC client name string
    pMsgProc - msg processing function pointer

Output:
    *pId - local TZIOC client ID

Returns:
    Handle to local TZIOC client
    NULL - failure
******************************************************************************/

tzioc_client_handle tzioc_client_open(
    const char *pName,
    tzioc_msg_proc_pfn pMsgProc,
    uint32_t ulPrivData,
    uint8_t *pId);

/*****************************************************************************
Summary:
    Unregister and close a local TZIOC client.

Description:

Input:
    hClient - local TZIOC client handle

Output:

Returns:

******************************************************************************/

void tzioc_client_close(
    tzioc_client_handle hClient);

/*****************************************************************************
Summary:
    Start a peer TZIOC client in the other world.

Description:
    If the named peer is already running and has registered with TZIOC, this
    func returns success.

    If the named peer has NOT already registered with TZIOC, a peer client
    task is spawn with given the executable name.

    There may be race conditions where multiple peer client tasks with the
    same client name are spawn. Only the first one can register with TZIOC
    successfully.

Input:
    hClient - local TZIOC client handle
    pPeerName - pointer to peer TZIOC client name string
    pPeerExec - pointer to peer TZIOC client executable name
    bPeerShared - whether peer TZIOC client can be shared

Output:

Returns:
    0 - success
    Errno - failure
******************************************************************************/

int tzioc_peer_start(
    tzioc_client_handle hClient,
    const char *pPeerName,
    const char *pPeerExec,
    bool bPeerShared);

/*****************************************************************************
Summary:
    Stop a peer TZIOC client in the other world.

Description:
    This func only has effect if the peer with the given handle has been
    started by the calling local TZIOC client.

    The reference count on the peer is decremented. The peer would be
    terminated if the reference count reaches zero.

Input:
    hClient - local TZIOC client handle
    pPeerName - pointer to peer TZIOC client name string

Output:

Returns:
    0 - success
    Errno - failure
******************************************************************************/

int tzioc_peer_stop(
    tzioc_client_handle hClient,
    const char *pPeerName);

/*****************************************************************************
Summary:
    Get a peer TZIOC client ID in the other world.

Description:

Input:
    hClient - local TZIOC client handle
    pPeerName - pointer to peer TZIOC client name string

Output:

Returns:
    0 - success
    Errno - failure
******************************************************************************/

int tzioc_peer_getid(
    tzioc_client_handle hClient,
    const char *pPeerName);

/*****************************************************************************
Summary:
    Send a TZIOC msg.

Description:
    This call is non-blocking.

Input:
    hClient - local TZIOC client handle
    pHdr - pointer to TZIOC msg header

Output:

Returns:
    0 - success
    Errno - failure
******************************************************************************/

int tzioc_msg_send(
    tzioc_client_handle hClient,
    tzioc_msg_hdr *pHdr);

/*****************************************************************************
Summary:
    Allocate a buffer from TZIOC shared memory.

Description:

Input:
    hClient - local TZIOC client handle
    ulSize - size of the

Output:

Returns:
    Pointer to allocated buffer - success;
    NULL - failure
******************************************************************************/

void *tzioc_mem_alloc(
    tzioc_client_handle hClient,
    uint32_t ulSize);

/*****************************************************************************
Summary:
    Free a buffer previously allocated from TZIOC shared memory.

Description:

Input:
    hClient - local TZIOC client handle

Output:

Returns:

******************************************************************************/

void tzioc_mem_free(
    tzioc_client_handle hClient,
    void *pBuff);

/*****************************************************************************
Summary:
    Convert TZIOC shared memory offset to virtual address.

Description:
    The address must lie within the TZIOC shared memory.

Input:
    hClient - local TZIOC client handle
    ulOffset - offset in TZIOC shared memory

Output:

Returns:
    Virtual address - success
    (uint32_t)-1 - failure
******************************************************************************/

uint32_t tzioc_offset2vaddr(
    tzioc_client_handle hClient,
    uint32_t ulOffset);

/*****************************************************************************
Summary:
    Convert virtual address to offset in TZIOC shared memory.

Description:
    The address must lie within the TZIOC shared memory.

Input:
    hClient - local TZIOC client handle
    ulVaddr - virtual address

Output:

Returns:
    TZIOC shared memory offset - success
    (uint32_t)-1 - failure
******************************************************************************/

uint32_t tzioc_vaddr2offset(
    tzioc_client_handle hClient,
    uint32_t ulVaddr);

/*****************************************************************************
Summary:
    Call SMC to switch to the other world.

Description:

Input:
    hClient - local TZIOC client handle
    ucMode - SMC mode (ignored for now)

Output:

Returns:
    0 - success
    Errno - failure
******************************************************************************/

int tzioc_call_smc(
    tzioc_client_handle hClient,
    uint8_t ucMode);

#ifdef __cplusplus
}
#endif

#endif /* TZIOC_API_H */
