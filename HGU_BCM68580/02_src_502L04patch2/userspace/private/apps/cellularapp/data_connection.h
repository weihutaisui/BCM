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


#ifndef _DATA_CONNECTION_H
#define _DATA_CONNECTION_H

#include "cellular_msg.h"

#define DATA_CONNECTIONS_MAX 4
#define DATA_CONNECTIONS_MAX_RETRY 3

typedef enum data_connection_state
{
    DcInactiveState = 0,
    DcActivatingState = 1,
    DcActiveState = 2,
    DcDisconnectingState = 3,
    DcRetryingState = 4,
    DcDisconnectingErrorCreatingConnection = 5
}DataConnectionState;

typedef struct apn_setting
{
    char* carrier;
    char* apn;
    char* proxy;
    UINT32 port;
    char* mmsc;
    char* mmsProxy;
    UINT32 mmsPort;
    char* user;
    char* password;
    int authType;
    char* types;
    int id;
    char* numeric;
    char* protocol;
    char* roamingProtocol;
    UBOOL8 carrierEnabled;
    int bearer;
}ApnSetting;

typedef struct data_connection
{
    int mInitialMaxRetry;
    struct apn_setting apnSetting;
    int mRilRat;
    int mProfileId;
    int mDataRegState;
    DataConnectionState mState;
    int mTag;
    int mCid;     
    int mIndex;
}DataConnection;

typedef enum dc_setup_result
{
    SUCCESS,
    RETRY,
    ERR_BadCommand,
    ERR_UnacceptableParameter,
    ERR_GetLastErrorFromRil,
    ERR_State,
    ERR_RilError
}DcSetupResult;

void data_connection_init();
void data_connection_process(struct internal_message* msg);
void clear_all_data_connections();
int find_data_connection_by_cid(int cid);
int find_free_data_connection();
UBOOL8 data_connection_init_entry(int dc_index, struct apn_setting *apn); 
struct apn_context * data_connection_getApnContext(int dc_index);

#endif

