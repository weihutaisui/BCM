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
#ifndef _RDD_TUPLE_LKP_H
#define _RDD_TUPLE_LKP_H


#include "rdd.h"
#include "rdd_common.h"


typedef struct natc_params
{
    uint32_t connection_key_offset;
} natc_params_t;

typedef enum
{
    ctx_size_8        = 0,
    ctx_size_16       = 1,
    ctx_size_24       = 2,
    ctx_size_32       = 3,
    ctx_size_40       = 4,
    ctx_size_48       = 5,
    ctx_size_56       = 6,
    ctx_size_64       = 7,
    ctx_size_72       = 8,
    ctx_size_80       = 9,
    ctx_size_88       = 10,
    ctx_size_96       = 11,
    ctx_size_104      = 12,
    ctx_size_112      = 13,
    ctx_size_120      = 14,
    ctx_size_128      = 15,
    ctx_size_not_used = 16
} natc_var_size_ctx;

int rdd_nat_cache_init(const rdd_module_t *module);

int rdd_ip_class_key_entry_var_size_ctx_compose(rdpa_ip_flow_key_t *tuple_entry, uint8_t *connection_entry,
    uint8_t *connection_entry_no_size, natc_var_size_ctx ctx_size);
int rdd_ip_class_key_entry_compose(rdpa_ip_flow_key_t *tuple_entry, uint8_t *connection_entry);
void rdd_ip_class_key_entry_decompose(rdpa_ip_flow_key_t *tuple_entry, uint8_t *sub_tbl_id, uint8_t *tuple_entry_ptr);

#endif /* RDD_TUPLE_LKP_H_ */

