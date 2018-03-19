/*
   Copyright (c) 2017 Broadcom Corporation
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
/*
 * xport_drv.c
 *
 */

//includes
#include "xport_ag.h"
#include "xport_drv.h"
#include <asm/delay.h>
#include "xport_intr.h"
#include "bcm_map_part.h"
void remap_ru_block_addrs(uint32_t block_index, const ru_block_rec *ru_blocks[]);
#define UDELAY udelay
#include "mii_shared.h"
#include "boardparms.h"

static xport_init_s local_xport_cfg = {};

int validate_xport_configuration(xport_init_s *init_params)
{
    return XPORT_ERR_OK;
}

static int xport_speed_to_xlmac(XPORT_PORT_RATE xport_speed)
{
    int ret = 0;

    switch (xport_speed)
    {
    case XPORT_RATE_10MB:
        ret = XLMAC_PORT_SPEED_10MB;
        break;
    case XPORT_RATE_100MB:
        ret = XLMAC_PORT_SPEED_100MB;
        break;
    case XPORT_RATE_1000MB:
        ret = XLMAC_PORT_SPEED_1000MB;
        break;
    case XPORT_RATE_2500MB:
        ret = XLMAC_PORT_SPEED_2500MB;
        break;
    case XPORT_RATE_10G:
        ret = XLMAC_PORT_SPEED_10G;
        break;
    case XPORT_RATE_UNKNOWN:
    default:
        pr_err("Wrong xport speed %d\n", xport_speed);
    }

    return ret;
}

static XPORT_PORT_RATE xport_speed_get(uint8_t portid)
{
    uint8_t speed_mode, no_sop_for_crc_hg, hdr_mode;
    XPORT_PORT_RATE xport_speed = XPORT_RATE_UNKNOWN;
    
    if (ag_drv_xport_xlmac_core_mode_get(portid, &speed_mode, &no_sop_for_crc_hg, &hdr_mode))
        return XPORT_RATE_UNKNOWN;
    
    switch (speed_mode)
    {
    case XLMAC_PORT_SPEED_10MB: /* 10M */
        xport_speed = XPORT_RATE_10MB;
        break;

    case XLMAC_PORT_SPEED_100MB: /* 100Mbps */
        xport_speed = XPORT_RATE_100MB;
        break;

    case XLMAC_PORT_SPEED_1000MB: /* 1000Mbps */
        xport_speed = XPORT_RATE_1000MB;
        break;

    case XLMAC_PORT_SPEED_2500MB: /* 2500Mbps */
        xport_speed = XPORT_RATE_2500MB;
        break;

    case XLMAC_PORT_SPEED_10G: /* 10G */
        xport_speed = XPORT_RATE_10G;
        break;

    default:
        xport_speed = XPORT_RATE_UNKNOWN;
    }
    
    return xport_speed;
}


int xport_xlmac_init(xport_init_s *init_params)
{
    xport_xlmac_core_ctrl     ctrl;
    xport_xlmac_core_tx_ctrl  tx_ctrl;
    xport_xlmac_core_rx_ctrl  rx_ctrl;
    uint32_t i;
    int rc = XPORT_ERR_OK;
    int p0_active = 0;

    if (XPORT_INTF_VALID(init_params->prt_mux_sel[0])) p0_active = 1;

    /* XPORT_REG_XPORT_CNTRL_1 */
    rc = rc ? rc : ag_drv_xport_reg_xport_cntrl_1_set(0 /* msbus_clk_sel - never operate both link simulteneously*/,
                                                      0 /* wan_led0_sel - keep default. Need update if design share LED for both wan ports*/, 
                                                      0 /* timeout_rst_disable */, 
                                                      p0_active?1:0 /* p0_mode; 0=GMII, 1=XGMII */);
    for (i = 0; i < XPORT_NUM_OF_PORTS; i++)
    {
        ag_drv_xport_xlmac_core_mode_set(i, 
                                         0x2 /* speed_mode */, 
                                         0   /* no_sop_for_crc_hg */,
                                         0   /* hdr_mode */);

        rc = rc ? rc : ag_drv_xport_xlmac_core_tx_ctrl_get(i, &tx_ctrl);

        tx_ctrl.crc_mode = XLMAC_TX_CTRL_CRC_MODE_PER_PKT /* = 3 (not defined in RDB) */;
        tx_ctrl.pad_en = 1;
        tx_ctrl.tx_threshold = 2; /* TX_THRESHOLD depends on how "fast" XRDP writes into XPORT. 
                                     If any underflow in XLMAC we can increase the value. */
        rc = rc ? rc : ag_drv_xport_xlmac_core_tx_ctrl_set(i, &tx_ctrl);

        ag_drv_xport_xlmac_core_tx_crc_corrupt_ctrl_set(i, 
                                                        0 /*prog_tx_crc*/, 
                                                        0 /*tx_crc_corruption_mode*/, 
                                                        0 /*tx_crc_corrupt_en*/, 
                                                        0 /*tx_err_corrupts_crc*/);

        rc = rc ? rc : ag_drv_xport_xlmac_core_rx_ctrl_get(i, &rx_ctrl);
        rx_ctrl.rx_any_start = 0;
        rx_ctrl.strip_crc = 0; /* CRC will be validated the BBH */
        rx_ctrl.strict_preamble = 0; /* Keep this disabled otherwise interop issue with some bad equipment */
        rx_ctrl.runt_threshold = 0x40;
        rx_ctrl.rx_pass_ctrl = 1;
        rx_ctrl.rx_pass_pause = 1;
        rx_ctrl.rx_pass_pfc = 1;
        rc = rc ? rc : ag_drv_xport_xlmac_core_rx_ctrl_set(i, &rx_ctrl);

        /* Configure the SPEED_MODE in XLMAC_MODE register based on link speed detected */

        rc = ag_drv_xport_xlmac_core_ctrl_get(i, &ctrl);
        ctrl.soft_reset = 0; /* Bring the MAC Port out of reset */
        rc = rc ? rc : ag_drv_xport_xlmac_core_ctrl_set(i, &ctrl);
        /* Keep Timestamp Adjust to HW default */
    }

    return rc;
}

int xport_msbus_init(xport_init_s *init_params)
{
    xport_mab_ctrl ctrl_xlmac;
    uint32_t xlmac_port;
    int rc = XPORT_ERR_OK;

    /* Keep MSBUS TX arbitrater ARB_Mode and weights to HW defaults */

    rc = rc ? rc : ag_drv_xport_mab_ctrl_get(&ctrl_xlmac);
    ctrl_xlmac.link_down_rst_en = 0;
    ctrl_xlmac.xgmii_rx_rst = 0;
    ctrl_xlmac.xgmii_tx_rst = 0;

    for (xlmac_port = 0; xlmac_port < XPORT_NUM_OF_PORTS_PER_XLMAC; xlmac_port++)
    {
        ctrl_xlmac.gmii_rx_rst &= ~(1 << xlmac_port);
        ctrl_xlmac.gmii_tx_rst &= ~(1 << xlmac_port);
    }

    rc = rc ? rc : ag_drv_xport_mab_ctrl_set(&ctrl_xlmac);

    return rc;
}

int xport_reinit_driver(xport_init_s *init_params, int log)
{
    if (validate_xport_configuration(init_params))
    {
        pr_err("XPORT configuration validation failed\n");
        return XPORT_ERR_PARAM;
    }

    //init xlmac
    if (xport_xlmac_init(init_params))
    {
        pr_err("Init XPORT XLMAC failed\n");
        return XPORT_ERR_PARAM;
    }
    if (log)
        pr_err("XLMAC!\n");

    //init msbus
    if (xport_msbus_init(init_params))
    {
        pr_err("Init XPORT MSBUS failed\n");
        return XPORT_ERR_PARAM;
    }
    if (log)
        pr_err("MSBUS!\n");

    /* Serdes is outside of XPORT block */

    /*save shadow copy of init configuration for runtime use*/
    memcpy(&local_xport_cfg, init_params, sizeof(local_xport_cfg));

    return XPORT_ERR_OK;
}

int xport_init_driver(xport_init_s *init_params)
{
    /* ioRemap virtual addresses of XPORT */
    remap_ru_block_addrs(XPORT_IDX, RU_XPORT_BLOCKS);

    /* Serdes insert/remove, PHY/RGMII link up/down interrupts could be registered */

    if (xport_reinit_driver(init_params, 1))
        return XPORT_ERR_PARAM;

    return XPORT_ERR_OK;
}

int xport_get_port_configuration(uint32_t portid, xport_port_cfg_s *port_conf)
{
    int rc;
    xport_xlmac_core_ctrl xlmac_ctrl;
    xport_xlmac_core_tx_ctrl xlmac_tx_ctrl;
    xport_xlmac_core_rx_ctrl xlmac_rx_ctrl;

    rc = ag_drv_xport_xlmac_core_ctrl_get(portid, &xlmac_ctrl);
    rc = rc ? rc : ag_drv_xport_xlmac_core_tx_ctrl_get(portid, &xlmac_tx_ctrl);
    rc = rc ? rc : ag_drv_xport_xlmac_core_rx_ctrl_get(portid, &xlmac_rx_ctrl);
    port_conf->average_igp = xlmac_tx_ctrl.average_ipg;
    port_conf->pad_en = xlmac_tx_ctrl.pad_en;
    port_conf->local_loopback = xlmac_ctrl.local_lpbk;
    port_conf->pad_threashold = xlmac_tx_ctrl.pad_threshold;
    port_conf->tx_preamble_len = xlmac_tx_ctrl.tx_preamble_length;
    port_conf->tx_threshold = xlmac_tx_ctrl.tx_threshold;
    port_conf->speed = xport_speed_get((uint8_t)portid);
    return rc;
}

int xport_set_port_configuration(uint32_t portid, xport_port_cfg_s *port_conf)
{
    int rc = 0;
    xport_xlmac_core_ctrl xlmac_ctrl;
    xport_xlmac_core_tx_ctrl xlmac_tx_ctrl;
    xport_xlmac_core_rx_ctrl xlmac_rx_ctrl;
    uint8_t msbus_clk_sel, wan_led0_sel, timeout_rst_disable, p0_mode;

    rc = rc ? rc : ag_drv_xport_xlmac_core_ctrl_get(portid, &xlmac_ctrl);
    rc = rc ? rc : ag_drv_xport_xlmac_core_tx_ctrl_get(portid, &xlmac_tx_ctrl);
    rc = rc ? rc : ag_drv_xport_xlmac_core_rx_ctrl_get(portid, &xlmac_rx_ctrl);
    rc = rc ? rc : ag_drv_xport_reg_xport_cntrl_1_get(&msbus_clk_sel, &wan_led0_sel, &timeout_rst_disable, &p0_mode);

    xlmac_tx_ctrl.average_ipg = port_conf->average_igp;
    xlmac_tx_ctrl.pad_en = port_conf->pad_en;
    xlmac_ctrl.local_lpbk = port_conf->local_loopback;
    rc = rc ? rc : ag_drv_xport_xlmac_core_mode_set(portid, xport_speed_to_xlmac(port_conf->speed), 0, 0);
    xlmac_tx_ctrl.pad_threshold = port_conf->pad_threashold;
    xlmac_tx_ctrl.tx_preamble_length = port_conf->tx_preamble_len;

    rc = rc ? rc : ag_drv_xport_xlmac_core_ctrl_set(portid, &xlmac_ctrl);
    rc = rc ? rc : ag_drv_xport_xlmac_core_tx_ctrl_set(portid, &xlmac_tx_ctrl);
    rc = rc ? rc : ag_drv_xport_xlmac_core_rx_ctrl_set(portid, &xlmac_rx_ctrl);

    p0_mode = port_conf->speed == XPORT_RATE_10G? 1: 0;
    rc = rc ? rc : ag_drv_xport_reg_xport_cntrl_1_set(msbus_clk_sel, wan_led0_sel, timeout_rst_disable, p0_mode);

    return rc;
}

int xport_get_pause_configuration(uint32_t portid, xport_flow_ctrl_cfg_s *flow_ctrl)
{
    int rc;
    xport_xlmac_core_rx_ctrl xlmac_rx_ctrl;
    xport_xlmac_core_pause_ctrl pause_ctrl;

    rc = ag_drv_xport_xlmac_core_rx_ctrl_get(portid, &xlmac_rx_ctrl);
    rc = rc ? rc : ag_drv_xport_xlmac_core_tx_mac_sa_get(portid, &flow_ctrl->tx_ctrl_sa);
    rc = rc ? rc : ag_drv_xport_xlmac_core_rx_mac_sa_get(portid, &flow_ctrl->rx_ctrl_sa);
    rc = rc ? rc : ag_drv_xport_xlmac_core_pause_ctrl_get(portid, &pause_ctrl);

    flow_ctrl->pause_refresh_en = pause_ctrl.pause_refresh_en;
    flow_ctrl->pause_refresh_timer = pause_ctrl.pause_refresh_timer;
    flow_ctrl->pause_xoff_timer = pause_ctrl.pause_xoff_timer;
    flow_ctrl->rx_pass_ctrl = xlmac_rx_ctrl.rx_pass_ctrl;
    flow_ctrl->rx_pass_pause = xlmac_rx_ctrl.rx_pass_pause;
    flow_ctrl->rx_pause_en = pause_ctrl.rx_pause_en;
    flow_ctrl->tx_pause_en = pause_ctrl.tx_pause_en;

    return rc;
}

int xport_set_pause_configuration(uint32_t portid, xport_flow_ctrl_cfg_s *flow_ctrl)
{
    int rc;
    xport_xlmac_core_rx_ctrl xlmac_rx_ctrl;
    xport_xlmac_core_pause_ctrl pause_ctrl;

    rc = ag_drv_xport_xlmac_core_rx_ctrl_get(portid, &xlmac_rx_ctrl);
    rc = rc ? rc : ag_drv_xport_xlmac_core_tx_mac_sa_set(portid, flow_ctrl->tx_ctrl_sa);
    rc = rc ? rc : ag_drv_xport_xlmac_core_rx_mac_sa_set(portid, flow_ctrl->rx_ctrl_sa);
    rc = rc ? rc : ag_drv_xport_xlmac_core_pause_ctrl_get(portid, &pause_ctrl);

    pause_ctrl.pause_refresh_en = flow_ctrl->pause_refresh_en;
    pause_ctrl.pause_refresh_timer = flow_ctrl->pause_refresh_timer;
    pause_ctrl.pause_xoff_timer = flow_ctrl->pause_xoff_timer;
    xlmac_rx_ctrl.rx_pass_ctrl = flow_ctrl->rx_pass_ctrl;
    xlmac_rx_ctrl.rx_pass_pause = flow_ctrl->rx_pass_pause;
    pause_ctrl.rx_pause_en = flow_ctrl->rx_pause_en;
    pause_ctrl.tx_pause_en = flow_ctrl->tx_pause_en;

    rc = rc ? rc : ag_drv_xport_xlmac_core_rx_ctrl_set(portid, &xlmac_rx_ctrl);
    rc = rc ? rc : ag_drv_xport_xlmac_core_pause_ctrl_set(portid, &pause_ctrl);

    return rc;
}

int xport_get_port_rxtx_enable(uint32_t portid, uint8_t *rx_en, uint8_t *tx_en)
{
    int rc;
    xport_xlmac_core_ctrl xlmac_ctrl;

    rc = ag_drv_xport_xlmac_core_ctrl_get(portid, &xlmac_ctrl);

    *rx_en = xlmac_ctrl.rx_en;
    *tx_en = xlmac_ctrl.tx_en;

    return rc ;
}

int xport_set_port_rxtx_enable(uint32_t portid, uint8_t rx_en, uint8_t tx_en)
{
    int rc;
    xport_xlmac_core_ctrl xlmac_ctrl;

    rc = ag_drv_xport_xlmac_core_ctrl_get(portid, &xlmac_ctrl);

    xlmac_ctrl.rx_en = rx_en;
    xlmac_ctrl.tx_en = tx_en;

    return rc ? rc : ag_drv_xport_xlmac_core_ctrl_set(portid, &xlmac_ctrl);
}

int  xport_get_phyid(uint32_t port, uint16_t *phyid)
{
    pr_err("%s() - Not implemented\n",__FUNCTION__);
    return 0;
}

int xport_reset_phy_cfg(uint32_t port, xport_port_phycfg_s *phycfg)
{
    pr_err("%s() - Not implemented\n",__FUNCTION__);
    return 0;
}

/*This status analysis is for Broadcom's PHYs only as it described in the EGPHY register specification */
int xport_get_brcm_phy_status(uint16_t phyid,xport_port_status_s *port_status)
{
    pr_err("%s() - Not implemented\n",__FUNCTION__);
    return 0;
}

int xport_get_port_status(uint32_t port, xport_port_status_s *port_status)
{
    int rc = XPORT_ERR_OK;
    xport_xlmac_core_ctrl ctrl;
    ag_drv_xport_xlmac_core_ctrl_get(port, &ctrl);
    port_status->mac_lpbk = ctrl.local_lpbk;
    port_status->mac_rx_en = ctrl.rx_en;
    port_status->mac_tx_en = ctrl.tx_en;
    port_status->rate = xport_speed_get(port);
    if (port == 0) /* XGMII 10G Serdes */
    {
        port_status->duplex = XPORT_FULL_DUPLEX;
    }
    else
    {
        xport_reg_crossbar_status crossbar_status;
        ag_drv_xport_reg_crossbar_status_get(&crossbar_status);
        port_status->duplex = crossbar_status.full_duplex ? XPORT_FULL_DUPLEX : XPORT_HALF_DUPLEX;
        port_status->port_up = crossbar_status.link_status;
    }
    {
        xport_xlmac_core_pause_ctrl pause_ctrl;
        ag_drv_xport_xlmac_core_pause_ctrl_get(port,&pause_ctrl);
        port_status->rx_pause_en = pause_ctrl.rx_pause_en;
        port_status->tx_pause_en = pause_ctrl.tx_pause_en;
    }

    return rc;
}

int xport_port_mtu_get(uint32_t portid, uint16_t *port_mtu)
{
    return ag_drv_xport_xlmac_core_rx_max_size_get(portid, port_mtu);
}

int xport_port_mtu_set(uint32_t portid, uint16_t port_mtu)
{
    uint8_t instance = portid % 4;

    /* set the XLMAC MTU */
    ag_drv_xport_xlmac_core_rx_max_size_set(portid, port_mtu);

    /* align the MIB max packet size for accurate accounting */
    switch (instance)
    {
    case 0:
        return ag_drv_xport_mib_reg_gport0_max_pkt_size_set(port_mtu);
    case 1:
        return ag_drv_xport_mib_reg_gport1_max_pkt_size_set(port_mtu);
    case 2:
        return ag_drv_xport_mib_reg_gport2_max_pkt_size_set(port_mtu);
    case 3:
        return ag_drv_xport_mib_reg_gport3_max_pkt_size_set(port_mtu);
    default:
        pr_err("%s(%d):Wrong portid %d",__FUNCTION__, __LINE__, portid);
        return XPORT_ERR_PARAM;
    }
}

int xport_get_port_link_status(uint32_t port, uint8_t *link_up)
{
    pr_err("%s() - Not implemented\n",__FUNCTION__);
    return XPORT_ERR_OK;
}

int xport_port_eee_set(uint32_t portid, uint8_t enable)
{
    pr_err("%s() - Not implemented\n",__FUNCTION__);

    return 0;
}

char *xport_rate_to_str(XPORT_PORT_RATE rate)
{
    switch (rate)
    {
    case XPORT_RATE_10MB: return "10M";
    case XPORT_RATE_100MB: return "100M";
    case XPORT_RATE_1000MB: return "1G";
    case XPORT_RATE_2500MB: return "2.5G";
    case XPORT_RATE_10G: return "10G";
    default: return "Unknown";
    }
    return "Unkown";
}

int xport_str_to_rate(char *str)
{
    if (!strcmp(str, "10M"))
        return XPORT_RATE_10MB;
    if (!strcmp(str, "100M"))
        return XPORT_RATE_100MB;
    if (!strcmp(str, "1G"))
        return XPORT_RATE_1000MB;
    if (!strcmp(str, "2.5G"))
        return XPORT_RATE_2500MB;
    if (!strcmp(str, "10G"))
        return XPORT_RATE_10G;

    return XPORT_RATE_UNKNOWN;
}

