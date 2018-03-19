/*
* <:copyright-BRCM:2016:proprietary:gpon
* 
*    Copyright (c) 2016 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :> 
*/

#include "gpon_stat.h"
#include "gpon_db.h"
#include "gpon_all.h"
#include "gpon_logger.h"

extern GPON_DATABASE_DTS gs_gpon_database;
BIP8_COUNTERS gs_bip8_counters;

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_bip_error_counter                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Get BIP errors                                                    */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function get the BIP errors counter                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_bip_errors - Number of BIP errors detected                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_bip_error_counter( uint32_t * const xo_bip_errors)
{
    return ag_drv_gpon_rx_pm_counter_bip_get(xo_bip_errors);
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_get_fec_counters                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Get FEC related counters                                       */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function returns the number of bytes the FEC decoder corrected,      */
/*   the number of bit errors the FEC block corrected and the number of       */
/*   codewords the FEC block could not correct.                               */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_rx_get_fec_counters(uint32_t *fec_corrected_bytes,
    uint32_t *fec_corrected_codewords, uint32_t *fec_uncorrectable_codewords)
{
    int drv_error; 

    drv_error = ag_drv_gpon_rx_pm_counter_symerror_get(fec_corrected_bytes);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read fec corrected bytes due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_rx_pm_counter_fecccwcnt_get(fec_corrected_codewords);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read fec corrected codewords due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_rx_pm_counter_fecuccwcnt_get((uint16_t *)fec_uncorrectable_codewords);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
            "Unable to read fec uncorrected words due to driver error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_get_hec_counters                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Get HEC related counters                                       */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   The function returns the Number of HEC related counters                  */
/******************************************************************************/
PON_ERROR_DTE gpon_rx_get_hec_counters(PON_RX_HEC_COUNTERS_DTE * const rxpon_hec_counter)
{
    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_rx_ploam_counters                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Read Ploam counters                                                */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the Ploam counters collected by the GPON stack     */
/*   Valid ploam counter.                                                     */
/*     Valid ploam counter ONU-id                                             */
/*     Valid ploam counter broadcast.                                         */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_rxpon_ploam_counters - Ploam counters output structure                */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_ERROR_INVALID_STATE - Invalid state                                */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_rx_ploam_counters(PON_RX_PLOAM_COUNTERS_DTE *const xo_rxpon_ploam_counters)
{
    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    ag_drv_gpon_rx_pm_counter_plonucnt_get(&xo_rxpon_ploam_counters->valid_onu_id_ploam_counter);
    ag_drv_gpon_rx_pm_counter_plbcstcnt_get(&xo_rxpon_ploam_counters->valid_broadcast_ploam_counter);

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_tcont_fe_queue_pd_counter                                       */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Get Tcont Front End queue Packet Descriptors Counter           */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*  This function  returns the number of valid packet descriptors             */
/*  in a specific TX queue.                                                   */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*       xi_tcont_id - Tcont ID(0 - 39)                                       */
/*                                                                            */
/*       xo_pd_counter - PD counter                                           */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_ERROR_INVALID_STATE - Invalid state                                */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_tcont_fe_queue_pd_counter(uint32_t xi_tcont_id,
    uint16_t *const xo_pd_counter)
{
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    drv_error = ag_drv_gpon_tx_fifo_statistics_tqpdcntr_get(xi_tcont_id, xo_pd_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read pd_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_txpon_pm_counters                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Tx GPON -  Read PM counters                                              */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the PM counters collected by the GPON stack        */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_pm_counters - PM counters output structure                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_tx_pm_counters(uint16_t *const xo_illegal_access_counter,
    uint32_t * const xo_idle_ploam_counter, uint32_t * const xo_normal_ploam_counter,
    uint16_t *const xo_requested_ranging_ploam_number, uint16_t *const xo_requested_urgent_ploam_number)
{
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED )
    {
        return PON_ERROR_INVALID_STATE;
    }

    drv_error = ag_drv_gpon_tx_statistics_counters_iac_get(xo_illegal_access_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read illegal_access_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_statistics_counters_ripc_get(xo_idle_ploam_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read idle_ploam_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_statistics_counters_rnpc_get(xo_normal_ploam_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read xo_normal_ploam_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_statistics_counters_rspc_get(xo_requested_urgent_ploam_number, 
        xo_requested_ranging_ploam_number);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read spaecial PLOAM counters due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}

PON_ERROR_DTE _gpon_get_tx_pm_counters(uint16_t *illegal_access_counter,
    uint32_t *idle_ploam_counter, uint32_t *tx_ploam_counter)
{
    uint16_t ranging_ploam_number, urgent_ploam_number;
    int rc = gpon_get_tx_pm_counters(illegal_access_counter, idle_ploam_counter,
        tx_ploam_counter, &ranging_ploam_number, &urgent_ploam_number);

    *tx_ploam_counter += ranging_ploam_number + urgent_ploam_number;

    return rc;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_tcont_queue_pm_counters                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Read TCONT PM counters                                             */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the PM counters collected by the GPON stack        */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_pm_counters - PM counters output structure                            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_tcont_queue_pm_counters(uint32_t xi_fifo_id,
    uint32_t * const xo_transmitted_idle_counter, uint32_t * const xo_transmitted_gem_counter,
    uint32_t * const xo_transmitted_packet_counter, uint16_t *const xo_requested_dbr_counter,
    uint16_t * const xo_valid_access_counter)
{
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    if (xi_fifo_id < 0 || xi_fifo_id > 3)
    {
        return PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR;
    }

    drv_error = ag_drv_gpon_tx_statistics_counters_tic_get(xi_fifo_id, xo_transmitted_idle_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read transmitted_idle_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_statistics_counters_tgc_get(xi_fifo_id, xo_transmitted_gem_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read transmitted_gem_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_statistics_counters_tpc_get(xi_fifo_id, xo_transmitted_packet_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read transmitted_packet_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_statistics_counters_rac_get(xi_fifo_id, xo_valid_access_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read valid_access_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_tx_statistics_counters_rdbc_get(xi_fifo_id, xo_requested_dbr_counter);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read requested_dbr_counter due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_rx_unkonw_ploam_counter                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Read rx unknown ploam counter                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the rx unknown ploam counterk                      */
/*                                                                            */
/* This function is synchronous                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_rx_unknown_counters - PM counters output structure                    */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*   PON_NO_ERROR - Success                                                   */
/*   PON_GENERAL_ERROR - General error                                        */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_rx_unkonw_ploam_counter(uint32_t *const xo_rx_unknown_counters)
{
    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    *xo_rx_unknown_counters = gs_gpon_database.unknown_ploam.non_standard_rx_msg;

    /* Clear counter */
    gs_gpon_database.unknown_ploam.non_standard_rx_msg = 0;


    return PON_NO_ERROR;
}

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_encryption_error_counter                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Read Encryption Error counter                                     */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the Encryption error counter                       */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_rx_encryption_error - Encryption error counter                        */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*   PON_NO_ERROR - Success                                                   */
/*   PON_GENERAL_ERROR - General error                                        */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_encryption_error_counter(uint32_t *const xo_rx_encryption_error)
{
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    return ag_drv_gpon_rx_pm_counter_aeserror_get(xo_rx_encryption_error);
}


void p_read_bip8_error_counters(uint32_t *xi_bip_error, BIP8_MIRROR_TYPE_DTS xi_bip8_process_request)
{
    PON_ERROR_DTE gpon_error;
    uint32_t bip_count = 0;

    gpon_error = gpon_get_bip_error_counter(&bip_count);
    if (gpon_error != PON_NO_ERROR)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read bip error counter");
        return;
    }

    MS_CHECK_AND_UPDATE_COUNTER_SATURATION(gs_bip8_counters.ber_interval_bip8_counter_mirror, bip_count);
    MS_CHECK_AND_UPDATE_COUNTER_SATURATION(gs_bip8_counters.critical_ber_bip8_counter_mirror, bip_count);
    MS_CHECK_AND_UPDATE_COUNTER_SATURATION(gs_bip8_counters.user_bip8_counter_mirror, bip_count);

    /* Reading counter by BER interval process */
    switch (xi_bip8_process_request)
    {
    case CS_BER_INTERVAL_TYPE:
        /* return value and clear mirror */
        *xi_bip_error = gs_bip8_counters.ber_interval_bip8_counter_mirror;
        gs_bip8_counters.ber_interval_bip8_counter_mirror = 0;
        break;

    case CS_CRITICAL_BER_TYPE:
        /* return value and clear mirror */
        *xi_bip_error = gs_bip8_counters.critical_ber_bip8_counter_mirror;
        gs_bip8_counters.critical_ber_bip8_counter_mirror = 0;
        break;

    case CS_USER_TYPE:
        /* return value and clear mirror */
        *xi_bip_error = gs_bip8_counters.user_bip8_counter_mirror;
        gs_bip8_counters.user_bip8_counter_mirror = 0;
        break;

    default:
        break;
    }
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_ploam_crc_error_counter                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Get PLOAM rx crc ploam counter                                           */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns crc ploam counter.                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_request_type                                                          */
/*                                                                            */
/*   xo_ploam_crc_errors                                                      */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_ERROR_DTE   - Return code                                             */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_ploam_crc_error_counter(
    PON_RXPON_REQUEST_DTE xi_request_type, uint32_t *const xo_ploam_crc_errors)
{
    uint32_t ploamerr;
    int drv_error;

    drv_error = ag_drv_gpon_rx_pm_counter_ploamerr_get(&ploamerr);
    if (drv_error > 0) 
{
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to read CRC errror due to driver return Error %d", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    switch (xi_request_type)
    {
        case CE_COUNTER_STACK_REQUEST:
            /* Calculate received counters values */
            * xo_ploam_crc_errors  = gs_gpon_database.rxpon_gpon_conters.stack_crc_counter + ploamerr;
            /* Clear counters */
            gs_gpon_database.rxpon_gpon_conters.stack_crc_counter = 0;
            /* Update gmp counters */
            gs_gpon_database.rxpon_gpon_conters.gmp_crc_counter += ploamerr;
            break;

        case CE_COUNTER_RDPA_REQUEST:
            /* Calculate received counters values */
            * xo_ploam_crc_errors = gs_gpon_database.rxpon_gpon_conters.gmp_crc_counter + ploamerr;
            /* Clear GEM counters */
            gs_gpon_database.rxpon_gpon_conters.gmp_crc_counter = 0;
            /* Update stack counters */
            gs_gpon_database.rxpon_gpon_conters.stack_crc_counter += ploamerr;
            break;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   fi_update_rxpon_get_bw_map_error_counters                                */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL - Get gem rx bip ploam counter                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns crc ploam counter.                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xi_request_type -                                                        */
/*                                                                            */
/*   xo_bip_error_counter -                                                   */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   BL_ERROR_DTE   - Return code                                             */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE fi_update_rxpon_get_bw_map_error_counters(
    PON_RXPON_REQUEST_DTE xi_request_type, uint32_t *const xo_bip_error_counter)
{
    uint32_t bip_errors;
    PON_ERROR_DTE gpon_error;

    gpon_error = gpon_get_bip_error_counter(&bip_errors);
    if (gpon_error != PON_NO_ERROR)
        return gpon_error;

    switch (xi_request_type)
    {
        case CE_COUNTER_STACK_REQUEST:

            /* Calculate received counters values */
            * xo_bip_error_counter = gs_gpon_database.rxpon_gpon_conters.stack_bip_error_counter + bip_errors;

            /* Clear GEM per port counters */
            gs_gpon_database.rxpon_gpon_conters.stack_bip_error_counter = 0;

            /* Update bridge per flow counters */
            gs_gpon_database.rxpon_gpon_conters.gmp_bip_error_counter += bip_errors;
            break;

        case CE_COUNTER_RDPA_REQUEST:
            /* Calculate received counters values */
            *xo_bip_error_counter = gs_gpon_database.rxpon_gpon_conters.gmp_bip_error_counter + bip_errors;

            /* Clear GEM per port counters */
            gs_gpon_database.rxpon_gpon_conters.gmp_bip_error_counter = 0;

            /* Update bridge per flow counters */
            gs_gpon_database.rxpon_gpon_conters.stack_bip_error_counter += bip_errors;
            break;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_bw_alloc_counter                                                */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Get BW allocation Counter                                          */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*  This function returns the BW allocation counter for a specific alloc id   */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*       xi_alloc_id - Alloc ID(0 - 1023)                                     */
/*                                                                            */
/*       xo_bw_counter - BW allocated counter                                 */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_ERROR_INVALID_STATE - Invalid state                                */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_bw_alloc_counter (uint16_t xi_alloc_id, uint32_t *const xo_bw_counter)
{
    uint8_t alloc_index_array[GPON_RX_ALLOCS_IN_ASSOCIATION_NUM];
    uint8_t table_index;
    uint8_t array_index;
    uint8_t counter_index = 0;
    bool alloc_id_found = BDMF_FALSE;
    int drv_error;

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Search for the alloc id in the alloc table */
    for (table_index = 0; table_index < MAX_NUMBER_OF_TCONTS; table_index++)
    {
        if (gs_gpon_database.tconts_table[table_index].alloc_id == xi_alloc_id)
        {
            alloc_id_found = BDMF_TRUE;
            break;
        }
    }

    if (!alloc_id_found)
    {
        return PON_ERROR_ALLOC_NOT_ASSIGNED;
    }

    drv_error = ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_get(&alloc_index_array[0], 
        &alloc_index_array[1], &alloc_index_array[2], &alloc_index_array[3]); 
    if (drv_error > 0)
    {
        return PON_ERROR_DRIVER_ERROR;
    }

    for(array_index = 0; array_index < GPON_RX_ALLOCS_IN_ASSOCIATION_NUM; array_index++)
    {
        if (alloc_index_array[array_index]==  table_index)
        {
            counter_index = array_index;
            break;
        }
    }

    if (array_index == GPON_RX_ALLOCS_IN_ASSOCIATION_NUM)
    {
        return PON_ERROR_ALLOC_NOT_ASSIGNED;
    }
    else
    {
        drv_error =  ag_drv_gpon_rx_pm_counter_bw_cnt_get(counter_index, xo_bw_counter);
        if (drv_error > 0)
        {
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_set_tcont_to_counter_group_association                           */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Set tcont index to counter group association.                  */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function configure tcont index to counter group association         */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_tx_set_tcont_to_counter_group_association(uint8_t tcont0,
    uint8_t tcont1, uint8_t tcont2, uint8_t tcont3) 
{
    int drv_error;

    drv_error = ag_drv_gpon_tx_general_configuration_qforcntr_set(tcont0, tcont1, tcont2, tcont3);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read tx valid bit due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_set_bw_recording_cfg                                             */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx -Set Bandwith Recording Configuration                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function set the configurations of the BW map recording mechanism   */
/*   controls the enable bit and the read interface configuraqtions           */
/*                                                                            */
/******************************************************************************/
static PON_ERROR_DTE gpon_rx_set_bw_recording_cfg(bdmf_boolean rcd_stop_mode,
    GPON_RX_BWMAP_RECORD_TYPE rcd_type, uint16_t  specific_alloc, bdmf_boolean rcd_enable) 
{
    gpon_rx_bwmap_record_config bwmap_cfg = {};
    int drv_error;

    switch (rcd_type)
    {
    case GPON_RX_BWMAP_RECORD_ALL_TYPE:            
        bwmap_cfg.rcd_all = BDMF_TRUE;               
        break;
    case GPON_RX_BWMAP_RECORD_ALL_ONU_TYPE:
        bwmap_cfg.rcd_all_onu = BDMF_TRUE;              
        break;
    case GPON_RX_BWMAP_RECORD_SPECIFIC_ALLOC_TYPE:
        bwmap_cfg.rcd_specific_alloc = BDMF_TRUE;        
        break;
    }

    bwmap_cfg.rcd_stop_on_map_end = rcd_stop_mode;
    bwmap_cfg.secific_alloc = specific_alloc;

    drv_error = ag_drv_gpon_rx_bwmap_record_config_set(&bwmap_cfg);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to set BW map record due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    drv_error = ag_drv_gpon_rx_bwmap_record_enable_set(rcd_enable);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to enable BW map record due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_start_bw_recording                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Start BW recording                                                */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function starts the BW recording process, by giving the needed      */
/*   parameters of data type required, and specific alloc number              */
/*   to be recorded.                                                          */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   rcd_stop - Whether to stop on map end or to stop when memory is full.    */
/*              If asserted, only one map will be recorded and the recording  */
/*              will be stopped at the end of the first map after recording   */
/*              enable.                                                       */  
/*                                                                            */
/*   rcd_type - type of record:                                               */
/*              0 - Record all accesses (Alloc-ID paramter is ignored)        */
/*              1 - Record only the accesses which are directed to one        */
/*                  of the 40 TCONTs of the ONU.                              */                                                                              
/*              2 - Record only accesses of a specific alloc-id.              */
/*                                                                            */
/*   alloc_id - configurable specific alloc id to record the access.          */
/*              valid if rcd_type = 2                                         */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*     PON_NO_ERROR - No errors or faults                                     */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_start_bw_recording(bdmf_boolean rcd_stop, uint32_t record_type, 
    uint32_t alloc_id)
{
    PON_ERROR_DTE gpon_error;

    /* Check GPON stack state */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    gpon_error = gpon_rx_set_bw_recording_cfg(rcd_stop, record_type, alloc_id, BDMF_TRUE);
    if (gpon_error != PON_NO_ERROR)
    {
        return PON_ERROR_DRIVER_ERROR;
    }

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_stop_bw_recording                                                   */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Stop BW recording                                                 */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Stops BW recording process.                                              */
/*   BW recording must be disable before recording again.                     */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_stop_bw_recording(void)
{
    return gpon_rx_set_bw_recording_cfg(0, 0, 0, BDMF_FALSE);
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_bw_recording_data                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Get BW recording data                                          */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Returns the recorded data and number of accesses recorded.               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   accesses_array - Array storing the recorded data                         */
/*                                                                            */
/*   access_read_num - Number of accesses in recorded data (1-64)             */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*     PON_NO_ERROR - No errors or faults                                     */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_bw_recording_data(GPON_ACCESS *accesses_array,
    int *const access_read_num)
{
    int drv_error;
    bdmf_boolean rcd_done;
    bdmf_boolean rcd_mismatch;
    bdmf_boolean ecd_empty;
    bdmf_boolean rcd_enable;
    uint8_t rcd_last_ptr;
    uint8_t read_index;
    gpon_rx_bwmap_record_config bwmap_cfg = {};

    /* Check GPON stack state */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    ag_drv_gpon_rx_bwmap_record_enable_get(&rcd_enable);
    if (!rcd_enable)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "BW map recording is disabled");
        return PON_ERROR_INVALID_PARAMETER;
    }

    memset(accesses_array, 0, sizeof(GPON_ACCESS)*GPON_NUMBER_OF_ACCESS_IN_MAP);

    drv_error = ag_drv_gpon_rx_bwmap_record_status_get(&rcd_done, &rcd_mismatch, 
        &ecd_empty, &rcd_last_ptr);
    if (drv_error > 0)
    {
        return PON_ERROR_DRIVER_ERROR;
    }

    /* Workaround -> RCD_MISMATCH ia always set when recording 
       until memory is full (RCD_STOP_ON_MAP_END=0) */
    drv_error = ag_drv_gpon_rx_bwmap_record_config_get(&bwmap_cfg);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to read BW map record cfg due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    if (!bwmap_cfg.rcd_stop_on_map_end)
        rcd_last_ptr = GPON_NUMBER_OF_ACCESS_IN_MAP;
    else if (ecd_empty)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "No access was recorded\n");
        return PON_ERROR_INVALID_INFO;
    }

    if (rcd_done)
    {
        for (read_index = 0; read_index < rcd_last_ptr; read_index++)
        {
            drv_error = ag_drv_gpon_rx_bwmap_record_read_if_set(read_index);
            if (drv_error > 0) 
            { 
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Failed to configure rcd address to read index=%d due to driver error %d\n", read_index, drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }

            /* bits 63:32 are located at the lower address, bits 31:0 at the higher address */
            drv_error = ag_drv_gpon_rx_bwmap_record_rdata_get(0, &accesses_array[read_index].data.upper);
            if (drv_error > 0) 
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Failed to read rcd data read index=%d upper register due to driver error %d\n", read_index, drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }

            drv_error = ag_drv_gpon_rx_bwmap_record_rdata_get(1, &accesses_array[read_index].data.lower);
            if (drv_error > 0) 
            { 
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                   "Failed to read rcd data read index=%d lower register due to driver error %d\n", read_index, drv_error);
                return PON_ERROR_DRIVER_ERROR;
            }

            *access_read_num = read_index + 1;    
        }
    }
    else
        *access_read_num =  0;

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_read_bw_record                                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Read BW recording data                                         */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   Returns the recorded data and number of accesses recorded.               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   accesses_array - Array storing the recorded data                         */
/*                                                                            */
/*   access_read_num - Number of accesses in recorded data (1-64)             */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Success or error returned code                           */
/*     PON_NO_ERROR - No errors or faults                                     */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_read_bw_record(PON_ACCESS *record_data,
    int *const access_read_num)
{
    record_data->access_type = 0;
    return gpon_get_bw_recording_data(record_data->gpon_access, access_read_num);
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_get_rx_pm_counters                                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON - Read PM counters                                                   */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the PM counters collected by the GPON stack        */
/*   This function is synchronous                                             */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*   xo_rxpon_pm_counters - PM counters output structure                      */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_ERROR_INVALID_STATE - Invalid state                                */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_get_rx_pm_counters(GPON_RXPON_PM_COUNTERS_DTE *const xo_rxpon_pm_counters)
{
    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    ag_drv_gpon_rx_pm_counter_bwmaperr_get(&xo_rxpon_pm_counters->bw_map_error_counter);
    ag_drv_gpon_rx_pm_counter_ploamerr_get(&xo_rxpon_pm_counters->ploam_crc_error_counter);
    ag_drv_gpon_rx_pm_counter_plenderr_get(&xo_rxpon_pm_counters->plend_crc_error_counter);
    ag_drv_gpon_rx_pm_counter_sfmismatch_get(&xo_rxpon_pm_counters->superframe_mismatch_counter);
    ag_drv_gpon_rx_pm_counter_bip_get(&xo_rxpon_pm_counters->bip_error_counter);
    ag_drv_gpon_rx_pm_counter_gemfrag_get(&xo_rxpon_pm_counters->gem_fragments_counter);
    ag_drv_gpon_rx_pm_counter_symerror_get(&xo_rxpon_pm_counters->fec_corrected_bytes_counter);
    ag_drv_gpon_rx_pm_counter_biterror_get(&xo_rxpon_pm_counters->fec_corrected_bits_counter);
    ag_drv_gpon_rx_pm_counter_ethfrag_get(&xo_rxpon_pm_counters->eth_fragments_counter);
    ag_drv_gpon_rx_pm_counter_fecccwcnt_get(&xo_rxpon_pm_counters->fec_corrected_codeword_counter);

    return PON_NO_ERROR;
}


PON_ERROR_DTE gpon_get_burst_prof(rdpa_gpon_burst_prof_t * const xo_burst_prof, const int index)
{
    p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, "gpon_get_burst_prof() should not be invoked!");
    return PON_ERROR_INVALID_STATE;
}


