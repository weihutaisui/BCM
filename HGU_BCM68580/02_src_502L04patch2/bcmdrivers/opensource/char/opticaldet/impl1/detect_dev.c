/*
<:copyright-BRCM:2015:DUAL/GPL:standard

   Copyright (c) 2015 Broadcom 
   All Rights Reserved

Unless you and Broadcom execute a separate written software license
agreement governing use of this software, this software is licensed
to you under the terms of the GNU General Public License version 2
(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
with the following added to such license:

   As a special exception, the copyright holders of this software give
   you permission to link this software with independent modules, and
   to copy and distribute the resulting executable under terms of your
   choice, provided that you also meet, for each linked independent
   module, the terms and conditions of the license of that module.
   An independent module is a module which is not derived from this
   software.  The special exception does not apply to any modifications
   of the software.

Not withstanding the above, under no circumstances may you combine
this software in any way with any other Broadcom software provided
under a license other than the GPL, without Broadcom's express prior
written consent.

:>
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/i2c.h>
#include <linux/slab.h>  /* kzalloc() */
#include <linux/types.h>
#include <bcmsfp_i2c.h>
#include "opticaldet.h"
#include <bcm_intr.h>
#include <boardparms.h>
#include <linux/interrupt.h>
#include <board.h>
#include "detect_dev_trx_data.h"

static int i2c_sfp_cb(struct notifier_block *nb, unsigned long action, void *data);
static struct notifier_block i2c_sfp_nb = {
    .notifier_call = i2c_sfp_cb,
};

static int _file_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int _file_release(struct inode *inode, struct file *file)
{
    return 0;
}

static long _detect_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
#ifdef CONFIG_BCM96838
    unsigned long *val = (unsigned long *)arg;
#endif

    switch (cmd)
    {
#ifdef CONFIG_BCM96838
        case OPTICALDET_IOCTL_DETECT:
            *val = opticaldetect();
            break;
        case OPTICALDET_IOCTL_SD:
            *val = signalDetect();
            break;
#endif
        default:
            printk("%s: ERROR: No such IOCTL", __FILE__);
            return -1;
    }

    return 0;
}

static const struct file_operations detect_file_ops =
{
    .owner = THIS_MODULE,
    .open = _file_open,
    .release = _file_release,
    .unlocked_ioctl = _detect_ioctl,
#if defined(CONFIG_COMPAT)
    .compat_ioctl = _detect_ioctl,
#endif
};

static void trx_init(int bus) 
{
    if( bus >= MAX_I2C_BUS )
        return;

    if( i2c_read_trx_data(bus) == 0 ) {
        trx_fixup(bus);
        trx_activate (bus);
    }
    return;
}

static void trx_deinit(int bus) 
{
    if( bus >= MAX_I2C_BUS )
        return;

    i2c_clear_trx_data(bus);
    return;
}

static int i2c_sfp_cb(struct notifier_block *nb, unsigned long action, void *data)
{
    int bus;

    bus = *((int*)data);
    if( action == SFP_STATUS_INSERTED )
        trx_init(bus);
    else 
        trx_deinit(bus);

    return NOTIFY_OK;
}

int __init detect_init(void)
{
    int ret, i;

    bcm_i2c_sfp_register_notifier(&i2c_sfp_nb);

    for( i = 0; i < MAX_I2C_BUS; i++ ) {
        if ( bcm_i2c_sfp_get_status(i) == SFP_STATUS_INSERTED ) {
            trx_init(i);
        }
    }

    ret = register_chrdev(DEV_MAJOR, DEV_CLASS, &detect_file_ops);
    pr_info(KERN_ALERT "Optical detection module %s.\n", ret ?
        "failed to load" : "loaded");

    return ret;
}
module_init(detect_init);

static void __exit detect_exit(void)
{
    bcm_i2c_sfp_unregister_notifier(&i2c_sfp_nb);

    unregister_chrdev(DEV_MAJOR, DEV_CLASS);
}
module_exit(detect_exit);

MODULE_AUTHOR("Broadcom");
MODULE_DESCRIPTION("Optical WAN detect driver");
MODULE_LICENSE("GPL");

