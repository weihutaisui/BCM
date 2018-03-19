/*
 * Common code for wl command line utility
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
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Broadcom Corporation.
 *
 * $Id: wlu_endian_handler.h 579766 2016-03-11 10:50:20Z $
 */

#ifndef _wlu_endian_handler_h_
#define _wlu_endian_handler_h_
#include <bcmendian.h>
#include <wlioctl.h>
#include "wlu_common.h"
void wl_iov_mac_full_params_endian_handler(wl_iov_mac_full_params_t *full_params);
void wl_iov_pktq_log_endian_handler(wl_iov_pktq_log_t* iov);
void wl_txbf_rateset_endian_handler(wl_txbf_rateset_t *txbf);
void wl_rateset_args_endian_handler(wl_rateset_args_t *rateset);
void  wlc_bcn_len_hist_endian_handler(wlc_bcn_len_hist_t  *bcn);
void wl_roam_prof_band_endian_handler(wl_roam_prof_band_t *pband);
#endif
