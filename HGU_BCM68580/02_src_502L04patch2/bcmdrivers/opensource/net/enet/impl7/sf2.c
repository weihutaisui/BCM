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

/*
 *  Created on: Dec/2016
 *      Author: steven.hsieh@broadcom.com
 */

#include "port.h"
#include "enet.h"
#include "runner.h"
#include "sf2.h"
#include <rdpa_api.h>
#include <bcmnet.h>
#include "crossbar_dev.h"
#include "phy_drv_sf2.h"

#include <linux/bcm_skb_defines.h>
#include <linux/rtnetlink.h>

#include "linux/bcm_log.h"
#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
#include "spdsvc_defs.h"
static bcmFun_t *enet_spdsvc_transmit;
#endif

#ifdef PKTC
#include <osl.h>
#endif

#include "pmc_switch.h"

#define BCM_PORT_FROM_TYPE2_TAG(tag) (tag & 0x1f) // Big Endian

#define BRCM_TYPE2               0x888A // Big Endian
#define BRCM_TAG_TYPE2_LEN       4
#define BRCM_TAG2_EGRESS         0x2000

#undef OFFSETOF
#define OFFSETOF(STYPE, MEMBER)     ((size_t) &((STYPE *)0)->MEMBER)


// based on enet\shared\bcmenet_common.h
typedef struct {
    unsigned char da[6];
    unsigned char sa[6];
    uint16 brcm_type;
    uint16 brcm_tag;
    uint16 encap_proto;
} __attribute__((packed)) BcmEnet_hdr2;

#include "bcm_map_part.h"
#include "boardparms.h"
#include "bcmmii.h"
#include "bcmmii_xtn.h"


#define DATA_TYPE_HOST_ENDIAN   (0x00<<24)
#define DATA_TYPE_BYTE_STRING   (0x01<<24)
#define DATA_TYPE_VID_MAC       (0x02<<24)
#define DATA_TYPE_MIB_COUNT     (0x03<<24)

// =========== global/static variables ====================
struct semaphore bcm_link_handler_config;

enetx_port_t *sf2_sw;   /* external SF2 switch */

#define SF2SW_RREG      port_sf2_sw.rreg
#define SF2SW_WREG      port_sf2_sw.wreg

/* static vars based on impl5\bcmsw.c */
static uint16_t sf2_dis_learning_ext = 0x0100; /* This default value does not matter */
static uint8_t  sf2_hw_switching_state = HW_SWITCHING_ENABLED;


/* ETHSWPRIOCONTROL, ETHSWQUEMAP related variables  based from impl5/bcmsw.c*/
static int lanUpPorts_g, wanUpPorts_g;     // TODO_DSL? in impl5 pVnetDev0_g->lanUpPorts, wanUpPorts  need to init and setup
#define total_ports (root_sw->s.port_count + sf2_sw->s.port_count)  // TODO_DSL? there should be more elegant way to get total ports

#define DefaultWANQueBitMap 0xaa
#define DefaultQueNoRemap 0x76543210 /* No remapping constant */
#define DefaultQueRemap 0x77553311 /* Default Map CPU Traffic from Queue 0 to 1 to get basic WAN gurantee */
#define MaxStreamNumber 40                                              // MaxStreamNumber
static uint32_t wanQueMap = DefaultWANQueBitMap,
                maxStreamNumber = MaxStreamNumber, queThreConfMode;
uint32_t queRemap = DefaultQueRemap;
static uint16_t sf2_sw_port_thred[FC_THRED_TOTAL_TYPES][FC_LAN_TXQ_QUEUES];     // sf2_sw_port_thred
static uint16_t sf2_imp0_thred[FC_THRED_TOTAL_TYPES][FC_LAN_TXQ_QUEUES];        // sf2_imp0_thred

#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
#if defined(CONFIG_BCM94908)
    #define DEFAULT_IMP_PBMAP       (PBMAP_MIPS|PBMAP_P5_IMP|PBMAP_P4_IMP)
    const int imp_to_emac[BP_MAX_SWITCH_PORTS+1] = {-1,-1,-1,-1,rdpa_emac2,rdpa_emac1,-1,-1,rdpa_emac0};

    /* Below mapping is used when P7 comes up at 2.5G link speed */
    int port_imp_map_2_5g[BP_MAX_SWITCH_PORTS] = 
                        {
                            /* P0 */  P4_PORT_ID,
                            /* P1 */  P5_PORT_ID,
                            /* P2 */  P5_PORT_ID,
                            /* P3 */  P5_PORT_ID, /* 5 /*/
                            /* P4 */  -1,   /* IMP Port */
                            /* P5 */  -1,   /* IMP Port */
                            /* P6 */  -1,   /* Unused/undefined switch port */
                            /* P7 */  IMP_PORT_ID /* 8 */
                        };
    /* Below mapping is used when P7 comes up at below 2.5G link speed */
    int port_imp_map_non_2_5g[BP_MAX_SWITCH_PORTS] = 
                        {
                            /* P0 */  P4_PORT_ID,
                            /* P1 */  P5_PORT_ID,
                            /* P2 */  P5_PORT_ID,
                            /* P3 */  IMP_PORT_ID, /* 8 /*/
                            /* P4 */  -1,   /* IMP Port */
                            /* P5 */  -1,   /* IMP Port */
                            /* P6 */  -1,   /* Unused/undefined switch port */
                            /* P7 */  IMP_PORT_ID /* 8 */
                        };

#elif defined(CONFIG_BCM963158)
    #define DEFAULT_IMP_PBMAP       (PBMAP_MIPS|PBMAP_P7_IMP|PBMAP_P5_IMP)
    const int imp_to_emac[BP_MAX_SWITCH_PORTS+1] = {-1,-1,-1,-1,-1,rdpa_emac1,-1,rdpa_emac2,rdpa_emac0};

    /* for 653158, both map_2_5g and map_non_2_5g are the same */
    #define port_imp_map_non_2_5g   port_imp_map_2_5g
    
    int port_imp_map_2_5g[BP_MAX_SWITCH_PORTS] = 
                        {
                            /* P0 */  P5_PORT_ID,
                            /* P1 */  P5_PORT_ID,
                            /* P2 */  P5_PORT_ID,
                            /* P3 */  P7_PORT_ID,
                            /* P4 */  P7_PORT_ID,
                            /* P5 */  -1,   /* IMP Port */
                            /* P6 */  IMP_PORT_ID, /* 8 */
                            /* P7 */  -1,   /* IMP Port */
                        };

#endif //defined(CONFIG_BCM963158)
uint32_t imp_pbmap[BP_MAX_ENET_MACS] = {[0 ... (BP_MAX_ENET_MACS-1)] = DEFAULT_IMP_PBMAP};
static int port_imp_emac_map[BP_MAX_SWITCH_PORTS] = {[0 ... (BP_MAX_SWITCH_PORTS-1)] = -1}; 
#endif

/* Deep Green Mode enabled flag currently tied to the WebGUI's "Advanced Setup -> Power Management -> Ethernet Auto Power Down & Sleep" checkbox */
#if defined(CONFIG_BCM_ETH_DEEP_GREEN_MODE)
static int deep_green_mode_enabled = 1;      //Keep track of whether Deep Green Mode feature is enabled/disabled
static int deep_green_mode_activated = 0;    //Keep track of whether Deep Green Mode feature is activated/deactivated (DGM is activated when the feature is enabled and all ports are disconnected)

static uint32 reg_low_power_exp1;  /* Store REG_LOW_POWER_EXP1 register value before enabling Deep Green Mode so that we can restore value when disabling Deep Green Mode */
#endif

// ----------- static SF2 functions -----------------------
static inline void dumpHexData1(uint8_t *pHead, uint32_t len)
{
    uint32_t i;
    uint8_t *c = pHead;
    for (i = 0; i < len; ++i) {
        if (i % 16 == 0)
            printk("\n");
        printk("0x%02X, ", *c++);
    }
    printk("\n");
}

static int ethsw_phy_pll_up(int ephy_and_gphy)
{
#if defined(CONFIG_BCM963268)
    int ephy_status_changed = 0;

#if NUM_INT_GPHYS > 0
    if (ephy_and_gphy)
    {
        int i;
        uint32 roboswGphyCtrl = GPIO->ROBOSWGPHYCTRL;

        /* Bring up internal GPHY PLLs if they are down */
        for (i = 0; i < NUM_INT_GPHYS; i++)
        {
            if ((roboswGphyCtrl & gphy_pwr_dwn[i]) && !(ephy_forced_pwr_down_status & (1<<PHYSICAL_PORT_TO_LOGICAL_PORT(i+NUM_INT_EPHYS, 0))))
            {
                roboswGphyCtrl &= ~gphy_pwr_dwn[i];
                ephy_status_changed = 1;
            }
        }
        if (ephy_status_changed) {
            GPIO->ROBOSWGPHYCTRL = roboswGphyCtrl;
        }
    }
#endif

    /* This is a safety measure in case one tries to access the EPHY */
    /* while the PLL/RoboSw is powered down */
#if defined(ROBOSW250_CLK_EN)
    if (!(PERF->blkEnables & ROBOSW250_CLK_EN))
    {
        /* Enable robosw clock */
#if defined(ROBOSW025_CLK_EN)
        PERF->blkEnables |= ROBOSW250_CLK_EN | ROBOSW025_CLK_EN;
#else
        PERF->blkEnables |= ROBOSW250_CLK_EN;
#endif
        ephy_status_changed = 1;
    }
#endif

    if (ephy_status_changed) {
        if (irqs_disabled() || (preempt_count() != 0)) {
            mdelay(PHY_PLL_ENABLE_DELAY);
        } else {
            msleep(PHY_PLL_ENABLE_DELAY);
        }
        return (msecs_to_jiffies(PHY_PLL_ENABLE_DELAY));
    }
#endif
    return 0;
}

static void _fast_age_start_done_ext(uint8_t ctrl)
{
    uint8_t timeout = 100;

    SF2SW_WREG(PAGE_CONTROL, REG_FAST_AGING_CTRL, &ctrl, 1);
    SF2SW_RREG(PAGE_CONTROL, REG_FAST_AGING_CTRL, &ctrl, 1);
    while (ctrl & FAST_AGE_START_DONE) {
        mdelay(1);
        SF2SW_RREG(PAGE_CONTROL, REG_FAST_AGING_CTRL, &ctrl, 1);
        if (!timeout--) {
            printk("Timeout of fast aging \n");
            break;
        }
    }

    /* Restore DYNAMIC bit for normal aging */
    ctrl = FAST_AGE_DYNAMIC;
    SF2SW_WREG(PAGE_CONTROL, REG_FAST_AGING_CTRL, &ctrl, 1);
}

void _fast_age_all_ext(uint8_t age_static)
{
    uint8_t v8;

    v8 = FAST_AGE_START_DONE | FAST_AGE_DYNAMIC;
    if (age_static) {
        v8 |= FAST_AGE_STATIC;
    }

    _fast_age_start_done_ext(v8);
}

static int _ethsw_phy_access(struct ethswctl_data *e, enetx_port_t *port)
{
    // based on shared\bcmswaccess.c:bcmsw_phy_access()
    uint16 phy_reg_val;
    uint32_t data = 0;
    int reg_offset;
    phy_dev_t *phy = get_active_phy(port->p.phy);

    reg_offset = (e->offset >> PHY_REG_S) & PHY_REG_M;

    if (e->type == TYPE_GET) {
        down(&sf2_sw->s.conf_sem);
        if (IsC45Phy(phy))
            phy_bus_c45_read32(phy, reg_offset, &phy_reg_val);
        else
            phy_bus_read(phy, reg_offset, &phy_reg_val);
        data = phy_reg_val;
        up(&sf2_sw->s.conf_sem);
        data = __le32_to_cpu(data);
        e->length = 4;
        memcpy((void*)(&e->data), (void*)&data, e->length);
    } else {
        memcpy((void *)&phy_reg_val, (void *)e->data, 2);
        phy_reg_val = __cpu_to_le16(phy_reg_val);
        down(&sf2_sw->s.conf_sem);
        if (IsC45Phy(phy))
            phy_bus_c45_write32(phy, reg_offset, phy_reg_val);
        else
            phy_bus_write(phy, reg_offset, phy_reg_val);
        up(&sf2_sw->s.conf_sem);
    }

    return 0;
}

static int _alignment_check(struct ethswctl_data *e)
{
    static u16 misalign[][2] = {{0x581, 2}, {0x583, 4}, {0x4001, 4}, {0x4005, 2}};
    int i;

    for(i=0; i < ARRAY_SIZE(misalign); i++)
    {
        if (e->offset == misalign[i][0] && e->length == misalign[i][1]) return 1;
    }

    if (((e->offset % 2) && (e->length == 2 || e->length == 6)) ||
            ((e->offset % 4) && (e->length == 4 || e->length == 8))) return 0;

    return 1;
}


static int _get_next_queue(int cur_que, int for_wan, int reverse)
{
    int q;

    if(reverse == 0)
    {
        q = 0;
        if (cur_que != -1) q = cur_que + 1;

        for (; q < FC_LAN_TXQ_QUEUES; q++)
        {
            if (for_wan)
            {
                if (wanQueMap & (1<<q)) return q;
            }
            else
            {
                if ((wanQueMap & (1<<q)) == 0) return q;
            }
        }
    }
    else
    {
        q = FC_LAN_TXQ_QUEUES - 1;
        if (cur_que != -1) q = cur_que - 1;
        for (; q >= 0; q--)
        {
            if (for_wan)
            {
                if (wanQueMap & (1<<q)) return q;
            }
            else
            {
                if ((wanQueMap & (1<<q)) == 0) return q;
            }
        }
    }
    return -1;
}
#define GET_NEXT_WAN_QUE(CUR_QUE) _get_next_queue(CUR_QUE, 1, 0)
#define GET_NEXT_LAN_QUE(CUR_QUE) _get_next_queue(CUR_QUE, 0, 0)
#define GET_PREV_WAN_QUE(CUR_QUE) _get_next_queue(CUR_QUE, 1, 1)
#define GET_PREV_LAN_QUE(CUR_QUE) _get_next_queue(CUR_QUE, 0, 1)

static void _sf2_conf_thred_2reg(int page, int reg, uint16_t tbl[][FC_LAN_TXQ_QUEUES])
{
    int t, q;

    for (t = 0; t < FC_THRED_TOTAL_TYPES; t++)
    {
        for (q = 0; q < FC_LAN_TXQ_QUEUES; q++)
        {
            SF2SW_WREG(page, reg + t*0x10 + q*2, &tbl[t][q], sizeof(tbl[0][0]));
        }
    }
}

int speed_macro_2_mbps(phy_speed_t spd)
{
    static int speed[PHY_SPEED_10000 + 1] =
    {
        [PHY_SPEED_10] = 10,
        [PHY_SPEED_100] = 100,
        [PHY_SPEED_1000] = 1000,
        [PHY_SPEED_2500] = 2500,
        [PHY_SPEED_5000] = 5000,
        [PHY_SPEED_10000] = 10000,
    };
    return speed[spd];
}

void port_sf2_print_status(enetx_port_t *p)
{
    phy_dev_t *phy = get_active_phy(p->p.phy);
    if (phy->link)
    {
        printk((KERN_CRIT "%s (%s switch port: %d) (Logical Port: %d) (phyId: %x) Link UP at %d mbps %s duplex\n"),
                p->dev->name, PORT_ON_ROOT_SW(p)?"Int":"Ext", p->p.mac->mac_id, 
                    PHYSICAL_PORT_TO_LOGICAL_PORT(p->p.mac->mac_id, PORT_ON_ROOT_SW(p)?0:1), phy->addr,
                speed_macro_2_mbps(phy->speed), phy->duplex?"full":"half");
    }
    else
    {
        printk((KERN_CRIT "%s (%s switch port: %d) (Logical Port: %d) (phyId: %x) Link DOWN.\n"),
                p->dev->name, PORT_ON_ROOT_SW(p)?"Int":"Ext", p->p.mac->mac_id, 
                    PHYSICAL_PORT_TO_LOGICAL_PORT(p->p.mac->mac_id, PORT_ON_ROOT_SW(p)?0:1), phy->addr);
    }
}

void port_sf2_deep_green_mode_handler(void);
static void _sf2_conf_que_thred(void);
static void _extsw_set_port_imp_map_2_5g(void);
static void _extsw_set_port_imp_map_non_2_5g(void);

/*
 * handle_link_status_change
 */
void link_change_handler(enetx_port_t *port, int linkstatus, int speed, int duplex)
{
    phy_dev_t *phy_dev = get_active_phy(port->p.phy);
    phy_drv_t *phy_drv = phy_dev->phy_drv;
    IOCTL_MIB_INFO *mib = port->priv2;
    mac_dev_t *mac_dev = port->p.mac;
    mac_drv_t *mac_drv = mac_dev->mac_drv;
    mac_cfg_t mac_cfg = {};

#if defined(CONFIG_BCM_ETH_PWRSAVE)
    int phyId = 0; // 0 is a valid phy_id but not an external phy_id. So we are OK initializing it to 0.

    if (!PORT_ON_EXTERNAL_SW(port))
    {
        phyId = phy_dev->meta_id;
    }

    ethsw_phy_pll_up(0);
#endif

    down(&bcm_link_handler_config);

    // if (phy_dev->link != linkstatus) {
        if (linkstatus) {
            // bcmeapi_link_check(port, linkstatus, speed);
#if defined(CONFIG_BCM_ETH_PWRSAVE)
            /* Link is up, so de-isolate the Phy  */
            if (IsExtPhyId(phyId) && phy_drv->isolate_phy)
            {
                phy_drv->isolate_phy(phy_dev, 0);
            }
#endif

            if (speed == 10000)
            {
                mib->ulIfSpeed = SPEED_10000MBIT;
                mac_cfg.speed = MAC_SPEED_10000;
                phy_dev->speed = PHY_SPEED_10000;
            }
            else if (speed == 2500)
            {
                mib->ulIfSpeed = SPEED_2500MBIT;
                mac_cfg.speed = MAC_SPEED_2500;
                phy_dev->speed = PHY_SPEED_2500;
            }
            else if (speed == 1000)
            {
                mib->ulIfSpeed = SPEED_1000MBIT;
                mac_cfg.speed = MAC_SPEED_1000;
                phy_dev->speed = PHY_SPEED_1000;
            }
            else if (speed == 100)
            {
                mib->ulIfSpeed = SPEED_100MBIT;
                mac_cfg.speed = MAC_SPEED_100;
                phy_dev->speed = PHY_SPEED_100;
            }
            else if (speed == 200)
            {
                mib->ulIfSpeed = SPEED_200MBIT;
            }
            else
            {
                mib->ulIfSpeed = SPEED_10MBIT;
                phy_dev->speed = PHY_SPEED_10;
                mac_cfg.speed = MAC_SPEED_10;
            }

            mac_cfg.duplex = phy_dev->duplex = duplex? PHY_DUPLEX_FULL: PHY_DUPLEX_HALF;

#if defined(CONFIG_BCM_JUMBO_FRAME) && !defined(CONFIG_BCM_MAX_MTU_SIZE)
            {
                int we_locked_rtnl = FALSE;
                if (!rtnl_is_locked())
                {
                    rtnl_lock();
                    we_locked_rtnl = TRUE;
                }

                if (speed == 1000) /* When jumbo frame support is enabled - the jumbo MTU is applicable only for 1000M interfaces */
                    dev_set_mtu(port->dev, BCM_ENET_DEFAULT_MTU_SIZE);
                else
                    dev_set_mtu(port->dev, (ENET_NON_JUMBO_MAX_MTU_PAYLOAD_SIZE));
                if (we_locked_rtnl == TRUE) {
                    rtnl_unlock();
                }
            }
#endif
            mib->ulIfLastChange  = (jiffies * 100) / HZ;
            mib->ulIfDuplex = (unsigned long)duplex;
            if(PORT_ROLE_IS_WAN(port))
            {
                wanUpPorts_g++;
            }
            else
            {
                lanUpPorts_g++;
            }
#if defined(CONFIG_BCM_EXT_SWITCH)
            _sf2_conf_que_thred();
            if (mib->ulIfSpeed == SPEED_2500MBIT)
            {
                _extsw_set_port_imp_map_2_5g();
            }
#endif
            /* notify linux after we have finished setting our internal state */
            if (netif_carrier_ok(port->dev) == 0)
            {
                netif_carrier_on(port->dev);
            }
        } else {
#if defined(CONFIG_BCM_ETH_PWRSAVE)
            /* Link is down, so isolate the Phy. To prevent switch rx lockup
               because of packets entering switch with DLL/Clock disabled */
            if (IsExtPhyId(phyId) && phy_drv->isolate_phy)
            {
                phy_drv->isolate_phy(phy_dev, 1);
            }
#endif

#if 0   // TODO_DSL
            if (PORT_ON_EXTERNAL_SW(port))
            {
                extsw_fast_age_port(LOGICAL_PORT_TO_PHYSICAL_PORT(port), 0);
            }
            else
            {
                fast_age_port(LOGICAL_PORT_TO_PHYSICAL_PORT(port), 0);
            }
#endif

#if defined(CONFIG_BCM_EXT_SWITCH)
            if (mib->ulIfSpeed == SPEED_2500MBIT)
            {
                _extsw_set_port_imp_map_non_2_5g();
            }
#endif /* CONFIG_BCM_EXT_SWITCH */

            mib->ulIfLastChange  = 0;
            mib->ulIfSpeed       = 0;
            mib->ulIfDuplex      = 0;

            if (PORT_ROLE_IS_WAN(port))
            {
                wanUpPorts_g--;
            }
            else
            {
                lanUpPorts_g--;
            }
#if defined(CONFIG_BCM_EXT_SWITCH)
            _sf2_conf_que_thred();
#endif

#if defined(CONFIG_BCM_GMAC)
            if (IsGmacPort( sw_port ) )
            {
                volatile GmacEEE_t *gmacEEEp = GMAC_EEE;
                gmacEEEp->eeeCtrl.linkUp = 0;
                gmacEEEp->eeeCtrl.enable = 0;
            }
#endif
            // ethsw_eee_port_enable(sw_port, 0, 0);
            /* notify linux after we have finished setting our internal state */
            if (netif_carrier_ok(port->dev) != 0)
            {
                netif_carrier_off(port->dev);
            }

        }
        phy_dev->link = linkstatus;
        port_sf2_print_status(port);

#if defined(CONFIG_BCM_ETH_DEEP_GREEN_MODE)
        port_sf2_deep_green_mode_handler();
#endif

        if (mac_drv->cfg_set)
        {
            mac_drv->cfg_set(mac_dev, &mac_cfg);
        }

        /* update EEE settings based on link status */
        enetx_queue_work(port, set_mac_eee_by_phy);
    // }

    up(&bcm_link_handler_config);
}

static void _sf2_conf_que_thred(void)
{
    int q, q1, t;
    int thredBase[FC_THRED_TOTAL_TYPES], maxStreams,
        maxFrameLength = 4096, lastLanQue, lastWanQue, wanUpPorts, lanUpPorts;

/*
    The percentage of various factors for different condition
    from base computation result. Don't use parantheses to avoid underflow.
    The design goal is:
        o WAN queues have higher thredsholds than all LAN queues.
        o Higher queues have higher thresholds than lower queues in the same group.
*/

/* Define IMP Port over LAN port threshold factor */
#define IMP_OVER_WAN_QUE_FACTOR 100/100

/*
   Define Threshold increment factor with higher priority queue to
   guarantee higher priority queue get slight more chance of resource.
   The value should not caused WAN/LAN threshold reserved when scaled up.
*/
#define LOW_QUE_OVER_HIGH_QUE_FACTOR 100/101

/* Define WAN Queue over LAN queu factor so that WAN queue gets higher resource chance */
#define LAN_QUE_OVER_WAN_QUE_FACTOR 100/105

/*
    PER Queue thredshold is to control resource ratio between multiple ports
    This parameter defines maximum stream number of traffic that will be controled
    strictly by Per Queue threads. When stream number is over this, configuration will
    become oversubscription in terms of stream number, thus total Thred will be kicked
    in first.
*/
#define PER_QUE_CONTROL_STREAMS 2

/*
    Define Factor of PerQueue Oversubscription Factors when streams are over PER_QUE_CONTROL_STREAMS.
    This is used to times to the difference to total thredsholds.
*/
#define PER_QUE_OVERSUB_FACTOR 2/100

    lanUpPorts = lanUpPorts_g;
    wanUpPorts = wanUpPorts_g;

    /* maxStreams is used to calculat Total Pause(Not Drop) to
        reserve head room in buffer to guarantee the minimum buffer by queue reserved threshold
        when stream number is below this number. Term "stream" means traffic to ONE queue. */
    if (queThreConfMode == ThreModeDynamic)
    {
        maxStreams = (lanUpPorts + wanUpPorts + 1) * FC_LAN_TXQ_QUEUES;
    }
    else
    {
        maxStreams = (total_ports + 1) * FC_LAN_TXQ_QUEUES;
    }

    if (maxStreams > maxStreamNumber)
    {
        maxStreams = maxStreamNumber;
    }

    /* Unify all computation to page unit */
    maxFrameLength /= SF2_BYTES_PER_PAGE;

    /* Set Reserved Threshold to one frame size to create hard guarantee for each queue */
    thredBase[FC_THRED_QUE_RSRVD_TYPE] = maxFrameLength;

    /*
        Total Drop Threshold:
        When total queue length exceeds Total Drop, all packet will be dropped even
        for queues under Reserved Threshold. This can only happen when external device
        ignores Pause frame. As the values for protocol violation case or misconfiguration case,
        the value is design as high as possible to minimum the impact of overrun above
        the total Pause and as final guard to total buffer.  Thus set to to one packet size room
        below hard limits.
    */
    thredBase[FC_THRED_TTL_DROP_TYPE] = SF2_MAX_BUFFER_IN_PAGE - maxFrameLength;

    /*
        Compute Total Pause Threshold:
        Need to guarantee hardware reserved threshold in EACH queuue.
        The value depends on simultaneous buffer requesting streams and frame length.
        The value is set to guarantee no drop
    */
    thredBase[FC_THRED_TTL_PAUSE_TYPE] = SF2_MAX_BUFFER_IN_PAGE - maxFrameLength * maxStreams;

    /*
        Total Hysteresis:
        Hysteresis will reflect the hop count from this device to the source and
        the latency of each hop's resume operation. Set this as high as possible
        related to PAUSE but at least one frame below PAUSE and no lower than reserve.
    */
    thredBase[FC_THRED_TTL_HYSTR_TYPE] = thredBase[FC_THRED_TTL_PAUSE_TYPE] - maxFrameLength;

    /*
        Calculate Per Queue thresholds based on maxStreams first.
        The Queue Thresholds will control the ratio before total Threshold is kicked in.
        But when the stream number is big, the Per queue threshold will be unnecessary too low.
        Do Per queue oversubscription if port number is more than strict control stream number
    */
    thredBase[FC_THRED_QUE_PAUSE_TYPE] = thredBase[FC_THRED_TTL_PAUSE_TYPE]/PER_QUE_CONTROL_STREAMS;
    thredBase[FC_THRED_QUE_DROP_TYPE] = thredBase[FC_THRED_TTL_DROP_TYPE]/PER_QUE_CONTROL_STREAMS;

    if ( maxStreams > PER_QUE_CONTROL_STREAMS)
    {
        thredBase[FC_THRED_QUE_PAUSE_TYPE] =
            thredBase[FC_THRED_QUE_PAUSE_TYPE] *100/(100 + maxStreams*100*PER_QUE_OVERSUB_FACTOR);
        thredBase[FC_THRED_QUE_DROP_TYPE] =
            thredBase[FC_THRED_QUE_DROP_TYPE]*100/(100 + maxStreams*100*PER_QUE_OVERSUB_FACTOR);
    }

    /* Set all queue's Hysteresis to be one frame below the PAUSE */
    thredBase[FC_THRED_QUE_HYSTR_TYPE] = thredBase[FC_THRED_QUE_PAUSE_TYPE] - maxFrameLength;

    /* Compute base WAN queue threashold */
    lastWanQue = GET_PREV_WAN_QUE(-1);

    /* Now Scale WAN Queue Thredshold up */
    q1 = lastWanQue;
    for (q = q1; q != -1; q1 = q, q = GET_PREV_WAN_QUE(q))
    {
        for (t = FC_THRED_TOTAL_TYPES - 1; t >= 0; t--)
        {
            if ( q == q1) /* Initialize last Queue thresholds */
            {
                sf2_sw_port_thred[t][q] = thredBase[t];
            }
            else
            {
                switch (t)
                {
                    case FC_THRED_QUE_RSRVD_TYPE:
                        /* Set reserve threshold to be the same */
                        sf2_sw_port_thred[t][q] = thredBase[FC_THRED_QUE_RSRVD_TYPE];
                        break;
                    case FC_THRED_QUE_HYSTR_TYPE:
                        /* Set Hysteresis one frame lower from Pause */
                        sf2_sw_port_thred[t][q] = sf2_sw_port_thred[FC_THRED_QUE_PAUSE_TYPE][q] - maxFrameLength;
                        break;
                    default:
                        /* Set the rest thresholds lowered down with Lower Queue Factor. */
                        sf2_sw_port_thred[t][q] = sf2_sw_port_thred[t][q1] * LOW_QUE_OVER_HIGH_QUE_FACTOR;
                }
            }
        }
    }

    /* Now Scale LAN Queue Thredshold down */
    lastLanQue = GET_PREV_LAN_QUE(-1);
    q1 = lastLanQue;
    for (q = q1; q != -1; q1 = q, q = GET_PREV_LAN_QUE(q1))
    {
        for (t = FC_THRED_TOTAL_TYPES - 1; t >= 0; t--)
        {
            switch (t)
            {
                case FC_THRED_QUE_RSRVD_TYPE:
                    /* Set reserve threshold to be the same */
                    sf2_sw_port_thred[t][q] = thredBase[FC_THRED_QUE_RSRVD_TYPE];
                    break;
                case FC_THRED_QUE_HYSTR_TYPE:
                    sf2_sw_port_thred[t][q] = sf2_sw_port_thred[FC_THRED_QUE_PAUSE_TYPE][q] - maxFrameLength;
                    break;
                default:
                    if (q == lastLanQue)
                    {
                        sf2_sw_port_thred[t][q] = thredBase[t];

                        /* If there is WAN Queue configured, scale down LAN queue with a factor */
                        if (lastWanQue != -1)
                        {
                            sf2_sw_port_thred[t][q] = sf2_sw_port_thred[t][q] * LAN_QUE_OVER_WAN_QUE_FACTOR;
                        }
                    }
                    else
                    {
                        sf2_sw_port_thred[t][q] = sf2_sw_port_thred[t][q1] * LOW_QUE_OVER_HIGH_QUE_FACTOR;
                    }
            }
        }
    }

    /* Configure IMP port */
    for (q = FC_LAN_TXQ_QUEUES - 1; q >= 0; q--)
    {
        for (t = FC_THRED_TOTAL_TYPES - 1; t >= 0; t--)
        {
            switch (t)
            {
                case FC_THRED_QUE_RSRVD_TYPE:
                    sf2_imp0_thred[t][q] = thredBase[t];
                    break;
                case FC_THRED_QUE_HYSTR_TYPE:
                    sf2_imp0_thred[t][q] = sf2_imp0_thred[FC_THRED_QUE_PAUSE_TYPE][q] - maxFrameLength;
                    break;
                default:
                    if (q == FC_LAN_TXQ_QUEUES - 1)
                    {
                        sf2_imp0_thred[t][q] = thredBase[t] * IMP_OVER_WAN_QUE_FACTOR;
                    }
                    else
                    {
                        sf2_imp0_thred[t][q] = sf2_imp0_thred[t][q + 1] * LOW_QUE_OVER_HIGH_QUE_FACTOR;
                    }
            }
        }
    }
    _sf2_conf_thred_2reg(PAGE_FC_LAN_TXQ, REG_FC_LAN_TXQ_THD_RSV_QN0, sf2_sw_port_thred);
    _sf2_conf_thred_2reg(PAGE_FC_IMP0_TXQ, REG_FC_IMP0_TXQ_THD_RSV_QN0, sf2_imp0_thred);
}

static void _extsw_setup_imp_ports(void)
{
    // based on impl5/bcmsw.c:extsw_setup_imp_ports()
    uint8_t  val8;
#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
    uint16_t val16;
#endif /* CONFIG_BCM_ENET_MULTI_IMP_SUPPORT */
    /* Assumption : External switch is always in MANAGED Mode w/ TAG enabled.
     * BRCM TAG enable in external switch is done via MDK as well
     * but it is not deterministic when the userspace app for external switch
     * will run. When it gets delayed and the device is already getting traffic,
     * all those packets are sent to CPU without external switch TAG.
     * To avoid the race condition - it is better to enable BRCM_TAG during driver init. */
    SF2SW_RREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL, &val8, sizeof(val8));
    val8 &= (~(BRCM_HDR_EN_GMII_PORT_5|BRCM_HDR_EN_IMP_PORT)); /* Reset HDR_EN bit on both ports */
    val8 |= BRCM_HDR_EN_IMP_PORT; /* Set only for IMP Port */
    SF2SW_WREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL, &val8, sizeof(val8));

#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
    /* NOTE : Forcing these setting here; SWMDK doesn't setup IMP when multiple IMP ports in-use */

    /* Enable IMP Port */
    val8 = ENABLE_MII_PORT | RECEIVE_BPDU;
    SF2SW_WREG(PAGE_MANAGEMENT, REG_GLOBAL_CONFIG, &val8, sizeof(val8));

    /* management mode, enable forwarding */
    SF2SW_RREG(PAGE_CONTROL, REG_SWITCH_MODE, &val8, sizeof(val8));
    val8 |= REG_SWITCH_MODE_FRAME_MANAGE_MODE | REG_SWITCH_MODE_SW_FWDG_EN;
    SF2SW_WREG(PAGE_CONTROL, REG_SWITCH_MODE, &val8, sizeof(val8));

    /* setup Switch MII1 port state override : 2000M with flow control enabled */
    val8 = IMP_LINK_OVERRIDE_2000FDX /*| REG_CONTROL_MPSO_FLOW_CONTROL*/; /* FIXME : Enabling flow control creates some issues */
    SF2SW_WREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(IMP_PORT_ID), &val8, sizeof(val8));

    /* enable rx bcast, ucast and mcast of imp port */
    val8 = REG_MII_PORT_CONTROL_RX_UCST_EN | REG_MII_PORT_CONTROL_RX_MCST_EN |
           REG_MII_PORT_CONTROL_RX_BCST_EN;
    SF2SW_WREG(PAGE_CONTROL, REG_MII_PORT_CONTROL, &val8, sizeof(val8));

    /* Forward lookup failure to use ULF/MLF/IPMC lookup fail registers */
    val8 = REG_PORT_FORWARD_MCST | REG_PORT_FORWARD_UCST | REG_PORT_FORWARD_IP_MCST;
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_FORWARD, &val8, sizeof(val8));

    /* Forward unlearned unicast and unresolved mcast to the MIPS */
    val16 = PBMAP_MIPS;
    SF2SW_WREG(PAGE_CONTROL, REG_UCST_LOOKUP_FAIL, &val16, sizeof(val16));
    SF2SW_WREG(PAGE_CONTROL, REG_MCST_LOOKUP_FAIL, &val16, sizeof(val16));
    SF2SW_WREG(PAGE_CONTROL, REG_IPMC_LOOKUP_FAIL, &val16, sizeof(val16));

    /* Disable learning on MIPS*/
    val16 = PBMAP_MIPS;
    SF2SW_WREG(PAGE_CONTROL, REG_DISABLE_LEARNING, &val16, sizeof(val16));

    /* NOTE : All regular setup for P8 IMP is done above ; Same as what SWMDK would do*/

#if defined(CONFIG_BCM94908)
    /* Enable BRCM TAG on P5 */
    SF2SW_RREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL, &val8, sizeof(val8));
    val8 |= BRCM_HDR_EN_GMII_PORT_5; /* Enable BRCM TAG on P5 */
    SF2SW_WREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL, &val8, sizeof(val8));
    /* Enale Link - port override register */
    SF2SW_RREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(5), &val8, sizeof(val8));
    val8 = LINK_OVERRIDE_1000FDX; /* Enable 1000FDX Link */
    val8 |= REG_PORT_GMII_SPEED_UP_2G; /* Speed up to 2G */
    SF2SW_WREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(5), &val8, sizeof(val8));
    /* Enable P5 TXRX */
    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+5, &val8, sizeof(val8));
    val8 &= ~REG_PORT_CTRL_DISABLE;
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+5, &val8, sizeof(val8));

    /* Enable BRCM TAG on P4 */
    SF2SW_RREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL2, &val8, sizeof(val8));
    val8 = BRCM_HDR_EN_P4; /* Enable BRCM TAG on P4 */
    SF2SW_WREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL2, &val8, sizeof(val8));
    /* Enale Link - port override register */
    SF2SW_RREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(4), &val8, sizeof(val8));
    val8 = LINK_OVERRIDE_1000FDX; /* Enable 1000FDX Link */
    SF2SW_WREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(4), &val8, sizeof(val8));
    /* Enable P4 TXRX */
    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+4, &val8, sizeof(val8));
    val8 &= ~REG_PORT_CTRL_DISABLE;
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+4, &val8, sizeof(val8));

#elif defined(CONFIG_BCM963158)
    // imp ports are p5,7,8  instead of p4,5,8
    /* Enable BRCM TAG on P5 */
    SF2SW_RREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL, &val8, sizeof(val8));
    val8 |= BRCM_HDR_EN_GMII_PORT_5; /* Enable BRCM TAG on P5 */
    SF2SW_WREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL, &val8, sizeof(val8));
    /* Enale Link - port override register */
    SF2SW_RREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(5), &val8, sizeof(val8));
    val8 = LINK_OVERRIDE_1000FDX; /* Enable 1000FDX Link */
    val8 |= REG_PORT_GMII_SPEED_UP_2G; /* Speed up to 2G */
    SF2SW_WREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(5), &val8, sizeof(val8));
    /* Enable P5 TXRX */
    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+5, &val8, sizeof(val8));
    val8 &= ~REG_PORT_CTRL_DISABLE;
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+5, &val8, sizeof(val8));

    /* Enable BRCM TAG on P7 */
    SF2SW_RREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL, &val8, sizeof(val8));
    val8 |= BRCM_HDR_EN_GMII_PORT_7; /* Enable BRCM TAG on P7 */
    SF2SW_WREG(PAGE_MANAGEMENT, REG_BRCM_HDR_CTRL, &val8, sizeof(val8));
    /* Enale Link - port override register */
    SF2SW_RREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(7), &val8, sizeof(val8));
    val8 = LINK_OVERRIDE_1000FDX; /* Enable 1000FDX Link */
    val8 |= REG_PORT_GMII_SPEED_UP_2G; /* Speed up to 2G */
    SF2SW_WREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(7), &val8, sizeof(val8));
    /* Enable P7 TXRX */
    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+7, &val8, sizeof(val8));
    val8 &= ~REG_PORT_CTRL_DISABLE;
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+7, &val8, sizeof(val8));

#endif //defined(CONFIG_BCM963158)

#endif /* CONFIG_BCM_ENET_MULTI_IMP_SUPPORT */
}

#include "bcm_chip_arch.h"

static void _extsw_set_pbvlan(int port, uint16_t fwdMap)
{
    SF2SW_WREG(PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P0 + (port * 2), (uint8_t *)&fwdMap, 2);
}

#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)

static uint16_t _extsw_get_pbvlan(int port)
{
    uint16_t val16;

    SF2SW_RREG(PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P0 + (port * 2), (uint8_t *)&val16, 2);
    return val16;
}

/*
    Use CFP to force Reserved Multicast Address to be received by
    IMP port correctly overriding Port Based VLAN set for load balancing.
*/
static int _bcmsw_add_cfp_rsvd_multicast_support(void)
{
    // based on impl5/bcmsw.c:bcmsw_add_cfp_rsvd_multicast_support()
    struct ethswctl_data _e, *e = &_e;
    cfpArg_t *cfpArg = &e->cfpArgs;

    memset(e, 0, sizeof(*e));

    cfpArg->da = 0x0180c2000000LL;
    cfpArg->da_mask = 0xffffff000000;
    cfpArg->argFlag |= CFP_ARG_DA_M;
    cfpArg->l3_framing= CfpL3NoIP;
    cfpArg->argFlag |= CFP_ARG_L3_FRAMING_M;
    cfpArg->op = CFPOP_APPEND;
    cfpArg->argFlag |= CFP_ARG_OP_M;
    cfpArg->chg_fpmap_ib = 2;
    cfpArg->argFlag |= CFP_ARG_CHG_FPMAP_IB_M;
    cfpArg->fpmap_ib = PBMAP_MIPS;
    cfpArg->argFlag |= CFP_ARG_FPMAP_IB_M;
    cfpArg->priority = 2;
    cfpArg->argFlag |= CFP_ARG_PRIORITY_M;

    return ioctl_extsw_cfp(e);
}

static void _bcmsw_print_imp_port_grouping(unsigned long port_map, int port_imp_map[])
{
    // based on impl5/bcmsw.c:bcmsw_print_imp_port_grouping()
    int port, imp_port, new_grp = 0;
    printk("NOTE: Using Port Grouping for IMP ports : ");
    for (imp_port = 0; imp_port <= BP_MAX_SWITCH_PORTS; imp_port++)
    {
        /* Not an IMP port -- continue */
        if (! ( (1<<imp_port) & DEFAULT_IMP_PBMAP ) ) continue;
        new_grp = 1;
        for (port = 0; port < BP_MAX_SWITCH_PORTS; port++) 
        {
            if ( ((1<<port) & port_map) && 
                 port_imp_map[port] == imp_port )
            {
                if (new_grp)
                {
                    printk("[");
                    new_grp = 0;
                }
                else
                {
                    printk(",");
                }
                printk(" %d",port);
            }
        }
        if (!new_grp)
        {
            printk(" --> %d ] ",imp_port);
        }
    }
    printk("\n");
}

static int *cur_port_imp_map;
static void _extsw_cfg_port_imp_grouping(int port_imp_map[])
{
    // based on impl5/bcmsw.c:extsw_cfg_port_imp_grouping()
    unsigned char port;
    unsigned long port_map;
    uint16 v16;
    const ETHERNET_MAC_INFO *EnetInfo = BpGetEthernetMacInfoArrayPtr();
    const ETHERNET_MAC_INFO *info = &EnetInfo[1];

    cur_port_imp_map = port_imp_map;
    /* Configure forwarding based on Port Grouping
     * By default all port's pbvlan is 0x1FF */
    port_map = info->sw.port_map; /* Get port map for external switch */
    /* NOTE : ports are scanned to cover last IMP port as well -- see details below */
    for (port = 0; port < BP_MAX_SWITCH_PORTS+1; port++)
    {
        v16 = 0;
        if ( !(DEFAULT_IMP_PBMAP & (1<<port)) && (port_imp_map[port] != -1) && ( (1<<port) & port_map ) )
        {
            v16 = _extsw_get_pbvlan(port) & chip_arch_all_portmap[1]; /* Get current PBVLAN Map */
            v16 &= ~(imp_pbmap[1]); /* Remove all IMP Ports from PBVLAN Map for external switch*/
            v16 |= (1<<port_imp_map[port]); /* Add back the desired IMP Port */
            _extsw_set_pbvlan(port, v16);
            port_imp_emac_map[port] = imp_to_emac[port_imp_map[port]];
        }
        else if ( DEFAULT_IMP_PBMAP & (1<<port) ) 
        { /* IMP Port - Block IMP to IMP forwarding */
            /* As such there is no need to block IMP-IMP forwarding because it should NEVER happen
             * But during initial runner development, it was noticed that runner was adding incorrect
             * Broadcom tag (that has destination port as other IMP), this results in packet getting 
             * looped back; In order to avoid this issue temporarily, following is done. 
             * Below change could be kept as permanent, though not needed. */ 
            v16 = _extsw_get_pbvlan(port) & chip_arch_all_portmap[1]; /* Get current PBVLAN Map */
            v16 &= ~(imp_pbmap[1]); /* Remove all IMP Ports from PBVLAN Map for external switch*/
            v16 |= (1<<port); /* Add back this IMP Port - Not required though */
            _extsw_set_pbvlan(port, v16);
        }
    }
    
    _bcmsw_print_imp_port_grouping(port_map, port_imp_map);
}

static void _extsw_set_port_imp_map_2_5g(void)
{
    _extsw_cfg_port_imp_grouping(port_imp_map_2_5g);
}

static void _extsw_set_port_imp_map_non_2_5g(void)
{
    _extsw_cfg_port_imp_grouping(port_imp_map_non_2_5g);
}


static void _extsw_setup_imp_fwding(void)
{
    // based on impl5/bcmsw.c:extsw_setup_imp_fwding()
    uint16 v16;

#if defined(CONFIG_BCM94908)
    /* Configure the Lookup failure registers to P4, P5, P8 */
    v16 = DEFAULT_IMP_PBMAP;
    SF2SW_WREG(PAGE_CONTROL, REG_UCST_LOOKUP_FAIL, &v16, sizeof(v16));
    SF2SW_WREG(PAGE_CONTROL, REG_MCST_LOOKUP_FAIL, &v16, sizeof(v16));
    SF2SW_WREG(PAGE_CONTROL, REG_IPMC_LOOKUP_FAIL, &v16, sizeof(v16));
    /* Disable learning on MIPS and P4/P5*/
    v16 = DEFAULT_IMP_PBMAP;
    SF2SW_WREG(PAGE_CONTROL, REG_DISABLE_LEARNING, &v16, sizeof(v16));

    _extsw_set_port_imp_map_non_2_5g(); /* By default we start with assuming no 2.5G port */
#elif defined(CONFIG_BCM963158)
    /* Configure the Lookup failure registers to P5, P7, P8 */
    v16 = DEFAULT_IMP_PBMAP;
    SF2SW_WREG(PAGE_CONTROL, REG_UCST_LOOKUP_FAIL, &v16, sizeof(v16));
    SF2SW_WREG(PAGE_CONTROL, REG_MCST_LOOKUP_FAIL, &v16, sizeof(v16));
    SF2SW_WREG(PAGE_CONTROL, REG_IPMC_LOOKUP_FAIL, &v16, sizeof(v16));
    /* Disable learning on MIPS and P5/P7*/
    v16 = DEFAULT_IMP_PBMAP;
    SF2SW_WREG(PAGE_CONTROL, REG_DISABLE_LEARNING, &v16, sizeof(v16));

    _extsw_set_port_imp_map_non_2_5g();
#endif //defined(CONFIG_BCM963158)

    _bcmsw_add_cfp_rsvd_multicast_support();
}
#else
static void _extsw_setup_imp_fwding(void) {}
static void _extsw_set_port_imp_map_2_5g(void) {}
static void _extsw_set_port_imp_map_non_2_5g(void) {}
#endif //!defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)

typedef struct rgmii_registers_s {
        uint32 rgmii_ctrl;
        uint32 rgmii_rx_clk_delay;
        uint32 mii_pad_ctrl;
        uint32 ext_physical_port;
} rgmii_registers_t;

rgmii_registers_t rgmii_port_reg_offset [] = {
#if defined(CONFIG_BCM94908)
             {  OFFSETOF(EthernetSwitchReg, rgmii_11_ctrl), 
                OFFSETOF(EthernetSwitchReg, rgmii_11_rx_clk_delay_ctrl), 
                OFFSETOF(Misc, miscxMIIPadCtrl[3]), 11},
#elif defined(CONFIG_BCM963138) || defined(CONFIG_BCM63148)
             {  OFFSETOF(EthernetSwitchReg, rgmii_5_ctrl), 
                OFFSETOF(EthernetSwitchReg, rgmii_5_rx_clk_delay_ctrl), 
                OFFSETOF(Misc, miscxMIIPadCtrl[1]), 5},

             {  OFFSETOF(EthernetSwitchReg, rgmii_7_ctrl), 
                OFFSETOF(EthernetSwitchReg, rgmii_7_rx_clk_delay_ctrl), 
                OFFSETOF(Misc, miscxMIIPadCtrl[2]), 7},

             {  OFFSETOF(EthernetSwitchReg, rgmii_11_ctrl), 
                OFFSETOF(EthernetSwitchReg, rgmii_11_rx_clk_delay_ctrl), 
                OFFSETOF(Misc, miscxMIIPadCtrl[3]), 11},

             {  OFFSETOF(EthernetSwitchReg, rgmii_12_ctrl), 
                OFFSETOF(EthernetSwitchReg, rgmii_12_rx_clk_delay_ctrl), 
                OFFSETOF(Misc, miscxMIIPadCtrl[0]), 12},
#elif defined(CONFIG_BCM963158)
             {  offsetof(EthernetSwitchReg, rgmii_11_ctrl), 
                OFFSETOF(EthernetSwitchReg, rgmii_11_rx_clk_delay_ctrl), 
                OFFSETOF(Misc, miscxMIIPadCtrl[1]), 11},

             {  OFFSETOF(EthernetSwitchReg, rgmii_12_ctrl), 
                OFFSETOF(EthernetSwitchReg, rgmii_12_rx_clk_delay_ctrl), 
                OFFSETOF(Misc, miscxMIIPadCtrl[0]), 12},

             {  OFFSETOF(EthernetSwitchReg, rgmii_13_ctrl), 
                OFFSETOF(EthernetSwitchReg, rgmii_13_rx_clk_delay_ctrl), 
                OFFSETOF(Misc, miscxMIIPadCtrl[2]), 3},
#endif
};

static rgmii_registers_t *get_rgmii_regs(int ext_port)
{
    int i;

    for (i=0; i<ARRAY_SIZE(rgmii_port_reg_offset); i++)
    {
        if (rgmii_port_reg_offset[i].ext_physical_port == ext_port) 
            return &rgmii_port_reg_offset[i];
    }
    return NULL;
}

static void phy_config_rgmii(phy_dev_t *phy_dev, int physical_port)
{
    rgmii_registers_t *rgmii_regs;
    uint32 *rgmii_ctrl, *rgmii_rx_clk_delay, *rgmii_pad_ctrl;
    uint32 rgmii_ctrl_v, rgmii_rx_clk_delay_v;

    rgmii_regs = get_rgmii_regs(physical_port);
    if (rgmii_regs == NULL) {
        printk("**** ERROR: physical port %d not a RGMII port\n", physical_port);
        return;
    }
    rgmii_ctrl = (uint32 *)((char *)ETHSW_REG + rgmii_regs->rgmii_ctrl);
    rgmii_rx_clk_delay = (uint32 *)((char *)ETHSW_REG + rgmii_regs->rgmii_rx_clk_delay);
    rgmii_pad_ctrl = (uint32 *)((char *)MISC + rgmii_regs->mii_pad_ctrl);

    if (IsRGMII_1P8V(phy_dev->meta_id))
        *rgmii_pad_ctrl = (*rgmii_pad_ctrl & ~MISC_XMII_PAD_MODEHV) | MISC_XMII_PAD_AMP_EN;
    else if (IsRGMII_2P5V(phy_dev->meta_id))
        *rgmii_pad_ctrl = (*rgmii_pad_ctrl | MISC_XMII_PAD_MODEHV) & ~MISC_XMII_PAD_SEL_GMII & ~MISC_XMII_PAD_AMP_EN;
    else if (IsRGMII_3P3V(phy_dev->meta_id))
        *rgmii_pad_ctrl = (*rgmii_pad_ctrl | MISC_XMII_PAD_MODEHV | MISC_XMII_PAD_SEL_GMII) & ~MISC_XMII_PAD_AMP_EN;

    rgmii_ctrl_v = (*rgmii_ctrl & ~ETHSW_RC_MII_MODE_MASK) | ETHSW_RC_RGMII_EN | ETHSW_RC_ID_MODE_DIS;
    rgmii_ctrl_v |= ETHSW_RC_EXT_GPHY;

    rgmii_rx_clk_delay_v = *rgmii_rx_clk_delay;

    /* Phy Tx clk Delay is on by default. phy reg 24, shadow reg 7
     * works with Rx delay -- ON by default. phy reg 28, shadow reg 3
     * No action on the phy side unless specified in boardparms
     */

    if (!phy_dev->delay_tx)
        rgmii_ctrl_v &= ~ETHSW_RC_ID_MODE_DIS; /* Clear TX_ID_DIS */

    if (!phy_dev->delay_rx)
        rgmii_rx_clk_delay_v &= ~(ETHSW_RXCLK_IDDQ|ETHSW_RXCLK_BYPASS); /* Clear Rx bypass */
 
    *rgmii_rx_clk_delay = rgmii_rx_clk_delay_v;
    *rgmii_ctrl = rgmii_ctrl_v;
}

static int tr_port_for_rgmii(enetx_port_t *p, void *ctx)
{
    phy_dev_t *phy_dev;
    int sw_override = 0;

    if (phy_is_crossbar(p->p.phy))
    {
        // this port is connected to cross bar, traverse the phys that is connected
        for (phy_dev = crossbar_phy_dev_first(p->p.phy); phy_dev; phy_dev = crossbar_phy_dev_next(phy_dev))
        {
            if (IsRGMII(phy_dev->meta_id))
                phy_config_rgmii(phy_dev, BP_CROSSBAR_PORT_TO_PHY_PORT(crossbar_external_endpoint(phy_dev)));
        }
        // turn on SW_OVERRIDE when port is connected to crossbar
        sw_override = 1;
    }
    else
    {
        phy_dev = p->p.phy;
        if (phy_dev)
        {
            if (IsRGMII(phy_dev->meta_id))
            {
                phy_config_rgmii(phy_dev, p->p.mac->mac_id);
                // turn on SW_OVERRIDE when phy is RGMII
                sw_override = 1;
            }
        }
    }

    if (sw_override && PORT_ON_EXTERNAL_SW(p))
    {
        uint8_t v8;
        SF2SW_RREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(p->p.mac->mac_id), &v8, 1);
        v8 |= REG_PORT_STATE_OVERRIDE;
        SF2SW_WREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(p->p.mac->mac_id), &v8, 1);
    }
    return 0;
}

void sf2_rgmii_config(void)
{
    port_traverse_ports(root_sw, tr_port_for_rgmii, PORT_CLASS_PORT, NULL);
}

static void _sf2_set_imp_speed(void)
{
    // based on impl5/bcmsw.c:sf2_enable_2_5g()
#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM94908)
    volatile u32 *sw_ctrl_reg = (void *)(SWITCH_REG_BASE);
    uint32 val32 = *sw_ctrl_reg;
    val32 |= SF2_IMP_2_5G_EN;
    *sw_ctrl_reg = val32;
#elif defined(CONFIG_BCM963158)
    volatile u32 *sw_ctrl_reg = (void *)(SWITCH_REG_BASE);
    uint32 val32 = *sw_ctrl_reg;
    val32 &= ~(IMP_SPEED_MASK);
    val32 |= DEFAULT_IMP_SPEEDS;
    *sw_ctrl_reg = val32;
#endif
}

static void _sf2_enable_p8_rdp_sel(void)
{
#if defined(CONFIG_BCM94908)
    volatile u32 *sw_ctrl_reg = (void *)(SWITCH_REG_BASE);
    uint32 val32 = *sw_ctrl_reg;
    val32 |= 1<<16; /* P8_RDP_SEL valid in 4908 */
    *sw_ctrl_reg = val32;
#elif defined(CONFIG_BCM963158)
    volatile u32 *sw_cross_bar_reg = (void*)(SWITCH_CROSSBAR_REG);
    uint32 val32 = *sw_cross_bar_reg;
    val32 |= 1<<5; /* P8_MUX_SEL 0=SysPort, 1=XRDP */
    *sw_cross_bar_reg = val32;
#endif
}

static void _extsw_port_trunk_init(void)
{
    // based on impl5/bcmsw.c:extsw_port_runk_init()
    int enable_trunk = 0;
#if defined(CONFIG_BCM_KERNEL_BONDING)
    enable_trunk |= 1;
#endif

    if (enable_trunk)
    {
        unsigned char v8;
        SF2SW_RREG(PAGE_MAC_BASED_TRUNK, REG_MAC_TRUNK_CTL, &v8, 1);
        v8 |= ( (1 & TRUNK_EN_LOCAL_M) << TRUNK_EN_LOCAL_S ); /* Enable Trunking */
        v8 |= ( (TRUNK_HASH_DA_SA_VID & TRUNK_HASH_SEL_M) << TRUNK_HASH_SEL_S ); /* Default VID+DA+SA Hash */
        SF2SW_WREG(PAGE_MAC_BASED_TRUNK, REG_MAC_TRUNK_CTL, &v8, 1);
        SF2SW_RREG(PAGE_MAC_BASED_TRUNK, REG_MAC_TRUNK_CTL, &v8, 1);
        printk("LAG/Trunking enabled <0x%02x>\n",v8);
    }
}

static void _sf2_tc_to_cos_default(void)
{
    int i, j;
    uint16_t reg_addr;
    uint32_t val32;

    for (i = 0; i <= SF2_IMP0_PORT; i++) // all ports except 6
    {
        if (i == SF2_INEXISTANT_PORT) continue; // skip port 6
        reg_addr = SF2_REG_PORTN_TC_TO_COS + i * 4;
        val32 = 0;
        for (j = 0; j <= SF2_QOS_TC_MAX; j++) // all TC s
        {
            //  TC to COS one-one mapping
            val32 |= (j & SF2_QOS_COS_MASK) << (j * SF2_QOS_COS_SHIFT);
        }
        SF2SW_WREG(PAGE_QOS, reg_addr, &val32, 4);
    }
}

static void _sf2_qos_default(void)
{
    uint32_t val32;
    uint32_t port;
    /* Set Global QoS Control */
    SF2SW_RREG(PAGE_QOS, SF2_REG_QOS_GLOBAL_CTRL, &val32, 4);
    val32 |= SF2_QOS_P8_AGGREGATION_MODE;
    SF2SW_WREG(PAGE_QOS, SF2_REG_QOS_GLOBAL_CTRL, &val32, 4);
    /* set SP scheduling on all ports (including IMP) by default */
    for (port=0; port <= BP_MAX_SWITCH_PORTS;  port++)
    {
        if (port == SF2_INEXISTANT_PORT) continue;
        SF2SW_RREG(PAGE_QOS_SCHEDULER, REG_PN_QOS_PRI_CTL_PORT_0 + (port), &val32, 4);
        val32 &= ~(PN_QOS_SCHED_SEL_M<<PN_QOS_SCHED_SEL_S); /* Clear Bits */
        val32 |= (SF2_ALL_Q_SP<<PN_QOS_SCHED_SEL_S); /* Set SP for all */
        SF2SW_WREG(PAGE_QOS_SCHEDULER, REG_PN_QOS_PRI_CTL_PORT_0 + (port), &val32, 4);
    }
    /* Set default TC to COS config */
    _sf2_tc_to_cos_default();
}

typedef struct acb_config_s {
    uint16 total_xon_hyst;
    uint16 xon_hyst;
    acb_queue_config_t acb_queue_config[64];
} acb_config_t;

static acb_config_t acb_profiles [] = {
    // profile 1
    {
        .total_xon_hyst = 6,
        .xon_hyst = 4,
        {
            // queue 0, (port 0) for LAN->LAN
#if defined(ACB_ALGORITHM2)   // For 138, Algorithm2 is applied
            {
                .pessimistic_mode = 0,
                .total_xon_en = 1,
                .pkt_len = 0,
                .xoff_threshold = 16,
            },
#else // For 148
            {
                .pessimistic_mode = 1,
                .total_xon_en = 1,
                .pkt_len = 6,
                .xoff_threshold = 16,
            },
#endif
            // queue 1
            {0},
            // queue 2
            {0},
            // queue 3
            {0},
            // queue 4
            {0},
            // queue 5
            {0},
            // queue 6
            {0},
            // queue 7, for IMP->LAN
            {
                .pessimistic_mode = 1,
                .total_xon_en = 1,
                .pkt_len = 6,
                .xoff_threshold = 16,
            },
            // queue 8 (port 1, q 0)
            {
                .pessimistic_mode = 1,
                .total_xon_en = 1,
                .pkt_len = 6,
                .xoff_threshold = 16,
            },
            // queue 9
            {0},
            // queue 10
            {0},
            // queue 11
            {0},
            // queue 12
            {0},
            // queue 13
            {0},
            // queue 14
            {0},
            // queue 15
            {
                .pessimistic_mode = 1,
                .total_xon_en = 1,
                .pkt_len = 6,
                .xoff_threshold = 16,
            },
        },

    },
};

void _sf2_conf_acb_conges_profile(int profile)
{

    /* based on impl5/bcmsw.c:sf2_conf_acb_conges_profile() */
    volatile uint32_t *sf2_acb_control    = (void *) (SF2_ACB_CONTROL_REG);
    volatile uint32_t *sf2_acb_xon_thresh = (void *) (sf2_acb_control + 1);
    volatile uint32_t *sf2_acb_que_config = (void *) (sf2_acb_control + 2);
    acb_config_t *p = &acb_profiles [profile];
    uint32_t val32;
    int q;
    acb_queue_config_t *qp;

    // acb disable
    *sf2_acb_control &= ~SF2_ACB_EN;
    *sf2_acb_xon_thresh = (p->total_xon_hyst & SF2_ACB_BUFS_THRESH_M) << SF2_ACB_TOTAL_XON_BUFS_S |
                          (p->xon_hyst & SF2_ACB_BUFS_THRESH_M) << SF2_ACB_XON_BUFS_S;
    for (q = 0; q <= SF2_ACB_QUE_MAX; q++)
    {

        /*
         * We have made room to configure each of the 64 queues with differently
         * defined per q values.
         * Here, we are however, duplicating q0 profiled ACB  config on every queue so we do not
         * leave queues congested for ever when ACB is enabled by default.
         */
        val32 = 0;
        qp = &p->acb_queue_config[0];
        val32 |= (qp->pessimistic_mode & SF2_ACB_QUE_PESSIMISTIC_M)
                                        << SF2_ACB_QUE_PESSIMISTIC_S;
        val32 |= (qp->total_xon_en & SF2_ACB_QUE_TOTAL_XON_M) << SF2_ACB_QUE_TOTAL_XON_S;
        val32 |= (qp->xon_en & SF2_ACB_QUE_XON_M) << SF2_ACB_QUE_XON_S;
        val32 |= (qp->total_xoff_en & SF2_ACB_QUE_TOTAL_XOFF_M) << SF2_ACB_QUE_TOTAL_XOFF_S;
        val32 |= (qp->pkt_len & SF2_ACB_QUE_PKT_LEN_M) << SF2_ACB_QUE_PKT_LEN_S;
        val32 |= (qp->total_xoff_threshold & SF2_ACB_BUFS_THRESH_M)
                                        << SF2_ACB_QUE_TOTOAL_XOFF_BUFS_S;
        val32 |= (qp->xoff_threshold & SF2_ACB_BUFS_THRESH_M) << SF2_ACB_QUE_XOFF_BUFS_S;
        *(sf2_acb_que_config + q) =  val32;
    }
    // acb enable
#if defined(ACB_ALGORITHM2)   // For 138, Algorithm2 is applied
    *sf2_acb_control |= SF2_ACB_EN | (SF2_ACB_ALGORITHM_M << SF2_ACB_ALGORITHM_S);
#else
    *sf2_acb_control |= SF2_ACB_EN;
#endif
}

void sf2_rreg_mmap(int page, int reg, void *data_out, int len);
void sf2_wreg_mmap(int page, int reg, void *data_in, int len);

static void extsw_rreg_wrap(int page, int reg, void *vptr, int len)
{
    uint8 val[8];
    uint8 *data = (uint8*)vptr;
    int type = len & SWAP_TYPE_MASK;

    len &= ~(SWAP_TYPE_MASK);

    /* Lower level driver always returnes in Little Endian data from history */
    sf2_rreg_mmap(page, reg, val, len);

    switch (len) {
        case 1:
            data[0] = val[0];
            break;
        case 2:
            *((uint16 *)data) = __le16_to_cpu(*((uint16 *)val));
            break;
        case 4:
            *((uint32 *)data) = __le32_to_cpu(*((uint32 *)val));
            break;
        case 6:
            switch (type) {
                case DATA_TYPE_HOST_ENDIAN:
                default:
                    /*
                        Value type register access
                        Input:  val:Le64 from Lower driver API
                        Output: data:Host64, a pointer to the begining of 64 bit buffer
                    */
                    *(uint64*)data = __le64_to_cpu(*(uint64 *)val);
                    break;
                case DATA_TYPE_BYTE_STRING:
                    /*
                        Byte array for MAC address
                        Input:  val:Mac[5...0] from lower driver
                        Output: data:Mac[0...5]
                    */
                    *(uint64 *)val = __swab64(*(uint64*)val);
                    memcpy(data, val+2, 6);
                    break;
            }
            break;
        case 8:
            switch (type) {
                case DATA_TYPE_HOST_ENDIAN:
                default:
                    /*
                        Input:  val: Le64 for lower driver API
                        Output: data: Host64
                    */
                    *(uint64 *)data = __le64_to_cpu(*(uint64*)val);
                    break;
                case DATA_TYPE_BYTE_STRING:
                    /*
                        Input:  val:byte[7...0] from lower driver API
                        Output: data:byte[0...7]
                    */
                    *(uint64 *)data = __swab64(*(uint64*)val);
                    break;
                case DATA_TYPE_VID_MAC:
                    /*
                        VID-MAC type;
                        Input:  val:Mac[5...0]|VidLeWord from Lower Driver API
                        Output: data:VidHostWord|Mac[0...5] for Caller
                    */
                    /* [Mac[5-0]]|[LEWord]; First always swap all bytes */
                    *((uint64 *)data) = __swab64(*((uint64 *)val));
                    /* Now is [BEWord]|[Mac[0-5]]; Conditional Swap 2 bytes */
                    *((uint16 *)&data[0]) = __be16_to_cpu(*((uint16 *)&data[0]));
                    /* Now is HostEndianWord|Mac[0-5] */
                    break;
                case DATA_TYPE_MIB_COUNT:
                    /*
                        MIB Counter Type:
                        Input:  [LeHiDw][LeLoDw]
                        Output: [HostHiDw][HostLoDw]
                    */
                    *((uint32 *)&data[0]) = __le32_to_cpu(*((uint32 *)&val[0]));
                    *((uint32 *)&data[4]) = __le32_to_cpu(*((uint32 *)&val[4]));
                    break;
            } // switch type
            break;
        default:
            printk("Length %d not supported\n", len);
            break;
    }
}

static void extsw_wreg_wrap(int page, int reg, void *vptr, int len)
{
    uint8 val[8];
    uint8 *data = (uint8*)vptr;
    int type = len & SWAP_TYPE_MASK;

    len  &= ~(SWAP_TYPE_MASK);

    switch (len) {
        case 1:
            val[0] = data[0];
            break;
        case 2:
            *((uint16 *)val) = __cpu_to_le16(*((uint16 *)data));
            break;
        case 4:
            *((uint32 *)val) = __cpu_to_le32(*((uint32 *)data));
            break;
        case 6:
            switch(type) {
                case DATA_TYPE_HOST_ENDIAN:
                default:
                    /*
                        Value type register access
                        Input:  data:Host64, a pointer to the begining of 64 bit buffer
                        Output: val:Le64
                    */
                    *(uint64 *)val = __cpu_to_le64(*(uint64 *)data);
                    break;
                case DATA_TYPE_BYTE_STRING:
                    /*
                        Byte array for MAC address
                        Input:  data:MAC[0...5] from Host
                        Output: val:Mac[5...0] for lower driver API
                    */
                    memcpy(val+2, data, 6);
                    *(uint64 *)val = __swab64(*(uint64*)val);
                    break;
            }
            break;
        case 8:
            switch (type)
            {
                case DATA_TYPE_HOST_ENDIAN:
                default:
                    /*
                        Input: data:Host64
                        Output:  val:Le64 for lower driver API
                    */
                    *(uint64 *)val = __cpu_to_le64(*(uint64*)data);
                    break;
                case DATA_TYPE_BYTE_STRING:
                    /*
                        Input:  data:byte[0...7]
                        Output: val:byte[7...0] for lower driver API
                    */
                    *(uint64 *)val = __swab64(*(uint64*)data);
                    break;
                case DATA_TYPE_VID_MAC:
                    /*
                        VID-MAC type;
                        Input:  VidHostWord|Mac[0...5]
                        Output: Mac[5..0.]|VidLeWord for Lower Driver API
                    */
                    /* Contains HostEndianWord|MAC[0-5] Always swap first*/
                    *((uint64 *)val) = __swab64(*((uint64 *)data));
                    /* Now it is MAC[5-0]|SwappedHostEndianWord */
                    /* Convert the SwappedHostEndianWord to Little Endian; thus BE */
                    *((uint16 *)&val[6]) = __cpu_to_be16(*((uint16 *)&val[6]));
                    /* Now is MAC[5-0]|LEWord as requested by HW */
                    break;
                case DATA_TYPE_MIB_COUNT:
                    /*
                        MIB Counter Type:
                        Input:  [HostHiDw][HostLoDw]
                        Output: [LeHiDw][LeLoDw]  for Lower driver API
                    */
                    *((uint32 *)&val[0]) = __le32_to_cpu(*((uint32 *)&data[0]));
                    *((uint32 *)&val[4]) = __le32_to_cpu(*((uint32 *)&data[4]));
                    break;
            } // switch type
            break;
        default:
            printk("Length %d not supported\n", len);
            break;
    } // switch len
    sf2_wreg_mmap(page, reg, val, len);
}

// =========== public ioctl functions =====================


// ----------- SIOCETHSWCTLOPS ETHSWCOSSCHED functions ---
/*
 * Get/Set StarFighter port scheduling policy
 *** Input params
 * e->type  GET/SET
 * e->port_qos_sched.num_spq  Tells SP/WRR policy to use on the port's queues
 * e->port_qos_sched.wrr_type Granularity packet or 256 byte
 * e->port  per port
 *** Output params
 * e->val has current sched policy - GET
 * Returns 0 for Success, Negative value for failure.
 */
int ioctl_extsw_cosq_sched(enetx_port_t *self, struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c:sf2_cosq_sched()
    int reg;
    int i, j;
    uint8_t data[8];
    uint8_t val8 = 0;

    down(&self->p.parent_sw->s.conf_sem);

    reg = REG_PN_QOS_PRI_CTL_PORT_0 + e->port * REG_PN_QOS_PRI_CTL_SZ;
    SF2SW_RREG(PAGE_QOS_SCHEDULER, reg, &val8, REG_PN_QOS_PRI_CTL_SZ);
    if (e->type == TYPE_GET) {
        switch ((val8 >> PN_QOS_SCHED_SEL_S ) & PN_QOS_SCHED_SEL_M)
        {
            case 0:
                e->port_qos_sched.sched_mode = BCM_COSQ_STRICT;
                break;
            case 5:
                e->port_qos_sched.sched_mode = BCM_COSQ_WRR;
                break;
            default:
                e->port_qos_sched.sched_mode = BCM_COSQ_COMBO;
                e->port_qos_sched.num_spq    = (val8 & PN_QOS_SCHED_SEL_M) << PN_QOS_SCHED_SEL_S;
                break;
        }
        e->port_qos_sched.port_qos_caps = QOS_SCHED_SP_CAP | QOS_SCHED_WRR_CAP | QOS_SCHED_WDR_CAP |
                                        QOS_SCHED_COMBO | QOS_PORT_SHAPER_CAP | QOS_QUEUE_SHAPER_CAP;
        e->port_qos_sched.max_egress_q = NUM_EGRESS_QUEUES;
        e->port_qos_sched.max_egress_spq = MAX_EGRESS_SPQ;
        e->port_qos_sched.wrr_type = (val8 >> PN_QOS_WDRR_GRAN_S) & PN_QOS_WDRR_GRAN_M?
                       QOS_ENUM_WRR_PKT: QOS_ENUM_WDRR_PKT;
        SF2SW_RREG(PAGE_QOS_SCHEDULER, REG_PN_QOS_WEIGHT_PORT_0 +
                       e->port * REG_PN_QOS_WEIGHTS, data, DATA_TYPE_HOST_ENDIAN|REG_PN_QOS_WEIGHTS);
        for (i = 0; i < BCM_COS_COUNT; i++) {
            e->weights[i] = data[i];
        }
    } else { // TYPE_SET
        val8 &= ~(PN_QOS_SCHED_SEL_M << PN_QOS_SCHED_SEL_S);
        if (e->port_qos_sched.sched_mode == BCM_COSQ_WRR) {
                val8 |= (SF2_ALL_Q_WRR & PN_QOS_SCHED_SEL_M) << PN_QOS_SCHED_SEL_S;
        } else if ((e->port_qos_sched.sched_mode == BCM_COSQ_SP) &&
                           (e->port_qos_sched.num_spq == 0)) {
                val8 |= (SF2_ALL_Q_SP & PN_QOS_SCHED_SEL_M) << PN_QOS_SCHED_SEL_S;

        } else {
            switch (e->port_qos_sched.num_spq) {
                case 1:
                    val8 |= (SF2_Q7_SP & PN_QOS_SCHED_SEL_M) << PN_QOS_SCHED_SEL_S;
                    break;
                case 2:
                    val8 |= (SF2_Q7_Q6_SP & PN_QOS_SCHED_SEL_M) << PN_QOS_SCHED_SEL_S;
                    break;
                case 3:
                    val8 |= (SF2_Q7_Q5_SP & PN_QOS_SCHED_SEL_M) << PN_QOS_SCHED_SEL_S;
                    break;
                case 4:
                    val8 |= (SF2_Q7_Q4_SP & PN_QOS_SCHED_SEL_M) << PN_QOS_SCHED_SEL_S;
                    break;
                default:
                    enet_dbg("Incorrect num_spq param %d", e->port_qos_sched.num_spq);
                    up(&self->p.parent_sw->s.conf_sem);
                    return -BCM_E_PARAM;
                    break;
            }
        }
        if (e->port_qos_sched.wrr_type == QOS_ENUM_WRR_PKT) {
            val8 |= SF2_WRR_PKT << PN_QOS_WDRR_GRAN_S;
        } else if (e->port_qos_sched.wrr_type == QOS_ENUM_WDRR_PKT) {
            val8 |= SF2_WDRR_PKT << PN_QOS_WDRR_GRAN_S;
        }
        SF2SW_WREG(PAGE_QOS_SCHEDULER, reg, &val8, REG_PN_QOS_PRI_CTL_SZ);
 // programming queue weights.
        if (e->port_qos_sched.num_spq || e->port_qos_sched.sched_mode == BCM_COSQ_WRR) {
                      // some or all queues in weighted mode.
            SF2SW_RREG(PAGE_QOS_SCHEDULER, REG_PN_QOS_WEIGHT_PORT_0 +
                           e->port * REG_PN_QOS_WEIGHTS, data, DATA_TYPE_HOST_ENDIAN | REG_PN_QOS_WEIGHTS);
            i = e->port_qos_sched.weights_upper? (BCM_COS_COUNT/2): 0;
            for (j = 0; j < BCM_COS_COUNT/2; i++, j++) {
                data[i] = e->weights[i];
            }
            SF2SW_WREG(PAGE_QOS_SCHEDULER, REG_PN_QOS_WEIGHT_PORT_0 + e->port * REG_PN_QOS_WEIGHTS,
                            data, DATA_TYPE_HOST_ENDIAN | REG_PN_QOS_WEIGHTS);
        }
    } // SET
    up(&self->p.parent_sw->s.conf_sem);
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWDUMPPAGE functions ---
void ioctl_extsw_dump_page0(void)
{
    // based on impl5/bcmsw.c:sf2_dump_page0()
    // TODO_DSL? Do we want to change memory mapped to register read access?
    int i = 0, page = 0;
    volatile EthernetSwitchCore *e = ETHSW_CORE;
    EthernetSwitchCore *f = (void *)NULL;

#define SHOW_REG_FIELD(mb) (int)(((long)&(f->mb)/sizeof(f->mb))&0xff), ((uint32)(e->mb)), ((uint32)(e->mb))
    printk("#The Page0 Registers \n");
    for (i=0; i<9; i++)
        printk("%02x %02x = 0x%02x (%u) \n", page, SHOW_REG_FIELD(port_traffic_ctrl[i])); /* 0x00 - 0x08 */

    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(switch_mode));
#if !defined(CONFIG_BCM963158)
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(pause_quanta)); /*0x0c */
#endif
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(imp_port_state)); /*0x0e */
#if !defined(CONFIG_BCM963158)
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(led_refresh)); /* 0x0f */
    for (i=0; i<2; i++)
        printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(led_function[i].led_f)); /* 0x10 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(led_function_map)); /* 0x14 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(led_enable_map)); /* 0x16 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(led_mode_map0)); /* 0x18 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(led_function_map1)); /* 0x1a */
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(reserved2[3]));  /* 0x1f */
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(port_forward_ctrl)); /* 0x21 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(protected_port_selection)); /* 0x24 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(wan_port_select)); /* 0x26 */
    printk("%02x %02x 0x%08x (%u) \n", page, SHOW_REG_FIELD(pause_capability)); /*0x28*/
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(reserved_multicast_control)); /* 0x2f */
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(txq_flush_mode_control)); /* 0x31 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(ulf_forward_map)); /* 0x32 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(mlf_forward_map));  /* 0x34 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(mlf_impc_forward_map)); /* 0x36 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(pause_pass_through_for_rx)); /* 0x38 */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(pause_pass_through_for_tx)); /* 0x3a */
    printk("%02x %02x 0x%04x (%u) \n", page, SHOW_REG_FIELD(disable_learning)); /* 0x3c */
    for (i=0; i<8; i++)
        printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(port_state_override[i])); /* 0x58 - 0x5f */
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(software_reset)); /* 0x79 */
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(pause_frame_detection)); /* 0x80 */
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(fast_aging_ctrl)); /* 0x88 */
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(fast_aging_port)); /* 0x89 */
    printk("%02x %02x 0x%02x (%u) \n", page, SHOW_REG_FIELD(fast_aging_vid));  /* 0x8a */
#endif
}

typedef struct unit_to_sw_s {
    int unit; 
    int cnt;
    enetx_port_t *sw;
} unit_to_sw_t;

static int tr_get_sw_from_unit(enetx_port_t *port, void *ctx)
{
    unit_to_sw_t *us = ctx;    

    if (us->unit == us->cnt) {
        us->sw = port;
        return 1;
    }
    us->cnt++;
    return 0;
}

static enetx_port_t *enet_get_sw_from_unit(int unit)
{
    int rc;
    unit_to_sw_t us = {unit};
    rc = port_traverse_ports(root_sw, tr_get_sw_from_unit, PORT_CLASS_SW, &us);
    if (rc)
        return us.sw;
    return NULL;
}

int tr_mdk_phy_pbmap(enetx_port_t *port, void *ctx)
{
    int *phy_map = ctx;
    phy_dev_t *phy = get_active_phy(port->p.phy);
    
    if (phy && IsPhyConnected(phy->meta_id) &&
            !port_is_enet_link_handling(port))
        *phy_map |= (1<<port->p.mac->mac_id);
    return 0;
}

static int enet_get_phy_mdk_pbmap(enetx_port_t *sw)
{
    int phy_map = 0;
    port_traverse_ports(sw, tr_mdk_phy_pbmap, PORT_CLASS_PORT, &phy_map);
    return phy_map;
}

// ----------- SIOCETHSWCTLOPS ETHSWINFO functions ---
int ioctl_extsw_info(struct ethswctl_data *e)
{
    // based on shared\bcmswaccess.c:enet_ioctl_ethsw_info()
    int bus_type = MBUS_NONE;
    ETHERNET_MAC_INFO *EnetInfo = (ETHERNET_MAC_INFO *)BpGetEthernetMacInfoArrayPtr();
    ETHERNET_MAC_INFO *info;
    unsigned int vend_id = 0, dev_id = 0, rev_id = 0;
    int i;
    enetx_port_t *port;
    phy_dev_t *phy_dev;
    phy_drv_t *phy_drv;
    u16 v16;
    enetx_port_t *sw;

    if (e->val > BP_MAX_ENET_MACS) {
        e->ret_val = bus_type;
        return BCM_E_NONE;
    }

    info = &EnetInfo[e->val];
    if ((info->ucPhyType == BP_ENET_EXTERNAL_SWITCH) ||
            (info->ucPhyType == BP_ENET_SWITCH_VIA_INTERNAL_PHY)) {

        switch (info->usConfigType) {
            case BP_ENET_CONFIG_MMAP:
                bus_type = MBUS_MMAP;   /* SF2 based bus type is always MMAP */
                if (e->val == 1) { /* unit 1 */
                    for (i = 0; i < BP_MAX_SWITCH_PORTS; i++) {
                        uint32_t meta_id;

                        port = unit_port_array[e->val][i];
                        phy_dev = get_active_phy(port->p.phy);
                        meta_id = phy_dev ? phy_dev->meta_id : 0;
                        if(((1<<i) & info->sw.port_map) == 0 || !IsPhyConnected(meta_id) ||
                                IsExtPhyId(meta_id) || !port) {
                            continue;
                        }

                        phy_drv = phy_dev->phy_drv;
                        phy_bus_read(phy_dev, 2, &v16);
                        vend_id = v16;
                        vend_id = __le32_to_cpu(vend_id);
                        phy_bus_read(phy_dev, 3, &v16);
                        dev_id = v16;
                        dev_id = __le32_to_cpu(dev_id);
                        enet_dbgv("vendor=%x dev=%x\n", vend_id, dev_id);
                        if (dev_id >= 0xb000) {
                            rev_id = dev_id & 0xF;
                            dev_id &= 0xFFF0;
                        }
                        break;
                    }

                    if(i == BP_MAX_SWITCH_PORTS) {
                        enet_err("Error: No integrated PHY defined for device ID in this board design.\n");
                        return -EFAULT;
                    }
                }
                break;
            default:
                break;
        }
    }

    e->ret_val = bus_type;
    e->vendor_id = vend_id;
    e->dev_id = dev_id;
    e->rev_id = rev_id;
    e->port_map = info->sw.port_map;

    sw = enet_get_sw_from_unit(e->val);
    e->phy_portmap = enet_get_phy_mdk_pbmap(sw);

    return BCM_E_NONE;
}


// ----------- SIOCETHSWCTLOPS ETHSWREGACCESS functions ---
int ioctl_extsw_regaccess(struct ethswctl_data *e, enetx_port_t *port)
{
    // based on shared\bcmswaccess.c:enet_ioctl_ethsw_regaccess()
    unsigned char data[8] = {0};

    if (e->offset & IS_PHY_ADDR_FLAG)
        return _ethsw_phy_access(e, port);

    if ( ((e->length != 1) && (e->length % 2)) || (e->length > 8)) {
        enet_err("Invalid length");
        return -EINVAL;
    }

    if (e->unit != 1) /* Assumption : SF2 is unit#1.
                       * Offset checks are not valid below for SF2 because all offsets
                       * are converted to 4-byte accesses later down the road. */
    {
        /*
            We do have unlignment register access: 0x0583 4 bytes and 0x0581 2bytes for VLAN configuration.
            So unligned access is needed and driver underneath is working correctly.
            Make exaption for 0x0583 and 0x0581
        */
        if (!_alignment_check(e))
        {
            enet_err("len = %d offset = 0x%04x alignment error !! \n", e->length, e->offset);
            return -EINVAL;
        }
    }

    if (e->type == TYPE_GET) {
        if (e->unit == 1) {
            SF2SW_RREG(((e->offset & 0xFF00)>>8), (e->offset & 0xFF), data, e->length);
        }
        else
        {
            enet_err("internal switch reg read not supported.!! (offset=%x, len=%d)\n", e->offset, e->length);
            return -EINVAL;
            //ethsw_read_reg(e->offset, data, e->length);
        }
        memcpy((void*)(&e->data), (void*)&data, e->length);

#if 0 && defined(enet_dbgv_enabled)
        //enet_dbgv("sf2 sw  read(page=%x reg=%x len=%d data=",  (e->offset & 0xFF00)>>8, e->offset & 0xFF, e->length);
        for (i = 0; i < e->length; i++)
            printk("%02x ", e->data[i]);
        printk(")\n");
#endif
    } else {
        if (e->unit == 1) {
#if 0 && defined(enet_dbgv_enabled)
            int i;
            enet_dbgv("sf2 sw write(page=%x reg=%x len=%d data=",  (e->offset & 0xFF00)>>8, e->offset & 0xFF, e->length);
            for (i = 0; i < e->length; i++)
                printk("%02x ", e->data[i]);
            printk(")\n");
#endif
            SF2SW_WREG(((e->offset & 0xFF00)>>8), (e->offset & 0xFF), e->data, e->length);
        }
        else
        {
            enet_err("internal switch reg write not supported.!! (offset=%x, len=%d)\n", e->offset, e->length);
            return -EINVAL;
            //ethsw_write_reg(e->offset, e->data, e->length);
        }
    }

    return BCM_E_NONE;

}

// ----------- SIOCETHSWCTLOPS ETHSWMIRROR functions ---
int ioctl_extsw_port_mirror_ops(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw.c:bcmsw_port_mirror_get/set() */
    uint16_t v16;

    if (e->type == TYPE_GET) {
        e->port_mirror_cfg.tx_port = -1;
        e->port_mirror_cfg.rx_port = -1;

        SF2SW_RREG(PAGE_MANAGEMENT, REG_MIRROR_CAPTURE_CTRL, &v16, sizeof(v16));
        if (v16 & REG_MIRROR_ENABLE)
        {
            e->port_mirror_cfg.enable = 1;
            e->port_mirror_cfg.mirror_port = v16 & REG_CAPTURE_PORT_M;
            e->port_mirror_cfg.blk_no_mrr = v16 & REG_BLK_NOT_MIRROR;
            SF2SW_RREG(PAGE_MANAGEMENT, REG_MIRROR_INGRESS_CTRL, &v16, sizeof(v16));
            e->port_mirror_cfg.ing_pmap = v16 & REG_INGRESS_MIRROR_M;
            SF2SW_RREG(PAGE_MANAGEMENT, REG_MIRROR_EGRESS_CTRL, &v16, sizeof(v16));
            e->port_mirror_cfg.eg_pmap = v16 & REG_EGRESS_MIRROR_M;
        }
        else
        {
            e->port_mirror_cfg.enable = 0;
        }
    } else {
        if (e->port_mirror_cfg.enable)
        {
            v16 = REG_MIRROR_ENABLE;
            v16 |= (e->port_mirror_cfg.mirror_port & REG_CAPTURE_PORT_M);
            v16 |= e->port_mirror_cfg.blk_no_mrr?REG_BLK_NOT_MIRROR:0;

            SF2SW_WREG(PAGE_MANAGEMENT, REG_MIRROR_CAPTURE_CTRL, &v16, sizeof(v16));
            v16 = e->port_mirror_cfg.ing_pmap & REG_INGRESS_MIRROR_M;
            SF2SW_WREG(PAGE_MANAGEMENT, REG_MIRROR_INGRESS_CTRL, &v16, sizeof(v16));
            v16 = e->port_mirror_cfg.eg_pmap & REG_INGRESS_MIRROR_M;
            SF2SW_WREG(PAGE_MANAGEMENT, REG_MIRROR_EGRESS_CTRL, &v16, sizeof(v16));
        }
        else
        {
            v16  = 0;
            SF2SW_WREG(PAGE_MANAGEMENT, REG_MIRROR_CAPTURE_CTRL, &v16, sizeof(v16));
        }
    }

    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWPORTTRUNK functions ---
int ioctl_extsw_port_trunk_ops(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw.c:bcmsw_port_trunk_get/set() */
    uint16_t v16;
    uint8_t v8;

    if (e->type == TYPE_GET) {
        SF2SW_RREG(PAGE_MAC_BASED_TRUNK, REG_TRUNK_GRP_CTL , &v16, 2);
        e->port_trunk_cfg.grp0_pmap = (v16 >> TRUNK_EN_GRP_S) & TRUNK_EN_GRP_M ;
        SF2SW_RREG(PAGE_MAC_BASED_TRUNK, REG_TRUNK_GRP_CTL+2 , &v16, 2);
        e->port_trunk_cfg.grp1_pmap = (v16 >> TRUNK_EN_GRP_S) & TRUNK_EN_GRP_M ;

        SF2SW_RREG(PAGE_MAC_BASED_TRUNK, REG_MAC_TRUNK_CTL, &v8, 1);
        e->port_trunk_cfg.enable = (v8 >> TRUNK_EN_LOCAL_S) & TRUNK_EN_LOCAL_M;
        e->port_trunk_cfg.hash_sel = (v8 >> TRUNK_HASH_SEL_S) & TRUNK_HASH_SEL_M;
    } else {
        SF2SW_RREG(PAGE_MAC_BASED_TRUNK, REG_MAC_TRUNK_CTL, &v8, 1);
        v8 &= ~(TRUNK_HASH_SEL_M<<TRUNK_HASH_SEL_S); /* Clear old hash selection first */
        v8 |= ( (e->port_trunk_cfg.hash_sel & TRUNK_HASH_SEL_M) << TRUNK_HASH_SEL_S ); /* Set Hash Selection */
        SF2SW_WREG(PAGE_MAC_BASED_TRUNK, REG_MAC_TRUNK_CTL, &v8, 1);
        printk("LAG/Trunking hash selection changed <0x%01x>\n",v8);
    }

    return 0;
}


// ----------- SIOCETHSWCTLOPS ETHSWCONTROL functions ---
/*
 * Get/Set StarFighter flow control options.
 *** Input params
 * e->type  GET/SET
 * e->sub_type Flow control type
 * e->val enable/disable above flow control type
 *** Output params
 * e->val has result for GET
 * Returns 0 for Success, Negative value for failure.
 */
static int _sf2_pause_drop_ctrl(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw.c sf2_pause_drop_ctrl() */
    uint16_t val = 0;
    uint16_t val2 = 0;
    if (e->type == TYPE_SET)    { // SET
        SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_PAUSE_DROP_CTRL, &val, 2);
        switch (e->sub_type) {
            case bcmSwitchFcMode:
                val2 = e->val? FC_CTRL_MODE_PORT: 0;
                SF2SW_WREG(PAGE_FLOW_CTRL_XTN, REG_FC_CTRL_MODE, &val2, 2);
                return 0;
            case bcmSwitchQbasedpauseEn:
                val &= ~FC_QUEUE_BASED_PAUSE_EN;
                val |= e->val? FC_QUEUE_BASED_PAUSE_EN: 0;
                break;
            case bcmSwitchTxBasedFc:
                val &= ~FC_TX_BASED_CTRL_EN;
                val |= e->val? FC_TX_BASED_CTRL_EN: 0;
                val &= ~FC_RX_BASED_CTRL_EN;
                val |= e->val? 0: FC_RX_BASED_CTRL_EN;
                val &= ~FC_RX_DROP_EN;
                val |= e->val? 0: FC_RX_DROP_EN;
                break;
            case bcmSwitchTxQdropEn:
                val &= ~FC_TX_TXQ_DROP_EN;
                val |= e->val? FC_TX_TXQ_DROP_EN: 0;
                break;
            case bcmSwitchTxTotdropEn:
                val &= ~FC_TX_TOTAL_DROP_EN;
                val |= e->val? FC_TX_TOTAL_DROP_EN: 0;
                break;
            case bcmSwitchTxQpauseEn:
                val &= ~FC_TX_TXQ_PAUSE_EN;
                val |= e->val? FC_TX_TXQ_PAUSE_EN: 0;
                break;
            case bcmSwitchTxTotPauseEn:
                val &= ~FC_TX_TOTAL_PAUSE_EN;
                val |= e->val? FC_TX_TOTAL_PAUSE_EN: 0;
                break;
            case bcmSwitchTxQpauseEnImp0:
                val &= ~FC_TX_IMP0_TXQ_PAUSE_EN;
                val |= e->val? FC_TX_IMP0_TXQ_PAUSE_EN: 0;
                break;
            case bcmSwitchTxTotPauseEnImp0:
                val &= ~FC_TX_IMP0_TOTAL_PAUSE_EN;
                val |= e->val? FC_TX_IMP0_TOTAL_PAUSE_EN: 0;
                break;
            default:
                printk("%s unknown fc type %u \n", __FUNCTION__, (unsigned int)e->sub_type);
                return -BCM_E_ERROR;
                break;
        }
        SF2SW_WREG(PAGE_FLOW_CTRL_XTN, REG_FC_PAUSE_DROP_CTRL, &val, 2);
    } else {
        //   GET
        val2 = 0;
        SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_PAUSE_DROP_CTRL, &val, 2);
        val2 |= val & FC_QUEUE_BASED_PAUSE_EN? 1 << bcmSwitchQbasedpauseEn: 0;
        val2 |= val & FC_TX_BASED_CTRL_EN? 1 << bcmSwitchTxBasedFc: 0;
        val2 |= val & FC_TX_TXQ_DROP_EN? 1 << bcmSwitchTxQdropEn: 0;
        val2 |= val & FC_TX_TOTAL_DROP_EN? 1 << bcmSwitchTxTotdropEn: 0;
        val2 |= val & FC_TX_TXQ_PAUSE_EN? 1 << bcmSwitchTxQpauseEn: 0;
        val2 |= val & FC_TX_TOTAL_PAUSE_EN? 1 << bcmSwitchTxTotPauseEn: 0;
        val2 |= val & FC_TX_IMP0_TXQ_PAUSE_EN? 1 << bcmSwitchTxQpauseEnImp0: 0;
        val2 |= val & FC_TX_IMP0_TOTAL_PAUSE_EN? 1 << bcmSwitchTxTotPauseEnImp0: 0;

        SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_CTRL_MODE, &val, 2);
        val2 |= val & FC_CTRL_MODE_PORT? 1 << bcmSwitchFcMode: 0;
        e->val = val2;
        enet_dbg("%s: val2 = 0x%x \n", __FUNCTION__, val2);
    }
    return 0;
}

int ioctl_extsw_control(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw.c:bcmeapi_ioctl_extsw_control() */
    int ret = 0;
    uint8_t val8 = 0;
    unsigned int val;
    switch (e->sw_ctrl_type) {
        case bcmSwitchBufferControl:

            if ((ret = _sf2_pause_drop_ctrl(e)) >= 0) {
                if (e->type == TYPE_GET) {
                    e->ret_val = e->val;
                    //enet_dbg("e->ret_val is = %4x\n", e->ret_val);
                 }
            }
            break;

        case bcmSwitch8021QControl:
            /* Read the 802.1Q control register */
            SF2SW_RREG(PAGE_8021Q_VLAN, REG_VLAN_GLOBAL_8021Q, &val8, 1);
            if (e->type == TYPE_GET) {
                val = (val8 >> VLAN_EN_8021Q_S) & VLAN_EN_8021Q_M;
                if (val && ((val8 >> VLAN_IVL_SVL_S) & VLAN_IVL_SVL_M))
                    val = 2; // IVL mode
                e->val  = val;
                //enet_dbg("e->val is = %4x\n", e->val);
            } else {  // 802.1Q SET
                /* Enable/Disable the 802.1Q */
                if (e->val == 0)
                    val8 &= (~(VLAN_EN_8021Q_M << VLAN_EN_8021Q_S));
                else {
                    val8 |= (VLAN_EN_8021Q_M << VLAN_EN_8021Q_S);
                    if (e->val == 1) // SVL
                        val8 &= (~(VLAN_IVL_SVL_M << VLAN_IVL_SVL_S));
                    else if (e->val == 2) // IVL
                        val8 |= (VLAN_IVL_SVL_M << VLAN_IVL_SVL_S);
                }
                SF2SW_WREG(PAGE_8021Q_VLAN, REG_VLAN_GLOBAL_8021Q, &val8, 1);
            }
            break;

        default:
            //up(&bcm_ethlock_switch_config);
            ret = -BCM_E_PARAM;
            break;
    } //switch
    return ret;
}


// ----------- SIOCETHSWCTLOPS ETHSWPRIOCONTROL functions ---
/*
 * Get/Set StarFighter switch Flowcontrol thresholds.
 *** Input params
 * e->type  GET/SET
 * e->sw_ctrl_type buffer threshold type
 * e->port that determines LAN/IMP0/IMP1 to pick the register set
 * e->val  buffer threshold value to write
 *** Output params
 * e->val has buffer threshold value read for GET
 * Returns 0 for Success, Negative value for failure.
 */
static int _sf2_prio_control(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw.c sf2_prio_control() */
    uint16_t val16;
    int reg = 0, page, curThreConfMode, curMaxStreamNumber;

    down(&sf2_sw->s.conf_sem);
    switch (e->sw_ctrl_type)
    {
        case bcmSwitchTxQThresholdConfigMode:
        case bcmSwitchTotalPorts:
        case bcmSwitchLinkUpLanPorts:
        case bcmSwitchLinkUpWanPorts:
        case bcmSwitchMaxStreams:
            break;
        case bcmSwitchTxQHiReserveThreshold:
            reg = REG_FC_LAN_TXQ_THD_RSV_QN0 + (e->priority * 2);
            break;
        case bcmSwitchTxQHiHysteresisThreshold:
            reg = REG_FC_LAN_TXQ_THD_HYST_QN0 + (e->priority * 2);
            break;
        case bcmSwitchTxQHiPauseThreshold:
            reg = REG_FC_LAN_TXQ_THD_PAUSE_QN0 + (e->priority * 2);
            break;
        case bcmSwitchTxQHiDropThreshold:
            reg = REG_FC_LAN_TXQ_THD_DROP_QN0 + (e->priority * 2);
            break;
        case bcmSwitchTotalHysteresisThreshold:
            reg = REG_FC_LAN_TOTAL_THD_HYST_QN0 + (e->priority * 2);
            break;
        case bcmSwitchTotalPauseThreshold:
            reg = REG_FC_LAN_TOTAL_THD_PAUSE_QN0 + (e->priority * 2);
            break;
        case bcmSwitchTotalDropThreshold:
            reg = REG_FC_LAN_TOTAL_THD_DROP_QN0 + (e->priority * 2);
            break;
        default:
            enet_err("Unknown threshold type \n");
            up(&sf2_sw->s.conf_sem);
            return -BCM_E_PARAM;
    }

    if(e->port == SF2_IMP0_PORT)
    {
        page = PAGE_FC_IMP0_TXQ;
    } else if(e->port == SF2_WAN_IMP1_PORT) {
        page = PAGE_FC_IMP1_TXQ;
    } else if ((e->port < SF2_IMP0_PORT) && (e->port != SF2_INEXISTANT_PORT)) {
        page = PAGE_FC_LAN_TXQ;
    } else {
        enet_err("port # %d error \n", e->port);
        up(&sf2_sw->s.conf_sem);
        return -BCM_E_PARAM;
    }

    //enet_dbg("Threshold: page %d  register offset = %#4x", page, reg);
    /* select port if port based threshold configuration in force */
    if (page == PAGE_FC_LAN_TXQ)
    {
        SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_CTRL_MODE, &val16, 2);
        if (val16 & FC_CTRL_MODE_PORT) {
            /* port number to port select register */
            val16 = 1 << (REG_FC_CTRL_PORT_P0 + e->port);
            SF2SW_WREG(PAGE_FLOW_CTRL_XTN, REG_FC_CTRL_PORT_SEL, &val16, 2);
        }
    }

    if (e->type == TYPE_GET)
    {
        switch (e->sw_ctrl_type)
        {
            case bcmSwitchTxQThresholdConfigMode:
                e->val = queThreConfMode;
                break;
            case bcmSwitchTotalPorts:
                e->val = total_ports + 1;
                break;
            case bcmSwitchLinkUpLanPorts:
                e->val = lanUpPorts_g;
                break;
            case bcmSwitchLinkUpWanPorts:
                e->val = wanUpPorts_g;
                break;
            case bcmSwitchMaxStreams:
                e->val = maxStreamNumber;
                break;
            default:
                SF2SW_RREG(page, reg, &val16, 2);
                //enet_dbg("Threshold read = %4x", val16);
                e->val = val16;
        }
    }
    else
    {
        switch (e->sw_ctrl_type)
        {
            case bcmSwitchTxQThresholdConfigMode:
                curThreConfMode = queThreConfMode;
                queThreConfMode = (uint32_t)e->val;
                if (curThreConfMode != queThreConfMode && queThreConfMode != ThreModeManual )
                {
                    _sf2_conf_que_thred();
                }
                break;
            case bcmSwitchMaxStreams:
                curMaxStreamNumber = maxStreamNumber;
                maxStreamNumber = e->val;
                if (maxStreamNumber == -1) maxStreamNumber = MaxStreamNumber;
                if (curMaxStreamNumber != maxStreamNumber && queThreConfMode != ThreModeManual )
                {
                    _sf2_conf_que_thred();
                }
                break;
            default:
                val16 = (uint32_t)e->val;
                //ent_dbg("e->val is = %4x", e->val);
                SF2SW_WREG(page, reg, &val16, 2);
        }
    }
    up(&sf2_sw->s.conf_sem);
    return 0;
}

int ioctl_extsw_prio_control(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw.c:bcmeapi_ioctl_extsw_prio_control() */
    int ret = 0;
    if ((ret =  _sf2_prio_control(e)) >= 0) {
        if (e->type == TYPE_GET) {
            e->ret_val = e->val;
            //enet_dbg("e->ret_val is = %4x", e->ret_val);
        }
    }
    return ret;
}

// ----------- SIOCETHSWCTLOPS ETHSWQUEMAP functions ---
int ioctl_extsw_que_map(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw.c:bcmeapi_ioctl_que_map() */
    if (e->type == TYPE_SET) {
        if (e->val != -1) wanQueMap = e->val;
        if (e->priority != -1) queRemap = e->priority;
        _sf2_conf_que_thred();
    }
    e->val = wanQueMap;
    e->priority = queRemap;

    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWQUEMON functions ---
static void _check_que_mon_port(int port)
{
    /* based on impl5/bcmsw_runner.c: check_que_mon_port() */
    static uint16 last_port = -1;

    if (last_port == port) return;
    last_port = port;
    SF2SW_WREG(PAGE_FLOW_CTRL_XTN, REG_FC_DIAG_CTRL, &last_port, 2);
    return;
}

int ioctl_extsw_que_mon(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw_runner.c:bcmeapi_ioctl_que_mon() */
    int port = e->port,
        que = e->priority,
        type = e->sw_ctrl_type,
        val, err = 0;
    uint16_t v16 = 0;

    switch(type)
    {
        case QUE_CUR_COUNT:
            _check_que_mon_port(port);
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_CUR_COUNT + que*2, &v16, 2);
            break;
        case QUE_PEAK_COUNT:
            _check_que_mon_port(port);
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_PEAK_COUNT + que*2, &v16, 2);
            break;
        case SYS_TOTAL_PEAK_COUNT:
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_SYS_TOTAL_PEAK_COUNT, &v16, 2);
            break;
        case SYS_TOTAL_USED_COUNT:
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_SYS_TOTAL_USED_COUNT, &v16, 2);
            break;

        case PORT_PEAK_RX_BUFFER:
            _check_que_mon_port(port);
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_PEAK_RX_BUFFER, &v16, 2);
            break;
        case QUE_FINAL_CONGESTED_STATUS:
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_FINAL_CONG_STAT + 2*port, &v16, 2);
            break;
        case PORT_PAUSE_HISTORY:
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_PAUSE_HISTORY, &v16, 2);
            break;
        case PORT_PAUSE_QUAN_HISTORY:
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_PAUSE_QUAN_HISTORY, &v16, 2);
            break;

        case PORT_RX_BASE_PAUSE_HISTORY:
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_RXBASE_PAUSE_HISTORY, &v16, 2);
            break;
        case PORT_RX_BUFFER_ERROR_HISTORY:
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_RX_BUFFER_ERR_HISTORY, &v16, 2);
            break;
        case QUE_CONGESTED_STATUS:
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_CONG_STATUS + 2*port, &v16, 2);
            break;
        case QUE_TOTAL_CONGESTED_STATUS:
            SF2SW_RREG(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_TOTAL_CONG_STATUS + 2*port, &v16, 2);
            break;
    }

    val = v16;
    e->val = val;
    e->ret_val = err;
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWPBVLAN functions ---
int ioctl_extsw_pbvlan(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw.c:bcmeapi_ioctl_extsw_pbvlan() */
    uint16_t val16;

    //enet_dbg("Given Port: 0x%02x \n ", e->port);
    if (e->port >= TOTAL_SWITCH_PORTS) {
        printk("Invalid Switch Port \n");
        return BCM_E_ERROR;
    }

    if (e->type == TYPE_GET) {
        down(&sf2_sw->s.conf_sem);
        SF2SW_RREG(PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P0 + (e->port*2), &val16, 2);
        up(&sf2_sw->s.conf_sem);
        e->fwd_map = val16;
        //enet_dbg("get e->fwd_map is = %4x\n", e->fwd_map);
    } else {
        val16 = (uint32_t)e->fwd_map;
        //enet_dbg("set e->fwd_map is = %4x\n", e->fwd_map);
        down(&sf2_sw->s.conf_sem);
        SF2SW_WREG(PAGE_PORT_BASED_VLAN, REG_VLAN_CTRL_P0 + (e->port*2), &val16, 2);
        up(&sf2_sw->s.conf_sem);
    }

    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWCOSPORTMAP functions ---
/*
 * Get/Set cos(queue) mapping, given priority (TC)
 *** Input params
 * e->type  GET/SET
 * e->queue - target queue
 * e->port  per port
 *** Output params
 * Returns 0 for Success, Negative value for failure.
 */
int ioctl_extsw_cosq_port_mapping(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw.c:bcmeapi_ioctl_extsw_cosq_port_mapping() */
    union {
        uint32_t val32;
        uint16_t val16;
    }val;
    int queue;
    int retval = 0;
    uint16_t reg_addr;
    uint16_t cos_shift;
    uint16_t cos_mask;
    uint16_t reg_len;

    //enet_dbg("Given port: %02d priority: %02d \n ", e->port, e->priority);
    if (e->port >= TOTAL_SWITCH_PORTS || e->port == SF2_INEXISTANT_PORT) {
        printk("Invalid Switch Port %02d \n", e->port);
        return -BCM_E_ERROR;
    }
    if ((e->priority > MAX_PRIORITY_VALUE) || (e->priority < 0)) {
        printk("Invalid Priority \n");
        return -BCM_E_ERROR;
    }
    reg_addr  = SF2_REG_PORTN_TC_TO_COS + e->port * 4;
    cos_shift = SF2_QOS_COS_SHIFT;
    cos_mask  = SF2_QOS_COS_MASK;
    reg_len   = 4;

    down(&sf2_sw->s.conf_sem);

    if (e->type == TYPE_GET) {
        SF2SW_RREG(PAGE_QOS, reg_addr, (uint8_t *)&val, reg_len);
        //enet_dbg("REG_QOS_PORT_PRIO_MAP_Px = %p\n", (void*)&val);
        /* Get the queue */
        queue = (val.val32 >> (e->priority * cos_shift)) & cos_mask;
        retval = queue & SF2_QOS_COS_MASK;
        //enet_dbg("%s queue is = %4x\n", __FUNCTION__, retval);
    } else {
        //enet_dbg("Given queue: 0x%02x \n ", e->queue);
        SF2SW_RREG(PAGE_QOS, reg_addr, (uint8_t *)&val, reg_len);
        val.val32 &= ~(cos_mask << (e->priority * cos_shift));
        val.val32 |= (e->queue & cos_mask) << (e->priority * cos_shift);
        SF2SW_WREG(PAGE_QOS, reg_addr, (uint8_t *)&val, reg_len);
    }
    up(&sf2_sw->s.conf_sem);
    return retval;
}

// ----------- SIOCETHSWCTLOPS ETHSWACBCONTROL functions ---
#if defined(ACB_ALGORITHM2)
static void _sf2_set_acb_algorithm(int algorithm)
{
    volatile uint32_t *sf2_acb_control    = (void *) (SF2_ACB_CONTROL_REG);
    volatile uint32_t *sf2_acb_que_config = (void *) (sf2_acb_control + 2);
    volatile uint32_t *sf2_acb_que_in_flight = (void *)(SF2_ACB_QUE0_PKTS_IN_FLIGHT);
    uint32_t val32, q;

    if (algorithm) // ACB_ALGORITHM2
    {
        *sf2_acb_control &= ~(SF2_ACB_EN|(SF2_ACB_ALGORITHM_M<<SF2_ACB_ALGORITHM_S)|
                (SF2_ACB_FLUSH_M<<SF2_ACB_FLUSH_S)|(SF2_ACB_EOP_DELAY_M<<SF2_ACB_EOP_DELAY_S));

        for (q = 0; q <= SF2_ACB_QUE_MAX; q++)
        {
            val32 = *(sf2_acb_que_config + q);
            val32 &= ~((SF2_ACB_QUE_PESSIMISTIC_M << SF2_ACB_QUE_PESSIMISTIC_S)|
                    (SF2_ACB_QUE_PKT_LEN_M<<SF2_ACB_QUE_PKT_LEN_S));
            *(sf2_acb_que_config + q) =  val32;
            *(sf2_acb_que_in_flight + q) = 0;
        }

        *sf2_acb_control |= SF2_ACB_EN|(SF2_ACB_ALGORITHM_M<<SF2_ACB_ALGORITHM_S)|
            (0x32<<SF2_ACB_EOP_DELAY_S);
    }
    else
    {
        *sf2_acb_control &= ~(SF2_ACB_EN|(SF2_ACB_ALGORITHM_M<<SF2_ACB_ALGORITHM_S)|
                (SF2_ACB_FLUSH_M<<SF2_ACB_FLUSH_S)|(SF2_ACB_EOP_DELAY_M<<SF2_ACB_EOP_DELAY_S));
        *sf2_acb_control &= ~SF2_ACB_EN;
        for (q = 0; q <= SF2_ACB_QUE_MAX; q++)
        {
            val32 = *(sf2_acb_que_config + q);
            val32 &= ~(SF2_ACB_QUE_PKT_LEN_M << SF2_ACB_QUE_PKT_LEN_S);
            val32 |= 6 << SF2_ACB_QUE_PKT_LEN_S;
            val32 |= (SF2_ACB_QUE_PESSIMISTIC_M << SF2_ACB_QUE_PESSIMISTIC_S);
            *(sf2_acb_que_config + q) =  val32;
            *(sf2_acb_que_in_flight + q) = 0;
        }
        *sf2_acb_control |= SF2_ACB_EN;
    }
}
#endif /* defined(ACB_ALGORITHM2) */

int ioctl_extsw_cfg_acb(struct ethswctl_data *e)
{
    /* based on impl5/bcmsw_runner.c:sf2_config_acb() */
    uint32_t val32, val;
    acb_q_params_t acb_conf_info;
    volatile uint32_t *sf2_acb_control    = (void *) (SF2_ACB_CONTROL_REG);
    volatile uint32_t *sf2_acb_xon_thresh = (void *) (sf2_acb_control + 1);
    volatile uint32_t *sf2_acb_que_config = (void *) (sf2_acb_control + 2);
#if defined(ACB_ALGORITHM2)   // For 138, Algorithm2 is applied
    volatile uint32_t *sf2_acb_que_in_flight = (void *)(SF2_ACB_QUE0_PKTS_IN_FLIGHT);
#endif

    if (e->queue < 0 || e->queue > SF2_ACB_QUE_MAX) {
        printk("%s parameter error, queue 0x%x \n", 	__FUNCTION__, e->queue);
        return BCM_E_PARAM;
    }
    val   = *sf2_acb_xon_thresh;
    val32 = *(sf2_acb_que_config + e->queue);
    if (e->type == TYPE_GET) {
        switch (e->sw_ctrl_type) {
            case acb_en:
                acb_conf_info.acb_en =  *sf2_acb_control & SF2_ACB_EN;
                break;
#if defined(ACB_ALGORITHM2)   // For 138, Algorithm2 is applied
            case acb_eop_delay:
                acb_conf_info.eop_delay =  (*sf2_acb_control >> SF2_ACB_EOP_DELAY_S) & SF2_ACB_EOP_DELAY_M;
                break;
            case acb_flush:
                acb_conf_info.flush =  (*sf2_acb_control >> SF2_ACB_FLUSH_S) & SF2_ACB_FLUSH_M;
                break;
            case acb_algorithm:
                acb_conf_info.algorithm =  (*sf2_acb_control >> SF2_ACB_ALGORITHM_S) & SF2_ACB_ALGORITHM_M;
                break;
#endif
            case acb_tot_xon_hyst:
                acb_conf_info.total_xon_hyst =  (val >> SF2_ACB_TOTAL_XON_BUFS_S)
                    & SF2_ACB_BUFS_THRESH_M;
                break;
            case acb_xon_hyst:
                acb_conf_info.xon_hyst =  (val >> SF2_ACB_XON_BUFS_S)
                                                        & SF2_ACB_BUFS_THRESH_M;
                break;
            case acb_q_pessimistic_mode:
                acb_conf_info.acb_queue_config.pessimistic_mode = (val32  >> SF2_ACB_QUE_PESSIMISTIC_S)
                                                        & SF2_ACB_QUE_PESSIMISTIC_M;
                break;
            case acb_q_total_xon_en:
                acb_conf_info.acb_queue_config.total_xon_en = (val32  >> SF2_ACB_QUE_TOTAL_XON_S)
                                                         & SF2_ACB_QUE_TOTAL_XON_M;
                break;
            case acb_q_xon_en:
                acb_conf_info.acb_queue_config.xon_en = (val32  >> SF2_ACB_QUE_XON_S)
                                                         & SF2_ACB_QUE_XON_M;
                break;
            case acb_q_total_xoff_en:
                acb_conf_info.acb_queue_config.total_xoff_en = (val32  >> SF2_ACB_QUE_TOTAL_XOFF_S)
                                                         & SF2_ACB_QUE_TOTAL_XOFF_M;
                break;
            case acb_q_pkt_len:
                acb_conf_info.acb_queue_config.pkt_len = (val32  >> SF2_ACB_QUE_PKT_LEN_S)
                                                         & SF2_ACB_QUE_PKT_LEN_M;
                break;
            case acb_q_tot_xoff_thresh:
                acb_conf_info.acb_queue_config.total_xoff_threshold = (val32  >> SF2_ACB_QUE_TOTOAL_XOFF_BUFS_S)
                                                         & SF2_ACB_BUFS_THRESH_M;
                break;
            case acb_q_xoff_thresh:
                acb_conf_info.acb_queue_config.xoff_threshold = (val32  >> SF2_ACB_QUE_XOFF_BUFS_S)
                                                         & SF2_ACB_BUFS_THRESH_M;
                break;
#if defined(ACB_ALGORITHM2)   // For 138, Algorithm2 is applied
            case acb_q_pkts_in_flight:
                acb_conf_info.pkts_in_flight = *(sf2_acb_que_in_flight + e->queue) & SF2_ACB_QUE_PKTS_IN_FLIGHT_M;
                break;
#endif
            case acb_parms_all:
                acb_conf_info.acb_en =  *sf2_acb_control & SF2_ACB_EN;
#if defined(ACB_ALGORITHM2)   // For 138, Algorithm2 is applied
                acb_conf_info.eop_delay =  (*sf2_acb_control >> SF2_ACB_EOP_DELAY_S) & SF2_ACB_EOP_DELAY_M;
                acb_conf_info.flush =  (*sf2_acb_control >> SF2_ACB_FLUSH_S) & SF2_ACB_FLUSH_M;
                acb_conf_info.algorithm =  (*sf2_acb_control >> SF2_ACB_ALGORITHM_S) & SF2_ACB_ALGORITHM_M;
#endif
                acb_conf_info.total_xon_hyst =  (val >> SF2_ACB_TOTAL_XON_BUFS_S)
                                                        & SF2_ACB_BUFS_THRESH_M;
                acb_conf_info.xon_hyst =  (val >> SF2_ACB_XON_BUFS_S)
                                                        & SF2_ACB_BUFS_THRESH_M;
                acb_conf_info.acb_queue_config.pessimistic_mode = (val32  >> SF2_ACB_QUE_PESSIMISTIC_S)
                                                        & SF2_ACB_QUE_PESSIMISTIC_M;
                acb_conf_info.acb_queue_config.total_xon_en = (val32  >> SF2_ACB_QUE_TOTAL_XON_S)
                                                         & SF2_ACB_QUE_TOTAL_XON_M;
                acb_conf_info.acb_queue_config.xon_en = (val32  >> SF2_ACB_QUE_XON_S)
                                                         & SF2_ACB_QUE_XON_M;
                acb_conf_info.acb_queue_config.total_xoff_en = (val32  >> SF2_ACB_QUE_TOTAL_XOFF_S)
                                                         & SF2_ACB_QUE_TOTAL_XOFF_M;
                acb_conf_info.acb_queue_config.pkt_len = (val32  >> SF2_ACB_QUE_PKT_LEN_S)
                                                         & SF2_ACB_QUE_PKT_LEN_M;
                acb_conf_info.acb_queue_config.total_xoff_threshold = (val32  >> SF2_ACB_QUE_TOTOAL_XOFF_BUFS_S)
                                                         & SF2_ACB_BUFS_THRESH_M;
                acb_conf_info.acb_queue_config.xoff_threshold = (val32  >> SF2_ACB_QUE_XOFF_BUFS_S)
                                                         & SF2_ACB_BUFS_THRESH_M;
#if defined(ACB_ALGORITHM2)   // For 138, Algorithm2 is applied
                acb_conf_info.pkts_in_flight = *(sf2_acb_que_in_flight + e->queue) & SF2_ACB_QUE_PKTS_IN_FLIGHT_M;
#endif
                break;
            default:
                printk("%s: Get op %#x Unsupported \n", __FUNCTION__, e->sw_ctrl_type);
                return BCM_E_PARAM;
                break;
        }
        if (copy_to_user (e->vptr, &acb_conf_info, sizeof(acb_q_params_t))) {
            return -EFAULT;
        }
    } else {  // SET
        switch (e->sw_ctrl_type) {
            case acb_en:
                if (e->val)
                    *sf2_acb_control |= SF2_ACB_EN;
                else
                    *sf2_acb_control &= ~SF2_ACB_EN;
                return 0;
                break;
#if defined(ACB_ALGORITHM2)   // For 138, Algorithm2 is applied
            case acb_eop_delay:
                *sf2_acb_control &= ~SF2_ACB_EN;
                *sf2_acb_control &= ~(SF2_ACB_EOP_DELAY_M << SF2_ACB_EOP_DELAY_S);
                *sf2_acb_control |= (e->val & SF2_ACB_EOP_DELAY_M) << SF2_ACB_EOP_DELAY_S;
                *sf2_acb_control |= SF2_ACB_EN;
                return 0;
            case acb_flush:
                *sf2_acb_control &= ~SF2_ACB_EN;
                *sf2_acb_control &= ~(SF2_ACB_FLUSH_M << SF2_ACB_FLUSH_S);
                *sf2_acb_control |= (e->val & SF2_ACB_FLUSH_M) << SF2_ACB_FLUSH_S;
                *sf2_acb_control |= SF2_ACB_EN;
                return 0;
            case acb_algorithm:
                _sf2_set_acb_algorithm(e->val);
                return 0;
#endif
            case acb_tot_xon_hyst:
                *sf2_acb_control &= ~SF2_ACB_EN;
                *sf2_acb_xon_thresh = val | (e->val & SF2_ACB_BUFS_THRESH_M) << SF2_ACB_TOTAL_XON_BUFS_S;
                *sf2_acb_control |= SF2_ACB_EN;
                return 0;
                break;
            case acb_xon_hyst:
                *sf2_acb_control &= ~SF2_ACB_EN;
                *sf2_acb_xon_thresh = val | (e->val & SF2_ACB_BUFS_THRESH_M) << SF2_ACB_XON_BUFS_S;
                *sf2_acb_control |= SF2_ACB_EN;
                return 0;
                break;
            case acb_q_pessimistic_mode:
                val32 &= ~(SF2_ACB_QUE_PESSIMISTIC_M << SF2_ACB_QUE_PESSIMISTIC_S);
                val32 |= (e->val & SF2_ACB_QUE_PESSIMISTIC_M) << SF2_ACB_QUE_PESSIMISTIC_S;
                break;
            case acb_q_total_xon_en:
                val32 &= ~(SF2_ACB_QUE_TOTAL_XON_M << SF2_ACB_QUE_TOTAL_XON_S);
                val32 |= (e->val & SF2_ACB_QUE_TOTAL_XON_M) << SF2_ACB_QUE_TOTAL_XON_S;
                break;
            case acb_q_xon_en:
                val32 &= ~(SF2_ACB_QUE_XON_M << SF2_ACB_QUE_XON_S);
                val32 |= (e->val & SF2_ACB_QUE_XON_M) << SF2_ACB_QUE_XON_S;
                break;
            case acb_q_total_xoff_en:
                val32 &= ~(SF2_ACB_QUE_TOTAL_XOFF_M << SF2_ACB_QUE_TOTAL_XOFF_S);
                val32 |= (e->val & SF2_ACB_QUE_TOTAL_XON_M) << SF2_ACB_QUE_TOTAL_XON_S;
                break;
            case acb_q_pkt_len:
                val32 |=  (e->val & SF2_ACB_QUE_PKT_LEN_M) << SF2_ACB_QUE_PKT_LEN_S;
                break;
            case acb_q_tot_xoff_thresh:
                val32 |=  (e->val & SF2_ACB_BUFS_THRESH_M) << SF2_ACB_QUE_TOTOAL_XOFF_BUFS_S;
                break;
            case acb_q_xoff_thresh:
                val32 |=  (e->val & SF2_ACB_BUFS_THRESH_M) << SF2_ACB_QUE_XOFF_BUFS_S;
                break;
#if defined(ACB_ALGORITHM2)   // For 138, Algorithm2 is applied
            case acb_q_pkts_in_flight:
                printk("Warning: This register should only be set by HW, but set here.\n");
                *sf2_acb_control &= ~SF2_ACB_EN;
                *(sf2_acb_que_in_flight + e->queue) = e->val & SF2_ACB_QUE_PKTS_IN_FLIGHT_M;
                *sf2_acb_control |= SF2_ACB_EN;
                return BCM_E_PARAM;
#endif
            default:
                printk("%s: Set op %#x Unsupported \n", __FUNCTION__, e->sw_ctrl_type);
                return BCM_E_PARAM;
                break;
        }
        *sf2_acb_control &= ~SF2_ACB_EN;
        *(sf2_acb_que_config + e->queue) = val32;
        *sf2_acb_control |= SF2_ACB_EN;
        return 0;
    }// Set
    return 0;
}


// ----------- SIOCETHSWCTLOPS ETHSWARLACCESS functions ---
// based on bcmsw_runner.h
#define _enet_arl_read( mac, vid, val ) FALSE
#define _enet_arl_write(mac, vid, val) {}
#define _enet_arl_dump() {}  /* This should return status actually ?? */
#define _enet_arl_dump_multiport_arl() {}
#define _fast_age_all(age_static) {}

static inline int enet_arl_remove(char *mac) {return 0;}

// based on bcmsw.h
int _enet_arl_search_ext(uint8_t *mac, uint32_t *vid, uint32_t *val, int op);
#define _enet_arl_dump_ext() _enet_arl_search_ext(0, 0, 0, TYPE_DUMP)
#define _enet_arl_read_ext(mc, vd, vl) _enet_arl_search_ext(mc, vd, vl, TYPE_GET)
#define _enet_arl_remove_ext(mc) _enet_arl_search_ext(mc, 0, 0, TYPE_SET)


static int _enet_arl_access_reg_op(uint8_t v8)
{
    int timeout;

    SF2SW_WREG(PAGE_AVTBL_ACCESS, REG_ARL_TBL_CTRL, &v8, 1);
    for ( timeout = 10, SF2SW_RREG(PAGE_AVTBL_ACCESS, REG_ARL_TBL_CTRL, &v8, 1);
            (v8 & ARL_TBL_CTRL_START_DONE) && timeout;
            --timeout, SF2SW_RREG(PAGE_AVTBL_ACCESS, REG_ARL_TBL_CTRL, &v8, 1))
    {
        mdelay(1);
    }

    if (timeout <= 0)
    {
        printk("Error: ARL Operation Timeout\n");
        return 0;
    }
    return 1;
}

/* v32: b31 is raw bit,
    If raw: register format; etherwise: b15 is Valid bit */
int _enet_arl_write_ext(uint8_t *mac, uint16_t vid, uint32_t v32)
{
    uint8_t mac_vid[8];
    uint32_t cur_v32;
    uint16_t ent_vid;
    int bin, empty_bin = -1;

    if (!(v32 & (1<<31))) v32 = ((v32 & 0xfc00) << 1) | (v32 & 0x1ff);  /* If it is raw, shift valid bit left */
    v32 &= ~(1<<31);

    /* Write the MAC Address and VLAN ID */
    SF2SW_WREG(PAGE_AVTBL_ACCESS, REG_ARL_MAC_INDX_LO, mac, 6|DATA_TYPE_BYTE_STRING);
    SF2SW_WREG(PAGE_AVTBL_ACCESS, REG_ARL_VLAN_INDX, &vid, 2);
    if (!_enet_arl_access_reg_op(ARL_TBL_CTRL_START_DONE | ARL_TBL_CTRL_READ)) return 0;

    for (bin = 0; bin < REG_ARL_BINS_PER_HASH; bin++)
    {
        /* Read transaction complete - get the MAC + VID */
        SF2SW_RREG(PAGE_AVTBL_ACCESS, REG_ARL_MAC_LO_ENTRY + bin*0x10, &mac_vid[0], 8|DATA_TYPE_VID_MAC);
        SF2SW_RREG(PAGE_AVTBL_ACCESS, REG_ARL_DATA_ENTRY + bin*0x10,(uint8_t *)&cur_v32, 4);
        ent_vid = *(uint16_t*)mac_vid;

        if (!(v32 & ARL_DATA_ENTRY_VALID_531xx))
        {
            /* If it is del op, find the matched bin */
            if (memcmp(&mac[0], &mac_vid[2], 6) != 0 || ent_vid != vid) continue;
        }
        else
        {
            /* If it is a modification or addition,
               find a matching entry, empty slot or last slot */
            if (memcmp(&mac[0], &mac_vid[2], 6) == 0 && vid == ent_vid) goto found_slot;
            if (!(cur_v32 & ARL_DATA_ENTRY_VALID_531xx) && empty_bin == -1) empty_bin = bin;  /* save empty bin for non matching case */
            if (bin < REG_ARL_BINS_PER_HASH-1) continue;  /* Continue to find next bin for matching if it not the last */
            /* No matching found here, if there is empty bin, use empty_bin or use last bin */
            if (empty_bin != -1) bin = empty_bin;
        }

        found_slot:

        /* Modify the data entry for this ARL */
        *(uint16 *)(&mac_vid[0]) = (vid & 0xFFF);
        memcpy(&mac_vid[2], &mac[0], 6);
        SF2SW_WREG(PAGE_AVTBL_ACCESS, REG_ARL_MAC_LO_ENTRY + bin*0x10, mac_vid, 8|DATA_TYPE_VID_MAC);
        SF2SW_WREG(PAGE_AVTBL_ACCESS, REG_ARL_DATA_ENTRY + bin*0x10,(uint8_t *)&v32, 4);

        /* Initiate a write transaction */
        if (!_enet_arl_access_reg_op(ARL_TBL_CTRL_START_DONE)) return 0;
        return 1;
    }
    enet_err("Error - can't find the requested ARL entry\n");
    return 0;
}

int _enet_arl_entry_op(uint8_t *mac, uint32_t *vid, uint32_t *val, int op, int *count, u8 *mac_vid, u32 data)
{
    switch(op)
    {
        case TYPE_DUMP:
            if (*count == 0) printk("\nExternal Switch ARL Dump:\n");
            if ((((*count)++) % 10)==0)
            {
                printk("  No: VLAN  MAC          DATA" "(15:Valid,14:Static,13:Age,12-10:Pri,8-0:Port/Pmap)\n");
            }

            printk("%4d: %04d  %02x%02x%02x%02x%02x%02x 0x%04x\n",
                    *count, *(uint16 *)&mac_vid[0],
                    mac_vid[2], mac_vid[3], mac_vid[4], mac_vid[5], mac_vid[6], mac_vid[7],
                    ((data & 0x1f800)>>1)|(data&0x1ff));
            break;
        case TYPE_SET:
            if (memcmp(&mac[0], &mac_vid[2], 6) == 0)
            {
                _enet_arl_write_ext(mac, *(u16*)mac_vid, 0);
                (*count)++;
            }
            break;
        case TYPE_GET:
            if (memcmp(&mac[0], &mac_vid[2], 6) == 0 &&
                    (*vid == -1 || *vid == *(u16*)mac_vid))
            {
                /* entry found */
                *vid = *(uint16_t*)mac_vid;
                if (*val & (1<<31)) /* Raw flag passed down from users space */
                {
                    *val = data;
                }
                else
                {
                    *val = ((data & 0x1f800)>>1)|(data & 0x1ff);
                }
                /* Return FALSE to terminate loop */
                return TRUE;
            }
            break;
    }
    return FALSE;
}

int _enet_arl_search_ext(uint8_t *mac, uint32_t *vid, uint32_t *val, int op)
{
    int timeout = 1000, count = 0, hash_ent;
    uint32_t cur_data;
    uint8_t v8, mac_vid[8];

    v8 = ARL_SRCH_CTRL_START_DONE;
    SF2SW_WREG(PAGE_AVTBL_ACCESS, REG_ARL_SRCH_CTRL_531xx, (uint8_t *)&v8, 1);

    for( SF2SW_RREG(PAGE_AVTBL_ACCESS, REG_ARL_SRCH_CTRL_531xx, (uint8_t *)&v8, 1);
            (v8 & ARL_SRCH_CTRL_START_DONE);
            SF2SW_RREG(PAGE_AVTBL_ACCESS, REG_ARL_SRCH_CTRL_531xx, (uint8_t *)&v8, 1))
    {
        /* Now read the Search Ctrl Reg Until :
         * Found Valid ARL Entry --> ARL_SRCH_CTRL_SR_VALID, or
         * ARL Search done --> ARL_SRCH_CTRL_START_DONE */
        for(timeout = 1000;
                (v8 & ARL_SRCH_CTRL_SR_VALID) == 0 && (v8 & ARL_SRCH_CTRL_START_DONE) && timeout-- > 0;
                mdelay(1),
                SF2SW_RREG(PAGE_AVTBL_ACCESS, REG_ARL_SRCH_CTRL_531xx, (uint8_t *)&v8, 1));

        if ((v8 & ARL_SRCH_CTRL_SR_VALID) == 0 || timeout <= 0) break;

        /* Found a valid entry */
        for (hash_ent = 0; hash_ent < REG_ARL_SRCH_HASH_ENTS; hash_ent++)
        {
            SF2SW_RREG(PAGE_AVTBL_ACCESS, REG_ARL_SRCH_MAC_LO_ENTRY0_531xx + hash_ent*0x10,&mac_vid[0], 8|DATA_TYPE_VID_MAC);
            SF2SW_RREG(PAGE_AVTBL_ACCESS, REG_ARL_SRCH_DATA_ENTRY0_531xx + hash_ent*0x10,(uint8_t *)&cur_data, 4);

//            enet_dbg("ARL_SRCH_result (%02x%02x%02x%02x%02x%02x%02x%02x) \n",
//                    mac_vid[0],mac_vid[1],mac_vid[2],mac_vid[3],mac_vid[4],mac_vid[5],mac_vid[6],mac_vid[7]);
//            enet_dbg("ARL_SRCH_DATA = 0x%08x \n", cur_data);

            if ((cur_data & ARL_DATA_ENTRY_VALID_531xx))
            {
                if (_enet_arl_entry_op(mac, vid, val, op, &count, mac_vid, cur_data)) return TRUE;
            }
        }
    }

    if (timeout <= 0)
    {
        printk("ARL Search Timeout for Valid to be 1 \n");
    }

    if (op == TYPE_DUMP) printk("Done: Total %d entries\n", count);
    if (op == TYPE_GET) return FALSE;
    return TRUE;
}

void _enet_arl_dump_ext_multiport_arl(void)
{
    uint16 v16;
    uint8 addr[8];
    int i, enabled;
    uint32 vect;
    static char *cmp_type[] = {"Disabled", "Etype", "MAC Addr", "MAC Addr & Etype"};

    SF2SW_RREG(PAGE_ARLCTRL, REG_MULTIPORT_CTRL, &v16, 2);
    enabled = v16 & ((MULTIPORT_CTRL_EN_M << (5*2))| (MULTIPORT_CTRL_EN_M << (4*2))| (MULTIPORT_CTRL_EN_M << (3*2))|
            (MULTIPORT_CTRL_EN_M << (2*2))| (MULTIPORT_CTRL_EN_M << (1*2))| (MULTIPORT_CTRL_EN_M << (0*2)));

    printk("\nExternal Switch Multiport Address Dump: Function %s\n", enabled? "Enabled": "Disabled");
    if (!enabled) return;

    printk("  Mapping to ARL matching: %s\n", v16 & (1<<MULTIPORT_CTRL_DA_HIT_EN)? "Lookup Hit": "Lookup Failed");
    for (i=0; i<6; i++)
    {
        enabled = (v16 & (MULTIPORT_CTRL_EN_M << (5*i)));
        SF2SW_RREG(PAGE_ARLCTRL, REG_MULTIPORT_ADDR1_LO + i*16, (uint8 *)&addr, sizeof(addr)|DATA_TYPE_VID_MAC);
        SF2SW_RREG(PAGE_ARLCTRL, REG_MULTIPORT_VECTOR1 + i*16, (uint8 *)&vect, sizeof(vect));
        printk("Mport Eth Type: 0x%04x, Mport Addrs: %02x:%02x:%02x:%02x:%02x:%02x, Port Map %04x, Cmp Type: %s\n",
                *(uint16 *)(addr),
                addr[2],
                addr[3],
                addr[4],
                addr[5],
                addr[6],
                addr[7],
                (int)vect, cmp_type[enabled & MULTIPORT_CTRL_EN_M]);
    }
    printk("External Switch Multiport Address Dump Done\n");
}

int ioctl_extsw_arl_access(struct ethswctl_data *e)
{
    // based on impl5\ethsw.c bcmeapi_ioctl_ethsw_arl_access()
    int ret;

    switch(e->type)
    {
        case TYPE_GET:
            enet_dbg("get e->mac: %02x %02x %02x %02x %02x %02x e->vid: %d\n", e->mac[5],
                    e->mac[4], e->mac[3], e->mac[2], e->mac[1], e->mac[0], e->vid);

            switch (e->unit)
            {
                case 0:
                    ret = _enet_arl_read( e->mac, &e->vid, &e->val );
                    break;
                case 1:
                    ret = _enet_arl_read_ext(e->mac, &e->vid, &e->val );
                    break;
                default:
                    e->unit = 0;
                    if ((ret = _enet_arl_read(e->mac, &e->vid, &e->val)))
                    {
                        break;
                    }

                    e->unit = 1;
                    ret = _enet_arl_read_ext(e->mac, &e->vid, &e->val );
            }

            if (ret == FALSE)
            {
                return BCM_E_ERROR;
            }
            break;

        case TYPE_SET:
            enet_dbg("set e->mac: %02x %02x %02x %02x %02x %02x e->vid: %d\n", e->mac[5],
                    e->mac[4], e->mac[3], e->mac[2], e->mac[1], e->mac[0], e->vid);

            if (e->unit == 1)
            {
                if(e->vid == 0xffff && (e->val & ARL_DATA_ENTRY_VALID) == 0)
                {
                    _enet_arl_remove_ext(e->mac);
                }
                else
                {
                    _enet_arl_write_ext(e->mac, e->vid, e->val);
                }
            }
            else
            {
                if(e->vid == 0xffff && (e->val & ARL_DATA_ENTRY_VALID) == 0)
                {
                    enet_arl_remove(e->mac);
                }
                else
                {
                    _enet_arl_write(e->mac, e->vid, e->val);
                }
            }
            break;

        case TYPE_DUMP:
            _enet_arl_dump();
            _enet_arl_dump_multiport_arl();

            _enet_arl_dump_ext();                           // TODO_DSL? probably should move arl access to per switch object
            _enet_arl_dump_ext_multiport_arl();
            break;

        case TYPE_FLUSH:
            /* Flush the ARL table */
            _fast_age_all(0);
            _fast_age_all_ext(0);
            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


// ----------- SIOCETHSWCTLOPS ETHSWCOSPRIORITYMETHOD functions ---
/* This function just serves Star Fighter. Legacy External switch
 * goes with Robo.
 *
 * Get/Set StarFighter cos priority method
 *** Input params
 * e->type  GET/SET
 * e->pkt_type_mask - ipv4/ipv6:802.1p:static mac destination or port Id based
 * e->val - ingress classifier TC src selection -- DSCP, vlan pri,
 *        -  MAC addr, PORT based (default vlan tag)
 * e->port  per port
 *** Output params
 * Returns 0 for Success, Negative value for failure.
 */
static int _qos_dscp_is_enabled(int port)
{
    u16 val16;
    SF2SW_RREG(PAGE_QOS, REG_QOS_DSCP_EN, (void *)&val16, 2);
    return (val16 >> port) & 1;
}

static int _qos_8021p_is_enabled(int port)
{
    u16 val16;
    SF2SW_RREG(PAGE_QOS, REG_QOS_8021P_EN, (void *)&val16, 2);
    return (val16 >> port) & 1;
}

static void _enable_dscp_qos(int port, int enable)
{
    u16 val16;
    SF2SW_RREG(PAGE_QOS, REG_QOS_DSCP_EN, (void *)&val16, 2);
    val16 &= ~(1 << port);
    val16 |= enable << port;
    SF2SW_WREG(PAGE_QOS, REG_QOS_DSCP_EN, (void *)&val16, 2);
}

static void _enable_8021p_qos(int port, int enable)
{
    u16 val16;
    SF2SW_RREG(PAGE_QOS, REG_QOS_8021P_EN, (void *)&val16, 2);
    val16 &= ~(1 << port);
    val16 |= enable << port;
    SF2SW_WREG(PAGE_QOS, REG_QOS_8021P_EN, (void *)&val16, 2);
}

/* Note: Method values are UAPI definition */
static int _isQoSMethodEnabled(int port, int method)
{
    switch(method)
    {
        case PORT_QOS:
        case MAC_QOS:
            return 1;
        case IEEE8021P_QOS:
            return _qos_8021p_is_enabled(port);
        case DIFFSERV_QOS:
            return _qos_dscp_is_enabled(port);
    }
    return 0;
}

/* Note: Method values are UAPI definition */
static void _enableQosMethod(int port, int method, int enable)
{
    switch(method)
    {
        case MAC_QOS:
        case PORT_QOS:
            return;
        case IEEE8021P_QOS:
            if (enable)
            {
                _enable_8021p_qos(port, 1); // Enable PCP for the port
            }
            else
            {
                _enable_8021p_qos(port, 0);
            }
            return;
        case DIFFSERV_QOS:
            if (enable)
            {
                _enable_dscp_qos(port, 1); // Enable DSCP for the port
            }
            else
            {
                _enable_dscp_qos(port, 0);
            }
            return;
    }
}

#define QOS_METHOD_CNVT_UAPI_AND_REG(regQoS)  (~(regQoS) & SF2_QOS_TC_SRC_SEL_VAL_MASK)
#define QOS_METHODS_CNVT_UAPI_AND_REG(regQoSPorts)  (~(regQoSPorts) & 0xffff)
int ioctl_extsw_cos_priority_method_cfg(struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c sf2_cos_priority_method_config()
    uint16_t val16, reg_addr, pkt_type_mask, tc_sel_src ;
    uint32_t val32;
    int i, enable_qos;

    down(&sf2_sw->s.conf_sem);

    //enet_dbg("%s port %d pkt_type 0x%x Given method: %02d \n ",__FUNCTION__,
    //        e->port, e->pkt_type_mask, e->val);
    if (e->port < 0 || e->port > SF2_IMP0_PORT ||  e->port == SF2_INEXISTANT_PORT) {
        enet_dbg("parameter error, port %d \n", e->port);
        return -BCM_E_PARAM;
    }
    reg_addr = SF2_REG_PORTN_TC_SELECT_TABLE + e->port * 2;
    pkt_type_mask = e->pkt_type_mask;
    if (e->type == TYPE_GET) {
        SF2SW_RREG(PAGE_QOS, reg_addr, &val16, 2);
        if (e->pkt_type_mask == SF2_QOS_TC_SRC_SEL_PKT_TYPE_ALL)
        {
            val32 = QOS_METHODS_CNVT_UAPI_AND_REG(val16);
            for (i = 0; i < e->pkt_type_mask; i++)
            {
                tc_sel_src = (val16 >> (i * 2)) & SF2_QOS_TC_SRC_SEL_VAL_MASK;
                enable_qos = _isQoSMethodEnabled(e->port, QOS_METHOD_CNVT_UAPI_AND_REG(tc_sel_src));
                val32 |= !enable_qos << (16+i);
            }
        } else {
            pkt_type_mask &=  SF2_QOS_TC_SRC_SEL_PKT_TYPE_MASK;
            val16 =  (val16 >> (pkt_type_mask * 2)) & SF2_QOS_TC_SRC_SEL_VAL_MASK;
            val16 = QOS_METHOD_CNVT_UAPI_AND_REG(val16);
            enable_qos = _isQoSMethodEnabled(e->port, val16);
            val32 = (!enable_qos << 16) | val16;
        }
        // bits programmed in TC Select Table registers and software notion are bit inversed.
        e->ret_val = val32;
    } else { // TYPE_SET
        reg_addr = SF2_REG_PORTN_TC_SELECT_TABLE + e->port * 2;

        /* when pkt_type_mask is NOT SF2_QOS_TC_SRC_SEL_PKT_TYPE_ALL,
            tc_sel_src b0-1 2 bit show of TC.  b16-17 2bits contains disable bit.  */
        if (e->pkt_type_mask != SF2_QOS_TC_SRC_SEL_PKT_TYPE_ALL)
        {
            tc_sel_src = QOS_METHOD_CNVT_UAPI_AND_REG(e->val);
            SF2SW_RREG(PAGE_QOS, reg_addr, &val16, 2);
            pkt_type_mask = e->pkt_type_mask & SF2_QOS_TC_SRC_SEL_PKT_TYPE_MASK;
            val16 &= ~(SF2_QOS_TC_SRC_SEL_VAL_MASK << (pkt_type_mask * 2));
            val16 |=  (tc_sel_src & SF2_QOS_TC_SRC_SEL_VAL_MASK ) << (pkt_type_mask * 2);
            enable_qos = !((e->val >> 16) & 1);
            //enet_dbg("%s: Write to: len %d page 0x%x reg 0x%x val 0x%x\n",
            //        __FUNCTION__, 2, PAGE_QOS, reg_addr, val16);
            _enableQosMethod(e->port, e->val & SF2_QOS_TC_SRC_SEL_VAL_MASK, enable_qos);
        }
        else    /* SF2_QOS_TC_SRC_SEL_PKT_TYPE_ALL */
        /* when pkt_type_mask is SF2_QOS_TC_SRC_SEL_PKT_TYPE_ALL,
            tc_sel_src's lower 16 bits contains TC selections for all 8 packet types.
            higher 8 bits contains disable bit for corresponding methods in lower 16bits.  */
        {
            val16 = QOS_METHODS_CNVT_UAPI_AND_REG(e->val);
            for (i = 0; i < e->pkt_type_mask; i++)
            {
                tc_sel_src = ((e->val) >> (i * 2)) & SF2_QOS_TC_SRC_SEL_VAL_MASK;
                enable_qos = !((e->val >> (16 + i)) & 1);
                _enableQosMethod(e->port, tc_sel_src, enable_qos);
            }
        }
        SF2SW_WREG(PAGE_QOS, reg_addr, &val16, 2);
    }

    up(&sf2_sw->s.conf_sem);
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWJUMBO functions ---
static uint32 _ConfigureJumboPort(uint32 regVal, int portVal, unsigned int configVal) // bill
{
    // based on impl5\bcmenet.c ConfigureJumboPort()
    UINT32 controlBit;

    // Test for valid port specifier.
    if ((portVal >= ETHSWCTL_JUMBO_PORT_GPHY_0) && (portVal <= ETHSWCTL_JUMBO_PORT_ALL))
    {
        // Switch on port ID.
        switch (portVal)
        {
            case ETHSWCTL_JUMBO_PORT_MIPS:
                controlBit = ETHSWCTL_JUMBO_PORT_MIPS_MASK;
                break;
            case ETHSWCTL_JUMBO_PORT_GPON:
                controlBit = ETHSWCTL_JUMBO_PORT_GPON_MASK;
                break;
            case ETHSWCTL_JUMBO_PORT_USB:
                controlBit = ETHSWCTL_JUMBO_PORT_USB_MASK;
                break;
            case ETHSWCTL_JUMBO_PORT_MOCA:
                controlBit = ETHSWCTL_JUMBO_PORT_MOCA_MASK;
                break;
            case ETHSWCTL_JUMBO_PORT_GPON_SERDES:
                controlBit = ETHSWCTL_JUMBO_PORT_GPON_SERDES_MASK;
                break;
            case ETHSWCTL_JUMBO_PORT_GMII_2:
                controlBit = ETHSWCTL_JUMBO_PORT_GMII_2_MASK;
                break;
            case ETHSWCTL_JUMBO_PORT_GMII_1:
                controlBit = ETHSWCTL_JUMBO_PORT_GMII_1_MASK;
                break;
            case ETHSWCTL_JUMBO_PORT_GPHY_1:
                controlBit = ETHSWCTL_JUMBO_PORT_GPHY_1_MASK;
                break;
            case ETHSWCTL_JUMBO_PORT_GPHY_0:
                controlBit = ETHSWCTL_JUMBO_PORT_GPHY_0_MASK;
                break;
            default: // ETHSWCTL_JUMBO_PORT_ALL:
                controlBit = ETHSWCTL_JUMBO_PORT_MASK_VAL;  // ALL bits
                break;
        }

        // Test for accept JUMBO frames.
        if (configVal != 0)
        {
            // Setup register value to accept JUMBO frames.
            regVal |= controlBit;
        }
        else
        {
            // Setup register value to reject JUMBO frames.
            regVal &= ~controlBit;
        }
    }

    // Return new JUMBO configuration control register value.
    return regVal;
}


int ioctl_extsw_port_jumbo_control(struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c bcmeapi_ioctl_extsw_port_jumbo_control()
    uint32_t val32;

    if (e->type == TYPE_GET)
    {
        // Read & log current JUMBO configuration control register.
        SF2SW_RREG(PAGE_JUMBO, REG_JUMBO_PORT_MASK, (uint8 *)&val32, 4);
        enet_dbg("JUMBO_PORT_MASK = 0x%08X", (unsigned int)val32);
        e->ret_val = val32;
    }
    else
    {
        // Read & log current JUMBO configuration control register.
        SF2SW_RREG(PAGE_JUMBO, REG_JUMBO_PORT_MASK, (uint8 *)&val32, 4);
        enet_dbg("Old JUMBO_PORT_MASK = 0x%08X", (unsigned int)val32);

        // Setup JUMBO configuration control register.
        val32 = _ConfigureJumboPort(val32, e->port, e->val);
        SF2SW_WREG(PAGE_JUMBO, REG_JUMBO_PORT_MASK, (uint8 *)&val32, 4);

        // Attempt to transfer register write value to user space & test for success.
        e->ret_val = val32;
    }
    return BCM_E_NONE;
}

// ----------- SIOCETHSWCTLOPS ETHSWCOSPCPPRIOMAP functions ---
/*
 * Get/Set PCP to TC mapping Tabe entry given 802.1p priotity (PCP)
 * and mapped priority
 *** Input params
 * e->type  GET/SET
 * e->val -  pcp
 * e->priority - mapped TC value, case of SET
 *** Output params
 * e->priority - mapped TC value, case of GET
 * Returns 0 for Success, Negative value for failure.
 */
int ioctl_extsw_pcp_to_priority_mapping(struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c bcmeapi_ioctl_extsw_pcp_to_priority_mapping()
    uint32_t val32;
    uint16_t reg_addr;

    enet_dbg("Given pcp: %02d \n ", e->val);
    if (e->val > MAX_PRIORITY_VALUE) {
        enet_err("Invalid PCP Value %02d \n", e->val);
        return BCM_E_ERROR;
    }

    if (e->port < 0 || e->port > SF2_IMP0_PORT ||  e->port == SF2_INEXISTANT_PORT) {
        enet_err("Invalid port number %02d \n", e->port);
        return BCM_E_ERROR;
    }
    reg_addr = e->port == SF2_IMP0_PORT? SF2_REG_QOS_PCP_IMP0:
               e->port == SF2_P7? SF2_REG_QOS_PCP_P7:
                          REG_QOS_8021P_PRIO_MAP + e->port * QOS_PCP_MAP_REG_SZ;

    down(&sf2_sw->s.conf_sem);

    if (e->type == TYPE_GET) {
        SF2SW_RREG(PAGE_QOS, reg_addr, (void *)&val32, 4);
        e->priority = (val32 >> (e->val * QOS_TC_S)) & QOS_TC_M;
        enet_dbg("pcp %d is mapped to priority: %d \n ", e->val, e->priority);
    } else {
        enet_dbg("Given pcp: %02d priority: %02d \n ", e->val, e->priority);
        if ((e->priority > MAX_PRIORITY_VALUE) || (e->priority < 0)) {
            enet_err("Invalid Priority \n");
            up(&sf2_sw->s.conf_sem);
            return BCM_E_ERROR;
        }
        SF2SW_RREG(PAGE_QOS, reg_addr, (void *)&val32, 4);
        val32 &= ~(QOS_TC_M << (e->val * QOS_TC_S));
        val32 |= (e->priority & QOS_TC_M) << (e->val * QOS_TC_S);
        SF2SW_WREG(PAGE_QOS, reg_addr, (void *)&val32, 4);
    }

    up(&sf2_sw->s.conf_sem);
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWCOSPIDPRIOMAP functions ---
/*
 * Get/Set PID to TC mapping Table entry given ingress port
 * and mapped priority
 *** Input params
 * e->type  GET/SET
 * e->priority - mapped TC value, case of SET
 *** Output params
 * e->priority - mapped TC value, case of GET
 * Returns 0 for Success, Negative value for failure.
 */
int ioctl_extsw_pid_to_priority_mapping(struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c sf2_pid_to_priority_mapping()
    uint32_t val32;

    enet_dbg("Given uint %02d port %02d \n ", e->unit, e->port);

    if (e->port < 0 || e->port > SF2_IMP0_PORT ||  e->port == SF2_INEXISTANT_PORT) {
        enet_err("Invalid port number %02d \n", e->port);
        return BCM_E_ERROR;
    }
    down(&sf2_sw->s.conf_sem);

    if (e->type == TYPE_GET) {
        SF2SW_RREG(PAGE_QOS, SF2_REG_PORT_ID_PRIO_MAP, (void *)&val32, 4);
        e->priority = (val32 >> (e->port * QOS_TC_S)) & QOS_TC_M;
        enet_dbg("port %d is mapped to priority: %d \n ", e->port, e->priority);
    } else {
        enet_dbg("Given port: %02d priority: %02d \n ", e->port, e->priority);
        SF2SW_RREG(PAGE_QOS, SF2_REG_PORT_ID_PRIO_MAP, (void *)&val32, 4);
        val32 &= ~(QOS_TC_M << (e->port * QOS_TC_S));
        val32 |= (e->priority & QOS_TC_M) << (e->port * QOS_TC_S);
        SF2SW_WREG(PAGE_QOS, SF2_REG_PORT_ID_PRIO_MAP, (void *)&val32, 4);
    }

    up(&sf2_sw->s.conf_sem);
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWCOSDSCPPRIOMAP functions ---
/*
 * Get/Set DSCP to TC mapping Tabe entry given dscp value and priority
 *** Input params
 * e->type  GET/SET
 * e->val -  dscp
 * e->priority - mapped TC value, case of SET
 *** Output params
 * e->priority - mapped TC value, case of GET
 * Returns 0 for Success, Negative value for failure.
 */
int ioctl_extsw_dscp_to_priority_mapping(struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c bcmeapi_ioctl_extsw_dscp_to_priority_mapping()
    uint64 val64 = 0;
    uint32_t mapnum;
    int dscplsbs;

    enet_dbg("Given dscp: %02d \n ", e->val);
    if (e->val > QOS_DSCP_M) {
        enet_err("Invalid DSCP Value \n");
        return BCM_E_ERROR;
    }

    down(&sf2_sw->s.conf_sem);

    dscplsbs = e->val & QOS_DSCP_MAP_LSBITS_M;
    mapnum = (e->val >> QOS_DSCP_MAP_S) & QOS_DSCP_MAP_M;

    if (e->type == TYPE_GET) {
        SF2SW_RREG(PAGE_QOS, REG_QOS_DSCP_PRIO_MAP0LO + mapnum * QOS_DSCP_MAP_REG_SZ,
                                 (void *)&val64, QOS_DSCP_MAP_REG_SZ | DATA_TYPE_HOST_ENDIAN);
        e->priority = (val64 >> (dscplsbs * QOS_TC_S)) & QOS_TC_M;
        enet_dbg("dscp %d is mapped to priority: %d \n ", e->val, e->priority);
    } else {
        enet_dbg("Given priority: %02d \n ", e->priority);
        if ((e->priority > MAX_PRIORITY_VALUE) || (e->priority < 0)) {
            enet_err("Invalid Priority \n");
            up(&sf2_sw->s.conf_sem);
            return BCM_E_ERROR;
        }
        // LE assumptions below, TODO
        SF2SW_RREG(PAGE_QOS, REG_QOS_DSCP_PRIO_MAP0LO + mapnum * QOS_DSCP_MAP_REG_SZ,
                                     (void *)&val64, QOS_DSCP_MAP_REG_SZ | DATA_TYPE_HOST_ENDIAN);
        val64 &= ~(((uint64)(QOS_TC_M)) << (dscplsbs * QOS_TC_S));
        val64 |= ((uint64)(e->priority & QOS_TC_M)) << (dscplsbs * QOS_TC_S);
        enet_dbg(" @ addr %#x val64 to write = 0x%llx \n",
                                (REG_QOS_DSCP_PRIO_MAP0LO + mapnum * QOS_DSCP_MAP_REG_SZ),
                                (uint64) val64);

        SF2SW_WREG(PAGE_QOS, REG_QOS_DSCP_PRIO_MAP0LO + mapnum * QOS_DSCP_MAP_REG_SZ,
                                            (void *)&val64, QOS_DSCP_MAP_REG_SZ | DATA_TYPE_HOST_ENDIAN);
    }

    up(&sf2_sw->s.conf_sem);
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWPORTRXRATE functions ---
/*
 *  Set the burst size and rate limit value of the selected port ingress rate.
 *      limit  :   rate limit value. (Kbits)
 *      burst_size  :   max burst size. (Kbits)
 */
int ioctl_extsw_port_irc_set(struct ethswctl_data *e)
{
    // based on impl5\bcmsw_runner.c bcmeapi_ioctl_ethsw_port_irc_set()
    uint32_t  val32, bs = 0, rf = 0;

    down(&sf2_sw->s.conf_sem);

    if (e->limit == 0) { /* Disable ingress rate control */
        SF2SW_RREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
        val32 &= ~REG_PN_BUCK1_ENABLE_MASK;
        SF2SW_WREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
    } else {    /* Enable ingress rate control */
        bs = e->burst_size / 8;
        if (bs <= 4)
        {
            bs = 0;
        }
        else if (bs <= 8)
        {
            bs = 1;
        }
        else if (bs <= 16)
        {
            bs = 2;
        }
        else if (bs <= 32)
        {
            bs = 3;
        }
        else if (bs <= 64)
        {
            bs = 4;
        }
        else
        {
            bs = REG_PN_BUCK1_SIZE_M;
        }

        rf = e->limit;
        if (rf <= 1800)
        {
            rf = rf * 125 / 8 / 1000;
            if (rf == 0) rf = 1;
        }
        else if (rf <= 100000)
        {
            rf = rf / 1000 + 27;
        }
        else
        {
            rf = rf / 1000 / 8 + 115;
            if ( rf > 240) rf = 240;
        }

        SF2SW_RREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
        val32 &= ~((REG_PN_BUCK1_SIZE_M << REG_PN_BUCK1_SIZE_S)| (REG_PN_BUCK1_REF_CNT_M << REG_PN_BUCK1_REF_CNT_S));
        val32 |= REG_PN_BUCK1_ENABLE_MASK | REG_PN_BUCK1_MODE_MASK; // use bucket 1
        val32 |= (rf & REG_PN_BUCK1_REF_CNT_M) << REG_PN_BUCK1_REF_CNT_S;
        val32 |= bs  << REG_PN_BUCK1_SIZE_S;
        SF2SW_WREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);

        SF2SW_RREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_CFG_PORT_0 + e->port * 2, &val32, 4);
	    val32 |= REG_PN_BUCK1_IFG_BYTES_MASK | (REG_PN_BUCK1_PKT_SEL_M << REG_PN_BUCK1_PKT_SEL_S);
        SF2SW_WREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_CFG_PORT_0 + e->port * 2, &val32, 4);
    }

    up(&sf2_sw->s.conf_sem);
    return BCM_E_NONE;
}

/*
 *   Get the burst size and rate limit value of the selected port ingress rate.
 */
int ioctl_extsw_port_irc_get(struct ethswctl_data *e)
{
    // based on impl5\bcmsw_runner.c bcmeapi_ioctl_ethsw_port_irc_get()
    uint32_t  val32, rf;

    down(&sf2_sw->s.conf_sem);

    SF2SW_RREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
    if ((val32 & REG_PN_BUCK1_ENABLE_MASK) == 0)
    {
        e->limit = 0;
    }
    else
    {
        rf  = (val32 >> REG_PN_BUCK1_REF_CNT_S) & REG_PN_BUCK1_REF_CNT_M;
        if (rf <= 28)
        {
            e->limit = rf * 8000 / 125;
        }
        else if (rf <= 127)
        {
            e->limit = (rf - 27 ) * 1000;
        }
        else
        {
            if (rf > 240) rf = 240;
            e->limit = (rf - 115) * 1000 * 8;
        }

        e->burst_size = 4 << ((val32 >> REG_PN_BUCK1_SIZE_S) & REG_PN_BUCK1_SIZE_M);
        if (e->burst_size > 64) e->burst_size = 488;
        e->burst_size *= 8;
    }

    up(&sf2_sw->s.conf_sem);
    return BCM_E_NONE;
}

// ----------- SIOCETHSWCTLOPS ETHSWPORTTXRATE functions ---
/*
 * Get/Set StarFighter Egress shaper control
 *** Input params
 * e->port  egress port that is configured
 * e->unit  switch unit
 * e->type  GET/SET
 * e->limit egress rate control limit in
 *          64 kbps(Byte mode) 125 pps packet mode
 * e->burst_size egress burst in 64 Byte units(Byte mode)
 *          or in packets (packet mode)
 * e->queue egress queue if specified (per queue config)
 *          or -1 is not specified
 *** Output params
 * e->vptr has result copied for GET
 * Returns 0 for Success, Negative value for failure.
 */
int ioctl_extsw_port_erc_config(struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c sf2_port_erc_config()
    uint16_t val16, page, reg;
    uint32_t val32;
    uint32_t pkt_flag;
    unsigned char q_shaper = e->queue >= 0;

    if (e->type == TYPE_SET) {

        /* find queue or port page*/
        page = q_shaper? PAGE_Q0_EGRESS_SHAPER + e->queue:
                                    PAGE_PORT_EGRESS_SHAPER;
        /* find the shaper mode */
        pkt_flag =  (e->sub_type == SHAPER_PACKET_MODE) ? SHAPER_PACKET_MODE : 0;

        /* configure shaper rate limit; limit = 0 means disable shaper */
        val32 = 0; /* reset rate limiting by default */
        if (e->limit)
        {
            if (pkt_flag == SHAPER_PACKET_MODE)
            {
                val32 = e->limit/125; /* shaper rate config in 125pps units */
            }
            else {
                val32 = e->limit/64; /* shaper rate config in 64Kbps units */
            }
            if (!val32) {
                val32 = 1; /* At least 64Kbps */
            }
            val32 = val32 & SHAPER_RATE_BURST_VAL_MASK;
        }
        reg  =   pkt_flag == SHAPER_PACKET_MODE? SF2_REG_PN_SHAPER_RATE_PKT:
                                                  SF2_REG_PN_SHAPER_RATE_BYTE;
        SF2SW_WREG(page, reg + e->port * 4, &val32, 4);

        /* configure shaper burst size */
        val32 = 0; /* reset burst size by default */
        if (e->limit) { /* Only set burst size if shaper is getting enabled */
            if (pkt_flag == SHAPER_PACKET_MODE) {
                val32 = e->burst_size; /* shaper burst config in 1 packet units */
            }
            else {
                val32 = (e->burst_size /* Kbits */ * 1000)/(8*64); /* shaper burst config in 64Byte units */
            }
            if (!val32) {
                val32 = 1;
            }
            val32 = val32 & SHAPER_RATE_BURST_VAL_MASK;
        }
        reg  =   pkt_flag == SHAPER_PACKET_MODE? SF2_REG_PN_SHAPER_BURST_SZ_PKT:
                                                  SF2_REG_PN_SHAPER_BURST_SZ_BYTE;
        SF2SW_WREG(page, reg + e->port * 4, &val32, 4);

        /* enable shaper for byte mode or pkt mode as the case may be. */
        SF2SW_RREG(page, SF2_REG_SHAPER_ENB_PKT_BASED, &val16, 2);
        val16 &= ~(1 << e->port);
        val16 |= pkt_flag == SHAPER_PACKET_MODE? (1 << e->port): 0;
        SF2SW_WREG(page, SF2_REG_SHAPER_ENB_PKT_BASED, &val16, 2);

        /* Enable/disable shaper */
        SF2SW_RREG(page, SF2_REG_SHAPER_ENB, &val16, 2);
        val16 &= ~(1 << e->port); /* Disable Shaper */
        val16 |= e->limit? (1 << e->port): 0; /* Enable Shaper, if needed */
        SF2SW_WREG(page, SF2_REG_SHAPER_ENB, &val16, 2);

        return 0;
    } else {
        /* Egress shaper stats*/
        egress_shaper_stats_t stats;

        page = q_shaper? PAGE_Q0_EGRESS_SHAPER + e->queue:
                                    PAGE_PORT_EGRESS_SHAPER;
        SF2SW_RREG(page, SF2_REG_SHAPER_ENB_PKT_BASED, &val16, 2);
        pkt_flag = (val16 & (1 << e->port))? 1: 0;
        stats.egress_shaper_flags = 0;
        stats.egress_shaper_flags |= pkt_flag? SHAPER_RATE_PACKET_MODE: 0;

        reg = pkt_flag? SF2_REG_PN_SHAPER_RATE_PKT: SF2_REG_PN_SHAPER_RATE_BYTE;
        SF2SW_RREG(page, reg + e->port * 4, &val32, 4);
        stats.egress_rate_cfg = val32 & SHAPER_RATE_BURST_VAL_MASK;

        reg = pkt_flag? SF2_REG_PN_SHAPER_BURST_SZ_PKT: SF2_REG_PN_SHAPER_BURST_SZ_BYTE;
        SF2SW_RREG(page, reg + e->port * 4, &val32, 4);
        stats.egress_burst_sz_cfg = val32 & SHAPER_RATE_BURST_VAL_MASK;

        reg = SF2_REG_PN_SHAPER_STAT;
        SF2SW_RREG(page, reg + e->port * 4, &val32, 4);
        stats.egress_cur_tokens = val32 & SHAPER_STAT_COUNT_MASK;
        stats.egress_shaper_flags |= val32 & SHAPER_STAT_OVF_MASK? SHAPER_OVF_FLAG: 0;
        stats.egress_shaper_flags |= val32 & SHAPER_STAT_INPF_MASK? SHAPER_INPF_FLAG: 0;

        SF2SW_RREG(page, SF2_REG_SHAPER_ENB, &val16, 2);
        stats.egress_shaper_flags |= (val16 & (1 << e->port))? SHAPER_ENABLE: 0;

        SF2SW_RREG(page, SF2_REG_SHAPER_BLK_CTRL_ENB, &val16, 2);
        stats.egress_shaper_flags |= (val16 & (1 << e->port))? SHAPER_BLOCKING_MODE: 0;

        // applies only for port shaper
        if (!q_shaper) {
            SF2SW_RREG(page, SF2_REG_SHAPER_INC_IFG_CTRL, &val16, 2);
            stats.egress_shaper_flags |= (val16 & (1 << e->port))? SHAPER_INCLUDE_IFG: 0;
        }

        /* Convert the return values based on mode */
        if (pkt_flag)
        {
            stats.egress_rate_cfg *= 125; /* Shaper rate in 125pps unit */
            /* stats.egress_burst_sz_cfg  - burst unit in packets */
        }
        else {
            stats.egress_rate_cfg *= 64; /* Shaper rate in 64Kbps unit */
            stats.egress_burst_sz_cfg = (stats.egress_burst_sz_cfg*8*64)/1000; /* Shaper burst is in 64Byte unit - convert into kbits */
        }
        if (e->vptr) {
            if (copy_to_user (e->vptr, &stats, sizeof(egress_shaper_stats_t))) {
                return -EFAULT;
            }
        } else {
            // Just support Legacy API
            e->limit = stats.egress_rate_cfg;
            e->burst_size =  stats.egress_burst_sz_cfg;
        }
    }
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWPORTSHAPERCFG functions ---
inline static void _extsw_reg16_bit_ops(uint16 page, uint16 reg, int bit, int on)
{
    uint16 val16;

    SF2SW_RREG(page, reg, &val16, 2);
    val16 &= ~(1 << bit);
    val16 |= on << bit;
    SF2SW_WREG(page, reg, &val16, 2);
}


/*
 * As part of setting StarFighter Egress shaper
 * configuration, turn on/off various shaper modes.
 *** Input params
 * e->port  egress port that is configured
 * e->unit  switch unit
 * e->type  SET
 * e->queue egress queue if specified (per queue config)
 *          or -1 is not specified
 * e->sub_type - Or'ed Flags
 * e->val = 1 | 0  for On or Off
 *  Output params None
 * Returns 0 for Success, Negative value for failure.
 */
int ioctl_extsw_port_shaper_config(struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c sf2_port_shaper_config()
    uint16 page, reg;
    unsigned char q_shaper;

    if (e->type == TYPE_SET) {
        /* confiure requested shaper parameters.
         * Notice: each q has its separate page.
         */
        q_shaper = e->queue >= 0;
        page = q_shaper? PAGE_Q0_EGRESS_SHAPER + e->queue:
                                    PAGE_PORT_EGRESS_SHAPER;
        if (e->sub_type & SHAPER_ENABLE) {
            reg = SF2_REG_SHAPER_ENB;
            _extsw_reg16_bit_ops(page, reg, e->port, e->val);
        }
        if (e->sub_type & SHAPER_RATE_PACKET_MODE) {
            reg = SF2_REG_SHAPER_ENB_PKT_BASED;
            _extsw_reg16_bit_ops(page, reg, e->port, e->val);
        }
        if (e->sub_type & SHAPER_BLOCKING_MODE) {
            reg = SF2_REG_SHAPER_BLK_CTRL_ENB;
            _extsw_reg16_bit_ops(page, reg, e->port, e->val);
        }
        if (e->sub_type & SHAPER_INCLUDE_IFG) {
            // applies only for port shaper
            if (!q_shaper) {
                reg = SF2_REG_SHAPER_INC_IFG_CTRL;
                _extsw_reg16_bit_ops(page, reg, e->port, e->val);
            }
        }
    }

    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWDOSCTRL functions ---
int ioctl_extsw_dos_ctrl(struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c enet_ioctl_ethsw_dos_ctrl()
    if (e->type == TYPE_GET)
    {
        uint32_t v32 = 0;
        uint8_t v8 = 0;

        SF2SW_RREG(PAGE_DOS_PREVENT_531xx, REG_DOS_CTRL, (uint8 *)&v32, 4);
        /* Taking short-cut : Not following BCM coding guidelines */
        if (v32 & IP_LAN_DROP_EN)  e->dosCtrl.ip_lan_drop_en = 1;
        if (v32 & TCP_BLAT_DROP_EN)  e->dosCtrl.tcp_blat_drop_en = 1;
        if (v32 & UDP_BLAT_DROP_EN)  e->dosCtrl.udp_blat_drop_en = 1;
        if (v32 & TCP_NULL_SCAN_DROP_EN)  e->dosCtrl.tcp_null_scan_drop_en = 1;
        if (v32 & TCP_XMAS_SCAN_DROP_EN)  e->dosCtrl.tcp_xmas_scan_drop_en = 1;
        if (v32 & TCP_SYNFIN_SCAN_DROP_EN)  e->dosCtrl.tcp_synfin_scan_drop_en = 1;
        if (v32 & TCP_SYNERR_SCAN_DROP_EN)  e->dosCtrl.tcp_synerr_drop_en = 1;
        if (v32 & TCP_SHORTHDR_SCAN_DROP_EN)  e->dosCtrl.tcp_shorthdr_drop_en = 1;
        if (v32 & TCP_FRAGERR_SCAN_DROP_EN)  e->dosCtrl.tcp_fragerr_drop_en = 1;
        if (v32 & ICMPv4_FRAG_DROP_EN)  e->dosCtrl.icmpv4_frag_drop_en = 1;
        if (v32 & ICMPv6_FRAG_DROP_EN)  e->dosCtrl.icmpv6_frag_drop_en = 1;
        if (v32 & ICMPv4_LONGPING_DROP_EN)  e->dosCtrl.icmpv4_longping_drop_en = 1;
        if (v32 & ICMPv6_LONGPING_DROP_EN)  e->dosCtrl.icmpv6_longping_drop_en = 1;

        SF2SW_RREG(PAGE_DOS_PREVENT_531xx, REG_DOS_DISABLE_LRN, (uint8 *)&v8, 1);
        if (v8 & DOS_DISABLE_LRN) e->dosCtrl.dos_disable_lrn = 1;
    }
    else if (e->type == TYPE_SET)
    {
        uint32_t v32 = 0;
        uint8_t v8 = 0;
        /* Taking short-cut : Not following BCM coding guidelines */
        if (e->dosCtrl.ip_lan_drop_en) v32 |= IP_LAN_DROP_EN;
        if (e->dosCtrl.tcp_blat_drop_en) v32 |= TCP_BLAT_DROP_EN;
        if (e->dosCtrl.udp_blat_drop_en) v32 |= UDP_BLAT_DROP_EN;
        if (e->dosCtrl.tcp_null_scan_drop_en) v32 |= TCP_NULL_SCAN_DROP_EN;
        if (e->dosCtrl.tcp_xmas_scan_drop_en) v32 |= TCP_XMAS_SCAN_DROP_EN;
        if (e->dosCtrl.tcp_synfin_scan_drop_en) v32 |= TCP_SYNFIN_SCAN_DROP_EN;
        if (e->dosCtrl.tcp_synerr_drop_en) v32 |= TCP_SYNERR_SCAN_DROP_EN;
        if (e->dosCtrl.tcp_shorthdr_drop_en) v32 |= TCP_SHORTHDR_SCAN_DROP_EN;
        if (e->dosCtrl.tcp_fragerr_drop_en) v32 |= TCP_FRAGERR_SCAN_DROP_EN;
        if (e->dosCtrl.icmpv4_frag_drop_en) v32 |= ICMPv4_FRAG_DROP_EN;
        if (e->dosCtrl.icmpv6_frag_drop_en) v32 |= ICMPv6_FRAG_DROP_EN;
        if (e->dosCtrl.icmpv4_longping_drop_en) v32 |= ICMPv4_LONGPING_DROP_EN;
        if (e->dosCtrl.icmpv6_longping_drop_en) v32 |= ICMPv6_LONGPING_DROP_EN;

        /* Enable DOS attack blocking functions) */
        SF2SW_WREG(PAGE_DOS_PREVENT_531xx, REG_DOS_CTRL, (uint8 *)&v32, 4);
        if (e->dosCtrl.dos_disable_lrn)
        { /* Enable */
            v8 = DOS_DISABLE_LRN;
        }
        else
        {
            v8 = 0;
        }
        SF2SW_WREG(PAGE_DOS_PREVENT_531xx, REG_DOS_DISABLE_LRN, (uint8 *)&v8, 1);
    }
    return BCM_E_NONE;
}

// ----------- SIOCETHSWCTLOPS ETHSWHWSTP functions ---
// Set STP state into SF2 register
void _ethsw_set_stp_mode(unsigned int unit, unsigned int port, unsigned char stpState)
{
    // based on impl5\bcmsw_runner.c bcmeapi_ethsw_set_stp_mode()
   unsigned char portInfo;

   if(unit==1) // SF2
   {
      SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL + (port),
                 &portInfo, sizeof(portInfo));
      portInfo &= ~REG_PORT_STP_MASK;
      portInfo |= stpState;
      SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL + (port),
                 &portInfo, sizeof(portInfo));
   }
}

// ----------- SIOCETHSWCTLOPS ETHSWPORTSTORMCTRL functions ---
int ioctl_extsw_port_storm_ctrl(struct ethswctl_data *e)
{
    // based on impl5\bcmsw.c bcmeapi_ioctl_extsw_port_storm_ctrl()
    uint32_t val32;

    down(&sf2_sw->s.conf_sem);
    if (e->type == TYPE_SET) {
        /* configure storm control rate & burst size */
        SF2SW_RREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
        val32 |= REG_PN_BUCK0_ENABLE_MASK | REG_PN_BUCK0_MODE_MASK; // use bucket 0
        val32 |= (e->limit & REG_PN_BUCK0_REF_CNT_M) << REG_PN_BUCK0_REF_CNT_S;
        val32 |= (e->burst_size & REG_PN_BUCK0_SIZE_M) << REG_PN_BUCK0_SIZE_S;
        SF2SW_WREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);

        /* pkt type */
        SF2SW_RREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_CFG_PORT_0 + e->port * 2, &val32, 4);
	    val32 &= ~(REG_PN_BUCK0_PKT_SEL_M << REG_PN_BUCK0_PKT_SEL_S);
	    val32 |= (e->pkt_type_mask & REG_PN_BUCK0_PKT_SEL_M) << REG_PN_BUCK0_PKT_SEL_S;
        SF2SW_WREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_CFG_PORT_0 + e->port * 2, &val32, 4);
    } else {
        SF2SW_RREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
        e->limit = (val32 >> REG_PN_BUCK0_REF_CNT_S) & REG_PN_BUCK0_REF_CNT_M;
        e->burst_size = (val32 >> REG_PN_BUCK0_SIZE_S) & REG_PN_BUCK0_SIZE_M;

        SF2SW_RREG(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_CFG_PORT_0 + e->port * 2, &val32, 4);
        e->pkt_type_mask = (val32 >> REG_PN_BUCK0_PKT_SEL_S) & REG_PN_BUCK0_PKT_SEL_M;
    }

    up(&sf2_sw->s.conf_sem);
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWMULTIPORT functions ---
int ioctl_extsw_set_multiport_address(uint8_t *addr)
{
    // based on impl5\bcmsw.c bcmsw_set_multiport_address_ext()
#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
    int i;
    uint32 v32;
    uint16 v16;
    uint8 v64[8];
    uint8 cur64[8];

    *(uint16*)(&v64[0]) = 0;
    memcpy(&v64[2], addr, 6);
    /* check if address is set already */
    for ( i = 0; i < MULTIPORT_CTRL_COUNT; i++ )
    {
       SF2SW_RREG(PAGE_ARLCTRL, (REG_MULTIPORT_ADDR1_LO + (i * 0x10)), (uint8 *)&cur64, sizeof(cur64)|DATA_TYPE_VID_MAC);
       if ( 0 == memcmp(&v64[0], &cur64[0], 8) )
       {
           return 0;
       }
    }

    /* add new entry */
    for ( i = 0; i < MULTIPORT_CTRL_COUNT; i++ )
    {
        SF2SW_RREG(PAGE_ARLCTRL, REG_MULTIPORT_CTRL, (uint8 *)&v16, 2);
        if ( 0 == (v16 & (MULTIPORT_CTRL_EN_M << (i << 1))))
        {
            v16 |= (1<<MULTIPORT_CTRL_DA_HIT_EN) | (MULTIPORT_CTRL_ADDR_CMP << (i << 1));
            SF2SW_WREG(PAGE_ARLCTRL, REG_MULTIPORT_CTRL, (uint8 *)&v16, 2);
            *(uint16*)(&v64[0]) = 0;
            memcpy(&v64[2], addr, 6);
            SF2SW_WREG(PAGE_ARLCTRL, (REG_MULTIPORT_ADDR1_LO + (i * 0x10)), (uint8 *)&v64, sizeof(v64)|DATA_TYPE_VID_MAC);
            v32 = imp_pbmap[1];
            SF2SW_WREG(PAGE_ARLCTRL, (REG_MULTIPORT_VECTOR1 + (i * 0x10)), (uint8 *)&v32, sizeof(v32));

            /* Set multiport VLAN control based on U/V_FWD_MAP;
               This is required so that VLAN tagged frames matching Multiport Address are forwarded according to V/U forwarding map */
            SF2SW_RREG(PAGE_8021Q_VLAN, REG_VLAN_MULTI_PORT_ADDR_CTL, &v16, sizeof(v16));
            v16 |=  (EN_MPORT_V_FWD_MAP | EN_MPORT_U_FWD_MAP) << (i*EN_MPORT_V_U_FWD_MAP_S) ;
            SF2SW_WREG(PAGE_8021Q_VLAN, REG_VLAN_MULTI_PORT_ADDR_CTL, &v16, sizeof(v16));

            return 0;
        }
    }
#endif /* defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT) */
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWDEEPGREENMODE functions ---
#if defined(CONFIG_BCM_ETH_DEEP_GREEN_MODE)


static void _ethsw_deep_green_mode_activate(void)
{
    // based on impl5/eth_pwrmngt.c:ethsw_deep_green_mode_activate()
    uint32 reg_val32;

    printk("===> Activate Deep Green Mode\n");

    /* Disable IMP port */
    SF2SW_RREG(PAGE_CONTROL, REG_MII_PORT_CONTROL, &reg_val32, 4);
    reg_val32 |= REG_MII_PORT_CONTROL_RX_DISABLE | REG_MII_PORT_CONTROL_TX_DISABLE;
    SF2SW_WREG(PAGE_CONTROL, REG_MII_PORT_CONTROL, &reg_val32, 4);
#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
#if defined(CONFIG_BCM94908)
    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+5, &reg_val32, 4);
    reg_val32 |= REG_PORT_CTRL_DISABLE;                          /* Disable P5 IMP */
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+5, &reg_val32, 4);

    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+4, &reg_val32, 4);
    reg_val32 |= REG_PORT_CTRL_DISABLE;                          /* Disable P4 IMP */
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+4, &reg_val32, 4);
#elif defined(CONFIG_BCM963158)
    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+5, &reg_val32, 4);
    reg_val32 |= REG_PORT_CTRL_DISABLE;                          /* Disable P5 IMP */
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+5, &reg_val32, 4);

    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+7, &reg_val32, 4);
    reg_val32 |= REG_PORT_CTRL_DISABLE;                          /* Disable P7 IMP */
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+7, &reg_val32, 4);
#endif
#endif //CONFIG_BCM_ENET_MULTI_IMP_SUPPORT

    /* Disable all ports' MAC TX/RX clocks (IMPORTANT: prevent all traffic into Switch while its clock is lowered) */
    SF2SW_RREG(PAGE_CONTROL, REG_LOW_POWER_EXP1, &reg_val32, 4);
    reg_low_power_exp1 = reg_val32;    //Store register's value so that we can restore this value when we disable Deep Green Mode
    reg_val32 |= REG_LOW_POWER_EXP1_SLEEP_MACCLK_PORT_MASK;
    SF2SW_WREG(PAGE_CONTROL, REG_LOW_POWER_EXP1, &reg_val32, 4);
#if !defined(CONFIG_BCM963158)
    /* Slow down system clock, stop port5 and port 8 mac clock */
    SF2SW_RREG(PAGE_CONTROL, REG_LOW_POWER_CTRL, &reg_val32, 4);
    reg_val32 |= REG_LOW_POWER_CTR_SLEEP_P8     | REG_LOW_POWER_CTR_SLEEP_P5                 |
                 REG_LOW_POWER_CTR_SLEEP_P4     | REG_LOW_POWER_CTR_TIMER_DISABLE            |
                 REG_LOW_POWER_CTR_EN_LOW_POWER | REG_LOW_POWER_CTR_LOW_POWER_DIVIDER_6P25MHZ;
    SF2SW_WREG(PAGE_CONTROL, REG_LOW_POWER_CTRL, &reg_val32, 4);

    /* Shut Down Channel 2 PLL */
    reg_val32 = 0x1f;
    SF2SW_WREG(PAGE_CONTROL, 0xdc, &reg_val32, 4);
#else
    /* Slow down system clock, stop port5 and port 8 mac clock */
    SF2SW_RREG(PAGE_CONTROL, REG_LOW_POWER_CTRL, &reg_val32, 4);
    reg_val32 |= REG_LOW_POWER_CTR_TIMER_DISABLE;
    SF2SW_WREG(PAGE_CONTROL, REG_LOW_POWER_CTRL, &reg_val32, 4);

    /* change switch clock back to lowpower mode */
    pmc_switch_clock_lowpower_mode (1);
#endif

}

static void _ethsw_deep_green_mode_deactivate(void)
{
    // based on impl5/eth_pwrmngt.c:ethsw_deep_green_mode_deactivate()
    uint32 reg_val32;

    printk("<=== Deactivate Deep Green Mode\n");

#if !defined(CONFIG_BCM963158)
    /* From PLL low power mode to Normal mode */
    reg_val32 = 0xc0;
    SF2SW_WREG(PAGE_CONTROL, 0xdc, &reg_val32, 4);
    reg_val32 = 0x0;
    SF2SW_WREG(PAGE_CONTROL, 0xdc, &reg_val32, 4);

    /* Bring back system clock and mac clocks */
    SF2SW_RREG(PAGE_CONTROL, REG_LOW_POWER_CTRL, &reg_val32, 4);
    reg_val32 &= ~(REG_LOW_POWER_CTR_SLEEP_P8     | REG_LOW_POWER_CTR_SLEEP_P5                 |
                   REG_LOW_POWER_CTR_SLEEP_P4     | REG_LOW_POWER_CTR_TIMER_DISABLE            |
                   REG_LOW_POWER_CTR_EN_LOW_POWER | REG_LOW_POWER_CTR_LOW_POWER_DIVIDER_6P25MHZ);
    SF2SW_WREG(PAGE_CONTROL, REG_LOW_POWER_CTRL, &reg_val32, 4);
#else
    /* change switch clock back to operation mode */
    pmc_switch_clock_lowpower_mode (0);
    /* Bring back system clock and mac clocks */
    SF2SW_RREG(PAGE_CONTROL, REG_LOW_POWER_CTRL, &reg_val32, 4);
    reg_val32 &= ~(REG_LOW_POWER_CTR_TIMER_DISABLE);
    SF2SW_WREG(PAGE_CONTROL, REG_LOW_POWER_CTRL, &reg_val32, 4);
#endif

    /* Enable IMP port */
    SF2SW_RREG(PAGE_CONTROL, REG_MII_PORT_CONTROL, &reg_val32, 4);
    reg_val32 &= ~(REG_MII_PORT_CONTROL_RX_DISABLE | REG_MII_PORT_CONTROL_TX_DISABLE);
    SF2SW_WREG(PAGE_CONTROL, REG_MII_PORT_CONTROL, &reg_val32, 4);
#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
#if defined(CONFIG_BCM94908)
    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+5, &reg_val32, 4);
    reg_val32 &= ~REG_PORT_CTRL_DISABLE;                        /* Enable P5 IMP */
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+5, &reg_val32, 4);

    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+4, &reg_val32, 4);
    reg_val32 &= ~REG_PORT_CTRL_DISABLE;                        /* Enable P4 IMP */
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+4, &reg_val32, 4);
#elif defined(CONFIG_BCM963158)
    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+5, &reg_val32, 4);
    reg_val32 &= ~REG_PORT_CTRL_DISABLE;                        /* Enable P5 IMP */
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+5, &reg_val32, 4);

    SF2SW_RREG(PAGE_CONTROL, REG_PORT_CTRL+7, &reg_val32, 4);
    reg_val32 &= ~REG_PORT_CTRL_DISABLE;                        /* Enable P7 IMP */
    SF2SW_WREG(PAGE_CONTROL, REG_PORT_CTRL+7, &reg_val32, 4);
#endif
#endif //CONFIG_BCM_ENET_MULTI_IMP_SUPPORT

    /* Set IMP port to link up */
    SF2SW_RREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(IMP_PORT_ID), &reg_val32, 4);
    reg_val32 |= REG_CONTROL_MPSO_LINKPASS;
    SF2SW_WREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(IMP_PORT_ID), &reg_val32, 4);

    /* Re-enable in-use ports' MAC TX/RX clocks (leave unused port's MAC TX/RX clock disabled) */
    reg_val32 = reg_low_power_exp1;    //Restore register's previous value from before we enabled Deep Green Mode
    SF2SW_WREG(PAGE_CONTROL, REG_LOW_POWER_EXP1, &reg_val32, 4);
}

static int tr_phy_link_up(enetx_port_t *port, void *_ctx)
{
    int *any_link_up = (int *)_ctx;
    phy_dev_t *phy = get_active_phy(port->p.phy);

    if (phy && phy->link)
    {
        *any_link_up = 1;    // found one port with link up
        return 1;           // stop scanning
    }
    return 0;
}

void port_sf2_deep_green_mode_handler(void)
{
    // based on impl5/eth_pwrmngt.c:ethsw_deep_green_mode_handler()
    int any_link_up = 0;
    
    // check if any port is linked up?
    port_traverse_ports(root_sw, tr_phy_link_up, PORT_CLASS_PORT, &any_link_up);
    
    /* (special case) If user uses WebGUI to disable Deep Green Mode feature then deactivate Deep Green Mode if necessary and exit function */
    if ( !deep_green_mode_enabled )  {
        //printk("Deep Green Mode feature is disabled in WebGUI.  Do nothing.\n");
        if (deep_green_mode_activated) {
            deep_green_mode_activated = 0;
            //printk("Deep Green Mode was activated.  Deactivating Deep Green Mode now...\n");
            _ethsw_deep_green_mode_deactivate();
        }
        return;
    }

    /* Only activate Deep Green Mode if all ports are linked down and Deep Green Mode wasn't already enabled */
    if ( (!any_link_up) && (!deep_green_mode_activated) ) {
        deep_green_mode_activated = 1;
        _ethsw_deep_green_mode_activate();
    /* Only deactivate Deep Green Mode if some ports are linked up and Deep Green Mode is currently enabled */
    } else if ( any_link_up && (deep_green_mode_activated) )  {
        deep_green_mode_activated = 0;
        _ethsw_deep_green_mode_deactivate();
    }
}

int ioctl_pwrmngt_get_deepgreenmode(int mode)
{
    // based on impl5/eth_pwrmngt.c:BcmPwrMngtGetDeepGreenMode()
    if (!mode) {
        return (deep_green_mode_enabled);
    } else {
        return (deep_green_mode_activated);
    }
}

int ioctl_pwrmngt_set_deepgreenmode(int enable)
{
    // based on impl5/eth_pwrmngt.c:BcmPwrMngtSetDeepGreenMode()
    if (deep_green_mode_enabled != enable) {
        deep_green_mode_enabled = enable;
        port_sf2_deep_green_mode_handler();
        printk("Deep Green Mode feature changed to %s (DGM status: %s)\n", enable?"enabled":"disabled", deep_green_mode_activated?"activated":"deactivated");
    }
    return 0;
}

#endif /* defined(CONFIG_BCM_ETH_DEEP_GREEN_MODE) */

// =========== sf2 public functions =======================

void extsw_set_mac_address(uint8_t *addr)
{
    // based on imp5\bcmenet.c bcm63xx_enet_probe() code fragment
#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
    /* Static MAC works for all scenarios, so just add multiport
     * MAC only when multiple IMP ports are in use. */
    ioctl_extsw_set_multiport_address(addr);
#else
    _enet_arl_write_ext(addr, 0, ARL_DATA_ENTRY_VALID|ARL_DATA_ENTRY_STATIC|IMP_PORT_ID);
#endif
}

#if defined(CRB_5X3_QGPHY3_WORKAROUND) /* 5x3 crossbar */
/* these functions are for qgphy3 workaround */
int sf2_set_mac_port_state(int phy_port, int link, int speed, int duplex)
{
    // based on imp5\bcmsw.c:bcmsw_set_mac_port_state()
    uint8 v8;

    v8 = REG_PORT_STATE_OVERRIDE;
    v8 |= (link != 0)? REG_PORT_STATE_LNK: 0;
    v8 |= (duplex != 0)? REG_PORT_STATE_FDX: 0;

    if (speed == 1000)
        v8 |= REG_PORT_STATE_1000;
    else if (speed == 100)
        v8 |= REG_PORT_STATE_100;

    down(&sf2_sw->s.conf_sem);
    SF2SW_WREG(PORT_OVERIDE_PAGE, PORT_OVERIDE_REG(phy_port), &v8, 1);
    up(&sf2_sw->s.conf_sem);
        
    return 0;
}

void sf2_force_mac_up(int port)
{
    // based on imp5\eth_pwrmngt.c:ethsw_force_mac_up()
    uint32 reg_val32 = 0;
    
    SF2SW_RREG(PAGE_CONTROL, REG_LOW_POWER_EXP1, &reg_val32, 4);
    reg_val32 &= ~((1<<port) << REG_LOW_POWER_EXP1_SLEEP_MACCLK_PORT_SHIFT);
    SF2SW_WREG(PAGE_CONTROL, REG_LOW_POWER_EXP1, &reg_val32, 4);
#if defined(CONFIG_BCM_ETH_DEEP_GREEN_MODE)
    reg_low_power_exp1 &= ~((1<<port) << REG_LOW_POWER_EXP1_SLEEP_MACCLK_PORT_SHIFT);
#endif
}

#endif /* defined(CRB_5X3_QGPHY3_WORKAROUND) */

static char *print_phy_attribute(int phy_id)
{
    if (IsSerdes(phy_id)) return "Serdes PHY";
    if (IsExtPhyId(phy_id))
    {
        if (IsRGMII(phy_id)) return "External RGMII PHY";
        return "External PHY";
    }
    return "Internal PHY";
}

static void sw_print_mac_phy_info(enetx_port_t *sw)
{
    int i, j;
    enetx_port_t *p;
    phy_dev_t *phy;
    struct net_device *dev;

    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < sw->s.port_count; i++)
        {
            if(!(p = sw->s.ports[i]))
                continue;

            if (p->port_class != PORT_CLASS_PORT)
                continue;

            if (j == 0)    /* Parent round */
            {
                if (p->p.child_sw) continue;

                if (!(dev = p->dev))
                    continue;
                if (phy_is_crossbar(p->p.phy))
                {
                    printk("%s: <%s sw port: %d> <Logical : %02d> MAC : %02X:%02X:%02X:%02X:%02X:%02X\n",
                            dev->name, (IS_ROOT_SW(sw)?"Int":"Ext"), p->p.mac->mac_id, PHYSICAL_PORT_TO_LOGICAL_PORT(p->p.mac->mac_id, IS_ROOT_SW(sw)?0:1),
                            dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2], dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);

                    for (phy = crossbar_phy_dev_first(p->p.phy); phy; phy = crossbar_phy_dev_next(phy))
                    {
                        int ext_idx = crossbar_external_endpoint(phy);
                        printk("    Chip Physical Port %2d, Cross Bar Port %d, PHY_ID <0x%08x:0x%02x:%s >\n",
                                BP_CROSSBAR_PORT_TO_PHY_PORT(ext_idx),  ext_idx,
                                phy->meta_id, phy->addr, print_phy_attribute(phy->meta_id));

                        if (phy->cascade_next && !(phy->cascade_next->flag & PHY_FLAG_NOT_PRESENTED))
                        {
                            printk("                                             PHY_ID <0x%08x:0x%02x:External Cascaded%s PHY>\n",
                                    phy->cascade_next->meta_id, phy->cascade_next->addr, IsC45Phy(phy->cascade_next)?" Claus45": "");
                        }
                    }
                }
                else
                {
                    if (!(phy = p->p.phy)) continue;
                    if (phy->phy_drv && phy->phy_drv->phy_type == PHY_TYPE_XGAE)
                        printk("%s: <%s sw port: %d> <Logical : %02d> PHY_ID <Not Available:10G Active PHY> MAC:%02X:%02X:%02X:%02X:%02X:%02X\n",
                                dev->name, (IS_ROOT_SW(sw)?"Int":"Ext"), p->p.mac->mac_id, PHYSICAL_PORT_TO_LOGICAL_PORT(p->p.mac->mac_id, IS_ROOT_SW(sw)?0:1),
                                dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2], dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);
                    else
                        printk("%s: <%s sw port: %d> <Logical : %02d> PHY_ID <0x%08x:0x%02x:%s> MAC:%02X:%02X:%02X:%02X:%02X:%02X\n",
                                dev->name, (IS_ROOT_SW(sw)?"Int":"Ext"), p->p.mac->mac_id, PHYSICAL_PORT_TO_LOGICAL_PORT(p->p.mac->mac_id, IS_ROOT_SW(sw)?0:1),
                                phy->meta_id, phy->addr, print_phy_attribute(phy->meta_id),
                                dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2], dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);

                    if (phy->cascade_next && !(phy->cascade_next->flag & PHY_FLAG_NOT_PRESENTED))
                    {
                        printk("                                      PHY_ID <0x%08x:0x%02x:External Cascaded%s PHY>\n",
                                phy->cascade_next->meta_id, phy->cascade_next->addr, IsC45Phy(phy->cascade_next)?" Claus45": "");
                    }
                }
            }
            else    /* Child switch round */
            {
                if (!p->p.child_sw) continue;

                sw_print_mac_phy_info(p->p.child_sw);
                continue;
            }
        }
    }
}

int enetxapi_post_config(void)
{
    serdes_work_around(NULL); 
    sw_print_mac_phy_info(root_sw);

    sf2_rgmii_config();

    return 0;
}

// =========== sf2 switch ops =============================
extern void add_unspecified_ports(enetx_port_t *sw, uint32_t port_map, uint32_t imp_map);

static int port_sf2_sw_init(enetx_port_t *self)
{
    uint8_t v8;

    sema_init(&bcm_link_handler_config, 1);
    
    sf2_sw = self;    /* init sf2_sw shortcut */
    /* this function creates all IMP port objects, and unused port objects,
       they need to be explicitly created, since board param file does not spcify them.
       These port objects are used by ioctl to access hw info.
    */
    add_unspecified_ports(self, chip_arch_all_portmap[1], chip_arch_mgmt_portmap[1]);

    _sf2_enable_p8_rdp_sel();
    _sf2_set_imp_speed();

    _extsw_setup_imp_ports();
    _extsw_setup_imp_fwding();

    // configure trunk groups if required.
    _extsw_port_trunk_init();

    // set ARL AGE_DYNAMIC bit for aging operations
    v8 = FAST_AGE_DYNAMIC;
    SF2SW_WREG(PAGE_CONTROL, REG_FAST_AGING_CTRL, &v8, 1);

    _sf2_qos_default();
    if (queThreConfMode != ThreModeManual)
    {
        _sf2_conf_que_thred(); // for acb testing
    }
    _sf2_conf_acb_conges_profile(0);

#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
    enet_spdsvc_transmit = bcmFun_get(BCM_FUN_ID_SPDSVC_TRANSMIT);
    BCM_ASSERT(enet_spdsvc_transmit != NULL);
#endif

    return port_runner_sw_init(self);
}

static int port_sf2_sw_uninit(enetx_port_t *self)
{
    return port_runner_sw_uninit(self);
}

/* only root switch does demux */
int port_sf2_sw_demux(enetx_port_t *sw, enetx_rx_info_t *rx_info, FkBuff_t *fkb, enetx_port_t **out_port)
{

    if (!rdpa_if_is_wan(rx_info->src_port))
    {
        int ret = port_generic_sw_demux(sf2_sw, rx_info, fkb, out_port);
        
#if defined(CONFIG_BCM_KERNEL_BONDING)
        /* Packet received from LAN/SF2-port configured as WAN; No BRCM TAG */
        if (!(*out_port && (*out_port)->p.bond_grp && (*out_port)->p.bond_grp->is_lan_wan_cfg))
#endif
            ((BcmEnet_hdr2*)(fkb->data))->brcm_type = htons(BRCM_TYPE2);

        return ret;
    }
    return port_generic_sw_demux(sw, rx_info, fkb, out_port);
}

/*
static int port_sf2_sw_mux(enetx_port_t *tx_port, pNBuff_t pNBuff, enetx_port_t **out_port)
{
    return 0;
}
*/

/* map SF2 external switch phyical port ID to rdpa_if */
static int port_sf2_sw_port_id_on_sw(port_info_t *port_info, int *port_id, port_type_t *port_type)
{
    *port_type = PORT_TYPE_SF2_PORT;
    *port_id = rdpa_if_lan0 + port_info->port;

    if (port_info->is_undef)
        *port_id = rdpa_if_none;
#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148)
    if (*port_id == rdpa_if_lan7)
        *port_id = rdpa_if_lan6;
#elif defined(CONFIG_BCM94908)
    if (*port_id == rdpa_if_lan7)
        *port_id = rdpa_if_lan4;
#elif defined(CONFIG_BCM963158)
    if (*port_id == rdpa_if_lan6)
        *port_id = rdpa_if_lan5;
#endif
    return 0;
}

static int port_sf2_sw_config_trunk(enetx_port_t *sw, enetx_port_t *port, int grp_no, int add)
{
    /* based on impl5\bcmsw.c:bcmsw_config_trunk() */
    uint16_t v16;

    down(&sf2_sw->s.conf_sem);
    SF2SW_RREG(PAGE_MAC_BASED_TRUNK, REG_TRUNK_GRP_CTL + (2*grp_no), &v16, 2);
    if (add)
    {
        v16 |= ( ( (1<< port->p.mac->mac_id) & TRUNK_EN_GRP_M ) << TRUNK_EN_GRP_S );
        enet_dbg("ADD : port %s to group <%d>; New pmap <0x%02x>\n", port->obj_name, grp_no, v16);
    }
    else
    {
        v16 &= ~( ( (1<< port->p.mac->mac_id) & TRUNK_EN_GRP_M ) << TRUNK_EN_GRP_S );
        enet_dbg("REM : port %s to group <%d>; New pmap <0x%02x>\n", port->obj_name, grp_no, v16);
    }
    SF2SW_WREG(PAGE_MAC_BASED_TRUNK, REG_TRUNK_GRP_CTL + (2*grp_no), &v16, 2);
    up(&sf2_sw->s.conf_sem);
    
    return port_runner_sw_config_trunk(sw, port, grp_no, add);
}

static int port_sf2_sw_update_pbvlan(enetx_port_t *sw, unsigned int pmap)
{
    int i;

    if (pmap == 0) return 0;    //nothing to do

    for (i=0; i < BP_MAX_SWITCH_PORTS; i++ )
    {
        if (pmap & (1<<i))
        {
#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
            _extsw_set_pbvlan(i, pmap | (1 << cur_port_imp_map[i]));
#else
            _extsw_set_pbvlan(i, pmap | (1 << IMP_PORT_ID));
#endif
        }
    }
    return 0;
}

// ----------- SIOCETHSWCTLOPS ETHSWSWITCHING functions ---
static int port_sf2_sw_hw_sw_state_set(enetx_port_t *sw, unsigned long state)
{
    /* based on impl5\ethsw.c:ethsw_set_hw_switching() */

    down(&sf2_sw->s.conf_sem);
    /*Don't do anything if already enabled/disabled.
     *Enable is implemented by restoring values saved by disable_hw_switching().
     *This check is necessary to make sure we get correct behavior when
     *enable_hw_switching() is called without a preceeding disable_hw_switching() call.
     */
    if (sf2_hw_switching_state != state) {
        int ndx;
        enetx_port_t **ports = sw->s.ports;

        if (state == HW_SWITCHING_ENABLED) {
            /* restore disable learning register */
            SF2SW_WREG(PAGE_CONTROL, REG_DISABLE_LEARNING, &sf2_dis_learning_ext, 2);

            /* when hardware switching is enabed, enable the Linux bridge to
              not to forward the bcast packets on hardware ports */
            for (ndx = 0; ndx < sw->s.port_count; ndx++)
                if (ports[ndx]->dev) ports[ndx]->dev->priv_flags |= IFF_HW_SWITCH;
        }
        else {
            uint16_t val = PBMAP_ALL;
            /* save setting */
            SF2SW_RREG(PAGE_CONTROL, REG_DISABLE_LEARNING, &sf2_dis_learning_ext, 2);
            /* disable learning on all ports */
            SF2SW_WREG(PAGE_CONTROL, REG_DISABLE_LEARNING, &val, 2);

            /* when hardware switching is disabled, enable the Linux bridge to
              forward the bcast packets on hardware ports as well */
            for (ndx = 0; ndx < sw->s.port_count; ndx++)
                if (ports[ndx]->dev) ports[ndx]->dev->priv_flags &= ~IFF_HW_SWITCH;

            /* flush arl table dynamic entries */
            _fast_age_all_ext(0);
        }

        sf2_hw_switching_state = state;
    }
    up(&sf2_sw->s.conf_sem);
    return 0;
}

static int port_sf2_sw_hw_sw_state_get(enetx_port_t *sw)
{
#if defined(enet_dbgv_enabled)
    //enetx_port_t **ports = sw->s.ports;
    uint16_t val;

    SF2SW_RREG(PAGE_CONTROL, REG_DISABLE_LEARNING, &val, 2);
    //enet_dbgv("rreg(pg=%x, reg=%x, len=2) val=%x\n", PAGE_CONTROL, REG_DISABLE_LEARNING, val);
#endif
    return sf2_hw_switching_state;
}


// =========== sf2 port ops =============================
extern bdmf_object_handle create_rdpa_port(rdpa_if rdpaif, rdpa_emac emac, bdmf_object_handle owner);
extern int link_switch_to_rdpa_port(bdmf_object_handle port_obj);

int port_sf2_port_init(enetx_port_t *self)
{
    bdmf_error_t rc;
    rdpa_if rdpaif = self->p.port_id;
    rdpa_emac emac = rdpa_emac_none;
    bdmf_object_handle switch_port_obj = NULL;
    static IOCTL_MIB_INFO mib[BP_MAX_ENET_MACS * BP_MAX_SWITCH_PORTS];
    static int mibAlloc;

    if (mibAlloc >= sizeof(mib)/sizeof(mib[0]))
        return -1;

    if (demux_on_sw(self->p.parent_sw, rdpaif, self))
        return -1;

    /* also register demux at root for receive processing if port not on root sw */
    if (!PORT_ON_ROOT_SW(self))
        if (demux_on_sw(root_sw, rdpaif, self))
            return -1;

    self->priv2 = &mib[mibAlloc++];

#ifdef CONFIG_BLOG
    self->n.blog_phy = BLOG_ENETPHY;
    self->n.blog_chnl = self->n.blog_chnl_rx = PHYSICAL_PORT_TO_LOGICAL_PORT(self->p.mac->mac_id, PORT_ON_ROOT_SW(self)?0:1);
    enet_dbgv("%s blog_chnl=%x\n", self->obj_name, self->n.blog_chnl);
#endif

    /* Override bp_parser settings, since once a rdpa port object is created, port role cannot change */
    self->p.port_cap = rdpa_if_is_wan(rdpaif) ? PORT_CAP_WAN_ONLY : PORT_CAP_LAN_ONLY;
    self->n.port_netdev_role = rdpa_if_is_wan(rdpaif) ? PORT_NETDEV_ROLE_WAN : PORT_NETDEV_ROLE_LAN;

    /* create_rdpa_port only for port on external switch */
    if (PORT_ON_ROOT_SW(self))
    {
        if (rdpa_if_is_wan(rdpaif))
            emac = rdpa_emac0 + self->p.mac->mac_id;
        else
            goto PORT_INIT_CONT;
    }
    else
    {
        /* get the rdpa switch port in order to configure as owner to extswitch lan ports */
        rc = rdpa_port_get(rdpa_if_switch, &switch_port_obj);
        if (rc)
        {
            enet_err("Failed to get rdpa switch port. rc=%d\n", rc);
            return -1;
        }
    }

    if (!(self->priv = create_rdpa_port(rdpaif, emac, switch_port_obj)))
    {
        enet_err("Failed to create RDPA port object for %s\n", self->obj_name);
        return -1;
    }

    if (rdpa_if_is_lag_and_switch(rdpaif) && (rc = link_switch_to_rdpa_port(self->priv)))
    {
        enet_err("Failed to link RDPA switch to port object %s. rc =%d\n", self->obj_name, rc);
        return rc;
    }

PORT_INIT_CONT:
    enet_dbg("Initialized %s role %s\n", self->obj_name, (self->n.port_netdev_role==PORT_NETDEV_ROLE_WAN)?"wan":"lan" );

    return 0;
}

static void port_sf2_port_open(enetx_port_t *self)
{
    // port is on external switch, also enable connected runner port
    port_open(sf2_sw->s.parent_port);

    port_generic_open(self);
}



// based on impl5\bcmenet.c:bcm63xx_fkb_put_tag()
static inline void enet_fkb_put_tag(FkBuff_t * fkb_p,
        struct net_device * dev, unsigned int port_map)
{
    int i;
    int tailroom, crc_len = 0;
    uint16 *from = (uint16*)fkb_p->data;
    BcmEnet_hdr2 *pHdr = (BcmEnet_hdr2 *)from;

    if (ntohs(pHdr->brcm_type) != BRCM_TYPE2)
    {
        uint16 * to = (uint16*)fkb_push(fkb_p, BRCM_TAG_TYPE2_LEN);
        pHdr = (BcmEnet_hdr2 *)to;
        for ( i=0; i<ETH_ALEN; *to++ = *from++, i++ ); /* memmove 2 * ETH_ALEN */
        /* set port of ingress brcm tag */
        pHdr->brcm_tag = htons(port_map);

    }
    /* set ingress brcm tag and TC bit */
    pHdr->brcm_type = htons(BRCM_TAG2_EGRESS | (SKBMARK_GET_Q_PRIO(fkb_p->mark) << 10));
    tailroom = ETH_ZLEN + BRCM_TAG_TYPE2_LEN - fkb_p->len;
    tailroom = (tailroom < 0) ? crc_len : crc_len + tailroom;
    fkb_pad(fkb_p, tailroom);
    fkb_p->dirty_p = _to_dptr_from_kptr_(fkb_p->data + fkb_p->len);
}

// based on impl5\bcmenet.c:bcm63xx_skb_put_tag()
static struct sk_buff *enet_skb_put_tag(struct sk_buff *skb,
        struct net_device *dev, unsigned int port_map)
{
    BcmEnet_hdr2 *pHdr = (BcmEnet_hdr2 *)skb->data;
    int i, headroom;
    int tailroom;

    if (ntohs(pHdr->brcm_type) == BRCM_TYPE2)
    {
        headroom = 0;
        tailroom = ETH_ZLEN + BRCM_TAG_TYPE2_LEN - skb->len;
    }
    else
    {
        headroom = BRCM_TAG_TYPE2_LEN;
        tailroom = ETH_ZLEN - skb->len;
    }

    if (tailroom < 0)
    {
        tailroom = 0;
    }

#if defined(CONFIG_BCM_USBNET_ACCELERATION)
    if ((skb_writable_headroom(skb) < headroom) || (skb_tailroom(skb) < tailroom))
#else
    if ((skb_headroom(skb) < headroom) || (skb_tailroom(skb) < tailroom))
#endif
    {
        struct sk_buff *oskb = skb;
        skb = skb_copy_expand(oskb, headroom, tailroom, GFP_ATOMIC);
        kfree_skb(oskb);
        if (!skb)
        {
            return NULL;
        }
    }
#if defined(CONFIG_BCM_USBNET_ACCELERATION)
    else if ((headroom != 0) && (skb->clone_wr_head == NULL))
#else
    else if ((headroom != 0) && !(skb_clone_writable(skb, headroom)))
#endif
    {
        skb = skb_unshare(skb, GFP_ATOMIC);
        if (!skb)
        {
            return NULL;
        }
    }

    if (tailroom > 0)
    {
        if (skb_is_nonlinear(skb))
        {
            /* Non linear skb whose skb->len is < minimum Ethernet Packet Length
               (ETHZLEN or ETH_ZLEN + BroadcomMgmtTag Length) */
            if (skb_linearize(skb))
            {
                return NULL;
            }
        }
        memset(skb->data + skb->len, 0, tailroom);  /* padding to 0 */
        skb_put(skb, tailroom);
    }

    if (headroom != 0)
    {
        uint16 *to, *from;
        BcmEnet_hdr2 *pHdr2 = (BcmEnet_hdr2 *)skb_push(skb, headroom);
        to = (uint16*)pHdr2;
        from = (uint16*)(skb->data + headroom);
        for ( i=0; i<ETH_ALEN; *to++ = *from++, i++ ); /* memmove 2 * ETH_ALEN */
        /* set ingress brcm tag and TC bit */
        pHdr2->brcm_type = htons(BRCM_TAG2_EGRESS | (SKBMARK_GET_Q_PRIO(skb->mark) << 10));
        pHdr2->brcm_tag  = htons(port_map);
    }
    return skb;
}

/* insert broadcom tag for external switch port */
static int port_sf2_tx_pkt_mod(enetx_port_t *port, pNBuff_t *pNBuff, uint8_t **data, uint32_t *len, unsigned int port_map)
{
    FkBuff_t *pFkb = 0;
    struct sk_buff *skb = 0;

    if (IS_FKBUFF_PTR(*pNBuff))
    {
        FkBuff_t * pFkbOrig = PNBUFF_2_FKBUFF(*pNBuff);

        pFkb = fkb_unshare(pFkbOrig);

        if (pFkb == FKB_NULL)
        {
            fkb_free(pFkbOrig);
            INC_STAT_TX_DROP(port,tx_dropped_no_fkb);
            return -1;
        }
        enet_fkb_put_tag(pFkb, port->dev, port_map); /* Portmap for external switch */
        *data = (void *)pFkb->data;
        *len  = pFkb->len;
        *pNBuff = PBUF_2_PNBUFF((void*)pFkb,FKBUFF_PTR);
    }
    else
    {
        skb = PNBUFF_2_SKBUFF(*pNBuff);
        skb = enet_skb_put_tag(skb, port->dev, port_map);    /* Portmap for external switch and also pads to 0 */
        if (skb == NULL) {
            INC_STAT_TX_DROP(port,tx_dropped_no_skb);
            return -1;
        }
        *data = (void *)skb->data;   /* Re-encode pNBuff for adjusted data and len */
        *len  = skb->len;
        *pNBuff = PBUF_2_PNBUFF((void*)skb,SKBUFF_PTR);
    }
    return 0;
}

static inline int bcm_mvtags_len(char *ethHdr)
{
    unsigned int end_offset = 0;
    BcmEnet_hdr2* bhd;
    uint16 brcm_type;

    bhd = (BcmEnet_hdr2*)ethHdr;
    brcm_type = ntohs(bhd->brcm_type);
    if (brcm_type == BRCM_TYPE2)
    {
        end_offset += BRCM_TAG_TYPE2_LEN;
    }

    return end_offset;
}

/* based on impl5\bcmenet.c bcm_type_trans(), bcm_mvtags_len() */
static int port_sf2_rx_pkt_mod(enetx_port_t *port, struct sk_buff *skb)
{
    unsigned int end_offset = 0, from_offset = 0;
    uint16 *to, *end, *from;

    skb_reset_mac_header(skb);
    end_offset = bcm_mvtags_len(skb->data);
    if (end_offset)
    {
        from_offset = OFFSETOF(struct ethhdr, h_proto);

        to = (uint16*)(skb->data + from_offset + end_offset) - 1;
        end = (uint16*)(skb->data + end_offset) - 1;
        from = (uint16*)(skb->data + from_offset) - 1;

        while ( to != end )
            *to-- = *from--;
    }

    skb_set_mac_header(skb, end_offset);

    skb_pull(skb, end_offset);
    return 0;
}

static int port_sf2_port_role_set(enetx_port_t *self, port_netdev_role_t role)
{
    /* based on impl5\bcmsw.c:bcmsw_config_wan() */
    uint16_t wan_port_map;
    /* Configure WAN port */
    SF2SW_RREG(PAGE_CONTROL, REG_WAN_PORT_MAP, &wan_port_map, sizeof(wan_port_map));
    if (role==PORT_NETDEV_ROLE_WAN)
    {
        wan_port_map |= (1<<self->p.mac->mac_id); /* Add the WAN port in the port map */
    }
    else
    {
        wan_port_map &= ~(1<<self->p.mac->mac_id); /* remove the WAN port in the port map */
    }
    SF2SW_WREG(PAGE_CONTROL, REG_WAN_PORT_MAP, &wan_port_map, sizeof(wan_port_map));

    enet_dbg(" %s port %s as WAN; wan_pmap <0x%02x>\n", (role==PORT_NETDEV_ROLE_WAN)?"Add":"Remove", self->obj_name, wan_port_map);

    /* Disable learning */
    SF2SW_RREG(PAGE_CONTROL, REG_DISABLE_LEARNING, &wan_port_map, sizeof(wan_port_map));
    if (role==PORT_NETDEV_ROLE_WAN)
    {
        wan_port_map |= (1<<self->p.mac->mac_id); /* Add the WAN port in the port map */
    }
    else
    {
        wan_port_map &= ~(1<<self->p.mac->mac_id); /* remove the WAN port in the port map */
    }
    SF2SW_WREG(PAGE_CONTROL, REG_DISABLE_LEARNING, &wan_port_map, sizeof(wan_port_map));

    /* NOTE : No need to change the PBVLAN map -- switch logic does not care about pbvlan when the port is WAN */

    /* NOTE: For multiple IMP port products, switch will send all traffic from WAN to P8
       but we should use CFP to send it to correct port based on port grouping
       TBD */

    /* Age all dynamically learnt ARL on this port */
//    extsw_fast_age_port(port, 0);       // TODO_DSL: TODO_KERNEL_BOND
    return 0;
}

#include "linux/if_bridge.h"
static void port_sf2_port_stp_set(enetx_port_t *self, int mode, int state)
{
    // based on impl5\bcmenet.c bcm_set_hw_stp()
    int unit = PORT_ON_ROOT_SW(self)?0:1;
    int port = self->p.mac->mac_id;
    int swPort= PHYSICAL_PORT_TO_LOGICAL_PORT(port, unit);
    unsigned char stpVal;

    //enet_dbgv("%s mode=%d state=%d\n", self->obj_name, mode, state);
    switch (mode)
    {
    case STP_MODE_ENABLE:
        root_sw->s.stpDisabledPortMap &= ~(1 << swPort);
        /* set stp state to forward in case bridge STP is off 
           if bridge STP is on, STP state machine will be forced to restart */
        _ethsw_set_stp_mode(unit, port, REG_PORT_STP_STATE_FORWARDING);
        break;
    case STP_MODE_DISABLE:
        root_sw->s.stpDisabledPortMap |= (1 << swPort);
        _ethsw_set_stp_mode(unit, port, REG_PORT_NO_SPANNING_TREE);
        break;
    default:    // STP_MODE_UNCHANGED
        if (root_sw->s.stpDisabledPortMap & (1<<swPort))
            break;
        switch (state)
        {
        case BR_STATE_BLOCKING:     stpVal= REG_PORT_STP_STATE_BLOCKING; break;
        case BR_STATE_FORWARDING:   stpVal= REG_PORT_STP_STATE_FORWARDING; break;
        case BR_STATE_LEARNING:     stpVal= REG_PORT_STP_STATE_LEARNING; break;
        case BR_STATE_LISTENING:    stpVal= REG_PORT_STP_STATE_LISTENING; break;
        case BR_STATE_DISABLED:     stpVal= REG_PORT_STP_STATE_DISABLED; break;
        default:                    stpVal= REG_PORT_NO_SPANNING_TREE; break;
        }
        _ethsw_set_stp_mode(unit, port, stpVal);
    }
}

// ----------- SIOCETHSWCTLOPS ETHSWDUMPMIB functions ---

/* mib dump for ports on external SF2 switch */
static int port_sf2_mib_dump(enetx_port_t *self, int all)
{
    /* based on impl5\bcmsw.c sf2_bcmsw_dump_mib_ext() */
    unsigned int v32, errcnt;
    uint8_t data[8] = {0};
    int port = self->p.mac->mac_id;

    /* Display Tx statistics */
    printk("External Switch Stats : Port# %d\n",port);
    SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXUPKTS, &v32, 4);  // Get TX unicast packet count
    printk("TxUnicastPkts:          %10u \n", v32);
    SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXMPKTS, &v32, 4);  // Get TX multicast packet count
    printk("TxMulticastPkts:        %10u \n",  v32);
    SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXBPKTS, &v32, 4);  // Get TX broadcast packet count
    printk("TxBroadcastPkts:        %10u \n", v32);
    SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXDROPS, &v32, 4);
    printk("TxDropPkts:             %10u \n", v32);

    if (all)
    {
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXOCTETS, data, DATA_TYPE_MIB_COUNT|8);
        v32 = (((uint32*)data)[0]);
        printk("TxOctetsLo:             %10u \n", v32);
        v32 = (((uint32*)data)[1]);
        printk("TxOctetsHi:             %10u \n", v32);
//
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TX64OCTPKTS, &v32, 4);
        printk("TxPkts64Octets:         %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TX127OCTPKTS, &v32, 4);
        printk("TxPkts65to127Octets:    %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TX255OCTPKTS, &v32, 4);
        printk("TxPkts128to255Octets:   %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TX511OCTPKTS, &v32, 4);
        printk("TxPkts256to511Octets:   %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TX1023OCTPKTS, &v32, 4);
        printk("TxPkts512to1023Octets:  %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXMAXOCTPKTS, &v32, 4);
        printk("TxPkts1024OrMoreOctets: %10u \n", v32);
//
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXQ0PKT, &v32, 4);
        printk("TxQ0Pkts:               %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXQ1PKT, &v32, 4);
        printk("TxQ1Pkts:               %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXQ2PKT, &v32, 4);
        printk("TxQ2Pkts:               %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXQ3PKT, &v32, 4);
        printk("TxQ3Pkts:               %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXQ4PKT, &v32, 4);
        printk("TxQ4Pkts:               %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXQ5PKT, &v32, 4);
        printk("TxQ5Pkts:               %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXQ6PKT, &v32, 4);
        printk("TxQ6Pkts:               %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXQ7PKT, &v32, 4);
        printk("TxQ7Pkts:               %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXCOL, &v32, 4);
        printk("TxCol:                  %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXSINGLECOL, &v32, 4);
        printk("TxSingleCol:            %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXMULTICOL, &v32, 4);
        printk("TxMultipleCol:          %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXDEFERREDTX, &v32, 4);
        printk("TxDeferredTx:           %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXLATECOL, &v32, 4);
        printk("TxLateCol:              %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXEXCESSCOL, &v32, 4);
        printk("TxExcessiveCol:         %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXFRAMEINDISC, &v32, 4);
        printk("TxFrameInDisc:          %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXPAUSEPKTS, &v32, 4);
        printk("TxPausePkts:            %10u \n", v32);
    }
    else
    {
        errcnt=0;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXCOL, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXSINGLECOL, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXMULTICOL, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXDEFERREDTX, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXLATECOL, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXEXCESSCOL, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_TXFRAMEINDISC, &v32, 4);
        errcnt += v32;
        printk("TxOtherErrors:          %10u \n", errcnt);
    }

    /* Display Rx statistics */
    SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXUPKTS, &v32, 4);  // Get RX unicast packet count
    printk("RxUnicastPkts:          %10u \n", v32);
    SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXMPKTS, &v32, 4);  // Get RX multicast packet count
    printk("RxMulticastPkts:        %10u \n",v32);
    SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXBPKTS, &v32, 4);  // Get RX broadcast packet count
    printk("RxBroadcastPkts:        %10u \n",v32);
    SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXDROPS, &v32, 4);
    printk("RxDropPkts:             %10u \n",v32);
    SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXDISCARD, &v32, 4);
    printk("RxDiscard:              %10u \n", v32);

    if (all)
    {
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXOCTETS, data, DATA_TYPE_MIB_COUNT|8);
        v32 = (((uint32*)data)[0]);
        printk("RxOctetsLo:             %10u \n", v32);
        v32 = (((uint32*)data)[1]);
        printk("RxOctetsHi:             %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXGOODOCT, data, DATA_TYPE_MIB_COUNT|8);
        v32 = (((uint32*)data)[0]);
        printk("RxGoodOctetsLo:         %10u \n", v32);
        v32 = (((uint32*)data)[1]);
        printk("RxGoodOctetsHi:         %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXJABBERS, &v32, 4);
        printk("RxJabbers:              %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXALIGNERRORS, &v32, 4);
        printk("RxAlignErrs:            %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXFCSERRORS, &v32, 4);
        printk("RxFCSErrs:              %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXFRAGMENTS, &v32, 4);
        printk("RxFragments:            %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXOVERSIZE, &v32, 4);
        printk("RxOversizePkts:         %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXUNDERSIZEPKTS, &v32, 4);
        printk("RxUndersizePkts:        %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXPAUSEPKTS, &v32, 4);
        printk("RxPausePkts:            %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXSACHANGES, &v32, 4);
        printk("RxSAChanges:            %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXSYMBOLERRORS, &v32, 4);
        printk("RxSymbolError:          %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RX64OCTPKTS, &v32, 4);
        printk("RxPkts64Octets:         %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RX127OCTPKTS, &v32, 4);
        printk("RxPkts65to127Octets:    %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RX255OCTPKTS, &v32, 4);
        printk("RxPkts128to255Octets:   %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RX511OCTPKTS, &v32, 4);
        printk("RxPkts256to511Octets:   %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RX1023OCTPKTS, &v32, 4);
        printk("RxPkts512to1023Octets:  %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXMAXOCTPKTS, &v32, 4);
        printk("RxPkts1024OrMoreOctets: %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXJUMBOPKT , &v32, 4);
        printk("RxJumboPkts:            %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXOUTRANGEERR, &v32, 4);
        printk("RxOutOfRange:           %10u \n", v32);
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXINRANGEERR, &v32, 4);
        printk("RxInRangeErr:           %10u \n", v32);
    }
    else
    {
        errcnt=0;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXJABBERS, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXALIGNERRORS, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXFCSERRORS, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXFRAGMENTS, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXOVERSIZE, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXUNDERSIZEPKTS, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXSYMBOLERRORS, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXOUTRANGEERR, &v32, 4);
        errcnt += v32;
        SF2SW_RREG(PAGE_MIB_P0 + (port), SF2_REG_MIB_P0_RXINRANGEERR, &v32, 4);
        errcnt += v32;
        printk("RxOtherErrors:          %10u \n", errcnt);

    }

    return 0;
}

static inline uint32_t port_sf2_tx_q_remap(enetx_port_t *port, uint32_t txq)
{
    // based on impl5\bcmenet_runner_inline.h:bcmeapi_enet_prepare_xmit()
    return (queRemap >> (txq * 4)) & 0xf;
}

#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
static inline uint32_t _get_hash(uint8_t *pDa, uint8_t *pSa)
{
    return pDa[5]+pSa[5];
}

static inline uint16_t port_sf2_tx_lb_imp(enetx_port_t *port, uint16_t port_id, void* pHdr)
{
    // based on impl5/bcmenet.c: bcmenet_load_balance_imp()
    uint16_t emac_to_use = 0;

#if defined(CONFIG_BCM94908)
    /* P8/EMAC0=2.5G [P3,P7], P5/EMAC1=2.5G [P1,P2], P4/EMAC2=1.4G [P0] */
    emac_to_use = port_imp_emac_map[port_id];
#elif defined(CONFIG_BCM963158)
    /* P8/EMAC0=2.5G [P6], P7/EMAC1=2G [P3,P4] and P5/EMAC2=3G [P0,P1,P2] */
    emac_to_use = port_imp_emac_map[port_id];
#endif
    //enet_dbgv("%s port_id=%d --group-->emac=%d\n", port->dev->name, port_id, emac_to_use);
    return emac_to_use;
}
#endif /* defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT) */

// based on impl5\bcmenet_runner_inline.h:bcmeapi_pkt_xmt_dispatch()
static int dispatch_pkt_sf2_lan(dispatch_info_t *dispatch_info)
{
    rdpa_cpu_tx_extra_info_t extra_info;
    int rc;

    extra_info.u32 = 0; /* Initialize */

    {
#if !defined(CONFIG_BCM963158)
        uint32_t phys_port = dispatch_info->port->p.mac->mac_id;
#endif
#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
        spdsvcHook_transmit_t spdsvc_transmit;

        spdsvc_transmit.pNBuff = dispatch_info->pNBuff;
        spdsvc_transmit.dev = NULL;
        spdsvc_transmit.header_type = SPDSVC_HEADER_TYPE_ETH_BCMTAG;
        spdsvc_transmit.phy_overhead = BCM_ENET_OVERHEAD;

        rc = enet_spdsvc_transmit(&spdsvc_transmit);
        if(rc < 0)
        {
            /* In case of error, NBuff will be free by spdsvc */
            return 0;
        }
        extra_info.is_spdsvc_setup_packet = rc;
#endif
        extra_info.lag_port = dispatch_info->lag_port;

#if !defined(CONFIG_BCM963158)
        rc = rdpa_cpu_tx_port_enet_lan((bdmf_sysb)dispatch_info->pNBuff, dispatch_info->egress_queue,
                                       phys_port, extra_info);
#else
        {
        rdpa_cpu_tx_info_t info = {};

        info.method = rdpa_cpu_tx_port;
        info.port = dispatch_info->port->p.port_id;
        info.cpu_port = rdpa_cpu_host;
        info.x.lan.queue_id = dispatch_info->egress_queue;
        info.drop_precedence = dispatch_info->drop_eligible;
        info.lag_index = dispatch_info->lag_port;

        rc = rdpa_cpu_send_sysb((bdmf_sysb)dispatch_info->pNBuff, &info);
        }
#endif
        if (rc < 0)
        {
            /* skb is already released by rdpa_cpu_tx_port_enet_lan() */
            INC_STAT_TX_DROP(dispatch_info->port,tx_dropped_runner_lan_fail);
            return -1;
        }
    }

    return 0;
}

void phy_link_change_cb(void *ctx);

void dslbase_phy_link_change_cb(void *ctx)
{
    phy_dev_t *phy = ctx;
    phy_dev_t *active_phy = get_active_phy(phy);    /* if phy is crossbar get actual phy that triggerred event */

    phy_dev_t *first_phy = cascade_phy_get_first(phy);
    enetx_port_t *p = first_phy->sw_port;

enet_dbgv(" %s port_phy_id:%x active_phy_id:%x\n", p->obj_name, p->p.phy->addr, active_phy->addr);
    p->p.phy_last_change = (jiffies * 100) / HZ;

    /* Set cascaded PHY chain */
    if (is_cascade_phy(active_phy))
    {
        phy_dev_t *end_phy = cascade_phy_get_last(active_phy);
        phy_dev_t *phy_dev;

        for (phy_dev = cascade_phy_get_first(active_phy); phy_dev != end_phy; phy_dev = phy_dev->cascade_next)
        {
            if (active_phy->link)
            {
                if (phy_dev != active_phy)
                    phy_dev->phy_drv->speed_set(phy_dev, active_phy->speed, active_phy->duplex);
            }
            phy_dev->link = active_phy->link;
        }

        if (!active_phy->link)
            serdes_work_around(cascade_phy_get_first(active_phy));
    }

    if (p->dev)
    {
        /* Print new status to console */
        link_change_handler(p, active_phy->link, 
            speed_macro_2_mbps(active_phy->speed), 
            active_phy->duplex == PHY_DUPLEX_FULL);
    }
}

sw_ops_t port_sf2_sw =
{
    .init = port_sf2_sw_init,
    .uninit = port_sf2_sw_uninit,
//  .port_demux = port_generic_sw_demux,    // external switch does not have demux
//  .port_mux = port_sf2_sw_mux,
    .stats_get = port_generic_stats_get,
//  .stats_clear
    .port_id_on_sw = port_sf2_sw_port_id_on_sw,
    .hw_sw_state_set = port_sf2_sw_hw_sw_state_set,
    .hw_sw_state_get = port_sf2_sw_hw_sw_state_get,
    .config_trunk = port_sf2_sw_config_trunk,
    .update_pbvlan = port_sf2_sw_update_pbvlan,
    .rreg = extsw_rreg_wrap,
    .wreg = extsw_wreg_wrap,
};

port_ops_t port_sf2_port =
{
    .init = port_sf2_port_init,
    .uninit = port_runner_port_uninit,
    .dispatch_pkt = dispatch_pkt_sf2_lan,
    .stats_get = port_runner_port_stats_get,
    .stats_clear = port_generic_stats_clear,
    .pause_get = port_runner_pause_get,
    .pause_set = port_runner_pause_set,
    .open = port_sf2_port_open,
    .mtu_set = port_generic_mtu_set,
    .tx_q_remap = port_sf2_tx_q_remap,
#if defined(CONFIG_BCM_ENET_MULTI_IMP_SUPPORT)
    .tx_lb_imp = port_sf2_tx_lb_imp,
#endif
    .tx_pkt_mod = port_sf2_tx_pkt_mod,  /* insert brcm tag for port on external switch */
    .rx_pkt_mod = port_sf2_rx_pkt_mod,  /* remove brcm tag for port on external switch */
    .mib_dump = port_sf2_mib_dump,
    .print_status = port_sf2_print_status,
    .role_set = port_sf2_port_role_set,
    .stp_set = port_sf2_port_stp_set,
};


