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

#include "bdmf_errno.h"
#include "rdd.h"
#include "XRDP_AG.h"
#include "rdd_spdsvc.h"
#include "rdp_drv_rnr.h"
#include "rdp_drv_fpm.h"
#include "rdd_cpu_tx.h"

#ifndef _CFE_

#ifdef CONFIG_BCM_SPDSVC_SUPPORT

/*
 * rdd_spdsvc.c
 */

#define SPDSVC_TIMER_PERIOD                                             100 /* usec */
#define SPDSVC_TIMER_HZ                                                 ( 1000000 / SPDSVC_TIMER_PERIOD ) /* sec */
#define SPDSVC_ETH_IFG                                                  20 /* bytes */
#define SPDSVC_ETH_CRC_LEN                                              4  /* bytes */
#define SPDSVC_ETH_OVERHEAD                                             (SPDSVC_ETH_CRC_LEN + SPDSVC_ETH_IFG) /* bytes */
                                                                        /* Ethernet packet + 2 VLAN Tags + PPPoE + Overhead */
#define SPDSVC_BUCKET_SIZE_MIN                                          (1514 + 8 + 8 + SPDSVC_ETH_OVERHEAD) /* bytes */
#define SPDSVC_CLEANUP_TIMER_DURATION_MS                                 1000

/* Uncomment the following line to enable debug prints */
/* #define SPDSVC_DEBUG */
#ifdef SPDSVC_DEBUG
#define SPDSVC_DBG(fmt, args...)   bdmf_print("%s#%d " fmt, __FUNCTION__, __LINE__, ## args)
#else
#define SPDSVC_DBG(fmt, args...)
#endif

#define CMD_STREAM                   "STREAM........."
#define CMD_STREAM_LENGTH            15

/* Speed service control block */
static struct spdsvc_control_block
{
    uint32_t kbps;
    uint32_t mbs;
    uint32_t copies;
    uint32_t total_length;
    bdmf_timer_t timer;         /* Timer used to release sysb when generation is finished */
    pbuf_t pbuf;
    int iters_before_release;   /* Number of timer iterations left before sysb release after copies becomes 0 */
#define SPDSVC_ITERS_BEFORE_RELEASE                                     2
    int gen_initialized;
} gen_control_block;

static inline uint32_t _rdd_spdsvc_kbps_to_tokens(uint32_t xi_kbps)
{
    return ( uint32_t )( (1000 * xi_kbps / 8) / SPDSVC_TIMER_HZ );
}

static inline uint32_t _rdd_spdsvc_mbs_to_bucket_size(uint32_t xi_mbs)
{
    uint32_t bucket_size = xi_mbs;

    if(bucket_size < SPDSVC_BUCKET_SIZE_MIN)
        bucket_size = SPDSVC_BUCKET_SIZE_MIN;

    return bucket_size;
}

static inline int _rdd_spdsvc_is_running(void)
{
    return (gen_control_block.pbuf.length != 0);
}


/* Timer that checks whether generation has finished and
 * releases sysb when yes.
 */
static void _rdd_spdsvc_gen_timer_cb(bdmf_timer_t *timer, unsigned long priv)
{
    uint32_t copies;

    if (!gen_control_block.pbuf.length)
        return;

    RDD_SPDSVC_GEN_PARAMS_TOTAL_COPIES_READ_G(copies, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);

    /* Generation still running ? */
    if (copies)
    {
        bdmf_timer_start(timer, bdmf_ms_to_ticks(SPDSVC_CLEANUP_TIMER_DURATION_MS));
        return;
    }

    /* Finished transmitting. Wait a bit to let queued packets exit */
    if (gen_control_block.iters_before_release++ < SPDSVC_ITERS_BEFORE_RELEASE)
    {
        bdmf_timer_start(timer, bdmf_ms_to_ticks(SPDSVC_CLEANUP_TIMER_DURATION_MS));
        return;

    }
    if (gen_control_block.pbuf.sysb)
    {
        /* Clear buffer address word in PD. This is the address TX_ABS_RECYCLE task
         * compares with to identify speed service PDs dropped by QM
         */
        RDD_SPDSVC_GEN_PARAMS_PROCESSING_TX_PD_3_WRITE_G(0, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
        bdmf_sysb_free(gen_control_block.pbuf.sysb);
        gen_control_block.pbuf.sysb = NULL;
    }
    gen_control_block.pbuf.length = 0;
    gen_control_block.pbuf.fpm_bn = 0;
}


bdmf_error_t rdd_spdsvc_gen_config ( uint32_t xi_kbps,
                                 uint32_t xi_mbs,
                                 uint32_t xi_copies,
                                 uint32_t xi_total_length,
                                 uint32_t xi_test_time_ms )
{
    if (_rdd_spdsvc_is_running())
    {
        BDMF_TRACE_RET(BDMF_ERR_ALREADY, "Can't reconfigure speed service while running.\n");
    }
    gen_control_block.copies = xi_copies;
    gen_control_block.kbps = xi_kbps;
    gen_control_block.mbs = xi_mbs;
    gen_control_block.total_length = xi_total_length;

    return BDMF_ERR_OK;
}

bdmf_error_t rdd_spdsvc_get_tx_result ( uint8_t *xo_running_p,
                                        uint32_t *xo_tx_packets_p,
                                        uint32_t *xo_tx_discards_p )
{
    uint32_t tx_packets;
    uint32_t tx_dropped;

    RDD_SPDSVC_GEN_PARAMS_TX_PACKETS_READ_G(tx_packets, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_TX_DROPPED_READ_G(tx_dropped, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    *xo_running_p = _rdd_spdsvc_is_running();
    *xo_tx_discards_p = tx_dropped;
    *xo_tx_packets_p = tx_packets - tx_dropped;

    return BDMF_ERR_OK;
}

bdmf_error_t rdd_spdsvc_gen_start(pbuf_t *pbuf,
                                  const rdpa_cpu_tx_info_t *info,
                                  RDD_CPU_TX_DESCRIPTOR_DTS *cpu_tx)
{
    uint32_t length = pbuf->length;
    RDD_PROCESSING_TX_DESCRIPTOR_DTS processing_tx_pd = {};
    uint32_t *p_processing_tx_pd=(uint32_t *)&processing_tx_pd;
    uint32_t tokens;
    uint32_t bucket_size;

    if (!gen_control_block.gen_initialized)
    {
        bdmf_timer_init(&gen_control_block.timer, _rdd_spdsvc_gen_timer_cb, 0);
        gen_control_block.gen_initialized = 1;
    }

    /* Make sure that it isn't running */
    if (_rdd_spdsvc_is_running())
    {
        if (pbuf->abs_flag)
            bdmf_sysb_free(pbuf->sysb);
        BDMF_TRACE_RET(BDMF_ERR_ALREADY, "Can't start speed service.\n");
    }

    if (!gen_control_block.total_length || !gen_control_block.copies)
    {
        if (pbuf->abs_flag)
            bdmf_sysb_free(pbuf->sysb);
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Can't start speed service. It is not configured.\n");
    }

    /* Fill in SPDSVC configuration record */
    tokens = _rdd_spdsvc_kbps_to_tokens(gen_control_block.kbps);
    bucket_size = _rdd_spdsvc_mbs_to_bucket_size(gen_control_block.mbs + tokens);

    RDD_SPDSVC_GEN_PARAMS_BUCKET_SIZE_WRITE_G(bucket_size, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_BUCKET_WRITE_G(0, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_TOKENS_WRITE_G(tokens, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_TOTAL_LENGTH_WRITE_G(gen_control_block.total_length, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_TIMER_PERIOD_WRITE_G(SPDSVC_TIMER_PERIOD, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_TX_PACKETS_WRITE_G(0, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_TX_DROPPED_WRITE_G(0, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    if (pbuf->abs_flag)
    {
        uintptr_t data_phys_addr = RDD_RSV_VIRT_TO_PHYS(pbuf->data);
        processing_tx_pd.abs = 1;
        processing_tx_pd.buffer_number_0_or_abs_0 = data_phys_addr & 0x3ffff;
        processing_tx_pd.payload_offset_or_abs_1 = (data_phys_addr >> 18) & 0x7ff;
        processing_tx_pd.union3 = (data_phys_addr >> 29) & 0x7;
    }
    else
    {
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Speed Service requires transmit from ABS address.\n");
        //processing_tx_pd.buffer_number_0_or_abs_0 = pbuf->fpm_bn;
        //processing_tx_pd.payload_offset_or_abs_1 = pbuf->offset;
    }
    processing_tx_pd.valid = 1;
    processing_tx_pd.first_level_q = cpu_tx->first_level_q;
    processing_tx_pd.packet_length = length;
    processing_tx_pd.lan = cpu_tx->lan;
    processing_tx_pd.ingress_port = cpu_tx->wan_flow_source_port;

    RDD_SPDSVC_GEN_PARAMS_PROCESSING_TX_PD_0_WRITE_G(
        p_processing_tx_pd[0], RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_PROCESSING_TX_PD_1_WRITE_G(
        p_processing_tx_pd[1], RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_PROCESSING_TX_PD_2_WRITE_G(
        p_processing_tx_pd[2], RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_PROCESSING_TX_PD_3_WRITE_G(
        p_processing_tx_pd[3], RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_GEN_PARAMS_TOTAL_COPIES_WRITE_G(gen_control_block.copies, RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR, 0);

    SPDSVC_DBG("Speed Service Generator started: tokens=%u, bucket_size=%u, copies=%u --> %p\n",
        tokens, bucket_size, gen_control_block.copies,
        DEVICE_ADDRESS(rdp_runner_core_addr[spdsvc_gen_runner_image] + RDD_SPDSVC_GEN_PARAMS_TABLE_ADDRESS_ARR[spdsvc_gen_runner_image]));
    SPDSVC_DBG("   bn=%x  va=%p  pa=%llx length=%u  offset=%u pd=%08x %08x %08x %08x\n",
        pbuf->fpm_bn, pbuf->data, RDD_RSV_VIRT_TO_PHYS(pbuf->data),
        pbuf->length, info->data_offset, p_processing_tx_pd[0], p_processing_tx_pd[1], p_processing_tx_pd[2], p_processing_tx_pd[3]);

    /* Kick speed service task */
    WMB();
    ag_drv_rnr_regs_cfg_cpu_wakeup_set(get_runner_idx(spdsvc_gen_runner_image), IMAGE_2_CPU_IF_2_SPDSVC_GEN_THREAD_NUMBER);

    /* Start timer to release the buffer when test is done */
    gen_control_block.iters_before_release = 0;
    gen_control_block.pbuf = *pbuf;
    bdmf_timer_start(&gen_control_block.timer, bdmf_ms_to_ticks(SPDSVC_CLEANUP_TIMER_DURATION_MS));

    return BDMF_ERR_OK;
}

bdmf_error_t rdd_spdsvc_analyzer_config(void)
{
    const char stream_prefix[]=CMD_STREAM;
    int i;

    /* Set up stream prefix */
    for ( i = 0 ; i < sizeof(stream_prefix); i++ )
    {
        GROUP_MWRITE_I_8(RDD_SPDSVC_ANALYZER_PARAMS_TABLE_ADDRESS_ARR, 0, i, stream_prefix[i]);
    }
    RDD_SPDSVC_ANALYZER_PARAMS_TS_FIRST_WRITE_G(0, RDD_SPDSVC_ANALYZER_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_ANALYZER_PARAMS_TS_LAST_WRITE_G(0, RDD_SPDSVC_ANALYZER_PARAMS_TABLE_ADDRESS_ARR, 0);
    SPDSVC_DBG("   prefix at %p\n", RDD_SPDSVC_ANALYZER_PARAMS_TABLE_PTR(processing_runner_image));

    RDD_SPDSVC_ANALYZER_PARAMS_IS_ON_WRITE_G(1, RDD_SPDSVC_ANALYZER_PARAMS_TABLE_ADDRESS_ARR, 0);
    
    
    return BDMF_ERR_OK;
}

bdmf_error_t rdd_spdsvc_analyzer_delete(void)
{
  RDD_SPDSVC_ANALYZER_PARAMS_IS_ON_WRITE_G(0, RDD_SPDSVC_ANALYZER_PARAMS_TABLE_ADDRESS_ARR, 0);
  
  return BDMF_ERR_OK;
}

bdmf_error_t rdd_spdsvc_analyzer_get_rx_time(uint32_t *rx_time_us)
{
    uint32_t ts_first;
    uint32_t ts_last;

    RDD_SPDSVC_ANALYZER_PARAMS_TS_FIRST_READ_G(ts_first, RDD_SPDSVC_ANALYZER_PARAMS_TABLE_ADDRESS_ARR, 0);
    RDD_SPDSVC_ANALYZER_PARAMS_TS_LAST_READ_G(ts_last, RDD_SPDSVC_ANALYZER_PARAMS_TABLE_ADDRESS_ARR, 0);

    *rx_time_us = ts_last - ts_first;

    return BDMF_ERR_OK;
}

#endif /* #ifndef _CFE_ */

#endif /* G9991 */
