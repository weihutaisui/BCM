/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

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
 *  Created on: Dec 2016
 *      Author: steven.hsieh@broadcom.com
 */

#ifndef __PHY_DRV_SF2_H__
#define __PHY_DRV_SF2_H__

#define REG_WRITE_32(reg, val) do{*(volatile uint32_t *)(reg) = (val);} while(0)
#define REG_READ_32(reg, var) do{(var) = *(volatile uint32_t *)(reg);} while(0)
#define REG_DIR_WRITE_32(reg, val) REG_WRITE_32(bcm_dev_phy2vir(reg), val)
#define REG_DIR_READ_32(reg, var) REG_READ_32(bcm_dev_phy2vir(reg), var)

#include "phy_drv.h"
#include "bcm_map_part.h"
#include "phy_drv_xgae.h"

typedef struct phy_serdes_s {
    int phy_type;
    int used;
    int power_mode;
    int sfp_status;
    int sfp_module_type;
    int cur_power_level;
    int config_speed;
    int current_speed;
    int i2cDetectDelay;
    int i2cInitDetectDelay;
    int link_changed;   /* Flag to create a link down for Kernel to notify speed change */ 
    s16 signal_detect_gpio;
    s16 sfp_module_detect_gpio;
    int signal_detect_invert;
    int speed_caps;
    int highest_speed, highest_speed_cap;
    void (*link_stats)(phy_dev_t *phy_dev);
    int (*serdes_init)(phy_dev_t *phy_dev);
    //void (*ethsw_serdes_speed_detect)(phy_dev_t *phy_dev);
    int (*speed_set)(phy_dev_t *phy_dev, phy_speed_t speed, phy_duplex_t duplex);
#define I2CDetectDelay 8
#define I2CInitDetectDelay 8
} phy_serdes_t;

typedef struct phy_cl45_s {
    int config_speed;
    int duplex;
    void *descriptor;
} phy_cl45_t;

void serdes_work_around(phy_dev_t *phy_dev);
static inline int phy_bus_c45_read32(phy_dev_t *phy_dev, uint32_t reg32, uint16_t *val_p)
{
    return phy_bus_c45_read(phy_dev, ((reg32)>>16)&0xffff, reg32&0xffff, val_p) +
           phy_bus_c45_read(phy_dev, ((reg32)>>16)&0xffff, reg32&0xffff, val_p);
}
#define phy_bus_c45_write32(phy_dev, reg32, val) \
    phy_bus_c45_write(phy_dev, ((reg32)>>16)&0xffff, reg32&0xffff, val)
#define IsC45Phy(phy) (phy->phy_drv->phy_type == PHY_TYPE_EXT3)

int ethsw_phy_exp_rw(phy_dev_t *phy_dev, u32 reg, u16 *v16_p, int rd);
static inline int ethsw_phy_exp_read32(phy_dev_t *phy_dev, u32 reg, u32 *v32_p)
{
    u16 v16; 
    int rc;
    rc = ethsw_phy_exp_rw(phy_dev, reg, &v16, 1); 
    *v32_p = v16;
    return rc;
}

#define ethsw_phy_exp_read(phy_dev, reg, v16_p) ethsw_phy_exp_rw(phy_dev, reg, v16_p, 1)
static inline int ethsw_phy_exp_write(phy_dev_t *phy_dev, u32 reg, u32 v)
{
    u16 v16=v; 
    int rc;
    rc = ethsw_phy_exp_rw(phy_dev, reg, &v16, 0); 
    return rc;
}

int sf2_serdes_phy_read_status(phy_dev_t *phy_dev);
int sf2_serdes_init(phy_dev_t *phy_dev);
int ethsw_serdes_power_mode_get(phy_dev_t *phy_dev, int *mode);
int ethsw_serdes_caps_get(phy_dev_t *phy_dev, uint32_t *caps);
int ethsw_serdes_cfg_speed_set(phy_dev_t *phy_dev, phy_speed_t speed, phy_duplex_t duplex);
int ethsw_serdes_speed_get(phy_dev_t *phy_dev, phy_speed_t *speed, phy_duplex_t *duplex);

#endif
