/*
* <:copyright-BRCM:2015:proprietary:gpon
* 
*    Copyright (c) 2015 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :> 
*/

/* -*- C -*-
 * logger_drv_main.c -- The Linux driver module wrapper to BroadLight GPON STACK.
 *

 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/aio.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/delay.h>		//For delay
#include <linux/device.h>
#include <linux/platform_device.h>

#include "bdmf_system.h"
#include "logger.h"
#include "logger_dev.h"


int logger_drv_major =   3022; /* Value must comply definition in targets/makeDevs */
int logger_drv_qset =    LOGGER_DRV_QSET;
int logger_drv_quantum = LOGGER_DRV_QUANTUM;

module_param(logger_drv_major, int, 0);
module_param(logger_drv_qset, int, 0);
module_param(logger_drv_quantum, int, 0);

struct logger_dev_t *logger_dev = NULL; 
int logger_drv_trim(struct logger_dev_t *dev);
void logger_drv_exit(void);

/* declare one cache pointer: use it for all devices */
struct kmem_cache *logger_drv_cache;

/* read buuffer for reading from queue */
static char *read_buffer = NULL;



/*
 * Open and close
 */

int logger_drv_open (struct inode *inode, struct file *filp)
{
	struct logger_dev_t *dev; /* device information */

	/*  Find the device */
	dev = container_of(inode->i_cdev, struct logger_dev_t, cdev);

	LDEBUG("logger_drv OPEN dev#%d\n",dev->count);

    	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (down_interruptible (&dev->sem))
			return -ERESTARTSYS;
		logger_drv_trim(dev); /* ignore errors */
		up (&dev->sem);
	}

	dev->count++;
	/* and use filp->private_data to point to the device data */
	filp->private_data = dev;

	LDEBUG("logger drv OPEN success\n"); 

	return 0;          /* success */
}

int logger_drv_release (struct inode *inode, struct file *filp)
{
	struct logger_dev_t *dev = filp->private_data; /* device information */

	/*  Find the device */
	dev->count--;
//	printk(KERN_NOTICE "logger_drv RELEASE dev#%d\n",dev->number);
	return 0;
}

/*
 * Follow the list 
 */
struct logger_dev_t *logger_drv_follow(struct logger_dev_t *dev, int n)
{
	while (n--) {
		if (!dev->next) {
			dev->next = kmalloc(sizeof(struct logger_dev_t), GFP_KERNEL);
			memset(dev->next, 0, sizeof(struct logger_dev_t));
		}
		dev = dev->next;
		continue;
	}
	return dev;
}

/*
 * Data management: read and write
 */

ssize_t logger_drv_read (struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct logger_dev_t *dev = filp->private_data; /* the first listitem */

	if (down_interruptible (&dev->sem))
	{
		return -ERESTARTSYS;
	}
	
	LDEBUG("%s\n", __FUNCTION__);

	if (!logger_get_from_queue(read_buffer, (unsigned long *)&count))
	{
		int r;
		if (read_buffer == NULL)
		{
			up(&dev->sem);
			return -EFAULT;
		}

		if (count > LOGGER_BUFFER_SIZE)
			count = LOGGER_BUFFER_SIZE;

		r = copy_to_user(buf, read_buffer, count);
		if (r)
		{
			up(&dev->sem);
			return -EFAULT;
		}

		up(&dev->sem);
		return count;
	}
	up(&dev->sem);
	 
	return -EFAULT;

}



/*
 * The ioctl() implementation
 */


long logger_drv_ioctl (struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
		return  -ENOTTY;
}



/*
 * The fops
 */

struct file_operations logger_drv_fops = {
	.owner =     THIS_MODULE,
	.read =	     logger_drv_read,
	.unlocked_ioctl = logger_drv_ioctl,
#if defined(CONFIG_COMPAT)
	.compat_ioctl = logger_drv_ioctl,
#endif
	.open =	     logger_drv_open,
	.release =   logger_drv_release,
};


int logger_drv_trim(struct logger_dev_t *dev)
{
	struct logger_dev_t *next, *dptr;
	int qset = dev->qset;   
	int i;

	if (dev->vmas) /* don't trim: there are active mappings */
		return -EBUSY;

/* all the list items */
	for (dptr = dev; dptr; dptr = next)
	{ 
		if (dptr->data) 
		{
			for (i = 0; i < qset; i++)
				if (dptr->data[i])
					kmem_cache_free(logger_drv_cache, dptr->data[i]);

			kfree(dptr->data);
			dptr->data=NULL;
		}
		next=dptr->next;
		if (dptr != dev)
			kfree(dptr); /* all of them but the first */
	}
	dev->size = 0;
	dev->qset = logger_drv_qset;
	dev->quantum = logger_drv_quantum;
	dev->next = NULL;
	return 0;
}


static void logger_drv_setup_cdev(struct logger_dev_t *dev, int index)
{
	int err;
	int devno = MKDEV(logger_drv_major, index);
    
	cdev_init(&dev->cdev, &logger_drv_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &logger_drv_fops;
	if ((err = cdev_add (&dev->cdev, devno, 1)))
		printk(KERN_NOTICE "Error %d adding logger%d", err, index);
}


/*
 * Finally, the module stuff
 */
static int is_chrdev_reg;

int logger_drv_init(void)
{
    dev_t dev = MKDEV(logger_drv_major, 0);
    int result;
    /*
     * Register your major, and accept a dynamic number.
     */
    if (!logger_drv_major)
    {
        printk("%s:%d Logger major is not defined\n", __FUNCTION__, __LINE__);
        return -1;
    }
    result = register_chrdev_region(dev, 0, LOGGER_DEVICE_NAME);
    if (result < 0)
        return result;

    is_chrdev_reg = 1;
    /* 
     * allocate the devices -- we can't have them static, as the number
     * can be specified at load time
     */
    logger_dev = kmalloc(sizeof (struct logger_dev_t), GFP_KERNEL);
    if (!logger_dev) 
    {
        printk("ERROR allocate logger device\n");
        unregister_chrdev_region(dev, 1);
        return -ENOMEM;
    }

    memset(logger_dev, 0, sizeof (struct logger_dev_t));
    logger_dev->quantum = logger_drv_quantum;
    logger_dev->qset = logger_drv_qset;
    sema_init (&logger_dev->sem, 1);
    logger_drv_setup_cdev(logger_dev, 0);

    /* Create a cache: Returns a ptr to the cache on success, NULL on failure. */
    logger_drv_cache = kmem_cache_create(LOGGER_DEVICE_NAME, logger_drv_quantum,
        SLAB_HWCACHE_ALIGN, 0, NULL); 

    if (!logger_drv_cache)
    {
        logger_drv_exit();
        return -ENOMEM;
    }

    /* Allocate the read buffer - can't allocated static, as its size > 1024 */
    read_buffer = kmalloc(LOGGER_BUFFER_SIZE+200, GFP_KERNEL);
    if (!read_buffer) 
    {
        printk("ERROR allocate read_buffer\n");
        unregister_chrdev_region(dev, 1);
        return -ENOMEM;
    }

    printk("LOGGER driver Init Done: %lu\n", logger_dev->log_id); 

    return 0; /* succeed */

}

void logger_drv_exit(void)
{
    printk(KERN_NOTICE "logger_drv END\n");

    if (!is_chrdev_reg)
        return; /* Nothing has been registered */
    cdev_del(&logger_dev->cdev);
    logger_drv_trim(logger_dev);
    kfree(logger_dev);
    kfree(read_buffer);

    if (logger_drv_cache)
        kmem_cache_destroy(logger_drv_cache);

    unregister_chrdev_region(MKDEV (logger_drv_major, 0), 1);

}
struct logger_dev_t* logger_get_logger_dev(void)
{
	return (struct logger_dev_t *)logger_dev; 
}
#if defined(EXPORT_LOGGER_SYMBOLS)
EXPORT_SYMBOL(logger_get_logger_dev); 
#endif

