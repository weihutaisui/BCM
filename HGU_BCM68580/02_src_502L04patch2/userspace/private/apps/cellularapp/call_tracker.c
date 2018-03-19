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

#include "cms.h"
#include "cms_log.h"
#include "ril_access.h"
#include "cms_access.h"
#include "call_tracker.h"
#include "cellular_msg.h"
#include "cellular_internal.h"
#include "cms_msg_cellular.h"

#include <assert.h>

static void process_call_dial(DevCellularCallDialMsgBody* dialInfo);
static void process_call_ringback_tone(AsyncResult * ar);
static void process_call_hangup(Object* o);
static void process_call_answer();
static void process_call_poll_result(AsyncResult * ar);
static void get_current_calls(Object *o);

static void on_operation_complete();

static struct internal_message* obtain_complete_message();
static void free_call_internal(RIL_Call *ril_call);
static void free_uusInfo(RIL_UUS_Info * uusInfo);


static Object *mCurrentCalls = NULL;

void call_tracker_init()
{
}


void call_tracker_process(struct internal_message *msg)
{
    AsyncResult *ar;
    DevCellularCallDialMsgBody *dialInfo;
    Object* o = internal_message_getObject(msg);
    switch(msg->what)
    {
        case EVENT_CALL_STATE_CHANGED:
            cms_access_notify_callStateChanged();
            break;

        case EVENT_CALL_DIAL:
            assert(o);
            dialInfo = (DevCellularCallDialMsgBody *)object_getData(o);
            process_call_dial(dialInfo);
            break;

        case EVENT_CALL_HANGUP:
            assert(o);
            process_call_hangup(o);
            break;

        case EVENT_CALL_ANSWER:
            process_call_answer();
            break;

        case EVENT_CALL_GET_CURRENT:
            get_current_calls(o);
            internal_message_setObject(msg, NULL);
            break;

        case EVENT_CALL_POLL_RESULT:
            assert(o);
            ar = (AsyncResult *)object_getData(o);
            process_call_poll_result(ar);
            break;

        case EVENT_CALL_OPERATION_COMPLETE:
            on_operation_complete();
            break;

        case EVENT_CALL_RINGBACK_TONE:
            assert(o);
            ar = (AsyncResult *)object_getData(o);
            process_call_ringback_tone(ar);
            break;

        default:
            break;
    }
}
/* We know this is a cms request which needs "sync" result from RIL. Get the 
 * cms request token from the object passed to us.
 * */
static void get_current_calls(Object *o)
{
    struct internal_message *msg;

    assert(o);

    msg = internal_message_obtain(EVENT_CALL_POLL_RESULT, CALL_TRACKER);
    internal_message_setObject(msg, o);
    assert(msg);

    ril_access_get_current_calls(msg);
}

static void process_call_dial(DevCellularCallDialMsgBody* dialInfo)
{
    RIL_UUS_Info uusInfo;
    assert(dialInfo);
    uusInfo.uusType = dialInfo->uusType;
    uusInfo.uusDcs = dialInfo->uusDcs;
    uusInfo.uusLength = dialInfo->uusLength;
    uusInfo.uusData = malloc(dialInfo->uusLength);
    assert(uusInfo.uusData);
    memcpy(uusInfo.uusData, dialInfo->uusData, dialInfo->uusLength);
    ril_access_dial((char *)&(dialInfo->address), dialInfo->clirMode, NULL,
                     obtain_complete_message());

}

static void process_call_hangup(Object* o)
{
    DevCellularCallHangupMsgBody* hangupInfo;

    assert(o);
    hangupInfo = (DevCellularCallHangupMsgBody *)object_getData(o);
    assert(hangupInfo);
    ril_access_hangup_connection(hangupInfo->gsmIndex, 
                                 obtain_complete_message());

}

static void process_call_answer()
{
    ril_access_accept_call(obtain_complete_message());
}


static void on_operation_complete()
{
    struct internal_message *msg;
    
    msg = internal_message_obtain(EVENT_CALL_POLL_RESULT, CALL_TRACKER);
    assert(msg);

    ril_access_get_current_calls(msg);
}

Object* call_tracker_getCurrentCalls()
{
    return mCurrentCalls;
}

static void process_call_ringback_tone(AsyncResult * ar)
{
    assert(ar);

    if(ar->error == 0)
    {
        int ints_length = (object_dataLength)(ar->result);
        int* ints = (int *)(object_getData(ar->result));
    
        cmsLog_debug("ringback tone contains %d data", ints_length);

        cms_access_notify_ringBackTone(ints, ints_length);
    }
}

static void process_call_poll_result(AsyncResult * ar)
{
    Object *o = NULL;
    Object *cms_token;
    int num, i;
    RIL_Call *ril_call_list;
    CmsRet ret;

    assert(ar);

    cms_token = ar->userObj;

    if(ar->error == 0)
    {    
        o = ar->result;
        assert(o);

        /* Free the call list kept in memory. Take care to free the second layer
         * memory pointed to by inner pointers.
         */
        if(mCurrentCalls != NULL)
        {
            num = object_dataLength(mCurrentCalls)/sizeof(RIL_Call);
            ril_call_list = (RIL_Call *)object_getData(mCurrentCalls);
            for(i=0; i<num; i++)
            { 
                free_call_internal(ril_call_list+i);
            }
            object_freeData(mCurrentCalls);
            free(mCurrentCalls);
            mCurrentCalls = NULL;
        }

        /*Copy over the call list into our statically kept memory.*/
        num = object_dataLength(o)/sizeof(RIL_Call);
        if(num > 0)
        {
            mCurrentCalls = object_obtain();
            assert(mCurrentCalls);
            object_setData(mCurrentCalls, object_getData(o), object_dataLength(o));
        }
        ret = CMSRET_SUCCESS;
    }
    else
    {
        ret = CMSRET_REQUEST_DENIED;
    }

    if(cms_token != NULL)
    {
        /* this is a request from cms, need to get the token to send back
         * cms response.*/
        CmsSyncToken *token = (CmsSyncToken *)object_getData(cms_token);
        cms_access_send_resp(token->serial, ret, o);
    }
}

static void free_call_internal(RIL_Call *ril_call)
{
    assert(ril_call);

    if(ril_call->number != NULL)
    {
        free(ril_call->number);
        ril_call->number = NULL;
    }
    if(ril_call->name != NULL)
    {
        free(ril_call->name);
        ril_call->name = NULL;
    }
    if(ril_call->uusInfo != NULL)
    {
        free_uusInfo(ril_call->uusInfo);
        ril_call->uusInfo = NULL;
    }
}

static void free_uusInfo(RIL_UUS_Info * uusInfo)
{
    assert(uusInfo);
    if(uusInfo->uusData != NULL)
    {
        free(uusInfo->uusData);
        uusInfo->uusData = NULL;
    }
    free(uusInfo);
}

static struct internal_message* obtain_complete_message()
{
    struct internal_message *msg;
    msg = internal_message_obtain(EVENT_CALL_OPERATION_COMPLETE, CALL_TRACKER);
    return msg;

}
