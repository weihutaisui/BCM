/*
 * Common code for wl command-line swiss-army-knife utility
 *
 * Copyright (C) 2016, Broadcom Ltd.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: wlu.c 590248 2016-03-11 11:09:24Z $
 */
#include "wlu_endian_handler.h"
#define FIELD_CONVERT32(ptr,field)  (ptr)->field=dtoh32((ptr)->field)
#define FIELD_CONVERT16(ptr,field)  (ptr)->field=dtoh16((ptr)->field)
#define FIELD_ARRAY_CONVERT32(array,index)  array[index]=dtoh32(array[index])
#define FIELD_ARRAY_CONVERT16(array,index)  array[index]=dtoh16(array[index])

void wl_iov_mac_full_params_endian_handler(wl_iov_mac_full_params_t *full_params)
{
    int i=0;
    FIELD_CONVERT32(&(full_params->params),num_addrs);
    for (i = 0; i < WL_IOV_MAC_PARAM_LEN; ++i) {
        FIELD_ARRAY_CONVERT32(full_params->extra_params.addr_info,i);
    }
}

void wl_iov_pktq_log_endian_handler(wl_iov_pktq_log_t* iov)
{

    int i,j;
    FIELD_CONVERT32(iov,version);
    FIELD_CONVERT32(&(iov->params),num_addrs);
    pktq_log_counters_v04_t *pcouners;
    pktq_log_counters_v05_t *p5couners;
    for (i = 0; i < WL_IOV_MAC_PARAM_LEN; ++i) {
        for (j = 0; j < WL_IOV_PKTQ_LOG_PRECS; ++j) {
            if(iov->version==5)
                pcouners=(pktq_log_counters_v04_t *)&(iov->pktq_log.v05.counters[i][j]);
            else
                pcouners=&(iov->pktq_log.v04.counters[i][j]);
            FIELD_CONVERT32(pcouners,requested);
            FIELD_CONVERT32(pcouners,stored);
            FIELD_CONVERT32(pcouners,saved);
            FIELD_CONVERT32(pcouners,selfsaved);
            FIELD_CONVERT32(pcouners,full_dropped);
            FIELD_CONVERT32(pcouners,dropped);
            FIELD_CONVERT32(pcouners,sacrificed);
            FIELD_CONVERT32(pcouners,busy);
            FIELD_CONVERT32(pcouners,retry);
            FIELD_CONVERT32(pcouners,ps_retry);
            FIELD_CONVERT32(pcouners,suppress);
            FIELD_CONVERT32(pcouners,retry_drop);
            FIELD_CONVERT32(pcouners,max_avail);
            FIELD_CONVERT32(pcouners,max_used);
            FIELD_CONVERT32(pcouners,queue_capacity);
            FIELD_CONVERT32(pcouners,rtsfail);
            FIELD_CONVERT32(pcouners,acked);
            FIELD_CONVERT32(pcouners,txrate_succ);
            FIELD_CONVERT32(pcouners,txrate_main);
            FIELD_CONVERT32(pcouners,throughput);
            FIELD_CONVERT32(pcouners,time_delta);
            if(iov->version==5)  {
                p5couners=(pktq_log_counters_v05_t *)pcouners;
                FIELD_CONVERT32(p5couners,airtime);
            }
        }
        if(iov->version==5)  {
            FIELD_ARRAY_CONVERT32(iov->pktq_log.v05.counter_info,i);
            FIELD_ARRAY_CONVERT32(iov->pktq_log.v05.pspretend_time_delta,i);
        } else {
            FIELD_ARRAY_CONVERT32(iov->pktq_log.v04.counter_info,i);
            FIELD_ARRAY_CONVERT32(iov->pktq_log.v04.pspretend_time_delta,i);
        }
    }
}

void wl_txbf_rateset_endian_handler(wl_txbf_rateset_t *txbf)
{
    int i=0;
    for (i = 0; i < TXBF_RATE_VHT_ALL; ++i) {
        FIELD_ARRAY_CONVERT16(txbf->txbf_rate_vht,i);
        FIELD_ARRAY_CONVERT16(txbf->txbf_rate_vht_bcm,i);
    }
}

void wl_rateset_args_endian_handler(wl_rateset_args_t *rateset)
{
    int i=0;
    FIELD_CONVERT32(rateset,count);
    for (i = 0; i <VHT_CAP_MCS_MAP_NSS_MAX ; ++i) {
        FIELD_ARRAY_CONVERT16(rateset->vht_mcs,i);
    }
}
void wl_roam_prof_band_endian_handler(wl_roam_prof_band_t *pband)
{
    int i=0;
    FIELD_CONVERT32(pband,band);
    FIELD_CONVERT16(pband,ver);
    FIELD_CONVERT16(pband,len);
    for (i = 0; i < WL_MAX_ROAM_PROF_BRACKETS; ++i) {
        FIELD_CONVERT16((wl_roam_prof_t *)(pband->roam_prof+i),nfscan);
        FIELD_CONVERT16((wl_roam_prof_t *)(pband->roam_prof+i),fullscan_period);
        FIELD_CONVERT16((wl_roam_prof_t *)(pband->roam_prof+i),init_scan_period);
        FIELD_CONVERT16((wl_roam_prof_t *)(pband->roam_prof+i),backoff_multiplier);
        FIELD_CONVERT16((wl_roam_prof_t *)(pband->roam_prof+i),max_scan_period);
    }
}

void  wlc_bcn_len_hist_endian_handler(wlc_bcn_len_hist_t  *bcn)
{
    int i=0;
    FIELD_CONVERT16(bcn,ver);
    FIELD_CONVERT16(bcn,cur_index);
    FIELD_CONVERT32(bcn,max_bcnlen);
    FIELD_CONVERT32(bcn,min_bcnlen);
    FIELD_CONVERT32(bcn,ringbuff_len);
    for (i = 0; i < bcn->ringbuff_len; ++i) {
        FIELD_ARRAY_CONVERT32(bcn->bcnlen_ring,i);
    }
}
