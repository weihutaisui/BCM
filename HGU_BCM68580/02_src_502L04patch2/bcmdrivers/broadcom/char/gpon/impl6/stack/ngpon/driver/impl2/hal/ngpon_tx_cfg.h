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

#ifndef _BCM6858_NGPON_TX_CFG_AG_H_
#define _BCM6858_NGPON_TX_CFG_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
typedef struct
{
    uint8_t scrmblr_en;
    uint8_t bip_en;
    uint8_t fec_en;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t enc_en;
    uint8_t reserved0;
    uint32_t s;
} ngpon_tx_cfg_func_en;


/**************************************************************************************************/
/* dv_pol: DV_polarity - DV output polarity. This bit determines whether the bit pattern specifie */
/*         d for DV signal is inverted or not.                                                    */
/* dv_setup_len: DV_setup_length - length of the DV setup in words.The DV during the setup period */
/*                is composed of (DV_SETUP_LEN-1) DV words of DV_POL followed by one DV setup wor */
/*               d is according to the DV_HOLD_PAT_SRC and DV_POL configuartion.                  */
/* dv_hold_len: DV_hold_length - length of the DV hold in words.The first DV word is according to */
/*               the DV_HOLD_PAT_SRC and DV_POL and the (DV_HOLD_LEN-1) following DV words are ac */
/*              cording to the DV_POL.                                                            */
/* dv_setup_pat_src: DV_setup_pattern_source - Select the DV pattern from the DV_SETUP_PAT regist */
/*                   er and according to the polarity configuration or only acording to the DV_po */
/*                   larity                                                                       */
/* dv_hold_pat_src: DV_hold_pattern_source - Select the DV pattern from the DV_HOLD_PAT register  */
/*                  and according to the polarity configuration or only acording to the DV_polari */
/*                  ty                                                                            */
/**************************************************************************************************/
typedef struct
{
    uint8_t dv_pol;
    uint8_t dv_setup_len;
    uint8_t dv_hold_len;
    uint8_t dv_setup_pat_src;
    uint8_t dv_hold_pat_src;
} ngpon_tx_cfg_dv_cfg;


/**************************************************************************************************/
/* occupy: OCCUPIED - The number of valid PLOAMs entries in the PLOAM 0 FIFO                      */
/* rd_adr: rd_adress - The address that then next PLOAM0 will be read from                        */
/* wr_adr: wr_adress - The address that then next PLOAM0 will be written to                       */
/* af: almost_full - Set when the FIFO has space for one more PLOAM                               */
/* f: full - FIFO full indication                                                                 */
/* ae: almost_empty - Set when the FIFO has only one valid PLOAM in it                            */
/* e: empty - Set when the FIFO is empty                                                          */
/**************************************************************************************************/
typedef struct
{
    uint8_t occupy;
    uint8_t rd_adr;
    uint8_t wr_adr;
    uint8_t af;
    uint8_t f;
    uint8_t ae;
    uint8_t e;
} ngpon_tx_cfg_plm_0_stat;


/**************************************************************************************************/
/* big_brst_filt_en: burst_too_big_filter_enable - If this bit is set all the access that cause t */
/*                   he accumulate grant sizes of the burst to exceeds frame size (9720) will not */
/*                    be executed.                                                                */
/* big_grnt_filt_en: grant_too_big_filter_enable - When this bit is set access with grant size bi */
/*                   gger then 9718 will not be executed.                                         */
/* too_many_accs_filt_en: too_many_access_filter_enable - If this bit is set all the access that  */
/*                        fallows the 16th access of a burst will not be executed                 */
/* not_first_accs_plm_filt_en: not_first_access_ploam_filter_enable - PLOAM indication can be set */
/*                              only in the first access of a burst.If a PLOAM bit is set in any  */
/*                             other burst access and this bit is set, that access will be filter */
/*                             ed.                                                                */
/* late_accs_filt_en: late_access_filter_enable - If an access is read by the ONU-TX and its star */
/*                    t time already passed and this bit is set, the access and all the fallowing */
/*                     burst access will not be executed.                                         */
/* wrng_gnt_size_filt_en: wrong_grant_size_filter_enable - Accesses with grat size of 2 or 3 are  */
/*                        illegal. When this bit is set such access will be filter, not be execut */
/*                        ed.                                                                     */
/* prof_filt_en: profile_filter_enable - If this bit is set every access that its profile_en bit  */
/*               isnt set, will not be executed.                                                  */
/* trim_too_big_grnt_en: trim_too_big_grant_enable - When this bit is set, the allocation which c */
/*                       rosses 9718 is truncated at 9718.Next allocations are discarded.         */
/* start_time_filt_en: start_time_too_big_filter_enable - When this bit is set access with start  */
/*                     time bigger then 9719 will not be executed.                                */
/**************************************************************************************************/
typedef struct
{
    uint8_t big_brst_filt_en;
    uint8_t big_grnt_filt_en;
    uint8_t too_many_accs_filt_en;
    uint8_t not_first_accs_plm_filt_en;
    uint8_t late_accs_filt_en;
    uint8_t wrng_gnt_size_filt_en;
    uint8_t prof_filt_en;
    uint8_t trim_too_big_grnt_en;
    uint8_t start_time_filt_en;
} ngpon_tx_cfg_af_err_filt;

int ag_drv_ngpon_tx_cfg_onu_tx_en_set(uint8_t transmitter_enable, uint8_t standard, uint8_t speed);
int ag_drv_ngpon_tx_cfg_onu_tx_en_get(uint8_t *transmitter_enable, uint8_t *standard, uint8_t *speed);
int ag_drv_ngpon_tx_cfg_func_en_set(const ngpon_tx_cfg_func_en *func_en);
int ag_drv_ngpon_tx_cfg_func_en_get(ngpon_tx_cfg_func_en *func_en);
int ag_drv_ngpon_tx_cfg_dat_pol_set(uint8_t tx_data_out_polarity);
int ag_drv_ngpon_tx_cfg_dat_pol_get(uint8_t *tx_data_out_polarity);
int ag_drv_ngpon_tx_cfg_tx_rng_dly_set(uint32_t tx_word_delay, uint8_t tx_frame_delay, uint8_t tx_bit_delay);
int ag_drv_ngpon_tx_cfg_tx_rng_dly_get(uint32_t *tx_word_delay, uint8_t *tx_frame_delay, uint8_t *tx_bit_delay);
int ag_drv_ngpon_tx_cfg_dv_cfg_set(const ngpon_tx_cfg_dv_cfg *dv_cfg);
int ag_drv_ngpon_tx_cfg_dv_cfg_get(ngpon_tx_cfg_dv_cfg *dv_cfg);
int ag_drv_ngpon_tx_cfg_dv_setup_pat_set(uint32_t value);
int ag_drv_ngpon_tx_cfg_dv_setup_pat_get(uint32_t *value);
int ag_drv_ngpon_tx_cfg_dv_hold_pat_set(uint32_t value);
int ag_drv_ngpon_tx_cfg_dv_hold_pat_get(uint32_t *value);
int ag_drv_ngpon_tx_cfg_dat_pat_cfg_set(uint8_t dat_pat_type, uint8_t setup_len, uint8_t hold_len);
int ag_drv_ngpon_tx_cfg_dat_pat_cfg_get(uint8_t *dat_pat_type, uint8_t *setup_len, uint8_t *hold_len);
int ag_drv_ngpon_tx_cfg_dat_pat_set(uint32_t value);
int ag_drv_ngpon_tx_cfg_dat_pat_get(uint32_t *value);
int ag_drv_ngpon_tx_cfg_dat_setup_pat_set(uint32_t tx_cfg_data_setup_pat_word, uint32_t value);
int ag_drv_ngpon_tx_cfg_dat_setup_pat_get(uint32_t tx_cfg_data_setup_pat_word, uint32_t *value);
int ag_drv_ngpon_tx_cfg_dat_hold_pat_set(uint32_t tx_cfg_data_hold_pat_word, uint32_t value);
int ag_drv_ngpon_tx_cfg_dat_hold_pat_get(uint32_t tx_cfg_data_hold_pat_word, uint32_t *value);
int ag_drv_ngpon_tx_cfg_tcont_to_onu_id_map_set(uint32_t ngpon_tx_onu_id_elem, uint8_t onu_id);
int ag_drv_ngpon_tx_cfg_tcont_to_onu_id_map_get(uint32_t ngpon_tx_onu_id_elem, uint8_t *onu_id);
int ag_drv_ngpon_tx_cfg_tcont_flush_set(uint8_t flsh_q, uint8_t flsh_en, uint8_t flsh_imm);
int ag_drv_ngpon_tx_cfg_tcont_flush_get(uint8_t *flsh_q, uint8_t *flsh_en, uint8_t *flsh_imm);
int ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_set(uint32_t ngpon_tx_onu_id_elem, uint16_t onu_id, uint8_t plm_num, uint8_t flbk_plm_num);
int ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_get(uint32_t ngpon_tx_onu_id_elem, uint16_t *onu_id, uint8_t *plm_num, uint8_t *flbk_plm_num);
int ag_drv_ngpon_tx_cfg_tx_fifo_grp_cfg_set(uint32_t ngpon_tx_fifo_grp, uint16_t base, uint16_t size);
int ag_drv_ngpon_tx_cfg_tx_fifo_grp_cfg_get(uint32_t ngpon_tx_fifo_grp, uint16_t *base, uint16_t *size);
int ag_drv_ngpon_tx_cfg_tx_fifo_q_cfg_set(uint32_t ngpon_tx_fifo_q, uint16_t base, uint16_t size);
int ag_drv_ngpon_tx_cfg_tx_fifo_q_cfg_get(uint32_t ngpon_tx_fifo_q, uint16_t *base, uint16_t *size);
int ag_drv_ngpon_tx_cfg_pd_q_cfg_set(uint32_t ngpon_tx_pd_q, uint16_t base, uint16_t size);
int ag_drv_ngpon_tx_cfg_pd_q_cfg_get(uint32_t ngpon_tx_pd_q, uint16_t *base, uint16_t *size);
int ag_drv_ngpon_tx_cfg_pd_grp_cfg_set(uint32_t ngpon_tx_pd_grp, uint16_t base);
int ag_drv_ngpon_tx_cfg_pd_grp_cfg_get(uint32_t ngpon_tx_pd_grp, uint16_t *base);
int ag_drv_ngpon_tx_cfg_dbr_cfg_set(uint8_t dbr_src, uint8_t dbr_flush);
int ag_drv_ngpon_tx_cfg_dbr_cfg_get(uint8_t *dbr_src, uint8_t *dbr_flush);
int ag_drv_ngpon_tx_cfg_dbr_set(uint32_t ngpon_tx_dbr, uint32_t bufocc);
int ag_drv_ngpon_tx_cfg_dbr_get(uint32_t ngpon_tx_dbr, uint32_t *bufocc);
int ag_drv_ngpon_tx_cfg_xgem_pyld_min_len_set(uint16_t value);
int ag_drv_ngpon_tx_cfg_xgem_pyld_min_len_get(uint16_t *value);
int ag_drv_ngpon_tx_cfg_tcont_to_cnt_grp_map_set(uint8_t grp_0_q_num, uint8_t grp_1_q_num, uint8_t grp_2_q_num, uint8_t grp_3_q_num);
int ag_drv_ngpon_tx_cfg_tcont_to_cnt_grp_map_get(uint8_t *grp_0_q_num, uint8_t *grp_1_q_num, uint8_t *grp_2_q_num, uint8_t *grp_3_q_num);
int ag_drv_ngpon_tx_cfg_plm_cfg_set(uint32_t tx_cfg_ploam_cfg_word, uint8_t prsstnt, uint8_t valid, uint8_t sw_en, uint8_t use_def);
int ag_drv_ngpon_tx_cfg_plm_cfg_get(uint32_t tx_cfg_ploam_cfg_word, uint8_t *prsstnt, uint8_t *valid, uint8_t *sw_en, uint8_t *use_def);
int ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_set(uint8_t rogue_level_clr, uint8_t rogue_diff_clr, uint8_t rogue_dv_sel);
int ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_get(uint8_t *rogue_level_clr, uint8_t *rogue_diff_clr, uint8_t *rogue_dv_sel);
int ag_drv_ngpon_tx_cfg_rogue_level_time_set(uint32_t window_size);
int ag_drv_ngpon_tx_cfg_rogue_level_time_get(uint32_t *window_size);
int ag_drv_ngpon_tx_cfg_rogue_diff_time_set(uint16_t window_size);
int ag_drv_ngpon_tx_cfg_rogue_diff_time_get(uint16_t *window_size);
int ag_drv_ngpon_tx_cfg_lpb_q_num_set(uint8_t value);
int ag_drv_ngpon_tx_cfg_lpb_q_num_get(uint8_t *value);
int ag_drv_ngpon_tx_cfg_xgem_hdr_opt_set(uint32_t value);
int ag_drv_ngpon_tx_cfg_xgem_hdr_opt_get(uint32_t *value);
int ag_drv_ngpon_tx_cfg_plm_0_stat_get(ngpon_tx_cfg_plm_0_stat *plm_0_stat);
int ag_drv_ngpon_tx_cfg_line_rate_set(uint8_t value);
int ag_drv_ngpon_tx_cfg_line_rate_get(uint8_t *value);
int ag_drv_ngpon_tx_cfg_start_offset_set(uint16_t value);
int ag_drv_ngpon_tx_cfg_start_offset_get(uint16_t *value);
int ag_drv_ngpon_tx_cfg_frame_length_minus_1_set(uint32_t value);
int ag_drv_ngpon_tx_cfg_frame_length_minus_1_get(uint32_t *value);
int ag_drv_ngpon_tx_cfg_idle_xgem_pyld_len_set(uint16_t value);
int ag_drv_ngpon_tx_cfg_idle_xgem_pyld_len_get(uint16_t *value);
int ag_drv_ngpon_tx_cfg_af_err_filt_set(const ngpon_tx_cfg_af_err_filt *af_err_filt);
int ag_drv_ngpon_tx_cfg_af_err_filt_get(ngpon_tx_cfg_af_err_filt *af_err_filt);
int ag_drv_ngpon_tx_cfg_dying_gasp_set(uint8_t dg);
int ag_drv_ngpon_tx_cfg_dying_gasp_get(uint8_t *dg);
int ag_drv_ngpon_tx_cfg_fragmentation_disable_set(uint8_t frgment_dis);
int ag_drv_ngpon_tx_cfg_fragmentation_disable_get(uint8_t *frgment_dis);
int ag_drv_ngpon_tx_cfg_xgtc_header_ind_7_1_set(uint8_t bits_7_1);
int ag_drv_ngpon_tx_cfg_xgtc_header_ind_7_1_get(uint8_t *bits_7_1);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_ngpon_tx_cfg_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

