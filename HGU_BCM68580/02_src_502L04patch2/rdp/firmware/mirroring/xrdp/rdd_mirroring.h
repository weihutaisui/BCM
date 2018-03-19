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

#ifndef _RDD_MIRRORING_H
#define _RDD_MIRRORING_H

#include "rdd.h"
#include "rdd_common.h"
#include "rdpa_types.h"
#include "rdp_platform.h"

#if defined(BCM63158)
#define RDD_MIRRORING_GET_TM_LAN_TASK(lan_port) IMAGE_0_DS_TM_TX_TASK_THREAD_NUMBER
#elif !defined(G9991)
#define RDD_MIRRORING_GET_TM_LAN_TASK(lan_port) (IMAGE_0_DS_TM_LAN0_THREAD_NUMBER + lan_port)
#endif /* G9991 */
#define IS_MIRRORING_CFG(qm_queue) (qm_queue <= QM_QUEUE_LAST + 1) 
typedef struct
{
    bbh_id_e src_tx_bbh_id;
    bdmf_boolean lan;
    uint16_t rx_dst_queue;
    uint16_t tx_dst_queue;
    rdd_vport_id_t  rx_dst_vport;
    rdd_vport_id_t  tx_dst_vport;
} rdd_mirroring_cfg_t;

/* API to RDPA level */
void rdd_mirroring_set(rdd_mirroring_cfg_t *rdd_mirroring_cfg);

/* Init Function */
void rdd_mirroring_cfg_init(void);

#endif
