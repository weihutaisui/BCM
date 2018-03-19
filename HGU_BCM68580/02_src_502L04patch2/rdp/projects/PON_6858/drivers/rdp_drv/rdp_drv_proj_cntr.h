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
#ifndef _RDP_DRV_PROJ_CNTR_H_
#define _RDP_DRV_PROJ_CNTR_H_

#include "rdp_drv_cntr.h"
#include "rdd_runner_proj_defs.h"
#include "XRDP_AG.h"

typedef enum
{
    RX_FLOW_CNTR_GROUP_ID,
    TX_FLOW_CNTR_GROUP_ID,
    TCAM_IPTV_DEF_CNTR_GROUP_ID,
    VARIOUS_CNTR_GROUP_ID,
    GENERAL_CNTR_GROUP_ID,
    US_TX_QUEUE_CNTR_GROUP_ID,
    DS_TX_QUEUE_CNTR_GROUP_ID,
    CPU_RX_CNTR_GROUP_ID,
    CPU_RX_METER_DROP_CNTR_GROUP_ID,
    POLICER_CNTR_GROUP_ID,
#ifdef G9991
    PORT_MCST_BCST_GROUP_ID,
#endif
    CNTR_GROUPS_NUMBER,
} cntr_group_id_t;

typedef enum
{
    NONE_CNTR_SUB_GROUP_ID,
    TCAM_CNTR_SUB_GROUP_ID,
    IPTV_CNTR_SUB_GROUP_ID,
    DEF_FLOW_CNTR_SUB_GROUP_ID,
} cntr_sub_group_id_t;


/****************************************************************************** 
  * 
  * CNTR GROUP 0 :
  * --------------
  * Structure: 4B+4B+2B
  *  
  * Usage: 
  *   Counter   | amount
  *   -------------------
  *   RX GEM    | 128
  *   RX vport  | 40
  *
  *  
*******************************************************************************/
#define CNTR0_CNTR_NUM  168
#define RX_FLOW_COUNTERS_NUM CNTR0_CNTR_NUM
#define RX_FLOW_CNTR_GROUP_INVLID_CNTR (CNTR0_CNTR_NUM-1)

/* CNPL GROUP 0: 4B + 4B (valid bytes, valid packets) */
#define CNTR0_CNPL0_BASE_ADDR 0
#define CNTR0_CNPL0_CNTR_SIZE 2
#define CNTR0_CNPL0_CNTR_TYPE 1

/* CNPL GROUP 1: 2B (drop packets) */
#define CNTR0_CNPL1_BASE_ADDR (CNTR0_CNPL0_BASE_ADDR + (((CNTR0_CNTR_NUM * (1 << CNTR0_CNPL0_CNTR_SIZE) * (1 << CNTR0_CNPL0_CNTR_TYPE)) + 7) >> 3))
#define CNTR0_CNPL1_CNTR_SIZE 1
#define CNTR0_CNPL1_CNTR_TYPE 0

/****************************************************************************** 
  * 
  * CNTR GROUP 1 :
  * --------------
  * Structure: 4B+4B
  *  
  * Usage: 
  *   Counter   | amount
  *   -------------------
  *   TX GEM    | 128
  *   TX vport  | 40
  *  
  *  
*******************************************************************************/
#define CNTR1_CNTR_NUM  168
#define TX_FLOW_COUNTERS_NUM CNTR1_CNTR_NUM
#define TX_FLOW_CNTR_GROUP_INVLID_CNTR (CNTR1_CNTR_NUM-1)

/* CNPL GROUP 2: 4B + 4B (valid bytes, valid packets) */
#define CNTR1_CNPL2_BASE_ADDR (CNTR0_CNPL1_BASE_ADDR + (((CNTR0_CNTR_NUM * (1 << CNTR0_CNPL1_CNTR_SIZE) * (1 << CNTR0_CNPL1_CNTR_TYPE)) + 7) >> 3))
#define CNTR1_CNPL2_CNTR_SIZE 2
#define CNTR1_CNPL2_CNTR_TYPE 1

/****************************************************************************** 
  * 
  * CNTR GROUP 2 :
  * --------------
  * Structure: 4B
  *  
  * Usage: 
  *   Counter   | amount
  *   -------------------
  *   TCAM /    | 
  *   IPTV /    |  1040 
  *   DEF flow  | 
  *  
*******************************************************************************/
#define CNTR2_CNTR_NUM  1040
#define TCAM_IPTV_DEF_CNTR_GROUP_INVLID_CNTR (CNTR2_CNTR_NUM-1)

/* CNPL GROUP 3: 4B (valid packets) */
#define CNTR2_CNPL3_BASE_ADDR (CNTR1_CNPL2_BASE_ADDR + (((CNTR1_CNTR_NUM * (1 << CNTR1_CNPL2_CNTR_SIZE) * (1 << CNTR1_CNPL2_CNTR_TYPE)) + 7) >> 3))
#define CNTR2_CNPL3_CNTR_SIZE 2
#define CNTR2_CNPL3_CNTR_TYPE 0

/****************************************************************************** 
  * 
  * CNTR GROUP 3 :
  * --------------
  * Structure: 2B
  *  
  * Usage: 
  *   Counter   | amount
  *   -------------------
  *   VARIOUS   | 256
  *  
*******************************************************************************/
#define CNTR3_CNTR_NUM  256
#define VARIOUS_CNTR_GROUP_INVLID_CNTR (CNTR3_CNTR_NUM-1)

/* CNPL GROUP 4: 2B (packets) */
#define CNTR3_CNPL4_BASE_ADDR (CNTR2_CNPL3_BASE_ADDR + (((CNTR2_CNTR_NUM * (1 << CNTR2_CNPL3_CNTR_SIZE) * (1 << CNTR2_CNPL3_CNTR_TYPE)) + 7) >> 3))
#define CNTR3_CNPL4_CNTR_SIZE 1
#define CNTR3_CNPL4_CNTR_TYPE 0

/****************************************************************************** 
  * 
  * CNTR GROUP 4 :
  * --------------
  * Structure: 4B
  *  
  * Usage: 
  *   Counter   | amount
  *   -------------------
  *   GENERAL   | 11
  *  
*******************************************************************************/
#define CNTR4_CNTR_NUM  11
#define GENERAL_CNTR_GROUP_INVLID_CNTR (CNTR4_CNTR_NUM-1)

/* CNPL GROUP 5: 4B (packets) */
#define CNTR4_CNPL5_BASE_ADDR (CNTR3_CNPL4_BASE_ADDR + (((CNTR3_CNTR_NUM * (1 << CNTR3_CNPL4_CNTR_SIZE) * (1 << CNTR3_CNPL4_CNTR_TYPE)) + 7) >> 3))
#define CNTR4_CNPL5_CNTR_SIZE 2
#define CNTR4_CNPL5_CNTR_TYPE 0

/****************************************************************************** 
  * 
  * CNTR GROUP 5 :
  * --------------
  * Structure: 4B+4B
  *  
  * Usage: 
  *   Counter   | amount
  *   -------------------
  *   TX QUEUE  | 128
  *  
  *  
*******************************************************************************/
#ifndef G9991

#define CNTR5_CNTR_NUM  130
#else
#define CNTR5_CNTR_NUM  96
#endif
#define US_TX_QUEUE_CNTR_GROUP_INVLID_CNTR (CNTR5_CNTR_NUM-1)

/* CNPL GROUP 6: 4B + 4B (valid bytes, valid packets) */
#define CNTR5_CNPL6_BASE_ADDR (CNTR4_CNPL5_BASE_ADDR + (((CNTR4_CNTR_NUM * (1 << CNTR4_CNPL5_CNTR_SIZE) * (1 << CNTR4_CNPL5_CNTR_TYPE)) + 7) >> 3))
#define CNTR5_CNPL6_CNTR_SIZE 2
#define CNTR5_CNPL6_CNTR_TYPE 1

/****************************************************************************** 
  * 
  * CNTR GROUP 6 :
  * --------------
  * Structure: 4B+4B
  *  
  * Usage: 
  *   Counter   | amount
  *   -------------------
  *   TX QUEUE  | 56
  *  
  *  
*******************************************************************************/
#ifndef G9991
#define CNTR6_CNTR_NUM  56
#else
#define CNTR6_CNTR_NUM  160
#endif
#define DS_TX_QUEUE_CNTR_GROUP_INVLID_CNTR (CNTR6_CNTR_NUM-1)

/* CNPL GROUP 7: 4B + 4B (valid bytes, valid packets) */
#define CNTR6_CNPL7_BASE_ADDR (CNTR5_CNPL6_BASE_ADDR + (((CNTR5_CNTR_NUM * (1 << CNTR5_CNPL6_CNTR_SIZE) * (1 << CNTR5_CNPL6_CNTR_TYPE)) + 7) >> 3))
#define CNTR6_CNPL7_CNTR_SIZE 2
#define CNTR6_CNPL7_CNTR_TYPE 1

/****************************************************************************** 
  * 
  * CNTR GROUP 7 :
  * --------------
  * Structure: 2B+2B
  *  
  * Usage: CPU RING , DHD CPU RING
  *  
*******************************************************************************/
#define CNTR7_CNTR_NUM  10
#define CPU_RX_CNTR_GROUP_INVLID_CNTR (CNTR7_CNTR_NUM-1)

/* CNPL GROUP 8: 2B (single counter for lock) */
#define CNTR7_CNPL8_BASE_ADDR (CNTR6_CNPL7_BASE_ADDR + (((CNTR6_CNTR_NUM * (1 << CNTR6_CNPL7_CNTR_SIZE) * (1 << CNTR6_CNPL7_CNTR_TYPE)) + 7) >> 3))
#define CNTR7_CNPL8_CNTR_SIZE 1
#define CNTR7_CNPL8_CNTR_TYPE 1

#define GET_FEED_RING_READ_PTR (*((volatile uint32_t*)DEVICE_ADDRESS(RU_BLK(CNPL).addr[0] +RU_REG_OFFSET(CNPL, MEMORY_DATA) + (CNTR7_CNPL8_BASE_ADDR << 3) + \
                                (1 << CNTR7_CNPL8_CNTR_SIZE) * (1 << CNTR7_CNPL8_CNTR_TYPE) * COUNTER_CPU_GROUP_READ_PTR_FEED)) >> 16)

/****************************************************************************** 
  * 
  * CNTR GROUP 8 :
  * --------------
  * Structure: 2B
  *  
  * Usage: CPU RX METERS DROPS 
  *   Counter   | amount
  *   -------------------
  *   DS DROP   | 16
  *   US DROP   | 16
  *  
*******************************************************************************/
#define CNTR8_CNTR_NUM  32
#define CPU_RX_METER_DROP_CNTR_GROUP_INVLID_CNTR (CNTR8_CNTR_NUM-1)

/* CNPL GROUP 9: 2B (packets) */
#define CNTR8_CNPL9_BASE_ADDR (CNTR7_CNPL8_BASE_ADDR + (((CNTR7_CNTR_NUM * (1 << CNTR7_CNPL8_CNTR_SIZE) * (1 << CNTR7_CNPL8_CNTR_TYPE)) + 7) >> 3))
#define CNTR8_CNPL9_CNTR_SIZE 1
#define CNTR8_CNPL9_CNTR_TYPE 0

/****************************************************************************** 
  * 
  * CNTR GROUP 9 :
  * --------------
  * Structure: (4B+4B) green
  *            (4B+4B) yellow
  *            (2B+2B) red
  * Usage: 
  *   Counter   | amount
  *   -------------------
  *   color     | 80
  *  
  *  
*******************************************************************************/
#define CNTR9_CNTR_NUM  80
#define POLICER_CNTR_GROUP_INVLID_CNTR (CNTR9_CNTR_NUM-1)

/* CNPL GROUP 10: 4B + 4B (valid bytes, valid packets) */
#define CNTR9_CNPL10_BASE_ADDR (CNTR8_CNPL9_BASE_ADDR + (((CNTR8_CNTR_NUM * (1 << CNTR8_CNPL9_CNTR_SIZE) * (1 << CNTR8_CNPL9_CNTR_TYPE)) + 7) >> 3))
#define CNTR9_CNPL10_CNTR_SIZE 2
#define CNTR9_CNPL10_CNTR_TYPE 1

/* CNPL GROUP 11: 4B + 4B (valid bytes, valid packets) */
#define CNTR9_CNPL11_BASE_ADDR (CNTR9_CNPL10_BASE_ADDR + (((CNTR9_CNTR_NUM * (1 << CNTR9_CNPL10_CNTR_SIZE) * (1 << CNTR9_CNPL10_CNTR_TYPE)) + 7) >> 3))
#define CNTR9_CNPL11_CNTR_SIZE 2
#define CNTR9_CNPL11_CNTR_TYPE 1

/* CNPL GROUP 12: 2B + 2B (drop packets. drop bytes) */
#define CNTR9_CNPL12_BASE_ADDR (CNTR9_CNPL11_BASE_ADDR + (((CNTR9_CNTR_NUM * (1 << CNTR9_CNPL11_CNTR_SIZE) * (1 << CNTR9_CNPL11_CNTR_TYPE)) + 7) >> 3))
#define CNTR9_CNPL12_CNTR_SIZE 1
#define CNTR9_CNPL12_CNTR_TYPE 1

#ifdef G9991
/******************************************************************************
  *
  * CNTR GROUP 10 :
  * --------------
  * Structure: 4B
  *
  * Usage: MULTICAST BROADCAST COUNTERS
  *   Counter   | amount
  *   -------------------
  *   rx mcast packets       | 40
  *   rx broadcast packets   | 40
  *   tx mcast packets       | 40
  *   tx broadcast packets   | 40
  *
*******************************************************************************/
#define CNTR10_CNTR_NUM  160
#define PORT_MCST_BCST_GROUP_INVLID_CNTR (CNTR10_CNTR_NUM-1)

/* CNPL GROUP 13: 2B (packets) */
#define CNTR10_CNPL13_BASE_ADDR (CNTR9_CNPL12_BASE_ADDR + (((CNTR9_CNTR_NUM * (1 << CNTR9_CNPL12_CNTR_SIZE) * (1 << CNTR9_CNPL12_CNTR_TYPE)) + 7) >> 3))
#define CNTR10_CNPL13_CNTR_SIZE 2
#define CNTR10_CNPL13_CNTR_TYPE 0

#define CNPL_END_ADDR (CNTR10_CNPL13_BASE_ADDR + (((CNTR10_CNTR_NUM * (1 << CNTR10_CNPL13_CNTR_SIZE) * (1 << CNTR10_CNPL13_CNTR_TYPE)) + 7)))
#define CNTR_END_ADDR (CNTR10_CNPL13_BASE_ADDR + (((CNTR10_CNTR_NUM * (1 << CNTR10_CNPL13_CNTR_SIZE) * (1 << CNTR10_CNPL13_CNTR_TYPE)) + 7) >> 3))
#else /*G9991*/
#define CNPL_END_ADDR (CNTR9_CNPL12_BASE_ADDR + (((CNTR9_CNTR_NUM * (1 << CNTR9_CNPL12_CNTR_SIZE) * (1 << CNTR9_CNPL12_CNTR_TYPE)) + 7)))
#define CNTR_END_ADDR (CNTR9_CNPL12_BASE_ADDR + (((CNTR9_CNTR_NUM * (1 << CNTR9_CNPL12_CNTR_SIZE) * (1 << CNTR9_CNPL12_CNTR_TYPE)) + 7) >> 3))
#endif

#if defined(BCM6858)
#define CNPL_MEMROY_END_ADDR 13440
#else
#define CNPL_MEMROY_END_ADDR 12288
#endif

#if (CNTR_END_ADDR * 8)> CNPL_MEMROY_END_ADDR
    #error " CNPL is exceed HW limit size in counters configuration "
#endif

#endif

