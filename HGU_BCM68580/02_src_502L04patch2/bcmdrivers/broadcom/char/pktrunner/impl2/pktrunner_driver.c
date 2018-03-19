/*
<:copyright-BRCM:2013:proprietary:standard

   Copyright (c) 2013 Broadcom 
   All Rights Reserved

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:> 
*/

/*
 *******************************************************************************
 * File Name  : pktrunner_driver.c
 *
 * Description: This file contains Linux character device driver entry points
 *              for the Runner Blog Driver.
 *******************************************************************************
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/bcm_log.h>
#include <linux/sysrq.h>

#include <linux/blog.h>

#include <rdpa_api.h>
#include "pktrunner_proto.h"

/* Character Device information */

#define PKT_RUNNER_VERSION               "0.1"
#define PKT_RUNNER_VER_STR               "v" PKT_RUNNER_VERSION
#define PKT_RUNNER_MODNAME               "Broadcom Runner Blog Driver"

#define PKT_RUNNER_DRV_NAME              "pktrunner"
#define PKT_RUNNER_DRV_MAJOR             3009
#define PKT_RUNNER_DRV_DEVICE_NAME       "/dev/" PKT_RUNNER_DRV_NAME

/*
 *------------------------------------------------------------------------------
 * Function Name: pktRunner_ioctl
 * Description  : Main entry point to handle user applications IOCTL requests.
 * Returns      : 0 - success or error
 *------------------------------------------------------------------------------
 */
static long pktRunner_ioctl(struct file *filep, unsigned int command, unsigned long arg)
{
    return 0;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: pktRunner_open
 * Description  : Called when an user application opens this device.
 * Returns      : 0 - success
 *------------------------------------------------------------------------------
 */
static int pktRunner_open(struct inode *inode, struct file *filp)
{
    return 0;
}

/* Global file ops */
static struct file_operations pktRunner_fops_g =
{
    .unlocked_ioctl  = pktRunner_ioctl,
#if defined(CONFIG_COMPAT)
    .compat_ioctl = pktRunner_ioctl,
#endif
    .open   = pktRunner_open,
};

/*
 *------------------------------------------------------------------------------
 * Function Name: pktRunner_construct
 * Description  : Initial function that is called at system startup that
 *                registers this device.
 * Returns      : None.
 *------------------------------------------------------------------------------
 */
int __init pktRunner_construct(void)
{
    int ret;

    bcmLog_setLogLevel(BCM_LOG_ID_PKTRUNNER, BCM_LOG_LEVEL_ERROR);

    ret = runnerProto_construct();
    if(ret != 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PKTRUNNER, "Could not runnerProto_construct");

        goto out;
    }

    if(register_chrdev(PKT_RUNNER_DRV_MAJOR, PKT_RUNNER_DRV_NAME, &pktRunner_fops_g))
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PKTRUNNER, "Unable to get major number <%d>", PKT_RUNNER_DRV_MAJOR);

        ret = -1;

        goto out;
    }

    bcm_printk(PKT_RUNNER_MODNAME " Char Driver " PKT_RUNNER_VER_STR " Registered <%d>\n", PKT_RUNNER_DRV_MAJOR);

out:
    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: pktRunner_destruct
 * Description  : Final function that is called when the module is unloaded.
 * Returns      : None.
 *------------------------------------------------------------------------------
 */
void __exit pktRunner_destruct(void)
{
    runnerProto_destruct();

    unregister_chrdev(PKT_RUNNER_DRV_MAJOR, PKT_RUNNER_DRV_NAME);

    BCM_LOG_NOTICE(BCM_LOG_ID_PKTRUNNER, PKT_RUNNER_MODNAME " Char Driver " PKT_RUNNER_VER_STR
                   " Unregistered<%d>", PKT_RUNNER_DRV_MAJOR);
}

module_init(pktRunner_construct);
module_exit(pktRunner_destruct);

//EXPORT_SYMBOL();

MODULE_DESCRIPTION(PKT_RUNNER_MODNAME);
MODULE_VERSION(PKT_RUNNER_VERSION);
MODULE_LICENSE("Proprietary");
