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

#include "bdmf_data_types.h"
#include "bdmf_errno.h"
#include "rdp_drv_proj_cntr.h"

bdmf_boolean cntr_group_0_occupied[CNTR0_CNTR_NUM]={};
bdmf_boolean cntr_group_1_occupied[CNTR1_CNTR_NUM]={};
bdmf_boolean cntr_group_2_occupied[CNTR2_CNTR_NUM]={};
bdmf_boolean cntr_group_3_occupied[CNTR3_CNTR_NUM]={};
bdmf_boolean cntr_group_4_occupied[CNTR4_CNTR_NUM]={};
bdmf_boolean cntr_group_5_occupied[CNTR5_CNTR_NUM]={};
bdmf_boolean cntr_group_6_occupied[CNTR6_CNTR_NUM]={};
bdmf_boolean cntr_group_7_occupied[CNTR7_CNTR_NUM]={};
bdmf_boolean cntr_group_8_occupied[CNTR8_CNTR_NUM]={};
bdmf_boolean cntr_group_9_occupied[CNTR9_CNTR_NUM]={};
#ifdef G9991
bdmf_boolean cntr_group_10_occupied[CNTR10_CNTR_NUM]={};
#endif
cntr_group_cfg_t cntr_group_cfg[CNTR_GROUPS_NUMBER] = {
     {RX_FLOW_CNTR_GROUP_ID, cntr_group_0_occupied, CNTR0_CNTR_NUM,
      {{0, CNTR0_CNPL0_BASE_ADDR, CNTR0_CNPL0_CNTR_SIZE, CNTR0_CNPL0_CNTR_TYPE, 1,  1, 1},
       {1, CNTR0_CNPL1_BASE_ADDR, CNTR0_CNPL1_CNTR_SIZE, CNTR0_CNPL1_CNTR_TYPE, 1,  1, 1},
       {}}},
     {TX_FLOW_CNTR_GROUP_ID, cntr_group_1_occupied, CNTR1_CNTR_NUM,
      {{2, CNTR1_CNPL2_BASE_ADDR, CNTR1_CNPL2_CNTR_SIZE, CNTR1_CNPL2_CNTR_TYPE, 1,  1, 1},
       {},
       {}}},
     {TCAM_IPTV_DEF_CNTR_GROUP_ID, cntr_group_2_occupied, CNTR2_CNTR_NUM,
      {{3, CNTR2_CNPL3_BASE_ADDR, CNTR2_CNPL3_CNTR_SIZE, CNTR2_CNPL3_CNTR_TYPE, 1,  0, 1},
       {},
       {}}},
     {VARIOUS_CNTR_GROUP_ID, cntr_group_3_occupied, CNTR3_CNTR_NUM,
      {{4, CNTR3_CNPL4_BASE_ADDR, CNTR3_CNPL4_CNTR_SIZE, CNTR3_CNPL4_CNTR_TYPE, 1,  0, 1},
       {},
       {}}},
     {GENERAL_CNTR_GROUP_ID, cntr_group_4_occupied, CNTR4_CNTR_NUM,
      {{5, CNTR4_CNPL5_BASE_ADDR, CNTR4_CNPL5_CNTR_SIZE, CNTR4_CNPL5_CNTR_TYPE, 1,  0, 1},
       {},
       {}}},
     {US_TX_QUEUE_CNTR_GROUP_ID, cntr_group_5_occupied, CNTR5_CNTR_NUM,
      {{6, CNTR5_CNPL6_BASE_ADDR, CNTR5_CNPL6_CNTR_SIZE, CNTR5_CNPL6_CNTR_TYPE, 1,  0, 1},
       {},
       {}}},
     {DS_TX_QUEUE_CNTR_GROUP_ID, cntr_group_6_occupied, CNTR6_CNTR_NUM,
      {{7, CNTR6_CNPL7_BASE_ADDR, CNTR6_CNPL7_CNTR_SIZE, CNTR6_CNPL7_CNTR_TYPE, 1,  0, 1},
       {},
       {}}},
     {CPU_RX_CNTR_GROUP_ID, cntr_group_7_occupied, CNTR7_CNTR_NUM,
      {{8, CNTR7_CNPL8_BASE_ADDR, CNTR7_CNPL8_CNTR_SIZE, CNTR7_CNPL8_CNTR_TYPE, 0,  0, 1},
       {},
       {}}},
     {CPU_RX_METER_DROP_CNTR_GROUP_ID, cntr_group_8_occupied, CNTR8_CNTR_NUM,
      {{9, CNTR8_CNPL9_BASE_ADDR, CNTR8_CNPL9_CNTR_SIZE, CNTR8_CNPL9_CNTR_TYPE, 1,  1, 1},
       {},
       {}}},
#ifdef G9991
     {POLICER_CNTR_GROUP_ID, cntr_group_9_occupied, CNTR9_CNTR_NUM,
      {{10, CNTR9_CNPL10_BASE_ADDR, CNTR9_CNPL10_CNTR_SIZE, CNTR9_CNPL10_CNTR_TYPE, 1,  0, 1},
       {11, CNTR9_CNPL11_BASE_ADDR, CNTR9_CNPL11_CNTR_SIZE, CNTR9_CNPL11_CNTR_TYPE, 1,  0, 1},
       {12, CNTR9_CNPL12_BASE_ADDR, CNTR9_CNPL12_CNTR_SIZE, CNTR9_CNPL12_CNTR_TYPE, 1,  0, 1}}},
     {PORT_MCST_BCST_GROUP_ID, cntr_group_10_occupied, CNTR10_CNTR_NUM,
      {{13, CNTR10_CNPL13_BASE_ADDR, CNTR10_CNPL13_CNTR_SIZE, CNTR10_CNPL13_CNTR_TYPE, 1, 1, 1},
       {},
       {}}}
#else
     {POLICER_CNTR_GROUP_ID, cntr_group_9_occupied, CNTR9_CNTR_NUM,
     {{10, CNTR9_CNPL10_BASE_ADDR, CNTR9_CNPL10_CNTR_SIZE, CNTR9_CNPL10_CNTR_TYPE, 1,  0, 1},
      {11, CNTR9_CNPL11_BASE_ADDR, CNTR9_CNPL11_CNTR_SIZE, CNTR9_CNPL11_CNTR_TYPE, 1,  0, 1},
      {12, CNTR9_CNPL12_BASE_ADDR, CNTR9_CNPL12_CNTR_SIZE, CNTR9_CNPL12_CNTR_TYPE, 1,  0, 1}}}
#endif
};

