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

#include <linux/slab.h>
#include <linux/of.h>

#include "tzioc_drv.h"
#include "tzioc_mem.h"

/* static pointer to mem module */
static struct tzioc_mem_module *pMemMod;
static struct tzioc_mem_cb *pMemCB;

/* exported mem control block to common code */
struct tzioc_mem_cb *pTziocMemCB;

int __init _tzioc_mem_module_init(void)
{
    struct device_node *parent, *node;
    const uint32_t *params;
    int paramCount;
    uint32_t heapsOffset, heapsSize;
    struct tzioc_mem_heap_layout heapsLayout[TZIOC_MEM_HEAP_MAX];
    char layoutString[16 * TZIOC_MEM_HEAP_MAX];
    int i;

    /* find TZIOC nw-heaps device tree node */
    parent = of_find_node_by_name(NULL, "tzioc");
    if (!parent) {
        LOGE("No TZIOC nw-heaps node in device tree");
        return -ENOENT;
    }

    node = of_find_node_by_name(parent, "nw-heaps");
    if (!node) {
        LOGE("No TZIOC nw-heaps node in device tree");
        return -ENOENT;
    }

    /* get TZIOC heaps reg property from device tree */
    params = of_get_property(node, "reg", &paramCount);
    if (!params) {
        LOGE("Failed to get TZIOC heaps reg property from device tree");
        return -EINVAL;
    }

    heapsOffset = of_read_ulong(&params[0], 1);
    heapsSize   = of_read_ulong(&params[1], 1);

    LOGI("TZIOC heaps at 0x%x, size 0x%x",
         (unsigned int)heapsOffset, (unsigned int)heapsSize);

    /* get TZIOC heaps layout property from device tree */
    memset(heapsLayout, 0, sizeof(heapsLayout));

    params = of_get_property(node, "layout", &paramCount);
    if (!params) {
        uint32_t numPages;
        uint32_t numHeaps;
        uint32_t numUnits;
        uint32_t unitPages;
        uint32_t availPages;
        int i;

        LOGE("Failed to get TZIOC heaps layout property from device tree");

        /* use default layout algorithm */
        numPages = heapsSize >> TZIOC_MEM_PAGE_SHIFT;

        /* set initial heaps and assignment unit */
        numHeaps = TZIOC_MEM_HEAP_MAX;
        numUnits = (1 << (numHeaps - 1)) * numHeaps;
        unitPages  = (numPages + numUnits - 1) / numUnits;

        /* reduce number of heaps if pages are not enough */
        while (numPages < ((1 << (numHeaps - 1)) * numHeaps)) {
            numHeaps--;
            unitPages = 1; /* unitPages can not go below 1 */
        }

        /* assign pages to heaps */
        availPages = numPages;
        for (i = numHeaps - 1; i >= 0; i--) {
            uint32_t buffPages = unitPages << i;
            uint32_t heapPages = availPages / (i + 1);
            uint32_t buffCount = heapPages / buffPages;

            heapsLayout[i].ulBuffPages = buffPages;
            heapsLayout[i].ulBuffCount = buffCount;

            availPages -= buffPages * buffCount;
        }
    }
    else {
        const uint32_t *layoutData = params;
        uint32_t layoutCount = paramCount / (sizeof(int) * 2);

        for (i = 0; i < layoutCount; i++) {
            heapsLayout[i].ulBuffPages =
                of_read_ulong(layoutData++, 1);
            heapsLayout[i].ulBuffCount =
                of_read_ulong(layoutData++, 1);
        }
    }

    layoutString[0] = '\0';
    for (i = 0; i < TZIOC_MEM_HEAP_MAX; i++) {
        if (!heapsLayout[i].ulBuffPages)
            break;

        sprintf(layoutString, "%s(%d %d) ",
            layoutString,
            (int)heapsLayout[i].ulBuffPages,
            (int)heapsLayout[i].ulBuffCount);
    }
    LOGI("TZIOC heaps layout %s", layoutString);

    /* alloc mem module */
    pMemMod = kzalloc(sizeof(struct tzioc_mem_module), GFP_KERNEL);
    if (!pMemMod) {
        LOGE("Failed to alloc TZIOC mem module");
        return -ENOMEM;
    }

    /* remember mem module in TZIOC device */
    tdev->pMemMod = pMemMod;

    /* init spinlock */
    spin_lock_init(&pMemMod->lock);

    /* init shared memory */

    /* init mem control block */
    pMemCB = &pMemMod->memCB;
    __tzioc_heaps_init(
        pMemCB->heaps,
        pMemCB->tables,
        heapsOffset,
        heapsSize,
        heapsLayout);

    /* export mem control block to common code */
    pTziocMemCB = pMemCB;

    LOGI("TZIOC mem module initialized");
    return 0;
}

int _tzioc_mem_module_deinit(void)
{
    /* reset exported mem control block to common code */
    pTziocMemCB = NULL;

    /* deinit mem control block */
    pMemCB = NULL;

    /* reset mem module in TZIOC device */
    tdev->pMemMod = NULL;

    /* free mem module control block */
    kfree(pMemMod);

    LOGI("TZIOC mem module deinitialized");
    return 0;
}

void *_tzioc_mem_alloc(
    struct tzioc_client *pClient,
    uint32_t ulSize)
{
    void *pBuff;
    int err = 0;

    spin_lock(&pMemMod->lock);
    err = __tzioc_mem_alloc(pClient->id, ulSize, &pBuff);
    spin_unlock(&pMemMod->lock);

    if (err) {
        LOGE("Failed to alloc mem, client %d", pClient->id);
        return NULL;
    }

    return pBuff;
}

void _tzioc_mem_free(
    struct tzioc_client *pClient,
    void *pBuff)
{
    UNUSED(pClient);

    spin_lock(&pMemMod->lock);
    __tzioc_mem_free(pClient->id, pBuff);
    spin_unlock(&pMemMod->lock);
}
