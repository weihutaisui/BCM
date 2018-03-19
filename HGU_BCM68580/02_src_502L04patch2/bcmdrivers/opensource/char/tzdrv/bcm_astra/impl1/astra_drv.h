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

#ifndef ASTRA_DRV_H
#define ASTRA_DRV_H

#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "astra_api.h"
#include "astra_ioctls.h"
#include "tzioc_drv.h"
#include "tzioc_msg.h"
#include "tzioc_mem.h"
#include "tzioc_client.h"
#include "tzioc_peer.h"
#include "tzioc_file.h"
#include "tzioc_coredump.h"

#define ASTRA_TIMEOUT_MSEC      5000
#define ASTRA_GETID_ATTEMPTS    5

#define ASTRA_CLIENT_NUM_MAX    16
#define ASTRA_UAPP_NUM_MAX      16
#define ASTRA_PEER_NUM_MAX      16
#define ASTRA_FILE_NUM_MAX      16

#define ASTRA_CLIENT_MAGIC      0x434c4e54 /* ASCII CLNT */
#define ASTRA_UAPP_MAGIC        0x55415050 /* ASCII UAPP */
#define ASTRA_PEER_MAGIC        0x50454552 /* ASCII PEER */
#define ASTRA_FILE_MAGIC        0x46494c45 /* ASCII FILE */

#define ASTRA_CLIENT_VALID(pClient) (pClient->magic == ASTRA_CLIENT_MAGIC)
#define ASTRA_UAPP_VALID(pUapp)     (pUapp->magic == ASTRA_UAPP_MAGIC)
#define ASTRA_PEER_VALID(pPeer)     (pPeer->magic == ASTRA_PEER_MAGIC)
#define ASTRA_FILE_VALID(pFile)     (pFile->magic == ASTRA_FILE_MAGIC)

#define ASTRA_MSG_RING_SIZE     0x10000 /* 64KB */
#define ASTRA_EVENT_RING_SIZE   0x1000 /* 4KB */

#define ASTRA_NUM_DEVICES       1

struct astra_device
{
    /* misc device */
    int astra_major;
    struct class *asclass;
    struct cdev *ascdev;
    struct device *asdevice;

    /* ioctl module */
    struct astra_ioctl_module *pIoctlMod;
};

struct coredump_device
{
    /* misc device */
    struct miscdevice *mdev;
    uint32_t buf_addr;
    uint32_t buf_size;
};

struct astra_client
{
    uint32_t magic;
    spinlock_t lock;
    wait_queue_head_t wq;

    char name[ASTRA_NAME_LEN_MAX];
    struct astra_uapp *pUapps[ASTRA_UAPP_NUM_MAX];
    struct astra_file *pFiles[ASTRA_FILE_NUM_MAX];

    struct tzioc_client *pTzClient;

    astra_event_callback pCallback;
    void *pPrivData;

    struct tzioc_ring_buf msgRing;
    void *pMsgBuff;

    /* user client specific */
    struct tzioc_ring_buf eventRing;
    void *pEventBuff;
    bool eventExit;
};

struct astra_uapp
{
    uint32_t magic;
    spinlock_t lock;
    wait_queue_head_t wq;

    char name[ASTRA_NAME_LEN_MAX];
    struct astra_client *pClient;
    struct astra_peer *pPeers[ASTRA_PEER_NUM_MAX];

    uint32_t tzStartWait :  1;
    uint32_t tzStopWait  :  1;
    uint32_t tzStarted   :  1;
    uint32_t reserved    : 29;
    int tzRetVal;
};

struct astra_peer
{
    uint32_t magic;
    spinlock_t lock;
    wait_queue_head_t wq;

    char name[ASTRA_NAME_LEN_MAX];
    struct astra_uapp *pUapp;

    uint32_t tzGetIdWait :  1;
    uint32_t tzGotId     :  1;
    uint32_t reserved    : 30;
    int tzRetVal;
    uint32_t tzId;
};

struct astra_file
{
    uint32_t magic;
    spinlock_t lock;
    wait_queue_head_t wq;

    char path[ASTRA_PATH_LEN_MAX];
    struct astra_client *pClient;

    uint32_t tzOpenWait  :  1;
    uint32_t tzCloseWait :  1;
    uint32_t tzWriteWait :  1;
    uint32_t tzReadWait  :  1;
    uint32_t tzOpen      :  1;
    uint32_t reserved    : 27;
    int tzRetVal;
};

/* exported functions */
int _astra_version_get(
    struct astra_version *pVersion);

int _astra_config_get(
    struct astra_config *pConfig);

int _astra_status_get(
    struct astra_status *pStatus);

struct astra_client *_astra_kernel_client_open(
    const char *pName,
    astra_event_callback pCallback,
    void *pPrivData);

void _astra_kernel_client_close(
    struct astra_client *pClient);

struct astra_client *_astra_user_client_open(
    const char *pName);

void _astra_user_client_close(
    struct astra_client *pClient);

struct astra_uapp *_astra_uapp_open(
    struct astra_client *pClient,
    const char *pName,
    const char *pPath);

void _astra_uapp_close(
    struct astra_uapp *pUapp);

struct astra_peer *_astra_peer_open(
    struct astra_uapp *pUapp,
    const char *pName);

void _astra_peer_close(
    struct astra_peer *pPeer);

int _astra_msg_send(
    struct astra_peer *pPeer,
    const void *pMsg,
    size_t msgLen);

int _astra_msg_receive(
    struct astra_client *pClient,
    struct astra_peer **ppPeer,
    void *pMsg,
    size_t *pMsgLen,
    int timeout);

void *_astra_mem_alloc(
    struct astra_client *pClient,
    size_t size);

void _astra_mem_free(
    struct astra_client *pClient,
    void *pBuff);

astra_paddr_t _astra_pmem_alloc(
    struct astra_client *pClient,
    size_t size);

void _astra_pmem_free(
    struct astra_client *pClient,
    astra_paddr_t paddr);

void *_astra_offset2vaddr(
    struct astra_client *pClient,
    uint32_t offset);

uint32_t _astra_vaddr2offset(
    struct astra_client *pClient,
    void *pBuff);

astra_file_handle _astra_file_open(
    struct astra_client *pClient,
    const char *pPath,
    int flags);

void _astra_file_close(
    struct astra_file *pFile);

int _astra_file_write(
    struct astra_file *pFile,
    astra_paddr_t paddr,
    size_t bytes);

int _astra_file_read(
    struct astra_file *pFile,
    astra_paddr_t paddr,
    size_t bytes);

int _astra_call_smc(
    struct astra_client *pClient,
    uint8_t ucMode);

int _astra_event_poll(
    struct astra_client *pClient,
    astra_event *pEvent,
    void *pEventData,
    size_t *pEventDataLen);

int _astra_event_exit(
    struct astra_client *pClient);

void _astra_uapp_coredump(
    struct astra_uapp *pUapp);


/* global variables */
extern struct astra_device *adev;

#endif /* ASTRA_DRV_H */
