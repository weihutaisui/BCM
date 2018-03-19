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


#ifndef _CELLULAR_MSG_H_
#define _CELLULAR_MSG_H_ 

#include "cms.h"


#define CELLULAR_PERIODIC_TASK_INTERVAL 5 


typedef struct object
{
    UINT32 data_len;
    void *data;
}Object;

typedef enum internal_message_dest{
    SERVICE_STATE_TRACKER,
    DC_TRACKER,
    DATA_CONNECTION,
    UICC_CONTROLLER,
    ICC_FILE_HANDLER,
    ADN_RECORD_LOADER,
    CALL_TRACKER,
    SMS_TRACKER,
    MISC_TRACKER,
    ALL
}InternalMessageDest;


typedef enum cellular_event{
    EVENT_RADIO_STATE_CHANGED,
    EVENT_NETWORK_STATE_CHANGED,
    EVENT_ICC_STATUS_CHANGED,
    EVENT_GET_ICC_STATUS_DONE,
    EVENT_DATA_STATE_CHANGED,
    EVENT_DATA_CONNECTION_ATTACHED,
    EVENT_DATA_CONNECTION_DETACHED,
    EVENT_TRY_SETUP_DATA,
    EVENT_RESTRICTED_STATE_CHANGED,
    EVENT_POLL_STATE_OPERATOR,
    EVENT_POLL_STATE_REGISTRATION,
    EVENT_POLL_STATE_GPRS,
    EVENT_QUERY_FACILITY_FDN_DONE,
    EVENT_QUERY_FACILITY_LOCK_DONE,
    EVENT_GET_IMSI_DONE,
    EVENT_GET_DEVICE_IDENTITY_DONE,
    EVENT_RECORD_LOADED,
    EVENT_GET_RECORD_SIZE_DONE,
    EVENT_READ_RECORD_DONE,
    EVENT_GET_ICCID_DONE,
    EVENT_GET_MSISDN_DONE,
    EVENT_ADN_LOAD_DONE,
    EVENT_EXT_RECORD_LOAD_DONE,
    EVENT_NEW_SMS,
    EVENT_NEW_SMS_ON_SIM,
    EVENT_GET_SMS_DONE,
    EVENT_GET_AVAILABLE_NETWORKS,
    EVENT_GET_AVAILABLE_NETWORKS_DONE,  
    EVENT_SIGNAL_STRENGTH_UPDATE,
    EVENT_DC_SETUP_DATA_CONNECTION_DONE,
    EVENT_DC_CONNECT,
    EVENT_DC_DISCONNECT,
    EVENT_DC_LOST_CONNECTION,
    EVENT_DC_DEACTIVE_DONE,
    EVENT_CMS_ENABLE_INTERFACE,
    EVENT_CMS_ENABLE_APN,
    EVENT_CALL_DIAL,
    EVENT_CALL_HANGUP,
    EVENT_CALL_ANSWER,
    EVENT_CALL_POLL_RESULT,
    EVENT_CALL_STATE_CHANGED,
    EVENT_CALL_OPERATION_COMPLETE,
    EVENT_CALL_RINGBACK_TONE,
    EVENT_CALL_GET_CURRENT,
    EVENT_PLL_ENABLE,
    EVENT_PLL_DISABLE,
    EVENT_MISC_GET_RIL_REQUEST,
    EVENT_MISC_GET_RIL_REQUEST_DONE,
}CellularEvent;

typedef struct internal_message
{
    InternalMessageDest dest;
    CellularEvent what;
    Object* object;
}InternalMessage;

typedef struct cms_sync_token
{
    SINT32 serial;
    Object* data;
}CmsSyncToken;

typedef struct ril_async_result
{
    int error;
    struct object *result;
    struct object *userObj;
}AsyncResult;

struct object* object_obtain();
UBOOL8 object_setData(struct object* t, const void* data, int size);
struct object * object_obtain_with_data(const void* data, int size);
void object_freeData(struct object *t);
UBOOL8 object_appendData(struct object *t, void *data, size_t data_len);
void* object_getData(struct object *t);
int object_dataLength(struct object *t);
UBOOL8 object_containsData(struct object *t);
struct internal_message* internal_message_obtain(int event, int dest);
struct internal_message * internal_message_obtain_with_data(int event, int dest, const void* data, int size);
void internal_message_freeObject(struct internal_message *msg);
void internal_message_free_and_null_ptr(struct internal_message **msg);
void internal_message_setObject(struct internal_message *msg, Object *object);
struct object* internal_message_getObject(struct internal_message *msg);

#endif
