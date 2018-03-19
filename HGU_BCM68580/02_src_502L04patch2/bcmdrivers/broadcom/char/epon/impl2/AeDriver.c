/*
*  Copyright 2011, Broadcom Corporation
*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :>
*/

//**************************************************************************
// File Name  : AeDriver.c
// This file mainly used to implement AE mode driver.
// Description: Broadcom EPON  Active Ethernet Interface Driver
//               
//**************************************************************************
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <asm/uaccess.h>

#include "EponUser.h"
#include "OntDirector.h"
#include "EponTypes.h"
#include "board.h"

#include "rdpa_api.h"
#include "mac_drv.h"
#include "phy_drv.h"
#include "wan_drv.h"

#if defined (CONFIG_BCM96846)
#undef READ32
#include "shared_utils.h"
#include "drivers_epon_ag.h"
#endif

#include "drv_epon_lif_ag.h"

uint64_t aeStatsLink[EponLinkHwStatCount];
uint64_t aeStatsLinkSwStats[SwStatIdNum];

#define AeTimerMs  (1000)
extern void enet_pon_drv_link_change(int link);
extern void enet_pon_drv_speed_change(phy_speed_t speed, phy_duplex_t duplex);
extern BOOL epon_mac_init_done;
extern phy_drv_t phy_drv_pon;

static uint8_t is_ext_phy = 0;
static uint8_t is_fix_mode = 1;
phy_speed_t laser2phy[LaserRate10G+1] = {PHY_SPEED_UNKNOWN, PHY_SPEED_1000, PHY_SPEED_2500, PHY_SPEED_10000};

void ae_enable_link(void)
{
    /*GBE egress_tm, only channel 0 is enabled*/
#if defined (CONFIG_BCM96858) || defined (CONFIG_BCM96856)
    XifCreateLink(0, 0x5555);
#endif    
    LifCreateLink(0, 0x5555);

    PonMgrStartLinks(1UL << 0);
    PonMgrSetBurstCap(0, NULL);
     
    PonMgrRxDisable();
    PonMgrRxEnable();
    LifLocked();
#if defined (CONFIG_BCM96858) || defined (CONFIG_BCM96856)
    XifLocked();
#endif 
    EponInSync(0);
    EponRcvdGrants();
}
        
static int epon_ae_enable(mac_dev_t *mac_dev)
{
    LifEnableLink(0);
#if defined (CONFIG_BCM96858) || defined (CONFIG_BCM96856)
    XifEnableLink(0);
#endif
    EponUpstreamEnable(1UL << 0);
    
    return 0;
}

static int epon_ae_disable(mac_dev_t *mac_dev)
{
    LifDisableLink(0);
#if defined (CONFIG_BCM96858) || defined (CONFIG_BCM96856)    
    XifDisableLink(0);
#endif
    EponUpstreamDisable(1UL << 0);
    
    return 0;
}

static int epon_ae_cfg_get(mac_dev_t *mac_dev, mac_cfg_t *mac_cfg)
{
    memset(mac_cfg, 0, sizeof(mac_cfg_t));
    
    mac_cfg->duplex = MAC_DUPLEX_FULL;

    switch (PonCfgDbGetUpRate())
    {
        case LaserRate1G:
            mac_cfg->speed = MAC_SPEED_1000;
            break;
        case LaserRate2G:
            mac_cfg->speed = MAC_SPEED_2500;
            break;
        case LaserRate10G:
            mac_cfg->speed = MAC_SPEED_10000;
            break;
        default:
            mac_cfg->speed = MAC_SPEED_UNKNOWN;
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Unknown AE MAC speed\n");
            return -1;
    }
    
    return 0;
}

static void epon_ae_us_speed_set(uint32_t speed_flag)
{
    bdmf_error_t rc = 0;
    bdmf_object_handle port_obj = NULL;
    rdpa_if port = rdpa_wan_type_to_if(rdpa_wan_epon);
    rdpa_speed_type speed;
    
    rc = rdpa_port_get(port, &port_obj);
    if (rc)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_AE, "Failed to get WAN port. rc=%d\n", rc);
        return;
    }
    
    speed = speed_flag ? rdpa_speed_1g : rdpa_speed_10g;
    rdpa_port_speed_set(port_obj, speed);
    
    bdmf_put(port_obj);
    return;
}

static void epon_ae_us_enable_set(bdmf_boolean enable)
{

    bdmf_error_t rc = 0;
    bdmf_object_handle port_obj = NULL;
    rdpa_if port = rdpa_wan_type_to_if(rdpa_wan_epon);
    rdpa_port_tm_cfg_t port_tm_cfg;

    rc = rdpa_port_get(port, &port_obj);
    if (rc)  
    {
        BCM_LOG_ERROR(BCM_LOG_ID_AE, "Failed to get WAN port. rc=%d\n", rc);
        return;
    }
    
    rc = rdpa_port_tm_cfg_get(port_obj, &port_tm_cfg);
    if (rc)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_AE, "Failed to get RDPA egress tm for WAN port. rc=%d\n", rc);
        bdmf_put(port_obj);
        return;
    }

    if (port_tm_cfg.sched)
        rdpa_egress_tm_enable_set(port_tm_cfg.sched, enable);
    
    bdmf_put(port_obj);
    udelay(1000);
}

static int ae_switching = 0;

static int epon_ae_cfg_set(mac_dev_t *mac_dev, mac_cfg_t *mac_cfg)
{
    char buf[PSP_BUFLEN_16];
    mac_speed_t speed = MAC_SPEED_UNKNOWN;
    serdes_wan_type_t serdes_wan_type;
    int rc = 0;
    
    switch (PonCfgDbGetUpRate())
    {
        case LaserRate1G:
            speed = MAC_SPEED_1000;
            break;
        case LaserRate2G:
            speed = MAC_SPEED_2500;
            break;
        case LaserRate10G:
            speed = MAC_SPEED_10000;
            break;
        default:
            break;
    }
    if ((speed == mac_cfg->speed) || is_fix_mode)
        return 0;

    ae_switching = 1;
    switch (mac_cfg->speed)
    {
        case MAC_SPEED_1000:
            memcpy(buf, "0101", 4);
            serdes_wan_type = SERDES_WAN_TYPE_AE;
            break;
        case MAC_SPEED_2500:
            memcpy(buf, "2525", 4);
            serdes_wan_type = SERDES_WAN_TYPE_AE_2_5G;
            break;
#if defined (CONFIG_BCM96858) || defined (CONFIG_BCM96856)   
        case MAC_SPEED_10000:
            memcpy(buf, "1010", 4);
            serdes_wan_type = SERDES_WAN_TYPE_AE_10G;
            break;
#endif     
        default:
            BCM_LOG_ERROR(BCM_LOG_ID_AE, "Speed can't be supported by AE MAC\n");
            return -1;
    }
    
    epon_ae_us_enable_set(0);
    PonMgrStopLinks(1UL << 0);
    epon_ae_us_speed_set(mac_cfg->speed != MAC_SPEED_10000);
    
    rc = wan_serdes_config(serdes_wan_type);
    if (rc)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_AE, "WAN serdes reconfig fails errno %d\n", rc);
        return rc;
    }

    buf[4] = '\0';
    kerSysScratchPadSet(RDPA_WAN_RATE_PSP_KEY, buf, 4);
    PonCfgDbInitPon();
    EponTopInit();
    PonMgrInit();
    OntDirInit();
    ae_enable_link();
    LifLaserTxModeSet(rdpa_epon_laser_tx_continuous);
    epon_ae_us_enable_set(1);
    
    memcpy(buf, "0000", 4);
    kerSysScratchPadSet(RDPA_WAN_RATE_PSP_KEY, buf, 4);
    ae_switching = 0;
    
    return 0;
}

static int epon_ae_stats_get(mac_dev_t *mac_dev, mac_stats_t *mac_stats)
{
    uint8_t stat;
    uint64_t dbuf;

    memset(mac_stats, 0, sizeof(mac_stats_t));

    for (stat = EponBiDnTotalBytesRx; stat <= EponBiDnUndersizedFramesRx; ++stat)
    {
        EponReadLinkStat(0, (EponLinkStatId)stat, &dbuf, FALSE);
        if ((stat >= EponBiDnBroadcastFramesRx) && (stat <= EponBiDnUnicastFramesRx))
        {
            U64AddEqU64(&aeStatsLinkSwStats[SwStatIdRxTotalFrames],&dbuf);
        }
        U64AddEqU64(&aeStatsLink[stat],&dbuf);
    }

    for (stat = EponBiUpTotalBytesTx; stat <= EponBiUpUnicastFramesTx; ++stat)
    {
        EponReadLinkStat(0, (EponLinkStatId)stat, &dbuf, FALSE);
        if ((stat >= EponBiUpBroadcastFramesTx) && (stat <= EponBiUpUnicastFramesTx))
        {
            U64AddEqU64(&aeStatsLinkSwStats[SwStatIdTxTotalFrames],&dbuf);
        }
        U64AddEqU64(&aeStatsLink[stat],&dbuf);
    }
    
    mac_stats->rx_byte = aeStatsLink[EponBiDnTotalBytesRx];
    mac_stats->rx_packet = aeStatsLinkSwStats[SwStatIdRxTotalFrames];
    mac_stats->rx_frame_64 = aeStatsLink[EponBiDn64ByteFramesRx];
    mac_stats->rx_frame_65_127 = aeStatsLink[EponBiDn65to127ByteFramesRx];
    mac_stats->rx_frame_128_255 = aeStatsLink[EponBiDn128to255ByteFramesRx];
    mac_stats->rx_frame_256_511 = aeStatsLink[EponBiDn256to511ByteFramesRx];
    mac_stats->rx_frame_512_1023 = aeStatsLink[EponBiDn512to1023ByteFramesRx];
    mac_stats->rx_frame_1024_1518 = aeStatsLink[EponBiDn1024to1518ByteFramesRx];
    mac_stats->rx_frame_1519_mtu = aeStatsLink[EponBiDn1518PlusByteFramesRx];
    mac_stats->rx_multicast_packet = aeStatsLink[EponBiDnMulticastFramesRx];
    mac_stats->rx_broadcast_packet = aeStatsLink[EponBiDnBroadcastFramesRx];
    mac_stats->rx_unicast_packet = aeStatsLink[EponBiDnUnicastFramesRx];
    mac_stats->rx_fcs_error = aeStatsLink[EponBiDnFcsErrors];
    mac_stats->rx_undersize_packet = aeStatsLink[EponBiDnUndersizedFramesRx];
    mac_stats->rx_oversize_packet = aeStatsLink[EponBiDnOversizedFramesRx];
    mac_stats->tx_byte = aeStatsLink[EponBiUpTotalBytesTx];
    mac_stats->tx_packet = aeStatsLinkSwStats[SwStatIdTxTotalFrames];
    mac_stats->tx_frame_64 = aeStatsLink[EponBiUp64ByteFramesTx];
    mac_stats->tx_frame_65_127 = aeStatsLink[EponBiUp65To127ByteFramesTx];
    mac_stats->tx_frame_128_255 = aeStatsLink[EponBiUp128To255ByteFramesTx];
    mac_stats->tx_frame_256_511 = aeStatsLink[EponBiUp256To511ByteFramesTx];
    mac_stats->tx_frame_512_1023 = aeStatsLink[EponBiUp512To1023ByteFamesTx];
    mac_stats->tx_frame_1024_1518 = aeStatsLink[EponBiUp1024To1518ByteFramesTx];
    mac_stats->tx_frame_1519_mtu = aeStatsLink[EponBiUp1518PlusByteFramesTx];
    mac_stats->tx_multicast_packet = aeStatsLink[EponBiUpMulticastFramesTx];
    mac_stats->tx_broadcast_packet = aeStatsLink[EponBiUpBroadcastFramesTx];
    mac_stats->tx_unicast_packet = aeStatsLink[EponBiUpUnicastFramesTx];
    
    return 0;
}

static int epon_ae_stats_clear(mac_dev_t *mac_dev)
{
    memset(aeStatsLink, 0, sizeof(aeStatsLink));
    memset(aeStatsLinkSwStats, 0, sizeof(aeStatsLinkSwStats));

    return 0;
}

#define AUTONEG_COMPLETE_COUNT  1000

#if defined (CONFIG_BCM96856)
int ae_int_phy_autoneg_restart(phy_dev_t *phy_dev, uint32_t caps)
{
    uint32_t autoneg_countdown = AUTONEG_COMPLETE_COUNT;
    uint16_t cf_autoneg_linktimer;
    uint8_t cf_autoneg_mode_sel, cf_autoneg_restart, cf_autoneg_en;
    uint8_t an_lp_remote_fault, an_sync_status, an_complete;
    
    ag_drv_lif_p2p_autoneg_control_get(&cf_autoneg_linktimer, &cf_autoneg_mode_sel,
                                                    &cf_autoneg_restart, &cf_autoneg_en);
    if (!cf_autoneg_en)
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Autoneg disabled\n");
        return -1;
    }
    
    do 
    {
        ag_drv_lif_p2p_autoneg_status_get(&an_lp_remote_fault, &an_sync_status, &an_complete);
        autoneg_countdown--;
    }while (!an_complete && autoneg_countdown);
    
    if (an_complete)
    {
        if (an_sync_status)
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Autoneg sucessfully\n");
        else
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Autoneg failed\n");
    }
        
    return 0;
}

void ae_int_phy_caps_set_hw(phy_dev_t *phy_dev, uint32_t caps)
{
    uint16_t v0;
    uint8_t  v1, v2, cf_autoneg_en;
    
    if (PonCfgDbGetUpRate() != LaserRate1G)
        return;
    
    ag_drv_lif_p2p_autoneg_control_get(&v0, &v1, &v2, &cf_autoneg_en);
    cf_autoneg_en = caps & PHY_CAP_AUTONEG;
    ag_drv_lif_p2p_autoneg_control_set(v0, v1, v2, cf_autoneg_en);
    
    if (cf_autoneg_en)
        ae_int_phy_autoneg_restart(phy_dev, caps);
}

void ae_int_phy_caps_get_hw(phy_dev_t *phy_dev, uint32_t *caps)
{
    uint16_t v0;
    uint8_t  v1, v2, cf_autoneg_en;
    
    if (PonCfgDbGetUpRate() != LaserRate1G)
        return;
    
    ag_drv_lif_p2p_autoneg_control_get(&v0, &v1, &v2, &cf_autoneg_en);
    if (cf_autoneg_en)
        *caps |= PHY_CAP_AUTONEG;
}
#elif defined (CONFIG_BCM96846)
int ae_int_phy_autoneg_restart(phy_dev_t *phy_dev, uint32_t caps)
{
    uint8_t rxsyncsqq, rxcorcs, fecrxoutcs;
    uint32_t debug_fec_sm = 0;
    uint32_t autoneg_countdown = AUTONEG_COMPLETE_COUNT;

    RU_REG_READ(0, LIF, DEBUG_FEC_SM, debug_fec_sm);
    rxsyncsqq = RU_FIELD_GET(0, LIF, DEBUG_FEC_SM, RXSYNCSQQ, debug_fec_sm);
    if (!(rxsyncsqq & 0x1))
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Autoneg disabled\n");
        return -1;
    }

    rxcorcs = RU_FIELD_GET(0, LIF, DEBUG_FEC_SM, RXCORCS, debug_fec_sm);
    /* restart autoneg */
    rxcorcs |= 0x1;
    debug_fec_sm = RU_FIELD_SET(0, LIF, DEBUG_FEC_SM, RXCORCS, debug_fec_sm, rxcorcs);
    RU_REG_WRITE(0, LIF, DEBUG_FEC_SM, debug_fec_sm);
    
    do 
    {
        ag_drv_lif_debug_fec_sm_get(&rxsyncsqq, &rxcorcs, &fecrxoutcs);
        autoneg_countdown--;
    }while (!(fecrxoutcs & 0x1) && autoneg_countdown);
    
    if (fecrxoutcs & 0x1)
    {
        if (fecrxoutcs & 0x2)
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Autoneg sucessfully\n");
        else
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Autoneg failed\n");
    }
    
    return 0;
}

void ae_int_phy_caps_set_hw(phy_dev_t *phy_dev, uint32_t caps)
{
    uint8_t rxsyncsqq;
    uint32_t debug_fec_sm = 0;
             
    if (UtilGetChipRev() == 0xA0)
        return;
    
    RU_REG_READ(0, LIF, DEBUG_FEC_SM, debug_fec_sm);
    rxsyncsqq = RU_FIELD_GET(0, LIF, DEBUG_FEC_SM, RXSYNCSQQ, debug_fec_sm);
    if (caps & PHY_CAP_AUTONEG)
        rxsyncsqq |= 0x1;
    else
        rxsyncsqq &= ~0x1;
    debug_fec_sm = RU_FIELD_SET(0, LIF, DEBUG_FEC_SM, RXSYNCSQQ, debug_fec_sm, rxsyncsqq);
    RU_REG_WRITE(0, LIF, DEBUG_FEC_SM, debug_fec_sm);
    
    if (caps & PHY_CAP_AUTONEG)
        ae_int_phy_autoneg_restart(phy_dev, caps);
}

void ae_int_phy_caps_get_hw(phy_dev_t *phy_dev, uint32_t *caps)
{
    uint8_t rxsyncsqq, v0, v1;
    
    if (UtilGetChipRev() == 0xA0)
        return;
    
    ag_drv_lif_debug_fec_sm_get(&rxsyncsqq, &v0, &v1);
    if (rxsyncsqq & 0x1)
        *caps |= PHY_CAP_AUTONEG;
}
#else
#define ae_int_phy_caps_set_hw(...)
#define ae_int_phy_caps_get_hw(...)
#endif

int ae_int_phy_caps_set(phy_dev_t *phy_dev, uint32_t caps)
{
   LaserRate rate = PonCfgDbGetUpRate();
    
    switch (rate)
    {
        case LaserRate1G:   /* Only 1G mode support autoneg*/
            if (caps & ~(PHY_CAP_AUTONEG|PHY_CAP_1000_FULL))
            {
                BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Ignore unsupported autoneg capabilities\n");
            }
            ae_int_phy_caps_set_hw(phy_dev, caps);
            break;
        default:
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Ignore autoneg capabilities\n");
            break;
    }
    
    return 0;
}

int ae_int_phy_caps_get(phy_dev_t *phy_dev, uint32_t *caps)
{
    LaserRate rate = PonCfgDbGetUpRate();
    
    *caps = 0;
    switch (rate)
    {
        case LaserRate10G:
            *caps |= PHY_CAP_10000;
            break;
        case LaserRate2G:
            *caps |= PHY_CAP_2500;
            break;
        case LaserRate1G:   /* Only 1G mode support autoneg*/
            ae_int_phy_caps_get_hw(phy_dev, caps);
            *caps |= PHY_CAP_1000_FULL;
            break;
        default:
            break;
    }
    
    return 0;
}

int ae_int_phy_speed_set(phy_dev_t *phy_dev, phy_speed_t speed, phy_duplex_t duplex)
{
    mac_cfg_t mac_cfg;
    
    switch(speed)
    {
        case PHY_SPEED_1000:
            mac_cfg.speed = MAC_SPEED_1000;
            ae_int_phy_caps_set_hw(phy_dev, 0);
            break;
        case PHY_SPEED_2500:
            mac_cfg.speed = MAC_SPEED_2500;
            break;
        case PHY_SPEED_10000:
            mac_cfg.speed = MAC_SPEED_10000;
            break;
        case PHY_SPEED_UNKNOWN:
            ae_int_phy_caps_set_hw(phy_dev, PHY_CAP_AUTONEG);
            return 0;
        default:
            BCM_LOG_ERROR(BCM_LOG_ID_AE, "Speed can't be supported by AE internal phy\n");
            return -1;
    }
    return epon_ae_cfg_set(NULL, &mac_cfg);
}

void AeHandle1sTimer (EponTimerModuleId moduleId)
{
    int up = 0;
    static int up_prev = 0;
    LaserRate rate = PonCfgDbGetUpRate();

    /* Nothing need to do when ae_switching becomes true*/
    if (ae_switching == 1)
        return;

    switch (rate)
    {
#if defined (CONFIG_BCM96858) || defined (CONFIG_BCM96856)
        case LaserRate10G:
            up = XifLocked();
            break;
#endif
        case LaserRate1G:
        case LaserRate2G:
            up = LifLocked();
            break;
        default:
            return;
    }
                
    /*External PHY dev*/
    if (is_ext_phy)
    {
        if (!up)
        {
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "AE MAC get out of sync\n");
        }else if (up && !up_prev)
        {
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "AE MAC get locked\n");
        }
    }else
    {
        /*Internal serdes PHY*/
        if (up != up_prev)
        {
            if (up)
            {
                enet_pon_drv_speed_change(laser2phy[rate],PHY_DUPLEX_FULL);         
            }
            enet_pon_drv_link_change(up);
        }
    }
    up_prev = up;
}


static int epon_ae_init(mac_dev_t *mac_dev)
{
    EponUsrModuleTimerRegister(EPON_TIMER_TO1,
        AeTimerMs,AeHandle1sTimer);
    
    is_ext_phy = (mac_dev->priv == NULL);
    if (!is_ext_phy)
    {
        phy_drv_pon.caps_set = ae_int_phy_caps_set;
        phy_drv_pon.caps_get = ae_int_phy_caps_get;
        phy_drv_pon.speed_set = ae_int_phy_speed_set;
    }
    return 0;
}

static int epon_ae_drv_init(mac_drv_t *mac_drv)
{
    memset(aeStatsLink, 0, sizeof(aeStatsLink));
    memset(aeStatsLinkSwStats, 0, sizeof(aeStatsLinkSwStats));

    mac_drv->initialized = 1;
    
    return 0;
}

static int epon_ae_mtu_set(mac_dev_t *mac_dev, int mtu)
{
    PonMgrMaxFrameSizeSet(mtu);

    return 0;
}

mac_drv_t mac_drv_epon_ae=
{
    .mac_type = MAC_TYPE_EPON_AE,
    .name = "EPONAE",
    .init = epon_ae_init,
    .enable = epon_ae_enable,
    .disable = epon_ae_disable,
    .cfg_get = epon_ae_cfg_get,
    .cfg_set = epon_ae_cfg_set,
    .stats_get = epon_ae_stats_get,
    .stats_clear = epon_ae_stats_clear,
    .mtu_set = epon_ae_mtu_set,
    .drv_init = epon_ae_drv_init,
};

#define PROC_CMD_MAX_LEN 64

static ssize_t proc_set_ae_mode(struct file *file, const char *buff, size_t len, loff_t *offset)
{
    char input[PROC_CMD_MAX_LEN];
    uint32_t val;
    int ret=0;
    
    if (len > PROC_CMD_MAX_LEN)
        len = PROC_CMD_MAX_LEN;
    
    if (copy_from_user(input, buff, len) != 0)
        return -EFAULT;
    
    ret = sscanf(input, "%d", &val);
    if (ret >= 1)
    {
        mac_cfg_t mac_cfg;
        if (val == 1)
        {
            mac_cfg.speed = MAC_SPEED_1000;
        }
        else if(val == 25)
        {
            mac_cfg.speed = MAC_SPEED_2500;
        }
        else
        {
            mac_cfg.speed = MAC_SPEED_10000;
        }
        epon_ae_cfg_set(NULL, &mac_cfg);
    }
    else
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_AE, "Error format!");
        return -EFAULT;
    }
    
    return len;
};

static struct file_operations set_ae_mode_proc =
{
    .write = proc_set_ae_mode,
};

void AeMacInit(void)
{
    EponUserTimerReset();

    PonMgrInit();
    OntDirInit();
    epon_mac_init_done = TRUE;
}

void EponAeDriverInit()
{
    int rc = 0, count = 0;
    unsigned short tx_gpio;
    BDMF_MATTR(gem_attrs, rdpa_gem_drv());
    rdpa_gem_flow_ds_cfg_t ds_cfg = {};
    bdmf_object_handle gem;
    struct proc_dir_entry *p0, *p1;
    char buf[PSP_BUFLEN_16];

    EponDriverInit();
    EponTopInit();
    DelayMs(10);
    AeMacInit();
    ae_enable_link();
    rc = rdpa_gem_index_set(gem_attrs, 0);
    ds_cfg.discard_prty = rdpa_discard_prty_low;
    ds_cfg.destination = rdpa_flow_dest_eth;
    rc = rc ? rc : rdpa_gem_ds_cfg_set(gem_attrs, &ds_cfg);        
    rc = bdmf_new_and_set(rdpa_gem_drv(), NULL, gem_attrs, &gem);
    if (rc < 0)
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_AE,  "Failed to create downstream runner port\n");
    }
    OntDirLaserTxModeSet(rdpa_epon_laser_tx_continuous);
    OntDirLaserRxPowerSet(1);
    rc = BpGetPonTxEnGpio(&tx_gpio);
    if (rc >= 0)
    {
        kerSysSetGpioState(tx_gpio, kGpioActive);
    }
    
    count = kerSysScratchPadGet(RDPA_WAN_RATE_PSP_KEY, buf, sizeof(buf));
    if (count > 0)
    {
        if (memcmp(buf ,"0000", 4))
        {
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "AE MAC working at fix mode\n");
        }
        else
        {
            is_fix_mode = 0; 
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "AE MAC working at auto mode\n");
        }
    }
    mac_driver_set(&mac_drv_epon_ae);
    
    p0 = proc_mkdir("aemode", NULL);
    if (!p0)
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_AE, "failed to create /proc/aemode !");
    }
    else
    {
        p1 = proc_create("switch", S_IWUSR | S_IRUSR, p0, &set_ae_mode_proc);
        if (!p1)
        {
            BCM_LOG_NOTICE(BCM_LOG_ID_AE, "failed to create /proc/aemode/switch !");
        }
    }
}

