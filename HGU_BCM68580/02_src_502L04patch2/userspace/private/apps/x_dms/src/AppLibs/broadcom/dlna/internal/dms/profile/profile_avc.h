/******************************************************************************
 *    (c)2009-2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 *****************************************************************************/
typedef enum {
    AVC_VIDEO_PROFILE_eINVALID,
    AVC_VIDEO_PROFILE_eBL_QCIF15,
    AVC_VIDEO_PROFILE_eBL_L1B_QCIF,
    AVC_VIDEO_PROFILE_eBL_L12_CIF15,
    AVC_VIDEO_PROFILE_eBL_CIF15,
    AVC_VIDEO_PROFILE_eBL_CIF15_520,
    AVC_VIDEO_PROFILE_eBL_CIF15_540,
    AVC_VIDEO_PROFILE_eBL_L2_CIF30,
    AVC_VIDEO_PROFILE_eBL_CIF30,
    AVC_VIDEO_PROFILE_eBL_CIF30_940,
    AVC_VIDEO_PROFILE_eBL_L3L_SD,
    AVC_VIDEO_PROFILE_eBL_L3_SD,
    AVC_VIDEO_PROFILE_eMP_SD,
    AVC_VIDEO_PROFILE_eMP_HD,
    AVC_VIDEO_PROFILE_eHP_HD
}AVC_Video_Profile;

typedef struct video_properties_s {
    int width;
    int height;
}Video_Properties;

static Video_Properties profile_cif_res[] = {
    { 352, 288 }, /* CIF */
    { 352, 240 }, /* 525SIF */
    { 352, 264 }, /*  */
    { 320, 240 }, /* QVGA 4:3 */
    { 320, 180 }, /* QVGA 16:9 */
    { 240, 180 }, /* 1/7 VGA 4:3 */
    { 240, 135 }, /* 1/7 VGA 16:9 */
    { 208, 160 }, /* 1/9 VGA 4:3 */
    { 176, 144 }, /* QCIF,625QCIF */
    { 176, 120 }, /* 525QCIF */
    { 160, 120 }, /* SQVGA 4:3 */
    { 160, 112 }, /* 1/16 VGA 4:3 */
    { 160, 90  }, /* SQVGA 16:9 */
    { 128, 96  }  /* SQCIF */
};

static Video_Properties profile_mp_l3_sd_res[] = {
    { 720, 576 }, /* 625 D1 */
    { 720, 480 }, /* 525 D1 */
    { 640, 480 }, /* VGA */
    { 640, 360 }  /* VGA 16:9 */
};

static Video_Properties profile_mp_sd_res[] = {
    { 720, 576 }, /* 625 D1 */
    { 720, 480 }, /* 525 D1 */
    { 704, 576 }, /* 625 4SIF */
    { 704, 480 }, /* 525 4SIF */
    { 640, 480 }, /* VGA */
    { 640, 360 }, /* VGA 16:9 */
    { 544, 576 }, /* 625 3/4 D1 */
    { 544, 480 }, /* 525 3/4 D1 */
    { 480, 576 }, /* 625 2/3 D1 */
    { 480, 480 }, /* 525 2/3 D1 */
    { 480, 360 }, /* 9/16 VGA 4:3 */
    { 472, 360 }, /* 9/16 VGA 4:3 */
    { 480, 270 }, /* 9/16 VGA 16:9 */
    { 480, 288 }, /*  */
    { 352, 576 }, /* 625 1/2 D1 */
    { 352, 480 }, /* 525 1/2 D1 */
    { 352, 288 }, /* CIF, 625SIF */  
    { 352, 240 }, /* 525SIF */
    { 320, 240 }, /* QVGA 4:3 */
    { 320, 180 }, /* QVGA 16:9 */
    { 240, 180 }, /* 1/7 VGA 4:3 */
    { 208, 160 }, /* 1/9 VGA 4:3 */
    { 176, 144 }, /* QCIF,625QCIF */
    { 176, 120 }, /* 525QCIF */
    { 160, 120 }, /* SQVGA 4:3 */
    { 160, 112 }, /* 1/16 VGA 4:3 */
    { 160, 90  }, /* SQVGA 16:9 */
    { 128, 96  }  /* SQCIF */
};

static Video_Properties profile_mp_hd_res[] = {
    { 1920, 1080 }, /* 1080p */
    { 1920, 1152 },
    { 1920, 540  }, /* 1080i */
    { 1280, 720  }  /* 720p */
};

static Video_Properties profile_hp_hd_res[] = {
    { 1920, 1080 }, /* 1080p */
    { 1920, 1152  }, /* 50i, 25p */
    { 1280, 720  }  /* 720p */
};

/********************/
/* MPEG-4 Container */
/********************/

static dms_dlna_profile avc_mp4_mp_sd_aac_mult5 = {
    "AVC_MP4_MP_SD_AAC_MULT5",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_mp_sd_heaac_l2 = {
    "AVC_MP4_MP_SD_HEAAC_L2",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_mp_sd_mpeg1_l3 = {
    "AVC_MP4_MP_SD_MPEG1_L3",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_mp_sd_ac3 = {
    "AVC_MP4_MP_SD_AC3",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_mp_sd_aac_ltp = {
    "AVC_MP4_MP_SD_AAC_LTP",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_mp_sd_aac_ltp_mult5 = {
    "AVC_MP4_MP_SD_AAC_LTP_MULT5",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_mp_sd_aac_ltp_mult7 = {
    "AVC_MP4_MP_SD_AAC_LTP_MULT7",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_mp_sd_atrac3plus = {
    "AVC_MP4_MP_SD_ATRAC3plus",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_bl_l3l_sd_aac = {
    "AVC_MP4_BL_L3L_SD_AAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_bl_l3l_sd_heaac = {
    "AVC_MP4_BL_L3L_SD_HEAAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_bl_l3_sd_aac = {
    "AVC_MP4_BL_L3_SD_AAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_mp_sd_bsac = {
    "AVC_MP4_MP_SD_BSAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_mp4_bl_cif30_aac_mult5 = {
    "AVC_MP4_BL_CIF30_AAC_MULT5",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_mp4_bl_cif30_heaac_l2 = {
    "AVC_MP4_BL_CIF30_HEAAC_L2",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_mp4_bl_cif30_mpeg1_l3 = {
    "AVC_MP4_BL_CIF30_MPEG1_L3",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_mp4_bl_cif30_ac3 = {
    "AVC_MP4_BL_CIF30_AC3",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_mp4_bl_cif30_aac_ltp = {
    "AVC_MP4_BL_CIF30_AAC_LTP",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_mp4_bl_cif30_aac_ltp_mult5 = {
    "AVC_MP4_BL_CIF30_AAC_LTP_MULT5",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_mp4_bl_l2_cif30_aac = {
    "AVC_MP4_BL_L2_CIF30_AAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_mp4_bl_cif30_bsac = {
    "AVC_MP4_BL_CIF30_BSAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_mp4_bl_cif30_bsac_mult5 = {
    "AVC_MP4_BL_CIF30_BSAC_MULT5",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_mp4_bl_cif15_heaac = {
    "AVC_MP4_BL_CIF15_HEAAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_mp4_bl_cif15_amr = {
    "AVC_MP4_BL_CIF15_AMR",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_mp4_bl_cif15_aac = {
    "AVC_MP4_BL_CIF15_AAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_mp4_bl_cif15_aac_520 = {
    "AVC_MP4_BL_CIF15_AAC_520",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_mp4_bl_cif15_aac_ltp = {
    "AVC_MP4_BL_CIF15_AAC_LTP",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_mp4_bl_cif15_aac_ltp_520 = {
    "AVC_MP4_BL_CIF15_AAC_LTP_520",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_mp4_bl_cif15_bsac = {
    "AVC_MP4_BL_CIF15_BSAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_mp4_bl_l12_cif15_heaac = {
    "AVC_MP4_BL_L12_CIF15_HEAAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_mp4_bl_l1b_qcif15_heaac = {
    "AVC_MP4_BL_L1B_QCIF15_HEAAC",
    MIME_VIDEO_MPEG_4,
    LABEL_VIDEO_QCIF15
};

/*********************/
/* MPEG-TS Container */
/*********************/

static dms_dlna_profile avc_ts_mp_sd_aac_mult5 = {
    "AVC_TS_MP_SD_AAC_MULT5",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_mult5_t = {
    "AVC_TS_MP_SD_AAC_MULT5_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_mult5_iso = {
    "AVC_TS_MP_SD_AAC_MULT5_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_heaac_l2 = {
    "AVC_TS_MP_SD_HEAAC_L2",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_heaac_l2_t = {
    "AVC_TS_MP_SD_HEAAC_L2_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_heaac_l2_iso = {
    "AVC_TS_MP_SD_HEAAC_L2_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_mpeg1_l3 = {
    "AVC_TS_MP_SD_MPEG1_L3",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_mpeg1_l3_t = {
    "AVC_TS_MP_SD_MPEG1_L3_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_mpeg1_l3_iso = {
    "AVC_TS_MP_SD_MPEG1_L3_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_ac3 = {
    "AVC_TS_MP_SD_AC3",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_ac3_t = {
    "AVC_TS_MP_SD_AC3_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_ac3_iso = {
    "AVC_TS_MP_SD_AC3_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_ltp = {
    "AVC_TS_MP_SD_AAC_LTP",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_ltp_t = {
    "AVC_TS_MP_SD_AAC_LTP_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_ltp_iso = {
    "AVC_TS_MP_SD_AAC_LTP_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_ltp_mult5 = {
    "AVC_TS_MP_SD_AAC_LTP_MULT5",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_ltp_mult5_t = {
    "AVC_TS_MP_SD_AAC_LTP_MULT5_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_ltp_mult5_iso = {
    "AVC_TS_MP_SD_AAC_LTP_MULT5_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_ltp_mult7 = {
    "AVC_TS_MP_SD_AAC_LTP_MULT7",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_ltp_mult7_t = {
    "AVC_TS_MP_SD_AAC_LTP_MULT7_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_aac_ltp_mult7_iso = {
    "AVC_TS_MP_SD_AAC_LTP_MULT7_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_bsac = {
    "AVC_TS_MP_SD_BSAC",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_bsac_t = {
    "AVC_TS_MP_SD_BSAC_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_mp_sd_bsac_iso = {
    "AVC_TS_MP_SD_BSAC_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_SD
};

static dms_dlna_profile avc_ts_bl_cif30_aac_mult5 = {
    "AVC_TS_BL_CIF30_AAC_MULT5",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_mult5_t = {
    "AVC_TS_BL_CIF30_AAC_MULT5_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_mult5_iso = {
    "AVC_TS_BL_CIF30_AAC_MULT5_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_heaac_l2 = {
   "AVC_TS_BL_CIF30_HEAAC_L2",
   MIME_VIDEO_MPEG_TS,
   LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_heaac_l2_t = {
  "AVC_TS_BL_CIF30_HEAAC_L2_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_heaac_l2_iso = {
  "AVC_TS_BL_CIF30_HEAAC_L2_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_mpeg1_l3 = {
    "AVC_TS_BL_CIF30_MPEG1_L3",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_mpeg1_l3_t = {
  "AVC_TS_BL_CIF30_MPEG1_L3_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_mpeg1_l3_iso = {
  "AVC_TS_BL_CIF30_MPEG1_L3_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_ac3 = {
  "AVC_TS_BL_CIF30_AC3",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_ac3_t = {
  "AVC_TS_BL_CIF30_AC3_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_ac3_iso = {
  "AVC_TS_BL_CIF30_AC3_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_ltp = {
  "AVC_TS_BL_CIF30_AAC_LTP",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_ltp_t = {
  "AVC_TS_BL_CIF30_AAC_LTP_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_ltp_iso = {
  "AVC_TS_BL_CIF30_AAC_LTP_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_ltp_mult5 = {
  "AVC_TS_BL_CIF30_AAC_LTP_MULT5",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_ltp_mult5_t = {
  "AVC_TS_BL_CIF30_AAC_LTP_MULT5_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_ltp_mult5_iso = {
  "AVC_TS_BL_CIF30_AAC_LTP_MULT5_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_940 = {
  "AVC_TS_BL_CIF30_AAC_940",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_940_t = {
  "AVC_TS_BL_CIF30_AAC_940_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_bl_cif30_aac_940_iso = {
  "AVC_TS_BL_CIF30_AAC_940_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_ts_mp_hd_aac_mult5 = {
  "AVC_TS_MP_HD_AAC_MULT5",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_mult5_t = {
  "AVC_TS_MP_HD_AAC_MULT5_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_mult5_iso = {
  "AVC_TS_MP_HD_AAC_MULT5_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_heaac_l2 = {
  "AVC_TS_MP_HD_HEAAC_L2",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_heaac_l2_t = {
  "AVC_TS_MP_HD_HEAAC_L2_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_heaac_l2_iso = {
  "AVC_TS_MP_HD_HEAAC_L2_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_mpeg1_l3 = {
  "AVC_TS_MP_HD_MPEG1_L3",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_mpeg1_l3_t = {
  "AVC_TS_MP_HD_MPEG1_L3_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_mpeg1_l3_iso = {
  "AVC_TS_MP_HD_MPEG1_L3_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_ac3 = {
  "AVC_TS_MP_HD_AC3",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_ac3_t = {
  "AVC_TS_MP_HD_AC3_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_ac3_iso = {
  "AVC_TS_MP_HD_AC3_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac = {
  "AVC_TS_MP_HD_AAC",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_t = {
  "AVC_TS_MP_HD_AAC_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_iso = {
  "AVC_TS_MP_HD_AAC_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_ltp = {
  "AVC_TS_MP_HD_AAC_LTP",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_ltp_t = {
  "AVC_TS_MP_HD_AAC_LTP_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_ltp_iso = {
  "AVC_TS_MP_HD_AAC_LTP_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_ltp_mult5 = {
  "AVC_TS_MP_HD_AAC_LTP_MULT5",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_ltp_mult5_t = {
    "AVC_TS_MP_HD_AAC_LTP_MULT5_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_ltp_mult5_iso = {
    "AVC_TS_MP_HD_AAC_LTP_MULT5_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_ltp_mult7 = {
  "AVC_TS_MP_HD_AAC_LTP_MULT7",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_ltp_mult7_t = {
    "AVC_TS_MP_HD_AAC_LTP_MULT7_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_mp_hd_aac_ltp_mult7_iso = {
    "AVC_TS_MP_HD_AAC_LTP_MULT7_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_ts_bl_cif15_aac = {
    "AVC_TS_BL_CIF15_AAC",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_aac_t = {
    "AVC_TS_BL_CIF15_AAC_T",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_aac_iso = {
    "AVC_TS_BL_CIF15_AAC_ISO",
    MIME_VIDEO_MPEG,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_aac_540 = {
    "AVC_TS_BL_CIF15_AAC_540",
    MIME_VIDEO_MPEG_TS,
    LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_aac_540_t = {
  "AVC_TS_BL_CIF15_AAC_540_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_aac_540_iso = {
  "AVC_TS_BL_CIF15_AAC_540_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_aac_ltp = {
  "AVC_TS_BL_CIF15_AAC_LTP",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_aac_ltp_t = {
  "AVC_TS_BL_CIF15_AAC_LTP_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_aac_ltp_iso = {
  "AVC_TS_BL_CIF15_AAC_LTP_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_bsac = {
  "AVC_TS_BL_CIF15_BSAC",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_bsac_t = {
  "AVC_TS_BL_CIF15_BSAC_T",
  MIME_VIDEO_MPEG_TS,
  LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_ts_bl_cif15_bsac_iso = {
  "AVC_TS_BL_CIF15_BSAC_ISO",
  MIME_VIDEO_MPEG,
  LABEL_VIDEO_CIF15
};

/******************/
/* 3GPP Container */
/******************/

static dms_dlna_profile avc_3gpp_bl_cif30_amr_wbplus = {
  "AVC_3GPP_BL_CIF30_AMR_WBplus",
  MIME_VIDEO_3GP,
  LABEL_VIDEO_CIF30
};

static dms_dlna_profile avc_3gpp_bl_cif15_amr_wbplus = {
  "AVC_3GPP_BL_CIF15_AMR_WBplus",
  MIME_VIDEO_3GP,
  LABEL_VIDEO_CIF15
};

static dms_dlna_profile avc_3gpp_bl_qcif15_aac = {
    "AVC_3GPP_BL_QCIF15_AAC",
    MIME_VIDEO_3GP,
    LABEL_VIDEO_QCIF15
};

static dms_dlna_profile avc_3gpp_bl_qcif15_aac_ltp = {
    "AVC_3GPP_BL_QCIF15_AAC_LTP",
    MIME_VIDEO_3GP,
    LABEL_VIDEO_QCIF15
};

static dms_dlna_profile avc_3gpp_bl_qcif15_heaac = {
    "AVC_3GPP_BL_QCIF15_HEAAC",
    MIME_VIDEO_3GP,
    LABEL_VIDEO_QCIF15
};

static dms_dlna_profile avc_3gpp_bl_qcif15_amr_wbplus = {
    "AVC_3GPP_BL_QCIF15_AMR_WBplus",
    MIME_VIDEO_3GP,
    LABEL_VIDEO_QCIF15
};

static dms_dlna_profile avc_3gpp_bl_qcif15_amr = {
    "AVC_3GPP_BL_QCIF15_AMR",
    MIME_VIDEO_3GP,
    LABEL_VIDEO_QCIF15
};

/* MKV container */
static dms_dlna_profile avc_mkv_mp_hd_heaac_l4 = {
    "AVC_MKV_MP_HD_HEAAC_L4",
    MIME_VIDEO_MKV,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_mkv_mp_hd_ac3 = {
    "AVC_MKV_MP_HD_AC3",
    MIME_VIDEO_MKV,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_mkv_mp_hd_mpeg1_l3= {
    "AVC_MKV_MP_HD_MPEG1_L3",
    MIME_VIDEO_MKV,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_mkv_hp_hd_aac_mult5= {
    "AVC_MKV_HP_HD_AAC_MULT5",
    MIME_VIDEO_MKV,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_mkv_hp_hd_heaac_l4= {
    "AVC_MKV_HP_HD_HEAAC_L4",
    MIME_VIDEO_MKV,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_mkv_hp_hd_ac3= {
    "AVC_MKV_HP_HD_AC3",
    MIME_VIDEO_MKV,
    LABEL_VIDEO_HD
};

static dms_dlna_profile avc_mkv_hp_hd_mpeg1_l3= {
    "AVC_MKV_HP_HD_MPEG1_L3",
    MIME_VIDEO_MKV,
    LABEL_VIDEO_HD
};


static const struct {
    dms_dlna_profile *profile;
    DlnaTransportType st;
    AVC_Video_Profile vp;
    audio_profile_type ap;
}avc_profiles_mapping[] = {
  /* MPEG-4 Container */
    { &avc_mp4_mp_sd_aac_mult5, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_MULT5 },
    { &avc_mp4_mp_sd_heaac_l2, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_mp4_mp_sd_mpeg1_l3, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_MP3 },
    { &avc_mp4_mp_sd_ac3, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AC3 },
    { &avc_mp4_mp_sd_aac_ltp, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP },
    { &avc_mp4_mp_sd_aac_ltp_mult5, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP_MULT5 },
    { &avc_mp4_mp_sd_aac_ltp_mult7, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP_MULT7 },
    { &avc_mp4_mp_sd_atrac3plus, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_ATRAC },
    { &avc_mp4_mp_sd_bsac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_BSAC },

    { &avc_mp4_bl_l3l_sd_aac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_L3L_SD, AUDIO_PROFILE_AAC },
    { &avc_mp4_bl_l3l_sd_heaac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_L3L_SD, AUDIO_PROFILE_AAC_HE_L2 },

    { &avc_mp4_bl_l3_sd_aac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_L3_SD, AUDIO_PROFILE_AAC },

    { &avc_mp4_bl_cif30_aac_mult5, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_MULT5 },
    { &avc_mp4_bl_cif30_heaac_l2, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_mp4_bl_cif30_mpeg1_l3, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_MP3 },
    { &avc_mp4_bl_cif30_ac3, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AC3 },
    { &avc_mp4_bl_cif30_aac_ltp, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_LTP },
    { &avc_mp4_bl_cif30_aac_ltp_mult5, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_LTP_MULT5 },
    { &avc_mp4_bl_cif30_bsac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_BSAC },
    { &avc_mp4_bl_cif30_bsac_mult5, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_BSAC_MULT5 },

    { &avc_mp4_bl_l2_cif30_aac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_L2_CIF30, AUDIO_PROFILE_AAC },

    { &avc_mp4_bl_cif15_heaac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_mp4_bl_cif15_amr, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AMR },
    { &avc_mp4_bl_cif15_aac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC },
    { &avc_mp4_bl_cif15_aac_520, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF15_520, AUDIO_PROFILE_AAC },
    { &avc_mp4_bl_cif15_aac_ltp, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC_LTP },
    { &avc_mp4_bl_cif15_aac_ltp_520, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF15_520, AUDIO_PROFILE_AAC_LTP },
    { &avc_mp4_bl_cif15_bsac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC_BSAC },

    { &avc_mp4_bl_cif15_aac_520, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_L1B_QCIF, AUDIO_PROFILE_AAC }, // brcm-bca: csp#1041537, MCVT AVC_MP4_BL_CIF15_AAC_520 failed.

    { &avc_mp4_bl_l12_cif15_heaac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_L12_CIF15, AUDIO_PROFILE_AAC_HE_L2 },

    { &avc_mp4_bl_cif15_aac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_L12_CIF15, AUDIO_PROFILE_AAC },

    { &avc_mp4_bl_l1b_qcif15_heaac, DlnaTransportType_eMp4,
    AVC_VIDEO_PROFILE_eBL_L1B_QCIF, AUDIO_PROFILE_AAC_HE_L2 },

    /* MPEG-TS Container */
    { &avc_ts_mp_sd_aac_mult5, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_MULT5 },
    { &avc_ts_mp_sd_aac_mult5_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_MULT5 },
    { &avc_ts_mp_sd_aac_mult5_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_MULT5 },

    { &avc_ts_mp_sd_heaac_l2, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_ts_mp_sd_heaac_l2_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_ts_mp_sd_heaac_l2_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_HE_L2 },

    { &avc_ts_mp_sd_mpeg1_l3, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_MP3 },
    { &avc_ts_mp_sd_mpeg1_l3_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_MP3 },
    { &avc_ts_mp_sd_mpeg1_l3_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_MP3 },

    { &avc_ts_mp_sd_ac3, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AC3 },
    { &avc_ts_mp_sd_ac3_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AC3 },
    { &avc_ts_mp_sd_ac3_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AC3 },

    { &avc_ts_mp_sd_aac_ltp, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP },
    { &avc_ts_mp_sd_aac_ltp_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP },
    { &avc_ts_mp_sd_aac_ltp_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP },

    { &avc_ts_mp_sd_aac_ltp_mult5, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP_MULT5 },
    { &avc_ts_mp_sd_aac_ltp_mult5_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP_MULT5 },
    { &avc_ts_mp_sd_aac_ltp_mult5_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP_MULT5 },

    { &avc_ts_mp_sd_aac_ltp_mult7, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP_MULT7 },
    { &avc_ts_mp_sd_aac_ltp_mult7_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP_MULT7 },
    { &avc_ts_mp_sd_aac_ltp_mult7_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_LTP_MULT7 },

    { &avc_ts_mp_sd_bsac, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_BSAC },
    { &avc_ts_mp_sd_bsac_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_BSAC },
    { &avc_ts_mp_sd_bsac_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_SD, AUDIO_PROFILE_AAC_BSAC },

    { &avc_ts_bl_cif30_aac_mult5, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_MULT5 },
    { &avc_ts_bl_cif30_aac_mult5_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_MULT5 },
    { &avc_ts_bl_cif30_aac_mult5_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_MULT5 },

    { &avc_ts_bl_cif30_heaac_l2, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_ts_bl_cif30_heaac_l2_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_ts_bl_cif30_heaac_l2_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_HE_L2 },

    { &avc_ts_bl_cif30_mpeg1_l3, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_MP3 },
    { &avc_ts_bl_cif30_mpeg1_l3_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_MP3 },
    { &avc_ts_bl_cif30_mpeg1_l3_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_MP3 },

    { &avc_ts_bl_cif30_ac3, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AC3 },
    { &avc_ts_bl_cif30_ac3_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AC3 },
    { &avc_ts_bl_cif30_ac3_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AC3 },

    { &avc_ts_bl_cif30_aac_ltp, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_LTP },
    { &avc_ts_bl_cif30_aac_ltp_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_LTP },
    { &avc_ts_bl_cif30_aac_ltp_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_LTP },

    { &avc_ts_bl_cif30_aac_ltp_mult5, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_LTP_MULT5 },
    { &avc_ts_bl_cif30_aac_ltp_mult5_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_LTP_MULT5 },
    { &avc_ts_bl_cif30_aac_ltp_mult5_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AAC_LTP_MULT5 },

    { &avc_ts_bl_cif30_aac_940, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30_940, AUDIO_PROFILE_AAC },
    { &avc_ts_bl_cif30_aac_940_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF30_940, AUDIO_PROFILE_AAC },
    { &avc_ts_bl_cif30_aac_940_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF30_940, AUDIO_PROFILE_AAC },

    { &avc_ts_mp_hd_aac_mult5, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_MULT5 },
    { &avc_ts_mp_hd_aac_mult5_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_MULT5 },
    { &avc_ts_mp_hd_aac_mult5_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_MULT5 },

    { &avc_ts_mp_hd_heaac_l2, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_ts_mp_hd_heaac_l2_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_ts_mp_hd_heaac_l2_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_HE_L2 },

    { &avc_ts_mp_hd_mpeg1_l3, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_MP3 },
    { &avc_ts_mp_hd_mpeg1_l3_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_MP3 },
    { &avc_ts_mp_hd_mpeg1_l3_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_MP3 },

    { &avc_ts_mp_hd_ac3, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AC3 },
    { &avc_ts_mp_hd_ac3_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AC3 },
    { &avc_ts_mp_hd_ac3_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AC3 },

    { &avc_ts_mp_hd_aac, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC },
    { &avc_ts_mp_hd_aac_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC },
    { &avc_ts_mp_hd_aac_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC },

    { &avc_ts_mp_hd_aac_ltp, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_LTP },
    { &avc_ts_mp_hd_aac_ltp_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_LTP },
    { &avc_ts_mp_hd_aac_ltp_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_LTP },

    { &avc_ts_mp_hd_aac_ltp_mult5, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_LTP_MULT5 },
    { &avc_ts_mp_hd_aac_ltp_mult5_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_LTP_MULT5 },
    { &avc_ts_mp_hd_aac_ltp_mult5_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_LTP_MULT5 },

    { &avc_ts_mp_hd_aac_ltp_mult7, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_LTP_MULT7 },
    { &avc_ts_mp_hd_aac_ltp_mult7_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_LTP_MULT7 },
    { &avc_ts_mp_hd_aac_ltp_mult7_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_LTP_MULT7 },

    { &avc_ts_bl_cif15_aac, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC },
    { &avc_ts_bl_cif15_aac_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC },
    { &avc_ts_bl_cif15_aac_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC },

    { &avc_ts_bl_cif15_aac_540, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF15_540, AUDIO_PROFILE_AAC },
    { &avc_ts_bl_cif15_aac_540_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF15_540, AUDIO_PROFILE_AAC },
    { &avc_ts_bl_cif15_aac_540_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF15_540, AUDIO_PROFILE_AAC },

    { &avc_ts_bl_cif15_aac_ltp, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC_LTP },
    { &avc_ts_bl_cif15_aac_ltp_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC_LTP },
    { &avc_ts_bl_cif15_aac_ltp_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC_LTP },

    { &avc_ts_bl_cif15_bsac, DlnaTransportType_TS_NO_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC_BSAC },
    { &avc_ts_bl_cif15_bsac_t, DlnaTransportType_TS_TIMESTAMP,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC_BSAC },
    { &avc_ts_bl_cif15_bsac_iso, DlnaTransportType_eTs,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AAC_BSAC },

    /* 3GPP Container */
    { &avc_3gpp_bl_cif30_amr_wbplus, DlnaTransportType_3GP,
    AVC_VIDEO_PROFILE_eBL_CIF30, AUDIO_PROFILE_AMR_WB },

    { &avc_3gpp_bl_cif15_amr_wbplus, DlnaTransportType_3GP,
    AVC_VIDEO_PROFILE_eBL_CIF15, AUDIO_PROFILE_AMR_WB },

    { &avc_3gpp_bl_qcif15_aac, DlnaTransportType_3GP,
    AVC_VIDEO_PROFILE_eBL_QCIF15, AUDIO_PROFILE_AAC },
    { &avc_3gpp_bl_qcif15_aac_ltp, DlnaTransportType_3GP,
    AVC_VIDEO_PROFILE_eBL_QCIF15, AUDIO_PROFILE_AAC_LTP },
    { &avc_3gpp_bl_qcif15_heaac, DlnaTransportType_3GP,
    AVC_VIDEO_PROFILE_eBL_QCIF15, AUDIO_PROFILE_AAC_HE_L2 },
    { &avc_3gpp_bl_qcif15_amr_wbplus, DlnaTransportType_3GP,
    AVC_VIDEO_PROFILE_eBL_QCIF15, AUDIO_PROFILE_AMR_WB },
    { &avc_3gpp_bl_qcif15_amr, DlnaTransportType_3GP,
    AVC_VIDEO_PROFILE_eBL_QCIF15, AUDIO_PROFILE_AMR },

    /* MKV container */
    { &avc_mkv_mp_hd_heaac_l4 , DlnaTransportType_eMkv,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AAC_HE_L4},
    { &avc_mkv_mp_hd_ac3 , DlnaTransportType_eMkv,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_AC3},
    { &avc_mkv_mp_hd_mpeg1_l3 , DlnaTransportType_eMkv,
    AVC_VIDEO_PROFILE_eMP_HD, AUDIO_PROFILE_MP3},
    { &avc_mkv_hp_hd_aac_mult5 , DlnaTransportType_eMkv,
    AVC_VIDEO_PROFILE_eHP_HD, AUDIO_PROFILE_AAC_MULT5},
    { &avc_mkv_hp_hd_heaac_l4 , DlnaTransportType_eMkv,
    AVC_VIDEO_PROFILE_eHP_HD, AUDIO_PROFILE_AAC_HE_L4},
    { &avc_mkv_hp_hd_ac3 , DlnaTransportType_eMkv,
    AVC_VIDEO_PROFILE_eHP_HD, AUDIO_PROFILE_AC3},
    { &avc_mkv_hp_hd_mpeg1_l3 , DlnaTransportType_eMkv,
    AVC_VIDEO_PROFILE_eHP_HD, AUDIO_PROFILE_MP3},
    { NULL, 0, 0, 0 }
};
