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

#ifndef GPON_IND_TASK_H_INCLUDED
#define GPON_IND_TASK_H_INCLUDED

#include "gpon_ploam.h"
#include "pon_sm_common.h"
/******************************************************************************/
/*                                                                            */
/* OAM alarm type                                                             */
/*                                                                            */
/******************************************************************************/
#ifndef GPON_OAM_ALARMS_ID_DEFINED
#define GPON_OAM_ALARMS_ID_DEFINED

typedef uint32_t GPON_OAM_ALARM_ID_DTE;

/* LCDG alarm id */
#define CE_OAM_ALARM_ID_LCDG ((GPON_OAM_ALARM_ID_DTE)0x1)

/* LOF alarm id */
#define CE_OAM_ALARM_ID_LOF  ((GPON_OAM_ALARM_ID_DTE)0x2)

/* PEE from OLT alarm */
#define CE_OAM_ALARM_ID_PEE  ((GPON_OAM_ALARM_ID_DTE)0x3)

/* SUF alarm id */
#define CE_OAM_ALARM_ID_SUF  ((GPON_OAM_ALARM_ID_DTE)0x4)

/* PLOAM synchronization loss */
#define CE_OAM_ALARM_ID_OAML ((GPON_OAM_ALARM_ID_DTE)0x5)

/* Frame loss */
#define CE_OAM_ALARM_ID_FRML ((GPON_OAM_ALARM_ID_DTE)0x6)

/* Cell delineation loss */
#define CE_OAM_ALARM_ID_LCD  ((GPON_OAM_ALARM_ID_DTE)0x7)

/* Downstream BER value exceed 10exp-5 */
#define CE_OAM_ALARM_ID_SD   ((GPON_OAM_ALARM_ID_DTE)0x9)

/* Message error */
#define CE_OAM_ALARM_ID_MEM  ((GPON_OAM_ALARM_ID_DTE)0xA)

/* Deactivate message with match PON_Id */
#define CE_OAM_ALARM_ID_DACT ((GPON_OAM_ALARM_ID_DTE)0xB)

/* Disable_Serial_Number message with match PON_Id */
#define CE_OAM_ALARM_ID_DIS  ((GPON_OAM_ALARM_ID_DTE)0xC)

/* LOF received in operational state */
#define CE_OAM_ALARM_ID_OPERATIONAL_LOF ((GPON_OAM_ALARM_ID_DTE)0xD)

#endif


typedef struct
{
    GPON_OAM_ALARM_ID_DTE oam_alarm_id;
    bool oam_alarm_status;
    union
    {
        GPON_DS_PLOAM ploam_message;
    } oam_payload;
}
GPON_OAM_DTE;


/******************************************************************************/
/*  Power level parameters structure                                          */
/******************************************************************************/
typedef uint8_t GPON_CHANGE_POWER_LEVEL_REASON_DTE;

/* Change power level ploam */
#define GPON_CHANGE_POWER_LEVEL_REASON_CHANGE_POWER_LEVEL_PLOAM ((GPON_CHANGE_POWER_LEVEL_REASON_DTE)0)
/* Serial number requests threshold crossed */
#define GPON_CHANGE_POWER_LEVEL_REASON_SN_REQUEST_THRESHOLD     ((GPON_CHANGE_POWER_LEVEL_REASON_DTE)1)
/* Upstream overhead ploam */
#define GPON_CHANGE_POWER_LEVEL_REASON_US_OVERHEAD_PLOAM        ((GPON_CHANGE_POWER_LEVEL_REASON_DTE)2)


typedef enum
{
    GPON_POWER_LEVEL_LOW = 0,
    GPON_POWER_LEVEL_NORMAL,
    GPON_POWER_LEVEL_HIGH,
    GPON_MAX_NUM_OF_POWER_LEVEL,
    GPON_POWER_LEVEL_ILLEGAL
}
GPON_POWER_LEVEL_MODE_DTE;


typedef struct
{
    GPON_POWER_LEVEL_MODE_DTE power_level_mode;
    GPON_CHANGE_POWER_LEVEL_REASON_DTE reason;
}
GPON_POWER_LEVEL_PARAMETERS_DTE;


typedef struct
{
    bool command;
    uint32_t interval;
}
TIMER_CTRL_DTE;

typedef struct
{
    /* Events and Alarms */
    EVENT_AND_ALARM_CALLBACK_DTE event_and_alarm_callback;
    /* DBR runner data callback */
    DBR_PROCESS_RUNNER_DATA_CALLBACK_DTE dbr_runner_data_callback;
    /* Sc SC callback */
    SC_SC_RUNNER_DATA_CALLBACK_DTE sc_sc_runner_callback;
    /* Flush callback */
    FLUSH_TCONT_CALLBACK_DTE flush_tcont_callback;
}
GPON_CALLBACKS_DTE;

typedef struct
{    
    PON_EVENTS_DTE message_id;

    union
    {
        GPON_OAM_DTE oam;
    	PON_FIFO_ID_DTE fifo_block;
    	uint32_t keepalive_timeout;
        GPON_DS_PLOAM ploam_message;
        PON_USER_INDICATION_DTE user_indication;
        TIMER_CTRL_DTE timer_control;
        GPON_CALLBACKS_DTE callback_routines;
        GPON_POWER_LEVEL_PARAMETERS_DTE power_level;
    } message_data;
}
GPON_MESSAGE_DTE;

/*

void gpon_default_events_alarms_callback(uint32_t xi_alarm_event_id, ...);
#define GPON_EVENTS_ALARMS_ROUTINE (p_default_event_and_alarm_callback)
*/
PON_ERROR_DTE f_gpon_send_message_to_pon_task(PON_EVENTS_DTE xi_msg_opcode, ...);
int p_pon_task(void *xi_parameter);

#endif 
