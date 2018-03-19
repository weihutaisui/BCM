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
#include <linux/version.h>
#include <linux/kconfig.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <asm/page.h>

#include "astra_api.h"
#include "astra_test_msg.h"
#include "astra_tdrv.h"

/*
 * Compiler Switches
 */

#define MEM_PROTECT_TEST        0

/*
 * Constant Definitions
 */

#define CLIENT_TEST_REP         16
#define FILE_TEST_REP           2
#define UAPP_HELLO_TEST_REP     2
#define UAPP_TAPP_TEST_REP      2
#define PEER_TAPP_TEST_REP      2
#define MSG_HELLO_TEST_REP      3
#define MEM_ALLOC_TEST_REP      4
#define MEM_PROTECT_TEST_REP    16

#define FILE_BUFF_SIZE          8192
#define MEM_ALLOC_BUFF_SIZE     1024
#define MEM_PROTECT_TEST_STEP   0x10

/*
 * Function Declarations
 */

static int __init astra_tdrv_init(void);
static void __exit astra_tdrv_exit(void);
static int astra_tdrv_deinit(void);

static int astra_tdrv_info(void);
static int astra_tdrv_client(void);
static int astra_tdrv_file(void);
static int astra_tdrv_uapp_hello(void);
static int astra_tdrv_uapp_tapp(void);
static int astra_tdrv_peer_tapp(void);
static int astra_tdrv_msg_hello(void);
static int astra_tdrv_mem_alloc(void);
#if MEM_PROTECT_TEST
static int astra_tdrv_mem_protect(void);
#endif

static void astra_tdrv_msg_proc(
    struct work_struct *work);

static void astra_tdrv_callback(
    astra_event event,
    void *pEventData,
    void *pPrivData);

/*
 * Variable Declarations
 */

static char astra_tdrv_name[16]="astra_tdrv";
module_param_string(tdrvname, astra_tdrv_name, sizeof(astra_tdrv_name), 0);

static struct astra_tdrv astra_tdrv;
static struct astra_tdrv *ptdrv = &astra_tdrv;

/*
 * Astra Test Driver Functions
 */

static int __init astra_tdrv_init(void)
{
    int err = 0;

    /* init test driver control block */
    memset(&astra_tdrv, 0, sizeof(astra_tdrv));

    /* init msg work, lock */
    INIT_WORK(&ptdrv->msgWork, astra_tdrv_msg_proc);
    spin_lock_init(&ptdrv->msgLock);

    LOGI("Astra test driver initialized\n");

    LOGI("Astra testing starts...\n");

    /* info test */
    err = astra_tdrv_info();
    if (err) goto ERR_EXIT;

    /* client test */
    err = astra_tdrv_client();
    if (err) goto ERR_EXIT;

    /* file test */
    err = astra_tdrv_file();
    if (err) goto ERR_EXIT;

    /* uapp test - hello */
    err = astra_tdrv_uapp_hello();
    if (err) goto ERR_EXIT;

    /* uapp test - tapp */
    err = astra_tdrv_uapp_tapp();
    if (err) goto ERR_EXIT;

    /* peer test - tapp */
    err = astra_tdrv_peer_tapp();
    if (err) goto ERR_EXIT;

    /* msg test - hello */
    err = astra_tdrv_msg_hello();
    if (err) goto ERR_EXIT;

    /* mem test - alloc */
    err = astra_tdrv_mem_alloc();
    if (err) goto ERR_EXIT;

#if MEM_PROTECT_TEST
    /* mem test - protect */
    err = astra_tdrv_mem_protect();
    if (err) goto ERR_EXIT;
#endif

    LOGI("Astra testing done!\n");

 ERR_EXIT:
    astra_tdrv_deinit();
    return err;
}

static void __exit astra_tdrv_exit(void)
{
    astra_tdrv_deinit();
}

static int astra_tdrv_deinit(void)
{
    int err = 0;

    if (ptdrv->hPeer) {
        /* close test peer */
        astra_peer_close(ptdrv->hPeer);

        /* clear test peer handle */
        ptdrv->hPeer = 0;
    }

    if (ptdrv->hUapp) {
        /* close test uapp */
        astra_uapp_close(ptdrv->hUapp);

        /* clear test uapp handle */
        ptdrv->hUapp = 0;
    }

    if (ptdrv->hClient) {
        /* close test client */
        astra_client_close(ptdrv->hClient);

        /* clear test client handle */
        ptdrv->hClient = 0;
    }

    /* cancel scheduled work */
    cancel_work_sync(&ptdrv->msgWork);

    LOGI("Astra test driver uninitialized\n");
    return err;
}

module_init(astra_tdrv_init);
module_exit(astra_tdrv_exit);
MODULE_LICENSE("GPL");

/*
 * Astra Test Functions
 */

static int astra_tdrv_info(void)
{
    /* get astra version */
    astra_version_get(&ptdrv->version);

    LOGI("Astra version: %d.%d.%d",
         ptdrv->version.major,
         ptdrv->version.minor,
         ptdrv->version.build);

    /* get astra config */
    astra_config_get(&ptdrv->config);

    LOGI("Astra shared  mem size: 0x%x", ptdrv->config.smemSize);
    LOGI("Astra private mem size: 0x%x", ptdrv->config.pmemSize);

    /* get astra status */
    astra_status_get(&ptdrv->status);

    LOGI("Astra is %s\n", (ptdrv->status.up) ? "up" : "down");
    return 0;
}

static int astra_tdrv_client(void)
{
    int rep;

    /* open/close test client repeatedly */
    for (rep = 0; rep < CLIENT_TEST_REP; rep++) {

        LOGI("client testing %d...", rep);

        /* open test client */
        ptdrv->hClient = astra_client_open(
            astra_tdrv_name,
            astra_tdrv_callback,
            0);  /* private data not used */

        if (!ptdrv->hClient) {
            LOGE("failed to open client");
            return -EFAULT;
        }

        LOGD("client handle %p", ptdrv->hClient);

        /* leave last test client */
        if (rep == CLIENT_TEST_REP - 1) {
            LOGI("client testing %d leave open\n", rep);
            break;
        }

        /* close test client */
        astra_client_close(ptdrv->hClient);

        /* clear test client handle */
        ptdrv->hClient = 0;

        LOGI("client testing %d done\n", rep);
    }

    LOGI("client testing done!\n");
    return 0;
}

static int astra_tdrv_file(void)
{
    int rep;

    /* open/close/write test file test_file.[n] */
    for (rep = 0; rep < FILE_TEST_REP; rep++) {
        char path[32];
        uint8_t *vaddr;
        astra_paddr_t paddr;
        uint32_t wchecksum, rchecksum;
        size_t wbytes, rbytes;
        size_t i;

        LOGI("file testing %d...", rep);

        /* prepare file path */
        sprintf(path, "test_file.%d", rep);

        /* open file to write */
        ptdrv->hFile = astra_file_open(
            ptdrv->hClient,
            path,
            O_CREAT | O_WRONLY);

        if (!ptdrv->hFile) {
            LOGE("failed to open file to write");
            return -EFAULT;
        }

        LOGD("file write handle %p", ptdrv->hFile);

        /* alloc file buffer */
        vaddr = kmalloc(FILE_BUFF_SIZE, GFP_KERNEL);
        paddr = (astra_paddr_t)__pa(vaddr);

        /* fill in file buffer */
        if (rep == 0) {
            /* fill in sequential data */
            for (i = 0; i < FILE_BUFF_SIZE; i++) {
                *(vaddr + i) = i & 0xFF;
            }
        }
        else {
            /* fill in random data */
            get_random_bytes(vaddr, FILE_BUFF_SIZE);
        }

        /* calculate simple checksum */
        wchecksum = 0;
        for (i = 0; i < FILE_BUFF_SIZE; i += 4) {
            wchecksum += *(uint32_t *)(vaddr + i);
        }

        LOGD("file write: paddr 0x%x size 0x%x checksum 0x%x",
             (unsigned int)__pa(vaddr), FILE_BUFF_SIZE, wchecksum);

        /* write to file */
        wbytes = astra_file_write(
            ptdrv->hFile,
            paddr,
            FILE_BUFF_SIZE);

        if (wbytes != FILE_BUFF_SIZE) {
            LOGE("failed to write to file %d", wbytes);
            return -EIO;
        }

        /* close file */
        astra_file_close(ptdrv->hFile);

        /* open file to read */
        ptdrv->hFile = astra_file_open(
            ptdrv->hClient,
            path,
            O_RDONLY);

        if (!ptdrv->hFile) {
            LOGE("failed to open file to read");
            return -EFAULT;
        }

        LOGD("file read handle %p", ptdrv->hFile);

        /* clear file buffer */
        memset(vaddr, 0, FILE_BUFF_SIZE);

        /* read from file */
        rbytes = astra_file_read(
            ptdrv->hFile,
            paddr,
            FILE_BUFF_SIZE);

        if (rbytes != FILE_BUFF_SIZE) {
            LOGE("failed to read from file");
            return -EIO;
        }

        /* calculate simple checksum */
        rchecksum = 0;
        for (i = 0; i < FILE_BUFF_SIZE; i += 4) {
            rchecksum += *(uint32_t *)(vaddr + i);
        }

        LOGI("file read: paddr 0x%x size 0x%x checksum 0x%x",
             (unsigned int)__pa(vaddr), FILE_BUFF_SIZE, rchecksum);

        /* free file buffer */
        kfree(vaddr);

        /* close file */
        astra_file_close(ptdrv->hFile);

        /* clear file handle */
        ptdrv->hFile = 0;

        if (rchecksum != wchecksum) {
            LOGE("mismatched file checksums");
            return -EIO;
        }

        LOGI("file testing %d done\n", rep);
    }

    LOGI("file testing done!\n");
    return 0;
}

static int astra_tdrv_uapp_hello(void)
{
    int rep;

    /* open/close test uapp hello repeatedly */
    for (rep = 0; rep < UAPP_HELLO_TEST_REP; rep++) {

        LOGI("uapp hello testing %d...", rep);

        /* open test uapp hello */
        ptdrv->hUapp = astra_uapp_open(
            ptdrv->hClient,
            "hello",
            "hello.elf");

        if (!ptdrv->hUapp) {
            LOGE("failed to open uapp hello");
            return -EFAULT;
        }

        LOGI("uapp hello handle %p", ptdrv->hUapp);

        /* close test uapp hello */
        astra_uapp_close(ptdrv->hUapp);

        /* clear test client handle */
        ptdrv->hUapp = 0;

        LOGI("uapp hello testing %d done\n", rep);
    }

    LOGI("uapp hello testing done!\n");
    return 0;
}

static int astra_tdrv_uapp_tapp(void)
{
    int rep;

    /* open/close test uapp tapp repeatedly */
    for (rep = 0; rep < UAPP_TAPP_TEST_REP; rep++) {

        LOGI("uapp tapp testing %d...", rep);

        /* open test uapp tapp */
        ptdrv->hUapp = astra_uapp_open(
            ptdrv->hClient,
            "astra_tapp",
            "astra_tapp.elf");

        if (!ptdrv->hUapp) {
            LOGE("failed to open uapp tapp");
            return -EFAULT;
        }

        LOGI("uapp tapp handle %p", ptdrv->hUapp);

        /* leave last test uapp tapp */
        if (rep == UAPP_TAPP_TEST_REP - 1) {
            LOGI("uapp tapp testing %d leave open\n", rep);
            break;
        }

        /* close test uapp tapp */
        astra_uapp_close(ptdrv->hUapp);

        /* clear test client handle */
        ptdrv->hUapp = 0;

        LOGI("uapp tapp testing %d done\n", rep);
    }

    LOGI("uapp tapp testing done!\n");
    return 0;
}

static int astra_tdrv_peer_tapp(void)
{
    int rep;

    /* open/close test peer repeatedly */
    for (rep = 0; rep < PEER_TAPP_TEST_REP; rep++) {

        LOGI("peer testing %d...", rep);

        /* open test peer */
        ptdrv->hPeer = astra_peer_open(
            ptdrv->hUapp,
            "astra_tapp");

        if (!ptdrv->hPeer) {
            LOGE("failed to open peer");
            return -EFAULT;
        }

        LOGI("peer handle %p", ptdrv->hPeer);

        /* leave last test peer */
        if (rep == PEER_TAPP_TEST_REP - 1) {
            LOGI("peer testing %d leave open\n", rep);
            break;
        }

        /* close test peer */
        astra_peer_close(ptdrv->hPeer);

        /* clear test client handle */
        ptdrv->hPeer = 0;

        LOGI("peer testing %d done\n", rep);
    }

    LOGI("peer testing done!\n");
    return 0;
}

static int astra_tdrv_msg_hello(void)
{
    int timeout;
    int rep;
    int err = 0;

    /* send hello cmd to peer */
    for (rep = 0; rep < MSG_HELLO_TEST_REP; rep++) {
        static uint8_t msg[sizeof(struct astra_test_msg_hdr) +
                           sizeof(struct astra_test_msg_hello_cmd)];
        struct astra_test_msg_hdr *pHdr =
            (struct astra_test_msg_hdr *)msg;
        struct astra_test_msg_hello_cmd *pCmd =
            (struct astra_test_msg_hello_cmd *)ASTRA_TEST_MSG_PAYLOAD(pHdr);

        LOGI("msg hello testing %d...", rep);

        pHdr->ucType = ASTRA_TEST_MSG_HELLO;
        pHdr->ucSeq  = 0;

        strcpy(pCmd->greet, "Hello from Linux astra test driver.");

        err = astra_msg_send(
            ptdrv->hPeer,
            (void *)&msg,
            sizeof(msg));

        if (err) {
            LOGE("failed to send hello msg");
            return err;
        }

        spin_lock(&ptdrv->msgLock);
        ptdrv->msgCnt++;
        spin_unlock(&ptdrv->msgLock);

        LOGI("msg hello testing %d done\n", rep);
    }

    /* switch to TZOS */
    astra_call_smc(ptdrv->hClient, 0x7);

    /* wait for rpy or timeout */
    timeout = 10;
    while (ptdrv->msgCnt > 0 && timeout--)
        msleep(1000);

    if (timeout == -1) {
        LOGE("timedout waiting for rpy");
        return -ETIMEDOUT;
    }

    LOGI("msg hello testing done!\n");
    return 0;
}

static int astra_tdrv_mem_alloc(void)
{
    int timeout;
    int rep;
    int err = 0;

    for (rep = 0; rep < MEM_ALLOC_TEST_REP; rep++) {
        uint8_t *vaddr;
        uint32_t offset;
        uint32_t buffsize;
        uint32_t checksum;
        size_t i;

        LOGI("mem alloc testing %d...", rep);

        buffsize = MEM_ALLOC_BUFF_SIZE << rep;

        /* allocate buffer from shared memory */
        vaddr = astra_mem_alloc(
            ptdrv->hClient,
            buffsize);

        if (!vaddr) {
            LOGE("failed to alloc shared memory buffer");
            return -ENOMEM;
        }

        /* convert virtual address to offset */
        offset = astra_vaddr2offset(
            ptdrv->hClient,
            vaddr);

        if (offset == 0) {
            LOGE("failed to convert virtual address to offset");
            return -EFAULT;
        }

        /* fill in random data */
        get_random_bytes(vaddr, buffsize);

        /* calculate simple checksum */
        checksum = 0;
        for (i = 0; i < buffsize; i += 4) {
            checksum += *(uint32_t *)(vaddr + i);
        }

        LOGD("mem alloc local: offset 0x%x size 0x%x checksum 0x%x",
             offset, buffsize, checksum);

        /* send mem alloc cmd to peer */
        {
            static uint8_t msg[sizeof(struct astra_test_msg_hdr) +
                               sizeof(struct astra_test_msg_mem_alloc_cmd)];
            struct astra_test_msg_hdr *pHdr =
                (struct astra_test_msg_hdr *)msg;
            struct astra_test_msg_mem_alloc_cmd *pCmd =
                (struct astra_test_msg_mem_alloc_cmd *)ASTRA_TEST_MSG_PAYLOAD(pHdr);

            pHdr->ucType = ASTRA_TEST_MSG_MEM_ALLOC;
            pHdr->ucSeq  = 0;

            pCmd->offset = offset;
            pCmd->size = buffsize;

            err = astra_msg_send(
                ptdrv->hPeer,
                (void *)&msg,
                sizeof(msg));

            if (err) {
                LOGE("failed to send mem alloc msg");
                return err;
            }

            spin_lock(&ptdrv->msgLock);
            ptdrv->msgCnt++;
            spin_unlock(&ptdrv->msgLock);

            /* switch to TZOS */
            astra_call_smc(ptdrv->hClient, 0x7);

            /* wait for rpy or timeout */
            timeout = 10;
            while (ptdrv->msgCnt > 0 && timeout--)
                msleep(1000);

            if (timeout == -1) {
                LOGE("timedout waiting for rpy");
                return -ETIMEDOUT;
            }
        }

        /* free shared memory buffer */
        astra_mem_free(
            ptdrv->hClient,
            vaddr);

        /* compare checksum */
        if (ptdrv->checksum != checksum) {
            LOGE("mismatched checksum from peer");
            return -EFAULT;
        }

        LOGI("mem alloc testing %d done\n", rep);
    }

    LOGI("mem alloc testing done!\n");
    return 0;
}

#if MEM_PROTECT_TEST
static int astra_tdrv_mem_protect(void)
{
    uint8_t *vaddrSmem, *vaddrProt;
    uint32_t paddrSmem, paddrProt;
    struct page *pageProt;
    int rep;
    uint32_t offset, data;

    /* get shared mem virtual address */
    vaddrSmem = astra_offset2vaddr(
        ptdrv->hClient,
        0 /* offset */);

    /* get shared mem physical address */
    paddrSmem = vmalloc_to_pfn(vaddrSmem) << PAGE_SHIFT;

    /* protected mem is just beyond shared mem */
    paddrProt = paddrSmem + ptdrv->config.smemSize;

    /* map protected mem temporarily */
    pageProt = pfn_to_page(PFN_DOWN(paddrProt));
    vaddrProt = kmap_atomic(pageProt);

    offset = 0;
    for (rep = 0; rep < MEM_PROTECT_TEST_REP; rep++) {
        data = *(uint32_t *)(vaddrProt + offset);
        offset += MEM_PROTECT_TEST_STEP;

        /* violations actually cause core dump !!! */
        if (data) {
            /* unmap protected mem */
            kunmap_atomic(vaddrProt);

            LOGE("mem unprotected: 0x%08x @ 0x%08x",
                 (unsigned int)data,
                 (unsigned int)paddrProt + offset);

            return -EFAULT;
        }
    }

    /* unmap protected mem */
    kunmap_atomic(vaddrProt);

    LOGI("mem protection testing done!\n");
    return 0;
}
#endif

static void astra_tdrv_msg_proc(
    struct work_struct *work)
{
    static uint8_t msg[1024];
    size_t msgLen = 1024;
    astra_peer_handle hPeer;
    int err = 0;

    while (1) {
        struct astra_test_msg_hdr *pHdr =
            (struct astra_test_msg_hdr *)msg;

        err = astra_msg_receive(
            ptdrv->hClient,
            &hPeer,
            &msg,
            &msgLen,
            ASTRA_WAIT_NONE);

        if (err) {
            if (err != -ENOMSG) continue;
            return;
        }

        spin_lock(&ptdrv->msgLock);
        ptdrv->msgCnt--;
        spin_unlock(&ptdrv->msgLock);

        switch (pHdr->ucType) {
        case ASTRA_TEST_MSG_HELLO:
            {
                struct astra_test_msg_hello_rpy *pRpy =
                    (struct astra_test_msg_hello_rpy *)ASTRA_TEST_MSG_PAYLOAD(pHdr);

                if (msgLen != sizeof(*pHdr) + sizeof(*pRpy)) {
                    LOGE("invalid hello rpy received");
                    return;
                }

                LOGD("hello rpy: %s", pRpy->reply);
            }
            break;

        case ASTRA_TEST_MSG_MEM_ALLOC:
            {
                struct astra_test_msg_mem_alloc_rpy *pRpy =
                    (struct astra_test_msg_mem_alloc_rpy *)ASTRA_TEST_MSG_PAYLOAD(pHdr);

                if (msgLen != sizeof(*pHdr) + sizeof(*pRpy)) {
                    LOGE("invalid mem alloc rpy received");
                    return;
                }

                LOGD("mem alloc rpy: checksum 0x%x", pRpy->checksum);

                /* remember checksum */
                ptdrv->checksum = pRpy->checksum;
            }
            break;

        default:
            LOGE("unknown peer msg %d", pHdr->ucType);
            return;
        }
    }
}

static void astra_tdrv_callback(
    astra_event event,
    void *pEventData,
    void *pPrivData)
{
    switch (event) {
    case ASTRA_EVENT_MSG_RECEIVED:
        LOGD("client event callback, msg received");
        schedule_work(&ptdrv->msgWork);
        break;

    case ASTRA_EVENT_UAPP_EXIT:
        LOGD("client event callback, userapp exit");
        break;

    default:
        LOGE("client event callback, unknown event %d", event);
    }
}
