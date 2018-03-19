/*
    <:copyright-BRCM:2013:DUAL/GPL:standard

       Copyright (c) 2013 Broadcom
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

/*
 * rdd_spdsvc.h
 */

#ifndef _RDD_SPDSVC_H_
#define _RDD_SPDSVC_H_

#include "rdd_cpu_tx.h"

bdmf_error_t rdd_spdsvc_init ( void );

bdmf_error_t rdd_spdsvc_gen_config ( uint32_t xi_kbps,
                                 uint32_t xi_mbs,
                                 uint32_t xi_copies,
                                 uint32_t xi_total_length,
                                 uint32_t xi_test_time_ms );

bdmf_error_t rdd_spdsvc_get_tx_result ( uint8_t *xo_running_p,
                                        uint32_t *xo_tx_packets_p,
                                        uint32_t *xo_tx_discards_p );

bdmf_error_t rdd_spdsvc_gen_start(pbuf_t *pbuf,
                                  const rdpa_cpu_tx_info_t *info,
                                  RDD_CPU_TX_DESCRIPTOR_DTS *cpu_tx);

bdmf_error_t rdd_spdsvc_analyzer_config(void);

bdmf_error_t rdd_spdsvc_analyzer_delete(void);

bdmf_error_t rdd_spdsvc_analyzer_get_rx_time(uint32_t *rx_time_us);

#endif /* _RDD_SPDSVC_H_ */
