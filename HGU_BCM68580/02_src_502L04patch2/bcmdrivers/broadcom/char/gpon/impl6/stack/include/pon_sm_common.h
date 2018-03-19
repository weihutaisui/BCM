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
#ifndef __PON_SM_COMMON_H
#define __PON_SM_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************/
/**                                                                        **/
/** Include files                                                          **/
/**                                                                        **/
/****************************************************************************/
#include "bl_gpon_stack_gpl.h"
#include "gpon_ploam.h"
#include "ngpon_ploam.h"
#include "rdpa_gpon_cfg.h"
#ifdef SIM_ENV
typedef struct
{
    uint32_t esc_be                :1 ;
    uint32_t esc_rogue             :1 ;
    uint32_t esc_mod_over_current  :1 ;
    uint32_t esc_bias_over_current :1 ;
    uint32_t esc_mpd_fault         :1 ;
    uint32_t esc_eye_safety        :1 ;
}PMD_ALARM_INDICATION_PARAMETERS_DTE;
#else
#include "gpon_tod_gpl.h"
#include "pmd.h"
#include "bdmf_shell.h"
#endif
#include "gpon_general.h"

/****************************************************************************/
/**                                                                        **/
/** Software definitions :                                                 **/
/**                                                                        **/
/****************************************************************************/

/******************************************************************************/
/* This type defines the general error codes returned by the various GPON     */
/* functions                                                                  */
/******************************************************************************/
#define PLOAM_PACKET_SIZE                       (512)
#define PLOAM_BUFFER_QUANTUM                    (512)

typedef uint32_t PON_ERROR_DTE ;

/* No errors or faults */
#define PON_NO_ERROR                                     ((PON_ERROR_DTE) 0)
/* General error */
#define PON_GENERAL_ERROR                                ((PON_ERROR_DTE) 1)
/* The function is called with invalid parameter */
#define PON_ERROR_INVALID_PARAMETER                      ((PON_ERROR_DTE) 2)
/* Software not initialized */
#define PON_ERROR_SW_NOT_INITIALIZED                     ((PON_ERROR_DTE) 3)
/* Software already initialized */
#define PON_ERROR_SW_ALREADY_INITIALIZED                 ((PON_ERROR_DTE) 4)
/* Software already terminated */
#define PON_ERROR_SW_ALREADY_TERMINATED                  ((PON_ERROR_DTE) 5)
/* General error */
#define PON_ERROR_GENERAL_ERROR                          ((PON_ERROR_DTE) 6)
/* Parameter Out-Of-Range */
#define PON_ERROR_PARAMETER_OUT_OF_RANGE_ERROR           ((PON_ERROR_DTE) 7)
/* Data base error */
#define PON_ERROR_INTERNAL_DB_ERROR                      ((PON_ERROR_DTE) 8)
/* flush done tcont is not done - time out */
#define PON_ERROR_TIME_OUT_FLUSH_IS_NOT_DONE             ((PON_ERROR_DTE) 9)
/* RT HW timer has not been initialized yet */
#define PON_HW_TIMER_IS_NOT_INITIALIZED                  ((PON_ERROR_DTE) 10)
/* DBA status reporting is already enabled */
#define  PON_DBR_ALREADY_ENABLED                         ((PON_ERROR_DTE) 11)
/* DBA status reporting is not enabled yet*/
#define  PON_DBR_NOT_ENABLED                             ((PON_ERROR_DTE) 12)
/* Upstream burst profiles haven't been received and enabled yet*/
#define  PON_BURST_PROF_NOT_ENABLED_YET                  ((PON_ERROR_DTE) 13)
/* Not enough memory */
#define PON_ERROR_NOT_ENOUGH_MEMORY                      ((PON_ERROR_DTE) 25)
/* Invalid object */
#define PON_ERROR_INVALID_OBJECT                         ((PON_ERROR_DTE) 30)
/* Driver error */
#define PON_ERROR_DRIVER_ERROR                           ((PON_ERROR_DTE) 31)
/* OS error */
#define PON_ERROR_OS_ERROR                               ((PON_ERROR_DTE) 32)
/* Invalid state */
#define PON_ERROR_INVALID_STATE                          ((PON_ERROR_DTE) 34)
/* Link not created */
#define PON_ERROR_LINK_NOT_CREATED                       ((PON_ERROR_DTE) 35)
/* Not supported */
#define PON_ERROR_NOT_SUPPORTED                          ((PON_ERROR_DTE) 36)
/* Illegal device id */
#define PON_ERROR_INVALID_INFO                           ((PON_ERROR_DTE) 50)
/* Illegal system clock timer ID of cpuif */
#define PON_ERROR_INVALID_SYSCLK_TIMER_ID                ((PON_ERROR_DTE) 1000)
/* Device already active */
#define PON_ERROR_LINK_ALREADY_ACTIVE                    ((PON_ERROR_DTE) 2200)
/* Device already inactive */
#define PON_ERROR_LINK_ALREADY_INACTIVE                  ((PON_ERROR_DTE) 2201)
/* Device not active */
#define PON_ERROR_LINK_NOT_ACTIVE                        ((PON_ERROR_DTE) 2202)
/* Device not inctive */
#define PON_ERROR_LINK_NOT_INACTIVE                      ((PON_ERROR_DTE) 2203)
/* No available hardware or software for the requested allocation. */
#define PON_ERROR_OUT_OF_RESOURCES                       ((PON_ERROR_DTE) 2300)
/* Illegal FIFO queue id ( e.g. in the GPON queue id is 0..31) */
#define PON_ERROR_QUEUE_ILLEGAL_ID                       ((PON_ERROR_DTE) 2400)
/* The FIFO queue is disabled for read/write operation */
#define PON_ERROR_QUEUE_DISABLED                         ((PON_ERROR_DTE) 2401)
/* FIFO queue is full */
#define PON_ERROR_QUEUE_FULL                             ((PON_ERROR_DTE) 2402)
/* FIFO queue is empty */
#define PON_ERROR_QUEUE_EMPTY                            ((PON_ERROR_DTE) 2403)
/* Tcont not assigned */
#define PON_ERROR_TCONT_NOT_ASSIGNED                     ((PON_ERROR_DTE) 2405)
/* ALLOC is already assign to another tcont */
#define PON_ERROR_ALLOC_ALREADY_ASSIGN_TO_ANOTHER_TCONT  ((PON_ERROR_DTE) 2406)
/* Tcont table if full */
#define PON_ERROR_TCONT_TABLE_IS_FULL                    ((PON_ERROR_DTE) 2407)
/* Alloc id already assign */
#define PON_ERROR_ALLOC_ALREADY_ASSIGN                   ((PON_ERROR_DTE) 2408)
/* Alloc id was not assign */
#define PON_ERROR_ALLOC_NOT_ASSIGNED                     ((PON_ERROR_DTE) 2409)
/* Alloc id was not assign */
#define PON_ERROR_ALLOC_NOT_IN_RANGE                     ((PON_ERROR_DTE) 2410)
/* Illegal grant id */
#define PON_ERROR_GRANT_ILLEGAL_ID                       ((PON_ERROR_DTE) 2500)
/* Invalid grant type */
#define PON_ERROR_GRANT_INVALID_TYPE                     ((PON_ERROR_DTE) 2501)
/* Port id already configured in other flow table instance */
#define PON_ERROR_PORT_ID_ALREADY_CFG                     ((PON_ERROR_DTE) 2502)
/* Tcont already configured */
#define PON_ERROR_TCONT_IS_ALREADY_CFG                    ((PON_ERROR_DTE) 2503)
/* The read/write operation from/to the GPON device failed due to opration timeout */
#define PON_ERROR_OPERATION_TIMEOUT                      ((PON_ERROR_DTE) 2600)
/* OS queue creating failed */
#define PON_ERROR_QUEUE_CREATE                       ((PON_ERROR_DTE) 3000)
/* OS queue send failed */
#define PON_ERROR_QUEUE_SEND                         ((PON_ERROR_DTE) 3002)
/* OS watchdog start failed */
#define PON_ERROR_WATCHDOG_START                     ((PON_ERROR_DTE) 3022)
/* IC failed */
#define PON_IC_ERROR                                     ((PON_ERROR_DTE) 3100)
/* FE buffer failure */
#define PON_FE_ERROR                                     ((PON_ERROR_DTE) 3200)
/*RT HW timer failure*/
#define PON_RT_HW_TIMER_ERROR                                 ((PON_ERROR_DTE) 3500)
//#endif

/******************************************************************************/
/* GPON/XGPON/NGPON MAC mode.                                                   */
/******************************************************************************/
#ifndef PON_MAC_MODE_DEFINED
#define PON_MAC_MODE_DEFINED

typedef uint32_t PON_MAC_MODE ;

#define NGPON_MODE_NGPON2_10G             0x2
#define NGPON_MODE_NGPON2_2_5G            0x3
#define NGPON_MODE_XGPON                  0x0
#define NGPON_MODE_ILLEGAL                0x1
#define NGPON_MODE_XGS                    0x4
#define GPON_MODE                         0x5

#endif /* PON_MAC_MODE_DEFINED */

/******************************************************************************/
/*                                                                            */
/* Gem block size                                                             */
/*                                                                            */
/******************************************************************************/
typedef uint32_t PON_GEM_BLOCK_SIZE_DTE;

#define GEM_BLOCK_SIZE_32_BYTES         ((PON_GEM_BLOCK_SIZE_DTE) 32)
#define GEM_BLOCK_SIZE_48_BYTES         ((PON_GEM_BLOCK_SIZE_DTE) 48)
#define GEM_BLOCK_SIZE_64_BYTES         ((PON_GEM_BLOCK_SIZE_DTE) 64)

/******************************************************************************/
/* This type describes the possible link states                               */
/******************************************************************************/
typedef uint32_t LINK_STATE_DTE;

/* Not ready */
#define LINK_STATE_NOT_READY       ((LINK_STATE_DTE) 0)
/* Inactive */
#define LINK_STATE_INACTIVE        ((LINK_STATE_DTE) 1)
/* Active, please see Link sub-state */
#define LINK_STATE_ACTIVE          ((LINK_STATE_DTE) 2)

/******************************************************************************/
/* This type describes the sub-state of the active link                       */
/******************************************************************************/
typedef uint32_t LINK_SUB_STATE_DTE;

/* None */
#define LINK_SUB_STATE_NONE             ((LINK_SUB_STATE_DTE) 0)
/* Standby */
#define LINK_SUB_STATE_STANDBY          ((LINK_SUB_STATE_DTE) 1)
/* Operation */
#define LINK_SUB_STATE_OPERATIONAL      ((LINK_SUB_STATE_DTE) 2)
/* Stop */
#define LINK_SUB_STATE_STOP             ((LINK_SUB_STATE_DTE) 3)

/******************************************************************************/
/* This type describes the Operation state of the link                        */
/******************************************************************************/
/* Define activation state machine states */
typedef uint32_t LINK_OPERATION_STATES_DTE;

#define OPERATION_STATE_O1                  ((LINK_OPERATION_STATES_DTE) 0)
#define OPERATION_STATE_O2                  ((LINK_OPERATION_STATES_DTE) 1)
#define OPERATION_STATE_O3                  ((LINK_OPERATION_STATES_DTE) 2)
#define OPERATION_STATE_O4                  ((LINK_OPERATION_STATES_DTE) 3)
#define OPERATION_STATE_O5                  ((LINK_OPERATION_STATES_DTE) 4)
#define OPERATION_STATE_O6                  ((LINK_OPERATION_STATES_DTE) 5)
#define OPERATION_STATE_O7                  ((LINK_OPERATION_STATES_DTE) 6)
#define OPERATION_STATE_O8                  ((LINK_OPERATION_STATES_DTE) 7)
#define OPERATION_STATE_O9                  ((LINK_OPERATION_STATES_DTE) 8)
#define OPERATION_STATES_NUMBER             ((LINK_OPERATION_STATES_DTE) OPERATION_STATE_O7+1)
#define OPERATION_XGPON_XGS_STATES_NUMBER   ((LINK_OPERATION_STATES_DTE) OPERATION_STATE_O7+1)
#define OPERATION_NGPON2_STATES_NUMBER      ((LINK_OPERATION_STATES_DTE) OPERATION_STATE_O9+1)

/******************************************************************************/
/* PON- flow id value type.                                                   */
/******************************************************************************/
#ifndef PON_FLOW_ID_DEFINED
#define PON_FLOW_ID_DEFINED

typedef uint32_t PON_FLOW_ID_DTE;

/* Standard PON-FLOW-ID */
#define FLOW_ID_LOW        ((PON_FLOW_ID_DTE) 0)
#define FLOW_ID_HIGH       ((PON_FLOW_ID_DTE) 255)
#define FLOW_ID_UNASSIGNED ((PON_FLOW_ID_DTE) 0xfff)

#define PON_FLOW_ID_DTE_IN_RANGE(v) ((v) >= FLOW_ID_LOW && (v) <= FLOW_ID_HIGH)

#endif

/******************************************************************************/
/* This type defines the Queue size range type.                               */
/******************************************************************************/
#ifndef PON_QUEUE_SIZE_DEFINED
#define PON_QUEUE_SIZE_DEFINED

typedef uint32_t PON_QUEUE_SIZE_DTE ;

#define PON_QUEUE_SIZE_DTE_LOW               (0)
#define PON_QUEUE_SIZE_DTE_HIGH              (20480)

#define PON_QUEUE_SIZE_DTE_IN_RANGE(v)       ((v) >= PON_QUEUE_SIZE_DTE_LOW && (v) <= PON_QUEUE_SIZE_DTE_HIGH)

#endif

/******************************************************************************/
/* This type defines the reasons for ranging stop types                       */
/******************************************************************************/
typedef uint32_t PON_RANGING_STOP_REASONS_INDICATION ;

/* Device deactivate */
#define PON_RANGING_STOP_REASON_LINK_DEACTIVATE   ((PON_RANGING_STOP_REASONS_INDICATION) 0)
/* Alarm On */
#define PON_RANGING_STOP_REASON_ALARM_ON          ((PON_RANGING_STOP_REASONS_INDICATION) 1)
/* SN Unmatch */
#define PON_RANGING_STOP_REASON_SN_UNMATCH        ((PON_RANGING_STOP_REASONS_INDICATION) 2)
/* Deactivate Pon Id */
#define PON_RANGING_STOP_REASON_DEACTIVATE_PON_ID ((PON_RANGING_STOP_REASONS_INDICATION) 3)
/* Disable SN */
#define PON_RANGING_STOP_REASON_DISABLE_SN        ((PON_RANGING_STOP_REASONS_INDICATION) 4)
/* Ranging End */
#define PON_RANGING_STOP_REASON_RANGING_END       ((PON_RANGING_STOP_REASONS_INDICATION) 5)
/* Timeout Expired */
#define PON_RANGING_STOP_REASON_TIMEOUT_EXPIRED   ((PON_RANGING_STOP_REASONS_INDICATION) 6)
/* Number of ranging stop reasos */
#define PON_RANGING_STOP_REASONS_NUMBER           ((PON_RANGING_STOP_REASONS_INDICATION) 7)

/******************************************************************************/
/* This type defines the Alloc id types
 * TODO check                                     */
/******************************************************************************/
typedef uint8_t PON_ALLOC_ID_TYPES_DTE;

#define PON_ALLOC_ID_TYPE_ATM     ((PON_ALLOC_ID_TYPES_DTE) CE_PAYLOAD_TYPE_ATM)

#define PON_ALLOC_ID_TYPE_GEM     ((PON_ALLOC_ID_TYPES_DTE) CE_PAYLOAD_TYPE_GEM)

#define PON_ALLOC_ID_TYPE_DBA     ((PON_ALLOC_ID_TYPES_DTE) CE_PAYLOAD_TYPE_DBA)

#define PON_ALLOC_ID_TYPE_DEALLOC ((PON_ALLOC_ID_TYPES_DTE) CE_PAYLOAD_TYPE_DEALLOC)

/******************************************************************************/
/* PON_Id field in the ploam messages
 * TODO check                             */
/******************************************************************************/
#ifndef PON_ID_DEFINED
#define PON_ID_DEFINED

typedef uint8_t PON_ID_DTE;

#define PON_ID_DTE_LOW          (0)
#define PON_ID_DTE_HIGH         (255)

#define PON_ID_DTE_IN_RANGE(v)  ((v) >= PON_ID_DTE_LOW && (v) <= PON_ID_DTE_HIGH)

#endif

/******************************************************************************/
/* PON-ID value type.                                                         */
/******************************************************************************/
#ifndef PON_ID_VALUE_DEFINED
#define PON_ID_VALUE_DEFINED

typedef uint32_t PON_ID_VALUE_DTE;

/* Standard broadcast PON-ID */
#define PON_ID_BROADCAST        ((PON_ID_VALUE_DTE) 0xFF)

#endif

/******************************************************************************/
/* PON- Port id value type.                                                   */
/******************************************************************************/
#ifndef PON_PORT_ID_DEFINED
#define PON_PORT_ID_DEFINED

typedef uint32_t PON_PORT_ID_DTE ;

#endif

/******************************************************************************/
/* PON- flow type value type.                                                   */
/******************************************************************************/
#ifndef PON_FLOW_TYPE_DEFINED
#define PON_FLOW_TYPE_DEFINED

typedef bool PON_FLOW_TYPE_DTE ;

/* Standard PON-FLOW-TYPE */
#define PON_FLOW_TYPE_ETH        ((PON_FLOW_TYPE_DTE) BDMF_FALSE)
#define PON_FLOW_TYPE_TDM        ((PON_FLOW_TYPE_DTE) BDMF_TRUE)

#endif

/******************************************************************************/
/* PON- flow type .                                                           */
/******************************************************************************/
#ifndef PON_FLOW_PRIORITY_DEFINED
#define PON_FLOW_PRIORITY_DEFINED

typedef uint32_t PON_FLOW_PRIORITY_DTE;

#define PON_FLOW_PRIORITY_LOW           ((PON_FLOW_PRIORITY_DTE) 0) /* Reset value */
#define PON_FLOW_PRIORITY_HIGH          ((PON_FLOW_PRIORITY_DTE) 1)
#define PON_FLOW_PRIORITY_PLOAM         ((PON_FLOW_PRIORITY_DTE) 2)
#define PON_FLOW_PRIORITY_EXCLUSIVE     ((PON_FLOW_PRIORITY_DTE) 3)
#define PON_FLOW_PRIORITY_OMCI          PON_FLOW_PRIORITY_EXCLUSIVE
#define PON_FLOW_PRIORITY_UNASSIGNED    ((PON_FLOW_PRIORITY_DTE) 0xff)
#define PON_FLOW_PRIORITY_MAX           ((PON_FLOW_PRIORITY_DTE) PON_FLOW_PRIORITY_UNASSIGNED)

#define GPON_PON_FLOW_PRIORITY_IN_RANGE(v) ((v) >= PON_FLOW_PRIORITY_HIGH && (v) <= PON_FLOW_PRIORITY_OMCI)
#define NGPON_PON_FLOW_PRIORITY_IN_RANGE(v) ((v) == PON_FLOW_PRIORITY_HIGH || (v) == PON_FLOW_PRIORITY_EXCLUSIVE)

#endif

/******************************************************************************/
/* This type defines the T-CONT id type.                                      */
/******************************************************************************/
#ifndef PON_TCONT_IDENTIFIER_DEFINED
#define PON_TCONT_IDENTIFIER_DEFINED

typedef uint32_t PON_TCONT_ID_DTE ;

#define PON_TCONT_ID_DTE_LOW               ((PON_TCONT_ID_DTE) 0)
#define PON_TCONT_ID_DTE_HIGH              ((PON_TCONT_ID_DTE) 39)

#define PON_TCONT_ID_DTE_IN_RANGE(v)       ((v) >= PON_TCONT_ID_DTE_LOW && (v) <= PON_TCONT_ID_DTE_HIGH)

#endif

/******************************************************************************/
/* This type defines the Alloc id values.                                     */
/******************************************************************************/
#ifndef PON_ALLOC_ID_DEFINED
#define PON_ALLOC_ID_DEFINED

typedef uint16_t PON_ALLOC_ID_DTE ;

#define PON_ALLOC_ID_DTE_LOW               ((PON_ALLOC_ID_DTE) 0)
#define PON_ALLOC_ID_DTE_HIGH              ((PON_ALLOC_ID_DTE) 4095)
#define PON_ALLOC_ID_DTE_NOT_ASSIGNED      ((PON_ALLOC_ID_DTE) 0xFFFF)

#define PON_ALLOC_ID_DTE_IN_RANGE(v)       ((v) >= PON_ALLOC_ID_DTE_LOW && (v) <= PON_ALLOC_ID_DTE_HIGH)

#endif

/******************************************************************************/
/*                                                                            */
/* OAM alarm status                                                           */
/*                                                                            */
/******************************************************************************/
#ifndef PON_OAM_ALARMS_STATUS_DEFINED
#define PON_OAM_ALARMS_STATUS_DEFINED

typedef bool PON_OAM_ALARM_STATUS_DTE ;

#define CE_OAM_ALARM_STATUS_ON     ( ( PON_OAM_ALARM_STATUS_DTE ) BDMF_TRUE )

#define CE_OAM_ALARM_STATUS_OFF    ( ( PON_OAM_ALARM_STATUS_DTE ) BDMF_FALSE )

#endif


/******************************************************************************/
/* This type defines the Alarm status struct                                  */
/******************************************************************************/
typedef struct
{
    /* Dis indication */
    bool dis ;

    /* Dact indication */
    bool dact ;

    /* Mem indication */
    bool mem ;

    /* Sd indication */
    bool sd ;

    /* Sf indication */
    bool sf ;

    /* Suf indication */
    bool suf ;

    /* Err indication */
    bool err ;

    /* Pee_Olt indication */
    bool pee_olt ;

    /* Tf indication */
    bool tf ;

    /* Lcd indication */
    bool lcd ;

    /* Oaml indication */
    bool oaml ;

    /* Los indication */
    bool los ;

    /* Onu state transition indication */
    bool onu_state_transition ;

    /* Bip Error indication */
    bool bip_error ;

    /* Keep alive indication */
    bool keep_alive ;

    /* Ranging Start indication */
    bool ranging_start ;

    /* Ranging Stop indication */
    bool ranging_stop ;

    /* Vendor Specific indication */
    bool vendor_specific ;

    /* Unknown Message indication */
    bool unknown_message ;
}
PON_ALARM_STATUS_DTE ;

/******************************************************************************/
/* This type define the LCDG indication parameters                             */
/******************************************************************************/
typedef struct
{
    bool alarm ;
}
PON_ALARM_INDICATION;

/******************************************************************************/
/* This type define the ROGUE LEVEL indication parameters                             */
/******************************************************************************/
typedef struct
{
    bool alarm ;
    BL_ROGUE_ONU_MODE_DTE rogue_type ;
}
PON_ROGUE_ONU_INDICATION;

/******************************************************************************/
/* This type define the MEM indication parameters                             */
/******************************************************************************/
typedef struct
{
    bool alarm ;
    GPON_DS_PLOAM unknown_msg ;
}
GPON_MEM_INDICATION;

/******************************************************************************/
/* This type define the MEM indication parameters                             */
/******************************************************************************/
typedef struct
{
    bool alarm ;
    NGPON_DS_PLOAM unknown_msg ;
}
NGPON_MEM_INDICATION;

/******************************************************************************/
/* This type define the OAML indication parameters                           */
/******************************************************************************/
typedef struct
{
    PON_ALARM_STATUS_DTE oaml_alarm_status ;
}
PON_OAML_INDICATION;


/******************************************************************************/
/* This type define the SD indication parameters                              */
/******************************************************************************/
typedef struct
{
    /* user bip rate */
    uint32_t user_bip_rate ;
    bool alarm ;
}
PON_SD_SF_INDICATION;

/******************************************************************************/
/* This type define the ONU state transition indication parameters            */
/******************************************************************************/
typedef struct
{
    /* ONU Old States */
    LINK_SUB_STATE_DTE onu_old_sub_state ;

    /* ONU New States */
    LINK_SUB_STATE_DTE onu_new_sub_state ;
}
PON_ONU_STATE_TRANSITION_INDICATION;

/******************************************************************************/
/* This type define the ONU Link state transition indication parameters       */
/******************************************************************************/
typedef struct
{
    /* ONU Old States */
    LINK_OPERATION_STATES_DTE onu_old_link_state ;

    /* ONU New States */
    LINK_OPERATION_STATES_DTE onu_new_link_state ;
}
PON_ONU_LINK_STATE_TRANSITION_INDICATION;


/******************************************************************************/
/* This type define the AES indication parameters                             */
/******************************************************************************/
typedef struct
{
    /* Key switching time */
    uint32_t current_superframe ;

    /* Key switching time */
    uint32_t key_switching_time ;

    /* Error id */
    uint32_t error_id ;
}
PON_AES_INDICATION;

/******************************************************************************/
/* This type define the bip error  indication parameters                      */
/******************************************************************************/
typedef struct
{
    /* Bip Error */
    uint32_t bip_error ;

    /* Bod error alarm status */
    bool alarm ;
}
PON_ERROR_INDICATION;

/******************************************************************************/
/* This type define the keep alive  indication parameters                     */
/******************************************************************************/
typedef struct
{
    uint32_t keepalive_time_value ;
}
PON_KEEP_ALIVE_INDICATION;

/******************************************************************************/
/* This type define the ranging start indication parameters                   */
/******************************************************************************/
typedef struct
{
}
PON_RANGING_START_INDICATION;

/******************************************************************************/
/* This type define the ranging stop  indication parameters                   */
/******************************************************************************/
typedef struct
{
    /* Ranging stop reason */
    PON_RANGING_STOP_REASONS_INDICATION ranging_stop_reason ;
}
PON_RANGING_STOP_INDICATION;

/******************************************************************************/
/* This type define the assign alloc id indication parameters                 */
/******************************************************************************/
typedef struct
{
    /* Alloc id */
    PON_ALLOC_ID_DTE alloc_id ;

    /* Assign/Deassign flag */
    bool assign_flag ;

    /* Alloc-id type */
    PON_ALLOC_ID_TYPES_DTE alloc_id_type ;
}
PON_ASSIGN_ALLOC_ID_INDICATION;

/******************************************************************************/
/* This type define the assign ONU id indication parameters                   */
/******************************************************************************/
typedef struct
{
    /* ONU id */
        PON_ID_VALUE_DTE onu_id ;
}
PON_ASSIGN_ONU_ID_INDICATION;

/******************************************************************************/
/* This type define the enable/disable filter indication parameters           */
/******************************************************************************/
typedef struct
{
    /* Flow / filter id */
    PON_FLOW_ID_DTE flow_id ;

    /* Enable / Disable filter */
    bool configure_flag ;
}
PON_CTRL_FILTER_INDICATION;

/*****************************************************************************************/
/* This type define the enable/disable and configuration of filter indication parameters */
/*****************************************************************************************/
typedef struct
{
    /* Flow / filter id */
    PON_FLOW_ID_DTE flow_id ;

    /* Enable / Disable filter */
    bool configure_flag ;

    /* port id */
    PON_PORT_ID_DTE port_id ;

    /* port mask id */
    PON_PORT_ID_DTE port_mask_id ;

    /* Enable / Disable encryption filter */
    bool encryption_mode ;

    /* Flow type - ( ETH/TDM) */
    PON_FLOW_TYPE_DTE flow_type ;

    /* Flow priority - (Low/High/Omci) */
    PON_FLOW_PRIORITY_DTE flow_priority;
}
PON_CONFIGURE_FILTER_INDICATION;

/******************************************************************************/
/* This type define the configure OMCI port id indication parameters               */
/******************************************************************************/
typedef struct
{
    /* Activate/Deactivate port-id flag */
    bool port_control ;

    /* Downstream port id */
    PON_PORT_ID_DTE port_id ;
}
PON_CONFIGURE_OMCI_PORT_ID_INDICATION;

/********************************************************************/
/* This type define the OPERATIONAL LOF indication parameters       */
/********************************************************************/
typedef struct
{
}
PON_OPERATIONAL_LOF_INDICATION;

/******************************************************************************/
/* This type define the PST indication parameters                             */
/******************************************************************************/
typedef struct
{
    /* line_number */
    bool line_number ;

    /* k1 */
    uint8_t k1 ;

    /* k2 */
    uint8_t k2 ;

    /* PST message */
    GPON_DS_PLOAM pst_msg ;
}
PON_PST_INDICATION_PARAMETERS_DTE;


/******************************************************************************/
/* This type define the parameters recieves by all indications                */
/******************************************************************************/
typedef union
{
    /* Lcdg indication */
        PON_ALARM_INDICATION lcdg;

    /* Lof indication */
        PON_ALARM_INDICATION lof;

    /* Dis indication */
        PON_ALARM_INDICATION dis;

    /* Dact indication */
        PON_ALARM_INDICATION dact;

    /* Mem indication */
        GPON_MEM_INDICATION gpon_mem;

    /* Mem indication */
    NGPON_MEM_INDICATION ngpon_mem;

    /* Oaml indication */
        PON_OAML_INDICATION oaml;

    /* Sd indication */
        PON_SD_SF_INDICATION sd;

    /* Sf indication */
        PON_SD_SF_INDICATION sf;

    /* Onu state transition indication */
        PON_ONU_STATE_TRANSITION_INDICATION onu_state_transition;

    /* Onu state transition indication */
        PON_ONU_LINK_STATE_TRANSITION_INDICATION onu_link_state_transition;

    /* Bip Error indication */
        PON_ERROR_INDICATION bip_error;

    /* SUF Error */
    PON_ALARM_INDICATION suf;

    /* Keep alive indication */
    PON_KEEP_ALIVE_INDICATION keep_alive;

    /* Ranging Start indication */
    PON_RANGING_START_INDICATION ranging_start;

    /* Ranging Stop indication */
    PON_RANGING_STOP_INDICATION ranging_stop;

    /* Assign Alloc Id indication */
    PON_ASSIGN_ALLOC_ID_INDICATION assign_alloc_id ;

    /* Assign ONU ID indication */
    PON_ASSIGN_ONU_ID_INDICATION assign_onu_id ;

    /* Control port indiction */
    PON_CTRL_FILTER_INDICATION control_port ;

    /* Configure port indication */
    PON_CONFIGURE_FILTER_INDICATION configure_port ;

    /* Configure OMCI port indication */
    PON_CONFIGURE_OMCI_PORT_ID_INDICATION configure_omci_port ;

    /* Pee message from OLT */
    PON_ALARM_INDICATION pee_olt;

    /* PST message from OLT */
    PON_PST_INDICATION_PARAMETERS_DTE pst ;

    /* AES messages from OLT */
    PON_AES_INDICATION aes ;

    /* OPERATIONAL LOF indication */
    PON_OPERATIONAL_LOF_INDICATION operational_lof ;

    /* OPERATIONAL ROGUE ONU indication */
    PON_ROGUE_ONU_INDICATION rogue_onu ;

    /* OPERATIONAL PMD alarm */
    PMD_ALARM_INDICATION_PARAMETERS_DTE pmd_alarm;
}
PON_INDICATIONS;

typedef struct
{
    uint32_t stack_bip_error_counter ;

    uint32_t gmp_bip_error_counter ;

    uint32_t stack_crc_counter;

    uint32_t gmp_crc_counter;
}
PON_COUNTERS_DTE;



/******************************************************************************/
/* This type defines the ONU serial number                                    */
/******************************************************************************/
#define VENDOR_CODE_FIELD_SIZE   (4)
#define VENDOR_SPECIFIC_FIELD_SIZE (4)

typedef struct
{
    /* Vendor code */
    uint8_t vendor_code[VENDOR_CODE_FIELD_SIZE] ;

    /* Serial number */
    uint8_t serial_number[VENDOR_SPECIFIC_FIELD_SIZE] ;
}
PON_SERIAL_NUMBER_DTE;

/******************************************************************************/
/* This type defines the ONU password for PON network access (see             */
/* Recommendation ITU-T/G.984.3)                                             */
/******************************************************************************/
#ifndef PON_PASSWORD_DEFINED
#define PON_PASSWORD_DEFINED

#define PON_PASSWORD_FIELD_SIZE 36 //(10)

typedef struct
{
    /* PON password */
    uint8_t password [PON_PASSWORD_FIELD_SIZE] __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}
__PACKING_ATTRIBUTE_STRUCT_END__

PON_PASSWORD_DTE;
#endif



/*************************************************************************************/
/*                                                                                   */
/* This type defines the opcodes of the messages destinate to user callback function */
/*                                                                                   */
/*************************************************************************************/
#ifndef PON_MESSAGES_TYPE_DEFINED
#define PON_MESSAGES_TYPE_DEFINED

typedef uint32_t PON_API_MESSAGE_OPCODES_DTE;

/* This opcode indicates a state transition to one of the next states: STOP, STANDBY or OPERATIONAL. */
#define PON_API_MESSAGE_OPCODE_GPON_SM_TRANSITION                    ((PON_API_MESSAGE_OPCODES_DTE) 0x0)
/* This opcode indicates a link state transition operation state changed */
#define PON_API_MESSAGE_OPCODE_GPON_LINK_STATE_TRANSITION            ((PON_API_MESSAGE_OPCODES_DTE) 0x1)
/* This opcode indicates that assign alloc id message has been sent by the OLT. */
#define PON_API_MESSAGE_OPCODE_ASSIGN_ALLOC_ID_MESSAGE               ((PON_API_MESSAGE_OPCODES_DTE) 0x4)
/* This opcode indicates that a divided slot grant allocation message has been sent by the OLT. */
#define PON_API_MESSAGE_OPCODE_DIVIDED_SLOT_GRANT_ALLOCATION_MESSAGE ((PON_API_MESSAGE_OPCODES_DTE) 0x5)
/* This opcode indicates that a vendor specific message has been sent by the OLT. */
#define PON_API_MESSAGE_OPCODE_VENDOR_SPECIFIC_MESSAGE               ((PON_API_MESSAGE_OPCODES_DTE) 0x6)
/* This opcode indicates that an unknown message has been sent by the OLT. */
#define PON_API_MESSAGE_OPCODE_UNKNOWN_MESSAGE                       ((PON_API_MESSAGE_OPCODES_DTE) 0x7)
/* This opcode indicates that a PST message has been sent by the OLT. */
#define PON_API_MESSAGE_OPCODE_PST_MESSAGE                           ((PON_API_MESSAGE_OPCODES_DTE) 0x8)
/* This opcode indicates a keep alive timeout, since a PLOAM message has not been sent for x seconds. */
#define PON_API_MESSAGE_OPCODE_KEEPALIVE_TIMEOUT                     ((PON_API_MESSAGE_OPCODES_DTE) 0xB)
/* This opcode indicates that FIFO full interrupt from the RX/TX FIFO was issued. */
#define PON_API_MESSAGE_OPCODE_FIFO_FULL                             ((PON_API_MESSAGE_OPCODES_DTE) 0xE)
/* This opcode indicates queue event happened (one or more of the queue interrupts in the FIFO was active .) */
#define PON_API_MESSAGE_OPCODE_QUEUE_EVENT                           ((PON_API_MESSAGE_OPCODES_DTE) 0x10)
/* This opcode indicates an software error in the apon stack in base level. */
#define PON_API_MESSAGE_OPCODE_BASE_SW_ERROR                         ((PON_API_MESSAGE_OPCODES_DTE) 0x11)
/* This opcode indictaes an OAM event/alarm */
#define PON_API_MESSAGE_OPCODE_OAM_EVENT                             ((PON_API_MESSAGE_OPCODES_DTE) 0x13)
/* This opcode indictaes Churn_VP message received. */
#define PON_API_MESSAGE_OPCODE_CHURN_VP_MESSAGE                      ((PON_API_MESSAGE_OPCODES_DTE) 0x16)
/* This opcode indictaes BIP error in the downstream. */
#define PON_API_MESSAGE_OPCODE_BIP_ERROR                             ((PON_API_MESSAGE_OPCODES_DTE) 0x17)
/* This opcode indictaes a link ( internal) error interrupt */
#define PON_API_MESSAGE_OPCODE_LINK_ERROR                            ((PON_API_MESSAGE_OPCODES_DTE) 0x19)
/* This opcode indictaes start of ranging. */
#define PON_API_MESSAGE_OPCODE_RANGING_START                         ((PON_API_MESSAGE_OPCODES_DTE) 0x1a)
/* This opcode indictaes stop of ranging */
#define PON_API_MESSAGE_OPCODE_RANGING_STOP                          ((PON_API_MESSAGE_OPCODES_DTE) 0x1b)
/* This opcode indictates encrypt port id */
#define PON_API_MESSAGE_OPCODE_ENCRYPT_PORT_ID                       ((PON_API_MESSAGE_OPCODES_DTE) 0x1c)
/* This opcode indictates configure OMCI port id */
#define PON_API_MESSAGE_OPCODE_CONFIGURE_OMCI_PORT_ID                ((PON_API_MESSAGE_OPCODES_DTE) 0x1d)
/* This opcode indictates AES message */
#define PON_API_MESSAGE_OPCODE_AES_KEY_SWITCHING_TIME                ((PON_API_MESSAGE_OPCODES_DTE) 0x1e)
/* This opcode indictates AES error */
#define PON_API_MESSAGE_OPCODE_AES_ERROR                             ((PON_API_MESSAGE_OPCODES_DTE) 0x1f)
/* This opcode indictates OPERATIONAL LOF indication */
#define PON_API_MESSAGE_OPCODE_OPERATIONAL_LOF                       ((PON_API_MESSAGE_OPCODES_DTE) 0x20)

#if defined ( MAC_CLEANUP)
/* This opcode indictates Non-Critical BER indication */
#define PON_API_MESSAGE_OPCODE_NON_CRITICAL_BER                      ((PON_API_MESSAGE_OPCODES_DTE) 0x21)
/* This opcode indictates Critical BER indication */
#define PON_API_MESSAGE_OPCODE_CRITICAL_BER                          ((PON_API_MESSAGE_OPCODES_DTE) 0x22)
#endif /* MAC_CLEANUP */

/* This opcode indicates that assign ONU id message has been sent by the OLT. */
#define PON_API_MESSAGE_OPCODE_ASSIGN_ONU_ID_MESSAGE                 ((PON_API_MESSAGE_OPCODES_DTE) 0x23)
/* This opcode indicates about rogue indication. */
#define PON_API_MESSAGE_OPCODE_ROGUE_ONU                             ((PON_API_MESSAGE_OPCODES_DTE) 0x24)

#define PON_API_MESSAGE_OPCODE_PMD_ALARM                             ((PON_API_MESSAGE_OPCODES_DTE) 0x25)

#endif

/******************************************************************************/
/*                                                                            */
/* Pon messaage id                                                            */
/*                                                                            */
/******************************************************************************/

typedef enum
{
    PON_EVENT_PST_PLOAM = 0 ,
    PON_EVENT_VENDOR_SPECIFIC ,
    PON_EVENT_KEEPALIVE_TIMEOUT ,
    PON_EVENT_USER_INDICATION ,
    PON_EVENT_QUEUE_EVENT ,
    PON_EVENT_OAM ,
    PON_EVENT_LINK_ERROR ,
    PON_EVENT_BER_TIMEOUT ,
    PON_EVENT_CRITICAL_BER_TIMEOUT,
    PON_EVENT_PEE_TIMEOUT,
    PON_EVENT_STATE_TRANSITION,
    PON_EVENT_CTRL_BER_TIMER,
    PON_EVENT_CTRL_PEE_TIMER,
    PON_EVENT_ASSIGN_CALLBACKS ,
    PON_EVENT_CHANGE_POWER_LEVEL,
    PON_EVENT_LINK_STATE_TRANSITION
}
PON_EVENTS_DTE;

/******************************************************************************/
/* This type defines the GPON indications types                               */
/******************************************************************************/
typedef uint32_t PON_INDICATIONS_DTE ;

/* Disable_Serial_Number message with match PON_Id and ENABLE flag*/
#define PON_INDICATION_DIS                 ((PON_INDICATIONS_DTE) 0)
/* PST Message */
#define PON_INDICATION_PST                 ((PON_INDICATIONS_DTE) 1)
/* DACT indication */
#define PON_INDICATION_DACT                ((PON_INDICATIONS_DTE) 2)
/* MEM indication */
#define GPON_INDICATION_MEM                 ((PON_INDICATIONS_DTE) 3)
/* SF indication */
#define PON_INDICATION_SF                  ((PON_INDICATIONS_DTE) 4)
/* SD indication */
#define PON_INDICATION_SD                  ((PON_INDICATIONS_DTE) 5)
/* SUF indication - Ranging Stop indication used instead */
#define PON_INDICATION_SUF                 ((PON_INDICATIONS_DTE) 6)
/* ERR indication */
#define PON_INDICATION_ERR                 ((PON_INDICATIONS_DTE) 7)
/* PEE_OLT indication */
#define PON_INDICATION_PEE_OLT             ((PON_INDICATIONS_DTE) 8)
/* TF indication */
#define PON_INDICATION_TF                  ((PON_INDICATIONS_DTE) 9)
/* LCDG indication */
#define PON_INDICATION_LCDG                ((PON_INDICATIONS_DTE) 10)
/* FRML indication */
#define PON_INDICATION_FRML                ((PON_INDICATIONS_DTE) 11)
/* OAML indication */
#define PON_INDICATION_OAML                ((PON_INDICATIONS_DTE) 12)
/* LOF indication */
#define PON_INDICATION_LOF                 ((PON_INDICATIONS_DTE) 13)
/* ONU state machine changed indication */
#define PON_INDICATION_ONU_STATE_CHANGE    ((PON_INDICATIONS_DTE) 14)
/* ADD or Remove indication of Alloc-id */
#define PON_INDICATION_ASSIGN_ALLOC_ID     ((PON_INDICATIONS_DTE) 15)
/* Assign indication of ONU-id */
#define PON_INDICATION_ASSIGN_ONU_ID       ((PON_INDICATIONS_DTE) 16)
/* Configure OMCI port id indication */
#define PON_INDICATION_CONFIGURE_OMCI_PORT_ID_FILTER ((PON_INDICATIONS_DTE) 17)
/* Modify encryption port id indication */
#define PON_INDICATION_MODIFY_ENCRYPTION_PORT_ID_FILTER ((PON_INDICATIONS_DTE) 18)
/* BIP Error indication */
#define PON_INDICATION_BIP_ERROR                      ((PON_INDICATIONS_DTE) 23)
/* Keep alive timeout indication */
#define PON_INDICATION_KEEP_ALIVE_TIMEOUT             ((PON_INDICATIONS_DTE) 24)
/* Ranging Start indication */
#define PON_INDICATION_RANGING_START                  ((PON_INDICATIONS_DTE) 25)
/* Ranging Stop indication */
#define PON_INDICATION_RANGING_STOP                   ((PON_INDICATIONS_DTE) 26)
/* Vendor Specific Message indication */
#define PON_INDICATION_VENDOR_SPECIFIC_MESSAGE        ((PON_INDICATIONS_DTE) 27)
/* AES Key switching time Message indication */
#define PON_INDICATION_AES_KEY_SWITCHING_TIME_MESSAGE ((PON_INDICATIONS_DTE) 28)
/* AES error Message indication */
#define PON_INDICATION_AES_ERROR                      ((PON_INDICATIONS_DTE) 29)
/* Unknown Message Indication */
#define PON_INDICATION_UNKNOWN_MESSAGE                ((PON_INDICATIONS_DTE) 30)
/* OPERATIONAL LOF indication */
#define PON_INDICATION_OPERATIONAL_LOF                ((PON_INDICATIONS_DTE) 31)
/* OPERATIONAL rogue level indication */
#define PON_INDICATION_ROGUE_ONU                      ((PON_INDICATIONS_DTE) 32)

#define PON_INDICATION_PMD_ALARM                      ((PON_INDICATIONS_DTE) 33)

#define NGPON_INDICATION_MEM                          ((PON_INDICATIONS_DTE) 33)


/*****************************************************************************/
/* This type defines the FIFO units in the ONU chip.                         */
/*****************************************************************************/
#ifndef PON_FIFO_UNIT_ID_DEFINED
#define PON_FIFO_UNIT_ID_DEFINED

typedef uint32_t PON_FIFO_ID_DTE ;

/* Tx-FIFO unit */
#define TXPON_FIFO_UNIT_LOW    ((PON_FIFO_ID_DTE) 0)

#define TXPON_FIFO_UNIT_HIGH   ((PON_FIFO_ID_DTE) 31)

#define TXPON_FIFO_UNIT_NUMBER ((PON_FIFO_ID_DTE) (TXPON_FIFO_UNIT_HIGH + 1))

#define TXPON_FIFO_ID_DTE_IN_RANGE(v) ((v) >= TXPON_FIFO_UNIT_LOW && (v) <= TXPON_FIFO_UNIT_HIGH)

#endif

typedef enum
{
    CE_COUNTER_STACK_REQUEST,
    CE_COUNTER_RDPA_REQUEST
}
PON_RXPON_REQUEST_DTE ;

/******************************************************************************/
/*                                                                            */
/* structure of pon overhead                                                  */
/*                                                                            */
/******************************************************************************/
#define PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES 16

typedef struct
{
    uint8_t overhead[PON_TX_PREAMBLE_OVERHEAD_SIZE_IN_BYTES];
}
PON_TX_OVERHEAD_DTE;

/* RX Ploam counters  */
typedef struct
{
     uint32_t  valid_onu_id_ploam_counter;
     uint32_t  valid_broadcast_ploam_counter;
}
PON_RX_PLOAM_COUNTERS_DTE;

/* RX Pm Hec counters  */
typedef struct
{
     uint32_t bwmap_hec_err_counter;
     uint32_t hlend_hec_err_counter;
     uint32_t sync_ponid_hec_err_counter;
     uint32_t bwmap_hec_fix_counter;
     uint32_t hlend_hec_fix_counter;
     uint32_t xhp_hec_fix_counter;
     uint32_t sync_sfc_hec_fix_counter;
     uint32_t sync_ponid_hec_fix_counter;
}
PON_RX_HEC_COUNTERS_DTE;


typedef struct
{
    uint32_t sync_lof_counter;
    uint32_t bwmap_hec_err_counter;
    uint32_t bwmap_invalid_burst_series_counter;
    uint32_t hlend_hec_err_counter;
    uint32_t del_lcdg_counter;
    uint32_t sync_ponid_hec_err_counter;
    uint32_t del_pass_pkt_counter;
    uint32_t bwmap_correct_counter;
    uint32_t bwmap_bursts_counter;
    uint32_t xhp_pass_pkt_counter;

    uint32_t dec_cant_allocate_counter;
    uint32_t dec_invalid_key_idx_counter;
    uint32_t plp_valid_onuid_counter[3];
    uint32_t plp_valid_bcst_counter;
    uint32_t mic_err_counter;
    uint32_t bwmap_hec_fix_counter;
    uint32_t hlend_hec_fix_counter;
    uint32_t xhp_hec_fix_counter;
    uint32_t sync_sfc_hec_fix_counter;
    uint32_t sync_ponid_hec_fix_counter;
    uint32_t xgem_overrun_counter;
    uint32_t bwmap_discard_dis_tx_counter;
    uint32_t fec_bit_err_counter;
    uint32_t fec_sym_err_counter;
    uint32_t fec_cw_err_counter;
    uint32_t fec_uc_cw_counter;
    uint32_t bwmap_num_alloc_tcont_counter[3];
    uint32_t bwmap_tot_bw_tcont_counter[3];
    uint32_t fs_bip_err_counter;
}
PON_RXPON_PM_COUNTERS;

/******************************************************************************/
/* This type defines the interrupts mask for the blocks level  interrupts     */
/******************************************************************************/

typedef struct
{
    bool txpon_interrupt ; /* This bit represents the TxPON block interrupts */
    bool rxpon_interrupt ; /* This bit represents the RxPON block interrupts */
    bool ploam_received_interrupt ; /* This bit represents the Runner block interrupts due to receiving Ploam */
}
PON_INTERRUPTS_MASK_DTE;

typedef struct
{
    uint32_t indication_id ;
    PON_INDICATIONS indication_data ;
}
PON_USER_INDICATION_DTE;


/******************************************************************************/
/* This type defines the TCONT table structs                                  */
/******************************************************************************/

/* T-CONT entry for the table of TCONTs */
typedef struct
{
    /* Alloc value */
    PON_ALLOC_ID_DTE alloc_id;

        /* T-CONT Queue Id */
        PON_TCONT_ID_DTE tcont_id;

    /* Tcont queue size */
    PON_QUEUE_SIZE_DTE tcont_queue_size;

    /* Assign flag - determine if assign alloc id message arrived */
        bool assign_alloc_valid_flag;

    /* Configure tcont flag - determine if the user call configure tcont API to this tcont*/
    bool config_tcont_valid_flag;

 }
PON_TCONT_DTE;

typedef struct
{
    uint8_t pon_id_type;
    uint8_t pon_identifier[7];
    uint8_t tx_optical_level[2];
}
PON_ID_PARAMETERS;

/******************************************************************************/
/*                                                                            */
/* structure of Rogue ONU paramters                                           */
/*                                                                            */
/******************************************************************************/
typedef struct
{
    bdmf_boolean rogue_onu_level_clear;
    bdmf_boolean rogue_onu_diff_clear;
    bdmf_boolean source_select;
    uint32_t rogue_onu_level_threshold; 
    uint16_t rogue_onu_diff_threshold;
}
PON_TX_ROGUE_ONU_PARAMETERS;


typedef struct
{
    bool  source_indication ;
    uint32_t   rogue_onu_level_threshold;
    uint32_t   rogue_onu_diff_threshold;
}
ROGUE_ONU_DETECTION_PARAMETERS ;


/* Rogue ONU interrupts processing */
typedef enum
{
  ROGUE_ONU_TX_INT_CLEAR,
  ROGUE_ONU_TX_INT_MASK,
  ROGUE_ONU_TX_INT_UNMASK
}
PON_ROGUE_ONU_TX_INT_CMD;


typedef struct 
{
    uint16_t starttime;
    uint16_t allocid;
    uint16_t sfc_ls;
    bdmf_boolean hec_ok;
    uint8_t bprofile;
    bdmf_boolean fwi;
    bdmf_boolean ploamu;
    bdmf_boolean dbru;
    uint16_t grantsize;
} 
NGPON_ACCESS;

typedef struct
{
    GPON_ACCESS *gpon_access;
    NGPON_ACCESS *ngpon_access;
    bool access_type;
}
PON_ACCESS;

#define NGPON_NUMBER_OF_ACCESS_IN_MAP 512
#define GPON_NUMBER_OF_ACCESS_IN_MAP 64 

typedef uint32_t AES_KEY_DTE;

/******************************************************************************/
/* This type defines the event and alarms function callback                  */
/******************************************************************************/
typedef void (* EVENT_AND_ALARM_CALLBACK_DTE) (PON_API_MESSAGE_OPCODES_DTE xi_message_id , PON_USER_INDICATION_DTE xi_user_indication_message);

typedef void (* DBR_PROCESS_RUNNER_DATA_CALLBACK_DTE) (uint32_t xi_runner_queue_id , uint32_t * xo_runner_queue_size_struct);

typedef void (* SC_SC_RUNNER_DATA_CALLBACK_DTE) (uint8_t  *xo_buffer,  uint8_t  *xo_flag);

typedef int (* FLUSH_TCONT_CALLBACK_DTE) (uint8_t tcont_id);


#ifndef SIM_ENV
typedef struct
{
    //Setters
    PON_ERROR_DTE (* pon_assign_user_callback) (EVENT_AND_ALARM_CALLBACK_DTE          xi_event_and_alarm_callback,
                                                DBR_PROCESS_RUNNER_DATA_CALLBACK_DTE  xi_dbr_runner_data_callback,
                                                SC_SC_RUNNER_DATA_CALLBACK_DTE        xi_sc_sc_runner_callback,
                                                FLUSH_TCONT_CALLBACK_DTE              xi_flush_tcont_callback);
    PON_ERROR_DTE (* pon_configure_onu_serial_number) (PON_SERIAL_NUMBER_DTE xi_serial_number);
    PON_ERROR_DTE (* pon_configure_onu_password) (PON_PASSWORD_DTE  xi_onu_password);
    PON_ERROR_DTE (* pon_link_activate) (bdmf_boolean xi_initial_state_disable);
    PON_ERROR_DTE (* pon_set_gem_block_size) (PON_GEM_BLOCK_SIZE_DTE xi_gem_block_size);
    PON_ERROR_DTE (* pon_dba_sr_process_initialize) (uint32_t xi_sr_reporting_block, uint32_t xi_update_interval);
    PON_ERROR_DTE (* pon_link_deactivate) (void);
    PON_ERROR_DTE (* pon_configure_link_params) (rdpa_gpon_link_cfg_t* const xi_link_default_configuration);
    PON_ERROR_DTE (* pon_send_dying_gasp_message) (void);
    PON_ERROR_DTE (* pon_send_pee_message) (void);
    PON_ERROR_DTE (* pon_send_pst_message) (PON_PST_INDICATION_PARAMETERS_DTE xi_pst_configuration_struct);
    PON_ERROR_DTE (* pon_dba_sr_process_terminate) (void);
    PON_ERROR_DTE (* pon_control_port_id_filter) (PON_FLOW_ID_DTE xi_flow_id,
                                                  bool xi_filter_status);
    PON_ERROR_DTE (* pon_modify_encryption_port_id_filter) (PON_FLOW_ID_DTE xi_flow_id,
                                                            bool xi_encryption_mode);
    PON_ERROR_DTE (* pon_configure_port_id_filter) (PON_FLOW_ID_DTE xi_flow_id,
                                                    PON_PORT_ID_DTE xi_port_id_filter,
                                                    PON_PORT_ID_DTE xi_port_id_mask,
                                                    bool xi_encryption_mode,
                                                    PON_FLOW_TYPE_DTE xi_flow_type,
                                                    PON_FLOW_PRIORITY_DTE xi_flow_priority,
                                                    bool crcEnable);
    PON_ERROR_DTE (* pon_configure_tcont) (PON_TCONT_ID_DTE xi_tcont_queue_id, 
                                           PON_ALLOC_ID_DTE xi_alloc_id);
    PON_ERROR_DTE (* pon_remove_tcont) (PON_TCONT_ID_DTE xi_tcont_id);
    PON_ERROR_DTE (* pon_link_reset) (void);
    PON_ERROR_DTE (* pon_mask_rx_pon_interrupts) (void);
    PON_ERROR_DTE (* pon_mask_tx_pon_interrupts) (void);
    PON_ERROR_DTE (* pon_isr_handler) (void * xi_ploam_message_ptr ,PON_INTERRUPTS_MASK_DTE xi_interrupt_vector);
    PON_ERROR_DTE (* pon_unmask_tx_pon_interrupts) (void);
    PON_ERROR_DTE (* pon_unmask_rx_pon_interrupts) (void);
    PON_ERROR_DTE (* pon_tx_set_tcont_to_counter_group_association) (uint8_t tcont0, uint8_t tcont1, uint8_t tcont2, uint8_t tcont3);
    PON_ERROR_DTE (* pon_tx_set_flush_valid_bit) (uint8_t tcont_id, bdmf_boolean flush_enable, bdmf_boolean flush_immediate);
    PON_ERROR_DTE (* pon_tx_generate_misc_transmit) (uint8_t prodc[16], uint8_t prcl, uint8_t brc, uint8_t prl, uint16_t msstart, uint16_t msstop, bdmf_boolean enable);

    //Getters
    PON_ERROR_DTE (* pon_get_link_status) (LINK_STATE_DTE * const xo_state,
                                          LINK_SUB_STATE_DTE * const xo_sub_state,
                                          LINK_OPERATION_STATES_DTE * const xo_operational_state);
    PON_ERROR_DTE (* pon_get_link_parameters) (rdpa_gpon_link_cfg_t * const xo_parameters);
    PON_ERROR_DTE (* pon_get_onu_id) (uint16_t * const xo_onu_id);
    PON_ERROR_DTE (* pon_get_overhead_and_length) (PON_TX_OVERHEAD_DTE * const xo_pon_overhead ,
                                                   uint8_t * const xo_pon_overhead_length,
                                                   uint8_t * const xo_pon_overhead_repetition_length,
                                                   uint8_t * const xo_pon_overhead_repetition);
    PON_ERROR_DTE (* pon_get_transceiver_power_level) (uint8_t * const xo_transceiver_power_level);
    PON_ERROR_DTE (* pon_get_equalization_delay) (uint32_t * const xo_equalization_delay);
    PON_ERROR_DTE (* pon_get_aes_encryption_key) (AES_KEY_DTE * xi_aes_encryption_key, uint8_t xi_fragment_index);
    PON_ERROR_DTE (* pon_get_bip_error_counter) (uint32_t * const xo_bip_errors);
    PON_ERROR_DTE (* pon_get_rxpon_ploam_counters) (PON_RX_PLOAM_COUNTERS_DTE * const xo_rxpon_ploam_counters);
    PON_ERROR_DTE (* pon_get_rx_unkonw_ploam_counter) (uint32_t * const  xo_rx_unknown_counters); 
    PON_ERROR_DTE (* pon_get_txpon_pm_counters) (uint16_t * const xo_illegal_access_counter,
                                                 uint32_t * const xo_idle_ploam_counter,
                                                 uint32_t * const xo_tx_ploam_counter); 
    PON_ERROR_DTE (* pon_get_port_id_configuration) (PON_FLOW_ID_DTE xi_flow_id,
                                                     bool * const xo_filter_status,
                                                     PON_PORT_ID_DTE * const xo_port_id_filter,
                                                     PON_PORT_ID_DTE * const xo_port_id_mask,
                                                     bool * const xo_encryption_mode,
                                                     PON_FLOW_TYPE_DTE * const xo_flow_type,
                                                     PON_FLOW_PRIORITY_DTE * const xo_flow_priority,
                                                     bool * xo_crc_enable);
    PON_ERROR_DTE (* pon_get_encryption_error_counter) (uint32_t *const xo_rx_encryption_error);
    PON_ERROR_DTE (* pon_get_tcont_configuration) (PON_TCONT_ID_DTE xi_tcont_id,
                                                   PON_TCONT_DTE * const xo_tcont_cfg);
    PON_ERROR_DTE (* pon_get_tcont_queue_pm_counters) (PON_FIFO_ID_DTE xi_fifo_id,
                                                       uint32_t * const xo_transmitted_idle_counter,
                                                       uint32_t * const xo_transmitted_gem_counter,
                                                       uint32_t * const xo_transmitted_packet_counter,
                                                       uint16_t * const xo_requested_dbr_counter,
                                                       uint16_t * const xo_valid_access_counter);
    PON_ERROR_DTE (* pon_get_ploam_crc_error_counter) (PON_RXPON_REQUEST_DTE xi_request_type,
                                                                   uint32_t * const xo_ploam_crc_errors);
    PON_ERROR_DTE (* pon_rx_get_receiver_status) (bdmf_boolean *lof_state,
                                                  bdmf_boolean *fec_state,
                                                  bdmf_boolean *lcdg_state);
    PON_ERROR_DTE (* pon_rx_get_fec_counters) (uint32_t *fec_corrected_bytes, uint32_t *fec_corrected_codewords, uint32_t *fec_uncorrectable_codewords);
    PON_ERROR_DTE (* pon_rx_get_hec_counters) (PON_RX_HEC_COUNTERS_DTE * const rxpon_hec_counter);
    PON_ERROR_DTE (* pon_tx_get_flush_valid_bit) (uint8_t *tcont_id, bdmf_boolean *flush_enable, bdmf_boolean *flush_immediate, bdmf_boolean *flush_done);
    PON_ERROR_DTE (* pon_get_pon_id_parameters) (PON_ID_PARAMETERS *xo_pon_param);
    PON_ERROR_DTE (* pon_start_bw_recording) (bdmf_boolean rcd_stop, uint32_t record_type, uint32_t alloc_id);
    PON_ERROR_DTE (* pon_stop_bw_recording) (void);
    PON_ERROR_DTE (* pon_get_bw_recording_data) (PON_ACCESS *accesses_array, int *const access_read_num);
    PON_ERROR_DTE (* pon_get_burst_prof) (rdpa_gpon_burst_prof_t * const xo_parameters, const int index);

    PON_ERROR_DTE (* pon_modify_tcont) (PON_TCONT_ID_DTE xi_tcont_id, PON_ALLOC_ID_DTE xi_new_alloc_id);
#ifdef USE_BDMF_SHELL
    void (* initialize_drv_pon_stack_shell) (bdmfmon_handle_t driver_dir);
    void (* exit_drv_pon_stack_shell) (void);
#endif

#ifdef CONFIG_BCM_GPON_TODD
    void (* pon_todd_set_tod_info) (uint32_t sframe_num_ls, 
        uint32_t sframe_num_ms, const gpon_todd_tstamp_t* tstamp_n);
    void (* pon_todd_get_tod_info) (uint32_t* sframe_num_ls, 
        uint32_t* sframe_num_ms, gpon_todd_tstamp_t* tstamp_n);
    void (* pon_todd_reg_1pps_start_cb) (gpon_todd_1pps_ctrl_cb_t onepps_start_cb);
    void (* pon_todd_get_tod) (gpon_todd_tstamp_t* tstamp, uint64_t *ts);
    void (* pon_tod_get_sfc) (uint32_t *sfc_ls, uint32_t *sfc_ms);
#endif


  int (* rogue_onu_detect_cfg) (void *cfg, void *prev_cfg) ;

  PON_ERROR_DTE (* set_multicast_encryption_key) (uint32_t key_index, uint8_t* encryption_key);

  void (* set_stack_mode) (PON_MAC_MODE stack_mode);
  uint32_t (* calc_ds_omci_crc_or_mic) (uint8_t *buffer, uint32_t length);
  uint32_t (* calc_us_omci_crc_or_mic) (uint8_t *buffer, uint32_t length);

} PON_SM_CALLBACK_FUNC;

int init_pon_module(PON_SM_CALLBACK_FUNC * p_pon_sm_cb);

#endif
#define MS_MILISEC_TO_MICROSEC(t) (t * 1000)

#ifdef __cplusplus
}
#endif


#endif/* __PON_SM_COMMON_H */

