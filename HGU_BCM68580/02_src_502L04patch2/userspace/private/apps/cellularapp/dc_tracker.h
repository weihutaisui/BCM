/*
 <:copyright-BRCM:2015:NONE/NONE:standard
 
    Copyright (c) 2015 Broadcom 
    All Rights Reserved
 
 :>
 *****************************************************************************
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ****************************************************************************

*/


#ifndef _DC_TRACKER_H
#define _DC_TRACKER_H 

#include "cms.h"
#include "data_connection.h"

#define MAX_APN_CONTEXT 4

typedef enum dc_reason
{
    REASON_SIM_LOADED,
}DcReason;

typedef enum dc_activity
{
    DC_ACTIVITY_NONE,
    DC_ACTIVITY_DATAIN,
    DC_ACTIVITY_DATAOUT,
    DC_ACTIVITY_DATAINANDOUT,
    DC_ACTIVITY_DORMANT
}DcActivity;

typedef enum dc_active_ph_link
{
    DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE = 0,
    DATA_CONNECTION_ACTIVE_PH_LINK_DOWN = 1,
    DATA_CONNECTION_ACTIVE_PH_LINK_UP = 2 
}DataConnectionActivePhLink;

/**
 * IDLE: ready to start data connection setup, default state
 * CONNECTING: state of issued startPppd() but not finish yet
 * SCANNING: data connection fails with one apn but other apns are available
 *           ready to start data connection on other apns (before INITING)
 * CONNECTED: IP connection is setup
 * DISCONNECTING: Connection.disconnect() has been called, but PDP
 *                context is not yet deactivated
 * FAILED: data connection fail for all apns settings
 * RETRYING: data connection failed but we're going to retry.
 *
 * getDataConnectionState() maps State to DataState
 *      FAILED or IDLE : DISCONNECTED
 *      RETRYING or CONNECTING or SCANNING: CONNECTING
 *      CONNECTED : CONNECTED or DISCONNECTING
 */
typedef enum dc_state{
    DC_STATE_IDLE,
    DC_STATE_CONNECTING,
    DC_STATE_SCANNING,
    DC_STATE_CONNECTED,
    DC_STATE_DISCONNECTING,
    DC_STATE_FAILED,
    DC_STATE_RETRYING
}DcState;

/* maximum APN candidate for auto select in all. */
#define APN_AUTO_SEL_MAX_CANDIDATE 6
/* maximum APN candidate for each apn auto select type. */
#define APN_AUTO_SEL_MAX_CANDIDATE_PER_TYPE 3

/* type for apn auto selection. only support default for current stage*/
/* caution: only add needed type here as it will impact static memory alloc*/
typedef enum
{
APN_TYPE_DEFAULT = 0,
APN_TYPE_MAX,
}ApnType;

typedef struct apn_context
{ 
    /* the apn context is indexed by its apn type.
     * the context is responsible to calculate appropriate apn_setting to be 
     * used for a particular apn type by using information from telephony 
     * data base and current network operator.
     * */
    ApnType mApnType;
    char* ifname;
    int priority;
    int mState;
    ApnSetting mApnSetting;
    /*pApnSetting could point to
    1, mApnSetting if APN is specified, or
    2, mAllApnSettings[][] if APN is auto selected by cellularApp    
    */
    ApnSetting *pApnSetting;
    UBOOL8 mDataEnabled;
    int connection_index;
}ApnContext;


void dc_tracker_init();
void dc_tracker_process(struct internal_message *msg);
void dc_tracker_state_change(int dc_index, DataConnectionState new_state);

#endif
