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
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "asm/opcodes-smc.h"

#include "astra_version.h"
#include "tzioc_api.h"
#include "tzioc_ioctls.h"
#include "tzioc_drv.h"
#include "tzioc_ioctl.h"
#include "tzioc_msg.h"
#include "tzioc_mem.h"
#include "tzioc_client.h"
#include "tzioc_sys_msg.h"
#include "tracelog.h"
#include "vuart.h"

#if !IOREMAP_SHARED_MEM
//#include "linux/brcmstb/memory_api.h"
void *brcmstb_memory_kva_map_phys(phys_addr_t phys, size_t size, bool cached);
int brcmstb_memory_kva_unmap(const void *kva);
#endif

#if KERNEL_BCM_MQ_PATCH
extern long bcm_mq_timedsend(
    struct task_struct *mqtask,
    mqd_t mqdes,
    const char *u_msg_ptr,
    size_t msg_len,
    unsigned int msg_prio,
    const struct timespec *u_abs_timeout);
#endif

/*
 * Function Declarations
 */

#if TZIOC_DEV_SUPPORT
static int tzioc_mdev_open(
    struct inode *inode,
    struct file *file);

static int tzioc_mdev_release(
    struct inode *ignored,
    struct file *file);

static int tzioc_mdev_mmap(
    struct file *file,
    struct vm_area_struct *vma);

static long tzioc_mdev_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long arg);
#endif /* TZIOC_DEV_SUPPORT */

static int __init tzioc_module_init(void);
static void __exit tzioc_module_exit(void);
static int tzioc_module_deinit(void);

#if KERNEL_IPI_PATCH
static void tzioc_isr(void);
#endif
static void tzioc_proc(struct work_struct *work);

static int tzioc_sys_msg_proc(struct tzioc_msg_hdr *pHdr);
#if TZIOC_MSG_ECHO
static int tzioc_echo_msg_proc(struct tzioc_msg_hdr *pHdr);
#endif /*TZIOC_MSG_ECHO */

extern int __init astra_module_init(void);
extern void __exit astra_module_exit(void);
extern int astra_module_deinit(void);

/*
 * Variable Declarations
 */

#if TZIOC_DEV_SUPPORT
static char tzioc_mdev_name[16] = "tzioc";
module_param_string(mdevname, tzioc_mdev_name, sizeof(tzioc_mdev_name), 0);

static const struct file_operations tzioc_mdev_fops = {
    .owner          = THIS_MODULE,
    .read           = NULL,
    .write          = NULL,
    .open           = tzioc_mdev_open,
    .release        = tzioc_mdev_release,
    .unlocked_ioctl = tzioc_mdev_ioctl,
    .mmap           = tzioc_mdev_mmap,
};

#endif /* TZIOC_DEV_SUPPORT */

static struct tzioc_device tzioc_tdev;
struct tzioc_device *tdev = &tzioc_tdev;

#if TZIOC_DEV_SUPPORT

/*
 * Misc Device Functions
 */

static int tzioc_mdev_open(
    struct inode *inode,
    struct file *file)
{
    int err = 0;

    err = generic_file_open(inode, file);
    if (unlikely(err)) {
        LOGE("Failed to open inode!");
        return err;
    }
    return 0;
}

static int tzioc_mdev_release(
    struct inode *ignored,
    struct file *file)
{
    return 0;
}

static int tzioc_mdev_mmap(
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

static long tzioc_mdev_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{
    if (!file) {
        LOGE("Invalid file in ioctl call");
        return -EBADF;
    }

    if (cmd < TZIOC_IOCTL_FIRST ||
        cmd >= TZIOC_IOCTL_LAST) {
        LOGE("Invalid cmd 0x%x in ioctl call", cmd);
        return -EINVAL;
    }

    return _tzioc_ioctl_do_ioctl(file, cmd, (void *)arg);
}

#endif /* TZIOC_DEV_SUPPORT */

/*
 * TZIOC Module Functions
 */

static int __init tzioc_module_init(void)
{
    struct device_node *node;
    const uint32_t *smem_params;
    uint64_t smemStart64, smemSize64;
    uint32_t version;
    uint32_t sysIrq;
    struct tzioc_msg_hdr hdr;
    int err = 0;
#if TZIOC_DEV_SUPPORT
    dev_t devid = 0;
#endif

    /* clear TZIOC device control block */
    memset(tdev, 0, sizeof(*tdev));

    /* find TZIOC device tree node */
    node = of_find_node_by_name(NULL, "tzioc");
    if (!node) {
        LOGE("No TZIOC node in device tree");
        err = -ENOENT;
        goto ERR_EXIT;
    }

    /* get shared memory info from device tree */
    smem_params = of_get_address(node, 0, &smemSize64, NULL);
    if (!smem_params) {
        LOGE("Failed to get TZIOC memory info from device tree");
        err = -EINVAL;
        goto ERR_EXIT;
    }

    smemStart64 = of_translate_address(node, smem_params);

    tdev->smemStart = (uint32_t)smemStart64;
    tdev->smemSize  = (uint32_t)smemSize64;
    LOGI("TZIOC shared memory at 0x%x, size 0x%x",
         tdev->smemStart, tdev->smemSize);

    /* map shared memory */
#if IOREMAP_SHARED_MEM
    tdev->psmem = (struct tzioc_shared_mem *)
        ioremap(tdev->smemStart, tdev->smemSize);
#else
    tdev->psmem = (struct tzioc_shared_mem *)
        brcmstb_memory_kva_map_phys(tdev->smemStart, tdev->smemSize, true);
#endif
    if (!tdev->psmem) {
        LOGE("Failed to remap TZIOC memory");
        err = -ENODEV;
        goto ERR_EXIT;
    }

    /* check version */
    version = tdev->psmem->ulMagic;
    if (version != ASTRA_VERSION_WORD) {
        LOGE("Mismatched TZIOC version 0x%08x, expecting 0x%08x",
             version, ASTRA_VERSION_WORD);
        err = -ENODEV;
        goto ERR_EXIT;
    }

    /* get system IRQ info from device tree */
    err = of_property_read_u32(node, "irq", &sysIrq);
    if (err) {
        LOGE("Failed to get TZIOC system IRQ info from device tree");
        err = -EINVAL;
        goto ERR_EXIT;
    }

    tdev->sysIrq = sysIrq;
    LOGI("TZIOC system IRQ %d", tdev->sysIrq);

    /* init work for bottom half */
    INIT_WORK(&tdev->sysIrqWork, tzioc_proc);

#if KERNEL_IPI_PATCH
    /* request system IRQ */
    err = set_ipi_handler(tdev->sysIrq, tzioc_isr, "TZIOC");
    if (err) {
        LOGE("Failed to set TZIOC system IRQ handler");
        tdev->sysIrq = 0;
        goto ERR_EXIT;
    }
#endif

#if TZIOC_DEV_SUPPORT
    tdev->tzcdev = cdev_alloc();

    err = alloc_chrdev_region(&devid, 0, TZIOC_NUM_DEVICES, tzioc_mdev_name);
    if (err < 0) {
        LOGE("Failed to allocate tzioc chrdev\n");
        goto ERR_EXIT;
    }

    tdev->tzioc_major = MAJOR(devid);

    tdev->tzclass = class_create (THIS_MODULE, tzioc_mdev_name);
    if (IS_ERR(tdev->tzclass)) {
        LOGE ("tzioc class creation failed\n");
        goto ERR_EXIT;
    }

    cdev_init (tdev->tzcdev, &tzioc_mdev_fops);

    err = cdev_add (tdev->tzcdev, MKDEV(tdev->tzioc_major, 0), 1);
    if (err) {
        LOGE ("Unable to add tzioc cdev\n");
        goto ERR_EXIT;
    }

    tdev->tzdevice = device_create (tdev->tzclass, NULL, MKDEV(tdev->tzioc_major, 0), NULL, tzioc_mdev_name, 0);

    if (IS_ERR (tdev->tzdevice)) {
        LOGE ("tzioc device creation failed\n");
        goto ERR_EXIT;
    }

#endif

    /* init spinlock */
    spin_lock_init(&tdev->lock);

    /* init ioctl module */
    err = _tzioc_ioctl_module_init();
    if (err) {
        LOGE("Failed to init TZIOC ioctl module");
        goto ERR_EXIT;
    }

    /* init msg module */
    err = _tzioc_msg_module_init();
    if (err) {
        LOGE("Failed to init TZIOC msg module");
        goto ERR_EXIT;
    }

    /* init mem module */
    err = _tzioc_mem_module_init();
    if (err) {
        LOGE("Failed to init TZIOC mem module");
        goto ERR_EXIT;
    }

    /* init client module */
    err = _tzioc_client_module_init();
    if (err) {
        LOGE("Failed to init TZIOC client module");
        goto ERR_EXIT;
    }

    /* open system client */
    tdev->pSysClient = _tzioc_kernel_client_open(
        "tzioc_system",
        NULL, /* sys msgs dispatched directly */
        0); /* private data not used */

    if (!tdev->pSysClient) {
        LOGE("Failed to open TZIOC system client");
        goto ERR_EXIT;
    }

#if 0
    if (tracelog_init()) {
        LOGE("Failed to init tracelog driver");
        goto ERR_EXIT;
    }
#endif

    /* send system up cmd to TZOS */
    hdr.ucType = SYS_MSG_UP;
    hdr.ucOrig = TZIOC_CLIENT_ID_SYS;
    hdr.ucDest = TZIOC_CLIENT_ID_SYS;
    hdr.ucSeq  = 0;
    hdr.ulLen  = 0;

    err = _tzioc_msg_send(
        tdev->pSysClient,
        &hdr, NULL);

    if (err) {
        LOGE("Failed to send system up cmd");
        goto ERR_EXIT;
    }

    LOGI("TZIOC initialized");

    /* init astra module */
    err = astra_module_init();
    if (err) {
        LOGE("Failed to init astra module");
        goto ERR_EXIT;
    }

    /* immediately switch to TZOS */
    _tzioc_call_smc(0x7);
    return 0;

 ERR_EXIT:
    tzioc_module_deinit();
    return err;
}

static void __exit tzioc_module_exit(void)
{
    tzioc_module_deinit();
}

static int tzioc_module_deinit(void)
{
    struct tzioc_msg_hdr hdr;
    int err = 0;

    /* exit astra module */
    if (tdev->adev)
        astra_module_deinit();

    if (tdev->peerUp) {
        /* send system down msg to TZOS */
        hdr.ucType = SYS_MSG_DOWN;
        hdr.ucOrig = TZIOC_CLIENT_ID_SYS;
        hdr.ucDest = TZIOC_CLIENT_ID_SYS;
        hdr.ucSeq  = 0;
        hdr.ulLen  = 0;

        err = _tzioc_msg_send(
            tdev->pSysClient,
            &hdr, NULL);

        if (err) {
            LOGE("Failed to send system down msg to TZOS");
        }

        /* immediately switch to TZOS */
        _tzioc_call_smc(0x7);
    }

#if 0
    /* exit tracelog */
    tracelog_exit();

    /* exit vuart */
    bcm_vuart_exit();
#endif

    /* close system client */
    if (tdev->pSysClient)
        _tzioc_kernel_client_close(tdev->pSysClient);

    /* deinit client module */
    if (tdev->pClientMod)
        _tzioc_client_module_deinit();

    /* deinit mem module */
    if (tdev->pMemMod)
        _tzioc_mem_module_deinit();

    /* deinit msg module */
    if (tdev->pMsgMod)
        _tzioc_msg_module_deinit();

    /* deinit ioctl module */
    if (tdev->pIoctlMod)
        _tzioc_ioctl_module_deinit();

    /* unmap shared memory */
    if (tdev->psmem)
#if IOREMAP_SHARED_MEM
        iounmap(tdev->psmem);
#else
        brcmstb_memory_kva_unmap(tdev->psmem);
#endif

    /* free system IRQ */
#if KERNEL_IPI_PATCH
    if (tdev->sysIrq)
        clear_ipi_handler(tdev->sysIrq);

    /* cancel scheduled work */
    cancel_work_sync(&tdev->sysIrqWork);
#endif

#if TZIOC_DEV_SUPPORT

    if (!IS_ERR(tdev->tzdevice)) 
        device_destroy (tdev->tzclass, MKDEV (tdev->tzioc_major, 0));

    if (!IS_ERR(tdev->tzclass)) 
        class_destroy (tdev->tzclass);

    if (tdev->tzioc_major)
        unregister_chrdev_region (MKDEV(tdev->tzioc_major, 0), TZIOC_NUM_DEVICES);

    if (tdev->tzcdev) {
        cdev_del (tdev->tzcdev);
        tdev->tzcdev = NULL;
    }
#endif

    LOGI("TZIOC uninitialized");
    return err;
}

module_init(tzioc_module_init);
module_exit(tzioc_module_exit);
MODULE_LICENSE("GPL");

/*
 * TZIOC System Functions
 */
#if KERNEL_IPI_PATCH
static void tzioc_isr(void)
{   
    schedule_work(&tdev->sysIrqWork);    
}
#endif

static void tzioc_proc(struct work_struct *work)
{
    LOGD("Received TZIOC system IRQ");

    /* processing incoming msgs */
    while (1) {
        static uint8_t msg[TZIOC_MSG_SIZE_MAX];
        struct tzioc_msg_hdr *pHdr = (struct tzioc_msg_hdr *)msg;
        int err;

        /* get received msg */
        err = _tzioc_msg_receive(
            tdev->pSysClient,
            pHdr,
            TZIOC_MSG_PAYLOAD(pHdr),
            TZIOC_MSG_PAYLOAD_MAX);

        if (err == -ENOMSG)
            break;
        if (err) {
            LOGE("Error receiving msg, err %d", err);
            continue;
        }

        /* dispatch received msg */
        if (pHdr->ucDest == TZIOC_CLIENT_ID_SYS) {
            err = tzioc_sys_msg_proc(pHdr);
        }
#if TZIOC_MSG_ECHO
        else if (pHdr->ucDest == TZIOC_CLIENT_ID_MAX) {
            err = tzioc_echo_msg_proc(pHdr);
        }
#endif
        else {
            struct tzioc_client *pClient;

            pClient = _tzioc_client_find_by_id(pHdr->ucDest);

            if (!pClient) {
                LOGW("Unknown msg dest %d", pHdr->ucDest);
                continue;
            }

            if (pClient->kernel) {
                err = pClient->msgProc(pHdr, pClient->privData);
            }
            else {
                struct task_struct *pTask = (struct task_struct *)pClient->task;
                if (pTask->exit_state)
                    continue;
#if KERNEL_BCM_MQ_PATCH
                err = bcm_mq_timedsend(
                    pTask,
                    (mqd_t)pClient->msgQ,
                    (char *)pHdr,
                    pHdr->ulLen + sizeof(*pHdr),
                    0,
                    NULL);
#endif
            }
        }
    }
}

static int tzioc_sys_msg_proc(struct tzioc_msg_hdr *pHdr)
{
    int err = 0;

    LOGI("TZIOC system msg processing");

    switch (pHdr->ucType) {
    /* system up rpy */
    case SYS_MSG_UP:
        if (pHdr->ucOrig != TZIOC_CLIENT_ID_SYS ||
            pHdr->ulLen != 0) {
            LOGE("Invalid system up cmd received");
            err = -EINVAL;
            break;
        }

        tdev->peerUp = true;
        LOGI("TZIOC peer is up");
        break;

    /* system down rpy */
    case SYS_MSG_DOWN:
        if (pHdr->ucOrig != TZIOC_CLIENT_ID_SYS ||
            pHdr->ulLen != 0) {
            LOGE("Invalid system down cmd received");
            err = -EINVAL;
            break;
        }

        tdev->peerUp = false;
        LOGI("TZIOC peer is down");
        break;

#if 0
    /* system tracelog msgs */
    case SYS_MSG_TRACELOG_ON:
    case SYS_MSG_TRACELOG_OFF:
        if (pHdr->ucOrig != TZIOC_CLIENT_ID_SYS ||
            pHdr->ulLen == 0) {
            LOGE("Invalid tracelog cmd received");
            err = -EINVAL;
            break;
        }

        tracelog_msg_proc(pHdr);
        break;

    /* system vuart msgs */
    case SYS_MSG_VUART_ON:
    case SYS_MSG_VUART_OFF:
        if (pHdr->ucOrig != TZIOC_CLIENT_ID_SYS ||
            pHdr->ulLen == 0) {
            LOGE("Invalid vuart cmd received");
            err = -EINVAL;
            break;
        }

        bcm_vuart_msg_proc(pHdr);
        break;
#endif
    default:
        LOGW("Unknown system msg %d", pHdr->ucType);
        err = -ENOENT;
    }
    return err;
}

#if TZIOC_MSG_ECHO
static int tzioc_echo_msg_proc(struct tzioc_msg_hdr *pHdr)
{
    uint8_t id;
    int err = 0;

    id = pHdr->ucOrig;
    pHdr->ucOrig = pHdr->ucDest;
    pHdr->ucDest = id;

    err = _tzioc_msg_send(
        tdev->pSysClient,
        pHdr,
        TZIOC_MSG_PAYLOAD(pHdr));

    if (err) {
        LOGE("Failed to send echo msg");
    }
    return err;
}
#endif

#if defined(CONFIG_BCM963138)
/*
   This is only on A9mp+GCI1.0 based SoCs, like 63138/63148. On this SoCs,
   the Secure core will get the SGIs/PPIs or SPIs, even though they are
   configured as NS interrupts.
   To circumvent that, we do disable IRQs before the SMC call, and re-enable them after.
*/
#endif

int _tzioc_call_smc(uint32_t callnum)
{
    callnum &= 0x7;
#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148)
    asm volatile("cpsid i" : : : );
#endif
    asm volatile(
        "mov r0, %[cn] \r\n"
        __SMC(0)
        : /* No output registers */
        : [cn] "r" (callnum)
        : "r0" /* r0 is clobbered. */
    );
#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148)
    asm volatile("cpsie i" : : : );
#endif
       
    return 0;
}

uint32_t _tzioc_offset2addr(uint32_t ulOffset)
{
    if (ulOffset < tdev->smemSize)
        return ulOffset + (uint32_t)tdev->psmem;
    else
        return (uint32_t)-1;
}

uint32_t _tzioc_addr2offset(uint32_t ulAddr)
{
    if (ulAddr >= (uint32_t)tdev->psmem &&
        ulAddr <  (uint32_t)tdev->psmem + tdev->smemSize)
        return ulAddr - (uint32_t)tdev->psmem;
    else
        return (uint32_t)-1;
}

uint32_t _tzioc_vaddr2paddr(uint32_t ulVaddr)
{
    uint32_t ulOffset = _tzioc_addr2offset(ulVaddr);
    if (ulOffset != -1)
        return tdev->smemStart + ulOffset;
    else
        return (uint32_t)-1;
}

uint32_t _tzioc_paddr2vaddr(uint32_t ulPaddr)
{
    uint32_t ulOffset = ulPaddr - tdev->smemStart;
    return _tzioc_offset2addr(ulOffset);
}

int _tzioc_config_get(struct tzioc_config *pConfig)
{
    pConfig->smemSize = tdev->smemSize;
    pConfig->pmemSize = tdev->pmemSize;
    return 0;
}

int _tzioc_status_get(struct tzioc_status *pStatus)
{
    pStatus->up = tdev->peerUp;
    return 0;
}
