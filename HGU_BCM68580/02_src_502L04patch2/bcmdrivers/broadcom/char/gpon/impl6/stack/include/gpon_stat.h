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

#ifndef GPON_STAT_H_INCLUDED
#define GPON_STAT_H_INCLUDED

#include <linux/types.h>
#include "pon_sm_common.h"

#define GPON_RX_ALLOCS_IN_ASSOCIATION_NUM 4
/* Max Counter Value */
#define CS_MAX_COUNTER_VALUE 0xFFFFFFFF 
#define MS_CHECK_AND_UPDATE_COUNTER_SATURATION(m, n) {if ((CS_MAX_COUNTER_VALUE - m - n) > 0)\
    m += n; else m = CS_MAX_COUNTER_VALUE;}

typedef enum
{
    CS_BER_INTERVAL_TYPE,
    CS_CRITICAL_BER_TYPE,
    CS_USER_TYPE
}
BIP8_MIRROR_TYPE_DTS;

/* BW Recording type */
typedef enum
{
    GPON_RX_BWMAP_RECORD_ALL_TYPE,
    GPON_RX_BWMAP_RECORD_ALL_ONU_TYPE,
    GPON_RX_BWMAP_RECORD_SPECIFIC_ALLOC_TYPE
}
GPON_RX_BWMAP_RECORD_TYPE;

/* RX PON counters  */
typedef struct
{
    uint32_t bw_map_error_counter;
    uint32_t ploam_crc_error_counter;
    uint32_t plend_crc_error_counter;
    uint32_t superframe_mismatch_counter;
    uint32_t bip_error_counter;
    uint32_t gem_fragments_counter;
    uint32_t eth_fragments_counter;
    uint32_t fec_corrected_bytes_counter;
    uint32_t fec_corrected_bits_counter;
    uint32_t fec_corrected_codeword_counter;
}
GPON_RXPON_PM_COUNTERS_DTE;

typedef struct
{
    uint32_t ber_interval_bip8_counter_mirror;
    uint32_t critical_ber_bip8_counter_mirror;
    uint32_t user_bip8_counter_mirror;
}
BIP8_COUNTERS;

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
    uint32_t *fec_corrected_codewords, uint32_t *fec_uncorrectable_codewords);


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
PON_ERROR_DTE gpon_rx_get_hec_counters(PON_RX_HEC_COUNTERS_DTE * const rxpon_hec_counter);


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
PON_ERROR_DTE gpon_get_bip_error_counter( uint32_t * const xo_bip_errors);

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
PON_ERROR_DTE gpon_get_rx_ploam_counters(PON_RX_PLOAM_COUNTERS_DTE *const xo_rxpon_ploam_counters);


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
    uint16_t *const xo_pd_counter);


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
    uint16_t *const xo_requested_ranging_ploam_number, uint16_t *const xo_requested_urgent_ploam_number);

PON_ERROR_DTE _gpon_get_tx_pm_counters(uint16_t *const xo_illegal_access_counter,
    uint32_t * const xo_idle_ploam_counter,
    uint32_t * const xo_ploam_counter);

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
    uint16_t * const xo_valid_access_counter);


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
PON_ERROR_DTE gpon_get_rx_unkonw_ploam_counter(uint32_t *const xo_rx_unknown_counters);


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
PON_ERROR_DTE gpon_get_encryption_error_counter(uint32_t *const xo_rx_encryption_error);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   p_read_bip8_error_counters                                               */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Read BIP8 Error counter                                           */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function returns the bip8 error counter                             */
/*                                                                            */
/******************************************************************************/
void p_read_bip8_error_counters(uint32_t *xi_bip_error, BIP8_MIRROR_TYPE_DTS xi_bip8_process_request);


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
PON_ERROR_DTE gpon_get_bw_alloc_counter (uint16_t xi_alloc_id, uint32_t *const xo_bw_counter);


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
    PON_RXPON_REQUEST_DTE xi_request_type, uint32_t *const xo_bip_error_counter);


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
    uint8_t tcont1, uint8_t tcont2, uint8_t tcont3);


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
    PON_RXPON_REQUEST_DTE xi_request_type, uint32_t *const xo_ploam_crc_errors);



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
    uint32_t alloc_id);


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
    int *const access_read_num);


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
PON_ERROR_DTE gpon_stop_bw_recording(void);


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
    int *const access_read_num);


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
PON_ERROR_DTE gpon_get_rx_pm_counters(GPON_RXPON_PM_COUNTERS_DTE *const xo_rxpon_pm_counters);

PON_ERROR_DTE gpon_get_burst_prof(rdpa_gpon_burst_prof_t * const xo_burst_prof, const int index);

#endif 
