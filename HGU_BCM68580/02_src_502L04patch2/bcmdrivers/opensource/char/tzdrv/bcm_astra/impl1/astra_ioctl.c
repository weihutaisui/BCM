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

#include <asm/uaccess.h>
#include <asm/current.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include "astra_drv.h"
#include "astra_ioctl.h"

/* static pointer to ioctl module */
static struct astra_ioctl_module *pIoctlMod;

static int astra_ioctl_event_poll(struct file *file, void *arg);
static int astra_ioctl_event_exit(struct file *file, void *arg);
static int astra_ioctl_version_get(struct file *file, void *arg);
static int astra_ioctl_config_get(struct file *file, void *arg);
static int astra_ioctl_status_get(struct file *file, void *arg);
static int astra_ioctl_call_smc(struct file *file, void *arg);
static int astra_ioctl_client_open(struct file *file, void *arg);
static int astra_ioctl_client_close(struct file *file, void *arg);
static int astra_ioctl_uapp_open(struct file *file, void *arg);
static int astra_ioctl_uapp_close(struct file *file, void *arg);
static int astra_ioctl_peer_open(struct file *file, void *arg);
static int astra_ioctl_peer_close(struct file *file, void *arg);
static int astra_ioctl_msg_send(struct file *file, void *arg);
static int astra_ioctl_msg_receive(struct file *file, void *arg);
static int astra_ioctl_mem_alloc(struct file *file, void *arg);
static int astra_ioctl_mem_free(struct file *file, void *arg);
static int astra_ioctl_file_open(struct file *file, void *arg);
static int astra_ioctl_file_close(struct file *file, void *arg);
static int astra_ioctl_file_write(struct file *file, void *arg);
static int astra_ioctl_file_read(struct file *file, void *arg);
static int astra_ioctl_uapp_coredump(struct file *file,void *arg);

/* short-hand to get ioctl offset */
#define IOCTL_OFFSET(name)      (ASTRA_IOCTL_##name - ASTRA_IOCTL_FIRST)

int __init _astra_ioctl_module_init(void)
{
    /* alloc ioctl module */
    pIoctlMod = kzalloc(sizeof(struct astra_ioctl_module), GFP_KERNEL);
    if (!pIoctlMod) {
        LOGE("Failed to alloc astra ioctl module");
        return -ENOMEM;
    }

    /* remember ioctl module in astra device */
    adev->pIoctlMod = pIoctlMod;

    memset(pIoctlMod->handlers, 0, sizeof(pIoctlMod->handlers));

    pIoctlMod->handlers[IOCTL_OFFSET(EVENT_POLL  )] = astra_ioctl_event_poll;
    pIoctlMod->handlers[IOCTL_OFFSET(EVENT_EXIT  )] = astra_ioctl_event_exit;
    pIoctlMod->handlers[IOCTL_OFFSET(VERSION_GET )] = astra_ioctl_version_get;
    pIoctlMod->handlers[IOCTL_OFFSET(CONFIG_GET  )] = astra_ioctl_config_get;
    pIoctlMod->handlers[IOCTL_OFFSET(STATUS_GET  )] = astra_ioctl_status_get;
    pIoctlMod->handlers[IOCTL_OFFSET(CALL_SMC    )] = astra_ioctl_call_smc;
    pIoctlMod->handlers[IOCTL_OFFSET(CLIENT_OPEN )] = astra_ioctl_client_open;
    pIoctlMod->handlers[IOCTL_OFFSET(CLIENT_CLOSE)] = astra_ioctl_client_close;
    pIoctlMod->handlers[IOCTL_OFFSET(UAPP_OPEN   )] = astra_ioctl_uapp_open;
    pIoctlMod->handlers[IOCTL_OFFSET(UAPP_CLOSE  )] = astra_ioctl_uapp_close;
    pIoctlMod->handlers[IOCTL_OFFSET(PEER_OPEN   )] = astra_ioctl_peer_open;
    pIoctlMod->handlers[IOCTL_OFFSET(PEER_CLOSE  )] = astra_ioctl_peer_close;
    pIoctlMod->handlers[IOCTL_OFFSET(MSG_SEND    )] = astra_ioctl_msg_send;
    pIoctlMod->handlers[IOCTL_OFFSET(MSG_RECEIVE )] = astra_ioctl_msg_receive;
    pIoctlMod->handlers[IOCTL_OFFSET(MEM_ALLOC   )] = astra_ioctl_mem_alloc;
    pIoctlMod->handlers[IOCTL_OFFSET(MEM_FREE    )] = astra_ioctl_mem_free;
    pIoctlMod->handlers[IOCTL_OFFSET(FILE_OPEN   )] = astra_ioctl_file_open;
    pIoctlMod->handlers[IOCTL_OFFSET(FILE_CLOSE  )] = astra_ioctl_file_close;
    pIoctlMod->handlers[IOCTL_OFFSET(FILE_WRITE  )] = astra_ioctl_file_write;
    pIoctlMod->handlers[IOCTL_OFFSET(FILE_READ   )] = astra_ioctl_file_read;
	pIoctlMod->handlers[IOCTL_OFFSET(UAPP_COREDUMP)] = astra_ioctl_uapp_coredump;

    LOGI("Astra ioctl module initialized");
    return 0;
}

int _astra_ioctl_module_deinit(void)
{
    /* reset ioctl module in astra device */
    adev->pIoctlMod = NULL;

    /* free ioctl module control block */
    kfree(pIoctlMod);

    LOGI("Astra ioctl module deinitialized");
    return 0;
}

int _astra_ioctl_do_ioctl(struct file *file, uint32_t cmd, void *arg)
{
    if (pIoctlMod->handlers[cmd - ASTRA_IOCTL_FIRST]) {
        return pIoctlMod->handlers[cmd - ASTRA_IOCTL_FIRST](file, arg);
    }
    else {
        LOGE("Unsupported astra ioctl cmd 0x%x", (unsigned int)cmd);
        return -ENOTTY;
    }
}

static int astra_ioctl_event_poll(struct file *file, void *arg)
{
    struct astra_ioctl_event_poll_data eventPollData;
    struct astra_client *pClient;
    astra_event event;
    char eventData[16];
    size_t eventDataLen;
    int err = 0;

    err = copy_from_user(
        &eventPollData,
        (void *)arg,
        sizeof(eventPollData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct astra_client *)eventPollData.hClient;

    if (!pClient) {
        LOGE("Invalid args in astra ioctl event poll cmd");
        eventPollData.retVal = -EINVAL;
        goto RETURN;
    }

    eventPollData.retVal =
        _astra_event_poll(
            pClient,
            &event,
            &eventData,
            &eventDataLen);

    if (eventPollData.retVal) {
        goto RETURN;
    }

    eventPollData.event = event;
    eventPollData.eventDataLen = eventDataLen;
    if (eventDataLen) {
        memcpy(&eventPollData.eventData, &eventData, eventDataLen);
    }

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &eventPollData,
        sizeof(eventPollData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_event_exit(struct file *file, void *arg)
{
    struct astra_ioctl_event_exit_data eventExitData;
    struct astra_client *pClient;
    int err = 0;

    err = copy_from_user(
        &eventExitData,
        (void *)arg,
        sizeof(eventExitData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct astra_client *)eventExitData.hClient;

    if (!pClient) {
        LOGE("Invalid args in astra ioctl event exit cmd");
        eventExitData.retVal = -EINVAL;
        goto RETURN;
    }

    _astra_event_exit(pClient);

    eventExitData.retVal = 0;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &eventExitData,
        sizeof(eventExitData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_version_get(struct file *file, void *arg)
{
    struct astra_ioctl_version_get_data versionGetData;
    int err = 0;

    versionGetData.retVal =
        _astra_version_get(&versionGetData.version);

    err = copy_to_user(
        (void *)arg,
        &versionGetData,
        sizeof(versionGetData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_config_get(struct file *file, void *arg)
{
    struct astra_ioctl_config_get_data configGetData;
    int err = 0;

    configGetData.retVal =
        _astra_config_get(&configGetData.config);

    err = copy_to_user(
        (void *)arg,
        &configGetData,
        sizeof(configGetData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_status_get(struct file *file, void *arg)
{
    struct astra_ioctl_status_get_data statusGetData;
    int err = 0;

    statusGetData.retVal =
        _astra_status_get(&statusGetData.status);

    err = copy_to_user(
        (void *)arg,
        &statusGetData,
        sizeof(statusGetData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_call_smc(struct file *file, void *arg)
{
    struct astra_ioctl_call_smc_data callSmcData;
    struct astra_client *pClient;
    int err = 0;

    err = copy_from_user(
        &callSmcData,
        (void *)arg,
        sizeof(callSmcData));

    if (err) {
        LOGE("Failed to access ASTRA ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct astra_client *)callSmcData.hClient;

    if (!pClient) {
        LOGE("Invalid args in astra ioctl call smc cmd");
        callSmcData.retVal = -EINVAL;
        goto RETURN;
    }

    callSmcData.retVal =
        _astra_call_smc(
            pClient,
            callSmcData.mode);

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &callSmcData,
        sizeof(callSmcData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_client_open(struct file *file, void *arg)
{
    struct astra_ioctl_client_open_data clientOpenData;
    struct astra_client **pClients;
    struct astra_client *pClient;
    char *pName;
    int err = 0;
    int i;

    err = copy_from_user(
        &clientOpenData,
        (void *)arg,
        sizeof(clientOpenData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pName = clientOpenData.name;

    if (pName[0] == '\0') {
        LOGE("Invalid args in astra ioctl client open cmd");
        clientOpenData.retVal = -EINVAL;
        goto RETURN;
    }

    pClient = _astra_user_client_open(pName);

    if (pClient == NULL) {
        LOGE("Failed to open astra user client");
        clientOpenData.retVal = -EFAULT;
        goto RETURN;
    }

    /* add client to file */
    pClients = (struct astra_client **)file->private_data;
    for (i = 0; i < ASTRA_CLIENT_NUM_MAX; i++) {
        if (!pClients[i]) {
            pClients[i] = pClient;
            break;
        }
    }

    if (i == ASTRA_CLIENT_NUM_MAX) {
        LOGE("Max number of clients reached for the process");
        _astra_user_client_close(pClient);
        clientOpenData.retVal = -EFAULT;
        goto RETURN;
    }

    clientOpenData.retVal = 0;
    clientOpenData.hClient = pClient;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &clientOpenData,
        sizeof(clientOpenData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_client_close(struct file *file, void *arg)
{
    struct astra_ioctl_client_close_data clientCloseData;
    struct astra_client **pClients;
    struct astra_client *pClient;
    int err = 0;
    int i;

    err = copy_from_user(
        &clientCloseData,
        (void *)arg,
        sizeof(clientCloseData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct astra_client *)clientCloseData.hClient;

    if (!pClient) {
        LOGE("Invalid args in astra ioctl client close cmd");
        clientCloseData.retVal = -EINVAL;
        goto RETURN;
    }

    _astra_user_client_close(pClient);

    /* clear client from file */
    pClients = (struct astra_client **)file->private_data;
    for (i = 0; i < ASTRA_CLIENT_NUM_MAX; i++) {
        if (pClients[i] == pClient) {
            pClients[i] = NULL;
            break;
        }
    }

    clientCloseData.retVal = 0;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &clientCloseData,
        sizeof(clientCloseData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_uapp_open(struct file *file, void *arg)
{
    struct astra_ioctl_uapp_open_data uappOpenData;
    struct astra_client *pClient;
    struct astra_uapp *pUapp;
    char *pName;
    char *pPath;
    int err = 0;

    err = copy_from_user(
        &uappOpenData,
        (void *)arg,
        sizeof(uappOpenData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct astra_client *)uappOpenData.hClient;
    pName = uappOpenData.name;
    pPath = uappOpenData.path;

    if (!pClient ||
        pName[0] == '\0' ||
        pPath[0] == '\0') {
        LOGE("Invalid args in astra ioctl userapp open cmd");
        uappOpenData.retVal = -EINVAL;
        goto RETURN;
    }

    pUapp = _astra_uapp_open(
        pClient,
        pName,
        pPath);

    if (pUapp == NULL) {
        LOGE("Failed to open astra uapp");
        uappOpenData.retVal = -EFAULT;
        goto RETURN;
    }

    uappOpenData.retVal = 0;
    uappOpenData.hUapp = pUapp;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &uappOpenData,
        sizeof(uappOpenData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_uapp_close(struct file *file, void *arg)
{
    struct astra_ioctl_uapp_close_data uappCloseData;
    struct astra_uapp *pUapp;
    int err = 0;

    err = copy_from_user(
        &uappCloseData,
        (void *)arg,
        sizeof(uappCloseData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pUapp = (struct astra_uapp *)uappCloseData.hUapp;

    if (!pUapp) {
        LOGE("Invalid args in astra ioctl userapp close cmd");
        uappCloseData.retVal = -EINVAL;
        goto RETURN;
    }

    _astra_uapp_close(pUapp);

    uappCloseData.retVal = 0;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &uappCloseData,
        sizeof(uappCloseData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_peer_open(struct file *file, void *arg)
{
    struct astra_ioctl_peer_open_data peerOpenData;
    struct astra_uapp *pUapp;
    struct astra_peer *pPeer;
    char *pName;
    int err = 0;

    err = copy_from_user(
        &peerOpenData,
        (void *)arg,
        sizeof(peerOpenData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pUapp = (struct astra_uapp *)peerOpenData.hUapp;
    pName = peerOpenData.name;

    if (!pUapp ||
        pName[0] == '\0') {
        LOGE("Invalid args in astra ioctl peer open cmd");
        peerOpenData.retVal = -EINVAL;
        goto RETURN;
    }

    pPeer = _astra_peer_open(
        pUapp,
        pName);

    if (pPeer == NULL) {
        LOGE("Failed to open astra peer");
        peerOpenData.retVal = -EFAULT;
        goto RETURN;
    }

    peerOpenData.retVal = 0;
    peerOpenData.hPeer = pPeer;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &peerOpenData,
        sizeof(peerOpenData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_peer_close(struct file *file, void *arg)
{
    struct astra_ioctl_peer_close_data peerCloseData;
    struct astra_peer *pPeer;
    int err = 0;

    err = copy_from_user(
        &peerCloseData,
        (void *)arg,
        sizeof(peerCloseData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pPeer = (struct astra_peer *)peerCloseData.hPeer;

    if (!pPeer) {
        LOGE("Invalid args in astra ioctl peer close cmd");
        peerCloseData.retVal = -EINVAL;
        goto RETURN;
    }

    _astra_peer_close(pPeer);

    peerCloseData.retVal = 0;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &peerCloseData,
        sizeof(peerCloseData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_msg_send(struct file *file, void *arg)
{
    /* temporarily use static intermediate msg buffer with spin_lock */
    static uint8_t kernelMsg[ASTRA_MSG_LEN_MAX];
    static DEFINE_SPINLOCK(kernelMsgLock);

    struct astra_ioctl_msg_send_data msgSendData;
    struct astra_peer *pPeer;
    const void *pUserMsg;
    size_t msgLen;
    int err = 0;

    err = copy_from_user(
        &msgSendData,
        (void *)arg,
        sizeof(msgSendData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pPeer = (struct astra_peer *)msgSendData.hPeer;
    pUserMsg = msgSendData.pMsg;
    msgLen = msgSendData.msgLen;

    if (!pPeer || !pUserMsg ||
        msgLen == 0 || msgLen > ASTRA_MSG_LEN_MAX) {
        LOGE("Invalid args in astra ioctl msg send cmd");
        msgSendData.retVal = -EINVAL;
        goto RETURN;
    }

    spin_lock(&kernelMsgLock);

    err = copy_from_user(
        kernelMsg,
        pUserMsg,
        msgLen);

    if (err) {
        LOGE("Failed to access msg data");
        spin_unlock(&kernelMsgLock);
        return -EFAULT;
    }

    msgSendData.retVal =
        _astra_msg_send(
            pPeer,
            kernelMsg,
            msgLen);

    spin_unlock(&kernelMsgLock);

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &msgSendData,
        sizeof(msgSendData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_msg_receive(struct file *file, void *arg)
{
    /* temporarily use static intermediate msg buffer with spin_lock */
    static uint8_t kernelMsg[ASTRA_MSG_LEN_MAX];
    static DEFINE_SPINLOCK(kernelMsgLock);

    struct astra_ioctl_msg_receive_data msgReceiveData;
    struct astra_client *pClient;
    struct astra_peer *pPeer;
    void *pUserMsg;
    size_t msgLen;
    int timeout;
    int err = 0;

    err = copy_from_user(
        &msgReceiveData,
        (void *)arg,
        sizeof(msgReceiveData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct astra_client *)msgReceiveData.hClient;
    pUserMsg = msgReceiveData.pMsg;
    msgLen = msgReceiveData.msgLen;
    timeout = msgReceiveData.timeout;

    if (!pClient || !pUserMsg ||
        msgLen == 0) {
        LOGE("Invalid args in astra ioctl msg receive cmd");
        msgReceiveData.retVal = -EINVAL;
        goto RETURN;
    }

    spin_lock(&kernelMsgLock);

    msgReceiveData.retVal =
        _astra_msg_receive(
            pClient,
            &pPeer,
            kernelMsg,
            &msgLen,
            timeout);

    if (msgReceiveData.retVal) {
        spin_unlock(&kernelMsgLock);
        goto RETURN;
    }

    err = copy_to_user(
        pUserMsg,
        kernelMsg,
        msgLen);

    spin_unlock(&kernelMsgLock);

    if (err) {
        LOGE("Failed to access msg data");
        return -EFAULT;
    }

    msgReceiveData.retVal = 0;
    msgReceiveData.hPeer = pPeer;
    msgReceiveData.msgLen = msgLen;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &msgReceiveData,
        sizeof(msgReceiveData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_mem_alloc(struct file *file, void *arg)
{
    struct astra_ioctl_mem_alloc_data memAllocData;
    struct astra_client *pClient;
    size_t size;
    void *pBuff;
    uint32_t buffOffset;
    int err = 0;

    err = copy_from_user(
        &memAllocData,
        (void *)arg,
        sizeof(memAllocData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct astra_client *)memAllocData.hClient;
    size = memAllocData.size;

    if (!pClient || size == 0) {
        LOGE("Invalid args in astra ioctl mem alloc cmd");
        memAllocData.retVal = -EINVAL;
        goto ERR_RETURN;
    }

    pBuff = _astra_mem_alloc(
        pClient,
        size);

    if (!pBuff) {
        LOGE("Failed to alloc astra mem");
        memAllocData.retVal = -ENOMEM;
        goto ERR_RETURN;
    }

    buffOffset = _astra_vaddr2offset(
        pClient,
        pBuff);

    if (buffOffset == 0) {
        LOGE("Failed to convert vaddr to offset");
        memAllocData.retVal = -EFAULT;
        goto ERR_RETURN;
    }

    memAllocData.buffOffset = buffOffset;
    memAllocData.retVal = 0;

 ERR_RETURN:
    err = copy_to_user(
        (void *)arg,
        &memAllocData,
        sizeof(memAllocData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_mem_free(struct file *file, void *arg)
{
    struct astra_ioctl_mem_free_data memFreeData;
    struct astra_client *pClient;
    void *pBuff;
    uint32_t buffOffset;
    int err = 0;

    err = copy_from_user(
        &memFreeData,
        (void *)arg,
        sizeof(memFreeData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct astra_client *)memFreeData.hClient;
    buffOffset = memFreeData.buffOffset;

    if (!pClient || buffOffset == 0) {
        LOGE("Invalid args in TZIOC ioctl client close cmd");
        memFreeData.retVal = -EINVAL;
        goto ERR_RETURN;
    }

    pBuff = _astra_offset2vaddr(
        pClient,
        buffOffset);

    if (!pBuff) {
        LOGE("Failed to convert offset to vaddr");
        memFreeData.retVal = -EFAULT;
        goto ERR_RETURN;
    }

    _astra_mem_free(
        pClient,
        pBuff);

    memFreeData.retVal = 0;

 ERR_RETURN:
    err = copy_to_user(
        (void *)arg,
        &memFreeData,
        sizeof(memFreeData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_file_open(struct file *file, void *arg)
{
    struct astra_ioctl_file_open_data fileOpenData;
    struct astra_client *pClient;
    struct astra_file *pFile;
    char *pPath;
    int flags;
    int err = 0;

    err = copy_from_user(
        &fileOpenData,
        (void *)arg,
        sizeof(fileOpenData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct astra_client *)fileOpenData.hClient;
    pPath = fileOpenData.path;
    flags = fileOpenData.flags;

    if (!pClient ||
        pPath[0] == '\0') {
        LOGE("Invalid args in astra ioctl file open cmd");
        fileOpenData.retVal = -EINVAL;
        goto RETURN;
    }

    pFile = _astra_file_open(
        pClient,
        pPath,
        flags);

    if (pFile == NULL) {
        LOGE("Failed to open astra file");
        fileOpenData.retVal = -EFAULT;
        goto RETURN;
    }

    fileOpenData.retVal = 0;
    fileOpenData.hFile = pFile;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &fileOpenData,
        sizeof(fileOpenData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_file_close(struct file *file, void *arg)
{
    struct astra_ioctl_file_close_data fileCloseData;
    struct astra_file *pFile;
    int err = 0;

    err = copy_from_user(
        &fileCloseData,
        (void *)arg,
        sizeof(fileCloseData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pFile = (struct astra_file *)fileCloseData.hFile;

    if (!pFile) {
        LOGE("Invalid args in astra ioctl file close cmd");
        fileCloseData.retVal = -EINVAL;
        goto RETURN;
    }

    _astra_file_close(pFile);

    fileCloseData.retVal = 0;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &fileCloseData,
        sizeof(fileCloseData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_file_write(struct file *file, void *arg)
{
    struct astra_ioctl_file_write_data fileWriteData;
    struct astra_file *pFile;
    astra_paddr_t paddr;
    size_t bytes;
    int err = 0;

    err = copy_from_user(
        &fileWriteData,
        (void *)arg,
        sizeof(fileWriteData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pFile = (struct astra_file *)fileWriteData.hFile;
    paddr = fileWriteData.paddr;
    bytes = fileWriteData.bytes;

    if (!pFile || paddr == 0 || bytes == 0) {
        LOGE("Invalid args in astra ioctl file write cmd");
        fileWriteData.retVal = -EINVAL;
        goto RETURN;
    }

    err = _astra_file_write(
        pFile,
        paddr,
        bytes);

    if (err < 0) {
        LOGE("Failed to write to astra file");
        fileWriteData.retVal = -EFAULT;
        goto RETURN;
    }

    fileWriteData.retVal = 0;
    fileWriteData.bytes = err;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &fileWriteData,
        sizeof(fileWriteData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_file_read(struct file *file, void *arg)
{
    struct astra_ioctl_file_read_data fileReadData;
    struct astra_file *pFile;
    astra_paddr_t paddr;
    size_t bytes;
    int err = 0;

    err = copy_from_user(
        &fileReadData,
        (void *)arg,
        sizeof(fileReadData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pFile = (struct astra_file *)fileReadData.hFile;
    paddr = fileReadData.paddr;
    bytes = fileReadData.bytes;

    if (!pFile || paddr == 0 || bytes == 0) {
        LOGE("Invalid args in astra ioctl file read cmd");
        fileReadData.retVal = -EINVAL;
        goto RETURN;
    }

    err = _astra_file_read(
        pFile,
        paddr,
        bytes);

    if (err < 0) {
        LOGE("Failed to read to astra file");
        fileReadData.retVal = -EFAULT;
        goto RETURN;
    }

    fileReadData.retVal = 0;
    fileReadData.bytes = err;

 RETURN:
    err = copy_to_user(
        (void *)arg,
        &fileReadData,
        sizeof(fileReadData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int astra_ioctl_uapp_coredump(struct file *file,void *arg)
{
    struct astra_ioctl_uapp_coredump_data uappCoredumpData;
    struct astra_uapp *pUapp;
    int err = 0;

    err = copy_from_user(
        &uappCoredumpData,
        (void *)arg,
        sizeof(uappCoredumpData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    pUapp = (struct astra_uapp *)uappCoredumpData.hUapp;

    if (!pUapp) {
        LOGE("Invalid args in astra ioctl userapp close cmd");
        uappCoredumpData.retVal = -EINVAL;
        goto RETURN;
    }

    _astra_uapp_coredump(pUapp);

    uappCoredumpData.retVal = 0;

RETURN:
    err = copy_to_user(
        (void *)arg,
        &uappCoredumpData,
        sizeof(uappCoredumpData));

    if (err) {
        LOGE("Failed to access astra ioctl arguments");
        return -EFAULT;
    };

    return 0;
}
