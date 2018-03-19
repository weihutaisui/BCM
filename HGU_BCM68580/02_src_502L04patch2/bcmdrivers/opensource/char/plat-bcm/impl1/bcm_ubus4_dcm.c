
/*
<:copyright-BRCM:2017:GPL/GPL:standard

         Copyright (c) 2017 Broadcom
         All Rights Reserved

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published 
by the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, 
or by writing to the Free Software Foundation, Inc., 59 Temple Place - 
Suite 330, Boston, MA 02111-1307, USA.

:>
*/


#include <linux/kernel.h>
#include <linux/init.h>
#include <bcm_map_part.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>

// magically creates /sys/module/ubus4_dcm/parameter directory
#ifdef MODULE_PARAM_PREFIX
#undef MODULE_PARAM_PREFIX
#endif
#define MODULE_PARAM_PREFIX "ubus4_dcm."

// local defines
#define	UBUS_DCM_ON			1
#define	UBUS_DCM_OFF		0
#define	DCM_DIVIDER_DEFAULT	3					// 1/8 of UBUS_CLK
#define DCM_THOLD_TO_HIGH_FREQ_DEFAULT	1		// 1  UBUS req
#define DCM_THOLD_TO_LOW_FREQ_DEFAULT	0x400	// 1K UBUS_CLK cycles

//===========================================
// UBUS4 master registeration
//===========================================

// There is no one single file which has the official info about the mapping 
// The following mapping are obtained by doing
// (a) grep -r MST_PORT_CFG_REG_MST_PORT_NODE design/chip/* | grep -v svn | grep START
// (b) manually, using a test board to read from 
// 			MST_PORT_CFG_REG_MST_PORT_NODE_*_Port_Info.{Port_Type and Port_ID}
// (c) check that the expected modules are registered
// 			SYS_CLIENT_REG_CFG_UBUS_MAPPED_MASTER_STATUS_0
// 			SYS_CLIENT_REG_CFG_UBUS_MAPPED_XRDP_MASTER_STATUS_0 

// 63158 specific
typedef enum ubus4_mst_bit_pos_t {
	MST_PORT_NODE_B53_BIT_POS=2, 	// UBUS4 SYS
	MST_PORT_NODE_PER_BIT_POS,
	MST_PORT_NODE_USB_BIT_POS, 
	MST_PORT_NODE_SPU_BIT_POS, 
	MST_PORT_NODE_DSL_BIT_POS, 
	MST_PORT_NODE_PER_DMA_BIT_POS, 
	MST_PORT_NODE_PCIE0_BIT_POS, 
	MST_PORT_NODE_PCIE2_BIT_POS, 
	MST_PORT_NODE_PCIE3_BIT_POS, 
	MST_PORT_NODE_DSLCPU_BIT_POS, 
	MST_PORT_NODE_PMC_BIT_POS=13, 
	MST_PORT_NODE_QM_BIT_POS=22, 	// XRDP
	MST_PORT_NODE_DQM_BIT_POS, 
	MST_PORT_NODE_DMA0_BIT_POS, 
	MST_PORT_NODE_NATC_BIT_POS=26,
	MST_PORT_NODE_LAST_BIT_POS } ubus4_mst_bit_pos_t;	// last

// generic structure
typedef struct ub_reg_addr_map_t {
	ubus4_mst_bit_pos_t	bit_pos;
	void *				base;
	} ub_reg_addr_map_t;

//===========================================
// Generic functions
//===========================================

static u32 dcm_enable_get(void)
{
    return ((UBUS4CLK->ClockCtrl & UBUS4_CLK_BYPASS_MASK) ? 0 : 1);
}

static void dcm_enable_set(u32 enable)
{
    u32 d32;
    u32 m32;

    m32 = UBUS4_CLK_BYPASS_MASK|UBUS4_CLK_CTRL_EN_MASK;

    if (enable==0) {	// disable : CLK_BYPASS=1, CLK_CTRL_EN=1
		d32 = (1<<UBUS4_CLK_BYPASS_SHIFT) | (1<<UBUS4_CLK_CTRL_EN_SHIFT); 
        UBUS4CLK->ClockCtrl     = (UBUS4CLK->ClockCtrl 	   & ~m32) | d32;
        UBUS4XRDPCLK->ClockCtrl = (UBUS4XRDPCLK->ClockCtrl & ~m32) | d32;

    } else {		//enable -> CLK_BYPASS=0, CLK_CTRL_EN=1
		d32 = 1<<UBUS4_CLK_CTRL_EN_SHIFT; 
        UBUS4CLK->ClockCtrl 	= (UBUS4CLK->ClockCtrl     & ~m32) | d32;
        UBUS4XRDPCLK->ClockCtrl = (UBUS4XRDPCLK->ClockCtrl & ~m32) | d32;
    }
}

static void dcm_divider_set(uint32 val)
{
    uint32_t d32=0, m32=0;

    m32 = (~(UBUS4_MID_CLK_SEL_MASK|UBUS4_MIN_CLK_SEL_MASK));
    d32 = (val<<UBUS4_MIN_CLK_SEL_SHIFT)|(val<<UBUS4_MID_CLK_SEL_SHIFT);
    UBUS4CLK->ClockCtrl		= (UBUS4CLK->ClockCtrl     & m32) | d32;
    UBUS4XRDPCLK->ClockCtrl = (UBUS4XRDPCLK->ClockCtrl & m32) | d32;
}

static u32 dcm_divider_get(void)
{
	return ((UBUS4CLK->ClockCtrl & UBUS4_MIN_CLK_SEL_MASK) >> 
			UBUS4_MIN_CLK_SEL_SHIFT);
}

// Controls how soon the clock scaling changed
// For each UBUS4 segment
static void dcm_thold_set(uint32 val1, uint32 val2)
{
	//
	// switch to high freq
	UBUS4CLK    ->Min2Mid_threshhold = val1;
	UBUS4CLK    ->Mid2Max_threshhold = val1;
    UBUS4XRDPCLK->Min2Mid_threshhold = val1;
    UBUS4XRDPCLK->Mid2Max_threshhold = val1;

	// switch to low freq
	UBUS4CLK    ->Mid2Min_threshhold = val2;
	UBUS4CLK    ->Max2Mid_threshhold = val2;
    UBUS4XRDPCLK->Mid2Min_threshhold = val2;
    UBUS4XRDPCLK->Max2Mid_threshhold = val2;
}

static u32 dcm_thold_get(void)
{
    return (UBUS4CLK->Mid2Min_threshhold);
}

// Controls how soon the congestion signal is asserted
// For individual master
static void dcm_ubus_threshold_wr(void * base, u32 val)
{
#define DCM_UBUS_CONGESTION_THRESHOLD 3
	((MstPortNode*) base )->port_cfg[DCM_UBUS_CONGESTION_THRESHOLD] = val;
}

// set default values
static void dcm_config_set(void)
{
	u32 d32, mst_registered;

	volatile Ubus4ModuleClientRegistration * ubus4_sys_client_registration;
	volatile Ubus4ModuleClientRegistration * ubus4_xrdp_client_registration;
	
	ubus4_sys_client_registration  = UBUSSYSTOP_REGISTRATION;
	ubus4_xrdp_client_registration = UBUSSYSXRDP_REGISTRATION;

    // UBUS Congestion Threshold regs. 
    // Only writes to those mst nodes that are registered
    // Set to '0' to response to the 1st UBUS request

	// ubus system
	mst_registered = ubus4_sys_client_registration->MstStatus[0];
    if (mst_registered &       (1<<MST_PORT_NODE_B53_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_B53_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_PER_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_PER_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_USB_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_USB_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_SPU_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_SPU_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_DSL_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_DSL_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_PMC_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_PMC_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_PCIE0_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_PCIE0_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_PCIE2_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_PCIE2_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_PCIE3_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_PCIE3_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_DSLCPU_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_DSLCPU_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_PER_DMA_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_PER_DMA_BASE,0);

	// ubus XRDP
	mst_registered = ubus4_xrdp_client_registration->MstStatus[0];
    if (mst_registered &       (1<<MST_PORT_NODE_QM_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_QM_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_DQM_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_DQM_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_DMA0_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_DMA0_BASE,0);
    if (mst_registered &       (1<<MST_PORT_NODE_NATC_BIT_POS)) 
		dcm_ubus_threshold_wr(MST_PORT_NODE_NATC_BASE,0);

	// Due to RTL design issue, it is required to set this to 1 instead of 0
    d32 = 1<<XRDP_RCQ_GENERAL_DMA_ARB_CFG_CONGEST_THRESHOLD_SHIFT;
    *((volatile uint32_t*) XRDP_RCQ_GEN_CFG) = d32;

    // always use DCM clock
    UBUS4CLK->ClockCtrl 	&= ~UBUS4_CLK_BYPASS_MASK;  // always use DCM clock
    UBUS4XRDPCLK->ClockCtrl &= ~UBUS4_CLK_BYPASS_MASK;  // always use DCM clock

    dcm_divider_set(DCM_DIVIDER_DEFAULT);
    dcm_thold_set(DCM_THOLD_TO_HIGH_FREQ_DEFAULT, 
				  DCM_THOLD_TO_LOW_FREQ_DEFAULT);
        
}

#ifdef  DEBUG		
static void dcm_config_get(void)	
{

#define DCM_THOLD(base) \
    ((MstPortNode*) base )->port_cfg[DCM_UBUS_CONGESTION_THRESHOLD]

    printk("UBUS Congestion Threshold Register value for:\n");
    printk("  SYS_BASE     = 0x%08x\n",DCM_THOLD(UBUS_SYS_MODULE_BASE      ));
    printk("  SYS_XRDP_BASE= 0x%08x\n",DCM_THOLD(UBUS_SYS_MODULE_XRDP_BASE ));

    printk("  B53_BASE     = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_B53_BASE    ));
    printk("  DSL_BASE     = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_DSL_BASE    ));
    printk("  PER_BASE     = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_PER_BASE    ));
    printk("  PMC_BASE     = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_PMC_BASE    ));
    printk("  SPU_BASE     = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_SPU_BASE    ));
    printk("  USB_BASE     = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_USB_BASE    ));
    printk("  PCIE0_BASE   = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_PCIE0_BASE  ));
    printk("  PCIE2_BASE   = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_PCIE2_BASE  ));
    printk("  PCIE3_BASE   = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_PCIE3_BASE  ));
    printk("  DSLCPU_BASE  = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_DSLCPU_BASE ));
    printk("  PER_DMA_BASE = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_PER_DMA_BASE));

    printk("  \n");
    printk("  DMA0_BASE    = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_DMA0_BASE   ));
    printk("  DMA1_BASE    = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_DMA1_BASE   ));
    printk("  DQM_BASE     = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_DQM_BASE    ));
    printk("  NATC_BASE    = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_NATC_BASE   ));
    printk("  QM_BASE      = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_QM_BASE     ));
    printk("  RQ0_BASE     = 0x%08x\n",DCM_THOLD(MST_PORT_NODE_RQ0_BASE    ));

    printk("\n");
	printk("%s = 0x%08x\n", 
    	"RCQ_GENERAL_CONFIG_RCQ_COMMON_REGS_RCQ_GENERAL_CONFIG_DMA_ARB_CFG", 
        (uint32_t) *((uint64_t*) XRDP_RCQ_GEN_CFG));
    printk("\n");
    printk("UBUS4CLK    ->ClockCtrl = 0x%08x\n", UBUS4CLK    ->ClockCtrl);
    printk("UBUS4XRDPCLK->ClockCtrl = 0x%08x\n", UBUS4XRDPCLK->ClockCtrl);
    printk("\n");
    printk("UBUS4CLK    ->Min2Mid_threshhold = 0x%04x\n", 
			UBUS4CLK    ->Min2Mid_threshhold);
    printk("UBUS4CLK    ->Mid2Max_threshhold = 0x%04x\n", 
			UBUS4CLK    ->Mid2Max_threshhold);
    printk("UBUS4XRDPCLK->Min2Mid_threshhold = 0x%04x\n", 
			UBUS4XRDPCLK->Min2Mid_threshhold);
    printk("UBUS4XRDPCLK->Mid2Max_threshhold = 0x%04x\n", 
			UBUS4XRDPCLK->Mid2Max_threshhold);
    printk("\n");
    printk("UBUS4CLK    ->Mid2Min_threshhold = 0x%04x\n", 
			UBUS4CLK    ->Mid2Min_threshhold);
    printk("UBUS4CLK    ->Max2Mid_threshhold = 0x%04x\n", 
			UBUS4CLK    ->Max2Mid_threshhold);
    printk("UBUS4XRDPCLK->Mid2Min_threshhold = 0x%04x\n", 
			UBUS4XRDPCLK->Mid2Min_threshhold);
    printk("UBUS4XRDPCLK->Max2Mid_threshhold = 0x%04x\n", 
			UBUS4XRDPCLK->Max2Mid_threshhold);
}
#endif // DEBUG

//===========================================
// function declaration in /sys/module/ubus4_dcm/parameter/* 
//===========================================

static int ubus4_dcm_enable_get (char *buffer, struct kernel_param *kp)
{
    return sprintf(buffer, "%d\t : valid value { 0, 1}", dcm_enable_get());
}

static int ubus4_dcm_enable_set(const char *val, struct kernel_param *kp)
{
    if (!strncmp(val, "0", 1)) {
		dcm_enable_set (UBUS_DCM_OFF);	
    }  else if (!strncmp(val, "1", 1)) {
		dcm_enable_set (UBUS_DCM_ON);	
	} else {
        return -EINVAL;
	}
    return 0;
}

static int ubus4_dcm_divider_get (char *buffer, struct kernel_param *kp)
{
    return sprintf(buffer, 
				"%d\t : valid value { 1, 2, 3, 4, 5, 6, 7}", 
				dcm_divider_get());
}

static int ubus4_dcm_divider_set(const char *val, struct kernel_param *kp)
{
	u32 ival;

	kstrtouint (val, 0, &ival);

    if (ival>=1 && ival <=7) {
		dcm_divider_set(ival);
    } else {
		return -EINVAL;	// invalid input
    }
    return 0;
}

static int ubus4_dcm_thold_get (char *buffer, struct kernel_param *kp)
{
    return sprintf(buffer, 
		"0x%04x\t : valid value {1..0xFFFF}",dcm_thold_get());
}

static int ubus4_dcm_thold_set(const char *val, struct kernel_param *kp)
{
	u32	ival;

	kstrtouint (val, 0, &ival);

    if ((ival&0xFFFF) && ((ival&0xFFFF) <= 0xFFFF)) {
		dcm_thold_set (1,ival);
	} else {
		return -EINVAL;	// invalid input
	}
    return 0;
}

#ifdef DEBUG
static int ubus4_dcm_config_get (char *buffer, struct kernel_param *kp)
{
	dcm_config_get();	// for debug purpose
    return sprintf(buffer, "valid value { any, don't care }");
}
#endif

static int ubus4_dcm_config_set(const char *val, struct kernel_param *kp)
{
	dcm_config_set();
    return 0;
}

// registering parameters in /sys/module/ubus4_dcm/parameter directory
#define DCM_MPC(a,b,c,d,e) module_param_call(a,b,c,d,e)
DCM_MPC(enable,    ubus4_dcm_enable_set,   ubus4_dcm_enable_get,   NULL, 0644);
DCM_MPC(divider,   ubus4_dcm_divider_set,  ubus4_dcm_divider_get,  NULL, 0644);
DCM_MPC(threshold, ubus4_dcm_thold_set,    ubus4_dcm_thold_get,    NULL, 0644);
#ifdef DEBUG
DCM_MPC(config,  ubus4_dcm_config_set, ubus4_dcm_config_get, NULL, 0644);
#else
DCM_MPC(config,  ubus4_dcm_config_set, NULL, NULL, 0644);
#endif


// Note: IMPORTANT      
// other kernel module codes come after may change default setup.
// it is recommanded to re-enable this feature when everything is up.

// called when kernal boot options is specified : ubus4_dcm=disable/enable
static int __init ubus_dcm_init_param(char *str)
{
	dcm_config_set();
	if (!strcmp(str, "disable")) {
	 	dcm_enable_set(UBUS_DCM_OFF);
	} else {
	 	dcm_enable_set(UBUS_DCM_ON);
	}
	return 1;
}

__setup("ubus4_dcm=",ubus_dcm_init_param);

static int __init ubus_dcm_init(void)
{
	dcm_config_set();
	dcm_enable_set(UBUS_DCM_ON);	// default ON 
	return 0;
}

late_initcall(ubus_dcm_init);

