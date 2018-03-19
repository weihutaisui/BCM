/***************************************************************************
 * <:copyright-BRCM:2016:DUAL/GPL:standard
 * 
 *    Copyright (c) 2016 Broadcom 
 *    All Rights Reserved
 * 
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 * with the following added to such license:
 * 
 *    As a special exception, the copyright holders of this software give
 *    you permission to link this software with independent modules, and
 *    to copy and distribute the resulting executable under terms of your
 *    choice, provided that you also meet, for each linked independent
 *    module, the terms and conditions of the license of that module.
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications
 *    of the software.
 * 
 * Not withstanding the above, under no circumstances may you combine
 * this software in any way with any other Broadcom software provided
 * under a license other than the GPL, without Broadcom's express prior
 * written consent.
 * 
 * :>
 ***************************************************************************/
#ifndef ASTRA_TDRV_H
#define ASTRA_TDRV_H

#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/printk.h>

#include "astra_api.h"

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#ifndef LOGI
#define LOGD(format, ...) pr_debug("%s: " format "\n", __FUNCTION__, ## __VA_ARGS__)
#define LOGW(format, ...) pr_warn ("%s: " format "\n", __FUNCTION__, ## __VA_ARGS__)
#define LOGE(format, ...) pr_err  ("%s: " format "\n", __FUNCTION__, ## __VA_ARGS__)
#define LOGI(format, ...) pr_info ("%s: " format "\n", __FUNCTION__, ## __VA_ARGS__)
#endif

/* astra test control block */
struct astra_tdrv {
    struct astra_version version;
    struct astra_config config;
    struct astra_status status;

    astra_client_handle hClient;
    astra_file_handle hFile;
    astra_uapp_handle hUapp;
    astra_peer_handle hPeer;

    /* msg work */
    struct work_struct msgWork;

    /* outstanding test msg count and lock */
    spinlock_t msgLock;
    volatile int msgCnt;

    /* mem pmap/malloc data */
    uint32_t checksum;
};

#endif /* ASTRA_TDRV_H */
