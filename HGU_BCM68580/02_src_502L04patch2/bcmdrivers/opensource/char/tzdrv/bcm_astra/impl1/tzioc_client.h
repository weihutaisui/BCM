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

#ifndef TZIOC_CLIENT_H
#define TZIOC_CLIENT_H

#include <linux/types.h>
#include <linux/spinlock.h>

#include "tzioc_common.h"

/* client module */
struct tzioc_client_module {
    /* spinlock */
    spinlock_t lock;

    /* client control block */
    struct tzioc_client_cb clientCB;
};

int __init _tzioc_client_module_init(void);
int _tzioc_client_module_deinit(void);

struct tzioc_client *_tzioc_client_find_by_id(uint8_t id);
struct tzioc_client *_tzioc_client_find_by_name(const char *pName);
struct tzioc_client *_tzioc_client_find_by_task(const struct task_struct *pTask);

struct tzioc_client *_tzioc_kernel_client_open(
    const char *pName,
    tzioc_msg_proc_pfn pMsgProc,
    uint32_t ulPrivData);

void _tzioc_kernel_client_close(
    struct tzioc_client *pClient);

struct tzioc_client *_tzioc_user_client_open(
    const char *pName,
    struct task_struct *pTask,
    int msgQ);

void _tzioc_user_client_close(
    struct tzioc_client *pClient);

#endif /* TZIOC_CLIENT_H */
