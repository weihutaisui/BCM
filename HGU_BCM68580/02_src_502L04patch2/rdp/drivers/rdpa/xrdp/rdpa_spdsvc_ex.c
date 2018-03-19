/*
* <:copyright-BRCM:2014:DUAL/GPL:standard
*
*    Copyright (c) 2014 Broadcom
*    All Rights Reserved
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed
* to you under the terms of the GNU General Public License version 2
* (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
* with the following added to such license:
*
*    As a special exception, the copyright holders of this software give
*    you permission to link this software with independent modules, and
*    to copy and distribute the resulting executable under terms of your
*    choice, provided that you also meet, for each linked independent
*    module, the terms and conditions of the license of that module.
*    An independent module is a module which is not derived from this
*    software.  The special exception does not apply to any modifications
*    of the software.
*
* Not withstanding the above, under no circumstances may you combine
* this software in any way with any other Broadcom software provided
* under a license other than the GPL, without Broadcom's express prior
* written consent.
*
* :>
*/

/*
 * rdpa_spdsvc_ex.c
 * RDPA Speed Service XRDP interface
 */
#include <bdmf_dev.h>
#include <rdpa_api.h>
#include "rdpa_int.h"
#include <rdd.h>
#include <rdpa_spdsvc.h>
#include <rdd_spdsvc.h>
#include "rdpa_spdsvc_ex.h"

bdmf_error_t rdpa_rdd_spdsvc_gen_config(const rdpa_spdsvc_generator_t *generator_p)
{
    return rdd_spdsvc_gen_config(generator_p->kbps, generator_p->mbs,
                                 generator_p->copies, generator_p->total_length,
                                 generator_p->test_time_ms);
}

/* Get generator counters and running status. Analyzer counters remain unchanged */
bdmf_error_t rdpa_rdd_spdsvc_gen_get_result(uint8_t *running_p,
                                            uint32_t *tx_packets_p,
                                            uint32_t *tx_discards_p)
{
    return rdd_spdsvc_get_tx_result(running_p,
                                    tx_packets_p,
                                    tx_discards_p);
}

bdmf_error_t rdpa_rdd_spdsvc_analyzer_config(void)
{
    return rdd_spdsvc_analyzer_config();
}

bdmf_error_t rdpa_rdd_spdsvc_analyzer_delete(void)
{
    return rdd_spdsvc_analyzer_delete();
}

bdmf_error_t rdpa_rdd_spdsvc_analyzer_get_rx_time(uint32_t *rx_time_us)
{
    return rdd_spdsvc_analyzer_get_rx_time(rx_time_us);
}
