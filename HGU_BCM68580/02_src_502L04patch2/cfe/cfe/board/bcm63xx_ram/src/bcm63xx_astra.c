/*  *********************************************************************
    *  Broadcom Common Firmware Environment (CFE)
    *
    *  Board device initialization      File: bcm94710_devs.c
    *
    *  This is the "C" part of the board support package.  The
    *  routines to create and initialize the console, wire up
    *  device drivers, and do other customization live here.
    *
    *  Author:  Mitch Lichtenberg (mpl@broadcom.com)
    *
    *********************************************************************
    *
    *  XX Copyright 2000,2001
    *  Broadcom Corporation. All rights reserved.
    *
    *  BROADCOM PROPRIETARY AND CONFIDENTIAL
    *
    *  This software is furnished under license and may be used and
    *  copied only in accordance with the license.
    ********************************************************************* */
#include "bcm63xx_util.h"
#include "bcm63xx_dtb.h"

int decompressLZMA(void *src, unsigned srcLen, void *dst, unsigned dstLen);
void *memcpy(void *dest, const void *src, unsigned n);
// XXX can't include "cfe_iocb.h" due to unused cfe_iocb_t typedef
void _cfe_flushcache(int, void *, void *);
unsigned int g_astra_dtb;
unsigned int g_astra_addr;
uint64_t g_astra_size;
extern unsigned short g_force_mode;
#define CFE_CACHE_FLUSH_RANGE 64

static int astra_load(const char *path, unsigned pathlen, unsigned char *addr, int lz)
{
    unsigned size = 0;
    unsigned char *data = 0;
    int rc = cfe_fs_fetch_file(path, pathlen, &data, &size);

    if (rc) {
        printf("%s: %s rc %d\n", __func__, path, rc);
        return 0;
    }

    if (lz) {
        // decompress to aligned buffer and flush cache range
        rc = decompressLZMA(data + 5 * sizeof (uint32), size, addr, 16 << 20);
        if (rc)
            printf("%s: %s rc %d\n", __func__, path, rc);
    } else
        // copy to aligned buffer and flush cache range
        memcpy(addr, data, size);
    _cfe_flushcache(CFE_CACHE_FLUSH_RANGE, addr, addr + size - 1);
    printf("%s: copy %s size %u to %p\n", __func__, path, size, addr);
    return size;
}

// load astra image(s) from boot filesystem
void astra_init(void)
{
    static const char binpath[] = "astra.lz";
    const unsigned binpathlen = sizeof binpath - 1;
    static const char dtbpath[] = "astra.dtb";
    const unsigned dtbpathlen = sizeof dtbpath - 1;
    int dtb_len = 0;
    void* astra_dtb;

    /* Allocate memory to load Astra dtb */
    astra_dtb = KMALLOC(CFG_DTB_MAX_SIZE, 0);

    /* Load uncompressed Astra device tree blob */
    if (astra_dtb && (dtb_len = astra_load(dtbpath, dtbpathlen, astra_dtb, 0))) {
       char node[64];
       int nodeoffset = 0;
       const char *propdata;
       int proplen = 0;

       dtb_init(astra_dtb, 0);
       dtb_prepare(DTB_PREPARE_FDT, astra_dtb, dtb_len);
       /* Look for Astra memory node in the dtb */
       sprintf(node,"%s%s",DT_ROOT_NODE,DT_MEMORY_NODE);
       nodeoffset = fdt_path_offset(astra_dtb, node);
       if (nodeoffset == -FDT_ERR_NOTFOUND) {
          printf ("ERROR: Failed to obtain Astra memory node\n");
          dtb_done();
          goto err_out;
       }
       /* Extract memory region information from device tree */
       propdata = fdt_getprop(astra_dtb, nodeoffset, "reg", &proplen);
       g_astra_addr = fdt32_to_cpu(*(uint32_t*)propdata);
       g_astra_size = fdt32_to_cpu(*(uint32_t*)(propdata + sizeof(uint32_t)));
       /* Move Astra dtb at the end of the Astra image */
       g_astra_dtb = g_astra_addr + g_astra_size;
       memcpy((void*)g_astra_dtb, astra_dtb, dtb_len);
       _cfe_flushcache(CFE_CACHE_FLUSH_RANGE, (void*)g_astra_dtb, (void*)(g_astra_dtb + dtb_len));
       /* Update total Astra size including Astra dtb, which must be MB aligned */
       g_astra_size = (g_astra_size + dtb_len + 0xFFFFF) & 0xFFF00000;
       dtb_done();
    }
    else {
      printf ("ERROR: Failed to load Astra dtb\n");
      goto err_out;
    }

    /* Load compressed Astra image */
    if (g_astra_addr && astra_load(binpath, binpathlen, (void*)g_astra_addr, 1))
       g_force_mode = 1;
    else
       printf ("ERROR: Failed to allocate memory\n");

 err_out:
    KFREE(astra_dtb);
}

void reserve_astra_memory(void)
{
   char dt_node_name[64];

   return;
   sprintf(dt_node_name, "%s%s", DT_RSVD_PREFIX_STR, ASTRA_BASE_ADDR_STR);
   dtb_set_reserved_memory(dt_node_name, g_astra_addr, g_astra_size);
}
