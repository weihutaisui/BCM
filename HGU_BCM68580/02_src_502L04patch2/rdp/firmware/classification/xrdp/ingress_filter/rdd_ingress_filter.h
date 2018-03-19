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

#ifndef _RDD_INGRESS_FILTER_H
#define _RDD_INGRESS_FILTER_H

#include "rdd.h"

#define MAX_NUMBER_OF_INGRESS_FILTERS       32

/* Ingress filter action */
typedef enum
{
    rdd_filter_action_drop    = 0,
    rdd_filter_action_trap    = 1,
} rdd_filter_action;

/* modular firmware */
int rdd_ingress_filter_module_init(const rdd_module_t *module);

int rdd_ingress_filter_vport_to_profile_set(uint8_t vport, uint8_t profile);
void rdd_ingress_filter_profile_cfg(uint8_t profile, uint32_t filter_mask, uint32_t action_mask);
void rdd_ingress_filter_1588_cfg(int enable);
int rdd_ingress_filter_drop_counter_get(uint8_t filter, rdpa_traffic_dir dir, uint16_t *drop_counter);
void rdd_ingress_filter_cpu_bypass_cfg_set(uint8_t profile, bdmf_boolean cpu_bypass);

#endif
