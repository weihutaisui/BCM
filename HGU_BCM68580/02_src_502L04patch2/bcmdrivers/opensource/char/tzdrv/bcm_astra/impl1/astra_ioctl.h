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

#ifndef ASTRA_IOCTL_H
#define ASTRA_IOCTL_H

#include <linux/fs.h>
#include "astra_ioctls.h"

/* ioctl handler type */
typedef int (*astra_ioctl_handler)(struct file *file, void *arg);

/* ioctl module */
struct astra_ioctl_module {
    /* ioctl handlers */
    astra_ioctl_handler handlers[ASTRA_IOCTL_LAST - ASTRA_IOCTL_FIRST];
};

int __init _astra_ioctl_module_init(void);
int _astra_ioctl_module_deinit(void);

int _astra_ioctl_do_ioctl(struct file *file, uint32_t cmd, void *arg);

#endif /* ASTRA_IOCTL_H */
