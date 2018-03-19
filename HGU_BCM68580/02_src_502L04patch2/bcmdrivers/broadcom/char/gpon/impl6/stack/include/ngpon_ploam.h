/*
* <:copyright-BRCM:2015:proprietary:gpon
* 
*    Copyright (c) 2015 Broadcom 
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
#ifndef N_PLOAM_H_INCLUDED
#define N_PLOAM_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <packing.h>

/* Please Notice: The file supported for little endian byte orderying only! */

/*****************************************************************************/
/*                                                                           */
/* Constants                                                                 */
/*                                                                           */
/*****************************************************************************/

/* Broadcast ONU-ID */
#define BROADCAST_ONU_ID            ((uint16_t) 0x3FF)
/* XGS Broadcast ONU-ID for 10G channel*/
#define XGS_BROADCAST_ONU_ID        ((uint16_t) 0x3FE)
/* No ONU-ID assigned */
#define NO_ONU_ID_ASSIGNED_YET      ((uint16_t) 0x3FF)
#define MAX_VALID_ONU_ID            ((uint16_t) 0x3FC) 
/* PLOAM data size */
#define PLOAM_DATA_SIZE             (36)
/* MIC size */
#define PLOAM_MIC_SIZE              (8)
/* DS MIC Direction code */
#define DS_PLOAM_MIC_CDIR           (0x01)
/* US MIC Direction code */
#define US_PLOAM_MIC_CDIR           (0x02)

#define SERIAL_NUMBER_FIELD_SIZE    (8)

/* Downstream message IDs */
#define DS_BURST_PROFILE_MESSAGE_ID              ((uint8_t)0x01)
#define DS_ASSIGN_ONU_ID_MESSAGE_ID              ((uint8_t)0x03)
#define DS_RANGING_TIME_MESSAGE_ID               ((uint8_t)0x04)
#define DS_DEACTIVATE_ONU_ID_MESSAGE_ID          ((uint8_t)0x05)
#define DS_DISABLE_SERIAL_NUMBER_MESSAGE_ID      ((uint8_t)0x06)
#define DS_REQUEST_REGISTRATION_MESSAGE_ID       ((uint8_t)0x09)
#define DS_ASSIGN_ALLOC_ID_MESSAGE_ID            ((uint8_t)0x0A)
#define DS_KEY_CONTROL_MESSAGE_ID                ((uint8_t)0x0D)
#define DS_SLEEP_ALLOW_MESSAGE_ID                ((uint8_t)0x12)
#define DS_CALIBRATION_REQUEST_MESSAGE_ID        ((uint8_t)0x13)
#define DS_ADJUST_TX_WAVELENGTH_MESSAGE_ID       ((uint8_t)0x14)
#define DS_TUNING_CONTROL_MESSAGE_ID             ((uint8_t)0x15)
#define DS_SYSTEM_PROFILE_MESSAGE_ID             ((uint8_t)0x17)
#define DS_CHANNEL_PROFILE_MESSAGE_ID            ((uint8_t)0x18)
#define DS_PROTECTION_CONTROL_MESSAGE_ID         ((uint8_t)0x19)
#define DS_CHANGE_POWER_LEVEL_MESSAGE_ID         ((uint8_t)0x1A)
#define DS_POWER_CONSUMPTION_INQUIRE_MESSAGE_ID  ((uint8_t)0x1B)
#define DS_RATE_CONTROL_MESSAGE_ID               ((uint8_t)0x1C)

/* Upstream message IDs */
#define US_SERIAL_NUMBER_ONU_MESSAGE_ID          ((uint8_t)0x01)
#define US_REGISTRATION_MESSAGE_ID               ((uint8_t)0x02)
#define US_KEY_REPORT_MESSAGE_ID                 ((uint8_t)0x05)
#define US_ACKNOWLEDGE_MESSAGE_ID                ((uint8_t)0x09)
#define US_SLEEP_REQUEST_MESSAGE_ID              ((uint8_t)0x10)
#define US_TUNING_RESPONSE_MESSAGE_ID            ((uint8_t)0x1A)
#define US_POWER_CONSUMPTION_REPORT_MESSAGE_ID   ((uint8_t)0x1B)
#define US_RATE_RESPONSE_MESSAGE_ID              ((uint8_t)0x1C)

/*****************************************************************************/
/*                                                                           */
/* Downstream messages                                                       */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Profile                                                                   */
/*****************************************************************************/
#define DELIMITER_MAX_LENGTH  8
#define PREAMBLE_MAX_LENGTH   8
#define PON_TAG_SIZE          8

typedef struct
{
    uint8_t index: 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t applicability_to_specific_line_rate: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t version: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ DS_PROFILE_CONTROL;

typedef struct
{
    uint8_t fec: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ us_fec_indication;

typedef struct
{
    uint8_t length: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ delimiter_length_in_octets;

typedef struct
{
    uint8_t length: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ preamble_length_in_octets;

typedef struct
{
    DS_PROFILE_CONTROL profile_index           __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    us_fec_indication fec_indiciation          __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    delimiter_length_in_octets delimiter_length __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t delimiter [ DELIMITER_MAX_LENGTH ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    preamble_length_in_octets preamble_length  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t preamble_repeat_counter            __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t preamble [ PREAMBLE_MAX_LENGTH ]   __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t pon_tag [ PON_TAG_SIZE ]             __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified [ 7 ]                  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_BURST_PROFILE_PLOAM;

/*****************************************************************************/
/* Assign ONU-ID                                                             */
/*****************************************************************************/
typedef struct
{
    uint16_t assigned_onu_id     __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t serial_number [ SERIAL_NUMBER_FIELD_SIZE ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified [ 26 ]   __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_ASSIGN_ONU_ID_PLOAM;

/*****************************************************************************/
/* Ranging time                                                              */
/*****************************************************************************/

#define P_ABSOLUTE       ( 1 )
#define P_RELATIVE       ( 0 )
#define SIGN_POSITIVE    ( 0 )
#define SIGN_NEGATIVE    ( 1 )
#define delay_for_current_wavelength 0
#define delay_for_specified_wavelength_pair 1

typedef struct
{
    uint8_t absolute_or_relative  : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t sign                  : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t is_current_wavelength : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t reserved              : 5 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ ds_ranging_time_control;

#define DS_EQUALIZATION_DELAY_FIELD_SIZE ( 4 )

typedef struct
{
    ds_ranging_time_control options __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t equalization_delay[DS_EQUALIZATION_DELAY_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ds_pon_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t us_pon_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[23] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_RANGING_TIME_PLOAM;


/*****************************************************************************/
/* Deactivate ONU-ID                                                         */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified [ 36 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_DEACTIVATE_ONU_ID_PLOAM ;


/*****************************************************************************/
/* Disable serial number                                                     */
/*****************************************************************************/
#define SERIAL_NUMBER_UNICAST_DISABLE   0xFF
#define SERIAL_NUMBER_UNICAST_ENABLE    0x00
#define SERIAL_NUMBER_BROADCAST_DISABLE 0x0F
#define SERIAL_NUMBER_BROADCAST_ENABLE  0xF0
#define SERIAL_NUMBER_DISCOVERY_DISABLE 0x3F

typedef struct
{
    uint8_t control __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t serial_number[SERIAL_NUMBER_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[27]  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_DISABLE_SERIAL_NUMBER_PLOAM;

/*****************************************************************************/
/* Request registration message                                              */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified [ 36 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_REQUEST_REGISTRATION_PLOAM ;


/*****************************************************************************/
/* Assign Alloc-ID                                                           */
/*****************************************************************************/
#define ALLOC_TYPE_XGEM     1 
#define ALLOC_TYPE_DEALLOC  255
#define ALLOC_ID_MIN_ASSIGN 1024

typedef struct
{
    uint16_t alloc_id : 14 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved : 2  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_ALLOC_ID;

typedef struct
{
    union
    {
        DS_ALLOC_ID alloc_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint16_t alloc_16;
    };
    uint8_t alloc_id_type __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t alloc_id_scope[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[31] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_ASSIGN_ALLOC_ID_PLOAM;


/*****************************************************************************/
/* key control                                                               */
/*****************************************************************************/
#define KEY_CONTROL_GENERATE_NEW_KEY     0
#define KEY_CONTROL_CONFIRM_EXISTING_KEY 1
#define KEY_INDEX_FIRST_KEY              1
#define KEY_INDEX_SECOND_KEY             2

typedef struct
{
    uint8_t control: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 7  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ key_control_flag;

typedef struct
{
    uint8_t index: 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 6  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ key;

typedef struct
{
    uint8_t reserved           __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    key_control_flag control_flag __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    key key_index              __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t key_length         __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified [ 32 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_KEY_CONTROL_PLOAM;


/*****************************************************************************/
/* Sleep allow                                                               */
/*****************************************************************************/
#define SLEEP_ALLOW_OFF 0
#define SLEEP_ALLOW_ON  1

typedef struct
{
    uint8_t sleep_allowed: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 7  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ sleep_allow_control_flag;

typedef struct
{
    sleep_allow_control_flag control __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[35] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_SLEEP_ALLOW_PLOAM;


/*****************************************************************************/
/* Calibration Request message                                               */
/*****************************************************************************/
typedef struct
{
    uint8_t vendor_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t vssn[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t current_pon_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t correlation_tag[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t target_ds_pon_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t target_us_pon_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[14] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_CALIBRATION_REQUEST_PLOAM;


/*****************************************************************************/
/* Adjust Tx Wavelength message                                              */
/*****************************************************************************/
#define adjust_tx_to_lower_frequency 0
#define adjust_tx_to_higher_frequency 1

typedef struct
{
    uint8_t direction: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 7  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ tx_frequency_adjust_direction;

typedef struct
{
    uint8_t vendor_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t vssn[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t current_pon_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t correlation_tag[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    tx_frequency_adjust_direction frequency_adjust_direction __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t frequency_adjust_size[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[19] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_ADJUST_TX_WAVELENGTH_PLOAM;


/*****************************************************************************/
/* Tuning_Control message                                                    */
/*****************************************************************************/
#define operation_code_request 0x00
#define operation_code_complete_d 0x01

#define rollback_available_when_tuning_fails 1
#define no_rollback_available_when_tuning_fails 0

typedef struct
{
    uint8_t rollback_flag: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ rollback;

typedef struct
{
    uint8_t calibration_flag: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ calibration;

typedef struct
{
    uint8_t operation_code __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint16_t scheduled_sfc    __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    rollback rollback_flag __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t target_ds_pon_id    __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t target_us_pon_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    calibration calibration_tag __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[23] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_TUNING_CONTROL_PLOAM;


/*****************************************************************************/
/* System Profile message                                                    */
/*****************************************************************************/
#define ptp_wdm_us_operation_wavelength_band_expanded_spectrum 0
#define ptp_wdm_us_operation_wavelength_band_shared_spectrum 1
#define twdm_us_operation_wavelength_wide_band 0
#define twdm_us_operation_wavelength_reduced_band 1
#define twdm_us_operation_wavelength_narrow_band 2

#define amcc_use_flag_not_available 0
#define amcc_use_flag_available 1
#define amcc_min_calibration_accuracy_in_band_protected 1
#define amcc_min_calibration_accuracy_in_band_uncalibrated 1
#define amcc_min_calibration_accuracy_in_band_loose 2
#define amcc_min_calibration_accuracy_in_band_sufficient 3

#define min_calibration_accuracy_for_activation_sufficient 0
#define min_calibration_accuracy_for_activation_loose 1

typedef struct
{
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t version: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ system_profile_version;

typedef struct
{
    uint8_t twdm: 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved1: 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ptp_wdm: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved0: 3 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ us_operation_wavelength_bands;

typedef struct
{
    uint8_t ch_profile_num: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ twdm_profile_channel_count;

typedef struct
{
    uint8_t reserved0: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t amcc_use_flag: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved1: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t min_calibration_accuracy_for_activation_for_in_band: 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ twdm_amcc_control;

typedef struct
{
    uint8_t min_calibration_accuracy_for_activation: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ ptp_wdm_calibration_accuracy;

typedef struct
{
    uint8_t ng2sys_id[3] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    system_profile_version version __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    us_operation_wavelength_bands us_operation_wave_band __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    twdm_profile_channel_count twdm_channel_count __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t channel_spacing_twdm __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t us_mse_twdm __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t fsr_twdm[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    twdm_amcc_control amcc_control __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t loose_calibration_bound_twdm __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ptp_wdm_channel_count[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t channel_spacing_ptp_wdm __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t us_mse_ptp_wdm __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t fsr_ptp_wdm[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    ptp_wdm_calibration_accuracy ptp_wdm_calibration __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t loose_calibration_bound_ptp_wdm __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[16] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_SYSTEM_PROFILE_PLOAM;


/*****************************************************************************/
/* Channel Profile message                                                    */
/*****************************************************************************/
#define this_channel_indicator_channel_profile_pertains_to_another 0
#define this_channel_indicator_channel_profile_pertains_to_tx_twdm 1
#define void_indicator_descriptor_valid 0
#define void_indicator_descriptor_ignored 1

#define ds_freq_offset_unknown 128

#define ds_tc_layer_line_rate_10Gbps 0
#define ds_tc_layer_line_rate_2_5Gbps 1
#define ds_line_code_nrz 0

#define sn_grants_not_offered 0
#define sn_grants_offered 1

typedef struct
{
    uint8_t us_void_indicator: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ds_void_indicator: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t this_channel_indicator: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t index: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ channel_profile_control_twdm;

typedef struct
{
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t profile_version: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ channel_profile_version;

typedef struct
{
    uint8_t ds_line_code: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ds_fec: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ds_tc_layer_line_rate: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ channel_profile_ds_rate;

typedef struct
{
    uint8_t index: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ channel_partition_index;

typedef struct
{
    uint8_t amcc_sn_grants: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t in_band_sn_grants: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 6 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ sn_grants_type_indication;

typedef struct
{
    uint8_t channel_id: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ assigned_us_wavelength_channel_id;

typedef struct
{
    uint8_t type_b_support: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t type_a_support: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 6 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ channel_profile_optical_link_type;

typedef struct
{
    uint8_t rate_2_5G_support: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t rate_10G_support: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 6 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ us_nominal_line_rate;

typedef struct
{
    uint8_t level: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ default_attenuation;

#ifdef G989_3_AMD1 
#define DS_CHANNEL_PROFILE_TWDM_RESERVED_BYTES 9
#else
#define DS_CHANNEL_PROFILE_TWDM_RESERVED_BYTES 11
#endif

typedef struct
{
    channel_profile_control_twdm control __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    channel_profile_version version __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t pon_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ds_freq_offset __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    channel_profile_ds_rate ds_rate __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    channel_partition_index channel_partition __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t default_response_channel __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    sn_grants_type_indication sn_grant_type __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#ifdef G989_3_AMD1 
    uint32_t amcc_win_spec __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#else
    uint16_t amcc_win_spec __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
    assigned_us_wavelength_channel_id uwlch_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t us_freq __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    channel_profile_optical_link_type optical_link_type __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    us_nominal_line_rate us_rate __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    default_attenuation default_onu_attenuation __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t response_threshold __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[DS_CHANNEL_PROFILE_TWDM_RESERVED_BYTES] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ DS_CHANNEL_PROFILE_TWDM_PLOAM;


typedef struct
{
    uint8_t rate_class4_6Gbps: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t rate_class3_10Gbps: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t rate_class2_2_5Gbps: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t rate_class1_1Gbps: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ rate_classes;

#define amcc_type_inidication_transparent 0
#define amcc_type_inidication_transcoded 1
#define engaged_flag_channel_available 0
#define engaged_flag_channel_onu_paired 1
#define protection_flag_generic 0
#define protection_flag_used_for_protection 0

typedef struct
{
    uint8_t reserved0: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t amcc_type_inidication: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t engaged_flag: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t protection_flag: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved1: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t this_channel_indicator: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ds_void_indicator: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t us_void_indicator: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ channel_profile_control_ptp_wdm;

typedef struct
{
    channel_profile_control_ptp_wdm control __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t channel_profile_index[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    channel_profile_version version __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t pon_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t service_type __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t dwlch_id[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ds_freq[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    rate_classes ds_rate __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t channel_partition __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t uwlch_id[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t us_freq[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    rate_classes us_rate __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t pon_tag_digest[8] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ DS_CHANNEL_PROFILE_PTP_WDM_PLOAM;


typedef union
{
    DS_CHANNEL_PROFILE_TWDM_PLOAM twdm;
    DS_CHANNEL_PROFILE_PTP_WDM_PLOAM ptp_wdm;
}NGPON_DS_CHANNEL_PROFILE_PLOAM;


/*****************************************************************************/
/* Protection control message                                                */
/*****************************************************************************/
#ifdef G989_3_AMD1
#define PROTECTION_CONTROL_RESERVED_BYTES 27
#else
#define PROTECTION_CONTROL_RESERVED_BYTES 28
#endif

typedef struct
{
    uint32_t protect_ds_pon_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t protect_us_pon_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#ifdef G989_3_AMD1
    uint8_t enable_flag __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
    uint8_t unspecified[PROTECTION_CONTROL_RESERVED_BYTES] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_PROTECTION_CONTROL_PLOAM;


/*****************************************************************************/
/* Power Level message                                                       */
/*****************************************************************************/
#define operation_type_direct_attenuation_level 0
#define operation_type_decrease_attenuation_level 1
#define operation_type_increase_attenuation_level 2
#define operation_type_request_current_attenuation_level 3

typedef struct
{
    uint8_t type: 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 6 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ operation_type_attenuation;

typedef struct
{
    operation_type_attenuation operation_type __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t attenuation __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[34] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_CHANGE_POWER_LEVEL_PLOAM;


/*****************************************************************************/
/* Power Consumption message                                                       */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified[36] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_POWER_CONSUMPTION_INQUIRE_PLOAM;


/*****************************************************************************/
/* Rate Control message                                                      */
/*****************************************************************************/
#define nominal_line_rate_1Gbps 0
#define nominal_line_rate_2_5Gbps 1
#define nominal_line_rate_10Gbps 2
#define nominal_line_rate_6Gbps 3

typedef struct
{
    uint8_t us: 3 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved1: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ds: 3 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved0: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ nominal_line_rates;

typedef struct
{
    uint8_t operation_code __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t squeduled_sfc[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    rollback rollback_flag __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    nominal_line_rates rates __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[31] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_RATE_CONTROL_PLOAM;


/*****************************************************************************/
/*                                                                           */
/* Upstream messages                                                       */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Serial number ONU                                                         */
/*****************************************************************************/
#define RANDOM_DELAY_FIELD_SIZE  ( 4 )
#define DEFAULT_US_SN_ONU_PLOAM_SEQUENCE_NUM  ( 0x00 )

typedef struct
{
    uint8_t serial_number[SERIAL_NUMBER_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t random_delay[RANDOM_DELAY_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t correlation_tag[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t current_ds_pon_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t current_us_pon_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t calibration_record_status_twdm[8] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t tuning_granularity __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t step_tuning_time __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    us_nominal_line_rate us_line_rate_capability __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t attenuation __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t power_level_capabilty __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_SERIAL_NUMBER_ONU_PLOAM;


/*****************************************************************************/
/* Registration                                                              */
/*****************************************************************************/
#define REGISTRATION_ID_FIELD_SIZE ( 36 )

typedef struct
{
    uint8_t registration_id[REGISTRATION_ID_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_REGISTRATION_PLOAM;


/*****************************************************************************/
/* Key report                                                                */
/*****************************************************************************/
#define KEY_REPROT_TYPE_NEW_KEY      0 
#define KEY_REPROT_TYPE_EXISTING_KEY 1 
#define KEY_REPROT_INDEX_FIRST_KEY   1 
#define KEY_REPROT_INDEX_SECOND_KEY  2 
#define FRAGMENT_NUMBER_MAX_VALUE    7 
#define KEY_FRAGMENT_FIELD_SIZE      32

typedef struct
{
    uint8_t type: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ key_report_type;

typedef struct
{
    uint8_t index: 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 6 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ key_report_index;

typedef struct
{
    uint8_t frag_num: 3 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 5 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ fragment_num;

typedef struct
{
    key_report_type report_type __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    key_report_index key_index       __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    fragment_num fragment_number __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t key_fragment[KEY_FRAGMENT_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_KEY_REPORT_PLOAM;

/*****************************************************************************/
/* Acknowledge                                                               */
/*****************************************************************************/
#define ACKNOWLEDGE_OK                   0 
#define ACKNOWLEDGE_NO_MESSAGE           1 
#define ACKNOWLEDGE_BUSY                 2 
#define ACKNOWLEDGE_UNKNOWN_MESSAGE_TYPE 3 
#define ACKNOWLEDGE_PARAMETER_ERROR      4 
#define ACKNOWLEDGE_PROCESSING_ERROR     5 

typedef struct
{
    uint8_t completion_code __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t attenuation __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t power_levelling_capability __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[33] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_ACKNOWLEDGE_PLOAM;


/*****************************************************************************/
/* Sleep request                                                             */
/*****************************************************************************/
#define SLEEP_REQUEST_AWAKE  ( 0 )
#define SLEEP_REQUEST_DOZE   ( 1 )
#define SLEEP_REQUEST_SLEEP  ( 2 )
#define DEFAULT_US_SLEEP_REQUEST_PLOAM_SEQUENCE_NUM  ( 0x00 )

typedef struct
{
    uint8_t activity_level     __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified [ 35 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_SLEEP_REQUEST_PLOAM;


/*****************************************************************************/
/* Tuning Response                                                           */
/*****************************************************************************/
#define operation_code_ack 0
#define operation_code_nack 1
#define operation_code_complete_u 3
#define operation_code_rollback 4

#define response_ack 0x0000
#define response_complete_u 0x0000 
#define response_nack_int_sfc 0x0001
#define response_nack_ds_albl 0x0002
#define response_nack_ds_void 0x0004
#define response_nack_ds_part 0x0008
#define response_nack_ds_tunr 0x0010
#define response_nack_ds_lnrt 0x0020
#define response_nack_ds_lncd 0x0040
#define response_nack_us_albl 0x0080
#define response_nack_us_void 0x0100
#define response_nack_us_tunr 0x0200
#define response_nack_us_clbr 0x0400
#define response_nack_us_lktp 0x0800
#define response_nack_us_lnrt 0x1000
#define response_nack_us_lncd 0x2000
#define response_rollback_com_ds 0x0001  
#define response_rollback_ds_albl 0x0002  
#define response_rollback_ds_lktp 0x0004  
#define response_rollback_us_albl 0x0008  
#define response_rollback_us_void 0x0010  
#define response_rollback_us_tunr 0x0020  
#define response_rollback_us_lktp 0x0040  
#define response_rollback_us_lnrt 0x0080  
#define response_rollback_us_lncd 0x0100  


typedef struct
{
    uint8_t us_nominal_line_rate_2_5G: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t us_nominal_line_rate_10G: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 6 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ us_line_rate_capability_twdm;

typedef struct
{
    uint8_t us_nominal_line_rate_2_5G: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t us_nominal_line_rate_10G: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t us_nominal_line_rate_1G: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t us_nominal_line_rate_6G: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ us_line_rate_capability_ptp_wdm;

typedef union
{
    us_line_rate_capability_twdm twdm;
    us_line_rate_capability_ptp_wdm ptp_wdm;
}us_line_rate_capability;


typedef union
{
    uint8_t calibration_record_status_twdm[8];
    uint8_t sn_digest_ptp_wdm[8];
} sn_digest_ptp_wdm_calibration_record_twdm;

typedef struct
{
    uint8_t operation_code __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint16_t response_code __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t vendor_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t vssn[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint16_t correlation_tag __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t pon_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    assigned_us_wavelength_channel_id uwlch_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    sn_digest_ptp_wdm_calibration_record_twdm sn_digest_calibration_record;
    uint8_t tuning_granularity __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t one_step_tuning_time __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    us_line_rate_capability us_line_rate __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t attenuation __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t power_leveling_capability __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[6] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_TUNING_RESPONSE_PLOAM;


/*****************************************************************************/
/* Power Consumption Report message                                          */
/*****************************************************************************/
typedef struct
{
    uint8_t twdm_wavelength_channel __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t power_consumption_associated_twdm[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ power_consumption_per_channel;

typedef struct
{
    power_consumption_per_channel power_per_channel[8] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ power_consumptions;

typedef struct
{
    power_consumptions power_consumption __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[12] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_POWER_CONSUMPTION_REPORT_PLOAM;


/*****************************************************************************/
/* Rate Response                                                             */
/*****************************************************************************/
#define rate_response_ack 0x0
#define rate_response_complete_u 0x00
#define rate_response_nack_ds_out_of_supported_rx_tuning_range 0x01
#define rate_response_nack_us_out_of_supported_tx_tuning_range 0x02
#define rate_response_nack_not_ready_tx_tuning_scheduled_sfc 0x03
#define rate_response_rollback_no_dsync 0x10

typedef struct
{
    uint8_t operation_code __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t response_code __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[34] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_RATE_RESPONSE_PLOAM;

/*****************************************************************************/
/*                                                                           */
/* Messaages unions                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Downstream                                                                */
/*****************************************************************************/
typedef union
{
    NGPON_DS_BURST_PROFILE_PLOAM             burst_profile         __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_ASSIGN_ONU_ID_PLOAM             assign_onu_id         __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_RANGING_TIME_PLOAM              ranging_time          __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_DEACTIVATE_ONU_ID_PLOAM         deactivate_onu_id     __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_DISABLE_SERIAL_NUMBER_PLOAM     disable_serial_number __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_REQUEST_REGISTRATION_PLOAM      request_registration  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_ASSIGN_ALLOC_ID_PLOAM           assign_alloc_id       __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_KEY_CONTROL_PLOAM               key_control           __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_SLEEP_ALLOW_PLOAM               sleep_allow           __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_CALIBRATION_REQUEST_PLOAM       calibration_request   __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    NGPON_DS_ADJUST_TX_WAVELENGTH_PLOAM      adjust_tx_wavelength  __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    NGPON_DS_TUNING_CONTROL_PLOAM            tuning_control        __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    NGPON_DS_SYSTEM_PROFILE_PLOAM            system_profile        __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    NGPON_DS_CHANNEL_PROFILE_PLOAM           channel_profile       __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    NGPON_DS_PROTECTION_CONTROL_PLOAM        protection_control    __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    NGPON_DS_CHANGE_POWER_LEVEL_PLOAM        change_power_level    __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    NGPON_DS_POWER_CONSUMPTION_INQUIRE_PLOAM power_consumption     __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    NGPON_DS_RATE_CONTROL_PLOAM              rate_control          __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    uint8_t unknown_message[PLOAM_DATA_SIZE]                 __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_MESSAGE;

typedef struct
{
    uint16_t onu_id          __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t  message_id      __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t  sequence_number __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_DS_MESSAGE message __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t mic[PLOAM_MIC_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_DS_PLOAM;


/*****************************************************************************/
/* Upstream                                                                  */
/*****************************************************************************/
typedef union
{
    NGPON_US_SERIAL_NUMBER_ONU_PLOAM serial_number_onu __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_US_REGISTRATION_PLOAM      registration      __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_US_KEY_REPORT_PLOAM        key_report        __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_US_ACKNOWLEDGE_PLOAM       acknowledge       __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_US_SLEEP_REQUEST_PLOAM     sleep_request     __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_US_TUNING_RESPONSE_PLOAM   tuning_response   __PACKING_ATTRIBUTE_FIELD_LEVEL__;  
    NGPON_US_POWER_CONSUMPTION_REPORT_PLOAM power_consumption_report __PACKING_ATTRIBUTE_FIELD_LEVEL__;  
    NGPON_US_RATE_RESPONSE_PLOAM     rate_response     __PACKING_ATTRIBUTE_FIELD_LEVEL__;  
    uint8_t unknown_message[PLOAM_DATA_SIZE]     __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_MESSAGE;

typedef struct
{
    uint16_t onu_id          __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t  message_id      __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t  sequence_number __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    NGPON_US_MESSAGE message __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t mic[PLOAM_MIC_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_US_PLOAM;


/*****************************************************************************/
/* Combined                                                                  */
/*****************************************************************************/
typedef union
{
    NGPON_DS_PLOAM downstream;
    NGPON_US_PLOAM upstream;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ NGPON_PLOAM;



#ifdef __cplusplus
}
#endif
#endif
