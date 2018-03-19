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

#ifndef PLOAM_H_INCLUDED
#define PLOAM_H_INCLUDED

#include <bdmf_system.h>
#define CE_PLOAM_DATA_SIZE (10)
#define PLOAM_MESSAGE_SIZE (12)

/* Downstream message IDs */
#define CE_DS_US_OVERHEAD_MESSAGE_ID              ( ( uint8_t ) 0x01 )
#define CE_DS_SERIAL_NUMBER_MASK_MESSAGE_ID       ( ( uint8_t ) 0x02 )
#define CE_DS_ASSIGN_ONU_ID_MESSAGE_ID            ( ( uint8_t ) 0x03 )
#define CE_DS_RANGING_TIME_MESSAGE_ID             ( ( uint8_t ) 0x04 )
#define CE_DS_DEACTIVATE_ONU_ID_MESSAGE_ID        ( ( uint8_t ) 0x05 )
#define CE_DS_DISABLE_SERIAL_NUMBER_MESSAGE_ID    ( ( uint8_t ) 0x06 )
#define CE_DS_ENCRYPTED_PORT_ID_MESSAGE_ID        ( ( uint8_t ) 0x08 )
#define CE_DS_REQUEST_PASSWORD_MESSAGE_ID         ( ( uint8_t ) 0x09 )
#define CE_DS_ASSIGN_ALLOC_ID_MESSAGE_ID          ( ( uint8_t ) 0x0A )
#define CE_DS_NO_MESSAGE_MESSAGE_ID               ( ( uint8_t ) 0x0B )
#define CE_DS_POPUP_MESSAGE_ID                    ( ( uint8_t ) 0x0C )
#define CE_DS_REQUEST_KEY_MESSAGE_ID              ( ( uint8_t ) 0x0D )
#define CE_DS_CONFIGURE_PORT_ID_MESSAGE_ID        ( ( uint8_t ) 0x0E )
#define CE_DS_PHYSICAL_EQUIPMENT_ERROR_MESSAGE_ID ( ( uint8_t ) 0x0F )
#define CE_DS_CHANGE_POWER_LEVEL_MESSAGE_ID       ( ( uint8_t ) 0x10 )
#define CE_DS_PST_MESSAGE_ID                      ( ( uint8_t ) 0x11 )
#define CE_DS_BER_INTERVAL_MESSAGE_ID             ( ( uint8_t ) 0x12 )
#define CE_DS_KEY_SWITCHING_TIME_MESSAGE_ID       ( ( uint8_t ) 0x13 )
#define CE_DS_EXTENDED_BURST_LENGTH_MESSAGE_ID    ( ( uint8_t ) 0x14 )
#define CE_DS_PON_ID_MESSAGE_ID                   ( ( uint8_t ) 0x15 )
#define CE_DS_SWIFT_POPUP_MESSAGE_ID              ( ( uint8_t ) 0x16 )
#define CE_DS_RANGING_ADJUSTMENT_MESSAGE_ID       ( ( uint8_t ) 0x17 )
#define CE_DS_SLEEP_ALLOW_MESSAGE_ID              ((uint8_t)0x20) 

/* Upstream message IDs */
#define CE_US_SERIAL_NUMBER_ONU_MESSAGE_ID        ( ( uint8_t ) 0x01 )
#define CE_US_PASSWORD_MESSAGE_ID                 ( ( uint8_t ) 0x02 )
#define CE_US_DYING_GASP_MESSAGE_ID               ( ( uint8_t ) 0x03 )
#define CE_US_NO_MESSAGE_MESSAGE_ID               ( ( uint8_t ) 0x04 )
#define CE_US_ENCRYPTION_KEY_MESSAGE_ID           ( ( uint8_t ) 0x05 )
#define CE_US_PHYSICAL_EQUIPMENT_ERROR_MESSAGE_ID ( ( uint8_t ) 0x06 )
#define CE_US_PST_MESSAGE_ID                      ( ( uint8_t ) 0x07 )
#define CE_US_REMOTE_ERROR_INDICATION_MESSAGE_ID  ( ( uint8_t ) 0x08 )
#define CE_US_ACKNOWLEDGE_MESSAGE_ID              ( ( uint8_t ) 0x09 )
#define CE_US_SLEEP_REQUEST_MESSAGE_ID            ((uint8_t)0x10) 

/*****************************************************************************/
/*                                                                           */
/* Downstream messages                                                       */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Upstream overhead                                                         */
/*****************************************************************************/
#define CE_PRE_EQUALIZATION_DELAY_DONT_USE ( 0 )
#define CE_PRE_EQUALIZATION_DELAY_USE      ( 1 )

#define CE_SERIAL_NUMBER_MASK_DISABLED ( 0 )
#define CE_SERIAL_NUMBER_MASK_ENABLED  ( 1 )

#define CE_POWER_LEVEL_MODE_0_NORMAL      ( 0 )
#define CE_POWER_LEVEL_MODE_1_NORMAL_3_DB ( 1 )
#define CE_POWER_LEVEL_MODE_2_NORMAL_6_DB ( 2 )
#define CE_POWER_LEVEL_MODE_RESERVED      ( 3 )

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint8_t reserved                          : 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t pre_equalization_status           : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t serial_number_mask_status         : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t extra_serial_number_transmissions : 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t power_level_mode                  : 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
#else
    uint8_t power_level_mode                  : 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t extra_serial_number_transmissions : 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t serial_number_mask_status         : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t pre_equalization_status           : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved                          : 2 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_US_OVERHEAD_OPTIONS;

typedef struct
{
    uint8_t number_of_guard_bits            __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t number_of_type_1_preamble_bits  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t number_of_type_2_preamble_bits  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t pattern_of_type_3_preamble_bits __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t delimiter[3]                    __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    DS_US_OVERHEAD_OPTIONS options          __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t preassigned_equalization_delay[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
} 
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_US_OVERHEAD_PLOAM;


/*****************************************************************************/
/* Serial Number mask                                                        */
/*****************************************************************************/
#define CE_DS_PLOAM_MESSAGE_SERIAL_NUMBER_FIELD_SIZE (8)

typedef struct
{
    uint8_t number_of_valid_bits __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t serial_number[CE_DS_PLOAM_MESSAGE_SERIAL_NUMBER_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_SERIAL_NUMBER_MASK_PLOAM;


/*****************************************************************************/
/* Assign ONU-ID                                                             */
/*****************************************************************************/
typedef struct
{
    uint8_t assigned_onu_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t serial_number[CE_DS_PLOAM_MESSAGE_SERIAL_NUMBER_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_ASSIGN_ONU_ID_PLOAM;


/*****************************************************************************/
/* Ranging Time                                                              */
/*****************************************************************************/
#define CE_PATH_MAIN       ( 0 )
#define CE_PATH_PROTECTION ( 1 )

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint8_t reserved : 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t path     : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
#else
    uint8_t path     : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t reserved : 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_RANGING_TIME_OPTIONS;

typedef struct
{
    DS_RANGING_TIME_OPTIONS options           __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t delay[4]                    __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[5]              __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_RANGING_TIME_PLOAM;


/*****************************************************************************/
/* Deactivate ONU-ID                                                         */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified[10] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_DEACTIVATE_ONU_ID_PLOAM;


/*****************************************************************************/
/* Disable Serial Number                                                     */
/*****************************************************************************/
#define CE_SERIAL_NUMBER_UNICAST_DISABLE  ( 0xFF )
#define CE_SERIAL_NUMBER_UNICAST_ENABLE   ( 0x00 )
#define CE_SERIAL_NUMBER_MULTICAST_ENABLE ( 0x0F )

typedef struct
{
    uint8_t control             __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t serial_number[8] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified         __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
} 
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_DISABLE_SERIAL_NUMBER_PLOAM;


/*****************************************************************************/
/* Configure VP/VC                                                           */
/*****************************************************************************/
#define CE_VP_VC_DEACTIVATE ( 0 )
#define CE_VP_VC_ACTIVATE   ( 1 )

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint8_t reserved : 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t activate : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#else
    uint8_t activate : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved : 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_CONFIGURE_VP_VC_OPTIONS;

typedef struct
{
    DS_CONFIGURE_VP_VC_OPTIONS options          __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t                      atm_header [ 4 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t                      mask [ 4 ]       __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t                      unspecified      __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_CONFIGURE_VP_VC_PLOAM;


/*****************************************************************************/
/* Encrypted Port ID                                                         */
/*****************************************************************************/
#define CE_PORT_ID_NOT_ENCRYPTED ( 0 )
#define CE_PORT_ID_ENCRYPTED     ( 1 )

#define CE_TYPE_VPI     ( 0 )
#define CE_TYPE_PORT_ID ( 1 )

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint8_t reserved  : 6 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t type      : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t encrypted : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#else
    uint8_t encrypted : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t type      : 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved  : 6 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_ENCRYPTED_PORT_ID_OR_VPI_OPTIONS;

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint16_t port_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#else
    uint8_t reserved: 4  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint16_t port_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_ENCRYPTED_PORT_ID_OR_VPI_PORT_ID;

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint16_t vpi_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#else
    uint8_t reserved: 4  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint16_t vpi_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_ENCRYPTED_PORT_ID_OR_VPI_VPI_ID;

typedef struct
{
    DS_ENCRYPTED_PORT_ID_OR_VPI_OPTIONS options __PACKING_ATTRIBUTE_FIELD_LEVEL__;

    union
    {
        DS_ENCRYPTED_PORT_ID_OR_VPI_PORT_ID port_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint16_t port_id_16;
    };

    union
    {
        DS_ENCRYPTED_PORT_ID_OR_VPI_VPI_ID vpi_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint16_t vpi_16;
    };
    uint8_t unspecified[5] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_ENCRYPTED_PORT_ID_OR_VPI_PLOAM;


/*****************************************************************************/
/* Request Password                                                          */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified[10] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_REQUEST_PASSWORD_PLOAM;


/*****************************************************************************/
/* Assign Alloc ID                                                           */
/*****************************************************************************/
#define CE_PAYLOAD_TYPE_ATM     ( 0 )
#define CE_PAYLOAD_TYPE_GEM     ( 1 )
#define CE_PAYLOAD_TYPE_DBA     ( 2 )
#define CE_PAYLOAD_TYPE_DEALLOC ( 255 )

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint16_t alloc_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
#else
    uint8_t reserved : 4  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint16_t alloc_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_ASSIGN_ALLOC_ID_ALLOC_ID;

typedef struct
{
    union
    {
        DS_ASSIGN_ALLOC_ID_ALLOC_ID alloc_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint16_t alloc_16;
    };
    uint8_t alloc_id_type __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t unspecified[7] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_ASSIGN_ALLOC_ID_PLOAM;


/*****************************************************************************/
/* No Message                                                                */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified [ 10 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_NO_MESSAGE_PLOAM;


/*****************************************************************************/
/* Popup                                                                     */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified [ 10 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
} 
__PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_POPUP_PLOAM;


/*****************************************************************************/
/* Request key                                                               */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified [ 10 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_REQUEST_KEY_PLOAM;


/*****************************************************************************/
/* Configure Port ID                                                         */
/*****************************************************************************/
#define CE_PORT_ID_DEACTIVATE ( 0 )
#define CE_PORT_ID_ACTIVATE   ( 1 )

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint8_t reserved: 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t activate: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#else
    uint8_t activate: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 7 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_CONFIGURE_PORT_ID_OPTIONS;

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint16_t port_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t reserved: 4  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#else
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint16_t port_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ DS_CONFIGURE_PORT_ID_PORT_ID;

typedef struct
{
    DS_CONFIGURE_PORT_ID_OPTIONS options __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    union
    {
        DS_CONFIGURE_PORT_ID_PORT_ID port_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint16_t port_id_16;
    };
    uint8_t unspecified[7] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
} 
__PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_CONFIGURE_PORT_ID_PLOAM;


/*****************************************************************************/
/* PEE - Physical Equipment Error                                            */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified [ 10 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_PHYSICAL_EQUIPMENT_ERROR_PLOAM;


/*****************************************************************************/
/* Change Power Level                                                        */
/*****************************************************************************/
#define CE_CHANGE_POWER_LEVEL_NO_ACTION_A ( 0 )
#define CE_CHANGE_POWER_LEVEL_NO_ACTION_B ( 3 )
#define CE_CHANGE_POWER_LEVEL_DECREASE    ( 1 )
#define CE_CHANGE_POWER_LEVEL_INCREASE    ( 2 )
#define CE_CHANGE_POWER_LEVEL_LIMIT       ( 3 )

typedef struct
{
    uint8_t indication __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified [ 9 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_CHANGE_POWER_LEVEL_PLOAM;


/*****************************************************************************/
/* PST                                                                       */
/*****************************************************************************/
typedef struct
{
    uint8_t line_number       __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t k1                __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t k2                __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified[7] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_PST_PLOAM;


/*****************************************************************************/
/* BER Interval                                                              */
/*****************************************************************************/
typedef struct
{
    uint8_t interval [ 4 ]    __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified [ 6 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_BER_INTERVAL_PLOAM;


/*****************************************************************************/
/* Key switching time                                                        */
/*****************************************************************************/
typedef struct
{
    uint8_t frame_counter [ 4 ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified [ 6 ]   __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_KEY_SWITCHING_TIME_PLOAM;


/*****************************************************************************/
/* Extended burst length                                                     */
/*****************************************************************************/
typedef struct
{
    uint8_t pre_range_type_3_number  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t range_type_3_number      __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_EXTENDED_BURST_LENGTH_PLOAM;


/*****************************************************************************/
/* Sleep Allow                                                               */
/*****************************************************************************/
#define CE_SLEEP_ALLOW_OFF	( 0 )
#define CE_SLEEP_ALLOW_ON	( 1 )

typedef struct
{
	uint8_t sleep_allow		__PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
	uint8_t unspecified [ 9 ]	__PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
__PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_SLEEP_ALLOW_PLOAM; 


/*****************************************************************************/
/* DS Swift Popup                                                            */
/*****************************************************************************/
typedef struct
{
    uint8_t reserved[10] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_SWIFT_POPUP_PLOAM;


/*****************************************************************************/
/* Ranging Adjustment                                                        */
/*****************************************************************************/
#define DS_RANGING_ADJUSTMENT_PLOAM_INCREASE_EQD 0
#define DS_RANGING_ADJUSTMENT_PLOAM_DECREASE_EQD 2

typedef struct
{
    uint8_t decrease_flag __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t eqd_delay[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved[5] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_RANGING_ADJUSTMENT_PLOAM;


/*****************************************************************************/
/* PON ID                                                                    */
/*****************************************************************************/
#define DS_PON_ID_TYPE_REPORT_OLT 0
#define DS_PON_ID_TYPE_REPORT_ONU 1

#define DS_PON_ID_TYPE_BUDGET_CLASS_A 0
#define DS_PON_ID_TYPE_BUDGET_CLASS_B 1
#define DS_PON_ID_TYPE_BUDGET_CLASS_B_PLUS 2
#define DS_PON_ID_TYPE_BUDGET_CLASS_C 3
#define DS_PON_ID_TYPE_BUDGET_CLASS_C_PLUS 4

typedef struct
{
    uint8_t tol_report: 1 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t budget_class: 3 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t reserved: 4 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ DS_PON_ID_TYPE ;

typedef struct
{
    DS_PON_ID_TYPE pon_id_type __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t pon_identifier[7] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t tx_optical_level[2] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_PON_ID_PLOAM;


/*****************************************************************************/
/*                                                                           */
/* Upstream messages                                                       */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Serial number ONU                                                         */
/*****************************************************************************/
#define CE_TRANSPORT_NOT_SUPPORTED ( 0 )
#define CE_TRANSPORT_SUPPORTED     ( 1 )

#define CE_POWER_LEVEL_LOW      ( 0 )
#define CE_POWER_LEVEL_MEDIUM   ( 1 )
#define CE_POWER_LEVEL_HIGH     ( 2 )
#define CE_POWER_LEVEL_RESERVED ( 3 )

#define CE_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE  ( 10 )

typedef struct
{
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
    uint16_t random_delay  : 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t atm_supported : 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t gem_supported : 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t power_level   : 2  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
#else
    uint8_t power_level   : 2  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t gem_supported : 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t atm_supported : 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint16_t random_delay  : 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
#endif
}
 __PACKING_ATTRIBUTE_STRUCT_END__ US_SERIAL_NUMBER_ONU_OPTIONS;

typedef struct
{
    uint8_t vendor_id[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t vendor_specific_serial_number[4] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    US_SERIAL_NUMBER_ONU_OPTIONS options __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_SERIAL_NUMBER_ONU_PLOAM;


/*****************************************************************************/
/* Password                                                                  */
/*****************************************************************************/
typedef struct
{
    uint8_t password[CE_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_PASSWORD_PLOAM;


/*****************************************************************************/
/* Dying Gasp                                                                */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified [ CE_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_DYING_GASP_PLOAM;


/*****************************************************************************/
/* No Message                                                                */
/*****************************************************************************/
typedef struct
{
    uint8_t unspecified [ CE_PLOAM_MESSAGE_PAYLOAD_FIELD_SIZE ] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_NO_MESSAGE_PLOAM;


/*****************************************************************************/
/* Encryption Key                                                            */
/*****************************************************************************/
typedef struct
{
    uint8_t key_index      __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t fragment_index __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t key [ 8 ]      __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_ENCRYPTION_KEY_PLOAM;


/*****************************************************************************/
/* PEE - Physical Equipment Error                                            */
/*****************************************************************************/
#define US_PLOAM_PEE_UNSPECIFIED_FIELD_SIZE 10

typedef struct
{
    uint8_t unspecified[US_PLOAM_PEE_UNSPECIFIED_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_PHYSICAL_EQUIPMENT_ERROR_PLOAM;


/*****************************************************************************/
/* PST                                                                       */
/*****************************************************************************/
#define CE_US_PLOAM_MESSAGE_PST_UNSPECIFIED_FIELD_SIZE 7

typedef struct
{
    uint8_t line_number       __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t k1                __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t k2                __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified[CE_US_PLOAM_MESSAGE_PST_UNSPECIFIED_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_PST_PLOAM;


/*****************************************************************************/
/* Remote Error Indication                                                   */
/*****************************************************************************/
#define US_PLOAM_MESSAGE_REI_ERROR_COUNT_FIELD_SIZE 4
#define US_PLOAM_MESSAGE_REI_UNSPECIFIED_FIELD_SIZE 5

typedef struct
{
    uint8_t error_count[US_PLOAM_MESSAGE_REI_ERROR_COUNT_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t sequence_number   __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
    uint8_t unspecified[US_PLOAM_MESSAGE_REI_UNSPECIFIED_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_REI_PLOAM;



/*****************************************************************************/
/* Vendor Specific message                                                   */
/*****************************************************************************/
typedef struct
{
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_VENDOR_SPECIFIC_PLOAM;


/*****************************************************************************/
/* Acknowledge                                                               */
/*****************************************************************************/
#define CE_DS_PLOAM_MESSAGE_ACK_FIELD_SIZE 9
typedef struct
{
    uint8_t ds_message_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t ds_message[9] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_ACKNOWLEDGE_PLOAM;


/*****************************************************************************/
/* Sleep Request                                                             */
/*****************************************************************************/
#define CE_SLEEP_REQUEST_AWAKE	( 0 )
#define CE_SLEEP_REQUEST_DOZE	( 1 )
#define CE_SLEEP_REQUEST_SLEEP	( 2 )

typedef struct
{
	uint8_t activity_level	__PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
	uint8_t unspecified [ 9 ]	__PACKING_ATTRIBUTE_FIELD_LEVEL__ ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_SLEEP_REQUEST_PLOAM; 


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
    GPON_DS_US_OVERHEAD_PLOAM              upstream_overhead        __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_SERIAL_NUMBER_MASK_PLOAM       serial_number_mask       __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_ASSIGN_ONU_ID_PLOAM            assign_onu_id            __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_RANGING_TIME_PLOAM             ranging_time             __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_DEACTIVATE_ONU_ID_PLOAM        deactivate_onu_id        __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_DISABLE_SERIAL_NUMBER_PLOAM    disable_serial_number    __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_CONFIGURE_VP_VC_PLOAM          configure_vp_vc          __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_ENCRYPTED_PORT_ID_OR_VPI_PLOAM encrypted_port_id_or_vpi __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_REQUEST_PASSWORD_PLOAM         request_password         __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_ASSIGN_ALLOC_ID_PLOAM          assign_alloc_id          __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_NO_MESSAGE_PLOAM               no_message               __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_POPUP_PLOAM                    popup                    __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_REQUEST_KEY_PLOAM              request_key              __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_CONFIGURE_PORT_ID_PLOAM        configure_port_id        __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_PHYSICAL_EQUIPMENT_ERROR_PLOAM physical_equipment_error __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_CHANGE_POWER_LEVEL_PLOAM       change_power_level       __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_PST_PLOAM                      pst                      __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_BER_INTERVAL_PLOAM             ber_interval             __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_KEY_SWITCHING_TIME_PLOAM       key_switching_time       __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_EXTENDED_BURST_LENGTH_PLOAM    extended_burst_length    __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_PON_ID_PLOAM                   pon_id                   __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    GPON_DS_RANGING_ADJUSTMENT_PLOAM       ranging_adjustment       __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    GPON_DS_SWIFT_POPUP_PLOAM              swift_popup              __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    GPON_DS_SLEEP_ALLOW_PLOAM              sleep_allow              __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
    uint8_t                                unknown_message[CE_PLOAM_DATA_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_MESSAGE;

typedef struct
{
    uint8_t onu_id     __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t message_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_DS_MESSAGE message __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_DS_PLOAM;


/*****************************************************************************/
/* Upstream                                                                  */
/*****************************************************************************/
typedef union
{
    GPON_US_SERIAL_NUMBER_ONU_PLOAM        serial_number_onu         __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_PASSWORD_PLOAM                 password                  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_DYING_GASP_PLOAM               dying_gasp                __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_NO_MESSAGE_PLOAM               no_message                __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_ENCRYPTION_KEY_PLOAM           encryption_key            __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_PHYSICAL_EQUIPMENT_ERROR_PLOAM physical_equipment_error  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_PST_PLOAM                      pst                       __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_REI_PLOAM                      remote_error_indication   __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_VENDOR_SPECIFIC_PLOAM          vendor_specific           __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_ACKNOWLEDGE_PLOAM              acknowledge               __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_SLEEP_REQUEST_PLOAM            sleep_request             __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t                                unknown_message[CE_PLOAM_DATA_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__; 
}
__PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_MESSAGE;

typedef struct
{
    uint8_t onu_id     __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint8_t message_id __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    GPON_US_MESSAGE message __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_US_PLOAM;


/*****************************************************************************/
/* Combined                                                                  */
/*****************************************************************************/
typedef union
{
    GPON_DS_PLOAM downstream ;
    GPON_US_PLOAM upstream ;
}
 __PACKING_ATTRIBUTE_STRUCT_END__ GPON_PLOAM;

#endif
