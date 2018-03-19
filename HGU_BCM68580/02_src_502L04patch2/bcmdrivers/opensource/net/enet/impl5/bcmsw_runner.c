/*
    Copyright 2000-2010 Broadcom Corporation

    <:label-BRCM:2011:DUAL/GPL:standard

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


#define _BCMENET_LOCAL_

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/mii.h>
#include <linux/stddef.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <board.h>
#include "boardparms.h"
#include <bcm_map_part.h>
#include "bcm_intr.h"
#include "bcmenet.h"
#include "ethsw.h"
#include "ethsw_phy.h"
#include "bcmmii.h"
#include "bcmsw_runner.h"


#include "hwapi_mac.h"
#include "bcmsw.h"
#include "bcmswshared.h"
#include <rdpa_api.h>
#include <rdpa_types.h>

extern int wan_port_id;
extern spinlock_t bcm_extsw_access;
extern struct net_device* vnet_dev[];
#ifdef RDPA_VPORTS
extern struct net_device *rdpa_vport_to_dev[];
#endif
extern bdmf_object_handle rdpa_cpu_obj;
static struct emac_stats cached_rdpa_emac_stat[rdpa_emac__num_of];

void bcmsw_pmdio_rreg(int page, int reg, uint8 *data, int len)
{
    BCM_ENET_LINK_DEBUG("read op; page = %x; reg = %x; len = %d \n",
        (unsigned int) page, (unsigned int) reg, len);

    spin_lock_bh(&bcm_extsw_access);
    spin_unlock_bh(&bcm_extsw_access);

}
void bcmsw_pmdio_wreg(int page, int reg, uint8 *data, int len)
{
    BCM_ENET_LINK_DEBUG("write op; page = %x; reg = %x; len = %d \n",
        (unsigned int) page, (unsigned int) reg, len);
    BCM_ENET_LINK_DEBUG("given data = %02x %02x %02x %02x \n",
        data[0], data[1], data[2], data[3]);

    spin_lock_bh(&bcm_extsw_access);
    spin_unlock_bh(&bcm_extsw_access);
}

int ethsw_set_mac_hw(int port, PHY_STAT ps)
{
    bdmf_object_handle  port_obj = NULL;
    rdpa_if             rdpa_port;
    rdpa_emac_cfg_t     emac_cfg = {};
    bdmf_error_t        rc = BDMF_ERR_OK;
    rdpa_port_dp_cfg_t  port_cfg = {};
    u32 v32;

    rdpa_port = rdpa_port_map_from_hw_port(port, 0);
    if (rdpa_port == rdpa_if_none)
    {
        printk("\n\n\r **** invalid port %d - has no EMAC related\n", port);
        return -1;
    }

    rc = rdpa_port_get(rdpa_port, &port_obj);
    if (rc)
        return rc;

    rc = rdpa_port_cfg_get(port_obj, &port_cfg);
    if (rc != BDMF_ERR_OK)
    {
       printk("failed to rdpa_port_cfg_get rc=%d\n",rc);
       goto error;
    }

    if( port_cfg.emac == rdpa_emac_none )
    {
        printk("\n\n\r **** EMAC for port %d is not configured\n", port);
        return -1;
    }

    mac_hwapi_set_rxtx_enable(port_cfg.emac,0,0);
    v32 = *(u32 *)SWITCH_REG_LED_WAN_CNTRL_LED;
    mac_hwapi_get_configuration(port_cfg.emac,&emac_cfg);

    if (ps.lnk) /* Change speed/duplex and Enable UNIMAC rx/tx only if link is UP */
    {

        if (ps.fdx)
            emac_cfg.full_duplex = 1;
        else
            emac_cfg.full_duplex = 0;

        /* If no speed is set, use current speed read */
        if (ps.spd100)
            emac_cfg.rate = rdpa_emac_rate_100m;
        else if (ps.spd1000)
            emac_cfg.rate = rdpa_emac_rate_1g;
        else if (ps.spd2500)
            emac_cfg.rate = rdpa_emac_rate_2_5g;
        else if (ps.spd10)
            emac_cfg.rate = rdpa_emac_rate_10m;

        mac_hwapi_set_configuration(port_cfg.emac,&emac_cfg);
        mac_hwapi_set_rxtx_enable(port_cfg.emac,1,1);
        v32 |= SWITCH_REG_LED_WAN_TX_EN |SWITCH_REG_LED_WAN_RX_EN;
    }
    else
    {
        emac_cfg.full_duplex = 1;   /* Set UNI MAC to full dulex when link is down to work around HW issue */
        mac_hwapi_set_configuration(port_cfg.emac,&emac_cfg);
        /* Disable WAN LED to work around UNI MAC LED issue */
        v32 &= ~(SWITCH_REG_LED_WAN_TX_EN |SWITCH_REG_LED_WAN_RX_EN);
    }

    *(u32 *)SWITCH_REG_LED_WAN_CNTRL_LED = v32;

error:
    bdmf_put(port_obj);
    return rc;
}

void bcmeapi_ethsw_init_hw(int unit, uint32_t portMap,  int wanPortMap)
{
#if defined (CONFIG_BCM_JUMBO_FRAME)
    uint16 v16 = MAX_HW_JUMBO_FRAME_SIZE;
    /* 
       Set MIB values for jumbo frames to reflect our maximum frame size.
       Need to set size to hardware max size, otherwise byte counter and
       frame counters in hardware will be inconsistent
     */
    extsw_wreg_wrap(PAGE_JUMBO, REG_JUMBO_FRAME_SIZE, (uint8 *)&v16, 2);   
#endif
}

int bcmeapi_init_ext_sw_if(extsw_info_t *extSwInfo)
{
    ETHERNET_MAC_INFO *EnetInfo = EnetGetEthernetMacInfo();
    ETHERNET_MAC_INFO *info;
    uint32 sw_port, port_map;
    uint8 v8 = 0;

    /* Check if external switch is configured in boardparams */
    info = &EnetInfo[1];
   if (!((info->ucPhyType == BP_ENET_EXTERNAL_SWITCH) ||
         (info->ucPhyType == BP_ENET_SWITCH_VIA_INTERNAL_PHY)))
   {
      printk("No External switch connected\n");
      return -ENODEV;
   }
#if 0
   /* Runner external switch config is done through board init gatemakerpro_init()
    * Keeping the code in for later use - in case the same is not done through RDPA */
   {
      /* configure Runner for External switch */
      BL_SWITCH_CFG_DTE  xi_switch_cfg;
      BL_ERROR_DTE retVal;
      xi_switch_cfg.bridge_port_id        = CE_BL_BRIDGE_PORT_LAN_4;
      xi_switch_cfg.switch_hdr_type       = BRCM_HDR_OPCODE_0;
      xi_switch_cfg.remove_hdr_when_trap  = CE_STT_TRUE;
      retVal =  bl_api_cfg_ext_switch(&xi_switch_cfg);
      if ( retVal != CE_BL_NO_ERROR )
      {
         printk("\n\nERROR !! Runner External Switch cfg failed <%d>\n\n",(int)retVal);
      }
   }
#endif /* 0 */

/* Get the internal switch/runner MAC info */
   info = &EnetInfo[0];
   if (info->sw.port_map & (1<<extSwInfo->connected_to_internalPort))
   { /* Not needed but a valid check */
      unsigned long runner_phy_id = info->sw.phy_id[extSwInfo->connected_to_internalPort];
      PHY_STAT ps = {0};

      switch (runner_phy_id & (PHY_LNK_CFG_M << PHY_LNK_CFG_S))
      {

          case FORCE_LINK_10HD:
              ps.lnk = 1;
              break;

          case FORCE_LINK_10FD:
              ps.lnk = 1;
              ps.fdx = 1;
              break;

          case FORCE_LINK_100HD:
              ps.lnk = 1;
              ps.spd100 = 1;
              break;

          case FORCE_LINK_100FD:
              ps.lnk = 1;
              ps.spd100 = 1;
              ps.fdx = 1;
              break;

          case FORCE_LINK_1000FD:
              ps.lnk = 1;
              ps.spd1000 = 1;
              ps.fdx = 1;
              break;

          default:
              printk("Invalid Link/PHY config for internal port connected to external switch <0x%x>\n",(unsigned int)runner_phy_id);
         return -1;
      }

      /* Now set the MAC */
      ethsw_set_mac_hw(extSwInfo->connected_to_internalPort, ps);
   }

   info = &EnetInfo[1]; /* External Switch Info from Boardparams */
   sw_port = 0; /* Start with first port */
   port_map = info->sw.port_map;
   for (; port_map; sw_port++, port_map /= 2 )
   {
      /* Skip the switch ports which are not in the port_map */
      while ((port_map % 2) == 0)
      {
         port_map /= 2;
         sw_port++;
      }
      /* Enable RX and TX - these gets disabled in board driver (something different in Lilac based designs)*/
      v8 &= (~REG_PORT_CTRL_DISABLE);
   }

   return 0; /* Success */
}

#if !defined(bcmeapi_ioctl_que_mon)
static void check_que_mon_port(int port)
{
    static uint16 last_port = -1;

    if (last_port == port) return;
    last_port = port;
    extsw_wreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_DIAG_CTRL, (uint8 *)&last_port, 2);
    return;
}

int bcmeapi_ioctl_que_mon(BcmEnet_devctrl *pDevCtrl, struct ethswctl_data *e)
{
    int port = e->port,
        unit = e->unit,
        que = e->priority,
        type = e->sw_ctrl_type,
        val, err = 0;
    uint16 v16 = 0;

    if(unit != 1)
    {
        err = -1;
        goto error;
    }

    switch(type)
    {
        case QUE_CUR_COUNT:
            check_que_mon_port(port);
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_CUR_COUNT + que*2, (uint8 *)&v16, 2);
            break;
        case QUE_PEAK_COUNT:
            check_que_mon_port(port);
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_PEAK_COUNT + que*2, (uint8 *)&v16, 2);
            break;
        case SYS_TOTAL_PEAK_COUNT:
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_SYS_TOTAL_PEAK_COUNT, (uint8 *)&v16, 2);
            break;
        case SYS_TOTAL_USED_COUNT:
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_SYS_TOTAL_USED_COUNT, (uint8 *)&v16, 2);
            break;

        case PORT_PEAK_RX_BUFFER:
            check_que_mon_port(port);
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_PEAK_RX_BUFFER, (uint8 *)&v16, 2);
            break;
        case QUE_FINAL_CONGESTED_STATUS:
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_FINAL_CONG_STAT + 2*port, (uint8 *)&v16, 2);
            break;
        case PORT_PAUSE_HISTORY:
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_PAUSE_HISTORY, (uint8 *)&v16, 2);
            break;
        case PORT_PAUSE_QUAN_HISTORY:
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_PAUSE_QUAN_HISTORY, (uint8 *)&v16, 2);
            break;

        case PORT_RX_BASE_PAUSE_HISTORY:
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_RXBASE_PAUSE_HISTORY, (uint8 *)&v16, 2);
            break;
        case PORT_RX_BUFFER_ERROR_HISTORY:
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_PORT_RX_BUFFER_ERR_HISTORY, (uint8 *)&v16, 2);
            break;
        case QUE_CONGESTED_STATUS:
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_CONG_STATUS + 2*port, (uint8 *)&v16, 2);
            break;
        case QUE_TOTAL_CONGESTED_STATUS:
            extsw_rreg_wrap(PAGE_FLOW_CTRL_XTN, REG_FC_QUE_TOTAL_CONG_STATUS + 2*port, (uint8 *)&v16, 2);
            break;
    }

error:
    val = v16;
    e->val = val;
    e->ret_val = err;
    return 0;
}
#endif

int bcmeapi_ioctl_ethsw_clear_port_stats(struct ethswctl_data *e){
    uint32_t port = e->port;
    bdmf_object_handle port_obj = NULL;
    rdpa_port_stat_t stat;
    int rc = 0;
 
    memset(&stat, 0, sizeof(stat));

    if ((rdpa_emac0 + port) >= rdpa_emac__num_of)
    {
         BCM_ENET_ERROR("invalid lan port id %d \n", port);
         return -1;
    }

    bdmf_lock();
    rc = rdpa_port_get(rdpa_if_lan0 + port, &port_obj);
    if (rc)
       goto unlock_exit;

    /* Reset rdpa statistics */
    rdpa_port_stat_set(port_obj, &stat);

    unlock_exit:
    if (port_obj)
       bdmf_put(port_obj);

    bdmf_unlock();
    return rc;
}

int bcmeapi_ioctl_ethsw_get_port_stats(struct ethswctl_data *e)
{
    bdmf_object_handle port_obj = NULL;
    rdpa_port_stat_t port_stat;
    int rc;
    rdpa_if port_index = IsLogPortWan(e->port)? rdpa_wan_type_to_if(rdpa_wan_gbe) : rdpa_if_lan0 + e->port;

    BCM_ENET_INFO("Port = %2d", e->port);

    bdmf_lock();

    rc = rdpa_port_get(port_index, &port_obj);
    if (rc)
    {
        BCM_ENET_ERROR("invalid port id %d \n", e->port);
        goto unlock_exit;
    }

    memset(&port_stat, 0, sizeof(rdpa_port_stat_t));
    rc = rdpa_port_stat_get(port_obj, &port_stat);
    if (rc != BDMF_ERR_OK)
    {
        BCM_ENET_ERROR("failed to read port statistics rc=%d\n", rc);
        goto unlock_exit;
    }

    e->rdpa_port_stats_s.rx_valid_pkt = (unsigned long long)port_stat.rx_valid_pkt;
    e->rdpa_port_stats_s.rx_crc_error_pkt = (unsigned long long)port_stat.rx_crc_error_pkt;
    e->rdpa_port_stats_s.rx_discard_1 = (unsigned long long)port_stat.rx_discard_1;
    e->rdpa_port_stats_s.rx_discard_2 = (unsigned long long)port_stat.rx_discard_2;
    e->rdpa_port_stats_s.bbh_drop_1 = (unsigned long long)port_stat.bbh_drop_1;
    e->rdpa_port_stats_s.bbh_drop_2 = (unsigned long long)port_stat.bbh_drop_2;
    e->rdpa_port_stats_s.bbh_drop_3 = (unsigned long long)port_stat.bbh_drop_3;
    e->rdpa_port_stats_s.rx_discard_max_length = (unsigned long long)port_stat.rx_discard_max_length;
    e->rdpa_port_stats_s.rx_discard_min_length = (unsigned long long)port_stat.rx_discard_min_length;
    e->rdpa_port_stats_s.tx_valid_pkt = (unsigned long long)port_stat.tx_valid_pkt;
    e->rdpa_port_stats_s.tx_discard = (unsigned long long)port_stat.tx_discard;
    e->rdpa_port_stats_s.discard_pkt = (unsigned long long)port_stat.discard_pkt;
    
    e->rdpa_port_stats_s.rx_valid_bytes = (unsigned long long)port_stat.rx_valid_bytes;
    e->rdpa_port_stats_s.tx_valid_bytes = (unsigned long long)port_stat.tx_valid_bytes;
    e->rdpa_port_stats_s.rx_multicast_pkt = (unsigned long long)port_stat.rx_multicast_pkt;
    e->rdpa_port_stats_s.tx_multicast_pkt = (unsigned long long)port_stat.tx_multicast_pkt;
    e->rdpa_port_stats_s.rx_broadcast_pkt = (unsigned long long)port_stat.rx_broadcast_pkt;
    e->rdpa_port_stats_s.tx_broadcast_pkt = (unsigned long long)port_stat.tx_broadcast_pkt;

    if (port_obj)
        bdmf_put(port_obj);

unlock_exit:
    bdmf_unlock();

    return rc;
}

int bcmeapi_ioctl_ethsw_clear_port_emac(struct ethswctl_data *e)
{
    uint32_t port = e->port;
    bdmf_object_handle port_obj = NULL;
    rdpa_port_dp_cfg_t  port_cfg = {};
    rdpa_emac_stat_t emac_stat;
    int rc = 0;

    if ((rdpa_emac0 + port) >= rdpa_emac__num_of)
    {
        BCM_ENET_ERROR("invalid lan port id %d \n", port);
        return -1;
    }

    /* clear local data */
    memset(&cached_rdpa_emac_stat[rdpa_emac0 + port], 0, sizeof(struct emac_stats));

    /* clear counters in rdpa */
    bdmf_lock();

    rc = rdpa_port_get(rdpa_if_lan0 + port, &port_obj);
    if (rc)
        goto unlock_exit;

    rc = rdpa_port_cfg_get(port_obj, &port_cfg);
    if (rc != BDMF_ERR_OK)
    {
        printk("failed to rdpa_port_cfg_get rc=%d\n", rc);
        goto unlock_exit;
    }
    
    if (rdpa_emac_none == port_cfg.emac)
    {
        BCM_ENET_NOTICE("invalid emac id %d \n", port_cfg.emac);
        goto unlock_exit;
    }
    
    mac_hwapi_get_rx_counters (port_cfg.emac, &emac_stat.rx);
    mac_hwapi_get_tx_counters (port_cfg.emac, &emac_stat.tx);

unlock_exit:
    if (port_obj)
        bdmf_put(port_obj);

    bdmf_unlock();
    return rc;
}

void bcmsw_get_emac_stats(rdpa_emac emac){

    rdpa_emac_stat_t emac_stat;
        
    bdmf_lock();

    mac_hwapi_get_rx_counters (emac, &emac_stat.rx);

    mac_hwapi_get_tx_counters (emac, &emac_stat.tx);
    
    /* rdpa counters are read and clear, need to store data into local cache */
    cached_rdpa_emac_stat[emac].rx_byte += emac_stat.rx.byte;
    cached_rdpa_emac_stat[emac].rx_packet += emac_stat.rx.packet;
    cached_rdpa_emac_stat[emac].rx_frame_64 += emac_stat.rx.frame_64;
    cached_rdpa_emac_stat[emac].rx_frame_65_127 += emac_stat.rx.frame_65_127;
    cached_rdpa_emac_stat[emac].rx_frame_128_255 += emac_stat.rx.frame_128_255;
    cached_rdpa_emac_stat[emac].rx_frame_256_511 += emac_stat.rx.frame_256_511;
    cached_rdpa_emac_stat[emac].rx_frame_512_1023 += emac_stat.rx.frame_512_1023;
    cached_rdpa_emac_stat[emac].rx_frame_1024_1518 += emac_stat.rx.frame_1024_1518;
    cached_rdpa_emac_stat[emac].rx_frame_1519_mtu += emac_stat.rx.frame_1519_mtu;
    cached_rdpa_emac_stat[emac].rx_unicast_packet += emac_stat.rx.unicast_packet;
    cached_rdpa_emac_stat[emac].rx_multicast_packet += emac_stat.rx.multicast_packet;
    cached_rdpa_emac_stat[emac].rx_broadcast_packet += emac_stat.rx.broadcast_packet;
    cached_rdpa_emac_stat[emac].rx_alignment_error += emac_stat.rx.alignment_error;
    cached_rdpa_emac_stat[emac].rx_frame_length_error += emac_stat.rx.frame_length_error;
    cached_rdpa_emac_stat[emac].rx_code_error += emac_stat.rx.code_error;
    cached_rdpa_emac_stat[emac].rx_carrier_sense_error += emac_stat.rx.carrier_sense_error;
    cached_rdpa_emac_stat[emac].rx_fcs_error += emac_stat.rx.fcs_error;
    cached_rdpa_emac_stat[emac].rx_control_frame += emac_stat.rx.control_frame;
    cached_rdpa_emac_stat[emac].rx_pause_control_frame += emac_stat.rx.pause_control_frame;
    cached_rdpa_emac_stat[emac].rx_unknown_opcode += emac_stat.rx.unknown_opcode;
    cached_rdpa_emac_stat[emac].rx_undersize_packet += emac_stat.rx.undersize_packet;
    cached_rdpa_emac_stat[emac].rx_oversize_packet += emac_stat.rx.oversize_packet;
    cached_rdpa_emac_stat[emac].rx_fragments += emac_stat.rx.fragments;
    cached_rdpa_emac_stat[emac].rx_jabber += emac_stat.rx.jabber;
    cached_rdpa_emac_stat[emac].rx_overflow += emac_stat.rx.overflow;
                             
    cached_rdpa_emac_stat[emac].tx_byte += emac_stat.tx.byte;
    cached_rdpa_emac_stat[emac].tx_packet += emac_stat.tx.packet;
    cached_rdpa_emac_stat[emac].tx_frame_64 += emac_stat.tx.frame_64;
    cached_rdpa_emac_stat[emac].tx_frame_65_127 += emac_stat.tx.frame_65_127;
    cached_rdpa_emac_stat[emac].tx_frame_128_255 += emac_stat.tx.frame_128_255;
    cached_rdpa_emac_stat[emac].tx_frame_256_511 += emac_stat.tx.frame_256_511;
    cached_rdpa_emac_stat[emac].tx_frame_512_1023 += emac_stat.tx.frame_512_1023;
    cached_rdpa_emac_stat[emac].tx_frame_1024_1518 += emac_stat.tx.frame_1024_1518;
    cached_rdpa_emac_stat[emac].tx_frame_1519_mtu += emac_stat.tx.frame_1519_mtu;
    cached_rdpa_emac_stat[emac].tx_fcs_error += emac_stat.tx.fcs_error;
    cached_rdpa_emac_stat[emac].tx_unicast_packet += emac_stat.tx.unicast_packet;
    cached_rdpa_emac_stat[emac].tx_multicast_packet += emac_stat.tx.multicast_packet;
    cached_rdpa_emac_stat[emac].tx_broadcast_packet += emac_stat.tx.broadcast_packet;
    cached_rdpa_emac_stat[emac].tx_excessive_collision += emac_stat.tx.excessive_collision;
    cached_rdpa_emac_stat[emac].tx_late_collision += emac_stat.tx.late_collision;
    cached_rdpa_emac_stat[emac].tx_single_collision += emac_stat.tx.single_collision;
    cached_rdpa_emac_stat[emac].tx_multiple_collision += emac_stat.tx.multiple_collision;
    cached_rdpa_emac_stat[emac].tx_total_collision += emac_stat.tx.total_collision;
    cached_rdpa_emac_stat[emac].tx_pause_control_frame += emac_stat.tx.pause_control_frame;
    cached_rdpa_emac_stat[emac].tx_deferral_packet += emac_stat.tx.deferral_packet;
    cached_rdpa_emac_stat[emac].tx_excessive_deferral_packet += emac_stat.tx.excessive_deferral_packet;
    cached_rdpa_emac_stat[emac].tx_jabber_frame += emac_stat.tx.jabber_frame;
    cached_rdpa_emac_stat[emac].tx_control_frame += emac_stat.tx.control_frame;
    cached_rdpa_emac_stat[emac].tx_oversize_frame += emac_stat.tx.oversize_frame;
    cached_rdpa_emac_stat[emac].tx_undersize_frame += emac_stat.tx.undersize_frame;
    cached_rdpa_emac_stat[emac].tx_fragments_frame += emac_stat.tx.fragments_frame;
    cached_rdpa_emac_stat[emac].tx_error += emac_stat.tx.error;
    cached_rdpa_emac_stat[emac].tx_underrun += emac_stat.tx.underrun;

    bdmf_unlock();
}

int bcmeapi_ioctl_ethsw_get_port_emac(struct ethswctl_data *e)
{
    uint32_t port = e->port;
    bdmf_object_handle port_obj = NULL;
    rdpa_port_dp_cfg_t  port_cfg = {};
    int rc;

    BCM_ENET_INFO("Port = %2d", e->port);

    if ( (rdpa_emac0 + port) >= rdpa_emac__num_of )
    {
        BCM_ENET_ERROR("invalid lan port id %d \n", port);
        return -1;
    }

    rc = rdpa_port_get(rdpa_if_lan0 + port, &port_obj);
    if (rc )
        goto unlock_exit;

    rc = rdpa_port_cfg_get(port_obj, &port_cfg);
    if ( rc != BDMF_ERR_OK)
    {
       printk("failed to rdpa_port_cfg_get rc=%d\n",rc);
       goto unlock_exit;
    }

    if (rdpa_emac_none == port_cfg.emac)
    {
        BCM_ENET_DEBUG("invalid emac id %d \n", port_cfg.emac);
        goto unlock_exit;
    }

    bcmsw_get_emac_stats(port_cfg.emac);

    memcpy((void*)&e->emac_stats_s, (void*)&cached_rdpa_emac_stat[port_cfg.emac], sizeof(struct emac_stats));

    unlock_exit:
    if (port_obj)
      bdmf_put(port_obj);

    return rc;
}

int bcmsw_unimac_rxtx_op(int port, int get, int *disable)
{
    bdmf_object_handle  port_obj = NULL;
    rdpa_if             rdpa_port;
    bdmf_error_t        rc = BDMF_ERR_OK;
    rdpa_port_dp_cfg_t  port_cfg = {};
    u32 txEnable, rxEnable;

    rdpa_port = rdpa_port_map_from_hw_port(port, 0);
    if (rdpa_port == rdpa_if_none)
    {
        printk("\n\n\r **** invalid port %d - has no EMAC related\n", port);
        return -1;
    }

    rc = rdpa_port_get(rdpa_port, &port_obj);
    if (rc)
        return rc;

    rc = rdpa_port_cfg_get(port_obj, &port_cfg);
    if (rc != BDMF_ERR_OK)
    {
       printk("failed to rdpa_port_cfg_get rc=%d\n",rc);
       goto error;
    }

    if( port_cfg.emac == rdpa_emac_none )
    {
        printk("\n\n\r **** EMAC for port %d is not configured\n", port);
        return -1;
    }

        txEnable = rxEnable = 0;
    if (get) 
    {
        mac_hwapi_get_rxtx_enable(port_cfg.emac, &rxEnable, &txEnable);
        *disable = (txEnable? 0: REG_PORT_TX_DISABLE) | (rxEnable? 0: REG_PORT_RX_DISABLE);
    }
    else 
    {
        mac_hwapi_get_rxtx_enable(port_cfg.emac, &rxEnable, &txEnable);

        txEnable = !(*disable & REG_PORT_TX_DISABLE);
        rxEnable = !(*disable & REG_PORT_RX_DISABLE);
        mac_hwapi_set_rxtx_enable(port_cfg.emac, rxEnable, txEnable);

        mac_hwapi_get_rxtx_enable(port_cfg.emac, &rxEnable, &txEnable);
    }

error:
    bdmf_put(port_obj);
    return rc;
}

/*
 *  Function : bcmeapi_ioctl_ethsw_port_irc_set
 *
 *  Purpose :
 *  Set the burst size and rate limit value of the selected port ingress rate.
 *
 *  Parameters :
 *      unit   :   unit id
 *      port   :   port id.
 *      limit  :   rate limit value. (Kbits)
 *      burst_size  :   max burst size. (Kbits)
 *
 *  Return :
 *
 *  Note :
 *      Set the limit and burst size to bucket 0(storm control use bucket1).
 *
 */
int bcmeapi_ioctl_ethsw_port_irc_set(struct ethswctl_data *e)
{
    uint32_t  val32, bs = 0, rf = 0;

    down(&bcm_ethlock_switch_config);

    if (e->limit == 0) { /* Disable ingress rate control */
        extsw_rreg_wrap(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
        val32 &= ~REG_PN_BUCK1_ENABLE_MASK;
        extsw_wreg_wrap(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
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

        extsw_rreg_wrap(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
        val32 &= ~((REG_PN_BUCK1_SIZE_M << REG_PN_BUCK1_SIZE_S)| (REG_PN_BUCK1_REF_CNT_M << REG_PN_BUCK1_REF_CNT_S));
        val32 |= REG_PN_BUCK1_ENABLE_MASK | REG_PN_BUCK1_MODE_MASK; // use bucket 1
        val32 |= (rf & REG_PN_BUCK1_REF_CNT_M) << REG_PN_BUCK1_REF_CNT_S;
        val32 |= bs  << REG_PN_BUCK1_SIZE_S;
        extsw_wreg_wrap(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);

        extsw_rreg_wrap(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_CFG_PORT_0 + e->port * 2, &val32, 4);
	    val32 |= REG_PN_BUCK1_IFG_BYTES_MASK | (REG_PN_BUCK1_PKT_SEL_M << REG_PN_BUCK1_PKT_SEL_S);
        extsw_wreg_wrap(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_CFG_PORT_0 + e->port * 2, &val32, 4);
    }

    up(&bcm_ethlock_switch_config);
    return BCM_E_NONE;
}

/*
 *  Function : bcmeapi_ioctl_ethsw_port_irc_get
 *
 *  Purpose :
 *   Get the burst size and rate limit value of the selected port ingress rate.
 *
 *  Parameters :
 *      unit        :   unit id
 *      port   :   port id.
 *      limit  :   rate limit value. (Kbits)
 *      burst_size  :   max burst size. (Kbits)
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      Set the limit and burst size to bucket 0(storm control use bucket1).
 *
 */
int bcmeapi_ioctl_ethsw_port_irc_get(struct ethswctl_data *e)
{
    uint32_t  val32, rf;

    down(&bcm_ethlock_switch_config);

    extsw_rreg_wrap(PAGE_PORT_STORM_CONTROL, REG_PN_STORM_CTL_RATE_PORT_0 + e->port * 4, &val32, 4);
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

    up(&bcm_ethlock_switch_config);
    return BCM_E_NONE;
}

static inline int IsValidRunnerPort(int port)
{
#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    /* 63138 can only have a WAN port on Runner -
       need to put in this cluge because RDPA and Ethernet Driver do not work well together when External Switch is in place.
       Hate to do this - but no other option;
       Without this check - If Runner EMAC_0 is not configured as WAN - blindly we will consider this port as LAN */
    return IsLogPortWan(PHYSICAL_PORT_TO_LOGICAL_PORT(port, 0)) ? 1 : 0;
#else
    return 1;
#endif
}

void bcmeapi_reset_mib(void)
{
    memset(cached_rdpa_emac_stat, 0, sizeof(cached_rdpa_emac_stat));
}

int bcmeapi_ethsw_dump_mib(int port, int type, int queue)
{
    int					rc;
    bdmf_object_handle 	port_obj = NULL;
    rdpa_if             rdpa_port = rdpa_if_lan0 + (rdpa_if)port;
    rdpa_port_dp_cfg_t  port_cfg = {};

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    rdpa_port = rdpa_wan_type_to_if(rdpa_wan_gbe); 
#endif

    rc = rdpa_port_get(rdpa_port,&port_obj);
    if ( rc != BDMF_ERR_OK)
    {
        printk("failed to get rdpa port object rc=%d\n",rc);
        return -1;
    }

    rc = rdpa_port_cfg_get(port_obj, &port_cfg);
    if ( rc != BDMF_ERR_OK)
    {
       printk("failed to rdpa_port_cfg_get rc=%d\n",rc);
       return -1;
    }

    bcmsw_get_emac_stats(port_cfg.emac);

    bdmf_put(port_obj);
    printk("\nRunner Stats : Port# %d\n",port);

    /* Display Tx statistics */
     /* Display Tx statistics */
    printk("\n");
    printk("TxUnicastPkts:          %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_packet);
    printk("TxMulticastPkts:        %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_multicast_packet);
    printk("TxBroadcastPkts:        %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_broadcast_packet);
    printk("TxDropPkts:             %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_error);

    /* Display remaining tx stats only if requested */
    if (type) {
        printk("TxBytes:                %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_byte);
        printk("TxFragments:            %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_fragments_frame);
        printk("TxCol:                  %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_total_collision);
        printk("TxSingleCol:            %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_single_collision);
        printk("TxMultipleCol:          %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_multiple_collision);
        printk("TxDeferredTx:           %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_deferral_packet);
        printk("TxLateCol:              %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_late_collision);
        printk("TxExcessiveCol:         %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_excessive_collision);
        printk("TxPausePkts:            %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_pause_control_frame);
        printk("TxExcessivePkts:        %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_excessive_deferral_packet);
        printk("TxJabberFrames:         %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_jabber_frame);
        printk("TxFcsError:             %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_fcs_error);
        printk("TxCtrlFrames:           %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_control_frame);
        printk("TxOverSzFrames:         %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_oversize_frame);
        printk("TxUnderSzFrames:        %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_undersize_frame);
        printk("TxUnderrun:             %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_underrun);
        printk("TxPkts64Octets:         %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_frame_64);
        printk("TxPkts65to127Octets:    %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_frame_65_127);
        printk("TxPkts128to255Octets:   %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_frame_128_255);
        printk("TxPkts256to511Octets:   %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_frame_256_511);
        printk("TxPkts512to1023Octets:  %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_frame_512_1023);
        printk("TxPkts1024to1518Octets: %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_frame_1024_1518);
        printk("TxPkts1519toMTUOctets:  %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].tx_frame_1519_mtu);
    }                                                   

    /* Display Rx statistics */
    printk("\n");
    printk("RxUnicastPkts:          %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_packet);
    printk("RxMulticastPkts:        %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_multicast_packet);
    printk("RxBroadcastPkts:        %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_broadcast_packet);

    /* Display remaining rx stats only if requested */
    if (type) {
        printk("RxBytes:                %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_byte);
        printk("RxJabbers:              %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_jabber);
        printk("RxAlignErrs:            %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_alignment_error);
        printk("RxFCSErrs:              %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_fcs_error);
        printk("RxFragments:            %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_fragments);
        printk("RxOversizePkts:         %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_oversize_packet);
        printk("RxUndersizePkts:        %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_undersize_packet);
        printk("RxPausePkts:            %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_pause_control_frame);
        printk("RxOverflow:             %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_overflow);
        printk("RxCtrlPkts:             %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_control_frame);
        printk("RxUnknownOp:            %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_unknown_opcode);
        printk("RxLenError:             %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_frame_length_error);
        printk("RxCodeError:            %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_code_error);
        printk("RxCarrierSenseErr:      %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_carrier_sense_error);
        printk("RxPkts64Octets:         %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_frame_64);
        printk("RxPkts65to127Octets:    %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_frame_65_127);
        printk("RxPkts128to255Octets:   %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_frame_128_255);
        printk("RxPkts256to511Octets:   %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_frame_256_511);
        printk("RxPkts512to1023Octets:  %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_frame_512_1023);
        printk("RxPkts1024to1522Octets: %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_frame_1024_1518);
        printk("RxPkts1523toMTU:        %10u \n", (unsigned int)cached_rdpa_emac_stat[port_cfg.emac].rx_frame_1519_mtu);
    }
    return 0;
}

#ifdef RDPA_VPORTS
int bcmeapi_GetStatsExt(int lan_port, struct rtnl_link_stats64 *stats)
{
    bdmf_object_handle port_obj = NULL;
    rdpa_port_stat_t port_stat;
    rdpa_if rdpaif = rdpa_if_lan0 + lan_port;
    int rc;

    rc = rdpa_port_get(rdpaif, &port_obj);
    if (rc)
        goto port_unlock_exit;
    
    rdpa_port_stat_get(port_obj, &port_stat);

    stats->multicast = port_stat.rx_multicast_pkt;
    stats->rx_bytes = port_stat.rx_valid_bytes;
    stats->rx_packets = port_stat.rx_valid_pkt;
    stats->tx_bytes = port_stat.tx_valid_bytes;
    stats->tx_packets = port_stat.tx_valid_pkt;
#if defined(CONFIG_BCM_KF_EXTSTATS)
    stats->tx_multicast_packets = port_stat.tx_multicast_pkt;
    stats->rx_broadcast_packets = port_stat.rx_broadcast_pkt;
    stats->tx_broadcast_packets = port_stat.tx_broadcast_pkt;
#endif
port_unlock_exit:
    if (port_obj)
        bdmf_put(port_obj);

    return rc;
}
#endif

int ethsw_get_hw_stats(int port, struct rtnl_link_stats64 *stats)
{
   bdmf_object_handle port_obj = NULL;
   rdpa_port_dp_cfg_t port_cfg;
   struct net_device* vnet_dev_lcl;
   int vportIdx;
   int rc;

   /*validate device is already initialized first device is the bcmsw*/
   vportIdx = LOGICAL_PORT_TO_VPORT(PHYSICAL_PORT_TO_LOGICAL_PORT(port, 0)); /* Runner is always unit=0 */
   if (vportIdx < 0 || vportIdx > MAX_NUM_OF_VPORTS)
   {
       return 0;
   }
   vnet_dev_lcl = vnet_dev[vportIdx];
   if (!vnet_dev_lcl)
   {
      return 0;
   }

   rc = IsValidRunnerPort(port);
   if (!rc)
       goto port_stat_exit; /* Return the cached stats as last seen for this port */

   BCM_ENET_INFO("Port = %2d", port);

   {
      rdpa_if rdpaif = rdpa_if_lan0 + port;
      /* Check if the runner Ethernet port is configured as WAN */
      if (IsLogPortWan(PHYSICAL_PORT_TO_LOGICAL_PORT(port, 0))) {
          rdpaif = rdpa_wan_type_to_if(rdpa_wan_gbe);
      }
      rc = rdpa_port_get(rdpaif, &port_obj);
      if (rc)
         goto port_unlock_exit;

      rc = rdpa_port_cfg_get(port_obj, &port_cfg);
      if (rc || port_cfg.emac == rdpa_emac_none)
          goto port_unlock_exit;

      bcmsw_get_emac_stats(port_cfg.emac);

      stats->collisions = cached_rdpa_emac_stat[port_cfg.emac].tx_total_collision;
      stats->multicast = cached_rdpa_emac_stat[port_cfg.emac].rx_multicast_packet;
      stats->rx_bytes = cached_rdpa_emac_stat[port_cfg.emac].rx_byte;
      stats->rx_packets = cached_rdpa_emac_stat[port_cfg.emac].rx_packet;
      stats->rx_crc_errors = cached_rdpa_emac_stat[port_cfg.emac].rx_fcs_error;
      stats->rx_errors = cached_rdpa_emac_stat[port_cfg.emac].rx_alignment_error +
                               cached_rdpa_emac_stat[port_cfg.emac].rx_code_error +
                               cached_rdpa_emac_stat[port_cfg.emac].rx_frame_length_error ;
      stats->rx_length_errors = cached_rdpa_emac_stat[port_cfg.emac].rx_frame_length_error;
      stats->tx_bytes = cached_rdpa_emac_stat[port_cfg.emac].tx_byte;
      stats->tx_errors = cached_rdpa_emac_stat[port_cfg.emac].tx_error;
      stats->tx_packets = cached_rdpa_emac_stat[port_cfg.emac].tx_packet;

#if defined(CONFIG_BCM_KF_EXTSTATS)
      stats->tx_multicast_packets = cached_rdpa_emac_stat[port_cfg.emac].tx_multicast_packet;
#endif
      port_unlock_exit:
      if (port_obj)
         bdmf_put(port_obj);
   }

port_stat_exit:

   return rc;
}

int bcmeapi_ioctl_ethsw_sal_dal_set (struct ethswctl_data *e)
{
   uint32_t port = e->port;
   bdmf_object_handle port_obj = NULL;
   rdpa_if rdpaif;
   rdpa_port_dp_cfg_t cfg;
   int rc = 0;

   if(e->unit) /*unit 0 - emacs, unit 1 - wan*/
   {
      rdpaif = rdpa_wan_type_to_if(rdpa_wan_gbe); 
   }
   else
   {
      rdpaif = rdpa_if_lan0 + port;
      if ( rdpa_emac0 + port>= rdpa_emac__num_of )
      {
         BCM_ENET_ERROR("invalid lan port id %d \n", port);
         return -1;
      }
   }
   rc = rdpa_port_get(rdpaif, &port_obj);
   if (rc )
      goto unlock_exit;
   rc = rdpa_port_cfg_get(port_obj , &cfg);
   if (rc >= 0)
   {
      cfg.sal_enable = e->  sal_dal_en;
      cfg.dal_enable = e->  sal_dal_en;
      rc = rdpa_port_cfg_set(port_obj, &cfg);
   }
   unlock_exit:
   if(port_obj)
      bdmf_put(port_obj);
   return rc;
}

int bcmeapi_ioctl_ethsw_mtu_set(struct ethswctl_data *e)
{
   int rc = 0;

    if(NULL == phyPortId_to_netdev(e->port, -1)){
        BCM_ENET_ERROR("invalid lan port id %d \n", e->port);
        return -1;
   }

    mac_hwapi_set_rx_max_frame_len(rdpa_emac0 + e->port, e->mtu+ENET_MAX_MTU_EXTRA_SIZE);

   return rc;
}

int bcmeapi_ioctl_ethsw_ifname_to_rdpaif(struct ethswctl_data *e)
{
    int i;
    struct net_device *dev = dev_get_by_name(&init_net, e->ifname);

    if (!dev)
        return -1;

    /* find root device */
    while( 1 )
    {
        if(netdev_path_is_root(dev))
        {
            break;
        }
        dev_put(dev);
        dev = netdev_path_next_dev(dev);
        dev_hold(dev);
    }

#ifdef RDPA_VPORTS
    for (i = 0; i < rdpa_if__number_of; i++)
    {
        if (rdpa_vport_to_dev[i] == dev)
        {
            e->val = i;
            return 0;
        }
    }
#endif
    for (i = 0; i < MAX_NUM_OF_VPORTS; i++)
    {
        if (vnet_dev[i] == dev)
            goto Exit;
    }
Exit:
    if (dev)
    {
        BcmEnet_devctrl *priv = (BcmEnet_devctrl *)netdev_priv(dev);
        e->val = priv->rdpa_port;
        dev_put(dev);
        return 0;
    }
    return -1;
}

// Set STP state into SF2 register
void bcmeapi_ethsw_set_stp_mode(unsigned int unit, unsigned int port, unsigned char stpState)
{
   unsigned char portInfo;

   if(unit==1) // SF2
   {
      extsw_rreg_wrap(PAGE_CONTROL, REG_PORT_CTRL + (port),
                 &portInfo, sizeof(portInfo));
      portInfo &= ~REG_PORT_STP_MASK;
      portInfo |= stpState;
      extsw_wreg_wrap(PAGE_CONTROL, REG_PORT_CTRL + (port),
                 &portInfo, sizeof(portInfo));
   }
}
/* Add/Remove the given physical port to RDPA bond Object */
int bcm_enet_rdp_config_bond(int add, int grp_no, int unit, uint16_t port)
{
    rdpa_if rdpa_port = bcmenet_logical_port_to_rdpa_if(PHYSICAL_PORT_TO_LOGICAL_PORT(port,unit));
    rdpa_if rdpa_bond = rdpa_if_bond0 + grp_no;
    bdmf_object_handle port_obj = NULL;
    bdmf_object_handle bond_obj = NULL;
    int rc;

    if ( rdpa_port == rdpa_if_none )
    {
        printk("\n%s() - Error : Invalid rdpa port for unit=%d port=%d\n\n",__FUNCTION__, unit, port);
        return -1;
    }

    if ( rdpa_bond > rdpa_if_bond_max )
    {
        printk("\n%s() - Error : Invalid rdpa bond %d for grp_no=%d unit=%d port=%d\n\n",__FUNCTION__, rdpa_bond,grp_no, unit, port);
        return -1;
    }

    rc = rdpa_port_get(rdpa_port, &port_obj);
    if (rc)
    {
        printk("\n%s() - Error : NO rdpa port for rdpa_if %d unit=%d port=%d\n\n",__FUNCTION__, rdpa_port, unit, port);
        return -1;
    }

    /* get the rdpa bond port in order to link to lan ports */
    rc = rdpa_port_get(rdpa_bond, &bond_obj);
    if (rc)
    {
        if (add)
        {
            /* Bond object does not exist - Create one */
            BDMF_MATTR(rdpa_port_attrs, rdpa_port_drv());
            rdpa_port_index_set(rdpa_port_attrs, rdpa_bond);
            rc = bdmf_new_and_set(rdpa_port_drv(), NULL, rdpa_port_attrs, &bond_obj);
            if (rc)
            {
                printk("%s %s Failed to create bond port rc(%d)\n", __FILE__, __FUNCTION__, rc);
                goto error_exit;
            }
        }
        else
        {
            printk("\n%s() - Error : No rdpa bond %d for grp_no=%d unit=%d port=%d\n\n",__FUNCTION__, rdpa_bond,grp_no, unit, port);
            goto error_exit;
        }
    }

    if (add)
    {
        /* Link the port with bond object */
        rc = bdmf_link(bond_obj, port_obj, NULL);
        if (rc)
        {
            printk("%s %s Failed to link bond port %d for grp_no=%d unit=%d port=%d\n", __FILE__, __FUNCTION__, rdpa_bond,grp_no, unit, port);
            goto error_exit;
        }
    }
    else
    {
        /* UnLink the port from bond object */
        rc = bdmf_unlink(bond_obj, port_obj);
        if (rc)
        {
            printk("%s %s Failed to unlink bond port %d for grp_no=%d unit=%d port=%d\n", __FILE__, __FUNCTION__, rdpa_bond,grp_no, unit, port);
            goto error_exit;
        }

        if (bdmf_get_next_us_link(bond_obj, NULL) == NULL)
        {
            /* No More linked objects to this bond object - destroy */
            if ( bdmf_destroy(bond_obj) )
            {
                printk("%s %s Failed to destroy bond port %d for grp_no=%d unit=%d port=%d\n", __FILE__, __FUNCTION__, rdpa_bond,grp_no, unit, port);
            }
            else
            {
                bond_obj = NULL;
            }
        }
    }

error_exit:
    if (port_obj)
    {
        bdmf_put(port_obj);
    }
    if (bond_obj)
    {
        bdmf_put(bond_obj);
    }
    return rc;
}

