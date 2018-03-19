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
#include <linux/jiffies.h>
#include <linux/timer.h>

#include "tzioc_api.h"
#include "uappd_msg.h"
#include "cryptoflow_msg.h"
#include "cryptoflow_tdrv.h"

/*
 * Compiler Switches
 */

/*
 * Constant Definitions
 */
#define TEST_CIPHER_DATA_SIZE 128
#define TEST_DECRYPT 1

#ifndef TEST_DECRYPT
static unsigned char crypt_input_data[] = {
0x1d, 0xf2, 0x64, 0x33, 0x97, 0xde, 0x2a, 0x53, 
0xff, 0xb6, 0x19, 0x81, 0xf2, 0x26, 0xde, 0x4b,
0xc6, 0x9b, 0x2b, 0x19, 0x87, 0xfc, 0x4c, 0xa5,
0x8b, 0x01, 0x87, 0xbd, 0x2b, 0xa9, 0xf0, 0xdd,
0xba, 0x6d, 0x0d, 0x7c, 0x13, 0x2e, 0x18, 0x30,
0xe1, 0x19, 0xef, 0x3c, 0x7e, 0x9b, 0x45, 0x6c,
0x67, 0xec, 0x97, 0x9b, 0x97, 0x4f, 0x8a, 0x23,
0x5d, 0xbb, 0xe0, 0x42, 0x5d, 0x47, 0x58, 0x88,
0x7a, 0x9e, 0xa3, 0xb2, 0x3c, 0x1f, 0x9e, 0xf7,
0x1e, 0xd2, 0x96, 0x4f, 0xb5, 0xd0, 0x2f, 0x2d,
0x20, 0x24, 0x50, 0x6b, 0xe7, 0x79, 0x23, 0x41,
0x0f, 0x19, 0x5d, 0x9b, 0x69, 0x60, 0x05, 0xbc,
0x80, 0x2f, 0x11, 0xc4, 0x2e, 0x75, 0xe0, 0x03,
0x36, 0xaa, 0xc5, 0xcb, 0xea, 0x82, 0x2e, 0xaf,
0x07, 0xe3, 0x96, 0xb4, 0x92, 0xf0, 0x5d, 0xb4,
0xf6, 0xa1, 0xbd, 0xf5, 0x1c, 0xa6, 0x58, 0x90,
};
#endif
static unsigned char crypt_key[] = { 
0xe5, 0x92, 0x37, 0x42, 0xfa, 0x76, 0x5b, 0xd7,
0xf9, 0x7a, 0xcd, 0x20, 0x30, 0xf5, 0x31, 0xa1,
0xf1, 0x11, 0x7e, 0xbc, 0xc8, 0xee, 0xda, 0x44,
0x34, 0xc0, 0x4b, 0x8e, 0x28, 0xb7, 0xe3, 0xad 
};

#ifdef TEST_DECRYPT
static unsigned char crypt_result[] = {
0x98, 0x68, 0xc4, 0x74, 0xc4, 0xef, 0x61, 0xa4,
0x16, 0x5e, 0x51, 0x58, 0x2d, 0xa8, 0x01, 0x0f,
0xbb, 0xd3, 0xc8, 0x4f, 0x07, 0x7a, 0x35, 0x78,
0xb1, 0x9c, 0xb9, 0x09, 0xed, 0x30, 0x61, 0xed,
0x42, 0xcd, 0x8e, 0x0d, 0xfb, 0xf6, 0xfa, 0x6a,
0xc1, 0x11, 0xc0, 0xa7, 0x2a, 0xa4, 0x4a, 0x1c,
0x6f, 0xa8, 0x3f, 0xa2, 0x7a, 0x29, 0x7d, 0xb0,
0x2d, 0x87, 0xce, 0x09, 0x84, 0x81, 0x4b, 0xfe,
0x78, 0xf5, 0xf6, 0x78, 0x11, 0xb7, 0x69, 0x9b,
0x09, 0x81, 0x92, 0x2b, 0x43, 0xc8, 0x72, 0x73,
0x11, 0x75, 0x8d, 0x4b, 0x9e, 0x01, 0x21, 0x59,
0x53, 0xc9, 0x13, 0x38, 0xec, 0x49, 0xef, 0x3e,
0x93, 0xa9, 0x79, 0x6b, 0xa6, 0xce, 0xd0, 0x90,
0xe7, 0x76, 0xe7, 0xa5, 0x85, 0xd8, 0x67, 0xbb,
0xc2, 0x4c, 0xea, 0x7f, 0xa4, 0xb7, 0x7e, 0x26,
0xca, 0x99, 0x1e, 0x32, 0x39, 0x08, 0x04, 0x86};
#endif

static unsigned char iv[16] = 
{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    


#define TEST_AUTH_DATA_SIZE 256

static unsigned char auth_input_data[] = {
0xaa, 0x08, 0x10, 0x2b, 0x34, 0x86, 0xb3, 0xae, 
0x27, 0x7e, 0x21, 0xd9, 0x6d, 0xa8, 0x1e, 0x5e,
0x20, 0x15, 0x30, 0x23, 0xee, 0x18, 0x78, 0x3c,
0xf6, 0x1b, 0x2c, 0xe4, 0x56, 0xe3, 0x8a, 0x8c,
0x88, 0xde, 0xe9, 0x56, 0x8e, 0x1b, 0x5b, 0x5e,
0x59, 0x32, 0x0b, 0x04, 0x78, 0x67, 0x8e, 0x9f,
0x34, 0xe5, 0x91, 0xfd, 0x1f, 0xf9, 0xee, 0x3e,
0xa7, 0xfe, 0xbe, 0x14, 0x58, 0xfa, 0x3c, 0x1c,
0x9b, 0x68, 0xae, 0x24, 0xfc, 0xc0, 0xa3, 0x1b,
0x16, 0xb8, 0xa9, 0x77, 0xee, 0xfe, 0x8b, 0xdb,
0x66, 0x83, 0x65, 0xb4, 0x04, 0xe3, 0xa3, 0x35,
0xef, 0xab, 0xa8, 0x83, 0xfe, 0xde, 0xe5, 0x77,
0x8c, 0xf4, 0x64, 0x6a, 0xcf, 0x08, 0x8c, 0xca,
0x59, 0xba, 0x9d, 0xcb, 0x43, 0x22, 0x90, 0xd7,
0x73, 0x0b, 0x6b, 0x8c, 0x45, 0xc9, 0x3e, 0x31,
0x26, 0x0c, 0x53, 0x2e, 0x93, 0x5d, 0x8e, 0xfa,
0xc7, 0x92, 0xb7, 0x4b, 0xf8, 0x93, 0x1a, 0xf7,
0x58, 0xe6, 0x07, 0x74, 0xce, 0x1c, 0xa0, 0xf5,
0xf6, 0x5d, 0xf9, 0xb0, 0xa5, 0x17, 0x1b, 0xb9,
0x29, 0x05, 0x8f, 0x32, 0xaf, 0xf8, 0x00, 0xe0,
0x0b, 0x77, 0xce, 0x03, 0x1c, 0x98, 0xee, 0xc0,
0x20, 0xe7, 0xce, 0xe2, 0xfa, 0xf6, 0xab, 0x55,
0x4a, 0xa5, 0x29, 0xc9, 0x63, 0x9b, 0xd7, 0xe7,
0x3b, 0x78, 0xbe, 0xd1, 0x89, 0x82, 0xbd, 0xb0,
0x7f, 0x0f, 0xd2, 0xa8, 0x10, 0x5a, 0x3e, 0xb3,
0xcb, 0xd5, 0xc7, 0xc5, 0x9d, 0x48, 0x0e, 0xe5,
0x36, 0xad, 0xfc, 0xb8, 0xeb, 0x47, 0xee, 0x0e,
0x33, 0x6f, 0xf6, 0x59, 0xaf, 0x82, 0xb5, 0x8c,
0xe9, 0x70, 0xb6, 0xe1, 0x62, 0xab, 0xf8, 0x35,
0x31, 0x2a, 0x97, 0x80, 0x09, 0x1b, 0x0d, 0x3f,
0xed, 0x4c, 0xfe, 0xdd, 0x90, 0xfd, 0xc0, 0x47,
0x92, 0x3f, 0x38, 0xc6, 0x8e, 0x67, 0xeb, 0x8f,
};

static unsigned char auth_key[] = {
0xc4, 0xe2, 0x5a, 0x8c, 0x6f, 0xea, 0x15, 0x68,
0xcb, 0x9b, 0x18, 0xec, 0x99, 0xd1, 0x69, 0x92,
0x73, 0x88, 0xad, 0xfc, 0x73, 0x88, 0xad, 0xfc,
0xc4, 0xe2, 0x5a, 0x8c, 0x6f, 0xea, 0x15, 0x68,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#ifdef TEST_DECRYPT
static unsigned char auth_result[] = {
0x97, 0xbb, 0x75, 0xb5, 0x36, 0x1d, 0x54, 0xbf,
0xa9, 0xb2, 0xc5, 0xd4, 0xec, 0x83, 0x47, 0x40,
0xac, 0x9d, 0xba, 0x27, 0xf1, 0xb1, 0x85, 0xb0,
0xfa, 0x24, 0x58, 0xf3, 0x81, 0xc8, 0x7c, 0xc8,
};
#endif
/*
 * Function Declarations
 */

static int __init cryptoflow_tdrv_init(void);
static void __exit cryptoflow_tdrv_exit(void);
static int cryptoflow_tdrv_deinit(void);
static int cryptoflow_tdrv_ex_packet_message(void);
static int cryptoflow_tdrv_ex_config_message(void);
static int cryptoflow_tdrv_msg_proc(
    tzioc_msg_hdr *pHdr,
    uint32_t ulPrivData);


#define NUM_PACKET_MSGS_TO_SEND 1
/*
 * Variable Declarations
 */

static char cryptoflow_tdrv_name[16]="cryptoflow_tdrv";
module_param_string(tdrvname, cryptoflow_tdrv_name, sizeof(cryptoflow_tdrv_name), 0);

static struct cryptoflow_tdrv cryptoflow_tdrv;
static struct cryptoflow_tdrv *ptdrv = &cryptoflow_tdrv;
static unsigned int messagesRcvd;
/*
 * cryptoflow Test Driver Functions
 */

static int __init cryptoflow_tdrv_init(void)
{
    int err = 0;
    int timeout;
    
    /* init test driver control block */
    memset(&cryptoflow_tdrv, 0, sizeof(cryptoflow_tdrv));

    /* init msg work, lock */
    //INIT_WORK(&ptdrv->msgWork, cryptoflow_tdrv_msg_proc);
    spin_lock_init(&ptdrv->msgLock);

    LOGI("crypto test driver initialized\n");

    LOGI("crypto example testing starts...\n");

    /* init msg proc func */
    ptdrv->pMsgProc = cryptoflow_tdrv_msg_proc;

    /* open test client */
    ptdrv->hClient = tzioc_client_open(
        cryptoflow_tdrv_name,
        ptdrv->pMsgProc,
        0, /* private data not used */
        &ptdrv->clientId);

    if (!ptdrv->hClient) {
        LOGE("failed to open client");
        return -EFAULT;
    }

    err = tzioc_peer_start(
            ptdrv->hClient,
            "cryptoflow_app",
            "cryptoflow_app.elf",
            false);
    if (err)
    {
        LOGE("failed to start peer cryptoflow_app");
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
        LOGE("timedout waiting for peer_start rpy");
        return -ETIMEDOUT;
    }

    err = tzioc_peer_getid(
            ptdrv->hClient,
            "cryptoflow_app");

    if (err) {
        LOGE("failed to get peer cryptoflow_app id");
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
        LOGE("timedout waiting for peer id rpy");
        return -ETIMEDOUT;
    }


    LOGI("Crypto client handle 0x%x, client id %d",
         ptdrv->hClient,
         ptdrv->clientId);
    
    err = cryptoflow_tdrv_ex_config_message();
    if (err) goto ERR_EXIT;

    err = cryptoflow_tdrv_ex_packet_message();
    if (err) goto ERR_EXIT;

    err = tzioc_peer_stop(
            ptdrv->hClient,
            "cryptoflow_app");
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
        LOGE("timedout waiting for peer_stop rpy");
        return -ETIMEDOUT;
    }


 ERR_EXIT:
    cryptoflow_tdrv_deinit();
    return err;
}

static void __exit cryptoflow_tdrv_exit(void)
{
    cryptoflow_tdrv_deinit();
}

static int cryptoflow_tdrv_deinit(void)
{
    int err = 0;

    if (ptdrv->hClient) {
        /* close test client */
        tzioc_client_close(ptdrv->hClient);

        /* clear test client handle */
        ptdrv->hClient = 0;
    }
    
    LOGI("Astra test driver uninitialized\n");
    return err;
}

module_init(cryptoflow_tdrv_init);
module_exit(cryptoflow_tdrv_exit);
MODULE_LICENSE("GPL");


#define CRYPTOFLOW_MSG_PAYLOAD(pHdr)    ((uint8_t *)pHdr + sizeof(cryptoflow_msg_hdr))

static int cryptoflow_tdrv_ex_config_message(void)
{

    int timeout;
    int i;
    int err = 0;

    static uint8_t msg[sizeof(struct tzioc_msg_hdr) +
                       sizeof(struct cryptoflow_msg_hdr) +
                       sizeof(struct cryptoflow_msg_config_cmd)];
    struct tzioc_msg_hdr *pHdr =
        (struct tzioc_msg_hdr *)msg;
    struct cryptoflow_msg_hdr *pCryptoHdr =
        (struct cryptoflow_msg_hdr *)TZIOC_MSG_PAYLOAD(pHdr);
    struct cryptoflow_msg_config_cmd *pCmd =
        (struct cryptoflow_msg_config_cmd *)CRYPTOFLOW_MSG_PAYLOAD(pCryptoHdr);

    LOGI("msg config testing...");

    pHdr->ucType = 0;
    pHdr->ucSeq  = 0;
    pHdr->ucOrig = ptdrv->clientId;
    pHdr->ucDest = ptdrv->peerId;
    pHdr->ulLen  = sizeof(*pCryptoHdr) + sizeof(*pCmd);
    
    pCryptoHdr->ucType = CRYPTOFLOW_MSG_CONFIG;
    pCryptoHdr->ucSeq  = 0;
    
    pCmd->authAlg = CRYPTOFLOW_AUTH_SHA256;
    pCmd->authMode = CRYPTOFLOW_AUTH_MODE_HMAC;
    pCmd->authSize = 12; 
    pCmd->cipherAlg = CRYPTOFLOW_CIPHER_AES256;
    pCmd->cipherMode = CRYPTOFLOW_CIPHER_MODE_CBC;
    pCmd->flowId = 0;

    // Populate with dummy keys as example
    for (i=0; i<CRYPTO_FLOW_MAX_KEY_SIZE; i++)
    {
        pCmd->authKey[i] = auth_key[i];
        pCmd->cipherKey[i] = crypt_key[i];
    }

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

    LOGI("msg config testing done\n");
    
    /* switch to TZOS */
    tzioc_call_smc(ptdrv->hClient, 0x7);

    /* wait for config msg replies or timeout */
    timeout = 10;
    while (ptdrv->msgCnt > 0 && timeout--)
        msleep(1000);

    if (timeout == -1) {
        LOGE("timedout waiting for rpy");
        return -ETIMEDOUT;
    }
    
    //LOGI("config testing done!\n");

    return 0;
}

    

static int cryptoflow_tdrv_ex_packet_message(void)
{
    int timeout;
    int err = 0;
    uint8_t *pdata;
    dma_addr_t pdata_dma;
    uint8_t *pdata_dst;
    dma_addr_t pdata_dst_dma;    
    uint32_t i;
    unsigned long js, je, tet;
    
    static uint8_t msgPkt[sizeof(struct tzioc_msg_hdr) +
                           sizeof(struct cryptoflow_msg_hdr) +
                           sizeof(struct cryptoflow_msg_packet_cmd)];
    
    struct tzioc_msg_hdr *pHdr =
        (struct tzioc_msg_hdr *)msgPkt;
    struct cryptoflow_msg_hdr *pCryptoHdr =
        (struct cryptoflow_msg_hdr *)TZIOC_MSG_PAYLOAD(pHdr);
    struct cryptoflow_msg_packet_cmd *pCmd =
        (struct cryptoflow_msg_packet_cmd *)CRYPTOFLOW_MSG_PAYLOAD(pCryptoHdr);

    messagesRcvd = 0;
    js = jiffies;
    pdata = kmalloc(512, GFP_KERNEL);
    memset(pdata, 0, 48);
    pdata_dma = virt_to_phys(pdata);

    pdata_dst = kmalloc(512, GFP_KERNEL);
    memset(pdata_dst, 0, 44);
    pdata_dst_dma = virt_to_phys(pdata_dst);
        
    for (i=0;i<NUM_PACKET_MSGS_TO_SEND;i++)
    {
        pHdr->ucType = 0;
        pHdr->ucSeq  = 0;
        pHdr->ucOrig = ptdrv->clientId;
        pHdr->ucDest = ptdrv->peerId;
        pHdr->ulLen  = sizeof(*pCryptoHdr) + sizeof(*pCmd);
        
        pCryptoHdr->ucType = CRYPTOFLOW_MSG_PACKET;
        pCryptoHdr->ucSeq  = (i & 0xFF);

        //LOGI("Sending message command with seq num %d\n", pCryptoHdr->ucSeq);
        
#ifdef  TEST_DECRYPT
        /* ingress buffer will have
                128 byte cipher data
                16 byte IV
                256 byte auth input data
                
           egress buff will hold
                128 bytes cipher output data
                12 byte hash
        */
        // Copy in real data:
        // Auth input
        memcpy(pdata, crypt_result, sizeof(crypt_result));
        memcpy((pdata+sizeof(crypt_result)), iv, sizeof(iv));
        memcpy((pdata+sizeof(crypt_result)+sizeof(iv)), auth_input_data, sizeof(auth_input_data));
        memcpy((pdata_dst+sizeof(crypt_result)), auth_result, sizeof(auth_result));
        
        // Dummy physical addresses
        pCmd->authDstAddr = pdata_dst_dma + sizeof(crypt_result);
        pCmd->flowId = 0;   // Send to preconfigured flow 0
        pCmd->bEncrypt = 0; // Encryption
        pCmd->cipherSrcAddr = pdata_dma;
        pCmd->cipherLen = 128; // Encryption length
        pCmd->cipherDstAddr = pdata_dst_dma;
        pCmd->authSrcAddr = pdata_dma+sizeof(crypt_result) + sizeof(iv); // Dummy physical address for physical auth start address
        pCmd->authLen = sizeof(auth_input_data); // Example Auth length (can be truncated)
        pCmd->authDstAddr = pdata_dst_dma + sizeof(crypt_result); // On encryption, where to write the auth result.  On decryption, auth result to check against 
        pCmd->ivAddr = pdata_dma+sizeof(crypt_result); // Dummy Physical address of IV.  Length known based on cipherMode

        printk("CryptoTdrv Phys: authDstAddr 0x%x, authSrcAddr 0x%x\n", (uint32_t)pCmd->authDstAddr, (uint32_t)pCmd->authSrcAddr);
        printk("CryptoTdrv Phys: cipherDstAddr 0x%x, cipherSrcAddr 0x%x, cipherIv 0x%x\n", (uint32_t)pCmd->cipherDstAddr, (uint32_t)pCmd->cipherDstAddr, (uint32_t)pCmd->ivAddr);
#else // encrypt
        /* ingress buffer will have
                       128 byte cipher data
                       16 byte IV
                       128 byte auth input data
                       
                  egress buff will hold
                       128 bytes cipher output data
                       12 byte hash
               */
        // Copy in real data:
        // Auth input
        memcpy(pdata, crypt_input_data, sizeof(crypt_input_data));
        memcpy((pdata+sizeof(crypt_input_data)), iv, sizeof(iv));
        memcpy((pdata+sizeof(crypt_input_data)+sizeof(iv)), auth_input_data, sizeof(auth_input_data));
       
        // Dummy physical addresses
        pCmd->authDstAddr = pdata_dst_dma + sizeof(crypt_input_data);
        pCmd->flowId = 0;   // Send to preconfigured flow 0
        pCmd->bEncrypt = 1; // Encryption
        pCmd->cipherSrcAddr = pdata_dma;
        pCmd->cipherLen = 128; // Encryption length
        pCmd->cipherDstAddr = pdata_dst_dma;
        pCmd->authSrcAddr = pdata_dma+sizeof(crypt_input_data) + sizeof(iv); // Dummy physical address for physical auth start address
        pCmd->authLen = sizeof(auth_input_data); // Example Auth length (can be truncated)
        pCmd->authDstAddr = pdata_dst_dma + sizeof(crypt_input_data); // On encryption, where to write the auth result.  On decryption, auth result to check against 
        pCmd->ivAddr = pdata_dma+sizeof(crypt_input_data); // Dummy Physical address of IV.  Length known based on cipherMode

        printk("CryptoTdrv Phys: authDstAddr 0x%x, authSrcAddr 0x%x\n", (uint32_t)pCmd->authDstAddr, (uint32_t)pCmd->authSrcAddr);
        printk("CryptoTdrv Phys: cipherDstAddr 0x%x, cipherSrcAddr 0x%x, cipherIv 0x%x\n", (uint32_t)pCmd->cipherDstAddr, (uint32_t)pCmd->cipherDstAddr, (uint32_t)pCmd->ivAddr);

#endif

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

        /* wait for packet msg replies or timeout */
        timeout = 100;
        while (ptdrv->msgCnt > 0 && timeout--)
            msleep(1);

        if (timeout == -1) {
            LOGE("timedout waiting for rpy");
            return -ETIMEDOUT;
        }
    }
    je = jiffies;
    tet = je - js;
    LOGI("Pkt Test Done: Js %lu Je %lu Tet %lu\n", js, je, tet);
    
    LOGI("Hz is %lu Ms %lu\n", HZ, (tet * 1000 / HZ));
    return 0;
}

static int cryptoflow_tdrv_msg_proc(
    tzioc_msg_hdr *pHdr,
    uint32_t ulPrivData)
{
    struct cryptoflow_msg_hdr *pCryptoHdr =
        (struct cryptoflow_msg_hdr *)TZIOC_MSG_PAYLOAD(pHdr);

    UNUSED(ulPrivData);

    spin_lock(&ptdrv->msgLock);
    ptdrv->msgCnt--;
    spin_unlock(&ptdrv->msgLock);

    LOGI("Message from ASTRA received, pHdr->ucOrig %d, pCryptoHdr->ucType %d\n", pHdr->ucOrig, pCryptoHdr->ucType);

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

                LOGE("uapp getid rpy: name %s, retVal %d, id %d",
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

    if (pHdr->ucOrig == 2) {
        //LOGI("Message type %d from ASTRA received\n", pCryptoHdr->ucType);
        switch (pCryptoHdr->ucType) {
        /* test echo msg */
        case CRYPTOFLOW_MSG_CONFIG:
            {
                
            }
            break;

        case CRYPTOFLOW_MSG_PACKET:
            {
                //LOGI("Packet reply from Astra seq %d\n", pCryptoHdr->ucSeq);
                if (pCryptoHdr->ucSeq != (messagesRcvd & 0xFF))
                {
                    LOGI("Rcvd seq %d but expected %d\n", pCryptoHdr->ucSeq, messagesRcvd);
                }
                messagesRcvd++;
                if (messagesRcvd == NUM_PACKET_MSGS_TO_SEND)
                {
                    LOGI("Received last message from Astra\n");
                }
            }
            break;
        default:
            LOGE("unknown echo msg %d", pHdr->ucType);
            return -ENOENT;
        }
    }
   

    return 0;
}



