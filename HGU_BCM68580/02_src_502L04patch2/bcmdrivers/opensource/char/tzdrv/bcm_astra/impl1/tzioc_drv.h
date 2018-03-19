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

#ifndef TZIOC_DRV_H
#define TZIOC_DRV_H

#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/printk.h>
#include <linux/cdev.h>
#include <linux/device.h>

/* compiler switches */
#define KERNEL_IPI_PATCH        1
#define KERNEL_BCM_MQ_PATCH     0
#define IOREMAP_SHARED_MEM      0
#define TZIOC_DEV_SUPPORT       1
#define TZIOC_MSG_ECHO          0

#define TZIOC_NUM_DEVICES       1

/* utility macros */
#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#ifndef LOGI
#define LOGD(format, ...) pr_devel("%s: " format "\n", __FUNCTION__, ## __VA_ARGS__)
#define LOGW(format, ...) pr_warn ("%s: " format "\n", __FUNCTION__, ## __VA_ARGS__)
#define LOGE(format, ...) pr_err  ("%s: " format "\n", __FUNCTION__, ## __VA_ARGS__)
#define LOGI(format, ...) pr_info ("%s: " format "\n", __FUNCTION__, ## __VA_ARGS__)
#endif

/* TZIOC config */
struct tzioc_config {
    uint32_t smemSize;
    uint32_t pmemSize;
};

struct tzioc_status {
    bool up;
};

/* TZIOC device */
struct tzioc_device {
#if TZIOC_DEV_SUPPORT
    int tzioc_major;
    struct class *tzclass;
    struct cdev *tzcdev;
    struct device *tzdevice;
#endif /* TZIOC_DEV_SUPPORT */

    /* parameters from device tree */
    uint32_t smemStart;
    uint32_t smemSize;
    uint32_t sysIrq;

    /* shared memory */
    struct tzioc_shared_mem *psmem;

    /* system IRQ work */
    struct work_struct sysIrqWork;

    /* spinlock for data access */
    spinlock_t lock;

    /* ioctl module */
    struct tzioc_ioctl_module *pIoctlMod;

    /* msg module */
    struct tzioc_msg_module *pMsgMod;

    /* mem module */
    struct tzioc_mem_module *pMemMod;

    /* client module */
    struct tzioc_client_module *pClientMod;

    /* system client */
    struct tzioc_client *pSysClient;

    /* astra device */
    struct astra_device *adev;

    /* peer state */
    bool peerUp;
    uint32_t pmemSize;
};

/* exported functions */
int _tzioc_call_smc(uint32_t callnum);

uint32_t _tzioc_offset2addr(uint32_t ulOffset);
uint32_t _tzioc_addr2offset(uint32_t ulAddr);

/* in case physical address, not offset, is used */
uint32_t _tzioc_vaddr2paddr(uint32_t ulVaddr);
uint32_t _tzioc_paddr2vaddr(uint32_t ulPaddr);

int _tzioc_config_get(struct tzioc_config *pConfig);
int _tzioc_status_get(struct tzioc_status *pStatus);

/* global variables */
extern struct tzioc_device *tdev;

#endif /* TZIOC_DRV_H */
