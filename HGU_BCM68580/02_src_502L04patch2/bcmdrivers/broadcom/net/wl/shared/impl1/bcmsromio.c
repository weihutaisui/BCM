/*
    Copyright (c) 2017 Broadcom
    All Rights Reserved

    <:label-BRCM:2017:DUAL/GPL:standard

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

#if defined(WLC_LOW) || defined(DSLCPE_DONGLEHOST_WOMBO)
#ifndef LINUX_VERSION_CODE 
#include <linuxver.h>
#endif

#define MAX_SROM_FILE_SIZE SROM_MAX

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <osl.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/unistd.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <typedefs.h>
#include <bcmdevs.h>
#include "boardparms.h"
#include "bcmsrom_fmt.h"
#include "siutils.h"
#include "bcmutils.h"

#ifdef DSLCPE_DONGLEHOST_WOMBO
int BCMATTACHFN(sprom_update_params)(si_t *sih, uint16 *buf, bool *idx );
#else
int BCMATTACHFN(sprom_update_params)(si_t *sih, uint16 *buf );
#endif
extern int BpGetBoardId( char *pszBoardId);

#define MAX_TOT_NVRAM_FILE_NAME_SIZE 256
#define NVRAM_FILE_NAME_SIZE 64
char nvramloaded[] = "/tmp/nvramloaded";
char nvramloaded_last[NVRAM_FILE_NAME_SIZE] = {0};

int findMatching(char *buf, char *words, int size)
{
	int i = 0;
	char *cur = buf;
	while(i < size/NVRAM_FILE_NAME_SIZE){
	        if(!strcmp(cur, words)) 
		        return 1;
		else 
		        while(*cur++!='\0');
		cur++;
		i++;
	}
	return 0;
}

void remove_substr(char *buf, char *rm)
{
	int i = 0;
	char *cur = buf;
	while (i <= 4) {
		if (!strncmp(cur, rm, NVRAM_FILE_NAME_SIZE)) {
			memset(cur, 0, NVRAM_FILE_NAME_SIZE);
		} else {
			cur += NVRAM_FILE_NAME_SIZE;
		}
		i++;
	}
	return;
}

int readSromFile(si_t *sih, uint chipId, void *buf, uint nbytes, char *pBoardId)
{
	struct file *fp = NULL;
	char fname[64]={0};
	char BoardId[32]={0};
	mm_segment_t fs;
	char *base = NULL;
	int size = 0;
	int patch_status = BP_BOARD_ID_NOT_FOUND;
	int ret = -1;
	char devpath[SI_DEVPATH_BUFSZ];

	struct file *nvramloaded_fp = NULL;
	char buff[MAX_TOT_NVRAM_FILE_NAME_SIZE] = {0}; 

	nvramloaded_fp = filp_open(nvramloaded, O_RDWR|O_CREAT, 0644);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
	if (!IS_ERR(nvramloaded_fp)) {
#else
	if (!IS_ERR(nvramloaded_fp) && nvramloaded_fp->f_op && nvramloaded_fp->f_op->read ) {
#endif
		fs = get_fs();
		set_fs(get_ds());
		nvramloaded_fp->f_pos = 0;	

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))  
		size = vfs_read(nvramloaded_fp, (void *)buff, MAX_TOT_NVRAM_FILE_NAME_SIZE, &nvramloaded_fp->f_pos);
#else
		size = nvramloaded_fp->f_op->read(nvramloaded_fp, (void *)buff, MAX_TOT_NVRAM_FILE_NAME_SIZE, &nvramloaded_fp->f_pos);
#endif
		set_fs(fs);
	}
	else {
		printf("read/create /tmp/nvramloaded error %ld\n", PTR_ERR(nvramloaded_fp));	  
		return ret;
	}

	si_devpath(sih, devpath, sizeof(devpath));
	printk("wl: ID=%s\n", devpath);
	
	if (pBoardId)
		sprintf(BoardId, "_%s", pBoardId);

	if ((chipId & 0xff00) == 0x4300 || (chipId & 0xff00) == 0x6300 ) /* DSLCPE_WOC */
	{
		int i = 1;
		sprintf(fname, "/etc/wlan/bcm%04x%s_map.bin", chipId, BoardId);
		while(findMatching(buff, fname, size))
		        sprintf(fname, "/etc/wlan/bcm%04x%s_wl%d_map.bin", chipId, BoardId, i++);
	}
	else if ((chipId / 1000) == 43) 
	{
		int i = 1;
		sprintf(fname, "/etc/wlan/bcm%d%s_map.bin", chipId, BoardId);
		while(findMatching(buff, fname, size))
		        sprintf(fname, "/etc/wlan/bcm%d%s_wl%d_map.bin", chipId, BoardId, i++);
	}
	else if ((chipId / 1000) == 53)
	{ //For 47189
		int i = 1;
		sprintf(fname, "/etc/wlan/bcm%s%s_map.bin", "47189", BoardId);
		while(findMatching(buff, fname, size))
		        sprintf(fname, "/etc/wlan/bcm%s%s_wl%d_map.bin", "47189", BoardId, i++);
	}
	else {
		filp_close(nvramloaded_fp,NULL);
		return ret;
	}
#if defined(CONFIG_BRCM_IKOS) /* DSLCPE_WOC */
	goto direct_read;
#endif
	fp = filp_open(fname, O_RDONLY, 0);	

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
	if (!IS_ERR(fp)) {
#else
	if (!IS_ERR(fp) && fp->f_op && fp->f_op->read ) {
#endif
		fs = get_fs();
		set_fs(get_ds());
		
		fp->f_pos = 0;

		base = kmalloc(MAX_SROM_FILE_SIZE, GFP_KERNEL);
		if (!base) {
			printk("%s: failed to malloc.\n", __FUNCTION__);
			filp_close(fp, NULL);
			return ret;
		}
		
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
		size = vfs_read(fp, (void *)base, MAX_SROM_FILE_SIZE, &fp->f_pos);
#else
		size = fp->f_op->read(fp, (void *)base, MAX_SROM_FILE_SIZE, &fp->f_pos);
#endif
		if (size <= 0) {
			printk("Failed to read image header from '%s'.\n", fname);
		} else {		
			printk("wl: loading %s\n", fname);
			/* patch srom map with info from boardparams */
			patch_status = BpUpdateWirelessSromMap(chipId, (uint16*)base, size/sizeof(uint16));
				
			/* patch srom map with info from CPE's flash */
#ifndef DSLCPE_DONGLEHOST_WOMBO
			sprom_update_params(sih, (uint16 *)base);
#endif
			bcopy(base, buf, MIN(size,nbytes));
			ret = 0;
		}
		set_fs(fs);		
		filp_close(fp, NULL);

		kfree(base);

		fs = get_fs();
		set_fs(get_ds());
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
		vfs_write(nvramloaded_fp, fname, NVRAM_FILE_NAME_SIZE, &nvramloaded_fp->f_pos);
#else
		nvramloaded_fp->f_op->write(nvramloaded_fp, fname, NVRAM_FILE_NAME_SIZE, &nvramloaded_fp->f_pos);
#endif
		set_fs(fs);

		/* also save the last loaded srom map file name */
		sprintf(nvramloaded_last, fname);
	}
	else {
#if defined(CONFIG_BRCM_IKOS) /* DSLCPE_WOC */
direct_read:
#endif
		if (pBoardId) {
			filp_close(nvramloaded_fp, NULL);
			return ret;
		}

		/* After meeting followings, This path is hit.
		*  When board specific srom file (e.g bcm<chipid>_<boardid>_map.bin) is not found, and  
		*  normal srom file (e.g bcm<chipid>_map.bin) is not found. Or
		*  CONFIG_BRCM_IKOS enabled
		*/

#ifdef DSLCPE_WOMBO_BUILTIN_SROM
		{
			uint16 *wl_srom_map = NULL;
			extern uint16 wl_srom_map_4306[64];
			extern uint16 wl_srom_map_4318[64];		
			extern uint16 wl_srom_map_4321[220];
			extern uint16 wl_srom_map_4322[220];
			extern uint16 wl_srom_map_a8d6[220]; /* 43222 */
			extern uint16 wl_srom_map_6362[220]; /* 43226, DSLCPE_WOC */
			int len = 0;
			{			
				switch (chipId) {
					case 0x4306:
						wl_srom_map = wl_srom_map_4306;
						len = sizeof(wl_srom_map_4306);
						break;
					case 0x4318:
						wl_srom_map = wl_srom_map_4318;
						len = sizeof(wl_srom_map_4318);
						break;				
					case 0x4321:
						wl_srom_map = wl_srom_map_4321;
						len = sizeof(wl_srom_map_4321);
						break;
					case 0xa8d6: /* 43222 */
						wl_srom_map = wl_srom_map_a8d6;
						len = sizeof(wl_srom_map_a8d6);
						break;						
					case 0x4322:
						wl_srom_map = wl_srom_map_4322;				
						len = sizeof(wl_srom_map_4322);
						break;
					case 0x6362: /* 43226, DSLCPE_WOC */
						wl_srom_map = wl_srom_map_6362;
						len = sizeof(wl_srom_map_6362);
						break;
					default:
						break;
				}
			}
			printk("Reading srom image from kernel\n");		
			if (wl_srom_map) {
				bcopy(wl_srom_map, buf, MIN(len, nbytes));
				ret = 0;
			} else {
				printk("Error reading srom image from kernel\n");
				ret = -1;
			}		
		}			
#endif	/* DSLCPE_WOMBO_BUILTIN_SROM */

	}
	filp_close(nvramloaded_fp, NULL);
	return ret;
}

int BCMATTACHFN(init_srom_sw_map)(si_t *sih, uint chipId, void *buf, uint nbytes) 
{	
	int ret = -1;
	char pszBoardId[32];

	ASSERT(nbytes <= MAX_SROM_FILE_SIZE);

	BpGetBoardId(pszBoardId);

	/* Two kinds of srom files needs be checked. 
	*  The first is chip+board depended (e.g bcm<chipid>_<boardid>_map.bin).
	*  If it is not found, the second one, chip depended (e.g bcm<chipid>_map.bin), needs be checked.
	*/ 
	if ((ret = readSromFile(sih, chipId, buf, nbytes, pszBoardId)) != 0) {

		ret = readSromFile(sih, chipId, buf, nbytes, NULL);
	}

	return ret;			
}


int read_sromfile(void *swmap, void *buf, uint offset, uint nbytes)
{
	bcopy((char*)swmap+offset, (char*)buf, nbytes);
	return 0;
}


extern int kerSysGetWlanSromParamsLen(void);
extern int kerSysGetWlanSromParams( unsigned char *wlanCal, unsigned short len);
#define SROM_PARAMS_LEN 256

typedef struct entry_struct {
unsigned short offset;
unsigned short value;
} Entry_struct;

typedef struct adapter_struct {
unsigned char id[SI_DEVPATH_BUFSZ];
unsigned short entry_num;
struct entry_struct  entry[1];
} Adapter_struct;

#ifdef DSLCPE_DONGLEHOST_WOMBO
int BCMATTACHFN(sprom_update_params)(si_t *sih, uint16 *buf, bool *idx )
#else
int BCMATTACHFN(sprom_update_params)(si_t *sih, uint16 *buf )
#endif
{
	uint16 adapter_num=0, entry_num=0, pos=0;
	
	struct adapter_struct *adapter_ptr = NULL;
	struct entry_struct *entry_ptr = NULL;
	
	char id[SI_DEVPATH_BUFSZ];
	int i=0, j=0;
	int ret = 0;
	   
	char devpath[SI_DEVPATH_BUFSZ];
	char *params = NULL;
	int param_len = kerSysGetWlanSromParamsLen();
	if (param_len<=0) 
		return -1;
	params = kmalloc(param_len,GFP_KERNEL);
	if (!params) 
		return -1;

	si_devpath(sih, devpath, sizeof(devpath));
	kerSysGetWlanSromParams( params, param_len);
	
	/* params format
	adapter_num uint16
	devId1            char 16
	entry_num      uint 16
	offset1            uint16
	value1            uint16
	offset2            uint16
	value2            uint16
	...
	devId2           char 16
	entry_num     uint 16
	offset1           uint16
	value2           uint16
	
	*/
       adapter_num = *(uint16 *)(params);
       pos = 2;

	for ( i=0; (i< adapter_num) && (pos <param_len); i++ ) {
        adapter_ptr = (struct adapter_struct *)(params +pos);
		strncpy( id, adapter_ptr->id, SI_DEVPATH_BUFSZ );			  
 		entry_num=  adapter_ptr->entry_num;
		if ( !strncmp(id, devpath, strlen(devpath) ) ) {
 		       entry_ptr = (struct entry_struct *) &(adapter_ptr->entry);
 		       printk("wl: updating srom from flash...\n");
 		       for ( j=0; j< entry_num; j++, entry_ptr++ ) {
				buf[entry_ptr->offset] = entry_ptr->value;
#ifdef DSLCPE_DONGLEHOST_WOMBO
				idx[entry_ptr->offset] = 1;
#endif
		      }

			ret = 1;
			break;
		}

		/*goto next adapter parameter*/
		pos += SI_DEVPATH_BUFSZ + sizeof(uint16)+ entry_num*sizeof(uint16)*2; 
       }
	kfree(params);
	return ret;
}


int BCMATTACHFN(init_sromvars_map)(si_t *sih, uint chipId, void *buf, uint nbytes) 
{	
	struct file *fp = NULL;
	char fname[64] = {0};
	int ret = -1;
	mm_segment_t fs;
	int size;

	if ((chipId & 0xff00) == 0x4300 || (chipId & 0xff00) == 0x6300 ) /* DSLCPE_WOC */
		sprintf(fname, "/etc/wlan/bcm%04x_vars.bin", chipId);
	else if ((chipId / 1000) == 43) 
		sprintf(fname, "/etc/wlan/bcm%d_vars.bin", chipId);
	else
		return ret;

	ASSERT(nbytes <= VARS_MAX);

	fp = filp_open(fname, O_RDONLY, 0);	

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
	if (!IS_ERR(fp)) {
#else
	if (!IS_ERR(fp) && fp->f_op && fp->f_op->read ) {
#endif
		fs = get_fs();
		set_fs(get_ds());
		
		fp->f_pos = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
		size = vfs_read(fp, (void *)buf, VARS_MAX, &fp->f_pos);
#else
		size = fp->f_op->read(fp, (void *)buf, VARS_MAX, &fp->f_pos);
#endif
		if (size > 0) {
			printk("wl:loading %s, file size=%d\n", fname, size);
			ret = 0;
		}
		set_fs(fs);		
		
		filp_close(fp, NULL);							
	} else {

#ifdef DSLCPE_WOMBO_BUILTIN_SROM
		{
			uint16 *wl_sromvars_map = NULL;
			extern char wl_sromvars_4331[];
			int len = 0;
			if(ret == -1) {
				
				switch (chipId) {
					case 0x4331:
						wl_sromvars_map = wl_sromvars_4331;
						len = sizeof(wl_sromvars_4331);
						break;
					default:
						break;
				}
			}
			printk("Reading vars image from kernel\n");		
			if (wl_srom_map) {
				bcopy(wl_sromvars_map, buf, MIN(len, nbytes));
				ret = 0;
			} else {
				printk("Error reading vars image from kernel\n");
				ret = -1;
			}		
		}
#endif	/* DSLCPE_WOMBO_BUILTIN_SROM */
	}

	return ret;	
}

int read_nvramfile(char *fname, void *buf)
{
	struct file *fp = NULL;
	int ret = -1;
	mm_segment_t fs;
	int size=0;
	
	fp = filp_open(fname, O_RDONLY, 0);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
	if (!IS_ERR(fp)) {
#else
	if (!IS_ERR(fp) && fp->f_op && fp->f_op->read ) {
#endif
		fs = get_fs();
		set_fs(get_ds());

		fp->f_pos = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
		size = vfs_read(fp, (void *)buf, VARS_MAX, &fp->f_pos);
#else
		size = fp->f_op->read(fp, (void *)buf, VARS_MAX, &fp->f_pos);
#endif
		if (size > 0) {
			printk("wl: reading %s, file size=%d\n", fname, size);
			ret = 0;
		}
		set_fs(fs);

		filp_close(fp, NULL);
	}

	return ret;
}

int BCMATTACHFN(init_nvramvars_cmn)(si_t *sih, void *buf)
{
	char fname[] = "/etc/wlan/bcmcmn_nvramvars.bin";

	/* read common nvram */
	return read_nvramfile(fname, buf);
}

int BCMATTACHFN(init_nvramvars_chip)(si_t *sih, uint chipId, void *buf)
{
	char fname[64];
	int ret = -1;

	/* read nvram per chip */
	if ((chipId & 0xff00) == 0x4300 || (chipId & 0xff00) == 0x6300 )
		sprintf(fname, "/etc/wlan/bcm%04x_nvramvars.bin", chipId);
	else if ((chipId / 1000) == 43)
		sprintf(fname, "/etc/wlan/bcm%d_nvramvars.bin", chipId);
	else if ((chipId / 1000) == 53) // 47189 53573
		sprintf(fname, "/etc/wlan/bcm%d_nvramvars.bin",47189);
	else
		return ret;

	return read_nvramfile(fname, buf);
}

void BCMATTACHFN(reinit_loaded_srommap)(void)
{
	mm_segment_t fs;
	struct file *nvramloaded_fp = NULL;
	char buff_loaded[256] = {0};

	nvramloaded_fp = filp_open(nvramloaded, O_RDWR, 0644);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
	if (!IS_ERR(nvramloaded_fp)) {
#else
	if (!IS_ERR(nvramloaded_fp) && nvramloaded_fp->f_op && nvramloaded_fp->f_op->read ) {
#endif
		fs = get_fs();
		set_fs(get_ds());
		nvramloaded_fp->f_pos = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
		vfs_read(nvramloaded_fp, (void *)buff_loaded, sizeof(buff_loaded), &nvramloaded_fp->f_pos);
#else
		nvramloaded_fp->f_op->read(nvramloaded_fp, (void *)buff_loaded, sizeof(buff_loaded), &nvramloaded_fp->f_pos);
#endif
		set_fs(fs);
	} else {
		/* no such loaded file exists */
		return;
	}

	/* remove the srommap file of nvramloaded_last */
	remove_substr(buff_loaded, nvramloaded_last);

	/* write back the update nvramloaded fname */
	fs = get_fs();
	set_fs(get_ds());
	nvramloaded_fp->f_pos = 0;
	if (strlen(buff_loaded) == 0)
		memset(buff_loaded, 0, sizeof(buff_loaded));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0))
	vfs_write(nvramloaded_fp, buff_loaded, sizeof(buff_loaded), &nvramloaded_fp->f_pos);
#else
	nvramloaded_fp->f_op->write(nvramloaded_fp, buff_loaded, sizeof(buff_loaded), &nvramloaded_fp->f_pos);
#endif
	set_fs(fs);

	filp_close(nvramloaded_fp, NULL);
}

#endif /* WLC_LOW */
