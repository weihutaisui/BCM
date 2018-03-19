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
#include "cpuload_msg.h"
#include "cpuload_tdrv.h"

/*
 * Compiler Switches
 */

/*
 * Function Declarations
 */

static int __init cpuload_tdrv_init(void);
static void __exit cpuload_tdrv_exit(void);
static int cpuload_tdrv_deinit(void);

static int cpuload_tdrv_open(void);
static int cpuload_tdrv_peer_start(void);
static int cpuload_tdrv_msg_stop(void);

static int cpuload_tdrv_msg_echo(void);
static int cpuload_tdrv_msg_hello(void);

static int cpuload_tdrv_msg_proc(
    tzioc_msg_hdr *pHdr,
    uint32_t ulPrivData);

/*
 * Variable Declarations
 */

static char cpuload_tdrv_name[16]="cpuload_tdrv";
static char cpuload_tapp_name[16]="cpuload_tapp";

module_param_string(tdrvname, cpuload_tdrv_name, sizeof(cpuload_tdrv_name), 0);

static struct cpuload_tdrv cpuload_tdrv;
static struct cpuload_tdrv *ptdrv = &cpuload_tdrv;

/*
 * TZIOC Test Driver Functions
 */

static int __init cpuload_tdrv_init(void)
{
    int err = 0;

    /* init test driver control block */
    memset(&cpuload_tdrv, 0, sizeof(cpuload_tdrv));

    /* init msg proc func */
    ptdrv->pMsgProc = cpuload_tdrv_msg_proc;

    /* init msg lock */
    spin_lock_init(&ptdrv->msgLock);

    LOGI("CpuLoad App Started\n");

    /* client test */
    err = cpuload_tdrv_open();
    if (err) goto ERR_EXIT;

    /* Start TA*/
    err = cpuload_tdrv_peer_start();
    if (err) goto ERR_EXIT;

    /* msg test - echo */
    err = cpuload_tdrv_msg_echo();
    if (err) goto ERR_EXIT;

    /* msg test - hello */
    err = cpuload_tdrv_msg_hello();
    if (err) goto ERR_EXIT;

    return 0;
ERR_EXIT:
    cpuload_tdrv_deinit();
    return err;
}

static void __exit cpuload_tdrv_exit(void)
{
    cpuload_tdrv_deinit();
}

static int cpuload_tdrv_deinit(void)
{
    int err = 0;

    err = cpuload_tdrv_msg_stop();
    if (err) LOGE("Error trying to stop TA\n");

    if (ptdrv->peerId) {
        /* stop peer tzioc_tapp */
        tzioc_peer_stop(
            ptdrv->hClient,
            cpuload_tapp_name);

        /* clear id for peer tzioc_tapp */
        ptdrv->peerId = 0;
    }

    if (ptdrv->hClient) {
        /* close test client */
        tzioc_client_close(ptdrv->hClient);

        /* clear test client handle */
        ptdrv->hClient = 0;
    }

    LOGI("CpuLoad App Stopped\n");
    return err;
}

module_init(cpuload_tdrv_init);
module_exit(cpuload_tdrv_exit);
MODULE_LICENSE("GPL");

/*
 * TZIOC Test Functions
 */

static int cpuload_tdrv_open(void)
{

    /* open/close test client repeatedly */

    /* open test client */
    ptdrv->hClient = tzioc_client_open(
        cpuload_tdrv_name,
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


    return 0;
}

static int cpuload_tdrv_peer_start(void)
{
    int timeout;
    int err = 0;


    /* start peer tzioc_tapp */
    err = tzioc_peer_start(
        ptdrv->hClient,
        cpuload_tapp_name,
        "cpuload_tapp.elf",
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
        cpuload_tapp_name);

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

    return 0;
}

static int cpuload_tdrv_msg_stop(void)
{
    int timeout;
    int err = 0;

    static uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                       sizeof(struct cpu_load_msg_stop)];

    struct tzioc_msg_hdr *pHdr =
        (struct tzioc_msg_hdr *)msg;
    struct cpu_load_msg_stop *pCmd =
        (struct cpu_load_msg_stop *)TZIOC_MSG_PAYLOAD(pHdr);

    pHdr->ucType = CPU_LOAD_MSG_STOP;
    pHdr->ucOrig = ptdrv->clientId;
    pHdr->ucDest = ptdrv->peerId;
    pHdr->ucSeq  = 0;
    pHdr->ulLen  = sizeof(*pCmd);

    pCmd->value = 0; /* ignore for now */

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

    return 0;
}

static int cpuload_tdrv_msg_echo(void)
{
    int timeout;
    int rep = 0;
    int err = 0;

    /* send echo msg repeatedly */
    static uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                       sizeof(struct cpu_load_msg_echo)];

    struct tzioc_msg_hdr *pHdr =
        (struct tzioc_msg_hdr *)msg;
    struct cpu_load_msg_echo *pEcho =
        (struct cpu_load_msg_echo *)TZIOC_MSG_PAYLOAD(pHdr);

    LOGI("msg echo testing %d...", rep);

    /* send echo msg to echo client */
    pHdr->ucType = CPU_LOAD_MSG_ECHO;
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

static int cpuload_tdrv_msg_hello(void)
{
    int timeout;
    int err = 0;

    static uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                       sizeof(struct cpu_load_msg_hello_cmd)];
    struct tzioc_msg_hdr *pHdr =
        (struct tzioc_msg_hdr *)msg;
    struct cpu_load_msg_hello_cmd *pCmd =
        (struct cpu_load_msg_hello_cmd *)TZIOC_MSG_PAYLOAD(pHdr);

    pHdr->ucType = CPU_LOAD_MSG_HELLO;
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


    /* switch to TZOS */
    tzioc_call_smc(ptdrv->hClient, 0x7);

    /* wait for rpy or timeout */
    timeout = 100;
    while (ptdrv->msgCnt > 0 && timeout--)
        msleep(1000);

    if (timeout == -1) {
        LOGE("timedout waiting for rpy");
        return -ETIMEDOUT;
    }

    return 0;
}


static int cpuload_tdrv_msg_proc(
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
        case CPU_LOAD_MSG_ECHO:
            {
                struct cpu_load_msg_echo *pEcho =
                    (struct cpu_load_msg_echo *)TZIOC_MSG_PAYLOAD(pHdr);

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
        case CPU_LOAD_MSG_HELLO:
            {
                struct cpu_load_msg_hello_rpy *pRpy =
                    (struct cpu_load_msg_hello_rpy *)TZIOC_MSG_PAYLOAD(pHdr);

                if (pHdr->ulLen  != sizeof(*pRpy)) {
                    LOGE("invalid hello rpy received");
                    return -EINVAL;
                }

                LOGD("hello rpy: %s", pRpy->reply);
            }
            break;
            case CPU_LOAD_MSG_STOP:
                {
                    struct cpu_load_msg_stop *pRpy =
                        (struct cpu_load_msg_stop *)TZIOC_MSG_PAYLOAD(pHdr);

                    if (pHdr->ulLen  != sizeof(*pRpy)) {
                        LOGE("invalid hello rpy received");
                        return -EINVAL;
                    }

                    LOGD("TA Stopped rpy: %d", pRpy->value);
                }
            break;

        }
    }
    else {
        LOGE("unknown msg origin %d", pHdr->ucOrig);
        return -ENOENT;
    }

    return 0;
}
