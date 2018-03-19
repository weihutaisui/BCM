/*
<:copyright-BRCM:2015:GPL/GPL:standard

   Copyright (c) 2015 Broadcom 
   All Rights Reserved

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

:>
*/
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/of_fdt.h>
#include <linux/sched.h>
#include <linux/sizes.h>
#include <asm/pgtable.h>
#include <asm/system_misc.h>
#include <bcm_map_part.h>
#include <board.h>
#include <asm/mach/map.h>
#include <linux/memblock.h>
#include <bcm_extirq.h>
#if !defined(CONFIG_BCM96858)
#include <pmc_drv.h>
#endif

#if defined (CONFIG_BCM947189) || defined(CONFIG_BCM96846)
#include <linux/init.h>
#include "board.h"
#include <asm/mach/map.h>
#endif
#if defined(CONFIG_BCM96846)
extern void create_mapping(struct map_desc *md);
#endif

#ifdef CONFIG_OF

extern unsigned long reserved_mem_total;
extern unsigned long getMemorySize(void);
extern int rsvd_mem_cnt;
extern reserve_mem_t reserve_mem[TOTAL_RESERVE_MEM_NUM];
extern void check_if_rootfs_is_set(char *cmdline);
extern int bcm_early_scan_dt(unsigned long node, const char *uname, int depth, void *data);

#if defined(CONFIG_BCM_B15_MEGA_BARRIER)
static int __init bcm_reserve_mem_add_list(char* rsrv_name, uint32_t phys_offs, uint32_t size)
{
	if (reserved_mem_total + size > phys_offs) {
		return -1;
	}
	if (rsvd_mem_cnt >= TOTAL_RESERVE_MEM_NUM) {
		printk("reserved memory count %d reached the total memory reserve count %d!!!", rsvd_mem_cnt, TOTAL_RESERVE_MEM_NUM);
		return -1; 
	}
	strcpy(reserve_mem[rsvd_mem_cnt].name, rsrv_name);
	reserve_mem[rsvd_mem_cnt].phys_addr = phys_offs - size; 
	reserve_mem[rsvd_mem_cnt].size = size;
	memblock_remove(reserve_mem[rsvd_mem_cnt].phys_addr, reserve_mem[rsvd_mem_cnt].size);
	reserved_mem_total += size;
	rsvd_mem_cnt++;
	return 0;	
}
#endif

int __init bcm_dt_postinit(void)
{
	check_if_rootfs_is_set(boot_command_line);
	return 0;
}

int __init bcm_reserve_memory(void)
{
#if defined(CONFIG_BCM96846)
    struct map_desc desc[TOTAL_RESERVE_MEM_NUM+1];
    int i;

    printk(KERN_INFO "     Reserved memory mapping...\n");
    printk(KERN_INFO "                phys          virt       size\n");
    for(i = 0; i < rsvd_mem_cnt; i++ )
    {
        if (reserve_mem[i].mapped == 0)
        {
            desc[i].virtual = (unsigned long)phys_to_virt(reserve_mem[i].phys_addr);
            desc[i].pfn = __phys_to_pfn(reserve_mem[i].phys_addr);
            desc[i].length = reserve_mem[i].size;
            desc[i].type = MT_MEMORY_RWX_NONCACHED;
            reserve_mem[i].mapped = 1;
            create_mapping(&desc[i]);
        }
        printk("   %s   0x%08x  0x%08x  0x%08x\n", reserve_mem[i].name, (unsigned int)reserve_mem[i].phys_addr, (unsigned int)desc[i].virtual, (unsigned int)reserve_mem[i].size);
    }
#else
	uint32_t phys_offs = getMemorySize();
	int i;
	if (phys_offs > SZ_256M) {
		phys_offs = SZ_256M;
	}
	for(i = 0 ; i < rsvd_mem_cnt; i++ ) {
		/*memblock_remove(reserve_mem[i].phys_addr, reserve_mem[i].size);*/
		if (phys_offs > reserve_mem[i].phys_addr) {
			phys_offs = reserve_mem[i].phys_addr;
		}  
	}

#if defined(CONFIG_BCM_B15_MEGA_BARRIER)
	if (bcm_reserve_mem_add_list(B15_MEGA_BARRIER, 
					phys_offs, 
					SECTION_SIZE)) {
		return -1;
	}
#endif
	for(i = 0 ; i < rsvd_mem_cnt; i++ ) {
		printk(KERN_INFO "Reserved memory: phys 0x%08x size 0x%08x for %s\n", 
				reserve_mem[i].phys_addr,  
				(uint32_t)reserve_mem[i].size, 
				reserve_mem[i].name);
	}
#endif

	return 0;
}

EXPORT_SYMBOL(bcm_reserve_memory);

#endif

#if defined(CONFIG_BCM947189) || defined(CONFIG_BCM96846)
static void bcm_sys_restart(enum reboot_mode reboot_mode, const char *cmd)
{
#if !defined(CONFIG_BRCM_IKOS)
	kerSysSoftReset();
#endif
}

int __init bcm_arch_early_init(void)

{
	arm_pm_restart = bcm_sys_restart;	

#ifdef CONFIG_OF
	memset(reserve_mem, 0x0, sizeof(reserve_mem_t)*TOTAL_RESERVE_MEM_NUM);
	of_scan_flat_dt(bcm_early_scan_dt, NULL);
#ifdef CONFIG_BCM96846
	bcm_reserve_memory();
#endif
#endif
	check_if_rootfs_is_set(boot_command_line);

#if defined(CONFIG_BCM96846)
	pmc_init();
#endif

#if !defined(CONFIG_BRCM_IKOS)
	kerSysEarlyFlashInit();
	kerSysFlashInit();
#endif

	/* Setup external irqs */
	bcm_extirq_init();

	return 0;
}

early_initcall(bcm_arch_early_init);
#endif

