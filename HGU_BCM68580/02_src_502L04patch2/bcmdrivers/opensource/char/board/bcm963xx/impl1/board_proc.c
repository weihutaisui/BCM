/*
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
*/


#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/ctype.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mtd/mtd.h>

#include <bcmtypes.h>
#include <bcm_map_part.h>
#include <board.h>
#include <boardparms.h>

#include "board_proc.h"
#include "board_image.h"
#include "board_wl.h"
#include "board_wd.h"

#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848) || defined(CONFIG_BCM96858)
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
extern int proc_show_rdp_mem(char *buf, char **start, off_t off, int cnt, int *eof, void *data);
#else
int proc_show_rdp_mem( struct file *file, char __user *buf, size_t len, loff_t *pos);
#endif
#endif

#if defined (WIRELESS)
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
static int proc_get_wl_nandmanufacture(char *page, char **start, off_t off, int cnt, int *eof, void *data);
#ifdef BUILD_NAND
static int proc_get_wl_mtdname(char *page, char **start, off_t off, int cnt, int *eof, void *data);
#endif
#else
static ssize_t proc_get_wl_nandmanufacture(struct file * file, char * buff, size_t len, loff_t *offset);
#ifdef BUILD_NAND
static ssize_t proc_get_wl_mtdname(struct file * file, char * buff, size_t len, loff_t *offset);
#endif
#endif
#endif

static void str_to_num(char* in, char *out, int len);
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
static ssize_t proc_get_param(char *page, char **start, off_t off, int cnt, int *eof, void *data);
static ssize_t proc_get_param_string(char *page, char **start, off_t off, int cnt, int *eof, void *data);
static ssize_t proc_set_param(struct file *f, const char *buf, unsigned long cnt, void *data);
static ssize_t proc_set_led(struct file *f, const char *buf, unsigned long cnt, void *data);
#else
static ssize_t proc_get_param(struct file *, char *, size_t, loff_t *);
static ssize_t proc_get_param_string(struct file *, char *, size_t, loff_t *);
static ssize_t proc_set_param(struct file *, const char *, size_t, loff_t *);
static ssize_t proc_set_led(struct file *, const char *, size_t, loff_t *);
#endif

#if defined(CONFIG_BCM_WATCHDOG_TIMER)
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
static ssize_t proc_get_watchdog(char *page, char **start, off_t off, int cnt, int *eof, void *data);
static ssize_t proc_set_watchdog(struct file *f, const char *buf, unsigned long cnt, void *data);
#else
static ssize_t proc_get_watchdog(struct file *, char *, size_t, loff_t *);
static ssize_t proc_set_watchdog(struct file *, const char *, size_t, loff_t *);
#endif
#endif /* defined(CONFIG_BCM_WATCHDOG_TIMER) */

int add_proc_files(void);
int del_proc_files(void);

static ssize_t __proc_get_boardid(char *buf, int cnt)
{
    char boardid[NVRAM_BOARD_ID_STRING_LEN];
    kerSysNvRamGetBoardId(boardid);
    sprintf(buf, "%s", boardid);
    return strlen(boardid);
}
static ssize_t __proc_get_socinfo(char *buf, int cnt)
{
    char socname[15] = {0};
    int i;
    int n=0;

    kerSysGetChipName( socname, strlen(socname));

    for( i=0; i< strlen(socname); i++ )
    {
        if(socname[i] == '_')
        {
            socname[i]='\0';
            break;
        }
    }
            
    n += sprintf(buf,   "SoC Name        :BCM%s\n", socname);
    n += sprintf(buf+n, "Revision        :%s\n", &socname[i+1]);

    return n;
}
static ssize_t __proc_get_wan_type(char *buf)
{
    int n = 0;

    unsigned int wan_type = 0, t;
    int i, j, len = 0;

    BpGetOpticalWan(&wan_type);
    if (wan_type == BP_NOT_DEFINED)
    {
        n=sprintf(buf, "none");
        return n;
    }

    for (i = 0, j = 0; wan_type; i++)
    {
        t = wan_type & (1 << i);
        if (!t)
            continue;

        wan_type &= ~(1 << i);
        if (j++)
        {
            sprintf(buf + len, "\n");
            len++;
        }

        switch (t)
        {
        case BP_OPTICAL_WAN_GPON:
            n+=sprintf(buf + len, "gpon");
            break;
        case BP_OPTICAL_WAN_EPON:
            n+=sprintf(buf + len, "epon");
            break;
        case BP_OPTICAL_WAN_AE:
            n+=sprintf(buf + len, "ae");
            break;
        default:
            n+=sprintf(buf + len, "unknown");
            break;
        }
        len += n;
    }

    return len;
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
static ssize_t proc_get_wan_type(char *buf, char **start, off_t off, int cnt, int *eof, void *data)
{
    *eof=1;
    return __proc_get_wan_type(buf);
}
static ssize_t proc_get_boardid(char *buf, char **start, off_t off, int cnt, int *eof, void *data)
{
    *eof=1;
    return __proc_get_boardid(buf, cnt);
}
#else
static ssize_t proc_get_boardid( struct file *file,
                                       char __user *buf,
                                       size_t len, loff_t *pos)
{
    int ret=0;
    if(*pos == 0)
    {
       *pos=__proc_get_boardid(buf, len);
           if(likely(*pos != 0)) //something went wrong
        ret=*pos;
    }
    return ret;
}

static ssize_t proc_get_socinfo( struct file *file,
                                       char __user *buf,
                                       size_t len, loff_t *pos)
{
    int ret=0;
    if(*pos == 0)
    {
       *pos=__proc_get_socinfo(buf, len);
           if(likely(*pos != 0)) //something went wrong
        ret=*pos;
    }
    return ret;
}
static ssize_t proc_get_wan_type( struct file *file,
                                       char __user *buf,
                                       size_t len, loff_t *pos)
{
    int ret=0;
    if(*pos == 0)
    {
       *pos=__proc_get_wan_type(buf);
       if(likely(*pos != 0)) //something went wrong
           ret=*pos;
    }
    return ret;
}
#endif

// Use this ONLY to convert strings of bytes to strings of chars
// use functions from linux/kernel.h for everything else
static void str_to_num(char* in, char* out, int len)
{
    int i;
    memset(out, 0, len);

    for (i = 0; i < len * 2; i ++)
    {
        if ((*in >= '0') && (*in <= '9'))
            *out += (*in - '0');
        else if ((*in >= 'a') && (*in <= 'f'))
            *out += (*in - 'a') + 10;
        else if ((*in >= 'A') && (*in <= 'F'))
            *out += (*in - 'A') + 10;
        else
            *out += 0;

        if ((i % 2) == 0)
            *out *= 16;
        else
            out ++;

        in ++;
    }
    return;
}

static ssize_t __proc_set_param(struct file *f, const char *buf, unsigned long cnt, void *data)
{
    NVRAM_DATA *pNvramData;
    char input[32];

    int i = 0;
    int r = cnt;
    int offset  = ((int *)data)[0];
    int length  = ((int *)data)[1];

    if ((offset < 0) || (offset + length > sizeof(NVRAM_DATA)))
        return 0;

    if ((cnt > 32) || (copy_from_user(input, buf, cnt) != 0))
        return -EFAULT;

    for (i = 0; i < r; i ++)
    {
        if (!isxdigit(input[i]))
        {
            memmove(&input[i], &input[i + 1], r - i - 1);
            r --;
            i --;
        }
    }

    if (NULL != (pNvramData = readNvramData()))
    {
        str_to_num(input, ((char *)pNvramData) + offset, length);
        writeNvramData(pNvramData);
    }
    else
    {
        cnt = 0;
    }


    if (pNvramData)
        kfree(pNvramData);

    return cnt;
}
static ssize_t __proc_get_param(char *page, int cnt, void *data)
{
    int i = 0;
    int r = 0;
    int offset  = ((int *)data)[0];
    int length  = ((int *)data)[1];
    NVRAM_DATA *pNvramData;

    if ((offset < 0) || (offset + length > sizeof(NVRAM_DATA)))
        return 0;

    if (NULL != (pNvramData = readNvramData()))
    {
        for (i = 0; i < length; i ++)
            r += sprintf(page + r, "%02x ", ((unsigned char *)pNvramData)[offset + i]);
    }

    r += sprintf(page + r, "\n");
    if (pNvramData)
        kfree(pNvramData);
    return (r < cnt)? r: 0;


}
static ssize_t __proc_get_param_string(char *page, int cnt, void *data)
{
    int r = 0;
    int offset  = (long)data;
    NVRAM_DATA *pNvramData;

    if ((offset < 0) || (offset > sizeof(NVRAM_DATA)))
        return 0;

    if (NULL != (pNvramData = readNvramData()))
    {
        r += snprintf(page + r, cnt, "%s\n", (char *)pNvramData + offset);
    }

    if (pNvramData)
        kfree(pNvramData);
    return (r < cnt)? r: 0;
}
static ssize_t __proc_set_led(struct file *f, const char *buf, unsigned long cnt, void *data)
{
    // char leddata[16];
    unsigned int leddata;
    char input[32];
    int i;

    if (cnt > 31)
        cnt = 31;

    if (copy_from_user(input, buf, cnt) != 0)
        return -EFAULT;


    for (i = 0; i < cnt; i ++)
    {
        if (!isxdigit(input[i]))
        {
            input[i] = 0;
        }
    }
    input[i] = 0;

    if (0 != kstrtouint(input, 16, &leddata)) 
        return -EFAULT;

    kerSysLedCtrl ((leddata & 0xff00) >> 8, leddata & 0xff);
    return cnt;
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
static ssize_t proc_get_param(char *page, char **start, off_t off, int cnt, int *eof, void *data)
{
    *eof = 1;
    return __proc_get_param(page, cnt, data); 

}

static ssize_t proc_get_param_string(char *page, char **start, off_t off, int cnt, int *eof, void *data)
{
    *eof = 1;
    return __proc_get_param_string(page, cnt, data); 

}


static ssize_t proc_set_param(struct file *f, const char *buf, unsigned long cnt, void *data)
{
    return __proc_set_param(f,buf,cnt,data);
}

/*
 * This function expect input in the form of:
 * echo "xxyy" > /proc/led
 * where xx is hex for the led number
 * and   yy is hex for the led state.
 * For example,
 *     echo "0301" > led
 * will turn on led 3
 */
static ssize_t proc_set_led(struct file *f, const char *buf, unsigned long cnt, void *data)
{
    return __proc_set_led(f, buf, cnt, data);
}
#else
static ssize_t proc_get_param(struct file * file, char * buff, size_t len, loff_t *offset)
{

    int ret=0;
    if(*offset == 0)
    {
        *offset =__proc_get_param(buff, len, PDE_DATA(file_inode(file))); 
        if(likely(*offset != 0)) //something went wrong
            ret=*offset;
    }
    return ret;
}
static ssize_t proc_get_param_string(struct file * file, char * buff, size_t len, loff_t *offset)
{

    int ret=0;
    if(*offset == 0)
    {
        *offset =__proc_get_param_string(buff, len, PDE_DATA(file_inode(file))); 
        if(likely(*offset != 0)) //something went wrong
            ret=*offset;
    }
    return ret;
}
static ssize_t proc_set_param(struct file *file, const char *buff, size_t len, loff_t *offset)
{
    return __proc_set_param(file,buff,len,PDE_DATA(file_inode(file)));
}
static ssize_t proc_set_led(struct file *file, const char *buff, size_t len, loff_t *offset)
{
    int ret=-1;
    if(*offset == 0)
    {
        *offset=__proc_set_led(file, buff, len, PDE_DATA(file_inode(file)));
        if(likely(*offset != 0)) //something went wrong
            ret=*offset;
    }    
    return ret;

}
#endif

#if defined(CONFIG_BCM_WATCHDOG_TIMER)
static ssize_t __proc_get_watchdog(char *page, int cnt, void *data)
{
    int r = 0;

    r += sprintf(page + r, "watchdog enabled=%u timer=%u us suspend=%u\n", 
                           watchdog_data.enabled, 
                           watchdog_data.timer, 
                           watchdog_data.suspend);
    r += sprintf(page + r, "         userMode=%u userThreshold=%u userTimeout=%u\n", 
                           watchdog_data.userMode, 
                           watchdog_data.userThreshold/2, 
                           watchdog_data.userTimeout/2);
    return (r < cnt)? r: 0;
}

static ssize_t __proc_set_watchdog(struct file *f, const char *buf, unsigned long cnt, void *data)
{
    char input[64];
    unsigned int enabled, timer;
    unsigned int userMode, userThreshold;
   
    if (cnt > 64) 
    {
        cnt = 64;
    }

    if (copy_from_user(input, buf, cnt) != 0) 
    {
        return -EFAULT;
    }

    if (strncasecmp(input, "OK", 2) == 0)
    {
        bcm_reset_watchdog();
        return cnt;
    }

    if (sscanf(input, "%u %u %u %u", &enabled, &timer, &userMode, &userThreshold) != 4)
    {
        printk("\nError format, it is as:\n");
        printk("\"enabled(0|1) timer(us) userMode(0|1) userThreshold\"\n");
        return -EFAULT;
    }

    bcm_set_watchdog(enabled, timer, userMode, userThreshold);

    return cnt;
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
static ssize_t proc_get_watchdog(char *page, char **start, off_t off, int cnt, int *eof, void *data)
{
    *eof = 1;
    return __proc_get_watchdog(page, cnt, data);
}

static ssize_t proc_set_watchdog(struct file *f, const char *buf, unsigned long cnt, void *data)
{
    return __proc_set_watchdog(f, buf, cnt, data);
}
#else
static ssize_t proc_get_watchdog(struct file *file, char *buff, size_t len, loff_t *offset)
{

    if(*offset != 0)
        return 0;
    *offset = __proc_get_watchdog(buff, len, PDE_DATA(file_inode(file)));

    return *offset;

}
static ssize_t proc_set_watchdog (struct file *file, const char *buff, size_t len, loff_t *offset)
{
    int ret=-1;

    if(*offset == 0)
    {
       *offset=__proc_set_watchdog(file, buff, len, PDE_DATA(file_inode(file)));
       if(likely(*offset != 0)) //something went wrong
          ret=*offset;
    }
return ret;
}
#endif
#endif


#if defined(WIRELESS)
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
static int proc_get_wl_nandmanufacture(char *page, char **start, off_t off, int cnt, int *eof, void *data)
{
    int r = 0;
    r += sprintf(page + r, "%d", _get_wl_nandmanufacture());
    return (r < cnt)? r: 0;
}

#ifdef BUILD_NAND
static int proc_get_wl_mtdname(char *page, char **start, off_t off, int cnt, int *eof, void *data)
{
    int r=0;
    struct mtd_info *mtd = get_mtd_device_nm(WLAN_MFG_PARTITION_NAME);
    if( !IS_ERR_OR_NULL(mtd) )
            r += sprintf(page + r, "mtd%d",mtd->index );
    return (r < cnt)? r: 0;
}
#endif

#else
/*  for higher version 4.1 kernel */
static ssize_t proc_get_wl_nandmanufacture(struct file * file, char * buff, size_t len, loff_t *pos)
{
    ssize_t ret=0;
    if(*pos == 0)
    {
        (*pos) = sprintf(buff, "%d", _get_wl_nandmanufacture());
        if(likely(*pos != 0)) 
            ret=*pos;
    }
    return ret;
}

#ifdef BUILD_NAND
static ssize_t proc_get_wl_mtdname(struct file * file, char * buff, size_t len, loff_t *pos)
{
    ssize_t ret=0;
    if(*pos == 0)
    {
        struct mtd_info *mtd = get_mtd_device_nm(WLAN_MFG_PARTITION_NAME);
        if( !IS_ERR_OR_NULL(mtd) ) {
           (*pos) = sprintf(buff, "mtd%d",mtd->index );
           if(likely(*pos != 0)) 
               ret=*pos;
         }
    }
    return ret;
}
#endif
#endif
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
#else
#ifdef WIRELESS
   static struct file_operations wl_get_nandmanufacture_proc = {
       .read=proc_get_wl_nandmanufacture,
       .write=NULL,
    };
#ifdef BUILD_NAND
   static struct file_operations wl_get_mtdname_proc = {
       .read=proc_get_wl_mtdname,
       .write=NULL,
    };
#endif
#endif
   static struct file_operations base_mac_add_proc = {
       .read=proc_get_param,
       .write=proc_set_param,
    };
   static struct file_operations bootline_proc = {
       .read=proc_get_param_string,
       .write=NULL,
    };
    static struct file_operations led_proc = {
       .write=proc_set_led,
    };
#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848)
    static struct file_operations rdp_mem_proc = {
       .read=proc_show_rdp_mem,
    };
#endif
    static struct file_operations supp_optical_wan_types_proc = {
       .read=proc_get_wan_type,
    };
#if defined(CONFIG_BCM_WATCHDOG_TIMER)
    static struct file_operations watchdog_proc = {
       .read=proc_get_watchdog,
       .write=proc_set_watchdog,
    };
#endif
    static struct file_operations boardid_proc = {
       .read=proc_get_boardid,
    };
    static struct file_operations socinfo_proc = {
       .read=proc_get_socinfo,
    };
#endif

int add_proc_files(void)
{
#define offset(type, elem) ((size_t)&((type *)0)->elem)

    static int BaseMacAddr[2] = {offset(NVRAM_DATA, ucaBaseMacAddr), NVRAM_MAC_ADDRESS_LEN};

    struct proc_dir_entry *p0;
    struct proc_dir_entry *p1;
    struct proc_dir_entry *p2;
#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848)
    struct proc_dir_entry *p3;
#endif
    struct proc_dir_entry *p4;
    struct proc_dir_entry *p5;

    p0 = proc_mkdir("nvram", NULL);

    if (p0 == NULL)
    {
        printk("add_proc_files: failed to create proc files!\n");
        return -1;
    }

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11)
#if defined(WIRELESS)
    p1 = create_proc_entry("wl_nand_manufacturer", 0444, p0);
    if (p1 == NULL)
    {
        printk("add_proc_files: failed to create proc files!\n");
        return -1;
    }
    p1->read_proc   = proc_get_wl_nandmanufacture;
    p1->write_proc =NULL;

#ifdef BUILD_NAND

    p1 = create_proc_entry("wl_nand_mtdname", 0444, p0);
    if (p1 == NULL)
    {
        printk("add_proc_files: failed to create proc files!\n");
        return -1;
    }
    p1->read_proc   = proc_get_wl_mtdname;
    p1->write_proc =NULL;
#endif
#endif
    p1 = create_proc_entry("BaseMacAddr", 0644, p0);

    if (p1 == NULL)
    {
        printk("add_proc_files: failed to create proc files!\n");
        return -1;
    }

    p1->data        = BaseMacAddr;
    p1->read_proc   = proc_get_param;
    p1->write_proc  = proc_set_param;

    p1 = create_proc_entry("bootline", 0644, p0);

    if (p1 == NULL)
    {
        printk("add_proc_files: failed to create proc files!\n");
        return -1;
    }

    p1->data        = (void *)offset(NVRAM_DATA, szBootline);
    p1->read_proc   = proc_get_param_string;
    p1->write_proc  = NULL;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    //New linux no longer requires proc_dir_entry->owner field.
#else
    p1->owner       = THIS_MODULE;
#endif

    p1 = create_proc_entry("led", 0644, NULL);
    if (p1 == NULL)
        return -1;

    p1->write_proc  = proc_set_led;

#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848)
    p3 = create_proc_entry("show_rdp_mem", 0444, NULL);
    if (p3 == NULL)
        return -1;

    p3->read_proc  = proc_show_rdp_mem;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    //New linux no longer requires proc_dir_entry->owner field.
#else
    p1->owner       = THIS_MODULE;
#endif

    p2 = create_proc_entry("supported_optical_wan_types", 0444, p0);
    if (p2 == NULL)
        return -1;
    p2->read_proc = proc_get_wan_type;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    //New linux no longer requires proc_dir_entry->owner field.
#else
    p2->owner       = THIS_MODULE;
#endif

#if defined(CONFIG_BCM_WATCHDOG_TIMER)
    p2 = create_proc_entry("watchdog", 0644, NULL);
    if (p2 == NULL)
    {
        printk("add_proc_files: failed to create watchdog proc file!\n");
        return -1;
    }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    //New linux no longer requires proc_dir_entry->owner field.
#else
    p2->owner       = THIS_MODULE;
#endif

    p2->data        = NULL;
    p2->read_proc   = proc_get_watchdog;
    p2->write_proc  = proc_set_watchdog;
#endif /* defined(CONFIG_BCM_WATCHDOG_TIMER) */

    p4 = create_proc_entry("boardid", 0444, p0);
    if (p4 == NULL)
        return -1;
    p4->read_proc = proc_get_boardid;
#else /* LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,11) */

#if defined(WIRELESS)
    p1 = proc_create("wl_nand_manufacturer", S_IRUSR, p0,&wl_get_nandmanufacture_proc);
    if (p1 == NULL)
    {
        printk("add_proc_files: failed to create proc files!\n");
        return -1;
    }

#ifdef BUILD_NAND

    p1 = proc_create("wl_nand_mtdname", S_IRUSR, p0,&wl_get_mtdname_proc);
    if (p1 == NULL)
    {
        printk("add_proc_files: failed to create proc files!\n");
        return -1;
    }
#endif
#endif
     p1 = proc_create_data("BaseMacAddr", S_IRUSR, p0, &base_mac_add_proc, BaseMacAddr);

    if (p1 == NULL)
    {
        printk("add_proc_files: failed to create proc files!\n");
        return -1;
    }

     p1 = proc_create_data("bootline", S_IRUSR, p0, &bootline_proc, (void *)offset(NVRAM_DATA, szBootline));

    if (p1 == NULL)
    {
        printk("add_proc_files: failed to create proc files!\n");
        return -1;
    }

    p1 = proc_create("led", S_IWUSR | S_IRUSR, NULL, &led_proc);
    if (p1 == NULL)
        return -1;

#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848) 
    p3 = proc_create("show_rdp_mem", S_IRUSR, p0, &rdp_mem_proc);
    if (p3 == NULL)
        return -1;
#endif

    p2 = proc_create("supported_optical_wan_types", S_IRUSR, p0, &supp_optical_wan_types_proc);
    if (p2 == NULL)
        return -1;

#if defined(CONFIG_BCM_WATCHDOG_TIMER)
    p2 = proc_create("watchdog", S_IRUSR|S_IWUSR, p0, &watchdog_proc);
    if (p2 == NULL)
    {
        printk("add_proc_files: failed to create watchdog proc file!\n");
        return -1;
    }
#endif /* defined(CONFIG_BCM_WATCHDOG_TIMER) */

    p4 = proc_create("boardid", S_IRUSR, p0, &boardid_proc);
    if (p4 == NULL)
        return -1;

    p5 = proc_create("socinfo", S_IRUSR, NULL, &socinfo_proc);
    if (p5 == NULL)
        return -1;
#endif

    return 0;
}

int del_proc_files(void)
{
    remove_proc_entry("nvram", NULL);
    remove_proc_entry("led", NULL);

    return 0;
}
