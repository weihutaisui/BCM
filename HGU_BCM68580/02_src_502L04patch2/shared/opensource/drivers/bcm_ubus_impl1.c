/*
 * <:copyright-BRCM:2017:DUAL/GPL:standard
 * 
 *    Copyright (c) 2017 Broadcom 
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

/* BCM UBUS4 supporting routines */

#include "bcm_map_part.h"
#include "bcm_ubus4.h"
#ifdef _CFE_
#include "lib_types.h"
#include "lib_printf.h"
#define printk  printf
#define udelay  cfe_usleep
#else // Linux
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/delay.h>
#include "board.h"
#endif

//#define ENABLE_UBUS_REMAP_DEBUG_LOG     
#ifdef  ENABLE_UBUS_REMAP_DEBUG_LOG
#define UBUS_REMAP_DEBUG_LOG(fmt, ...) printk("%s:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define UBUS_REMAP_DEBUG_LOG(fmt, ...)  
#endif

#ifdef CONFIG_BCM_UBUS_DECODE_REMAP
#define DECODE_WIN0 0
#define DECODE_WIN1 1
#define CACHE_BIT_OFF 0
#define CACHE_BIT_ON 1
#endif

extern unsigned long getMemorySize(void);
#define MST_START_DDR_ADDR              0

#ifdef CONFIG_BCM_UBUS_DECODE_REMAP
#if !defined(CONFIG_BCM963158)
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "proc_cmd.h"


#define PROC_DIR            "driver/ubus"
#define UBUS_PROC_FILE      "ubus_cmd"
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *cmd_proc_file;
static int ubus_proc_cmd_remap_master_all(int argc, char *argv[]);
static int ubus_proc_cmd_remap_master_x(int argc, char *argv[]);
static int ubus_proc_cmd_remap_master_reset_all(int argc, char *argv[]);

static struct proc_cmd_ops ubus_remap_command_entries[] = 
{
    { .name = "remap_master_x",         .do_command	= ubus_proc_cmd_remap_master_x },
    { .name = "remap_master_all",       .do_command	= ubus_proc_cmd_remap_master_all},
    { .name = "remap_master_reset_all", .do_command	= ubus_proc_cmd_remap_master_reset_all},
};

static struct proc_cmd_table ubus_remap_command_table = 
{
    .module_name = "ubus_masters",
    .size = sizeof(ubus_remap_command_entries) / sizeof(ubus_remap_command_entries[0]),
    .ops = ubus_remap_command_entries
};
#endif /* !CONFIG_BCM963158 */
unsigned int g_board_size_power_of_2;
EXPORT_SYMBOL(g_board_size_power_of_2);
#endif /* CONFIG_BCM_UBUS_DECODE_REMAP */

/*ubus4 credit table */
#if defined(CONFIG_BCM963158)
     /* only includes the non default credit value, default is 4 in 63158 */
static ubus_credit_cfg_t ubus_credit_tbl[MST_PORT_NODE_LAST][UBUS_MAX_PORT_NUM+1] = {
    { {UBUS_PORT_ID_MEMC, 3}, {UBUS_PORT_ID_SYS, 1}, {UBUS_PORT_ID_PSRAM, 8}, {UBUS_PORT_ID_SYSXRDP, 1}, {-1,-1} },  //MST_PORT_NODE_B53
    { {UBUS_PORT_ID_BIU, 3}, {UBUS_PORT_ID_SYS, 1}, {-1,-1} },                                                       //MST_PORT_NODE_USB
    { {UBUS_PORT_ID_BIU, 5}, {UBUS_PORT_ID_PCIE0, 1}, {UBUS_PORT_ID_PER, 1}, {UBUS_PORT_ID_SYS, 1},                 //MST_PORT_NODE_PCIE0
      {UBUS_PORT_ID_FPM, 1}, {UBUS_PORT_ID_VPB, 1}, {UBUS_PORT_ID_SYSXRDP, 1}, {-1,-1} }, 
    { {UBUS_PORT_ID_BIU, 6}, {UBUS_PORT_ID_SYS, 1}, {UBUS_PORT_ID_FPM, 1}, {UBUS_PORT_ID_VPB, 1}, {-1,-1} },        //MST_PORT_NODE_PCIE3
    { {UBUS_PORT_ID_BIU, 4}, {UBUS_PORT_ID_SYS, 1}, {UBUS_PORT_ID_FPM, 1}, {UBUS_PORT_ID_VPB, 1}, {-1,-1} },        //MST_PORT_NODE_PCIE2
    { {UBUS_PORT_ID_BIU, 1}, {UBUS_PORT_ID_MEMC, 1}, {UBUS_PORT_ID_USB, 1}, {UBUS_PORT_ID_PCIE0, 1},                //MST_PORT_NODE_PMC
      {UBUS_PORT_ID_PCIE3, 1}, {UBUS_PORT_ID_PCIE2, 1}, {UBUS_PORT_ID_PER, 1}, {UBUS_PORT_ID_PMC, 1}, 
      {UBUS_PORT_ID_WAN, 1}, {UBUS_PORT_ID_SYS, 1}, {UBUS_PORT_ID_SWH, 1}, {UBUS_PORT_ID_SPU, 1}, 
      {UBUS_PORT_ID_DSL, 1}, {UBUS_PORT_ID_QM, 1}, {UBUS_PORT_ID_FPM, 1}, {UBUS_PORT_ID_VPB, 1}, 
      {UBUS_PORT_ID_PSRAM, 1}, {UBUS_PORT_ID_SYSXRDP, 1}, {-1,-1} },
    { {UBUS_PORT_ID_BIU, 1}, {UBUS_PORT_ID_SYS, 1}, {-1,-1} },                                                      //MST_PORT_NODE_PER
    { {UBUS_PORT_ID_BIU, 4}, {UBUS_PORT_ID_PER, 1}, {UBUS_PORT_ID_SYS, 1}, {UBUS_PORT_ID_DSL, 4},                   //MST_PORT_NODE_PERDMA
      {UBUS_PORT_ID_PSRAM, 8}, {-1,-1} },
    { {UBUS_PORT_ID_MEMC, 8}, {UBUS_PORT_ID_PER, 1}, {UBUS_PORT_ID_WAN, 1}, {UBUS_PORT_ID_SYS, 1},                  //MST_PORT_NODE_DSLCPU
      {UBUS_PORT_ID_DSL, 1}, {-1,-1} },
    { {UBUS_PORT_ID_BIU, 1}, {UBUS_PORT_ID_PER, 1}, {UBUS_PORT_ID_WAN, 1}, {UBUS_PORT_ID_SYS, 1},                   //MST_PORT_NODE_DSL
      {UBUS_PORT_ID_DSL, 1}, {-1,-1} },
    { {UBUS_PORT_ID_BIU, 4}, {UBUS_PORT_ID_PER, 1}, {UBUS_PORT_ID_SYS, 1}, {UBUS_PORT_ID_DSL, 4},                   //MST_PORT_NODE_SPU
      {UBUS_PORT_ID_PSRAM, 8}, {-1,-1} },
    { {UBUS_PORT_ID_BIU, 8}, {UBUS_PORT_ID_SYSXRDP, 1}, {-1,-1} },                                                  //MST_PORT_NODE_QM
    { {UBUS_PORT_ID_BIU, 4}, {UBUS_PORT_ID_FPM, 2}, {UBUS_PORT_ID_SYSXRDP, 1}, {-1,-1} },                           //MST_PORT_NODE_DQM
    { {UBUS_PORT_ID_BIU, 8}, {UBUS_PORT_ID_SYSXRDP, 1}, {-1,-1} },                                                  //MST_PORT_NODE_NATC
    { {UBUS_PORT_ID_BIU, 9}, {UBUS_PORT_ID_SYSXRDP, 1}, {-1,-1} },                                                  //MST_PORT_NODE_DMA0
    { {UBUS_PORT_ID_BIU, 6}, {UBUS_PORT_ID_USB, 1}, {UBUS_PORT_ID_PCIE0, 2}, {UBUS_PORT_ID_PCIE3, 2},               //MST_PORT_NODE_RQ0
      {UBUS_PORT_ID_PCIE2, 2}, {UBUS_PORT_ID_PER, 1}, {UBUS_PORT_ID_WAN, 1}, {UBUS_PORT_ID_SWH, 1}, 
      {UBUS_PORT_ID_SPU, 1}, {UBUS_PORT_ID_QM, 10}, {UBUS_PORT_ID_FPM, 2}, {UBUS_PORT_ID_VPB, 2}, 
      {UBUS_PORT_ID_PSRAM, 10}, {UBUS_PORT_ID_SYSXRDP, 1}, {-1,-1} },
    { {UBUS_PORT_ID_BIU, 8}, {UBUS_PORT_ID_SYS, 1}, {UBUS_PORT_ID_DSL, 8}, {UBUS_PORT_ID_PSRAM, 8}, {-1,-1} },      //MST_PORT_NODE_SWH
    { {-1,-1} },                                                                                                    //MST_PORT_NODE_DMA1
};
#endif //#if defined(CONFIG_BCM963158)

// XXX  Dima To check
#if defined (CONFIG_BCM96858) || defined(_BCM96858_) || defined(CONFIG_BCM96836) || defined(_BCM96836_) || defined(CONFIG_BCM96846) || defined(_BCM96846_) || defined(CONFIG_BCM963158) || defined(_BCM963158_) || defined(CONFIG_BCM96856) || defined(_BCM96856_)
int log2_32 (unsigned int value)
{
    unsigned int result = 0;
    if( value < 1)
        return 0;

    while (value > 1) {
        ++result;
        value >>= 1;
    }

    return result;
}
#ifndef _CFE_
EXPORT_SYMBOL(log2_32);
#endif
// XXX  Dima To check
int ubus_master_decode_wnd_cfg(MST_PORT_NODE node, int win, unsigned int phys_addr, unsigned int size_power_of_2, int port_id, unsigned int cache_bit_en)
{
    MstPortNode *master_addr = NULL; 
    int ret = 0;

    UBUS_REMAP_DEBUG_LOG("\x1b[35m node[%d] win[%d] phys_addr[0x%x] power_of_2[%d] port_id[%d] cache_bit[%d]\x1b[0m\n", 
                         node, win, phys_addr, size_power_of_2, port_id,cache_bit_en);


    if((win > 3) || (size_power_of_2 > 31) || (phys_addr & ((1 << size_power_of_2)-1)))
    {
        printk("\x1b[35m Paramets Error:  win[%d] phys_addr[0x%x] power_of_2[%d] port_id[%d] cache_bit[%d]\x1b[0m\n", 
               win, phys_addr, size_power_of_2, port_id, cache_bit_en);
        return -1;
    }

     switch(node)
    {
        case MST_PORT_NODE_PCIE0:
            master_addr = (MstPortNode *)MST_PORT_NODE_PCIE0_BASE;
            break;
#if !defined(CONFIG_BCM96846) && !defined(_BCM96846_) && !defined(CONFIG_BCM963158) && !defined(CONFIG_BCM96856) && !defined(_BCM96856_)
        case MST_PORT_NODE_SATA:    
            master_addr = (MstPortNode *)MST_PORT_NODE_SATA_BASE;
            break;
#endif
        case MST_PORT_NODE_USB:
            master_addr = (MstPortNode *)MST_PORT_NODE_USB_BASE;
            break;
        case MST_PORT_NODE_PER:
            master_addr = (MstPortNode *)MST_PORT_NODE_PER_BASE;
            break;
        case MST_PORT_NODE_DMA0:
            master_addr = (MstPortNode *)MST_PORT_NODE_DMA0_BASE;
            break;
        case MST_PORT_NODE_RQ0:
            master_addr = (MstPortNode *)MST_PORT_NODE_RQ0_BASE;
            break;
        case MST_PORT_NODE_NATC:
            master_addr = (MstPortNode *)MST_PORT_NODE_NATC_BASE;
            break;
        case MST_PORT_NODE_DQM:
            master_addr = (MstPortNode *)MST_PORT_NODE_DQM_BASE;
            break;
        case MST_PORT_NODE_QM:
            master_addr = (MstPortNode *)MST_PORT_NODE_QM_BASE;
            break;
#if defined (CONFIG_BCM96858) || defined(_BCM96858_) || defined (CONFIG_BCM963158) || defined(_BCM963158_)
        case MST_PORT_NODE_B53:
            master_addr = (MstPortNode *)MST_PORT_NODE_B53_BASE;
            break;
        case MST_PORT_NODE_PMC:
            master_addr = (MstPortNode *)MST_PORT_NODE_PMC_BASE;
            break;
        case MST_PORT_NODE_DMA1:
            master_addr = (MstPortNode *)MST_PORT_NODE_DMA1_BASE;
            break;
#if defined (CONFIG_BCM96858) || defined(_BCM96858_)
        case MST_PORT_NODE_PCIE1:
            master_addr = (MstPortNode *)MST_PORT_NODE_PCIE1_BASE;
            break;
        case MST_PORT_NODE_APM:
            master_addr = (MstPortNode *)MST_PORT_NODE_APM_BASE;
            break;
        case MST_PORT_NODE_RQ1:
            master_addr = (MstPortNode *)MST_PORT_NODE_RQ1_BASE;
            break;
        case MST_PORT_NODE_RQ2:
            master_addr = (MstPortNode *)MST_PORT_NODE_RQ2_BASE;
            break;
        case MST_PORT_NODE_RQ3:
            master_addr = (MstPortNode *)MST_PORT_NODE_RQ3_BASE;
            break;
#endif
#if defined (CONFIG_BCM963158) || defined(_BCM963158_)
        case MST_PORT_NODE_DSL:
            master_addr = (MstPortNode *)MST_PORT_NODE_DSL_BASE;
            break;
        case MST_PORT_NODE_DSLCPU:
            master_addr = (MstPortNode *)MST_PORT_NODE_DSLCPU_BASE;
            break;
        case MST_PORT_NODE_SWH:
            master_addr = (MstPortNode *)MST_PORT_NODE_SWH_BASE;
            break;
        case MST_PORT_NODE_SPU:
            master_addr = (MstPortNode *)MST_PORT_NODE_SPU_BASE;
            break;
        case MST_PORT_NODE_PCIE3:
            master_addr = (MstPortNode *)MST_PORT_NODE_PCIE3_BASE;
            break;
#endif
#endif
#if defined (CONFIG_BCM963158) || defined(_BCM963158_) || defined (CONFIG_BCM96856)
        case MST_PORT_NODE_PCIE2:
            master_addr = (MstPortNode *)MST_PORT_NODE_PCIE2_BASE;
            break;
#endif
#if defined(CONFIG_BCM96836) || defined(_BCM96836_) || defined (CONFIG_BCM963158) || defined(_BCM963158_) || \
            defined(CONFIG_BCM96856) || defined(_BCM96856_)
        case MST_PORT_NODE_PERDMA:
          master_addr = (MstPortNode *)MST_PORT_NODE_PER_DMA_BASE;
          break;
#endif
     default:
         printk("\x1b[35m ERROR line[[%d]]: node[%d] \x1b[0m\n",__LINE__, node);
         ret = -1;
     }

    if(!ret)
    {
#if defined(CONFIG_BCM963158) || defined(_BCM963158_)
        /* 63158 has the all the master connected to the CCI as default so no need to
        configure the map. Just turn on the cache configuration */
        if( cache_bit_en )
        {
            master_addr->decode_cfg.cache_cfg = 0x1;
            master_addr->decode_cfg.ctrl &= ~DECODE_CFG_CTRL_CACHE_SEL_MASK;
            master_addr->decode_cfg.ctrl |= DECODE_CFG_CTRL_CACHE_SEL_CFG_REG;  
        }
        else
        {
            master_addr->decode_cfg.cache_cfg = 0x0;
            master_addr->decode_cfg.ctrl &= ~DECODE_CFG_CTRL_CACHE_SEL_MASK;
            master_addr->decode_cfg.ctrl |= DECODE_CFG_CTRL_CACHE_SEL_DEF;  
        }
#else
        if(size_power_of_2)
        {
            master_addr->decode_cfg.window[win].base_addr =  (phys_addr>>8);
            master_addr->decode_cfg.window[win].remap_addr = (phys_addr>>8);
#if (CONFIG_BCM_CHIP_REV == 0x6836A0)
            if (node == MST_PORT_NODE_RQ0)
                master_addr->decode_cfg.window[win].remap_addr |= 1 << 24;
#endif

            if( (port_id == DECODE_CFG_PID_B53) && (cache_bit_en))
            {
                    master_addr->decode_cfg.window[win].attributes = 
                                (DECODE_CFG_CACHE_BITS | DECODE_CFG_ENABLE_ADDR_ONLY | (size_power_of_2 << DECODE_CFG_SIZE_SHIFT) | port_id) ;
                    UBUS_REMAP_DEBUG_LOG("\x1b[35m base_addr[0x%x] remap_addr[0x%x] attributes[0x%x]\x1b[0m\n", 
                                         master_addr->decode_cfg.window[win].base_addr, 
                                         master_addr->decode_cfg.window[win].remap_addr,
                                         master_addr->decode_cfg.window[win].attributes);
             }
            else
            {
                master_addr->decode_cfg.window[win].attributes = 
                                (DECODE_CFG_ENABLE_ADDR_ONLY | (size_power_of_2 << DECODE_CFG_SIZE_SHIFT) | port_id) ;

                UBUS_REMAP_DEBUG_LOG("\x1b[35m base_addr[0x%x] remap_addr[0x%x] attributes[0x%x]\x1b[0m\n",
                                     master_addr->decode_cfg.window[win].base_addr, 
                                     master_addr->decode_cfg.window[win].remap_addr,
                                     master_addr->decode_cfg.window[win].attributes); 
            }
        }
        else
        {
            master_addr->decode_cfg.window[win].base_addr = 0;
            master_addr->decode_cfg.window[win].remap_addr = 0;
            master_addr->decode_cfg.window[win].attributes = 0;
        }
#endif
    }

    return ret;
}
#ifndef _CFE_
EXPORT_SYMBOL(ubus_master_decode_wnd_cfg);
#endif
#endif  //#if defined (CONFIG_BCM96858) || defined(_BCM96858_) || defined(CONFIG_BCM96836) || defined(_BCM96836_) || defined(CONFIG_BCM96846) || defined(_BCM96846_) || defined(CONFIG_BCM963158) || defined(_BCM963158_)


#if defined(CONFIG_BCM963158) || defined(_BCM963158_)
int ubus_remap_to_biu_cfg_wlu_srcpid(int srcpid, int enable)
{
    volatile CoherencyPortCfgReg_t* cpcfg_reg = (volatile CoherencyPortCfgReg_t * const)UBUS_COHERENCY_PORT_CFG_BASE;
    int i = 0, reg, bit;

    if( srcpid > MAX_WLU_SRCPID_NUM )
        return -1;

    if( srcpid == MAX_WLU_SRCPID_NUM ) {
        for( i = 0; i < MAX_WLU_SRCPID_REG_NUM; i++ )
            cpcfg_reg->wlu_srcpid[i] = enable ? 0xffffffff : 0;
    } else {
        reg = WLU_SRCPID_TO_REG_OFFSET(srcpid);
        bit = WLU_SRCPID_TO_REG_BIT(srcpid);
        if( enable ) 
            cpcfg_reg->wlu_srcpid[reg] |= (0x1<<bit);
        else 
            cpcfg_reg->wlu_srcpid[reg] &= ~(0x1<<bit);
    }

    return 0;
}
#endif //#if defined(CONFIG_BCM963158) || defined(_BCM963158_)

#if defined(CONFIG_BCM_UBUS_DECODE_REMAP) && !defined(CONFIG_BCM963158)
#define SIZE_OF_REG_BYTES     (4)
static int ubus_remap_to_biu_cfg_queue_srcpid(unsigned long lut_idx, unsigned int *p_srcpid_queus_value)
{
    CoherencyPortCfgReg_t *reg_addr = 
        (CoherencyPortCfgReg_t*)(UBUS_COHERENCY_PORT_CFG_LUT_BASE + lut_idx * SIZE_OF_REG_BYTES); 

    if((lut_idx > 31) || (NULL == p_srcpid_queus_value))
        return -1;

    reg_addr->queue_cfg = (((p_srcpid_queus_value[0] & 0xf) << SRCPID_TO_QUEUE_0_BITS_SHIFT) |
                           ((p_srcpid_queus_value[1] & 0xf) << SRCPID_TO_QUEUE_1_BITS_SHIFT) |
                           ((p_srcpid_queus_value[2] & 0xf) << SRCPID_TO_QUEUE_2_BITS_SHIFT) |
                           ((p_srcpid_queus_value[3] & 0xf) << SRCPID_TO_QUEUE_3_BITS_SHIFT) |
                           ((p_srcpid_queus_value[4] & 0xf) << SRCPID_TO_QUEUE_4_BITS_SHIFT) |
                           ((p_srcpid_queus_value[5] & 0xf) << SRCPID_TO_QUEUE_5_BITS_SHIFT) |
                           ((p_srcpid_queus_value[6] & 0xf) << SRCPID_TO_QUEUE_6_BITS_SHIFT) |
                           ((p_srcpid_queus_value[7] & 0xf) << SRCPID_TO_QUEUE_7_BITS_SHIFT));

    UBUS_REMAP_DEBUG_LOG("\x1b[35m reg_addr[0x%p] reg_value[0x%x]\x1b[0m\n",
                         (unsigned int*)(reg_addr), reg_addr->queue_cfg); 
    
    return 0;                        
}

static int ubus_remap_to_biu_cfg_queue_depth(unsigned long q_depth_idx, unsigned int *p_depth_queus_value)
{
    CoherencyPortCfgReg_t *reg_addr = 
        (CoherencyPortCfgReg_t*)(UBUS_COHERENCY_PORT_CFG_DEPTH_BASE + q_depth_idx * SIZE_OF_REG_BYTES); 

    if((q_depth_idx > 3) || (NULL == p_depth_queus_value))
        return -1;

    reg_addr->queue_cfg = (((p_depth_queus_value[0] & 0xff) << DEPTH_TO_QUEUE_0_BITS_SHIFT) |
                           ((p_depth_queus_value[1] & 0xff) << DEPTH_TO_QUEUE_1_BITS_SHIFT) |
                           ((p_depth_queus_value[2] & 0xff) << DEPTH_TO_QUEUE_2_BITS_SHIFT) |
                           ((p_depth_queus_value[3] & 0xff) << DEPTH_TO_QUEUE_3_BITS_SHIFT));

    UBUS_REMAP_DEBUG_LOG("\x1b[35m reg_addr[0x%p] reg_value[0x%x]\x1b[0m\n",
                         (unsigned int*)(reg_addr), reg_addr->queue_cfg); 

    return 0;                        
}

static int ubus_remap_to_biu_cfg_queue_thresh(unsigned long q_thresh_idx, unsigned int *p_thresh_queus_value)
{
    CoherencyPortCfgReg_t *reg_addr = 
        (CoherencyPortCfgReg_t*)(UBUS_COHERENCY_PORT_CFG_CBS_BASE + q_thresh_idx * SIZE_OF_REG_BYTES); 

    if((q_thresh_idx > 8) || (NULL == p_thresh_queus_value))
        return -1;

    reg_addr->queue_cfg = (((p_thresh_queus_value[0] & 0xffff) << THRESH_TO_QUEUE_0_BITS_SHIFT) |
                          ((p_thresh_queus_value [1] & 0xffff) << THRESH_TO_QUEUE_1_BITS_SHIFT));
                    
    UBUS_REMAP_DEBUG_LOG("\x1b[35m reg_addr[0x%p] reg_value[0x%x]\x1b[0m\n",
                         (unsigned int*)(reg_addr), reg_addr->queue_cfg); 

    return 0;                        
}

static int ubus_remap_to_biu_cfg_cir_incr(unsigned long q_cirinc_idx, unsigned int *p_cirinc_queus_value)
{
    CoherencyPortCfgReg_t *reg_addr =
         (CoherencyPortCfgReg_t*)(UBUS_COHERENCY_PORT_CFG_CIR_INCR_BASE + q_cirinc_idx * SIZE_OF_REG_BYTES); 

    if((q_cirinc_idx > 3) || (NULL == p_cirinc_queus_value))
        return -1;

    reg_addr->queue_cfg = (((p_cirinc_queus_value[0] & 0xff) << CIR_INCR_TO_QUEUE_0_BITS_SHIFT) |
                          ((p_cirinc_queus_value [1] & 0xff) << CIR_INCR_TO_QUEUE_1_BITS_SHIFT) |
                          ((p_cirinc_queus_value [2] & 0xff) << CIR_INCR_TO_QUEUE_2_BITS_SHIFT) |
                          ((p_cirinc_queus_value [3] & 0xff) << CIR_INCR_TO_QUEUE_3_BITS_SHIFT));

    UBUS_REMAP_DEBUG_LOG("\x1b[35m reg_addr[0x%p] reg_value[0x%x]\x1b[0m\n",
                         (unsigned int*)(reg_addr), reg_addr->queue_cfg); 

    return 0;                        
}

static int ubus_remap_to_biu_cfg_ref_cnt(unsigned long q_ref_cnt_idx, unsigned int *p_ref_cnt_value)
{
    CoherencyPortCfgReg_t *reg_addr = 
       (CoherencyPortCfgReg_t*)(UBUS_COHERENCY_PORT_CFG_REF_COUNT_BASE + q_ref_cnt_idx * SIZE_OF_REG_BYTES); 

    if((q_ref_cnt_idx > 1) || (NULL == p_ref_cnt_value))
        return -1;

    reg_addr->queue_cfg = (((p_ref_cnt_value[0] & 0xf) << REF_CNT_0_BITS_SHIFT) |
                           ((p_ref_cnt_value[1] & 0xf) << REF_CNT_1_BITS_SHIFT) |
                           ((p_ref_cnt_value[2] & 0xf) << REF_CNT_2_BITS_SHIFT) |
                           ((p_ref_cnt_value[3] & 0xf) << REF_CNT_3_BITS_SHIFT) |
                           ((p_ref_cnt_value[4] & 0xf) << REF_CNT_4_BITS_SHIFT) |
                           ((p_ref_cnt_value[5] & 0xf) << REF_CNT_5_BITS_SHIFT) |
                           ((p_ref_cnt_value[6] & 0xf) << REF_CNT_6_BITS_SHIFT) |
                           ((p_ref_cnt_value[7] & 0xf) << REF_CNT_7_BITS_SHIFT));
                    
    UBUS_REMAP_DEBUG_LOG("\x1b[35m  reg_addr[0x%p] reg_value[0x%x]\x1b[0m\n",
                         (unsigned int*)(reg_addr), reg_addr->queue_cfg); 
    
    return 0;                        
}

#ifndef _CFE_
static int ubus_proc_cmd_remap_master_x(int argc, char *argv[])
{
    char *usage = "Usage: remap_master_x <masters id (0-5)> [<remap 0|1>]";
    unsigned int remap_master = 0xffffffff;
    unsigned int if_cache = 0xffffffff;
    int rc = 0;

    if (argc < 3)
        goto wrong_params;

    if (kstrtos32(argv[1], 10, &remap_master))
        goto wrong_params;

    if (remap_master < MST_PORT_NODE_PCIE0 || remap_master > MST_PORT_NODE_LAST - MST_PORT_NODE_DMA0)
        goto wrong_params;


    if (kstrtou32(argv[2], 10, &if_cache))
        goto wrong_params;

    if (if_cache > 1)
        goto wrong_params; 
    
    
    rc = ubus_master_decode_wnd_cfg(remap_master, 
                                    0/* Always use window 0 to remap all and wind 1 to coherent rings */, 
                                    MST_START_DDR_ADDR, 
                                    g_board_size_power_of_2, 
                                    DECODE_CFG_PID_B53, 
                                    if_cache);/* For now not support cache mode*/
    if (rc < 0)
        printk("Error %s line[%d] node[%d] address[0x%x] size[%d]:\n",
               __FILE__, __LINE__, remap_master, MST_START_DDR_ADDR, g_board_size_power_of_2); 
    return rc;


wrong_params:
    pr_info("%s\n", usage);
    return rc;

}

static int ubus_proc_cmd_remap_master_all(int argc, char *argv[])
{
    char *usage = "Usage: remap_master_all <[<remap 0|1>]";
    unsigned int if_remap = 0xffffffff;
    int rc = 0;
    unsigned int i = 0;

    if (argc < 2)
        goto wrong_params;

    if (kstrtou32(argv[1], 10, &if_remap))
        goto wrong_params;

    if (if_remap > 1)
        goto wrong_params;
    
     
    for (;i < MST_PORT_NODE_LAST - MST_PORT_NODE_DMA0; i++ ) 
    {
        rc = ubus_master_decode_wnd_cfg(i, 
                                        0, /* Always use window 0 to remap all and wind 0 to coherent rings */
                                        MST_START_DDR_ADDR, 
                                        g_board_size_power_of_2, 
                                        DECODE_CFG_PID_B53, 
                                        if_remap);
        if (rc < 0)
        {
            printk("Error %s line[%d] node[%d] address[0x%x] size[%d]: \n",
                    __FILE__, __LINE__, i,  MST_START_DDR_ADDR, g_board_size_power_of_2);
            return rc;
        }

    } 
    
    return rc;                      


wrong_params:
    pr_info("%s\n", usage);
    return rc;
}

static int ubus_proc_cmd_remap_master_reset_all(int argc, char *argv[])
{
    char *usage = "Usage: remap_master_reset_all";
    int rc = 0;
    unsigned int i = 0;

    if (argc < 1)
        goto wrong_params;
    
     
    for (;i < MST_PORT_NODE_LAST; i++ ) 
    {
        rc = ubus_master_decode_wnd_cfg(i, 
                                        0 /* Always use window 1 to remap all and wind 0 to coherent rings */,  
                                        MST_START_DDR_ADDR, 
                                        0,/* to reset parameters */
                                        DECODE_CFG_PID_B53, 
                                        0);
        if (rc < 0)
        {
            printk("Error %s line[%d] node[%d] address[0x%x] size[%d]: \n",
                   __FILE__, __LINE__, i, MST_START_DDR_ADDR,0);
            return rc;
        }

    } 
    
    return rc;                      


wrong_params:
    pr_info("%s\n", usage);
    return rc;
}

static int create_ubus_proc(void)
{
    int rc = 0;

    proc_dir = proc_mkdir(PROC_DIR, NULL);
    if (!proc_dir) 
    {
        pr_err("Failed to create PROC directory %s.\n",
            PROC_DIR);
        goto error;
    }


    cmd_proc_file = proc_create_cmd(UBUS_PROC_FILE, proc_dir, &ubus_remap_command_table);
    if (!cmd_proc_file) 
    {
        pr_err("Failed to create %s\n", UBUS_PROC_FILE);
        goto error;
    }

    return rc;

error:
    if (cmd_proc_file) 
    {
        remove_proc_entry(UBUS_PROC_FILE, proc_dir);
        cmd_proc_file = NULL;
    }	
    if (proc_dir)
    {
        remove_proc_entry(PROC_DIR, NULL);
        proc_dir = NULL;
    }
    
    return -1;	
}
#endif // #ifndef _CFE_
   
#define  SRC_PID_Q_NUM      (8)
#define  DEPTH_Q_NUM        (4)
#define  THRESH_Q_NUM       (2)
#define  CIR_INCR_Q_NUM     (4)
#define  REF_CNT_NUM        (8)
static int configure_biu_pid_to_queue(void)
{
    int rc = 0;
    unsigned int srcpid_queus_value[SRC_PID_Q_NUM] = {0};
    unsigned int depth_queus_value[DEPTH_Q_NUM] = {0};
    unsigned int thresh_queus_value[THRESH_Q_NUM] = {0};
    unsigned int cir_incr_queus_value[CIR_INCR_Q_NUM] = {0};
    unsigned int ref_cnt_value[REF_CNT_NUM] = {0};
    unsigned long lut_idx;
    unsigned long depth_idx;
    unsigned long thresh_idx;
    unsigned long cir_incr_idx;
    unsigned long ref_cnt_idx;

    lut_idx = 0;
    srcpid_queus_value[0] = 0;
    srcpid_queus_value[1] = 0;
    srcpid_queus_value[2] = 0;
    srcpid_queus_value[3] = 0;
    srcpid_queus_value[4] = 1;
    srcpid_queus_value[5] = 1;
    srcpid_queus_value[6] = 1;
    srcpid_queus_value[7] = 1;
    rc = ubus_remap_to_biu_cfg_queue_srcpid(lut_idx, srcpid_queus_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

    lut_idx = 1;
    srcpid_queus_value[0] = 0;
    srcpid_queus_value[1] = 0;
    srcpid_queus_value[2] = 0;
    srcpid_queus_value[3] = 2;
    srcpid_queus_value[4] = 3;
    srcpid_queus_value[5] = 4;
    srcpid_queus_value[6] = 3;
    srcpid_queus_value[7] = 5;
    rc = ubus_remap_to_biu_cfg_queue_srcpid(lut_idx, srcpid_queus_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

    depth_idx = 0;
    depth_queus_value[0] = 0x10;
    depth_queus_value[1] = 0x10;
    depth_queus_value[2] = 8;
    depth_queus_value[3] = 8;
    rc = ubus_remap_to_biu_cfg_queue_depth(depth_idx, depth_queus_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

    depth_idx = 1;
    depth_queus_value[0] = 8;
    depth_queus_value[1] = 8;
    depth_queus_value[2] = 0;
    depth_queus_value[3] = 0;
    rc = ubus_remap_to_biu_cfg_queue_depth(depth_idx, depth_queus_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

    thresh_idx = 0;
    thresh_queus_value[0] = 0x100;
    thresh_queus_value[1] = 0x100;
    rc = ubus_remap_to_biu_cfg_queue_thresh(thresh_idx, thresh_queus_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

    thresh_idx = 1;
    thresh_queus_value[0] = 0x1000;
    thresh_queus_value[1] = 0x400;
    rc = ubus_remap_to_biu_cfg_queue_thresh(thresh_idx, thresh_queus_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

    thresh_idx = 2;
    thresh_queus_value[0] = 0x400;
    thresh_queus_value[1] = 0x1000;
    rc = ubus_remap_to_biu_cfg_queue_thresh(thresh_idx, thresh_queus_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

    cir_incr_idx = 0;
    cir_incr_queus_value[0] = 1;
    cir_incr_queus_value[1] = 1;
    cir_incr_queus_value[2] = 4;
    cir_incr_queus_value[3] = 4;
    rc = ubus_remap_to_biu_cfg_cir_incr(cir_incr_idx, cir_incr_queus_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

    cir_incr_idx = 1;
    cir_incr_queus_value[0] = 2;
    cir_incr_queus_value[1] = 3;
    cir_incr_queus_value[2] = 0;
    cir_incr_queus_value[3] = 0;
    rc = ubus_remap_to_biu_cfg_cir_incr(cir_incr_idx, cir_incr_queus_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

    ref_cnt_idx = 0;
    ref_cnt_value[0] = 7;
    ref_cnt_value[1] = 4;
    ref_cnt_value[2] = 2;
    ref_cnt_value[3] = 2;
    ref_cnt_value[4] = 3;
    ref_cnt_value[5] = 2;
    ref_cnt_value[6] = 0;
    ref_cnt_value[7] = 0;
    rc = ubus_remap_to_biu_cfg_ref_cnt(ref_cnt_idx, ref_cnt_value);
    if (rc < 0)
    {
        printk("Error %s line[%d]\n",__FILE__, __LINE__);
        goto exit_;
    }

exit_:
    if (rc < 0)
        printk("Error: line[%d]\n",__LINE__);

    return rc;                       
}
#endif //#if defined(CONFIG_BCM_UBUS_DECODE_REMAP) && !defined(CONFIG_BCM963158)

/* CONFIG_BCM_FPM_COHERENCY_EXCLUDE is the case when all the memory is coherent except for FPM pool, since CCI-400 
 * won't sustain 10G traffic load.
 * In that case we remap FPM pool area to be non-coherent
 */
#if defined(CONFIG_BCM_FPM_COHERENCY_EXCLUDE)
static void ubus_masters_remap_fpm_pool(void)
{
    void *fpm_pool_addr;
    uint32_t fpm_pool_size;
    int i, rc;

    if (BcmMemReserveGetByName(FPMPOOL_BASE_ADDR_STR, &fpm_pool_addr, &fpm_pool_size))
    {
        printk("misc_hw_init: failed to get fpm_pool base address\n");
        return;
    }
    fpm_pool_addr = virt_to_phys(fpm_pool_addr);
    fpm_pool_size = log2_32(fpm_pool_size);
    for (i = 0; i < MST_PORT_NODE_LAST; i++) 
    {
        if (ubus_master_decode_wnd_cfg(i, DECODE_WIN1, fpm_pool_addr, fpm_pool_size, DECODE_CFG_PID_B53, CACHE_BIT_OFF))
        {
            printk("misc_hw_init: decode window configure failure; node[%d] phys address[0x%x] size[%d]: \n",
                    i, fpm_pool_addr, fpm_pool_size);
            break;
        }
    }
}
#endif /* CONFIG_BCM_FPM_COHERENCY_EXCLUDE */

#ifdef CONFIG_BCM_UBUS_DECODE_REMAP
int remap_ubus_masters_biu(void)
{
    int rc = 0;
    unsigned int i = 0;

#if defined(CONFIG_BCM96836) || defined(_BCM96836_)
    /* Need to add proc and update start_mst_port and last_mst_port port_id */
    rc = create_ubus_proc();
    if (rc < 0)
        goto exit_;
#endif
    /* Calculate board size of power 2 */
    g_board_size_power_of_2 = log2_32(getMemorySize());

    UBUS_REMAP_DEBUG_LOG("\x1b[35m board_sdram_size[0x%lx] board_size_power_of_2[%d]\x1b[0m\n",
                         getMemorySize(), g_board_size_power_of_2);

    for (; i < MST_PORT_NODE_LAST; i++ ) 
    {
        rc = ubus_master_decode_wnd_cfg(i, 
                                        DECODE_WIN0, 
                                        MST_START_DDR_ADDR, 
                                        g_board_size_power_of_2, 
                                        DECODE_CFG_PID_B53, 
                                        IS_DDR_COHERENT ? CACHE_BIT_ON : CACHE_BIT_OFF);
        if (rc < 0)
        {
            printk("Error %s line[%d] node[%d] address[0x%x] size[%d]: \n",
                   __FILE__, __LINE__, i, MST_START_DDR_ADDR,g_board_size_power_of_2);
            goto exit_;
        }
    }

exit_:
    if (rc < 0)
        printk("Error: line[%d] rc = %d\n",__LINE__, rc);

    return rc;                       
}
#endif /* CONFIG_BCM_UBUS_DECODE_REMAP */

#if defined(CONFIG_BCM963158)
void apply_ubus_credit_each_master(int master)
{
    int i;
    ubus_credit_cfg_t credit;

    if( master < 0 || master >= MST_PORT_NODE_LAST )
        return; 

    for( i = 0; i < UBUS_MAX_PORT_NUM; i++ ) {
        credit = ubus_credit_tbl[master][i];
        if( credit.port_id == -1 )
            break;
        ubus_master_set_token_credits(master, credit.port_id, credit.credit);
    }

    return;
}
static void apply_ubus_credit(void) 
{
    /* this function only apply the master that is powered on by default */
    /* other master must apply credit aftet it is power on in pmc driver */
    apply_ubus_credit_each_master(MST_PORT_NODE_B53);
    apply_ubus_credit_each_master(MST_PORT_NODE_PMC);
    apply_ubus_credit_each_master(MST_PORT_NODE_PER);
    apply_ubus_credit_each_master(MST_PORT_NODE_PERDMA);
    apply_ubus_credit_each_master(MST_PORT_NODE_SPU);

    return;
}
#endif

static void ubus_master_port_node_cfg(void)
{
#if defined(_BCM96858_) || defined(CONFIG_BCM96858)
#ifndef _CFE_
    ubus_master_set_token_credits(MST_PORT_NODE_PER, 1, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_USB, 1, 4);
    ubus_master_set_token_credits(MST_PORT_NODE_PMC, 1, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_DMA0, 1, 9);
    ubus_master_set_token_credits(MST_PORT_NODE_DMA1, 1, 9);
    ubus_master_set_token_credits(MST_PORT_NODE_NATC, 1, 4);
    ubus_master_set_token_credits(MST_PORT_NODE_QM, 1, 10);
    ubus_master_set_token_credits(MST_PORT_NODE_DQM, 1, 7);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ0, 1, 8);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ1, 1, 4);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ2, 1, 4);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ3, 1, 4);

#else //_CFE_
    /*Set UBUS credits for runner quads */
    ubus_master_set_token_credits(MST_PORT_NODE_RQ0, 20, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ1, 20, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ2, 20, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ3, 20, 1);

    ubus_master_set_token_credits(MST_PORT_NODE_RQ0, 32, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ1, 33, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ2, 34, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_RQ3, 35, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_PMC, 32, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_PMC, 33, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_PMC, 34, 1);
    ubus_master_set_token_credits(MST_PORT_NODE_PMC, 35, 1);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ0, 32, 0x9);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ0, 33, 0x209);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ0, 34, 0x109);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ0, 35, 0x309);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ1, 32, 0x9);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ1, 33, 0x209);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ1, 34, 0x109);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ1, 35, 0x309);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ2, 32, 0x8);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ2, 33, 0x208);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ2, 34, 0x108);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ2, 35, 0x308);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ3, 32, 0x8);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ3, 33, 0x208);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ3, 34, 0x108);
    ubus_master_set_rte_addr(MST_PORT_NODE_RQ3, 35, 0x308);
    ubus_master_set_rte_addr(MST_PORT_NODE_DMA0, 32, 0x0);
    ubus_master_set_rte_addr(MST_PORT_NODE_DMA0, 33, 0x8);
    ubus_master_set_rte_addr(MST_PORT_NODE_DMA0, 34, 0x4);
    ubus_master_set_rte_addr(MST_PORT_NODE_DMA0, 35, 0xc);
    ubus_master_set_rte_addr(MST_PORT_NODE_DMA1, 32, 0x1);
    ubus_master_set_rte_addr(MST_PORT_NODE_DMA1, 33, 0x9);
    ubus_master_set_rte_addr(MST_PORT_NODE_DMA1, 34, 0x5);
    ubus_master_set_rte_addr(MST_PORT_NODE_DMA1, 35, 0xd);
    ubus_master_set_rte_addr(MST_PORT_NODE_DQM, 32, 0x2);
    ubus_master_set_rte_addr(MST_PORT_NODE_DQM, 33, 0x6);
    ubus_master_set_rte_addr(MST_PORT_NODE_DQM, 34, 0x24);
    ubus_master_set_rte_addr(MST_PORT_NODE_DQM, 35, 0x64);
    ubus_master_set_rte_addr(MST_PORT_NODE_NATC, 32, 0x0);
    ubus_master_set_rte_addr(MST_PORT_NODE_NATC, 33, 0x8);
    ubus_master_set_rte_addr(MST_PORT_NODE_NATC, 34, 0x4);
    ubus_master_set_rte_addr(MST_PORT_NODE_NATC, 35, 0xc);
#endif
#endif
#if defined(CONFIG_BCM963158)
    apply_ubus_credit();
#endif
}

#if defined(CONFIG_BCM96858) || defined(_BCM96858_) || defined(CONFIG_BCM96836) || defined(_BCM96836_) || \
    defined(CONFIG_BCM96846) || defined(_BCM96846_) || defined(CONFIG_BCM96856) || defined(_BCM96856_)
// ucb commands
#define UCB_CMD_RD               0
#define UCB_CMD_RD_RPLY          1
#define UCB_CMD_WR               2
#define UCB_CMD_WR_ACK           3
#define UCB_CMD_BCAST            4

static void write_reg_thr_sm(uint32_t ucbid, uint32_t addr, uint32_t data)
{
    // check if response fifo is full
    while( UBUSSYSTOP->ReadUcbStatus & 0x40000000);

    UBUSSYSTOP->UcbData = data;
    
    UBUSSYSTOP->UcbHdr = (addr/4) | (UCB_CMD_WR<<12) | (ucbid<<16) | (0x1<<24);
    
    // check if resp fifo has data
    while( !(UBUSSYSTOP->ReadUcbStatus & 0x80000000 ) );
  
    UBUSSYSTOP->ReadUcbHdr;
}

static uint32_t read_reg_thr_sm(uint32_t ucbid, uint32_t addr)
{
    // check if response fifo is full
    while( UBUSSYSTOP->ReadUcbStatus & 0x40000000);    

    UBUSSYSTOP->UcbHdr = (addr/4) | (UCB_CMD_RD<<12) | (ucbid<<16) | (0x1<<24);

    // check if resp fifo has data
    while( !(UBUSSYSTOP->ReadUcbStatus & 0x80000000 ) );

    UBUSSYSTOP->ReadUcbHdr;

    return UBUSSYSTOP->ReadUcbData;
}


void ubus_deregister_port(int ucbid)
{
    // Never play with invalid ubus id
    if (ucbid < 0) return;

    write_reg_thr_sm(ucbid, 0x1c, 0x1);

    // poll status bit for port unregistered
    while ( read_reg_thr_sm(ucbid, 0x1c) != 0x1 );
}

void ubus_register_port(int ucbid)
{
    // Never play with invalid ubus id
    if (ucbid < 0) return;

#if defined (CONFIG_BCM96836) || defined(_BCM96836_) || defined(CONFIG_BCM96858) || defined(_BCM96858_) || \
    defined(CONFIG_BCM96846) || defined(_BCM96846_) || defined (CONFIG_BCM96856) || defined(_BCM96856_)
#if defined(CONFIG_BCM96858) || defined(_BCM96858_)
    if ((ucbid==UCB_NODE_ID_MST_USB) || (ucbid==UCB_NODE_ID_SLV_USB) || (ucbid==UCB_NODE_ID_MST_SATA) || (ucbid==UCB_NODE_ID_SLV_SATA))
#endif
    {
    write_reg_thr_sm(ucbid, 0x1c, 0x0);
    // poll status bit for port registered
    while ( read_reg_thr_sm(ucbid, 0x1c) != 0x2 );
    }
#endif
}
#endif //#if defined(CONFIG_BCM96858) || defined(_BCM96858_) || defined(CONFIG_BCM96836) || defined(_BCM96836_) || defined(CONFIG_BCM96846) || defined(_BCM96846_)

#if defined(CONFIG_BCM96858) || defined(_BCM96858_)
int ubus_master_set_rte_addr(MST_PORT_NODE node, int port, int val)
{
    volatile MstPortNode *master_addr = NULL;
    int ret = 0;

    switch(node)
    {
        case MST_PORT_NODE_PCIE0:
        master_addr = (MstPortNode *)MST_PORT_NODE_PCIE0_BASE;
        break;
        case MST_PORT_NODE_PCIE1:
        master_addr = (MstPortNode *)MST_PORT_NODE_PCIE1_BASE;
        break;
        case MST_PORT_NODE_B53:
        master_addr = (MstPortNode *)MST_PORT_NODE_B53_BASE;
        break;
        case MST_PORT_NODE_SATA:
        master_addr = (MstPortNode *)MST_PORT_NODE_SATA_BASE;
        break;
        case MST_PORT_NODE_USB:
        master_addr = (MstPortNode *)MST_PORT_NODE_USB_BASE;
        break;
        case MST_PORT_NODE_PMC:
        master_addr = (MstPortNode *)MST_PORT_NODE_PMC_BASE;
        break;
        case MST_PORT_NODE_APM:
        master_addr = (MstPortNode *)MST_PORT_NODE_APM_BASE;
        break;
        case MST_PORT_NODE_PER:
        master_addr = (MstPortNode *)MST_PORT_NODE_PER_BASE;
        break;
        case MST_PORT_NODE_DMA0:
        master_addr = (MstPortNode *)MST_PORT_NODE_DMA0_BASE;
        break;
        case MST_PORT_NODE_DMA1:
        master_addr = (MstPortNode *)MST_PORT_NODE_DMA1_BASE;
        break;
        case MST_PORT_NODE_RQ0:
        master_addr = (MstPortNode *)MST_PORT_NODE_RQ0_BASE;
        break;
        case MST_PORT_NODE_RQ1:
        master_addr = (MstPortNode *)MST_PORT_NODE_RQ1_BASE;
        break;
        case MST_PORT_NODE_RQ2:
        master_addr = (MstPortNode *)MST_PORT_NODE_RQ2_BASE;
        break;
        case MST_PORT_NODE_RQ3:
        master_addr = (MstPortNode *)MST_PORT_NODE_RQ3_BASE;
        break;
        case MST_PORT_NODE_NATC:
        master_addr = (MstPortNode *)MST_PORT_NODE_NATC_BASE;
        break;
        case MST_PORT_NODE_DQM:
        master_addr = (MstPortNode *)MST_PORT_NODE_DQM_BASE;
        break;
        case MST_PORT_NODE_QM:
        master_addr = (MstPortNode *)MST_PORT_NODE_QM_BASE;
        break;
        default:
        ret = -1;
    }

    if(!master_addr)
    {
        printk("Node %d master address is zero\n", node);
        return -1;
    }

    if(!ret)
        master_addr->routing_addr[port] = val;

    return ret;
}
#endif //#if defined(CONFIG_BCM96858) || defined(_BCM96858_)

#if defined(CONFIG_BCM96858) || defined(_BCM96858_) || defined(CONFIG_BCM96836) || defined(_BCM96836_) || \
    defined(CONFIG_BCM96846) || defined(_BCM96846_) || defined(CONFIG_BCM963158) || defined(CONFIG_BCM96856) || defined(_BCM96856_)
/*this function is used to set UBUS route credits per usub master, should be equivalent configuration at masters*/
int ubus_master_set_token_credits(MST_PORT_NODE node, int token, int credits)
{
    volatile MstPortNode *master_addr = NULL;
    int ret = 0;

    switch(node)
    {
        case MST_PORT_NODE_RQ0:
        master_addr = (MstPortNode *)MST_PORT_NODE_RQ0_BASE;
        break;
        case MST_PORT_NODE_PCIE0:
        master_addr = (MstPortNode *)MST_PORT_NODE_PCIE0_BASE;
        break;        
#if !defined(CONFIG_BCM96846) && !defined(_BCM96846_) && !defined(CONFIG_BCM963158) && !defined(CONFIG_BCM96856) && !defined(_BCM96856_)
        case MST_PORT_NODE_SATA:
        master_addr = (MstPortNode *)MST_PORT_NODE_SATA_BASE;
        break;
#endif
        case MST_PORT_NODE_USB:
        master_addr = (MstPortNode *)MST_PORT_NODE_USB_BASE;
        break;
        case MST_PORT_NODE_DMA0:
        master_addr = (MstPortNode *)MST_PORT_NODE_DMA0_BASE;
        break;
        case MST_PORT_NODE_PER:
        master_addr = (MstPortNode *)MST_PORT_NODE_PER_BASE;
        break;
        case MST_PORT_NODE_NATC:
        master_addr = (MstPortNode *)MST_PORT_NODE_NATC_BASE;
        break;
        case MST_PORT_NODE_DQM:
        master_addr = (MstPortNode *)MST_PORT_NODE_DQM_BASE;
        break;
        case MST_PORT_NODE_QM:
        master_addr = (MstPortNode *)MST_PORT_NODE_QM_BASE;
        break;        
#if defined(CONFIG_BCM96858) || defined(_BCM96858_) || defined(CONFIG_BCM963158) 
        case MST_PORT_NODE_B53:
        master_addr = (MstPortNode *)MST_PORT_NODE_B53_BASE;
        break;
        case MST_PORT_NODE_PMC:
        master_addr = (MstPortNode *)MST_PORT_NODE_PMC_BASE;
        break;
        case MST_PORT_NODE_DMA1:
        master_addr = (MstPortNode *)MST_PORT_NODE_DMA1_BASE;
        break;
#endif
#if defined(CONFIG_BCM96858) || defined(_BCM96858_)
        case MST_PORT_NODE_PCIE1:
        master_addr = (MstPortNode *)MST_PORT_NODE_PCIE1_BASE;
        break;
        case MST_PORT_NODE_APM:
        master_addr = (MstPortNode *)MST_PORT_NODE_APM_BASE;
        break;
        case MST_PORT_NODE_RQ1:
        master_addr = (MstPortNode *)MST_PORT_NODE_RQ1_BASE;
        break;
        case MST_PORT_NODE_RQ2:
        master_addr = (MstPortNode *)MST_PORT_NODE_RQ2_BASE;
        break;
        case MST_PORT_NODE_RQ3:
        master_addr = (MstPortNode *)MST_PORT_NODE_RQ3_BASE;
        break;
#endif
#if defined(CONFIG_BCM963158)
        case MST_PORT_NODE_PERDMA:
        master_addr = (MstPortNode *)MST_PORT_NODE_PER_DMA_BASE;
        break;
        case MST_PORT_NODE_DSL:
        master_addr = (MstPortNode *)MST_PORT_NODE_DSL_BASE;
        break;
        case MST_PORT_NODE_DSLCPU:
        master_addr = (MstPortNode *)MST_PORT_NODE_DSLCPU_BASE;
        break;
        case MST_PORT_NODE_SWH:
        master_addr = (MstPortNode *)MST_PORT_NODE_SWH_BASE;
        break;
        case MST_PORT_NODE_SPU:
        master_addr = (MstPortNode *)MST_PORT_NODE_SPU_BASE;
        break;
        case MST_PORT_NODE_PCIE2:
        master_addr = (MstPortNode *)MST_PORT_NODE_PCIE2_BASE;
            break;
        case MST_PORT_NODE_PCIE3:
        master_addr = (MstPortNode *)MST_PORT_NODE_PCIE3_BASE;
        break;
#endif
        default:
        ret = -1;
    }

    if(!master_addr)
    {
        printk("Node %d master address is zero\n", node);
        return -1;
    }
    if(!ret) {
        //printk("Master node %02d(%p) credit for ubus port %02d is 0x%08x\n", node, &master_addr->token[token], token, master_addr->token[token]);
        master_addr->token[token] = credits;
        //printk("Master node %02d(%p) credit for ubus port %02d set to %02d, read back 0x%08x\n", node, &master_addr->token[token], token, credits, master_addr->token[token]);
    }
    return ret;
}

#ifndef _CFE_
EXPORT_SYMBOL(ubus_master_set_token_credits);
#endif
#endif //#if defined(CONFIG_BCM96858) || defined(_BCM96858_) || defined(CONFIG_BCM96836) || defined(_BCM96836_) || defined(CONFIG_BCM96846) || defined(_BCM96846_) || defined(CONFIG_BCM963158) || defined(CONFIG_BCM96856) || defined(_BCM96856_) || defined(CONFIG_BCM96856) || defined(_BCM96856_)

void bcm_ubus_config(void)
{
    ubus_master_port_node_cfg();
#ifdef CONFIG_BCM_UBUS_DECODE_REMAP
    remap_ubus_masters_biu();
#if defined(CONFIG_BCM963158)
    ubus_remap_to_biu_cfg_wlu_srcpid(MAX_WLU_SRCPID_NUM, 1);
#else
    configure_biu_pid_to_queue();
#endif
#ifdef CONFIG_BCM_FPM_COHERENCY_EXCLUDE
    /* FPM pool memory must not be cache coherent since CCI-400 can't handle 10Gig bandwidth */
   ubus_masters_remap_fpm_pool();
#endif
#endif /* CONFIG_BCM_UBUS_DECODE_REMAP */
}
