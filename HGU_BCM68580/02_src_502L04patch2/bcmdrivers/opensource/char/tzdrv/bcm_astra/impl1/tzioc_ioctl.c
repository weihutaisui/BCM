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

#include "tzioc_drv.h"
#include "tzioc_client.h"
#include "tzioc_msg.h"
#include "tzioc_mem.h"
#include "tzioc_ioctl.h"

/* static pointer to ioctl module */
static struct tzioc_ioctl_module *pIoctlMod;

static int tzioc_ioctl_client_open(struct file *file, void *arg);
static int tzioc_ioctl_client_close(struct file *file, void *arg);
static int tzioc_ioctl_msg_send(struct file *file, void *arg);
static int tzioc_ioctl_mem_alloc(struct file *file, void *arg);
static int tzioc_ioctl_mem_free(struct file *file, void *arg);
static int tzioc_ioctl_call_smc(struct file *file, void *arg);

/* short-hand to get ioctl offset */
#define IOCTL_OFFSET(name)      (TZIOC_IOCTL_##name - TZIOC_IOCTL_FIRST)

int __init _tzioc_ioctl_module_init(void)
{
    /* alloc ioctl module */
    pIoctlMod = kzalloc(sizeof(struct tzioc_ioctl_module), GFP_KERNEL);
    if (!pIoctlMod) {
        LOGE("Failed to alloc TZIOC ioctl module");
        return -ENOMEM;
    }

    /* remember ioctl module in TZIOC device */
    tdev->pIoctlMod = pIoctlMod;

    memset(pIoctlMod->handlers, 0, sizeof(pIoctlMod->handlers));

    pIoctlMod->handlers[IOCTL_OFFSET(CLIENT_OPEN )] = tzioc_ioctl_client_open;
    pIoctlMod->handlers[IOCTL_OFFSET(CLIENT_CLOSE)] = tzioc_ioctl_client_close;
    pIoctlMod->handlers[IOCTL_OFFSET(MSG_SEND    )] = tzioc_ioctl_msg_send;
    pIoctlMod->handlers[IOCTL_OFFSET(MEM_ALLOC   )] = tzioc_ioctl_mem_alloc;
    pIoctlMod->handlers[IOCTL_OFFSET(MEM_FREE    )] = tzioc_ioctl_mem_free;
    pIoctlMod->handlers[IOCTL_OFFSET(CALL_SMC    )] = tzioc_ioctl_call_smc;

    LOGI("TZIOC ioctl module initialized");
    return 0;
}

int _tzioc_ioctl_module_deinit(void)
{
    /* reset ioctl module in TZIOC device */
    tdev->pIoctlMod = NULL;

    /* free ioctl module control block */
    kfree(pIoctlMod);

    LOGI("TZIOC ioctl module deinitialized");
    return 0;
}

int _tzioc_ioctl_do_ioctl(struct file *file, uint32_t cmd, void *arg)
{
    if (pIoctlMod->handlers[cmd - TZIOC_IOCTL_FIRST]) {
        return pIoctlMod->handlers[cmd - TZIOC_IOCTL_FIRST](file, arg);
    }
    else {
        LOGE("Unsupported TZIOC ioctl cmd 0x%x", (unsigned int)cmd);
        return -ENOTTY;
    }
}

static int tzioc_ioctl_client_open(struct file *file, void *arg)
{
    struct tzioc_ioctl_client_open_data clientOpenData;
    struct tzioc_client *pClient;
    char *pName;
    int msgQ;

    if (copy_from_user(
            &clientOpenData,
            (void *)arg,
            sizeof(clientOpenData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    pName = clientOpenData.name;
    msgQ = (int)clientOpenData.msgQ;

    if (pName[0] == '\0' ||
        msgQ < 0) {
        LOGE("Invalid args in TZIOC ioctl client open cmd");
        return -EINVAL;
    }

    pClient = _tzioc_user_client_open(
        pName,
        current,
        msgQ);

    if (pClient == NULL) {
        LOGE("Failed to open user client");
        clientOpenData.retVal = -EFAULT;
        goto ERR_RETURN;
    }

    clientOpenData.hClient   = (uint32_t)pClient;
    clientOpenData.id        = (uint32_t)pClient->id;
    clientOpenData.smemStart = tdev->smemStart;
    clientOpenData.smemSize  = tdev->smemSize;
    clientOpenData.retVal    = 0;

 ERR_RETURN:
    if (copy_to_user(
            (void *)arg,
            &clientOpenData,
            sizeof(clientOpenData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int tzioc_ioctl_client_close(struct file *file, void *arg)
{
    struct tzioc_ioctl_client_close_data clientCloseData;
    struct tzioc_client *pClient;

    if (copy_from_user(
            &clientCloseData,
            (void *)arg,
            sizeof(clientCloseData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct tzioc_client *)clientCloseData.hClient;

    if (!pClient) {
        LOGE("Invalid args in TZIOC ioctl client close cmd");
        return -EINVAL;
    }

    _tzioc_user_client_close(pClient);

    clientCloseData.retVal = 0;

    if (copy_to_user(
            (void *)arg,
            &clientCloseData,
            sizeof(clientCloseData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int tzioc_ioctl_msg_send(struct file *file, void *arg)
{
    struct tzioc_ioctl_msg_send_data msgSendData;
    struct tzioc_client *pClient;
    struct tzioc_msg_hdr *pHdr;
    static uint8_t aucPayload[TZIOC_MSG_PAYLOAD_MAX];

    if (copy_from_user(
            &msgSendData,
            (void *)arg,
            sizeof(msgSendData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct tzioc_client *)msgSendData.hClient;
    pHdr = &msgSendData.hdr;

    if (!pClient ||
        pHdr->ucOrig != pClient->id ||
        pHdr->ulLen > TZIOC_MSG_PAYLOAD_MAX) {
        LOGE("Invalid args in TZIOC ioctl client close cmd");
        return -EINVAL;
    }

    if (pHdr->ulLen) {
        if (copy_from_user(
                aucPayload,
                (void *)msgSendData.payloadAddr,
                pHdr->ulLen)) {
            LOGE("Failed to access msg payload");
            return -EFAULT;
        }
    }

    msgSendData.retVal = _tzioc_msg_send(
        pClient,
        pHdr,
        aucPayload);

    if (copy_to_user(
            (void *)arg,
            &msgSendData,
            sizeof(msgSendData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int tzioc_ioctl_mem_alloc(struct file *file, void *arg)
{
    struct tzioc_ioctl_mem_alloc_data memAllocData;
    struct tzioc_client *pClient;
    void *pBuff;
    uint32_t ulBuffOffset;

    if (copy_from_user(
            &memAllocData,
            (void *)arg,
            sizeof(memAllocData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct tzioc_client *)memAllocData.hClient;

    if (!pClient) {
        LOGE("Invalid args in TZIOC ioctl client close cmd");
        return -EINVAL;
    }

    pBuff = _tzioc_mem_alloc(
        pClient,
        memAllocData.size);

    if (!pBuff) {
        LOGE("Failed to alloc mem");
        memAllocData.retVal = -ENOMEM;
        goto ERR_RETURN;
    }

    ulBuffOffset = _tzioc_addr2offset((uint32_t)pBuff);

    if (ulBuffOffset == -1) {
        LOGE("Failed to convert vaddr to offset");
        _tzioc_mem_free(
            pClient,
            pBuff);
        memAllocData.retVal = -EFAULT;
        goto ERR_RETURN;
    }

    memAllocData.buffOffset = ulBuffOffset;
    memAllocData.retVal = 0;

 ERR_RETURN:
    if (copy_to_user(
            (void *)arg,
            &memAllocData,
            sizeof(memAllocData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int tzioc_ioctl_mem_free(struct file *file, void *arg)
{
    struct tzioc_ioctl_mem_free_data memFreeData;
    struct tzioc_client *pClient;
    void *pBuff;

    if (copy_from_user(
            &memFreeData,
            (void *)arg,
            sizeof(memFreeData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct tzioc_client *)memFreeData.hClient;

    if (!pClient) {
        LOGE("Invalid args in TZIOC ioctl client close cmd");
        return -EINVAL;
    }

    pBuff = (void *)_tzioc_offset2addr(memFreeData.buffOffset);

    if ((uint32_t)pBuff == -1) {
        LOGE("Failed to convert offset to vaddr");
        memFreeData.retVal = -EFAULT;
        goto ERR_RETURN;
    }

    _tzioc_mem_free(
        pClient,
        pBuff);

    memFreeData.retVal = 0;

 ERR_RETURN:
    if (copy_to_user(
            (void *)arg,
            &memFreeData,
            sizeof(memFreeData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    return 0;
}

static int tzioc_ioctl_call_smc(struct file *file, void *arg)
{
    struct tzioc_ioctl_call_smc_data callSmcData;
    struct tzioc_client *pClient;

    if (copy_from_user(
            &callSmcData,
            (void *)arg,
            sizeof(callSmcData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    pClient = (struct tzioc_client *)callSmcData.hClient;

    if (!pClient) {
        LOGE("Invalid args in TZIOC ioctl client close cmd");
        return -EINVAL;
    }

    _tzioc_call_smc(callSmcData.mode);

    callSmcData.retVal = 0;

    if (copy_to_user(
            (void *)arg,
            &callSmcData,
            sizeof(callSmcData))) {
        LOGE("Failed to access TZIOC ioctl arguments");
        return -EFAULT;
    };

    return 0;
}
