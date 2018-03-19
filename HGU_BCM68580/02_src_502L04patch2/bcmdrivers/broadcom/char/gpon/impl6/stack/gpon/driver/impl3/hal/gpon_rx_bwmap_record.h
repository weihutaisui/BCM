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

#ifndef _GPON_RX_BWMAP_RECORD_AG_H_
#define _GPON_RX_BWMAP_RECORD_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"


/**************************************************************************************************/
/* rcd_stop_on_map_end: rcd_stop_on_map_end - Whether to stop on map end or to stop when memory i */
/*                      s full.If asserted, only one map will be recorded and the recording will  */
/*                      be stopped at the end of the first map after recording enable.            */
/* rcd_all: rcd_all - Record all accesses (do not filter any of the accesses according to Alloc-I */
/*          D for recording)                                                                      */
/* rcd_all_onu: rcd_all_onu - Record only the accesses which are directed to one of the 40 TCONTs */
/*               of the ONU.                                                                      */
/* rcd_specific_alloc: rcd_specific_alloc - Record only accesses of a specific Alloc-ID as config */
/*                     ured in the next field.                                                    */
/* secific_alloc: secific_alloc - Configurable Alloc-ID to record the access which are directed t */
/*                o.Used only when rcd_specific_alloc bit is asserted.                            */
/**************************************************************************************************/
typedef struct
{
    bdmf_boolean rcd_stop_on_map_end;
    bdmf_boolean rcd_all;
    bdmf_boolean rcd_all_onu;
    bdmf_boolean rcd_specific_alloc;
    uint16_t secific_alloc;
} gpon_rx_bwmap_record_config;

bdmf_error_t ag_drv_gpon_rx_bwmap_record_config_set(const gpon_rx_bwmap_record_config *config);
bdmf_error_t ag_drv_gpon_rx_bwmap_record_config_get(gpon_rx_bwmap_record_config *config);
bdmf_error_t ag_drv_gpon_rx_bwmap_record_enable_set(bdmf_boolean rcd_enable);
bdmf_error_t ag_drv_gpon_rx_bwmap_record_enable_get(bdmf_boolean *rcd_enable);
bdmf_error_t ag_drv_gpon_rx_bwmap_record_status_get(bdmf_boolean *rcd_done, bdmf_boolean *rcd_mismatch, bdmf_boolean *ecd_empty, uint8_t *rcd_last_ptr);
bdmf_error_t ag_drv_gpon_rx_bwmap_record_read_if_set(uint8_t read_addr);
bdmf_error_t ag_drv_gpon_rx_bwmap_record_read_if_get(uint8_t *read_addr);
bdmf_error_t ag_drv_gpon_rx_bwmap_record_rdata_get(uint32_t word_id, uint32_t *rcd_data);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_rx_bwmap_record_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

