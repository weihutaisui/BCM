/*
<:copyright-BRCM:2007:proprietary:standard

   Copyright (c) 2007 Broadcom 
   All Rights Reserved

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
*/
/*
 *******************************************************************************
 * File Name  : fcachedrv.c
 *
 * Description: This file contains Linux character device driver entry points
 *              for the Flow Cache Driver.
 *******************************************************************************
 */

/*----- Includes -----*/

#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/notifier.h>
#include <linux/bcm_log.h>
#include <linux/vmalloc.h>
#include "fcache.h"
#include "flwstats.h"
#if defined(CONFIG_BCM_FHW)
#include "fcachehw.h"
#endif
//#include "pktCmf.h"

/*----- Defines -----*/

/* Override global debug and/or assert compilation for Driver layer */
#define CLRsys              CLRy
#define DBGsys              "FCACHE:DRV"
#if !defined(CC_CONFIG_FCACHE_DRV_DBGLVL)
#undef PKT_DBG_SUPPORTED
#endif
#if defined(PKT_DBG_SUPPORTED)
static int pktDbgLvl = CC_CONFIG_FCACHE_DRV_DBGLVL;
#endif
#if !defined(CC_FCACHE_DRV_ASSERT)
#undef PKT_ASSERT_SUPPORTED
#endif
#if defined(CC_CONFIG_FCACHE_COLOR)
#define PKT_DBG_COLOR_SUPPORTED
#endif
#include "pktDbg.h"
#if defined(CC_CONFIG_FCACHE_DEBUG)    /* Runtime debug level setting */
int fcacheDrvDebug(int lvl) { dbg_config( lvl ); return lvl; }
#endif

#if defined(CONFIG_BCM_FHW)
extern uint32_t fhw_is_hw_cap_enabled(uint32_t cap_mask);
extern int  fhw_construct(void);
extern void fhw_destruct(void);
#endif

/*----- Exported callbacks from fcache.c ONLY to be invoked by fcachedrv */
extern void fcache_slice(unsigned long data, uint32_t slice_ix);
extern void fcache_interval(uint32_t intv);
extern int  fcache_print_get_next(enumFcacheDrvProcType eProcType, int index);
extern int  fcache_print_by_idx(char * p, enumFcacheDrvProcType eProcType, int index);
extern int  fcache_host_dev_mac_print(char *p, int *index_p, int *linesbudget_p);
extern int fcache_error_stats_print(char *p, int *index_p, int *linesbudget_p);
extern int fcache_evict_stats_print(char *p, int *index_p, int *linesbudget_p);
extern int fcache_notify_evt_stats_print(char *p, int *index_p, 
    int *linesbudget_p);
extern int fcache_query_evt_stats_print(char *p, int *index_p, 
    int *linesbudget_p);
extern int fcache_slow_path_stats_print(char *p, int *index_p, 
    int *linesbudget_p);
extern int  fcache_max_ent(void);
extern int  fcache_set_max_ent(int maxFlowEntries);

extern int  fcache_fdb_print(char * p, int * index_p, int * linesbudget_p);
extern int  fcache_fdb_max_ent(void);
extern int  fcache_set_fdb_max_ent(int maxFdbEntries);

extern int fcache_flush_flow(int flowid);
extern void fcache_dump_flow_info(uint32_t flowid);
extern int  fcache_reset_stats(void);
extern int  fcache_construct(void);
extern void fcache_destruct(void);
extern int  flwStatsInit(void);

extern int flwStatsGetQueryNum(void);
extern int flwStatsDumpToStruct(FlwStatsDumpInfo_t *flwStDumpInfo_p, FlwStatsDumpEntryInfo_t *flwStDumpEntry_p);
extern int flwStatsCreateQuery(FlwStatsQueryInfo_t *newQuery);
extern int flwStatsGetQuery(FlwStatsQueryInfo_t *newQuery);
extern int flwStatsDeleteQuery(FlwStatsQueryInfo_t *newQuery);
extern int flwStatsClearQuery(FlwStatsQueryInfo_t *newQuery);

/*----- Forward declarations -----*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0)
static int  fcacheDrvIoctl(struct inode *inode, struct file *filep,
                           unsigned int command, unsigned long arg);
#else
static long fcacheDrvIoctl(struct file *filep, unsigned int command, 
                           unsigned long arg);
#endif

static int  fcacheDrvOpen(struct inode *inode, struct file *filp);

static void fcacheDrvTimer(unsigned long data);

static int  fcacheDrvNetDevNotifier(struct notifier_block *this,
                                    unsigned long event, void *ptr);

static ssize_t  fcacheDrvHostDevMacProcfs(struct file *file, char __user *page, 
        size_t len, loff_t *offset);

#if defined(CC_CONFIG_FCACHE_DBGLVL)
static ssize_t fcacheDrvFdbProcfs(struct file *file, char __user *page, 
        size_t len, loff_t *offset);
#endif

static ssize_t fcacheDrvErrorStatsProcfs(struct file *file, char __user *page,
        size_t len, loff_t *offset);

static ssize_t fcacheDrvEvictStatsProcfs(struct file *file, char __user *page,
        size_t len, loff_t *offset);

static ssize_t fcacheDrvNotifyEvtStatsProcfs(struct file *file, char __user *page, 
        size_t len, loff_t *offset);

static ssize_t fcacheDrvQueryEvtStatsProcfs(struct file *file, char __user *page, 
        size_t len, loff_t *offset);

static ssize_t fcacheDrvSlowPathStatsProcfs(struct file *file, char __user *page,
        size_t len, loff_t *offset);

/*----- Globals -----*/
typedef struct {
    uint32_t slice_ix;          /* slice number processed */
    uint32_t num_slices;        /* total number of slice timers */
    uint32_t slice_period;      /* timer period of each slice timer */

    struct timer_list timer;

    struct file_operations fops;
    struct notifier_block netdev_notifier;

    int     interval;           /* Timer interval in jiffies */
    int     proc_fs_created;    /* Singleton proc file system initialization */
    int     index;              /* Index */
    int     fdb_index;          /* Index into fdblist */
} __attribute__((aligned(16))) FcacheDrv_t;


static FcacheDrv_t fcacheDrv_g = {
    .fops = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0)
        .ioctl          = fcacheDrvIoctl,
#else
        .unlocked_ioctl = fcacheDrvIoctl,
#if defined(CONFIG_COMPAT)
        .compat_ioctl = fcacheDrvIoctl,
#endif
#endif
        .open           = fcacheDrvOpen
    },

    .netdev_notifier = {
        .notifier_call  = fcacheDrvNetDevNotifier,
    },

    .interval           = FCACHE_REFRESH_INTERVAL,
    .proc_fs_created    = 0,
    .index              = 0,
    .fdb_index          = FDB_IX_INVALID,
};


#undef FCACHE_DECL
#define FCACHE_DECL(x) #x,

const char * fcacheDrvIoctlName[] =
{
    FCACHE_DECL(FCACHE_IOCTL_STATUS)
    FCACHE_DECL(FCACHE_IOCTL_ENABLE)
    FCACHE_DECL(FCACHE_IOCTL_UNUSED) /* FIXME: This value does not work */
    FCACHE_DECL(FCACHE_IOCTL_DISABLE)
    FCACHE_DECL(FCACHE_IOCTL_FLUSH)
    FCACHE_DECL(FCACHE_IOCTL_DEFER)
    FCACHE_DECL(FCACHE_IOCTL_MCAST)
    FCACHE_DECL(FCACHE_IOCTL_IPV6)
    FCACHE_DECL(FCACHE_IOCTL_RESET_STATS)
    FCACHE_DECL(FCACHE_IOCTL_MONITOR)
    FCACHE_DECL(FCACHE_IOCTL_TIMER)
    FCACHE_DECL(FCACHE_IOCTL_CREATE_FLWSTATS)
    FCACHE_DECL(FCACHE_IOCTL_GET_FLWSTATS)
    FCACHE_DECL(FCACHE_IOCTL_DELETE_FLWSTATS)
    FCACHE_DECL(FCACHE_IOCTL_CLEAR_FLWSTATS)
    FCACHE_DECL(FCACHE_IOCTL_GET_FLWSTATS_NUM)
    FCACHE_DECL(FCACHE_IOCTL_DUMP_FLWSTATS)
    FCACHE_DECL(FCACHE_IOCTL_GRE)
    FCACHE_DECL(FCACHE_IOCTL_DEBUG)
    FCACHE_DECL(FCACHE_IOCTL_MCAST_LEARN)
    FCACHE_DECL(FCACHE_IOCTL_ACCEL_MODE)
    FCACHE_DECL(FCACHE_IOCTL_DUMP_FLOW_INFO)
    FCACHE_DECL(FCACHE_IOCTL_INVALID)
};

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheInterval
 * Description  : Exported fcache API to reset the timeout interval
 *------------------------------------------------------------------------------
 */
int fcacheInterval(int interval)
{
    unsigned long newtime;

    if ( interval == -1 )
       return fcacheDrv_g.interval;

    if ( interval < FCACHE_REFRESH_MIN_INTERVAL )
    {
       fc_error( "invalid timer value (less than allowed)");
       return FCACHE_ERROR;
    }

    fcacheDrv_g.interval = msecs_to_jiffies(interval); /* convert msec to jiffies */
    fcache_interval( jiffies_to_msecs(fcacheDrv_g.interval) / 1000 );
    fcacheDrv_g.slice_period = (fcacheDrv_g.interval/fcacheDrv_g.num_slices);

    newtime = jiffies + fcacheDrv_g.slice_period;
    mod_timer( &fcacheDrv_g.timer, newtime );        /* kick start timer */
    return fcacheDrv_g.interval;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheUpdateXtmStats
 * Description  : helper function for fcache.o.  fcache.o cannot have
 *                any #ifdef CONFIG_xxx so this function must be moved
 *                out to fachedrv.c.
 *------------------------------------------------------------------------------
 */
extern void bcmxtmrt_update_hw_stats(Blog_t *blog_p,
                                     unsigned int hits, unsigned int octets);

void fcacheUpdateXtmStats( Blog_t * blog_p, uint32_t prev_hits,
                                            uint32_t prev_octets )
{
#if defined(CONFIG_BCM_XTMRT) || defined(CONFIG_BCM_XTMRT_MODULE)
    bcmxtmrt_update_hw_stats(blog_p, prev_hits, prev_octets);
#endif
}

#define BLOG_LOCK_REQUIRED(cmd) \
((cmd != FCACHE_IOCTL_GET_FLWSTATS_NUM) && \
 (cmd != FCACHE_IOCTL_DUMP_FLWSTATS) && \
 (cmd != FCACHE_IOCTL_CREATE_FLWSTATS) && \
 (cmd != FCACHE_IOCTL_GET_FLWSTATS) && \
 (cmd != FCACHE_IOCTL_DELETE_FLWSTATS) && \
 (cmd != FCACHE_IOCTL_CLEAR_FLWSTATS))

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvIoctl
 * Description  : Main entry point to handle user applications IOCTL requests
 *                Flow Cache Utility.
 * Returns      : 0 - success or error
 *------------------------------------------------------------------------------
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0)
static int fcacheDrvIoctl(struct inode *inode, struct file *filep,
                          unsigned int command, unsigned long arg)
#else
static long fcacheDrvIoctl(struct file *filep, unsigned int command, 
                           unsigned long arg)
#endif
{
    FcacheIoctl_t cmd;
    int ret = FCACHE_SUCCESS;

    if ( command > FCACHE_IOCTL_INVALID )
        cmd = FCACHE_IOCTL_INVALID;
    else
        cmd = (FcacheIoctl_t)command;

    dbgl_print( DBG_EXTIF, "cmd<%d> %s arg<%lu>",
                command, fcacheDrvIoctlName[cmd], arg );

    /* protect the fc linked lists by disabling all interrupts */

    if (BLOG_LOCK_REQUIRED(cmd))
    {
        BLOG_LOCK_BH();
    }

    switch ( cmd )
    {
        case FCACHE_IOCTL_STATUS :
        {
            FcStatusInfo_t fcStatusInfo;
			
            /* Copy statusInfo structure from user space to kernel space */
            copy_from_user((void*) &fcStatusInfo, (void*) arg, sizeof(FcStatusInfo_t));

            fcStatusInfo.interval = jiffies_to_msecs(fcacheDrv_g.interval);
			
            /* Get statusInfo from fc */
            fc_status_ioctl(&fcStatusInfo);
			
            /* Copy structure back to user space */
            copy_to_user((void*) arg, (void*) &fcStatusInfo, sizeof(FcStatusInfo_t));	
	  
            break;
        }

        case FCACHE_IOCTL_ENABLE :
        {
            fc_bind_blog( 1 );
            break;
        }

        case FCACHE_IOCTL_DISABLE :
        {
            fc_bind_blog( 0 );
            break;
        }

        case FCACHE_IOCTL_FLUSH:
        {
            FcFlushParams_t fcFlushParams;
            int count;

            /* Copy flushParams structure from user space to kernel space */
            copy_from_user((void*) &fcFlushParams, (void*) arg, sizeof(FcFlushParams_t));

            count = fc_flush_params(&fcFlushParams);

            dbgl_print( DBG_EXTIF, "Flow Cache flushed %d items\n", count );

            break;
        }

        case FCACHE_IOCTL_DEFER:
        {
#if defined(CC_CONFIG_FCACHE_DEFER)
            ret = fcacheDefer( (int) arg );
            dbgl_print( DBG_EXTIF,
                 "Packet CMF Hw flow activates after %d hits in Sw Fcache\n",
                 ret );
#else
            ret = FCACHE_ERROR;
#endif
            break;
        }

        case FCACHE_IOCTL_MCAST:
        {
            blog_support_mcast( arg );
            break;
        }

        case FCACHE_IOCTL_MCAST_LEARN:
        {
            blog_support_mcast_learn( arg );
            break;
        }

        case FCACHE_IOCTL_ACCEL_MODE:
        {
            blog_support_accel_mode( arg );

#if defined(CONFIG_BCM_FHW)
            if (arg == BLOG_ACCEL_MODE_L23)
            {
                uint32_t cap_mask = (1<<HW_CAP_L2_UCAST);

                if (!fhw_is_hw_cap_enabled(cap_mask))
                    printk( CLRbold2 "WARNING: No HW acceleration support for L2 flows" CLRnl );
            }
#endif
            break;
        }

        case FCACHE_IOCTL_IPV6:
        {
            blog_support_ipv6( arg );
            break;
        }

        case FCACHE_IOCTL_RESET_STATS:
        {
            ret = fcache_reset_stats();
            break;
        }

        case FCACHE_IOCTL_MONITOR:
        {
            ret = fcacheMonitor( (arg) ? 1 : 0 );
            break;
        }

        case FCACHE_IOCTL_TIMER:
        {
            ret = fcacheInterval( (int) arg );
            break;
        }

        case FCACHE_IOCTL_GET_FLWSTATS_NUM:
        {
            /* get queries entry number */
            ret = flwStatsGetQueryNum();

            break;
        }

        case FCACHE_IOCTL_DUMP_FLWSTATS:
        {
            FlwStatsDumpInfo_t flwStDumpInfo;
            FlwStatsDumpEntryInfo_t *flwStDumpEntry;

            /* Copy dump structure from user space to kernel space */
            copy_from_user((void*) &flwStDumpInfo, (void *) arg, sizeof(FlwStatsDumpInfo_t));

            if(is_compat_task())
            {
                BCM_IOC_PTR_ZERO_EXT(flwStDumpInfo.FlwStDumpEntry);
            }

            flwStDumpEntry = kmalloc((sizeof(FlwStatsDumpEntryInfo_t)*(flwStDumpInfo.num_entries)), GFP_KERNEL);

            if(flwStDumpEntry)
            {
                /* fill dump info structure */
                ret = flwStatsDumpToStruct(&flwStDumpInfo, flwStDumpEntry);

                if(ret == 0)/* Copy structure back to user space */		
                {
                    copy_to_user((void*) arg, (void*) &flwStDumpInfo, sizeof(FlwStatsDumpInfo_t));	
                    copy_to_user((void *)(flwStDumpInfo.FlwStDumpEntry), (void *)flwStDumpEntry, (sizeof(FlwStatsDumpEntryInfo_t)*(flwStDumpInfo.num_entries)));
                }
                kfree(flwStDumpEntry);
            }
            else
            {
                ret = -2;
            }
            break;
        }

        case FCACHE_IOCTL_CREATE_FLWSTATS:
        {
            FlwStatsQueryInfo_t query;

            /* Copy query structure from user space to kernel space */
            copy_from_user((void*) &query, (void*) arg,
                sizeof(FlwStatsQueryInfo_t));

            /* Create query */
            ret = flwStatsCreateQuery(&query);

            /* Copy structure back to user space */
            copy_to_user((void*) arg, (void*) &query,
                sizeof(FlwStatsQueryInfo_t));	
            break;
        }

        case FCACHE_IOCTL_GET_FLWSTATS:
        {
            FlwStatsQueryInfo_t queryinfo;

            /* Copy query structure from user space to kernel space */
            copy_from_user((void*) &queryinfo, (void*) arg,
                sizeof(FlwStatsQueryInfo_t));

            /* Run query */
            ret = flwStatsGetQuery(&queryinfo);

            /* Copy structure back to user space */
            copy_to_user((void*) arg, (void*) &queryinfo,
                sizeof(FlwStatsQueryInfo_t));	
            break;
        }

        case FCACHE_IOCTL_DELETE_FLWSTATS:
        {
            FlwStatsQueryInfo_t queryinfo;

            /* Copy query structure from user space to kernel space */
            copy_from_user((void*) &queryinfo, (void*) arg,
                sizeof(FlwStatsQueryInfo_t));

            /* Run delete. No need to copy back structure - we only 
               return an error code */
            ret = flwStatsDeleteQuery(&queryinfo);
            break;
        }

        case FCACHE_IOCTL_CLEAR_FLWSTATS:
        {
            FlwStatsQueryInfo_t queryinfo;

            /* Copy query structure from user space to kernel space */
            copy_from_user((void*) &queryinfo, (void*) arg,
                sizeof(FlwStatsQueryInfo_t));

            /* Clear counters. No need to copy back structure - we only 
               return an error code */
            ret = flwStatsClearQuery(&queryinfo);
            break;
        }

        case FCACHE_IOCTL_GRE:
        {
            blog_support_gre( arg );
            break;
        }
        case FCACHE_IOCTL_L2TP:
        {
            blog_support_l2tp( arg );
            break;
        }
	
        case FCACHE_IOCTL_DEBUG :
        {
#if defined(CC_CONFIG_FCACHE_DEBUG)
            int layer = (arg>>8) & 0xFF;
            int level = arg & 0xFF;

            switch ( layer )
            {
                case FCACHE_DBG_DRV_LAYER: 
                    ret = fcacheDrvDebug( level );
                    break;

                case FCACHE_DBG_FC_LAYER:  
                    ret = fcacheDebug( level );
                    break;
#if defined(CONFIG_BCM_FHW)
                case FCACHE_DBG_FHW_LAYER:  
                    ret = fcacheFhwDebug( level );
                    break;
#endif                    

                default: 
                    ret = FCACHE_ERROR;
            }
#else
            fc_error( "CC_CONFIG_FCACHE_DEBUG not defined");
            ret = FCACHE_ERROR;
#endif
            break;
        }

        case FCACHE_IOCTL_DUMP_FLOW_INFO:
        {
            fcache_dump_flow_info( arg );
            break;
        }

        default :
        {
            fc_error( "Invalid cmd[%u]", command );
            ret = FCACHE_ERROR;
        }
    }

    if (BLOG_LOCK_REQUIRED(cmd))
    {
        BLOG_UNLOCK_BH();
    }

    return ret;

} /* fcacheDrvIoctl */

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvOpen
 * Description  : Called when a user application opens this device.
 * Returns      : 0 - success
 *------------------------------------------------------------------------------
 */
int fcacheDrvOpen(struct inode *inode, struct file *filp)
{
    dbgl_print( DBG_EXTIF, "Access Flow Cache Char Device" );
    return FCACHE_SUCCESS;
} /* fcacheDrvOpen */


/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvTimer
 * Description  : Periodic slice timer callback. Passes timeout to fcache.o
 * Flow Cache timer interval is divided into a number of slice intervals.
 * The distribution of fcache entries to a slice interval is based on slice
 * mask, which is number of slices minus 1. The number of slices is max 
 * flow cache entries divided by number of entries served in one slice timer.
 *------------------------------------------------------------------------------
 */
void fcacheDrvTimer(unsigned long data)
{
    BLOG_LOCK_BH();

    fcacheDrv_g.timer.data++;
    fcacheDrv_g.timer.expires = jiffies + fcacheDrv_g.slice_period;
    add_timer( &fcacheDrv_g.timer );

    fcache_slice(fcacheDrv_g.timer.data, fcacheDrv_g.slice_ix);

    fcacheDrv_g.slice_ix++;
    if (fcacheDrv_g.slice_ix >= fcacheDrv_g.num_slices)
        fcacheDrv_g.slice_ix = 0;

    BLOG_UNLOCK_BH();
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvNetDevNotifier
 * Description  : Receive notifications of link state changes and device down
 *                and forward them to fcache.o via blog_notify()
 *------------------------------------------------------------------------------
 */
int fcacheDrvNetDevNotifier(struct notifier_block *this,
                            unsigned long event, void *dev_ptr)
{
    struct net_device *dev_p = NETDEV_NOTIFIER_GET_DEV(dev_ptr);
    char *dev_addr;
    dev_addr = (char *) blog_request( NETDEV_ADDR, dev_p, 0, 0 );
    dbgl_print( DBG_INTIF, "dev<%s> dev_p<0x%08x> event<%lu>", 
            ((struct net_device *) dev_p)->name, (unsigned int)dev_p, event); 

    blog_lock();
    switch (event) {
        case NETDEV_UP:
            blog_notify( UP_NETDEVICE, dev_p, (unsigned long) dev_addr, 0 );
            break;

        case NETDEV_CHANGE:
            if ( blog_request( LINK_NOCARRIER, dev_p, 0, 0) )
                blog_notify( DN_NETDEVICE, dev_p, 0, 0 );
            else
                blog_notify( UP_NETDEVICE, dev_p, (unsigned long) dev_addr, 0 );
            break;

        case NETDEV_DOWN:
            blog_notify( DN_NETDEVICE, dev_p, 0, 0 );
            break;

        case NETDEV_GOING_DOWN:
            blog_notify( DESTROY_NETDEVICE, dev_p, 0, 0 );
            break;

        case NETDEV_CHANGEMTU:
            blog_notify( UPDATE_NETDEVICE, dev_p, 0, 0 );
            break;

        case NETDEV_CHANGEADDR:
            blog_notify( CHANGE_ADDR, dev_p, (unsigned long) dev_addr, 0 );
            break;
       
        default:
            break;
    }
    blog_unlock();

    return NOTIFY_DONE;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvHostDevMacProcfs
 * Description  : Handler to list Host Device and MAC address into the ProcFs
 *------------------------------------------------------------------------------
 */
static inline ssize_t fcacheDrvHostDevMacProcfs(struct file *file, char __user *page,
								  size_t len, loff_t *offset)
{
    int index = 0;
    int linesbudget = 64;
    int bytes = 0;

    /*TODO: fix this fucntion - if number of mac's is more than linebudget
     * this does not work
     */

    BLOG_LOCK_BH();

    // MOD_INC_USE_COUNT;

    if ( *offset == 0 ) 
        bytes = fcache_host_dev_mac_print( page, &index, &linesbudget );

    // MOD_DEC_USE_COUNT;

    BLOG_UNLOCK_BH();

    return bytes;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvFdbProcFs
 * Description  : Handler to list active fdbs into the ProcFs
 *------------------------------------------------------------------------------
 */
ssize_t  fcacheDrvFdbProcfs(struct file *file, char __user *page,
                        size_t len, loff_t *offset)
{
    int index;
    int linesbudget = 5;
    int bytes = 0;

    BLOG_LOCK_BH();

    // MOD_INC_USE_COUNT;

    /* fetch last index at which stopped */
    index = fcacheDrv_g.fdb_index;

    if ( *offset == 0 ) 
        index = FLOW_IX_INVALID;

    if ( index < fcache_fdb_max_ent())
        bytes = fcache_fdb_print( page, &index, &linesbudget );

    /* save current index at which we stopped due to lines budget exhaust */
    fcacheDrv_g.fdb_index = index;

	// MOD_DEC_USE_COUNT;

    BLOG_UNLOCK_BH();

    return bytes;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvSlowPathStatsProcfs
 * Description  : Handler to print the slow path stats
 *------------------------------------------------------------------------------
 */
static ssize_t fcacheDrvSlowPathStatsProcfs(struct file *file, char __user *page,
        size_t len, loff_t *offset)
{
    int index=0;
    int linesbudget = 10;
    int bytes = 0;

    BLOG_LOCK_BH();

    // MOD_INC_USE_COUNT;

    if ( *offset == 0 ) 
        bytes = fcache_slow_path_stats_print( page, &index, &linesbudget );

    // MOD_DEC_USE_COUNT;

    BLOG_UNLOCK_BH();

    return bytes;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvNotifyEvtStatsProcfs
 * Description  : Handler to print the notify event stats
 *------------------------------------------------------------------------------
 */
static ssize_t fcacheDrvNotifyEvtStatsProcfs(struct file *file, char __user *page, 
        size_t len, loff_t *offset)
{
    int index=0;
    int linesbudget = 10;
    int bytes = 0;

    BLOG_LOCK_BH();

    // MOD_INC_USE_COUNT;

    if ( *offset == 0 ) 
        bytes = fcache_notify_evt_stats_print( page, &index, &linesbudget );

    // MOD_DEC_USE_COUNT;

    BLOG_UNLOCK_BH();

    return bytes;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvQueryEvtStatsProcfs
 * Description  : Handler to print the query event stats
 *------------------------------------------------------------------------------
 */
static ssize_t fcacheDrvQueryEvtStatsProcfs(struct file *file, char __user *page, 
        size_t len, loff_t *offset)
{
    int index=0;
    int linesbudget = 10;
    int bytes = 0;

    BLOG_LOCK_BH();

    // MOD_INC_USE_COUNT;

    if ( *offset == 0 ) 
        bytes = fcache_query_evt_stats_print( page, &index, &linesbudget );

    // MOD_DEC_USE_COUNT;

    BLOG_UNLOCK_BH();

    return bytes;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvEvictStatsProcfs
 * Description  : Handler to print the flow evict stats
 *------------------------------------------------------------------------------
 */
static ssize_t fcacheDrvEvictStatsProcfs(struct file *file, char __user *page,
        size_t len, loff_t *offset)
{
    int index=0;
    int linesbudget = 10;
    int bytes = 0;

    BLOG_LOCK_BH();

    // MOD_INC_USE_COUNT;

    if ( *offset == 0 ) 
        bytes = fcache_evict_stats_print( page, &index, &linesbudget );

    // MOD_DEC_USE_COUNT;

    BLOG_UNLOCK_BH();

    return bytes;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvErrorStatsProcfs
 * Description  : Handler to print the errors stats
 *------------------------------------------------------------------------------
 */
static ssize_t fcacheDrvErrorStatsProcfs(struct file *file, char __user *page,
        size_t len, loff_t *offset)
{
    int index=0;
    int linesbudget = 10;
    int bytes = 0;

    BLOG_LOCK_BH();

    // MOD_INC_USE_COUNT;

    if ( *offset == 0 ) 
        bytes = fcache_error_stats_print( page, &index, &linesbudget );

    // MOD_DEC_USE_COUNT;

    BLOG_UNLOCK_BH();

    return bytes;
}


static void *fcacheDrv_seq_start(struct seq_file *s, loff_t *pos)
{
    static int flwIdx=0;

    if (*pos == 0) 
    {
       flwIdx = FLOW_IX_INVALID;
    }

    if (flwIdx >= fcache_max_ent()) 
    {
        return NULL;
    }

    return &flwIdx;
}

static void *fcacheDrv_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    struct file *file = (struct file *)(s->private);
    enumFcacheDrvProcType eProcType = (enumFcacheDrvProcType)PDE_DATA(file_inode(file));
    int *index = (int *)v;

    (*index) = fcache_print_get_next(eProcType, *index);

    (*pos)++;

    if ((*index) >= fcache_max_ent()) 
    {
       return NULL;
    }
    return v;
}

static void fcacheDrv_seq_stop(struct seq_file *s, void *v)
{
    return;
}

static int fcacheDrv_seq_show(struct seq_file *s, void *v)
{
    struct file *file = (struct file *)(s->private);
    enumFcacheDrvProcType eProcType = (enumFcacheDrvProcType)PDE_DATA(file_inode(file));
    int *index = (int *)v;
    char buffer[900]={0};
    int bytes = 0;

    BLOG_LOCK_BH();

    bytes = fcache_print_by_idx(buffer, eProcType, *index);

    BLOG_UNLOCK_BH();

    if (bytes) 
    {
       seq_puts(s, buffer);
    }

    return 0;
}


static struct seq_operations fcacheDrv_seq_ops = {
    .start = fcacheDrv_seq_start,
    .next  = fcacheDrv_seq_next,
    .stop  = fcacheDrv_seq_stop,
    .show  = fcacheDrv_seq_show,
};

static int fcacheDrv_procfs_open(struct inode *inode, struct file *file)
{
    struct seq_file *s;
    int ret;

    if ((ret = seq_open(file, &fcacheDrv_seq_ops)) >= 0)
    {
        s = file->private_data;
        s->private = file;
    }
    return ret;
}

static struct file_operations fcacheDrvProcfs_proc = {
        .open = fcacheDrv_procfs_open,
        .read = seq_read,
        .llseek = seq_lseek,
        .release = seq_release,
};

static struct file_operations fcacheDrvHostDevMacProcfs_proc = {
        .read = fcacheDrvHostDevMacProcfs,
};

static struct file_operations fcacheDrvFdbProcfs_proc = {
        .read = fcacheDrvFdbProcfs,
};

static struct file_operations fcacheDrvErrorStatsProcfs_proc = {
        .read = fcacheDrvErrorStatsProcfs,
};
static struct file_operations fcacheDrvEvictStatsProcfs_proc = {
        .read = fcacheDrvEvictStatsProcfs,
};
static struct file_operations fcacheDrvNotifyEvtStatsProcfs_proc = {
        .read = fcacheDrvNotifyEvtStatsProcfs,
};
static struct file_operations fcacheDrvQueryEvtStatsProcfs_proc = {
        .read = fcacheDrvQueryEvtStatsProcfs,
};
static struct file_operations fcacheDrvSlowPathStatsProcfs_proc = {
        .read = fcacheDrvSlowPathStatsProcfs,
};




/*
 *------------------------------------------------------------------------------
 * Function Name: pktflow_construct
 * Description  : Initial function that is called at system startup that
 *                registers this device.
 * Returns      : Error or FCACHE_DRV_MAJOR
 *------------------------------------------------------------------------------
 */

static int __init pktflow_construct(void)
{
    int deferral;
    int maxFlowEntries;
    int maxFdbEntries;
    int hwFlowMonitor;  /* Monitor and age-out HW Accelerator flows */

    dbg_config( CC_CONFIG_FCACHE_DRV_DBGLVL );

    dbgl_func( DBG_BASIC );

    /* Flow cache acceleration mode */
#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    blog_support_accel_mode( BLOG_ACCEL_MODE_L23 );
#else
    blog_support_accel_mode( BLOG_ACCEL_MODE_L3 );
#endif

    /* Should be power of 2 */

/* Sets how many SW FC flows are supported. Used to dynamically allocate SW FC
 * DDR according to the number of flows. 
 */  
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
#ifdef CONFIG_BRCM_MINIGW
    maxFlowEntries = 8192;
#else
#ifdef XRDP
    maxFlowEntries = 16384;
#else
    maxFlowEntries = 16384;
#endif
#endif
    maxFdbEntries = 256;
    hwFlowMonitor = 0;
#else
    maxFlowEntries = 1024;
    maxFdbEntries = 128;
    hwFlowMonitor = 1;
#endif

    if ((maxFlowEntries > FCACHE_MAX_ENTRIES) || 
        (maxFdbEntries > FCACHE_FDB_MAX_ENTRIES))
    {
        print( CLRerr "Invalid values either maxFlowEntries<%d> "
                      "or maxFdbEntries<%d>" CLRnl,
                      maxFlowEntries, maxFdbEntries);
        return FCACHE_ERROR;
    }

    fcache_set_max_ent(maxFlowEntries);
    fcache_set_fdb_max_ent(maxFdbEntries);

    /*
     * ========================
     * Initialize fcache state
     * ========================
     */
    if ( fcache_construct() == FCACHE_ERROR )
        return FCACHE_ERROR;

    fcacheMonitor(hwFlowMonitor);

    /* Register a character device for Ioctl handling */
    if ( register_chrdev(FCACHE_DRV_MAJOR, FCACHE_DRV_NAME, &fcacheDrv_g.fops) )
    {
        print( CLRerr "%s Unable to get major number <%d>" CLRnl,
                  __FUNCTION__, FCACHE_DRV_MAJOR);
        return FCACHE_ERROR;
    }

    print( CLRbold PKTFLOW_MODNAME " Char Driver " PKTFLOW_VER_STR
                   " Registered<%d>" CLRnl, FCACHE_DRV_MAJOR );

#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
    deferral = 1;
#else
    deferral = FCACHE_REACTIVATE;
#endif

    fcacheDefer(deferral);
    fcache_interval( jiffies_to_msecs(fcacheDrv_g.interval) / 1000 );

    /* Start a periodic OS refresh timer  */
    init_timer( &fcacheDrv_g.timer );       /* initialize periodic timer */

    fcacheDrv_g.slice_ix = 0;
    fcacheDrv_g.num_slices = fcache_max_ent()/FCACHE_SLICE_MAX_ENT_COUNT;
    fcacheDrv_g.slice_period = (fcacheDrv_g.interval/fcacheDrv_g.num_slices);

    fcacheDrv_g.timer.expires = jiffies + fcacheDrv_g.slice_period;
    fcacheDrv_g.timer.function = fcacheDrvTimer;
    fcacheDrv_g.timer.data = (unsigned long)0;

    add_timer( &fcacheDrv_g.timer );        /* kick start timer */

#if defined(CC_CONFIG_FCACHE_PROCFS)
    /*
     * Save returned value of proc_mkdir() create_proc_read_entry() and set
     * owner field of struct proc_dir_entry, to support dynamic unloading of
     * pktcmf module. See MOD_INC_USE_COUNT/MOD_DEC_USE_COUNT in each read_proc
     * handler function, to avoid unload while userspace is accessing proc file.
     */
    if ( fcacheDrv_g.proc_fs_created == 0 )
    {
        struct proc_dir_entry *entry;
        proc_mkdir( FCACHE_PROCFS_DIR_PATH, NULL );
        proc_mkdir( FCACHE_MISC_PROCFS_DIR_PATH, NULL );

        entry = proc_create_data(FCACHE_PROCFS_DIR_PATH "/nflist", 
            S_IRUGO, NULL, &fcacheDrvProcfs_proc, (void*)FCACHE_DRV_PROC_TYPE_NF);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for nflist" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_PROCFS_DIR_PATH "/brlist", 
            S_IRUGO, NULL, &fcacheDrvProcfs_proc, (void*)FCACHE_DRV_PROC_TYPE_BR);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for brlist" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_PROCFS_DIR_PATH "/l2list", 
            S_IRUGO, NULL, &fcacheDrvProcfs_proc, (void*)FCACHE_DRV_PROC_TYPE_L2);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for l2list" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_MISC_PROCFS_DIR_PATH "/mcastlist", 
            S_IRUGO, NULL, &fcacheDrvProcfs_proc, (void*)FCACHE_DRV_PROC_TYPE_MCAST);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for mcastlist" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_MISC_PROCFS_DIR_PATH "/mcastdnatlist", 
            S_IRUGO, NULL, &fcacheDrvProcfs_proc, (void*)FCACHE_DRV_PROC_TYPE_MDNAT);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for mcastdnatlist" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_MISC_PROCFS_DIR_PATH "/rtpseqlist", S_IRUGO,
            NULL, &fcacheDrvProcfs_proc, (void*)FCACHE_DRV_PROC_TYPE_RTP_SEQ);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for rtpseqlist" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_MISC_PROCFS_DIR_PATH "/host_dev_mac", S_IRUGO,
            NULL, &fcacheDrvHostDevMacProcfs_proc, (void*)NULL);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for host_dev_mac" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_MISC_PROCFS_DIR_PATH "/fdblist", S_IRUGO,
            NULL, &fcacheDrvFdbProcfs_proc, NULL);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for fdblist" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        proc_mkdir( FCACHE_STATS_PROCFS_DIR_PATH, NULL );

        entry = proc_create_data(FCACHE_STATS_PROCFS_DIR_PATH "/errors",
            S_IRUGO, NULL, &fcacheDrvErrorStatsProcfs_proc,
            (void*)&fcacheDrv_g.index);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for errors" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_STATS_PROCFS_DIR_PATH "/evict",
            S_IRUGO, NULL, &fcacheDrvEvictStatsProcfs_proc,
            (void*)&fcacheDrv_g.index);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for evict" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_STATS_PROCFS_DIR_PATH "/notfiy",
            S_IRUGO, NULL, &fcacheDrvNotifyEvtStatsProcfs_proc,
            (void*)&fcacheDrv_g.index);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for notify" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_STATS_PROCFS_DIR_PATH "/query",
            S_IRUGO, NULL, &fcacheDrvQueryEvtStatsProcfs_proc,
            (void*)&fcacheDrv_g.index);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for query" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        entry = proc_create_data(FCACHE_STATS_PROCFS_DIR_PATH "/slow_path",
            S_IRUGO, NULL, &fcacheDrvSlowPathStatsProcfs_proc,
            (void*)&fcacheDrv_g.index);
        if (!entry)
        {
           print( CLRerr "%s Unable to create proc entry for slow path" CLRnl,
                 __FUNCTION__);
           return FCACHE_ERROR;
        }

        fcacheDrv_g.proc_fs_created = 1;
        printk( "Created Proc FS /procfs/" FCACHE_PROCFS_DIR_PATH "\n");
    }
#endif

    /* Register handler for network device notifications */
    register_netdevice_notifier( &fcacheDrv_g.netdev_notifier );
    printk( CLRbold PKTFLOW_MODNAME "registered with netdev chain" CLRnl );


    /* Make fcache a Blog Client: invokes bind_blog() */
#if !defined(CC_CONFIG_FCACHE_BLOG_MANUAL)
    fc_bind_blog( 1 );              /* Blogging enabled on module loading */
    printk( CLRbold PKTFLOW_MODNAME "learning via BLOG enabled." CLRnl );
#else
    printk( CLRbold2 "WARNING: Flow Cache not bound to Blog" CLRnl );
#endif

#if defined(CONFIG_BCM_FHW)
    fhw_construct();
#endif

    /* Create fcStatsThread and Initialize */
    flwStatsInit();

    printk( CLRbold "Constructed " PKTFLOW_MODNAME PKTFLOW_VER_STR CLRnl );

    return 0;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: pktflow_destruct
 * Description  : Initial function that is called at system startup that
 *                registers this device.
 * Returns      : None
 *
 * CAUTION      : MODULE UNLOADING ONLY FOR LAB !!!
 *
 *------------------------------------------------------------------------------
 */
void pktflow_destruct(void)
{
    dbgl_func( DBG_BASIC );

    fc_bind_blog( 0 );              /* Blogging disabled on module unloading */
    printk( CLRbold PKTFLOW_MODNAME "learning via BLOG disabled." CLRnl );


    del_timer( &fcacheDrv_g.timer );/* Delete timer */

#if defined(CONFIG_BCM_FHW)
    fhw_destruct();
#endif

    fcache_destruct();              /* fcache.o: reset all flow state */

    /* Un register for network device notifications */
    unregister_netdevice_notifier( &fcacheDrv_g.netdev_notifier );
    printk( CLRbold2 PKTFLOW_MODNAME "unregistered with netdev chain" CLRnl );

#if defined(CC_CONFIG_FCACHE_PROCFS)
    /* Delete proc filesystem entries */
    if ( fcacheDrv_g.proc_fs_created == 1 )
    {
        remove_proc_entry( FCACHE_STATS_PROCFS_DIR_PATH "/slow_path", NULL );
        remove_proc_entry( FCACHE_STATS_PROCFS_DIR_PATH "/query", NULL );
        remove_proc_entry( FCACHE_STATS_PROCFS_DIR_PATH "/notify", NULL );
        remove_proc_entry( FCACHE_STATS_PROCFS_DIR_PATH "/evict", NULL );
        remove_proc_entry( FCACHE_STATS_PROCFS_DIR_PATH "/errors", NULL );
        remove_proc_entry( FCACHE_STATS_PROCFS_DIR_PATH, NULL );

        remove_proc_entry( FCACHE_MISC_PROCFS_DIR_PATH "/fdblist", NULL );
        remove_proc_entry( FCACHE_MISC_PROCFS_DIR_PATH "/host_dev_mac", NULL );
        remove_proc_entry( FCACHE_MISC_PROCFS_DIR_PATH "/rtpseqlist", NULL );
        remove_proc_entry( FCACHE_MISC_PROCFS_DIR_PATH "/mcastdnatlist", NULL );
        remove_proc_entry( FCACHE_MISC_PROCFS_DIR_PATH "/mcastlist", NULL );
        remove_proc_entry( FCACHE_MISC_PROCFS_DIR_PATH , NULL );

        remove_proc_entry( FCACHE_PROCFS_DIR_PATH "/l2list", NULL );
        remove_proc_entry( FCACHE_PROCFS_DIR_PATH "/nflist", NULL );
        remove_proc_entry( FCACHE_PROCFS_DIR_PATH "/brlist", NULL );
        remove_proc_entry( FCACHE_PROCFS_DIR_PATH, NULL );
        printk( "Deleted Proc FS /procfs/" FCACHE_PROCFS_DIR_PATH "\n");
        fcacheDrv_g.proc_fs_created = 0;
    }
#endif

    /* Un register character device */
    unregister_chrdev( FCACHE_DRV_MAJOR, FCACHE_DRV_NAME );

    printk( CLRbold2 PKTFLOW_MODNAME " Char Driver " PKTFLOW_VER_STR
                    " Unregistered<%d>" CLRnl, FCACHE_DRV_MAJOR );

    printk( CLRbold2 "Destructed " PKTFLOW_MODNAME PKTFLOW_VER_STR CLRnl );
    /*----- OK, Safe to unload now -----*/
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fcacheDrvFlushAll
 * Description  : flushes all the dynamic flows.
 * Returns      : 0 - success or error
 *------------------------------------------------------------------------------
 */
int fcacheDrvFlushAll( void )
{
    int ret = FCACHE_SUCCESS;

    ret = fc_flush();

    return ret;
}



/*
 *------------------------------------------------------------------------------
 * Function Name: fc_kmalloc
 * Description  : wrapper function for kmalloc, when successfully allocating
 *                memory, the memory should be cache line aligned.
 *                There are configurations in the kernel that can provide
 *                cache aligned allocation.
 *------------------------------------------------------------------------------
 */
void *fc_kmalloc(size_t size, unsigned int flags)
{
    gfp_t kflags;

    if(flags == FCACHE_ALLOC_TYPE_ATOMIC)
    {
        kflags = GFP_ATOMIC;
    }
    else if(flags == FCACHE_ALLOC_TYPE_KERNEL)
    {
        kflags = GFP_KERNEL;
    }
    else
    {
        print( CLRerr "%s Unknown alloc flags <%u>" CLRnl,
                  __FUNCTION__, flags);
        return NULL;
    }

    return vmalloc(size);
}

void fc_kfree(const void *p)
{
    vfree(p);
}


/*
 *------------------------------------------------------------------------------
 * Function Name: fc_skb_clone
 * Description  : wrapper function for skb_clone
 *------------------------------------------------------------------------------
 */
struct sk_buff *fc_skb_clone(struct sk_buff *skb, unsigned int flags)
{
    gfp_t kflags;

    if(flags == FCACHE_ALLOC_TYPE_ATOMIC)
    {
        kflags = GFP_ATOMIC;
    }
    else if(flags == FCACHE_ALLOC_TYPE_KERNEL)
    {
        kflags = GFP_KERNEL;
    }
    else
    {
        print( CLRerr "%s Unknown alloc flags <%u>" CLRnl,
                  __FUNCTION__, flags);
        return NULL;
    }
    
    return skb_clone(skb, kflags);
}
/*
 *------------------------------------------------------------------------------
 * Function Name: fc_xlate_fkb_to_skb
 * Description  : wrapper function to translate fkb to skb 
 *------------------------------------------------------------------------------
 */
struct sk_buff * fc_xlate_fkb_to_skb(struct fkbuff * fkb_p, uint8_t *dirty_p)
{
	return (skb_xlate_dp(fkb_p, dirty_p));
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fc_skb_frag_xmit4
 * Description  : fragmnet and xmit an ipv4 packet
 *------------------------------------------------------------------------------
 */
void fc_skb_frag_xmit4(struct sk_buff *origskb, struct net_device *txdev,
                     uint32_t is_pppoe, uint32_t minMtu, void *ipp)
{
    return skb_frag_xmit4(origskb, txdev, is_pppoe, minMtu, ipp);
}

/*
 *------------------------------------------------------------------------------
 * Function Name: fc_skb_frag_xmit6
 * Description  : fragmnet and xmit an ipv6 packet
 *------------------------------------------------------------------------------
 */
void fc_skb_frag_xmit6(struct sk_buff *origskb, struct net_device *txdev,
                     uint32_t is_pppoe, uint32_t minMtu, void *ipp)
{
    return skb_frag_xmit6(origskb, txdev, is_pppoe, minMtu, ipp);
}

EXPORT_SYMBOL(fcacheInterval);      /* Symbol exported by fcacheDrv.o */
EXPORT_SYMBOL(fcacheDrvFlushAll);

module_init(pktflow_construct);
module_exit(pktflow_destruct);

MODULE_DESCRIPTION(PKTFLOW_MODNAME);
MODULE_VERSION(PKTFLOW_VERSION);

MODULE_LICENSE("Proprietary");
