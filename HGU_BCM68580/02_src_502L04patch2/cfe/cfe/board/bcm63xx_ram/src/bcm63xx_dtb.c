/*
<:copyright-BRCM:2015:DUAL/GPL:standard

   Copyright (c) 2015 Broadcom 
   All Rights Reserved

Unless you and Broadcom execute a separate written software license
agreement governing use of this software, this software is licensed
to you under the terms of the GNU General Public License version 2
(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
with the following added to such license:

   As a special exception, the copyright holders of this software give
   you permission to link this software with independent modules, and
   to copy and distribute the resulting executable under terms of your
   choice, provided that you also meet, for each linked independent
   module, the terms and conditions of the license of that module.
   An independent module is a module which is not derived from this
   software.  The special exception does not apply to any modifications
   of the software.

Not withstanding the above, under no circumstances may you combine
this software in any way with any other Broadcom software provided
under a license other than the GPL, without Broadcom's express prior
written consent.

:>
*/

#include "cfe.h"
#include "lib_types.h"
#include "lib_string.h"
#include "lib_printf.h"
#include "bcm63xx_dtb.h"
#include "bcm63xx_util.h"
#include "bcm63xx_blparms.h"
#include "bsp_config.h"
#include "boardparms.h"
#include "flash_api.h"
#if (INC_EMMC_FLASH_DRIVER == 1) 
#include "dev_emmcflash.h"
#endif

#if defined(_BCM96858_) || defined(_BCM94908_) || defined(_BCM96836_) || defined(_BCM963158_) || defined(_BCM96846_) ||\
    defined(_BCM96856_)
#include "bcm_otp.h"
#endif

/* default dtb */
extern unsigned char dtb[];
extern unsigned int dtb_len;

static void *dtb_ptr = NULL;
static int dtb_sz = 0;
static dtb_prepare_opt_t dtb_prepare_status = DTB_PREPARE_NONE;

static int dtb_setprop(void *fdt, const char *node_path, const char *property,
                   uint32_t *val_array, int size)
{
    int offset = fdt_path_offset(fdt, node_path);

    if (offset == -FDT_ERR_NOTFOUND)
        return NULL;

    return fdt_setprop(fdt, offset, property, val_array, size);
}

const void *dtb_getprop(const void *fdt, const char *node_path,
                   const char *property, int *len)
{
    int offset = fdt_path_offset(fdt, node_path);

    if (offset == -FDT_ERR_NOTFOUND)
        return NULL;

    return fdt_getprop(fdt, offset, property, len);
}

static const int dtb_delnode(void *fdt, const char *node_path)
{
    int offset = fdt_path_offset(fdt, node_path);

    if (offset == -FDT_ERR_NOTFOUND)
        return NULL;

    return fdt_del_node(fdt, offset);
}


void dtb_init(void* dtb_addr, int len)
{
    dtb_ptr = (void *)dtb_addr;
    if (len)
       memset(dtb_ptr, 0x0, len);
}

static int dtb_prepare_fdt(const unsigned char* dt_img, int len)
{
    unsigned char* pdtb = dtb_ptr;
    if (!pdtb ) { 
         printf("%s ERROR: invalid pointer\n",__func__);
         return -1;
    }

    if( !dt_img  || len <= 0 ) { 
         printf("%s ERROR:invalid dt image \n",__func__);
         return -1;
    }

    if( len + BLPARMS_LEN  > CFG_DTB_MAX_SIZE ) {
       printf("%s ERROR:invalid dtb size %d\n",__func__, len + BLPARMS_LEN);
       return -1;
    }

    memcpy(pdtb, dt_img, len);

    if( fdt_open_into(pdtb, pdtb, len + BLPARMS_LEN + sizeof(NVRAM_DATA)) != 0 ) {
       printf("%s ERROR:can't open fdt\n",__func__);
       return -1;
    }

    return 0;

}

dtb_prepare_opt_t dtb_get_prepare_status()
{
    return dtb_prepare_status;
}

int dtb_prepare(enum DTB_PREPARE_OPT prep_opt, const unsigned char* img, int size)
{
    unsigned char* dtb_img = NULL; 
    unsigned int dtb_img_size = 0; 
    int ret = 0;

    switch(prep_opt) {
       case DTB_PREPARE_FDT_DEF:
            if(    ( flash_get_flash_type() ==  FLASH_IFC_NAND )
                || ( flash_get_flash_type() ==  FLASH_IFC_SPINAND ) )
            {
#if (INC_NAND_FLASH_DRIVER == 1) || (INC_SPI_NAND_DRIVER == 1)
                ret = cfe_fs_fetch_file(NAND_FLASH_BOOT_DTB_NAME, 
                    NAND_FLASH_BOOT_DTB_NAME_LEN, &dtb_img, &dtb_img_size ); 
#endif
            }
            else if(( flash_get_flash_type() ==  FLASH_IFC_SPI )
                 || ( flash_get_flash_type() ==  FLASH_IFC_HS_SPI ) )
            {
#if (INC_NAND_FLASH_DRIVER==0)
                ret = getDtbFromTag(&dtb_img, &dtb_img_size);
#endif
            }
#if (INC_EMMC_FLASH_DRIVER == 1) 
            else if( flash_get_flash_type() == FLASH_IFC_UNSUP_EMMC )
            {
                ret = emmc_get_dtb( NAND_FLASH_BOOT_DTB_NAME,
                    NAND_FLASH_BOOT_DTB_NAME_LEN, &dtb_img, &dtb_img_size);
            }
#endif
            else
                ret = -1;
            break; 
      case DTB_PREPARE_FDT:
            dtb_img = (unsigned char*)img;
            dtb_img_size = size; 
            break;
      default:
            ret = -1;
    }

    if (!ret) {
            ret = dtb_prepare_fdt(dtb_img, dtb_img_size);
            dtb_prepare_status = prep_opt;
    }
    else
        printf("Failed to retrieve the dtb!!!\n");
    return ret;
} 


int dtb_done(void)
{
    int ret = 0;
    if (!dtb_ptr) {
        return -1;
    }
    ret = fdt_pack(dtb_ptr);
    dtb_sz = fdt_totalsize(dtb_ptr);
    dtb_prepare_status = DTB_PREPARE_NONE;
    return ret;
}

int dtb_append_data_blob( const char* blob_name, const char * blob_data, unsigned int blob_size )
{
    int offset;
    if (!dtb_ptr) {
        return -1;
    }
    offset = fdt_path_offset(dtb_ptr, "/");
    if (offset == -FDT_ERR_NOTFOUND)
        return offset;

    return fdt_appendprop(dtb_ptr, offset, blob_name, blob_data, blob_size);
}

int dtb_set_blparms(const char* parms, unsigned int size)
{
    return dtb_append_data_blob(BRCM_BLPARMS_PROP, parms, size);
}

int dtb_del_reserved_memory(char* name)
{
    char node_name[96];

    if (!dtb_ptr) {
        return -1;
    }
    sprintf(node_name, "/reserved-memory/%s", name);
    return dtb_delnode(dtb_ptr, node_name);
}


static void dtb_get_addr_size_cells(const char* dtb, 
                                 const char* node_path,
                                 uint32_t* addr_cell_sz,
                                 uint32_t* size_cell_sz) 
{
    uint32_t* prop;
    int len; 
    prop = (uint32_t*)dtb_getprop(dtb, node_path, "#address-cells", &len);
    *addr_cell_sz = prop? cpu_to_fdt32(*prop):OF_NODE_ADDR_CELLS_DEFAULT; 
    prop = (uint32_t*)dtb_getprop(dtb_ptr, node_path, "#size-cells", &len);
    *size_cell_sz = prop? cpu_to_fdt32(*prop):OF_NODE_SIZE_CELLS_DEFAULT; 
}
#if CFG_DT_EXTRA 
static int dtb_setprop_ul(void* fdt, int node_offs, const char* prop, unsigned long val)
{
    uint32_t addr_cell_sz, size_cell_sz;
    dtb_get_addr_size_cells(dtb_ptr, DT_ROOT_NODE, &addr_cell_sz, &size_cell_sz); 
    if (addr_cell_sz > 2 || size_cell_sz > 2) {
        printf("%s Cells sizes are not supported %d %d \n", __func__, addr_cell_sz, size_cell_sz);
        return -1;
    }
    return size_cell_sz==1? fdt_setprop_u32(fdt, node_offs, prop, (uint32_t) val) : 
                               fdt_setprop_u64(fdt, node_offs, prop, val); 
}
#endif
int dtb_getprop_reg(const char* node_name_par, 
                    const char *node_name,
                    uint64_t* addr, 
                    uint64_t* size)
{
    uint32_t addr_cell_sz, size_cell_sz; 
    const uint32_t* prop;
    int len;
    char node_path[96];
    uint64_t temp;

    if (!dtb_ptr) {
        return -1;
    }
    sprintf(node_path, "/%s", node_name_par);
    dtb_get_addr_size_cells(dtb_ptr, node_path, &addr_cell_sz, &size_cell_sz); 
    if (addr_cell_sz > 2 || size_cell_sz > 2) {
        printf("%s  Cells sizes are not supported %d %d \n", __func__, addr_cell_sz, size_cell_sz);
        return -1;
    }
    sprintf(node_path, "/%s/%s", node_name_par, node_name);
    prop = (const uint32_t*)dtb_getprop(dtb_ptr, node_path, "reg", &len);
    if (prop &&  len == (size_cell_sz+addr_cell_sz)*sizeof(uint32_t) ) {
        if (size_cell_sz == 1) {
           *size = (uint64_t)fdt32_to_cpu(*(prop+addr_cell_sz));
        } else {
            memcpy((unsigned char*)&temp, (unsigned char*)(prop+addr_cell_sz), sizeof(uint64_t));
            *size = fdt64_to_cpu(temp); 
        }

        if (addr_cell_sz == 1) {
            *addr = (uint64_t)fdt32_to_cpu(*prop); 
        } else {
            memcpy((unsigned char*)&temp, (unsigned char*)prop, sizeof(uint64_t));
            *addr = fdt64_to_cpu(temp); 
        }
        return 0;

    }
    return -1;
}

int dtb_get_reserved_memory(char* name, uint64_t* addr, uint64_t* size)
{
    const uint32_t* prop;
    char node_name[96];
    int len;
    uint32_t addr_cell_sz, size_cell_sz;
    uint64_t temp;

    dtb_get_addr_size_cells(dtb_ptr,"/reserved-memory", &addr_cell_sz, &size_cell_sz);
    if (addr_cell_sz > 2 || size_cell_sz > 2)
    {
        printf("%s  Cells sizes are not supported %d %d \n", __func__, addr_cell_sz, size_cell_sz);
        return -1;
    }

    sprintf(node_name, "/reserved-memory/%s", name);

    prop = dtb_getprop(dtb_ptr, node_name, "reg", &len);
    if(prop)
    {
        if (addr_cell_sz == 1)
        {
            *addr = fdt32_to_cpu(*(uint32_t*)prop);
        }
        else
        {
            memcpy((unsigned char*)&temp, (unsigned char*)prop, sizeof(uint64_t));
            *((uint64_t*) addr) = fdt64_to_cpu(temp);
        }

        if (size_cell_sz == 1)
        {
            *(size) = fdt32_to_cpu(*(uint32_t*)(prop + addr_cell_sz));
        }
        else
        {
            memcpy((unsigned char*)&temp, (unsigned char*)(prop + addr_cell_sz), sizeof(uint64_t));
            *(uint64_t*) (size) = fdt64_to_cpu(temp);
        }
        return 0;
    }
    printf("dtb_get_reserved_memory failed to get reserved memory node %s\n", node_name);
    return -1;
}

int dtb_set_reserved_memory(char* name, uint64_t addr, uint64_t size)
{
    const void* prop;
    char node_name[96];
    uint32_t propval[4];
    int len;
    uint32_t addr_cell_sz, size_cell_sz; 
    uint64_t temp;

    /*printf("dtb_set_reserved_memory name %s, addr 0x%x, size 0x%x\n", name, (uint32_t)addr, (uint32_t)size);*/

    if (!dtb_ptr) {
        return -1;
    }
    dtb_get_addr_size_cells(dtb_ptr,"/reserved-memory", &addr_cell_sz, &size_cell_sz); 
    if (addr_cell_sz > 2 || size_cell_sz > 2) {
        printf("%s  Cells sizes are not supported %d %d \n", __func__, addr_cell_sz, size_cell_sz);
        return -1;
    }

    sprintf(node_name, "/reserved-memory/%s", name);

    /* assume address and size are 64 bit for 4908. need to read #address-cells and #size_cells
       determine the actual size */
    prop = dtb_getprop(dtb_ptr, node_name, "reg", &len);
    /* sanity check */
    if (prop &&  len == (size_cell_sz+addr_cell_sz)*sizeof(uint32_t) ) {
        if (addr_cell_sz == 1) {
            *propval = cpu_to_fdt32(addr);
        } else {
            temp = cpu_to_fdt64(addr); 
            memcpy((unsigned char*)propval, (unsigned char*)&temp, sizeof(uint64_t));
        }
 
        if (size_cell_sz == 1) {
            *(propval+addr_cell_sz) = cpu_to_fdt32(size); 
        } else {
            temp = cpu_to_fdt64(size); 
            memcpy((unsigned char*)(propval+addr_cell_sz), (unsigned char*)&temp, sizeof(uint64_t));
        }
        /* setting only size of the memory e.g. size_cell of the 'reg' */
        memcpy((void*)prop, (char*)propval, len);
        dtb_setprop(dtb_ptr, node_name, "reg", (uint32_t*)prop, len);
        return 0;
    }
    printf("WARNING: Node's property %s is not defined\n",node_name);

    return -1;
}

static int dtb_update_memory_reg(char* prop, uint64_t addr, uint64_t size, uint32_t addr_cell_sz, uint32_t size_cell_sz)
{
    uint32_t tmp32;
    uint64_t tmp64;
    int len = 0;

    printf("/memory = 0x%llx bytes @ 0x%llx\n", size, addr); 

    if (addr_cell_sz == 1 && addr > 0x100000000)
        return -1;

    if (size_cell_sz == 1 && size > 0x100000000)
        return -1;

    if (addr_cell_sz == 1) {
        tmp32 = cpu_to_fdt32((uint32_t)addr);
        memcpy((unsigned char*)prop, (unsigned char*)&tmp32, sizeof(uint32_t));
        len += sizeof(uint32_t);
        prop += sizeof(uint32_t);
    } else {
        tmp64 = cpu_to_fdt64(addr);
        memcpy((unsigned char*)prop, (unsigned char*)&tmp64, sizeof(uint64_t));
        len += sizeof(uint64_t);
        prop += sizeof(uint64_t);
    }

    if (size_cell_sz == 1) {
        tmp32 = cpu_to_fdt32((uint32_t)size);
        memcpy((unsigned char*)prop, (unsigned char*)&tmp32, sizeof(uint32_t));
        len += sizeof(uint32_t);
        prop += sizeof(uint32_t);
    } else {
        tmp64 = cpu_to_fdt64(size);
        memcpy((unsigned char*)prop, (unsigned char*)&tmp64, sizeof(uint64_t));
        len += sizeof(uint64_t);
        prop += sizeof(uint64_t);
    }

    return len;
}

int dtb_set_memory(uint64_t memsize)
{
    char* prop, propval[32]; /*maximum two reg entry with 64bit addr and size */ 
    int len, total_len, split = 0;
    uint32_t addr_cell_sz, size_cell_sz;
    uint64_t size, addr, split_size;
    int ret = -1;

    /* For the chips that defines PHYS_DRAM_SPLIT_SIZE, it means the DDR memory 
       space is not continous if total size is large than the split size. Eg in 
       63158, first 2G is from address 0x0 to 0x08000-0000. The next 2G ddr is at
       0x10000-0000 to 0x18000-0000. So we have to add two reg entries to the 
       memory nodes */
#ifdef PHYS_DRAM_SPLIT_SIZE
    split_size = PHYS_DRAM_SPLIT_SIZE;
#else
    /* no split in ddr space, ddr size must be less than 2G. set a dummy large
       size here so split will not be enabled */
    split_size = 0x80000000;
#endif

    if (!dtb_ptr) {
        return -1;
    }

    dtb_get_addr_size_cells(dtb_ptr,"/", &addr_cell_sz, &size_cell_sz); 
    if (addr_cell_sz > 2 || size_cell_sz > 2) {
        printf("%s Cells sizes are not supported %d %d \n", __func__, addr_cell_sz, size_cell_sz);
        return -1;
    }

    memset(propval, 0x0, sizeof(propval));

    prop = (char* )dtb_getprop(dtb_ptr, "/memory", "reg", &total_len);
    if (prop &&  total_len == (size_cell_sz+addr_cell_sz)*sizeof(uint32_t) && total_len < sizeof(propval)) {
        memcpy(propval, prop, total_len);
        total_len = 0;
        if (memsize > split_size ) {
            split = 1;
            size = split_size;
        } else {
            split = 0;
            size = memsize;
        }
        addr = DRAM_BASE;

        len = dtb_update_memory_reg(propval, addr, size, addr_cell_sz, size_cell_sz);
        if (len < 0) {
            ret = -2;
            goto done;
        }
        total_len += len;

        if( split ) {
#if defined(DRAM_BASE_2)
            addr = DRAM_BASE_2;
#else
            addr = DRAM_BASE+size;
#endif
            size = memsize - split_size;
            len = dtb_update_memory_reg(propval+len, addr, size, addr_cell_sz, size_cell_sz);
            if (len < 0) {
               ret = -3;
               goto done;
            }
            total_len += len;
        }

        dtb_setprop(dtb_ptr, "/memory", "reg", (uint32_t*)propval, total_len );
        ret = 0;
    }

done:
    if( ret != 0 )
        printf("dtb_set_memory failed ret=%d\n", ret);

    return ret;
}

int dtb_set_chosen_root(char * root_device_full_pathname )
{
    void* fdt = dtb_ptr;
    const void *propdata =NULL;
    int nodeoffset, res = -1,proplen = 0;

    char *node = NULL,*prop,**err_msg = &prop, *bootargs;
    if (!fdt) {
        return res;
    }
    node = KMALLOC(sizeof(DT_CHOSEN_NODE)+sizeof(DT_ROOT_NODE),0);
    if (!node) {
       goto err_out;
    }
    sprintf(node,"%s%s",DT_ROOT_NODE,DT_CHOSEN_NODE);
    nodeoffset = fdt_path_offset(fdt, node);
    if (nodeoffset == -FDT_ERR_NOTFOUND) {
       *err_msg = node;
       goto err_out;
    }
    prop = DT_BOOTARGS_PROP;
    propdata = fdt_getprop(fdt, nodeoffset, prop, &proplen); 
    
    /* edit this to include mmc device root node in DT_BOOTARGS */
    bootargs = KMALLOC(proplen+DT_BOOTARGS_MAX_SIZE,0); 
    if (!bootargs) {
         goto err_out;
    }
    sprintf(bootargs," root=%s rootwait ", root_device_full_pathname);

    if (propdata) {
        lib_strncat(bootargs, propdata, proplen+DT_BOOTARGS_MAX_SIZE);
    }

    printf("Setting ROOTFS: root=%s\n", root_device_full_pathname);
    res = fdt_setprop_string(fdt, nodeoffset, prop, bootargs); 
    if (res) {
        goto err_out;
    }

    res = 0;
err_out:
    KFREE(node);
    if (bootargs) {
        KFREE(bootargs);
    }
    if (res && *err_msg) {
        printf("Error accessing %s\n",*err_msg);
    }
    return res;
}

int dtb_set_chosen_initrd(uint64_t initrd_addr, uint64_t initrd_size)
{
    void* fdt = dtb_ptr;
    const void *propdata =NULL;
    int nodeoffset, res = -1,proplen = 0;

    char *node = NULL,*prop,**err_msg = &prop, *bootargs;
    if (!fdt) {
        return res;
    }
    node = KMALLOC(sizeof(DT_CHOSEN_NODE)+sizeof(DT_ROOT_NODE),0);
    if (!node) {
       goto err_out;
    }
    sprintf(node,"%s%s",DT_ROOT_NODE,DT_CHOSEN_NODE);
    nodeoffset = fdt_path_offset(fdt, node);
    if (nodeoffset == -FDT_ERR_NOTFOUND) {
       *err_msg = node;
       goto err_out;
    }
    prop = DT_INITRD_START_PROP;
    res = fdt_setprop_u32(fdt, nodeoffset, prop, _PA(initrd_addr));
    if (res) {
        goto err_out;
    }
    prop = DT_INITRD_END_PROP;
    res = fdt_setprop_u32(fdt, nodeoffset, prop, _PA(initrd_addr)+initrd_size);
    if (res) {
        goto err_out;
    }
    prop = DT_BOOTARGS_PROP;
    propdata = fdt_getprop(fdt, nodeoffset, prop, &proplen); 

    bootargs = KMALLOC(proplen+DT_BOOTARGS_MAX_SIZE,0); 
    if (!bootargs) {
         goto err_out;
    }
    sprintf(bootargs," %s %s0x%llx ",DT_BOOTARGS,DT_BOOTARGS_UPD_OPT, initrd_size);
    if (propdata) {
        lib_strncat(bootargs, propdata, proplen+DT_BOOTARGS_MAX_SIZE);
    }
    res = fdt_setprop_string(fdt, nodeoffset, prop, bootargs); 
    if (res) {
        goto err_out;
    }

    res = 0;
err_out:
    KFREE(node);
    if (bootargs) {
        KFREE(bootargs);
    }
    if (res && *err_msg) {
        printf("Error accessing %s\n",*err_msg);
    }
    return res;
}

#if defined(_BCM96858_) || defined(_BCM94908_) || defined(_BCM96836_) || defined(_BCM963158_) || defined(_BCM96846_) ||\
    defined(_BCM96856_)
int dtb_set_nr_cpus(void)
{
    unsigned int nr_cpus, nr_bp_cpus = 0;
    int ret;
#if defined(_BCM96836_) || defined(_BCM96846_) || defined(_BCM96856_)
    int max_num_of_cpu = 2;
#else
    int max_num_of_cpu = 4;
#endif

    if (!dtb_ptr) {
        return -1;
    }
    ret = bcm_otp_get_nr_cpus(&nr_cpus);
    if (ret)
        return -1;

    nr_cpus = max_num_of_cpu - nr_cpus;

    /* check if there is overwrite from board parameter */
    if( BpGetMaxNumCpu(&nr_bp_cpus) == BP_SUCCESS && nr_bp_cpus < nr_cpus )
        nr_cpus = nr_bp_cpus;

    while(nr_cpus < max_num_of_cpu)
    {
        int len;
        char cpu_node[32];
        void* cpu_property;

        sprintf(cpu_node, "/cpus/cpu@%d", nr_cpus);
        cpu_property = (void*)dtb_getprop(dtb_ptr, cpu_node, "device_type", &len);
        if (cpu_property)
        {
            memcpy((char*)(cpu_property), "not", 3);
            dtb_setprop(dtb_ptr, cpu_node, "device_type", (uint32_t*)cpu_property, len);
        }
        else
        {
            printf("failed to set number of cpus to dtb\n");
            return -1;
        }
        nr_cpus++;
    }

    return 0;

}
#endif

int dtb_set_cmdline(char* cmdline)
{
    return 0;
}

int dtb_get_cpu_rel_addr(uint64_t* rel_addr, int cpu)
{
    int len;
    void* rel_addr_property;
    uint64_t addr;
    char cpu_node[32];

    if (!dtb_ptr) {
        return -1;
    }
    sprintf(cpu_node, "/cpus/cpu@%d", cpu);
    rel_addr_property = (uint64_t*)dtb_getprop(dtb_ptr, cpu_node, "cpu-release-addr", &len);
    if( rel_addr_property )
    {
        memcpy((char*)&addr, (char*)(rel_addr_property), sizeof(uint64_t));
        *rel_addr = fdt64_to_cpu(addr);
        //printf("cpu release address 0x%llx\n", *rel_addr);

        return 0;
    }
    else
    {
        printf("dtb_get_cpu_rel_addr failed to get the property\n");
        return -1;
    }
}

int dtb_get_fdt_size(void)
{
     return dtb_sz; 
}

const void* dtb_get_fdt(void)
{
     return dtb_ptr;
}
