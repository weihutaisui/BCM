/*
<:copyright-BRCM:2017:DUAL/GPL:standard 

   Copyright (c) 2017 Broadcom 
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

/*
 ****************************************************************************
 * File Name  : bcm_i2c.c
 *
 * Description: This file contains the platform dependent cod for detecting 
 *    and adding i2c device to the system
 ***************************************************************************/

#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#include <bcm_intr.h>
#include <boardparms.h>
#include <board.h>
#include <bcmsfp_i2c.h>

#define MAX_I2C_ADDR         8

#define BCM_I2C_DEBUG
#ifdef BCM_I2C_DEBUG
#define BCM_I2C_LOG(fmt, args...) printk("bcm_i2c: %s " fmt,  __FUNCTION__, ##args)
#else
#define BCM_I2C_LOG(fmt, args...)
#endif

struct bcm_i2c_platform_data {
    struct i2c_client* i2c_clients[MAX_I2C_ADDR];
    /* sfp related stuff */
    int sfp_status;
    unsigned short sfp_intr_num;
    unsigned short sfp_mod_abs_gpio;
    int sfp_polling;
    int sfp_intr;
    int is_pmd;
}bcm_i2c_platform_data;

struct sfp_work {
	struct delayed_work dwork;
	int    bus;
} sfp_work;


static struct bcm_i2c_platform_data bcm_i2c_data[MAX_I2C_BUS];

static unsigned short normal_i2c[] = { 0x0, I2C_CLIENT_END };

static struct i2c_board_info sfp_board_info[MAX_SFP_I2C_ADDR]  = {
    {   /* SFP EEPROM client */
        I2C_BOARD_INFO("sfp_eeprom", SFP_I2C_EEPROM_ADDR),
    },
    {  	/* SFP diagnostic & monitor cient */
        I2C_BOARD_INFO("sfp_diag", SFP_I2C_DIAG_ADDR),
    },
    {	/* SFP Eth PHY client*/
	I2C_BOARD_INFO("sfp_phy", SFP_I2C_PHY_ADDR),
    },
};

static struct i2c_board_info pmd_board_info[MAX_PMD_I2C_ADDR]  = {
    {   /* pmd reg client */
        I2C_BOARD_INFO("pmd_reg", PMD_I2C_REG_ADDR),
    },
    {  	/* SFP diagnostic & monitor cient */
        I2C_BOARD_INFO("pmd_iram", PMD_I2C_IRAM_ADDR),
    },
    {	/* SFP client on address 0x52, not sure what it is*/
	I2C_BOARD_INFO("pmd_dram", PMD_I2C_DRAM_ADDR),
    },
};

static struct sfp_work sfp_delay_work[MAX_I2C_BUS];

static struct blocking_notifier_head sfp_notifier;
static DECLARE_RWSEM(sfp_sts_sem);

static int skip_i2c_client_det = 0;

static int is_sfp_plugin(unsigned short gpio);
static int add_sfp_i2c_client(int bus);
static int remove_sfp_i2c_client(int bus);

static int check_sfp_status(int bus, struct bcm_i2c_platform_data* pdata)
{
    if( is_sfp_plugin(pdata->sfp_mod_abs_gpio) ) {
        if( pdata->sfp_status == SFP_STATUS_REMOVED ) {
            BCM_I2C_LOG("sfp plugged in, add clients...\n");
            if( add_sfp_i2c_client(bus) == 0 )
                blocking_notifier_call_chain(&sfp_notifier, SFP_STATUS_INSERTED, (void*)((uintptr_t)&bus));
        } else if( pdata->sfp_status == SFP_STATUS_INSERTED && skip_i2c_client_det ) {
            /* UGLY workaround for 6858, which always add sfp during the init, we just have to notify 
             the listener that sfp is really plugged in */
            blocking_notifier_call_chain(&sfp_notifier, SFP_STATUS_INSERTED, (void*)((uintptr_t)&bus));
        }
    } else {
        if( pdata->sfp_status == SFP_STATUS_INSERTED ) {
            BCM_I2C_LOG("sfp plugged out, remove clients...\n");
            if( remove_sfp_i2c_client(bus) == 0 )
                blocking_notifier_call_chain(&sfp_notifier, SFP_STATUS_REMOVED, (void*)((uintptr_t)&bus));
        }
    }

    return 0;
}

static void sfp_cb(struct work_struct *work_arg)
{
    int bus;
    struct delayed_work *delay_work;
    struct sfp_work *sfp_delay_work;
    struct bcm_i2c_platform_data* pdata;
	
    delay_work = container_of(work_arg, struct delayed_work, work);
    sfp_delay_work = container_of(delay_work, struct sfp_work, dwork);

    bus = sfp_delay_work->bus;
    pdata = &bcm_i2c_data[bus];

    check_sfp_status(bus, pdata);
}

static irqreturn_t sfp_isr(int irq, void *arg)
{
    int bus = (int)((uintptr_t)arg);
    //BCM_I2C_LOG("sfp_isr interrupt called irq %d for i2c bus %d!\n", irq, bus);
    
    if( bus < MAX_I2C_BUS )   
        schedule_delayed_work(&sfp_delay_work[bus].dwork, HZ/3); 
    
#if !defined(CONFIG_ARM) && !defined(CONFIG_ARM64)
    BcmHalInterruptEnable(irq);
#else
    BcmHalExternalIrqClear(irq);
#endif

    return IRQ_HANDLED;
}

static int is_sfp_plugin(unsigned short gpio)
{
    int gpioActHigh = 0, plugin = 0;
    unsigned int value = 0;

    gpioActHigh = gpio&BP_ACTIVE_LOW ? 0 : 1;
    value = kerSysGetGpioValue(gpio);

    if( (value&&gpioActHigh) || (!value&&!gpioActHigh) )
        plugin = 1;

    return plugin;
}

static int sfp_probe(struct i2c_adapter * adap, unsigned short addr)
{
    char buf[2];
    struct i2c_msg msg[2];
    int ret = 0, try = 0, bus;
    struct bcm_i2c_platform_data* pdata;

    if( skip_i2c_client_det )
        return 1;

    bus = adap->nr;
    if( bus < MAX_I2C_ADDR ) { 
        pdata = &bcm_i2c_data[bus];
        if( pdata->is_pmd ) { /* pmd device does not need probe. i2c client always exists */
            return 1;
        }
    }

    while( try < 3) {
        msg[0].addr = msg[1].addr = addr;
        msg[0].flags = msg[1].flags = 0;

        msg[0].len = 1;
        buf[0] = 0x0;
        msg[0].buf = buf;

        msg[1].flags |= I2C_M_RD;
        if( addr == SFP_I2C_PHY_ADDR )
            msg[1].len = 0x2; /* PHY Register is 16 bit */
        else
            msg[1].len = 0x1;
        msg[1].buf = buf;

        ret = i2c_transfer(adap, msg, 2);
        //BCM_I2C_LOG("try %d own probe for client addr 0x%x ret %d reg 0 0x%x\n", try, addr, ret, buf[0]);
        if( ret == 2 )
            break;
        try++;
	msleep(50);
    }

    return (ret != 2 ) ? 0 : 1;
}


static int add_sfp_i2c_client(int bus)
{
    int i ,rc = 0, num_i2c_addr;
    struct i2c_adapter *i2c_adap = NULL;
    struct bcm_i2c_platform_data* pdata = &bcm_i2c_data[bus];
    struct i2c_board_info* board_info;

    i2c_adap =  i2c_get_adapter(bus);
    if( i2c_adap == NULL ) {
        return -1;
    }

    if( pdata->is_pmd ) {
        num_i2c_addr = MAX_PMD_I2C_ADDR;
        board_info = pmd_board_info;
    } else {
        num_i2c_addr = MAX_SFP_I2C_ADDR;
        board_info = sfp_board_info;
    }
    
    for( i = 0; i < num_i2c_addr && board_info->addr; i++ ) {
        normal_i2c[0] = board_info->addr;
        pdata->i2c_clients[i] = i2c_new_probed_device(i2c_adap, board_info, normal_i2c, sfp_probe);
        if( pdata->i2c_clients[i] == NULL ) {
            BCM_I2C_LOG("i2c device at address 0x%x does not exist!\n", board_info->addr);
        }
        board_info++;
    }
    i2c_put_adapter(i2c_adap);

    down_write(&sfp_sts_sem);
    pdata->sfp_status = SFP_STATUS_INSERTED;
    up_write(&sfp_sts_sem);

    return rc;
}

static int remove_sfp_i2c_client(int bus)
{
    int i;
    struct bcm_i2c_platform_data* pdata = &bcm_i2c_data[bus];

    for( i = 0; i < MAX_SFP_I2C_ADDR; i++ ) {
        if ( pdata->i2c_clients[i] )
            i2c_unregister_device( pdata->i2c_clients[i]);
    }

    down_write(&sfp_sts_sem);
    pdata->sfp_status = SFP_STATUS_REMOVED;
    up_write(&sfp_sts_sem);

    return 0;
}

#ifdef CONFIG_BP_PHYS_INTF

static int find_sfp_i2c_bus_in_intf(int bus, unsigned short intf_type)
{
    int i, intf_num;
    unsigned short mgmt_type, mgmt_bus_num;

    if((intf_num = BpGetPhyIntfNumByType(intf_type))) {
        for( i = 0; i < intf_num; i++ ) {
            if( BpGetIntfMgmtType(intf_type, i, &mgmt_type ) == BP_SUCCESS && mgmt_type == BP_INTF_MGMT_TYPE_I2C ) {
                if( BpGetIntfMgmtBusNum(intf_type, i, &mgmt_bus_num) == BP_SUCCESS && mgmt_bus_num == bus ) {
                    return i; 
	        }
            }
        }
    }

    return -1;
}
#endif

static int get_num_of_i2c_bus(void) 
{
    int num_i2c_bus;

#ifdef CONFIG_BP_PHYS_INTF
    num_i2c_bus = BpGetPhyIntfNumByType(BP_INTF_TYPE_I2C);
#else
    /* assume only one i2c bus for the old style board parameter */
    num_i2c_bus = 1;
#endif

    return num_i2c_bus;
}

static int sfp_polling_func(void * arg) 
{
    int i, num_i2c_bus;
    struct bcm_i2c_platform_data* pdata;

    num_i2c_bus =  get_num_of_i2c_bus();
    while(1) {
        for( i = 0; i < num_i2c_bus; i++ ) {
            pdata = &bcm_i2c_data[i];
            if(pdata->sfp_polling) {
                check_sfp_status(i, pdata);
            }
        }
        msleep(300);
    }

    return 0;
}


static int create_sfp_polling_thread(void)
{
    int i, num_i2c_bus, polling = 0; 
    struct bcm_i2c_platform_data* pdata;
    struct task_struct *  thread;

    num_i2c_bus =  get_num_of_i2c_bus();
    for( i = 0; i < num_i2c_bus; i++ ) {
        pdata = &bcm_i2c_data[i];
        if(pdata->sfp_polling) {
            polling = 1;
            break;
        }
    }

    if(polling) {
        thread = kthread_run(sfp_polling_func, NULL, "sfp_polling");
	if(!thread) {
            BCM_I2C_LOG("failed to create threada!!!\n");
            return -1;
	}
        BCM_I2C_LOG("thread created\n"); 
    }
   
    return 0;
}

static int check_sfp_i2c_device(int bus, struct bcm_i2c_platform_data* pdata)
{
#ifdef CONFIG_BP_PHYS_INTF
    unsigned short intf_type;
    int intf_idx;
#endif
#if defined(CONFIG_BCM_PON) || defined(CONFIG_BCM963158)
    unsigned short txen, rstn, optic_type = (unsigned short)(-1);
    int rc;
#endif
    int add_sfp = 0;

#ifdef CONFIG_BP_PHYS_INTF
    if( bcm_i2c_sfp_get_intf(bus, &intf_type, &intf_idx) != 0 )
        return 0;

    BCM_I2C_LOG("i2c bus %d used by intf type %d idx %d\n", bus, intf_type, intf_idx);
    /* check if SFP use interrupt */
    if (BpGetOpticalModulePresenceExtIntr(intf_type, intf_idx, &pdata->sfp_intr_num) == BP_SUCCESS && pdata->sfp_intr_num != BP_EXT_INTR_NONE) {
#else
    if (BpGetOpticalModulePresenceExtIntr(&pdata->sfp_intr_num) == BP_SUCCESS && pdata->sfp_intr_num != BP_EXT_INTR_NONE) {
#endif
        /* check if the sfp is already plugged in */
#ifdef CONFIG_BP_PHYS_INTF
       if( BpGetOpticalModulePresenceExtIntrGpio(intf_type, intf_idx, &pdata->sfp_mod_abs_gpio) == BP_SUCCESS && pdata->sfp_mod_abs_gpio != BP_GPIO_NONE) {
#else
       if( BpGetOpticalModulePresenceExtIntrGpio(&pdata->sfp_mod_abs_gpio) == BP_SUCCESS && pdata->sfp_mod_abs_gpio != BP_GPIO_NONE ) {
#endif
            pdata->sfp_intr = 1;
            BCM_I2C_LOG("i2c bus %d sfp detection using interrupt\n", bus);
            kerSysSetGpioDirInput(pdata->sfp_mod_abs_gpio);
            if( is_sfp_plugin(pdata->sfp_mod_abs_gpio) ) {
                 BCM_I2C_LOG("sfp plugged in when power on, add clients...\n");
                 add_sfp = 1;
	    }
        }
    } else {
        /* check if it has sfp mod_abs gpio pin for polling */
#ifdef CONFIG_BP_PHYS_INTF
        if (BpGetSfpModDetectGpio(intf_type, intf_idx, &pdata->sfp_mod_abs_gpio) == BP_SUCCESS ) {
#else
        if (BpGetSfpDetectGpio(&pdata->sfp_mod_abs_gpio) == BP_SUCCESS ) {
#endif
            BCM_I2C_LOG("i2c bus %d sfp detection using polling\n", bus);
            pdata->sfp_polling = 1;
            kerSysSetGpioDirInput(pdata->sfp_mod_abs_gpio);
            if( is_sfp_plugin(pdata->sfp_mod_abs_gpio) ) {
                 BCM_I2C_LOG("sfp plugged in when power on, add clients...\n");
	    }
        }
    }

#if defined(CONFIG_BCM_PON) || defined(CONFIG_BCM963158)
    rc = BpGetGponOpticsType(&optic_type);
#if defined(CONFIG_BCM96858)
    /* UGLY workaround for 6858.  6858 board paramter does not differentiate SFP and SFF board and
       can not add a new board id for SFF board due to field upgrade compability issue.  So always add
       i2c device and no detection of i2c client.  This mimics the old i2c code behavior */
    if( rc == BP_SUCCESS ) {
        add_sfp = 1;
        skip_i2c_client_det = 1;
        BCM_I2C_LOG("i2c bus %d always add clients for 6858 board\n", bus);
    }
#else
    /* if not using interrupt/polling but board has xPON optical module soldered down, then
       sfp is always present, assuming only have one xPON or SGMII interface in the board */
    if( pdata->sfp_intr == 0 && pdata->sfp_polling == 0 && rc == BP_SUCCESS
#ifdef CONFIG_BP_PHYS_INTF
        && intf_type == BP_INTF_TYPE_xPON
#endif
    )
    {
        add_sfp = 1;
        BCM_I2C_LOG("i2c bus %d sfp soldered on board, add clients...\n", bus);
    }
#endif

    /* some SFP need VDDT to power on */
#ifdef CONFIG_BP_PHYS_INTF
    if ( intf_type == BP_INTF_TYPE_xPON )
#endif
    {
        if( BpGetPonTxEnGpio(&txen) == BP_SUCCESS ) {
            kerSysSetGpioDir(txen);
            kerSysSetGpioState(txen, kGpioActive);
        }
        /* PMD device need taken out of reset */
        if( optic_type == BP_GPON_OPTICS_TYPE_PMD ) {
            pdata->is_pmd = 1;
            if (BpGetGpioPmdReset(&rstn) == BP_SUCCESS) {
                kerSysSetGpioDir(rstn);
                kerSysSetGpioState(rstn, kGpioInactive);
                kerSysSetGpioState(rstn, kGpioActive);
                BCM_I2C_LOG("PMD taken out of reset\n");
            }
        }
    }
#endif

    if( pdata->sfp_intr ) {
        INIT_DELAYED_WORK(&sfp_delay_work[bus].dwork, sfp_cb);
        sfp_delay_work[bus].bus = bus;
        ext_irq_connect(pdata->sfp_intr_num, (void *)((uintptr_t)bus), (FN_HANDLER)sfp_isr);
    }

    return add_sfp;
}

static __init int bcm_add_i2c(void)
{
    int ret = 0, i, num_i2c_bus, add_sfp = 0;
    struct bcm_i2c_platform_data* pdata;

    BLOCKING_INIT_NOTIFIER_HEAD(&sfp_notifier);

    memset((void*)bcm_i2c_data, 0x0, sizeof(bcm_i2c_data));

    num_i2c_bus =  get_num_of_i2c_bus();

    for( i = 0; i < num_i2c_bus && i < MAX_I2C_BUS; i++ ) {
        pdata = &bcm_i2c_data[i];

        /* check sfp i2c device first */
        add_sfp =  check_sfp_i2c_device(i, pdata);

        if( add_sfp ) {
            ret = add_sfp_i2c_client(i);
        }

	/* add any other i2c devcie in this bus */
    }

    /* add polling thread if SFP does not support interrupt */
    ret = create_sfp_polling_thread();

    return ret;
}


int bcm_i2c_sfp_register_notifier(struct notifier_block* nb)
{
    return blocking_notifier_chain_register(&sfp_notifier, nb);
}
EXPORT_SYMBOL(bcm_i2c_sfp_register_notifier);

int bcm_i2c_sfp_unregister_notifier(struct notifier_block* nb)
{
    return blocking_notifier_chain_unregister(&sfp_notifier, nb);
}
EXPORT_SYMBOL(bcm_i2c_sfp_unregister_notifier);

int bcm_i2c_sfp_get_status(int bus)
{
    int status = SFP_STATUS_INVALID;

    if( bus < MAX_I2C_BUS )  {
        down_read(&sfp_sts_sem);
        status = bcm_i2c_data[bus].sfp_status;
        up_read(&sfp_sts_sem);
    }

    return status;
}
EXPORT_SYMBOL(bcm_i2c_sfp_get_status);

#ifdef CONFIG_BP_PHYS_INTF
/*Given the interface type and index, find the i2c bus number */
int bcm_i2c_sfp_get_bus_num(unsigned short intf_type, int intf_idx, int *bus)
{
    int ret = -1;
    unsigned short mgmt_type, bus_num;

    if( BpGetIntfMgmtType(intf_type, intf_idx, &mgmt_type ) == BP_SUCCESS && 
         mgmt_type == BP_INTF_MGMT_TYPE_I2C ) 
    {
        if( BpGetIntfMgmtBusNum(intf_type, intf_idx, &bus_num) == BP_SUCCESS ) {
            *bus = bus_num;
            ret = 0;
        }
    }

    return ret;
}
EXPORT_SYMBOL(bcm_i2c_sfp_get_bus_num);

/*Given the i2c bus number, find interface type and index */
int bcm_i2c_sfp_get_intf(int bus, unsigned short* intf_type, int* intf_idx)
{
    /* Only xPON, SGMII interface has sfp */
    *intf_idx = find_sfp_i2c_bus_in_intf(bus, BP_INTF_TYPE_xPON);
    if( *intf_idx != - 1) {
        *intf_type = BP_INTF_TYPE_xPON;
	return 0;
    }

    *intf_idx = find_sfp_i2c_bus_in_intf(bus, BP_INTF_TYPE_SGMII);
    if( *intf_idx != - 1) {
        *intf_type = BP_INTF_TYPE_SGMII;
	return 0;
    }

    return -1;
}
EXPORT_SYMBOL(bcm_i2c_sfp_get_intf);
#endif

static void bcm_remove_i2c(void)
{
    int bus;
    for( bus = 0; bus < MAX_I2C_BUS; bus++ )
        remove_sfp_i2c_client(bus);

    return;
}

module_init(bcm_add_i2c);
module_exit(bcm_remove_i2c);

MODULE_LICENSE("GPL");
