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
#include <linux/dma-mapping.h>

#include "tzioc_api.h"
#include "uappd_msg.h"
#include "tzioc_test_msg.h"
#include "tzioc_tdrv.h"

/*
 * Compiler Switches
 */

#define MAP_PADDR_DEVICE        0
#define MAP_PADDRS_DEVICE       0

/*
 * Constant Definitions
 */

#define CLIENT_TEST_REP         16
#define PEER_HELLO_TEST_REP     2
#define PEER_TAPP_TEST_REP      1
#define MSG_ECHO_TEST_REP       16
#define MSG_HELLO_TEST_REP      3
#define MEM_ALLOC_TEST_REP      4
#define MAP_PADDR_TEST_REP      2
#define MAP_PADDRS_TEST_REP     2

#define MEM_ALLOC_BUFF_SIZE     (1024)
#define MAP_PADDR_BUFF_SIZE     (16 * 1024)
#define MAP_PADDRS_BUFF_COUNT   16
#define MAP_PADDRS_BUFF_SIZE    (16 * 1024)

/*
 * Function Declarations
 */

static int __init tzioc_tdrv_init(void);
static void __exit tzioc_tdrv_exit(void);

static int tzioc_tdrv_deinit(void);

static int tzioc_tdrv_client(void);
static int tzioc_tdrv_peer_hello(void);
static int tzioc_tdrv_peer_tapp(void);
static int tzioc_tdrv_msg_echo(void);
static int tzioc_tdrv_msg_hello(void);
static int tzioc_tdrv_mem_alloc(void);
static int tzioc_tdrv_map_paddr(void);
static int tzioc_tdrv_map_paddrs(void);

static int tzioc_tdrv_msg_proc(
    tzioc_msg_hdr *pHdr,
    uint32_t ulPrivData);

/*
 * Variable Declarations
 */

static char tzioc_tdrv_name[16]="tzioc_tdrv";
module_param_string(tdrvname, tzioc_tdrv_name, sizeof(tzioc_tdrv_name), 0);

static struct tzioc_tdrv tzioc_tdrv;
static struct tzioc_tdrv *ptdrv = &tzioc_tdrv;

/*
 * TZIOC Test Driver Functions
 */

static int __init tzioc_tdrv_init(void)
{
    int err = 0;

    /* init test driver control block */
    memset(&tzioc_tdrv, 0, sizeof(tzioc_tdrv));

    /* init msg proc func */
    ptdrv->pMsgProc = tzioc_tdrv_msg_proc;

    /* init msg lock */
    spin_lock_init(&ptdrv->msgLock);

    LOGI("TZIOC test driver initialized\n");

    LOGI("TZIOC testing starts...\n");

    /* client test */
    err = tzioc_tdrv_client();
    if (err) goto ERR_EXIT;

    /* msg test - echo */
    err = tzioc_tdrv_msg_echo();
    if (err) goto ERR_EXIT;

    /* peer test - hello */
    err = tzioc_tdrv_peer_hello();
    if (err) goto ERR_EXIT;

    /* peer test - test app */
    err = tzioc_tdrv_peer_tapp();
    if (err) goto ERR_EXIT;

    /* msg test - hello */
    err = tzioc_tdrv_msg_hello();
    if (err) goto ERR_EXIT;

    /* mem test - alloc*/
    err = tzioc_tdrv_mem_alloc();
    if (err) goto ERR_EXIT;

    /* map test - single paddr */
    err = tzioc_tdrv_map_paddr();
    if (err) goto ERR_EXIT;

    /* map test - multiple paddrs */
    err = tzioc_tdrv_map_paddrs();
    if (err) goto ERR_EXIT;

    LOGI("TZIOC testing done!\n");

 ERR_EXIT:
    tzioc_tdrv_deinit();
    return err;
}

static void __exit tzioc_tdrv_exit(void)
{
    tzioc_tdrv_deinit();
}

static int tzioc_tdrv_deinit(void)
{
    int err = 0;

    if (ptdrv->peerId) {
        /* stop peer tzioc_tapp */
        tzioc_peer_stop(
            ptdrv->hClient,
            "tzioc_tapp");

        /* clear id for peer tzioc_tapp */
        ptdrv->peerId = 0;
    }

    if (ptdrv->hClient) {
        /* close test client */
        tzioc_client_close(ptdrv->hClient);

        /* clear test client handle */
        ptdrv->hClient = 0;
    }

    LOGI("TZIOC test driver uninitialized\n");
    return err;
}

module_init(tzioc_tdrv_init);
module_exit(tzioc_tdrv_exit);
MODULE_LICENSE("GPL");

/*
 * TZIOC Test Functions
 */

static int tzioc_tdrv_client(void)
{
    int rep;

    /* open/close test client repeatedly */
    for (rep = 0; rep < CLIENT_TEST_REP; rep++) {

        LOGI("client testing %d...", rep);

        /* open test client */
        ptdrv->hClient = tzioc_client_open(
            tzioc_tdrv_name,
            ptdrv->pMsgProc,
            0, /* private data not used */
            &ptdrv->clientId);

        if (!ptdrv->hClient) {
            LOGE("failed to open client");
            return -EFAULT;
        }

        LOGI("client handle 0x%x, client id %d",
             ptdrv->hClient,
             ptdrv->clientId);

        /* leave last test client */
        if (rep == CLIENT_TEST_REP - 1) break;

        /* close test client */
        tzioc_client_close(ptdrv->hClient);

        /* clear test client handle */
        ptdrv->hClient = 0;

        LOGI("client testing %d done\n", rep);
    }

    LOGI("client testing done!\n");
    return 0;
}

static int tzioc_tdrv_peer_hello(void)
{
    int timeout;
    int rep;
    int err = 0;

    for (rep = 0; rep < PEER_HELLO_TEST_REP; rep++) {

        LOGI("peer hello app testing %d...", rep);

        /* start peer hello */
        err = tzioc_peer_start(
            ptdrv->hClient,
            "hello",
            "hello.elf",
            false);

        if (err) {
            LOGE("failed to start peer");
            return err;
        }

        spin_lock(&ptdrv->msgLock);
        ptdrv->msgCnt++;
        spin_unlock(&ptdrv->msgLock);

        /* wait for rpy */
        timeout = 10;
        while (ptdrv->msgCnt > 0 && timeout--)
            msleep(1000);

        if (timeout == -1) {
            LOGE("timedout waiting for rpy");
            return -ETIMEDOUT;
        }

        /* stop peer hello */
        err = tzioc_peer_stop(
            ptdrv->hClient,
            "hello");

        if (err) {
            LOGE("failed to stop peer");
            return err;
        }

        spin_lock(&ptdrv->msgLock);
        ptdrv->msgCnt++;
        spin_unlock(&ptdrv->msgLock);

        /* wait for rpy */
        timeout = 10;
        while (ptdrv->msgCnt > 0 && timeout--)
            msleep(1000);

        if (timeout == -1) {
            LOGE("timedout waiting for rpy");
            return -ETIMEDOUT;
        }

        spin_lock(&ptdrv->msgLock);
        ptdrv->msgCnt++;
        spin_unlock(&ptdrv->msgLock);

        /* wait for exit nfy for peer hello */
        timeout = 10;
        while (ptdrv->msgCnt > 0 && timeout--)
            msleep(1000);

        if (timeout == -1) {
            LOGE("timedout waiting for rpy");
            return -ETIMEDOUT;
        }

        LOGI("peer hello app testing %d done\n", rep);
    }

    LOGI("peer hello app testing done!\n");
    return 0;
}

static int tzioc_tdrv_peer_tapp(void)
{
    int timeout;
    int rep;
    int err = 0;

    for (rep = 0; rep < PEER_TAPP_TEST_REP; rep++) {

        LOGI("peer test app testing %d...", rep);

        /* start peer tzioc_tapp */
        err = tzioc_peer_start(
            ptdrv->hClient,
            "tzioc_tapp",
            "tzioc_tapp.elf",
            false);

        if (err) {
            LOGE("failed to start peer");
            return err;
        }

        spin_lock(&ptdrv->msgLock);
        ptdrv->msgCnt++;
        spin_unlock(&ptdrv->msgLock);

        /* wait for rpy */
        timeout = 10;
        while (ptdrv->msgCnt > 0 && timeout--)
            msleep(1000);

        if (timeout == -1) {
            LOGE("timedout waiting for rpy");
            return -ETIMEDOUT;
        }

        /* get id for peer tzioc_tapp */
        err = tzioc_peer_getid(
            ptdrv->hClient,
            "tzioc_tapp");

        if (err) {
            LOGE("failed to get peer id");
            return err;
        }

        spin_lock(&ptdrv->msgLock);
        ptdrv->msgCnt++;
        spin_unlock(&ptdrv->msgLock);

        /* wait for rpy */
        timeout = 10;
        while (ptdrv->msgCnt > 0 && timeout--)
            msleep(1000);

        if (timeout == -1) {
            LOGE("timedout waiting for rpy");
            return -ETIMEDOUT;
        }

        /* leave last peer active */
        if (rep == PEER_TAPP_TEST_REP - 1) break;

        /* stop peer tzioc_tapp */
        err = tzioc_peer_stop(
            ptdrv->hClient,
            "tzioc_tapp");

        if (err) {
            LOGE("failed to stop peer");
            return err;
        }

        spin_lock(&ptdrv->msgLock);
        ptdrv->msgCnt++;
        spin_unlock(&ptdrv->msgLock);

        /* clear id for peer tzioc_tapp */
        ptdrv->peerId = 0;

        /* wait for rpy */
        timeout = 10;
        while (ptdrv->msgCnt > 0 && timeout--)
            msleep(1000);

        if (timeout == -1) {
            LOGE("timedout waiting for rpy");
            return -ETIMEDOUT;
        }

        LOGI("peer test app testing %d done\n", rep);
    }

    LOGI("peer test app testing done!\n");
    return 0;
}

static int tzioc_tdrv_msg_echo(void)
{
    int timeout;
    int rep;
    int err = 0;

    /* send echo msg repeatedly */
    for (rep = 0; rep < MSG_ECHO_TEST_REP; rep++) {
        static uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                           sizeof(struct tzioc_test_msg_echo)];
        struct tzioc_msg_hdr *pHdr =
            (struct tzioc_msg_hdr *)msg;
        struct tzioc_test_msg_echo *pEcho =
            (struct tzioc_test_msg_echo *)TZIOC_MSG_PAYLOAD(pHdr);

        LOGI("msg echo testing %d...", rep);

        /* send echo msg to echo client */
        pHdr->ucType = TZIOC_TEST_MSG_ECHO;
        pHdr->ucOrig = ptdrv->clientId;
        pHdr->ucDest = TZIOC_CLIENT_ID_ECHO;
        pHdr->ucSeq  = 0;
        pHdr->ulLen  = sizeof(*pEcho);

        pEcho->value = rep;

        err = tzioc_msg_send(
            ptdrv->hClient,
            pHdr);

        if (err) {
            LOGE("failed to send echo msg");
            return err;
        }

        spin_lock(&ptdrv->msgLock);
        ptdrv->msgCnt++;
        spin_unlock(&ptdrv->msgLock);

        LOGI("msg echo testing %d done\n", rep);
    }

    /* switch to TZOS */
    tzioc_call_smc(ptdrv->hClient, 0x7);

    /* wait for echo msgs or timeout */
    timeout = 10;
    while (ptdrv->msgCnt > 0 && timeout--)
        msleep(1000);

    if (timeout == -1) {
        LOGE("timedout waiting for rpy");
        return -ETIMEDOUT;
    }

    LOGI("msg echo testing done!\n");
    return 0;
}

static int tzioc_tdrv_msg_hello(void)
{
    int timeout;
    int rep;
    int err = 0;

    /* send hello cmd to peer */
    for (rep = 0; rep < MSG_HELLO_TEST_REP; rep++) {
        static uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                           sizeof(struct tzioc_test_msg_hello_cmd)];
        struct tzioc_msg_hdr *pHdr =
            (struct tzioc_msg_hdr *)msg;
        struct tzioc_test_msg_hello_cmd *pCmd =
            (struct tzioc_test_msg_hello_cmd *)TZIOC_MSG_PAYLOAD(pHdr);

        LOGI("msg hello testing %d...", rep);

        pHdr->ucType = TZIOC_TEST_MSG_HELLO;
        pHdr->ucOrig = ptdrv->clientId;
        pHdr->ucDest = ptdrv->peerId;
        pHdr->ucSeq  = 0;
        pHdr->ulLen  = sizeof(*pCmd);

        strcpy(pCmd->greet, "Hello from Linux TZIOC test driver.");

        err = tzioc_msg_send(
            ptdrv->hClient,
            pHdr);

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
    tzioc_call_smc(ptdrv->hClient, 0x7);

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

static int tzioc_tdrv_mem_alloc(void)
{
    int timeout;
    int rep;
    int err = 0;

    for (rep = 0; rep < MEM_ALLOC_TEST_REP; rep++) {
        uint8_t *vaddr;
        uint32_t offset;
        uint32_t buffsize;
        uint32_t checksum = 0;
        size_t i;

        LOGI("mem alloc testing %d...", rep);

        buffsize = MEM_ALLOC_BUFF_SIZE << rep;

        /* allocate buffer from shared memory */
        vaddr = tzioc_mem_alloc(
            ptdrv->hClient,
            buffsize);

        if (!vaddr) {
            LOGE("failed to alloc shared memory buffer");
            return -ENOMEM;
        }

        /* convert virtual address to offset */
        offset = tzioc_vaddr2offset(
            ptdrv->hClient,
            (uint32_t)vaddr);

        if (offset == (uint32_t)-1) {
            LOGE("failed to convert virtual address to offset");
            return -EFAULT;
        }

        /* fill in random data */
        get_random_bytes(vaddr, buffsize);

        /* calculate simple checksum */
        for (i = 0; i < buffsize; i += 4) {
            checksum += *(uint32_t *)(vaddr + i);
        }

        LOGD("mem alloc local: offset 0x%x size 0x%x checksum 0x%x",
             offset, buffsize, checksum);

        /* send mem alloc cmd to peer */
        {
            static uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                               sizeof(struct tzioc_test_msg_mem_alloc_cmd)];
            struct tzioc_msg_hdr *pHdr =
                (struct tzioc_msg_hdr *)msg;
            struct tzioc_test_msg_mem_alloc_cmd *pCmd =
                (struct tzioc_test_msg_mem_alloc_cmd *)TZIOC_MSG_PAYLOAD(pHdr);

            pHdr->ucType = TZIOC_TEST_MSG_MEM_ALLOC;
            pHdr->ucOrig = ptdrv->clientId;
            pHdr->ucDest = ptdrv->peerId;
            pHdr->ucSeq  = 0;
            pHdr->ulLen  = sizeof(*pCmd);

            pCmd->offset = offset;
            pCmd->size = buffsize;

            err = tzioc_msg_send(
                ptdrv->hClient,
                pHdr);

            if (err) {
                LOGE("failed to send mem alloc msg");
                return err;
            }

            spin_lock(&ptdrv->msgLock);
            ptdrv->msgCnt++;
            spin_unlock(&ptdrv->msgLock);

            /* switch to TZOS */
            tzioc_call_smc(ptdrv->hClient, 0x7);

            /* wait for rpy or timeout */
            timeout = 10;
            while (ptdrv->msgCnt > 0 && timeout--)
                msleep(1000);

            if (timeout == -1) {
                LOGE("timedout waiting for rpy");
                return -ETIMEDOUT;
            }
        }

        /* compare checksum */
        if (ptdrv->checksum != checksum) {
            LOGE("mismatched checksum from peer");
            return -EFAULT;
        }

        /* free shared memory buffer */
        tzioc_mem_free(
            ptdrv->hClient,
            vaddr);

        LOGI("mem alloc testing %d done\n", rep);
    }

    LOGI("mem alloc testing done!\n");
    return 0;
}

static int tzioc_tdrv_map_paddr(void)
{
    int timeout;
    int rep;
    int err = 0;

    for (rep = 0; rep < MAP_PADDR_TEST_REP; rep++) {
        uint8_t *vaddr;
#if MAP_PADDR_DEVICE
        dma_addr_t daddr;
#endif
        uint32_t checksum = 0;
        size_t i;

        LOGI("map paddr testing %d...", rep);

        /* alloc kernel buffer */
#if MAP_PADDR_DEVICE
        /* as uncached and unbuffered via DMA coherent region */
        vaddr = dma_alloc_coherent(
            NULL,
            MAP_PADDR_BUFF_SIZE,
            &daddr,
            GFP_KERNEL);
#else
        vaddr = kmalloc(
            MAP_PADDR_BUFF_SIZE,
            GFP_KERNEL);
#endif
        if (!vaddr) {
            LOGE("failed to alloc kernel buffer");
            return -ENOMEM;
        }

        /* fill in data */
        if (rep == 0) {
            for (i = 0; i < MAP_PADDR_BUFF_SIZE; i++) {
                *(vaddr + i) = i & 0xFF;
            }
        }
        else {
            /* fill in random data */
            get_random_bytes(vaddr, MAP_PADDR_BUFF_SIZE);
        }

        /* calculate simple checksum */
        for (i = 0; i < MAP_PADDR_BUFF_SIZE; i += 4) {
            checksum += *(uint32_t *)(vaddr + i);
        }

        LOGD("map paddr local: paddr 0x%x size 0x%x checksum 0x%x",
             (unsigned int)__pa(vaddr), MAP_PADDR_BUFF_SIZE, checksum);

        /* send map paddr cmd to peer */
        {
            static uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                               sizeof(struct tzioc_test_msg_map_paddr_cmd)];
            struct tzioc_msg_hdr *pHdr =
                (struct tzioc_msg_hdr *)msg;
            struct tzioc_test_msg_map_paddr_cmd *pCmd =
                (struct tzioc_test_msg_map_paddr_cmd *)TZIOC_MSG_PAYLOAD(pHdr);

            pHdr->ucType = TZIOC_TEST_MSG_MAP_PADDR;
            pHdr->ucOrig = ptdrv->clientId;
            pHdr->ucDest = ptdrv->peerId;
            pHdr->ucSeq  = 0;
            pHdr->ulLen  = sizeof(*pCmd);

            pCmd->paddr = __pa(vaddr);
            pCmd->size  = MAP_PADDR_BUFF_SIZE;
            pCmd->flags = 0;

            err = tzioc_msg_send(
                ptdrv->hClient,
                pHdr);

            if (err) {
                LOGE("failed to send map paddr msg");
                return err;
            }

            spin_lock(&ptdrv->msgLock);
            ptdrv->msgCnt++;
            spin_unlock(&ptdrv->msgLock);

            /* switch to TZOS */
            tzioc_call_smc(ptdrv->hClient, 0x7);

            /* wait for rpy or timeout */
            timeout = 10;
            while (ptdrv->msgCnt > 0 && timeout--)
                msleep(1000);

            if (timeout == -1) {
                LOGE("timedout waiting for rpy");
                return -ETIMEDOUT;
            }
        }

        /* compare checksum */
        if (ptdrv->checksum != checksum) {
            LOGE("mismatched checksum from peer");
            return -EFAULT;
        }

        /* free kernel buffer */
#if MAP_PADDR_DEVICE
        dma_free_coherent(
            NULL,
            MAP_PADDR_BUFF_SIZE,
            vaddr,
            daddr);
#else
        kfree(vaddr);
#endif
        LOGI("map paddr testing %d done\n", rep);
    }

    LOGI("map paddr testing done!\n");
    return 0;
}

static int tzioc_tdrv_map_paddrs(void)
{
    int timeout;
    int rep;
    int err = 0;

    for (rep = 0; rep < MAP_PADDRS_TEST_REP; rep++) {
        uint8_t *vaddrs[MAP_PADDRS_BUFF_COUNT];
#if MAP_PADDRS_DEVICE
        dma_addr_t daddrs[MAP_PADDRS_BUFF_COUNT];
#endif
        int idx;
        uint32_t checksum = 0;
        size_t i;

        LOGI("map paddrs testing %d...", rep);

        LOGD("map paddr local: count %d", MAP_PADDRS_BUFF_COUNT);

        for (idx = 0; idx < MAP_PADDRS_BUFF_COUNT; idx++) {
            /* alloc kernel buffer */
#if MAP_PADDRS_DEVICE
            /* as uncached and unbuffered via DMA coherent region */
            vaddrs[idx] = dma_alloc_coherent(
                NULL,
                MAP_PADDRS_BUFF_SIZE,
                &daddrs[idx],
                GFP_KERNEL);
#else
            vaddrs[idx] = kmalloc(
                MAP_PADDRS_BUFF_SIZE,
                GFP_KERNEL);
#endif
            if (!vaddrs[idx]) {
                LOGE("failed to alloc kernel buffer");
                return -ENOMEM;
            }

            LOGD("\t%d: paddr 0x%x, size 0x%x", idx,
                 (unsigned int)__pa(vaddrs[idx]), MAP_PADDRS_BUFF_SIZE);

            /* fill in data */
            if (rep == 0) {
                for (i = 0; i < MAP_PADDRS_BUFF_SIZE; i++) {
                    *(vaddrs[idx] + i) = i & 0xFF;
                }
            }
            else {
                /* fill in random data */
                get_random_bytes(vaddrs[idx], MAP_PADDRS_BUFF_SIZE);
            }

            /* calculate simple checksum */
            for (i = 0; i < MAP_PADDRS_BUFF_SIZE; i += 4) {
                checksum += *(uint32_t *)(vaddrs[idx] + i);
            }
        }

        LOGD("\ttotal: checksum 0x%x", checksum);

        /* send map paddrs cmd to peer */
        {
            static uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                               sizeof(struct tzioc_test_msg_map_paddrs_cmd)];
            struct tzioc_msg_hdr *pHdr =
                (struct tzioc_msg_hdr *)msg;
            struct tzioc_test_msg_map_paddrs_cmd *pCmd =
                (struct tzioc_test_msg_map_paddrs_cmd *)TZIOC_MSG_PAYLOAD(pHdr);

            pHdr->ucType = TZIOC_TEST_MSG_MAP_PADDRS;
            pHdr->ucOrig = ptdrv->clientId;
            pHdr->ucDest = ptdrv->peerId;
            pHdr->ucSeq  = 0;
            pHdr->ulLen  = sizeof(*pCmd);

            pCmd->count = MAP_PADDRS_BUFF_COUNT;

            for (idx = 0; idx < MAP_PADDRS_BUFF_COUNT; idx++) {
                pCmd->paddrs[idx] = __pa(vaddrs[idx]);
                pCmd->sizes[idx]  = MAP_PADDRS_BUFF_SIZE;
                pCmd->flags[idx]  = 0;
            }

            err = tzioc_msg_send(
                ptdrv->hClient,
                pHdr);

            if (err) {
                LOGE("failed to send map paddr msg");
                return err;
            }

            spin_lock(&ptdrv->msgLock);
            ptdrv->msgCnt++;
            spin_unlock(&ptdrv->msgLock);

            /* switch to TZOS */
            tzioc_call_smc(ptdrv->hClient, 0x7);

            /* wait for rpy or timeout */
            timeout = 10;
            while (ptdrv->msgCnt > 0 && timeout--)
                msleep(1000);

            if (timeout == -1) {
                LOGE("timedout waiting for rpy");
                return -ETIMEDOUT;
            }
        }

        /* compare checksum */
        if (ptdrv->checksum != checksum) {
            LOGE("mismatched checksum from peer");
            return -EFAULT;
        }

        /* free kernel buffer */
        for (idx = 0; idx < MAP_PADDRS_BUFF_COUNT; idx++) {
#if MAP_PADDRS_DEVICE
            dma_free_coherent(
                NULL,
                MAP_PADDRS_BUFF_SIZE,
                vaddrs[idx],
                daddrs[idx]);
#else
            kfree(vaddrs[idx]);
#endif
        }

        LOGI("map paddr testing %d done\n", rep);
    }

    LOGI("map paddr testing done!\n");
    return 0;
}

static int tzioc_tdrv_msg_proc(
    tzioc_msg_hdr *pHdr,
    uint32_t ulPrivData)
{
    UNUSED(ulPrivData);

    spin_lock(&ptdrv->msgLock);
    ptdrv->msgCnt--;
    spin_unlock(&ptdrv->msgLock);

    if (pHdr->ucOrig == TZIOC_CLIENT_ID_UAPPD) {
        switch (pHdr->ucType) {
        /* user app start rpy */
        case UAPPD_MSG_UAPP_START:
            {
                struct uappd_msg_uapp_start_rpy *pRpy =
                    (struct uappd_msg_uapp_start_rpy *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pRpy)) {
                    LOGE("invalid uapp start rpy received");
                    return -EINVAL;
                }

                LOGD("uapp start rpy: name %s, retVal %d",
                     pRpy->name, pRpy->retVal);
            }
            break;

        /* user app stop rpy */
        case UAPPD_MSG_UAPP_STOP:
            {
                struct uappd_msg_uapp_stop_rpy *pRpy =
                    (struct uappd_msg_uapp_stop_rpy *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pRpy)) {
                    LOGE("invalid uapp stop rpy received");
                    return -EINVAL;
                }

                LOGD("uapp stop rpy: name %s, retVal %d",
                     pRpy->name, pRpy->retVal);
            }
            break;

        /* user app getid rpy */
        case UAPPD_MSG_UAPP_GETID:
            {
                struct uappd_msg_uapp_getid_rpy *pRpy =
                    (struct uappd_msg_uapp_getid_rpy *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pRpy)) {
                    LOGE("invalid uapp getid rpy received");
                    return -EINVAL;
                }

                LOGD("uapp getid rpy: name %s, retVal %d, id %d",
                     pRpy->name, pRpy->retVal, pRpy->id);

                /* remember peer id */
                ptdrv->peerId = pRpy->id;
            }
            break;

        /* user app exit nfy */
        case UAPPD_MSG_UAPP_EXIT:
            {
                struct uappd_msg_uapp_exit_nfy *pNfy =
                    (struct uappd_msg_uapp_exit_nfy *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pNfy)) {
                    LOGE("invalid uapp exit nfy received");
                    return -EINVAL;
                }

                LOGD("uapp exit nfy: name %s",
                     pNfy->name);
            }
            break;

        default:
            LOGE("unknown uappd msg %d", pHdr->ucType);
            return -ENOENT;
        }
    }
    else if (pHdr->ucOrig == TZIOC_CLIENT_ID_ECHO) {
        switch (pHdr->ucType) {
        /* test echo msg */
        case TZIOC_TEST_MSG_ECHO:
            {
                struct tzioc_test_msg_echo *pEcho =
                    (struct tzioc_test_msg_echo *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pEcho)) {
                    LOGE("invalid echo msg received");
                    return -EINVAL;
                }

                LOGD("echo msg: value %d", pEcho->value);
            }
            break;

        default:
            LOGE("unknown echo msg %d", pHdr->ucType);
            return -ENOENT;
        }
    }
    else if (pHdr->ucOrig != 0 && pHdr->ucOrig == ptdrv->peerId) {
        switch (pHdr->ucType) {
        case TZIOC_TEST_MSG_HELLO:
            {
                struct tzioc_test_msg_hello_rpy *pRpy =
                    (struct tzioc_test_msg_hello_rpy *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pRpy)) {
                    LOGE("invalid hello rpy received");
                    return -EINVAL;
                }

                LOGD("hello rpy: %s", pRpy->reply);
            }
            break;

        case TZIOC_TEST_MSG_MEM_ALLOC:
            {
                struct tzioc_test_msg_mem_alloc_rpy *pRpy =
                    (struct tzioc_test_msg_mem_alloc_rpy *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pRpy)) {
                    LOGE("invalid mem alloc rpy received");
                    return -EINVAL;
                }

                LOGD("mem alloc rpy: checksum 0x%x", pRpy->checksum);

                /* remember checksum */
                ptdrv->checksum = pRpy->checksum;
            }
            break;

        case TZIOC_TEST_MSG_MAP_PADDR:
            {
                struct tzioc_test_msg_map_paddr_rpy *pRpy =
                    (struct tzioc_test_msg_map_paddr_rpy *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pRpy)) {
                    LOGE("invalid map paddr rpy received");
                    return -EINVAL;
                }

                LOGD("map paddr rpy: checksum 0x%x", pRpy->checksum);

                /* remember checksum */
                ptdrv->checksum = pRpy->checksum;
            }
            break;

        case TZIOC_TEST_MSG_MAP_PADDRS:
            {
                struct tzioc_test_msg_map_paddr_rpy *pRpy =
                    (struct tzioc_test_msg_map_paddr_rpy *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pRpy)) {
                    LOGE("invalid map paddr rpy received");
                    return -EINVAL;
                }

                LOGD("map paddr rpy: checksum 0x%x", pRpy->checksum);

                /* remember checksum */
                ptdrv->checksum = pRpy->checksum;
            }
            break;

        default:
            LOGE("unknown peer msg %d", pHdr->ucType);
            return -ENOENT;
        }
    }
    else {
        LOGE("unknown msg origin %d", pHdr->ucOrig);
        return -ENOENT;
    }

    return 0;
}
