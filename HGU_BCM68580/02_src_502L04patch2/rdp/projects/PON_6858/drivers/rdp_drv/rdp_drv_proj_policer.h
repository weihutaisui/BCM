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
#ifndef _RDP_DRV_PROJ_POLICER_H_
#define _RDP_DRV_PROJ_POLICER_H_

#include "rdp_drv_proj_cntr.h"

/****************************************************************************** 
  * 
  * POLICER GROUP 0 :
  * --------------
  * Structure: 8B 
  * Parameters: 8B 
  *  
  * Usage: 
  *   Policer   | amount
  *   -------------------
  *   Double    | 80
  *  
  *  
*******************************************************************************/
#define CNPL_POLICER_NUM  80
#define CNPL_POLICER_SIZE  8

/* first group defined as dual bucket */
/* to set single bucket parameters of second bucket should be zero */
#define CNPL_GROUP_DUAL_BUCKET_INDEX  0

/* second group not used. workaround so policer index 0 won't be runover
   by the second group*/
#define CNPL_GROUP_ONE_INDEX 1

/* POLICER GROUP 0: Double */
#define CNPL_POLICER_BASE_ADDR  CNTR_END_ADDR
#define CNPL_POLICER_END_ADDR  (CNPL_POLICER_BASE_ADDR + ((CNPL_POLICER_NUM * CNPL_POLICER_SIZE) >> 3))

#define CNPL_POLICER_PARAM_BASE_ADDR  CNPL_POLICER_END_ADDR
#define CNPL_POLICER_PARAM_END_ADDR  (CNPL_POLICER_PARAM_BASE_ADDR + ((CNPL_POLICER_NUM * CNPL_POLICER_SIZE) >> 3))

#if (CNPL_POLICER_PARAM_END_ADDR * 8)> CNPL_MEMROY_END_ADDR
    #error " CNPL is exceed HW limit size in policer configuration "
#endif

#endif

