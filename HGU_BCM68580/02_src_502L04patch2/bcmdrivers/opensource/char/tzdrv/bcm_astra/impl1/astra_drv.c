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

#include <linux/version.h>
#include <linux/kconfig.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/jiffies.h>

#include "astra_version.h"
#include "astra_drv.h"
#include "astra_ioctl.h"
#include "uappd_msg.h"

/*
 * Function Declarations
 */

static int astra_mdev_open(
    struct inode *inode,
    struct file *file);

static int astra_mdev_release(
    struct inode *ignored,
    struct file *file);

static int astra_mdev_mmap(
    struct file *file,
    struct vm_area_struct *vma);

static long astra_mdev_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long arg);

static int astra_coredev_open(
    struct inode *inode,
    struct file *file);

static int astra_coredev_release(
    struct inode *ignored,
    struct file *file);

static int astra_coredev_mmap(
    struct file *file,
    struct vm_area_struct *vma);


int __init astra_module_init(void);
void __exit astra_module_exit(void);
int astra_module_deinit(void);

/*
 * Variable Declarations
 */

static char astra_mdev_name[16]="astra";
module_param_string(adevname, astra_mdev_name, sizeof(astra_mdev_name), 0);

static const struct file_operations astra_mdev_fops = {
    .owner          = THIS_MODULE,
    .read           = NULL,
    .write          = NULL,
    .open           = astra_mdev_open,
    .release        = astra_mdev_release,
    .unlocked_ioctl = astra_mdev_ioctl,
    .mmap           = astra_mdev_mmap,
};

static struct astra_device astra_adev;
struct astra_device *adev = &astra_adev;

/* Core Dump Device */
static char astra_coredev_name[16]="astra_coredump";
module_param_string(coredevname, astra_coredev_name, sizeof(astra_coredev_name), 0);

static const struct file_operations astra_coredev_fops = {
    .owner          = THIS_MODULE,
    .read           = NULL,
    .write          = NULL,
    .open           = astra_coredev_open,
    .release        = astra_coredev_release,
    .unlocked_ioctl = NULL,
    .mmap           = astra_coredev_mmap,
};

static struct miscdevice astra_coredev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = astra_coredev_name,
    .fops = &astra_coredev_fops,
};

static struct coredump_device coredump_dev;
struct coredump_device *cdev = &coredump_dev;

/*
 * Misc Device Functions
 */

static int astra_mdev_open(
    struct inode *inode,
    struct file *file)
{
    int err = 0;

    err = generic_file_open(inode, file);
    if (unlikely(err)) {
        LOGE("Failed to open inode!");
        return err;
    }

    /* alloc client array */
    file->private_data = kzalloc(
        sizeof(struct astra_client *) * ASTRA_CLIENT_NUM_MAX,
        GFP_KERNEL);

    return 0;
}

static int astra_mdev_release(
    struct inode *ignored,
    struct file *file)
{
    struct astra_client **pClients;
    int i;

    /* close all clients */
    pClients = (struct astra_client **)file->private_data;
    for (i = 0; i < ASTRA_CLIENT_NUM_MAX; i++) {
        if (pClients[i]) {
            _astra_user_client_close(pClients[i]);
        }
    }

    /* free client array */
    kfree(file->private_data);

    return 0;
}

static int astra_mdev_mmap(
    struct file *file,
    struct vm_area_struct *vma)
{
    unsigned offset = vma->vm_pgoff << PAGE_SHIFT;
    unsigned size = vma->vm_end - vma->vm_start;

    if (offset + size > tdev->smemSize)
        return -EINVAL;

#if IOREMAP_SHARED_MEM
    /* allow uncached mmap */
    if (file->f_flags & O_SYNC)
        vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
#endif

    return remap_pfn_range(
        vma,
        vma->vm_start,
        (tdev->smemStart + offset) >> PAGE_SHIFT,
        size,
        vma->vm_page_prot);
}

static long astra_mdev_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{
    if (!file) {
        LOGE("Invalid file in ioctl call");
        return -EBADF;
    }

    if (cmd < ASTRA_IOCTL_FIRST ||
        cmd >= ASTRA_IOCTL_LAST) {
        LOGE("Invalid cmd 0x%x in ioctl call", cmd);
        return -EINVAL;
    }

    return _astra_ioctl_do_ioctl(file, cmd, (void *)arg);
}

/*
 * Astra Module Functions
 */

int __init astra_module_init(void)
{
    int err = 0;
    dev_t devid = 0;

    adev->ascdev = cdev_alloc();

    err = alloc_chrdev_region (&devid, 0, ASTRA_NUM_DEVICES, astra_mdev_name);
    if (err < 0) {
        LOGE ("Failed to allocate astra chrdev\n");
        goto ERR_EXIT;
    }

    adev->astra_major = MAJOR(devid);

    adev->asclass = class_create (THIS_MODULE, astra_mdev_name);
    if (IS_ERR(adev->asclass)) {
        LOGE ("astra class creation failed\n");
        goto ERR_EXIT;
    }

    cdev_init (adev->ascdev, &astra_mdev_fops);

    err = cdev_add (adev->ascdev, MKDEV(adev->astra_major, 0), ASTRA_NUM_DEVICES);

    if (err) {
        LOGE ("Unable to add astra cdev\n");
        goto ERR_EXIT;
    }
    adev->asdevice = device_create (adev->asclass, NULL, MKDEV(adev->astra_major, 0), NULL, astra_mdev_name, 0);

    if (IS_ERR (adev->asdevice)) {
        LOGE ("astra device creation failed\n");
        goto ERR_EXIT;
    }

    /* register Core Dump device */
    err = misc_register(&astra_coredev);
    if (err) {
        LOGE("Failed to register coredump device");
        goto ERR_EXIT;
    }

    cdev->mdev = &astra_coredev;

    /* init ioctl module */
    err = _astra_ioctl_module_init();
    if (err) {
        LOGE("Failed to init astra ioctl module");
        goto ERR_EXIT;
    }

    /* remember astra device in TZIOC device */
    tdev->adev = adev;

    LOGI("Astra initialized");
    return 0;

 ERR_EXIT:
    astra_module_deinit();
    return err;
}

void __exit astra_module_exit(void)
{
    astra_module_deinit();
}

int astra_module_deinit(void)
{
    int err = 0;

    /* reset astra device in TZIOC device */
    tdev->adev = NULL;

    /* deinit ioctl module */
    if (adev->pIoctlMod)
        _astra_ioctl_module_deinit();

    if (!IS_ERR(adev->asdevice)) 
        device_destroy (adev->asclass, MKDEV (adev->astra_major, 0));

    if (!IS_ERR(adev->asclass))
        class_destroy (adev->asclass);

    if (adev->astra_major)
        unregister_chrdev_region (MKDEV(adev->astra_major, 0), ASTRA_NUM_DEVICES);

    if (adev->ascdev) {
        cdev_del (adev->ascdev);
        adev->ascdev = NULL;
    }

    /* deregister coredump device */
    if (cdev->mdev)
        misc_deregister(cdev->mdev);

    LOGI("Astra uninitialized");
    return err;
}

/*
 * Astra Core Functions
 */

static struct astra_uapp *astra_find_uapp_by_name(
    struct astra_client *pClient,
    const char *pName)
{
    struct astra_uapp *pUapp;
    int i;

    spin_lock(&pClient->lock);

    for (i = 0; i < ASTRA_UAPP_NUM_MAX; i++) {
        if (pClient->pUapps[i]) {
            pUapp = pClient->pUapps[i];

            if (!strncmp(pUapp->name, pName, ASTRA_NAME_LEN_MAX)) {
                spin_unlock(&pClient->lock);
                return pUapp;
            }
        }
    }

    spin_unlock(&pClient->lock);
    return NULL;
}

static struct astra_peer *astra_find_peer_by_name(
    struct astra_client *pClient,
    const char *pName)
{
    struct astra_uapp *pUapp;
    struct astra_peer *pPeer;
    int i, j;

    spin_lock(&pClient->lock);

    for (i = 0; i < ASTRA_UAPP_NUM_MAX; i++) {
        if (pClient->pUapps[i]) {
            pUapp = pClient->pUapps[i];

            spin_lock(&pUapp->lock);

            for (j = 0; j < ASTRA_PEER_NUM_MAX; j++) {
                if (pUapp->pPeers[j]) {
                    pPeer = pUapp->pPeers[j];

                    if (!strncmp(pPeer->name, pName, ASTRA_NAME_LEN_MAX)) {
                        spin_unlock(&pUapp->lock);
                        spin_unlock(&pClient->lock);
                        return pPeer;
                    }
                }
            }
            spin_unlock(&pUapp->lock);
        }
    }

    spin_unlock(&pClient->lock);
    return NULL;
}

static struct astra_peer *astra_find_peer_by_tzid(
    struct astra_client *pClient,
    uint8_t tzId)
{
    struct astra_uapp *pUapp;
    struct astra_peer *pPeer;
    int i, j;

    spin_lock(&pClient->lock);

    for (i = 0; i < ASTRA_UAPP_NUM_MAX; i++) {
        if (pClient->pUapps[i]) {
            pUapp = pClient->pUapps[i];

            spin_lock(&pUapp->lock);

            for (j = 0; j < ASTRA_PEER_NUM_MAX; j++) {
                if (pUapp->pPeers[j]) {
                    pPeer = pUapp->pPeers[j];

                    if (pPeer->tzId == tzId) {
                        spin_unlock(&pUapp->lock);
                        spin_unlock(&pClient->lock);
                        return pPeer;
                    }
                }
            }
            spin_unlock(&pUapp->lock);
        }
    }

    spin_unlock(&pClient->lock);
    return NULL;
}

static struct astra_file *astra_find_file_by_path(
    struct astra_client *pClient,
    const char *pPath)
{
    struct astra_file *pFile;
    int i;

    spin_lock(&pClient->lock);

    for (i = 0; i < ASTRA_FILE_NUM_MAX; i++) {
        if (pClient->pFiles[i]) {
            pFile = pClient->pFiles[i];

            if (!strncmp(pFile->path, pPath, ASTRA_NAME_LEN_MAX)) {
                spin_unlock(&pClient->lock);
                return pFile;
            }
        }
    }

    spin_unlock(&pClient->lock);
    return NULL;
}

static int astra_uapp_start_rpy_proc(
    struct astra_client *pClient,
    tzioc_msg_hdr *pHdr)
{
    struct uappd_msg_uapp_start_rpy *pRpy =
        (struct uappd_msg_uapp_start_rpy *)TZIOC_MSG_PAYLOAD(pHdr);
    struct astra_uapp *pUapp;

    if (pHdr->ulLen  != sizeof(*pRpy)) {
        LOGE("Invalid userapp start rpy received");
        return -EBADMSG;
    }

    LOGD("Userapp start rpy: name %s, retVal %d",
         pRpy->name, pRpy->retVal);

    /* find astra userapp */
    pUapp = astra_find_uapp_by_name(pClient, pRpy->name);

    if (!pUapp) {
        LOGE("Failed to find astra userapp %s", pRpy->name);
        return -ENOENT;
    }

    if (!pUapp->tzStartWait) {
        LOGE("Invalid state of astra userapp %s", pRpy->name);
        return -EINVAL;
    }

    /* wake up userapp open func with return value */
    pUapp->tzStartWait = false;
    pUapp->tzRetVal = pRpy->retVal;
    wake_up_interruptible(&pUapp->wq);

    return 0;
}

static int astra_uapp_stop_rpy_proc(
    struct astra_client *pClient,
    tzioc_msg_hdr *pHdr)
{
    struct uappd_msg_uapp_stop_rpy *pRpy =
        (struct uappd_msg_uapp_stop_rpy *)TZIOC_MSG_PAYLOAD(pHdr);
    struct astra_uapp *pUapp;

    if (pHdr->ulLen  != sizeof(*pRpy)) {
        LOGE("Invalid userapp stop rpy received");
        return -EBADMSG;
    }

    LOGD("Userapp stop rpy: name %s, retVal %d",
         pRpy->name, pRpy->retVal);

    /* find astra userapp */
    pUapp = astra_find_uapp_by_name(pClient, pRpy->name);

    if (!pUapp) {
        LOGE("Failed to find astra userapp %s", pRpy->name);
        return -ENOENT;
    }

    if (!pUapp->tzStopWait) {
        LOGE("Invalid state of astra userapp %s", pRpy->name);
        return -EINVAL;
    }

    /* wake up userapp close func with return value */
    pUapp->tzStopWait = false;
    pUapp->tzRetVal = pRpy->retVal;
    wake_up_interruptible(&pUapp->wq);

    return 0;
}

static int astra_uapp_exit_nfy_proc(
    struct astra_client *pClient,
    tzioc_msg_hdr *pHdr)
{
    struct uappd_msg_uapp_exit_nfy *pNfy =
        (struct uappd_msg_uapp_exit_nfy *)TZIOC_MSG_PAYLOAD(pHdr);
    struct astra_uapp *pUapp;

    if (pHdr->ulLen  != sizeof(*pNfy)) {
        LOGE("Invalid userapp exit nfy received");
        return -EBADMSG;
    }

    LOGD("Userapp exit nfy: name %s",
         pNfy->name);

    /* find astra userapp */
    pUapp = astra_find_uapp_by_name(pClient, pNfy->name);

    if (!pUapp) {
        LOGE("Failed to find astra userapp %s", pNfy->name);
        return -ENOENT;
    }

    /* callback with userapp exit event */
    if (pClient->pCallback) {
        pClient->pCallback(
            ASTRA_EVENT_UAPP_EXIT,
            (astra_uapp_handle)pUapp,
            pClient->pPrivData);
    }

    return 0;
}

static int astra_uapp_getid_rpy_proc(
    struct astra_client *pClient,
    tzioc_msg_hdr *pHdr)
{
    struct uappd_msg_uapp_getid_rpy *pRpy =
        (struct uappd_msg_uapp_getid_rpy *)TZIOC_MSG_PAYLOAD(pHdr);
    struct astra_peer *pPeer;

    if (pHdr->ulLen  != sizeof(*pRpy)) {
        LOGE("Invalid userapp getid rpy received");
        return -EBADMSG;
    }

    LOGD("Userapp getid rpy: name %s, retVal %d, id %d",
         pRpy->name, pRpy->retVal, pRpy->id);

    /* find astra peer */
    pPeer = astra_find_peer_by_name(pClient, pRpy->name);

    if (!pPeer) {
        LOGE("Failed to find astra peer %s", pRpy->name);
        return -ENOENT;
    }

    if (!pPeer->tzGetIdWait) {
        LOGE("Invalid state of astra peer %s", pRpy->name);
        return -EINVAL;
    }

    /* wake up peer open func with return value */
    pPeer->tzGetIdWait = false;
    pPeer->tzRetVal = pRpy->retVal;
    pPeer->tzId = pRpy->id;
    wake_up_interruptible(&pPeer->wq);

    return 0;
}

static int astra_file_open_rpy_proc(
    struct astra_client *pClient,
    tzioc_msg_hdr *pHdr)
{
    struct uappd_msg_file_open_rpy *pRpy =
        (struct uappd_msg_file_open_rpy *)TZIOC_MSG_PAYLOAD(pHdr);
    struct astra_file *pFile;

    if (pHdr->ulLen  != sizeof(*pRpy)) {
        LOGE("Invalid file open rpy received");
        return -EBADMSG;
    }

    LOGD("File open rpy: path %s, retVal %d",
         pRpy->path, pRpy->retVal);

    /* find astra file */
    pFile = astra_find_file_by_path(pClient, pRpy->path);

    if (!pFile) {
        LOGE("Failed to find astra file %s", pRpy->path);
        return -ENOENT;
    }

    if (!pFile->tzOpenWait) {
        LOGE("Invalid state of astra file %s", pRpy->path);
        return -EINVAL;
    }

    /* wake up file open func with return value */
    pFile->tzOpenWait = false;
    pFile->tzRetVal = pRpy->retVal;
    wake_up_interruptible(&pFile->wq);

    return 0;
}

static int astra_file_close_rpy_proc(
    struct astra_client *pClient,
    tzioc_msg_hdr *pHdr)
{
    struct uappd_msg_file_close_rpy *pRpy =
        (struct uappd_msg_file_close_rpy *)TZIOC_MSG_PAYLOAD(pHdr);
    struct astra_file *pFile;

    if (pHdr->ulLen  != sizeof(*pRpy)) {
        LOGE("Invalid file close rpy received");
        return -EBADMSG;
    }

    LOGD("File close rpy: path %s, retVal %d",
         pRpy->path, pRpy->retVal);

    /* find astra file */
    pFile = astra_find_file_by_path(pClient, pRpy->path);

    if (!pFile) {
        LOGE("Failed to find astra file %s", pRpy->path);
        return -ENOENT;
    }

    if (!pFile->tzCloseWait) {
        LOGE("Invalid state of astra file %s", pRpy->path);
        return -EINVAL;
    }

    /* wake up file close func with return value */
    pFile->tzCloseWait = false;
    pFile->tzRetVal = pRpy->retVal;
    wake_up_interruptible(&pFile->wq);

    return 0;
}

static int astra_file_write_rpy_proc(
    struct astra_client *pClient,
    tzioc_msg_hdr *pHdr)
{
    struct uappd_msg_file_write_rpy *pRpy =
        (struct uappd_msg_file_write_rpy *)TZIOC_MSG_PAYLOAD(pHdr);
    struct astra_file *pFile;

    if (pHdr->ulLen  != sizeof(*pRpy)) {
        LOGE("Invalid file write rpy received");
        return -EBADMSG;
    }

    LOGD("File write rpy: path %s, retVal %d",
         pRpy->path, pRpy->retVal);

    /* find astra file */
    pFile = astra_find_file_by_path(pClient, pRpy->path);

    if (!pFile) {
        LOGE("Failed to find astra file %s", pRpy->path);
        return -ENOENT;
    }

    if (!pFile->tzWriteWait) {
        LOGE("Invalid state of astra file %s", pRpy->path);
        return -EINVAL;
    }

    /* wake up file write func with return value */
    pFile->tzWriteWait = false;
    pFile->tzRetVal = pRpy->retVal;
    wake_up_interruptible(&pFile->wq);

    return 0;
}

static int astra_file_read_rpy_proc(
    struct astra_client *pClient,
    tzioc_msg_hdr *pHdr)
{
    struct uappd_msg_file_read_rpy *pRpy =
        (struct uappd_msg_file_read_rpy *)TZIOC_MSG_PAYLOAD(pHdr);
    struct astra_file *pFile;

    if (pHdr->ulLen  != sizeof(*pRpy)) {
        LOGE("Invalid file read rpy received");
        return -EBADMSG;
    }

    LOGD("File read rpy: path %s, retVal %d",
         pRpy->path, pRpy->retVal);

    /* find astra file */
    pFile = astra_find_file_by_path(pClient, pRpy->path);

    if (!pFile) {
        LOGE("Failed to find astra file %s", pRpy->path);
        return -ENOENT;
    }

    if (!pFile->tzReadWait) {
        LOGE("Invalid state of astra file %s", pRpy->path);
        return -EINVAL;
    }

    /* wake up file read func with return value */
    pFile->tzReadWait = false;
    pFile->tzRetVal = pRpy->retVal;
    wake_up_interruptible(&pFile->wq);

    return 0;
}

static int astra_uapp_coredump_rpy_proc(
    struct astra_client *pClient,
    tzioc_msg_hdr *pHdr)
{
    struct uappd_msg_uapp_coredump_rpy *pRpy =
        (struct uappd_msg_uapp_coredump_rpy *)TZIOC_MSG_PAYLOAD(pHdr);
    struct astra_uapp *pUapp;

    if (pHdr->ulLen  != sizeof(*pRpy)) {
        LOGE("Invalid file read rpy received");
        return -EBADMSG;
    }

    LOGI("CoreDump rpy: retVal %d", pRpy->retVal);

    /* find astra userapp */
    pUapp = astra_find_uapp_by_name(pClient, pRpy->name);

    if (!pUapp) {
        LOGE("Failed to find astra userapp %s", pRpy->name);
        return -ENOENT;
    }

    if (!pUapp->tzStopWait) {
        LOGE("Invalid state of astra userapp %s", pRpy->name);
        return -EINVAL;
    }

    /* wake up userapp close func with return value */
    pUapp->tzStopWait = false;
    pUapp->tzRetVal = pRpy->retVal;
    wake_up_interruptible(&pUapp->wq);

    return 0;
}


static uint32_t astra_msg_ring_offset2addr(uint32_t ulOffset) {
    return ulOffset;
}

static inline bool astra_msg_ring_empty(
    struct tzioc_ring_buf *pRing)
{
    return (pRing->ulWrOffset == pRing->ulRdOffset);
}

static int astra_msg_ring_write(
    struct tzioc_ring_buf *pRing,
    struct tzioc_msg_hdr *pTzHdr,
    void *pMsg)
{
    uint32_t ulWrOffset;
    uint32_t ulSpace, ulNxSpace;
    int err = 0;

    ulWrOffset = pRing->ulWrOffset;

    /* write hdr */
    err = ring_poke(
        pRing,
        ulWrOffset,
        (uint8_t *)pTzHdr,
        sizeof(*pTzHdr));

    if (err) return err;

    ulWrOffset = ring_wrap(
        pRing,
        ulWrOffset,
        sizeof(*pTzHdr));

    if (pTzHdr->ulLen == 0) goto DONE;

    /* skip end of buffer if necessary */
    ulNxSpace = pRing->ulBuffOffset + pRing->ulBuffSize - ulWrOffset;

    if (ulNxSpace < pTzHdr->ulLen) {
        ulSpace = ring_space(
            pRing,
            ulWrOffset,
            pRing->ulRdOffset);

        if (ulSpace < ulNxSpace + pTzHdr->ulLen) {
            return -ENOSPC;
        }
        ulWrOffset = pRing->ulBuffOffset;
    }

    /* write payload */
    err = ring_poke(
        pRing,
        ulWrOffset,
        pMsg,
        pTzHdr->ulLen);

    if (err) return err;

    ulWrOffset = ring_wrap(
        pRing,
        ulWrOffset,
        pTzHdr->ulLen);

 DONE:
    pRing->ulWrOffset = ulWrOffset;
    return 0;
}

static int astra_msg_ring_read(
    struct tzioc_ring_buf *pRing,
    struct tzioc_msg_hdr *pTzHdr,
    void *pMsg)
{
    uint32_t ulRdOffset;
    uint32_t ulSpace, ulNxSpace;
    int err = 0;

    ulRdOffset = pRing->ulRdOffset;

    /* read hdr */
    err = ring_peek(
        pRing,
        ulRdOffset,
        (uint8_t *)pTzHdr,
        sizeof(*pTzHdr));

    if (err) return err;

    ulRdOffset = ring_wrap(
        pRing,
        ulRdOffset,
        sizeof(*pTzHdr));

    if (pTzHdr->ulLen == 0) goto DONE;

    /* skip end of buffer if necessary */
    ulNxSpace = pRing->ulBuffOffset + pRing->ulBuffSize - ulRdOffset;

    if (ulNxSpace < pTzHdr->ulLen) {
        ulSpace = ring_space(
            pRing,
            ulRdOffset,
            pRing->ulRdOffset);

        if (ulSpace < ulNxSpace + pTzHdr->ulLen) {
            return -EINVAL;
        }
        ulRdOffset = pRing->ulBuffOffset;
    }

    /* read payload */
    err = ring_peek(
        pRing,
        ulRdOffset,
        pMsg,
        pTzHdr->ulLen);

    if (err) return err;

    ulRdOffset = ring_wrap(
        pRing,
        ulRdOffset,
        pTzHdr->ulLen);

 DONE:
    pRing->ulRdOffset = ulRdOffset;
    return 0;
}

static int astra_msg_proc(
    struct tzioc_msg_hdr *pTzHdr,
    uint32_t ulPrivData)
{
    struct astra_client *pClient = (struct astra_client *)ulPrivData;
    int err = 0;

    if (pTzHdr->ucOrig == TZIOC_CLIENT_ID_UAPPD) {
        switch (pTzHdr->ucType) {
        /* userapp start rpy */
        case UAPPD_MSG_UAPP_START:
            astra_uapp_start_rpy_proc(pClient, pTzHdr);
            break;

        /* userapp stop rpy */
        case UAPPD_MSG_UAPP_STOP:
            astra_uapp_stop_rpy_proc(pClient, pTzHdr);
            break;

        /* userapp exit nfy */
        case UAPPD_MSG_UAPP_EXIT:
            astra_uapp_exit_nfy_proc(pClient, pTzHdr);
            break;

        /* userapp getid rpy */
        case UAPPD_MSG_UAPP_GETID:
            astra_uapp_getid_rpy_proc(pClient, pTzHdr);
            break;

        /* file open rpy */
        case UAPPD_MSG_FILE_OPEN:
            astra_file_open_rpy_proc(pClient, pTzHdr);
            break;

        /* file close rpy */
        case UAPPD_MSG_FILE_CLOSE:
            astra_file_close_rpy_proc(pClient, pTzHdr);
            break;

        /* file write rpy */
        case UAPPD_MSG_FILE_WRITE:
            astra_file_write_rpy_proc(pClient, pTzHdr);
            break;

        /* file read rpy */
        case UAPPD_MSG_FILE_READ:
            astra_file_read_rpy_proc(pClient, pTzHdr);
            break;

        /* Coredump rpy */
        case UAPPD_MSG_UAPP_COREDUMP:
            astra_uapp_coredump_rpy_proc(pClient, pTzHdr);
            break;

        default:
            LOGE("Unknown uappd msg %d", pTzHdr->ucType);
            return -ENOENT;
        }

        /* msg proc done */
        return 0;
    }

    /* copy msg to msg ring */
    err = astra_msg_ring_write(
        &pClient->msgRing,
        pTzHdr,
        TZIOC_MSG_PAYLOAD(pTzHdr));

    if (err) {
        LOGE("Failed to copy received msg");
        return err;
    }

    /* callback with msg received event */
    if (pClient->pCallback) {
        pClient->pCallback(
            ASTRA_EVENT_MSG_RECEIVED,
            NULL,
            pClient->pPrivData);
    }

    return 0;
}

static void astra_event_proc(
    astra_event event,
    void *pEventData,
    void *pPrivData)
{
    struct astra_client *pClient = (struct astra_client *)pPrivData;
    tzioc_msg_hdr tzHdr;
    int err = 0;

    /* encapsulate event in msg */
    tzHdr.ucType = event;

    switch (event) {
    case ASTRA_EVENT_MSG_RECEIVED:
        tzHdr.ulLen = 0;
        break;

    case ASTRA_EVENT_UAPP_EXIT:
        tzHdr.ulLen = sizeof(astra_uapp_handle);
        break;

    default:
        LOGE("Unknown astra event %d", event);
        return;
    }

    /* write event to event ring */
    err = astra_msg_ring_write(
        &pClient->eventRing,
        &tzHdr,
        pEventData);

    if (err) {
        LOGE("Failed to write astra event");
        return;
    }

    /* wake up event poll */
    wake_up_interruptible(&pClient->wq);
}

int _astra_version_get(struct astra_version *pVersion)
{
    /* fill in astra version */
    pVersion->major = ASTRA_VERSION_MAJOR;
    pVersion->minor = ASTRA_VERSION_MINOR;
    pVersion->build = ASTRA_VERSION_BUILD;
    return 0;
}

int _astra_config_get(struct astra_config *pConfig)
{
    struct tzioc_config tzConfig;
    int err = 0;

    /* get tzioc config */
    err = _tzioc_config_get(&tzConfig);

    if (err) {
        LOGE("Failed to get tzioc config");
        return err;
    }

    /* fill in astra config */
    pConfig->smemSize = tzConfig.smemSize;
    pConfig->pmemSize = tzConfig.pmemSize;
    return 0;
}

int _astra_status_get(struct astra_status *pStatus)
{
    struct tzioc_status tzStatus;
    int err = 0;

    /* get tzioc status */
    err = _tzioc_status_get(&tzStatus);

    if (err) {
        LOGE("Failed to get tzioc config");
        return err;
    }

    /* fill in astra status */
    pStatus->up = tzStatus.up;
    return 0;
}

struct astra_client *_astra_kernel_client_open(
    const char *pName,
    astra_event_callback pCallback,
    void *pPrivData)
{
    struct astra_client *pClient;

    /* alloc astra client */
    pClient = kzalloc(sizeof(*pClient), GFP_KERNEL);

    if (!pClient) {
        LOGE("Failed to allocate astra client");
        return NULL;
    }

    /* init astra client */
    pClient->magic = ASTRA_CLIENT_MAGIC;
    spin_lock_init(&pClient->lock);
    init_waitqueue_head(&pClient->wq);

    strncpy(pClient->name, pName, ASTRA_NAME_LEN_MAX);
    pClient->pCallback = pCallback;
    pClient->pPrivData = pPrivData;

    /* alloc msg buffer */
    pClient->pMsgBuff = kmalloc(ASTRA_MSG_RING_SIZE, GFP_KERNEL);

    if (!pClient->pMsgBuff) {
        LOGE("Failed to allocate msg ring data buffer");
        goto ERR_EXIT;
    }

    /* init msg ring */
    __tzioc_ring_init(
        &pClient->msgRing,
        (uint32_t)pClient->pMsgBuff,
        ASTRA_MSG_RING_SIZE,
        TZIOC_RING_CREATE | TZIOC_RING_WRITE | TZIOC_RING_READ,
        astra_msg_ring_offset2addr);

    /* open tzioc client */
    pClient->pTzClient = _tzioc_kernel_client_open(
        pName,
        astra_msg_proc,
        (uint32_t)pClient);

    if (!pClient->pTzClient) {
        LOGE("Failed to open tzioc client");
        goto ERR_EXIT;
    }

    return pClient;

 ERR_EXIT:
    _astra_kernel_client_close(pClient);
    return NULL;
}

void _astra_kernel_client_close(
    struct astra_client *pClient)
{
    int i;

    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return;
    }

    /* close all associated userapps */
    for (i = 0; i < ASTRA_UAPP_NUM_MAX; i++) {
        if (pClient->pUapps[i]) {
            _astra_uapp_close(pClient->pUapps[i]);
        }
    }

    /* close all associated files */
    for (i = 0; i < ASTRA_FILE_NUM_MAX; i++) {
        if (pClient->pFiles[i]) {
            _astra_file_close(pClient->pFiles[i]);
        }
    }

    /* close tzioc client */
    if (pClient->pTzClient) {
        _tzioc_kernel_client_close(pClient->pTzClient);
    }

    /* free msg buffer */
    if (pClient->pMsgBuff) {
        kfree(pClient->pMsgBuff);
    }

    /* free astra client */
    pClient->magic = 0;
    kfree(pClient);
}

struct astra_client *_astra_user_client_open(
    const char *pName)
{
    struct astra_client *pClient;

    /* open astra kernel client */
    pClient = _astra_kernel_client_open(
        pName,
        astra_event_proc,
        NULL);

    if (!pClient) {
        LOGE("Failed to allocate astra kernel client");
        return NULL;
    }

    /* set kernel client private data */
    pClient->pPrivData = (void *)pClient;

    /* alloc event buffer */
    pClient->pEventBuff = kmalloc(ASTRA_EVENT_RING_SIZE, GFP_KERNEL);

    if (!pClient->pEventBuff) {
        LOGE("Failed to allocate event ring data buffer");
        goto ERR_EXIT;
    }

    /* init event ring */
    __tzioc_ring_init(
        &pClient->eventRing,
        (uint32_t)pClient->pEventBuff,
        ASTRA_EVENT_RING_SIZE,
        TZIOC_RING_CREATE | TZIOC_RING_WRITE | TZIOC_RING_READ,
        astra_msg_ring_offset2addr);

    return pClient;

 ERR_EXIT:
    _astra_user_client_close(pClient);
    return NULL;
}

void _astra_user_client_close(
    struct astra_client *pClient)
{
    /* free event buffer */
    if (pClient->pEventBuff) {
        kfree(pClient->pEventBuff);
    }

    /* close astra kernel client */
    _astra_kernel_client_close(pClient);
}

struct astra_uapp *_astra_uapp_open(
    struct astra_client *pClient,
    const char *pName,
    const char *pPath)
{
    struct astra_uapp *pUapp;
    int err, i;

    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return NULL;
    }

    /* alloc astra userapp */
    pUapp = kzalloc(sizeof(*pUapp), GFP_KERNEL);

    if (!pUapp) {
        LOGE("Failed to allocate astra userapp");
        return NULL;
    }

    /* init astra userapp */
    pUapp->magic = ASTRA_UAPP_MAGIC;
    spin_lock_init(&pUapp->lock);
    init_waitqueue_head(&pUapp->wq);

    strncpy(pUapp->name, pName, ASTRA_NAME_LEN_MAX);
    pUapp->pClient = pClient;

    /* add to astra client */
    spin_lock(&pClient->lock);

    for (i = 0; i < ASTRA_UAPP_NUM_MAX; i++) {
        if (!pClient->pUapps[i])
            break;
    }

    if (i == ASTRA_UAPP_NUM_MAX) {
        LOGE("Max number of astra userapps reached");
        goto ERR_EXIT;
    }

    pClient->pUapps[i] = pUapp;
    spin_unlock(&pClient->lock);

    /* wait for tzioc reply */
    pUapp->tzStartWait = true;

    /* start tzioc userapp */
    err = _tzioc_peer_start(
        pClient->pTzClient,
        pName,
        pPath,
        false);

    if (err) {
        LOGE("Failed to start tzioc userapp");
        goto ERR_EXIT;
    }

    err = wait_event_interruptible_timeout(
        pUapp->wq,
        !pUapp->tzStartWait,
        msecs_to_jiffies(ASTRA_TIMEOUT_MSEC));

    if (!ASTRA_UAPP_VALID(pUapp)) {
        LOGE("Astra userapp has been destoyed");
        return NULL;
    }

    if (err == 0 || err == -ERESTARTSYS) {
        LOGE("Failed to wait for tzioc userapp start reply");
        goto ERR_EXIT;
    }

    if (pUapp->tzRetVal) {
        LOGE("Failed to start tzioc userapp in secure world");
        goto ERR_EXIT;
    }

    pUapp->tzStarted = true;
    return pUapp;

 ERR_EXIT:
    _astra_uapp_close(pUapp);
    return NULL;
}

void _astra_uapp_close(
    struct astra_uapp *pUapp)
{
    struct astra_client *pClient = pUapp->pClient;
    int err, i;

    if (!ASTRA_UAPP_VALID(pUapp)) {
        LOGE("Invalid astra userapp handle");
        return;
    }

    /* close all associated peers */
    for (i = 0; i < ASTRA_PEER_NUM_MAX; i++) {
        if (pUapp->pPeers[i]) {
            astra_peer_close(pUapp->pPeers[i]);
        }
    }

    /* stop tzioc userapp */
    if (pUapp->tzStarted ||
        pUapp->tzStartWait /* only in force kill case */) {

        /* wait for tzioc reply */
        pUapp->tzStopWait = true;

        err = _tzioc_peer_stop(
            pClient->pTzClient,
            pUapp->name);

        if (err) {
            LOGE("Failed to close tzioc userapp");
            goto ERR_CONT;
        }
    }

    err = wait_event_interruptible_timeout(
        pUapp->wq,
        !pUapp->tzStopWait,
        msecs_to_jiffies(ASTRA_TIMEOUT_MSEC));

    if (!ASTRA_UAPP_VALID(pUapp)) {
        LOGE("Astra userapp has been destoyed");
        return;
    }

    if (err == 0 || err == -ERESTARTSYS) {
        LOGE("Failed to wait for tzioc userapp stop reply");
        goto ERR_CONT;
    }

    if (pUapp->tzRetVal) {
        LOGE("Failed to stop tzioc userapp in secure world");
        goto ERR_CONT;
    }

    pUapp->tzStarted = false;

 ERR_CONT:
    /* remove from astra client */
    if (pUapp->pClient) {
        spin_lock(&pClient->lock);

        for (i = 0; i < ASTRA_UAPP_NUM_MAX; i++) {
            if (pClient->pUapps[i] == pUapp) {
                pClient->pUapps[i] = 0;
                break;
            }
        }

        if (i == ASTRA_UAPP_NUM_MAX) {
            LOGE("Failed to find astra userapps %s in client %s",
                 pUapp->name, pClient->name);
        }
        spin_unlock(&pClient->lock);
    }

    /* free astra userapp */
    pUapp->magic = 0;
    kfree(pUapp);
}

struct astra_peer *_astra_peer_open(
    struct astra_uapp *pUapp,
    const char *pName)
{
    struct astra_peer *pPeer;
    int err, i, attempts;

    if (!ASTRA_UAPP_VALID(pUapp)) {
        LOGE("Invalid astra userapp handle");
        return NULL;
    }

    /* alloc astra peer */
    pPeer = kzalloc(sizeof(*pPeer), GFP_KERNEL);

    if (!pPeer) {
        LOGE("Failed to allocate astra peer");
        return NULL;
    }

    /* init astra peer */
    pPeer->magic = ASTRA_PEER_MAGIC;
    spin_lock_init(&pPeer->lock);
    init_waitqueue_head(&pPeer->wq);

    strncpy(pPeer->name, pName, ASTRA_NAME_LEN_MAX);
    pPeer->pUapp = pUapp;

    /* add to astra userapp */
    spin_lock(&pUapp->lock);

    for (i = 0; i < ASTRA_PEER_NUM_MAX; i++) {
        if (!pUapp->pPeers[i])
            break;
    }

    if (i == ASTRA_PEER_NUM_MAX) {
        LOGE("Max number of astra peer reached");
        goto ERR_EXIT;
    }

    pUapp->pPeers[i] = pPeer;
    spin_unlock(&pUapp->lock);

    /* get tzioc peer id */
    attempts = ASTRA_GETID_ATTEMPTS;
    do {
        /* wait for tzioc peer id */
        pPeer->tzGetIdWait = true;

    err = _tzioc_peer_getid(
        pUapp->pClient->pTzClient,
        pName);

    if (err) {
        LOGE("Failed to get tzioc peer id");
        goto ERR_EXIT;
    }

    err = wait_event_interruptible_timeout(
        pPeer->wq,
        !pPeer->tzGetIdWait,
        msecs_to_jiffies(ASTRA_TIMEOUT_MSEC));

    if (!ASTRA_PEER_VALID(pPeer)) {
        LOGE("Astra peer has been destoyed");
        return NULL;
    }

    if (err == 0 || err == -ERESTARTSYS) {
        LOGE("Failed to wait for tzioc peer id");
        goto ERR_EXIT;
    }

        if (pPeer->tzRetVal == 0) {
            pPeer->tzGotId = true;
            break;
        }
        else if (pPeer->tzRetVal != -ENOENT) {
        LOGE("Failed to get tzioc peer id from secure world");
        goto ERR_EXIT;
    }

        LOGW("Failed to get tzioc peer id from secure world, try again");

    } while (--attempts);

    return pPeer;

 ERR_EXIT:
    _astra_peer_close(pPeer);
    return NULL;
}

void _astra_peer_close(
    struct astra_peer *pPeer)
{
    struct astra_uapp *pUapp = pPeer->pUapp;
    int i;

    if (!ASTRA_PEER_VALID(pPeer)) {
        LOGE("Invalid astra peer handle");
        return;
    }

    /* remove from astra userapp */
    if (pPeer->pUapp) {
        spin_lock(&pUapp->lock);

        for (i = 0; i < ASTRA_PEER_NUM_MAX; i++) {
            if (pUapp->pPeers[i] == pPeer) {
                pUapp->pPeers[i] = 0;
                break;
            }
        }

        if (i == ASTRA_PEER_NUM_MAX) {
            LOGE("Failed to find astra peer %s in userapp %s",
                 pPeer->name, pUapp->name);
        }
        spin_unlock(&pUapp->lock);
    }

    /* free astra peer */
    pPeer->magic = 0;
    kfree(pPeer);
}

int _astra_msg_send(
    struct astra_peer *pPeer,
    const void *pMsg,
    size_t msgLen)
{
    struct astra_uapp *pUapp = pPeer->pUapp;
    struct astra_client *pClient = pUapp->pClient;
    struct tzioc_msg_hdr tzHdr;

    if (!ASTRA_PEER_VALID(pPeer)) {
        LOGE("Invalid astra peer handle");
        return -ENOENT;
    }

    /* fill in tzioc msg header */
    memset(&tzHdr, 0, sizeof(tzHdr));
    tzHdr.ucOrig = pClient->pTzClient->id;
    tzHdr.ucDest = pPeer->tzId;
    tzHdr.ulLen = msgLen;

    return _tzioc_msg_send(
        pClient->pTzClient,
        &tzHdr,
        (uint8_t *)pMsg);
}

int _astra_msg_receive(
    struct astra_client *pClient,
    struct astra_peer **ppPeer,
    void *pMsg,
    size_t *pMsgLen,
    int timeout)
{
    struct astra_peer *pPeer;
    struct tzioc_msg_hdr tzHdr;
    int err = 0;

    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return -ENOENT;
    }

    UNUSED(timeout);

    /* retrieve msg from msg ring */
    err = astra_msg_ring_read(
        &pClient->msgRing,
        &tzHdr,
        pMsg);

    if (err) {
        if (err != -ENOMSG)
            LOGE("Failed to retrieve received msg");
        return err;
    }

    /* find astra peer */
    pPeer = astra_find_peer_by_tzid(pClient, tzHdr.ucOrig);

    if (!pPeer) {
        LOGE("Failed to find astra peer with tzid %d", tzHdr.ucOrig);
        return -ENOENT;
    }

    *ppPeer = pPeer;
    *pMsgLen = tzHdr.ulLen;
    return 0;
}

void *_astra_mem_alloc(
    struct astra_client *pClient,
    size_t size)
{
    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return NULL;
    }

    return _tzioc_mem_alloc(
        pClient->pTzClient,
        size);
}

void _astra_mem_free(
    struct astra_client *pClient,
    void *pBuff)
{
    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return;
    }

    _tzioc_mem_free(
        pClient->pTzClient,
        pBuff);
}

astra_paddr_t _astra_pmem_alloc(
    struct astra_client *pClient,
    size_t size)
{
    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return 0;
    }

    /* To Be Done */
    return 0;
}

void _astra_pmem_free(
    struct astra_client *pClient,
    astra_paddr_t paddr)
{
    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return;
    }

    /* To Be Done */
}

void *_astra_offset2vaddr(
    struct astra_client *pClient,
    uint32_t offset)
{
    uint32_t vaddr;

    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return NULL;
    }

    vaddr = _tzioc_offset2addr(offset);
    return (vaddr == -1) ? NULL : (void *)vaddr;
}

uint32_t _astra_vaddr2offset(
    struct astra_client *pClient,
    void *pBuff)
{
    uint32_t offset;

    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return 0;
    }

    offset = _tzioc_addr2offset((uint32_t)pBuff);
    return (offset == -1) ? 0 : offset;
}

astra_file_handle _astra_file_open(
    struct astra_client *pClient,
    const char *pPath,
    int flags)
{
    struct astra_file *pFile;
    int err, i;

    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return NULL;
    }

    /* alloc astra file */
    pFile = kzalloc(sizeof(*pFile), GFP_KERNEL);

    if (!pFile) {
        LOGE("Failed to allocate astra file");
        return NULL;
    }

    /* init astra file */
    pFile->magic = ASTRA_FILE_MAGIC;
    spin_lock_init(&pFile->lock);

    strncpy(pFile->path, pPath, ASTRA_PATH_LEN_MAX);
    pFile->pClient = pClient;

    init_waitqueue_head(&pFile->wq);

    /* add to astra client */
    spin_lock(&pClient->lock);

    for (i = 0; i < ASTRA_FILE_NUM_MAX; i++) {
        if (!pClient->pFiles[i])
            break;
    }

    if (i == ASTRA_FILE_NUM_MAX) {
        LOGE("Max number of astra files reached");
        goto ERR_EXIT;
    }

    pClient->pFiles[i] = pFile;    
    spin_unlock(&pClient->lock);

    /* wait for tzioc reply */
    pFile->tzOpenWait = true;

    /* open tzioc file */
    err = _tzioc_file_open(
        pClient->pTzClient,
        pPath,
        flags);

    if (err) {
        LOGE("Failed to open tzioc file");
        goto ERR_EXIT;
    }
    
    err = wait_event_interruptible_timeout(
        pFile->wq,
        !pFile->tzOpenWait,
        msecs_to_jiffies(ASTRA_TIMEOUT_MSEC));

    if (!ASTRA_FILE_VALID(pFile)) {
        LOGE("Astra file has been destoyed");
        return NULL;
    }

    if (err == 0 || err == -ERESTARTSYS) {
        LOGE("Failed to wait for tzioc file open reply");
        goto ERR_EXIT;
    }

    if (pFile->tzRetVal) {
        LOGE("Failed to open tzioc file in secure world");
        goto ERR_EXIT;
    }

    pFile->tzOpen = true;
    return pFile;

 ERR_EXIT:
    _astra_file_close(pFile);
    return NULL;
}

void _astra_file_close(
    struct astra_file *pFile)
{
    struct astra_client *pClient = pFile->pClient;
    int err, i;

    if (!ASTRA_FILE_VALID(pFile)) {
        LOGE("Invalid astra file handle");
        return;
    }

    /* wait for tzioc reply */
    pFile->tzCloseWait = true;

    /* close tzioc file */
    err = _tzioc_file_close(
        pClient->pTzClient,
        pFile->path);

    if (err) {
        LOGE("Failed to close tzioc file");
        goto ERR_CONT;
    }
    
    err = wait_event_interruptible_timeout(
        pFile->wq,
        !pFile->tzCloseWait,
        msecs_to_jiffies(ASTRA_TIMEOUT_MSEC));

    if (!ASTRA_FILE_VALID(pFile)) {
        LOGE("Astra file has been destoyed");
        return;
    }

    if (err == 0 || err == -ERESTARTSYS) {
        LOGE("Failed to wait for tzioc file close reply");
        goto ERR_CONT;
    }

    if (pFile->tzRetVal) {
        LOGE("Failed to close tzioc file in secure world");
        goto ERR_CONT;
    }

    pFile->tzOpen = false;

 ERR_CONT:
    /* remove from astra client */
    if (pFile->pClient) {
        spin_lock(&pClient->lock);

        for (i = 0; i < ASTRA_FILE_NUM_MAX; i++) {
            if (pClient->pFiles[i] == pFile) {
                pClient->pFiles[i] = 0;
                break;
            }
        }

        if (i == ASTRA_FILE_NUM_MAX) {
            LOGE("Failed to find astra file %s in client %s",
                 pFile->path, pClient->name);
        }
        spin_unlock(&pClient->lock);
    }

    /* free astra userapp */
    pFile->magic = 0;
    kfree(pFile);
}

int _astra_file_write(
    struct astra_file *pFile,
    astra_paddr_t paddr,
    size_t bytes)
{
    struct astra_client *pClient = pFile->pClient;
    int err;

    if (!ASTRA_FILE_VALID(pFile)) {
        LOGE("Invalid astra file handle");
        return -ENOENT;
    }

    /* wait for tzioc reply */
    pFile->tzWriteWait = true;

    /* write to tzioc file */
    err = _tzioc_file_write(
        pClient->pTzClient,
        pFile->path,
        paddr,
        bytes);

    if (err) {
        LOGE("Failed to write tzioc file");
        goto ERR_CONT;
    }

    err = wait_event_interruptible_timeout(
        pFile->wq,
        !pFile->tzWriteWait,
        msecs_to_jiffies(ASTRA_TIMEOUT_MSEC));

    if (!ASTRA_FILE_VALID(pFile)) {
        LOGE("Astra file has been destoyed");
        return -ENOENT;
    }

    if (err == 0 || err == -ERESTARTSYS) {
        LOGE("Failed to wait for tzioc file write reply");
        if (!err) err = -ETIMEDOUT;
        goto ERR_CONT;
    }

    if (pFile->tzRetVal < 0) {
        LOGE("Failed to write to tzioc file in secure world");
        err = pFile->tzRetVal;
        goto ERR_CONT;
    }

    return pFile->tzRetVal;

 ERR_CONT:
    /* clean up if necessary */
    return err;
}

int _astra_file_read(
    struct astra_file *pFile,
    astra_paddr_t paddr,
    size_t bytes)
{
    struct astra_client *pClient = pFile->pClient;
    int err;

    if (!ASTRA_FILE_VALID(pFile)) {
        LOGE("Invalid astra file handle");
        return -ENOENT;
    }

    /* wait for tzioc reply */
    pFile->tzReadWait = true;

    /* read from tzioc file */
    err = _tzioc_file_read(
        pClient->pTzClient,
        pFile->path,
        paddr,
        bytes);

    if (err) {
        LOGE("Failed to read tzioc file");
        goto ERR_CONT;
    }

    err = wait_event_interruptible_timeout(
        pFile->wq,
        !pFile->tzReadWait,
        msecs_to_jiffies(ASTRA_TIMEOUT_MSEC));

    if (!ASTRA_FILE_VALID(pFile)) {
        LOGE("Astra file has been destoyed");
        return -ENOENT;
    }

    if (err == 0 || err == -ERESTARTSYS) {
        LOGE("Failed to wait for tzioc file read reply");
        if (!err) err = -ETIMEDOUT;
        goto ERR_CONT;
    }

    if (pFile->tzRetVal < 0) {
        LOGE("Failed to read from tzioc file in secure world");
        err = pFile->tzRetVal;
        goto ERR_CONT;
    }

    return pFile->tzRetVal;

 ERR_CONT:
    /* clean up if necessary */
    return err;
}

int _astra_call_smc(
    struct astra_client *pClient,
    uint8_t ucMode)
{
    if (!ASTRA_CLIENT_VALID(pClient)) {
        LOGE("Invalid astra client handle");
        return -ENOENT;
    }

    /* assuming ucMode == SMC callnum */
    return _tzioc_call_smc((uint32_t)ucMode);
}

int _astra_event_poll(
    struct astra_client *pClient,
    astra_event *pEvent,
    void *pEventData,
    size_t *pEventDataLen)
{
    struct tzioc_msg_hdr tzHdr;
    int err = 0;

    if (!(pClient->eventExit ||
          !astra_msg_ring_empty(&pClient->eventRing))) {

        /* wait for astra event */
        err = wait_event_interruptible(
            pClient->wq,
            (pClient->eventExit ||
             !astra_msg_ring_empty(&pClient->eventRing)));

        if (err) {
            LOGE("Failed to wait for astra event");
            return err;
        }
    }

    /* check for event exit */
    if (pClient->eventExit) {
        return -ENODEV;
    }

    /* retrieve event from event ring */
    err = astra_msg_ring_read(
        &pClient->eventRing,
        &tzHdr,
        pEventData);

    if (err) {
        LOGE("Failed to retrieve astra event");
        return err;
    }

    *pEvent = (astra_event)tzHdr.ucType;
    *pEventDataLen = tzHdr.ulLen;
    return 0;
}

int _astra_event_exit(
    struct astra_client *pClient)
{
    /* mark event exit */
    pClient->eventExit = true;

    /* wake up event poll */
    wake_up_interruptible(&pClient->wq);
    return 0;
}

void _astra_uapp_coredump(
    struct astra_uapp *pUapp)
{
    struct astra_client *pClient = pUapp->pClient;
    int err;
    unsigned int * vaddr_1;

    if (!ASTRA_UAPP_VALID(pUapp)) {
        LOGE("Invalid astra userapp handle");
        return;
    }

    if (pUapp->tzStarted ||
        pUapp->tzStartWait /* only in force kill case */) {
        err = _tzioc_peer_coredump(
            pClient->pTzClient,
            pUapp->name,
            cdev->buf_addr,
            cdev->buf_size);

        if (err) {
            LOGE("Failed to coredump userapp");
            goto ERR_CONT;
        }
    }

    /* wait for tzioc reply */
    pUapp->tzStopWait = true;
    err = wait_event_interruptible_timeout(
        pUapp->wq,
        !pUapp->tzStopWait,
        msecs_to_jiffies(ASTRA_TIMEOUT_MSEC));

    if (!ASTRA_UAPP_VALID(pUapp)) {
        LOGE("Astra userapp has been destoyed");
        return;
    }

    if (err == 0 || err == -ERESTARTSYS) {
        LOGE("Failed to wait for tzioc userapp stop reply");
        goto ERR_CONT;
    }

    if (pUapp->tzRetVal) {
        LOGE("Failed to stop tzioc userapp in secure world");
        goto ERR_CONT;
    }
    pUapp->tzStarted = false;

    vaddr_1 = (unsigned int *)__va(cdev->buf_addr);

ERR_CONT:
    _astra_uapp_close(pUapp);
}

/* Core Dump Device Functions */
static int astra_coredev_open(
    struct inode *inode,
    struct file *file)
{
    int err = 0;

    printk("Core device OPen\n");
    err = generic_file_open(inode, file);
    if (unlikely(err)) {
        LOGE("Failed to open inode!");
        return err;
    }
    return 0;
}

static int astra_coredev_release(
    struct inode *ignored,
    struct file *file)
{
    return 0;
}

#define COREDUMP_FILE_SIZE		4*1024*1024

static int astra_coredev_mmap(
    struct file *file,
    struct vm_area_struct *vma)
{
    uint8_t *vaddr;
    unsigned offset = vma->vm_pgoff << PAGE_SHIFT;
    unsigned size = vma->vm_end - vma->vm_start;

    vaddr = kmalloc(COREDUMP_FILE_SIZE, GFP_USER);
    cdev->buf_addr= (astra_paddr_t)__pa(vaddr);
    cdev->buf_size = COREDUMP_FILE_SIZE;

    printk("Core device MMAP size=0x%x\n",size);

    return remap_pfn_range(
        vma,
        vma->vm_start,
        (cdev->buf_addr+offset)>> PAGE_SHIFT,
        size,
        vma->vm_page_prot);
    return 0;
}
